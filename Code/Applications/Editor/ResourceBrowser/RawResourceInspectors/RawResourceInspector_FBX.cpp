#include "RawResourceInspector_FBX.h"
#include "Tools/Core/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderMesh.h"
#include "Tools/Render/ResourceDescriptors/ResourceDescriptor_RenderTexture.h"
#include "Tools/Physics/ResourceDescriptors/ResourceDescriptor_PhysicsMesh.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationClip.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Engine/Render/Mesh/StaticMesh.h"
#include "Engine/Render/Mesh/SkeletalMesh.h"
#include "System/Animation/AnimationSkeleton.h"
#include "Engine/Animation/AnimationClip.h"
#include "Engine/Physics/PhysicsMesh.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

using namespace fbxsdk;

//-------------------------------------------------------------------------

namespace KRG::Resource
{
    KRG_RAW_FILE_INSPECTOR_FACTORY( InspectorFactoryFbx, "fbx", ResourceInspectorFBX );

    //-------------------------------------------------------------------------

    ResourceInspectorFBX::ResourceInspectorFBX( TypeSystem::TypeRegistry const& typeRegistry, Resource::ResourceDatabase const& resourceDatabase, FileSystem::Path const& filePath )
        : RawResourceInspector( typeRegistry, resourceDatabase, filePath )
        , m_sceneContext( filePath )
    {
        KRG_ASSERT( FileSystem::Exists( filePath ) );

        if ( m_sceneContext.IsValid() )
        {
            ReadFileContents();
        }
    }

    void ResourceInspectorFBX::ReadFileContents()
    {
        FbxGeometryConverter geomConverter( m_sceneContext.m_pManager );
        geomConverter.SplitMeshesPerMaterial( m_sceneContext.m_pScene, true );

        //-------------------------------------------------------------------------
        // Meshes
        //-------------------------------------------------------------------------

        TInlineVector<FbxMesh*, 20> meshes;
        auto numGeometries = m_sceneContext.m_pScene->GetGeometryCount();
        for ( auto i = 0; i < numGeometries; i++ )
        {
            auto pGeometry = m_sceneContext.m_pScene->GetGeometry( i );
            if ( pGeometry->Is<FbxMesh>() )
            {
                FbxMesh* pMesh = static_cast<FbxMesh*>( pGeometry );

                auto& meshInfo = m_meshes.emplace_back();
                meshInfo.m_nameID = StringID( pMesh->GetNode()->GetName() );
                meshInfo.m_isSkinned = pMesh->GetDeformerCount( FbxDeformer::eSkin ) > 0;
            }
        }

        //-------------------------------------------------------------------------
        // Skeletons
        //-------------------------------------------------------------------------

        TVector<FbxNode*> skeletonRootNodes;
        m_sceneContext.FindAllNodesOfType( FbxNodeAttribute::eSkeleton, skeletonRootNodes );

        for ( auto& pSkeletonNode : skeletonRootNodes )
        {
            BoneSkeletonRootInfo skeletonRootInfo;
            skeletonRootInfo.m_nameID = StringID( pSkeletonNode->GetName() );

            NullSkeletonRootInfo* pNullSkeletonRoot = nullptr;
            if ( auto pParentNode = pSkeletonNode->GetParent() )
            {
                if ( auto pNodeAttribute = pParentNode->GetNodeAttribute() )
                {
                    if ( pNodeAttribute->GetAttributeType() == FbxNodeAttribute::eNull )
                    {
                        StringID const parentID( pParentNode->GetName() );

                        // Try find existing skeleton with this ID
                        for ( auto& nullSkeletonRoot : m_nullSkeletonRoots )
                        {
                            if ( nullSkeletonRoot.m_nameID == parentID )
                            {
                                pNullSkeletonRoot = &nullSkeletonRoot;
                                break;
                            }
                        }

                        // Create new parent skeleton
                        if ( pNullSkeletonRoot == nullptr )
                        {
                            pNullSkeletonRoot = &m_nullSkeletonRoots.emplace_back();
                            pNullSkeletonRoot->m_nameID = parentID;
                        }
                    }
                }
            }

            // Add to the list of skeletons
            if ( pNullSkeletonRoot != nullptr )
            {
                pNullSkeletonRoot->m_childSkeletons.push_back( skeletonRootInfo );
            }
            else
            {
                m_skeletonRoots.push_back( skeletonRootInfo );
            }
        }

        //-------------------------------------------------------------------------
        // Animations
        //-------------------------------------------------------------------------

        TVector<FbxAnimStack*> stacks;
        m_sceneContext.FindAllAnimStacks( stacks );
        for ( auto pAnimStack : stacks )
        {
            auto pTakeInfo = m_sceneContext.m_pScene->GetTakeInfo( pAnimStack->GetNameWithoutNameSpacePrefix() );
            if ( pTakeInfo != nullptr )
            {
                auto& animInfo = m_animations.emplace_back();
                animInfo.m_nameID = StringID( pAnimStack->GetName() );
                animInfo.m_duration = Seconds( (float) pTakeInfo->mLocalTimeSpan.GetDuration().GetSecondDouble() );

                FbxTime const duration = pTakeInfo->mLocalTimeSpan.GetDuration();
                FbxTime::EMode mode = duration.GetGlobalTimeMode();
                animInfo.m_frameRate = (float) duration.GetFrameRate( mode );
            }
        }
    }

    void ResourceInspectorFBX::DrawFileInfo()
    {
        ImGui::PushStyleVar( ImGuiStyleVar_ChildRounding, 4.0f );
        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8.0f, 4.0f ) );
        ImGui::PushStyleColor( ImGuiCol_ChildBg, ImGuiX::Style::s_backgroundColorSemiDark.Value );
        if ( ImGui::BeginChild( "Contents", ImVec2( -1, 90 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding ) )
        {
            {
                ImGuiX::ScopedFont const sf( ImGuiX::Font::MediumBold, Colors::Orange );
                ImGui::Text( "File Info" );
            }
            ImGui::Separator();

            ImGui::Text( "Raw File: %s", m_filePath.c_str() );

            if ( m_sceneContext.IsValid() )
            {

                ImGui::Text( "Original Up Axis: %s", Math::ToString( m_sceneContext.GetOriginalUpAxis() ) );
                ImGui::Text( "Scale: %.2f", m_sceneContext.GetScaleConversionMultiplier() == 1.0f ? "M" : "CM" );
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor();
        ImGui::PopStyleVar( 2 );
    }

    void ResourceInspectorFBX::DrawFileContents()
    {
        if ( m_sceneContext.IsValid() )
        {
            InlineString tmpString;

            //-------------------------------------------------------------------------
            // Meshes
            //-------------------------------------------------------------------------

            bool haveSkeletalMeshes = false;

            if ( !m_meshes.empty() )
            {
                ImGuiX::TextSeparator( "Static Meshes", 10, ImGui::GetColumnWidth() );

                bool const isCombinedMeshSelected = ( m_selectedItemType == InfoType::StaticMesh ) && !m_selectedItemID.IsValid();
                if ( ImGui::Selectable( KRG_ICON_CUBES" Combined Static Mesh", isCombinedMeshSelected, ImGuiSelectableFlags_DontClosePopups ) )
                {
                    m_selectedItemType = InfoType::StaticMesh;
                    m_selectedItemID = StringID();
                    OnSwitchSelectedItem();
                }

                for ( auto const& meshInfo : m_meshes )
                {
                    tmpString.sprintf( KRG_ICON_CUBE" %s##StaticMesh", meshInfo.m_nameID.c_str() );
                    bool isSelected = ( m_selectedItemType == InfoType::StaticMesh ) && meshInfo.m_nameID == m_selectedItemID;
                    if ( ImGui::Selectable( tmpString.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups ) )
                    {
                        m_selectedItemType = InfoType::StaticMesh;
                        m_selectedItemID = meshInfo.m_nameID;
                        OnSwitchSelectedItem();
                    }

                    haveSkeletalMeshes |= meshInfo.m_isSkinned;
                }
            
                ImGuiX::TextSeparator( "Physics Meshes", 10, ImGui::GetColumnWidth() );

                bool const isCombinedPhysicsMeshSelected = ( m_selectedItemType == InfoType::PhysicsMesh ) && !m_selectedItemID.IsValid();
                if ( ImGui::Selectable( KRG_ICON_CUBES" Combined Physics Mesh", isCombinedPhysicsMeshSelected, ImGuiSelectableFlags_DontClosePopups ) )
                {
                    m_selectedItemType = InfoType::PhysicsMesh;
                    m_selectedItemID = StringID();
                    OnSwitchSelectedItem();
                }

                for ( auto const& meshInfo : m_meshes )
                {
                    tmpString.sprintf( KRG_ICON_CUBE" %s##PhysicsMesh", meshInfo.m_nameID.c_str() );
                    bool isSelected = ( m_selectedItemType == InfoType::PhysicsMesh ) && meshInfo.m_nameID == m_selectedItemID;
                    if ( ImGui::Selectable( tmpString.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups ) )
                    {
                        m_selectedItemType = InfoType::PhysicsMesh;
                        m_selectedItemID = meshInfo.m_nameID;
                        OnSwitchSelectedItem();
                    }
                }
            }

            if ( haveSkeletalMeshes )
            {
                ImGuiX::TextSeparator( "Skeletal Meshes", 10, ImGui::GetColumnWidth() );

                for ( auto const& meshInfo : m_meshes )
                {
                    if ( !meshInfo.m_isSkinned )
                    {
                        continue;
                    }

                    tmpString.sprintf( KRG_ICON_USER" %s", meshInfo.m_nameID.c_str() );
                    bool isSelected = ( m_selectedItemType == InfoType::SkeletalMesh ) && meshInfo.m_nameID == m_selectedItemID;
                    if ( ImGui::Selectable( tmpString.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups ) )
                    {
                        m_selectedItemType = InfoType::SkeletalMesh;
                        m_selectedItemID = meshInfo.m_nameID;
                        OnSwitchSelectedItem();
                    }
                }
            }

            //-------------------------------------------------------------------------
            // Skeletons
            //-------------------------------------------------------------------------

            if ( !m_nullSkeletonRoots.empty() )
            {
                ImGuiX::TextSeparator( "Skeletons", 10, ImGui::GetColumnWidth() );

                for ( auto const& skeletonRoot : m_nullSkeletonRoots )
                {
                    tmpString.sprintf( KRG_ICON_CIRCLE" %s", skeletonRoot.m_nameID.c_str() );

                    bool isSelected = ( m_selectedItemType == InfoType::Skeleton ) && skeletonRoot.m_nameID == m_selectedItemID;
                    if ( ImGui::Selectable( tmpString.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups ) )
                    {
                        m_selectedItemType = InfoType::Skeleton;
                        m_selectedItemID = skeletonRoot.m_nameID;
                        OnSwitchSelectedItem();
                    }

                    ImGui::Indent();
                    for ( auto const& childSkeletonRoot : skeletonRoot.m_childSkeletons )
                    {
                        tmpString.sprintf( KRG_ICON_MALE" %s", childSkeletonRoot.m_nameID.c_str() );

                        isSelected = ( m_selectedItemType == InfoType::Skeleton ) && childSkeletonRoot.m_nameID == m_selectedItemID;
                        if ( ImGui::Selectable( tmpString.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups ) )
                        {
                            m_selectedItemType = InfoType::Skeleton;
                            m_selectedItemID = childSkeletonRoot.m_nameID;
                            OnSwitchSelectedItem();
                        }
                    }
                    ImGui::Unindent();
                }

                for ( auto const& skeletonRoot : m_skeletonRoots )
                {
                    tmpString.sprintf( KRG_ICON_MALE" %s", skeletonRoot.m_nameID.c_str() );

                    bool isSelected = ( m_selectedItemType == InfoType::Skeleton ) && skeletonRoot.m_nameID == m_selectedItemID;
                    if ( ImGui::Selectable( tmpString.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups ) )
                    {
                        m_selectedItemType = InfoType::Skeleton;
                        m_selectedItemID = skeletonRoot.m_nameID;
                        OnSwitchSelectedItem();
                    }
                }
            }

            //-------------------------------------------------------------------------
            // Animations
            //-------------------------------------------------------------------------

            if ( !m_animations.empty() )
            {
                ImGuiX::TextSeparator( "Animations", 10, ImGui::GetColumnWidth() );

                for ( auto const& animationInfo : m_animations )
                {
                    tmpString.sprintf( KRG_ICON_FILM" %s", animationInfo.m_nameID.c_str() );

                    bool isSelected = ( m_selectedItemType == InfoType::Animation ) && animationInfo.m_nameID == m_selectedItemID;
                    if( ImGui::Selectable( tmpString.c_str(), isSelected, ImGuiSelectableFlags_DontClosePopups ) )
                    {
                        m_selectedItemType = InfoType::Animation;
                        m_selectedItemID = animationInfo.m_nameID;
                        OnSwitchSelectedItem();
                    }
                }
            }
        }
    }

    void ResourceInspectorFBX::DrawResourceDescriptorCreator()
    {
        if ( m_selectedItemType != InfoType::None )
        {
            {
                ImGuiX::ScopedFont const sf( ImGuiX::Font::LargeBold, ImGuiX::Style::s_selectionAccent );

                switch ( m_selectedItemType )
                {
                    case InfoType::StaticMesh:
                    {
                        ImGui::Text( "Create New Static Mesh" );
                    }
                    break;

                    case InfoType::SkeletalMesh:
                    {
                        ImGui::Text( "Create New Skeletal Mesh" );
                    }
                    break;

                    case InfoType::PhysicsMesh:
                    {
                        ImGui::Text( "Create New Physics Mesh" );
                    }
                    break;

                    case InfoType::Skeleton:
                    {
                        ImGui::Text( "Create New Skeleton" );
                    }
                    break;

                    case InfoType::Animation:
                    {
                        ImGui::Text( "Create New Animation" );
                    }
                    break;

                    default:
                    KRG_UNREACHABLE_CODE();
                    break;
                }

            }

            //-------------------------------------------------------------------------

            m_propertyGrid.DrawGrid();

            //-------------------------------------------------------------------------

            if ( ImGui::Button( "Create New Resource", ImVec2( -1, 0 ) ) )
            {
                switch ( m_selectedItemType )
                {
                    case InfoType::StaticMesh:
                    {
                        CreateNewDescriptor( Render::StaticMesh::GetStaticResourceTypeID(), m_pDescriptor );
                    }
                    break;

                    case InfoType::SkeletalMesh:
                    {
                        CreateNewDescriptor( Render::SkeletalMesh::GetStaticResourceTypeID(), m_pDescriptor );
                    }
                    break;

                    case InfoType::PhysicsMesh:
                    {
                        CreateNewDescriptor( Physics::PhysicsMesh::GetStaticResourceTypeID(), m_pDescriptor );
                    }
                    break;

                    case InfoType::Skeleton:
                    {
                        CreateNewDescriptor( Animation::Skeleton::GetStaticResourceTypeID(), m_pDescriptor );
                    }
                    break;

                    case InfoType::Animation:
                    {
                        CreateNewDescriptor( Animation::AnimationClip::GetStaticResourceTypeID(), m_pDescriptor );
                    }
                    break;

                    default:
                    KRG_UNREACHABLE_CODE();
                    break;
                }
            }
        }
    }

    void ResourceInspectorFBX::OnSwitchSelectedItem()
    {
        m_propertyGrid.SetTypeToEdit( nullptr );
        KRG::Delete( m_pDescriptor );

        //-------------------------------------------------------------------------

        switch ( m_selectedItemType )
        {
            case InfoType::StaticMesh:
            {
                auto pDesc = KRG::New<Render::StaticMeshResourceDescriptor>();
                pDesc->m_meshPath = ResourcePath::FromFileSystemPath( m_rawResourceDirectory, m_filePath );
                pDesc->m_meshName = m_selectedItemID.IsValid() ? m_selectedItemID.c_str() : "";
                m_pDescriptor = pDesc;
            }
            break;

            case InfoType::PhysicsMesh:
            {
                auto pDesc = KRG::New<Physics::PhysicsMeshResourceDescriptor>();
                pDesc->m_meshPath = ResourcePath::FromFileSystemPath( m_rawResourceDirectory, m_filePath );
                pDesc->m_meshName = m_selectedItemID.IsValid() ? m_selectedItemID.c_str() : "";
                m_pDescriptor = pDesc;
            }
            break;

            case InfoType::SkeletalMesh:
            {
                auto pDesc = KRG::New<Render::SkeletalMeshResourceDescriptor>();
                pDesc->m_meshPath = ResourcePath::FromFileSystemPath( m_rawResourceDirectory, m_filePath );
                pDesc->m_meshName = m_selectedItemID.IsValid() ? m_selectedItemID.c_str() : "";
                m_pDescriptor = pDesc;
            }
            break;

            case InfoType::Skeleton:
            {
                auto pDesc = KRG::New<Animation::SkeletonResourceDescriptor>();
                pDesc->m_skeletonPath = ResourcePath::FromFileSystemPath( m_rawResourceDirectory, m_filePath );
                pDesc->m_skeletonRootBoneName = m_selectedItemID.c_str();
                m_pDescriptor = pDesc;
            }
            break;

            case InfoType::Animation:
            {
                auto pDesc = KRG::New<Animation::AnimationClipResourceDescriptor>();
                pDesc->m_animationPath = ResourcePath::FromFileSystemPath( m_rawResourceDirectory, m_filePath );
                pDesc->m_animationName = m_selectedItemID.c_str();
                m_pDescriptor = pDesc;
            }
            break;

            default:
            KRG_UNREACHABLE_CODE();
            break;
        }

        m_propertyGrid.SetTypeToEdit( m_pDescriptor );
    }
}
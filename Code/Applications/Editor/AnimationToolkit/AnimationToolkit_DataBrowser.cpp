#include "AnimationToolkit_DataBrowser.h"
#include "FileTypes/SkeletonResourceFile.h"
#include "FileTypes/AnimationResourceFile.h"
#include "Tools/Resource/RawAssets/RawAssetReader.h"
#include "Tools/Animation/ResourceCompilers/AnimationCompiler.h"
#include "Tools/Animation/ResourceCompilers/SkeletonCompiler.h"
#include "Tools/Core/FileSystem/FileSystemHelpers.h"
#include "Tools/Core/TypeSystem/Serialization/TypeWriter.h"
#include "Engine/Animation/AnimationClip.h"
#include "System/DevTools/CommonWidgets/InterfaceHelpers.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Logging/Log.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    AnimationResourceDescriptorCreator::AnimationResourceDescriptorCreator( EditorModel* pModel )
        : ResourceDescriptorCreator( pModel )
    {
        m_decriptorTypes.push_back( { "Animation", AnimationClip::GetStaticResourceTypeID() } );
        m_decriptorTypes.push_back( { "Skeleton", Skeleton::GetStaticResourceTypeID() } );
    }

    void AnimationResourceDescriptorCreator::CreateNewDescriptor( FileSystem::Path const& rawFile ) const
    {
        int32 const selectedTypeIdx = GetSelectedTypeIndex();
        KRG_ASSERT( selectedTypeIdx < m_decriptorTypes.size() );

        TypeSystem::Serialization::TypeWriter typeWriter( m_pModel->GetTypeRegistry() );
        if ( selectedTypeIdx == 0 )
        {
            SkeletonResourceDescriptor desc;
            desc.m_skeletonDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), rawFile );
            typeWriter << desc;
        }
        else
        {
            AnimationResourceDescriptor desc;
            desc.m_animationDataPath = DataPath::FromFileSystemPath( m_pModel->GetSourceDataDirectory(), rawFile );
            typeWriter << desc;
        }
      
        typeWriter.WriteToFile( CreateDescriptorFilePath( rawFile ) );
    }
}

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    static Color const g_rawFileColor = Colors::LightGray;
    static Color const g_skeletonColor = Colors::HotPink;
    static Color const g_animationColor = Colors::GreenYellow;

    //-------------------------------------------------------------------------

    void DataBrowser::AnimationBrowserTreeItem::DrawControls()
    {
        if ( IsDirectory() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( IsRawFile() )
        {
            ImGui::TextColored( g_rawFileColor.ToFloat4(), "Raw" );
        }
        else if ( IsResourceOfType<Skeleton>() )
        {
            ImGui::TextColored( g_skeletonColor.ToFloat4(), "Skeleton" );
        }
        else if ( IsResourceOfType<AnimationClip>() )
        {
            ImGui::TextColored( g_animationColor.ToFloat4(), "Animation" );
        }
    }

    //-------------------------------------------------------------------------

    DataBrowser::DataBrowser( EditorModel* pModel )
        : TEditorTool<EditorModel>( pModel )
        , m_propertyGrid( pModel )
        , m_descriptorCreator( pModel )
        , m_dataBrowserTreeView( pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory(), { ".skel", ".anim", ".fbx", ".gltf" } )
    {
        m_onDoubleClickEventID = m_dataBrowserTreeView.OnItemDoubleClicked().Bind( [this] ( TreeViewItem* pItem ) { OnBrowserItemDoubleClicked( pItem ); } );
        m_dataBrowserTreeView.RebuildBrowserTree();
        UpdateVisibility();
    }

    DataBrowser::~DataBrowser()
    {
        m_dataBrowserTreeView.OnItemDoubleClicked().Unbind( m_onDoubleClickEventID );
    }

    //-------------------------------------------------------------------------

    void DataBrowser::FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager )
    {
        if ( !IsOpen() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        m_dataBrowserTreeView.Update( context );

        //-------------------------------------------------------------------------

        DrawBrowser( context );

        //-------------------------------------------------------------------------

        auto pSelectedItem = m_dataBrowserTreeView.GetSelectedItem<AnimationBrowserTreeItem>();
        if ( pSelectedItem != nullptr && pSelectedItem->IsFile() )
        {
            if ( pSelectedItem->GetPath() != m_inspectedFile )
            {
                m_inspectedFile = pSelectedItem->GetPath();

                if ( pSelectedItem->IsRawFile() )
                {
                    if ( RawAssets::ReadFileInfo( m_inspectedFile, m_assetInfo ) )
                    {
                        m_validAssetInfo = true;
                    }
                    else
                    {
                        m_validAssetInfo = false;
                        KRG_LOG_ERROR( "DataBrowser", "Failed to read raw resource file: %s", m_inspectedFile.c_str() );
                    }
                }
                else // Resource File
                {
                    /*if ( OpenInspectedResourceFile( context ) )
                    {
                        m_propertyGrid.SetTypeToEdit( &m_pResourceDescriptor );
                    }
                    else
                    {
                        m_propertyGrid.SetTypeToEdit( nullptr );
                        KRG_LOG_ERROR( "DataBrowser", "Failed to deserialize resource file: %s", m_inspectedFile.c_str() );
                    }*/
                }
            }
        }
        else // Clear inspected file
        {
            m_inspectedFile = FileSystem::Path();
            m_assetInfo.Reset();
        }

        //-------------------------------------------------------------------------

        DrawInfoPanel( context );
    }

    //-------------------------------------------------------------------------
    // Browser
    //-------------------------------------------------------------------------

    void DataBrowser::UpdateVisibility()
    {
        auto VisibilityFunc = [this] ( TreeViewItem const* pItem )
        {
            bool isVisible = true;

            // Type filter
            //-------------------------------------------------------------------------

            auto pDataFileItem = static_cast<DataBrowserTreeItem const*>( pItem );
            if ( pDataFileItem->IsFile() )
            {
                if ( pDataFileItem->IsRawFile() )
                {
                    isVisible = m_showRawFiles;
                }
                else
                {
                    isVisible = true;
                }
            }

            // Text filter
            //-------------------------------------------------------------------------

            if ( isVisible && m_filterBuffer[0] != 0 )
            {
                String lowercaseLabel = pItem->GetLabel();
                lowercaseLabel.make_lower();

                char tempBuffer[256];
                strcpy( tempBuffer, m_filterBuffer );

                char* token = strtok( tempBuffer, " " );
                while ( token )
                {
                    if ( lowercaseLabel.find( token ) == String::npos )
                    {
                        isVisible = false;
                        break;
                    }

                    token = strtok( NULL, " " );
                }
            }

            //-------------------------------------------------------------------------

            return isVisible;
        };

        //-------------------------------------------------------------------------

        m_dataBrowserTreeView.UpdateItemVisibility( VisibilityFunc );
    }

    void DataBrowser::DrawBrowser( UpdateContext const& context )
    {
        KRG_PROFILE_FUNCTION();

        if ( ImGui::Begin( "Data Browser", nullptr ) )
        {
            // Text Filter
            //-------------------------------------------------------------------------

            ImGui::SetNextItemWidth( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 31 );
            if ( ImGui::InputText( "##Filter", m_filterBuffer, 256 ) )
            {
                // Convert buffer to lower case
                int32 i = 0;
                while ( i < 256 && m_filterBuffer[i] != 0 )
                {
                    m_filterBuffer[i] = eastl::CharToLower( m_filterBuffer[i] );
                    i++;
                }

                UpdateVisibility();
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
            if ( ImGui::Button( KRG_ICON_ERASER "##Clear Filter" ) )
            {
                m_filterBuffer[0] = 0;
                UpdateVisibility();
            }

            // Type Filter + Controls
            //-------------------------------------------------------------------------

            if ( ImGui::Checkbox( "Raw Files", &m_showRawFiles ) )
            {
                UpdateVisibility();
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 42 );
            if ( ImGui::Button( KRG_ICON_PLUS "##Expand All" ) )
            {
                m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( true ); } );
            }

            ImGui::SameLine( ImGui::GetWindowContentRegionWidth() + ImGui::GetStyle().WindowPadding.x - 20 );
            if ( ImGui::Button( KRG_ICON_MINUS "##Collapse ALL" ) )
            {
                m_dataBrowserTreeView.ForEachItem( [] ( TreeViewItem* pItem ) { pItem->SetExpanded( false ); } );
            }

            //-------------------------------------------------------------------------

            ImGui::Separator();

            // Browser
            //-------------------------------------------------------------------------

            m_dataBrowserTreeView.Draw();
        }
        ImGui::End();
    }

    //-------------------------------------------------------------------------

    void DataBrowser::OnBrowserItemDoubleClicked( TreeViewItem* pItem )
    {
        auto pDataFileItem = static_cast<DataBrowserTreeItem const*>( pItem );
        if ( pDataFileItem->IsDirectory() || pDataFileItem->IsRawFile() )
        {
            return;
        }

        if ( pDataFileItem->IsResourceOfType<Skeleton>() )
        {
            ResourceID const resourceID( pDataFileItem->GetDataPath() );
            m_model.OpenResourceFile<SkeletonResourceFile>( resourceID );
        }
        else if ( pDataFileItem->IsResourceOfType<AnimationClip>() )
        {
            ResourceID const resourceID( pDataFileItem->GetDataPath() );
            m_model.OpenResourceFile<AnimationResourceFile>( resourceID );
        }
    }

    bool DataBrowser::OpenInspectedResourceFile( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedFile.ExistsAndIsFile() );

        TypeSystem::Serialization::TypeReader typeReader( m_model.GetTypeRegistry() );
        if ( typeReader.ReadFromFile( m_inspectedFile ) )
        {
            TypeSystem::TypeDescriptor typeDesc;
            if ( typeReader.ReadType( typeDesc ) )
            {
                m_pResourceDescriptor = TypeSystem::TypeCreator::CreateTypeFromDescriptor<Resource::ResourceDescriptor>( m_model.GetTypeRegistry(), typeDesc );
                return true;
            }
        }

        return false;
    }

    bool DataBrowser::SaveInspectedResourceFile( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedFile.ExistsAndIsFile() && m_pResourceDescriptor != nullptr );

        TypeSystem::Serialization::TypeWriter typeWriter( m_model.GetTypeRegistry() );
        //typeWriter << m_typeInstance;
        return typeWriter.WriteToFile( m_inspectedFile );
    }

    //-------------------------------------------------------------------------
    // Info Panel
    //-------------------------------------------------------------------------

    void DataBrowser::DrawInfoPanel( UpdateContext const& context )
    {
        if ( ImGui::Begin( "Data File Info" ) )
        {
            if ( m_inspectedFile.IsValid() )
            {
                auto pSelectedItem = m_dataBrowserTreeView.GetSelectedItem<AnimationBrowserTreeItem>();
                KRG_ASSERT( pSelectedItem != nullptr && pSelectedItem->IsFile() );
                if ( pSelectedItem->IsRawFile() )
                {
                    DrawRawFileInfo( context );
                }
                //else if ( m_typeInstance.IsValid() )
                //{
                //    DrawResourceFileInfo( context );
                //}
                //else
                //{
                //    // Do Nothing
                //}
            }
        }
        ImGui::End();
    }

    void DataBrowser::DrawResourceFileInfo( UpdateContext const& context )
    {
        //KRG_ASSERT( m_typeInstance.IsValid() && m_inspectedFile.IsFilePath() );

        ////-------------------------------------------------------------------------

        //float const cellContentWidth = ImGui::GetContentRegionAvail().x;
        //float const itemSpacing = ImGui::GetStyle().ItemSpacing.x / 2;
        //float const buttonAreaWidth = 80;
        //float const textAreaWidth = cellContentWidth - buttonAreaWidth - itemSpacing;
        //float const buttonStartPosX = textAreaWidth + itemSpacing;

        //ImGui::AlignTextToFramePadding();
        //ImGui::Text( m_inspectedFile.GetFileName().c_str() );
        //float const actualTextWidth = ImGui::GetItemRectSize().x;

        //ImGui::SameLine( 0, textAreaWidth - actualTextWidth + itemSpacing );
        //if ( m_propertyGrid.IsDirty() )
        //{
        //    if ( ImGuiX::ButtonColored( Colors::LimeGreen.ToFloat4(), KRG_ICON_FLOPPY_O " Save", ImVec2( buttonAreaWidth, 0 ) ) )
        //    {
        //        SaveInspectedResourceFile( context );
        //        m_propertyGrid.ClearDirty();
        //    }
        //}
        //else
        //{
        //    ImGuiX::ButtonColored( ImGuiX::Theme::s_textColorDisabled, KRG_ICON_FLOPPY_O " Save", ImVec2( buttonAreaWidth, 0 ) );
        //}

        ////-------------------------------------------------------------------------

        //m_propertyGrid.DrawGrid();
    }

    void DataBrowser::DrawRawFileInfo( UpdateContext const& context )
    {
        KRG_ASSERT( m_inspectedFile.IsFilePath() );

        ImGui::Text( "Raw File: %s", m_inspectedFile.c_str() );
        ImGui::Separator();

        if ( m_validAssetInfo )
        {
            // Asset Info
            //-------------------------------------------------------------------------

            ImGuiTableFlags flags = ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_RowBg;
            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 4, 4 ) );
            if ( ImGui::BeginTable( "Animation Info", 2, 0 ) )
            {
                ImGui::TableSetupColumn( "Type", ImGuiTableColumnFlags_WidthFixed, 120 );
                ImGui::TableSetupColumn( "Animation Name", ImGuiTableColumnFlags_NoHide );

                for ( auto const& mesh : m_assetInfo.m_meshes )
                {
                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    ImGui::AlignTextToFramePadding();
                    if ( mesh.m_isSkinned )
                    {
                        ImGui::TextColored( g_skeletonColor.ToFloat4(), "Skeletal Animation:" );

                    }
                    else // Static
                    {
                        ImGui::TextColored( g_animationColor.ToFloat4(), "Static Animation:" );
                    }

                    //-------------------------------------------------------------------------

                    ImGui::TableNextColumn();
                    ImGuiX::SelectableText( mesh.m_name, -1 );
                }

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();

            // Create new descriptor
            //-------------------------------------------------------------------------

            if ( ImGui::Button( "Create New Descriptor", ImVec2( -1, 0 ) ) )
            {
                ImGui::OpenPopup( "Create New Descriptor##Modal" );
            }

            if ( ImGui::BeginPopupModal( "Create New Descriptor##Modal", NULL, ImGuiWindowFlags_AlwaysAutoResize ) )
            {
                if ( m_descriptorCreator.ShowCreatorDialog( m_inspectedFile ) )
                {
                    ImGui::CloseCurrentPopup();
                }

                //-------------------------------------------------------------------------

                ImGui::EndPopup();
            }
        }
    }
}
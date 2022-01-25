#include "Workspace_AnimationClip.h"
#include "Tools/Animation/Events/AnimationEventEditor.h"
#include "Tools/Animation/Events/AnimationEventData.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationClip.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "Engine/Animation/Components/Component_AnimationClipPlayer.h"
#include "Engine/Animation/Systems/EntitySystem_Animation.h"
#include "Engine/Render/Components/Component_SkeletalMesh.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "System/Animation/AnimationPose.h"
#include "System/Core/Math/MathStringHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationClipWorkspaceFactory, AnimationClip, AnimationClipWorkspace );

    //-------------------------------------------------------------------------

    AnimationClipWorkspace::AnimationClipWorkspace( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : TResourceWorkspace<AnimationClip>( context, pWorld, resourceID )
        , m_propertyGrid( *m_pTypeRegistry, *m_pResourceDatabase )
    {}

    AnimationClipWorkspace::~AnimationClipWorkspace()
    {
        KRG_ASSERT( m_pEventEditor == nullptr );
    }

    void AnimationClipWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0, bottomDockID = 0, bottomLeftDockID = 0, bottomRightDockID = 0;
        ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.33f, &bottomDockID, &topDockID );
        ImGui::DockBuilderSplitNode( bottomDockID, ImGuiDir_Right, 0.25f, &bottomRightDockID, &bottomLeftDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), topDockID );
        ImGui::DockBuilderDockWindow( m_timelineWindowName.c_str(), bottomLeftDockID );
        ImGui::DockBuilderDockWindow( m_trackDataWindowName.c_str(), bottomRightDockID );
        ImGui::DockBuilderDockWindow( m_detailsWindowName.c_str(), bottomRightDockID );
        ImGui::DockBuilderDockWindow( m_descriptorWindowName.c_str(), bottomRightDockID );
    }

    //-------------------------------------------------------------------------

    void AnimationClipWorkspace::Initialize( UpdateContext const& context )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        TResourceWorkspace<AnimationClip>::Initialize( context );

        m_timelineWindowName.sprintf( "Timeline##%u", GetID() );
        m_detailsWindowName.sprintf( "Details##%u", GetID() );
        m_trackDataWindowName.sprintf( "Track Data##%u", GetID() );

        CreatePreviewEntity();
    }

    void AnimationClipWorkspace::Shutdown( UpdateContext const& context )
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        if ( m_pEventEditor != nullptr )
        {
            KRG::Delete( m_pEventEditor );
        }

        m_pPreviewEntity = nullptr;
        m_pAnimationComponent = nullptr;
        m_pMeshComponent = nullptr;

        TResourceWorkspace<AnimationClip>::Shutdown( context );
    }

    void AnimationClipWorkspace::CreatePreviewEntity()
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        // Load resource descriptor for skeleton to get the preview mesh
        auto pAnimClipDescriptor = GetDescriptorAs<AnimationClipResourceDescriptor>();
        if ( pAnimClipDescriptor->m_pSkeleton.IsValid() )
        {
            FileSystem::Path const resourceDescPath = GetFileSystemPath( pAnimClipDescriptor->m_pSkeleton.GetResourcePath() );
            SkeletonResourceDescriptor resourceDesc;
            if ( TryReadResourceDescriptorFromFile( *m_pTypeRegistry, resourceDescPath, resourceDesc ) && resourceDesc.m_previewMesh.IsValid() )
            {
                // Create a preview mesh component
                m_pMeshComponent = KRG::New<Render::SkeletalMeshComponent>( StringID( "Mesh Component" ) );
                m_pMeshComponent->SetSkeleton( pAnimClipDescriptor->m_pSkeleton.GetResourceID() );
                if ( resourceDesc.m_previewMesh.IsValid() )
                {
                    m_pMeshComponent->SetMesh( resourceDesc.m_previewMesh.GetResourceID() );
                }
            }
        }

        //-------------------------------------------------------------------------

        // Create animation component
        m_pAnimationComponent = KRG::New<AnimationClipPlayerComponent>( StringID( "Animation Component" ) );
        m_pAnimationComponent->SetAnimation( m_pResource.GetResourceID() );

        //-------------------------------------------------------------------------

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        m_pPreviewEntity->CreateSystem<AnimationSystem>();
        m_pPreviewEntity->AddComponent( m_pAnimationComponent );

        if ( m_pMeshComponent != nullptr )
        {
            m_pPreviewEntity->AddComponent( m_pMeshComponent );
        }

        AddEntityToWorld( m_pPreviewEntity );
    }

    void AnimationClipWorkspace::BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToBeReloaded, TVector<ResourceID> const& resourcesToBeReloaded )
    {
        TResourceWorkspace<AnimationClip>::BeginHotReload( usersToBeReloaded, resourcesToBeReloaded );

        // If someone messed with this resource outside of this editor - destroy the event editor!
        if ( m_pDescriptor == nullptr )
        {
            if ( m_pEventEditor != nullptr )
            {
                KRG::Delete( m_pEventEditor );
            }

            m_propertyGrid.SetTypeToEdit( nullptr );
        }

        if ( IsHotReloading() )
        {
            if ( m_pMeshComponent != nullptr )
            {
                m_pPreviewEntity->DestroyComponent( m_pMeshComponent );
                m_pMeshComponent = nullptr;
            }
        }
    }

    void AnimationClipWorkspace::EndHotReload()
    {
        TResourceWorkspace<AnimationClip>::EndHotReload();

        if ( m_pPreviewEntity == nullptr )
        {
            CreatePreviewEntity();
        }
    }

    //-------------------------------------------------------------------------

    void AnimationClipWorkspace::UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        if ( IsResourceLoaded() )
        {
            // Lazy init of the event editor - since we need to wait for the resource to be loaded
            if ( m_pEventEditor == nullptr )
            {
                m_pEventEditor = KRG::New<EventEditor>( *m_pTypeRegistry, m_descriptorPath, m_pResource->GetNumFrames(), m_pResource->GetFPS() );
            }

            // Update position
            //-------------------------------------------------------------------------

            if ( m_isRootMotionEnabled )
            {
                m_characterTransform = m_pResource->GetRootTransform( m_pAnimationComponent->GetAnimTime() );
            }
            else
            {
                m_characterTransform = Transform::Identity;
            }

            // HACK
            if ( m_pMeshComponent != nullptr )
            {
                m_pMeshComponent->SetWorldTransform( m_characterTransform );
            }

            // Draw in viewport
            //-------------------------------------------------------------------------

            auto drawingCtx = GetDrawingContext();

            Pose const* pPose = m_pAnimationComponent->GetPose();
            if ( pPose != nullptr )
            {
                drawingCtx.Draw( *pPose, m_characterTransform );
            }

            m_pResource->DrawRootMotionPath( drawingCtx, Transform::Identity );
        }

        //-------------------------------------------------------------------------

        DrawDescriptorWindow( context, pWindowClass );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawTrackDataWindow( context );

        ImGui::SetNextWindowClass( pWindowClass );
        DrawTimelineWindow( context );

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( m_detailsWindowName.c_str() ) )
        {
            m_propertyGrid.DrawGrid();
        }
        ImGui::End();
    }

    void AnimationClipWorkspace::DrawViewportToolbar( UpdateContext const& context, Render::Viewport const* pViewport )
    {
        if ( !IsResourceLoaded() )
        {
            return;
        }

        //-------------------------------------------------------------------------

        ImGui::SetNextItemWidth( 46 );
        if ( ImGui::BeginCombo( "##AnimOptions", KRG_ICON_COG, ImGuiComboFlags_HeightLarge ) )
        {
            ImGui::MenuItem( "Root Motion", nullptr, &m_isRootMotionEnabled );

            ImGui::EndCombo();
        }

        ImGui::Indent();

        ImVec4 const color = ImGuiX::ConvertColor( Colors::Yellow );
        ImGui::TextColored( color, "Avg Linear Velocity: %.2f m/s", m_pResource->GetAverageLinearVelocity() );
        ImGui::TextColored( color, "Avg Angular Velocity: %.2f r/s", m_pResource->GetAverageAngularVelocity().ToFloat() );
        ImGui::TextColored( color, "Distance Covered: %.2fm", m_pResource->GetTotalRootMotionDelta().GetTranslation().GetLength3() );

        if ( m_pEventEditor != nullptr )
        {
            ImGui::TextColored( color, "Frame: %.f/%d", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetNumFrames(), m_pResource->GetNumFrames() );
            ImGui::TextColored( color, "Time: %.2fs/%0.2fs", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetDuration(), m_pResource->GetDuration().ToFloat() );
        }

        ImGui::Unindent();
    }

    void AnimationClipWorkspace::DrawTimelineWindow( UpdateContext const& context )
    {
        // Draw timeline window
        //-------------------------------------------------------------------------

        ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 0, 0 ) );
        if ( ImGui::Begin( m_timelineWindowName.c_str() ) )
        {
            if ( IsWaitingForResource() )
            {
                ImGui::Text( "Loading:" );
                ImGui::SameLine();
                ImGuiX::DrawSpinner( "Loading" );
            }
            else if ( HasLoadingFailed() )
            {
                ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
            }
            else
            {
                // Track editor and property grid
                //-------------------------------------------------------------------------

                m_pEventEditor->UpdateAndDraw( context, m_pAnimationComponent );

                // Transfer dirty state from property grid
                if ( m_propertyGrid.IsDirty() )
                {
                    m_pEventEditor->MarkDirty();
                }

                // Handle selection changes
                auto const& selectedItems = m_pEventEditor->GetSelectedItems();
                if ( !selectedItems.empty() )
                {
                    auto pAnimEventItem = static_cast<EventItem*>( selectedItems.back() );
                    m_propertyGrid.SetTypeToEdit( pAnimEventItem->GetEvent() );
                }
                else // Clear property grid
                {
                    m_propertyGrid.SetTypeToEdit( nullptr );
                }
            }
        }
        ImGui::End();
        ImGui::PopStyleVar();
    }

    void AnimationClipWorkspace::DrawTrackDataWindow( UpdateContext const& context )
    {
        if ( ImGui::Begin( m_trackDataWindowName.c_str() ) )
        {
            if ( IsResourceLoaded() )
            {
                // There may be a frame delay between the UI and the entity system creating the pose
                Pose const* pPose = m_pAnimationComponent->GetPose();
                if ( pPose != nullptr )
                {
                    if ( ImGui::BeginTable( "TrackDataTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg ) )
                    {
                        ImGui::TableSetupColumn( "Bone", ImGuiTableColumnFlags_WidthStretch );
                        ImGui::TableSetupColumn( "Transform", ImGuiTableColumnFlags_WidthStretch );
                        ImGui::TableHeadersRow();

                        //-------------------------------------------------------------------------

                        Skeleton const* pSkeleton = pPose->GetSkeleton();
                        int32 const numBones = pSkeleton->GetNumBones();

                        ImGuiListClipper clipper;
                        clipper.Begin( numBones );
                        while ( clipper.Step() )
                        {
                            for ( int boneIdx = clipper.DisplayStart; boneIdx < clipper.DisplayEnd; boneIdx++ )
                            {
                                Transform const& boneTransform = ( boneIdx == 0 ) ? m_pResource->GetRootTransform( m_pAnimationComponent->GetAnimTime() ) : pPose->GetGlobalTransform( boneIdx );

                                ImGui::TableNextColumn();
                                ImGui::Text( "%d. %s", boneIdx, pSkeleton->GetBoneID( boneIdx ).c_str() );

                                ImGui::TableNextColumn();
                                ImGuiX::DisplayTransform( boneTransform );
                            }
                        }
                        clipper.End();

                        ImGui::EndTable();
                    }
                }
            }
            else
            {
                ImGui::Text( "Nothing to show!" );
            }
        }
        ImGui::End();
    }

    bool AnimationClipWorkspace::IsDirty() const
    {
        if ( TResourceWorkspace<AnimationClip>::IsDirty() )
        {
            return true;
        }

        if ( m_pEventEditor != nullptr && m_pEventEditor->IsDirty() )
        {
            return m_pEventEditor->IsDirty();
        }

        return false;
    }

    bool AnimationClipWorkspace::Save()
    {
        if ( !TResourceWorkspace<AnimationClip>::Save() )
        {
            return false;
        }

        if ( m_pEventEditor != nullptr && !m_pEventEditor->RequestSave() )
        {
            return false;
        }

        m_propertyGrid.ClearDirty();
        return true;
    }
}
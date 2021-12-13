#include "Workspace_AnimationClip.h"
#include "Tools/Animation/Events/AnimationEventEditor.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "Engine/Animation/AnimationPose.h"
#include "Engine/Animation/Components/Component_AnimationClipPlayer.h"
#include "Engine/Animation/Components/Component_AnimatedMeshes.h"
#include "Engine/Animation/Systems/EntitySystem_Animation.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "Engine/Core/Update/UpdateContext.h"
#include "Engine/Core/DevUI/NumericUIHelpers.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationClipWorkspaceFactory, AnimationClip, AnimationClipWorkspace );

    //-------------------------------------------------------------------------

    AnimationClipWorkspace::AnimationClipWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : TResourceWorkspace<AnimationClip>( context, pWorld, resourceID )
        , m_propertyGrid( *context.m_pTypeRegistry, *context.m_pResourceDatabase )
    {}

    AnimationClipWorkspace::~AnimationClipWorkspace()
    {
        if ( m_pEventEditor != nullptr )
        {
            KRG::Delete( m_pEventEditor );
        }
    }

    void AnimationClipWorkspace::Initialize( UpdateContext const& context )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        TResourceWorkspace<AnimationClip>::Initialize( context );

        m_timelineWindowName.sprintf( "Timeline##%u", GetID() );
        m_detailsWindowName.sprintf( "Details##%u", GetID() );
        m_trackDataWindowName.sprintf( "Track Data##%u", GetID() );

        //-------------------------------------------------------------------------

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        m_pPreviewEntity->CreateSystem<AnimationSystem>();

        m_pAnimationComponent = KRG::New<AnimationClipPlayerComponent>( StringID( "Animation Component" ) );
        m_pAnimationComponent->SetAnimation( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( m_pAnimationComponent );

        // We dont own the entity as soon as we add it to the map
        m_pWorld->GetPersistentMap()->AddEntity( m_pPreviewEntity );
    }

    void AnimationClipWorkspace::Shutdown( UpdateContext const& context )
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        m_pPreviewEntity = nullptr;
        m_pAnimationComponent = nullptr;
        m_pMeshComponent = nullptr;

        TResourceWorkspace<AnimationClip>::Shutdown( context );
    }

    void AnimationClipWorkspace::BeginHotReload( TVector<ResourceID> const& resourcesToBeReloaded )
    {
        m_pPreviewEntity->DestroyComponent( m_pMeshComponent );
        m_pMeshComponent = nullptr;
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

    void AnimationClipWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        if ( IsLoaded() )
        {
            // Lazy init of the event editor
            if ( m_pEventEditor == nullptr )
            {
                m_pEventEditor = KRG::New<EventEditor>( *m_editorContext.m_pTypeRegistry, m_editorContext.GetRawResourceDirectoryPath(), m_pResource.GetPtr() );
            }

            // Initialize preview mesh
            if ( m_pMeshComponent == nullptr && m_pPreviewEntity->IsActivated() )
            {
                // Load resource descriptor for skeleton to get the preview mesh
                FileSystem::Path const resourceDescPath = m_editorContext.ToFileSystemPath( m_pResource->GetSkeleton()->GetResourcePath() );
                SkeletonResourceDescriptor resourceDesc;
                TryReadResourceDescriptorFromFile( *m_editorContext.m_pTypeRegistry, resourceDescPath, resourceDesc );

                // Create a preview mesh component
                m_pMeshComponent = KRG::New<AnimatedMeshComponent>( StringID( "Mesh Component" ) );
                m_pMeshComponent->SetSkeleton( m_pResource->GetSkeleton()->GetResourceID() );
                if ( resourceDesc.m_previewMesh.IsValid() )
                {
                    m_pMeshComponent->SetMesh( resourceDesc.m_previewMesh.GetResourceID() );
                }
                m_pPreviewEntity->AddComponent( m_pMeshComponent );
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
        if ( !IsLoaded() )
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
        ImGui::TextColored( color, "Avg Linear Velocity: %.2fm/s", m_pResource->GetAverageLinearVelocity() );
        ImGui::TextColored( color, "Avg Angular Velocity: %.2fm/s", m_pResource->GetAverageAngularVelocity().ToFloat() );
        ImGui::TextColored( color, "Distance Covered: %.2fm", m_pResource->GetTotalRootMotionDelta().GetTranslation().GetLength3() );
        ImGui::TextColored( color, "Frame: %.f/%d", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetNumFrames(), m_pResource->GetNumFrames() );
        ImGui::TextColored( color, "Time: %.2fs/%0.2fs", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetDuration(), m_pResource->GetDuration().ToFloat() );

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
            if ( IsLoaded() )
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
#include "Workspace_AnimationClip.h"
#include "Tools/Animation/Events/AnimationEventEditor.h"
#include "Tools/Animation/ResourceDescriptors/ResourceDescriptor_AnimationSkeleton.h"
#include "Tools/Core/Widgets/InterfaceHelpers.h"
#include "Engine/Animation/AnimationPose.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "Engine/Animation/Systems/AnimationSystem.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "System/Core/Math/MathStringHelpers.h"
#include "System/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationClipWorkspaceFactory, AnimationClip, AnimationClipWorkspace );

    //-------------------------------------------------------------------------

    AnimationClipWorkspace::AnimationClipWorkspace( EditorContext const& context, EntityWorld* pWorld, ResourceID const& resourceID )
        : TResourceWorkspace<AnimationClip>( context, pWorld, resourceID )
        , m_propertyGrid( *context.m_pTypeRegistry, context.m_sourceResourceDirectory )
    {}

    AnimationClipWorkspace::~AnimationClipWorkspace()
    {
        if ( m_pEventEditor != nullptr )
        {
            KRG::Delete( m_pEventEditor );
        }
    }

    void AnimationClipWorkspace::Initialize()
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        TResourceWorkspace<AnimationClip>::Initialize();

        m_timelineWindowName.sprintf( "Timeline##%u", GetID() );
        m_detailsWindowName.sprintf( "Details##%u", GetID() );
        m_trackDataWindowName.sprintf( "Track Data##%u", GetID() );

        //-------------------------------------------------------------------------

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        m_pPreviewEntity->CreateSystem<AnimationSystem>();

        m_pAnimationComponent = KRG::New<AnimationPlayerComponent>( StringID( "Animation Component" ) );
        m_pAnimationComponent->SetAnimation( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( m_pAnimationComponent );

        // We dont own the entity as soon as we add it to the map
        m_pWorld->GetPersistentMap()->AddEntity( m_pPreviewEntity );
    }

    void AnimationClipWorkspace::Shutdown()
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        m_pPreviewEntity = nullptr;
        m_pAnimationComponent = nullptr;
        m_pMeshComponent = nullptr;
    }

    void AnimationClipWorkspace::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomLeftDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );
        ImGuiID bottomRightDockID = ImGui::DockBuilderSplitNode( bottomDockID, ImGuiDir_Right, 0.25f, nullptr, &bottomLeftDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowID(), topDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( m_timelineWindowName.c_str(), bottomLeftDockID );
        ImGui::DockBuilderDockWindow( m_trackDataWindowName.c_str(), bottomRightDockID );
        ImGui::DockBuilderDockWindow( m_detailsWindowName.c_str(), bottomRightDockID );
    }

    void AnimationClipWorkspace::UpdateAndDrawWindows( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        if ( IsLoaded() )
        {
            // Lazy init of the event editor
            if ( m_pEventEditor == nullptr )
            {
                m_pEventEditor = KRG::New<EventEditor>( *m_editorContext.m_pTypeRegistry, m_editorContext.m_sourceResourceDirectory, m_pResource.GetPtr() );
            }

            // Initialize preview mesh
            if ( m_pMeshComponent == nullptr )
            {
                // Load resource descriptor for skeleton to get the preview mesh
                FileSystem::Path const resourceDescPath = m_pResource->GetSkeleton()->GetResourcePath().ToFileSystemPath( m_editorContext.m_sourceResourceDirectory );
                SkeletonResourceDescriptor resourceDesc;
                TryReadResourceDescriptorFromFile( *m_editorContext.m_pTypeRegistry, resourceDescPath, resourceDesc );

                // Create a preview mesh component
                m_pMeshComponent = KRG::New<AnimatedMeshComponent>( StringID( "Mesh Component" ) );
                m_pMeshComponent->SetSkeleton( m_pResource->GetSkeleton()->GetResourceID() );
                m_pMeshComponent->SetMesh( resourceDesc.m_previewMesh.GetResourceID() );
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

        ImGui::AlignTextToFramePadding();
        ImGui::Text( "Frame: %06.2f / %d", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetNumFrames(), m_pResource->GetNumFrames() );

        ImGuiX::VerticalSeparator();

        ImGui::Text( "Time: %05.2fs / %05.2fs", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetDuration(), m_pResource->GetDuration().ToFloat() );

        ImGuiX::VerticalSeparator();

        ImGui::Text( "Avg Linear Velocity: %.2fm/s", m_pResource->GetAverageLinearVelocity() );

        ImGuiX::VerticalSeparator();

        ImGui::Text( "Avg Angular Velocity: %.2fm/s", m_pResource->GetAverageAngularVelocity().ToFloat() );

        ImGuiX::VerticalSeparator();

        ImGui::Text( "Distance Covered: %.2fm", m_pResource->GetTotalRootMotionDelta().GetTranslation().GetLength3() );

        ImGuiX::VerticalSeparator();

        ImGui::Checkbox( "Root Motion", &m_isRootMotionEnabled );

        ImGuiX::VerticalSeparator();
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

                        Transform const rootTransform = m_isRootMotionEnabled ? m_pResource->GetRootTransform( m_pAnimationComponent->GetAnimTime() ) : Transform::Identity;

                        ImGui::TableNextColumn();
                        ImGui::Text( "0. Root" );

                        ImGui::TableNextColumn();
                        ImGui::Text( "Rot: %s", Math::ToString( rootTransform.GetRotation() ).c_str() );
                        ImGui::Text( "Tra: %s", Math::ToString( rootTransform.GetTranslation() ).c_str() );
                        ImGui::Text( "Scl: %s", Math::ToString( rootTransform.GetScale() ).c_str() );

                        //-------------------------------------------------------------------------

                        Skeleton const* pSkeleton = pPose->GetSkeleton();
                        int32 const numBones = pSkeleton->GetNumBones();

                        for ( auto i = 1; i < numBones; i++ )
                        {
                            Transform const& boneTransform = pPose->GetGlobalTransform( i );

                            ImGui::TableNextColumn();
                            ImGui::Text( "%d. %s", i, pSkeleton->GetBoneID( i ).c_str() );

                            ImGui::TableNextColumn();
                            ImGui::Text( "Rot: %s", Math::ToString( boneTransform.GetRotation() ).c_str() );
                            ImGui::Text( "Tra: %s", Math::ToString( boneTransform.GetTranslation() ).c_str() );
                            ImGui::Text( "Scl: %s", Math::ToString( boneTransform.GetScale() ).c_str() );
                        }

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
        return ( m_pEventEditor != nullptr ) ? m_pEventEditor->IsDirty() : false;
    }

    bool AnimationClipWorkspace::Save()
    {
        if ( m_pEventEditor != nullptr )
        {
            if ( m_pEventEditor->RequestSave() )
            {
                m_propertyGrid.ClearDirty();
                return true;
            }
        }

        return false;
    }
}
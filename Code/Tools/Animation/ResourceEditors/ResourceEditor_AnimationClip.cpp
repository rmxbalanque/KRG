#include "ResourceEditor_AnimationClip.h"
#include "Tools/Animation/Events/AnimationEventEditor.h"
#include "Engine/Animation/Components/AnimationPlayerComponent.h"
#include "Engine/Animation/Systems/AnimationSystem.h"
#include "System/Imgui/Widgets/InterfaceHelpers.h"
#include "System/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    KRG_RESOURCE_WORKSPACE_FACTORY( AnimationClipWorkspaceFactory, AnimationClip, AnimationClipResourceEditor );

    //-------------------------------------------------------------------------

    char const* const AnimationClipResourceEditor::s_infoWindowName = "Info Window##AnimationClip";

    //-------------------------------------------------------------------------

    AnimationClipResourceEditor::AnimationClipResourceEditor( EditorModel* pModel, ResourceID const& resourceID )
        : TResourceEditorWorkspace<AnimationClip>( pModel, resourceID )
        , m_propertyGrid( *pModel->GetTypeRegistry(), pModel->GetSourceDataDirectory() )
    {}

    AnimationClipResourceEditor::~AnimationClipResourceEditor()
    {
        if ( m_pEventEditor != nullptr )
        {
            KRG::Delete( m_pEventEditor );
        }
    }

    void AnimationClipResourceEditor::Activate( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity == nullptr );

        // We dont own the entity as soon as we add it to the map
        auto pPersistentMap = pPreviewWorld->GetPersistentMap();

        m_pPreviewEntity = KRG::New<Entity>( StringID( "Preview" ) );
        pPersistentMap->AddEntity( m_pPreviewEntity );

        m_pPreviewEntity->CreateSystem<AnimationSystem>();

        m_pAnimationComponent = KRG::New<AnimationPlayerComponent>( StringID( "Animation Component" ) );
        m_pAnimationComponent->SetAnimation( m_pResource.GetResourceID() );
        m_pPreviewEntity->AddComponent( m_pAnimationComponent );
    }

    void AnimationClipResourceEditor::Deactivate( EntityWorld* pPreviewWorld )
    {
        KRG_ASSERT( m_pPreviewEntity != nullptr );

        auto pPersistentMap = pPreviewWorld->GetPersistentMap();
        pPersistentMap->DestroyEntity( m_pPreviewEntity->GetID() );
        m_pPreviewEntity = nullptr;
        m_pAnimationComponent = nullptr;
    }

    void AnimationClipResourceEditor::InitializeDockingLayout( ImGuiID dockspaceID ) const
    {
        ImGuiID topDockID = 0;
        ImGuiID bottomDockID = ImGui::DockBuilderSplitNode( dockspaceID, ImGuiDir_Down, 0.5f, nullptr, &topDockID );

        // Dock viewport
        ImGuiDockNode* pTopNode = ImGui::DockBuilderGetNode( topDockID );
        pTopNode->LocalFlags |= ImGuiDockNodeFlags_NoDockingSplitMe | ImGuiDockNodeFlags_NoDockingOverMe;
        ImGui::DockBuilderDockWindow( GetViewportWindowName(), topDockID );

        // Dock windows
        ImGui::DockBuilderDockWindow( s_infoWindowName, bottomDockID );
    }

    void AnimationClipResourceEditor::Draw( UpdateContext const& context, Render::ViewportManager& viewportManager, ImGuiWindowClass* pWindowClass )
    {
        auto DrawWindowContents = [this, &context] ()
        {
            if ( IsWaitingForResource() )
            {
                ImGui::Text( "Loading:" );
                ImGui::SameLine();
                ImGuiX::DrawSpinner( "Loading" );
                return;
            }

            if ( HasLoadingFailed() )
            {
                ImGui::Text( "Loading Failed: %s", m_pResource.GetResourceID().c_str() );
                return;
            }

            if ( m_pEventEditor == nullptr )
            {
                m_pEventEditor = KRG::New<EventEditor>( *m_pModel->GetTypeRegistry(), m_pModel->GetSourceDataDirectory(), m_pResource.GetPtr() );
            }

            // Draw debug info in viewport
            //-------------------------------------------------------------------------

            auto drawingCtx = context.GetDrawingContext();
            m_pResource->DrawRootMotionPath( drawingCtx, Transform::Identity );

            // Anim Info
            //-------------------------------------------------------------------------

            ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 8, 0 ) );
            ImGui::BeginChild( "AnimInfo", ImVec2( 0, 18 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
            {
                ImGui::AlignTextToFramePadding();
                ImGui::Text( "Frame: %06.2f / %d", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetNumFrames(), m_pResource->GetNumFrames() );

                ImGuiX::VerticalSeparator();

                ImGui::Text( "Time: %05.2fs / %05.2fs", m_pEventEditor->GetPlayheadPositionAsPercentage().ToFloat() * m_pResource->GetDuration(), m_pResource->GetDuration().ToFloat() );

                ImGuiX::VerticalSeparator();

                ImGui::Text( "Avg Linear Velocity: %.2fm/s", m_pResource->GetAverageLinearVelocity() );

                ImGuiX::VerticalSeparator();

                ImGui::Text( "Avg Angular Velocity: %.2fm/s", m_pResource->GetAverageAngularVelocity().ToFloat() );

                ImGuiX::VerticalSeparator();

                ImGui::Text( "Distance Covered: %.2fm", m_pResource->GetTotalRootMotionDelta().GetTranslation().Length3() );
            }
            ImGui::EndChild();
            ImGui::PopStyleVar();

            // Track editor and property grid
            //-------------------------------------------------------------------------

            ImGui::PushStyleVar( ImGuiStyleVar_CellPadding, ImVec2( 0, 0 ) );
            if ( ImGui::BeginTable( "LayoutTable", 2, ImGuiTableFlags_Resizable | ImGuiTableFlags_Borders | ImGuiTableFlags_SizingFixedFit ) )
            {
                ImGui::TableSetupColumn( "TrackEditor", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "PropertyGrid", ImGuiTableColumnFlags_WidthFixed, 400 );

                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                ImGui::BeginChild( "TE", ImVec2( 0, 0 ), false );
                {
                    m_pEventEditor->Update( context, m_pAnimationComponent );
                }
                ImGui::EndChild();

                //-------------------------------------------------------------------------

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

                //-------------------------------------------------------------------------

                ImGui::TableNextColumn();
                ImGui::PushStyleVar( ImGuiStyleVar_WindowPadding, ImVec2( 4, 0 ) );
                ImGui::BeginChild( "PG", ImVec2( 0, 0 ), false, ImGuiWindowFlags_AlwaysUseWindowPadding );
                {
                    m_propertyGrid.DrawGrid();
                }
                ImGui::EndChild();
                ImGui::PopStyleVar();

                ImGui::EndTable();
            }
            ImGui::PopStyleVar();
        };

        //-------------------------------------------------------------------------

        ImGui::SetNextWindowClass( pWindowClass );
        if ( ImGui::Begin( s_infoWindowName ) )
        {
            DrawWindowContents();
        }
        ImGui::End();
    }
}
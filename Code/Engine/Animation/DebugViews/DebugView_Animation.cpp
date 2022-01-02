#include "DebugView_Animation.h"
#include "Engine/Animation/Systems/WorldSystem_Animation.h"
#include "Engine/Animation/Components/Component_AnimationGraph.h"
#include "Engine/Core/Entity/EntityWorld.h"
#include "Engine/Core/Entity/EntityWorldUpdateContext.h"
#include "System/Render/Imgui/ImguiX.h"
#include "System/Core/Math/MathStringHelpers.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Animation
{
    void AnimationDebugView::DrawGraphControlParameters( AnimationGraphComponent* pGraphComponent )
    {
        if ( ImGui::BeginTable( "OverlayActionsTable", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
        {
            ImGui::TableSetupColumn( "Parameter", ImGuiTableColumnFlags_WidthStretch, 0.5f );
            ImGui::TableSetupColumn( "Value", ImGuiTableColumnFlags_WidthStretch );
            ImGui::TableHeadersRow();

            int32 const numControlParameters = pGraphComponent->m_pGraphInstance->GetNumControlParameters();
            for ( GraphNodeIndex i = 0; i < numControlParameters; i++ )
            {
                StringID const nodeID = pGraphComponent->m_pGraphInstance->GetControlParameterID( i );
                GraphValueType const valueType = pGraphComponent->m_pGraphInstance->GetControlParameterType( i );

                InlineString stringValue;
                switch ( valueType )
                {
                    case GraphValueType::Bool:
                    {
                        if ( pGraphComponent->m_pGraphInstance->GetControlParameterValue<bool>( pGraphComponent->m_graphContext, i ) )
                        {
                            stringValue = "True";
                        }
                        else
                        {
                            stringValue = "False";
                        }
                    }
                    break;

                    case GraphValueType::ID:
                    {
                        StringID const value = pGraphComponent->m_pGraphInstance->GetControlParameterValue<StringID>( pGraphComponent->m_graphContext, i );
                        if ( value.IsValid() )
                        {
                            stringValue.sprintf( "%s", value.c_str() );
                        }
                        else
                        {
                            stringValue = "Unset";
                        }
                    }
                    break;

                    case GraphValueType::Int:
                    {
                        stringValue.sprintf( "%d", pGraphComponent->m_pGraphInstance->GetControlParameterValue<int32>( pGraphComponent->m_graphContext, i ) );
                    }
                    break;

                    case GraphValueType::Float:
                    {
                        stringValue.sprintf( "%.3f", pGraphComponent->m_pGraphInstance->GetControlParameterValue<float>( pGraphComponent->m_graphContext, i ) );
                    }
                    break;

                    case GraphValueType::Vector:
                    {
                        Vector const value = pGraphComponent->m_pGraphInstance->GetControlParameterValue<Vector>( pGraphComponent->m_graphContext, i );
                        stringValue = Math::ToString( value );
                    }
                    break;

                    case GraphValueType::Target:
                    {
                        Target const value = pGraphComponent->m_pGraphInstance->GetControlParameterValue<Target>( pGraphComponent->m_graphContext, i );
                        if ( !value.IsTargetSet() )
                        {
                            stringValue = "Unset";
                        }
                        else if ( value.IsBoneTarget() )
                        {
                            stringValue.sprintf( "Bone: %s", value.GetBoneID().c_str() );
                        }
                        else
                        {
                            stringValue = "TODO";
                        }
                    }
                    break;

                    default:
                    {
                        KRG_UNREACHABLE_CODE();
                    }
                    break;
                }

                //-------------------------------------------------------------------------

                ImGui::TableNextRow();

                ImGui::TableSetColumnIndex( 0 );
                ImGui::TextColored( ImGuiX::ConvertColor( GetColorForValueType( valueType ) ), "%s", nodeID.c_str() );

                ImGui::TableSetColumnIndex( 1 );
                ImGui::Text( stringValue.c_str() );
            }

            ImGui::EndTable();
        }
    }

    void AnimationDebugView::DrawTaskTreeRow( AnimationGraphComponent* pGraphComponent, TaskSystem* pTaskSystem, TaskIndex currentTaskIdx )
    {
        static const char* const stageLabels[] = { "ERROR!", "Pre-Physics", "Post-Physics" };

        auto pTask = pTaskSystem->m_tasks[currentTaskIdx];
        InlineString const rowLabel( InlineString::CtorSprintf(), "%s - %s", stageLabels[(int32) pTask->GetActualUpdateStage()], pTask->GetDebugText().c_str() );

        //String const& nodePath = pGraphComponent->m_pGraphVariation->GetDefinition()->GetNodePath( sampledEvent.GetSourceNodeIndex() );

        ImGui::SetNextItemOpen( true );
        ImGui::PushStyleColor( ImGuiCol_Text, pTask->GetDebugColor().ToFloat4() );
        if ( ImGui::TreeNode( rowLabel.c_str() ) )
        {
            for ( auto taskIdx : pTask->GetDependencyIndices() )
            {
                DrawTaskTreeRow( pGraphComponent, pTaskSystem, taskIdx );
            }

            ImGui::TreePop();
        }
        ImGui::PopStyleColor();
    }

    void AnimationDebugView::DrawRootMotionRow( AnimationGraphComponent* pGraphComponent, RootMotionActionRecorder* pRootMotionRecorder, int16 currentActionIdx )
    {
        static char const* const actionTypes[] = { "Error", "Sample", "Modify", "Blend" };

        RootMotionActionRecorder::RecordedAction const* pAction = nullptr;

        InlineString rowLabel;
        if ( currentActionIdx != InvalidIndex )
        {
            pAction = &pRootMotionRecorder->GetRecordedActions()[currentActionIdx];
            String const& nodePath = pGraphComponent->m_pGraphVariation->GetDefinition()->GetNodePath( pAction->m_nodeIdx );
            rowLabel.sprintf( "%s - %s", actionTypes[(int32) pAction->m_actionType], nodePath.c_str() );
        }
        else
        {
            rowLabel = "No root motion action registered";
        }

        //-------------------------------------------------------------------------

        ImGui::SetNextItemOpen( true );
        ImGuiX::PushFont( ImGuiX::Font::MediumBold );
        if ( ImGui::TreeNode( rowLabel.c_str() ) )
        {
            ImGui::PopFont();

            ImGui::Indent( 20.0f );
            ImGuiX::PushFont( ImGuiX::Font::Small );
            if ( pAction != nullptr )
            {
                ImGui::Text( "R: %s, T: %s", Math::ToString( pAction->m_rootMotionDelta.GetRotation() ).c_str(), Math::ToString( pAction->m_rootMotionDelta.GetTranslation() ).c_str() );
            }
            else
            {
                ImGui::Text( "No Root Motion" );
            }

            ImGui::PopFont();
            ImGui::Unindent( 20.0f );

            //-------------------------------------------------------------------------

            if ( pAction != nullptr )
            {
                for ( auto taskIdx : pAction->m_dependencies )
                {
                    DrawRootMotionRow( pGraphComponent, pRootMotionRecorder, taskIdx );
                }
            }

            ImGui::TreePop();
        }
    }

    void AnimationDebugView::DrawGraphActiveTasksDebugView( AnimationGraphComponent* pGraphComponent )
    {
        if ( !pGraphComponent->IsInitialized() )
        {
            ImGui::Text( "Uninitialized Graph Component" );
            return;
        }

        auto pTaskSystem = pGraphComponent->m_pTaskSystem;
        if ( !pTaskSystem->HasTasks() )
        {
            ImGui::Text( "No Active Tasks" );
            return;
        }

        // Tasks
        //-------------------------------------------------------------------------

        ImGuiX::TextSeparator( "Animation Tasks" );

        TaskIndex const finalTask = (TaskIndex) pTaskSystem->m_tasks.size() - 1;
        DrawTaskTreeRow( pGraphComponent, pTaskSystem, finalTask );

        // Root Motion
        //-------------------------------------------------------------------------
        
        ImGui::NewLine();
        ImGuiX::TextSeparator( "Root Motion" );

        RootMotionActionRecorder* pRootMotionRecorder = pGraphComponent->m_graphContext.GetRootMotionActionRecorder();
        if ( pRootMotionRecorder->HasRecordedActions() )
        {
            DrawRootMotionRow( pGraphComponent, pRootMotionRecorder, pRootMotionRecorder->GetLastActionIndex() );
        }
        else
        {
            ImGui::Text( "No root motion actions this frame!" );
        }
    }

    void AnimationDebugView::DrawGraphSampledEventsView( AnimationGraphComponent* pGraphComponent )
    {
        if ( !pGraphComponent->IsInitialized() )
        {
            ImGui::Text( "Uninitialized Graph Component" );
            return;
        }

        //-------------------------------------------------------------------------

        if ( ImGui::BeginTable( "OverlayActionsTable", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
        {
            ImGui::TableSetupColumn( "##Type", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 14 );
            ImGui::TableSetupColumn( "##Active", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 14 );
            ImGui::TableSetupColumn( "Event", ImGuiTableColumnFlags_WidthStretch, 0.5f );
            ImGui::TableSetupColumn( "Source", ImGuiTableColumnFlags_WidthStretch, 0.5f );
            ImGui::TableSetupColumn( "%", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 40 );
            ImGui::TableSetupColumn( "Weight", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 40 );
            ImGui::TableHeadersRow();

            //-------------------------------------------------------------------------

            auto const& sampledEvents = pGraphComponent->m_graphContext.m_sampledEvents;
            for ( auto const& sampledEvent : sampledEvents )
            {
                ImGui::TableNextRow();

                ImGui::TableNextColumn();
                if ( sampledEvent.IsStateEvent() )
                {
                    ImGui::TextColored( Colors::Cyan.ToFloat4(), KRG_ICON_ARROWS_H );
                }
                else
                {
                    ImGui::TextColored( Colors::GreenYellow.ToFloat4(), KRG_ICON_FILM );
                }

                //-------------------------------------------------------------------------

                ImGui::TableNextColumn();
                if ( sampledEvent.IsFromActiveBranch() )
                {
                    ImGui::TextColored( Colors::Green.ToFloat4(), KRG_ICON_CHECK );
                }
                else
                {
                    ImGui::TextColored( Colors::LightGray.ToFloat4(), KRG_ICON_MINUS );
                }

                //-------------------------------------------------------------------------

                ImGui::TableNextColumn();
                if ( sampledEvent.IsStateEvent() )
                {
                    ImGui::Text( sampledEvent.GetStateEventID().c_str() );
                }
                else
                {
                    InlineString const debugString = sampledEvent.GetEvent()->GetDebugText();
                    ImGui::Text( debugString.c_str() );
                }

                //-------------------------------------------------------------------------

                ImGui::TableNextColumn();
                String const& nodePath = pGraphComponent->m_pGraphVariation->GetDefinition()->GetNodePath( sampledEvent.GetSourceNodeIndex() );
                ImGui::Text( nodePath.c_str() );

                ImGui::TableNextColumn();
                ImGui::Text( "%.1f", sampledEvent.GetPercentageThrough().ToFloat() * 100 );

                ImGui::TableNextColumn();
                ImGui::Text( "%.2f", sampledEvent.GetWeight() );
            }

            ImGui::EndTable();
        }
    }

    //-------------------------------------------------------------------------

    AnimationDebugView::AnimationDebugView()
    {
        m_menus.emplace_back( DebugMenu( "Animation", [this] ( EntityWorldUpdateContext const& context ) { DrawMenu( context ); } ) );
    }

    void AnimationDebugView::Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld )
    {
        m_pWorld = pWorld;
        m_pAnimationWorldSystem = pWorld->GetWorldSystem<AnimationWorldSystem>();
    }

    void AnimationDebugView::Shutdown()
    {
        m_pAnimationWorldSystem = nullptr;
        m_pWorld = nullptr;
    }

    //-------------------------------------------------------------------------

    AnimationDebugView::ComponentDebugSettings* AnimationDebugView::GetDebugSettings( ComponentID ID )
    {
        for ( auto& debuggedComponent : m_componentDebugSettings )
        {
            if ( debuggedComponent.m_ID == ID )
            {
                return &debuggedComponent;
            }
        }

        return &m_componentDebugSettings.emplace_back( ID );
    }

    void AnimationDebugView::DestroyDebugSettings( ComponentID ID )
    {
        for ( int32 i = 0; i < (int32) m_componentDebugSettings.size(); i++ )
        {
            if ( m_componentDebugSettings[i].m_ID == ID )
            {
                m_componentDebugSettings.erase_unsorted( m_componentDebugSettings.begin() + i );
                break;
            }
        }

        KRG_UNREACHABLE_CODE();
    }

    //-------------------------------------------------------------------------

    void AnimationDebugView::DrawMenu( EntityWorldUpdateContext const& context )
    {
        InlineString componentName;
        for ( AnimationGraphComponent* pGraphComponent : m_pAnimationWorldSystem->m_graphComponents )
        {
            EntityID const entityID = pGraphComponent->GetEntityID();
            auto pEntity = m_pWorld->FindEntity( entityID );
            KRG_ASSERT( pEntity != nullptr );

            auto pDebugSettings = GetDebugSettings( pGraphComponent->GetID() );

            //-------------------------------------------------------------------------

            componentName.sprintf( "%s (%u)", pGraphComponent->GetName().c_str(), pGraphComponent->GetID().m_ID );
            if ( ImGui::BeginMenu( componentName.c_str() ) )
            {
                ImGuiX::TextSeparator( "Graph" );

                if ( ImGui::MenuItem( "Show Control Parameters" ) )
                {
                    pDebugSettings->m_drawControlParameters = true;
                }

                if ( ImGui::MenuItem( "Show Sampled Events" ) )
                {
                    pDebugSettings->m_drawSampledEvents = true;
                }

                ImGuiX::TextSeparator( "Tasks" );

                if ( ImGui::MenuItem( "Show Active Tasks" ) )
                {
                    pDebugSettings->m_drawActiveTasks = true;
                }

                if ( ImGui::BeginMenu( "Visualization" ) )
                {
                    TaskSystem::DebugMode const debugMode = pGraphComponent->GetTaskSystemDebugMode();

                    bool const isVisualizationOff = debugMode == TaskSystem::DebugMode::Off;
                    if ( ImGui::RadioButton( "No Visualization", isVisualizationOff ) )
                    {
                        pGraphComponent->SetTaskSystemDebugMode( TaskSystem::DebugMode::Off );
                    }

                    bool const isFinalPoseEnabled = debugMode == TaskSystem::DebugMode::FinalPose;
                    if ( ImGui::RadioButton( "FinalPose", isFinalPoseEnabled ) )
                    {
                        pGraphComponent->SetTaskSystemDebugMode( TaskSystem::DebugMode::FinalPose );
                    }

                    bool const isPoseTreeEnabled = debugMode == TaskSystem::DebugMode::PoseTree;
                    if ( ImGui::RadioButton( "Pose Tree", isPoseTreeEnabled ) )
                    {
                        pGraphComponent->SetTaskSystemDebugMode( TaskSystem::DebugMode::PoseTree );
                    }

                    bool const isDetailedPoseTreeEnabled = debugMode == TaskSystem::DebugMode::DetailedPoseTree;
                    if ( ImGui::RadioButton( "Detailed Pose Tree", isDetailedPoseTreeEnabled ) )
                    {
                        pGraphComponent->SetTaskSystemDebugMode( TaskSystem::DebugMode::DetailedPoseTree );
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
        }
    }

    void AnimationDebugView::DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        InlineString title;

        for ( int32 i = (int32) m_componentDebugSettings.size() - 1; i >= 0; i-- )
        {
            bool stopDebug = false;
            auto ppFoundComponent = m_pAnimationWorldSystem->m_graphComponents.FindItem( m_componentDebugSettings[i].m_ID );
            if ( ppFoundComponent != nullptr )
            {
                auto pGraphComponent = *ppFoundComponent;

                //-------------------------------------------------------------------------

                EntityID const entityID = pGraphComponent->GetEntityID();
                auto pEntity = m_pWorld->FindEntity( entityID );
                KRG_ASSERT( pEntity != nullptr );

                //-------------------------------------------------------------------------

                if ( m_componentDebugSettings[i].m_drawControlParameters )
                {
                    bool keepOpen = true;
                    title.sprintf( "Control Parameters: %s (%s)", pGraphComponent->GetName().c_str(), pEntity->GetName().c_str() );
                    ImGui::SetNextWindowSize( ImVec2( 600, 700 ), ImGuiCond_FirstUseEver );
                    if ( ImGui::Begin( title.c_str(), &keepOpen, ImGuiWindowFlags_NoSavedSettings) )
                    {
                        DrawGraphControlParameters( pGraphComponent );
                    }
                    ImGui::End();

                    if ( !keepOpen )
                    {
                        m_componentDebugSettings[i].m_drawControlParameters = false;
                    }
                }

                //-------------------------------------------------------------------------

                if ( m_componentDebugSettings[i].m_drawActiveTasks )
                {
                    bool keepOpen = true;
                    title.sprintf( "Active Tasks: %s (%s)", pGraphComponent->GetName().c_str(), pEntity->GetName().c_str() );
                    ImGui::SetNextWindowSize( ImVec2( 600, 700 ), ImGuiCond_FirstUseEver );
                    if ( ImGui::Begin( title.c_str(), &keepOpen, ImGuiWindowFlags_NoSavedSettings ) )
                    {
                        DrawGraphActiveTasksDebugView( pGraphComponent );
                    }
                    ImGui::End();

                    if ( !keepOpen )
                    {
                        m_componentDebugSettings[i].m_drawActiveTasks = false;
                    }
                }

                //-------------------------------------------------------------------------

                if ( m_componentDebugSettings[i].m_drawSampledEvents )
                {
                    bool keepOpen = true;
                    title.sprintf( "Sampled Events: %s (%s)", pGraphComponent->GetName().c_str(), pEntity->GetName().c_str() );
                    ImGui::SetNextWindowSize( ImVec2( 600, 700 ), ImGuiCond_FirstUseEver );
                    if ( ImGui::Begin( title.c_str(), &keepOpen, ImGuiWindowFlags_NoSavedSettings ) )
                    {
                        DrawGraphSampledEventsView( pGraphComponent );
                    }
                    ImGui::End();

                    if ( !keepOpen )
                    {
                        m_componentDebugSettings[i].m_drawSampledEvents = false;
                    }
                }
            }
            else // Erase invalid component ID
            {
                stopDebug = true;
            }

            //-------------------------------------------------------------------------

            if ( stopDebug )
            {
                m_componentDebugSettings.erase_unsorted( m_componentDebugSettings.begin() + i );
            }
        }
    }

    void AnimationDebugView::DrawOverlayElements( EntityWorldUpdateContext const& context )
    {
        auto drawingCtx = context.GetDrawingContext();

        for ( auto const& debugSettings : m_componentDebugSettings )
        {
            auto ppFoundComponent = m_pAnimationWorldSystem->m_graphComponents.FindItem( debugSettings.m_ID );
            if ( ppFoundComponent != nullptr )
            {
                auto pGraphComponent = *ppFoundComponent;
                pGraphComponent->DrawDebug( drawingCtx );
            }
        }
    }
}
#endif
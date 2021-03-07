#include "PhysicsDebugViewController.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/Components/PhysicsComponent.h"
#include "Engine/Physics/PhysicsWorldSystem.h"
#include "Engine/Physics/PhysX.h"
#include "System/Imgui/ImguiSystem.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    PhysicsDebugViewController::PhysicsDebugViewController()
    {
        auto drawDebugMenu = [this] ( UpdateContext const& context )
        {
            DrawPhysicsMenu( context );
        };

        m_menuCallbacks.emplace_back( Debug::DebugMenuCallback( "Debug Options", "Physics", drawDebugMenu ) );
    }

    void PhysicsDebugViewController::Initialize( PhysicsSystem* pPhysicsSystem, PhysicsWorldSystem* pPhysicsWorldSystem )
    {
        KRG_ASSERT( pPhysicsSystem != nullptr && pPhysicsWorldSystem != nullptr );
        m_pPhysicsSystem = pPhysicsSystem;
        m_pPhysicsWorldSystem = pPhysicsWorldSystem;
    }

    void PhysicsDebugViewController::Shutdown()
    {
        m_pPhysicsSystem = nullptr;
    }

    //-------------------------------------------------------------------------

    void PhysicsDebugViewController::DrawPhysicsMenu( UpdateContext const& context )
    {
        //-------------------------------------------------------------------------
        // Scene
        //-------------------------------------------------------------------------

        uint32 debugFlags = m_pPhysicsSystem->GetDebugFlags();
        float drawDistance = m_pPhysicsSystem->GetDebugDrawDistance();

        bool stateUpdated = false;

        stateUpdated |= ImGui::CheckboxFlags( "Enable Debug Visualization", &debugFlags, 1 << PxVisualizationParameter::eSCALE );
        stateUpdated |= ImGui::SliderFloat( "Visualization Distance", &drawDistance, 1.0f, 100.0f );

        DrawPVDMenu( context );

        ImGui::Separator();

        stateUpdated |= ImGui::CheckboxFlags( "Collision AABBs", &debugFlags, 1 << PxVisualizationParameter::eCOLLISION_AABBS );
        stateUpdated |= ImGui::CheckboxFlags( "Collision Shapes", &debugFlags, 1 << PxVisualizationParameter::eCOLLISION_SHAPES );
        stateUpdated |= ImGui::CheckboxFlags( "Collision Axes", &debugFlags, 1 << PxVisualizationParameter::eCOLLISION_AXES );
        stateUpdated |= ImGui::CheckboxFlags( "Collision Face Normals", &debugFlags, 1 << PxVisualizationParameter::eCOLLISION_FNORMALS );
        stateUpdated |= ImGui::CheckboxFlags( "Collision Edges", &debugFlags, 1 << PxVisualizationParameter::eCOLLISION_EDGES );

        ImGui::Separator();

        stateUpdated |= ImGui::CheckboxFlags( "Contact Points", &debugFlags, 1 << PxVisualizationParameter::eCONTACT_POINT );
        stateUpdated |= ImGui::CheckboxFlags( "Contact Normals", &debugFlags, 1 << PxVisualizationParameter::eCONTACT_NORMAL );
        stateUpdated |= ImGui::CheckboxFlags( "Contact Error", &debugFlags, 1 << PxVisualizationParameter::eCONTACT_ERROR );
        stateUpdated |= ImGui::CheckboxFlags( "Contact Force", &debugFlags, 1 << PxVisualizationParameter::eCONTACT_FORCE );

        ImGui::Separator();

        stateUpdated |= ImGui::CheckboxFlags( "Actor Axes", &debugFlags, 1 << PxVisualizationParameter::eACTOR_AXES );
        stateUpdated |= ImGui::CheckboxFlags( "Body Axes", &debugFlags, 1 << PxVisualizationParameter::eBODY_AXES );
        stateUpdated |= ImGui::CheckboxFlags( "Linear Velocity", &debugFlags, 1 << PxVisualizationParameter::eBODY_LIN_VELOCITY );
        stateUpdated |= ImGui::CheckboxFlags( "Angular Velocity", &debugFlags, 1 << PxVisualizationParameter::eBODY_ANG_VELOCITY );
        stateUpdated |= ImGui::CheckboxFlags( "Mass Axes", &debugFlags, 1 << PxVisualizationParameter::eBODY_MASS_AXES );
        stateUpdated |= ImGui::CheckboxFlags( "Joint Limits", &debugFlags, 1 << PxVisualizationParameter::eJOINT_LIMITS );
        stateUpdated |= ImGui::CheckboxFlags( "Joint Local Frames", &debugFlags, 1 << PxVisualizationParameter::eJOINT_LOCAL_FRAMES );

        if ( stateUpdated )
        {
            m_pPhysicsSystem->SetDebugFlags( debugFlags );
            m_pPhysicsSystem->SetDebugDrawDistance( drawDistance );
        }

        ImGui::Separator();

        ImGui::Checkbox( "Draw Dynamic Actor Bounds", &m_pPhysicsWorldSystem->m_drawDynamicActorBounds );
        ImGui::Checkbox( "Draw Kinematic Actor Bounds", &m_pPhysicsWorldSystem->m_drawKinematicActorBounds );

        //-------------------------------------------------------------------------
        // Component Debug
        //-------------------------------------------------------------------------

        ImGui::Separator();

        if ( ImGui::Button( "Show Physics Component List", ImVec2( -1, 0 ) ) )
        {
            m_isComponentWindowOpen = true;
        }

        if ( ImGui::Button( "Show Material Database", ImVec2( -1, 0 ) ) )
        {
            m_isMaterialDatabaseWindowOpen = true;
        }
    }

    void PhysicsDebugViewController::DrawWindows( UpdateContext const& context )
    {
        if ( m_isComponentWindowOpen )
        {
            DrawComponentsWindow( context );
        }

        if ( m_isMaterialDatabaseWindowOpen )
        {
            DrawMaterialDatabaseWindow( context );
        }
    }

    //-------------------------------------------------------------------------

    void PhysicsDebugViewController::DrawPVDMenu( UpdateContext const& context )
    {
        if ( ImGui::BeginMenu( "PhysX Visual Debugger" ) )
        {
            if ( !m_pPhysicsSystem->IsConnectedToPVD() )
            {
                if ( ImGui::Button( "Connect to PVD", ImVec2( -1, 0 ) ) )
                {
                    m_pPhysicsSystem->ConnectToPVD();
                }
            }
            else
            {
                if ( ImGui::Button( "Disconnect From PVD", ImVec2( -1, 0 ) ) )
                {
                    m_pPhysicsSystem->DisconnectFromPVD();
                }
            }

            ImGui::SliderFloat( "Recording Time (s)", &m_recordingTimeSeconds, 0.25f, 10.0f );
            if ( ImGui::Button( "PVD Timed Recording", ImVec2( -1, 0 ) ) )
            {
                m_pPhysicsSystem->ConnectToPVD( m_recordingTimeSeconds );
            }

            ImGui::EndMenu();
        }
    }

    void PhysicsDebugViewController::DrawComponentsWindow( UpdateContext const& context )
    {
        ImGui::SetNextWindowBgAlpha( 0.5f );
        if ( ImGui::Begin( "Physics Components", &m_isComponentWindowOpen ) )
        {
            for ( auto const& entityRecord : m_pPhysicsWorldSystem->m_registeredEntities )
            {
                for ( PhysicsComponent* pPhysicsComponent : entityRecord.m_components )
                {
                    ImGui::Text( "%s", pPhysicsComponent->GetName().c_str() );
                    if ( ImGui::IsItemHovered() )
                    {
                        auto drawingContext = context.GetDrawingContext();
                        drawingContext.DrawBox( pPhysicsComponent->GetWorldBounds(), Colors::Cyan.GetAlphaVersion( 0.25f ) );
                        drawingContext.DrawWireBox( pPhysicsComponent->GetWorldBounds(), Colors::Cyan );
                    }
                }
            }
        }
        ImGui::End();
    }

    void PhysicsDebugViewController::DrawMaterialDatabaseWindow( UpdateContext const& context )
    {
        ImGui::SetNextWindowBgAlpha( 0.5f );
        if ( ImGui::Begin( "Physics Material Database", &m_isMaterialDatabaseWindowOpen ) )
        {
            if ( ImGui::BeginTable( "Resource Request History Table", 6, ImGuiTableFlags_Borders | ImGuiTableFlags_Resizable ) )
            {
                ImGui::TableSetupColumn( "Material ID", ImGuiTableColumnFlags_WidthStretch );
                ImGui::TableSetupColumn( "Static Friction", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 96 );
                ImGui::TableSetupColumn( "Dynamic Friction", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 96 );
                ImGui::TableSetupColumn( "Restitution", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 96 );
                ImGui::TableSetupColumn( "Friction Combine", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 128 );
                ImGui::TableSetupColumn( "Restitution Combine", ImGuiTableColumnFlags_WidthFixed | ImGuiTableColumnFlags_NoResize, 128 );

                //-------------------------------------------------------------------------

                ImGui::TableHeadersRow();

                //-------------------------------------------------------------------------

                for ( auto const& materialPair : m_pPhysicsSystem->m_materials )
                {
                    PxMaterial const* pMaterial = materialPair.second.m_pMaterial;

                    ImGui::TableNextRow();

                    //-------------------------------------------------------------------------

                    static char const* const combineText[] = { "Average", "Min", "Multiply", "Max" };
                    auto GetCombineText = [] ( PxCombineMode::Enum combineMode )
                    {
                        return combineText[(uint8) combineMode];
                    };

                    //-------------------------------------------------------------------------

                    ImGui::TableSetColumnIndex( 0 );
                    ImGui::Text( materialPair.second.m_ID.c_str() );

                    ImGui::TableSetColumnIndex( 1 );
                    ImGui::Text( "%.3f", pMaterial->getStaticFriction() );

                    ImGui::TableSetColumnIndex( 2 );
                    ImGui::Text( "%.3f", pMaterial->getDynamicFriction() );

                    ImGui::TableSetColumnIndex( 3 );
                    ImGui::Text( "%.3f", pMaterial->getRestitution() );

                    ImGui::TableSetColumnIndex( 4 );
                    ImGui::Text( GetCombineText( pMaterial->getFrictionCombineMode() ) );

                    ImGui::TableSetColumnIndex( 5 );
                    ImGui::Text( GetCombineText( pMaterial->getRestitutionCombineMode() ) );
                }

                //-------------------------------------------------------------------------

                ImGui::EndTable();
            }
        }
        ImGui::End();
    }
}
#endif
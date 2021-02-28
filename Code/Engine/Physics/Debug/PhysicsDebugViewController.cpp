#include "PhysicsDebugViewController.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysicsScene.h"
#include "Engine/Physics/PhysX.h"
#include "System/Imgui/ImguiSystem.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        PhysicsDebugViewController::PhysicsDebugViewController()
        {
            auto drawDebugMenu = [this] ( UpdateContext const& context )
            {
                DrawDebugMenu( context );
            };

            m_menuCallbacks.emplace_back( Debug::DebugMenuCallback( "Debug Options", "Physics", drawDebugMenu ) );
        }

        void PhysicsDebugViewController::Initialize( PhysicsSystem* pPhysicsSystem )
        {
            KRG_ASSERT( pPhysicsSystem != nullptr );
            m_pPhysicsSystem = pPhysicsSystem;
        }

        void PhysicsDebugViewController::Shutdown()
        {
            m_pPhysicsSystem = nullptr;
        }

        //-------------------------------------------------------------------------

        void PhysicsDebugViewController::DrawDebugMenu( UpdateContext const& context )
        {
            DrawPhysXMenu( context );

            //-------------------------------------------------------------------------

            ImGui::Separator();

            //-------------------------------------------------------------------------
        }

        void PhysicsDebugViewController::DrawWindows( UpdateContext const& context )
        {}

        void PhysicsDebugViewController::DrawPhysXMenu( UpdateContext const& context )
        {
            if ( ImGui::BeginMenu( "PhysX" ) )
            {
                //-------------------------------------------------------------------------
                // Scenes
                //-------------------------------------------------------------------------

                auto const& scenes = m_pPhysicsSystem->GetScenes();
                for ( auto pScene : scenes )
                {
                    DrawSceneMenu( pScene );
                }

                //-------------------------------------------------------------------------
                // PVD
                //-------------------------------------------------------------------------

                ImGui::Separator();

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

                //-------------------------------------------------------------------------

                ImGui::EndMenu();
            }
        }

        void PhysicsDebugViewController::DrawSceneMenu( PhysicsScene* pScene )
        {
            KRG_ASSERT( pScene != nullptr );

            if ( ImGui::BeginMenu( pScene->GetDebugName().c_str() ) )
            {
                // Debug State
                //-------------------------------------------------------------------------

                U32 debugFlags = pScene->GetDebugFlags();
                float drawDistance = pScene->GetDebugDrawDistance();

                bool stateUpdated = false;

                stateUpdated |= ImGui::CheckboxFlags( "Enable Debug Visualization", &debugFlags, 1 << PxVisualizationParameter::eSCALE );
                stateUpdated |= ImGui::SliderFloat( "Visualization Distance", &drawDistance, 1.0f, 100.0f );

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

                ImGui::Separator();

                if ( stateUpdated )
                {
                    pScene->SetDebugFlags( debugFlags );
                    pScene->SetDebugDrawDistance( drawDistance );
                }

                ImGui::EndMenu();
            }
        }
    }
}
#endif
#include "PhysicsDebugViewController.h"
#include "Engine/Physics/PhysicsSystem.h"
#include "Engine/Physics/PhysX.h"
#include "System/Imgui/ImguiCore.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION

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

        void PhysicsDebugViewController::Initialize( PhysicsSystem* pPhysicsWorld )
        {
            KRG_ASSERT( pPhysicsWorld != nullptr );
            m_pPhysicsWorld = pPhysicsWorld;
        }

        void PhysicsDebugViewController::Shutdown( PhysicsSystem* pPhysicsWorld )
        {
            m_pPhysicsWorld = nullptr;
        }

        //-------------------------------------------------------------------------

        void PhysicsDebugViewController::DrawDebugMenu( UpdateContext const& context )
        {
            // Debug State
            //-------------------------------------------------------------------------

            U32 debugFlags = m_pPhysicsWorld->GetDebugFlags();
            float drawDistance = m_pPhysicsWorld->GetDebugDrawDistance();

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
                m_pPhysicsWorld->SetDebugFlags( debugFlags );
                m_pPhysicsWorld->SetDebugDrawDistance( drawDistance );
            }

            // PVD
            //-------------------------------------------------------------------------

            if ( !m_pPhysicsWorld->IsConnectedToPVD() )
            {
                if ( ImGui::Button( "Connect to PVD", ImVec2( -1, 0 ) ) )
                {
                    m_pPhysicsWorld->ConnectToPVD();
                }
            }
            else
            {
                if ( ImGui::Button( "Disconnect From PVD", ImVec2( -1, 0 ) ) )
                {
                    m_pPhysicsWorld->DisconnectFromPVD();
                }
            }

            ImGui::SliderFloat( "Recording Time (s)", &m_recordingTimeSeconds, 0.25f, 10.0f );
            if ( ImGui::Button( "PVD Timed Recording", ImVec2( -1, 0 ) ) )
            {
                m_pPhysicsWorld->ConnectToPVD( m_recordingTimeSeconds );
            }
        }

        void PhysicsDebugViewController::DrawWindows( UpdateContext const& context )
        {}
    }
}
#endif
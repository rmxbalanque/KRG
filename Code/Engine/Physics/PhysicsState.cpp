#include "PhysicsState.h"
#include "imgui.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    #if KRG_DEVELOPMENT_TOOLS
    void PhysicsState::DrawDebugUI()
    {
        ImGui::Text( "No Debug" );
    }
    #endif

    //-------------------------------------------------------------------------

    PhysicsStateController::~PhysicsStateController()
    {
        KRG_ASSERT( !m_registeredStates.empty() );
        m_pActiveState->Deactivate();

        for ( auto pRegisteredState : m_registeredStates )
        {
            KRG::Delete( pRegisteredState );
        }
        m_registeredStates.clear();
        m_pActiveState = nullptr;
    }
}
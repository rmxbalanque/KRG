#include "NavPower.h"
#include "bfxPlanner.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    NavPower* NavPower::Instance = nullptr;

    //-------------------------------------------------------------------------

    void NavPower::Initialize()
    {
        KRG_ASSERT( Instance == nullptr );
        Instance = KRG::New<NavPower>();

        //-------------------------------------------------------------------------

        Instance->m_pNavpowerInstance = bfx::SystemCreate( bfx::SystemParams( 2.0f, bfx::Z_UP ), &Instance->m_allocator );
        bfx::RegisterPlannerSystem();
        bfx::SystemStart();
    }

    void NavPower::Shutdown()
    {
        KRG_ASSERT( Instance != nullptr );

        //-------------------------------------------------------------------------
        
        bfx::SystemStop();
        bfx::SystemDestroy();

        //-------------------------------------------------------------------------

        KRG::Delete( Instance );
    }

    bfx::Instance* NavPower::GetNavpowerInstance()
    {
        KRG_ASSERT( Instance != nullptr );
        return Instance->m_pNavpowerInstance;
    }
}
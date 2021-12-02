#include "NavPower.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    // HACK
    //-------------------------------------------------------------------------

    bfx::Mover* CreateMover( Vector const& pos, Quaternion const& rot, bfx::MoverTune* pTune )
    {
        return bfx::CreateMover( Navmesh::ToBfx( pos ), Navmesh::ToBfx( rot ), pTune );
    }

    void DestroyMover( bfx::Mover* pMover )
    {
        bfx::DestroyMover( pMover );
    }
}
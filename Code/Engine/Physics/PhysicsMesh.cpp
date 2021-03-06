#include "PhysicsMesh.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    bool PhysicsMesh::IsValid() const
    {
        return m_pMesh != nullptr;
    }
}
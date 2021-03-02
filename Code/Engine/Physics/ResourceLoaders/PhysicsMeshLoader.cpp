#include "PhysicsMeshLoader.h"
#include "Engine/Physics/PhysX.h"
#include "Engine/Physics/PhysicsMesh.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------
namespace KRG::Physics
{
    class PhysXSerializedInputData : public physx::PxInputStream
    {
    public:

        PhysXSerializedInputData( TVector<Byte> const& buffer ) : m_buffer( buffer ) {}

    private:

        virtual PxU32 read( void* dest, PxU32 count ) override final
        {
            KRG_ASSERT( dest != nullptr );
            memcpy( dest, &m_buffer[m_readByteIdx], count );
            m_readByteIdx += count;
            return count;
        }

    private:

        TVector<Byte> const&    m_buffer;
        size_t                  m_readByteIdx = 0;
    };

    //-------------------------------------------------------------------------

    PhysicsMeshLoader::PhysicsMeshLoader()
    {
        m_loadableTypes.push_back( PhysicsMesh::GetStaticResourceTypeID() );
    }

    void PhysicsMeshLoader::SetPhysics( PxPhysics* pPhysics )
    {
        KRG_ASSERT( pPhysics != nullptr && m_pPhysics == nullptr );
        m_pPhysics = pPhysics;
    }

    bool PhysicsMeshLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryMemoryArchive& archive ) const
    {
        KRG_ASSERT( archive.IsValid() );

        // Create mesh resource
        PhysicsMesh* pPhysicsGeometry = KRG::New<PhysicsMesh>();
        archive >> *pPhysicsGeometry;

        // Deserialize cooked mesh data
        TVector<Byte> cookedTriangleMeshData;
        archive >> cookedTriangleMeshData;

        PhysXSerializedInputData cooked( cookedTriangleMeshData );
        pPhysicsGeometry->m_pTriangleMesh = m_pPhysics->createTriangleMesh( cooked );

        pResourceRecord->SetResourceData( pPhysicsGeometry );
        return true;
    }

    void PhysicsMeshLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
    {
        PhysicsMesh* pPhysicsGeometry = pResourceRecord->GetResourceData<PhysicsMesh>();
        if ( pPhysicsGeometry != nullptr )
        {
            if ( pPhysicsGeometry->m_pTriangleMesh != nullptr )
            {
                pPhysicsGeometry->m_pTriangleMesh->release();
                pPhysicsGeometry->m_pTriangleMesh = nullptr;
            }
        }

        ResourceLoader::UnloadInternal( resID, pResourceRecord );
    }
}
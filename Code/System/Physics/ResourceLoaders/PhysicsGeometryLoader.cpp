#include "PhysicsGeometryLoader.h"
#include "System/Physics/PhysicsWorld.h"
#include "System/Physics/PhysicsGeometry.h"
#include "System/Physics/PhysX.h"
#include "System/Core/Serialization/BinaryArchive.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------
namespace KRG
{
    namespace Physics
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

        PhysicsGeometryLoader::PhysicsGeometryLoader()
        {
            m_loadableTypes.push_back( PhysicsGeometry::GetStaticResourceTypeID() );
        }

        bool PhysicsGeometryLoader::LoadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord, Serialization::BinaryArchive& archive ) const
        {
            KRG_ASSERT( archive.IsValid() );

            // Create mesh resource
            PhysicsGeometry* pPhysicsGeometry = KRG::New<PhysicsGeometry>();
            archive >> *pPhysicsGeometry;

            // Deserialize cooked mesh data
            if ( pPhysicsGeometry->m_hasTriangleMeshData )
            {
                TVector<Byte> cookedTriangleMeshData;
                archive >> cookedTriangleMeshData;

                PhysXSerializedInputData cooked( cookedTriangleMeshData );
                pPhysicsGeometry->m_pTriangleMesh = PhysicsWorld::GetPhysics()->createTriangleMesh( cooked );
            }

            pResourceRecord->SetResourceData( pPhysicsGeometry );
            return true;
        }

        void PhysicsGeometryLoader::UnloadInternal( ResourceID const& resID, Resource::ResourceRecord* pResourceRecord ) const
        {
            PhysicsGeometry* pPhysicsGeometry = pResourceRecord->GetResourceData<PhysicsGeometry>();
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
}
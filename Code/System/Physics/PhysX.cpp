#include "PhysX.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        Float3 const Constants::Gravity = Float3( 0, 0, -9.81f );
        PhysX* PhysX::Core = nullptr;
        physx::PxPhysics* PhysX::Physics = nullptr;

        //-------------------------------------------------------------------------
        // Task Dispatcher
        //-------------------------------------------------------------------------

        class PhysXTaskDispatcher : public PxCpuDispatcher
        {

        public:

            virtual void submitTask( PxBaseTask & task ) override
            {
                auto pTask = &task;

                // Surprisingly it is faster to run all physics tasks on a single thread since there is a fair amount of gaps when spreading the tasks across multiple cores.
                // TODO: re-evaluate this when we have additional work. Perhaps we can interleave other tasks while physics tasks are waiting
                pTask->run();
                pTask->release();
            }

            virtual PxU32 getWorkerCount() const override
            {
                return 1;
            }
        };

        //-------------------------------------------------------------------------

        void PhysX::Initialize()
        {
            KRG_ASSERT( Core == nullptr );
            Core = KRG::New<PhysX>();

            //-------------------------------------------------------------------------

            Core->m_pFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, Core->m_allocator, Core->m_errorCallback );
            Core->m_pDispatcher = KRG::New<PhysXTaskDispatcher>();

            #if KRG_DEBUG_INSTRUMENTATION
            Core->m_pPVD = PxCreatePvd( *Core->m_pFoundation );
            Core->m_pPVDTransport = PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
            #endif

            PxTolerancesScale tolerancesScale;
            tolerancesScale.length = Constants::LengthScale;
            tolerancesScale.speed = Constants::SpeedScale;
            Core->m_pPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *Core->m_pFoundation, tolerancesScale, true, Core->m_pPVD );

            //-------------------------------------------------------------------------

            Physics = Core->m_pPhysics;
        }

        void PhysX::Shutdown()
        {
            KRG_ASSERT( Core != nullptr );
            KRG_ASSERT( Core->m_pFoundation != nullptr && Core->m_pPhysics != nullptr && Core->m_pDispatcher != nullptr );

            #if KRG_DEBUG_INSTRUMENTATION
            KRG_ASSERT( !Core->m_pPVD->isConnected() );
            #endif

            //-------------------------------------------------------------------------

            Physics = nullptr;

            Core->m_pPhysics->release();

            #if KRG_DEBUG_INSTRUMENTATION
            Core->m_pPVDTransport->release();
            Core->m_pPVD->release();
            #endif

            KRG::Delete( Core->m_pDispatcher );
            Core->m_pFoundation->release();

            KRG::Delete( Core );
        }
    }
}
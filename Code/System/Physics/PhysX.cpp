#include "PhysX.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        Float3 const Constants::Gravity = Float3( 0, 0, -9.81f );
        PhysX* PhysX::Instance = nullptr;
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
            KRG_ASSERT( Instance == nullptr );
            Instance = KRG::New<PhysX>();

            //-------------------------------------------------------------------------

            Instance->m_pFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, Instance->m_allocator, Instance->m_errorCallback );
            Instance->m_pDispatcher = KRG::New<PhysXTaskDispatcher>();

            #if KRG_DEBUG_INSTRUMENTATION
            Instance->m_pPVD = PxCreatePvd( *Instance->m_pFoundation );
            Instance->m_pPVDTransport = PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
            #endif

            PxTolerancesScale tolerancesScale;
            tolerancesScale.length = Constants::LengthScale;
            tolerancesScale.speed = Constants::SpeedScale;
            Instance->m_pPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *Instance->m_pFoundation, tolerancesScale, true, Instance->m_pPVD );

            //-------------------------------------------------------------------------

            Physics = Instance->m_pPhysics;
        }

        void PhysX::Shutdown()
        {
            KRG_ASSERT( Instance != nullptr );
            KRG_ASSERT( Instance->m_pFoundation != nullptr && Instance->m_pPhysics != nullptr && Instance->m_pDispatcher != nullptr );

            #if KRG_DEBUG_INSTRUMENTATION
            KRG_ASSERT( !Instance->m_pPVD->isConnected() );
            #endif

            //-------------------------------------------------------------------------

            Physics = nullptr;

            Instance->m_pPhysics->release();

            #if KRG_DEBUG_INSTRUMENTATION
            Instance->m_pPVDTransport->release();
            Instance->m_pPVD->release();
            #endif

            KRG::Delete( Instance->m_pDispatcher );
            Instance->m_pFoundation->release();

            KRG::Delete( Instance );
        }
    }
}
#include "PhysicsWorld.h"
#include "PhysX.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
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
        // Event Callback
        //-------------------------------------------------------------------------

        class SimulationEventCallback : public PxSimulationEventCallback
        {
            virtual void onContact( PxContactPairHeader const& pairHeader, PxContactPair const* pPairs, PxU32 numPairs ) override final {}
            virtual void onTrigger( PxTriggerPair* pPairs, PxU32 numPairs ) override final {}
            virtual void onConstraintBreak( PxConstraintInfo*, PxU32 numConstraints ) override final {}
            virtual void onWake( PxActor**, PxU32 numActors ) override final {}
            virtual void onSleep( PxActor**, PxU32 numActors ) override final {}
            virtual void onAdvance( PxRigidBody const* const* pBodyBuffer, PxTransform const* pPoseBuffer, PxU32 const numEntries ) override final {}
        };

        //-------------------------------------------------------------------------
        // Physics Core
        //-------------------------------------------------------------------------
        // PhysX has restrictions regarding the number of PxFoundation/PxPhysics per process so we need this global instance to take care of that

        namespace
        {
            struct PhysicsCore
            {
                PhysicsCore()
                {
                    m_pFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, m_allocator, m_errorCallback );
                    m_pDispatcher = KRG::New<PhysXTaskDispatcher>();

                    #if KRG_DEBUG_INSTRUMENTATION
                    m_pPVD = PxCreatePvd( *m_pFoundation );
                    m_pPVDTransport = PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
                    #endif

                    PxTolerancesScale tolerancesScale;
                    tolerancesScale.length = Constants::LengthScale;
                    tolerancesScale.speed = Constants::SpeedScale;
                    m_pPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *m_pFoundation, tolerancesScale, true, m_pPVD );

                    //-------------------------------------------------------------------------

                    // Create PhysX scene

                    KRG_ASSERT( m_pScene == nullptr );
                    PxSceneDesc sceneDesc( tolerancesScale );
                    sceneDesc.gravity = ToPx( Constants::Gravity );
                    sceneDesc.cpuDispatcher = m_pDispatcher;
                    sceneDesc.filterShader = PxDefaultSimulationFilterShader;
                    m_pScene = m_pPhysics->createScene( sceneDesc );

                    //-------------------------------------------------------------------------

                    // PVD
                    #if KRG_DEBUG_INSTRUMENTATION
                    auto pPvdClient = m_pScene->getScenePvdClient();
                    pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
                    pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
                    pPvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
                    #endif

                    //-------------------------------------------------------------------------

                    #if KRG_DEBUG_INSTRUMENTATION
                    m_debugFlags = 1 << PxVisualizationParameter::eCOLLISION_SHAPES;
                    ReflectDebugVisualizationState();
                    #endif
                }

                ~PhysicsCore()
                {
                    KRG_ASSERT( m_pFoundation != nullptr && m_pPhysics != nullptr && m_pDispatcher != nullptr );

                    #if KRG_DEBUG_INSTRUMENTATION
                    if ( m_pPVD->isConnected() )
                    {
                        m_pPVD->disconnect();
                    }
                    #endif

                    //-------------------------------------------------------------------------

                    if ( m_pScene != nullptr )
                    {
                        m_pScene->release();
                        m_pScene = nullptr;
                    }

                    //-------------------------------------------------------------------------

                    m_pPhysics->release();

                    #if KRG_DEBUG_INSTRUMENTATION
                    m_pPVDTransport->release();
                    m_pPVD->release();
                    #endif

                    KRG::Delete( m_pDispatcher );
                    m_pFoundation->release();
                }

                void Update( Seconds deltaTime )
                {
                    {
                        KRG_PROFILE_GROUPED_SCOPE_COLOR( "Physics", "Simulate", MP_YELLOW );
                        m_pScene->simulate( 1.0f / 60.0f );
                    }

                    {
                        KRG_PROFILE_GROUPED_SCOPE_COLOR( "Physics", "Fetch Results", MP_YELLOW );
                        m_pScene->fetchResults( true );
                    }

                    #if KRG_DEBUG_INSTRUMENTATION
                    {
                        KRG_PROFILE_GROUPED_SCOPE_COLOR( "Physics", "Debug Update", MP_YELLOW );
                        UpdateRecordingPVD( deltaTime );
                    }
                    #endif
                }

                #if KRG_DEBUG_INSTRUMENTATION
                void UpdateRecordingPVD( Seconds TimeDelta )
                {
                    if ( m_recordingTimeLeft >= 0 )
                    {
                        if ( m_pPVD->isConnected() )
                        {
                            m_recordingTimeLeft -= TimeDelta;
                            if ( m_recordingTimeLeft < 0 )
                            {
                                m_pPVD->disconnect();
                                m_recordingTimeLeft = -1.0f;
                            }
                        }
                        else
                        {
                            m_recordingTimeLeft = -1.0f;
                        }
                    }
                }

                void ReflectDebugVisualizationState()
                {
                    KRG_ASSERT( m_pScene != nullptr );

                    auto SetVisualizationParameter = [this] ( PxVisualizationParameter::Enum flag, F32 onValue, F32 offValue )
                    {
                        bool const isFlagSet = ( m_debugFlags & ( 1 << flag ) ) != 0;
                        m_pScene->setVisualizationParameter( flag, isFlagSet ? onValue : offValue );
                    };

                    SetVisualizationParameter( PxVisualizationParameter::eSCALE, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_AABBS, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_AXES, 0.25f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_FNORMALS, 0.15f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCOLLISION_EDGES, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCONTACT_POINT, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCONTACT_NORMAL, 0.25f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eCONTACT_FORCE, 0.25f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eACTOR_AXES, 0.25f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eBODY_AXES, 0.25f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eBODY_LIN_VELOCITY, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eBODY_ANG_VELOCITY, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eBODY_MASS_AXES, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eJOINT_LIMITS, 1.0f, 0.0f );
                    SetVisualizationParameter( PxVisualizationParameter::eJOINT_LOCAL_FRAMES, 1.0f, 0.0f );
                }
                #endif

            public:

                PhysXAllocator                          m_allocator;
                PhysXUserErrorCallback                  m_errorCallback;
                PxFoundation*                           m_pFoundation = nullptr;
                PxPvd*                                  m_pPVD = nullptr;
                PxPhysics*                              m_pPhysics = nullptr;
                PxCpuDispatcher*                        m_pDispatcher = nullptr;

                physx::PxScene*                         m_pScene = nullptr;
                physx::PxSimulationEventCallback*       m_pEventCallbackHandler = nullptr;

                #if KRG_DEBUG_INSTRUMENTATION
                Seconds                                 m_recordingTimeLeft = -1.0f;
                U32                                     m_debugFlags = 0;
                F32                                     m_debugDrawDistance = 10.0f;
                #endif

                #if KRG_DEBUG_INSTRUMENTATION
                PxPvdTransport*                         m_pPVDTransport = nullptr;
                #endif
            };

            //-------------------------------------------------------------------------

            static PhysicsCore*                         g_pPhysicsCore = nullptr;
        }

        //-------------------------------------------------------------------------

        void PhysicsWorld::Initialize()
        {
            KRG_ASSERT( g_pPhysicsCore == nullptr );
            g_pPhysicsCore = KRG::New<PhysicsCore>();
        }

        void PhysicsWorld::Shutdown()
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr );
            KRG::Delete( g_pPhysicsCore );
        }

        void PhysicsWorld::Update( Seconds deltaTime )
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr );
            g_pPhysicsCore->Update( deltaTime );
        }

        //-------------------------------------------------------------------------

        PxPhysics* PhysicsWorld::GetPhysics()
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr && g_pPhysicsCore->m_pPhysics != nullptr );
            return g_pPhysicsCore->m_pPhysics;
        }

        physx::PxScene* PhysicsWorld::GetScene()
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr && g_pPhysicsCore->m_pScene != nullptr );
            return g_pPhysicsCore->m_pScene;
        }

        //-------------------------------------------------------------------------
        // DEBUG
        //-------------------------------------------------------------------------

        #if KRG_DEBUG_INSTRUMENTATION
        U32 PhysicsWorld::GetDebugFlags()
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr && g_pPhysicsCore->m_pScene != nullptr );
            return g_pPhysicsCore->m_debugFlags;
        }

        void PhysicsWorld::SetDebugFlags( U32 debugFlags )
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr && g_pPhysicsCore->m_pScene != nullptr );
            g_pPhysicsCore->m_debugFlags = debugFlags;
            g_pPhysicsCore->ReflectDebugVisualizationState();
        }

        float PhysicsWorld::GetDebugDrawDistance()
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr && g_pPhysicsCore->m_pScene != nullptr );
            return g_pPhysicsCore->m_debugDrawDistance;
        }

        void PhysicsWorld::SetDebugDrawDistance( float drawDistance )
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr && g_pPhysicsCore->m_pScene != nullptr );
            g_pPhysicsCore->m_debugDrawDistance = Math::Max( drawDistance, 0.0f );
        }

        bool PhysicsWorld::IsConnectedToPVD()
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr );
            return g_pPhysicsCore->m_pPVD->isConnected();
        }

        void PhysicsWorld::ConnectToPVD( Seconds timeToRecord )
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr );
            if( !g_pPhysicsCore->m_pPVD->isConnected() )
            {
                g_pPhysicsCore->m_pPVD->connect( *g_pPhysicsCore->m_pPVDTransport, PxPvdInstrumentationFlag::eALL );
                g_pPhysicsCore->m_recordingTimeLeft = timeToRecord;
            }
        }

        void PhysicsWorld::DisconnectFromPVD()
        {
            KRG_ASSERT( g_pPhysicsCore != nullptr );
            if ( g_pPhysicsCore->m_pPVD->isConnected() )
            {
                g_pPhysicsCore->m_pPVD->disconnect();
            }
        }
        #endif
    }
}
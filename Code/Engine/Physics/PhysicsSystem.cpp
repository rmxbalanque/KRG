#include "PhysicsSystem.h"
#include "Engine/Physics/Components/PhysicsGeometryComponent.h"
#include "System/Entity/Entity.h"
#include "System/Core/Profiling/Profiling.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    UpdatePriorityList const PhysicsSystem::PriorityList = UpdatePriorityList().EnableUpdateStage( UpdateStage::Physics );

    //-------------------------------------------------------------------------

    namespace
    {
        class PhysXTaskDispatcher final : public PxCpuDispatcher
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
    }

    //-------------------------------------------------------------------------

    void PhysicsSystem::Initialize()
    {
        KRG_ASSERT( m_pFoundation == nullptr && m_pPhysics == nullptr && m_pDispatcher == nullptr );

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
    }

    void PhysicsSystem::Shutdown()
    {
        KRG_ASSERT( m_pFoundation != nullptr && m_pPhysics != nullptr && m_pDispatcher != nullptr );

        #if KRG_DEBUG_INSTRUMENTATION
        KRG_ASSERT( !m_pPVD->isConnected() );
        #endif

        //-------------------------------------------------------------------------

        m_pPhysics->release();

        #if KRG_DEBUG_INSTRUMENTATION
        m_pPVDTransport->release();
        m_pPVD->release();
        #endif

        KRG::Delete( m_pDispatcher );
        m_pFoundation->release();
    }

    //-------------------------------------------------------------------------

    void PhysicsSystem::InitializeEntitySystem( SystemRegistry const& systemRegistry )
    {
        KRG_ASSERT( m_pPhysics != nullptr );

        // Create PhysX scene
        //-------------------------------------------------------------------------

        auto const& tolerancesScale = m_pPhysics->getTolerancesScale();

        KRG_ASSERT( m_pScene == nullptr );
        PxSceneDesc sceneDesc( tolerancesScale );
        sceneDesc.gravity = ToPx( Constants::Gravity );
        sceneDesc.cpuDispatcher = m_pDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        m_pScene = m_pPhysics->createScene( sceneDesc );

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

    void PhysicsSystem::ShutdownEntitySystem()
    {
        KRG_ASSERT( m_pPhysics != nullptr );

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
    }

    void PhysicsSystem::UpdateEntitySystem( UpdateContext const& ctx )
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
            UpdateRecordingPVD( ctx.GetDeltaTime() );
        }
        #endif
    }

    //-------------------------------------------------------------------------

    void PhysicsSystem::RegisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        auto pGeometryComponent = ComponentCast<PhysicsGeometryComponent>( pComponent );
        if ( pGeometryComponent != nullptr )
        {
            auto& registeredComponent = m_geometryComponents.AddRecord( pEntity->GetID() );
            registeredComponent.m_pComponent = pGeometryComponent;

            // Create physics actor
            if ( pGeometryComponent->m_pPhysicsGeometry->GetTriangleMesh() != nullptr )
            {
                auto pMaterial = m_pPhysics->createMaterial( 0.5f, 0.5f, 0.6f );

                Transform const& worldTransform = pGeometryComponent->GetWorldTransform();

                // Create shape
                PxTriangleMeshGeometry triMeshGeo( pGeometryComponent->m_pPhysicsGeometry->GetTriangleMesh() );
                triMeshGeo.scale = ToPx( worldTransform.GetScale() );
                PxShape* pShape = m_pPhysics->createShape( triMeshGeo, *pMaterial );
                if ( pShape != nullptr )
                {
                    // Create physx actor
                    PxTransform const bodyPose( ToPx( worldTransform.GetTranslation() ), ToPx( worldTransform.GetRotation() ) );
                    PxRigidStatic* pStaticBody = m_pPhysics->createRigidStatic( bodyPose );
                    pStaticBody->attachShape( *pShape );
                    m_pScene->addActor( *pStaticBody );
                    pGeometryComponent->m_pPhysicsActor = pStaticBody;

                    // Release created temp resources
                    pShape->release();
                    pMaterial->release();
                }
            }
        }
    }

    void PhysicsSystem::UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent )
    {
        auto const pRecord = m_geometryComponents[pEntity->GetID()];
        if ( pRecord != nullptr )
        {
            // Destroy physics actor
            auto pGeometryComponent = pRecord->m_pComponent;
            KRG_ASSERT( pGeometryComponent != nullptr );

            if ( pGeometryComponent->m_pPhysicsActor != nullptr )
            {
                pGeometryComponent->m_pPhysicsActor->release();
                pGeometryComponent->m_pPhysicsActor = nullptr;
            }

            // Remove record
            m_geometryComponents.RemoveRecord( pEntity->GetID() );
        }
    }

    //------------------------------------------------------------------------- 
    // Debug
    //-------------------------------------------------------------------------

    #if KRG_DEBUG_INSTRUMENTATION
    void PhysicsSystem::SetDebugFlags( U32 debugFlags )
    {
        KRG_ASSERT( m_pScene != nullptr );
        m_debugFlags = debugFlags;
        ReflectDebugVisualizationState();
    }

    void PhysicsSystem::SetDebugDrawDistance( float drawDistance )
    {
        KRG_ASSERT( m_pScene != nullptr );
        m_debugDrawDistance = Math::Max( drawDistance, 0.0f );
    }

    bool PhysicsSystem::IsConnectedToPVD()
    {
        KRG_ASSERT( m_pPVD != nullptr );
        return m_pPVD->isConnected();
    }

    void PhysicsSystem::ConnectToPVD( Seconds timeToRecord )
    {
        KRG_ASSERT( m_pPVD != nullptr );
        if ( !m_pPVD->isConnected() )
        {
            m_pPVD->connect( *m_pPVDTransport, PxPvdInstrumentationFlag::eALL );
            m_recordingTimeLeft = timeToRecord;
        }
    }

    void PhysicsSystem::DisconnectFromPVD()
    {
        KRG_ASSERT( m_pPVD != nullptr );
        if ( m_pPVD->isConnected() )
        {
            m_pPVD->disconnect();
        }
    }

    void PhysicsSystem::UpdateRecordingPVD( Seconds TimeDelta )
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

    void PhysicsSystem::ReflectDebugVisualizationState()
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
}
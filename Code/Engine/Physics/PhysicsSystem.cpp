#include "PhysicsSystem.h"
#include "System/Physics/PhysX.h"
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

    void PhysicsSystem::Initialize()
    {
        PhysX::Initialize();
    }

    void PhysicsSystem::Shutdown()
    {
        PhysX::Shutdown();
    }

    //-------------------------------------------------------------------------

    void PhysicsSystem::InitializeEntitySystem( SystemRegistry const& systemRegistry )
    {
        KRG_ASSERT( PhysX::Physics != nullptr );

        // Create PhysX scene
        //-------------------------------------------------------------------------

        auto const& tolerancesScale = PhysX::Physics->getTolerancesScale();

        KRG_ASSERT( m_pScene == nullptr );
        PxSceneDesc sceneDesc( tolerancesScale );
        sceneDesc.gravity = ToPx( Constants::Gravity );
        sceneDesc.cpuDispatcher = PhysX::Instance->m_pDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;
        m_pScene = PhysX::Physics->createScene( sceneDesc );

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
        KRG_ASSERT( PhysX::Physics != nullptr );

        #if KRG_DEBUG_INSTRUMENTATION
        if ( PhysX::Instance->m_pPVD->isConnected() )
        {
            PhysX::Instance->m_pPVD->disconnect();
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
                auto pMaterial = PhysX::Physics->createMaterial( 0.5f, 0.5f, 0.6f );

                Transform const& worldTransform = pGeometryComponent->GetWorldTransform();

                // Create shape
                PxTriangleMeshGeometry triMeshGeo( pGeometryComponent->m_pPhysicsGeometry->GetTriangleMesh() );
                triMeshGeo.scale = ToPx( worldTransform.GetScale() );
                PxShape* pShape = PhysX::Physics->createShape( triMeshGeo, *pMaterial );
                if ( pShape != nullptr )
                {
                    // Create physx actor
                    PxTransform const bodyPose( ToPx( worldTransform.GetTranslation() ), ToPx( worldTransform.GetRotation() ) );
                    PxRigidStatic* pStaticBody = PhysX::Physics->createRigidStatic( bodyPose );
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
        KRG_ASSERT( PhysX::Instance->m_pPVD != nullptr );
        return PhysX::Instance->m_pPVD->isConnected();
    }

    void PhysicsSystem::ConnectToPVD( Seconds timeToRecord )
    {
        KRG_ASSERT( PhysX::Instance->m_pPVD != nullptr );
        if ( !PhysX::Instance->m_pPVD->isConnected() )
        {
            PhysX::Instance->m_pPVD->connect( *PhysX::Instance->m_pPVDTransport, PxPvdInstrumentationFlag::eALL );
            m_recordingTimeLeft = timeToRecord;
        }
    }

    void PhysicsSystem::DisconnectFromPVD()
    {
        KRG_ASSERT( PhysX::Instance->m_pPVD != nullptr );
        if ( PhysX::Instance->m_pPVD->isConnected() )
        {
            PhysX::Instance->m_pPVD->disconnect();
        }
    }

    void PhysicsSystem::UpdateRecordingPVD( Seconds TimeDelta )
    {
        if ( m_recordingTimeLeft >= 0 )
        {
            if ( PhysX::Instance->m_pPVD->isConnected() )
            {
                m_recordingTimeLeft -= TimeDelta;
                if ( m_recordingTimeLeft < 0 )
                {
                    PhysX::Instance->m_pPVD->disconnect();
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
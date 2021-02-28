#include "PhysicsSystem.h"
#include "Engine/Physics/Components/PhysicsGeometryComponent.h"
#include "System/Entity/Entity.h"
#include "System/Core/Profiling/Profiling.h"
#include "PhysicsScene.h"

//-------------------------------------------------------------------------

using namespace physx;

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    void PhysicsSystem::Initialize()
    {
        KRG_ASSERT( m_pFoundation == nullptr && m_pPhysics == nullptr && m_pDispatcher == nullptr );

        m_pAllocatorCallback = KRG::New<PhysXAllocator>();
        m_pErrorCallback = KRG::New<PhysXUserErrorCallback>();

        m_pFoundation = PxCreateFoundation( PX_PHYSICS_VERSION, *m_pAllocatorCallback, *m_pErrorCallback );
        m_pDispatcher = KRG::New<PhysXTaskDispatcher>();

        #if KRG_DEVELOPMENT_TOOLS
        m_pPVD = PxCreatePvd( *m_pFoundation );
        m_pPVDTransport = PxDefaultPvdSocketTransportCreate( "127.0.0.1", 5425, 10 );
        #endif

        PxTolerancesScale tolerancesScale;
        tolerancesScale.length = Constants::LengthScale;
        tolerancesScale.speed = Constants::SpeedScale;
        m_pPhysics = PxCreatePhysics( PX_PHYSICS_VERSION, *m_pFoundation, tolerancesScale, true, m_pPVD );
        m_pCooking = PxCreateCooking( PX_PHYSICS_VERSION, *m_pFoundation, PxCookingParams( tolerancesScale ) );

        //-------------------------------------------------------------------------

        PhysicsScene::CreateUnitCylinderMesh( *m_pPhysics, m_pCooking );
    }

    void PhysicsSystem::Shutdown()
    {
        KRG_ASSERT( m_pFoundation != nullptr && m_pPhysics != nullptr && m_pDispatcher != nullptr );

        PhysicsScene::DestroyUnitCylinderMesh();

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        if ( m_pPVD->isConnected() )
        {
            m_pPVD->disconnect();
        }
        #endif

        //-------------------------------------------------------------------------

        m_pCooking->release();
        m_pPhysics->release();

        #if KRG_DEVELOPMENT_TOOLS
        m_pPVDTransport->release();
        m_pPVD->release();
        #endif

        KRG::Delete( m_pDispatcher );
        m_pFoundation->release();

        KRG::Delete( m_pErrorCallback );
        KRG::Delete( m_pAllocatorCallback );
    }

    void PhysicsSystem::Update( UpdateContext& ctx )
    {
        for ( auto pScene : m_scenes )
        {
            pScene->Simulate();
        }

        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        UpdatePVD( ctx.GetDeltaTime() );
        #endif
    }

    //-------------------------------------------------------------------------
    // Scene Management
    //-------------------------------------------------------------------------

    PhysicsScene* PhysicsSystem::CreateScene()
    {
        return m_scenes.emplace_back( KRG::New<PhysicsScene>( *m_pPhysics, m_pDispatcher ) );
    }

    void PhysicsSystem::DestroyScene( PhysicsScene* pScene )
    {
        auto foundIter = eastl::find( m_scenes.begin(), m_scenes.end(), pScene );
        KRG_ASSERT( foundIter != m_scenes.end() );
        KRG::Delete( *foundIter );
        m_scenes.erase_unsorted( foundIter );
    }

    //------------------------------------------------------------------------- 
    // Debug
    //-------------------------------------------------------------------------

    #if KRG_DEVELOPMENT_TOOLS
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

    void PhysicsSystem::UpdatePVD( Seconds TimeDelta )
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
    #endif
}
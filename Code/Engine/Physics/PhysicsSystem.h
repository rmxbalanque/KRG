#pragma once

#include "_Module/API.h"
#include "PhysX.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Physics
{
    class PhysicsScene;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_PHYSICS_API PhysicsSystem : public ISystem
    {

    public:

        KRG_SYSTEM_ID( PhysicsSystem );

    public:

        PhysicsSystem() = default;

        void Initialize();
        void Shutdown();
        void Update( UpdateContext& ctx );

        inline physx::PxPhysics& GetPxPhysics() const { return *m_pPhysics; }
        inline physx::PxCpuDispatcher* GetPxCpuDispatcher() const { return m_pDispatcher; }

        // Scenes
        //-------------------------------------------------------------------------

        PhysicsScene* CreateScene();
        void DestroyScene( PhysicsScene* pScene );

        inline TInlineVector<PhysicsScene*, 2> const& GetScenes() const { return m_scenes; }

        // Debug
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        bool IsConnectedToPVD();
        void ConnectToPVD( Seconds timeToRecord = -1.0f );
        void DisconnectFromPVD();
        void UpdatePVD( Seconds TimeDelta );
        #endif

    private:

        physx::PxFoundation*                            m_pFoundation = nullptr;
        physx::PxPhysics*                               m_pPhysics = nullptr;
        physx::PxCooking*                               m_pCooking = nullptr;
        physx::PxCpuDispatcher*                         m_pDispatcher = nullptr;
        physx::PxAllocatorCallback*                     m_pAllocatorCallback = nullptr;
        physx::PxErrorCallback*                         m_pErrorCallback = nullptr;

        TInlineVector<PhysicsScene*, 2>                 m_scenes;

        #if KRG_DEVELOPMENT_TOOLS
        physx::PxPvd*                                   m_pPVD = nullptr;
        physx::PxPvdTransport*                          m_pPVDTransport = nullptr;
        Seconds                                         m_recordingTimeLeft = -1.0f;
        #endif
    };
}
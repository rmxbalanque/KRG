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
    // Physics System
    //-------------------------------------------------------------------------
    // This is effectively a singleton since Physx only allows a single instance of the foundation class

    class KRG_ENGINE_PHYSICS_API PhysicsSystem : public ISystem
    {
        friend class SystemScopeLock;

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

        void CreateSharedMeshes();
        void DestroySharedMeshes();

        // System Lock
        //-------------------------------------------------------------------------
        // This is used to ensure thread safety for any system operations like creating resources, etc...

        inline Threading::Mutex& GetSystemMutex() { return m_systemLock; }

    private:

        physx::PxFoundation*                            m_pFoundation = nullptr;
        physx::PxPhysics*                               m_pPhysics = nullptr;
        physx::PxCooking*                               m_pCooking = nullptr;
        physx::PxCpuDispatcher*                         m_pDispatcher = nullptr;
        physx::PxAllocatorCallback*                     m_pAllocatorCallback = nullptr;
        physx::PxErrorCallback*                         m_pErrorCallback = nullptr;

        Threading::Mutex                                m_systemLock;
        TInlineVector<PhysicsScene*, 2>                 m_scenes;

        #if KRG_DEVELOPMENT_TOOLS
        physx::PxPvd*                                   m_pPVD = nullptr;
        physx::PxPvdTransport*                          m_pPVDTransport = nullptr;
        Seconds                                         m_recordingTimeLeft = -1.0f;
        #endif
    };

    //-------------------------------------------------------------------------
    // Physics System Lock
    //-------------------------------------------------------------------------
    // Lock the global physics within a specified scope

    class SystemScopeLock
    {
    public:

        inline SystemScopeLock( PhysicsSystem& system )
            : m_system( system )
        {
            system.GetSystemMutex().lock();
        }

        inline SystemScopeLock( PhysicsSystem* pSystem )
            : SystemScopeLock( *pSystem )
        {}

        inline ~SystemScopeLock()
        {
            m_system.GetSystemMutex().unlock();
        }

        SystemScopeLock( SystemScopeLock const& ) = delete;
        SystemScopeLock( SystemScopeLock&& ) = delete;
        SystemScopeLock& operator=( SystemScopeLock const& ) = delete;
        SystemScopeLock& operator=( SystemScopeLock&& ) = delete;

    private:

        PhysicsSystem&   m_system;
    };
}
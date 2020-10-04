#pragma once
#include "_Module/API.h"
#include "System/Core/Types/Time.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxSimulationEventCallback;
    class PxScene;
    class PxPhysics;
    class PxCpuDispatcher;
}

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class KRG_SYSTEM_PHYSICS_API PhysicsWorld
        {

        public:

            // Initialize the core physics systems
            static void Initialize();

            // Shutdown the core physics systems
            static void Shutdown();

            // Update the physics world
            static void Update( Seconds deltaTime );

            // Get created physics system
            static physx::PxPhysics* GetPhysics();

            // Get the primary physics scene
            static physx::PxScene* GetScene();

            // Debug
            //-------------------------------------------------------------------------

            #if KRG_DEBUG_INSTRUMENTATION
            static U32 GetDebugFlags();
            static void SetDebugFlags( U32 debugFlags);
            static float GetDebugDrawDistance();
            static void SetDebugDrawDistance( float drawDistance );

            static bool IsConnectedToPVD();
            static void ConnectToPVD( Seconds timeToRecord = -1.0f );
            static void DisconnectFromPVD();
            #endif
        };
    }
}
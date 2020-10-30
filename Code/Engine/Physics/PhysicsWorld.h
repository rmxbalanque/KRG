#pragma once

#include "_Module/API.h"
#include "System/Entity/EntityGlobalSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace physx
{
    class PxSimulationEventCallback;
    class PxScene;
}

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Physics
    {
        class PhysicsGeometryComponent;

        //-------------------------------------------------------------------------

        class KRG_ENGINE_PHYSICS_API PhysicsWorld : public ISystem, public IGlobalEntitySystem
        {
            struct RegisteredGeometryComponent : public EntityRegistryRecord
            {
                PhysicsGeometryComponent*               m_pComponent = nullptr;
            };

        public:

            KRG_SYSTEM_ID( PhysicsWorld );
            KRG_ENTITY_GLOBAL_SYSTEM( PhysicsWorld );

        public:

            PhysicsWorld() = default;

            void Initialize();
            void Shutdown();

            // Get the primary physics scene
            inline physx::PxScene* GetScene() { return m_pScene; }

            //-------------------------------------------------------------------------

            // Debug
            #if KRG_DEBUG_INSTRUMENTATION
            inline U32 GetDebugFlags() const { return m_debugFlags; }
            void SetDebugFlags( U32 debugFlags );
            inline float GetDebugDrawDistance() const { return m_debugDrawDistance; }
            void SetDebugDrawDistance( float drawDistance );

            bool IsConnectedToPVD();
            void ConnectToPVD( Seconds timeToRecord = -1.0f );
            void DisconnectFromPVD();

            void UpdateRecordingPVD( Seconds TimeDelta );
            void ReflectDebugVisualizationState();
            #endif

        private:

            virtual void InitializeEntitySystem( SystemRegistry const& systemRegistry ) override;
            virtual void ShutdownEntitySystem() override;
            virtual void UpdateEntitySystem( UpdateContext const& ctx ) override;

            virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
            virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        private:

            physx::PxScene*                                 m_pScene = nullptr;
            physx::PxSimulationEventCallback*               m_pEventCallbackHandler = nullptr;

            #if KRG_DEBUG_INSTRUMENTATION
            Seconds                                         m_recordingTimeLeft = -1.0f;
            U32                                             m_debugFlags = 0;
            F32                                             m_debugDrawDistance = 10.0f;
            #endif

            EntityRegistry<RegisteredGeometryComponent>     m_geometryComponents;
        };
    }
}
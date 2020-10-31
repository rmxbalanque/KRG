#pragma once

#include "_Module/API.h"
#include "System/Navmesh/NavPower.h"
#include "System/Entity/EntityGlobalSystem.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"

//-------------------------------------------------------------------------

namespace KRG::Navmesh
{
    class NavmeshComponent;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_NAVMESH_API NavmeshSystem : public ISystem, public IGlobalEntitySystem
    {
        struct RegisteredNavmeshComponent : public EntityRegistryRecord
        {
            NavmeshComponent*               m_pComponent = nullptr;
        };

        struct RegisteredNavmesh
        {
            RegisteredNavmesh( UUID ID, char* pNavmesh ) : m_componentID( ID ), m_pNavmesh( pNavmesh ) { KRG_ASSERT( ID.IsValid() && pNavmesh != nullptr ); }

            UUID    m_componentID;
            char*   m_pNavmesh;
        };

        #if KRG_DEBUG_INSTRUMENTATION
        class NavmeshDebugRenderer final : public bfx::Renderer
        {

        public:

            inline void SetDebugDrawingSystem( Debug::DrawingSystem* pDebugDrawingSystem ) { m_pDebugDrawingSystem = pDebugDrawingSystem; }

        private:

            virtual void DrawLineList( bfx::LineSegment const* pLines, U32 numLines, bfx::Color const& color ) override;
            virtual void DrawTriList( bfx::Triangle const* pTris, U32 numTris, bfx::Color const& color ) override;
            virtual void DrawString( bfx::Color const& color, char const* str ) override;
            virtual void DrawString( bfx::Color const& color, bfx::Vector3 const& pos, char const* str ) override;

        private:

            Debug::DrawingSystem*                       m_pDebugDrawingSystem = nullptr;
        };
        #endif

    public:

        KRG_SYSTEM_ID( NavmeshSystem );
        KRG_ENTITY_GLOBAL_SYSTEM( NavmeshSystem );

    public:

        NavmeshSystem() = default;

        void Initialize();
        void Shutdown();

    private:

        virtual void InitializeEntitySystem( SystemRegistry const& systemRegistry ) override;
        virtual void ShutdownEntitySystem() override;
        virtual void UpdateEntitySystem( UpdateContext const& ctx ) override;

        virtual void RegisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;
        virtual void UnregisterComponent( Entity const* pEntity, EntityComponent* pComponent ) override final;

        void RegisterNavmesh( NavmeshComponent* pComponent );
        void UnregisterNavmesh( NavmeshComponent* pComponent );

    private:

        EntityRegistry<RegisteredNavmeshComponent>      m_navmeshComponents;
        TVector<RegisteredNavmesh>                      m_registeredNavmeshes;

        #if KRG_DEBUG_INSTRUMENTATION
        NavmeshDebugRenderer                            m_debugRenderer;
        #endif
    };
}
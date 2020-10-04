#pragma once

#include "Engine/Core/DebugUI/DebugUI.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    class EntityWorld;
    class Entity;
    class EntityComponent;
    class SpatialEntityComponent;

    //-------------------------------------------------------------------------

    class EntityDebugViewController : public Debug::DebugView
    {
    public:

        EntityDebugViewController();

        inline void Initialize( EntityWorld* pWorld ) { KRG_ASSERT( pWorld != nullptr ); m_pWorld = pWorld; }
        inline void Shutdown() { m_pWorld = nullptr; }

    private:

        virtual void DrawWindows( UpdateContext const& context ) override final;

        void DrawWorldBrowser( UpdateContext const& context );
        void DrawComponentEntry( EntityComponent const* pComponent );
        void DrawSpatialComponentTree( SpatialEntityComponent const* pComponent );

    private:

        EntityWorld*        m_pWorld = nullptr;
        bool                m_isWorldBrowserOpen = false;

        // Browser Data
        TVector<Entity*>    m_entities;
        Entity*             m_pSelectedEntity = nullptr;
    };
}
#endif
#pragma once

#include "Engine/Core/DebugTools/DebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
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

        void DrawMapLoader( UpdateContext const& context );

    private:

        EntityWorld*        m_pWorld = nullptr;
        bool                m_isWorldBrowserOpen = false;
        bool                m_isMapLoaderOpen = false;

        // Browser Data
        TVector<Entity*>    m_entities;
        Entity*             m_pSelectedEntity = nullptr;
    };
}
#endif
#pragma once

#include "Engine/Core/Entity/Debug/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class EntityWorldManager;
    class Entity;
    class EntityComponent;
    class SpatialEntityComponent;

    //-------------------------------------------------------------------------

    class EntityDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( EntityDebugView );

    public:

        EntityDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context ) override;

        void DrawMenu( EntityUpdateContext const& context );
        void DrawWorldBrowser( EntityUpdateContext const& context );
        void DrawComponentEntry( EntityComponent const* pComponent );
        void DrawSpatialComponentTree( SpatialEntityComponent const* pComponent );

        void DrawMapLoader( EntityUpdateContext const& context );

    private:

        EntityWorld*            m_pWorld = nullptr;
        bool                    m_isWorldBrowserOpen = false;
        bool                    m_isMapLoaderOpen = false;

        // Browser Data
        TVector<Entity*>        m_entities;
        Entity*                 m_pSelectedEntity = nullptr;
    };
}
#endif
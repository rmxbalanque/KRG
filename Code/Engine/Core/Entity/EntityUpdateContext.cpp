#include "EntityUpdateContext.h"
#include "EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityUpdateContext::EntityUpdateContext( UpdateContext const& context, EntityWorld* pWorld )
        : UpdateContext( context )
        , m_pWorld( pWorld )
        , m_rawDeltaTime( m_deltaTime )
        , m_isGameWorld( pWorld->IsGameWorld() )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        // Apply world time scale
        if ( !pWorld->IsPaused() )
        {
            m_deltaTime *= pWorld->GetTimeScale();
        }
    }

    IWorldEntitySystem* EntityUpdateContext::GetWorldSystem( uint32 worldSystemID ) const
    {
        return m_pWorld->GetWorldSystem( worldSystemID );
    }

    EntityModel::EntityMap* EntityUpdateContext::GetPersistentMap() const
    {
        return m_pWorld->GetPersistentMap();
    }

    Render::Viewport const* EntityUpdateContext::GetViewport() const
    {
        return m_pWorld->GetViewport();
    }

    EntityWorldID const& EntityUpdateContext::GetWorldID() const
    {
        return m_pWorld->GetID();
    }

    #if KRG_DEVELOPMENT_TOOLS
    Drawing::DrawContext EntityUpdateContext::GetDrawingContext() const
    {
        return m_pWorld->m_debugDrawingSystem.GetDrawingContext();
    }

    Drawing::DrawingSystem* EntityUpdateContext::GetDebugDrawingSystem() const
    {
        return m_pWorld->GetDebugDrawingSystem();
    }
    #endif
}
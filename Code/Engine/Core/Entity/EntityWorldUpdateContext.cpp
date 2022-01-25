#include "EntityWorldUpdateContext.h"
#include "EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EntityWorldUpdateContext::EntityWorldUpdateContext( UpdateContext const& context, EntityWorld* pWorld )
        : UpdateContext( context )
        , m_pWorld( pWorld )
        , m_rawDeltaTime( m_deltaTime )
        , m_isGameWorld( pWorld->IsGameWorld() )
    {
        KRG_ASSERT( m_pWorld != nullptr );

        // Handle paused world
        if ( pWorld->IsPaused() )
        {
            if ( pWorld->IsTimeStepRequested() )
            {
                m_deltaTime = 1.0f / 60.0f;
            }
            else // Set delta time to 0.0f
            {
                m_deltaTime = 0.0f;
                m_isPaused = true;
            }
        }
        else // Apply world time scale
        {
            m_deltaTime *= pWorld->GetTimeScale();
        }

        KRG_ASSERT( m_deltaTime >= 0.0f );
    }

    IWorldEntitySystem* EntityWorldUpdateContext::GetWorldSystem( uint32 worldSystemID ) const
    {
        return m_pWorld->GetWorldSystem( worldSystemID );
    }

    EntityModel::EntityMap* EntityWorldUpdateContext::GetPersistentMap() const
    {
        return m_pWorld->GetPersistentMap();
    }

    Render::Viewport const* EntityWorldUpdateContext::GetViewport() const
    {
        return m_pWorld->GetViewport();
    }

    Input::InputState const* EntityWorldUpdateContext::GetInputState() const
    {
        return m_pWorld->GetInputState();
    }

    float EntityWorldUpdateContext::GetTimeScale() const
    {
        return m_pWorld->GetTimeScale();
    }

    EntityWorldID const& EntityWorldUpdateContext::GetWorldID() const
    {
        return m_pWorld->GetID();
    }

    #if KRG_DEVELOPMENT_TOOLS
    Drawing::DrawContext EntityWorldUpdateContext::GetDrawingContext() const
    {
        return m_pWorld->m_debugDrawingSystem.GetDrawingContext();
    }

    Drawing::DrawingSystem* EntityWorldUpdateContext::GetDebugDrawingSystem() const
    {
        return m_pWorld->GetDebugDrawingSystem();
    }
    #endif
}
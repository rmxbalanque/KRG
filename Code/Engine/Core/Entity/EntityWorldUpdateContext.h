#pragma once
#include "Engine/Core/_Module/API.h"
#include "EntityIDs.h"
#include "Engine/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class IWorldEntitySystem;
    class EntityWorld;
    namespace Input { class InputState; }
    namespace Render { class Viewport; }
    namespace EntityModel{ class EntityMap; }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityWorldUpdateContext : public UpdateContext
    {
    public:

        EntityWorldUpdateContext( UpdateContext const& context, EntityWorld* pWorld );

        // Get the original delta time for this frame (without the world timescale applied)
        KRG_FORCE_INLINE Seconds GetRawDeltaTime() const { return m_rawDeltaTime; }

        // Get the time scaling for the current world
        float GetTimeScale() const;

        // Get the world ID - threadsafe
        EntityWorldID const& GetWorldID() const;

        // Get an entity world system - threadsafe since these never changed during the lifetime of a world
        template<typename T> inline T* GetWorldSystem() const
        {
            static_assert( std::is_base_of<KRG::IWorldEntitySystem, T>::value, "T is not derived from IWorldEntitySystem" );
            return Cast<T>( GetWorldSystem( T::s_entitySystemID ) );
        }

        // Get the world type - threadsafe since this never changes
        KRG_FORCE_INLINE bool IsGameWorld() const { return m_isGameWorld; }

        // Is the world paused? i.e. time scale <= 0.0f;
        inline bool IsWorldPaused() const { return m_isPaused; }

        // Get the persistent map - threadsafe - all dynamic entity creation is done in this map
        EntityModel::EntityMap* GetPersistentMap() const;

        // Get the viewport for this world
        Render::Viewport const* GetViewport() const;

        // Get the input state for this world
        Input::InputState const* GetInputState() const;

        // Get the debug drawing context for this world - threadsafe
        #if KRG_DEVELOPMENT_TOOLS
        Drawing::DrawContext GetDrawingContext() const;
        Drawing::DrawingSystem* GetDebugDrawingSystem() const;
        #endif

    private:

        // Threadsafe since these never changed during the lifetime of a world
        IWorldEntitySystem* GetWorldSystem( uint32 worldSystemID ) const;

        // Delete any ability to copy this struct
        explicit EntityWorldUpdateContext( EntityWorldUpdateContext const& ) = delete;
        explicit EntityWorldUpdateContext( EntityWorldUpdateContext&& ) = delete;
        EntityWorldUpdateContext& operator=( EntityWorldUpdateContext const& ) = delete;
        EntityWorldUpdateContext& operator=( EntityWorldUpdateContext&& ) = delete;

    private:

        EntityWorld*                    m_pWorld = nullptr;
        Seconds                         m_rawDeltaTime;
        bool                            m_isGameWorld = true;
        bool                            m_isPaused = false;
    };
}
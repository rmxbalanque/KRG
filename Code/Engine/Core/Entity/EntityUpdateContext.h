#pragma once
#include "Engine/Core/_Module/API.h"
#include "EntityIDs.h"
#include "Engine/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class IWorldEntitySystem;
    class EntityWorld;
    namespace Render { class Viewport; }
    namespace EntityModel{ class EntityMap; }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityUpdateContext : public UpdateContext
    {
    public:

        EntityUpdateContext( UpdateContext const& context, EntityWorld* pWorld );

        // Get the original delta time for this frame (without the world timescale applied)
        KRG_FORCE_INLINE Seconds GetRawDeltaTime() const { return m_rawDeltaTime; }

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

        // Get the persistent map - threadsafe - all dynamic entity creation is done in this map
        EntityModel::EntityMap* GetPersistentMap() const;

        // Get the viewport for this world
        Render::Viewport const* GetViewport() const;

        // Get the debug drawing context for this world - threadsafe
        #if KRG_DEVELOPMENT_TOOLS
        Drawing::DrawContext GetDrawingContext() const;
        Drawing::DrawingSystem* GetDebugDrawingSystem() const;
        #endif

    private:

        // Threadsafe since these never changed during the lifetime of a world
        IWorldEntitySystem* GetWorldSystem( uint32 worldSystemID ) const;

        // Delete any ability to copy this struct
        explicit EntityUpdateContext( EntityUpdateContext const& ) = delete;
        explicit EntityUpdateContext( EntityUpdateContext&& ) = delete;
        EntityUpdateContext& operator=( EntityUpdateContext const& ) = delete;
        EntityUpdateContext& operator=( EntityUpdateContext&& ) = delete;

    private:

        EntityWorld*                    m_pWorld = nullptr;
        Seconds                         m_rawDeltaTime;
        bool                            m_isGameWorld = true;
    };
}
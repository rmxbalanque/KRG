#pragma once
#include "Engine/Core/_Module/API.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class Viewport; }

//-------------------------------------------------------------------------

namespace KRG
{
    class IWorldEntitySystem;
    class EntityWorld;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityUpdateContext : public UpdateContext
    {
    public:

        EntityUpdateContext( UpdateContext const& context, EntityWorld* pWorld );

        // Get the world ID - threadsafe
        UUID const& GetWorldID() const;

        // Get an entity world system - threadsafe since these never changed during the lifetime of a world
        template<typename T> inline T* GetWorldSystem() const
        {
            static_assert( std::is_base_of<KRG::IWorldEntitySystem, T>::value, "T is not derived from IWorldEntitySystem" );
            return Cast<T>( GetWorldSystem( T::s_entitySystemID ) );
        }

        // Get the viewport for this world
        Render::Viewport const* GetViewport() const;

        // Get the debug drawing context for this world - threadsafe
        #if KRG_DEVELOPMENT_TOOLS
        Debug::DrawingContext GetDrawingContext() const;
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
    };
}
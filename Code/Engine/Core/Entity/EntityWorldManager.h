#pragma once

#include "Engine/Core/_Module/API.h"
#include "System/Resource/ResourceRequesterID.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Systems/SystemRegistry.h"
#include "System/Core/Types/Event.h"
#include "System/TypeSystem/TypeInfo.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;
    class EntityWorld;
    namespace Render { class Viewport; }

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API EntityWorldManager : public ISystem
    {
        friend class EntityDebugView;

    public:

        KRG_SYSTEM_ID( EntityWorldManager );

    public:

        ~EntityWorldManager();

        void Initialize( SystemRegistry const& systemsRegistry );
        void Shutdown();

        // Loading
        //-------------------------------------------------------------------------

        bool IsBusyLoading() const;
        void UpdateLoading();

        // Worlds
        //-------------------------------------------------------------------------

        // TEMP HACK
        inline EntityWorld* GetPrimaryWorld() { return m_worlds[0]; }
        void SetViewportForPrimaryWorld( Render::Viewport* pViewport );

        inline TMultiUserEvent<EntityWorld*> OnCreateNewWorld() { return m_createNewWorldEvent; }
        void UpdateWorlds( UpdateContext const& context );

        // Hot Reload
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload );
        void EndHotReload();
        #endif

    private:

        void CreateWorld();
        void DestroyWorld( EntityWorld* pWorld );

    private:

        SystemRegistry const*                               m_pSystemsRegistry = nullptr;
        TInlineVector<EntityWorld*, 5>                      m_worlds;
        TMultiUserEventInternal<EntityWorld*>               m_createNewWorldEvent;
        TVector<TypeSystem::TypeInfo const*>                m_worldSystemTypeInfos;

        #if KRG_DEVELOPMENT_TOOLS
        TVector<TypeSystem::TypeInfo const*>                m_debugViewTypeInfos;
        #endif
    };
}
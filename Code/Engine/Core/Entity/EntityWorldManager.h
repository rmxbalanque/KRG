#pragma once

#include "Engine/Core/_Module/API.h"
#include "System/Resource/ResourceRequesterID.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;
    class EntityWorld;
    class SystemRegistry;
    enum class EntityWorldType : uint8;
    namespace TypeSystem { struct TypeInfo; }
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

        //-------------------------------------------------------------------------

        // Called at the start of the frame
        void StartFrame();

        // Called at the end of the frame, just before rendering
        void EndFrame();

        // Loading
        //-------------------------------------------------------------------------

        bool IsBusyLoading() const;
        void UpdateLoading();

        // Worlds
        //-------------------------------------------------------------------------

        inline EntityWorld* GetPrimaryWorld() { return m_worlds[0]; }
        TInlineVector<EntityWorld*, 5> const& GetWorlds() const { return m_worlds; }

        EntityWorld* CreateWorld( EntityWorldType worldType );
        void DestroyWorld( EntityWorld* pWorld );

        inline TMultiUserEvent<EntityWorld*> OnCreateNewWorld() { return m_createNewWorldEvent; }
        void UpdateWorlds( UpdateContext const& context );

        // Editor
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void SetPlayerControllerState( EntityWorld* pWorld, bool isControllerEnabled );
        #endif

        // Hot Reload
        //-------------------------------------------------------------------------

        #if KRG_DEVELOPMENT_TOOLS
        void BeginHotReload( TVector<Resource::ResourceRequesterID> const& usersToReload );
        void EndHotReload();
        #endif

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
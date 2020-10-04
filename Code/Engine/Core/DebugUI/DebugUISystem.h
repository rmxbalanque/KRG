#pragma once

#include "../_Module/API.h"
#include "DebugUI.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Systems/ISystem.h"
#include "System/Core/Settings/Setting.h"
#include "System/Core/Types/Containers.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Debug
    {
        class KRG_ENGINE_CORE_API DebugUISystem : public ISystem
        {
        public:

            KRG_SYSTEM_ID( KRG::Debug::DebugUISystem );

        private:

            struct Menu
            {
                using MenuPath = TInlineVector<String, 5>;

                static MenuPath CreatePathFromString( String const& pathString );

            public:

                Menu( String const& title ) : m_title( title ) { KRG_ASSERT( !m_title.empty() ); }

                inline bool IsEmpty() const { return m_childMenus.empty() && m_callbacks.empty(); }

                void AddCallback( DebugMenuCallback const* pCallback );
                void RemoveCallback( DebugMenuCallback const* pCallback );
                void RemoveEmptyChildMenus();
                void DrawMenu( UpdateContext const& context );

            private:

                Menu& FindOrAddSubMenu( MenuPath const& path );
                bool TryFindMenuCallback( DebugMenuCallback const* pCallback );
                bool TryFindAndRemoveMenuCallback( DebugMenuCallback const* pCallback );

            public:

                String                              m_title;
                TVector<Menu>                       m_childMenus;
                TVector<DebugMenuCallback const*>      m_callbacks;
            };

        public:

            DebugUISystem() = default;
            ~DebugUISystem();

            void Initialize( SettingsRegistry const& settingsRegistry );
            void Shutdown();

            void RegisterDebugView( DebugView* pDebugView );
            void UnregisterDebugView( DebugView* pDebugView );

            void Update( UpdateContext const& context );

        private:

            void RegisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks );
            void UnregisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks );

            void RegisterDebugSettings();
            void UnregisterDebugSettings();

            void DrawOverlayMenu( UpdateContext const& context );
            void DrawDebugWindows( UpdateContext const& context );
            void DrawOverlayStatusBar( UpdateContext const& context );

            void DrawSettingUI( Setting* pSettingBase );

        protected:

            SettingsRegistry const*             m_pSettingsRegistry = nullptr;
            TVector<DebugMenuCallback>          m_debugSettingMenuCallbacks;
            TVector<DebugView*>                 m_viewControllers;
            Seconds                             m_avgTimeDelta = 0.0f;
            Menu                                m_mainMenu = Menu("Main Menu");
            bool                                m_debugOverlayEnabled = false;
        };
    }
}
#endif
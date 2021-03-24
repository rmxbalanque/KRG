#pragma once

#include "../_Module/API.h"
#include "DebugView.h"
#include "System/DevTools/DevelopmentTools.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/Settings/Setting.h"
#include "System/Core/Types/Containers.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    namespace Math { class Viewport; }

    //-------------------------------------------------------------------------

    namespace Debug
    {
        class KRG_ENGINE_CORE_API DebugTools final : public ImGuiX::DevelopmentTools
        {

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

                String                                      m_title;
                TVector<Menu>                               m_childMenus;
                TVector<DebugMenuCallback const*>           m_callbacks;
            };

            struct ModalPopupMessage
            {
                String                                      m_ID;
                String                                      m_channel;
                String                                      m_message;
                Log::Severity                               m_severity;
                bool                                        m_isOpen = true;
            };

        public:

            DebugTools() = default;
            ~DebugTools();

            virtual void Initialize( SettingsRegistry const& settingsRegistry ) override final;
            virtual void Shutdown() override final;
            virtual void Update( UpdateContext const& context, Render::ViewportManager& viewportSystem ) override final;

            void RegisterDebugView( DebugView* pDebugView );
            void UnregisterDebugView( DebugView* pDebugView );

            inline bool HasModalPopupOpen() const { return !m_modalPopups.empty(); }

        private:

            void RegisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks );
            void UnregisterMenuCallbacks( TVector<DebugMenuCallback> const& callbacks );

            void RegisterDebugSettings();
            void UnregisterDebugSettings();

            void DrawPopups( UpdateContext const& context );
            void DrawOverlayMenu( UpdateContext const& context );
            void DrawDebugWindows( UpdateContext const& context );
            void DrawOverlayStatusBar( UpdateContext const& context );

            void DrawSettingUI( Setting* pSettingBase );

        protected:

            SettingsRegistry const*             m_pSettingsRegistry = nullptr;
            TVector<DebugMenuCallback>          m_debugSettingMenuCallbacks;
            TVector<DebugView*>                 m_viewControllers;
            TVector<ModalPopupMessage>          m_modalPopups;
            Seconds                             m_avgTimeDelta = 0.0f;
            Menu                                m_mainMenu = Menu("Main Menu");
            bool                                m_debugOverlayEnabled = false;
        };
    }
}
#endif
#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class DebugSetting;
    class UpdateContext;

    //-------------------------------------------------------------------------

    class SystemDebugView final : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( SystemDebugView );

    public:

        SystemDebugView();

    private:

        virtual void DrawWindows( EntityUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override {};
        void DrawMenu( EntityUpdateContext const& context );
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API SystemLogView
    {
    public:

        SystemLogView();
        bool Draw( UpdateContext const& context );

    public:

        bool                                                m_showLogMessages = true;
        bool                                                m_showLogWarnings = true;
        bool                                                m_showLogErrors = true;

    private:

        InlineString<255>                                   m_logFilter;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API DebugSettingsView
    {
    public:

        bool Draw( UpdateContext const& context );
    };
}
#endif
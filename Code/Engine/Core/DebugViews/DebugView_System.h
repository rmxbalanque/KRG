#pragma once

#include "Engine/Core/Entity/Debug/EntityWorldDebugView.h"

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

        virtual void DrawWindows( EntityUpdateContext const& context ) override {};
        void DrawMenu( EntityUpdateContext const& context );
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API SystemLogView
    {
    public:

        bool Draw( UpdateContext const& context );

    private:

        InlineString<255>                                   m_logFilter;
        bool                                                m_showLogMessages = true;
        bool                                                m_showLogWarnings = true;
        bool                                                m_showLogErrors = true;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API DebugSettingsView
    {
    public:

        bool Draw( UpdateContext const& context );
    };
}
#endif
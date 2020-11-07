#pragma once

#include "Engine/Core/DebugUI/DebugUI.h"

//-------------------------------------------------------------------------

#if KRG_DEBUG_INSTRUMENTATION
namespace KRG
{
    namespace Debug
    {
        class SystemDebugViewController : public DebugView
        {
        public:

            SystemDebugViewController();

        private:

            virtual void DrawWindows( UpdateContext const& context ) override final;

            void DrawSystemMenu( UpdateContext const& context );
            void DrawSystemLogWindow( UpdateContext const& context );

        private:

            bool        m_isSystemLogWindowOpen = false;
        };
    }
}
#endif
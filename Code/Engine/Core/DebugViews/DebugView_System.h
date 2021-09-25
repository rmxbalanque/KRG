#pragma once

#include "Engine/Core/DebugTools/DebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
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
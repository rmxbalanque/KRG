#pragma once

#include "Engine/Core/Entity/Debug/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG
{
    class DebugSetting;

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
}
#endif
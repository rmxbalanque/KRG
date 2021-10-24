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

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context ) override;

        void DrawSystemMenu( EntityUpdateContext const& context );
        void DrawSystemLogWindow( EntityUpdateContext const& context );

        void DrawDebugSetting( DebugSetting* pDebugSetting );

    private:

        bool        m_isSystemLogWindowOpen = false;
    };
}
#endif
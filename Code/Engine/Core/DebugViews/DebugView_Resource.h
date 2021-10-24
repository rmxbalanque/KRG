#pragma once

#include "Engine/Core/Entity/Debug/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Resource
{
    class ResourceSystem;

    //-------------------------------------------------------------------------

    class ResourceDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( ResourceDebugView );

    public:

        ResourceDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityUpdateContext const& context ) override;

        void DrawResourceMenu( EntityUpdateContext const& context );
        void DrawHistoryWindow( EntityUpdateContext const& context );
        void DrawReferenceTrackerWindow( EntityUpdateContext const& context );

    private:

        ResourceSystem*         m_pResourceSystem = nullptr;
        bool                    m_isHistoryWindowOpen = false;
        bool                    m_isReferenceTrackerWindowOpen = false;
    };
}
#endif
#pragma once

#include "Engine/Core/DebugTools/DebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Resource
{
    class ResourceSystem;

    //-------------------------------------------------------------------------

    class ResourceDebugViewController : public Debug::DebugView
    {
    public:

        ResourceDebugViewController();

        inline void Initialize( ResourceSystem* pResourceSystem ) { KRG_ASSERT( pResourceSystem != nullptr ); m_pResourceSystem = pResourceSystem; }
        inline void Shutdown() { m_pResourceSystem = nullptr; }

    private:

        virtual void DrawWindows( UpdateContext const& context ) override final;

        void DrawResourceMenu( UpdateContext const& context );
        void DrawHistoryWindow( UpdateContext const& context );
        void DrawReferenceTrackerWindow( UpdateContext const& context );

    private:

        ResourceSystem*         m_pResourceSystem = nullptr;
        bool                    m_isHistoryWindowOpen = false;
        bool                    m_isReferenceTrackerWindowOpen = false;
    };
}
#endif
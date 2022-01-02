#pragma once

#include "Engine/Core/Entity/EntityWorldDebugView.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::Resource
{
    class ResourceSystem;

    //-------------------------------------------------------------------------

    class KRG_ENGINE_CORE_API ResourceDebugView : public EntityWorldDebugView
    {
        KRG_REGISTER_TYPE( ResourceDebugView );

    public:

        static void DrawResourceLogWindow( ResourceSystem* pResourceSystem, bool* pIsOpen );
        static void DrawReferenceTrackerWindow( ResourceSystem* pResourceSystem, bool* pIsOpen );

    public:

        ResourceDebugView();

    private:

        virtual void Initialize( SystemRegistry const& systemRegistry, EntityWorld const* pWorld ) override;
        virtual void Shutdown() override;
        virtual void DrawWindows( EntityWorldUpdateContext const& context, ImGuiWindowClass* pWindowClass ) override;

        void DrawResourceMenu( EntityWorldUpdateContext const& context );

    private:

        ResourceSystem*         m_pResourceSystem = nullptr;
        bool                    m_isHistoryWindowOpen = false;
        bool                    m_isReferenceTrackerWindowOpen = false;
    };
}
#endif
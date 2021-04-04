#pragma once
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class ViewportManager; }
namespace KRG { class UpdateContext; }

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Editor Tool
    //-------------------------------------------------------------------------
    // Base class for an editor tool (a window or logical set of windows), helps to manage common window state

    class EditorTool
    {
    public:

        virtual ~EditorTool() {}

        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}
        virtual void FrameEndUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}

        // Window State
        //-------------------------------------------------------------------------

        virtual char const * const GetName() = 0;

        bool IsOpen() const { return m_isOpen; }
        void SetOpen( bool isOpen ) { m_isOpen = isOpen; }
        void Open() { m_isOpen = true; }
        void Close() { m_isOpen = false; }

    protected:

        bool m_isOpen = false;
    };
}
#pragma once

#include "EditorModel.h"
#include "System/DevTools/ImguiX.h"

//-------------------------------------------------------------------------

namespace KRG::Render { class ViewportManager; }
namespace KRG { class UpdateContext; }

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Editor Tool Base
    //-------------------------------------------------------------------------
    // Base class for an editor tool (a window or logical set of windows), helps to manage common window state
    // DO NOT DERIVED FROM THIS CLASS - derive from 'TEditorTool' instead

    class KRG_TOOLS_CORE_API EditorTool
    {
        template<typename ModelType> friend class TEditorTool;

    public:

        virtual ~EditorTool() {}

        virtual void Initialize( UpdateContext const& context ) {};
        virtual void Shutdown( UpdateContext const& context ) {};

        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}
        virtual void FrameEndUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) {}

        // Window State
        //-------------------------------------------------------------------------

        virtual char const * const GetName() = 0;

        bool IsOpen() const { return m_isOpen; }
        void SetOpen( bool isOpen ) { m_isOpen = isOpen; }
        void Open() { m_isOpen = true; }
        void Close() { m_isOpen = false; }

    private:

        EditorTool() = default;

    protected:

        bool m_isOpen = false;
    };

    //-------------------------------------------------------------------------

    template<typename ModelType>
    class TEditorTool : public EditorTool
    {
    public:

        TEditorTool( EditorModel* pModel )
            : EditorTool()
            , m_model( static_cast<ModelType&>( *pModel ) )
        {
            KRG_ASSERT( pModel != nullptr );
        }

    protected:

        ModelType&  m_model;
    };
}
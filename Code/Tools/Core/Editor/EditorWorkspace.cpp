#include "EditorWorkspace.h"

//-------------------------------------------------------------------------

namespace KRG
{
    char const* const EditorWorkspace::s_viewportWindowName = "Viewport";

    //-------------------------------------------------------------------------

    EditorWorkspace::EditorWorkspace( EditorContext const& context, EntityWorld* pWorld )
        : m_editorContext( context )
        , m_pWorld( pWorld )
    {
        KRG_ASSERT( m_pWorld != nullptr );
    }
}
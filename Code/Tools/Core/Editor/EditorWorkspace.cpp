#include "EditorWorkspace.h"
#include "Engine/Core/Entity/EntityWorld.h"

//-------------------------------------------------------------------------

namespace KRG
{
    Debug::DrawingContext EditorWorkspace::GetDrawingContext()
    {
        return m_pWorld->GetDebugDrawingSystem()->GetDrawingContext();
    }

    //-------------------------------------------------------------------------

    EditorWorkspace::EditorWorkspace( EditorContext const& context, EntityWorld* pWorld )
        : m_editorContext( context )
        , m_pWorld( pWorld )
    {
        KRG_ASSERT( m_pWorld != nullptr );
    }

    void EditorWorkspace::Initialize()
    {
        m_workspaceWindowID.sprintf( "%s##%u", GetTitle(), GetID() );
        m_viewportWindowID.sprintf( "Viewport##%u", GetID() );
        m_dockspaceID.sprintf( "Dockspace##%u", GetID() );
    }
}
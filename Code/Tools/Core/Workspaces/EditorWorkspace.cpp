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

    EditorWorkspace::EditorWorkspace( EditorContext const& context, EntityWorld* pWorld, String const& displayName )
        : m_editorContext( context )
        , m_pWorld( pWorld )
        , m_displayName( displayName )
    {
        KRG_ASSERT( m_pWorld != nullptr );
    }

    void EditorWorkspace::SetDisplayName( String const& name )
    {
        m_displayName = name;
        m_workspaceWindowID.sprintf( "%s###window%u", m_displayName.c_str(), GetID() );
    }

    void EditorWorkspace::Initialize( UpdateContext const& context )
    {
        SetDisplayName( m_displayName );
        m_viewportWindowID.sprintf( "Viewport##%u", GetID() );
        m_dockspaceID.sprintf( "Dockspace##%u", GetID() );
    }
}
#include "Workspace_EntityCollectionEditor.h"
#include "Engine/Core/Entity/EntitySerialization.h"
#include "System/Core/Threading/TaskSystem.h"
#include "Engine/Core/Update/UpdateContext.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityCollectionEditor::EntityCollectionEditor( WorkspaceInitializationContext const& context, EntityWorld* pWorld, ResourceID const& collectionResourceID )
        : EntityEditorBaseWorkspace( context, pWorld )
        , m_collection( collectionResourceID )
    {
        SetDisplayName( collectionResourceID.GetFileNameWithoutExtension() );
    }

    //-------------------------------------------------------------------------

    void EntityCollectionEditor::Initialize( UpdateContext const& context )
    {
        EntityEditorBaseWorkspace::Initialize( context );
        LoadResource( &m_collection );
        m_collectionInstantiated = false;
    }

    void EntityCollectionEditor::Shutdown( UpdateContext const& context )
    {
        if ( m_collection.IsLoaded() )
        {
            UnloadResource( &m_collection );
        }

        m_collectionInstantiated = false;

        EntityEditorBaseWorkspace::Shutdown( context );
    }

    //-------------------------------------------------------------------------

    bool EntityCollectionEditor::Save()
    {
        auto pEditedMap = m_context.GetMap();
        if ( pEditedMap == nullptr || !( pEditedMap->IsLoaded() || pEditedMap->IsActivated() ) )
        {
            return false;
        }

        EntityCollectionDescriptor ecd;
        if ( !pEditedMap->CreateDescriptor( *m_pTypeRegistry, ecd ) )
        {
            return false;
        }

        FileSystem::Path const filePath = GetFileSystemPath( m_collection.GetResourcePath() );
        return Serialization::WriteEntityCollectionToFile( *m_pTypeRegistry, ecd, filePath);
    }

    //-------------------------------------------------------------------------

    void EntityCollectionEditor::UpdateWorkspace( UpdateContext const& context, ImGuiWindowClass* pWindowClass )
    {
        if ( !m_collectionInstantiated )
        {
            if ( m_collection.IsLoaded() )
            {
                // Create transient map for the collection editing
                auto pMap = m_pWorld->CreateTransientMap();
                pMap->AddEntityCollection( context.GetSystem<TaskSystem>(), m_context.m_typeRegistry, *m_collection.GetPtr() );
                
                // Unload the collection resource
                m_collectionInstantiated = true;
                UnloadResource( &m_collection );

                // Set the context to operate on the new map
                m_context.SetMapToUse( pMap->GetID() );
            }
        }

        EntityEditorBaseWorkspace::UpdateWorkspace( context, pWindowClass );
    }
}
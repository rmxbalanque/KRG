#include "Workspace_MapEditor.h"
#include "Tools/Core/ThirdParty/pfd/portable-file-dialogs.h"
#include "Tools/Core/Helpers/CommonDialogs.h"
#include "Engine/Core/Entity/EntitySerialization.h"
#include "System/Core/FileSystem/FileSystem.h"

//-------------------------------------------------------------------------

namespace KRG::EntityModel
{
    EntityMapEditor::EntityMapEditor( WorkspaceInitializationContext const& context, EntityWorld* pWorld )
        : EntityEditorBaseWorkspace( context, pWorld )
    {
        m_gizmo.SetTargetTransform( &m_gizmoTransform );
        SetDisplayName( "Map Editor" );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::CreateNewMap()
    {
        // Should we save the current map before unloading?
        if ( IsDirty() )
        {
            pfd::message saveChangesMsg( m_loadedMap.c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no );
            if ( saveChangesMsg.result() == pfd::button::yes )
            {
                Save();
            }
        }

        // Get new map filename
        //-------------------------------------------------------------------------

        auto const mapFilename = pfd::save_file( "Create New Map", m_pResourceDatabase->GetRawResourceDirectoryPath().c_str(), { "Map Files", "*.map" } ).result();
        if ( mapFilename.empty() )
        {
            return;
        }

        FileSystem::Path const mapFilePath( mapFilename.c_str() );
        if ( FileSystem::Exists( mapFilePath ) )
        {
            if ( pfd::message( "Confirm Overwrite", "Map file exists, should we overwrite it?", pfd::choice::yes_no, pfd::icon::error ).result() == pfd::button::no )
            {
                return;
            }
        }

        ResourceID const mapResourceID = GetResourcePath( mapFilePath );
        if ( mapResourceID == m_loadedMap )
        {
            pfd::message( "Error", "Cant override currently loaded map!", pfd::choice::ok, pfd::icon::error ).result();
            return;
        }

        if ( mapResourceID.GetResourceTypeID() != EntityMapDescriptor::GetStaticResourceTypeID() )
        {
            pfd::message( "Error", "Invalid map extension provided! Maps need to have the .map extension!", pfd::choice::ok, pfd::icon::error ).result();
            return;
        }

        // Write the map out to a new path and load it
        //-------------------------------------------------------------------------

        EntityCollectionDescriptor emptyMap;
        if ( Serialization::WriteEntityCollectionToFile( *m_pTypeRegistry, emptyMap, mapFilePath ) )
        {
            LoadMap( mapResourceID );
        }
        else
        {
            pfd::message( "Error", "Failed to create new map!", pfd::choice::ok, pfd::icon::error ).result();
        }
    }

    void EntityMapEditor::SelectAndLoadMap()
    {
        auto const selectedMap = pfd::open_file( "Load Map", m_pResourceDatabase->GetRawResourceDirectoryPath().c_str(), { "Map Files", "*.map" }, pfd::opt::none ).result();
        if ( selectedMap.empty() )
        {
            return;
        }

        FileSystem::Path const mapFilePath( selectedMap[0].c_str() );
        ResourceID const mapToLoad = GetResourcePath( mapFilePath );
        LoadMap( mapToLoad );
    }

    void EntityMapEditor::LoadMap( TResourcePtr<EntityModel::EntityMapDescriptor> const& mapToLoad )
    {
        if ( mapToLoad.GetResourceID() != m_loadedMap )
        {
            m_context.ClearSelection();

            // Should we save the current map before unloading?
            if ( IsDirty() )
            {
                pfd::message saveChangesMsg( m_loadedMap.c_str(), "You have unsaved changes! Do you want to save map?", pfd::choice::yes_no);
                if ( saveChangesMsg.result() == pfd::button::yes )
                {
                    Save();
                }
            }

            // Unload current map
            if ( m_loadedMap.IsValid() && m_pWorld->HasMap( m_loadedMap ) )
            {
                m_pWorld->UnloadMap( m_loadedMap );
            }

            // Load map
            m_loadedMap = mapToLoad.GetResourceID();
            m_pWorld->LoadMap( m_loadedMap );
            m_context.SetMapToUse( m_loadedMap );
            SetDisplayName( m_loadedMap.GetResourcePath().GetFileNameWithoutExtension() );
        }
    }

    void EntityMapEditor::SaveMap()
    {
        Save();
    }

    void EntityMapEditor::SaveMapAs()
    {
        auto pEditedMap = m_context.GetMap();
        if ( pEditedMap == nullptr || !( pEditedMap->IsLoaded() || pEditedMap->IsActivated() ) )
        {
            return;
        }

        // Get new map filename
        //-------------------------------------------------------------------------

        FileSystem::Path const mapFilePath = SaveDialog( "Map", GetFileSystemPath( m_loadedMap ).GetParentDirectory().c_str(), "Map File");
        if ( !mapFilePath.IsValid() )
        {
            return;
        }

        // Write the map out to a new path and load it
        //-------------------------------------------------------------------------

        EntityCollectionDescriptor ecd;
        if ( !pEditedMap->CreateDescriptor( *m_pTypeRegistry, ecd ) )
        {
            return;
        }

        if ( Serialization::WriteEntityCollectionToFile( *m_pTypeRegistry, ecd, mapFilePath ) )
        {
            ResourceID const mapResourcePath = GetResourcePath( mapFilePath );
            LoadMap( mapResourcePath );
        }
        else
        {
            pfd::message( "Error", "Failed to save file!", pfd::choice::ok, pfd::icon::error ).result();
        }
    }

    bool EntityMapEditor::Save()
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

        FileSystem::Path const filePath = GetFileSystemPath( m_loadedMap );
        return Serialization::WriteEntityCollectionToFile( *m_pTypeRegistry, ecd, filePath );
    }

    //-------------------------------------------------------------------------

    void EntityMapEditor::OnGamePreviewStarted()
    {
        m_pWorld->SuspendUpdates();
        m_isGamePreviewRunning = true;
    }

    void EntityMapEditor::OnGamePreviewEnded()
    {
        m_isGamePreviewRunning = false;
        m_pWorld->ResumeUpdates();
    }
}
#pragma once

#include "EditorFile.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/Core/Update/UpdateContext.h"
#include "System/Core/FileSystem/FileSystemPath.h"
#include "System/Resource/ResourceSystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    //-------------------------------------------------------------------------
    // Editor Model
    //-------------------------------------------------------------------------
    // Base class for an editor model, this helps to manage global editor state across multiple tools
    // Each editor will create its own model and supply it to its various tools

    class EditorModel
    {

    public:

        virtual ~EditorModel();

        virtual void Initialize( UpdateContext const& context );
        virtual void Shutdown( UpdateContext const& context );
        virtual void Update( UpdateContext const& context ) {}

        // Core Systems and Settings
        //-------------------------------------------------------------------------

        inline FileSystem::Path const& GetSourceDataDirectory() const { return m_sourceDataDirectory; }
        inline FileSystem::Path const& GetCompiledDataDirectory() const { return m_compiledDataDirectory; }
        inline TypeSystem::TypeRegistry const& GetTypeRegistry() const { return *m_pTypeRegistry; }

        // File Management
        //-------------------------------------------------------------------------

        template<typename T>
        void OpenResourceFile( ResourceID const& resourceID )
        {
            static_assert( std::is_base_of<EditorFile, T>::value, "T must derived from TResourceFile" );

            KRG_ASSERT( resourceID.IsValid() );
            FileSystem::Path const path = resourceID.GetDataPath().ToFileSystemPath( m_sourceDataDirectory );

            //-------------------------------------------------------------------------

            auto foundFileIter = eastl::find( m_openFiles.begin(), m_openFiles.end(), path, [] ( EditorFile*& pExistingFile, FileSystem::Path const& path ) { return pExistingFile->GetFilePath() == path; } );
            if ( foundFileIter == m_openFiles.end() )
            {
                m_openFiles.emplace_back( KRG::New<T>( this, m_sourceDataDirectory, m_pResourceSystem, resourceID ) );
            }
            else // Switch active file
            {
                // Do nothing...
            }
        }

        void CloseFile( EditorFile* pFile );
        inline TVector<EditorFile*> const& GetOpenFiles() const { return m_openFiles; }
        inline bool IsFileOpen( FileSystem::Path const& path ) const;

        // The active file is the file that has "focus" and is currently being previewed or edited.
        inline bool IsActiveFile( EditorFile* pFile ) const { return m_pActiveFile == pFile; }
        void SetActiveFile( EditorFile* pFile );
        void ClearActiveFile();

    protected:

        FileSystem::Path                    m_sourceDataDirectory;
        FileSystem::Path                    m_compiledDataDirectory;

        // Core editor systems
        TypeSystem::TypeRegistry const*     m_pTypeRegistry = nullptr;
        Resource::ResourceSystem*           m_pResourceSystem = nullptr;
        EntityWorld*                        m_pPreviewWorld = nullptr;

        // File Management
        TVector<EditorFile*>                m_openFiles;
        EditorFile*                         m_pActiveFile = nullptr;
    };
}
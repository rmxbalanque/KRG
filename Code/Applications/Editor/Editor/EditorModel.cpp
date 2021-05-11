#include "EditorModel.h"
#include "System/Entity/EntityWorld.h"
#include "System/Resource/ResourceSettings.h"
#include "System/Core/Settings/SettingsRegistry.h"

//-------------------------------------------------------------------------

namespace KRG
{
    EditorModel::~EditorModel()
    {
        KRG_ASSERT( m_pTypeRegistry == nullptr );
        KRG_ASSERT( m_pPreviewWorld == nullptr );
        KRG_ASSERT( m_pResourceSystem == nullptr );
        KRG_ASSERT( m_openFiles.empty() );
    }

    void EditorModel::Initialize( UpdateContext const& context )
    {
        m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
        KRG_ASSERT( m_pTypeRegistry != nullptr );

        m_pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
        KRG_ASSERT( m_pResourceSystem != nullptr );

        m_pPreviewWorld = context.GetSystem<EntityWorld>();
        KRG_ASSERT( m_pPreviewWorld != nullptr );

        auto pSystemRegistry = context.GetSystem<SettingsRegistry>();
        KRG_ASSERT( pSystemRegistry != nullptr );

        auto pResourceSettings = pSystemRegistry->GetSettings<Resource::Settings>();
        KRG_ASSERT( pResourceSettings != nullptr );

        m_sourceDataDirectory = pResourceSettings->m_sourceDataPath;
        m_compiledDataDirectory = pResourceSettings->m_compiledDataPath;
    }

    void EditorModel::Shutdown( UpdateContext const& context )
    {
        ClearActiveFile();

        //-------------------------------------------------------------------------

        for ( auto& pOpenFile : m_openFiles )
        {
            KRG::Delete( pOpenFile );
        }

        m_openFiles.clear();

        //-------------------------------------------------------------------------

        m_pResourceSystem = nullptr;
        m_pPreviewWorld = nullptr;
        m_pTypeRegistry = nullptr;
        m_compiledDataDirectory = FileSystem::Path();
        m_sourceDataDirectory = FileSystem::Path();
    }

    bool EditorModel::IsFileOpen( FileSystem::Path const& path ) const
    {
        for ( auto pFile : m_openFiles )
        {
            if ( pFile->GetFilePath() == path )
            {
                return true;
            }
        }

        return false;
    }

    void EditorModel::CloseFile( EditorFile* pFile )
    {
        KRG_ASSERT( pFile != nullptr );

        if ( m_pActiveFile == pFile )
        {
            ClearActiveFile();
        }

        //-------------------------------------------------------------------------

        auto foundFileIter = eastl::find( m_openFiles.begin(), m_openFiles.end(), pFile );
        KRG_ASSERT( foundFileIter != m_openFiles.end() );
        KRG::Delete( *foundFileIter );
        m_openFiles.erase( foundFileIter );
    }

    //-------------------------------------------------------------------------

    void EditorModel::SetActiveFile( EditorFile* pFile )
    {
        KRG_ASSERT( pFile != nullptr );
        KRG_ASSERT( VectorContains( m_openFiles, pFile ) );

        if ( m_pActiveFile == pFile )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_pActiveFile != nullptr )
        {
            ClearActiveFile();
        }

        pFile->SetActive( m_pPreviewWorld );
        m_pActiveFile = pFile;
    }

    void EditorModel::ClearActiveFile()
    {
        if ( m_pActiveFile != nullptr )
        {
            m_pActiveFile->SetInactive( m_pPreviewWorld );
            m_pActiveFile = nullptr;
        }
    }
}
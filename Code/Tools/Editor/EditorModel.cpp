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
        KRG_ASSERT( m_openTabs.empty() );
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

        for ( auto& pOpenFile : m_openTabs )
        {
            KRG::Delete( pOpenFile );
        }

        m_openTabs.clear();

        //-------------------------------------------------------------------------

        m_pResourceSystem = nullptr;
        m_pPreviewWorld = nullptr;
        m_pTypeRegistry = nullptr;
        m_compiledDataDirectory = FileSystem::Path();
        m_sourceDataDirectory = FileSystem::Path();
    }

    void EditorModel::Update( UpdateContext const& context )
    {}

    bool EditorModel::IsFileOpen( FileSystem::Path const& path ) const
    {
        for ( auto pFile : m_openTabs )
        {
            if ( pFile->GetFilePath() == path )
            {
                return true;
            }
        }

        return false;
    }

    void EditorModel::CloseFile( EditorFileTab* pFile )
    {
        KRG_ASSERT( pFile != nullptr );

        if ( m_pActiveTab == pFile )
        {
            ClearActiveFile();
        }

        //-------------------------------------------------------------------------

        auto foundFileIter = eastl::find( m_openTabs.begin(), m_openTabs.end(), pFile );
        KRG_ASSERT( foundFileIter != m_openTabs.end() );
        KRG::Delete( *foundFileIter );
        m_openTabs.erase( foundFileIter );
    }

    //-------------------------------------------------------------------------

    void EditorModel::SetActiveFile( EditorFileTab* pFile )
    {
        KRG_ASSERT( pFile != nullptr );
        KRG_ASSERT( VectorContains( m_openTabs, pFile ) );

        if ( m_pActiveTab == pFile )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( m_pActiveTab != nullptr )
        {
            ClearActiveFile();
        }

        pFile->SetActive( m_pPreviewWorld );
        m_pActiveTab = pFile;
    }

    void EditorModel::ClearActiveFile()
    {
        if ( m_pActiveTab != nullptr )
        {
            m_pActiveTab->SetInactive( m_pPreviewWorld );
            m_pActiveTab = nullptr;
        }
    }
}
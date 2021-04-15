#include "EditorModel.h"
#include "System/Core/Settings/ConfigSettings.h"

//-------------------------------------------------------------------------

namespace KRG
{
    static ConfigSettingString const g_sourceDataPath( "SourceDataPath", "Paths" );
    static ConfigSettingString const g_compiledDataPath( "CompiledDataPath", "Paths" );

    //-------------------------------------------------------------------------

    EditorModel::~EditorModel()
    {
        KRG_ASSERT( m_pTypeRegistry == nullptr && m_pDataBrowserModel == nullptr );
    }

    void EditorModel::Initialize( UpdateContext const& context )
    {
        m_sourceDataDirectory = FileSystem::Path( g_sourceDataPath );
        KRG_ASSERT( m_sourceDataDirectory.ExistsAndIsDirectory() );
        
        m_compiledDataDirectory = FileSystem::Path( g_compiledDataPath );
        KRG_ASSERT( m_compiledDataDirectory.ExistsAndIsDirectory() );

        m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
        KRG_ASSERT( m_pTypeRegistry != nullptr );
    }

    void EditorModel::Shutdown( UpdateContext const& context )
    {
        m_pTypeRegistry = nullptr;
        m_compiledDataDirectory = FileSystem::Path();
        m_sourceDataDirectory = FileSystem::Path();
    }

    void EditorModel::Update( UpdateContext const& context )
    {
        // Optional
        if ( m_pDataBrowserModel != nullptr )
        {
            m_pDataBrowserModel->Update( context );
        }
    }
}
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
        m_pResourceSystem = nullptr;
        m_pPreviewWorld = nullptr;
        m_pTypeRegistry = nullptr;
        m_compiledDataDirectory = FileSystem::Path();
        m_sourceDataDirectory = FileSystem::Path();
    }
}
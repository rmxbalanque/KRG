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
        KRG_ASSERT( m_pTypeRegistry == nullptr );
    }

    void EditorModel::Initialize( UpdateContext const& context )
    {
        m_sourceDataDirectory = FileSystem::Path( g_sourceDataPath );
        KRG_ASSERT( m_sourceDataDirectory.ExistsAndIsDirectory() );
        
        m_compiledDataDirectory = FileSystem::Path( g_compiledDataPath );
        KRG_ASSERT( m_compiledDataDirectory.ExistsAndIsDirectory() );

        m_pTypeRegistry = context.GetSystem<TypeSystem::TypeRegistry>();
        KRG_ASSERT( m_pTypeRegistry != nullptr );

        m_pResourceSystem = context.GetSystem<Resource::ResourceSystem>();
        KRG_ASSERT( m_pResourceSystem != nullptr );
    }

    void EditorModel::Shutdown( UpdateContext const& context )
    {
        ClearActiveResource();

        //-------------------------------------------------------------------------

        for ( auto& pResource : m_loadedResources )
        {
            m_pResourceSystem->UnloadResource( pResource );
        }

        m_loadedResources.clear();

        //-------------------------------------------------------------------------

        m_pResourceSystem = nullptr;
        m_pTypeRegistry = nullptr;
        m_compiledDataDirectory = FileSystem::Path();
        m_sourceDataDirectory = FileSystem::Path();
    }

    void EditorModel::LoadResource( ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );

        auto const SearchPredicate = [] ( Resource::ResourcePtr const& pResource, ResourceID const& resourceID ) { return pResource.GetResourceID() == resourceID; };

        auto foundResourceIter = eastl::find( m_loadedResources.begin(), m_loadedResources.end(), resourceID, SearchPredicate );
        if ( foundResourceIter != m_loadedResources.end() )
        {
            return;
        }

        auto& resourcePtr = m_loadedResources.emplace_back( Resource::ResourcePtr( resourceID ) );
        m_pResourceSystem->LoadResource( resourcePtr );
    }

    void EditorModel::UnloadResource( ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );

        auto const SearchPredicate = [] ( Resource::ResourcePtr const& pResource, ResourceID const& resourceID ) { return pResource.GetResourceID() == resourceID; };

        auto foundResourceIter = eastl::find( m_loadedResources.begin(), m_loadedResources.end(), resourceID, SearchPredicate );
        KRG_ASSERT( foundResourceIter != m_loadedResources.end() );

        if ( resourceID == m_activeResource )
        {
            ClearActiveResource();
        }

        m_pResourceSystem->UnloadResource( *foundResourceIter );
        m_loadedResources.erase( foundResourceIter );
    }

    bool EditorModel::IsLoadedResource( ResourceID const& resourceID ) const
    {
        for ( auto& resourcePtr : m_loadedResources )
        {
            if ( resourcePtr.GetResourceID() == resourceID )
            {
                return true;
            }
        }

        return false;
    }

    Resource::ResourcePtr const& EditorModel::GetLoadedResource( ResourceID const& resourceID ) const
    {
        for ( auto& resourcePtr : m_loadedResources )
        {
            if ( resourcePtr.GetResourceID() == resourceID )
            {
                return resourcePtr;
            }
        }

        //-------------------------------------------------------------------------

        KRG_UNREACHABLE_CODE();
        static const Resource::ResourcePtr temp;
        return temp;
    }

    Resource::ResourcePtr const& EditorModel::GetActiveResourcePtr() const
    {
        KRG_ASSERT( HasActiveResource() ); 
        return GetLoadedResource( m_activeResource );
    }

    //-------------------------------------------------------------------------

    void EditorModel::SetActiveResource( ResourceID const& resourceID )
    {
        KRG_ASSERT( resourceID.IsValid() );

        if ( m_activeResource == resourceID )
        {
            return;
        }

        //-------------------------------------------------------------------------

        if ( HasActiveResource() )
        {
            OnClearActiveResource();
        }

        KRG_ASSERT( IsLoadedResource( resourceID ) );
        m_activeResource = resourceID;

        OnSetActiveResource( resourceID );
    }

    void EditorModel::ClearActiveResource()
    {
        if ( m_activeResource.IsValid() )
        {
            OnClearActiveResource();
            m_activeResource = ResourceID();
        }
    }
}
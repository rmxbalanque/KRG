#pragma once

#include "IResource.h"
#include "System/Core/Types/LoadingStatus.h"
#include "System/Core/Types/UUID.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        //-------------------------------------------------------------------------
        // A unique record for each requested resource
        //-------------------------------------------------------------------------

        class KRG_SYSTEM_RESOURCE_API ResourceRecord
        {
            friend class ResourceSystem;
            friend class ResourceRequest;
            friend class ResourceLoader;

        public:

            ResourceRecord( ResourceID resourceID ) : m_resourceID( resourceID ) { KRG_ASSERT( resourceID.IsValid() ); }
            ~ResourceRecord();

            inline bool IsValid() const { return m_resourceID.IsValid(); }
            inline ResourceID const& GetResourceID() const { return m_resourceID; }
            inline ResourceTypeID GetResourceTypeID() const { return m_resourceID.GetResourceTypeID(); }

            inline void SetLoadingStatus( LoadingStatus status ) { m_loadingStatus = status; }
            inline LoadingStatus GetLoadingStatus() const { return m_loadingStatus; }

            inline IResource* GetResourceData() { return m_pResource; }
            inline IResource const* GetResourceData() const { return m_pResource; }
            inline void SetResourceData( IResource* pResourceData ) { m_pResource = pResourceData; }

            template<typename T> 
            inline T* GetResourceData() { return static_cast<T*>( m_pResource ); }

            //-------------------------------------------------------------------------

            inline bool HasReferences() const { return !m_references.empty(); }

            inline void AddReference( UUID const& requesterID )
            { 
                m_references.emplace_back( requesterID );
            }

            inline void RemoveReference( UUID const& requesterID )
            { 
                auto iter = eastl::find( m_references.begin(), m_references.end(), requesterID );
                KRG_ASSERT( iter != m_references.end() );
                m_references.erase_unsorted( iter );
            }

            //-------------------------------------------------------------------------

            inline bool IsLoading() const { return m_loadingStatus == LoadingStatus::Loading; }
            inline bool IsLoaded() const { return m_loadingStatus == LoadingStatus::Loaded; }
            inline bool IsUnloading() const { return m_loadingStatus == LoadingStatus::Unloading; }
            inline bool IsUnloaded() const { return m_loadingStatus == LoadingStatus::Unloaded; }
            inline bool HasLoadingFailed() const { return m_loadingStatus == LoadingStatus::Failed; }

            inline bool IsBeingProcessed() const { return m_isBeingProcessed; }
            inline TInlineVector<ResourceID, 4> const& GetInstallDependencies() const { return m_installDependencyResourceIDs; }

        protected:

            ResourceID                              m_resourceID;                                   // The ID of the resource this record refers to
            IResource*                              m_pResource = nullptr;                          // The actual loaded resource data
            LoadingStatus                           m_loadingStatus = LoadingStatus::Unloaded;      // The state of this resource
            TVector<UUID>                           m_references;                                   // The list of references to this resources
            TInlineVector<ResourceID, 4>            m_installDependencyResourceIDs;                 // The list of resources that need to be loaded and installed before we can install this resource
            bool                                    m_isBeingProcessed = false;                     // Fake lock on resource ptr and dependencies, set when being loaded/unloaded
        };
    }
}
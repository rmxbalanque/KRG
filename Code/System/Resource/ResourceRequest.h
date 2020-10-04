#pragma once

#include "ResourceRecord.h"
#include "ResourceLoader.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        class KRG_SYSTEM_RESOURCE_API ResourceRequest
        {
            friend class ResourceSystem;

        public:

            enum class Stage
            {
                None = -1,

                // Load Stages
                RequestRawResource,
                WaitForRawResourceRequest,
                LoadResource,
                LoadDependencies,
                WaitForLoadDependencies,
                InstallResource,

                // Unload Stages
                UninstallResource,
                UnloadDependencies,
                UnloadResource,

                // Special Cases
                CancelWaitForLoadDependencies, // This stage is needed so we can resume correctly when going from load -> unload -> load
                CancelRawResourceRequest,

                Complete,
            };

            enum class Type
            {
                Invalid = -1,
                Load,
                Unload,
            };

            struct RequestContext
            {
                eastl::function<void( ResourceRequest* )> m_createRawRequestRequestFunction;
                eastl::function<void( ResourceRequest* )> m_cancelRawRequestRequestFunction;
                eastl::function<void( UUID const&, ResourcePtr& )> m_loadResourceFunction;
                eastl::function<void( UUID const&, ResourcePtr& )> m_unloadResourceFunction;
            };

        public:

            ResourceRequest() = default;
            ResourceRequest( UUID const& userID, ResourceRecord* pRecord, ResourceLoader* pResourceLoader );

            inline bool IsValid() const { return m_pResourceRecord != nullptr; }
            inline bool IsActive() const { return m_stage != Stage::Complete; }
            inline bool IsComplete() const { return m_stage == Stage::Complete; }
            inline bool IsLoadRequest() const { return m_type == Type::Load; }
            inline bool IsUnloadRequest() const { return m_type == Type::Unload; }
            
            inline ResourceID const& GetResourceID() const { return m_pResourceRecord->GetResourceID(); }
            inline ResourceTypeID GetResourceTypeID() const { return m_pResourceRecord->GetResourceTypeID(); }
            inline LoadingStatus GetLoadingStatus() const { return m_pResourceRecord->GetLoadingStatus(); }

            inline bool operator==( ResourceRequest const& other ) const { return GetResourceID() == other.GetResourceID(); }
            inline bool operator!=( ResourceRequest const& other ) const { return GetResourceID() != other.GetResourceID(); }

            //-------------------------------------------------------------------------

            // Called by the resource system to update the request progress
            void Update( RequestContext& requestContext );

            // Called by the resource provider once the request operation completes and provides the raw resource data
            void OnRawResourceRequestComplete( TVector<Byte>&& rawResourceData );

            // This will interrupt a load task and convert it into an unload task
            void SwitchToLoadTask();

            // This will interrupt an unload task and convert it into a load task
            void SwitchToUnloadTask();

            // This will flag an task as a reload task and perform a full unload then full reload
            void SwitchToReloadTask();

        private:

            void RequestRawResource( RequestContext& requestContext );
            void LoadResource( RequestContext& requestContext );
            void LoadDependencies( RequestContext& requestContext );
            void WaitForLoadDependencies( RequestContext& requestContext );
            void InstallResource( RequestContext& requestContext );
            void UninstallResource( RequestContext& requestContext );
            void UnloadDependencies( RequestContext& requestContext );
            void UnloadResource( RequestContext& requestContext );
            void CancelRawRequestRequest( RequestContext& requestContext );

        private:

            UUID                    m_userID;
            ResourceRecord*         m_pResourceRecord = nullptr;
            ResourceLoader*         m_pResourceLoader = nullptr;
            TVector<Byte>           m_rawResourceData;
            InstallDependencyList   m_installDependencies;
            Type                    m_type = Type::Invalid;
            Stage                   m_stage = Stage::None;
            bool                    m_isReloadRequest = false;
        };

    }
}
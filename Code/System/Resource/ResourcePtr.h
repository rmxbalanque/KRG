#pragma once

#include "ResourceRecord.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Resource
    {
        //-------------------------------------------------------------------------
        // Generic Resource Ptr
        //-------------------------------------------------------------------------
        // There is no direct access to runtime resources through generic resource ptr
        // You should generally try to avoid using generic resource ptrs

        class ResourcePtr
        {
            friend ResourceRecord;
            friend class ResourceSystem;

            KRG_SERIALIZE_MEMBERS( m_resourceID );

        public:

            ResourcePtr() = default;
            ResourcePtr( nullptr_t ) {};
            ResourcePtr( ResourceID id ) : m_resourceID( id ) { KRG_ASSERT( id.IsValid() ); }
            ResourcePtr( Resource::ResourcePtr const& rhs ) { operator=( rhs ); }
            ResourcePtr( Resource::ResourcePtr&& rhs ) { operator=( eastl::move( rhs ) ); }

            inline bool IsValid() const { return m_resourceID.IsValid(); }
            inline ResourceID const& GetResourceID() const { return m_resourceID; }
            inline ResourceTypeID GetResourceTypeID() const { return m_resourceID.GetResourceTypeID(); }

            template<typename T>
            inline T* GetPtr() { return (T*) m_pResource; }

            inline bool operator==( nullptr_t ) { return m_pResource == nullptr; }
            inline bool operator!=( nullptr_t ) { return m_pResource != nullptr; }
            inline bool operator==( ResourcePtr const& rhs ) const { return m_resourceID == rhs.m_resourceID; }
            inline bool operator!=( ResourcePtr const& rhs ) const { return m_resourceID != rhs.m_resourceID; }

            inline ResourcePtr& operator=( ResourcePtr const& rhs ) 
            {
                // Can't change a loaded resource, unload it first
                KRG_ASSERT( m_pResource == nullptr || m_pResource->IsUnloaded() );
                m_resourceID = rhs.m_resourceID;
                m_pResource = rhs.m_pResource;
                return *this;
            }

            inline ResourcePtr& operator=( ResourcePtr&& rhs )
            {
                m_resourceID = rhs.m_resourceID;
                m_pResource = rhs.m_pResource;
                rhs.m_resourceID = ResourceID();
                rhs.m_pResource = nullptr;
                return *this;
            }

            // Load status
            //-------------------------------------------------------------------------

            inline LoadingStatus GetLoadingStatus() const { return ( m_pResource != nullptr ) ? m_pResource->GetLoadingStatus() : LoadingStatus::Unloaded; }
            inline bool IsLoading() const { return GetLoadingStatus() == LoadingStatus::Loading; }
            inline bool IsLoaded() const { return GetLoadingStatus() == LoadingStatus::Loaded; }
            inline bool IsUnloading() const { return GetLoadingStatus() == LoadingStatus::Unloading; }
            inline bool IsUnloaded() const { return GetLoadingStatus() == LoadingStatus::Unloaded; }
            inline bool HasLoadingFailed() const { return GetLoadingStatus() == LoadingStatus::Failed; }

        protected:

            ResourceID                  m_resourceID;
            ResourceRecord const*       m_pResource = nullptr;
        };
    }

    //-------------------------------------------------------------------------
    // Templatized Resource Ptr
    //-------------------------------------------------------------------------

    template <typename T>
    class TResourcePtr : public Resource::ResourcePtr
    {
        static_assert( std::is_base_of<KRG::Resource::IResource, T>::value, "Invalid specialization for TResourcePtr, only classes derived from IResource are allowed." );

        KRG_SERIALIZE_MEMBERS( m_resourceID );

    public:

        TResourcePtr() : ResourcePtr() {}
        TResourcePtr( nullptr_t ) : ResourcePtr( nullptr ) {}
        TResourcePtr( ResourceID ID ) : Resource::ResourcePtr( ID ) { KRG_ASSERT( ID.GetResourceTypeID() == T::GetStaticResourceTypeID() ); }
        TResourcePtr( Resource::ResourcePtr const& otherResourcePtr ) { operator=( otherResourcePtr ); }

        // Move ctor
        TResourcePtr( Resource::ResourcePtr&& otherResourcePtr ) 
        { 
            operator=( otherResourcePtr );
            otherResourcePtr.m_resourceID = ResourceID();
            otherResourcePtr.m_pResource = nullptr;
        }

        inline bool operator==( nullptr_t ) const { return m_pResource == nullptr; }
        inline bool operator!=( nullptr_t ) const { return m_pResource != nullptr; }
        inline bool operator==( ResourcePtr const& rhs ) const { return m_resourceID == rhs.m_resourceID; }
        inline bool operator!=( ResourcePtr const& rhs ) const { return m_resourceID != rhs.m_resourceID; }
        inline bool operator==( TResourcePtr const& rhs ) const { return m_resourceID == rhs.m_resourceID; }
        inline bool operator!=( TResourcePtr const& rhs ) const { return m_resourceID != rhs.m_resourceID; }

        inline T const* operator->() const { KRG_ASSERT( m_pResource != nullptr ); return static_cast<T const*>( m_pResource->GetResourceData() ); }
        inline T const* GetPtr() const { KRG_ASSERT( m_pResource != nullptr ); return static_cast<T const*>( m_pResource->GetResourceData() ); }

        inline ResourceTypeID GetSpecializedResourceTypeID() const { return T::GetStaticResourceTypeID(); }

        inline TResourcePtr<T>& operator=( ResourcePtr const& rhs )
        {
            // Can't change a loaded resource, unload it first
            KRG_ASSERT( m_pResource == nullptr || m_pResource->IsUnloaded() );

            if ( rhs.GetResourceTypeID() == T::GetStaticResourceTypeID() )
            {
                TResourcePtr<T> const& castPtr = reinterpret_cast<TResourcePtr<T> const&>( rhs );
                m_resourceID = castPtr.m_resourceID;
                m_pResource = castPtr.m_pResource;
            }
            else // Invalid Assignment
            {
                KRG_HALT();
            }

            return *this;
        }

        template <typename U>
        inline bool operator==( const TResourcePtr<U>& rhs ) { return m_resourceID == rhs.m_resourceID; }

        template <typename U>
        inline bool operator!=( const TResourcePtr<U>& rhs ) { return m_resourceID != rhs.m_resourceID; }
    };
}
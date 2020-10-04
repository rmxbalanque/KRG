#pragma once

#include "EntityComponent.h"
#include "System/Core/Logging/Log.h"

//-------------------------------------------------------------------------
// Entity Component Pointer
//-------------------------------------------------------------------------
// * Runtime helper to allow to safe dynamic type conversions
//-------------------------------------------------------------------------

namespace KRG
{
    template<typename T> class TEntityComponentPtr;

    //-------------------------------------------------------------------------

    class EntityComponentPtr
    {
        template<typename T> friend class TEntityComponentPtr;

    public:

        EntityComponentPtr() = default;

        EntityComponentPtr( EntityComponent* pComponent ) : m_pComponent( pComponent )
        {
            KRG_ASSERT( pComponent != nullptr );
        }

        EntityComponentPtr( nullptr_t ) : m_pComponent( nullptr ) {}

        KRG_FORCE_INLINE EntityComponent* operator->() const { return m_pComponent; }
        KRG_FORCE_INLINE EntityComponent* GetRawPtr() const { return m_pComponent; }
        KRG_FORCE_INLINE bool operator !=( nullptr_t ) const { return m_pComponent != nullptr; }
        KRG_FORCE_INLINE bool operator ==( nullptr_t ) const { return m_pComponent == nullptr; }
        KRG_FORCE_INLINE bool operator !=( EntityComponentPtr const& rhs ) const { return m_pComponent != rhs.m_pComponent; }
        KRG_FORCE_INLINE bool operator ==( EntityComponentPtr const& rhs ) const { return m_pComponent == rhs.m_pComponent; }
        KRG_FORCE_INLINE operator bool() const { return m_pComponent != nullptr; }

        template<typename T>
        inline EntityComponentPtr( TEntityComponentPtr<T>& pComponent );

    private:

        EntityComponent* m_pComponent = nullptr;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    class TEntityComponentPtr
    {
        template<typename V> friend class TEntityComponentPtr;
        friend class EntityComponentPtr;

        static_assert( std::is_base_of<KRG::EntityComponent, T>::value, "Invalid specialization for TEntityComponentPtr, only classes derived from EntityComponent are allowed." );

    public:

        TEntityComponentPtr() = default;
        TEntityComponentPtr( nullptr_t ) {}
        TEntityComponentPtr( EntityComponentPtr const& entityPtr ) : TEntityComponentPtr( entityPtr.m_pComponent ) {}

        KRG_FORCE_INLINE T* operator->() const { return m_pComponent; }
        KRG_FORCE_INLINE T* GetRawPtr() const { return m_pComponent; }
        KRG_FORCE_INLINE bool operator !=( nullptr_t ) const { return m_pComponent != nullptr; }
        KRG_FORCE_INLINE bool operator ==( nullptr_t ) const { return m_pComponent == nullptr; }
        KRG_FORCE_INLINE bool operator !=( EntityComponentPtr const& rhs ) const { return m_pComponent != rhs.m_pComponent; }
        KRG_FORCE_INLINE bool operator ==( EntityComponentPtr const& rhs ) const { return m_pComponent == rhs.m_pComponent; }
        KRG_FORCE_INLINE bool operator !=( T const* pComponent ) const { return m_pComponent != pComponent; }
        KRG_FORCE_INLINE bool operator ==( T const* pComponent ) const { return m_pComponent == pComponent; }
        KRG_FORCE_INLINE operator bool() const { return m_pComponent != nullptr; }

        template<typename V> KRG_FORCE_INLINE bool operator !=( const TEntityComponentPtr<V>& rhs ) const { return m_pComponent != rhs.m_pComponent; }
        template<typename V> KRG_FORCE_INLINE bool operator ==( const TEntityComponentPtr<V>& rhs ) const { return m_pComponent == rhs.m_pComponent; }

        // Create from raw entity component ptr
        inline TEntityComponentPtr( EntityComponent* pComponent )
        {
            if ( pComponent == nullptr )
            {
                return;
            }

            // Get type info ptr from type registry
            auto pTypeInfo = pComponent->GetTypeInfo();
            if ( pTypeInfo == nullptr )
            {
                KRG_LOG_WARNING( "Entity", "Trying to create an entity component ptr from an unregistered type" );
                return;
            }

            // Check whether entity type supports interface T
            if ( pTypeInfo->IsDerivedFrom( T::TypeInfoPtr->m_ID ) )
            {
                m_pComponent = static_cast<T*>( pComponent );
            }
        }

        // Create from entity component ptr
        template<typename V>
        inline TEntityComponentPtr( TEntityComponentPtr<V> const& pComponent )
        {
            if ( pComponent.m_pComponent == nullptr )
            {
                return;
            }

            // Get type info ptr from entity instance and check whether entity type supports interface T
            if ( pComponent.m_pComponent->GetTypeInfo()->HasParentOfType( T::TypeInfoPtr->m_ID ) )
            {
                m_pComponent = static_cast<T*>( pComponent.m_pComponent );
            }
        }

    private:

        T* m_pComponent = nullptr;
    };

    //-------------------------------------------------------------------------

    template<typename T>
    inline EntityComponentPtr::EntityComponentPtr( TEntityComponentPtr<T>& pComponent )
        : m_pComponent( pComponent.m_pComponent )
    {
        KRG_ASSERT( pComponent != nullptr );
    }
}

//-------------------------------------------------------------------------

template <typename T>
inline bool operator== ( T const* pComponent, KRG::TEntityComponentPtr<T> const& componentPtr )
{
    return componentPtr == pComponent;
}

template <typename T>
inline bool operator!= ( T const* pComponent, KRG::TEntityComponentPtr<T> const& componentPtr )
{
    return componentPtr != pComponent;
}
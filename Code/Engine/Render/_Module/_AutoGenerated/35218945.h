#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 35218945

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Render\Components\SkeletalMeshComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Render::SkeletalMeshComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_RENDER_API void serialize( Archive& archive, KRG::Render::SkeletalMeshComponent& type )
    {
        archive( cereal::base_class<KRG::Render::MeshComponent>( &type ), KRG_NVP( m_materialOverrides ), KRG_NVP( m_transform ), KRG_NVP( m_pMesh ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Render::SkeletalMeshComponent const* pActualDefaultTypeInstance = ( KRG::Render::SkeletalMeshComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materialOverrides" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 2045102662;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::Material" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materialOverrides;
            propertyInfo.m_offset = offsetof( KRG::Render::SkeletalMeshComponent, m_materialOverrides );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materialOverrides.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materialOverrides.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TResourcePtr<KRG::Render::Material> );
            propertyInfo.m_size = sizeof( TVector<KRG::TResourcePtr<KRG::Render::Material>> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 2045102662;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Render::SkeletalMeshComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pMesh" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 2045102662;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::SkeletalMesh" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pMesh;
            propertyInfo.m_offset = offsetof( KRG::Render::SkeletalMeshComponent, m_pMesh );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Render::SkeletalMesh> );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_RENDER_API TTypeHelper<KRG::Render::SkeletalMeshComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Render::SkeletalMeshComponent> StaticTypeHelper;

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Render::SkeletalMeshComponent ), alignof( KRG::Render::SkeletalMeshComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Render::SkeletalMeshComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Render::SkeletalMeshComponent" );
                    typeInfo.m_size = sizeof( KRG::Render::SkeletalMeshComponent );
                    typeInfo.m_alignment = alignof( KRG::Render::SkeletalMeshComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Render::MeshComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Render::SkeletalMeshComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Render::SkeletalMeshComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pDefaultTypeInstance )->~SkeletalMeshComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Render::SkeletalMeshComponent>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Render::SkeletalMeshComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materialOverrides )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->LoadResource( resourcePtr, requesterID );
                        }
                    }

                    if ( pActualType->m_pMesh.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pMesh, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materialOverrides )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->UnloadResource( resourcePtr, requesterID );
                        }
                    }

                    if ( pActualType->m_pMesh.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pMesh, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto const& resourcePtr : pActualType->m_materialOverrides )
                    {
                        if ( !resourcePtr.IsValid() || resourcePtr.HasLoadingFailed() )
                        {
                            status = LoadingStatus::Failed;
                        }
                        else if ( resourcePtr.IsUnloaded() || resourcePtr.IsLoading() )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    if ( !pActualType->m_pMesh.IsValid() || pActualType->m_pMesh.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pMesh.IsUnloaded() || pActualType->m_pMesh.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto const& resourcePtr : pActualType->m_materialOverrides )
                    {
                        KRG_ASSERT( !resourcePtr.IsLoading() );
                        if ( !resourcePtr.IsUnloaded() )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    KRG_ASSERT( !pActualType->m_pMesh.IsLoading() );
                    if ( !pActualType->m_pMesh.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pType );
                    if ( propertyID == 2164280863 )
                    {
                        return KRG::Render::Material::GetStaticResourceTypeID();
                    }

                    if ( propertyID == 1674268345 )
                    {
                        return KRG::Render::SkeletalMesh::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pType );

                    if ( arrayID == 2164280863 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_materialOverrides.size() )
                        {
                            pActualType->m_materialOverrides.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_materialOverrides[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent const*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        return pActualType->m_materialOverrides.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2164280863 )
                    {
                        return sizeof( KRG::TResourcePtr<KRG::Render::Material> );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        pActualType->m_materialOverrides.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        pActualType->m_materialOverrides.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        pActualType->m_materialOverrides.erase( pActualType->m_materialOverrides.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( void const* pTypeInstance, void const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Render::SkeletalMeshComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Render::SkeletalMeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::SkeletalMeshComponent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2164280863 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2436416701 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1674268345 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( void const* pTypeInstance, void const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Render::SkeletalMeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::SkeletalMeshComponent const*>( pOtherTypeInstance );

                    if ( propertyID == 2164280863 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_materialOverrides.size() )
                            {
                                return false;
                            }

                            return pType->m_materialOverrides[arrayIdx] == pOtherType->m_materialOverrides[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_materialOverrides.size() != pOtherType->m_materialOverrides.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_materialOverrides.size(); i++ )
                            {
                                if( pType->m_materialOverrides[i] != pOtherType->m_materialOverrides[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 2436416701 )
                    {
                        return pType->m_transform == pOtherType->m_transform;
                    }

                    if ( propertyID == 1674268345 )
                    {
                        return pType->m_pMesh == pOtherType->m_pMesh;
                    }

                    return false;
                }

                virtual void ResetToDefault( void* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Render::SkeletalMeshComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshComponent*>( pTypeInstance );

                    if ( propertyID == 2164280863 )
                    {
                        pActualType->m_materialOverrides = pDefaultType->m_materialOverrides;
                        return;
                    }

                    if ( propertyID == 2436416701 )
                    {
                        pActualType->m_transform = pDefaultType->m_transform;
                        return;
                    }

                    if ( propertyID == 1674268345 )
                    {
                        pActualType->m_pMesh = pDefaultType->m_pMesh;
                        return;
                    }

                }

            };
        }
    }
}

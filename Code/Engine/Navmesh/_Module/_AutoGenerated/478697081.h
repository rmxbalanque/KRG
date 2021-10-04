#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 478697081

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Navmesh\Components\NavmeshComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Navmesh::NavmeshComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_NAVMESH_API void serialize( Archive& archive, KRG::Navmesh::NavmeshComponent& type )
    {
        archive( cereal::base_class<KRG::SpatialEntityComponent>( &type ), KRG_NVP( m_transform ), KRG_NVP( m_pNavmeshData ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Navmesh::NavmeshComponent> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Navmesh::NavmeshComponent const* pActualDefaultTypeInstance = ( KRG::Navmesh::NavmeshComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 2518398360;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Navmesh::NavmeshComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pNavmeshData" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 2518398360;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Navmesh::NavmeshData" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pNavmeshData;
            propertyInfo.m_offset = offsetof( KRG::Navmesh::NavmeshComponent, m_pNavmeshData );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Navmesh::NavmeshData> );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_NAVMESH_API TTypeHelper<KRG::Navmesh::NavmeshComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Navmesh::NavmeshComponent> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Navmesh::NavmeshComponent ), alignof( KRG::Navmesh::NavmeshComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Navmesh::NavmeshComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Navmesh::NavmeshComponent" );
                    typeInfo.m_size = sizeof( KRG::Navmesh::NavmeshComponent );
                    typeInfo.m_alignment = alignof( KRG::Navmesh::NavmeshComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::SpatialEntityComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Navmesh::NavmeshComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Navmesh::NavmeshComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Navmesh::NavmeshComponent" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pDefaultTypeInstance )->~NavmeshComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Navmesh::NavmeshComponent>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Navmesh::NavmeshComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );

                    if ( pActualType->m_pNavmeshData.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pNavmeshData, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );

                    if ( pActualType->m_pNavmeshData.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pNavmeshData, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pNavmeshData.IsValid() || pActualType->m_pNavmeshData.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pNavmeshData.IsUnloaded() || pActualType->m_pNavmeshData.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_pNavmeshData.IsLoading() );
                    if ( !pActualType->m_pNavmeshData.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );
                    if ( propertyID == 2013971497 )
                    {
                        return KRG::Navmesh::NavmeshData::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent const*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Navmesh::NavmeshComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Navmesh::NavmeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Navmesh::NavmeshComponent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2436416701 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2013971497 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Navmesh::NavmeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Navmesh::NavmeshComponent const*>( pOtherTypeInstance );

                    if ( propertyID == 2436416701 )
                    {
                        return pType->m_transform == pOtherType->m_transform;
                    }

                    if ( propertyID == 2013971497 )
                    {
                        return pType->m_pNavmeshData == pOtherType->m_pNavmeshData;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Navmesh::NavmeshComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pTypeInstance );

                    if ( propertyID == 2436416701 )
                    {
                        pActualType->m_transform = pDefaultType->m_transform;
                        return;
                    }

                    if ( propertyID == 2013971497 )
                    {
                        pActualType->m_pNavmeshData = pDefaultType->m_pNavmeshData;
                        return;
                    }

                }

            };
        }
    }
}

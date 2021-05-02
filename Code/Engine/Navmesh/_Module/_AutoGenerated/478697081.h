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
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Navmesh::NavmeshComponent> >( void const* pDefaultTypeInstance )
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
            propertyInfo.m_flags.Set( 0 );
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
            propertyInfo.m_flags.Set( 0 );
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

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Navmesh::NavmeshComponent ), alignof( KRG::Navmesh::NavmeshComponent ) );
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

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pDefaultTypeInstance )->~NavmeshComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Navmesh::NavmeshComponent>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Navmesh::NavmeshComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );

                    if ( pActualType->m_pNavmeshData.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pNavmeshData, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );

                    if ( pActualType->m_pNavmeshData.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pNavmeshData, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
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

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
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

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Navmesh::NavmeshComponent*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
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

            };
        }
    }
}

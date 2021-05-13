#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1504628824

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Physics\Components\PhysicsMeshComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsMeshComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsMeshComponent& type )
    {
        archive( cereal::base_class<KRG::Physics::PhysicsComponent>( &type ), KRG_NVP( m_actorType ), KRG_NVP( m_layers ), KRG_NVP( m_transform ), KRG_NVP( m_shapeType ), KRG_NVP( m_physicsMaterialOverrideIDs ), KRG_NVP( m_pPhysicsMesh ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMeshComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsMeshComponent const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsMeshComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_actorType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ActorType" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_actorType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_actorType );
            propertyInfo.m_size = sizeof( KRG::Physics::ActorType );
            propertyInfo.m_flags.Set( 4 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_layers" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TBitFlags" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::Layers" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_layers;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_layers );
            propertyInfo.m_size = sizeof( KRG::TBitFlags<KRG::Physics::Layers> );
            propertyInfo.m_flags.Set( 8 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_shapeType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_shapeType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_shapeType );
            propertyInfo.m_size = sizeof( KRG::Physics::ShapeType );
            propertyInfo.m_flags.Set( 4 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_physicsMaterialOverrideIDs" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_physicsMaterialOverrideIDs;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_physicsMaterialOverrideIDs );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_physicsMaterialOverrideIDs.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_physicsMaterialOverrideIDs.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::StringID );
            propertyInfo.m_size = sizeof( TVector<KRG::StringID> );
            propertyInfo.m_flags.Set( 2 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pPhysicsMesh" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::PhysicsMesh" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pPhysicsMesh;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_pPhysicsMesh );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Physics::PhysicsMesh> );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsMeshComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsMeshComponent> StaticTypeHelper;

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Physics::PhysicsMeshComponent ), alignof( KRG::Physics::PhysicsMeshComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Physics::PhysicsMeshComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMeshComponent" );
                    typeInfo.m_size = sizeof( KRG::Physics::PhysicsMeshComponent );
                    typeInfo.m_alignment = alignof( KRG::Physics::PhysicsMeshComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Physics::PhysicsComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMeshComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Physics::PhysicsMeshComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMeshComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pDefaultTypeInstance )->~PhysicsMeshComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsMeshComponent>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Physics::PhysicsMeshComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );

                    if ( pActualType->m_pPhysicsMesh.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pPhysicsMesh, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );

                    if ( pActualType->m_pPhysicsMesh.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pPhysicsMesh, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pPhysicsMesh.IsValid() || pActualType->m_pPhysicsMesh.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pPhysicsMesh.IsUnloaded() || pActualType->m_pPhysicsMesh.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_pPhysicsMesh.IsLoading() );
                    if ( !pActualType->m_pPhysicsMesh.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );
                    if ( propertyID == 1875393011 )
                    {
                        return KRG::Physics::PhysicsMesh::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );

                    if ( arrayID == 2552322612 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_physicsMaterialOverrideIDs.size() )
                        {
                            pActualType->m_physicsMaterialOverrideIDs.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_physicsMaterialOverrideIDs[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent const*>( pTypeInstance );

                    if ( arrayID == 2552322612 )
                    {
                        return pActualType->m_physicsMaterialOverrideIDs.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2552322612 )
                    {
                        return sizeof( KRG::StringID );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2552322612 )
                    {
                        pActualType->m_physicsMaterialOverrideIDs.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2552322612 )
                    {
                        pActualType->m_physicsMaterialOverrideIDs.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2552322612 )
                    {
                        pActualType->m_physicsMaterialOverrideIDs.erase( pActualType->m_physicsMaterialOverrideIDs.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( void const* pTypeInstance, void const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Physics::PhysicsMeshComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 403392667 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1456246424 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2436416701 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3877779688 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2552322612 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1875393011 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( void const* pTypeInstance, void const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent const*>( pOtherTypeInstance );

                    if ( propertyID == 403392667 )
                    {
                        return pType->m_actorType == pOtherType->m_actorType;
                    }

                    if ( propertyID == 1456246424 )
                    {
                        return pType->m_layers == pOtherType->m_layers;
                    }

                    if ( propertyID == 2436416701 )
                    {
                        return pType->m_transform == pOtherType->m_transform;
                    }

                    if ( propertyID == 3877779688 )
                    {
                        return pType->m_shapeType == pOtherType->m_shapeType;
                    }

                    if ( propertyID == 2552322612 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_physicsMaterialOverrideIDs.size() )
                            {
                                return false;
                            }

                            return pType->m_physicsMaterialOverrideIDs[arrayIdx] == pOtherType->m_physicsMaterialOverrideIDs[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_physicsMaterialOverrideIDs.size() != pOtherType->m_physicsMaterialOverrideIDs.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_physicsMaterialOverrideIDs.size(); i++ )
                            {
                                if( pType->m_physicsMaterialOverrideIDs[i] != pOtherType->m_physicsMaterialOverrideIDs[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 1875393011 )
                    {
                        return pType->m_pPhysicsMesh == pOtherType->m_pPhysicsMesh;
                    }

                    return false;
                }

                virtual void ResetToDefault( void* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pTypeInstance );

                    if ( propertyID == 403392667 )
                    {
                        pActualType->m_actorType = pDefaultType->m_actorType;
                        return;
                    }

                    if ( propertyID == 1456246424 )
                    {
                        pActualType->m_layers = pDefaultType->m_layers;
                        return;
                    }

                    if ( propertyID == 2436416701 )
                    {
                        pActualType->m_transform = pDefaultType->m_transform;
                        return;
                    }

                    if ( propertyID == 3877779688 )
                    {
                        pActualType->m_shapeType = pDefaultType->m_shapeType;
                        return;
                    }

                    if ( propertyID == 2552322612 )
                    {
                        pActualType->m_physicsMaterialOverrideIDs = pDefaultType->m_physicsMaterialOverrideIDs;
                        return;
                    }

                    if ( propertyID == 1875393011 )
                    {
                        pActualType->m_pPhysicsMesh = pDefaultType->m_pPhysicsMesh;
                        return;
                    }

                }

            };
        }
    }
}

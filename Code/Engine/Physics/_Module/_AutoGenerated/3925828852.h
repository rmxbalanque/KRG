#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3925828852

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Physics\Components\PhysicsCapsuleComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsCapsuleComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsCapsuleComponent& type )
    {
        archive( cereal::base_class<KRG::Physics::PhysicsComponent>( &type ), KRG_NVP( m_actorType ), KRG_NVP( m_layers ), KRG_NVP( m_transform ), KRG_NVP( m_shapeType ), KRG_NVP( m_physicsMaterialID ), KRG_NVP( m_capsuleHalfHeight ), KRG_NVP( m_capsuleRadius ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsCapsuleComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsCapsuleComponent const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsCapsuleComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_actorType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ActorType" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_actorType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_actorType );
            propertyInfo.m_size = sizeof( KRG::Physics::ActorType );
            propertyInfo.m_flags.Set( 4 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_layers" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TBitFlags" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::Layers" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_layers;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_layers );
            propertyInfo.m_size = sizeof( KRG::TBitFlags<KRG::Physics::Layers> );
            propertyInfo.m_flags.Set( 8 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_shapeType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_shapeType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_shapeType );
            propertyInfo.m_size = sizeof( KRG::Physics::ShapeType );
            propertyInfo.m_flags.Set( 4 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_physicsMaterialID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_physicsMaterialID;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_physicsMaterialID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_capsuleHalfHeight" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_capsuleHalfHeight;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_capsuleHalfHeight );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_capsuleRadius" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_capsuleRadius;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_capsuleRadius );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsCapsuleComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsCapsuleComponent> StaticTypeHelper;

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Physics::PhysicsCapsuleComponent ), alignof( KRG::Physics::PhysicsCapsuleComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Physics::PhysicsCapsuleComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::PhysicsCapsuleComponent" );
                    typeInfo.m_size = sizeof( KRG::Physics::PhysicsCapsuleComponent );
                    typeInfo.m_alignment = alignof( KRG::Physics::PhysicsCapsuleComponent );
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

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsCapsuleComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Physics::PhysicsCapsuleComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsCapsuleComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pDefaultTypeInstance )->~PhysicsCapsuleComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsCapsuleComponent>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Physics::PhysicsCapsuleComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent const*>( pTypeInstance );

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

                virtual void ClearArray( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( void const* pTypeInstance, void const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Physics::PhysicsCapsuleComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent const*>( pOtherTypeInstance );

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

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1626624371 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4161473071 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2434943242 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( void const* pTypeInstance, void const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent const*>( pOtherTypeInstance );

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

                    if ( propertyID == 1626624371 )
                    {
                        return pType->m_physicsMaterialID == pOtherType->m_physicsMaterialID;
                    }

                    if ( propertyID == 4161473071 )
                    {
                        return pType->m_capsuleHalfHeight == pOtherType->m_capsuleHalfHeight;
                    }

                    if ( propertyID == 2434943242 )
                    {
                        return pType->m_capsuleRadius == pOtherType->m_capsuleRadius;
                    }

                    return false;
                }

                virtual void ResetToDefault( void* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pTypeInstance );

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

                    if ( propertyID == 1626624371 )
                    {
                        pActualType->m_physicsMaterialID = pDefaultType->m_physicsMaterialID;
                        return;
                    }

                    if ( propertyID == 4161473071 )
                    {
                        pActualType->m_capsuleHalfHeight = pDefaultType->m_capsuleHalfHeight;
                        return;
                    }

                    if ( propertyID == 2434943242 )
                    {
                        pActualType->m_capsuleRadius = pDefaultType->m_capsuleRadius;
                        return;
                    }

                }

            };
        }
    }
}

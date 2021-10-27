#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 2430320459

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "E:\Git\KRG\Code\Engine\Physics\Components\PhysicsBoxComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsBoxComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsBoxComponent& type )
    {
        archive( cereal::base_class<KRG::Physics::PhysicsComponent>( &type ), KRG_NVP( m_actorType ), KRG_NVP( m_layers ), KRG_NVP( m_transform ), KRG_NVP( m_shapeType ), KRG_NVP( m_physicsMaterialID ), KRG_NVP( m_boxExtents ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsBoxComponent> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsBoxComponent const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsBoxComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_actorType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ActorType" );
            propertyInfo.m_parentTypeID = 2002438577;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_actorType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsBoxComponent, m_actorType );
            propertyInfo.m_size = sizeof( KRG::Physics::ActorType );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_layers" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TBitFlags" );
            propertyInfo.m_parentTypeID = 2002438577;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::Layers" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_layers;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsBoxComponent, m_layers );
            propertyInfo.m_size = sizeof( KRG::TBitFlags<KRG::Physics::Layers> );
            propertyInfo.m_flags.Set( 17 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 2002438577;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsBoxComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_shapeType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
            propertyInfo.m_parentTypeID = 2002438577;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_shapeType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsBoxComponent, m_shapeType );
            propertyInfo.m_size = sizeof( KRG::Physics::ShapeType );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_physicsMaterialID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 2002438577;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_physicsMaterialID;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsBoxComponent, m_physicsMaterialID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_boxExtents" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float3" );
            propertyInfo.m_parentTypeID = 2002438577;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_boxExtents;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsBoxComponent, m_boxExtents );
            propertyInfo.m_size = sizeof( KRG::Float3 );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsBoxComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsBoxComponent> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Physics::PhysicsBoxComponent ), alignof( KRG::Physics::PhysicsBoxComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Physics::PhysicsBoxComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::PhysicsBoxComponent" );
                    typeInfo.m_size = sizeof( KRG::Physics::PhysicsBoxComponent );
                    typeInfo.m_alignment = alignof( KRG::Physics::PhysicsBoxComponent );
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

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsBoxComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Physics::PhysicsBoxComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsBoxComponent" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pDefaultTypeInstance )->~PhysicsBoxComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsBoxComponent>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Physics::PhysicsBoxComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Physics::PhysicsBoxComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent const*>( pOtherTypeInstance );

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

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2948082710 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent const*>( pOtherTypeInstance );

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

                    if ( propertyID == 2948082710 )
                    {
                        return pType->m_boxExtents == pOtherType->m_boxExtents;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsBoxComponent*>( pTypeInstance );

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

                    if ( propertyID == 2948082710 )
                    {
                        pActualType->m_boxExtents = pDefaultType->m_boxExtents;
                        return;
                    }

                }

            };
        }
    }
}

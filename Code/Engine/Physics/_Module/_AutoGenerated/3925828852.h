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
        archive( cereal::base_class<KRG::Physics::PhysicsShapeComponent>( &type ), KRG_NVP( m_pPhysicsMaterial ), KRG_NVP( m_transform ), KRG_NVP( m_capsuleHalfHeight ), KRG_NVP( m_capsuleRadius ) );
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

            propertyInfo.m_ID = StringID( "m_pPhysicsMaterial" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::PhysicsMaterial" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pPhysicsMaterial;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_pPhysicsMaterial );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Physics::PhysicsMaterial> );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_capsuleHalfHeight" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::F32" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_capsuleHalfHeight;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_capsuleHalfHeight );
            propertyInfo.m_size = sizeof( KRG::F32 );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_capsuleRadius" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::F32" );
            propertyInfo.m_parentTypeID = 235916608;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_capsuleRadius;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsCapsuleComponent, m_capsuleRadius );
            propertyInfo.m_size = sizeof( KRG::F32 );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsCapsuleComponent> : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsCapsuleComponent> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
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

                    pParentType = KRG::Physics::PhysicsShapeComponent::StaticTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsCapsuleComponent> >( DefaultTypeInstancePtr );
                    KRG::Physics::PhysicsCapsuleComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsCapsuleComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pDefaultTypeInstance )->~PhysicsCapsuleComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsCapsuleComponent>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );

                    if ( pActualType->m_pPhysicsMaterial.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pPhysicsMaterial, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );

                    if ( pActualType->m_pPhysicsMaterial.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pPhysicsMaterial, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pPhysicsMaterial.IsValid() || pActualType->m_pPhysicsMaterial.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pPhysicsMaterial.IsUnloaded() || pActualType->m_pPhysicsMaterial.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_pPhysicsMaterial.IsLoading() );
                    if ( !pActualType->m_pPhysicsMaterial.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, U32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, U32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsCapsuleComponent*>( pType );
                    if ( propertyID == 838471742 )
                    {
                        return KRG::Physics::PhysicsMaterial::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}

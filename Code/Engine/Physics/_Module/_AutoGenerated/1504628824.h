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
        archive( cereal::base_class<KRG::Physics::PhysicsComponent>( &type ), KRG_NVP( m_actorType ), KRG_NVP( m_pPhysicsMaterial ), KRG_NVP( m_transform ), KRG_NVP( m_shapeType ), KRG_NVP( m_additionalPhysicsMaterials ), KRG_NVP( m_pPhysicsMesh ) );
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
            propertyInfo.m_flags.SetAll( 4 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pPhysicsMaterial" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::PhysicsMaterial" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pPhysicsMaterial;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_pPhysicsMaterial );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Physics::PhysicsMaterial> );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_shapeType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_shapeType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_shapeType );
            propertyInfo.m_size = sizeof( KRG::Physics::ShapeType );
            propertyInfo.m_flags.SetAll( 4 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_additionalPhysicsMaterials" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::PhysicsMaterial" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_additionalPhysicsMaterials;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_additionalPhysicsMaterials );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_additionalPhysicsMaterials.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_additionalPhysicsMaterials.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TResourcePtr<KRG::Physics::PhysicsMaterial> );
            propertyInfo.m_size = sizeof( TVector<KRG::TResourcePtr<KRG::Physics::PhysicsMaterial>> );
            propertyInfo.m_flags.SetAll( 2 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pPhysicsMesh" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 3338692939;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Physics::PhysicsMesh" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pPhysicsMesh;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshComponent, m_pPhysicsMesh );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Physics::PhysicsMesh> );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsMeshComponent> : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsMeshComponent> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
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

                    pParentType = KRG::Physics::PhysicsComponent::StaticTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMeshComponent> >( DefaultTypeInstancePtr );
                    KRG::Physics::PhysicsMeshComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMeshComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pDefaultTypeInstance )->~PhysicsMeshComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsMeshComponent>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );

                    if ( pActualType->m_pPhysicsMaterial.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pPhysicsMaterial, requesterID );
                    }

                    for ( auto& resourcePtr : pActualType->m_additionalPhysicsMaterials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->LoadResource( resourcePtr, requesterID );
                        }
                    }

                    if ( pActualType->m_pPhysicsMesh.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pPhysicsMesh, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );

                    if ( pActualType->m_pPhysicsMaterial.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pPhysicsMaterial, requesterID );
                    }

                    for ( auto& resourcePtr : pActualType->m_additionalPhysicsMaterials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->UnloadResource( resourcePtr, requesterID );
                        }
                    }

                    if ( pActualType->m_pPhysicsMesh.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pPhysicsMesh, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pPhysicsMaterial.IsValid() || pActualType->m_pPhysicsMaterial.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pPhysicsMaterial.IsUnloaded() || pActualType->m_pPhysicsMaterial.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    for ( auto const& resourcePtr : pActualType->m_additionalPhysicsMaterials )
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

                    KRG_ASSERT( !pActualType->m_pPhysicsMaterial.IsLoading() );
                    if ( !pActualType->m_pPhysicsMaterial.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    for ( auto const& resourcePtr : pActualType->m_additionalPhysicsMaterials )
                    {
                        KRG_ASSERT( !resourcePtr.IsLoading() );
                        if ( !resourcePtr.IsUnloaded() )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    KRG_ASSERT( !pActualType->m_pPhysicsMesh.IsLoading() );
                    if ( !pActualType->m_pPhysicsMesh.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );
                    if ( arrayID == 1527938720 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_additionalPhysicsMaterials.size() )
                        {
                            pActualType->m_additionalPhysicsMaterials.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_additionalPhysicsMaterials[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshComponent*>( pType );
                    if ( propertyID == 838471742 )
                    {
                        return KRG::Physics::PhysicsMaterial::GetStaticResourceTypeID();
                    }

                    if ( propertyID == 1527938720 )
                    {
                        return KRG::Physics::PhysicsMaterial::GetStaticResourceTypeID();
                    }

                    if ( propertyID == 1875393011 )
                    {
                        return KRG::Physics::PhysicsMesh::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}

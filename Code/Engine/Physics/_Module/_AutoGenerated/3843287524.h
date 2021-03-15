#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3843287524

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Physics\Components\PhysicsSphereComponent.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsSphereComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsSphereComponent& type )
    {
        archive( cereal::base_class<KRG::Physics::PhysicsComponent>( &type ), KRG_NVP( m_actorType ), KRG_NVP( m_transform ), KRG_NVP( m_shapeType ), KRG_NVP( m_physicsMaterialID ), KRG_NVP( m_radius ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsSphereComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsSphereComponent const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsSphereComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_actorType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ActorType" );
            propertyInfo.m_parentTypeID = 2460940567;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_actorType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsSphereComponent, m_actorType );
            propertyInfo.m_size = sizeof( KRG::Physics::ActorType );
            propertyInfo.m_flags.SetFlags( 4 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 2460940567;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsSphereComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_shapeType" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
            propertyInfo.m_parentTypeID = 2460940567;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_shapeType;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsSphereComponent, m_shapeType );
            propertyInfo.m_size = sizeof( KRG::Physics::ShapeType );
            propertyInfo.m_flags.SetFlags( 4 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_physicsMaterialID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 2460940567;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_physicsMaterialID;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsSphereComponent, m_physicsMaterialID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_radius" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "float" );
            propertyInfo.m_parentTypeID = 2460940567;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_radius;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsSphereComponent, m_radius );
            propertyInfo.m_size = sizeof( float );
            propertyInfo.m_flags.SetFlags( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsSphereComponent> : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsSphereComponent> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Physics::PhysicsSphereComponent ), alignof( KRG::Physics::PhysicsSphereComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Physics::PhysicsSphereComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::PhysicsSphereComponent" );
                    typeInfo.m_size = sizeof( KRG::Physics::PhysicsSphereComponent );
                    typeInfo.m_alignment = alignof( KRG::Physics::PhysicsSphereComponent );
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

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsSphereComponent> >( DefaultTypeInstancePtr );
                    KRG::Physics::PhysicsSphereComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsSphereComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsSphereComponent*>( pDefaultTypeInstance )->~PhysicsSphereComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsSphereComponent>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsSphereComponent*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsSphereComponent*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsSphereComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsSphereComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsSphereComponent*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsSphereComponent*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}

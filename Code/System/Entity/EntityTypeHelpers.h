#pragma once
#include "../_Module/API.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/TypeSystem/TypeRegistry.h"

#include "Entity.h"
#include "EntitySpatialComponent.h"
#include "EntitySystem.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace TypeHelpers
        {
            template<>
            class KRG_SYSTEM_ENTITY_API TTypeHelper<Entity> : public ITypeHelper
            {
                static TTypeHelper<Entity> StaticTypeHelper;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Entity" );
                    typeInfo.m_size = sizeof( Entity );
                    typeInfo.m_alignment = alignof( Entity );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper;
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::Entity );

                    // Register Type
                    Entity::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Entity" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Entity>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& userID, void* pType ) const override final
                {}

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, U32 arrayID, size_t arrayIdx ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, U32 propertyID ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }
            };

            //-------------------------------------------------------------------------

            template<>
            class KRG_SYSTEM_ENTITY_API TTypeHelper<EntityComponent> : public ITypeHelper
            {
                static TTypeHelper<EntityComponent> StaticTypeHelper;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::EntityComponent" );
                    typeInfo.m_size = sizeof( EntityComponent );
                    typeInfo.m_alignment = alignof( EntityComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper;
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Register Type
                    EntityComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::EntityComponent" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual void* CreateType() const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract entity component!
                    return nullptr;
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {}

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, U32 arrayID, size_t arrayIdx ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, U32 propertyID ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }
            };

            //-------------------------------------------------------------------------

            template<>
            class KRG_SYSTEM_ENTITY_API TTypeHelper<SpatialEntityComponent> : public ITypeHelper
            {
                static TTypeHelper<SpatialEntityComponent> StaticTypeHelper;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::SpatialEntityComponent" );
                    typeInfo.m_size = sizeof( SpatialEntityComponent );
                    typeInfo.m_alignment = alignof( SpatialEntityComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper;
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = KRG::EntityComponent::StaticTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register Type
                    //-------------------------------------------------------------------------

                    SpatialEntityComponent::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::SpatialEntityComponent" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual void* CreateType() const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract entity component!
                    return nullptr;
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {}

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, U32 arrayID, size_t arrayIdx ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, U32 propertyID ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }
            };

            //-------------------------------------------------------------------------

            template<>
            class KRG_SYSTEM_ENTITY_API TTypeHelper<IEntitySystem> : public ITypeHelper
            {
                static TTypeHelper<IEntitySystem> StaticTypeHelper;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::IEntitySystem" );
                    typeInfo.m_size = sizeof( IEntitySystem );
                    typeInfo.m_alignment = alignof( IEntitySystem );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper;
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntitySystem );

                    // Register Type
                    IEntitySystem::StaticTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::IEntitySystem" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual void* CreateType() const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract entity system!
                    return nullptr;
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {}

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, U32 arrayID, size_t arrayIdx ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, U32 propertyID ) const override final
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }
            };
        }

        //-------------------------------------------------------------------------

        KRG_SYSTEM_ENTITY_API void RegisterCoreEntityTypes( TypeRegistry& typeRegistry );
        KRG_SYSTEM_ENTITY_API void UnregisterCoreEntityTypes( TypeRegistry& typeRegistry );
    }

    //-------------------------------------------------------------------------

    template<class Archive>
    void serialize( Archive& archive, KRG::Entity& type )
    {
        KRG_HALT(); // Not allowed to be directly serialized
    }

    template<class Archive>
    void serialize( Archive& archive, KRG::EntityComponent& type )
    {
        KRG_HALT(); // Not allowed to be directly serialized
    }

    template<class Archive>
    void serialize( Archive& archive, KRG::SpatialEntityComponent& type )
    {
        KRG_HALT(); // Not allowed to be directly serialized
    }

    template<class Archive>
    void serialize( Archive& archive, KRG::IEntitySystem& type )
    {
        KRG_HALT(); // Not allowed to be directly serialized
    }
}
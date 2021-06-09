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
            class KRG_SYSTEM_ENTITY_API TTypeHelper<Entity> final : public ITypeHelper
            {
                static TTypeHelper<Entity> StaticTypeHelper;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override
                {
                    static Entity entity;
                    return &entity;
                }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Entity" );
                    typeInfo.m_size = sizeof( Entity );
                    typeInfo.m_alignment = alignof( Entity );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper;
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::Entity );

                    // Register Type
                    Entity::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Entity" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual IRegisteredType* CreateType() const override
                {
                    return KRG::New<KRG::Entity>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override
                {
                    new (pAllocatedMemory) KRG::Entity();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& userID, IRegisteredType* pType ) const override
                {}

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override { KRG_UNREACHABLE_CODE(); }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override { return false; }
                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override { return false; }
                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override {}
            };

            //-------------------------------------------------------------------------

            template<>
            class KRG_SYSTEM_ENTITY_API TTypeHelper<EntityComponent> final : public ITypeHelper
            {
                static TTypeHelper<EntityComponent> StaticTypeHelper;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override
                {
                    return nullptr;
                }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::EntityComponent" );
                    typeInfo.m_size = sizeof( EntityComponent );
                    typeInfo.m_alignment = alignof( EntityComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper;
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Register Type
                    EntityComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::EntityComponent" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual IRegisteredType* CreateType() const override
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract entity component!
                    return nullptr;
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override
                {
                    KRG_HALT();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override
                {}

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override { KRG_UNREACHABLE_CODE(); }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override { return false; }
                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override { return false; }
                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override {}
            };

            //-------------------------------------------------------------------------

            template<>
            class KRG_SYSTEM_ENTITY_API TTypeHelper<SpatialEntityComponent> final : public ITypeHelper
            {
                static TTypeHelper<SpatialEntityComponent> StaticTypeHelper;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override
                {
                    return nullptr;
                }

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

                    TypeSystem::TypeInfo const* pParentType = KRG::EntityComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register Type
                    //-------------------------------------------------------------------------

                    SpatialEntityComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::SpatialEntityComponent" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual IRegisteredType* CreateType() const override
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract entity component!
                    return nullptr;
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override
                {
                    KRG_HALT();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override
                {}

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override { KRG_UNREACHABLE_CODE(); }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override { return false; }
                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override { return false; }
                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override {}
            };

            //-------------------------------------------------------------------------

            template<>
            class KRG_SYSTEM_ENTITY_API TTypeHelper<IEntitySystem> final : public ITypeHelper
            {
                static TTypeHelper<IEntitySystem> StaticTypeHelper;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override
                {
                    return nullptr;
                }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::IEntitySystem" );
                    typeInfo.m_size = sizeof( IEntitySystem );
                    typeInfo.m_alignment = alignof( IEntitySystem );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper;
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntitySystem );

                    // Register Type
                    IEntitySystem::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::IEntitySystem" );
                    typeRegistry.UnregisterType( ID );
                }

                virtual IRegisteredType* CreateType() const override
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract entity system!
                    return nullptr;
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override
                {
                    KRG_HALT();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override
                {}

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override
                {}

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Loaded;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override
                {
                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override { KRG_UNREACHABLE_CODE(); }
                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override { KRG_UNREACHABLE_CODE(); }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override
                {
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override { return false; }
                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override { return false; }
                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override {}
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
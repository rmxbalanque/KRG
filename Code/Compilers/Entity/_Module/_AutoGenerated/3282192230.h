#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3282192230

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Compilers\Entity\NavmeshCompiler\NavmeshCompiler.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::AI::NavmeshResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_RESOURCECOMPILERS_ENTITY_API void serialize( Archive& archive, KRG::AI::NavmeshResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_resourceTypeID ), KRG_NVP( m_map ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::AI::NavmeshResourceDescriptor> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::AI::NavmeshResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::AI::NavmeshResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_resourceTypeID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceTypeID" );
            propertyInfo.m_parentTypeID = 156097317;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_resourceTypeID;
            propertyInfo.m_offset = offsetof( KRG::AI::NavmeshResourceDescriptor, m_resourceTypeID );
            propertyInfo.m_size = sizeof( KRG::ResourceTypeID );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_map" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 156097317;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::EntityMap" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_map;
            propertyInfo.m_offset = offsetof( KRG::AI::NavmeshResourceDescriptor, m_map );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::EntityMap> );
            propertyInfo.m_flags.SetAll( 0 );
            m_properties.insert( TPair<StringID, PropertyInfo>( propertyInfo.m_ID, propertyInfo ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_RESOURCECOMPILERS_ENTITY_API TTypeHelper<KRG::AI::NavmeshResourceDescriptor> : public ITypeHelper
            {
                static TTypeHelper<KRG::AI::NavmeshResourceDescriptor> StaticTypeHelper;

                static void const* DefaultTypeInstancePtr;

            public:

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::AI::NavmeshResourceDescriptor ), alignof( KRG::AI::NavmeshResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::AI::NavmeshResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::AI::NavmeshResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::AI::NavmeshResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::AI::NavmeshResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::TypeInfoPtr;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::AI::NavmeshResourceDescriptor> >( DefaultTypeInstancePtr );
                    KRG::AI::NavmeshResourceDescriptor::TypeInfoPtr = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::AI::NavmeshResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( DefaultTypeInstancePtr );
                    reinterpret_cast<KRG::AI::NavmeshResourceDescriptor*>( pDefaultTypeInstance )->~NavmeshResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::AI::NavmeshResourceDescriptor>();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::AI::NavmeshResourceDescriptor*>( pType );

                    if ( pActualType->m_map.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_map, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::AI::NavmeshResourceDescriptor*>( pType );

                    if ( pActualType->m_map.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_map, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::AI::NavmeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    KRG_ASSERT( !pActualType->m_map.IsUnloading() );
                    if ( !pActualType->m_map.IsValid() || pActualType->m_map.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_map.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::AI::NavmeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_map.IsLoading() );
                    if ( !pActualType->m_map.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetDynamicArrayElementDataPtr( void* pType, U32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::AI::NavmeshResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, U32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::AI::NavmeshResourceDescriptor*>( pType );
                    if ( propertyID == 4120638347 )
                    {
                        return KRG::EntityMap::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

            };
        }
    }
}

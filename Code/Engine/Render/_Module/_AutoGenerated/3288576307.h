#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3288576307

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "E:\Git\KRG\Code\Engine\Render\Components\StaticMeshComponent.h"

//-------------------------------------------------------------------------
// Enum Helper: KRG::Render::Mobility
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_Render_Mobility
            {
            public:

                static void RegisterEnum( TypeSystem::TypeRegistry& typeRegistry );
                static void UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry );
            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Render::StaticMeshComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_RENDER_API void serialize( Archive& archive, KRG::Render::StaticMeshComponent& type )
    {
        archive( cereal::base_class<KRG::Render::MeshComponent>( &type ), KRG_NVP( m_materialOverrides ), KRG_NVP( m_transform ), KRG_NVP( m_pMesh ), KRG_NVP( m_mobility ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshComponent> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Render::StaticMeshComponent const* pActualDefaultTypeInstance = ( KRG::Render::StaticMeshComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materialOverrides" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 819047598;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::Material" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materialOverrides;
            propertyInfo.m_offset = offsetof( KRG::Render::StaticMeshComponent, m_materialOverrides );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materialOverrides.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materialOverrides.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TResourcePtr<KRG::Render::Material> );
            propertyInfo.m_size = sizeof( TVector<KRG::TResourcePtr<KRG::Render::Material>> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_transform" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Transform" );
            propertyInfo.m_parentTypeID = 819047598;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_transform;
            propertyInfo.m_offset = offsetof( KRG::Render::StaticMeshComponent, m_transform );
            propertyInfo.m_size = sizeof( KRG::Transform );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pMesh" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 819047598;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::StaticMesh" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pMesh;
            propertyInfo.m_offset = offsetof( KRG::Render::StaticMeshComponent, m_pMesh );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Render::StaticMesh> );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_mobility" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Render::Mobility" );
            propertyInfo.m_parentTypeID = 819047598;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_mobility;
            propertyInfo.m_offset = offsetof( KRG::Render::StaticMeshComponent, m_mobility );
            propertyInfo.m_size = sizeof( KRG::Render::Mobility );
            propertyInfo.m_flags.Set( 9 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_RENDER_API TTypeHelper<KRG::Render::StaticMeshComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Render::StaticMeshComponent> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Render::StaticMeshComponent ), alignof( KRG::Render::StaticMeshComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Render::StaticMeshComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Render::StaticMeshComponent" );
                    typeInfo.m_size = sizeof( KRG::Render::StaticMeshComponent );
                    typeInfo.m_alignment = alignof( KRG::Render::StaticMeshComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Render::MeshComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Render::StaticMeshComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Render::StaticMeshComponent" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Render::StaticMeshComponent*>( pDefaultTypeInstance )->~StaticMeshComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Render::StaticMeshComponent>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Render::StaticMeshComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materialOverrides )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->LoadResource( resourcePtr, requesterID );
                        }
                    }

                    if ( pActualType->m_pMesh.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pMesh, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materialOverrides )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->UnloadResource( resourcePtr, requesterID );
                        }
                    }

                    if ( pActualType->m_pMesh.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pMesh, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto const& resourcePtr : pActualType->m_materialOverrides )
                    {
                        if ( !resourcePtr.IsValid() || resourcePtr.HasLoadingFailed() )
                        {
                            status = LoadingStatus::Failed;
                        }
                        else if ( !resourcePtr.IsLoaded() )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    if ( !pActualType->m_pMesh.IsValid() || pActualType->m_pMesh.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( !pActualType->m_pMesh.IsLoaded() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto const& resourcePtr : pActualType->m_materialOverrides )
                    {
                        KRG_ASSERT( !resourcePtr.IsLoading() );
                        if ( !resourcePtr.IsUnloaded() )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    KRG_ASSERT( !pActualType->m_pMesh.IsLoading() );
                    if ( !pActualType->m_pMesh.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pType );
                    if ( propertyID == 2164280863 )
                    {
                        return KRG::Render::Material::GetStaticResourceTypeID();
                    }

                    if ( propertyID == 1674268345 )
                    {
                        return KRG::Render::StaticMesh::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pType );

                    if ( arrayID == 2164280863 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_materialOverrides.size() )
                        {
                            pActualType->m_materialOverrides.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_materialOverrides[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent const*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        return pActualType->m_materialOverrides.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2164280863 )
                    {
                        return sizeof( KRG::TResourcePtr<KRG::Render::Material> );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        pActualType->m_materialOverrides.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        pActualType->m_materialOverrides.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pTypeInstance );

                    if ( arrayID == 2164280863 )
                    {
                        pActualType->m_materialOverrides.erase( pActualType->m_materialOverrides.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Render::StaticMeshComponent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Render::StaticMeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::StaticMeshComponent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2164280863 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2436416701 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1674268345 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3955994802 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Render::StaticMeshComponent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::StaticMeshComponent const*>( pOtherTypeInstance );

                    if ( propertyID == 2164280863 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_materialOverrides.size() )
                            {
                                return false;
                            }

                            return pType->m_materialOverrides[arrayIdx] == pOtherType->m_materialOverrides[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_materialOverrides.size() != pOtherType->m_materialOverrides.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_materialOverrides.size(); i++ )
                            {
                                if( pType->m_materialOverrides[i] != pOtherType->m_materialOverrides[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 2436416701 )
                    {
                        return pType->m_transform == pOtherType->m_transform;
                    }

                    if ( propertyID == 1674268345 )
                    {
                        return pType->m_pMesh == pOtherType->m_pMesh;
                    }

                    if ( propertyID == 3955994802 )
                    {
                        return pType->m_mobility == pOtherType->m_mobility;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Render::StaticMeshComponent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshComponent*>( pTypeInstance );

                    if ( propertyID == 2164280863 )
                    {
                        pActualType->m_materialOverrides = pDefaultType->m_materialOverrides;
                        return;
                    }

                    if ( propertyID == 2436416701 )
                    {
                        pActualType->m_transform = pDefaultType->m_transform;
                        return;
                    }

                    if ( propertyID == 1674268345 )
                    {
                        pActualType->m_pMesh = pDefaultType->m_pMesh;
                        return;
                    }

                    if ( propertyID == 3955994802 )
                    {
                        pActualType->m_mobility = pDefaultType->m_mobility;
                        return;
                    }

                }

            };
        }
    }
}

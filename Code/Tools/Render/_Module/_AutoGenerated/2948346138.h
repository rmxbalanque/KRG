#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 2948346138

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "E:\Git\KRG\Code\Tools\Render\ResourceDescriptors\ResourceDescriptor_RenderMesh.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Render::MeshResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_RENDER_API void serialize( Archive& archive, KRG::Render::MeshResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_meshPath ), KRG_NVP( m_materials ), KRG_NVP( m_meshName ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::MeshResourceDescriptor> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Render::MeshResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Render::MeshResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourcePath" );
            propertyInfo.m_parentTypeID = 2202398079;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshPath;
            propertyInfo.m_offset = offsetof( KRG::Render::MeshResourceDescriptor, m_meshPath );
            propertyInfo.m_size = sizeof( KRG::ResourcePath );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materials" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 2202398079;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::Material" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materials;
            propertyInfo.m_offset = offsetof( KRG::Render::MeshResourceDescriptor, m_materials );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materials.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materials.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TResourcePtr<KRG::Render::Material> );
            propertyInfo.m_size = sizeof( TVector<KRG::TResourcePtr<KRG::Render::Material>> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshName" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 2202398079;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshName;
            propertyInfo.m_offset = offsetof( KRG::Render::MeshResourceDescriptor, m_meshName );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_RENDER_API TTypeHelper<KRG::Render::MeshResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Render::MeshResourceDescriptor> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Render::MeshResourceDescriptor ), alignof( KRG::Render::MeshResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Render::MeshResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Render::MeshResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Render::MeshResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Render::MeshResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::MeshResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Render::MeshResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Render::MeshResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pDefaultTypeInstance )->~MeshResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Render::MeshResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Render::MeshResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->LoadResource( resourcePtr, requesterID );
                        }
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->UnloadResource( resourcePtr, requesterID );
                        }
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto const& resourcePtr : pActualType->m_materials )
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

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto const& resourcePtr : pActualType->m_materials )
                    {
                        KRG_ASSERT( !resourcePtr.IsLoading() );
                        if ( !resourcePtr.IsUnloaded() )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pType );
                    if ( propertyID == 2630520838 )
                    {
                        return KRG::Render::Material::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pType );

                    if ( arrayID == 2630520838 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_materials.size() )
                        {
                            pActualType->m_materials.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_materials[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor const*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        return pActualType->m_materials.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2630520838 )
                    {
                        return sizeof( KRG::TResourcePtr<KRG::Render::Material> );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.erase( pActualType->m_materials.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Render::MeshResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Render::MeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::MeshResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2562534183 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2630520838 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4270100199 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Render::MeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::MeshResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 2562534183 )
                    {
                        return pType->m_meshPath == pOtherType->m_meshPath;
                    }

                    if ( propertyID == 2630520838 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_materials.size() )
                            {
                                return false;
                            }

                            return pType->m_materials[arrayIdx] == pOtherType->m_materials[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_materials.size() != pOtherType->m_materials.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_materials.size(); i++ )
                            {
                                if( pType->m_materials[i] != pOtherType->m_materials[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 4270100199 )
                    {
                        return pType->m_meshName == pOtherType->m_meshName;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Render::MeshResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Render::MeshResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 2562534183 )
                    {
                        pActualType->m_meshPath = pDefaultType->m_meshPath;
                        return;
                    }

                    if ( propertyID == 2630520838 )
                    {
                        pActualType->m_materials = pDefaultType->m_materials;
                        return;
                    }

                    if ( propertyID == 4270100199 )
                    {
                        pActualType->m_meshName = pDefaultType->m_meshName;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Render::StaticMeshResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_RENDER_API void serialize( Archive& archive, KRG::Render::StaticMeshResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Render::MeshResourceDescriptor>( &type ), KRG_NVP( m_meshPath ), KRG_NVP( m_materials ), KRG_NVP( m_meshName ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshResourceDescriptor> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Render::StaticMeshResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Render::StaticMeshResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourcePath" );
            propertyInfo.m_parentTypeID = 1441604552;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshPath;
            propertyInfo.m_offset = offsetof( KRG::Render::StaticMeshResourceDescriptor, m_meshPath );
            propertyInfo.m_size = sizeof( KRG::ResourcePath );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materials" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 1441604552;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::Material" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materials;
            propertyInfo.m_offset = offsetof( KRG::Render::StaticMeshResourceDescriptor, m_materials );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materials.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materials.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TResourcePtr<KRG::Render::Material> );
            propertyInfo.m_size = sizeof( TVector<KRG::TResourcePtr<KRG::Render::Material>> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshName" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 1441604552;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshName;
            propertyInfo.m_offset = offsetof( KRG::Render::StaticMeshResourceDescriptor, m_meshName );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_RENDER_API TTypeHelper<KRG::Render::StaticMeshResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Render::StaticMeshResourceDescriptor> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Render::StaticMeshResourceDescriptor ), alignof( KRG::Render::StaticMeshResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Render::StaticMeshResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Render::StaticMeshResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Render::StaticMeshResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Render::StaticMeshResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Render::MeshResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::StaticMeshResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Render::StaticMeshResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Render::StaticMeshResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pDefaultTypeInstance )->~StaticMeshResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Render::StaticMeshResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Render::StaticMeshResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->LoadResource( resourcePtr, requesterID );
                        }
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->UnloadResource( resourcePtr, requesterID );
                        }
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto const& resourcePtr : pActualType->m_materials )
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

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto const& resourcePtr : pActualType->m_materials )
                    {
                        KRG_ASSERT( !resourcePtr.IsLoading() );
                        if ( !resourcePtr.IsUnloaded() )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pType );
                    if ( propertyID == 2630520838 )
                    {
                        return KRG::Render::Material::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pType );

                    if ( arrayID == 2630520838 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_materials.size() )
                        {
                            pActualType->m_materials.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_materials[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor const*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        return pActualType->m_materials.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2630520838 )
                    {
                        return sizeof( KRG::TResourcePtr<KRG::Render::Material> );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.erase( pActualType->m_materials.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Render::StaticMeshResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2562534183 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2630520838 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4270100199 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 2562534183 )
                    {
                        return pType->m_meshPath == pOtherType->m_meshPath;
                    }

                    if ( propertyID == 2630520838 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_materials.size() )
                            {
                                return false;
                            }

                            return pType->m_materials[arrayIdx] == pOtherType->m_materials[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_materials.size() != pOtherType->m_materials.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_materials.size(); i++ )
                            {
                                if( pType->m_materials[i] != pOtherType->m_materials[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 4270100199 )
                    {
                        return pType->m_meshName == pOtherType->m_meshName;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Render::StaticMeshResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 2562534183 )
                    {
                        pActualType->m_meshPath = pDefaultType->m_meshPath;
                        return;
                    }

                    if ( propertyID == 2630520838 )
                    {
                        pActualType->m_materials = pDefaultType->m_materials;
                        return;
                    }

                    if ( propertyID == 4270100199 )
                    {
                        pActualType->m_meshName = pDefaultType->m_meshName;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Render::SkeletalMeshResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_RENDER_API void serialize( Archive& archive, KRG::Render::SkeletalMeshResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Render::MeshResourceDescriptor>( &type ), KRG_NVP( m_meshPath ), KRG_NVP( m_materials ), KRG_NVP( m_meshName ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshResourceDescriptor> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Render::SkeletalMeshResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Render::SkeletalMeshResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourcePath" );
            propertyInfo.m_parentTypeID = 150806482;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshPath;
            propertyInfo.m_offset = offsetof( KRG::Render::SkeletalMeshResourceDescriptor, m_meshPath );
            propertyInfo.m_size = sizeof( KRG::ResourcePath );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_materials" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 150806482;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Render::Material" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_materials;
            propertyInfo.m_offset = offsetof( KRG::Render::SkeletalMeshResourceDescriptor, m_materials );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_materials.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_materials.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::TResourcePtr<KRG::Render::Material> );
            propertyInfo.m_size = sizeof( TVector<KRG::TResourcePtr<KRG::Render::Material>> );
            propertyInfo.m_flags.Set( 5 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshName" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 150806482;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshName;
            propertyInfo.m_offset = offsetof( KRG::Render::SkeletalMeshResourceDescriptor, m_meshName );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_RENDER_API TTypeHelper<KRG::Render::SkeletalMeshResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Render::SkeletalMeshResourceDescriptor> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Render::SkeletalMeshResourceDescriptor ), alignof( KRG::Render::SkeletalMeshResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Render::SkeletalMeshResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Render::SkeletalMeshResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Render::SkeletalMeshResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Render::SkeletalMeshResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Render::MeshResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Render::SkeletalMeshResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Render::SkeletalMeshResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Render::SkeletalMeshResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pDefaultTypeInstance )->~SkeletalMeshResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Render::SkeletalMeshResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Render::SkeletalMeshResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->LoadResource( resourcePtr, requesterID );
                        }
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pType );

                    for ( auto& resourcePtr : pActualType->m_materials )
                    {
                        if ( resourcePtr.IsValid() )
                        {
                            pResourceSystem->UnloadResource( resourcePtr, requesterID );
                        }
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto const& resourcePtr : pActualType->m_materials )
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

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto const& resourcePtr : pActualType->m_materials )
                    {
                        KRG_ASSERT( !resourcePtr.IsLoading() );
                        if ( !resourcePtr.IsUnloaded() )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pType );
                    if ( propertyID == 2630520838 )
                    {
                        return KRG::Render::Material::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pType );

                    if ( arrayID == 2630520838 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_materials.size() )
                        {
                            pActualType->m_materials.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_materials[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor const*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        return pActualType->m_materials.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2630520838 )
                    {
                        return sizeof( KRG::TResourcePtr<KRG::Render::Material> );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pTypeInstance );

                    if ( arrayID == 2630520838 )
                    {
                        pActualType->m_materials.erase( pActualType->m_materials.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Render::SkeletalMeshResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2562534183 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2630520838 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4270100199 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 2562534183 )
                    {
                        return pType->m_meshPath == pOtherType->m_meshPath;
                    }

                    if ( propertyID == 2630520838 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_materials.size() )
                            {
                                return false;
                            }

                            return pType->m_materials[arrayIdx] == pOtherType->m_materials[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_materials.size() != pOtherType->m_materials.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_materials.size(); i++ )
                            {
                                if( pType->m_materials[i] != pOtherType->m_materials[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 4270100199 )
                    {
                        return pType->m_meshName == pOtherType->m_meshName;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Render::SkeletalMeshResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 2562534183 )
                    {
                        pActualType->m_meshPath = pDefaultType->m_meshPath;
                        return;
                    }

                    if ( propertyID == 2630520838 )
                    {
                        pActualType->m_materials = pDefaultType->m_materials;
                        return;
                    }

                    if ( propertyID == 4270100199 )
                    {
                        pActualType->m_meshName = pDefaultType->m_meshName;
                        return;
                    }

                }

            };
        }
    }
}

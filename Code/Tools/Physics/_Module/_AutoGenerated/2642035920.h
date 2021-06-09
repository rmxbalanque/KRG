#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 2642035920

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Tools\Physics\ResourceCompilers\PhysicsMeshCompiler.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsMeshResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_PHYSICS_API void serialize( Archive& archive, KRG::Physics::PhysicsMeshResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_meshDataPath ), KRG_NVP( m_meshName ), KRG_NVP( m_isConvexMesh ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMeshResourceDescriptor> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Physics::PhysicsMeshResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Physics::PhysicsMeshResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshDataPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 1428198006;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshDataPath;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshResourceDescriptor, m_meshDataPath );
            propertyInfo.m_size = sizeof( KRG::DataPath );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_meshName" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 1428198006;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_meshName;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshResourceDescriptor, m_meshName );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_isConvexMesh" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "bool" );
            propertyInfo.m_parentTypeID = 1428198006;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_isConvexMesh;
            propertyInfo.m_offset = offsetof( KRG::Physics::PhysicsMeshResourceDescriptor, m_isConvexMesh );
            propertyInfo.m_size = sizeof( bool );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_PHYSICS_API TTypeHelper<KRG::Physics::PhysicsMeshResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Physics::PhysicsMeshResourceDescriptor> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Physics::PhysicsMeshResourceDescriptor ), alignof( KRG::Physics::PhysicsMeshResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Physics::PhysicsMeshResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMeshResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Physics::PhysicsMeshResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Physics::PhysicsMeshResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Physics::PhysicsMeshResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Physics::PhysicsMeshResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Physics::PhysicsMeshResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pDefaultTypeInstance )->~PhysicsMeshResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Physics::PhysicsMeshResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Physics::PhysicsMeshResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Physics::PhysicsMeshResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1114334994 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4270100199 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3042015119 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 1114334994 )
                    {
                        return pType->m_meshDataPath == pOtherType->m_meshDataPath;
                    }

                    if ( propertyID == 4270100199 )
                    {
                        return pType->m_meshName == pOtherType->m_meshName;
                    }

                    if ( propertyID == 3042015119 )
                    {
                        return pType->m_isConvexMesh == pOtherType->m_isConvexMesh;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Physics::PhysicsMeshResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 1114334994 )
                    {
                        pActualType->m_meshDataPath = pDefaultType->m_meshDataPath;
                        return;
                    }

                    if ( propertyID == 4270100199 )
                    {
                        pActualType->m_meshName = pDefaultType->m_meshName;
                        return;
                    }

                    if ( propertyID == 3042015119 )
                    {
                        pActualType->m_isConvexMesh = pDefaultType->m_isConvexMesh;
                        return;
                    }

                }

            };
        }
    }
}

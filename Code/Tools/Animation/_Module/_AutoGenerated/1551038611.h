#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 1551038611

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Tools\Animation\ResourceCompilers\AnimationSkeletonCompiler.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::SkeletonResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::SkeletonResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_skeletonDataPath ), KRG_NVP( m_skeletonRootBoneName ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::SkeletonResourceDescriptor> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::SkeletonResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Animation::SkeletonResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_skeletonDataPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 1130933343;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_skeletonDataPath;
            propertyInfo.m_offset = offsetof( KRG::Animation::SkeletonResourceDescriptor, m_skeletonDataPath );
            propertyInfo.m_size = sizeof( KRG::DataPath );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_skeletonRootBoneName" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 1130933343;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_skeletonRootBoneName;
            propertyInfo.m_offset = offsetof( KRG::Animation::SkeletonResourceDescriptor, m_skeletonRootBoneName );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::SkeletonResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::SkeletonResourceDescriptor> StaticTypeHelper;

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Animation::SkeletonResourceDescriptor ), alignof( KRG::Animation::SkeletonResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::SkeletonResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::SkeletonResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Animation::SkeletonResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Animation::SkeletonResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::SkeletonResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::SkeletonResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::SkeletonResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pDefaultTypeInstance )->~SkeletonResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::SkeletonResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::SkeletonResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor const*>( pTypeInstance );

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

                virtual void ClearArray( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( void* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( void* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( void const* pTypeInstance, void const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::SkeletonResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 764560898 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4050318144 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( void const* pTypeInstance, void const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 764560898 )
                    {
                        return pType->m_skeletonDataPath == pOtherType->m_skeletonDataPath;
                    }

                    if ( propertyID == 4050318144 )
                    {
                        return pType->m_skeletonRootBoneName == pOtherType->m_skeletonRootBoneName;
                    }

                    return false;
                }

                virtual void ResetToDefault( void* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::SkeletonResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 764560898 )
                    {
                        pActualType->m_skeletonDataPath = pDefaultType->m_skeletonDataPath;
                        return;
                    }

                    if ( propertyID == 4050318144 )
                    {
                        pActualType->m_skeletonRootBoneName = pDefaultType->m_skeletonRootBoneName;
                        return;
                    }

                }

            };
        }
    }
}

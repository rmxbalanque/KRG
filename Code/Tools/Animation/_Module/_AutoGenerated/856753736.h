#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 856753736

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Tools\Animation\ResourceCompilers\ResourceCompiler_AnimationClip.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::AnimationClipResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::AnimationClipResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_animationDataPath ), KRG_NVP( m_pSkeleton ), KRG_NVP( m_animationName ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationClipResourceDescriptor> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::AnimationClipResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Animation::AnimationClipResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_animationDataPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 272179067;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_animationDataPath;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationClipResourceDescriptor, m_animationDataPath );
            propertyInfo.m_size = sizeof( KRG::DataPath );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pSkeleton" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 272179067;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Animation::Skeleton" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pSkeleton;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationClipResourceDescriptor, m_pSkeleton );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Animation::Skeleton> );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_animationName" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 272179067;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_animationName;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationClipResourceDescriptor, m_animationName );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::AnimationClipResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::AnimationClipResourceDescriptor> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::AnimationClipResourceDescriptor ), alignof( KRG::Animation::AnimationClipResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::AnimationClipResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::AnimationClipResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Animation::AnimationClipResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Animation::AnimationClipResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationClipResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::AnimationClipResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::AnimationClipResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pDefaultTypeInstance )->~AnimationClipResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::AnimationClipResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::AnimationClipResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pType );

                    if ( pActualType->m_pSkeleton.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pSkeleton, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pType );

                    if ( pActualType->m_pSkeleton.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pSkeleton, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pSkeleton.IsValid() || pActualType->m_pSkeleton.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pSkeleton.IsUnloaded() || pActualType->m_pSkeleton.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_pSkeleton.IsLoading() );
                    if ( !pActualType->m_pSkeleton.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pType );
                    if ( propertyID == 1093290281 )
                    {
                        return KRG::Animation::Skeleton::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::AnimationClipResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1385787790 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1093290281 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1418688656 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 1385787790 )
                    {
                        return pType->m_animationDataPath == pOtherType->m_animationDataPath;
                    }

                    if ( propertyID == 1093290281 )
                    {
                        return pType->m_pSkeleton == pOtherType->m_pSkeleton;
                    }

                    if ( propertyID == 1418688656 )
                    {
                        return pType->m_animationName == pOtherType->m_animationName;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationClipResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 1385787790 )
                    {
                        pActualType->m_animationDataPath = pDefaultType->m_animationDataPath;
                        return;
                    }

                    if ( propertyID == 1093290281 )
                    {
                        pActualType->m_pSkeleton = pDefaultType->m_pSkeleton;
                        return;
                    }

                    if ( propertyID == 1418688656 )
                    {
                        pActualType->m_animationName = pDefaultType->m_animationName;
                        return;
                    }

                }

            };
        }
    }
}

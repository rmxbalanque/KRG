#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3002288438

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "d:\Kruger\Code\Tools\Animation\ResourceCompilers\ResourceCompiler_AnimationGraph.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::AnimationGraphVariationResourceDescriptor
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::AnimationGraphVariationResourceDescriptor& type )
    {
        archive( cereal::base_class<KRG::Resource::ResourceDescriptor>( &type ), KRG_NVP( m_graphDataPath ), KRG_NVP( m_variationID ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationGraphVariationResourceDescriptor> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::AnimationGraphVariationResourceDescriptor const* pActualDefaultTypeInstance = ( KRG::Animation::AnimationGraphVariationResourceDescriptor const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_graphDataPath" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::DataPath" );
            propertyInfo.m_parentTypeID = 1714462230;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_graphDataPath;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationGraphVariationResourceDescriptor, m_graphDataPath );
            propertyInfo.m_size = sizeof( KRG::DataPath );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_variationID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 1714462230;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_variationID;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationGraphVariationResourceDescriptor, m_variationID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::AnimationGraphVariationResourceDescriptor> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::AnimationGraphVariationResourceDescriptor> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::AnimationGraphVariationResourceDescriptor ), alignof( KRG::Animation::AnimationGraphVariationResourceDescriptor ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::AnimationGraphVariationResourceDescriptor;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::AnimationGraphVariationResourceDescriptor" );
                    typeInfo.m_size = sizeof( KRG::Animation::AnimationGraphVariationResourceDescriptor );
                    typeInfo.m_alignment = alignof( KRG::Animation::AnimationGraphVariationResourceDescriptor );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Resource::ResourceDescriptor::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationGraphVariationResourceDescriptor> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::AnimationGraphVariationResourceDescriptor::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::AnimationGraphVariationResourceDescriptor" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pDefaultTypeInstance )->~AnimationGraphVariationResourceDescriptor();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::AnimationGraphVariationResourceDescriptor>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::AnimationGraphVariationResourceDescriptor();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::AnimationGraphVariationResourceDescriptor::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3260057806 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 174645900 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor const*>( pOtherTypeInstance );

                    if ( propertyID == 3260057806 )
                    {
                        return pType->m_graphDataPath == pOtherType->m_graphDataPath;
                    }

                    if ( propertyID == 174645900 )
                    {
                        return pType->m_variationID == pOtherType->m_variationID;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationGraphVariationResourceDescriptor*>( pTypeInstance );

                    if ( propertyID == 3260057806 )
                    {
                        pActualType->m_graphDataPath = pDefaultType->m_graphDataPath;
                        return;
                    }

                    if ( propertyID == 174645900 )
                    {
                        pActualType->m_variationID = pDefaultType->m_variationID;
                        return;
                    }

                }

            };
        }
    }
}

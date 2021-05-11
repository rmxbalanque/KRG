#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 436129338

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "D:\Kruger\Code\Engine\Animation\Components\AnimationPlayerComponent.h"

//-------------------------------------------------------------------------
// Enum Helper: KRG::Animation::AnimationPlayerComponent::PlayMode
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            class EnumHelper_KRG_Animation_AnimationPlayerComponent_PlayMode
            {
            public:

                static void RegisterEnum( TypeSystem::TypeRegistry& typeRegistry );
                static void UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry );
            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::AnimationPlayerComponent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_ENGINE_ANIMATION_API void serialize( Archive& archive, KRG::Animation::AnimationPlayerComponent& type )
    {
        archive( cereal::base_class<KRG::Animation::AnimationComponent>( &type ), KRG_NVP( m_pAnimation ), KRG_NVP( m_playMode ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationPlayerComponent> >( void const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::AnimationPlayerComponent const* pActualDefaultTypeInstance = ( KRG::Animation::AnimationPlayerComponent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_pAnimation" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::TResourcePtr" );
            propertyInfo.m_parentTypeID = 3997956009;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "KRG::Animation::AnimationClip" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_pAnimation;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationPlayerComponent, m_pAnimation );
            propertyInfo.m_size = sizeof( KRG::TResourcePtr<KRG::Animation::AnimationClip> );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_playMode" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Animation::AnimationPlayerComponent::PlayMode" );
            propertyInfo.m_parentTypeID = 3997956009;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_playMode;
            propertyInfo.m_offset = offsetof( KRG::Animation::AnimationPlayerComponent, m_playMode );
            propertyInfo.m_size = sizeof( KRG::Animation::AnimationPlayerComponent::PlayMode );
            propertyInfo.m_flags.Set( 4 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_ENGINE_ANIMATION_API TTypeHelper<KRG::Animation::AnimationPlayerComponent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::AnimationPlayerComponent> StaticTypeHelper;

                static void const* s_pDefaultTypeInstancePtr;

            public:

                virtual void const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = KRG::Alloc( sizeof( KRG::Animation::AnimationPlayerComponent ), alignof( KRG::Animation::AnimationPlayerComponent ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::AnimationPlayerComponent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::AnimationPlayerComponent" );
                    typeInfo.m_size = sizeof( KRG::Animation::AnimationPlayerComponent );
                    typeInfo.m_alignment = alignof( KRG::Animation::AnimationPlayerComponent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::EntityComponent );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::AnimationComponent::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::AnimationPlayerComponent> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::AnimationPlayerComponent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::AnimationPlayerComponent" );
                    typeRegistry.UnregisterType( ID );

                    void*& pDefaultTypeInstance = const_cast<void*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::AnimationPlayerComponent*>( pDefaultTypeInstance )->~AnimationPlayerComponent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual void* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::AnimationPlayerComponent>();
                }

                virtual void CreateTypeInPlace( void* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::AnimationPlayerComponent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent*>( pType );

                    if ( pActualType->m_pAnimation.IsValid() )
                    {
                        pResourceSystem->LoadResource( pActualType->m_pAnimation, requesterID );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, void* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent*>( pType );

                    if ( pActualType->m_pAnimation.IsValid() )
                    {
                        pResourceSystem->UnloadResource( pActualType->m_pAnimation, requesterID );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    if ( !pActualType->m_pAnimation.IsValid() || pActualType->m_pAnimation.HasLoadingFailed() )
                    {
                        status = LoadingStatus::Failed;
                    }
                    else if ( pActualType->m_pAnimation.IsUnloaded() || pActualType->m_pAnimation.IsLoading() )
                    {
                        return LoadingStatus::Loading;
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( void* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    KRG_ASSERT( !pActualType->m_pAnimation.IsLoading() );
                    if ( !pActualType->m_pAnimation.IsUnloaded() )
                    {
                        return LoadingStatus::Unloading;
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual Byte* GetArrayElementDataPtr( void* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent*>( pType );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( void const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent const*>( pTypeInstance );
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( void const* pTypeInstance, uint32 arrayID ) const override final
                {
                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( void* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent*>( pType );
                    if ( propertyID == 1571632376 )
                    {
                        return KRG::Animation::AnimationClip::GetStaticResourceTypeID();
                    }

                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual bool IsDefaultValue( void const* pValueInstance, uint32 propertyID, size_t arrayIdx = InvalidIndex ) const override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::AnimationPlayerComponent const*>( GetDefaultTypeInstancePtr() );
                    if ( propertyID == 1571632376 )
                    {
                        return *reinterpret_cast<KRG::TResourcePtr<KRG::Animation::AnimationClip> const*>( pValueInstance ) == pDefaultType->m_pAnimation;
                    }

                    if ( propertyID == 1968318620 )
                    {
                        return *reinterpret_cast<KRG::Animation::AnimationPlayerComponent::PlayMode const*>( pValueInstance ) == pDefaultType->m_playMode;
                    }

                    return false;
                }

            };
        }
    }
}

#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 862053867

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "E:\Git\KRG\Code\Tools\Animation\GraphEditor\ToolsGraph\AnimationToolsGraph_StateMachineNode.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent& type )
    {
        archive( cereal::base_class<KRG::IRegisteredType>( &type ), KRG_NVP( m_ID ), KRG_NVP( m_timeValue ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 53885560;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent, m_ID );
            propertyInfo.m_size = sizeof( KRG::StringID );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_timeValue" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Seconds" );
            propertyInfo.m_parentTypeID = 53885560;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_timeValue;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent, m_timeValue );
            propertyInfo.m_size = sizeof( KRG::Seconds );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent ), alignof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::IRegisteredType::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pDefaultTypeInstance )->~TimedStateEvent();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pType );

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pType );

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pType );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const*>( pTypeInstance );

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
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pTypeInstance );

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1196316147 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const*>( pOtherTypeInstance );

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 1196316147 )
                    {
                        return pType->m_timeValue == pOtherType->m_timeValue;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent*>( pTypeInstance );

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 1196316147 )
                    {
                        pActualType->m_timeValue = pDefaultType->m_timeValue;
                        return;
                    }

                }

            };
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::Tools_StateBaseNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::Tools_StateBaseNode& type )
    {
        archive( cereal::base_class<KRG::GraphEditor::SM::State>( &type ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ), KRG_NVP( m_name ), KRG_NVP( m_entryEvents ), KRG_NVP( m_executeEvents ), KRG_NVP( m_exitEvents ), KRG_NVP( m_timeRemainingEvents ), KRG_NVP( m_timeElapsedEvents ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_name" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_entryEvents" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_executeEvents" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_exitEvents" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::StringID" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_timeRemainingEvents" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_timeElapsedEvents" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent" );
            propertyInfo.m_parentTypeID = 235598726;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = nullptr;
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_StateBaseNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::Tools_StateBaseNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::Tools_StateBaseNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 
                    typeInfo.m_metadata.SetFlag( ETypeInfoMetaData::Abstract );

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::GraphEditor::SM::State::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Tools_StateBaseNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::Tools_StateBaseNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_StateBaseNode" );
                    typeRegistry.UnregisterType( ID );

                }

                virtual IRegisteredType* CreateType() const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract type!
                    return nullptr;
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_HALT(); // Error! Trying to instantiate an abstract type!
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pType );

                    for ( auto& propertyValue : pActualType->m_timeRemainingEvents )
                    {
                        KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                    for ( auto& propertyValue : pActualType->m_timeElapsedEvents )
                    {
                        KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pType );

                    for ( auto& propertyValue : pActualType->m_timeRemainingEvents )
                    {
                        KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                    for ( auto& propertyValue : pActualType->m_timeElapsedEvents )
                    {
                        KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto& propertyValue : pActualType->m_timeRemainingEvents )
                    {
                        status = KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    for ( auto& propertyValue : pActualType->m_timeElapsedEvents )
                    {
                        status = KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto& propertyValue : pActualType->m_timeRemainingEvents )
                    {
                        status = KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    for ( auto& propertyValue : pActualType->m_timeElapsedEvents )
                    {
                        status = KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pType );

                    if ( arrayID == 2442522957 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_entryEvents.size() )
                        {
                            pActualType->m_entryEvents.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_entryEvents[arrayIdx];
                    }

                    if ( arrayID == 3643044983 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_executeEvents.size() )
                        {
                            pActualType->m_executeEvents.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_executeEvents[arrayIdx];
                    }

                    if ( arrayID == 3626258234 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_exitEvents.size() )
                        {
                            pActualType->m_exitEvents.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_exitEvents[arrayIdx];
                    }

                    if ( arrayID == 2974062592 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_timeRemainingEvents.size() )
                        {
                            pActualType->m_timeRemainingEvents.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_timeRemainingEvents[arrayIdx];
                    }

                    if ( arrayID == 2604687028 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_timeElapsedEvents.size() )
                        {
                            pActualType->m_timeElapsedEvents.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_timeElapsedEvents[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode const*>( pTypeInstance );

                    if ( arrayID == 2442522957 )
                    {
                        return pActualType->m_entryEvents.size();
                    }

                    if ( arrayID == 3643044983 )
                    {
                        return pActualType->m_executeEvents.size();
                    }

                    if ( arrayID == 3626258234 )
                    {
                        return pActualType->m_exitEvents.size();
                    }

                    if ( arrayID == 2974062592 )
                    {
                        return pActualType->m_timeRemainingEvents.size();
                    }

                    if ( arrayID == 2604687028 )
                    {
                        return pActualType->m_timeElapsedEvents.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2442522957 )
                    {
                        return sizeof( KRG::StringID );
                    }

                    if ( arrayID == 3643044983 )
                    {
                        return sizeof( KRG::StringID );
                    }

                    if ( arrayID == 3626258234 )
                    {
                        return sizeof( KRG::StringID );
                    }

                    if ( arrayID == 2974062592 )
                    {
                        return sizeof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent );
                    }

                    if ( arrayID == 2604687028 )
                    {
                        return sizeof( KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pTypeInstance );

                    if ( arrayID == 2442522957 )
                    {
                        pActualType->m_entryEvents.clear();
                        return;
                    }

                    if ( arrayID == 3643044983 )
                    {
                        pActualType->m_executeEvents.clear();
                        return;
                    }

                    if ( arrayID == 3626258234 )
                    {
                        pActualType->m_exitEvents.clear();
                        return;
                    }

                    if ( arrayID == 2974062592 )
                    {
                        pActualType->m_timeRemainingEvents.clear();
                        return;
                    }

                    if ( arrayID == 2604687028 )
                    {
                        pActualType->m_timeElapsedEvents.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pTypeInstance );

                    if ( arrayID == 2442522957 )
                    {
                        pActualType->m_entryEvents.emplace_back();
                        return;
                    }

                    if ( arrayID == 3643044983 )
                    {
                        pActualType->m_executeEvents.emplace_back();
                        return;
                    }

                    if ( arrayID == 3626258234 )
                    {
                        pActualType->m_exitEvents.emplace_back();
                        return;
                    }

                    if ( arrayID == 2974062592 )
                    {
                        pActualType->m_timeRemainingEvents.emplace_back();
                        return;
                    }

                    if ( arrayID == 2604687028 )
                    {
                        pActualType->m_timeElapsedEvents.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pTypeInstance );

                    if ( arrayID == 2442522957 )
                    {
                        pActualType->m_entryEvents.erase( pActualType->m_entryEvents.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 3643044983 )
                    {
                        pActualType->m_executeEvents.erase( pActualType->m_executeEvents.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 3626258234 )
                    {
                        pActualType->m_exitEvents.erase( pActualType->m_exitEvents.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 2974062592 )
                    {
                        pActualType->m_timeRemainingEvents.erase( pActualType->m_timeRemainingEvents.begin() + arrayIdx );
                        return;
                    }

                    if ( arrayID == 2604687028 )
                    {
                        pActualType->m_timeElapsedEvents.erase( pActualType->m_timeElapsedEvents.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::Tools_StateBaseNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2454640797 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 4230898639 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3460589943 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2442522957 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3643044983 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3626258234 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2974062592 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2604687028 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode const*>( pOtherTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        return pType->m_canvasPosition == pOtherType->m_canvasPosition;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        return pType->m_ID == pOtherType->m_ID;
                    }

                    if ( propertyID == 3460589943 )
                    {
                        return pType->m_name == pOtherType->m_name;
                    }

                    if ( propertyID == 2442522957 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_entryEvents.size() )
                            {
                                return false;
                            }

                            return pType->m_entryEvents[arrayIdx] == pOtherType->m_entryEvents[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_entryEvents.size() != pOtherType->m_entryEvents.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_entryEvents.size(); i++ )
                            {
                                if( pType->m_entryEvents[i] != pOtherType->m_entryEvents[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 3643044983 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_executeEvents.size() )
                            {
                                return false;
                            }

                            return pType->m_executeEvents[arrayIdx] == pOtherType->m_executeEvents[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_executeEvents.size() != pOtherType->m_executeEvents.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_executeEvents.size(); i++ )
                            {
                                if( pType->m_executeEvents[i] != pOtherType->m_executeEvents[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 3626258234 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_exitEvents.size() )
                            {
                                return false;
                            }

                            return pType->m_exitEvents[arrayIdx] == pOtherType->m_exitEvents[arrayIdx];
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_exitEvents.size() != pOtherType->m_exitEvents.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_exitEvents.size(); i++ )
                            {
                                if( pType->m_exitEvents[i] != pOtherType->m_exitEvents[i] )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 2974062592 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_timeRemainingEvents.size() )
                            {
                                return false;
                            }

                            return KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_timeRemainingEvents[arrayIdx], &pOtherType->m_timeRemainingEvents[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_timeRemainingEvents.size() != pOtherType->m_timeRemainingEvents.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_timeRemainingEvents.size(); i++ )
                            {
                                if( !KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_timeRemainingEvents[i], &pOtherType->m_timeRemainingEvents[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    if ( propertyID == 2604687028 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_timeElapsedEvents.size() )
                            {
                                return false;
                            }

                            return KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_timeElapsedEvents[arrayIdx], &pOtherType->m_timeElapsedEvents[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_timeElapsedEvents.size() != pOtherType->m_timeElapsedEvents.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_timeElapsedEvents.size(); i++ )
                            {
                                if( !KRG::Animation::Graph::Tools_StateBaseNode::TimedStateEvent::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_timeElapsedEvents[i], &pOtherType->m_timeElapsedEvents[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_StateBaseNode*>( pTypeInstance );

                    if ( propertyID == 2454640797 )
                    {
                        pActualType->m_canvasPosition = pDefaultType->m_canvasPosition;
                        return;
                    }

                    if ( propertyID == 4230898639 )
                    {
                        pActualType->m_ID = pDefaultType->m_ID;
                        return;
                    }

                    if ( propertyID == 3460589943 )
                    {
                        pActualType->m_name = pDefaultType->m_name;
                        return;
                    }

                    if ( propertyID == 2442522957 )
                    {
                        pActualType->m_entryEvents = pDefaultType->m_entryEvents;
                        return;
                    }

                    if ( propertyID == 3643044983 )
                    {
                        pActualType->m_executeEvents = pDefaultType->m_executeEvents;
                        return;
                    }

                    if ( propertyID == 3626258234 )
                    {
                        pActualType->m_exitEvents = pDefaultType->m_exitEvents;
                        return;
                    }

                    if ( propertyID == 2974062592 )
                    {
                        pActualType->m_timeRemainingEvents = pDefaultType->m_timeRemainingEvents;
                        return;
                    }

                    if ( propertyID == 2604687028 )
                    {
                        pActualType->m_timeElapsedEvents = pDefaultType->m_timeElapsedEvents;
                        return;
                    }

                }

            };
        }
    }
}

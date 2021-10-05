#pragma once

//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

// HeaderID: 3696221119

#include "../API.h"
#include "System/TypeSystem/TypeRegistry.h"
#include "System/TypeSystem/ITypeHelper.h"
#include "System/Core/Serialization/Serialization.h"
#include "d:\Kruger\Code\Tools\Animation\ToolsGraph\Nodes\AnimationToolsNode_AnimationClip.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::Graph::Tools_AnimationClipNode
//-------------------------------------------------------------------------

namespace KRG
{
    template<class Archive>
    KRG_TOOLS_ANIMATION_API void serialize( Archive& archive, KRG::Animation::Graph::Tools_AnimationClipNode& type )
    {
        archive( cereal::base_class<KRG::Animation::Graph::Tools_DataSlotNode>( &type ), KRG_NVP( m_defaultResourceID ), KRG_NVP( m_overrides ), KRG_NVP( m_canvasPosition ), KRG_NVP( m_ID ), KRG_NVP( m_name ), KRG_NVP( m_sampleRootMotion ), KRG_NVP( m_allowLooping ) );
    }

    //-------------------------------------------------------------------------

    namespace TypeSystem
    {
        template<>
        void TypeInfo::RegisterProperties< TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Tools_AnimationClipNode> >( IRegisteredType const* pDefaultTypeInstance )
        {
            KRG_ASSERT( pDefaultTypeInstance != nullptr );
            KRG::Animation::Graph::Tools_AnimationClipNode const* pActualDefaultTypeInstance = ( KRG::Animation::Graph::Tools_AnimationClipNode const* ) pDefaultTypeInstance;

            PropertyInfo propertyInfo;

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_defaultResourceID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::ResourceID" );
            propertyInfo.m_parentTypeID = 1606986776;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_defaultResourceID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_AnimationClipNode, m_defaultResourceID );
            propertyInfo.m_size = sizeof( KRG::ResourceID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_overrides" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue" );
            propertyInfo.m_parentTypeID = 1606986776;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_overrides;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_AnimationClipNode, m_overrides );
            propertyInfo.m_pDefaultArrayData = pActualDefaultTypeInstance->m_overrides.data();
            propertyInfo.m_arraySize = (int32) pActualDefaultTypeInstance->m_overrides.size();
            propertyInfo.m_arrayElementSize = (int32) sizeof( KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue );
            propertyInfo.m_size = sizeof( TVector<KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue> );
            propertyInfo.m_flags.Set( 36 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_canvasPosition" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::Float2" );
            propertyInfo.m_parentTypeID = 1606986776;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_canvasPosition;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_AnimationClipNode, m_canvasPosition );
            propertyInfo.m_size = sizeof( KRG::Float2 );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_ID" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::UUID" );
            propertyInfo.m_parentTypeID = 1606986776;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_ID;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_AnimationClipNode, m_ID );
            propertyInfo.m_size = sizeof( KRG::UUID );
            propertyInfo.m_flags.Set( 0 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_name" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "KRG::String" );
            propertyInfo.m_parentTypeID = 1606986776;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_name;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_AnimationClipNode, m_name );
            propertyInfo.m_size = sizeof( KRG::String );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_sampleRootMotion" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "bool" );
            propertyInfo.m_parentTypeID = 1606986776;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_sampleRootMotion;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_AnimationClipNode, m_sampleRootMotion );
            propertyInfo.m_size = sizeof( bool );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );

            //-------------------------------------------------------------------------

            propertyInfo.m_ID = StringID( "m_allowLooping" );
            propertyInfo.m_typeID = TypeSystem::TypeID( "bool" );
            propertyInfo.m_parentTypeID = 1606986776;
            propertyInfo.m_templateArgumentTypeID = TypeSystem::TypeID( "" );
            propertyInfo.m_pDefaultValue = &pActualDefaultTypeInstance->m_allowLooping;
            propertyInfo.m_offset = offsetof( KRG::Animation::Graph::Tools_AnimationClipNode, m_allowLooping );
            propertyInfo.m_size = sizeof( bool );
            propertyInfo.m_flags.Set( 1 );
            m_properties.emplace_back( propertyInfo );
            m_propertyMap.insert( TPair<StringID, int32>( propertyInfo.m_ID, int32( m_properties.size() ) - 1 ) );
        }

        //-------------------------------------------------------------------------

        namespace TypeHelpers
        {
            template<>
            class KRG_TOOLS_ANIMATION_API TTypeHelper<KRG::Animation::Graph::Tools_AnimationClipNode> final : public ITypeHelper
            {
                static TTypeHelper<KRG::Animation::Graph::Tools_AnimationClipNode> StaticTypeHelper;

                static IRegisteredType const* s_pDefaultTypeInstancePtr;

            public:

                virtual IRegisteredType const* GetDefaultTypeInstancePtr() const override { return s_pDefaultTypeInstancePtr; }

                static void RegisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    pDefaultTypeInstance = (IRegisteredType*) KRG::Alloc( sizeof( KRG::Animation::Graph::Tools_AnimationClipNode ), alignof( KRG::Animation::Graph::Tools_AnimationClipNode ) );
                    new ( pDefaultTypeInstance ) KRG::Animation::Graph::Tools_AnimationClipNode;

                    TypeSystem::TypeInfo typeInfo;
                    typeInfo.m_ID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_AnimationClipNode" );
                    typeInfo.m_size = sizeof( KRG::Animation::Graph::Tools_AnimationClipNode );
                    typeInfo.m_alignment = alignof( KRG::Animation::Graph::Tools_AnimationClipNode );
                    typeInfo.m_pTypeHelper = &StaticTypeHelper; 

                    // Parent Types 
                    //-------------------------------------------------------------------------

                    TypeSystem::TypeInfo const* pParentType = nullptr;

                    pParentType = KRG::Animation::Graph::Tools_DataSlotNode::s_pTypeInfo;
                    KRG_ASSERT( pParentType != nullptr );
                    typeInfo.m_parentTypes.push_back( pParentType );

                    // Register properties and type
                    //-------------------------------------------------------------------------

                    typeInfo.RegisterProperties< KRG::TypeSystem::TypeHelpers::TTypeHelper<KRG::Animation::Graph::Tools_AnimationClipNode> >( s_pDefaultTypeInstancePtr );
                    KRG::Animation::Graph::Tools_AnimationClipNode::s_pTypeInfo = typeRegistry.RegisterType( typeInfo );
                }

                static void UnregisterType( TypeSystem::TypeRegistry& typeRegistry )
                {
                    auto const ID = TypeSystem::TypeID( "KRG::Animation::Graph::Tools_AnimationClipNode" );
                    typeRegistry.UnregisterType( ID );

                    IRegisteredType*& pDefaultTypeInstance = const_cast<IRegisteredType*&>( s_pDefaultTypeInstancePtr );
                    reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pDefaultTypeInstance )->~Tools_AnimationClipNode();
                    KRG::Free( pDefaultTypeInstance );
                }

                virtual IRegisteredType* CreateType() const override final
                {
                    return KRG::New<KRG::Animation::Graph::Tools_AnimationClipNode>();
                }

                virtual void CreateTypeInPlace( IRegisteredType* pAllocatedMemory ) const override final
                {
                    KRG_ASSERT( pAllocatedMemory != nullptr );
                    new( pAllocatedMemory ) KRG::Animation::Graph::Tools_AnimationClipNode();
                }

                virtual void LoadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pType );

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->LoadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual void UnloadResources( Resource::ResourceSystem* pResourceSystem, UUID const& requesterID, IRegisteredType* pType ) const override final
                {
                    KRG_ASSERT( pResourceSystem != nullptr );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pType );

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->UnloadResources( pResourceSystem, requesterID, &propertyValue );
                    }

                }

                virtual LoadingStatus GetResourceLoadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pType );
                    LoadingStatus status = LoadingStatus::Loaded;

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        status = KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( &propertyValue );
                        if ( status == LoadingStatus::Loading )
                        {
                            return LoadingStatus::Loading;
                        }
                    }

                    return status;
                }

                virtual LoadingStatus GetResourceUnloadingStatus( IRegisteredType* pType ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pType );
                    LoadingStatus status = LoadingStatus::Unloading;

                    for ( auto& propertyValue : pActualType->m_overrides )
                    {
                        status = KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->GetResourceUnloadingStatus( &propertyValue );
                        if ( status != LoadingStatus::Unloaded )
                        {
                            return LoadingStatus::Unloading;
                        }
                    }

                    return LoadingStatus::Unloaded;
                }

                virtual ResourceTypeID GetExpectedResourceTypeForProperty( IRegisteredType* pType, uint32 propertyID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pType );
                    // We should never get here since we are asking for a resource type of an invalid property
                    KRG_UNREACHABLE_CODE();
                    return ResourceTypeID();
                }

                virtual Byte* GetArrayElementDataPtr( IRegisteredType* pType, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pType );

                    if ( arrayID == 2183863778 )
                    {
                        if ( ( arrayIdx + 1 ) >= pActualType->m_overrides.size() )
                        {
                            pActualType->m_overrides.resize( arrayIdx + 1 );
                        }

                        return (Byte*) &pActualType->m_overrides[arrayIdx];
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return nullptr;
                }

                virtual size_t GetArraySize( IRegisteredType const* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode const*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        return pActualType->m_overrides.size();
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual size_t GetArrayElementSize( uint32 arrayID ) const override final
                {
                    if ( arrayID == 2183863778 )
                    {
                        return sizeof( KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue );
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                    return 0;
                }

                virtual void ClearArray( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        pActualType->m_overrides.clear();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void AddArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        pActualType->m_overrides.emplace_back();
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual void RemoveArrayElement( IRegisteredType* pTypeInstance, uint32 arrayID, size_t arrayIdx ) const override final
                {
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pTypeInstance );

                    if ( arrayID == 2183863778 )
                    {
                        pActualType->m_overrides.erase( pActualType->m_overrides.begin() + arrayIdx );
                        return;
                    }

                    // We should never get here since we are asking for a ptr to an invalid property
                    KRG_UNREACHABLE_CODE();
                }

                virtual bool AreAllPropertyValuesEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance ) const override final
                {
                    auto pTypeHelper = KRG::Animation::Graph::Tools_AnimationClipNode::s_pTypeInfo->m_pTypeHelper;
                    auto pType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode const*>( pOtherTypeInstance );

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 1592680383 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2183863778 ) )
                    {
                       return false;
                    }

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

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 3134434976 ) )
                    {
                       return false;
                    }

                    if( !pTypeHelper->IsPropertyValueEqual( pType, pOtherType, 2186242510 ) )
                    {
                       return false;
                    }

                    return true;
                }

                virtual bool IsPropertyValueEqual( IRegisteredType const* pTypeInstance, IRegisteredType const* pOtherTypeInstance, uint32 propertyID, int32 arrayIdx = InvalidIndex ) const override final
                {
                    auto pType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode const*>( pTypeInstance );
                    auto pOtherType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode const*>( pOtherTypeInstance );

                    if ( propertyID == 1592680383 )
                    {
                        return pType->m_defaultResourceID == pOtherType->m_defaultResourceID;
                    }

                    if ( propertyID == 2183863778 )
                    {
                        // Compare array elements
                        if ( arrayIdx != InvalidIndex )
                        {
                            if ( arrayIdx >= pOtherType->m_overrides.size() )
                            {
                                return false;
                            }

                            return KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_overrides[arrayIdx], &pOtherType->m_overrides[arrayIdx] );
                        }
                        else // Compare entire array contents
                        {
                            if ( pType->m_overrides.size() != pOtherType->m_overrides.size() )
                            {
                                return false;
                            }

                            for ( size_t i = 0; i < pType->m_overrides.size(); i++ )
                            {
                                if( !KRG::Animation::Graph::Tools_DataSlotNode::OverrideValue::s_pTypeInfo->m_pTypeHelper->AreAllPropertyValuesEqual( &pType->m_overrides[i], &pOtherType->m_overrides[i] ) )
                                {
                                    return false;
                                }
                            }

                            return true;
                        }
                    }

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

                    if ( propertyID == 3134434976 )
                    {
                        return pType->m_sampleRootMotion == pOtherType->m_sampleRootMotion;
                    }

                    if ( propertyID == 2186242510 )
                    {
                        return pType->m_allowLooping == pOtherType->m_allowLooping;
                    }

                    return false;
                }

                virtual void ResetToDefault( IRegisteredType* pTypeInstance, uint32 propertyID ) override final
                {
                    auto pDefaultType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode const*>( GetDefaultTypeInstancePtr() );
                    auto pActualType = reinterpret_cast<KRG::Animation::Graph::Tools_AnimationClipNode*>( pTypeInstance );

                    if ( propertyID == 1592680383 )
                    {
                        pActualType->m_defaultResourceID = pDefaultType->m_defaultResourceID;
                        return;
                    }

                    if ( propertyID == 2183863778 )
                    {
                        pActualType->m_overrides = pDefaultType->m_overrides;
                        return;
                    }

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

                    if ( propertyID == 3134434976 )
                    {
                        pActualType->m_sampleRootMotion = pDefaultType->m_sampleRootMotion;
                        return;
                    }

                    if ( propertyID == 2186242510 )
                    {
                        pActualType->m_allowLooping = pDefaultType->m_allowLooping;
                        return;
                    }

                }

            };
        }
    }
}

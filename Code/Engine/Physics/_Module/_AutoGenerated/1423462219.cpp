//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Engine\Physics\_Module\_AutoGenerated\1423462219.h"

//-------------------------------------------------------------------------
// Enum Helper: KRG::Physics::ActorType
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            void EnumHelper_KRG_Physics_ActorType::RegisterEnum( TypeSystem::TypeRegistry& typeRegistry )
            {
                TypeSystem::TypeInfo typeInfo;
                typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::ActorType" );
                typeInfo.m_size = sizeof( KRG::Physics::ActorType );
                typeInfo.m_alignment = alignof( KRG::Physics::ActorType );
                typeRegistry.RegisterType( typeInfo );

                TypeSystem::EnumInfo enumInfo;
                enumInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::ActorType" );
                enumInfo.m_underlyingType = TypeSystem::CoreTypes::Int32;
                enumInfo.m_constants.insert( TPair<StringID, int64>( StringID( "Static" ), 0 ) );
                enumInfo.m_constants.insert( TPair<StringID, int64>( StringID( "Kinematic" ), 2 ) );
                enumInfo.m_constants.insert( TPair<StringID, int64>( StringID( "Dynamic" ), 1 ) );

                typeRegistry.RegisterEnum( enumInfo );
            }

            void EnumHelper_KRG_Physics_ActorType::UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry )
            {
                auto const ID = TypeSystem::TypeID( "KRG::Physics::ActorType" );
                typeRegistry.UnregisterType( ID );
                typeRegistry.UnregisterEnum( ID );
            }
        }
    }
}

//-------------------------------------------------------------------------
// Enum Helper: KRG::Physics::ShapeType
//-------------------------------------------------------------------------

namespace KRG
{
    namespace TypeSystem
    {
        namespace EnumHelpers
        {
            void EnumHelper_KRG_Physics_ShapeType::RegisterEnum( TypeSystem::TypeRegistry& typeRegistry )
            {
                TypeSystem::TypeInfo typeInfo;
                typeInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
                typeInfo.m_size = sizeof( KRG::Physics::ShapeType );
                typeInfo.m_alignment = alignof( KRG::Physics::ShapeType );
                typeRegistry.RegisterType( typeInfo );

                TypeSystem::EnumInfo enumInfo;
                enumInfo.m_ID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
                enumInfo.m_underlyingType = TypeSystem::CoreTypes::Int32;
                enumInfo.m_constants.insert( TPair<StringID, int64>( StringID( "QueryOnly" ), 2 ) );
                enumInfo.m_constants.insert( TPair<StringID, int64>( StringID( "SimulationAndQuery" ), 0 ) );
                enumInfo.m_constants.insert( TPair<StringID, int64>( StringID( "SimulationOnly" ), 1 ) );

                typeRegistry.RegisterEnum( enumInfo );
            }

            void EnumHelper_KRG_Physics_ShapeType::UnregisterEnum( TypeSystem::TypeRegistry& typeRegistry )
            {
                auto const ID = TypeSystem::TypeID( "KRG::Physics::ShapeType" );
                typeRegistry.UnregisterType( ID );
                typeRegistry.UnregisterEnum( ID );
            }
        }
    }
}

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsComponent
//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* KRG::Physics::PhysicsComponent::s_pTypeInfo = nullptr;
    namespace TypeSystem
    {
        namespace TypeHelpers
        {
            IRegisteredType const* TTypeHelper<KRG::Physics::PhysicsComponent>::s_pDefaultTypeInstancePtr = nullptr;

            TTypeHelper<KRG::Physics::PhysicsComponent> TTypeHelper<KRG::Physics::PhysicsComponent>::StaticTypeHelper;
        }
    }

    void KRG::Physics::PhysicsComponent::Load( EntityModel::LoadingContext const& context, UUID requesterID )
    {
        KRG::Physics::PhysicsComponent::s_pTypeInfo->m_pTypeHelper->LoadResources( context.m_pResourceSystem, requesterID, this );
        m_status = Status::Loading;
    }

    void KRG::Physics::PhysicsComponent::Unload( EntityModel::LoadingContext const& context, UUID requesterID )
    {
        KRG::Physics::PhysicsComponent::s_pTypeInfo->m_pTypeHelper->UnloadResources( context.m_pResourceSystem, requesterID, this );
        m_status = Status::Unloaded;
    }

    void KRG::Physics::PhysicsComponent::UpdateLoading()
    {
        if( m_status == Status::Loading )
        {
            auto const resourceLoadingStatus = KRG::Physics::PhysicsComponent::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( this );
            if ( resourceLoadingStatus == LoadingStatus::Loading )
            {
                return; // Something is still loading so early-out
            }

            if ( resourceLoadingStatus == LoadingStatus::Failed )
            {
                m_status = EntityComponent::Status::LoadingFailed;
            }
            else
            {
                m_status = EntityComponent::Status::Loaded;
            }
        }
    }
}

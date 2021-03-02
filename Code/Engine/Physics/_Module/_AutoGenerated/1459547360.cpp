//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Engine\Physics\_Module\_AutoGenerated\1459547360.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Physics::PhysicsShapeComponent
//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* KRG::Physics::PhysicsShapeComponent::StaticTypeInfo = nullptr;
    namespace TypeSystem
    {
        namespace TypeHelpers
        {
            void const* TTypeHelper<KRG::Physics::PhysicsShapeComponent>::DefaultTypeInstancePtr = nullptr;

            TTypeHelper<KRG::Physics::PhysicsShapeComponent> TTypeHelper<KRG::Physics::PhysicsShapeComponent>::StaticTypeHelper;
        }
    }

    TypeSystem::TypeInfo const* KRG::Physics::PhysicsShapeComponent::GetTypeInfo() const
    {
        return KRG::Physics::PhysicsShapeComponent::StaticTypeInfo;
    }

    void KRG::Physics::PhysicsShapeComponent::Load( EntityModel::LoadingContext const& context, UUID requesterID )
    {
        KRG::Physics::PhysicsShapeComponent::StaticTypeInfo->m_pTypeHelper->LoadResources( context.m_pResourceSystem, requesterID, this );
        m_status = Status::Loading;
    }

    void KRG::Physics::PhysicsShapeComponent::Unload( EntityModel::LoadingContext const& context, UUID requesterID )
    {
        KRG::Physics::PhysicsShapeComponent::StaticTypeInfo->m_pTypeHelper->UnloadResources( context.m_pResourceSystem, requesterID, this );
        m_status = Status::Unloaded;
    }

    void KRG::Physics::PhysicsShapeComponent::UpdateLoading()
    {
        if( m_status == Status::Loading )
        {
            auto const resourceLoadingStatus = KRG::Physics::PhysicsShapeComponent::StaticTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( this );
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
//-------------------------------------------------------------------------
// This is an auto-generated file - DO NOT edit
//-------------------------------------------------------------------------

#include "D:\Kruger\Code\Engine\Animation\_Module\_AutoGenerated\3010573638.h"

//-------------------------------------------------------------------------
// TypeHelper: KRG::Animation::AnimationGraphComponent
//-------------------------------------------------------------------------

namespace KRG
{
    TypeSystem::TypeInfo const* KRG::Animation::AnimationGraphComponent::s_pTypeInfo = nullptr;
    namespace TypeSystem
    {
        namespace TypeHelpers
        {
            IRegisteredType const* TTypeHelper<KRG::Animation::AnimationGraphComponent>::s_pDefaultTypeInstancePtr = nullptr;

            TTypeHelper<KRG::Animation::AnimationGraphComponent> TTypeHelper<KRG::Animation::AnimationGraphComponent>::StaticTypeHelper;
        }
    }

    void KRG::Animation::AnimationGraphComponent::Load( EntityModel::LoadingContext const& context, UUID requesterID )
    {
        KRG::Animation::AnimationGraphComponent::s_pTypeInfo->m_pTypeHelper->LoadResources( context.m_pResourceSystem, requesterID, this );
        m_status = Status::Loading;
    }

    void KRG::Animation::AnimationGraphComponent::Unload( EntityModel::LoadingContext const& context, UUID requesterID )
    {
        KRG::Animation::AnimationGraphComponent::s_pTypeInfo->m_pTypeHelper->UnloadResources( context.m_pResourceSystem, requesterID, this );
        m_status = Status::Unloaded;
    }

    void KRG::Animation::AnimationGraphComponent::UpdateLoading()
    {
        if( m_status == Status::Loading )
        {
            auto const resourceLoadingStatus = KRG::Animation::AnimationGraphComponent::s_pTypeInfo->m_pTypeHelper->GetResourceLoadingStatus( this );
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

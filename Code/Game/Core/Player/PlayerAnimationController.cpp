#include "PlayerAnimationController.h"
#include "GraphControllers/PlayerGraphController_Locomotion.h"
#include "GraphControllers/PlayerGraphController_Ability.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    AnimationController::AnimationController( Animation::AnimationGraphComponent* pGraphComponent, Render::SkeletalMeshComponent* pMeshComponent )
        : Animation::GraphController( pGraphComponent, pMeshComponent )
    {
        m_subGraphControllers.emplace_back( KRG::New<LocomotionGraphController>( pGraphComponent, pMeshComponent ) );
        m_subGraphControllers.emplace_back( KRG::New<AbilityGraphController>( pGraphComponent, pMeshComponent ) );
        m_characterStateParam.TryBind( this );
    }

    void AnimationController::SetCharacterState( CharacterAnimationState state )
    {
        static StringID const characterStates[(uint8) CharacterAnimationState::NumStates] =
        {
            StringID( "Locomotion" ),
            StringID( "Falling" ),
            StringID( "Ability" ),
        };

        KRG_ASSERT( state < CharacterAnimationState::NumStates );
        m_characterStateParam.Set( this, characterStates[(uint8) state] );
    }
}
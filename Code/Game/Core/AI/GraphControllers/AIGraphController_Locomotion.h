#pragma once
#include "Engine/Animation/Graph/AnimationGraphController.h"

//-------------------------------------------------------------------------

namespace KRG::AI
{
    class LocomotionGraphController final : public Animation::SubGraphController
    {
    public:

        KRG_ANIMATION_SUBGRAPH_CONTROLLER_ID( LocomotionGraphController );

    public:

        LocomotionGraphController( Animation::AnimationGraphComponent* pGraphComponent, Animation::AnimatedMeshComponent* pMeshComponent );

        void SetLocomotionDesires( Seconds const deltaTime, Vector const& headingVelocityWS, Vector const& facingDirectionWS );

    private:

        ControlParameter<float>     m_speedParam = ControlParameter<float>( "Locomotion_Speed" );
        ControlParameter<Vector>    m_headingParam = ControlParameter<Vector>( "Locomotion_Heading" );
        ControlParameter<Vector>    m_facingParam = ControlParameter<Vector>( "Locomotion_Facing" );
    };
}
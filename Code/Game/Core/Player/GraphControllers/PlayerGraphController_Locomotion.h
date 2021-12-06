#pragma once
#include "Engine/Animation/Graph/AnimationGraphController.h"

//-------------------------------------------------------------------------

namespace KRG::Player
{
    class LocomotionGraphController final : public Animation::GraphController
    {
    public:

        KRG_ANIM_GRAPH_CONTROLLER_ID( LocomotionGraphController );

    public:

        LocomotionGraphController( Animation::AnimationGraphComponent* pGraphComponent );

        inline void SetSpeed( float speed )
        {
            SetControlParameterValue( m_speedParam, speed );
        }

    private:

        ControlParameter<float>     m_speedParam = ControlParameter<float>( "Locomotion_Speed" );
        ControlParameter<Vector>    m_headingParam = ControlParameter<Vector>( "Locomotion_Heading" );
        ControlParameter<Vector>    m_facingParam = ControlParameter<Vector>( "Locomotion_Facing" );
    };
}
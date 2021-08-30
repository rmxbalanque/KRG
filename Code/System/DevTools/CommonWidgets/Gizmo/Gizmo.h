#pragma once
#include "System/Core/Math/Matrix.h"
#include "System/Render/RenderViewport.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    class Gizmo
    {

    public:

        void Update( Render::Viewport const& viewport );
        inline Matrix const& GetTransform() const { return m_targetTransform; }

        inline void SwitchToWorldSpace() { m_coordinateSpace = CoordinateSpace::World; }
        inline void SwitchToObjectSpace() { m_coordinateSpace = CoordinateSpace::Object; }
        inline bool IsInWorldSpace() const { return m_coordinateSpace == CoordinateSpace::World; }
        inline bool IsInObjectSpace() const { return m_coordinateSpace == CoordinateSpace::Object; }

    protected:

        Gizmo( Matrix& m )
            : m_targetTransform( m )
        {}

        virtual void InternalUpdate( Render::Viewport const& viewport ) = 0;

    protected:

        Matrix&             m_targetTransform;
        Matrix              m_manipulationTransform;
        CoordinateSpace     m_coordinateSpace = CoordinateSpace::World;

        // Shared data
        Vector              m_origin_WS;
        Vector              m_axisEndPoint_WS_X;
        Vector              m_axisEndPoint_WS_Y;
        Vector              m_axisEndPoint_WS_Z;
        Vector              m_axisDir_WS_X;
        Vector              m_axisDir_WS_Y;
        Vector              m_axisDir_WS_Z;

        Vector              m_origin_SS;
        Vector              m_axisEndPoint_SS_X;
        Vector              m_axisEndPoint_SS_Y;
        Vector              m_axisEndPoint_SS_Z;
        Vector              m_axisDir_SS_X;
        Vector              m_axisDir_SS_Y;
        Vector              m_axisDir_SS_Z;

        Radians             m_offsetBetweenViewFwdAndAxis_WS_X;
        Radians             m_offsetBetweenViewFwdAndAxis_WS_Y;
        Radians             m_offsetBetweenViewFwdAndAxis_WS_Z;
    };
}
#pragma once
#include "Gizmo.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Math/Plane.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    class RotationGizmo : public Gizmo
    {

    public:

        enum class ManipulationMode
        {
            None,
            RotateX,
            RotateY,
            RotateZ,
            RotateScreen,
        };

        enum class HoveredWidget
        {
            None,
            RotateX,
            RotateY,
            RotateZ,
            RotateScreen,
        };

    public:

        RotationGizmo( Matrix& m ) : Gizmo( m ) {}

    private:

        virtual void InternalUpdate( Render::Viewport const& viewport ) override final;

        bool DrawScreenRotationGizmo( Render::Viewport const& viewport );

        bool DrawRotationWidget( Render::Viewport const& viewport, Vector const& axisOfRotation_ws, Color color );
        void DrawManipulationWidget( Render::Viewport const& viewport, Vector const& axisOfRotation_ws, Vector const& axisOfRotation_ss, Color color );

        void UpdateMode( Render::Viewport const& viewport );
        void PerformManipulation( Render::Viewport const& viewport );

    private:

        ManipulationMode    m_mode = ManipulationMode::None;
        bool                m_isScreenRotationWidgetHovered = false;
        bool                m_isAxisRotationWidgetHoveredX = false;
        bool                m_isAxisRotationWidgetHoveredY = false;
        bool                m_isAxisRotationWidgetHoveredZ = false;

        Float2              m_rotationStartMousePosition = Float2::Zero;
        Quaternion          m_originalStartRotation = Quaternion::Identity;
        Vector              m_rotationAxis = Vector::Zero;
        Radians             m_rotationDeltaAngle = 0.0f;
    };
}
#pragma once
#include "Gizmo.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    class ScalingGizmo : public Gizmo
    {

    public:

        enum class ManipulationMode
        {
            None,
            ScaleX,
            ScaleY,
            ScaleZ,
            ScaleXYZ,
        };

    public:

        ScalingGizmo( Matrix& m ) : Gizmo( m ) {}

    private:

        virtual void InternalUpdate( Render::Viewport const& viewport ) override final;

        bool DrawOrigin( Vector const& position );
        bool DrawScaleAxis( Vector const& start, Vector const& end, Color color );
        bool DrawPlaneManipulationWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color );

        void UpdateMode( Render::Viewport const& viewport );
        void PerformManipulation( Render::Viewport const& viewport );

    private:

        Vector              m_positionOffset = Vector::Zero;
        ManipulationMode    m_mode = ManipulationMode::None;
        bool                m_isOriginHovered = false;
        bool                m_isAxisNegatedX = false;
        bool                m_isAxisNegatedY = false;
        bool                m_isAxisNegatedZ = false;
        bool                m_isAxisHoveredX = false;
        bool                m_isAxisHoveredY = false;
        bool                m_isAxisHoveredZ = false;
    };
}
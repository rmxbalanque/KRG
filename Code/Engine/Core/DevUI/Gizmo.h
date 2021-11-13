#pragma once
#include "Engine/Core/_Module/API.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

namespace KRG::ImGuiX
{
    class KRG_ENGINE_CORE_API Gizmo
    {
    public:

        enum class GizmoMode
        {
            None = 0,
            Rotation,
            Translation,
            Scale
        };

        enum class ManipulationMode
        {
            None = 0,

            RotateX,
            RotateY,
            RotateZ,
            RotateScreen,

            TranslateX,
            TranslateY,
            TranslateZ,
            TranslateXY,
            TranslateXZ,
            TranslateYZ,

            ScaleX,
            ScaleY,
            ScaleZ,
            ScaleXYZ,
        };

    public:

        // Draw the gizmo, returns true if any manipulation was performed
        bool Draw( Render::Viewport const& viewport );
        inline Transform const& GetTransform() const { return *m_pTargetTransform; }

        inline GizmoMode GetMode() const { return m_gizmoMode; }
        void SwitchMode( GizmoMode newMode );
        void SwitchToNextMode();

        inline void SwitchToWorldSpace() { m_coordinateSpace = CoordinateSpace::World; }
        inline void SwitchToLocalSpace() { m_coordinateSpace = CoordinateSpace::Local; }
        inline bool IsInWorldSpace() const { return m_coordinateSpace == CoordinateSpace::World; }
        inline bool IsInLocalSpace() const { return m_coordinateSpace == CoordinateSpace::Local; }

        inline void SetTargetTransform( Transform* pTargetTransform );

    private:

        void ResetState();

        // Rotation
        bool Rotation_DrawScreenGizmo( Render::Viewport const& viewport );
        bool Rotation_DrawWidget( Render::Viewport const& viewport, Vector const& axisOfRotation_WS, Color color );
        void Rotation_DrawManipulationWidget( Render::Viewport const& viewport, Vector const& axisOfRotation_WS, Vector const& axisOfRotation_ss, Color color );
        void Rotation_UpdateMode( Render::Viewport const& viewport );
        void Rotation_PerformManipulation( Render::Viewport const& viewport );
        void Rotation_Update( Render::Viewport const& viewport );

        // Translation
        bool Translation_DrawAxis( Vector const& start, Vector const& end, Color color );
        bool Translation_DrawPlaneWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color );
        void Translation_UpdateMode( Render::Viewport const& viewport );
        void Translation_PerformManipulation( Render::Viewport const& viewport );
        void Translation_Update( Render::Viewport const& viewport );

        // Scale
        bool Scale_DrawOrigin( Vector const& originPosition );
        bool Scale_DrawAxis( Vector const& start, Vector const& end, Color color );
        bool Scale_DrawPlaneWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color );
        void Scale_UpdateMode( Render::Viewport const& viewport );
        void Scale_PerformManipulation( Render::Viewport const& viewport );
        void Scale_Update( Render::Viewport const& viewport );

    protected:

        Transform*          m_pTargetTransform = nullptr;
        Transform           m_manipulationTransform;
        CoordinateSpace     m_coordinateSpace = CoordinateSpace::World;
        GizmoMode           m_gizmoMode = GizmoMode::None;
        ManipulationMode    m_manipulationMode = ManipulationMode::None;

        // Updated each frame
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

        // Rotation State
        bool                m_isScreenRotationWidgetHovered = false;
        bool                m_isAxisRotationWidgetHoveredX = false;
        bool                m_isAxisRotationWidgetHoveredY = false;
        bool                m_isAxisRotationWidgetHoveredZ = false;

        Float2              m_rotationStartMousePosition = Float2::Zero;
        Quaternion          m_originalStartRotation = Quaternion::Identity;
        Vector              m_rotationAxis = Vector::Zero;
        Radians             m_rotationDeltaAngle = 0.0f;

        // Translation State
        Vector              m_translationOffset = Vector::Zero;
        bool                m_isAxisHoveredX = false;
        bool                m_isAxisHoveredY = false;
        bool                m_isAxisHoveredZ = false;
        bool                m_isPlaneHoveredXY = false;
        bool                m_isPlaneHoveredXZ = false;
        bool                m_isPlaneHoveredYZ = false;

        // Scale State
        Vector              m_positionOffset = Vector::Zero;
        bool                m_isOriginHovered = false;
        bool                m_isAxisNegatedX = false;
        bool                m_isAxisNegatedY = false;
        bool                m_isAxisNegatedZ = false;
    };
}
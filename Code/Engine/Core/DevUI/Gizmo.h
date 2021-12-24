#pragma once
#include "Engine/Core/_Module/API.h"
#include "System/Render/RenderViewport.h"
#include "System/Core/Math/Matrix.h"
#include "System/Core/Types/Color.h"
#include "System/Core/Types/Event.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    class KRG_ENGINE_CORE_API Gizmo
    {
        enum class AxisCap
        {
            Dot,
            Arrow
        };

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

        void SetCoordinateSystemSpace( CoordinateSpace space );
        inline bool IsInWorldSpace() const { return m_coordinateSpace == CoordinateSpace::World; }
        inline bool IsInLocalSpace() const { return m_coordinateSpace == CoordinateSpace::Local; }

        inline void SetTargetTransform( Transform* pTargetTransform );

        inline TEventHandle<> OnManipulationStarted() { return m_manipulationStarted; }
        inline TEventHandle<> OnManipulationEnded() { return m_manipulationEnded; }

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
        void Translation_UpdateMode( Render::Viewport const& viewport );
        void Translation_PerformManipulation( Render::Viewport const& viewport );
        void Translation_DrawAndUpdate( Render::Viewport const& viewport );

        // Scale
        void Scale_UpdateMode( Render::Viewport const& viewport );
        void Scale_PerformManipulation( Render::Viewport const& viewport );
        void Scale_DrawAndUpdate( Render::Viewport const& viewport );

        // Draw Helpers
        void DrawAxisGuide( Vector const& axis, Color color );
        bool DrawAxisWidget( Vector const& start, Vector const& end, Color color, AxisCap cap, float originOffset );
        bool DrawPlaneWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color );

    protected:

        Transform*                  m_pTargetTransform = nullptr;
        Transform                   m_manipulationTransform;
        CoordinateSpace             m_coordinateSpace = CoordinateSpace::World;
        GizmoMode                   m_gizmoMode = GizmoMode::None;
        ManipulationMode            m_manipulationMode = ManipulationMode::None;

        TEvent<>                    m_manipulationStarted;
        TEvent<>                    m_manipulationEnded;

        // Updated each frame
        Vector                      m_origin_WS;
        Vector                      m_axisEndPoint_WS_X;
        Vector                      m_axisEndPoint_WS_Y;
        Vector                      m_axisEndPoint_WS_Z;
        Vector                      m_axisDir_WS_X;
        Vector                      m_axisDir_WS_Y;
        Vector                      m_axisDir_WS_Z;

        Vector                      m_origin_SS;
        Vector                      m_axisEndPoint_SS_X;
        Vector                      m_axisEndPoint_SS_Y;
        Vector                      m_axisEndPoint_SS_Z;
        Vector                      m_axisDir_SS_X;
        Vector                      m_axisDir_SS_Y;
        Vector                      m_axisDir_SS_Z;
        float                       m_axisLength_SS_X;
        float                       m_axisLength_SS_Y;
        float                       m_axisLength_SS_Z;
        float                       m_axisScale_SS_X;
        float                       m_axisScale_SS_Y;
        float                       m_axisScale_SS_Z;

        Radians                     m_offsetBetweenViewFwdAndAxis_WS_X;
        Radians                     m_offsetBetweenViewFwdAndAxis_WS_Y;
        Radians                     m_offsetBetweenViewFwdAndAxis_WS_Z;

        bool                        m_shouldDrawAxis_X = false;
        bool                        m_shouldDrawAxis_Y = false;
        bool                        m_shouldDrawAxis_Z = false;

        // Rotation State
        bool                        m_isScreenRotationWidgetHovered = false;
        bool                        m_isAxisRotationWidgetHoveredX = false;
        bool                        m_isAxisRotationWidgetHoveredY = false;
        bool                        m_isAxisRotationWidgetHoveredZ = false;

        Float2                      m_rotationStartMousePosition = Float2::Zero;
        Quaternion                  m_originalStartRotation = Quaternion::Identity;
        Vector                      m_rotationAxis = Vector::Zero;
        Radians                     m_rotationDeltaAngle = Radians( 0.0f );

        // Translation State
        Vector                      m_translationOffset = Vector::Zero;
        bool                        m_isAxisHoveredX = false;
        bool                        m_isAxisHoveredY = false;
        bool                        m_isAxisHoveredZ = false;
        bool                        m_isPlaneHoveredXY = false;
        bool                        m_isPlaneHoveredXZ = false;
        bool                        m_isPlaneHoveredYZ = false;

        // Scale State
        Vector                      m_positionOffset = Vector::Zero;
        bool                        m_isOriginHovered = false;
    };
}
#endif
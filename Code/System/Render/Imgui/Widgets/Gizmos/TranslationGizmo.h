#pragma once
#include "Gizmo.h"
#include "System/Core/Types/Color.h"

//-------------------------------------------------------------------------

#if KRG_DEVELOPMENT_TOOLS
namespace KRG::ImGuiX
{
    class TranslationGizmo : public Gizmo
    {

    public:

        enum class ManipulationMode
        {
            None,
            TranslateX,
            TranslateY,
            TranslateZ,
            TranslateXY,
            TranslateXZ,
            TranslateYZ,
        };

    public:

        TranslationGizmo( Matrix& m ) : Gizmo( m ) {}

    private:

        virtual void InternalUpdate( Render::Viewport const& viewport ) override final;

        bool DrawTranslationAxis( Vector const& start, Vector const& end, Color color );
        bool DrawPlaneTranslationWidget( Vector const& origin, Vector const& axis0, Vector const& axis1, Color color );

        void UpdateMode( Render::Viewport const& viewport );
        void PerformManipulation( Render::Viewport const& viewport );

    private:

        Vector              m_translationOffset = Vector::Zero;
        ManipulationMode    m_mode = ManipulationMode::None;
        bool                m_isAxisHoveredX = false;
        bool                m_isAxisHoveredY = false;
        bool                m_isAxisHoveredZ = false;
        bool                m_isPlaneHoveredXY = false;
        bool                m_isPlaneHoveredXZ = false;
        bool                m_isPlaneHoveredYZ = false;
    };
}
#endif
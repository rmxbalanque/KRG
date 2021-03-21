#pragma once
#include "System/Core/Math/Matrix.h"

//-------------------------------------------------------------------------

namespace KRG
{
    class UpdateContext;
    namespace World { class EditorWorld; }
    namespace Math { class Viewport; }
    namespace ImGuiX { class Gizmo; }

    //-------------------------------------------------------------------------

    namespace Editor
    {
        class TempController
        {
            enum class ManipulationMode
            {
                None,
                Translate,
                Rotate,
                Scale,
            };

        public:

            TempController( World::EditorWorld* pWorld );
            ~TempController();

            void StartFrame( UpdateContext const& context, Math::Viewport const& viewport );
            void EndFrame( UpdateContext const& context );

        private:

            void DrawEditorUI( UpdateContext const& context, Math::Viewport const& viewport );

            void DrawManipulationToolbar( UpdateContext const& context, Math::Viewport const& viewport );
            void DrawOrientationGuide( UpdateContext const& context, Math::Viewport const& viewport );
            void DrawHackToolbar( UpdateContext const& context, Math::Viewport const& viewport );

        private:

            World::EditorWorld*             m_pWorld = nullptr;
            ImGuiX::Gizmo*                  m_pGizmo = nullptr;
            ManipulationMode                m_manipulationMode = ManipulationMode::None;
            //TEntityPtr<SpatialEntity>       m_pObjectBeingManipulated = nullptr;

            char                            m_hackBuffer[256];
        };
    }
}
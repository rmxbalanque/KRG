#pragma once

#include "AnimationGraphEditor_Model.h"
#include "AnimationGraphEditor_ControlParameterEditor.h"
#include "AnimationGraphEditor_GraphEditor.h"
#include "AnimationGraphEditor_PropertyGrid.h"
#include "AnimationGraphEditor_VariationEditor.h"
#include "Tools/Core/Editors/StandaloneEditor.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class AnimationGraphEditor final : public TStandaloneEditor<GraphEditorModel>
    {
    public:

        ~AnimationGraphEditor();

    private:

        virtual char const* GetName() const override final { return "Kruger Animation Graph Toolkit"; }

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

        virtual void FrameStartUpdate( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

        virtual void DrawMainMenu( UpdateContext const& context, Render::ViewportManager& viewportManager ) override;

    private:

        GraphControlParameterEditor*        m_pControlParameterEditor = nullptr;
        GraphEditorView*                    m_pGraphEditor = nullptr;
        GraphPropertyGrid*                  m_pPropertyGrid = nullptr;
        GraphVariationEditor*               m_pVariationEditor = nullptr;
    };
}
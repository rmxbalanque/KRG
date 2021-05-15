#pragma once

#include "AnimGraphToolkit_ControlParameterEditor.h"
#include "AnimGraphToolkit_GraphEditor.h"
#include "AnimGraphToolkit_PropertyGrid.h"
#include "Tools/Editor/EditorToolkit.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Tools
{
    class AnimGraphToolkit final : public TEditorToolkit<EditorModel>
    {
    public:

        ~AnimGraphToolkit();

    private:

        virtual char const* GetName() const override final { return "Kruger Animation Graph Toolkit"; }

        virtual void SetUserFlags( uint64 flags ) override;
        virtual uint64 GetUserFlags() const override;

        virtual void Initialize( UpdateContext const& context, SettingsRegistry const& settingsRegistry ) override;
        virtual void Shutdown( UpdateContext const& context ) override;

    private:

        GraphControlParameterEditor*     m_pControlParameterEditor = nullptr;
        GraphEditor*                     m_pGraphEditor = nullptr;
        GraphPropertyGrid*               m_pPropertyGrid = nullptr;
    };
}
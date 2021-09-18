#pragma once
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"
#include "Tools/Animation/Graph/AnimationGraphTools_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ResultToolsNode;

    //-------------------------------------------------------------------------

    class StateLayerDataToolsNode final : public FlowToolsNode
    {
        KRG_REGISTER_TYPE( StateLayerDataToolsNode );

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "State Layer Data"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
    };

    //-------------------------------------------------------------------------

    class KRG_TOOLS_ANIMATION_API StateBaseToolsNode : public GraphEditor::SM::State
    {
        friend class StateMachineToolsNode;
        KRG_REGISTER_TYPE( StateBaseToolsNode );

    public:

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }

    protected:

        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    protected:

        EXPOSE String                       m_name = "State";
        EXPOSE TVector<StringID>            m_entryEvents;
        EXPOSE TVector<StringID>            m_executeEvents;
        EXPOSE TVector<StringID>            m_exitEvents;
    };

    //-------------------------------------------------------------------------

    class StateToolsNode final : public StateBaseToolsNode
    {
        friend class StateMachineToolsNode;
        KRG_REGISTER_TYPE( StateToolsNode );

    public:

        struct TimedStateEvent : public IRegisteredType
        {
            KRG_REGISTER_TYPE( TimedStateEvent );

            EXPOSE StringID                 m_ID;
            EXPOSE Seconds                  m_timeValue;
        };

    public:

        ResultToolsNode const* GetBlendTreeRootNode() const;
        StateLayerDataToolsNode const* GetLayerDataNode() const;
        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "State"; }
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        EXPOSE TVector<TimedStateEvent>     m_timeRemainingEvents;
        EXPOSE TVector<TimedStateEvent>     m_timeElapsedEvents;
    };

    //-------------------------------------------------------------------------

    class OffStateToolsNode final : public StateBaseToolsNode
    {
        friend class StateMachineToolsNode;
        KRG_REGISTER_TYPE( OffStateToolsNode );

    private:

        virtual char const* GetTypeName() const override { return "Off State"; }
        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::PaleVioletRed ); }
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;
    };
}
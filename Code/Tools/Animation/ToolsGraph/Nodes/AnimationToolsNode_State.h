#pragma once
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"
#include "Tools/Animation/ToolsGraph/AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_ResultNode;

    //-------------------------------------------------------------------------

    class Tools_StateLayerDataNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_StateLayerDataNode );

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "State Layer Data"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
    };

    //-------------------------------------------------------------------------

    class Tools_StateBaseNode : public GraphEditor::SM::State
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( Tools_StateBaseNode );

    public:

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }

    protected:

        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    protected:

        KRG_EXPOSE String                       m_name = "State";
        KRG_EXPOSE TVector<StringID>            m_entryEvents;
        KRG_EXPOSE TVector<StringID>            m_executeEvents;
        KRG_EXPOSE TVector<StringID>            m_exitEvents;
    };

    //-------------------------------------------------------------------------

    class Tools_StateNode final : public Tools_StateBaseNode
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( Tools_StateNode );

    public:

        struct TimedStateEvent : public IRegisteredType
        {
            KRG_REGISTER_TYPE( TimedStateEvent );

            KRG_EXPOSE StringID                 m_ID;
            KRG_EXPOSE Seconds                  m_timeValue;
        };

    public:

        Tools_ResultNode const* GetBlendTreeRootNode() const;
        Tools_StateLayerDataNode const* GetLayerDataNode() const;
        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

    private:

        virtual char const* GetTypeName() const override { return "State"; }
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

    private:

        KRG_EXPOSE TVector<TimedStateEvent>     m_timeRemainingEvents;
        KRG_EXPOSE TVector<TimedStateEvent>     m_timeElapsedEvents;
    };

    //-------------------------------------------------------------------------

    class Tools_OffStateNode final : public Tools_StateBaseNode
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( Tools_OffStateNode );

    private:

        virtual char const* GetTypeName() const override { return "Off State"; }
        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::PaleVioletRed ); }
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;
    };
}
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

    class StateToolsNode final : public GraphEditor::SM::State
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

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "State"; }
        virtual void DrawExtraControls( GraphEditor::DrawingContext const& ctx ) override;

        ResultToolsNode const* GetBlendTreeRootNode() const;
        StateLayerDataToolsNode const* GetLayerDataNode() const;

    private:

        EXPOSE String                       m_name = "State";
        EXPOSE TVector<StringID>            m_entryEvents;
        EXPOSE TVector<StringID>            m_executeEvents;
        EXPOSE TVector<StringID>            m_exitEvents;
        EXPOSE TVector<TimedStateEvent>     m_timeRemainingEvents;
        EXPOSE TVector<TimedStateEvent>     m_timeElapsedEvents;
    };
}
#pragma once
#include "Tools/Animation/Graph/AnimationGraphTools_FlowNode.h"
#include "Tools/Animation/Graph/AnimationGraphTools_FlowGraph.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Transition.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class StateToolsNode;

    //-------------------------------------------------------------------------

    class TransitionToolsNode : public FlowToolsNode
    {
        friend class StateMachineToolsNode;
        KRG_REGISTER_TYPE( TransitionToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Transition"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual bool IsUserCreatable() const override { return true; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }

    protected:

        EXPOSE String                               m_name = "Transition";
        EXPOSE Math::Easing::Type                   m_blendWeightEasingType = Math::Easing::Type::Linear;
        EXPOSE TransitionNode::RootMotionBlend      m_rootMotionBlend = TransitionNode::RootMotionBlend::Blend;
        EXPOSE Seconds                              m_duration = 0.3f;
        EXPOSE float                                m_syncEventOffset = 0.0f;
        EXPOSE bool                                 m_isSynchronized = false;
        EXPOSE bool                                 m_clampDurationToSource = true;
        EXPOSE bool                                 m_keepSourceSyncEventIdx = false;
        EXPOSE bool                                 m_keepSourceSyncEventPercentageThrough = false;
        EXPOSE bool                                 m_canBeForced = false;
    };

    //-------------------------------------------------------------------------

    class GlobalTransitionToolsNode final : public TransitionToolsNode
    {
        friend class GlobalTransitionsToolsGraph;
        KRG_REGISTER_TYPE( GlobalTransitionToolsNode );

    public:

        inline UUID const& GetEndStateID() const { return m_stateID; }

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Global Transition"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }

    private:

        REGISTER UUID                               m_stateID;
    };

    // Transition conduit
    //-------------------------------------------------------------------------

    class TransitionConduitToolsNode final : public GraphEditor::SM::TransitionConduit
    {
        KRG_REGISTER_TYPE( TransitionConduitToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Transition"; }
    };

    // The Flow graph for the state machine node
    //-------------------------------------------------------------------------

    class GlobalTransitionsToolsGraph final : public FlowToolGraph
    {
        KRG_REGISTER_TYPE( GlobalTransitionsToolsGraph );

    public:

        GlobalTransitionsToolsGraph() : FlowToolGraph( GraphType::ValueTree ) {}

        void Update( TInlineVector<StateToolsNode*, 20> const& states );
    };

    // State machine node
    //-------------------------------------------------------------------------

    class GlobalTransitionsToolsNode final : public GraphEditor::SM::Node
    {
        KRG_REGISTER_TYPE( GlobalTransitionsToolsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        inline void Update( TInlineVector<StateToolsNode*, 20> const& states )
        {
            Cast<GlobalTransitionsToolsGraph>( GetSecondaryGraph() )->Update( states );
        }

        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::OrangeRed ); }
        virtual char const* GetTypeName() const override { return "Global Transitions"; }
        virtual bool IsUserCreatable() const override { return false; }
    };
}
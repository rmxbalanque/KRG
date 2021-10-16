#pragma once
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowNode.h"
#include "Tools/Animation/GraphEditor/ToolsGraph/AnimationToolsGraph_FlowGraph.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Transition.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_StateBaseNode;

    //-------------------------------------------------------------------------

    class Tools_TransitionNode : public Tools_GraphNode
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( Tools_TransitionNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Transition"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual bool IsUserCreatable() const override { return true; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }

    protected:

        KRG_EXPOSE String                                       m_name = "Transition";
        KRG_EXPOSE Math::Easing::Type                           m_blendWeightEasingType = Math::Easing::Type::Linear;
        KRG_EXPOSE TransitionNode::RootMotionBlend       m_rootMotionBlend = TransitionNode::RootMotionBlend::Blend;
        KRG_EXPOSE Seconds                                      m_duration = 0.3f;
        KRG_EXPOSE float                                        m_syncEventOffset = 0.0f;
        KRG_EXPOSE bool                                         m_isSynchronized = false;
        KRG_EXPOSE bool                                         m_clampDurationToSource = true;
        KRG_EXPOSE bool                                         m_keepSourceSyncEventIdx = false;
        KRG_EXPOSE bool                                         m_keepSourceSyncEventPercentageThrough = false;
        KRG_EXPOSE bool                                         m_canBeForced = false;
    };

    //-------------------------------------------------------------------------

    class Tools_GlobalTransitionNode final : public Tools_TransitionNode
    {
        friend class GlobalTransitionsToolsGraph;
        KRG_REGISTER_TYPE( Tools_GlobalTransitionNode );

    public:

        inline UUID const& GetEndStateID() const { return m_stateID; }

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Global Transition"; }
        virtual char const* GetCategory() const override { return "State Machine"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }

    private:

        KRG_REGISTER UUID                               m_stateID;
    };

    // Transition conduit
    //-------------------------------------------------------------------------

    class Tools_TransitionConduitNode final : public GraphEditor::SM::TransitionConduit
    {
        KRG_REGISTER_TYPE( Tools_TransitionConduitNode );

    public:

        bool HasTransitions() const;

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;
        virtual char const* GetTypeName() const override { return "Transition"; }
        virtual ImColor GetNodeColor() const override;
    };

    // The Flow graph for the state machine node
    //-------------------------------------------------------------------------

    class GlobalTransitionsToolsGraph final : public FlowGraph
    {
        friend class Tools_GlobalTransitionsNode;
        KRG_REGISTER_TYPE( GlobalTransitionsToolsGraph );

    public:

        GlobalTransitionsToolsGraph() : FlowGraph( GraphType::ValueTree ) {}

        bool HasGlobalTransitionForState( UUID const& stateID ) const;

    private:

        virtual void OnShowGraph() override { UpdateGlobalTransitionNodes(); }

        void UpdateGlobalTransitionNodes();
    };

    // State machine node
    //-------------------------------------------------------------------------

    class Tools_GlobalTransitionsNode final : public GraphEditor::SM::Node
    {
        KRG_REGISTER_TYPE( Tools_GlobalTransitionsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;
        inline bool HasGlobalTransitionForState( UUID const& stateID ) const { return Cast<GlobalTransitionsToolsGraph>( GetSecondaryGraph() )->HasGlobalTransitionForState( stateID ); }
        void UpdateGlobalTransitionNodes();

    private:

        virtual ImColor GetNodeColor() const override { return ImGuiX::ConvertColor( Colors::OrangeRed ); }
        virtual char const* GetTypeName() const override { return "Global Transitions"; }
        virtual bool IsUserCreatable() const override { return false; }
    };
}
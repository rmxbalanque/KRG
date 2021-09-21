#pragma once
#include "Tools/Animation/ToolsGraph/AnimationToolsGraph_FlowNode.h"
#include "Tools/Animation/ToolsGraph/AnimationToolsGraph_FlowGraph.h"
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

        EXPOSE String                                       m_name = "Transition";
        EXPOSE Math::Easing::Type                           m_blendWeightEasingType = Math::Easing::Type::Linear;
        EXPOSE TransitionNode::RootMotionBlend       m_rootMotionBlend = TransitionNode::RootMotionBlend::Blend;
        EXPOSE Seconds                                      m_duration = 0.3f;
        EXPOSE float                                        m_syncEventOffset = 0.0f;
        EXPOSE bool                                         m_isSynchronized = false;
        EXPOSE bool                                         m_clampDurationToSource = true;
        EXPOSE bool                                         m_keepSourceSyncEventIdx = false;
        EXPOSE bool                                         m_keepSourceSyncEventPercentageThrough = false;
        EXPOSE bool                                         m_canBeForced = false;
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

        REGISTER UUID                               m_stateID;
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
        virtual ImColor GetHighlightColor() const override;
    };

    // The Flow graph for the state machine node
    //-------------------------------------------------------------------------

    class GlobalTransitionsToolsGraph final : public FlowGraph
    {
        KRG_REGISTER_TYPE( GlobalTransitionsToolsGraph );

    public:

        GlobalTransitionsToolsGraph() : FlowGraph( GraphType::ValueTree ) {}

        void Update( TInlineVector<Tools_StateBaseNode*, 20> const& states );

        bool HasGlobalTransitionForState( UUID const& stateID ) const;
    };

    // State machine node
    //-------------------------------------------------------------------------

    class Tools_GlobalTransitionsNode final : public GraphEditor::SM::Node
    {
        KRG_REGISTER_TYPE( Tools_GlobalTransitionsNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        inline void Update( TInlineVector<Tools_StateBaseNode*, 20> const& states )
        {
            Cast<GlobalTransitionsToolsGraph>( GetSecondaryGraph() )->Update( states );
        }

        inline bool HasGlobalTransitionForState( UUID const& stateID ) const
        {
            return Cast<GlobalTransitionsToolsGraph>( GetSecondaryGraph() )->HasGlobalTransitionForState( stateID );
        }

        virtual ImColor GetHighlightColor() const override { return ImGuiX::ConvertColor( Colors::OrangeRed ); }
        virtual char const* GetTypeName() const override { return "Global Transitions"; }
        virtual bool IsUserCreatable() const override { return false; }
    };
}
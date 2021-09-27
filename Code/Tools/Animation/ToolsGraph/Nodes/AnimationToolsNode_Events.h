#pragma once
#include "../AnimationToolsGraph_FlowNode.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_IDs.h"
#include "Engine/Animation/Graph/Nodes/AnimationGraphNode_Events.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_GenericEventConditionNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_GenericEventConditionNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Generic Event Condition"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE GenericEventConditionNode::Operator         m_operator = GenericEventConditionNode::Operator::Or;
        KRG_EXPOSE EventSearchMode                             m_searchMode = EventSearchMode::SearchAll;
        KRG_EXPOSE TVector<StringID>                           m_eventIDs;
    };

    //-------------------------------------------------------------------------

    class Tools_GenericEventPercentageThroughNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_GenericEventPercentageThroughNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Generic Event Percentage Through"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE EventSearchMode                             m_searchMode = EventSearchMode::SearchAll;
        KRG_EXPOSE bool                                        m_preferHighestPercentageThrough = false;
        KRG_EXPOSE StringID                                    m_eventID;
    };

    //-------------------------------------------------------------------------

    class Tools_FootEventConditionNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FootEventConditionNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Footstep Condition"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE FootEvent::PhaseCondition                   m_phaseCondition = FootEvent::PhaseCondition::LeftFootDown;
        KRG_EXPOSE bool                                        m_preferHighestPercentageThrough = false;
    };

    //-------------------------------------------------------------------------

    class Tools_FootstepEventPercentageThroughNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_FootstepEventPercentageThroughNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Footstep Percentage Through"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE FootEvent::PhaseCondition                   m_phaseCondition = FootEvent::PhaseCondition::LeftFootDown;
        KRG_EXPOSE bool                                        m_preferHighestPercentageThrough = false;
    };

    //-------------------------------------------------------------------------

    class Tools_SyncEventConditionNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_SyncEventConditionNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Sync Event Condition"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE SyncEventConditionNode::TriggerMode          m_triggerMode = SyncEventConditionNode::TriggerMode::ExactlyAtEventIndex;
        KRG_EXPOSE int32                                        m_syncEventIdx = InvalidIndex;
    };

    //-------------------------------------------------------------------------

    class Tools_CurrentSyncEventNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_CurrentSyncEventNode );

    public:

        virtual void Initialize( GraphEditor::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Current Sync Event"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };
}
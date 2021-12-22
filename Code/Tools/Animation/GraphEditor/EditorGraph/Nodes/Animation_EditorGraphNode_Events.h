#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_IDs.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Events.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class GenericEventConditionEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( GenericEventConditionEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Generic Event Condition"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE GenericEventConditionNode::Operator         m_operator = GenericEventConditionNode::Operator::Or;
        KRG_EXPOSE EventSearchMode                             m_searchMode = EventSearchMode::SearchAll;
        KRG_EXPOSE TVector<StringID>                           m_eventIDs;
    };

    //-------------------------------------------------------------------------

    class GenericEventPercentageThroughEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( GenericEventPercentageThroughEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Generic Event Percentage Through"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
        virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

    private:

        KRG_EXPOSE EventSearchMode                             m_searchMode = EventSearchMode::SearchAll;
        KRG_EXPOSE bool                                        m_preferHighestPercentageThrough = false;
        KRG_EXPOSE StringID                                    m_eventID;
    };

    //-------------------------------------------------------------------------

    class FootEventConditionEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FootEventConditionEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Footstep Condition"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE FootEvent::PhaseCondition                   m_phaseCondition = FootEvent::PhaseCondition::LeftFootDown;
        KRG_EXPOSE bool                                        m_preferHighestPercentageThrough = false;
    };

    //-------------------------------------------------------------------------

    class FootstepEventPercentageThroughEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( FootstepEventPercentageThroughEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Footstep Percentage Through"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE FootEvent::PhaseCondition                   m_phaseCondition = FootEvent::PhaseCondition::LeftFootDown;
        KRG_EXPOSE bool                                        m_preferHighestPercentageThrough = false;
    };

    //-------------------------------------------------------------------------

    class SyncEventConditionEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( SyncEventConditionEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Sync Event Condition"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;

    private:

        KRG_EXPOSE SyncEventConditionNode::TriggerMode          m_triggerMode = SyncEventConditionNode::TriggerMode::ExactlyAtEventIndex;
        KRG_EXPOSE int32                                        m_syncEventIdx = InvalidIndex;
    };

    //-------------------------------------------------------------------------

    class CurrentSyncEventEditorNode final : public EditorGraphNode
    {
        KRG_REGISTER_TYPE( CurrentSyncEventEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Current Sync Event"; }
        virtual char const* GetCategory() const override { return "Events"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree, GraphType::ValueTree ); }
        virtual NodeIndex Compile( EditorGraphCompilationContext& context ) const override;
    };
}
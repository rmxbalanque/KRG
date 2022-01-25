#pragma once
#include "Animation_EditorGraph_FlowGraph.h"
#include "Tools/Core/VisualGraph/VisualGraph_StateMachineGraph.h"
#include "Engine/Animation/Graph/Nodes/Animation_RuntimeGraphNode_Transition.h"

//-------------------------------------------------------------------------

namespace KRG::Animation
{
    class StateMachineGraph;

    //-------------------------------------------------------------------------
    // STATE NODES
    //-------------------------------------------------------------------------

    namespace GraphNodes
    {
        // Base class for all states
        class StateBaseEditorNode : public VisualGraph::SM::State
        {
            friend class StateMachineEditorNode;
            KRG_REGISTER_TYPE( StateBaseEditorNode );

        public:

            struct TimedStateEvent : public IRegisteredType
            {
                KRG_REGISTER_TYPE( TimedStateEvent );

                KRG_EXPOSE StringID                 m_ID;
                KRG_EXPOSE Seconds                  m_timeValue;
            };

        public:

            virtual char const* GetDisplayName() const override { return m_name.c_str(); }

        protected:

            virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;

        protected:

            KRG_EXPOSE String                       m_name = "State";
            KRG_EXPOSE TVector<StringID>            m_entryEvents;
            KRG_EXPOSE TVector<StringID>            m_executeEvents;
            KRG_EXPOSE TVector<StringID>            m_exitEvents;
            KRG_EXPOSE TVector<TimedStateEvent>     m_timeRemainingEvents;
            KRG_EXPOSE TVector<TimedStateEvent>     m_timeElapsedEvents;
        };

        // The result node for a state's layer settings
        class StateLayerDataEditorNode final : public EditorGraphNode
        {
            KRG_REGISTER_TYPE( StateLayerDataEditorNode );

            virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

            virtual char const* GetTypeName() const override { return "State Layer Data"; }
            virtual char const* GetCategory() const override { return "State Machine"; }
            virtual bool IsUserCreatable() const override { return false; }
            virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }
        };

        // A basic state
        class StateEditorNode final : public StateBaseEditorNode
        {
            friend class StateMachineEditorNode;
            KRG_REGISTER_TYPE( StateEditorNode );

        public:

            ResultEditorNode const* GetBlendTreeRootNode() const;
            StateLayerDataEditorNode const* GetLayerDataNode() const;
            virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        private:

            virtual char const* GetTypeName() const override { return "State"; }
        };

        // An off-state
        class OffStateEditorNode final : public StateBaseEditorNode
        {
            friend class StateMachineEditorNode;
            KRG_REGISTER_TYPE( OffStateEditorNode );

        private:

            virtual char const* GetTypeName() const override { return "Off State"; }
            virtual ImColor GetNodeTitleColor() const override { return ImGuiX::ConvertColor( Colors::PaleVioletRed ); }
            virtual void DrawExtraControls( VisualGraph::DrawContext const& ctx ) override;
        };
    }

    //-------------------------------------------------------------------------
    // ENTRY STATE OVERRIDES
    //-------------------------------------------------------------------------

    namespace GraphNodes
    {
        // The result node for the entry state overrides
        class EntryStateOverrideConditionsEditorNode final : public EditorGraphNode
        {
            friend StateMachineGraph;
            KRG_REGISTER_TYPE( EntryStateOverrideConditionsEditorNode );

        public:

            void UpdateInputPins();
            virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }

        private:

            virtual char const* GetTypeName() const override { return "Entry State Conditions"; }
            virtual char const* GetCategory() const override { return "State Machine"; }
            virtual bool IsUserCreatable() const override { return false; }

        private:

            // For each pin, what state does it represent
            KRG_REGISTER TVector<UUID>  m_pinToStateMapping;
        };

        // The entry state override node
        class EntryStateOverrideConduitEditorNode final : public VisualGraph::SM::Node
        {
            KRG_REGISTER_TYPE( EntryStateOverrideConduitEditorNode );

        public:

            virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        private:

            virtual bool IsVisibleNode() const override { return false; }
            virtual ImColor GetNodeTitleColor() const override { return ImGuiX::ConvertColor( Colors::SlateBlue ); }
            virtual char const* GetTypeName() const override { return "Entry Overrides"; }
            virtual bool IsUserCreatable() const override { return false; }
        };
    }

    // The entry state overrides flow graph
    class EntryStateOverrideEditorGraph final : public FlowGraph
    {
        KRG_REGISTER_TYPE( EntryStateOverrideEditorGraph );

    public:

        EntryStateOverrideEditorGraph() : FlowGraph( GraphType::ValueTree ) {}
        virtual void Initialize( VisualGraph::BaseNode* pParentNode ) override;

    private:

        virtual void OnShowGraph() override;
    };

    //-------------------------------------------------------------------------
    // TRANSITIONS
    //-------------------------------------------------------------------------

    namespace GraphNodes
    {
        // The result node for a transition
        class TransitionEditorNode : public EditorGraphNode
        {
            friend class StateMachineEditorNode;
            KRG_REGISTER_TYPE( TransitionEditorNode );

        public:

            virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

            virtual char const* GetDisplayName() const override { return m_name.c_str(); }
            virtual char const* GetTypeName() const override { return "Transition"; }
            virtual char const* GetCategory() const override { return "Transitions"; }
            virtual bool IsUserCreatable() const override { return true; }
            virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::TransitionTree ); }
            virtual void DrawInfoText( VisualGraph::DrawContext const& ctx ) override;

        protected:

            KRG_EXPOSE String                                       m_name = "Transition";
            KRG_EXPOSE Math::Easing::Type                           m_blendWeightEasingType = Math::Easing::Type::Linear;
            KRG_EXPOSE RootMotionBlendMode                          m_rootMotionBlend = RootMotionBlendMode::Blend;
            KRG_EXPOSE Seconds                                      m_duration = 0.3f;
            KRG_EXPOSE float                                        m_syncEventOffset = 0.0f;
            KRG_EXPOSE bool                                         m_isSynchronized = false;
            KRG_EXPOSE bool                                         m_clampDurationToSource = true;
            KRG_EXPOSE bool                                         m_keepSourceSyncEventIdx = false;
            KRG_EXPOSE bool                                         m_keepSourceSyncEventPercentageThrough = false;
            KRG_EXPOSE bool                                         m_canBeForced = false;
        };

        // Transition conduit
        class TransitionConduitEditorNode final : public VisualGraph::SM::TransitionConduit
        {
            KRG_REGISTER_TYPE( TransitionConduitEditorNode );

        public:

            bool HasTransitions() const;

            virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;
            virtual char const* GetTypeName() const override { return "Transition"; }
            virtual ImColor GetNodeBorderColor( VisualGraph::DrawContext const& ctx, VisualGraph::NodeVisualState visualState ) const override;
        };
    }

    //-------------------------------------------------------------------------

     // The Flow graph for the state machine node
    class GlobalTransitionsEditorGraph final : public FlowGraph
    {
        KRG_REGISTER_TYPE( GlobalTransitionsEditorGraph );

    public:

        GlobalTransitionsEditorGraph() : FlowGraph( GraphType::ValueTree ) {}
        void UpdateGlobalTransitionNodes();

    private:

        virtual void OnShowGraph() override { UpdateGlobalTransitionNodes(); }
    };

    namespace GraphNodes
    {
        // The result node for a global transition
        class GlobalTransitionEditorNode final : public TransitionEditorNode
        {
            friend GlobalTransitionsEditorGraph;
            friend StateMachineGraph;
            KRG_REGISTER_TYPE( GlobalTransitionEditorNode );

        public:

            inline UUID const& GetEndStateID() const { return m_stateID; }

            virtual char const* GetDisplayName() const override { return m_name.c_str(); }
            virtual char const* GetTypeName() const override { return "Global Transition"; }
            virtual char const* GetCategory() const override { return "State Machine"; }
            virtual bool IsUserCreatable() const override { return false; }
            virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::ValueTree ); }

        private:

            KRG_REGISTER UUID                                       m_stateID;
        };

        // The global transition node present in state machine graphs
        class GlobalTransitionConduitEditorNode final : public VisualGraph::SM::Node
        {
            KRG_REGISTER_TYPE( GlobalTransitionConduitEditorNode );

        public:

            virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        private:

            virtual bool IsVisibleNode() const override { return false; }
            virtual ImColor GetNodeTitleColor() const override { return ImGuiX::ConvertColor( Colors::OrangeRed ); }
            virtual char const* GetTypeName() const override { return "Global Transitions"; }
            virtual bool IsUserCreatable() const override { return false; }
        };
    }

    //-------------------------------------------------------------------------
    // STATE MACHINE
    //-------------------------------------------------------------------------

    // The animation state machine graph
    class StateMachineGraph final : public VisualGraph::StateMachineGraph
    {
        KRG_REGISTER_TYPE( StateMachineGraph );

    public:

        void CreateNewState( ImVec2 const& mouseCanvasPos );
        void CreateNewOffState( ImVec2 const& mouseCanvasPos );

        GraphNodes::GlobalTransitionConduitEditorNode const* GetGlobalTransitionConduit() const { return m_pGlobalTransitionConduit; }
        GraphNodes::GlobalTransitionConduitEditorNode* GetGlobalTransitionConduit() { return m_pGlobalTransitionConduit; }

        GraphNodes::EntryStateOverrideConduitEditorNode const* GetEntryStateOverrideConduit() const { return m_pEntryOverridesConduit; }
        GraphNodes::EntryStateOverrideConduitEditorNode* GetEntryStateOverrideConduit() { return m_pEntryOverridesConduit; }

        bool HasGlobalTransitionForState( UUID const& stateID ) const;
        GraphNodes::EditorGraphNode const* GetEntryConditionNodeForState( UUID const& stateID ) const;

    private:

        void UpdateDependentNodes();

        virtual void Initialize( VisualGraph::BaseNode* pParentNode ) override;
        virtual bool CanDeleteNode( VisualGraph::BaseNode const* pNode ) const override;
        virtual UUID RegenerateIDs( THashMap<UUID, UUID>& IDMapping ) override;
        virtual VisualGraph::SM::TransitionConduit* CreateTransitionNode() const override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;
        virtual void PostPasteNodes( TInlineVector<VisualGraph::BaseNode*, 20> const& pastedNodes ) override;

    private:

        GraphNodes::EntryStateOverrideConduitEditorNode*            m_pEntryOverridesConduit = nullptr;
        GraphNodes::GlobalTransitionConduitEditorNode*              m_pGlobalTransitionConduit = nullptr;
    };

    namespace GraphNodes
    {
        // The state machine node shown in blend trees
        class StateMachineEditorNode final : public EditorGraphNode
        {
            KRG_REGISTER_TYPE( StateMachineEditorNode );

        public:

            virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

            virtual char const* GetDisplayName() const override { return m_name.c_str(); }
            virtual char const* GetTypeName() const override { return "State Machine"; }
            virtual char const* GetCategory() const override { return ""; }
            virtual ImColor GetNodeTitleColor() const override { return ImGuiX::ConvertColor( Colors::CornflowerBlue ); }

            virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
            virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

        private:

            GraphNodeIndex CompileState( EditorGraphCompilationContext& context, StateBaseEditorNode const* pBaseStateNode ) const;
            GraphNodeIndex CompileTransition( EditorGraphCompilationContext& context, TransitionEditorNode const* pTransitionNode, GraphNodeIndex targetStateNodeIdx ) const;

        private:

            KRG_EXPOSE String m_name = "SM";
        };
    }
}
#pragma once
#include "AnimationGraphTools_FlowNode.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class EntryStateOverrideNode;
    class GlobalTransitionsToolsNode;

    //-------------------------------------------------------------------------

    class StateMachineToolsGraph final : public GraphEditor::StateMachineGraph
    {
        friend class StateMachineToolsNode;
        KRG_REGISTER_TYPE( StateMachineToolsGraph );

    public:

        void CreateNewState( ImVec2 const& mouseCanvasPos );

    private:

        virtual void Initialize( GraphEditor::BaseNode* pParentNode ) override;
        virtual bool CanDeleteNode( GraphEditor::BaseNode const* pNode ) const override;
        virtual void PostDestroyNode( UUID const& nodeID ) override;

        // Updates the global transitions and entry state override nodes
        void UpdateDependentNodes();

        virtual GraphEditor::SM::TransitionConduit* CreateTransitionNode() const override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;

    private:

        EntryStateOverrideNode*         m_pEntryOverridesNode = nullptr;
        GlobalTransitionsToolsNode*     m_pGlobalTransitionsNode = nullptr;
    };
}
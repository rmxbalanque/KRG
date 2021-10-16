#pragma once
#include "AnimationToolsGraph_FlowNode.h"
#include "Tools/Core/GraphEditor/StateMachine/StateMachineGraph.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_EntryStateOverrideNode;
    class Tools_GlobalTransitionsNode;

    //-------------------------------------------------------------------------

    class StateMachineGraph final : public GraphEditor::StateMachineGraph
    {
        friend class Tools_StateMachineNode;
        KRG_REGISTER_TYPE( StateMachineGraph );

    public:

        void CreateNewState( ImVec2 const& mouseCanvasPos );
        void CreateNewOffState( ImVec2 const& mouseCanvasPos );

        Tools_GlobalTransitionsNode const* GetGlobalTransitionsNode() const { return m_pGlobalTransitionsNode; }

    private:

        virtual void Initialize( GraphEditor::BaseNode* pParentNode ) override;
        virtual bool CanDeleteNode( GraphEditor::BaseNode const* pNode ) const override;

        virtual GraphEditor::SM::TransitionConduit* CreateTransitionNode() const override;
        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;

        void UpdateDependentNodes();

    private:

        Tools_EntryStateOverrideNode*         m_pEntryOverridesNode = nullptr;
        Tools_GlobalTransitionsNode*          m_pGlobalTransitionsNode = nullptr;
    };
}
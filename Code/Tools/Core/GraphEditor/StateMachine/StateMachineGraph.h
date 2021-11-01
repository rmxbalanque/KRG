#pragma once

#include "../GraphEditor.h"

//-------------------------------------------------------------------------

namespace KRG::GraphEditor
{
    class StateMachineGraph;
    class StateMachineGraphView;

    //-------------------------------------------------------------------------

    namespace SM
    {
        class KRG_TOOLS_CORE_API Node : public BaseNode
        {
            friend StateMachineGraph;
            friend StateMachineGraphView;

            KRG_REGISTER_TYPE( Node );
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_CORE_API State : public Node
        {
            friend StateMachineGraph;
            friend StateMachineGraphView;

            KRG_REGISTER_TYPE( State );
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_CORE_API TransitionConduit : public Node
        {
            friend StateMachineGraph;
            friend StateMachineGraphView;

            KRG_REGISTER_TYPE( TransitionConduit );

            inline UUID const GetStartStateID() const { return m_startStateID; }
            inline UUID const GetEndStateID() const { return m_endStateID; }

        private:

            KRG_REGISTER UUID   m_startStateID;
            KRG_REGISTER UUID   m_endStateID;
        };
    }

    //-------------------------------------------------------------------------

    class KRG_TOOLS_CORE_API StateMachineGraph : public BaseGraph
    {
        KRG_REGISTER_TYPE( StateMachineGraph );

        friend StateMachineGraphView;

    public:

        inline UUID const& GetDefaultEntryStateID() const { return m_entryStateID; }
        void SetDefaultEntryState( UUID const& newDefaultEntryStateID );

        // Transitions
        //-------------------------------------------------------------------------

        bool DoesTransitionConduitExist( SM::State const* pStartState, SM::State const* pEndState ) const;
        bool CanCreateTransitionConduit( SM::State const* pStartState, SM::State const* pEndState ) const;

        void CreateTransitionConduit( SM::State const* pStartState, SM::State const* pEndState );
        void DestroyAllTransitionConduitsForState( SM::State const* pState );

    protected:

        void UpdateEntryState();

        virtual void PreDestroyNode( BaseNode* pNodeAboutToBeDestroyed ) override;
        virtual void PostDestroyNode( UUID const& nodeID ) override;

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;

    protected:

        // Users need to implement this function to create the appropriate transition node
        virtual SM::TransitionConduit* CreateTransitionNode() const = 0;

    protected:

        KRG_REGISTER UUID   m_entryStateID;
    };
}
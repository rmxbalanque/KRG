#pragma once

#include "VisualGraph_BaseGraph.h"

//-------------------------------------------------------------------------

namespace KRG::VisualGraph
{
    class StateMachineGraph;
    class GraphView;

    //-------------------------------------------------------------------------

    namespace SM
    {
        class KRG_TOOLS_CORE_API Node : public BaseNode
        {
            friend StateMachineGraph;
            friend GraphView;

            KRG_REGISTER_TYPE( Node );
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_CORE_API State : public Node
        {
            friend StateMachineGraph;
            friend GraphView;

            KRG_REGISTER_TYPE( State );
        };

        //-------------------------------------------------------------------------

        class KRG_TOOLS_CORE_API TransitionConduit : public Node
        {
            friend StateMachineGraph;
            friend GraphView;

            KRG_REGISTER_TYPE( TransitionConduit );

            // Conduits are special since they are drawn as connections
            virtual ImColor GetNodeBorderColor( VisualGraph::DrawContext const& ctx, NodeVisualState visualState ) const override;

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

        friend GraphView;

    public:

        inline UUID const& GetDefaultEntryStateID() const { return m_entryStateID; }
        void SetDefaultEntryState( UUID const& newDefaultEntryStateID );

        // Transitions
        //-------------------------------------------------------------------------

        bool DoesTransitionConduitExist( SM::State const* pStartState, SM::State const* pEndState ) const;
        bool CanCreateTransitionConduit( SM::State const* pStartState, SM::State const* pEndState ) const;
        void CreateTransitionConduit( SM::State const* pStartState, SM::State const* pEndState );

    protected:

        virtual BaseNode const* GetMostSignificantNode() const override { return FindNode( m_entryStateID ); }

        void UpdateEntryState();

        virtual UUID RegenerateIDs( THashMap<UUID, UUID>& IDMapping ) override;

        virtual bool CanCreateNode( TypeSystem::TypeInfo const* pNodeTypeInfo ) const override { return pNodeTypeInfo->IsDerivedFrom( SM::Node::GetStaticTypeID() ); }

        virtual void PreDestroyNode( BaseNode* pNodeAboutToBeDestroyed ) override;
        virtual void PostDestroyNode( UUID const& nodeID ) override;

        virtual void SerializeCustom( TypeSystem::TypeRegistry const& typeRegistry, RapidJsonValue const& graphObjectValue ) override;

    protected:

        // Users need to implement this function to create the appropriate transition node
        virtual SM::TransitionConduit* CreateTransitionNode() const = 0;

    protected:

        KRG_REGISTER UUID       m_entryStateID;
    };
}
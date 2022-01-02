#pragma once
#include "Tools/Animation/GraphEditor/EditorGraph/Animation_EditorGraph_FlowGraph.h"
#include "Engine/Physics/PhysicsRagdoll.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::GraphNodes
{
    class PoweredRagdollEditorNode final : public DataSlotEditorNode
    {
        KRG_REGISTER_TYPE( PoweredRagdollEditorNode );

    public:

        virtual void Initialize( VisualGraph::BaseGraph* pParent ) override;

        virtual char const* GetTypeName() const override { return "Powered Ragdoll"; }
        virtual char const* GetCategory() const override { return "Physics"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual GraphNodeIndex Compile( EditorGraphCompilationContext& context ) const override;

        virtual ResourceTypeID GetSlotResourceTypeID() const override { return Physics::RagdollDefinition::GetStaticResourceTypeID(); }

    private:

        KRG_EXPOSE StringID                m_profileID;
        KRG_EXPOSE float                   m_physicsBlendWeight = 1.0f;
        KRG_EXPOSE bool                    m_isGravityEnabled = false;
    };
}
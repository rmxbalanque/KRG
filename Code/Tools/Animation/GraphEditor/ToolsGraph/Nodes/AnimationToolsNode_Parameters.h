#pragma once
#include "../AnimationToolsGraph_FlowNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class Tools_ControlParameterNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ControlParameterNode );
        friend class AnimationToolsGraph;

    public:

        Tools_ControlParameterNode() = default;
        Tools_ControlParameterNode( String const& name, ValueType type );

        virtual void Initialize( GraphEditor::BaseGraph* pParentGraph ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Control Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual bool IsPersistentNode() const override { return true; }

    private:

        KRG_REGISTER String                     m_name;
        KRG_REGISTER ValueType                  m_type = ValueType::Float;
    };

    //-------------------------------------------------------------------------

    class Tools_ControlParameterReferenceNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_ControlParameterReferenceNode );
        friend class AnimationToolsGraph;

    public:

        Tools_ControlParameterReferenceNode() = default;
        Tools_ControlParameterReferenceNode( Tools_ControlParameterNode const* pParameter );

        virtual void Initialize( GraphEditor::BaseGraph* pParentGraph ) override;

        inline Tools_ControlParameterNode const* GetReferencedParameter() const { return m_pParameter; }
        inline UUID const& GetReferencedParameterID() const { return m_parameterUUID; }

        virtual char const* GetDisplayName() const override { return m_pParameter->GetDisplayName(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Control Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual bool IsDestroyable() const override { return true; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        Tools_ControlParameterNode const*   m_pParameter = nullptr;
        KRG_REGISTER UUID                       m_parameterUUID;
    };

    //-------------------------------------------------------------------------

    class Tools_VirtualParameterNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_VirtualParameterNode );
        friend class AnimationToolsGraph;

    public:

        Tools_VirtualParameterNode() = default;
        Tools_VirtualParameterNode( String const& name, ValueType type );

        virtual void Initialize( GraphEditor::BaseGraph* pParentGraph ) override;

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Virtual Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        KRG_REGISTER String                     m_name;
        KRG_REGISTER ValueType                  m_type = ValueType::Float;
    };

    //-------------------------------------------------------------------------

    class Tools_VirtualParameterReferenceNode final : public Tools_GraphNode
    {
        KRG_REGISTER_TYPE( Tools_VirtualParameterReferenceNode );
        friend class AnimationToolsGraph;

    public:

        Tools_VirtualParameterReferenceNode() = default;
        Tools_VirtualParameterReferenceNode( Tools_VirtualParameterNode const* pParameter );

        virtual void Initialize( GraphEditor::BaseGraph* pParentGraph ) override;

        inline Tools_VirtualParameterNode const* GetReferencedParameter() const { return m_pParameter; }
        inline UUID const& GetReferencedParameterID() const { return m_parameterUUID; }

        virtual char const* GetDisplayName() const override { return m_pParameter->GetDisplayName(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Virtual Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual bool IsDestroyable() const override { return true; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        Tools_VirtualParameterNode const*       m_pParameter = nullptr;
        KRG_REGISTER UUID                           m_parameterUUID;
    };
}
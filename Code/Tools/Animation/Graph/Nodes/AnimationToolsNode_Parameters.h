#pragma once
#include "../AnimationGraphTools_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ControlParameterToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ControlParameterToolsNode );
        friend class ToolsAnimationGraph;

    public:

        ControlParameterToolsNode() = default;
        ControlParameterToolsNode( String const& name, NodeValueType type );

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Control Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
        virtual bool IsPersistentNode() const override { return true; }

    private:

        REGISTER String m_name;
    };

    //-------------------------------------------------------------------------

    class ControlParameterReferenceToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ControlParameterReferenceToolsNode );
        friend class ToolsAnimationGraph;

    public:

        ControlParameterReferenceToolsNode() = default;
        ControlParameterReferenceToolsNode( ControlParameterToolsNode const* pParameter );

        inline ControlParameterToolsNode const* GetReferencedParameter() const { return m_pParameter; }
        inline UUID const& GetReferencedParameterID() const { return m_parameterUUID; }

        virtual char const* GetDisplayName() const override { return m_pParameter->GetDisplayName(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Control Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        ControlParameterToolsNode const*        m_pParameter = nullptr;
        REGISTER UUID                           m_parameterUUID;
    };

    //-------------------------------------------------------------------------

    class VirtualParameterToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( VirtualParameterToolsNode );
        friend class ToolsAnimationGraph;

    public:

        VirtualParameterToolsNode() = default;
        VirtualParameterToolsNode( String const& name, NodeValueType type );

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Virtual Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        REGISTER String m_name;
    };

    //-------------------------------------------------------------------------

    class VirtualParameterReferenceToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( VirtualParameterReferenceToolsNode );
        friend class ToolsAnimationGraph;

    public:

        VirtualParameterReferenceToolsNode() = default;
        VirtualParameterReferenceToolsNode( VirtualParameterToolsNode const* pParameter );

        inline VirtualParameterToolsNode const* GetReferencedParameter() const { return m_pParameter; }
        inline UUID const& GetReferencedParameterID() const { return m_parameterUUID; }

        virtual char const* GetDisplayName() const override { return m_pParameter->GetDisplayName(); }
        virtual char const* GetTypeName() const override { return "Parameter"; }
        virtual char const* GetCategory() const override { return "Virtual Parameters"; }
        virtual bool IsUserCreatable() const override { return false; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree, GraphType::ValueTree, GraphType::TransitionTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        VirtualParameterToolsNode const*        m_pParameter = nullptr;
        REGISTER UUID                           m_parameterUUID;
    };
}
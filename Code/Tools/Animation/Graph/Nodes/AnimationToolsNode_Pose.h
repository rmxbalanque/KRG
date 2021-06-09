#pragma once
#include "../AnimationGraphTools_Node.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ZeroPoseToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ZeroPoseToolsNode );

    public:

        ZeroPoseToolsNode();

        virtual char const* GetTypeName() const override { return "Zero Pose"; }
        virtual char const* GetCategory() const override { return "Poses"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class ReferencePoseToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( ReferencePoseToolsNode );

    public:

        ReferencePoseToolsNode();

        virtual char const* GetTypeName() const override { return "Reference Pose"; }
        virtual char const* GetCategory() const override { return "Poses"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;
    };

    //-------------------------------------------------------------------------

    class AnimationPoseToolsNode final : public ToolsNode
    {
        KRG_REGISTER_TYPE( AnimationPoseToolsNode );

    public:

        AnimationPoseToolsNode();

        virtual char const* GetDisplayName() const override { return m_name.c_str(); }
        virtual char const* GetTypeName() const override { return "Animation Pose"; }
        virtual char const* GetCategory() const override { return "Poses"; }
        virtual TBitFlags<GraphType> GetAllowedParentGraphTypes() const override { return TBitFlags<GraphType>( GraphType::BlendTree ); }
        virtual NodeIndex Compile( ToolsGraphCompilationContext& context ) const override;

    private:

        EXPOSE String       m_name = "Pose";
        EXPOSE FloatRange   m_inputTimeRemapRange = FloatRange( 0, 1 );
    };
}
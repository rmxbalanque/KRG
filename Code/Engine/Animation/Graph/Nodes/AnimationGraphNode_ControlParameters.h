#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"
#include "Engine/Animation/AnimationBoneMask.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    //-------------------------------------------------------------------------
    // Control Parameters
    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterBool final : public ValueNodeBool
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBool::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        bool m_value = false;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterID final : public ValueNodeID
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeID::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        StringID m_value;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterInt final : public ValueNodeInt
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeInt::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        int32 m_value = 0;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterFloat final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        float m_value = 0.0f;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterVector final : public ValueNodeVector
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeVector::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        Vector m_value = Vector::Zero;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterTarget final : public ValueNodeTarget
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeTarget::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        Target m_value;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ControlParameterBoneMask final : public ValueNodeBoneMask
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBoneMask::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
        virtual void SetValueInternal( GraphContext& context, void const* pInValue ) override;

    private:

        BoneMask m_value;
    };

    //-------------------------------------------------------------------------
    // Virtual control parameters
    //-------------------------------------------------------------------------
    // NOTE: Virtual parameters are not allowed to cache values, as their result can change 

    class KRG_ENGINE_ANIMATION_API VirtualParameterBool final : public ValueNodeBool
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBool::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeBool::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeBool* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterID final : public ValueNodeID
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeID::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeID::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeID* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterInt final : public ValueNodeInt
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeInt::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeInt::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeInt* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------
     
    class KRG_ENGINE_ANIMATION_API VirtualParameterFloat final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeFloat* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterVector final : public ValueNodeVector
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeVector::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeVector::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeVector* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterTarget final : public ValueNodeTarget
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeTarget::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeTarget::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeTarget* m_pChildNode = nullptr;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API VirtualParameterBoneMask final : public ValueNodeBoneMask
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBoneMask::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeBoneMask::Settings, m_childNodeIdx );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            NodeIndex m_childNodeIdx = InvalidIndex;
        };

    private:

        virtual void InitializeInternal( GraphContext& context ) override;
        virtual void ShutdownInternal( GraphContext& context ) override;
        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;

    private:

        ValueNodeBoneMask* m_pChildNode = nullptr;
    };
}
#pragma once
#include "Engine/Animation/Graph/AnimationGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class KRG_ENGINE_ANIMATION_API ConstValueBool final : public ValueNodeBool
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeBool::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeBool::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            bool m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstValueID final : public ValueNodeID
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeID::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeID::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            StringID m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstValueInt final : public ValueNodeInt
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeInt::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeInt::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            int32 m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstValueFloat final : public ValueNodeFloat
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeFloat::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeFloat::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            float m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstValueVector final : public ValueNodeVector
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeVector::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeVector::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            Vector m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class KRG_ENGINE_ANIMATION_API ConstValueTarget final : public ValueNodeTarget
    {
    public:

        struct KRG_ENGINE_ANIMATION_API Settings final : public ValueNodeTarget::Settings
        {
            KRG_REGISTER_TYPE( Settings );
            KRG_SERIALIZE_GRAPHNODESETTINGS( ValueNodeTarget::Settings, m_value );

            virtual void InstantiateNode( TVector<GraphNode*> const& nodePtrs, InitOptions options ) const override;

            Target m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };
}
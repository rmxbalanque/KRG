#pragma once
#include "Engine/Animation/Graph/AnimGraphNode.h"

//-------------------------------------------------------------------------

namespace KRG::Animation::Graph
{
    class ConstValueBool : public ValueNodeBool
    {
    public:

        struct Settings : public ValueNodeBool::Settings
        {
            bool m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class ConstValueID : public ValueNodeID
    {
    public:

        struct Settings : public ValueNodeID::Settings
        {
            StringID m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class ConstValueInt : public ValueNodeInt
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            int32 m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class ConstValueFloat : public ValueNodeFloat
    {
    public:

        struct Settings : public ValueNodeFloat::Settings
        {
            float m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class ConstValueVector : public ValueNodeVector
    {
    public:

        struct Settings : public ValueNodeVector::Settings
        {
            Vector m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };

    //-------------------------------------------------------------------------

    class ConstValueTarget : public ValueNodeTarget
    {
    public:

        struct Settings : public ValueNodeTarget::Settings
        {
            Target m_value;
        };

    private:

        virtual void GetValueInternal( GraphContext& context, void* pOutValue ) override;
    };
}
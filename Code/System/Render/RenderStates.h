#pragma once

#include "System/Core/Math/Math.h"
#include "RenderResourceHandle.h"

//-------------------------------------------------------------------------

namespace KRG
{
    namespace Render
    {
        struct ScissorRect
        {
            int32 m_left;
            int32 m_top;
            int32 m_right;
            int32 m_bottom;
        };

        enum class Topology : uint8
        {
            PointList = 0,
            LineList,
            LineStrip,
            TriangleList,
            TriangleStrip,

            None,
        };

        enum class WindingMode : uint8
        {
            Clockwise = 0,
            CounterClockwise,
        };

        enum class CullMode : uint8
        {
            BackFace = 0,
            FrontFace,
            None,
        };

        enum class FillMode : uint8
        {
            Solid = 0,
            Wireframe,
        };

        enum class BlendValue : uint8
        {
            Zero = 0,
            One,
            SourceColor,
            InverseSourceColor,
            SourceAlpha,
            InverseSourceAlpha,
            DestinationColor,
            InverseDestinationColor,
            DestinationAlpha,
            InverseDestinationAlpha,
            SourceAlphaSaturated,
            BlendFactor,
            InverseBlendFactor,
            Source1Color,
            InverseSource1Color,
            Source1Alpha,
            InverseSource1Alpha,
        };

        enum class BlendOp : uint8
        {
            Add = 0,
            SourceMinusDestination,
            DestinationMinusSource,
            Min,
            Max,
        };

        enum class DepthTestMode : uint8
        {
            On = 0,
            Off,
            ReadOnly,
        };

        //-------------------------------------------------------------------------

        struct RasterizerState
        {
            friend class RenderDevice;

            ~RasterizerState() { KRG_ASSERT( !m_resourceHandle.IsValid() ); }

            inline bool IsValid() const { return m_resourceHandle.IsValid(); }

            ResourceHandle const& GetResourceHandle() const { return m_resourceHandle; }

        public:

            CullMode                m_cullMode = CullMode::BackFace;
            FillMode                m_fillMode = FillMode::Solid;
            WindingMode             m_windingMode = WindingMode::CounterClockwise;
            bool                    m_scissorCulling = false;

        private:

            ResourceHandle          m_resourceHandle;
        };

        //-------------------------------------------------------------------------

        struct BlendState
        {
            friend class RenderDevice;

            ~BlendState() { KRG_ASSERT( !m_resourceHandle.IsValid() ); }

            inline bool IsValid() const { return m_resourceHandle.IsValid(); }

            ResourceHandle const& GetResourceHandle() const { return m_resourceHandle; }

        public:

            BlendValue              m_srcValue = BlendValue::One;
            BlendValue              m_dstValue = BlendValue::Zero;
            BlendOp                 m_blendOp = BlendOp::Add;
            BlendValue              m_srcAlphaValue = BlendValue::One;
            BlendValue              m_dstAlphaValue = BlendValue::Zero;
            BlendOp                 m_blendOpAlpha = BlendOp::Add;
            bool                    m_blendEnable = false;

        private:

            ResourceHandle          m_resourceHandle;
        };
    }
}
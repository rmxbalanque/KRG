#include "DebugShaderCommon.hlsli"

PixelShaderOutput main( VertexShaderOutput input )
{
    PixelShaderOutput output;
    output.m_color = input.m_color;
    return output;
}
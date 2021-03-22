#include "DebugShaderCommon.hlsli"

PixelShaderOutput main( VertexShaderOutput input )
{
    float d = abs( input.m_edgeDistance ) / input.m_size;
    d = smoothstep( 1.0, 1.0 - ( kAntialiasing / input.m_size ), d );

    PixelShaderOutput output;
    output.m_color = input.m_color;
    output.m_color.a *= d;
    return output;
}
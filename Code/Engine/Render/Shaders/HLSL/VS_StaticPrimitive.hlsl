cbuffer PrimitiveTransform
{
    matrix transform;
};

struct VertexShaderInput
{
    float4 m_pos : POSITION;
    float4 m_normal : NORMAL;
    float2 m_uv0 : TEXCOORD0;
    float2 m_uv1 : TEXCOORD1;
};

struct PixelShaderInput
{
    float4 m_pos : SV_POSITION;
    float2 m_uv : TEXCOORD;
};
 
PixelShaderInput main( VertexShaderInput input )
{
    PixelShaderInput psInput;
    psInput.m_pos = mul( transform, input.m_pos );
    psInput.m_uv = input.m_uv0;
    return psInput;
}
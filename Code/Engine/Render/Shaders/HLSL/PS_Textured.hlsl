struct PixelShaderInput
{
    float4 m_pos : SV_POSITION;
    float2 m_uv : TEXCOORD;
};

sampler sampler0;
Texture2D texture0;

float4 main( PixelShaderInput psInput ) : SV_TARGET
{
    return texture0.Sample( sampler0, psInput.m_uv );
}
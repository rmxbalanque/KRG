cbuffer ViewData : register( b0 )
{
    float4x4        g_worldViewProjectionMatrix;
    matrix          m_boneTransforms[255];
};

struct VertexShaderInput
{
    float4 m_pos : POSITION;
    float4 m_normal : NORMAL;
    float2 m_uv0 : TEXCOORD0;
    float2 m_uv1 : TEXCOORD1;
    int4   m_boneIndices : BLENDINDICES;
    float4 m_boneWeights : BLENDWEIGHTS;
};

struct PixelShaderInput
{
    float4 m_pos : SV_POSITION;
    float2 m_uv : TEXCOORD;
};

PixelShaderInput main( VertexShaderInput vs )
{
    PixelShaderInput psInput;

    float4 blendPos = float4( 0, 0, 0, 0 );
    for ( int i = 0; i < 4; ++i )
    {
        if ( vs.m_boneIndices[i] != -1 )
        {
            matrix boneTransform = m_boneTransforms[vs.m_boneIndices[i]];
            blendPos += mul( boneTransform, vs.m_pos ) * vs.m_boneWeights[i];
        }
    }

    psInput.m_pos = mul( g_worldViewProjectionMatrix, blendPos );
    psInput.m_uv = vs.m_uv0;
    return psInput;
}
// TODO convert to std: quas, fullscreen tri, cube, etc

TextureCube cubemap : register( t0 );
sampler bilinearSampler : register( s0 );

struct VerteShaderOutput
{
    float4 m_pos : SV_POSITION;
    float3 m_wpos : POSITION;
};

float4 main(VerteShaderOutput input) : SV_TARGET
{
	return cubemap.Sample(bilinearSampler, input.m_wpos.xzy); //TODO: fix cubemap generation
}
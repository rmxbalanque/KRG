// TODO convert to std: quads, fullscreen tri, cube, etc
cbuffer Transforms : register( b0 )
{
    matrix m_viewprojTransform;
};

struct VerteShaderOutput
{
    float4 m_pos : SV_POSITION;
    float3 m_wpos : POSITION;
};

VerteShaderOutput main( uint i : SV_VertexID )
{
    VerteShaderOutput output;

    float3 vertex;
    vertex.x = (0x287A >> i) & 1; //TODO : check constants
    vertex.y = (0x3D50 >> i) & 1; //TODO : check constants
    vertex.z = (0x31E3 >> i) & 1; //TODO : check constants

    output.m_wpos = vertex * 2 - 1;

    output.m_pos = mul( m_viewprojTransform, float4(output.m_wpos, 1.0) );

return output;
}
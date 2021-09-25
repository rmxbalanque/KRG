#define kAntialiasing 2.0

struct VertexShaderInput
{
    float4                  m_positionSize  : POSITION;
    float4                  m_color         : COLOR;
};

struct VertexShaderOutput
{
    linear        float4    m_position      : SV_POSITION;
    linear        float4    m_color         : COLOR;
    linear        float2    m_uv            : TEXCOORD;
    noperspective float     m_size          : SIZE;
    noperspective float     m_edgeDistance  : EDGE_DISTANCE;
};

struct PixelShaderOutput
{
    float4                  m_color         : SV_TARGET0;
};
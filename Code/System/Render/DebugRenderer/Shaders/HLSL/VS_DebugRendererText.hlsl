cbuffer TextGlobalData
{
    float4      g_viewportSize;
};

struct VertexShaderInput
{
    float2 pos : POSITION;
    float2 uv  : TEXCOORD0;
    float4 col : COLOR0;
};

struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR0;
    float2 uv  : TEXCOORD0;
};

PixelShaderInput main( VertexShaderInput input )
{
    float4 pointCS = { 0.0f, 0.0f, 0.0f, 1.0f };

    // Convert to clipspace
    //-------------------------------------------------------------------------

    // To Normalized pixel space
    pointCS.x = input.pos.x / g_viewportSize.x;
    pointCS.y = input.pos.y / g_viewportSize.y;

    // Invert Y
    pointCS.y = 1.0f - pointCS.y;

    // Convert from [0,1] to [-1,1]
    pointCS.x = ( pointCS.x * 2 ) - 1.0f;
    pointCS.y = ( pointCS.y * 2 ) - 1.0f;

    // Set PS input
    //-------------------------------------------------------------------------

    PixelShaderInput output;
    output.pos = pointCS;
    output.col = input.col;
    output.uv = input.uv;
    return output;
}
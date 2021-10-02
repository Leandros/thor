cbuffer vertexBuffer : register(b0)
{
    float4x4 projMat;
};

struct VSInput
{
    float2 pos  : POSITION;
    float4 col  : COLOR0;
    float2 uv   : TEXCOORD0;
};

struct PSInput
{
    float4 pos  : SV_POSITION;
    float4 col  : COLOR0;
    float2 uv   : TEXCOORD0;
};

PSInput main(VSInput i)
{
    PSInput o;
    o.pos = mul(projMat, float4(i.pos.xy, 0.0f, 1.0f));
    o.col = i.col;
    o.uv = i.uv;
    return o;
}

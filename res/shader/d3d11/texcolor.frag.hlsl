struct PSInput
{
    float4 pos  : SV_POSITION;
    float4 col  : COLOR0;
    float2 uv   : TEXCOORD0;
};

sampler sampler0;
Texture2D texture0;

float4 main(PSInput i) : SV_TARGET
{
    float4 col = i.col * texture0.Sample(sampler0, i.uv);
    return col;
}


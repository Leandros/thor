/* Constant Input */
cbuffer VS_CONSTANT_BUFFER : register(b0)
{
    float4x4 Model;
    float4x4 ModelInv;
    float4x4 MVP;
    float3 ViewPos;
};

/* Input */
struct VSInput {
    float3 Pos : POSITION;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 Bitangent : TANGENT1;
};

/* Output */
struct PSInput {
    float4 Pos : SV_POSITION;
    float3 FragPos : POSITION0;
    float2 TexCoord : TEXCOORD0;
    float3 TangentViewPos : POSITION1;
    float3 TangentFragPos : POSITION2;
    float3x3 TBN : TBN;
};

PSInput main(VSInput In)
{
    PSInput Out;
    float4 Pos = float4(In.Pos.xyz, 1.0f);
    Out.Pos = mul(Pos, MVP);
    Out.FragPos = (float3)mul(Pos, Model);
    Out.TexCoord = In.TexCoord;

    float3x3 normalMatrix = transpose((float3x3)ModelInv);
    float3 tan = normalize(mul(In.Tangent, normalMatrix));
    float3 bitan = normalize(mul(In.Bitangent, normalMatrix));
    float3 norm = normalize(mul(In.Normal, normalMatrix));

    float3x3 tbn = transpose(float3x3(tan, bitan, norm));
    Out.TangentViewPos = mul(ViewPos, tbn);
    Out.TangentFragPos = mul(Out.FragPos, tbn);
    Out.TBN = tbn;

    return Out;
}


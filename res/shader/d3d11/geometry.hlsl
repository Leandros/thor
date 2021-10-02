#ifndef GEOMETRY_HLSL
#define GEOMETRY_HLSL

/* Input */
struct GeometryVSIn {
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

/* Output */
struct GeometryVSOut {
    float4 position : SV_POSITION;
    float3 positionView : POSITIONVIEW;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD;
};

/* Textures */
Texture2D gDiffuseTexture : register(t0);
SamplerState gDiffuseSampler : register(s0);

#endif /* GEOMETRY_HLSL */


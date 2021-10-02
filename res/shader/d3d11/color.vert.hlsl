
/* struct VSInput */
/* { */
/*     float4 Pos : POSITION; */
/*     float3 Normal : NORMAL; */
/*     float2 Texcoord : TEXCOORD0; */
/* }; */

/* struct PSInput */
/* { */
/*     float4 Pos : SV_Position; */
/*     float3 Normal : NORMAL; */
/*     float2 TexCoord : TEXCOORD0; */
/*     float4 WorldPos : POSITION; */
/* }; */


/* PSInput main(VSInput In) */
/* { */
/*     PSInput Out; */
/*     Out.Pos = float4(1.0, 1.0, 1.0, 1.0); */
/*     Out.Normal = float3(1.0, 1.0, 1.0); */
/*     Out.TexCoord = float2(1.0, 1.0); */
/*     Out.WorldPos = float4(1.0, 1.0, 1.0, 1.0); */

/*     return Out; */
/* } */

/* Most Minimal Example: */
float4 main(float4 pos : POSITION) : SV_POSITION
{
    return pos;
}


/* struct PSInput */
/* { */
/*     float4 Pos : SV_Position; */
/*     float3 Normal : NORMAL; */
/*     float2 Texcoord : TEXCOORD0; */
/*     float4 WorldPos : POSITION; */
/* }; */


/* float4 main(PSInput In) : SV_Target */
/* { */
/*     return float4(1.0, 0.0, 0.0, 1.0); */
/* } */

/* Most minimal example: */
float4 main() : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}


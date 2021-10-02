#ifndef UTILITIES_HLSL
#define UTILITIES_HLSL

#include "frameconstants.hlsl"

float2 EncodeSphereMap(float3 n)
{
    float oneMinusZ = 1.0f - n.z;
    float p = sqrt(n.x * n.x + n.y * n.y + oneMinusZ * oneMinusZ);
    return n.xy / p * 0.5f + 0.5f;
}

float3 DecodeSphereMap(float2 e)
{
    float2 tmp = e - e * e;
    float f = tmp.x + tmp.y;
    float m = sqrt(4.0f * f - 1.0f);

    float3 n;
    n.xy = m * (e * 4.0f - 2.0f);
    n.z  = 3.0f - 8.0f * f;
    return n;
}

float3 ComputeFaceNormal(float3 position)
{
    return cross(ddx_coarse(position), ddy_coarse(position));
}

float linstep(float min, float max, float v)
{
    return saturate((v - min) / (max - min));
}

/* ========================================================================= */
/* Packing / Unpacking */
float4 UnpackRGBA16(uint2 e)
{
    return float4(f16tof32(e), f16tof32(e >> 16));
}
uint2 PackRGBA16(float4 c)
{
    return f32tof16(c.rg) | (f32tof16(c.ba) << 16);
}

/* Linearize the given 2D address + sample index into our flat framebuffer array */
uint GetFramebufferSampleAddress(uint2 coords, uint sampleIndex)
{
    /* Major ordering: Row (x), Col (y), MSAA sample */
    /* return (sampleIndex * FramebufferDimensions.y + coords.y) */
    /*     * FramebufferDimensions.x + coords.x; */

    /* Major ordering: Col (y), Row (x), MSAA sample */
    return (sampleIndex * FramebufferDimensions.x + coords.x)
        * FramebufferDimensions.y + coords.y;
}

#endif /* UTILITIES_HLSL */


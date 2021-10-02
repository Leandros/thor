#ifndef GEOMETRY_VERT_HLSL
#define GEOMETRY_VERT_HLSL

#include "frameconstants.hlsl"
#include "geometry.hlsl"

/* ========================================================================= */
/* Geometry Phase                                                            */
/* ========================================================================= */
GeometryVSOut main(GeometryVSIn input)
{
    GeometryVSOut output;
    output.position = mul(float4(input.position, 1.0f), CameraWorldViewProj);
    output.positionView = mul(float4(input.position, 1.0f), CameraWorldView).xyz;
    output.normal = mul(float4(input.normal, 0.0f), CameraWorldView).xyz;
    output.texCoord = input.texCoord;

    return output;
}

#endif /* GEOMETRY_VERT_HLSL */


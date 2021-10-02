#ifndef GBUFFER_FRAG_HLSL
#define GBUFFER_FRAG_HLSL

#include "gbuffer.hlsl"

void main(GeometryVSOut input, out GBuffer output)
{
    GBufferPS(input, output);
}

#endif /* GBUFFER_FRAG_HLSL */


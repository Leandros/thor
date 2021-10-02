#ifndef GBUFFER_ALPHA_FRAG_HLSL
#define GBUFFER_ALPHA_FRAG_HLSL

#include "gbuffer.hlsl"

void main(GeometryVSOut input, out GBuffer output)
{
    GBufferPS(input, output);

    /* Alpha Test */
    clip(output.albedo.a - 0.3f);

    output.normal_specular.xy = EncodeSphereMap(
        normalize(ComputeFaceNormal(input.positionView)));
}

#endif /* GBUFFER_ALPHA_FRAG_HLSL */


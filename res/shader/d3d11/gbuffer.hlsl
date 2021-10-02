#ifndef GBUFFER_HLSL
#define GBUFFER_HLSL

#include "shaderconfig.hlsl"
#include "frameconstants.hlsl"
#include "utilities.hlsl"
#include "geometry.hlsl"


struct GBuffer {
    float4 normal_specular : SV_TARGET0;
    float4 albedo : SV_TARGET1;
    float2 positionZGrad : SV_TARGET2;
};

Texture2DMS<float4, MSAA_SAMPLES> gGBufferTextures[4] : register(t0);

/* Data that we can read or derive from the surface shader outputs */
struct SurfaceData {
    float3 positionView;         // View space position
    float3 positionViewDX;       // Screen space derivatives
    float3 positionViewDY;       // of view space position
    float3 normal;               // View space normal
    float4 albedo;
    float specularAmount;        // Treated as a multiplier on albedo
    float specularPower;
};

/* ========================================================================= */
/* GBuffer Rendering                                                         */
/* ========================================================================= */
SurfaceData ComputeSurfaceDataFromGeometry(GeometryVSOut input)
{
    SurfaceData surface;
    surface.positionView = input.positionView;

    /*
     * These arguably aren't really useful in this path since they are really only used to
     * derive shading frequencies and composite derivatives but might as well compute them
     * in case they get used for anything in the future.
     * (Like the rest of these values, they will get removed by dead code elimination if
     * they are unused.)
     */
    surface.positionViewDX = ddx_coarse(surface.positionView);
    surface.positionViewDY = ddy_coarse(surface.positionView);

    surface.normal = normalize(input.normal);

    surface.albedo = gDiffuseTexture.Sample(gDiffuseSampler, input.texCoord);

    /* Map NULL diffuse textures to white */
    uint2 textureDim;
    gDiffuseTexture.GetDimensions(textureDim.x, textureDim.y);
    surface.albedo = (textureDim.x == 0U ? float4(1.0f, 1.0f, 1.0f, 1.0f) : surface.albedo);

    /*
     * We don't really have art asset-related values for these, so set them to something
     * reasonable for now... the important thing is that they are stored in the G-buffer for
     * representative performance measurement.
     */
    surface.specularAmount = 0.9f;
    surface.specularPower = 25.0f;

    return surface;
}

float3 ComputePositionViewFromZ(float2 positionScreen,
                                float viewSpaceZ)
{
    float2 screenSpaceRay = float2(positionScreen.x / CameraProj._11,
                                   positionScreen.y / CameraProj._22);

    float3 positionView;
    positionView.z = viewSpaceZ;
    /* Solve the two projection equations */
    positionView.xy = screenSpaceRay.xy * positionView.z;

    return positionView;
}

SurfaceData ComputeSurfaceDataFromGBufferSample(uint2 positionViewport, uint sampleIndex)
{
    /* Load the raw data from the GBuffer */
    GBuffer rawData;
    rawData.normal_specular = gGBufferTextures[0].Load(positionViewport.xy, sampleIndex).xyzw;
    rawData.albedo = gGBufferTextures[1].Load(positionViewport.xy, sampleIndex).xyzw;
    rawData.positionZGrad = gGBufferTextures[2].Load(positionViewport.xy, sampleIndex).xy;
    float zBuffer = gGBufferTextures[3].Load(positionViewport.xy, sampleIndex).x;

    float2 gbufferDim;
    uint dummy;
    gGBufferTextures[0].GetDimensions(gbufferDim.x, gbufferDim.y, dummy);

    /*
     * Compute screen/clip-space position and neighbour positions
     * NOTE: Mind DX11 viewport transform and pixel center!
     * NOTE: This offset can actually be precomputed on the CPU but it's
     * actually slower to read it from a constant buffer than to just recompute it.
     */
    float2 screenPixelOffset = float2(2.0f, -2.0f) / gbufferDim;
    float2 positionScreen = (float2(positionViewport.xy) + 0.5f) * screenPixelOffset.xy + float2(-1.0f, 1.0f);
    float2 positionScreenX = positionScreen + float2(screenPixelOffset.x, 0.0f);
    float2 positionScreenY = positionScreen + float2(0.0f, screenPixelOffset.y);

    /* Decode into reasonable outputs */
    SurfaceData data;

    /* Unproject depth buffer Z value into view space */
    float viewSpaceZ = CameraProj._43 / (zBuffer - CameraProj._33);

    data.positionView = ComputePositionViewFromZ(positionScreen, viewSpaceZ);
    data.positionViewDX = ComputePositionViewFromZ(
        positionScreenX, viewSpaceZ + rawData.positionZGrad.x) - data.positionView;
    data.positionViewDY = ComputePositionViewFromZ(
        positionScreenY, viewSpaceZ + rawData.positionZGrad.y) - data.positionView;

    data.normal = DecodeSphereMap(rawData.normal_specular.xy);
    data.albedo = rawData.albedo;

    data.specularAmount = rawData.normal_specular.z;
    data.specularPower = rawData.normal_specular.w;

    return data;
}

void ComputeSurfaceDataFromGBufferAllSamples(uint2 positionViewport,
                                             out SurfaceData surface[MSAA_SAMPLES])
{
    /* Load data for each sample */
    /* Most of this time only a small amount of this data is actually used so unrolling */
    /* this loop ensures that the compiler has an easy time with the dead code elimination. */
    [unroll] for (uint i = 0; i < MSAA_SAMPLES; ++i) {
        surface[i] = ComputeSurfaceDataFromGBufferSample(positionViewport, i);
    }
}

bool RequiresPerSampleShading(SurfaceData surface[MSAA_SAMPLES])
{
    bool perSample = false;
    const float maxZDelta = abs(surface[0].positionViewDX.z) + abs(surface[0].positionViewDY.z);
    const float minNormalDot = 0.99f;   /* Allow ~8 degree normal deviations */

    [unroll] for (uint i = 1; i < MSAA_SAMPLES; ++i) {
        /* Using the position derivatives of the triangle, check if all of the sample depths */
        /* could possibly have come from the same triangle/surface */
        perSample = perSample ||
            abs(surface[i].positionView.z - surface[0].positionView.z) > maxZDelta;

        /* Also flag places where the normal is different */
        perSample = perSample ||
            dot(surface[i].normal, surface[0].normal) < minNormalDot;
    }

    return perSample;
}

void GBufferPS(GeometryVSOut input, out GBuffer output)
{
    SurfaceData surface = ComputeSurfaceDataFromGeometry(input);
    output.normal_specular = float4(EncodeSphereMap(surface.normal),
                                surface.specularAmount,
                                surface.specularPower);
    output.albedo = surface.albedo;
    output.positionZGrad = float2(ddx_coarse(surface.positionView.z),
                                  ddy_coarse(surface.positionView.z));
}

#endif /* GBUFFER_HLSL */


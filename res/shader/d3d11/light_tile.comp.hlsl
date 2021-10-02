#ifndef LIGHT_TILE_COMP_HLSL
#define LIGHT_TILE_COMP_HLSL

#include "shaderconfig.hlsl"
#include "utilities.hlsl"
#include "lighting.hlsl"
#include "gbuffer.hlsl"

RWStructuredBuffer<uint2> gFramebuffer : register(u0);

groupshared uint minZ;
groupshared uint maxZ;

/* Light list for this tile. */
groupshared uint tileLightIndices[MAX_LIGHTS];
groupshared uint tileNumLights;

/*
 * List of pixels that require per-sample shading.
 * We encode two 16-bit x/y coordinates into one uint, to save memory.
 */
groupshared uint perSamplePixels[CSHADER_TILE_GROUP_SIZE];
groupshared uint numPerSamplePixels;


/* ========================================================================= */
/* Utilities */
void WriteSample(uint2 coords, uint2 sampleIndex, float4 value)
{
    uint idx = GetFramebufferSampleAddress(coords, sampleIndex.x);
    gFramebuffer[idx] = PackRGBA16(value);
}

/* Pack / Unpack two <= 16-bit coordinates into a single uint. */
uint PackCoords(uint2 coords)
{
    return coords.y << 16 | coords.x;
}

uint2 UnpackCoords(uint coords)
{
    return uint2(coords & 0xFFFF, coords >> 16);
}


/* ========================================================================= */
/* Compute Shader                                                            */
/* ========================================================================= */
[numthreads(CSHADER_TILE_GROUP_DIM, CSHADER_TILE_GROUP_DIM, 1)]
void main(uint3 groupID : SV_GroupID,
          uint3 dispatchThreadID : SV_DispatchThreadID,
          uint3 groupThreadID : SV_GroupThreadID)
{
    /*
     * NOTE: This is currently necessary rather than just using SV_GroupIndex
     * to work around a compiler bug on Fermi.
     */
    uint groupIndex = groupThreadID.y * CSHADER_TILE_GROUP_DIM + groupThreadID.x;

    /* How many total lights? */
    uint totalLights, dummy;
    gLight.GetDimensions(totalLights, dummy);

    uint2 globalCoords = dispatchThreadID.xy;
    SurfaceData surfaceSamples[MSAA_SAMPLES];
    ComputeSurfaceDataFromGBufferAllSamples(globalCoords, surfaceSamples);

    float minZSample = CameraNearFar.y;
    float maxZSample = CameraNearFar.x;
    {
        [unroll] for (uint sample = 0; sample < MSAA_SAMPLES; ++sample) {
            float viewSpaceZ = surfaceSamples[sample].positionView.z;
            bool validPixel = viewSpaceZ >= CameraNearFar.x
                           && viewSpaceZ >  CameraNearFar.y;
            [flatten] if (validPixel) {
                minZSample = min(minZSample, viewSpaceZ);
                maxZSample = max(maxZSample, viewSpaceZ);
            }
        }
    }

    /* Init shared memory light list and z-bounds. */
    if (groupIndex == 0) {
        tileNumLights = 0;
        numPerSamplePixels = 0;
        minZ = 0x7F7FFFFF; /* max float */
        maxZ = 0;
    }

    GroupMemoryBarrierWithGroupSync();

    // NOTE: Can do a parallel reduction here but now that we have MSAA and store sample frequency pixels
    // in shaded memory the increased shared memory pressure actually *reduces* the overall speed of the kernel.
    // Since even in the best case the speed benefit of the parallel reduction is modest on current architectures
    // with typical tile sizes, we have reverted to simple atomics for now.
    // Only scatter pixels with actual valid samples in them
    if (maxZSample >= minZSample) {
        InterlockedMin(minZ, asuint(minZSample));
        InterlockedMax(maxZ, asuint(maxZSample));
    }

    GroupMemoryBarrierWithGroupSync();

    float minTileZ = asfloat(minZ);
    float maxTileZ = asfloat(maxZ);

    // NOTE: This is all uniform per-tile (i.e. no need to do it per-thread) but fairly inexpensive
    // We could just precompute the frusta planes for each tile and dump them into a constant buffer...
    // They don't change unless the projection matrix changes since we're doing it in view space.
    // Then we only need to compute the near/far ones here tightened to our actual geometry.
    // The overhead of group synchronization/LDS or global memory lookup is probably as much as this
    // little bit of math anyways, but worth testing.

    /* Work out scale/bias from [0, 1] */
    float2 tileScale = float2(FramebufferDimensions.xy) * rcp(float(2 * CSHADER_TILE_GROUP_DIM));
    float2 tileBias = tileScale - float2(groupID.xy);

    /* Now work out composite projection matrix */
    /* Relevant matrix columns for this tile frusta */
    float4 c1 = float4(CameraProj._11 * tileScale.x, 0.0f, tileBias.x, 0.0f);
    float4 c2 = float4(0.0f, -CameraProj._22 * tileScale.y, tileBias.y, 0.0f);
    float4 c4 = float4(0.0f, 0.0f, 1.0f, 0.0f);

    /* Derive frustum planes */
    float4 frustumPlanes[6];
    /* Sides */
    frustumPlanes[0] = c4 - c1;
    frustumPlanes[1] = c4 + c1;
    frustumPlanes[2] = c4 - c2;
    frustumPlanes[3] = c4 + c2;
    /* Near/far */
    frustumPlanes[4] = float4(0.0f, 0.0f,  1.0f, -minTileZ);
    frustumPlanes[5] = float4(0.0f, 0.0f, -1.0f,  maxTileZ);

    /* Normalize frustum planes (near/far already normalized) */
    [unroll] for (uint i = 0; i < 4; ++i) {
        frustumPlanes[i] *= rcp(length(frustumPlanes[i].xyz));
    }

    /* Cull lights for this tile */
    for (uint lightIndex = groupIndex; lightIndex < totalLights; lightIndex += CSHADER_TILE_GROUP_SIZE) {
        PointLight light = gLight[lightIndex];

        /* Cull: point light sphere vs tile frustum */
        bool inFrustum = true;
        [unroll] for (uint i = 0; i < 6; ++i) {
            float d = dot(frustumPlanes[i], float4(light.positionView, 1.0f));
            inFrustum = inFrustum && (d >= -light.attenuationEnd);
        }

        [branch] if (inFrustum) {
            /* Append light to list */
            /* Compaction might be better if we expect a lot of lights */
            uint listIndex;
            InterlockedAdd(tileNumLights, 1, listIndex);
            tileLightIndices[listIndex] = lightIndex;
        }
    }

    GroupMemoryBarrierWithGroupSync();

    uint numLights = tileNumLights;

    /* Only process onscreen pixels (tiles can span screen edges) */
    if (all(globalCoords < FramebufferDimensions.xy)) {
        [branch] if (numLights > 0) {
            bool perSampleShading = RequiresPerSampleShading(surfaceSamples);
            float3 lit = float3(0.0f, 0.0f, 0.0f);
            for (uint tileLightIndex = 0; tileLightIndex < numLights; ++tileLightIndex) {
                PointLight light = gLight[tileLightIndices[tileLightIndex]];
                AccumulateBRDF(surfaceSamples[0], light, lit);
            }

            /* Write sample 0 result */
            WriteSample(globalCoords, 0, float4(lit, 1.0f));

            [branch] if (perSampleShading) {
                /* Create a list of pixels that need per-sample shading */
                uint listIndex;
                InterlockedAdd(numPerSamplePixels, 1, listIndex);
                perSamplePixels[listIndex] = PackCoords(globalCoords);
            } else {
                /* Otherwise per-pixel shading, so splat the result to all samples */
                [unroll] for (uint sample = 1; sample < MSAA_SAMPLES; ++sample) {
                    WriteSample(globalCoords, sample, float4(lit, 1.0f));
                }
            }
        } else {
            /* Otherwise no lights affect here so clear all samples */
            [unroll] for (uint sample = 0; sample < MSAA_SAMPLES; ++sample) {
                WriteSample(globalCoords, sample, float4(0.0f, 0.0f, 0.0f, 0.0f));
            }
        }
    }

#if MSAA_SAMPLES > 1
        /* NOTE: We were careful to write only sample 0 above if we are going to do sample */
        /* frequency shading below, so we don't need a device memory barrier here. */
        GroupMemoryBarrierWithGroupSync();

        /* Now handle any pixels that require per-sample shading */
        /* NOTE: Each pixel requires MSAA_SAMPLES - 1 additional shading passes */
        const uint shadingPassesPerPixel = MSAA_SAMPLES - 1;
        uint globalSamples = numPerSamplePixels * shadingPassesPerPixel;

        for (uint globalSample = groupIndex; globalSample < globalSamples; globalSample += CSHADER_TILE_GROUP_SIZE) {
            uint listIndex = globalSample / shadingPassesPerPixel;
            uint sampleIndex = globalSample % shadingPassesPerPixel + 1;        /* sample 0 has been handled earlier */

            uint2 sampleCoords = UnpackCoords(sPerSamplePixels[listIndex]);
            SurfaceData surface = ComputeSurfaceDataFromGBufferSample(sampleCoords, sampleIndex);

            float3 lit = float3(0.0f, 0.0f, 0.0f);
            for (uint tileLightIndex = 0; tileLightIndex < numLights; ++tileLightIndex) {
                PointLight light = gLight[tileLightIndices[tileLightIndex]];
                AccumulateBRDF(surface, light, lit);
            }
            WriteSample(sampleCoords, sampleIndex, float4(lit, 1.0f));
        }
#endif
}

#endif /* LIGHT_TILE_COMP_HLSL */


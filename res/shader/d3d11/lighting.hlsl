#ifndef LIGHTING_HLSL
#define LIGHTING_HLSL

#include "utilities.hlsl"
#include "gbuffer.hlsl"

struct PointLight {
    float3 positionView;
    float attenuationBegin;
    float3 color;
    float attenuationEnd;
};

StructuredBuffer<PointLight> gLight : register(t5);

/* As below, we separate this for diffuse/specular parts for convenience in deferred lighting */
void AccumulatePhongBRDF(float3 normal,
                         float3 lightDir,
                         float3 viewDir,
                         float3 lightContrib,
                         float specularPower,
                         inout float3 litDiffuse,
                         inout float3 litSpecular)
{
    /* Simple Phong */
    float NdotL = dot(normal, lightDir);
    [flatten] if (NdotL > 0.0f) {
        float3 r = reflect(lightDir, normal);
        float RdotV = max(0.0f, dot(r, viewDir));
        float specular = pow(RdotV, specularPower);

        litDiffuse += lightContrib * NdotL;
        litSpecular += lightContrib * specular;
    }
}

/* Accumulates separate "diffuse" and "specular" components of lighting from a given */
/* This is not possible for all BRDFs but it works for our simple Phong example here */
/* and this separation is convenient for deferred lighting. */
/* Uses an in-out for accumulation to avoid returning and accumulating 0 */
void AccumulateBRDFDiffuseSpecular(SurfaceData surface, PointLight light,
                                   inout float3 litDiffuse,
                                   inout float3 litSpecular)
{
    float3 directionToLight = light.positionView - surface.positionView;
    float distanceToLight = length(directionToLight);

    [branch] if (distanceToLight < light.attenuationEnd) {
        float attenuation = linstep(light.attenuationEnd, light.attenuationBegin, distanceToLight);
        directionToLight *= rcp(distanceToLight);   /* A full normalize/RSQRT might be as fast here anyways... */

        AccumulatePhongBRDF(surface.normal, directionToLight, normalize(surface.positionView),
            attenuation * light.color, surface.specularPower, litDiffuse, litSpecular);
    }
}

/* Uses an in-out for accumulation to avoid returning and accumulating 0 */
void AccumulateBRDF(SurfaceData surface, PointLight light,
                    inout float3 lit)
{
    float3 directionToLight = light.positionView - surface.positionView;
    float distanceToLight = length(directionToLight);

    [branch] if (distanceToLight < light.attenuationEnd) {
        float attenuation = linstep(light.attenuationEnd, light.attenuationBegin, distanceToLight);
        directionToLight *= rcp(distanceToLight);   /* A full normalize/RSQRT might be as fast here anyways... */

        float3 litDiffuse = float3(0.0f, 0.0f, 0.0f);
        float3 litSpecular = float3(0.0f, 0.0f, 0.0f);
        AccumulatePhongBRDF(surface.normal, directionToLight, normalize(surface.positionView),
            attenuation * light.color, surface.specularPower, litDiffuse, litSpecular);

        lit += surface.albedo.rgb * (litDiffuse + surface.specularAmount * litSpecular);
    }
}

#endif /* LIGHTING_HLSL */


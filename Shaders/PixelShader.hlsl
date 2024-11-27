// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 1
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "LightUtil.hlsli"

Texture2D gDiffuseMap : register(t0);

// List sampler states
SamplerState gSamplerPointWrap : register(s0);
SamplerState gSamplerPointClamp : register(s1);
SamplerState gSamplerLinearWrap : register(s2);
SamplerState gSamplerLinearClamp : register(s3);
SamplerState gSamplerAnisotropicWrap : register(s4);
SamplerState gSamplerAnisotropicClamp : register(s5);

// Pass Constant Buffer Access for multiple shaders
cbuffer PassConstants : register(b0)
{
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4x4 ViewProjectionMatrix;
    
    float4x4 InverseViewMatrix;
    float4x4 InverseProjectionMatrix;
    float4x4 InverseViewProjectionMatrix;
    
    // Lighting Traits
    float4 gAmbientLight;
    
    float3 CameraPosition;
    float2 RenderTargetSize;
    float2 InverseRenderTargetSize;
    float NearZ; // Near clip plane
    float FarZ; // Far clip plane
    float TotalTime;
    float DeltaTime;
};

cbuffer cbMaterial : register(b2)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMatTransform;
}

cbuffer cbLight : register(b3)
{
    Light gLights[16];
}

struct PSInput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
};


float4 main(PSInput input) : SV_TARGET
{
    // Get the color of the pixel based on the texture, using sampler
    float4 diffuseAlbedo = gDiffuseMap.Sample(gSamplerLinearWrap, input.TexC) * gDiffuseAlbedo;
    
    // Renormalize
    input.NormalW = normalize(input.NormalW);
    
    // Get to vector to eye
    float3 toEyeW = normalize(CameraPosition - input.PosW);
    
    // Indirect Lighting
    float4 ambient = gAmbientLight * diffuseAlbedo;
    
    const float shininess = 1.0f - gRoughness;
    Material mat = { diffuseAlbedo, gFresnelR0, shininess };
    float3 shadowFactor = 1.0f;
    float4 directLight = ComputeLighting(gLights, mat, input.PosW,
        input.NormalW, toEyeW, shadowFactor);
    
    float4 litColor = ambient + directLight;
    
    // Get alpha from diffuse material
    litColor.a = gDiffuseAlbedo.a;
    
    return litColor;
}
#include "LightUtil.hlsli"

// Pass Constant Buffer Access for multiple shaders
cbuffer PassConstants : register(b0)
{
    float4x4 ViewMatrix;
    float4x4 ProjectionMatrix;
    float4x4 ViewProjectionMatrix; // Add this if you precompute it.
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

cbuffer ObjectConstants : register(b1)
{
    float4x4 WorldMatrix;
    float4x4 TexTransform;
};

cbuffer MaterialConstants : register(b2)
{
    float4 gDiffuseAlbedo;
    float3 gFresnelR0;
    float gRoughness;
    float4x4 gMatTransform;
};

struct VSInput
{
    float3 PosL : POSITION;
    float3 NormalL : NORMAL;
    float3 TangentL : TANGENT;
    float2 TexCoord : TEXCOORD;
};

struct PSInput
{
    float4 PosH : SV_POSITION;
    float3 PosW : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC : TEXCOORD;
};

PSInput main(VSInput vin)
{
    PSInput result = (PSInput) 0.0f;

    // Transform to world space
    float4 posW = mul(float4(vin.PosL, 1.0f), WorldMatrix);
    result.PosW = posW.xyz;
    
    // Correctly transform the normal using inverse transpose
    
    result.NormalW = mul(vin.NormalL, (float3x3) WorldMatrix);
    
    // Transform to homogeneous clip space
    result.PosH = mul(posW, ViewMatrix); // Assuming ViewProjectionMatrix is precomputed.
    result.PosH = mul(result.PosH, ProjectionMatrix);
    
    // Output texture coordinates
    float4 texC = mul(float4(vin.TexCoord, 0.0f, 1.0f), TexTransform);
    result.TexC = mul(texC, gMatTransform).xy;
    
    return result;
}

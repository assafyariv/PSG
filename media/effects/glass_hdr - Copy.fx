// Copyright NVIDIA Corporation 2002-2005
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#include "include/textureHDR_RGBA.cg"
#include "include/NVSGMaterial.cg"

float4x4 wvp : WorldViewProjection;
float4x4 world : World;
float4x4 worldI : WorldI;
float4x4 viewI : ViewI;

NVSGMaterial  material;

struct appdata 
{
  float4 Position	 : POSITION;
  float4 texcoord0   : TEXCOORD0;
  float4 Normal	     : NORMAL;
};

struct VS_INPUT
{
  float4 position   : POSITION;        // object space position
  float4 normal     : NORMAL;           
};

struct vertexOutput 
{
    float4 Position  : POSITION;
    float4 texcoord0 : TEXCOORD0;
    float4 texcoord1 : TEXCOORD1;
    float4 texcoord2 : TEXCOORD2;
    float4 texcoord3 : TEXCOORD3;
    float4 texcoord4 : TEXCOORD4;
};

struct FRAGOUT
{
  float4 col : COLOR;
};

float3 knee (float3 x, float f)
{
    return log (x * f + 1) / f;
}

vertexOutput GlassVS(appdata IN,
                     uniform float4x4 WorldViewProj,
                     uniform float4x4 World,
                     uniform float4x4 WorldI,
                     uniform float4x4 ViewI) 
{
  vertexOutput OUT;
  OUT.Position = mul(WorldViewProj, IN.Position);
  OUT.texcoord0 = IN.texcoord0;
  OUT.texcoord1.xyz = mul(ViewI, float4(0.0, 0.0, 0.0, 1.0)).xyz;
  OUT.texcoord2.xyz = mul(World, IN.Position).xyz;
  OUT.texcoord3.xyz = mul(IN.Normal, WorldI).xyz;
  return OUT;
}

FRAGOUT glass( vertexOutput IN, 
               uniform samplerCUBE textureHDR_RGBA,
               uniform float minRefl, 
               uniform float maxRefl, 
               uniform float fresnelEXP,
               uniform float exposure,
               uniform float grayTarget,
	         uniform NVSGMaterial  material)
{
  float3 normal        = normalize(IN.texcoord3.xyz);
  float3 viewVector    = normalize(IN.texcoord2.xyz - IN.texcoord1.xyz);
  float3 reflVect      = reflect(viewVector, normal);

  exposure   = pow(2.0h, exposure + 2.47393h);
  
  float3 hdr;
  hdr.xyz = texCUBE(textureHDR_RGBA, reflVect);
  hdr     = max(float3(0.0h, 0.0h, 0.0h), hdr * exposure);
  // shouldn't this be done for each channel ?
  hdr     = (hdr > 0.0h) ? knee (hdr, 0.184874h) : hdr;
  hdr    *= grayTarget;

  // compute fresnel equation
 // float F = 1.0-(minRefl + (maxRefl-minRefl)*pow(1.0 - dot(-viewVector, reflVect), fresnelEXP));
float dotnv = dot(viewVector, normal);
 float F = minRefl + (maxRefl-minRefl)*pow(1.0 - abs(dotnv), fresnelEXP);

  FRAGOUT OUT; 
  float3 diff = material.diffuse.xyz;
  OUT.col = float4((F)*hdr.xyz + (1.0-F)* diff ,  (F) + material.diffuse.w);
  return OUT;
}

void glassMRT( vertexOutput IN, 
               uniform samplerCUBE textureHDR_RGBA,
               uniform float minRefl, 
               uniform float maxRefl, 
               uniform float fresnelEXP,
               uniform float exposure,
               uniform float grayTarget,
	         uniform NVSGMaterial  material  ,
                  out float4 normColor : COLOR0
                , out float4 deepColor : COLOR1)
{
  float3 normal        = normalize(IN.texcoord3.xyz);
  float3 viewVector    = normalize(IN.texcoord2.xyz - IN.texcoord1.xyz);
  float3 reflVect      = reflect(viewVector, normal);

  exposure   = pow(2.0h, exposure + 2.47393h);
  
  float3 hdr;
  hdr.xyz = texCUBE(textureHDR_RGBA, reflVect);
  hdr     = max(float3(0.0h, 0.0h, 0.0h), hdr * exposure);
  // shouldn't this be done for each channel ?
  hdr     = (hdr > 0.0h) ? knee (hdr, 0.184874h) : hdr;
  hdr    *= grayTarget;

  // compute fresnel equation
 // float F = 1.0-(minRefl + (maxRefl-minRefl)*pow(1.0 - dot(-viewVector, reflVect), fresnelEXP));

 float F = minRefl + (maxRefl-minRefl)*pow(1.0 - abs(dot(viewVector, normal)), fresnelEXP);
 float3 diff = material.diffuse.xyz;
 normColor = float4(1.0,0.0,0.0,1.0);
 deepColor = float4((F)*hdr.xyz + (1.0-F)* diff ,  (F) + material.diffuse.w);
}
float minRefl
<
  string gui = "slider";
  float uimin = 0.0;
  float uimax = 1.0;
  float uistep = 0.01;
> = 0.0;

float maxRefl
<
  string gui = "slider";
  float uimin = 0.0;
  float uimax = 1.0;
  float uistep = 0.01;
> = 0.37;

float fresnelEXP
<
  string gui = "slider";
  float uimin = 0.0;
  float uimax = 100.0;
  float uistep = 1.00;
> = 0.0;

float Exposure
<
  float uimin = -10.0f;
  float uimax =  10.0f;
  float uistep = 0.05f;
> = 0.0f;

float GrayTarget
<
  float uimin = -255.0f;
  float uimax =  255.0f;
  float uistep = 0.05f;
> = 0.33;

technique Main
{
  pass p0 
  {
    DepthTestEnable = true;
    DepthMask       = true;
    BlendEnable     = true;
    BlendFunc       = int2(SrcAlpha, OneMinusSrcAlpha);
    FrontFace       = CCW;
    CullFaceEnable  = true;
    VertexProgram   = compile latest GlassVS(wvp, world, worldI, viewI);
    FragmentProgram = compile latest glass(textureHDR_RGBA, minRefl, maxRefl, fresnelEXP, Exposure, GrayTarget,material);
  }
}

technique MainMRT
{
  pass p0 
  {
    DepthTestEnable = true;
    DepthMask       = true;
    BlendEnable     = true;
    BlendFunc       = int2(SrcAlpha, OneMinusSrcAlpha);
    FrontFace       = CCW;
    CullFaceEnable  = true;
     
    VertexProgram   = compile  latest GlassVS(wvp, world, worldI, viewI);
    FragmentProgram = compile  latest glassMRT(textureHDR_RGBA,  minRefl, maxRefl, fresnelEXP, Exposure, GrayTarget,material);
  }
}


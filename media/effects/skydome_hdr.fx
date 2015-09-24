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

float4x4 wvp    : WorldViewProjection;
float4x4 worldI : WorldI;

struct VS_INPUT
{
  float4 position   : POSITION;        // object space position
  float4 normal     : NORMAL;   
  float4 texcoord0  : TEXCOORD0;       // texture coordinate set 0         
};

struct VS_OUTPUT
{
  float4 position   : POSITION;      // homogeneous clip space position  
  float4 texcoord0  : TEXCOORD0;     // texture coordinate set 0  
  float4 texcoord1  : TEXCOORD1;
};

struct FRAGOUT
{
  float4 col        : COLOR;
};


float3 knee (float3 x, float f)
{
  return log (x * f + 1) / f;
}

VS_OUTPUT skybox_vs(VS_INPUT IN, uniform float4x4 WorldViewProj, uniform float4x4 WorldI) 
{
  VS_OUTPUT OUT;
  OUT.position      = mul(WorldViewProj, IN.position);
  OUT.texcoord1.xyz = mul(IN.normal  , WorldI).xyz;
  OUT.texcoord0.xyz = IN.position.xyz;
  return OUT;
}


FRAGOUT skybox_fs( VS_OUTPUT IN, 
                   uniform samplerCUBE firstEXR,
                   uniform float exposure,
                   uniform float grayTarget) : COLOR
{
  exposure   = pow(2.0h, exposure + 2.47393h);  

  float3 hdr;
  hdr.xyz = texCUBE(firstEXR,  IN.texcoord0.xyz);
//float bias = abs(IN.texcoord0.z);
  //hdr.xyz = texCUBEbias(firstEXR,  IN.texcoord0.xyz,1.0);
 
  hdr = max(float3(0.0h, 0.0h, 0.0h), hdr * exposure);

  // shouldn't this be done for each channel ?
  hdr = (hdr > 0.0h) ? knee (hdr, 0.184874h) : hdr;
  hdr *= grayTarget;

  FRAGOUT OUT; 
  OUT.col.xyz = hdr;
  return OUT;
}

void skybox_fsMRT( VS_OUTPUT IN, 
                   uniform samplerCUBE firstEXR,
                   uniform float exposure,
                   uniform float grayTarget 
                , out float4 normColor : COLOR0
                , out float4 deepColor : COLOR1) 
{
  exposure   = pow(2.0h, exposure + 2.47393h);  

  float3 hdr;
  hdr.xyz = texCUBE(firstEXR,  IN.texcoord0.xyz);
//float bias = abs(IN.texcoord0.z);
  //hdr.xyz = texCUBEbias(firstEXR,  IN.texcoord0.xyz,1.0);
 
  hdr = max(float3(0.0h, 0.0h, 0.0h), hdr * exposure);

  // shouldn't this be done for each channel ?
  hdr = (hdr > 0.0h) ? knee (hdr, 0.184874h) : hdr;
  hdr *= grayTarget;
deepColor = float4(hdr,1.0);
normColor = float4(1.0,0.0,0.0,1.0);
}
float Exposure
<
  float uimin = -10.0f;
  float uimax =  10.0f;
  float uistep = 0.05f;
> = -1.5f;

float GrayTarget
<
  float uimin = -255.0f;
  float uimax =  255.0f;
  float uistep = 0.05f;
> = 0.33f;



technique Main
{
  pass p0 
  {
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CW;
    CullFaceEnable  = true;

    VertexProgram   = compile latest skybox_vs(wvp, worldI);
    FragmentProgram = compile latest skybox_fs(textureHDR_RGBA, Exposure, GrayTarget);
  }
}
technique MainMRT
{
  pass p0 
  {
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CW;
    CullFaceEnable  = true;

    VertexProgram   = compile latest skybox_vs(wvp, worldI);
    FragmentProgram = compile latest skybox_fsMRT(textureHDR_RGBA, Exposure, GrayTarget);
  }
}

technique skybox_ARB
{
  pass p0 
  {
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CW;
    CullFaceEnable  = true;

    VertexProgram   = compile arbvp1 skybox_vs(wvp, worldI);
    FragmentProgram = compile arbfp1 skybox_fs(textureHDR_RGBA, Exposure, GrayTarget);
  }
}

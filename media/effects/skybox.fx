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

float4x4 wvp    : WorldViewProjection;
float4x4 worldI : WorldI;

struct VS_INPUT
{
  float4 position   : POSITION;        // object space position
  float4 normal     : NORMAL;           
};

struct VS_OUTPUT
{
  float4 position   : POSITION;      // homogeneous clip space position  
  float4 texcoord0  : TEXCOORD0;     // texture coordinate set 0  
};

struct FRAGOUT
{
  float4 col : COLOR;
};

VS_OUTPUT skybox_vs(VS_INPUT IN, uniform float4x4 WorldViewProj, uniform float4x4 WorldI) 
{
  VS_OUTPUT OUT;

  OUT.position      = mul(WorldViewProj, IN.position);
  OUT.texcoord0.xyz = mul(IN.normal, WorldI).xyz;

  return OUT;
}

FRAGOUT skybox_fs(VS_OUTPUT IN, uniform samplerCUBE EnvironmentMap)
{
  float4 cubeColor  = texCUBE(EnvironmentMap, IN.texcoord0.xyz); 

  FRAGOUT OUT; 
  OUT.col = cubeColor;

  return OUT;
}

texture cubeMap : EnvMap
<
  string File = "../textures/nvlobby_new.dds";
>;

samplerCUBE environmentMapSampler = sampler_state
{
  Texture = <cubeMap>;

  MinFilter = LinearMipMapLinear;
  MagFilter = Linear;

  WrapS  = ClampToEdge;
  WrapT  = ClampToEdge;
  WrapR  = ClampToEdge;
};

technique skybox_NV30
{
  pass p0 
  {   
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CCW;
    CullFaceEnable  = false;

    VertexProgram   = compile vp30 skybox_vs(wvp, worldI);
    FragmentProgram = compile fp30 skybox_fs(environmentMapSampler);
  }
}

technique skybox_NV40
{
  pass p0 
  {   
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CCW;
    CullFaceEnable  = false;

    VertexProgram   = compile vp40 skybox_vs(wvp, worldI);
    FragmentProgram = compile fp40 skybox_fs(environmentMapSampler);
  }
}

technique skybox_ARB
{
  pass p0 
  {   
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CCW;
    CullFaceEnable  = false;

    VertexProgram   = compile arbvp1 skybox_vs(wvp, worldI);
    FragmentProgram = compile arbfp1 skybox_fs(environmentMapSampler);
  }
}
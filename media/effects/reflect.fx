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

float4x4 worldviewprojection  : WORLDVIEWPROJECTION;
float4x4 world                : WORLD;
float4x4 viewi                : VIEWI;
float4x4 worldi               : WORLDI;

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

struct vOut
{
  float4 position : POSITION;
  float3 vReflect : TEXCOORD0;
};

vOut vertexReflect( float3 position: POSITION
                  , float3 normal : NORMAL
                  , uniform float4x4 wvp
                  , uniform float4x4 w
                  , uniform float4x4 vi
                  , uniform float4x4 wi
                  )
{
  vOut Out;
  Out.position = mul(wvp, float4(position, 1.0f)); // to clip

  float3 n = mul(normal, (float3x3)wi);
  n = normalize(n);

  float3 posW = mul(w, float4(position, 0.0));

  float3 eyeW = mul(vi, float4(0.0, 0.0, 0.0, 1.0)).xyz;   
  float3 i = posW-eyeW;

  Out.vReflect = reflect(i,n);
  return Out;
}

float4 fragmentReflect( float3 vRefl : TEXCOORD0
                      , uniform samplerCUBE env
                      ) : COLOR
{
  return texCUBE(env, vRefl);
}             

technique EnvironmentMapping_NV30
{
  pass p0
  {
    VertexProgram   = compile vp30 vertexReflect(worldviewprojection, world, viewi, worldi);
    FragmentProgram = compile fp30 fragmentReflect(environmentMapSampler);
  }
}

technique EnvironmentMapping_NV40
{
  pass p0
  {
    VertexProgram   = compile vp40 vertexReflect(worldviewprojection, world, viewi, worldi);
    FragmentProgram = compile fp40 fragmentReflect(environmentMapSampler);
  }
}

technique EnvironmentMapping_ARB
{
  pass p0
  {
    VertexProgram   = compile arbvp1 vertexReflect(worldviewprojection, world, viewi, worldi);
    FragmentProgram = compile arbfp1 fragmentReflect(environmentMapSampler);
  }
}
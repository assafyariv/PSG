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
float4x4 world  : World;
float4x4 worldI : WorldI;
float4x4 viewI  : ViewI;

struct appdata 
{
  float4 Position	 : POSITION;
  float4 texcoord0 : TEXCOORD0;
  float4 Normal	   : NORMAL;
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

struct pixelOutput 
{
  float4 col : COLOR;
};

vertexOutput mainVS( appdata          IN,
                     uniform float4x4 WorldViewProj,
                     uniform float4x4 World,
                     uniform float4x4 WorldI,
                     uniform float4x4 ViewI) 
{
  vertexOutput OUT;

  OUT.Position      = mul(WorldViewProj, IN.Position);
  OUT.texcoord0     = IN.texcoord0;
  OUT.texcoord1.xyz = mul(ViewI, float4(0.0, 0.0, 0.0, 1.0)).xyz;
  OUT.texcoord2.xyz = mul(World, IN.Position).xyz;
  OUT.texcoord3.xyz = mul(IN.Normal, WorldI).xyz;

  return OUT;
}

pixelOutput glass( vertexOutput   IN, 
                   uniform        samplerCUBE EnvironmentMap,  
                   uniform float  minRefl,
                   uniform float  maxRefl, 
                   uniform float  fresnelEXP)
{
  float3 normal     = normalize(IN.texcoord3.xyz);
  float3 viewVector = normalize(IN.texcoord2.xyz - IN.texcoord1.xyz);
  float3 reflVect   = normalize(reflect(viewVector, normal));
  float4 reflColor  = texCUBE(EnvironmentMap, reflVect);

  // compute fresnel equation
  float F = minRefl + (maxRefl-minRefl)*pow(1.0 - dot(-viewVector, reflVect), fresnelEXP);

  pixelOutput OUT; 
  OUT.col = float4(F*reflColor.xyz, (1.0-F));
  return OUT;
}

float Min
<
	string gui   = "slider";
	float uimin  = 0.0;
	float uimax  = 1.0;
	float uistep = 0.01;
> = 0.0;

float Max
<
	string gui   = "slider";
	float uimin  = 0.0;
	float uimax  = 1.0;
	float uistep = 0.01;
> = 1.0;

float FresnelExponent
<
	string gui   = "slider";
	float uimin  = 0.0;
	float uimax  = 100.0;
	float uistep = 1.00;
> = 1.0;

texture cubeMap : EnvMap
<
  string File         = "../textures/nvlobby_new.dds";
  string TextureType  = "Cube";
>;

samplerCUBE environmentMapSampler = sampler_state
{
	Texture   = <cubeMap>;
	MinFilter = LinearMipMapLinear;
	MagFilter = Linear;
  WrapS     = ClampToEdge;
  WrapT     = ClampToEdge;
  WrapR     = ClampToEdge;
};

technique Glass_NV30
{
	pass p0 
	{
    DepthTestEnable = true;
    DepthMask       = true;
    BlendEnable     = true;
    BlendFunc       = int2(SrcColor, OneMinusSrcColor);
    FrontFace       = CCW;
    CullFaceEnable  = true;

    VertexProgram   = compile vp30 mainVS(wvp, world, worldI, viewI);
    FragmentProgram = compile fp30 glass(environmentMapSampler, Min, Max, FresnelExponent);
	}
}

technique Glass_NV40
{
	pass p0 
	{
    DepthTestEnable = true;
    DepthMask       = true;
    BlendEnable     = true;
    BlendFunc       = int2(SrcColor, OneMinusSrcColor);
    FrontFace       = CCW;
    CullFaceEnable  = true;

    VertexProgram   = compile vp40 mainVS(wvp, world, worldI, viewI);
    FragmentProgram = compile fp40 glass(environmentMapSampler, Min, Max, FresnelExponent);
	}
}

technique Glass_ARB
{
	pass p0 
	{
    DepthTestEnable = true;
    DepthMask       = true;
    BlendEnable     = true;
    BlendFunc       = int2(SrcColor, OneMinusSrcColor);
    FrontFace       = CCW;
    CullFaceEnable  = true;

    VertexProgram   = compile arbvp1 mainVS(wvp, world, worldI, viewI);
    FragmentProgram = compile arbfp1 glass(environmentMapSampler, Min, Max, FresnelExponent);
	}
}
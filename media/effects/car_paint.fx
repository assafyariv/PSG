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

#include "include/matrices.cg"
#include "include/structs.cg"
#include "include/textureenv.cg"
#include "include/NVSGLight.cg"

NVSGLight light;
float4 lacquer = {0.0f, 0.0f, 0.5f, 1.0f};

VS_OUTPUT mainVS( VS_INPUT          IN,
                  uniform float4x4  WorldViewProj,
                  uniform float4x4  World,
                  uniform float4x4  WorldI,
                  uniform float4x4  ViewI,
                  uniform float     minRefl,
                  uniform float     maxRefl,
                  uniform float     fresnelEXP,
                  uniform NVSGLight light ) 
{
  VS_OUTPUT OUT;

  OUT.position  = mul(WorldViewProj, IN.position);
  OUT.texcoord0 = IN.texcoord0;
  
  float4 lightPos    = mul(ViewI, light.position);	
  float3 position    = mul(World, IN.position).xyz;
  float3 normal      = normalize(mul(IN.normal, WorldI).xyz); // multiply with inverse transpose	
  float3 eyePosition = mul(ViewI, float4(0.0, 0.0, 0.0, 1.0)).xyz;
  float3 viewVector  = normalize(eyePosition - position);
  float3 lightVector = normalize(lightPos.xyz - position);
  float3 halfway     = normalize(viewVector + lightVector);
  
  float  fres = minRefl + (maxRefl-minRefl) * pow((1.0-abs(dot(viewVector,normal))), fresnelEXP);
  
  OUT.texcoord4.w   = fres;
  OUT.texcoord4.xyz = viewVector;
  
  OUT.texcoord5.xyz = lightVector;
  OUT.texcoord6.xyz = normal;

  OUT.color = IN.color;  
  
  return OUT;
}

FRAGOUT mainPS( VS_OUTPUT IN,
                uniform samplerCUBE textureENV,
                uniform NVSGLight   light,
                uniform float       shininess,
                uniform float4      lacquer) 
{
  float3 normal      = normalize(IN.texcoord6.xyz);
  float3 lightVector = normalize(IN.texcoord5.xyz);
  float3 viewVector  = normalize(IN.texcoord4.xyz);

  float3 reflVector = reflect(-viewVector, IN.texcoord6.xyz);
  float4 cubeColor = texCUBE(textureENV, reflVector);

  float fres = IN.texcoord4.w;

  float3 diffuseLight = light.diffuse* abs(dot(lightVector, normal));

  // compute halfway vector 
  float3 halfway = normalize(lightVector + viewVector);

  // compute the specular term	
  float3 specularLight = light.specular*pow(abs(dot(normal, halfway)), shininess);

  float4 result = (1.0-fres)*(lacquer + float4(specularLight, 0.0)) + fres*cubeColor;

  FRAGOUT OUT; 
  OUT.color = result;
  return OUT;
}

float maxRefl
<
  string gui = "slider";
  float uimin = 0.1;
  float uimax = 1.0;
  float uistep = 0.05;
> = 1.0;

float minRefl
<
  string gui = "slider";
  float uimin = 0.0;
  float uimax = 1.0;
  float uistep = 0.05;
> = 0.1;

float fresnelEXP
<
  string gui = "slider";
  float uimin = 1.0;
  float uimax = 6.5;
  float uistep = 0.5;
> = 1.0;

float shininess
<
	string gui = "slider";
	float uimin = 0.0;
	float uimax = 40.0;
	float uistep = 2.0;
> = 8;

technique carpaint_NV30
{
	pass p0 
	{		
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CCW;
    CullFaceEnable  = true;

    VertexProgram   = compile vp30 mainVS(Wvp, World, WorldI, ViewI, minRefl, maxRefl, fresnelEXP, light);
    FragmentProgram = compile fp30 mainPS(TextureENV, light, shininess, lacquer);
  }
}

technique carpaint_NV40
{
	pass p0 
	{		
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CCW;
    CullFaceEnable  = true;

    VertexProgram   = compile vp40 mainVS(Wvp, World, WorldI, ViewI, minRefl, maxRefl, fresnelEXP, light);
    FragmentProgram = compile fp40 mainPS(TextureENV, light, shininess, lacquer);
  }
}

technique carpaint_ARB
{
	pass p0 
	{		
    DepthTestEnable = true;
    DepthMask       = true;
    FrontFace       = CCW;
    CullFaceEnable  = true;

    VertexProgram   = compile arbvp1 mainVS(Wvp, World, WorldI, ViewI, minRefl, maxRefl, fresnelEXP, light);
    FragmentProgram = compile arbfp1 mainPS(TextureENV, light, shininess, lacquer);
  }
}
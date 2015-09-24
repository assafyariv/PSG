// Copyright NVIDIA Corporation 2002-2006
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#include "include/NVSGLight.cg"
#include "include/NVSGMaterial.cg"

texture tex0;
texture tex1;

sampler2D shadowMap0 = sampler_state
{
  Texture   = <tex0>;
  MinFilter = Linear;MagFilter = Linear;WrapS = ClampToEdge;WrapT = ClampToEdge;
};

sampler2D shadowMap1 = sampler_state
{
  Texture   = <tex1>;
  MinFilter = Linear;MagFilter = Linear;WrapS = ClampToEdge;WrapT = ClampToEdge;
};


float4x4 worldViewProj : WorldViewProjection; // model to clip
float4x4 world         : World;   // model to world
float4x4 worldIT       : WorldIT; // model to world
float4x4 viewI         : ViewI;   // view to world 
float4x4 shadowMat[2];   // Texture Matrix


// gloabal ambiel term
float4 gAmbient : GlobalAmbient;

NVSGLight     light[2];
NVSGMaterial  material;
float         coneDelta = 15.0;

void lighting( NVSGLight light
              , float3 position
              , float3 normal
              , float3 viewVector
              , float shininess
              , out float3 ambient
              , out float3 diffuse
              , out float3 specular
              )
{
  // ambient term
  ambient = light.ambient.xyz ; 

  // diffuse term
  float3 L;
  if ( light.position.w == 0.0f )
  { 
    // directed light
    L = normalize(light.position).xyz;
  }
  else
  { 
    // point light
    L = normalize(light.position.xyz - position);
  }
  float dfactor = max(dot(normal,L), 0);
  diffuse = light.diffuse.xyz * dfactor;

  // specular term
  float3 H = normalize(L+viewVector);
  float sfactor = pow(max(dot(normal,H),0), shininess);
  if (dfactor <= 0) 
  {
    sfactor = 0.0f;
  }

  specular = light.specular.xyz * sfactor ;
}

void vertex( float3 position      : POSITION
            , float3 normal        : NORMAL
            , out float4 oPosition : POSITION
            , out float3 worldPos  : TEXCOORD0
            , out float3 eyePos    : TEXCOORD1             
            , out float3 oNormal   : TEXCOORD2

            , out float4 texCoordProj0   : TEXCOORD3
            , out float4 texCoordProj1   : TEXCOORD4

            , uniform float4x4 worldViewProj 
            , uniform float4x4 world
            , uniform float4x4 worldIT
            , uniform float4x4 viewI
            , uniform float4x4 lightMat[2]
            )
{
    oPosition = mul(worldViewProj, float4(position, 1.0f));

    // compute world space position
    worldPos = mul(world, float4(position, 1.0f)).xyz;

    // compute eye position in world space
    eyePos = mul(viewI, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;

    // compute the normal in world space  
    oNormal = mul(worldIT, float4(normal, 1.0f)).xyz;

    texCoordProj0 = mul(lightMat[0], float4(worldPos, 1.0f));
    texCoordProj1 = mul(lightMat[1], float4(worldPos, 1.0f));
} 

float4 fragment( 
    float3 worldPos  : TEXCOORD0
	, float3 eyePos    : TEXCOORD1
	, float3 normal    : TEXCOORD2

	, float4 texCoordProj0   : TEXCOORD3
	, float4 texCoordProj1   : TEXCOORD4

	, uniform float4 gAmbient
	, uniform NVSGLight light[2]
	, uniform NVSGMaterial material

	, uniform sampler2D s0
	, uniform sampler2D s1

) : COLOR
{
  float3 P = worldPos;
  float3 V = normalize(eyePos - P);
  float3 N = normalize(normal);

  float3 ambientLight = 0;
  float3 diffuseLight = 0;
  float3 specularLight = 0;

	float3 ambientSum = 0;
	float3 diffuseSum = 0;
	float3 specularSum = 0;
	
	float4 shadowCoeff = 0;

  lighting( light[0], P, N, V, material.shininess, ambientLight, diffuseLight, specularLight );
  ambientSum += ambientLight;

  // Weight only the diffuse and the specular light contribution by the   shadowCoeff;
  shadowCoeff = tex2Dproj(s0, texCoordProj0);
  diffuseSum += diffuseLight*shadowCoeff.xyz;    
  specularSum += specularLight*shadowCoeff.xyz;    

  lighting( light[1], P, N, V, material.shininess, ambientLight, diffuseLight, specularLight );
  ambientSum += ambientLight;

  shadowCoeff = tex2Dproj(s1, texCoordProj1);
  diffuseSum += diffuseLight*shadowCoeff.xyz;    
  specularSum += specularLight*shadowCoeff.xyz;    

  float4 ambient = 0;
  float4 diffuse = 0;
  float4 specular = 0;

  ambient = material.ambient * gAmbient * float4(ambientSum, 1);
  diffuse = material.diffuse * float4(diffuseSum, 1);
  specular = material.specular * float4(specularSum, 1); 

  return material.emission + ambient + diffuse + specular;
}

technique FragmentLighting_NV40
{
  pass p0
  {
    VertexProgram   = compile vp40 vertex(worldViewProj, world, worldIT, viewI, shadowMat);
    FragmentProgram = compile fp40 fragment(gAmbient, light, material, shadowMap0, shadowMap1);
  }
}

technique FragmentLighting_NV30
{
  pass p0
  {
    VertexProgram   = compile vp30 vertex(worldViewProj, world, worldIT, viewI, shadowMat);
    FragmentProgram = compile fp30 fragment(gAmbient, light, material, shadowMap0, shadowMap1);
  } 
}

technique FragmentLighting_ARB
{
  pass p0
  {
    VertexProgram   = compile arbvp1 vertex(worldViewProj, world, worldIT, viewI, shadowMat);
    FragmentProgram = compile arbfp1 fragment(gAmbient, light, material, shadowMap0, shadowMap1);
  }
}
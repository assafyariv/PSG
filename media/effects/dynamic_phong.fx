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

#include "include/NVSGLight.cg"
#include "include/NVSGMaterial.cg"
#include "include/phongLighting.cg"

float4x4 worldViewProj : WorldViewProjection; // model to clip
float4x4 world         : World;   // model to world
float4x4 worldIT       : WorldIT; // model to world
float4x4 viewI         : ViewI;   // view to world 

// gloabal ambiel term
float4 gAmbient : GlobalAmbient;

NVSGLight     light[];
NVSGMaterial  material;

void vertex( float3 position      : POSITION
           , float3 normal        : NORMAL
           , out float4 oPosition : POSITION
           , out float3 worldPos  : TEXCOORD0
           , out float3 eyePos    : TEXCOORD1             
           , out float3 oNormal   : TEXCOORD2 
           , uniform float4x4 worldViewProj 
           , uniform float4x4 world
           , uniform float4x4 worldIT
           , uniform float4x4 viewI
           )
{
	oPosition = mul(worldViewProj, float4(position, 1.0f));
	  
	// compute world space position
	worldPos = mul(world, float4(position, 1.0f)).xyz;
	  
	// compute eye position in world space
	eyePos = mul(viewI, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
	  
	oNormal = mul(worldIT, float4(normal, 1.0f)).xyz;
} 

float4 fragment( float3 worldPos  : TEXCOORD0
               , float3 eyePos    : TEXCOORD1
               , float3 normal    : TEXCOORD2
               , uniform float4 gAmbient
               , uniform NVSGMaterial material 
               ) : COLOR
{
  float3 P = worldPos;
  float3 V = normalize(eyePos - P);
  float3 N = normalize(normal);

  float3 ambientSum = 0;
  float3 diffuseSum = 0;
  float3 specularSum = 0;

  for ( int i=0; i<light.length; ++i )
  {
    float3 ambientLight;
    float3 diffuseLight;
    float3 specularLight;

    lighting( light[i], P, N, V, material.shininess, ambientLight, diffuseLight, specularLight );

    // sum up
    ambientSum += ambientLight;
    diffuseSum += diffuseLight;
    specularSum += specularLight;
  }

  float4 ambient = material.ambient * gAmbient * float4(ambientSum, 1);
  float4 diffuse = material.diffuse * float4(diffuseSum, 1);
  float4 specular = material.specular * float4(specularSum, 1);

  return material.emission + ambient + diffuse + specular; 
}

technique T0
{
	pass p0
	{
		VertexShader = compile latest vertex(worldViewProj, world, worldIT, viewI);
		PixelShader  = compile latest fragment(gAmbient, material);
	}
}

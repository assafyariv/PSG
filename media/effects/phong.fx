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
#include "include/phongLightingTor.cg"

#define M_PI 3.14159
#define MI_EPS 0.0001

float4x4 worldViewProj : WorldViewProjection; // model to clip
float4x4 world         : World;   // model to world
float4x4 worldIT       : WorldIT; // model to world
float4x4 viewI         : ViewI;   // view to world 

// gloabal ambiel term
float4 gAmbient : GlobalAmbient;

NVSGLight     light[8];
NVSGMaterial  material;

float
mi_ward_anisglossy(
	float3      di,         /* incident direction */
	float3      dr,         /* direction of reflection */
	float3      n,          /* normal */
	float3      u,          /* surface u direction */
	float3      v,          /* surface v direction */
	float       shiny_u,    /* shiny coefficient in u direction (5--100) */
	float       shiny_v)    /* shiny coefficient in v direction (5--100) */
{
	float3 h;   /* halfway vector */
	float  cosr, cosi, shiny_u2, shiny_v2;
	float  hn, hu, hv, expo;
	float  result = 0;

	cosr = dot(dr, n);
	cosi = -dot(di, n);

	if (cosi * cosr >= MI_EPS) {
		shiny_u2 = shiny_u * shiny_u;
		shiny_v2 = shiny_v * shiny_v;

		h = dr - di;
		h = normalize(h);
		hn = dot(h, n);
		hn *= hn;
		if (hn > 1.0) hn = 1.0;
		/*to avoid numerical instability when dot = 1*/
		hu = dot(h, u);
		hu *= hu;
		hv = dot(h, v);
		hv *= hv;

		/* exp will return 0 for small hn */
		if(hn >= MI_EPS * (hu*shiny_u2 + hv*shiny_v2)) {
			expo = - (hu*shiny_u2 + hv*shiny_v2) / hn;
			result = exp(expo) *
				shiny_u * shiny_v / (sqrt(cosi*cosr) * 4.0 * PI);
		}
	}
	return (result);
}

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
               , uniform NVSGLight light[8]
               , uniform NVSGMaterial material 
               ) : COLOR
{
  float3 P = worldPos;
  float3 V = normalize(eyePos - P);
  float3 N = normalize(normal);

  float3 ambientSum = 0;
  float3 diffuseSum = 0;
  float3 specularSum = 0;
  float frenel = lerp(0.5,0.02,pow(abs(dot(N,V)),1.0/5.0));
  for ( int i=0; i<light.length; ++i )
  {
    float3 ambientLight;
    float3 diffuseLight;
    float3 specularLight;
//zero roughness
    lighting_main( light[i], P, N, V,material.shininess,0.0, ambientLight, diffuseLight, specularLight );

    // sum up
    ambientSum += ambientLight;
    diffuseSum += diffuseLight;
    specularSum += specularLight;
  }

 

  float4 ambient = material.ambient * gAmbient * float4(ambientSum, 1);
  float4 diffuse = material.diffuse * float4(diffuseSum, 1);
  float4 specular = material.specular * float4(specularSum + frenel, 1);
  //specular *= mi_ward_anisglossy (N,-V,V,float3(1.0),float3(1.0),20.0,20.0);
  
  return material.emission + ambient + diffuse + specular; 
}

technique FragmentLighting_NV40
{
	pass p0
	{
		VertexProgram   = compile vp40 vertex(worldViewProj, world, worldIT, viewI);
		FragmentProgram = compile fp40 fragment(gAmbient, light, material);
	}
}

technique FragmentLighting_NV30
{
	pass p0
	{
		VertexProgram   = compile vp30 vertex(worldViewProj, world, worldIT, viewI);
		FragmentProgram = compile fp30 fragment(gAmbient, light, material);
	}
}

technique FragmentLighting_ARB
{
	pass p0
	{
		VertexProgram   = compile arbvp1 vertex(worldViewProj, world, worldIT, viewI);
		FragmentProgram = compile arbfp1 fragment(gAmbient, light, material);
	}
}

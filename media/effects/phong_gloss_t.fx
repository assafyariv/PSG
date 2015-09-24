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
#include "include/NVSGScene.cg"
#include "include/phongLightingTor.cg"

float4x4 worldViewProj : WorldViewProjection; // model to clip
float4x4 world         : World;   // model to world
float4x4 worldIT       : WorldIT; // model to world
float4x4 viewI         : ViewI;   // view to world 
float4x4 view         : View;   // view to world 
float4x4 worldview         : WorldView;   // view to world 

// gloabal ambiel term
//float4 gAmbient = {1.0,1.0,1.0,1.0} ;//: GlobalAmbient;

NVSGLight     light[8];
NVSGMaterial  material;
NVSGScene scene;

// Zero roughness means simple phong
//For other values <= 1 , we will use beckmann.
float roughness<
  string Desc   = "Roughness";
  string gui    = "slider";
  float uimin   = 0.0;
  float uimax   = 1.0;
  float uistep  = 0.1;
> = 0.1; 

float maxRefl
<
    string gui = "slider";
    float uimin = 0.1;
    float uimax = 1.0;
    float uistep = 0.05;
> = 0.5;

float minRefl
<
    string gui = "slider";
    float uimin = 0.0;
    float uimax = 1.0;
    float uistep = 0.05;
> = 0.02;

float fresnelEXP
<
    string gui = "slider";
    float uimin = 1.0;
    float uimax = 6.5;
    float uistep = 0.5;
> = 5.0;
float diffuseMapRotation = 0.0;
float4 diffuseMapPosition = {1.0,1.0,0.0,0.0} ;
texture diffuseMap ;

sampler2D diffuseMapSampler = sampler_state
{
  Texture = <diffuseMap>;

  MinFilter = LinearMipMapLinear;
  MagFilter = Linear;

  WrapS = Repeat;
  WrapT = Repeat;
  GenerateMipMap = true;
};

float GrayTarget
<
 string gui = "slider";
    float uimin = -255.0f;
    float uimax =  255.0f;
    float uistep = 0.05f;
> = 0.33f;


void vertex( float3 position      : POSITION
           , float3 normal        : NORMAL
	   , float2 texcoord0     : TEXCOORD0
           , out float4 oPosition : POSITION
           , out float4 worldPos  : TEXCOORD0
           , out float3 eyePos    : TEXCOORD1             
           , out float3 oNormal   : TEXCOORD2
	   , out float4 texUV     : TEXCOORD3 
   , out float oClp : CLP0 
 , out float oClp1 : CLP1 
           , uniform float4x4 worldViewProj 
           , uniform float4x4 world
           , uniform float4x4 worldIT
           , uniform float4x4 viewI
           , uniform float4x4 view
	  ,uniform float4 DiffuseMapPosition,
		uniform float texAlpha
           )
{
	oPosition = mul(worldViewProj, float4(position, 1.0f));

         float sinX = sin (texAlpha * 4);
         float cosX = cos (texAlpha * 4);
         float sinY = sin (texAlpha * 4 );
         float2x2 rotationMatrix = float2x2( cosX, -sinX, sinY, cosX);
  	 float2 scaledCoord    = float2(DiffuseMapPosition.z + texcoord0.x * DiffuseMapPosition.x,DiffuseMapPosition.w + texcoord0.y*DiffuseMapPosition.y);
	 texUV.xy = mul(scaledCoord,rotationMatrix);

	// compute eye position in world space
	float3 eyePosition = mul(viewI, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;
	float3 worldPosition =  mul(world, float4(position, 1.0f)).xyz;
	
        //float3 viewVector  = normalize(eyePosition -  worldPosition);
  
	float3 wnormal = mul(worldIT, float4(normal, 1.0f)).xyz;
        //float  fres = minrefl + (maxrefl-minrefl) * pow((1.0-abs(dot(viewVector,wnormal))), fresnelexp);
       // float  fres = lerp(maxrefl,minrefl,pow(abs(dot(wnormal,viewVector)),1.0/fresnelexp)); // I like this one better.
	oNormal = wnormal;
        eyePos = eyePosition;

	// custom clipping planes.
	float4 eyePoll  = mul(view, float4(worldPosition,1));
  	oClp =  dot(glstate.clip[0].plane,eyePoll);
oClp1 =  dot(glstate.clip[1].plane,eyePoll);
	// compute world space position
	worldPos.xyz = worldPosition.xyz;
	worldPos.w = 1.0;  
} 

float4 fragment( float4 worldPos  : TEXCOORD0
               , float3 eyePos    : TEXCOORD1
               , float3 normal    : TEXCOORD2
	       , float4 texUV     : TEXCOORD3
               , uniform float4 gAmbient
               , uniform NVSGLight light[8]
               , uniform NVSGMaterial material 
		, uniform float roughness
 		, uniform float minrefl
	        , uniform float maxrefl
	        , uniform float fresnelexp
		, uniform sampler2D DiffuseMapSampler
		, uniform float grayTarget
                ) : COLOR
{
  float3 P = worldPos.xyz;

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

    lighting_mainfres( light[i], P, N, V, material.shininess,roughness,minRefl, ambientLight, diffuseLight, specularLight );

    // sum up
    ambientSum += ambientLight;
    diffuseSum += diffuseLight;
    specularSum += specularLight;
  }
   

  float2 tc = texUV.xy ;
//  float4 diffuseColorTex = float4(tex2D(DiffuseMapSampler, tc).rgb,1.0);
  float4 diffuseColorTex = tex2D(DiffuseMapSampler, tc);

  float4  frenel =  lerp(maxrefl,0.0,pow(abs(dot(N,V)),1.0/fresnelexp)) ;//worldPos.w;//*  glossiness;
  float4 ambient = material.ambient * gAmbient +material.ambient * float4(ambientSum, 1);
  float4 diffuse = ((diffuseColorTex * material.diffuse) +(1-grayTarget)*material.diffuse) * float4(diffuseSum, 1);
  float4 specular = material.specular * float4(specularSum, 1);
// float4 specular = material.specular * float4(specularSum + frenel, 1);
  float4 mixCol = material.emission + ambient + diffuse + specular * (1-frenel)+frenel*material.specular;
  mixCol.w = material.diffuse.w * diffuseColorTex.w;
//  mixCol.w = material.diffuse.w;

  return mixCol;
}

void fragmentMRT( float4 worldPos  : TEXCOORD0
               , float3 eyePos    : TEXCOORD1
               , float3 normal    : TEXCOORD2
	       , float4 texUV     : TEXCOORD3
               , uniform float4 gAmbient
               , uniform NVSGLight light[8]
               , uniform NVSGMaterial material 
		, uniform float roughness
 		, uniform float minrefl
	        , uniform float maxrefl
	        , uniform float fresnelexp
		, uniform sampler2D DiffuseMapSampler
		, uniform float grayTarget
                , uniform float4x4 worldviewMat
                , out float4 normColor : COLOR0
                , out float4 deepColor : COLOR1
                ) 
{
  float3 P = worldPos.xyz;

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

    lighting_mainfres( light[i], P, N, V, material.shininess,roughness,minRefl, ambientLight, diffuseLight, specularLight );

    // sum up
    ambientSum += ambientLight;
    diffuseSum += diffuseLight;
    specularSum += specularLight;
  }
   

  float2 tc = texUV.xy ;
//  float4 diffuseColorTex = float4(tex2D(DiffuseMapSampler, tc).rgb,1.0);
  float4 diffuseColorTex = tex2D(DiffuseMapSampler, tc);

  float4  frenel =  lerp(maxrefl,0.0,pow(abs(dot(N,V)),1.0/fresnelexp)) ;//worldPos.w;//*  glossiness;
  float4 ambient = material.ambient * gAmbient +material.ambient * float4(ambientSum, 1);
  float4 diffuse = ((diffuseColorTex * material.diffuse) +(1-grayTarget)*material.diffuse) * float4(diffuseSum, 1);
  float4 specular = material.specular * float4(specularSum, 1);
// float4 specular = material.specular * float4(specularSum + frenel, 1);
  float4 mixCol = material.emission + ambient + diffuse + specular * (1-frenel)+frenel*material.specular;
  mixCol.w = material.diffuse.w * diffuseColorTex.w;
//  mixCol.w = material.diffuse.w;

float3 viewspacePos = mul (float4(P.xyz,1.0),worldviewMat).xyz;
normColor = float4(N.rgb, viewspacePos.z *0.01);

deepColor = mixCol;//float4(N.rgb,viewspacePos.z);

}

technique Main
{
	pass p0
	{
		VertexProgram   = compile latest vertex(worldViewProj, world, worldIT, viewI,view,diffuseMapPosition,diffuseMapRotation);
		FragmentProgram = compile latest fragment(scene.ambient, light, material,roughness,minRefl,maxRefl,fresnelEXP,diffuseMapSampler,GrayTarget );
	}
}

technique MainMRT
{
	pass p0
	{
		VertexProgram   = compile latest vertex(worldViewProj, world, worldIT, viewI,view,diffuseMapPosition,diffuseMapRotation);
		FragmentProgram = compile latest fragmentMRT(scene.ambient, light, material,roughness,minRefl,maxRefl,fresnelEXP,diffuseMapSampler,GrayTarget ,worldview);
	}
}

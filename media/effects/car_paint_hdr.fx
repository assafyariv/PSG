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
#include "include/textureHDR_RGBA.cg"
#include "include/NVSGLight.cg"
#include "include/NVSGMaterial.cg"
#include "include/phongLightingTor.cg"
#include "include/NVSGScene.cg"

NVSGLight     light[];
NVSGMaterial  material;
NVSGScene scene;

float4 lacquer = {0.0f, 0.0f, 0.0f, 1.0f};
float reflectivity = 0.5f;
float indexOfRefraction = 0.0f;

float roughness
<
  string Desc   = "Roughness";
  string gui    = "slider";
  float uimin   = 0.0;
  float uimax   = 1.0;
  float uistep  = 0.1;
> = 0.37; 


float maxRefl
<
    string gui = "slider";
    float uimin = 0.1;
    float uimax = 1.0;
    float uistep = 0.05;
> = 0.6;

float minRefl
<
    string gui = "slider";
    float uimin = 0.0;
    float uimax = 1.0;
    float uistep = 0.05;
> = 0.2;

float fresnelEXP
<
    string gui = "slider";
    float uimin = 1.0;
    float uimax = 6.5;
    float uistep = 0.5;
> = 1.0;

//float shininess
//<
//	string gui = "slider";
//	float uimin = 0.0;
//	float uimax = 40.0;
//	float uistep = 2.0;
//> = 12;

float Exposure
<
 string gui = "slider";
    float uimin = -10.0f;
    float uimax =  10.0f;
    float uistep = 0.05f;
> = -0.65f;

float GrayTarget
<
 string gui = "slider";
    float uimin = -255.0f;
    float uimax =  255.0f;
    float uistep = 0.05f;
> = 0.33f;

float4 knee (float4 x, float f)
{
  return log (x * f + 1) / f;
}

VS_OUTPUT mainVS( VS_INPUT IN,
                  uniform float4x4 WorldViewProj,
                  uniform float4x4 World,
                  uniform float4x4 WorldI,
                  uniform float4x4 ViewI,
		  uniform float4x4 View,
                  uniform float    minRefl,
                  uniform float    maxRefl,
                  uniform float    fresnelEXP,
		 uniform float roughh
               ) 
{
  VS_OUTPUT OUT;

  OUT.position  = mul(WorldViewProj, IN.position);
  OUT.texcoord0 = IN.texcoord0;
  
  float3 position    = mul(World, IN.position).xyz;
  float3 normal      = normalize(mul(IN.normal, WorldI).xyz); // multiply with inverse transpose	
  float3 eyePosition = mul(ViewI, float4(0.0, 0.0, 0.0, 1.0)).xyz;
  float3 viewVector  = normalize(eyePosition - position);
  
  float  fres = minRefl + (maxRefl-minRefl) * pow((1.0-abs(dot(viewVector,normal))), fresnelEXP) / (4.0- 3.0*(1.0-roughh));
  
  OUT.texcoord4.w   = fres;
  OUT.texcoord4.xyz = viewVector;
  
  OUT.texcoord5.xyz = position;
  OUT.texcoord6.xyz = normal;

  OUT.color = IN.color;  

  // custom clipping planes.
  float4 eyePoll  = mul(View, float4(position   ,1));
  OUT.oClp =  dot(glstate.clip[0].plane,eyePoll);
 OUT.oClp1 =  dot(glstate.clip[1].plane,eyePoll);
  return OUT;
}

FRAGOUT mainPS( VS_OUTPUT IN,
                uniform samplerCUBE textureENV,
                uniform NVSGMaterial material,
                uniform float4      lacquer,
                uniform float exposure,
                uniform float grayTarget,
 	        uniform float roughh,
		uniform float reflectivity) 
{
  float GRTT = 0.33; // the real Gray target
  float3 normal      = normalize(IN.texcoord6.xyz);
  float3 worldPos = IN.texcoord5.xyz;////normalize(IN.texcoord5.xyz);
  float3 viewVector  = IN.texcoord4.xyz;//normalize(IN.texcoord4.xyz);
  float3 reflVector;

  if (indexOfRefraction == 0.0)
     reflVector = reflect(-viewVector, IN.texcoord6.xyz);
  else
     reflVector  = refract(-viewVector, -normal,indexOfRefraction);

  exposure   = pow(2.0h, exposure + 2.47393h) ;

  float4 cubeColor;
  float mipbias = calcMipLevel (roughh,material.shininess);

  cubeColor = texCUBEbias(textureENV, reflVector, mipbias);
  cubeColor.xyz = max(float3(0.0h, 0.0h, 0.0h), cubeColor.xyz * exposure);
  // shouldn't this be done for each channel ?
  cubeColor = (cubeColor > 0.0h) ? knee (cubeColor, 0.184874h) : cubeColor;
  cubeColor*= GRTT ;

  float3 ambientSum = 0;
  float3 diffuseSum = 0;
  float3 specularSum = 0;
 // float myfrens = max(0.0,(fresnelEXP/7.0));

  //float roughh = grayTarget;
  for ( int i=0; i<light.length; ++i )
  {
    float3 ambientLight;
    float3 diffuseLight;
    float3 specularLight;

   lighting_mainfres( light[i], worldPos, normal, viewVector,material.shininess,roughh ,minRefl, ambientLight, diffuseLight, specularLight );

    // sum up
    ambientSum += ambientLight;
    diffuseSum += diffuseLight;
    specularSum += specularLight;
  }

  float fres = IN.texcoord4.w;
  float4 specular = float4(specularSum, 1.0)* (material.specular );
 //  specular = float4(specularSum, 1.0)* (fres * material.specular + (1-fres)*float4( (float4(1,1,1,1) - material.specular).xyz,1)); //+ pow(abs(dot( normal, viewVector)),1.0/fresnelEXP)*float4( (float4(1,1,1,1) - material.specular).xyz,1) );
  float4 metalColor = material.ambient*float4(ambientSum,1) + material.ambient*scene.ambient + material.diffuse*float4(diffuseSum, 1) + specular;
  float4 reflColor = (1.0-fres)*( float4(lacquer.xyz,1.0) * (1-grayTarget) ) + fres * cubeColor * reflectivity;
 //float4 reflColor = (1.0-fres)*( float4(lacquer.xyz,1.0) * (1-grayTarget) ) + cubeColor *((float4(float3(1.0,1.0,1.0)-specular.xyz, 1.0))*fres + specular)* reflectivity + material.diffuse*float4(diffuseSum, 1)*cubeColor*reflectivity;

  float4 result = metalColor + reflColor;
//  float4 result = (1.0-fres)*( float4(lacquer.xyz,1.0) * (1-grayTarget) + material.ambient*float4(ambientSum,1)*scene.ambient + material.diffuse*((grayTarget)*float4(diffuseSum, 1)) + (grayTarget)*float4(specularSum, 1.0)*material.specular) + fres*cubeColor;
  
// Post bleach effect. 
//    float4 base = result;
//    float3 lumCoeff = float3(0.25,0.65,0.1);
//    float lum = dot(lumCoeff,base.rgb);
//    float3 blend = lum.rrr;
//    float L = min(1,max(0,10*(lum- 0.45)));
//    float3 result1 = 2.0f * base.rgb * blend;
//    float3 result2 = 1.0f - 2.0f*(1.0f-blend)*(1.0f-base.rgb);
//    float3 newColor = lerp(result1,result2,L);
//    float3 A2 = 0.04 * base.a;
//    float3 mixRGB = A2 * newColor.rgb;
//    mixRGB += ((1.0f-A2) * base.rgb);
//    result = float4(mixRGB,1.0);
  FRAGOUT OUT; 
  OUT.color = result;
  return OUT;
}

void mainPSMRT( VS_OUTPUT IN,
                uniform samplerCUBE textureENV,
                uniform NVSGMaterial material,
                uniform float4      lacquer,
                uniform float exposure,
                uniform float grayTarget,
 	        uniform float roughh,
		uniform float reflectivity,
	        uniform float4x4 worldviewMat
                , out float4 normColor : COLOR0
                , out float4 deepColor : COLOR1) 
{
  float GRTT = 0.33; // the real Gray target
  float3 normal      = normalize(IN.texcoord6.xyz);
  float3 worldPos = IN.texcoord5.xyz;////normalize(IN.texcoord5.xyz);
  float3 viewVector  = IN.texcoord4.xyz;//normalize(IN.texcoord4.xyz);
  float3 reflVector;

  if (indexOfRefraction == 0.0)
     reflVector = reflect(-viewVector, IN.texcoord6.xyz);
  else
     reflVector  = refract(-viewVector, -normal,indexOfRefraction);

  exposure   = pow(2.0h, exposure + 2.47393h) ;

  float4 cubeColor;
  float mipbias = calcMipLevel (roughh,material.shininess);

  cubeColor = texCUBEbias(textureENV, reflVector, mipbias);
  cubeColor.xyz = max(float3(0.0h, 0.0h, 0.0h), cubeColor.xyz * exposure);
  // shouldn't this be done for each channel ?
  cubeColor = (cubeColor > 0.0h) ? knee (cubeColor, 0.184874h) : cubeColor;
  cubeColor*= GRTT ;

  float3 ambientSum = 0;
  float3 diffuseSum = 0;
  float3 specularSum = 0;
 // float myfrens = max(0.0,(fresnelEXP/7.0));

  //float roughh = grayTarget;
  for ( int i=0; i<light.length; ++i )
  {
    float3 ambientLight;
    float3 diffuseLight;
    float3 specularLight;

   lighting_mainfres( light[i], worldPos, normal, viewVector,material.shininess,roughh ,minRefl, ambientLight, diffuseLight, specularLight );

    // sum up
    ambientSum += ambientLight;
    diffuseSum += diffuseLight;
    specularSum += specularLight;
  }

  float fres = IN.texcoord4.w;
  float4 specular = float4(specularSum, 1.0)* (material.specular );
 //  specular = float4(specularSum, 1.0)* (fres * material.specular + (1-fres)*float4( (float4(1,1,1,1) - material.specular).xyz,1)); //+ pow(abs(dot( normal, viewVector)),1.0/fresnelEXP)*float4( (float4(1,1,1,1) - material.specular).xyz,1) );
  float4 metalColor = material.ambient*float4(ambientSum,1) + material.ambient*scene.ambient + material.diffuse*float4(diffuseSum, 1) + specular;
  float4 reflColor = (1.0-fres)*( float4(lacquer.xyz,1.0) * (1-grayTarget) ) + fres * cubeColor * reflectivity;
 //float4 reflColor = (1.0-fres)*( float4(lacquer.xyz,1.0) * (1-grayTarget) ) + cubeColor *((float4(float3(1.0,1.0,1.0)-specular.xyz, 1.0))*fres + specular)* reflectivity + material.diffuse*float4(diffuseSum, 1)*cubeColor*reflectivity;

  float4 result = metalColor + reflColor;
//  float4 result = (1.0-fres)*( float4(lacquer.xyz,1.0) * (1-grayTarget) + material.ambient*float4(ambientSum,1)*scene.ambient + material.diffuse*((grayTarget)*float4(diffuseSum, 1)) + (grayTarget)*float4(specularSum, 1.0)*material.specular) + fres*cubeColor;
  
// Post bleach effect. 
//    float4 base = result;
//    float3 lumCoeff = float3(0.25,0.65,0.1);
//    float lum = dot(lumCoeff,base.rgb);
//    float3 blend = lum.rrr;
//    float L = min(1,max(0,10*(lum- 0.45)));
//    float3 result1 = 2.0f * base.rgb * blend;
//    float3 result2 = 1.0f - 2.0f*(1.0f-blend)*(1.0f-base.rgb);
//    float3 newColor = lerp(result1,result2,L);
//    float3 A2 = 0.04 * base.a;
//    float3 mixRGB = A2 * newColor.rgb;
//    mixRGB += ((1.0f-A2) * base.rgb);
//    result = float4(mixRGB,1.0);
  //FRAGOUT OUT; 
 // OUT.color = result;
  //return OUT;

float3 viewspacePos = mul (float4(worldPos.xyz,1.0),worldviewMat).xyz;
normColor = float4(normal, abs(viewspacePos.z)*0.01);

//normColor = float4(normal.rgb,worldPos.z); 

deepColor =result;
}


technique Main
{
  pass p0 
  {
    DepthTestEnable = true;
    DepthMask = true;
    FrontFace = CCW;
    //CullFaceEnable = false;
    LightModelTwoSideEnable = true;
    
    VertexProgram   = compile latest mainVS(Wvp, World, WorldI, ViewI,View, minRefl, maxRefl, fresnelEXP,roughness);
    FragmentProgram = compile latest mainPS(textureHDR_RGBA,  material, lacquer, Exposure, GrayTarget,roughness,reflectivity);
  }
}
technique MainMRT
{
  pass p0 
  {
    DepthTestEnable = true;
    DepthMask = true;
    FrontFace = CCW;
    //CullFaceEnable = false;
    LightModelTwoSideEnable = true;
    
    VertexProgram   = compile latest mainVS(Wvp, World, WorldI, ViewI,View, minRefl, maxRefl, fresnelEXP,roughness);
    FragmentProgram = compile latest mainPSMRT(textureHDR_RGBA,  material, lacquer, Exposure, GrayTarget,roughness,reflectivity,WorldView);
  }
}
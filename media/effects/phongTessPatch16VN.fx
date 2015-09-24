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

#include "include/bezier.cg"
#include "include/matrices.cg"
#include "include/NVSGLight.cg"
#include "include/NVSGMaterial.cg"
#include "include/phongLighting.cg"
#include "include/tessellation.cg"

float4        gAmbient : GlobalAmbient;
NVSGLight     light[];
NVSGMaterial  material;

gp5vp void tessVertex
(
  in  float3 position  : POSITION,
  in  float3 normal    : NORMAL,
  out float3 oPosition : POSITION,
  out float3 oNormal   : TEXCOORD0
)
{
  oPosition = position;
  oNormal   = normal;
}

gp5tcp PATCH_16 void tessCtrl
(
  in int id                       : CONTROLPOINT_ID,
  in AttribArray<float3> position : POSITION,
  in AttribArray<float3> normal   : TEXCOORD0,

  out float3 oPosition     : POSITION,
  out float3 oNormal       : ATTR0,
  out float  oEdgeTess[4]  : EDGETESS,
  out float  oInnerTess[2] : INNERTESS
)
{
  // Per control-point output
  oPosition = position[id];
  oNormal   = normal[id];

  // Per patch output
  oEdgeTess[0] = calculateTessFactor( position[0], position[4], position[8], position[12] );
  oEdgeTess[1] = calculateTessFactor( position[0], position[1], position[2], position[3] );
  oEdgeTess[2] = calculateTessFactor( position[3], position[7], position[11], position[15] );
  oEdgeTess[3] = calculateTessFactor( position[12], position[13], position[14], position[15] );
  
  oInnerTess[0] = calculateTessFactor( position );
  oInnerTess[1] = oInnerTess[0];
}

gp5tep PATCH_16 void tessEval
(
  in float2              uv       : UV,
  in AttribArray<float3> position : POSITION,
  in AttribArray<float3> normal   : ATTR0,

  out float4 oPosition : POSITION,
  out float3 oNormal   : ATTR0,
  out float3 worldPos  : ATTR1,
  out float3 eyePos    : ATTR2,

  uniform float4x4 modelToClip,
  uniform float4x4 modelToWorld,
  uniform float4x4 worldToModel,
  uniform float4x4 viewToWorld
)
{
  float3 p = bezierRect3P( uv.x, uv.y, position );
  float3 n = bezierRect3P( uv.x, uv.y, normal );

  oPosition = mul( modelToClip, float4( p, 1.0f ) );
  oNormal = mul( float4( n, 0.0f ), worldToModel ).xyz;
  
  // compute world space position
  worldPos = mul( modelToWorld, float4( p, 1.0f ) ).xyz;
  
  // compute eye position in world space
  eyePos = mul( viewToWorld, float4( 0.0f, 0.0f, 0.0f, 1.0f ) ).xyz;
}

gp5fp void tessFragment
(
  float3 normal    : ATTR0,
  float3 worldPos  : ATTR1,
  float3 eyePos    : ATTR2,

  out float4 oColor : COLOR,

  uniform float4 gAmbient,
  uniform NVSGMaterial material 
)
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

  oColor = material.emission + ambient + diffuse + specular; 
}

technique FragmentLighting_Patches
{
  pass p0
  {
    VertexProgram                 = compile gp5vp tessVertex();
    TessellationControlProgram    = compile gp5tcp tessCtrl();
    TessellationEvaluationProgram = compile gp5tep "QUADS" tessEval( Wvp, World, WorldI, ViewI );
    FragmentProgram               = compile gp5fp tessFragment( gAmbient, material );
  }
}

// Copyright NVIDIA Corporation 2011
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#include "AvocadoPipeline.h"
#include <nvgl/SceneRendererGL2.h>
#include <nvgl/RenderTargetGLFBO.h>
#include <nvgl/RendererGLFSQ.h>
#include <nvsg/GeoNode.h>

#include "SceneFunctions.h"
#include <nvsg/PerspectiveCamera.h>
#include <nvsg/FaceAttribute.h>
#include <nvtraverser\SearchTraverser.h>
#include <nvsg/CoreTypes.h>
#include <nvmath/Vecnt.h>

using namespace nvsg;
using namespace nvgl;
using namespace nvui;
using namespace nvmath;
using namespace nvtraverser;
using namespace nvutil;

static const std::string g_cgfxStencilToColor = 
"float4 vertex(float4 vpos : POSITION) : POSITION\n"
"{\n"
"  return vpos;\n"
"}\n"
"float4 fragment() : COLOR\n"
"{\n"
"  return float4(1.0f);\n"
"}\n"
"technique StencilToColor\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexProgram   = compile latest vertex();\n"
"    FragmentProgram = compile latest fragment();\n"
"    DepthTestEnable = false;\n"
"    StencilFunc = { Equal, 1, ~0 };\n"
"    StencilOp   = { Keep, Keep, Keep };\n"
"    StencilTestEnable = true;\n"
"  }\n"
"}\n";

static const std::string g_cgfxDepthToColor = 
"float4 vertex(float4 vpos : POSITION) : POSITION\n"
"{\n"
"  return vpos;\n"
"}\n"
"float4 fragment(float4 fpos : POSITION) : COLOR\n"
"{\n"
"  return float4(fpos.z,fpos.z,fpos.z,1.0f);\n"
"}\n"
"technique StencilToColor\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexProgram   = compile latest vertex();\n"
"    FragmentProgram = compile latest fragment();\n"
"    DepthTestEnable = false;\n"
"    StencilFunc = { Equal, 1, ~0 };\n"
"    StencilOp   = { Keep, Keep, Keep };\n"
"    StencilTestEnable = true;\n"
"  }\n"
"}\n";
static const std::string g_cgfxSimple  = 
"//! The NVSGLight structure specifies the data layout for an effect light parameter.\n"
"/** A parameter of type NVSGLight will be automatic bound to a tree light in NVSG. */\n"
"struct NVSGLight\n"
"{\n"
"  float4 ambient;                //!< Specifies the ambient RGBA intensity of the light.\n"
"  float4 diffuse;                //!< Specifies the diffuse RGBA intensity of the light.\n"
"  float4 specular;               //!< Specifies the specular RGBA intensity of the light.\n"
"  float4 position;               //!< Specifies the light position in world coordinates.\n"
"  float4 direction;              //!< Specifies the light direction in world coordinates.\n"
"  float  spotExponent;           //!< Specifies the intensity distribution of the light.\n"
"  float  spotCutoff;             //!< Specifies the maximum spread angle of the light.\n"
"  float  constantAttenuation;    //!< Specifies the constant light attenuation factor.\n"
"  float  linearAttenuation;      //!< Specifies the linear light attenuation factor.\n"
"  float  quadraticAttenuation;   //!< Specifies the quadratic light attenuation factor.\n"
"};\n"
"NVSGLight lights[];\n"
"float4x4 world                 : WORLD;\n"
"float4x4 view_projection       : VIEWPROJECTION;\n"
//"float4x4 world_it              : WORLDINVERSETRANSPOSE;\n"
"float4x4 view_i                : VIEWINVERSE;\n"
"float3 Color = {1.0f, .0f, .0f};\n"
"float4 vertex(float4 vpos : POSITION, float3 normal : NORMAL,\n"
//"              out float4 ovpos : POSITION,
"              out float4 color : COLOR) : POSITION\n"
"{\n"
//"  ovpos = mul(view_projection, vpos);\n"
"  float4 P = mul(world, vpos); // position in world coordinates\n"
"  float3 eyePos = mul(view_i, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;\n"
"  float3 V = normalize(eyePos - vpos.xyz);\n"
"  float3 L = float3(0.25f);\n"//normalize(lights[0].position.xyz - P.xyz);\n"
"  float3 N = normalize (normal);\n"
"  float edge = max(dot(N, V), 0);\n"
"  float diffuseLight = max(dot(N, L), 0);\n"

//"  float edgeCol = (edge < 0.2f ? 0.0f : (edge < 0.6f ? 0.5f : 1.0f ));\n"
"  float edgeCol = (edge < 0.13f ? 0.0f : 1.0f);\n"
"  float diffCol = (diffuseLight < 0.13f ? 0.25f : (diffuseLight < 0.7f ? 0.6f : 1.0f) );\n"
"  edgeCol = (diffuseLight * edgeCol) ;\n"
"  color = float4 (edgeCol,edgeCol,0.0f,1.0f);\n"
"  return mul(view_projection, P); // position in clip space\n"
//"  return vpos;\n"
"}\n"
"float4 fragment(float4 pos : POSITION,\n"
"                in float4 color : COLOR) : COLOR\n"
"{\n"
"    return color;\n"
//"  return float4(1.0f,1.0f,0.0f,0.5f);\n"
"}\n"
"technique StencilToColor\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexProgram   = compile latest vertex();\n"
"    FragmentProgram = compile latest fragment();\n"
"  }\n"
"}\n";





static const std::string g_cgfxSi4mple =
"//! The NVSGLight structure specifies the data layout for an effect light parameter.\n"
"/** A parameter of type NVSGLight will be automatic bound to a tree light in NVSG. */\n"
"struct NVSGLight\n"
"{\n"
"  float4 ambient;                //!< Specifies the ambient RGBA intensity of the light.\n"
"  float4 diffuse;                //!< Specifies the diffuse RGBA intensity of the light.\n"
"  float4 specular;               //!< Specifies the specular RGBA intensity of the light.\n"
"  float4 position;               //!< Specifies the light position in world coordinates.\n"
"  float4 direction;              //!< Specifies the light direction in world coordinates.\n"
"  float  spotExponent;           //!< Specifies the intensity distribution of the light.\n"
"  float  spotCutoff;             //!< Specifies the maximum spread angle of the light.\n"
"  float  constantAttenuation;    //!< Specifies the constant light attenuation factor.\n"
"  float  linearAttenuation;      //!< Specifies the linear light attenuation factor.\n"
"  float  quadraticAttenuation;   //!< Specifies the quadratic light attenuation factor.\n"
"};\n"
"NVSGLight lights[];\n"
"// Vertex attribute interface:\n"
"struct APP2VP\n"
"{\n"
"  float4 position : POSITION;\n"
"  float3 normal   : NORMAL;\n"
"};\n"
"// Varyings interface:\n"
"struct VP2FP \n"
"{\n"
"  float4 position : POSITION;\n"
"  float3 normal   : TEXCOORD0;\n"
"  float3 worldPos : TEXCOORD1;\n"
"  float3 eyePos   : TEXCOORD2;\n"
"  float4 lightPos : TEXCOORD3;\n"
"};\n"
"// Non-tweakables:\n"
"// These semantics are known by SceniX and will be filled in during rendering tarversal.\n"
"float4x4 world                 : WORLD;\n"
"float4x4 view_projection       : VIEWPROJECTION;\n"
"float4x4 world_it              : WORLDINVERSETRANSPOSE;\n"
"float4x4 view_i                : VIEWINVERSE;\n"
"// User defined tweakables:\n"
//"float3 diffuse = float3(0.5f, 0.5f, 0.5f);\n"
"VP2FP vertex(APP2VP IN)\n"
"{\n"
"  VP2FP OUT;\n"
"  float4 P = mul(world, IN.position); // position in world coordinates\n"
"  OUT.position = mul(view_projection, P); // position in clip space\n"
"  OUT.normal   = mul(world_it, float4(IN.normal, 0.0f)).xyz;\n"
"  OUT.worldPos = P.xyz;\n"
"  OUT.eyePos   = mul(view_i, float4(0.0f, 0.0f, 0.0f, 1.0f)).xyz;\n"
"  OUT.lightPos = mul(shadowMatrix, P); // position in light coordinates\n"
"  return OUT;\n"
"}\n"
"float4 fragment(VP2FP IN) : COLOR \n"
"{\n"
"return float4(1.0f,0.0f,0.0f,1.0f);\n"
"}\n"
"technique StencilToColor\n"
"{\n"
"  pass p0\n"
"  {\n"
"    VertexProgram   = compile latest vertex();\n"
"    FragmentProgram = compile latest fragment();\n"
"  }\n"
"}\n";










static const std::string g_cgfxHighlight = 
// RendererGLFSQ sends TEXTURE_2D coordinates in TEXCOORD0 and TEXTURE_RECTANGLE coordinates in TEXCOORD1.
// Laplace filter kernel for single pixel silhouette outside of the object:
// 0  1  0\n"
// 1 -4  1\n"
// 0  1  0\n"
"float4 vertex(float4 vpos : POSITION, float2 tc : TEXCOORD1, out float2 tcOut : TEXCOORD0) : POSITION\n"
"{\n"
"  tcOut = tc;\n"
"  return vpos;\n"
"}\n"
"samplerRECT selection : RT0_COLOR_ATTACHMENT0 = sampler_state { MagFilter = Nearest; MinFilter = Nearest; };\n"
"float4 fragment(float2 tc : TEXCOORD0) : COLOR\n"
"{\n"
"  float sample = -4.0f * texRECT(selection, tc).x;\n"           // center
"  sample += texRECT(selection, float2(tc.x, tc.y - 1.0f)).x;\n" // bottom
"  sample += texRECT(selection, float2(tc.x - 1.0f, tc.y)).x;\n" // left
"  sample += texRECT(selection, float2(tc.x + 1.0f, tc.y)).x;\n" // right
"  sample += texRECT(selection, float2(tc.x, tc.y + 1.0f)).x;\n" // top
"  if (sample > 0.5f) \n"
"  return float4(0.0f, 1.0f, 0.0f, 1.0f);\n"
"  for (float dx = -2.0f; dx <= 2.0f; dx+=1.0f) \n"
"  for (float dy = -2.0f; dy <= 2.0f; dy+=1.0f) \n"
" {\n"
"  sample = -4.0f * texRECT(selection, float2(tc.x+dx,tc.y+dy)).x;\n"   
"  sample += texRECT(selection, float2(tc.x + dx, tc.y +dy - 1.0f)).x;\n" // bottom
"  sample += texRECT(selection, float2(tc.x + dx - 1.0f, dy+tc.y)).x;\n" // left
"  sample += texRECT(selection, float2(tc.x + dx + 1.0f, dy+tc.y)).x;\n" // right
"  sample += texRECT(selection, float2(tc.x + dx, tc.y + dy+1.0f)).x;\n" // top
"  if (sample > 0.0f) \n"
"  return float4(0.0f, 0.7f * (1.0f - ( sqrt(dx*dx+dy*dy) / 7.0f ) ) , 0.0f,1.0f - sqrt(dx*dx+dy*dy) / 7.0f );\n"
" }\n"
"  return float4(0.0f);\n" // yellow
"}\n"
"technique\n"
"{\n"
"  pass\n"
"  {\n"
"    VertexProgram   = compile latest vertex();\n"
"    FragmentProgram = compile latest fragment();\n"
"    AlphaFunc = { Greater, 0.0f };\n"
"    AlphaTestEnable = true;\n"
"    DepthTestEnable = false;\n"
"  }\n"
"}\n";

static const std::string g_cgfxGlowHighlight = 
// RendererGLFSQ sends TEXTURE_2D coordinates in TEXCOORD0 and TEXTURE_RECTANGLE coordinates in TEXCOORD1.
// Laplace filter kernel for single pixel silhouette outside of the object:
// 0  1  0\n"
// 1 -4  1\n"
// 0  1  0\n"
"float4 vertex(float4 vpos : POSITION, float2 tc : TEXCOORD1, out float2 tcOut : TEXCOORD0) : POSITION\n"
"{\n"
"  tcOut = tc;\n"
"  return vpos;\n"
"}\n"
"samplerRECT contour : RT0_COLOR_ATTACHMENT0 = sampler_state { MagFilter = Nearest; MinFilter = Nearest; };\n"
"float4 fragment(float2 tc : TEXCOORD0) : COLOR\n"
"{\n"
"  float sample = -4.0f * texRECT(contour, tc).y;\n"           // center
"  sample += texRECT(contour, float2(tc.x, tc.y - 1.0f)).y;\n" // bottom
"  sample += texRECT(contour, float2(tc.x - 1.0f, tc.y)).y;\n" // left
"  sample += texRECT(contour, float2(tc.x + 1.0f, tc.y)).y;\n" // right
"  sample += texRECT(contour, float2(tc.x, tc.y + 1.0f)).y;\n" // top
"  return (sample > 0.0f) ? float4(0.0f, 0.7f, 0.1f, 1.0f) : float4(0.0f);\n" // yellow
"}\n"
"technique\n"
"{\n"
"  pass\n"
"  {\n"
"    VertexProgram   = compile latest vertex();\n"
"    FragmentProgram = compile latest fragment();\n"
"    AlphaFunc = { Greater, 0.0f };\n"
"    AlphaTestEnable = true;\n"
"    DepthTestEnable = false;\n"
"  }\n"
"}\n";

static const std::string g_cgfxSillHighlight = 
// RendererGLFSQ sends TEXTURE_2D coordinates in TEXCOORD0 and TEXTURE_RECTANGLE coordinates in TEXCOORD1.
// Laplace filter kernel for single pixel silhouette outside of the object:
// 0  1  0\n"
// 1 -4  1\n"
// 0  1  0\n"

"\n"
"float4 vertex(float4 position : POSITION,float3 normal : NORMAL, out float4 oPosition: POSITION "
" , out float  diffuseLight  : TEXCOORD0, out float  specularLight : TEXCOORD1, out float  edge          : TEXCOORD2,"
" uniform float3 lightPosition,       uniform float3 eyePosition,        uniform float  shininess,   uniform float4x4 modelViewProj)\n"
"{\n"
"    oPosition = mul(modelViewProj, position);\n"
"  // Calculate diffuse lighting\n"
"  float3 N = normalize(normal);\n"
 " float3 L = normalize(lightPosition - position.xyz);\n"
"  diffuseLight = max(dot(N, L), 0);\n"

  "// Calculate specular lighting\n"
 " float3 V = normalize(eyePosition - position.xyz);\n"
 " float3 H = normalize(L + V);\n"
 " specularLight = pow(max(dot(N, H), 0), shininess);\n"
"  if (diffuseLight <= 0) specularLight = 0;\n"
 " edge = max(dot(N, V), 0);\n"
"}\n"
"float4 fragment(float diffuseLight : TEXCOORD0, float specularLight: TEXCOORD1,float edge : TEXCOORD2,  out float4 color : COLOR, "
"uniform float4 Kd, uniform float4 Ks,  uniform sampler1D diffuseRamp, uniform sampler1D specularRamp, uniform sampler1D edgeRamp)\n"
"{\n"
 " diffuseLight = tex1D(diffuseRamp, diffuseLight).x;\n"
 " specularLight = tex1D(specularRamp, specularLight).x;\n"
 " edge = tex1D(edgeRamp, edge).x;\n"
 " color = edge * (Kd * diffuseLight + Ks * specularLight);\n"
"}\n"
"technique\n"
"{\n"
"  pass\n"
"  {\n"
"    VertexProgram   = compile latest vertex();\n"
"    FragmentProgram = compile latest fragment();\n"
"    AlphaFunc = { Greater, 0.0f };\n"
"    AlphaTestEnable = true;\n"
"    DepthTestEnable = false;\n"
"  }\n"
"}\n";
nvutil::SmartPtr<SceneRendererPipeline::MonoViewStateProvider> SceneRendererPipeline::MonoViewStateProvider::create()
{
  return new SceneRendererPipeline::MonoViewStateProvider();
}

// This does nothing except for providing the setSceneRenderer() override which 
// allows to set the scene renderer before any OpenGL resources have been allocated.
SceneRendererPipeline::SceneRendererPipeline(int viewId) 
{
	m_isExporting = false;
	m_doingPreHighlight = false;
	m_highlighting	= true;
	m_shaderstart	= true;
	m_CellShading	= false;
	m_flatShading   = false;
	m_HLR			= false;
	m_useOptix		= false;
	m_hasHighlightedObject = false;
	m_viewId = viewId;
	m_backColor = nvmath::Vec4f  (0.97f,0.97f,0.985f,0.0f);

	m_cachedRoomTemp = 0;
	m_postprocGamma = 2.2f;
	m_postprocToneMap = 1.0;
	m_postprocFXAA = 1.0;

	m_postprocBrightness = 1.0f;
	m_postprocContrast = 1.0f;
	m_postprocHue = 0.0f;
	m_postprocSaturation = 1.0f;
	m_postprocSoftness = 0.0f;
	m_postprocNoise = 0.0f;
	m_postprocAOIntensity = 0.0;
	m_postEnabledBrightness = false;
	m_postEnabledContrast = false;
	m_postEnabledHue = false;
	m_postEnabledSaturation = false;
	m_postEnabledNoise = false;
	m_postEnabledSoften = false;
	m_postEnabledAO = false;
	m_postEnabledDOF = false;
	m_postEnabledGamma = false;
	m_postEnabledToneMap = false;
	m_postProcEnabled = false;
	m_monoViewStateProvider = MonoViewStateProvider::create();
}

SceneRendererPipeline::~SceneRendererPipeline()
{
  // The ViewerRendererWidget destructor makes the context current to allow cleanup of OpenGL resources!
	m_rendererSketch.reset ();
	m_sceneRendererSketch.reset ();
#ifdef _DOING_POST_PROCESS
	m_sceneRendererPostProc.reset ();
	m_postprocFBO->getRenderContextGL()->makeCurrent ();
	m_postprocFBO->clearAttachments ();
	m_postprocFBO.reset ();
#endif
#ifdef _DOING_POST_AO
	m_postprocAOMAP->getRenderContextGL()->makeCurrent ();
	m_postprocAOMAP->clearAttachments ();
	m_postprocAOMAP.reset ();
#endif
#ifdef _DOING_SHADER_HLR
		m_sketchFBO->getRenderContextGL()->makeCurrent();
 //   GLDisable(GL_STENCIL_TEST); // The CgFX shaders in the following two steps care for the state themselves.
		m_sketchFBO->clearAttachments();
		m_sketchFBO.reset();
#endif
	//m_sketchFBO->getRenderContextGL()->makeNoncurrent();

  m_rendererHighlight.reset();
  m_rendererStencilToColor.reset();
  m_sceneRendererHighlight.reset();
  m_highlightFBO.reset();

  // m_sceneRendererSketch.reset ();
 // m_sketchFBO.reset ();
 
  //m_imgProcEffect.reset ();
}

void
SceneRendererPipeline::ResetTargets ()
{
/*	  m_rendererHighlight.reset();
	  m_rendererStencilToColor.reset();
	  m_sceneRendererHighlight.reset();
	  m_highlightFBO.reset();*/
}
void SceneRendererPipeline::SetPostProcValue (std::string param,float val)
{
#ifdef _DOING_POST_PROCESS
	if (m_rendererPostproc)
	{
		float xv  = val;
		nvsg::CgFxSharedPtr cgf = m_rendererPostproc->getCgFx ();
		if (cgf)
		{
			CgFxEffectSharedPtr eff = CgFxWriteLock (cgf)->getEffect ();
			if (eff)
			{
				CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName (param);
				if (pone)
					CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&xv);
			}
		}
	}
#ifdef _DOING_POST_AO
	if (m_rendererPostprocAO)
	{
		float xv  = val;
		nvsg::CgFxSharedPtr cgf = m_rendererPostprocAO->getCgFx ();
		if (cgf)
		{
			CgFxEffectSharedPtr eff = CgFxWriteLock (cgf)->getEffect ();
			if (eff)
			{
				CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName (param);
				if (pone)
					CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&xv);
			}
		}
	}
#endif
#endif
}

bool SceneRendererPipeline::SetAmbientOcclusionTechnique (const std::string techName)
{
	
			nvsg::CgFxSharedPtr cgf = m_rendererPostprocAO->getCgFx ();
			if (cgf)
			{
				CgFxEffectSharedPtr eff = CgFxWriteLock (cgf)->getEffect ();
				if (eff)
				{
					CgFxTechnique tech = CgFxEffectWriteLock (eff)->getTechniqueByName (techName);
					if (tech)
					{
					  CgFxEffectWriteLock (eff)->setCurrentTechnique (tech);
					  return true;
					}
				}
			}
			return false;
}
bool SceneRendererPipeline::SetAmbientOcclusionParams (nvsg::ViewStateSharedPtr vs)
{
	bool res = true;
	if (m_rendererPostprocAO)
	{
		CameraSharedPtr cam = ViewStateReadLock (vs)->getCamera ();
		nvmath::Mat44f proj = CameraReadLock(cam)->getProjection (); 
		nvmath::Mat44f w2v = CameraReadLock(cam)->getWorldToViewMatrix ();
		nvmath::Mat44f v2w = CameraReadLock(cam)->getViewToWorldMatrix ();

		//proj =  proj *w2v ;
		nvmath::Vec3f viewDir = CameraReadLock(cam)->getDirection ();
	//	nvmath::Trafo traf ;
	//	traf.setMatrix (v2w);
	//	viewDir = traf * viewDir ;
		viewDir = viewDir * CameraReadLock(cam)->getOrientation();
		//viewDir =  vv * w2v ;
		//viewDir.normalize ();
		float x = viewDir[0];
		float y = viewDir[1];
		float z = viewDir[2];
		nvsg::CgFxSharedPtr cgf = m_rendererPostprocAO->getCgFx ();
		if (cgf)
		{
			CgFxEffectSharedPtr eff = CgFxWriteLock (cgf)->getEffect ();
			if (eff)
			{
				CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("gProjection");
				if (pone)
					CgFxEffectWriteLock (eff)->setTransformValue (pone,proj.getPtr());
				else
					res = false;
			    CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("gViewDirX");
				if (ptwo)
					CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo, &x);
				else
					res = false;

			    CgFxParameter ptwo2 = CgFxEffectWriteLock (eff)->getTweakableByName ("gViewDirY");
				if (ptwo2)
					CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo2, &y);
				else 
					res = false;

			    CgFxParameter ptwo3 = CgFxEffectWriteLock (eff)->getTweakableByName ("gViewDirZ");
				if (ptwo3)
					CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo3, &z);
				else res = false;

			} else res = false;
		} else res = false;
	} else res = false;
	return res;
}

void SceneRendererPipeline::SetPostProcBrightnessEnabled (bool val)
{
	m_postEnabledBrightness = val;
	if (val)
		SetPostProcValue ("gBrightness",	m_postprocBrightness);
	else
		SetPostProcValue ("gBrightness",	1.0);
}
void SceneRendererPipeline::SetPostProcContrastEnabled (bool val)
{
	m_postEnabledContrast = val;
	if (val)
		SetPostProcValue ("gContrast",	m_postprocContrast);
	else
		SetPostProcValue ("gContrast",	1.0);
}
void SceneRendererPipeline::SetPostProcHueEnabled (bool val)
{
	m_postEnabledHue = val;
	if (val)
		SetPostProcValue ("gHue",	m_postprocHue);
	else
		SetPostProcValue ("gHue",	0.0);
}
void SceneRendererPipeline::SetPostProcSaturationEnabled (bool val)
{
	m_postEnabledSaturation = val;
	if (val)
		SetPostProcValue ("gSaturation",	m_postprocSaturation);
	else
		SetPostProcValue ("gSaturation",	1.0);
}
void SceneRendererPipeline::SetPostProcNoiseEnabled (bool val)
{
	m_postEnabledNoise = val;
	if (val)
		SetPostProcValue ("gNoiseLevel",	m_postprocNoise);
	else
		SetPostProcValue ("gNoiseLevel",	0.0);
}
void SceneRendererPipeline::SetPostProcSoftenEnabled (bool val)
{
	m_postEnabledSoften = val;
	if (val)
		SetPostProcValue ("gSoftLevel",	m_postprocSoftness);
	else
		SetPostProcValue ("gSoftLevel",	0.0);
}

void SceneRendererPipeline::SetPostProcGAMMAEnabled (bool val)
{
	m_postEnabledGamma = val;
	if (val)
		SetPostProcValue ("gGamma",	m_postprocGamma);
	else
		SetPostProcValue ("gGamma",	1.0);
}

void SceneRendererPipeline::SetPostProcTONEMAPEnabled (bool val)
{
	m_postEnabledToneMap = val;
	if (val)
		SetPostProcValue ("gToneMap",	m_postprocToneMap);
	else
		SetPostProcValue ("gToneMap",	0.0);
}
void SceneRendererPipeline::SetPostProcAOEnabled (bool val)
{
	m_postEnabledAO = val;
	
	if (val)
		SetPostProcValue ("gAOStrength",	m_postprocAOIntensity);
	/*else
		SetPostProcValue ("gSoftLevel",	0.0);
		*/
}

void SceneRendererPipeline::SetPostProcDOFEnabled (bool val)
{
	m_postEnabledDOF = val;
		if (m_postEnabledDOF)
     	SetPostProcValue ("gDOFPlane",	m_postprocDOFPlane);
		else
				SetPostProcValue ("gDOFPlane",	-1.0f);
}
void SceneRendererPipeline::SetPostProcEnabled (bool val)
{
	m_postProcEnabled = val;

}

void SceneRendererPipeline::SetPostProcAOIntensity (float val)
{
	m_postprocAOIntensity = val*5.0f;
	if (m_postEnabledAO)
		SetPostProcValue ("gAOStrength",	m_postprocAOIntensity);
}

void SceneRendererPipeline::SetPostProcBrightness (float val)
{
	m_postprocBrightness = val * 5.0f;
	if (m_postEnabledBrightness)
		SetPostProcValue ("gBrightness",	m_postprocBrightness);
}

void SceneRendererPipeline::SetPostProcDOFPlane (float val)
{
	m_postprocDOFPlane = val;
	if (m_postEnabledDOF)
     	SetPostProcValue ("gDOFPlane",	m_postprocDOFPlane);
}

void SceneRendererPipeline::SetPostProcFXAA (float val)
{
	m_postprocFXAA = val;
	SetPostProcValue ("gTexScale",	m_postprocFXAA);
}
void SceneRendererPipeline::SetPostProcGAMMA (float val)
{
	m_postprocGamma = val;
	SetPostProcValue ("gGamma",	m_postprocGamma);
}
void SceneRendererPipeline::SetPostProcTONEMAP (float val)
{
	m_postprocToneMap = val;
	SetPostProcValue ("gToneMap",	m_postprocToneMap);
}

void SceneRendererPipeline::SetPostProcContrast (float val)
{
	m_postprocContrast = val * 10.0f;
	m_postprocContrast -= 5.0f;

	if (m_postEnabledContrast)
		SetPostProcValue ("gContrast",	m_postprocContrast);
}
void SceneRendererPipeline::SetPostProcHue (float val)
{
	m_postprocHue = val * 360.0f;

	if (m_postEnabledHue)
		SetPostProcValue ("gHue",	m_postprocHue);
}
void SceneRendererPipeline::SetPostProcNoise (float val)
{
	m_postprocNoise = val;
	if (m_postEnabledNoise)
		SetPostProcValue ("gNoiseLevel",	m_postprocNoise);
}
void SceneRendererPipeline::SetPostProcSoften (float val)
{
	m_postprocSoftness = val;
	if (m_postEnabledSoften)
		SetPostProcValue ("gSoftLevel",	m_postprocSoftness);
}
void SceneRendererPipeline::SetPostProcSaturation (float val)
{
	m_postprocSaturation = (val * 10.f) - 5.0f;
	if (m_postEnabledSaturation)
		SetPostProcValue ("gSaturation",	m_postprocSaturation);
}
// This is called from initializeGL().
bool SceneRendererPipeline::init(const SmartRenderContextGL &renderContext,
                                 const SmartRenderTargetGL  &renderTarget)
{
	 m_sceneRendererFront = SceneRendererGL2::create();
	/*
	char a[2] = {0,255};
	char b[2] = {0,255};
	char c[2] = {0,255};
	TextureGL1DSharedPtr tex1 = TextureGL1D::create (GL_RGBA8,2,0,GL_RGBA,GL_UNSIGNED_BYTE);
	nvgl::TextureGL1DWriteLock(tex1)->setData ((void*)a,GL_RGBA,GL_UNSIGNED_BYTE);
	nvgl::TextureGL1DWriteLock(tex1)->bind();
	TextureGL1DSharedPtr tex2 = TextureGL1D::create (GL_RGBA8,2,0,GL_RGBA,GL_UNSIGNED_BYTE);
	nvgl::TextureGL1DWriteLock(tex2)->setData ((void*)b,GL_RGBA,GL_UNSIGNED_BYTE);
	nvgl::TextureGL1DWriteLock(tex2)->bind();
	TextureGL1DSharedPtr tex3 = TextureGL1D::create (GL_RGBA8,2,0,GL_RGBA,GL_UNSIGNED_BYTE);
    nvgl::TextureGL1DWriteLock(tex3)->setData ((void*)c,GL_RGBA,GL_UNSIGNED_BYTE);
	nvgl::TextureGL1DWriteLock(tex3)->bind();*/

  // Create an FBO with rectangle texture color attachment and depth stencil render buffers.
  // This one remains monoscopic, SceneRendererPipeline::doRender() works per eye.
  m_highlightFBO = RenderTargetGLFBO::create(renderContext);

  // Set the defaults for the render pass.
 // m_highlightFBO->setClearColor(0.95f,0.95f,0.98f,0.0f);
  m_highlightFBO->setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  m_highlightFBO->setClearDepth(1.0);
  m_highlightFBO->setClearStencil(0);

#ifdef  _DOING_SHADER_HLR
  m_sketchFBO = RenderTargetGLFBO::create(renderContext);
  m_sketchFBO->setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  m_sketchFBO->setClearDepth(1.0);
  m_sketchFBO->setClearStencil(0);
#endif

#ifdef _DOING_POST_PROCESS
  m_postprocFBO = RenderTargetGLFBO::create(renderContext);
  m_postprocFBO->setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  m_postprocFBO->setClearDepth(1.0);
  m_postprocFBO->setClearStencil(0);
#endif
  
#ifdef _DOING_POST_AO
  m_postprocAOMAP = RenderTargetGLFBO::create(renderContext);
  m_postprocAOMAP->setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
  m_postprocAOMAP->setClearDepth(1.0);
  m_postprocAOMAP->setClearStencil(0);
#endif

  //bool bo;
  //m_efg = CgFx::createFromLump(g_cgfxHighlight, std::vector<std::string>(), std::string(""), bo);
   //CgFxEffectWriteLock effect( CgFxReadLock( m_efg )->getEffect() );
 // nvgl::GLCgGLBindEffect(effect);//CgFxEffectWriteLock(effect)->);
 // CgFxEffectWriteLock(effect)->
  // Make the OpenGL context on the renderContext current. It's needed for the create() operations.
  RenderContextGLStack rcglstack;
  rcglstack.push(renderContext);
  
  // Render to rectangle texture.
  m_highlightFBO->setAttachment(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0,
                                nvgl::TextureGLRectangle::create(GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE));

  // Depth and Stencil are Renderbuffers.
  SmartRenderbufferGL depthStencil(nvgl::RenderbufferGL::create(GL_DEPTH24_STENCIL8));
  m_highlightFBO->setAttachment(nvgl::RenderTargetGLFBO::DEPTH_ATTACHMENT,   depthStencil);
  m_highlightFBO->setAttachment(nvgl::RenderTargetGLFBO::STENCIL_ATTACHMENT, depthStencil);

  std::vector<GLenum> drawBuffers;
  drawBuffers.push_back(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0);
  m_highlightFBO->setDrawBuffers(drawBuffers);

#ifdef  _DOING_SHADER_HLR 
  // Render to rectangle texture.
  m_sketchFBO->setAttachment(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0,
                                nvgl::TextureGLRectangle::create(GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE));
  m_sketchFBO->setAttachment(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT1,
                                nvgl::TextureGLRectangle::create(GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE));
  // Depth and Stencil are Renderbuffers.

  SmartRenderbufferGL depthStencil2(nvgl::RenderbufferGL::create(GL_DEPTH24_STENCIL8));
  m_sketchFBO->setAttachment(nvgl::RenderTargetGLFBO::DEPTH_ATTACHMENT,   depthStencil2);
 //  m_sketchFBO->setAttachment(nvgl::RenderTargetGLFBO::DEPTH_STENCIL_ATTACHMENT,   
	//   nvgl::TextureGLRectangle::create(GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE));
  m_sketchFBO->setAttachment(nvgl::RenderTargetGLFBO::STENCIL_ATTACHMENT, depthStencil2);

  std::vector<GLenum> drawBuffers2;
  drawBuffers2.push_back(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0);
   drawBuffers2.push_back(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT1);
  m_sketchFBO->setDrawBuffers(drawBuffers2);
#endif
#ifdef  _DOING_POST_PROCESS
  // Render to rectangle texture.
  
  m_postprocFBO->setAttachment(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0,
//	  nvgl::TextureGL2DMultisample::create (GL_RGBA16,8));
                                nvgl::TextureGLRectangle::create(GL_RGBA16, GL_BGRA, GL_UNSIGNED_BYTE));
  // Depth and Stencil are Renderbuffers.
  SmartRenderbufferGL depthStencil3(nvgl::RenderbufferGL::create(GL_DEPTH24_STENCIL8));
  m_postprocFBO->setAttachment(nvgl::RenderTargetGLFBO::DEPTH_ATTACHMENT,   depthStencil3);
  m_postprocFBO->setAttachment(nvgl::RenderTargetGLFBO::STENCIL_ATTACHMENT, depthStencil3);

  std::vector<GLenum> drawBuffers3;
  drawBuffers3.push_back(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0);
  m_postprocFBO->setDrawBuffers(drawBuffers3);
#endif
#ifdef  _DOING_POST_AO
  // Render to rectangle texture.
  
  m_postprocAOMAP->setAttachment(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0,
//	  nvgl::TextureGL2DMultisample::create (GL_RGBA16,8));
                                nvgl::TextureGLRectangle::create(GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE));
  // Depth and Stencil are Renderbuffers.
  SmartRenderbufferGL depthStencil4(nvgl::RenderbufferGL::create(GL_DEPTH24_STENCIL8));
  m_postprocAOMAP->setAttachment(nvgl::RenderTargetGLFBO::DEPTH_ATTACHMENT,   depthStencil4);
  m_postprocAOMAP->setAttachment(nvgl::RenderTargetGLFBO::STENCIL_ATTACHMENT, depthStencil4);

  std::vector<GLenum> drawBuffers4;
  drawBuffers4.push_back(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0);
  m_postprocAOMAP->setDrawBuffers(drawBuffers4);
#endif
  rcglstack.pop();//(renderContext);

  // If there hasn't been a setSceneRender() we cannot reuse the m_sceneRenderer for the highlight rasterization.
  // Create the SceneRenderer used to render the highlighted objects of the scene into the highlightFBO stencil buffer.
  if (!m_sceneRendererHighlight)
  {
    m_sceneRendererHighlight = SceneRendererGL2::create();
  }

#ifdef _DOING_POST_PROCESS
    if (!m_sceneRendererPostProc)
	{
		m_sceneRendererPostProc = SceneRendererGL2::create();
	}
	{
		m_rendererPostproc = RendererGLFSQ::create(renderTarget);
		std::string err2;
		std::vector<std::string> sp;
		bool b = false;
		sp.push_back(nvutil::GetProccessDirectory () + "effects\\");
#ifdef _DOING_POST_AO
	   m_rendererPostprocAO = RendererGLFSQ::create(renderTarget);
	   CgFxSharedPtr hcg_ppAO = CgFx::createFromFile(std::string ("AvocadoPostProcAO.cgfx"),sp,err2,b);
	   m_rendererPostprocAO->setCgFx (hcg_ppAO);
	   b = false;
#endif
		CgFxSharedPtr hcg_pp = CgFx::createFromFile(std::string ("AvocadoPostProc.cgfx"),sp,err2,b);

		m_rendererPostproc->setCgFx (hcg_pp);
		
		//m_imgProcEffect = hcg;
		// m_rendererHighlight uses the previously rendered texture rectangle as input for the CgFX shader.
		const RenderTargetGLFBO::SmartAttachment &attachment11_pp = m_postprocFBO->getAttachment(RenderTargetGLFBO::COLOR_ATTACHMENT0);
		const RenderTargetGLFBO::SmartAttachmentTexture &texAtt11_pp = smart_cast<RenderTargetGLFBO::AttachmentTexture>(attachment11_pp);
		if (texAtt11_pp)
		{
			const TextureGLSharedPtr &texGL11_pp = texAtt11_pp->getTextureGL();
			m_rendererPostproc->setSamplerTextureByName("gNormSampler", texGL11_pp);
#ifdef _DOING_POST_AO
			m_rendererPostprocAO->setSamplerTextureByName("gNormSampler", texGL11_pp);
#endif
		}

#ifdef _DOING_POST_AO
		const RenderTargetGLFBO::SmartAttachment &attachment12_pp = m_sketchFBO->getAttachment(RenderTargetGLFBO::COLOR_ATTACHMENT0);
		const RenderTargetGLFBO::SmartAttachmentTexture &texAtt12_pp = smart_cast<RenderTargetGLFBO::AttachmentTexture>(attachment12_pp);
		if (texAtt12_pp)
		{
			const TextureGLSharedPtr &texGL12_pp = texAtt12_pp->getTextureGL();
			m_rendererPostprocAO->setSamplerTextureByName("gNormalSampler", texGL12_pp);
		}
		const RenderTargetGLFBO::SmartAttachment &attachment13_pp = m_sketchFBO->getAttachment(RenderTargetGLFBO::COLOR_ATTACHMENT1);
		const RenderTargetGLFBO::SmartAttachmentTexture &texAtt13_pp = smart_cast<RenderTargetGLFBO::AttachmentTexture>(attachment13_pp);
		if (texAtt13_pp)
		{
			const TextureGLSharedPtr &texGL13_pp = texAtt13_pp->getTextureGL();
			m_rendererPostprocAO->setSamplerTextureByName("gDepthSampler", texGL13_pp);
		}

		const RenderTargetGLFBO::SmartAttachment &attachment14_pp = m_postprocAOMAP->getAttachment(RenderTargetGLFBO::COLOR_ATTACHMENT0);
		const RenderTargetGLFBO::SmartAttachmentTexture &texAtt14_pp = smart_cast<RenderTargetGLFBO::AttachmentTexture>(attachment14_pp);
		if (texAtt14_pp)
		{
			const TextureGLSharedPtr &texGL14_pp = texAtt14_pp->getTextureGL();
			m_rendererPostprocAO->setSamplerTextureByName("gOccMapSampler", texGL14_pp);
		}
#endif
	}
#endif
#ifdef  _DOING_SHADER_HLR
  if (!m_sceneRendererSketch)
  {
      m_sceneRendererSketch = SceneRendererGL2::create();
  }
	m_rendererSketch = RendererGLFSQ::create(renderTarget);
	std::string err2;
	std::vector<std::string> sp;
	bool b;
	sp.push_back(nvutil::GetProccessDirectory () + "effects\\");
	CgFxSharedPtr hcg = CgFx::createFromFile(std::string ("phong_gloss_cellImgProc.cgfx"),sp,err2,b);
	m_rendererSketch->setCgFx (hcg);
	//m_imgProcEffect = hcg;
	// m_rendererHighlight uses the previously rendered texture rectangle as input for the CgFX shader.
  const RenderTargetGLFBO::SmartAttachment &attachment11 = m_sketchFBO->getAttachment(RenderTargetGLFBO::COLOR_ATTACHMENT0);
  const RenderTargetGLFBO::SmartAttachmentTexture &texAtt11 = smart_cast<RenderTargetGLFBO::AttachmentTexture>(attachment11);
  if (texAtt11)
  {
    const TextureGLSharedPtr &texGL11 = texAtt11->getTextureGL();
    m_rendererSketch->setSamplerTextureByName("gNormSampler", texGL11);
  }
   const RenderTargetGLFBO::SmartAttachment &attachment22 = m_sketchFBO->getAttachment(RenderTargetGLFBO::COLOR_ATTACHMENT1);
  const RenderTargetGLFBO::SmartAttachmentTexture &texAtt22 = smart_cast<RenderTargetGLFBO::AttachmentTexture>(attachment22);
  if (texAtt22)
  {
    const TextureGLSharedPtr &texGL22 = texAtt22->getTextureGL();
    m_rendererSketch->setSamplerTextureByName("gDeepSampler", texGL22);
  }
#endif
  // Create a full screen quad renderer for the stencil buffer to color attachment migration.
  m_rendererStencilToColor = RendererGLFSQ::create(m_highlightFBO);

  std::string err;
  bool failOnTextureLoad = false;

  CgFxSharedPtr cgfxStencilToColor = CgFx::createFromLump(g_cgfxStencilToColor, std::vector<std::string>(), err, failOnTextureLoad);
  if (!err.empty()) 
  {
    NVSG_ASSERT(!"Creation of StencilToColor CgFX failed.");
    return false;
  }
  NVSG_ASSERT(cgfxStencilToColor);
  m_rendererStencilToColor->setCgFx(cgfxStencilToColor);

  // Create a full screen quad renderer for the final texture to framebuffer operation rendering the hightlight outline.
  m_rendererHighlight = RendererGLFSQ::create(renderTarget);

  CgFxSharedPtr cgfxHighlight = CgFx::createFromLump(g_cgfxHighlight, std::vector<std::string>(), err, failOnTextureLoad);
  if (!err.empty()) 
  {
    NVSG_ASSERT(!"Creation of Highlight CgFX failed.");
    return false;
  }
  NVSG_ASSERT(cgfxHighlight);
  m_rendererHighlight->setCgFx(cgfxHighlight);

  // m_rendererHighlight uses the previously rendered texture rectangle as input for the CgFX shader.
  const RenderTargetGLFBO::SmartAttachment &attachment = m_highlightFBO->getAttachment(RenderTargetGLFBO::COLOR_ATTACHMENT0);
  const RenderTargetGLFBO::SmartAttachmentTexture &texAtt = smart_cast<RenderTargetGLFBO::AttachmentTexture>(attachment);
  if (texAtt)
  {
    const TextureGLSharedPtr &texGL = texAtt->getTextureGL();
    m_rendererHighlight->setSamplerTextureByName("selection", texGL);
  }

  return true;
}

void SceneRendererPipeline::replaceStateSet(const GeoNodeSharedPtr geoNode/*, const StateSetSharedPtr & newSsh*/)
{
		GeoNodeWriteLock geoNodeLock(geoNode);

		GeoNode::StateSetConstIterator ssiEnd = geoNodeLock->endStateSets();
		for ( GeoNode::StateSetConstIterator ssi = /*GeoNodeReadLock*/geoNodeLock->beginStateSets() ; ssi != ssiEnd ; ++ssi )
		{
			//std::string err;
			 bool failOnLoad = false;
			std::string error;
			//CgFxWriteLock efg_lock(m_efg);
			/*efg_lock->*/m_efg = CgFx::createFromLump(/*g_cgfxHighlight*/g_cgfxSimple, std::vector<std::string>(), error, failOnLoad);
			//CgFxEffectWriteLock effectNew(efg_lock->getEffect());
		    StateSetWriteLock ssil (*ssi);
			StatePass::AttributeIterator saciOne = ssil->findAttribute( OC_CGFX );
			ssil->removeAttribute(saciOne);
			ssil->addAttribute(m_efg);

			//CgFxEffectWriteLock effectOld( CgFxWriteLock( nvutil::sharedPtr_cast<CgFx>( *saciOne ) )->getEffect() );
			//effectOld = effectNew;
			//CgFxEffect *efg;
			//m_efg =  CgFxEffect::createFromLump(g_cgfxSimple, std::vector<std::string>(), err);
			//CgFxEffectReadLock effect( CgFxReadLock( CgFxSharedPtr(m_efg) )->getEffect() );
		}
}

void SceneRendererPipeline::replaceStateSet2(const GeoNodeSharedPtr geoNode, bool shaded/*, const StateSetSharedPtr & newSsh*/)
{
		GeoNodeWriteLock geoNodeLock(geoNode);
		
		if (geoNodeLock->getName () == std::string ("AvocadoRoom"))
		{
			if (!shaded)
			{
			//	geoNodeLock->setTraversalMask (0);
				m_cachedRoomTemp = geoNode;
			}
			//else
			//	geoNodeLock->setTraversalMask (1);
			return;
		}
		GeoNode::StateSetConstIterator ssiEnd = geoNodeLock->endStateSets();
		for ( GeoNode::StateSetConstIterator ssi = /*GeoNodeReadLock*/geoNodeLock->beginStateSets() ; ssi != ssiEnd ; ++ssi )
		{
			 StateSetWriteLock ssil (*ssi);
			if (!shaded)
			{
				StateSet::VariantIterator vit = ssil->findVariant (33);
				if (vit == ssil->endVariants())
				{
					//std::string err;
						bool failOnLoad = false;
					std::string error;
						std::string err2;
					std::vector<std::string> sp;
					bool b;
					sp.push_back(nvutil::GetProccessDirectory () + "effects\\");
					if (!m_efg)
					{
						CgFxSharedPtr hcg = CgFx::createFromFile(std::string ("phong_gloss_cell.cgfx"),sp,err2,b);
						m_efg = hcg;
					}
					StateVariantSharedPtr sva = StateVariant::create();
					StateVariantWriteLock(sva)->addAttribute (m_efg);

					StateVariantSharedPtr ova = StateVariant::create();
					StatePass::AttributeIterator ait = ssil->beginAttributes();
					while (ait != ssil->endAttributes ())
					{
						StateVariantWriteLock(ova)->addAttribute ((*ait));
						++ait;
					}
					
					ssil->setVariant (33,sva);
					ssil->setVariant (0,ova);
				}
				ssil->setActiveVariantKey (33);
			}
			else
			{
				//ssil->removeVariant (33);
				ssil->setActiveVariantKey (0);
			}
			//StatePass::AttributeIterator saciOne = ssil->findAttribute( OC_CGFX );
			//ssil->removeAttribute(saciOne);
			//ssil->addAttribute(m_efg);
			//StatePass
			//CgFxEffectWriteLock effectOld( CgFxWriteLock( nvutil::sharedPtr_cast<CgFx>( *saciOne ) )->getEffect() );
			//effectOld = effectNew;
			//CgFxEffect *efg;
			//m_efg =  CgFxEffect::createFromLump(g_cgfxSimple, std::vector<std::string>(), err);
			//CgFxEffectReadLock effect( CgFxReadLock( CgFxSharedPtr(m_efg) )->getEffect() );
		}
}

void SceneRendererPipeline::doShader( const ViewStateSharedPtr &viewState)
{
	//static bool m_shaderstart = true;
	if (m_shaderstart)
	{
		m_shaderstart = false;

		if(!viewState)
			return;
		m_scene = ViewStateReadLock(viewState)->getScene();
		//SceneReadLock scene(ViewStateReadLock(viewState)->getScene());
		NodeSharedPtr root = /*scene*/SceneReadLock(m_scene)->getRootNode();

		// select everything from here on down
		SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
		NVSG_ASSERT(st);
		st->setClassName("class nvsg::GeoNode");
		st->apply( root );

		const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
		std::vector<ObjectWeakPtr>::const_iterator it;
		for(it=searchResults.begin(); it!=searchResults.end(); it++)
		{
			//GeoNodeWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
			GeoNodeWriteLock node(dynamic_cast<GeoNodeWeakPtr>(*it));
			GeoNodeSharedPtr geonode2(dynamic_cast<GeoNodeWeakPtr>(*it));
			//GeoNodeSharedPtr geonode = nvutil::sharedPtr_cast<GeoNode*>(*it);
			replaceStateSet( geonode2/*new CommandReplaceStateSet( node, 0, newSsh )*/ );
		}
	}
}
void SceneRendererPipeline::doShader2( const ViewStateSharedPtr &viewState, bool shaded)
{
	//static bool m_shaderstart = true;
	if (m_shaderstart || shaded == true)
	{
		m_shaderstart = false;

		if(!viewState)
			return;
		m_scene = ViewStateReadLock(viewState)->getScene();
		//SceneReadLock scene(ViewStateReadLock(viewState)->getScene());
		NodeSharedPtr root = /*scene*/SceneReadLock(m_scene)->getRootNode();

		// select everything from here on down
		SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
		NVSG_ASSERT(st);
		st->setClassName("class nvsg::GeoNode");
		st->setBaseClassSearch(true);
		st->apply( root );
		if (!shaded)
			m_cachedRoomTemp = 0;
		const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
		std::vector<ObjectWeakPtr>::const_iterator it;
		for(it=searchResults.begin(); it!=searchResults.end(); it++)
		{
			//GeoNodeWeakPtr node = static_cast<GeoNodeWeakPtr>(*it);
		//	GeoNodeWriteLock node(dynamic_cast<GeoNodeWeakPtr>(*it));
			GeoNodeSharedPtr geonode2(dynamic_cast<GeoNodeWeakPtr>(*it));

			//GeoNodeSharedPtr geonode = nvutil::sharedPtr_cast<GeoNode*>(*it);
			replaceStateSet2( geonode2,shaded/*new CommandReplaceStateSet( node, 0, newSsh )*/ );
		}
		if (m_cachedRoomTemp)
		{
			if (!shaded)
				GeoNodeWriteLock (m_cachedRoomTemp)->setTraversalMask (0);
			else
			{
				GeoNodeWriteLock (m_cachedRoomTemp)->setTraversalMask (~0);
				m_cachedRoomTemp = 0;
			}
		}
		// Set far near planes for depth/normal pass..
		if (!shaded)
		{
			PerspectiveCameraSharedPtr camera( sharedPtr_cast<PerspectiveCamera>(ViewStateReadLock (viewState)->getCamera()) );
            PerspectiveCameraReadLock theCamera(camera);

			//CameraSharedPtr camp  = ViewStateReadLock(viewState)->getCamera();
			float farPlane = theCamera->getFarDistance ();
			float nearPlane = theCamera->getNearDistance ();
			//nearPlane = 0.0f;
			//farPlane = 100.0f;//SceneReadLock (ViewStateReadLock (viewState)->getScene())->getBoundingSphere().getRadius();
			if (m_efg)
			{
				CgFxEffectSharedPtr eff = CgFxWriteLock(m_efg)->getEffect ();
				if (eff)
				{
					CgFxParameter p_far = CgFxEffectWriteLock (eff)->getParameterByName ("gFar");
					CgFxParameter p_near = CgFxEffectWriteLock (eff)->getParameterByName ("gNear");
					if (p_far && p_near)
					{
						CgFxEffectWriteLock (eff)->setFloatParameterValue (p_far,&farPlane);
						CgFxEffectWriteLock (eff)->setFloatParameterValue (p_near,&nearPlane);
					}
				}
			}
		}
	}
}
void SceneRendererPipeline::doHLR (const ViewStateSharedPtr &viewState, const nvgl::SmartRenderTargetGLFB &renderTargetGL )
{

  renderTargetGL->setClearMask (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	  // m_sceneRendererGL2->getOptionStringPolygonMode()
  SmartSceneRendererGL2 sr2 = nvutil::smart_cast<SceneRendererGL2>(m_sceneRenderer);
  
  nvui::RendererOptionsWriteLock options( ViewStateReadLock(viewState)->getRendererOptions() );
  if ( options->hasProperty( (sr2->getOptionStringPolygonMode() ) ) )
  {

  options->setValue<int>( options->getProperty( sr2->getOptionStringPolygonMode() ), 2 );
   options->setValue<int>( options->getProperty( sr2->getOptionStringRenderTechnique()) , 0 );
 //  m_sceneRendererGL2->render();
//   renderTargetGL
//  nvui::s m_sceneRenderer->

    m_sceneRenderer->render(viewState, renderTargetGL, renderTargetGL->getStereoTarget());
   renderTargetGL->setClearMask (GL_COLOR_BUFFER_BIT);//GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
   
//  NVSG_ASSERT( options->hasProperty( m_sceneRendererGL2->getOptionStringPolygonMode() ) );
  options->setValue<int>( options->getProperty(  sr2->getOptionStringPolygonMode()), 1 );
  options->setValue<int>( options->getProperty(  sr2->getOptionStringRenderTechnique()), 0 );

   m_sceneRenderer->render(viewState, renderTargetGL , renderTargetGL->getStereoTarget());
    renderTargetGL->setClearMask (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
   options->setValue<int>( options->getProperty( sr2->getOptionStringPolygonMode() ), 2 );
   options->setValue<int>( options->getProperty(  sr2->getOptionStringRenderTechnique()) , 3 );
  }


}
void SceneRendererPipeline::SetSceneBackColor(int r,int g,int b)
{
	float f1 = float(r)/255.0f;
	float f2 = float (g)/255.0f;
	float f3 = float (b) / 255.0f;
	m_backColor = nvmath::Vec4f(f1,f2,f3,0.0f);
	
}

// Mind, this is called for left and right eye independently.
void SceneRendererPipeline::doRender(const ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget)
{
	if (isSmartPtrOf<SceneRendererGL2>(m_sceneRenderer))
	{
		 SmartSceneRendererGL2 sr3 = nvutil::smart_cast<SceneRendererGL2>(m_sceneRenderer);
 
	  //nvui::RendererOptionsWriteLock options( ViewStateReadLock(viewState)->getRendererOptions() );
	
	// if ( options->hasProperty( (sr3->getOptionStringFaceCullMode() ) ) )
	  {
		//	options->setValue<int>( options->getProperty(  sr3->getOptionStringFaceCullMode()), GLObjectRenderer::CULLMODE_BACK );
	  }
	  /* if ( options->hasProperty( (sr3->getOptionStringTransparencyDepthMask() ) ) )
	  {
			options->setValue<int>( options->getProperty(  sr3->getOptionStringTransparencyDepthMask()), GLObjectRenderer::CULLMODE_BACK );
	  }*/
  }
	int mask = int(pow(2.0,2*m_viewId+3));
  // Scene pass:
  // Call the current scene renderer and render the whole scene into the main render target (framebuffer).
  ViewStateWriteLock(viewState)->setTraversalMask(~0 & ~4); // Render the whole scene.
  SmartRenderTargetGLFB rt (dynamic_cast<RenderTargetGLFB*>(renderTarget.get()));

  // This renders only one eye even if the renderTarget is stereoscopic.
  // Important: The m_sceneRenderer holds a NOP StereoViewStateProvider to use the already adjusted stereo ViewState camera!
  if (m_CellShading)//m_HLR)
  {
	// doShader(viewState);
	    SmartRenderTargetGLFB sr2 = nvutil::smart_cast<RenderTargetGLFB>(renderTarget);
	  if (m_flatShading)
	  {
		  doShader2(viewState,true);
			resetShader();

		 
		// regular shaded pass
		   SceneWriteLock(ViewStateReadLock(viewState)->getScene()/*m_scene*/)->setBackColor (m_backColor);//nvmath::Vec4f (0.97f,0.97f,0.985f,0.0f));
		   sr2->setClearMask (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		   m_sceneRenderer->render(viewState, renderTarget , renderTarget->getStereoTarget());
			sr2->setClearMask (GL_DEPTH_BUFFER_BIT);
			// end sshaded passs
	  }
	   SceneWriteLock(ViewStateReadLock(viewState)->getScene()/*m_scene*/)->setBackColor (m_backColor);//'//nvmath::Vec4f (0.97f,0.97f,0.985f,0.0f));
	   
	  doShader2(viewState);

	  	 unsigned int width;
		unsigned int height;
		renderTarget->getSize(width, height);
		m_sketchFBO->setSize(width, height);
		 m_sketchFBO->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		 if (!m_flatShading)
		 {
			  sr2->setClearMask (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
			 sr2->setClearColor (m_backColor[0],m_backColor[1],m_backColor[2],1.0); 
			 sr2->getRenderContextGL()->makeCurrent();
             GLClear(GL_COLOR_BUFFER_BIT); // The CgFX shaders in the following two steps care for the state themselves.
	         sr2->getRenderContextGL()->makeNoncurrent();//makeCurrent();
	        
		 }
	   m_sceneRendererSketch->render(viewState, m_sketchFBO);
	    m_rendererSketch->render();

		
  }
  else if (m_HLR)
  {

	   SceneWriteLock(ViewStateReadLock(viewState)->getScene()/*m_scene*/)->setBackColor (m_backColor);//'//nvmath::Vec4f (0.97f,0.97f,0.985f,0.0f));
	   
	
	doHLR (viewState,rt);
  }
  else
  {
	
       SmartRenderTargetGLFB sr2 = nvutil::smart_cast<RenderTargetGLFB>(renderTarget);
	  
	 //SmartRenderTargetGLFB sr2 (dynamic_cast<RenderTargetGLFB*>(renderTarget.get()));	// Rotem

	  // if (m_backimage==0)
	   SceneWriteLock(ViewStateReadLock(viewState)->getScene()/*m_scene*/)->setBackColor (m_backColor);//nvmath::Vec4f (0.97f,0.97f,0.985f,0.0f));
	 
	   // Render background here...
	//  ->doRender(viewState, renderTarget);
	   
	   sr2->setClearMask (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	    
	 //  SceneWriteLock(ViewStateReadLock(viewState)->getScene()/*m_scene*/)->setBackImage (m_backimage);
		 
	   //if ViewStateReadLock (m_sceneRenderer->getViewState ())->
try
{
#ifdef _DOING_POST_PROCESS
	bool doPostProc = false;
	
	if (m_postProcEnabled && (m_postEnabledBrightness || 
		m_postEnabledContrast ||
		m_postEnabledHue || 
		m_postEnabledSaturation || 
		m_postEnabledNoise || 
		m_postEnabledGamma ||
		m_postEnabledToneMap ||
		m_postEnabledDOF || 
		m_postEnabledAO || 
		m_postEnabledSoften))
			doPostProc = true;

	if (m_useOptix || doPostProc == false )
	{
		  m_sceneRenderer->render(viewState, renderTarget , renderTarget->getStereoTarget());
	}
	else
	{
	   unsigned int width;
	   unsigned int height;
	
	   renderTarget->getSize(width, height);
	
#ifdef _DOING_POST_AO

	   // Creating normal and depth map textures.
	   if (m_postEnabledAO || m_postEnabledDOF)
	   {
	   		doShader2(viewState);
			// TODO Scale down the normal map and depth map for performance..
			
			m_sketchFBO->setSize((width * int(m_postprocFXAA)) / 2 , (height * int(m_postprocFXAA))/2);
			m_sketchFBO->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			m_sceneRendererSketch->render (viewState,m_sketchFBO);
			doShader2 (viewState,true);
			resetShader ();
	   }
#endif
		// TODO : 3 additional controls for quality/tonemap on-off/gamma on-off. AO on-off. crp.

	   // We must verify that tex scale uniform is the same. no room for error here.
	   SetPostProcFXAA (m_postprocFXAA);

	   m_postprocFBO->setSize(width * int(m_postprocFXAA), height * int(m_postprocFXAA) );
	   m_postprocFBO->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	   m_sceneRendererPostProc->render(viewState,m_postprocFBO);// renderTarget , renderTarget->getStereoTarget());
	   m_postprocFBO->setClearMask(GL_COLOR_BUFFER_BIT);

#ifdef _DOING_POST_AO
	   if (m_postEnabledAO || m_postEnabledDOF)
	   {
		    SetAmbientOcclusionParams (viewState);
			SetAmbientOcclusionTechnique ("OcclusionMapOnly");
			
     	    m_postprocAOMAP->setSize((width * int(m_postprocFXAA))/2, (height * int(m_postprocFXAA))/2 );
			// whie bg for occ map
			m_postprocAOMAP->setClearColor (1.0f,1.0f,1.0f,1.0f);
	        m_postprocAOMAP->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			//m_postprocAOMAP->getRenderContextGL()->makeCurrent();
            GLClear(GL_COLOR_BUFFER_BIT); 
			// Generate the occlusion map.
			m_rendererPostprocAO->render (m_postprocAOMAP);
			
			// Blur the occlusion map.
			m_sketchFBO->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
			SetAmbientOcclusionTechnique ("UseOccMap");
			m_rendererPostprocAO->render (m_sketchFBO);

			// Render post proc using the occulusion map.
			SetAmbientOcclusionTechnique ("Main");
			m_rendererPostprocAO->render (renderTarget);

		
			// Also important to hide the room environemnt.
	   }
		else
#endif
		// no AO, no DOF , no need for depth map.
		   m_rendererPostproc->render (renderTarget);
	}
#else
	   m_sceneRenderer->render(viewState, renderTarget , renderTarget->getStereoTarget());
#endif
}
catch
 (...)
{

}
	  sr2->setClearMask (GL_DEPTH_BUFFER_BIT);

	  //FrustumCameraWriteLock (newcam)->zoom (

	  ViewStateWriteLock(viewState)->setTraversalMask(4); // Render the whole scene.
	  
	//  m_sceneRenderer->setViewState(m_frontViewState);
	 nvmath::Quatf q = CameraReadLock(ViewStateReadLock(viewState)->getCamera())->getOrientation();
	// CameraWriteLock(ViewStateWriteLock(m_frontViewState)->getCamera())->setOrientation(q);
	// TransformWriteLock(SceneReadLock(ViewStateReadLock(viewState)->getScene())->getRootNode())->setOrientation(q);

/*
	 m_sceneRendererFront->render(m_frontViewState, renderTarget , renderTarget->getStereoTarget());
	  ViewStateWriteLock(viewState)->setTraversalMask(~0 & ~4); // Render the whole scene.
	  sr2->setClearMask (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	  */
  }

  if (  m_highlighting && m_hasHighlightedObject && !m_isExporting)
  {
    // Highlight pass:
    // Match the size of the highlightFBO to the destination renderTarget.
    unsigned int width;
    unsigned int height;
    renderTarget->getSize(width, height);
    m_highlightFBO->setSize(width, height);

	SceneWriteLock(ViewStateReadLock(viewState)->getScene()/*m_scene*/)->setBackColor (nvmath::Vec4f (0.0f,0.0f,0.0f,0.0f));
	
    // If an object is highlighted, render the highlighted object into the stencil buffer of the FBO.
	if (m_doingPreHighlight)
    ViewStateWriteLock(viewState)->setTraversalMask(mask/*2*/); // Render the non-highlighted objects.
	else
	{
		 mask = int(pow(2.0,2*m_viewId+4)) ;//| int(pow(2.0,m_viewId+3)) ;
	    ViewStateWriteLock(viewState)->setTraversalMask(mask );
	}
    // Setup the proper stencil state. 
    // Write a 1 for every rendered fragment into the stencil buffer
    // Note: This won't work with StencilAttributes inside the scene, or CgFX shaders which change the stencil state.
	m_highlightFBO->getRenderContextGL()->makeCurrent();
    GLStencilFunc(GL_NEVER, 1, ~0);
    GLStencilOp(GL_REPLACE, GL_KEEP, GL_KEEP);
    GLEnable(GL_STENCIL_TEST);
	m_highlightFBO->getRenderContextGL()->makeNoncurrent();//makeCurrent();
    // This is always using a SceneRendererGL2 to render the highlighted objects.
    m_highlightFBO->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear all.
    m_sceneRendererHighlight->render(viewState, m_highlightFBO);
	m_highlightFBO->getRenderContextGL()->makeCurrent();
    GLDisable(GL_STENCIL_TEST); // The CgFX shaders in the following two steps care for the state themselves.
	m_highlightFBO->getRenderContextGL()->makeNoncurrent();//makeCurrent();
    // Reset the traversal mask. // DAR Could rearrange the rendering order in this function to save one traversal mask reset.
    ViewStateWriteLock(viewState)->setTraversalMask(~0 & ~4); // Render the whole scene.

    // Highlight post-processing:
    // Migrate the stencil bit contents as white color into the texture rectangle.
    m_highlightFBO->setClearMask(GL_COLOR_BUFFER_BIT); // Do not clear the stencil! Don't care for depth.
    m_rendererStencilToColor->render();

    // Render the outline around the highlighted object onto the main renderTarget (framebuffer).
    m_rendererHighlight->render();
  }
  if (!m_HLR && !m_CellShading)
  {
    SmartRenderTargetGLFB sr2 = nvutil::smart_cast<RenderTargetGLFB>(renderTarget);
	//if (m_frontViewState)
	{
			m_sceneRendererFront->render(m_frontViewState, renderTarget , renderTarget->getStereoTarget());
	}
	  ViewStateWriteLock(viewState)->setTraversalMask(~0 & ~4); // Render the whole scene.
	  sr2->setClearMask (GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
  }
}

// This switches the renderer for the main pass between rasterizer and raytracer.
void SceneRendererPipeline::setSceneRenderer(const SmartSceneRenderer &sceneRenderer)
{
  m_sceneRenderer = sceneRenderer;
  // Do not separate the ViewState camera another time during the render() call issued inside the SceneRendererPipeline.
  m_sceneRenderer->setStereoViewStateProvider(m_monoViewStateProvider);

  // If the renderer is a SceneRendererGL2 reuse it for the highlighting to keep the number of RenderLists small.
  if (isSmartPtrOf<SceneRendererGL2>(m_sceneRenderer))
  {
    m_sceneRendererHighlight = m_sceneRenderer;
#ifdef _DOING_SHADER_HLR
	 m_sceneRendererSketch = m_sceneRenderer;
#endif
#ifdef _DOING_POST_PROCESS
	 m_sceneRendererPostProc = m_sceneRenderer;
#endif

  }
  else
  {
    m_sceneRendererHighlight = SceneRendererGL2::create();
#ifdef _DOING_SHADER_HLR
	m_sceneRendererSketch= SceneRendererGL2::create();
#endif
#ifdef _DOING_POST_PROCESS
	m_sceneRendererPostProc = SceneRendererGL2::create ();
#endif
  }
}

SmartSceneRenderer SceneRendererPipeline::getSceneRenderer() const 
{
  return m_sceneRenderer;
}

void SceneRendererPipeline::ToggleHighlighting()
{
  m_highlighting = !m_highlighting;//onOff;
 
}

void SceneRendererPipeline::ToggleCellShading(bool enable)
{
	  if (m_CellShading && !enable)
  {
	  doShader2 (m_viewState,true);
	  resetShader();
  }
  m_CellShading = enable;//!m_CellShading;//onOff;
  m_flatShading = false;

}

void SceneRendererPipeline::ToggleFlatShading(bool enable)
{
	// cell shader with extra flag..
  if (m_CellShading  && !enable)
  {
	  doShader2 (m_viewState,true);
	  resetShader();
  }
  m_flatShading = true;
  m_CellShading = enable;//!m_CellShading;//onOff;
}

void SceneRendererPipeline::ToggleHLR(bool enable)
{
  m_HLR = enable;//!m_HLR;//onOff;
}

bool SceneRendererPipeline::SetBoolUseOptix(bool newval)
{
	if (m_useOptix != newval)
	{
		m_useOptix = newval;
		return true;
	}
	return false;
	//m_useOptix = !m_useOptix;//onOff;
}
	bool SceneRendererPipeline::SaveContextToFile (std::string fileName, unsigned int width,unsigned int height,bool bitmap,bool tryOptix )
	{
		unsigned int actualWidth = 3300;//width;
		unsigned int actualHeight = 2600;//height;
		bool sizeChanged = false;

		if (width > 400 && height > 400)
		{
			actualWidth = width;
			actualHeight = height;
		} 
		else
		{
			actualWidth = 800;
			actualHeight = 800;
		}
		/// 2200 X 1300 works

	
		SmartRenderTargetGLFB sr2 = nvutil::smart_cast<RenderTargetGLFB>(getRenderTarget ());
		sr2->getRenderContextGL()->makeCurrent();

		if (m_exportFBO.get() != 0)
		{
				unsigned int oldW,oldH;
				m_exportFBO->getSize (oldW,oldH);
				if (oldW != actualWidth || oldH != actualHeight)
				{
					sizeChanged = true;
				}
		}

		if (m_exportFBO.get() == 0 || sizeChanged)
		{


		m_exportFBO = nvgl::RenderTargetGLFBO::create(sr2->getRenderContextGL());
		m_exportFBO->setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		m_exportFBO->setClearDepth(1.0);
		m_exportFBO->setClearStencil(0);
		// Render to rectangle texture.
		m_exportFBO->setAttachment(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0,
			nvgl::TextureGLRectangle::create(GL_RGBA8,actualWidth,actualHeight, GL_BGRA, GL_UNSIGNED_BYTE));
		//	nvgl::TextureGL2D::create (GL_RGBA8,2200,1300,0,GL_BGR,GL_UNSIGNED_BYTE));
		// Depth and Stencil are Renderbuffers.
		nvgl::SmartRenderbufferGL depthStencil(nvgl::RenderbufferGL::create(GL_DEPTH24_STENCIL8));
		m_exportFBO->setAttachment(nvgl::RenderTargetGLFBO::DEPTH_ATTACHMENT,   depthStencil);
		m_exportFBO->setAttachment(nvgl::RenderTargetGLFBO::STENCIL_ATTACHMENT, depthStencil);

		std::vector<GLenum> drawBuffers;
		drawBuffers.push_back(nvgl::RenderTargetGLFBO::COLOR_ATTACHMENT0);
		m_exportFBO->setDrawBuffers(drawBuffers);
		m_exportFBO->setSize(actualWidth, actualHeight);
		} 
		//this->m_sceneRendererGL2->setRenderTarget (m_exportFBO);

		unsigned int width22;
		unsigned int height22;
		
		this->getRenderTarget ()->getSize(width22, height22);
		m_exportFBO->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // Clear all.

		//this->m_sceneRendererRT->setRenderTarget (m_exportFBO);
		// this->m_sceneRendererRT->render ();
		m_isExporting = true;
		if (tryOptix)//bitmap  && tryOptix)
			// cases - 
			// printing/ ole doc/bmp export.
		//	this->m_sceneRendererRT->render  (m_viewState,m_exportFBO);
		    //this->m_sceneRendererGL2->render  (m_viewState,m_exportFBO);
			this->render (m_viewState,m_exportFBO);
		else
		 this->render (m_viewState,m_exportFBO);
			//this->m_sceneRendererGL2->render  (m_viewState,m_exportFBO);
	     m_isExporting = false;
		//this->m_sceneRendererGL2->setRenderTarget (m_renderTargetGL);
		//BufferSharedPtr buffer =BufferHost::create(); // = Image::Image ();
		//	m_renderTargetGL->copyToBuffer ( nvgl::RenderTargetGLFB::COLOR_ATTACHMENT0,Image::PixelFormat::IMG_RGB,Image::PixelDataType::IMG_BYTE,buffer);
		TextureHostSharedPtr te = m_exportFBO->getTextureHost ();
		TextureHostWriteLock (te)->convertPixelFormat (Image::IMG_RGB);
		TextureHostWriteLock (te)->scale (0,width,height);

		nvutil::saveTextureHost (fileName,te);
#if 0
		BufferSharedPtr buffer = TextureHostWriteLock (te)->getPixels();
		char *buf;

		size_t bufsize = BufferReadLock(buffer)->getSize ();
		buf = new char [bufsize];
		BufferReadLock(buffer)->getData(0,bufsize,buf);
		JpegFile file;
		if (!bitmap)
		{
			JpegFile::VertFlipBuf(buf,(((unsigned int)bufsize)/height),height);
			file.RGBToJpegFile (fileName,buf,width,height,true,99);
		}
		else
		{
			JpegFile::VertFlipBuf(buf,(((unsigned int)bufsize)/height),height);
			file.RGBToJpegFile (fileName + string("tmp"),buf,width,height,true,99);
			char *dataBuf = JpegFile::JpegFileToRGB (
				fileName + string("tmp"),
				&width,
				&height,
				std::string(fileName),
				true);
			delete [] dataBuf;

			// Should also delete the temp jpg or do the export directly..


			//JpegFile::VertFlipBuf(buf,(bufsize/height),height);
			//file.RGBToBitmapFile (fileName,buf,width,height,true,70);
		}
#endif
		return true;
	}

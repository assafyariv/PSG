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

#pragma once

#ifndef SCR_PIPLE
#define SCR_PIPLE

#include <nvui/SceneRenderer.h>
#include <nvsg/GeoNode.h>
#include <vector>
#include <string>

namespace nvgl
{
	class RenderContextGL;
	class SceneRendererGL2;
	class RenderTargetGLFB;
	class RenderTargetGLFBO;
	class RenderTargetGL;
	class RendererGLFSQ;
	typedef nvutil::SmartPtr< RenderContextGL > SmartRenderContextGL;
	typedef nvutil::SmartPtr< SceneRendererGL2 > SmartSceneRendererGL2;
	typedef nvutil::SmartPtr< RenderTargetGLFB > SmartRenderTargetGLFB;
	typedef nvutil::SmartPtr< RenderTargetGLFBO > SmartRenderTargetGLFBO;
	typedef nvutil::SmartPtr< RenderTargetGL > SmartRenderTargetGL;
	typedef nvutil::SmartPtr< RendererGLFSQ > SmartRendererGLFSQ;

}

namespace nvsg
{
	class Object;
	class Node;
	class GeoNode;
	class StateSet;
	class Group;
	//GeoNode::StateSetConstIterator;
	CORE_TYPES( Object, nvutil::HandledObject );
	CORE_TYPES( Node, Object );
	CORE_TYPES( GeoNode, Node );
	CORE_TYPES( Group, Node );
	CORE_TYPES( StateSet, Object );
	// CORE_TYPES( Texture, nvutil::HandledObject );
 // CORE_TYPES( TextureHost, Texture );
}

#define _DOING_SHADER_HLR
#define _DOING_POST_PROCESS
#define _DOING_POST_AO

class SceneRendererPipeline : public nvui::SceneRenderer
{
public:
	void SetPostProcBrightness (float val);
	void SetPostProcContrast (float val);
	void SetPostProcHue (float val);
	void SetPostProcSaturation (float val);
	void SetPostProcNoise (float val);
	void SetPostProcSoften(float val);
	void SetPostProcAOIntensity(float val);
	void SetPostProcEnabled (bool val);
	void SetPostProcBrightnessEnabled (bool val);
	void SetPostProcContrastEnabled (bool val);
	void SetPostProcHueEnabled (bool val);
	void SetPostProcSaturationEnabled (bool val);
	void SetPostProcNoiseEnabled (bool val);
	void SetPostProcSoftenEnabled(bool val);
	void SetPostProcAOEnabled(bool val);
	void SetPostProcDOFEnabled(bool val);
	void SetPostProcDOFPlane (float val);
	void SetPostProcGAMMA(float val);
	void SetPostProcGAMMAEnabled(bool val);
	void SetPostProcTONEMAPEnabled(bool val);
	void SetPostProcFXAA(float val);
	void SetPostProcTONEMAP(float val);

	void SetSceneBackColor(int r,int g,int b);
  SceneRendererPipeline(int viewId);
  ~SceneRendererPipeline();
  void resetShader () {m_shaderstart = true;}
  bool init(const nvgl::SmartRenderContextGL &renderContext, const nvgl::SmartRenderTargetGL &renderTarget);
  void doHLR (const nvsg::ViewStateSharedPtr &viewState, const nvgl::SmartRenderTargetGLFB &renderTargetGL);
  void replaceStateSet (const nvsg::GeoNodeSharedPtr geoNode/*, const nvsg::StateSetSharedPtr & newSsh*/);
   void replaceStateSet2(const nvsg::GeoNodeSharedPtr geoNode,bool shaded = false);
    void SetGeoNodeTechnique(const nvsg::GeoNodeSharedPtr geoNode,bool shaded = false);
  
 void doShader2( const nvsg::ViewStateSharedPtr &viewState,bool shaded = false);
 void SetShaderTechnique( const nvsg::ViewStateSharedPtr &viewState,bool shaded = false);
 
  void doShader( const nvsg::ViewStateSharedPtr &viewState);
  void setFrontViewState( const nvsg::ViewStateSharedPtr &viewState) {m_frontViewState = viewState;}
  void setSceneRenderer(const nvui::SmartSceneRenderer &sceneRenderer);
  nvui::SmartSceneRenderer getSceneRenderer() const; 
  void ToggleHighlighting();
  void ToggleCellShading(bool enable);
  void ToggleFlatShading(bool enable);
  void ToggleHLR(bool enable);
  bool SetBoolUseOptix(bool val);
  void ResetTargets ();
  void setHasHighlightedObject( bool val ) { m_hasHighlightedObject = val;}
  void SetHighlighting (bool hi) {m_highlighting = hi;}
  void SetDoingPreHihglight ( bool val ) { m_doingPreHighlight = val;}
  //void SetSceneBackImage(std::string filename);
  bool SaveContextToFile (std::string fileName, unsigned int width,unsigned int height,bool bitmap,bool tryOptix );
protected:
  virtual void doRender(const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget);

private:
  class MonoViewStateProvider : public SceneRenderer::StereoViewStateProvider
  {
  public:
	static nvutil::SmartPtr<MonoViewStateProvider> create();

  protected:
    nvsg::ViewStateSharedPtr calculateViewState( const nvsg::ViewStateSharedPtr &viewState, nvui::RenderTarget::StereoTarget eye )
    {
      return viewState;  // NOP, this lets SceneRenderer::doRender() use the incoming ViewState camera.
    }
  };

private:
	void SetPostProcValue (std::string param,float val);
	bool SetAmbientOcclusionParams (nvsg::ViewStateSharedPtr vs, int width, int height);
	bool SetAmbientOcclusionTechnique (const std::string techName);
	//nvsg::CgFxSharedPtr m_imgProcEffect;
	float					m_postprocBrightness;
	float					m_postprocContrast;
	float					m_postprocHue;
	float					m_postprocSaturation;
	float					m_postprocNoise;
	float					m_postprocSoftness;
	bool					m_postEnabledBrightness;
	bool					m_postEnabledContrast;
	bool					m_postEnabledHue;
	bool					m_postEnabledSaturation;
	bool					m_postEnabledNoise;
	bool					m_postEnabledSoften;
	bool					m_postEnabledGamma;
	bool					m_postEnabledToneMap;
	bool					m_postProcEnabled;
	bool					m_postEnabledAO;
	bool					m_postEnabledDOF;
	float					 m_postprocAOIntensity;
	float                    m_postprocFXAA;
	float                    m_postprocGamma;
	float					 m_postprocToneMap;
	float					 m_postprocDOFPlane;
	bool					m_doingMRT;
	bool					m_postProcSupported;
	nvmath::Vec4f			m_backColor;


	nvui::SmartSceneRenderer				m_sceneRendererFront;
  nvui::SmartSceneRenderer					m_sceneRenderer; // The renderer for the main image on the framebuffer (rasterizer or ray tracer).  
  nvutil::SmartPtr<MonoViewStateProvider>	m_monoViewStateProvider;
  nvsg::CgFxSharedPtr						m_efg; 
  nvsg::NodeSharedPtr						m_node;
  nvsg::GeoNodeSharedPtr					m_geoNode;
  nvsg::GeoNode::StateSetConstIterator		m_statIter;
  nvsg::StateSetSharedPtr					m_state;
  nvsg::ViewStateSharedPtr					m_frontViewState;
  nvgl::SmartRenderTargetGLFBO				m_highlightFBO;             // The monoscopic FBO for the highlight rendering and processing.
  nvgl::SmartSceneRendererGL2				m_sceneRendererHighlight;   // The renderer for the highlighted objects into the FBO.
  nvgl::SmartRendererGLFSQ					m_rendererStencilToColor;
  nvgl::SmartRendererGLFSQ					m_rendererHighlight;
  bool										m_highlighting;
  bool										m_CellShading;
  bool										m_HLR;
  bool										m_shaderstart;
  bool										m_useOptix;
  bool										m_doingPreHighlight;
  int m_viewId;
  bool										m_hasHighlightedObject;
  bool								m_flatShading;

#ifdef _DOING_SHADER_HLR
  // Shader based HLR/sketch.
   nvgl::SmartRenderTargetGLFBO				m_sketchFBO;   
   nvgl::SmartRendererGLFSQ					m_rendererSketch;
   nvgl::SmartSceneRendererGL2				m_sceneRendererSketch;   // The renderer for the highlighted objects into the FBO.
#endif
#ifdef _DOING_POST_PROCESS
	// post-proccesing effectts.
   nvsg::GeoNodeSharedPtr					m_cachedRoomTemp; // temporrary store the room pointer, so we can hide it in normal/depth pass.
   nvgl::SmartRenderTargetGLFBO				m_postprocFBO;   
   nvgl::SmartRenderTargetGLFBO				m_postprocAOMAP;   

    nvgl::SmartRendererGLFSQ				m_rendererPostproc;
	nvgl::SmartRendererGLFSQ				m_rendererPostprocAO;
    nvui::SmartSceneRenderer				m_sceneRendererPostProc;   // The renderer for the highlighted objects into the FBO.
#endif
	nvgl::SmartRenderTargetGLFBO				m_exportFBO;
	bool										m_isExporting;
};

typedef nvutil::SmartPtr<SceneRendererPipeline> SmartSceneRendererPipeline;

#endif

/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once

#ifndef INCLUDED_AVC_SCN_ADA
#define INCLUDED_AVC_SCN_ADA

#include <nvutil/SmartPtr.h>
#include <nvutil/Handle.h>
#include <nvrt/SceneRendererRTBuiltinGL.h>
#include <nvgl/RenderTargetGLFBO.h>
//#include <nvrt/RenderTargetRTBuiltin.h>
#include "AvocadoParams.h"
namespace nvutil
{
	//class Handle;
	class HandledObject;
	CORE_TYPES_BASE( HandledObject );
}

namespace nvgl
{
	class RenderTargetGLFBO;
	class RenderTargetGLFB;
	class SceneRendererGL2;
	class RenderContextGL;
	typedef nvutil::SmartPtr< RenderTargetGLFB > SmartRenderTargetGLFB;
	typedef nvutil::SmartPtr< SceneRendererGL2 > SmartSceneRendererGL2;
	typedef nvutil::SmartPtr< RenderContextGL > SmartRenderContextGL;
	typedef nvutil::SmartPtr< RenderTargetGLFBO > SmartRenderTargetGLFBO;
}

namespace nvrt
{
	class RenderTargetRT;
	class RTContext;
	class SceneRendererRTBuiltinGL;
	class RTBase;
	CORE_TYPES( RTBase, nvutil::HandledObject);
	CORE_TYPES( RTContext,RTBase );
	
	typedef nvutil::SmartPtr< RenderTargetRT > SmartRenderTargetRT;
	typedef nvutil::SmartPtr< SceneRendererRTBuiltinGL > SmartSceneRendererRTBuiltinGL;
}

namespace nvsg 
{
	class Object;
	class Node;
	class ViewState;
	class Drawable;
	class StateSet;
	class Scene;
	CORE_TYPES( Object, nvutil::HandledObject );
	CORE_TYPES( Node, Object );
	CORE_TYPES( Scene, Object );
	CORE_TYPES( ViewState, nvutil::HandledObject );
	CORE_TYPES( Drawable, Object );
	CORE_TYPES( StateSet, Object );
}
using namespace nvsg;

namespace nvtraverser 
{
	class Intersection;
}

namespace avocado
{
	class IDGenerator;
	typedef nvutil::SmartPtr< IDGenerator > SmartIDGenerator;
	class IDGenerator : public nvutil::RCObject 
	{
		public:
			unsigned int				m_nextID;
		public:
				IDGenerator() { m_nextID = 0;} 
				~IDGenerator() {}
			static SmartIDGenerator createNew () ;
			
			unsigned int RequestUniqueID () { return m_nextID++; }
	};

	class CNVSGViewData
	{
	public:
		CNVSGViewData();
		~CNVSGViewData();
		
		void SetAntiAliasing (bool enable = true);
		bool SaveContextToFile (string fileName,unsigned int width = 256,unsigned int height = 256,bool bitmap = false, bool tryOptix = true);
		bool SetupRenderContextGL(void *phWnd ,bool &optiXsupport, bool &antiAliasingSupport,bool from_hwnd = true,const nvgl::SmartRenderContextGL &shared = 0);
		nvgl::SmartRenderContextGL &GetRenderContextGL(){ return m_renderContextGL; }
		bool SetupSceneRendererGL2();
		void ResetAllContexts();
		void SetRenderContextSize(int cx, int cy);
		ViewStateSharedPtr &GetViewState(){ return m_viewState; }
		ViewStateSharedPtr GetFrontViewState(){ return m_frontLayerViewState; }
		bool SetViewState(SceneSharedPtr &scene) ;
		void SetupDefaultLight(ViewStateSharedPtr	&viewState);
		void SetCameraLocation (float *mat);
		void GetCameraLocation (float **mat);
		
		void SetGlobalHeadLight(float f);

	public:
		ViewStateSharedPtr			m_viewState;
		nvgl::SmartRenderContextGL			m_renderContextGL;
		nvgl::SmartRenderTargetGLFB			m_renderTargetGL;
		nvrt::SmartSceneRendererRTBuiltinGL	m_sceneRendererRT;
		nvrt::RTContextSharedPtr            m_renderContextRT;
		nvgl::SmartSceneRendererGL2			m_sceneRendererGL2;
		ViewStateSharedPtr			m_frontLayerViewState;
		ParamListSharedPtr			m_viewParams;
		bool						m_doingAA;
		nvgl::SmartRenderTargetGLFBO m_exportFBO;
	};

	class CNVSGDocData
	{
	public:
		CNVSGDocData();
		~CNVSGDocData();
		void ResetScene();
		void ResetRoomData();
		//std::string GetFileName(){ return m_filename; }
		SceneSharedPtr &GetScene(){ return m_scene; }
		void SetDefScene();
		SmartIDGenerator getIDGenerator () { return m_smartIDGen;}
		unsigned int RequestUniqueID () { return m_smartIDGen->RequestUniqueID(); }
		ParamListSharedPtr			m_docParams;
		bool SetSceneBackColor (int r, int g, int b);
		bool SetSceneBackImage (std::string filename);
		bool	SetLightsPreset (int pre,std::vector <nvsg::LightSourceSharedPtr> &return_lights);
		bool	SetLightDirection (float value,int LightIndex,int axis);
		bool	SetLightIntensity (float value,int LightIndex);
		bool	SetLightEnabled (bool value,int LightIndex);
		bool	SetLightAmbientColor (float value,int LightIndex);
		bool	SetLightDiffuseColor (float value,int LightIndex);
		bool	SetLightSpecularColor (float value,int LightIndex);
		bool SetRoomType (string type);
		bool SetRoomSize (float size);
		bool SetRoomScale (int axis, float scaleVal );
		bool SetGlobalExposure ( float scaleVal );
		bool SetGlobalAmbientIntensity(float f);
		bool SetGlobalAmbientColor(float f);
		nvsg::StateSetSharedPtr PrepareRoomStateSet();
		void SetSessionFolder (string folder);
	private:
		std::string				    m_filename;
		SceneSharedPtr				m_scene;
		SmartIDGenerator			m_smartIDGen;
		nvsg::GeoNodeSharedPtr m_cachedRoomGeo;
		nvsg::TransformSharedPtr m_cachedRoom;
		float					m_globalExposure;
		std::string m_cachedBgImage;
		float m_cachedScale[3];
		std::string m_roomType;
		float m_globalAmbientColor[3];
		float m_globalAmbientIntensity;
		std::string					m_sessionFolder;
	};

}
#endif
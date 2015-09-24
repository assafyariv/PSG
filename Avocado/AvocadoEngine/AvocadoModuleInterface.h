#pragma once
#include <nvutil/Handle.h>
#include "AvocadoMessageHandler.h"
#include "AvocadoScenixAdapter.h"
#include <nvrt/SceneRendererRTBuiltinGL.h>
namespace nvgl
{
	class RenderTargetGLFB;
	class SceneRendererGL2;
	class RenderContextGL;
	typedef nvutil::SmartPtr< RenderTargetGLFB > SmartRenderTargetGLFB;
	typedef nvutil::SmartPtr< SceneRendererGL2 > SmartSceneRendererGL2;
	typedef nvutil::SmartPtr< RenderContextGL > SmartRenderContextGL;
}

namespace nvsg {
	class Object;
	class Scene;
	class ViewState;
	CORE_TYPES( Object, nvutil::HandledObject );
	CORE_TYPES( Scene, Object );
	CORE_TYPES( ViewState, nvutil::HandledObject );
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
using namespace nvsg;

namespace avocado
{
	// TODO: make shared base module interface !!! Engine will have one general full modules list
	enum AvcModuleType {
		AVC_ENG_MOD_TYPE = 0,
		AVC_DOC_MOD_TYPE,
		AVC_VIEW_MOD_TYPE
	};

	class AvocadoEngineModule: public AvocadoMessageHandler
	{
	public:
		AvocadoEngineModule () {SetType(AVC_ENG_MOD_TYPE);}
		AvocadoEngineModule (string name) : AvocadoMessageHandler (name) {SetType(AVC_ENG_MOD_TYPE);}
		virtual ~AvocadoEngineModule () {}
		virtual AvocadoEngineModule *create () = 0;
		AvcModuleType	GetType(){ return m_type; }
		void SetType(AvcModuleType type){ m_type = type; }
		virtual bool OnUnload () = 0;
		virtual bool OnRegister () = 0;
		virtual bool registerModule (int engId)
		{
			m_engId = engId; 
			OnRegister(); 
			return true;
		}
		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint) = 0;
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint) = 0;
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) = 0;

		int				m_engId;
		AvcModuleType	m_type;
		string			m_name;
	};

	class AvocadoViewModule: public AvocadoMessageHandler
	{
	public:
		AvocadoViewModule () {SetType(AVC_VIEW_MOD_TYPE);}
		AvocadoViewModule (string name) : AvocadoMessageHandler (name) {SetType(AVC_VIEW_MOD_TYPE);}
		virtual ~AvocadoViewModule () {}
		virtual AvocadoViewModule* create () = 0;
		virtual AvcModuleType	GetType(){ return m_type; }
		virtual void SetType(AvcModuleType type){ m_type = type; }
		virtual bool OnUnload () { return true; }
		virtual bool OnRegister () { return true; } 
		virtual bool registerModule (const ViewStateSharedPtr				&frontViewState,
									 const ViewStateSharedPtr				&viewState,
									 const nvgl::SmartRenderTargetGLFB			renderTarget, 
									 const nvgl::SmartSceneRendererGL2			sceneRendererGL2,
									 const nvgl::SmartRenderContextGL			renderContextGL, 
									 const nvrt::SmartSceneRendererRTBuiltinGL	sceneRendererRT,
								     const nvrt::RTContextSharedPtr				renderContextRT,
									 int viewId)
		{ 
			m_viewState			= viewState; 
			m_frontViewState    = frontViewState;
			m_renderTarget		= renderTarget; 
			m_sceneRendererGL2	= sceneRendererGL2; 
			m_renderContextGL	= renderContextGL; 
			m_sceneRendererRT   = sceneRendererRT;
	         m_renderContextRT  = renderContextRT;
			m_viewId = viewId; 
			OnRegister(); 
			return true;
		}

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint) = 0;
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint) = 0;
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint) = 0;

		ViewStateSharedPtr			m_viewState;
		ViewStateSharedPtr			m_frontViewState;
		nvgl::SmartRenderTargetGLFB			m_renderTarget;
		nvgl::SmartSceneRendererGL2			m_sceneRendererGL2;
		nvgl::SmartRenderContextGL			m_renderContextGL;
		nvrt::SmartSceneRendererRTBuiltinGL	m_sceneRendererRT;
		nvrt::RTContextSharedPtr            m_renderContextRT;
		int									m_viewId;
		AvcModuleType						m_type;
	};

	class AvocadoDocModule: public AvocadoMessageHandler
	{
	public:
		AvocadoDocModule (string name) : AvocadoMessageHandler(name) {SetType(AVC_DOC_MOD_TYPE);}
		AvocadoDocModule () {SetType(AVC_DOC_MOD_TYPE);}
		virtual ~AvocadoDocModule () {}
		virtual AvocadoDocModule *create () = 0;
		virtual AvcModuleType	GetType(){ return m_type; }
		virtual void SetType(AvcModuleType type){ m_type = type; }
		
		virtual bool OnUnload ()  { return true; }
		/*virtual bool setScene (const SceneSharedPtr &scene)
		{
			m_scene = scene;
			return true;
		}*/
		virtual bool registerModule (const SceneSharedPtr &scene,const SmartIDGenerator &smartIDGen,
									 int docId, string sessionFolder)
		{
			m_scene = scene; 
			m_docId = docId;
			m_smartIDGen = smartIDGen;
			m_sessionFolder = sessionFolder;
			OnRegister();
			return true;
		}
		virtual bool OnRegister ()  { return true; }

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int docId, int x, int y, int zDelta, bool &needRepaint) = 0;
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint) = 0;
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) = 0;

		SceneSharedPtr			        m_scene;
		SmartIDGenerator			    m_smartIDGen;
		int								m_docId;
		AvcModuleType					m_type;
		std::string						m_sessionFolder;
	};

}
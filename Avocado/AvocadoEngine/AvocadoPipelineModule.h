#pragma once
#include "AvocadoModuleInterface.h"
#include "AvocadoPipeline.h"

namespace avocado 
{
	class AvocadoPipeline : public AvocadoViewModule 
	{
	public:
		AvocadoPipeline ();
		virtual ~AvocadoPipeline ();
		virtual AvocadoViewModule* create ();

		bool OnUnload ();
		virtual bool OnRegister ();

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint){ return false; }		// unused
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool &needRepaint);
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint) { return false; }	// unused

		
		 SmartSceneRendererPipeline	m_pipeline;
		 nvsg::GroupSharedPtr							m_currentHighlightedGroup;
		 nvsg::GroupSharedPtr							m_currentPreHighlightedGroup;
		 void HighlighStateSetCurrentDrawable (bool h);
		 nvsg::DrawableSharedPtr							m_currentHighlightedDrawable;
		 nvsg::DrawableSharedPtr							m_currentPreHighlightedDrawable;
		 int m_mainHighlightMask;
		 int m_subHighlightMask;
		 nvsg::StateSetSharedPtr		m_savedSS;
		 nvsg::StateSetSharedPtr		m_highSS;
		 std::string							m_savedRenderStyle;
		 int m_highlightGroupCount;
	};
}




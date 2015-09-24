#pragma once
#include "AvocadoModuleInterface.h"
#include "AvocadoEngineObject.h"

namespace avocado 
{
    class AvocadoEngineDocAnnotationElement: public AvocadoEngineDocElement
	{
	public:
		AvocadoEngineDocAnnotationElement(int docId,string ownerModuleName , string fileName);
		~AvocadoEngineDocAnnotationElement();
		void	ResetScene();
		virtual void createScene(SceneSharedPtr &scene);
	private:
		std::string					m_fileName;
		SceneSharedPtr				m_scene;
		int							m_docId;
	};


	class AvocadoAnnotations : public AvocadoDocModule 
	{
	public:
		AvocadoAnnotations ();
		virtual ~AvocadoAnnotations ();
		virtual AvocadoDocModule* create ();

		bool OnUnload ();
		virtual bool OnRegister ();
		int AddAnnotationElement(std::string type);
		bool DeleteAnnotationElement(int elemId);
		void ClearDocFileElements();
		std::vector<AvocadoEngineDocAnnotationElement *>::iterator GetAnnotationById(int elemId);
		void OnElementUnSelected (int eid,int vid);
		void OnElementSelected (int eid,int vid,bool sub);
		void OnElementPreSelected (int eid,int vid);
		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint){ return false; }		// unused
		virtual bool HandleAvocadoViewGeneralStringMessage (std::string msg, int viewId, std::string paramStr, bool &needRepaint){ return false; }	// unused
		virtual bool HandleAvocadoDocGeneralStringMessage (std::string msg, int docId, std::string paramStr, bool &needRepaint) ;

		std::vector<AvocadoEngineDocAnnotationElement *>	m_annotations;
		
	};
}
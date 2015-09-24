#pragma once
#include "AvocadoModuleInterface.h"
#include "AvocadoEngineObject.h"
#include <hash_map>

namespace avocado 
{
	
    class AvocadoEngineDocAnnotationElement: public AvocadoEngineDocElement
	{
	public:
		AvocadoEngineDocAnnotationElement(int docId,string ownerModuleName , string fileName);
		~AvocadoEngineDocAnnotationElement();

		virtual void attachmentMoved (int attID,nvsg::GroupSharedPtr &node,nvsg::SceneSharedPtr &scene) ;
		virtual string serialize() ;
		virtual void removeFromScene(SceneSharedPtr &scene); 
		virtual void createScene(SceneSharedPtr &scene,std::string sessionFolder);
		void annotationMoved (SceneSharedPtr &scene);
		void RecalcAnnotation ();
		void setLocation (float *mat,bool updateLastSaved);
		void setTargetAttachement (int x,int x2) {m_targetEID = x;m_targetEID2 = x2;}
		int GetTargetEID () {return m_targetEID;}
		int GetTargetEID2 () { return m_targetEID2;}
		bool setAnnotationParam (string paramName,string paramValue);
		virtual bool  SetVisibility (bool isVisible ) ;
	private:
		int							m_targetEID;
		int							m_targetEID2;
		std::string					m_type;
		nvsg::ClipPlaneSharedPtr	m_plane;
		nvsg::SpotLightSharedPtr	m_light;
		nvmath::Vec3f  m_cachedPt2;
		nvmath::Vec3f  m_cachedPt1;
		bool m_pt2Initialized;
		bool m_pt1Initialized;
	};

	typedef std::hash_map<int,AvocadoEngineDocAnnotationElement*> AnnotationElementHash;
	typedef std::hash_map<int,AvocadoEngineDocAnnotationElement*>::iterator AnnotationElementHasIterator;

	class AvocadoAnnotations : public AvocadoDocModule 
	{
	public:
		AvocadoAnnotations ();
		virtual ~AvocadoAnnotations ();
		virtual AvocadoDocModule* create ();

		bool OnUnload ();
		virtual bool OnRegister ();
		int AddAnnotationElement(std::string type);
		bool DeleteAnnotationElement(std::vector<AvocadoEngineDocAnnotationElement *>::iterator &it);
		void ClearDocFileElements();
		std::vector<AvocadoEngineDocAnnotationElement *>::iterator GetAnnotationById(int elemId);
		AvocadoEngineDocAnnotationElement * GetAnnotationPtrById(int elemId);
		void OnElementUnSelected (int eid,int vid);
		void OnElementSelected (int eid,int vid,bool sub);
		void OnElementPreSelected (int eid,int vid);
		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint){ return false; }		// unused
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint){ return false; }	// unused
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) ;
		bool AnnotationMoved (int eid);


		std::vector<AvocadoEngineDocAnnotationElement *>	m_annotations;
		AnnotationElementHash				m_elementHash;
	};
}
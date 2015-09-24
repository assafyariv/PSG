#pragma once
#include "AvocadoModuleInterface.h"
#include "AvocadoEngineObject.h"
#include <hash_map>

namespace avocado 
{
	
    class AvocadoEngineDocFileElement: public AvocadoEngineDocElement
	{
    private:
		bool DrinkMaterials (nvsg::StateSetSharedPtr originalSS);

	public:
		AvocadoEngineDocFileElement(int docId,string ownerModuleName , string fileName);

		~AvocadoEngineDocFileElement();
		virtual bool ApplyStateSet (nvsg::StateSetSharedPtr newStateSet, bool cache,bool overwrite, bool convertFFP = false) ;
		bool splitGroup (SceneSharedPtr &scene,std::string sessionFolder);
		bool addToGroup (SceneSharedPtr &scene,std::vector<AvocadoEngineDocFileElement*>	&children);
	
		bool createGroup (SceneSharedPtr &scene);
		virtual void removeFromScene(SceneSharedPtr &scene); 
		bool createFromElement (SceneSharedPtr &scene, AvocadoEngineDocFileElement *el,std::string geoName,bool &hasColor);
		virtual void createScene(SceneSharedPtr &scene,std::string sessionFolder,bool skip_optimization = false);
		void removeGeoNodes(std::vector<std::string>);	
		virtual string serialize() ;
	
		void setColor (int color[]);
		void setMaterial (int materialID);
		void setLocation (float *mat,bool updateLastSave = false);
		void setChildren (std::vector<AvocadoEngineDocFileElement*>	&children);
		AvocadoEngineDocFileElement* getChild (size_t i) { return m_children[i];}
		size_t getChildrenCount () {return m_children.size();}
		bool						m_isGroup;
		std::string						m_groupType;
	private:
		std::string					m_fileName;
	
		std::vector<AvocadoEngineDocFileElement*>	m_children;
		// utility functions to help out with 25 (and maybe growing!!) material properties. 
		void serializeMaterial (ParamListSharedPtr &ppl);

		bool ApplyStateSetRecursive (int &level,nvsg::StateSetSharedPtr newStateSet, bool cache, bool overwrite,bool convertFFP = false);
		
	};
	

	typedef std::hash_map<int,AvocadoEngineDocFileElement*> DocFileElementHash;
	typedef std::hash_map<int,AvocadoEngineDocFileElement*>::iterator DocFileElementHasIterator;

	class AvocadoImport : public AvocadoDocModule 
	{
	public:
		AvocadoImport ();
		virtual ~AvocadoImport ();
		virtual AvocadoDocModule* create ();

		// Registration
		bool OnUnload ();
		virtual bool OnRegister ();

		// Message handling.
		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint){ return false; }		// unused
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool &needRepaint){ return false; }	// unused
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) ;
	private:
		bool deserializeMaterial (ParamListSharedPtr &ppl,AvocadoMaterialInterface *m_intr);
		void SetElementColor (AvocadoEngineDocFileElement* eid, int r, int g, int b,bool applyShader = true,bool overwrite = false);
		void SetElementMaterial (AvocadoEngineDocFileElement* eid, int matid, bool overwriteGroup = false);
		void SetElementMaterialPropAll (int eid, ParamListSharedPtr &pl, bool applyBaseColor = false);
		void SetElementMaterialProp (int eid, float matid,std::string propname);
		void SetElementMaterialPropString (int eid, std::string matid,std::string propname);
		void SetElementMaterialProp3Float (int eid, string matID,float f0,float f1,float f2,float f3);
		// Element creation
		AvocadoEngineDocFileElement* AddDocInstancedElement(int eid,  AvocadoElementInterface::MetaDataList *metaData,int inputID,string inputName );
		AvocadoEngineDocFileElement* AddDocFileElement(std::string filename, AvocadoElementInterface::MetaDataList *metaData,float *mat,int inputID,std::string inputName,bool isGroup,std::string groupType);
		bool DeleteDocFileElement(std::vector<AvocadoEngineDocFileElement *>::iterator &it );
		bool DeleteElementChildren (int elemId,int docId);
		void ClearDocFileElements();
		std::vector<AvocadoEngineDocFileElement *>::iterator GetDocFileElemById(int elemId);
		 AvocadoEngineDocFileElement* GetDocFileElemPtrById (int elemId);
		// Element selection
		void OnElementUnSelected (int eid,int vid);
		void OnElementSelected (int eid,int vid,bool sub);
		void OnElementPreSelected (int eid,int vid);
		void HideElement (int eid);
		
		// Members
		std::vector<AvocadoEngineDocFileElement *>	m_docFileElements;	
		DocFileElementHash				m_elementHash;
		std::string						 m_lastbackimage;
	};
}
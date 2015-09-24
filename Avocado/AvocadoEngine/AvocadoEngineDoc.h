/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoEngineView.h"
#include "AvocadoEngineObject.h"
#include <hash_map>

namespace avocado
{
	class CNVSGDocData;
	class AvocadoViewStateData {
	public:
		nvmath::Mat44f cameraMatrix;
		std::string bg_image_file;
		std::string html_text;
		int     viewID;
		std::vector<std::pair<int,nvmath::Trafo>> elementLocation;
		std::vector<std::pair<int,bool>> elementVisibility;
	};
	
	class AvocadoMaterialStateData
	{
	public:
		std::vector<std::pair<int,string>>  m_ss;
		std::string   m_name;
	};

	class AvocadoEngineDoc: public AvocadoEngineObject
	{
		typedef std::hash_map<int,AvocadoEngineDocElement*> DocElementHash;
    	typedef std::hash_map<int,AvocadoEngineDocElement*>::iterator DocElementHasIterator;

	public:
		AvocadoEngineDoc() ;
		~AvocadoEngineDoc() ;
			void ResetDocIntr ();
		AvocadoDocInterface							*GetDocInterface(){ return m_docInterface; }
		CNVSGDocData								*GetCNVSGDocData();
		int											OnCreateView(void *phWnd, AvocadoViewInterface	*viewInterface, bool from_hwnd = true);
		//std::vector<int *>::iterator				GetDocFileElementById(int elemId);
		std::vector<AvocadoEngineDocElement *>::iterator GetDocElementById(int elemId);
		AvocadoEngineDocElement *					 GetDocElementPtrById(int elemId);
		//void										OnAddDocFileElement(int elemId);
		//void										OnDeleteDocFileElement(int elemId);
		void										OnAddDocElement(AvocadoEngineDocElement **elem);
		void										OnDeleteDocElement(int elemId);
		
		//void										ClearDocFileElements ();
		void										ClearDocElements ();
		void										ClearViewStates();
		void										ClearMaterialStates ();
		std::vector<AvocadoEngineView *>::iterator	GetViewById(int id);
		int											AddView(); 
		bool										DeleteView(int id);
		void										InvokePaintView(int id);
		void										NotifySelectionChanged (int vid,std::vector<int>  eids);
		void										NotifyElementsChanged();
		void										NotifyMaterialStateChanged();
		void										InvokePaintAll();
		void										PopulateViewStateWithElementLocation (int vid);
		void										RestoreElementLocationFromViewState (int vid);
		bool										OnPaintView(int id);
		bool										OnSizeView(int id, int px, int py);
		void										AddDocModule (AvocadoDocModule *module);
		virtual bool								HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint);
		virtual bool								HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool &needRepaint);
		virtual bool								HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint);
		bool										HandleSpecificModuleMessage (std::string msg, int docId, std::string paramStr, bool &needRepaint,string targetModule);
		void										SetDocInterface (AvocadoDocInterface *docInterface) ;
		void										ClearDocModules ();
		bool										SerializeDocument (std::string &serializedStr,bool isStoring);
		bool										InsertFile (std::string filename,AvocadoFileLinkInterface::FileType type,bool embed);
		bool										RemoveFile (std::string filename);
		bool										CompressFile (std::string &inpath,std::string &outpath, 
													std::vector<std::string> attachments,
													std::vector<std::string> embeddedModels,
													std::vector<std::string> embeddedTextures, 
													bool unzip);
		bool										isEmpty () { return (m_docElems.size() == 0);}
		bool										NotifyDocParamChanged (string paramStr);
		bool										NotifyViewStateChanged (int vsid);
		bool										UpdateDocParam (std::string paramStr);
		void										SetSessionFolder (std::string folder);
		void										AddMaterialState (std::string name,std::vector<int> eids);
		void										AddToMaterialState (int ms_index,std::vector<int> eids);
    	void										DeleteMaterialState (int);
		bool										SwitchToMaterialState (int);
		bool										SwitchToMaterialState (std::string ms_name);
	private:
		CNVSGDocData								*m_nvsgDocData;
		AvocadoDocInterface							*m_docInterface;
		std::vector<AvocadoEngineDocElement *>		m_docElems;
		std::vector<AvocadoEngineView *>			m_viewList;
		std::vector<AvocadoMaterialStateData>       m_materialStates;
		std::vector<AvocadoViewStateData>			m_viewStates;
		std::vector<AvocadoDocModule *>				m_modules;
		std::vector<AvocadoFileLinkInterface>				m_files;
		std::vector<AnimatedTransformSharedPtr>	m_animationWaitList;
		DocElementHash                                m_elementHash;
		std::string									m_sessionFolder;
	};
}
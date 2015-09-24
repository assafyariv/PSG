/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoEngineDoc.h"
#include "AvocadoAppOptionsInterface.h"
#include <nvutil/Timer.h>
#include <vector>

namespace avocado
{
	class AvocadoEngine : public AvocadoMessageHandler
	{
	public:
	/*enum AvocadoOptionType
	{
		AvocadoINT = 0,
		AvocadoFLOAT = 1,
		AvocadoSTRING = 2,
		AvocadoBOOL
	};*/
	public:
		AvocadoEngine(){}
		~AvocadoEngine(){}

		bool										AvocadoInit(bool multiThreded=false,std::string sessionFolder = "", std::string optionsFile = "");
		bool										AvocadoTerminate();
		std::vector<AvocadoEngineDoc *>::iterator	GetDocById(int docId);
		void										SetActiveDoc(int docId);
		AvocadoEngineDoc							*GetActiveDoc() { return m_activeDoc; }
		int											AddDoc(); 
		bool										DeleteDoc(int docId);
		int											OnCreateDoc(AvocadoDocInterface *docInterface);
		bool										DoCompressDoc (int docId, std::string &inpath,
														std::string &outpath, 
														std::vector<std::string> attachments,
														std::vector<std::string> embeddedModels,
														std::vector<std::string> embeddedTextures, 
														bool unzip);
		void										OnSerializeDoc(int docId, std::string &path,bool isImport = false, bool isStoring = false);
		
		void										AddModulesToDoc (int docId);
		void										AddDocModule (AvocadoDocModule * module);
		void										AddModulesToView (int viewId);
		void										AddViewModule (AvocadoViewModule * module);
		int											OnCreateView (void *phWnd, AvocadoViewInterface *viewInterface);
		int											OnCreateDCView (void *phWnd, AvocadoViewInterface *viewInterface);
		void										OnSizeView (int viewId, int px, int py);
		double										GetEngineTimer ();
		void										InvokePaintView(int viewId);
		void										NotifySelectionChanged (int viewIds, std::vector<int>  eids );
		void										NotifyElementsChanged ();
		void										InvokePaintAll ();
		int											GetID(){ return 0; }
		void										AddEngineModule (AvocadoEngineModule *module);
		void										ClearEngineModules ();
		virtual bool								HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint);
		virtual bool								HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint);
		virtual bool								HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint);
		bool										HandleSpecificModuleMessage (std::string msg, int docId, std::string paramStr, bool &needRepaint,string targetModule);
	
		bool										OnSendAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta);
		bool										OnSendAvocadoGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr,bool toAllModules);
		bool										OnSendAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr,string targetModule);
		void										RaiseAvocadoViewErrorMessage(int viewID,std::string err);
		void										RaiseAvocadoDocErrorMessage(int docID,std::string err);

		// Options
		bool									    SetAvocadoOption (std::string optionName,void *value, AvocadoOption::InternalType type);
		bool									    GetAvocadoOption (std::string optionName,void *value, AvocadoOption::InternalType type);
		bool										AvocadoSaveOverideOptions ();
		bool										GetAvocadoOptionStructure ( AvocadoOptionStructure & opt) const;
	private:
		// Options
		void										AvocadoInitDefaultOptions ();
	//void										AvocadoInitOptionStructure ();
		void										AvocadoReadOverideOptions (std::string filename);

		nvutil::Timer						m_todTimer;
		AvocadoEngineDoc					*m_activeDoc;
		std::vector<AvocadoEngineDoc *>		m_docList;
		std::vector<AvocadoDocModule *>		m_docModules;
		std::vector<AvocadoViewModule *>	m_viewModules;
		std::vector<AvocadoEngineModule *>	m_modules;
		std::string							m_sessionFolder;
		int									m_nestedMessageCount;
		int									m_nestedMessagePaintCount;

		/* Options handling */
		ParamListSharedPtr					m_defaultOptions;
		ParamListSharedPtr					m_overideOptions;
		std::string						    m_optionsFile;
		AvocadoOptionStructure				m_optionStructure;
	};
}
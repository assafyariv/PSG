/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#include "AvocadoEngine.h"
#include "AvocadoScenixAdapter.h"
#include "AvocadoInternalModules.h"

#include <nvui/SceneRenderer.h>
#include <nvgl/SceneRendererGL2.h>
#include <nvgl/RenderTargetGLFBO.h>
#include <nvgl/RendererGLFSQ.h>
#include <nvutil\Trace.h>


static std::string globalTracePath;
const std::string &GET_REG_AVOCADO_TRACE_FILE()
{
		return globalTracePath;
}

namespace avocado 
{
	// CAvocadoEngine

	bool AvocadoEngine::HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint)
	{
		bool ret = false;
		
		std::vector<AvocadoEngineView *>::iterator it = GetActiveDoc()->GetViewById (viewId);
		AvocadoEngineView *view = *it;
		ret = view->HandleAvocadoMouseStringMessage(msg,viewId,x,y,zDelta,needRepaint);
		// Handle the doc message in all views
		//ret = GetActiveDoc()->HandleAvocadoMouseStringMessage(msg,viewId,x,y,zDelta,needRepaint);
		
		return ret;
	}
	void AvocadoEngine::RaiseAvocadoViewErrorMessage(int viewID,std::string err)
	{
		if (GetActiveDoc())
		{
				std::vector<AvocadoEngineView *>::iterator it = GetActiveDoc()->GetViewById (viewID);
				AvocadoEngineView *view = *it;
				(*it)->GetViewInterface()->ErrorCallback (err.c_str ());
		}
	}
	void AvocadoEngine::RaiseAvocadoDocErrorMessage(int docID,std::string err)
	{
		//if (GetDocById(docID))
		std::vector<AvocadoEngineDoc *>::iterator it = GetDocById(docID);
		if (it != m_docList.end())
		{
			AvocadoEngineDoc *doc = *it;
			GetActiveDoc()->GetDocInterface()->ErrorCallback (err.c_str());
		}
	}
	bool AvocadoEngine::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());
		
		bool ret = false;
		
		if (msg == "OnPaint")
		{
			ret = GetActiveDoc()->OnPaintView(viewId);
		}
		else if (msg == "OnDestroy")
		{
			ret = GetActiveDoc()->DeleteView(viewId);
		}
		/*else if ()
		{
			HandleAvocadoViewGeneralStringMessage
		}*/
		else
		{
			std::vector<AvocadoEngineView *>::iterator it = GetActiveDoc()->GetViewById (viewId);
			AvocadoEngineView *view = *it;
			ret = view->HandleAvocadoViewGeneralStringMessage(msg,viewId,paramStr,needRepaint);
			// Handle the doc message in all views
			//ret = GetActiveDoc()->HandleAvocadoViewGeneralStringMessage(msg,viewId,paramStr,param,needRepaint);
		}
	
		return ret;
	}
	bool AvocadoEngine::HandleSpecificModuleMessage (std::string msg, int docId, std::string paramStr, bool &needRepaint,string targetModule)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());
		
		bool ret = false;
		
		std::vector<AvocadoEngineDoc *>::iterator iter = GetDocById (docId);
		if (iter != m_docList.end ())
		{
			AvocadoEngineDoc *doc= *iter;
			ret =  doc->HandleSpecificModuleMessage(msg,docId,paramStr,needRepaint,targetModule);
		}
		
		return ret;
	}

	bool AvocadoEngine::HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());

		bool ret = false;

		// First send the message to engine modules.
		for (size_t i=0;i<m_modules.size ();i++)
			m_modules[i]->HandleAvocadoDocGeneralStringMessage (msg,docId,paramStr,needRepaint);

	    // Now send to a specific document.
		std::vector<AvocadoEngineDoc *>::iterator iter = GetDocById (docId);
		if (iter != m_docList.end ())
		{
			AvocadoEngineDoc *doc= *iter;
			ret =  doc->HandleAvocadoDocGeneralStringMessage(msg,docId,paramStr,needRepaint);
		}
		
		return ret;
	}

	double AvocadoEngine::GetEngineTimer ()
	{
		//NVSG_TRACE();

		return m_todTimer.getTime ();
	}

	void AvocadoEngine::InvokePaintView(int viewId)
	{
		NVSG_TRACE();
		//if (m_nestedMessagePaintCount == 0)
		{
			GetActiveDoc()->InvokePaintView (viewId);
		}
		// else
	//	{
		//	NVSG_TRACE_OUT("NESTED REPAINT INTERSECEPTED");
	//	}
//		m_nestedMessagePaintCount ++;
	}
	void AvocadoEngine::NotifyElementsChanged()
	{
		NVSG_TRACE();

		GetActiveDoc()->NotifyElementsChanged ();
	}
	
	void AvocadoEngine::NotifySelectionChanged(int viewId,std::vector<int>  eids)
	{
		NVSG_TRACE();
		GetActiveDoc()->NotifySelectionChanged (viewId,eids);
	}
	void AvocadoEngine::InvokePaintAll ()
	{
		NVSG_TRACE();
		GetActiveDoc()->InvokePaintAll ();
	}

	int AvocadoEngine::OnCreateView (void *phWnd, AvocadoViewInterface *viewInterface)
	{
		NVSG_TRACE();
		int viewId = GetActiveDoc()->OnCreateView(phWnd,viewInterface);
		AddModulesToView(viewId);
		return viewId;
	}

	int AvocadoEngine::OnCreateDCView (void *phWnd, AvocadoViewInterface *viewInterface)
	{
		NVSG_TRACE();
		int viewId = GetActiveDoc()->OnCreateView(phWnd,viewInterface,false);
		AddModulesToView(viewId);
		return viewId;
	}

	void AvocadoEngine::OnSizeView (int viewId, int px, int py)
	{
		NVSG_TRACE();
		GetActiveDoc()->OnSizeView (viewId,px,py);
	}

	void AvocadoEngine::AddViewModule (AvocadoViewModule *module)
	{
		NVSG_TRACE();
		m_viewModules.push_back (module);
	}

	void AvocadoEngine::AddModulesToView(int viewId)
	{
		NVSG_TRACE();
		std::vector<AvocadoEngineView *>::iterator it = GetActiveDoc()->GetViewById (viewId);
		AvocadoEngineView *view = *it;
		for (size_t i=0;i<m_viewModules.size();i++)
			view->AddViewModule (m_viewModules[i]->create ());
	}

	void AvocadoEngine::AddDocModule (AvocadoDocModule *module)
	{
		NVSG_TRACE();
		m_docModules.push_back (module);
	}

	void AvocadoEngine::AddModulesToDoc(int docId)
	{
		NVSG_TRACE();
		std::vector<AvocadoEngineDoc *>::iterator it = GetDocById (docId);
		AvocadoEngineDoc *doc = *it;
		for (size_t i=0;i<m_docModules.size();i++)
			doc->AddDocModule (m_docModules[i]->create ());
	}

	void AvocadoEngine::AddEngineModule (AvocadoEngineModule *module)
	{
		NVSG_TRACE();
		m_modules.push_back (module);
		module->registerModule (GetID());
	}

	void AvocadoEngine::ClearEngineModules ()
	{
		NVSG_TRACE();
		for (size_t i=0;i<m_modules.size();i++)
		{	
			m_modules[i]->OnUnload ();
			delete m_modules[i];
			m_modules[i] = NULL;
		}
		m_modules.clear ();
	}
	void AvocadoEngine::AvocadoReadOverideOptions (std::string filename)
	{
		m_overideOptions = ParamList::createFromFile (filename);
	}
	bool AvocadoEngine::AvocadoSaveOverideOptions ()
	{
		return m_overideOptions->SaveToFile (m_optionsFile);
	}

	
	bool AvocadoEngine::AvocadoInit (bool multiThreded,string sessionFolder,string optionsFile)
	{
		// Global variable to store file name for trace output.
		globalTracePath =sessionFolder+"\\avocadoLog.log";
		NVSG_TRACE();
		
		// Not required so far, but might have a benefit.
		m_sessionFolder = sessionFolder;
		
		// Initialize SceniX.
		if (multiThreded)
			nvsg::nvsgInitialize (NVSG_MULTITHREADED); // SceniX
		else
			nvsg::nvsgInitialize (); // SceniX

		// Initialize optix.
		{
			nvrt::RTInitStatus rtinit_status =  nvrt::RTInit() ;
			if( rtinit_status!= nvrt::RT_INITIALIZED ) 
			{
				NVSG_TRACE_OUT("Optix Initialization error : \n");
				NVSG_TRACE_OUT(nvrt::RTGetInitStatusString(rtinit_status)  );
			} else
			{
				NVSG_TRACE_OUT("Optix Initialized Succesufully\n");
			}
		} 
		if (nvrt::RTIsSupported())
		{
			NVSG_TRACE_OUT("Optix supported\n");
		}
		else
		{
			NVSG_TRACE_OUT("Optix not supported\n");
		}
#if !defined(NDEBUG)
#if defined (D_NVSG_DGB_LEAK_DETECTION)
		// Enable leak detection only when needed, the debug executable is faster without.
		nvsg::nvsgSetDebugFlags ( nvsg::NVSG_DBG_ASSERT | nvsg::NVSG_DBG_LEAK_DETECTION );
#endif
#endif

		// Initizlize default options and read options file
		AvocadoInitDefaultOptions ();
		m_optionsFile = (optionsFile == "" ? "AvocadoOptions.conf" : optionsFile);
		AvocadoReadOverideOptions (m_optionsFile);


		m_nestedMessageCount = 0;
		m_nestedMessagePaintCount =0 ;

		// Start engine timer.
		m_todTimer.start ();

		// Load Modules.
		AddViewModule	( new AvocadoManipulator	() );
		AddViewModule	( new AvocadoAnimationManipulator		() );
		AddViewModule	( new AvocadoPipeline		() );
		AddViewModule	( new AvocadoPicker			() );
		AddViewModule	( new AvocadoDragger		() );

		AddDocModule	( new AvocadoImport			() );
		AddDocModule	( new AvocadoSelection		() );
		AddDocModule	( new AvocadoAnnotations	() );
		AddEngineModule	( new AvocadoNetwork		() );

		return true;
	}

	bool AvocadoEngine::AvocadoTerminate ()
	{
		//NVSG_TRACE();
		m_defaultOptions = 0;
		m_overideOptions = 0;

		for (size_t i=0;i<m_docList.size ();i++)
		{
			m_docList[i]->ResetDocIntr();
			delete (m_docList[i]);
			m_docList[i] = NULL;
		}
		m_docList.clear();

		//for (size_t i=0;i<m_viewModules.size ();i++)
		{
			//delete (m_viewModules[i]);
			AvocadoManipulator*	 am = (AvocadoManipulator*)m_viewModules[0];
			AvocadoAnimationManipulator*	amm = (AvocadoAnimationManipulator*)m_viewModules[1];
			AvocadoPipeline		 *plm = (AvocadoPipeline*)m_viewModules[2];
			AvocadoPicker			*pm = (AvocadoPicker*)m_viewModules[3];
			AvocadoDragger		*dm = (AvocadoDragger*)m_viewModules[4];
			delete dm;
			delete pm;
			delete plm;
			delete amm;
			delete am;
		}
		for (size_t i=0;i<m_viewModules.size ();i++)
			m_viewModules[i] = NULL;
		m_viewModules.clear ();

		//for (size_t i=0;i<m_docModules.size ();i++)
		{
			//delete (m_docModules[i]);
		
			 AvocadoImport*		ai =  (AvocadoImport*)m_docModules[0];
			 AvocadoSelection*	as =  (AvocadoSelection*)m_docModules[1];
			 AvocadoAnnotations* aa = (AvocadoAnnotations*) m_docModules[2];
			 delete ai;
			 delete as;
			 delete aa;
			
		}
		for (size_t i=0;i<m_docModules.size ();i++)
			m_docModules[i] = NULL;
		m_docModules.clear ();

		ClearEngineModules ();

		nvrt::RTShutdown();
		nvsg::nvsgTerminate ();

		return true;
	}

	void AvocadoEngine::SetActiveDoc (int docId)
	{
		NVSG_TRACE();
		std::vector<AvocadoEngineDoc *>::iterator it = GetDocById (docId);
		m_activeDoc = *it;
	}

	std::vector<AvocadoEngineDoc *>::iterator AvocadoEngine::GetDocById (int docId)
	{
		std::vector<AvocadoEngineDoc *>::iterator it = m_docList.begin ();
		while (it != m_docList.end ())
		{
			if ((*it)->GetID () == docId)
			{
				return it;
			}
			it++;
		}
		return m_docList.end ();
	}

	bool AvocadoEngine::DeleteDoc (int docId)
	{
		NVSG_TRACE();
		std::vector<AvocadoEngineDoc *>::iterator it = GetDocById (docId);
		delete *it;
		m_docList.erase (it);
		return true;
	}

	int AvocadoEngine::AddDoc ()
	{
		NVSG_TRACE();
		int id = 0;
		AvocadoEngineDoc *doc = new AvocadoEngineDoc ();
		if (!m_docList.empty())
		{
			std::vector<AvocadoEngineDoc *>::iterator it = m_docList.begin ();
			while (it != m_docList.end ())
			{
				if ((*it)->GetID () == id)
				{
					id++;
					it = m_docList.begin ();
					continue;
				}
				it++;
			}
		}
		std::stringstream idStr;
		idStr << id;
		std::string name = "doc_" + idStr.str();
		doc->SetID (id);
		doc->SetName (name);
		m_docList.push_back (doc);
		return id;
	}

	int AvocadoEngine::OnCreateDoc(AvocadoDocInterface *docInterface)
	{
		NVSG_TRACE();
		int docId = AddDoc ();
		std::vector<AvocadoEngineDoc *>::iterator it = GetDocById(docId);
		AvocadoEngineDoc *doc = *it;
		doc->GetCNVSGDocData()->SetDefScene();
		doc->SetDocInterface (docInterface);
		doc->SetSessionFolder (m_sessionFolder);
		AddModulesToDoc(docId);
		return docId;
	}
	bool AvocadoEngine::DoCompressDoc (int docId, std::string &inpath,
		std::string &outpath,
		std::vector<std::string> attachments, 
		std::vector<std::string> embeddedModels,
		std::vector<std::string> embeddedTextures, 
		bool unzip)
	{
		NVSG_TRACE();
			std::vector<AvocadoEngineDoc *>::iterator it = GetDocById(docId);
			AvocadoEngineDoc *doc = *it;
			return doc->CompressFile (inpath,outpath,attachments,embeddedModels,embeddedTextures,unzip);
	}
	void AvocadoEngine::OnSerializeDoc (int docId, std::string &path,bool isImport, bool isStoring)
	{
		NVSG_TRACE();
			std::vector<AvocadoEngineDoc *>::iterator it = GetDocById(docId);
			AvocadoEngineDoc *doc = *it;
			bool needRepaint = false;
			if (isImport)
			{
				bool firstImport = GetActiveDoc()->isEmpty();
				ParamListSharedPtr pl = ParamList::createNew ();
				pl->PushBool ("isRef",false);
				pl->PushBool ("isGroup",false);
				pl->PushString ("fileName",path);
				pl->PushInt ("MetaCount",0);

				HandleAvocadoDocGeneralStringMessage("AddDocFileElement",docId,pl->SerializeList (),needRepaint);
				if (firstImport)
						HandleAvocadoDocGeneralStringMessage("FitToPage",docId,path,needRepaint);
						
			}
			else
			{			
				{
					GetActiveDoc()->SerializeDocument (path,isStoring);
					if (0)
					//	0//!isStoring)
						HandleAvocadoDocGeneralStringMessage("FitToPage",docId,path,needRepaint);

					//HandleAvocadoDocGeneralStringMessage("SaveAvocadoDocument",docId,path,needRepaint);
				}
			}
			if (!isStoring)
			{
				NotifyElementsChanged ();
				HandleAvocadoDocGeneralStringMessage("OnLoadAnimation",docId,path,needRepaint);
			}
			if (needRepaint)
			{
			//	doc->InvokePaintAll();	// updates all view modules and invoke repaint 
			}
	}
	

	bool AvocadoEngine::OnSendAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta)
	{
		bool ret = false;
		bool needRepaint = false;
	//	m_nestedMessageCount++;
		ret = HandleAvocadoMouseStringMessage(msg,viewId,x,y,zDelta,needRepaint);
	//	m_nestedMessageCount--;
	//	if (m_nestedMessageCount == 0 && m_nestedMessagePaintCount >0)
	//	{
	//		needRepaint = true;
	//	}
		if (needRepaint)
			InvokePaintView(viewId);
		return ret;
	}

	bool AvocadoEngine::OnSendAvocadoGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool toAllModules)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());
		bool ret = false;
		bool needRepaint = false;
		//m_nestedMessageCount++;
		ret = HandleAvocadoViewGeneralStringMessage(msg,viewId,paramStr,needRepaint);
		// trying to be smart and catch extra repaints..
		//m_nestedMessageCount--;
		//if (m_nestedMessageCount == 0 && m_nestedMessagePaintCount >0)
		//{
		//	needRepaint = true;
		//}
		if (needRepaint && msg!= std::string("OnPaint"))
			InvokePaintView(viewId);
		return ret;
	}
	
	bool AvocadoEngine::OnSendAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, string targetModule)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());
		bool ret = false;
		bool needRepaint = false;
		if (targetModule == "")
			ret = HandleAvocadoDocGeneralStringMessage(msg,docId,paramStr,needRepaint);
		else
			ret = HandleSpecificModuleMessage(msg,docId,paramStr,needRepaint,targetModule);

		if (needRepaint && msg!= std::string("OnPaint"))
			InvokePaintAll();
		return ret;
	}
}
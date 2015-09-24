#include "AvocadoAppInterface.h"

#include "AvocadoEngine.h"
#include <nvgl/SceneRendererGL2.h>
#include <nvgl/RenderTargetGLFBO.h>

/* Later on , we will have an engine creator, that can create few engines, some which are local ,some remote */
/* Loading of engine modules to the engine is supposed to be done here also */
namespace avocado 
{
	static AvocadoEngine *theEngine = NULL;

	double __stdcall GetEngineTimer ()
	{
		return theEngine->GetEngineTimer ();
	}
	bool __stdcall GetEngineOptionBool (std::string name, bool *oval)
	{
		bool val = false;
		if ( theEngine->GetAvocadoOption (name,&val,AvocadoOption::BOOL ) )
		{
			*oval = val;
			return true;
		}
		return false;
	}
	AVDLL   bool __stdcall GetEngineOptionStructure (AvocadoOptionStructure &op)
	{
		return theEngine->GetAvocadoOptionStructure (op);
	}

	bool __stdcall GetEngineOptionInt (std::string name, int *oval)
	{
		int val = 0;
		if ( theEngine->GetAvocadoOption (name,&val,AvocadoOption::INT ) )
		{
			*oval = val;
			return true;
		}
		return false;
	}
	bool  __stdcall GetEngineOptionFloat (std::string name, float *oval)
	{
		float val = 0.0f;
		if ( theEngine->GetAvocadoOption (name,&val,AvocadoOption::FLOAT ) )
		{
			*oval = val;
			return true;
		}
		return false;
	}
	bool  __stdcall GetEngineOptionString (std::string name, string *oval)
	{
		string val ("");
		if ( theEngine->GetAvocadoOption (name,&val,AvocadoOption::STRING ) )
		{
			*oval = val;
			return true;
		}
		return false;
	}


	void  __stdcall SetEngineOptionString (std::string name, string val)
	{
		//string val ("");
		bool res = theEngine->SetAvocadoOption (name,(void*)(&val),AvocadoOption::STRING );
		return;
	}
	void  __stdcall SetEngineOptionBool (std::string name, bool val)
	{
		bool res = theEngine->SetAvocadoOption (name,(void*)(&val),AvocadoOption::BOOL);
		return;
	}
	void  __stdcall SetEngineOptionInt(std::string name, int val)
	{
		bool res = theEngine->SetAvocadoOption (name,(void*)(&val),AvocadoOption::INT);
		return;
	}
	void  __stdcall SetEngineOptionFloat (std::string name, float val)
	{
		bool res = theEngine->SetAvocadoOption (name,(void*)(&val),AvocadoOption::FLOAT );
		return;
	}
	bool  __stdcall SaveOptionsFile ()
	{
		bool res = theEngine->AvocadoSaveOverideOptions ();
		return res;
	}

	static int engineRefCount = 0;
	void __stdcall AvocadoInit (bool multiThreded,string sessionFolder)
	{
		if (engineRefCount == 0)
		{
			theEngine = new AvocadoEngine ();
			theEngine->AvocadoInit(multiThreded,sessionFolder);
		}
		engineRefCount++;
	}

	void __stdcall AvocadoTerminate ()
	{
		engineRefCount--;
		if (engineRefCount == 0)
		{
			theEngine->AvocadoTerminate ();
			delete theEngine;
		}
	}

	int  __stdcall OnCreateDoc(AvocadoDocInterface *docInterface)
	{
		return theEngine->OnCreateDoc (docInterface);
	}

	void __stdcall DoCompressDoc (int docId, std::string &inpath,std::string &outpath,
		std::vector<std::string> attachments, 
		std::vector<std::string> embeddedModels,
		std::vector<std::string> embeddedTextures, 
		bool unzip)
	{
		theEngine->DoCompressDoc (docId,inpath,outpath, attachments,embeddedModels,embeddedTextures,unzip);
	}

	void __stdcall OnSerializeDoc (int docId, std::string &path, bool isImport, bool isStoring)
	{
		theEngine->OnSerializeDoc (docId,path,isImport,isStoring);
	}

	void __stdcall SetActiveDoc (int docId)
	{
		theEngine->SetActiveDoc (docId);
	}

	int __stdcall OnCreateView (void *phWnd, AvocadoViewInterface *viewInterface)
	{
		return theEngine->OnCreateView(phWnd,viewInterface);
	}

	int __stdcall OnCreateDCView (void *phWnd, AvocadoViewInterface *viewInterface)
	{
		return theEngine->OnCreateDCView(phWnd,viewInterface);
	}

	void __stdcall OnSizeView (int viewId, int px, int py)
	{
		theEngine->OnSizeView (viewId,px,py);
	}

	void __stdcall InvokePaintView (int viewId)	// invoke application repaint
	{
		theEngine->InvokePaintView (viewId);
	}
	void __stdcall NotifySelectionChanged (int viewId,std::vector<int>  eids)	// invoke application repaint
	{
		theEngine->NotifySelectionChanged (viewId ,eids);
	}
	void __stdcall NotifyElementsChanged ()	// invoke application repaint
	{
		theEngine->NotifyElementsChanged ();
	}
	void __stdcall InvokePaintAll ()	// invoke application repaint
	{
		theEngine->InvokePaintAll ();
	}
	void __stdcall RaiseAvocadoViewErrorMessage (int viewID,std::string errMsg)	// invoke application repaint
	{
		theEngine->RaiseAvocadoViewErrorMessage (viewID,errMsg);
	}
	void __stdcall RaiseAvocadoDocErrorMessage (int docID,std::string errMsg)	// invoke application repaint
	{
		theEngine->RaiseAvocadoDocErrorMessage (docID,errMsg);
	}
	bool __stdcall OnSendAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta)
	{
		return theEngine->OnSendAvocadoMouseStringMessage(msg, viewId, x, y, zDelta);
	}

	bool __stdcall OnSendAvocadoGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool toAllModules)
	{
		return theEngine->OnSendAvocadoGeneralStringMessage(msg,viewId,paramStr,toAllModules);
	}
	bool __stdcall OnSendAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, string targetModule)
	{
		return theEngine->OnSendAvocadoDocGeneralStringMessage(msg,docId,paramStr,targetModule);
	}
}
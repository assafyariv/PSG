/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#include "AvocadoEngineView.h"
#include "AvocadoScenixAdapter.h"
#include <nvgl/SceneRendererGL2.h>
#include <nvgl/RenderTargetGLFBO.h>
#include "AvocadoInternalModules.h"

namespace avocado
{
	// AvocadoEngineView

	AvocadoEngineView::AvocadoEngineView ()
	{
		m_nvsgViewData = new CNVSGViewData ();
	}

	AvocadoEngineView::~AvocadoEngineView ()
	{	 
		ClearViewModules (); 
		delete m_nvsgViewData;
	}

	CNVSGViewData *AvocadoEngineView::GetCNVSGViewData ()
	{
		return m_nvsgViewData;
	}

	bool AvocadoEngineView::HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint)
	{
		for (size_t i=0;i<m_modules.size();i++)
		{
			m_modules[i]->HandleAvocadoMouseStringMessage(msg,viewId,x,y,zDelta,needRepaint);
		}
		return true;
	}

	bool AvocadoEngineView::UpdateViewParam (std::string paramStr)
	{
		string srcname;
		ParamListSharedPtr pl = ParamList::createFromString(paramStr);
		if (pl->IsEmpty())
			return false;
		Param* p = pl->GetParam(0);
		ParamListSharedPtr viewParams = GetCNVSGViewData()->m_viewParams;
		p->GetName(srcname);
		Param *pdst = viewParams->GetParam (srcname);
		if (pdst)
		{
			string vdst,vsrc;
			((StringParam*)pdst)->GetValue(vdst);
			((StringParam*)p)->GetValue (vsrc);
			if (vdst!= vsrc)
			{
				((StringParam*)pdst)->SetValue (vsrc);
					NotifyViewParamChanged (paramStr);
				return true;
			}
		}
		else
		{
			string pval;
			((StringParam*)p)->GetName (srcname);
			((StringParam*)p)->GetValue (pval);
			viewParams->PushString (srcname,pval);
			NotifyViewParamChanged (paramStr);
			return true;
		}
		return false;
	}
	bool AvocadoEngineView::NotifyViewParamChanged (string paramStr)
	{
		string srcname,srcval;
		ParamListSharedPtr pl = ParamList::createFromString(paramStr);
		Param* p = pl->GetParam(0);
		p->GetName(srcname);
		p->GetValue(srcval);
		GetViewInterface()->ViewParamChanged (srcname.c_str(),srcval.c_str());
		return true;
	}
	bool AvocadoEngineView::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());

		bool ret = false;
		if (msg == "SetViewParam")
		{
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			if (pl->GetParam ("AntiAliasing"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("AntiAliasing");
				string val;
				ip->GetValue (val);
				if (val == string ("0"))
					GetCNVSGViewData ()->SetAntiAliasing (false);
				else
					GetCNVSGViewData ()->SetAntiAliasing (true);
			}
			UpdateViewParam (paramStr);
			{
				//NotifyViewParamChanged (paramStr);
			}
		} else
		if (msg == "SaveViewToFileOld")
		{
			m_nvsgViewData->SaveContextToFile (paramStr,256,256,false,false);
		} else
			if (msg == "SaveViewToFileFullOld")
		{
			m_nvsgViewData->SaveContextToFile (paramStr,3300,2600,false,true);// m_nvsgViewData->m_renderTargetGL->getWidth(),
				//m_nvsgViewData->m_renderTargetGL->getHeight());
		} else
		if (msg == "SaveViewToBitmapFileOld")
		{
			ParamListSharedPtr  ppl = ParamList::createFromString (paramStr);
			int height,width;
			string filename;
			bool forceOptix = true;
			ppl->PopBool (forceOptix);
			ppl->PopInt (height);
			ppl->PopInt (width);
			ppl->PopString (filename);

			m_nvsgViewData->SaveContextToFile (filename,width,height,true,false);//forceOptix);
		}
		for (size_t i=0;i<m_modules.size();i++)
		{
			// TODO : Send the message to all modules , no break. each module will decide wheather or not to implement its virtual handlers.
			/* later on , set priorites for each module */
			ret = m_modules[i]->HandleAvocadoViewGeneralStringMessage(msg,viewId,paramStr,needRepaint);
			if (ret)
				break; // Message was handled succsusfully 
		}
		return ret;
	}

	void
	AvocadoEngineView::AddViewModule (AvocadoViewModule *module)
	{
		m_modules.push_back (module);
		module->registerModule (m_nvsgViewData->GetFrontViewState(),m_nvsgViewData->GetViewState(),m_nvsgViewData->m_renderTargetGL,m_nvsgViewData->m_sceneRendererGL2,m_nvsgViewData->m_renderContextGL,m_nvsgViewData->m_sceneRendererRT,m_nvsgViewData->m_renderContextRT,GetID());
	}


	void
	AvocadoEngineView::ClearViewModules ()
	{
		for (size_t i=0;i<m_modules.size();i++)
		{	
			m_modules[i]->OnUnload ();
			if (m_modules[i]->m_name == "ManipulatorModule")
			{
				AvocadoManipulator*	 am = (AvocadoManipulator*)m_modules[i];
				delete am;
			} 
			else if (m_modules[i]->m_name == "AnimationManipulatorModule")
			{
				AvocadoAnimationManipulator*	 am = (AvocadoAnimationManipulator*)m_modules[i];
				delete am;
			}
			else if (m_modules[i]->m_name == "PipelineModule")
			{
				AvocadoPipeline*	 am = (AvocadoPipeline*)m_modules[i];
				delete am;
			}
			else if (m_modules[i]->m_name == "PickerModule")
			{
				AvocadoPicker*	 am = (AvocadoPicker*)m_modules[i];
				delete am;
			}
			else if (m_modules[i]->m_name == "DraggerModule")
			{
				AvocadoDragger*	 am = (AvocadoDragger*)m_modules[i];
				delete am;
			}
			m_modules[i] = NULL;
		}
		m_modules.clear ();
	}
}
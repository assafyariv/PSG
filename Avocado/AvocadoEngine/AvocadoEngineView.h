/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoEngineObject.h"
#include "AvocadoMessageHandler.h"
#include "AvocadoModuleInterface.h"

namespace avocado
{
	class CNVSGViewData;

	class AvocadoEngineView : public AvocadoEngineObject, public AvocadoMessageHandler
	{
	public:
		AvocadoEngineView() ;
		~AvocadoEngineView() ;
		AvocadoViewInterface				*GetViewInterface(){ return m_viewInterface; }
		void								SetViewInterface(AvocadoViewInterface *viewInterface){ m_viewInterface = viewInterface; }
		CNVSGViewData						*GetCNVSGViewData();
		void								AddViewModule (AvocadoViewModule * module);
		virtual bool						HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint);
		virtual bool						HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint);
		virtual bool						HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint) { return false; }	// unused
		void								ClearViewModules ();
		bool								UpdateViewParam (string paramStr);
		bool								NotifyViewParamChanged (string paramStr);
	private:
		CNVSGViewData						*m_nvsgViewData;
		AvocadoViewInterface				*m_viewInterface;
		std::vector<AvocadoViewModule*>		m_modules;
		
	};
}
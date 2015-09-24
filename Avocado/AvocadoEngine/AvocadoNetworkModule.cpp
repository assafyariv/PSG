/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoSOAPClient.h"
#include "AvocadoNetworkModule.h"
#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>

namespace avocado
{
	AvocadoNetwork::AvocadoNetwork () : AvocadoEngineModule ("NetworkModule")
	{
		
	}

	AvocadoNetwork::~AvocadoNetwork ()
	{
	}

	AvocadoEngineModule* AvocadoNetwork::create ()
	{
		 return ( (AvocadoEngineModule *) new AvocadoNetwork);
	}

	bool AvocadoNetwork::OnUnload()
	{
		return true;
	}
    bool AvocadoNetwork::HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) 
	{
		if (msg == "DownloadFile")
		{
			ParamListSharedPtr pl  = ParamList::createFromString (paramStr);
			std::string filename;
			pl->PopString (filename);
			AvocadoSoapClient as;
			as.DownloadFile (filename.c_str(),"c:\\Temp\\down.zip");
			return true;
		}
		return false;
	}
	bool AvocadoNetwork::OnRegister()
	{
		AvocadoSoapClient as;
		return true;
	}
}
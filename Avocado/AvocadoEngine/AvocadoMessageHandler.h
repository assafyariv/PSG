/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoAppInterface.h"
#include "AvocadoParams.h"
#include <string>

namespace avocado
{
	class AvocadoMessageHandler
	{
	public:
		AvocadoMessageHandler ();
		AvocadoMessageHandler (string name) { m_name = name ; }
		virtual ~AvocadoMessageHandler ();

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint) = 0;
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint) = 0;
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) = 0;

		string m_name;
	};
}
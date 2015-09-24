#pragma once
#include "AvocadoModuleInterface.h"

namespace avocado 
{
	class AvocadoNetwork : public AvocadoEngineModule 
	{
	public:
		AvocadoNetwork ();
		virtual ~AvocadoNetwork ();
		virtual AvocadoEngineModule* create ();
		bool OnUnload ();
		virtual bool OnRegister ();

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint){ return false; }			// unused
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint){ return false; }	// unused
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint); 		// unused
	};
}
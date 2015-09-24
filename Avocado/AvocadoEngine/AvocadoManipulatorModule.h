#pragma once
#include "AvocadoModuleInterface.h"
#include "AvocadoManipulator.h"



namespace avocado 
{
class AvocadoManipulator : public AvocadoViewModule
	{
	public:
		AvocadoManipulator ();
		virtual ~AvocadoManipulator ();
		virtual AvocadoViewModule* create ();

		bool OnUnload ();
		virtual bool OnRegister ();

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint);
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const  std::string &paramStr, bool &needRepaint);//{ return false; }	// unused
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) { return false; }	// unused

		void ResetMan();
		float getETime();

		bool OnMouseMDown(int x, int y, bool &needRepaint);
		bool OnMouseMUp();
		bool OnMouseMove(int x, int y, bool &needRepaint);
		bool OnMouseWheel(short zDelta , int x, int y, bool &needRepaint);

		CylindricalCameraManipulator	*m_man;
		bool							m_mouseDown;
		double							m_lastTime;
		int								m_wheel;
		bool							m_shiftState;
		bool							m_ctrlState;
		int								m_manipulatiomMode;
	};
}
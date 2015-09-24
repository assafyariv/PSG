#pragma once
#include "AvocadoAppInterface.h"
#include "AvocadoModuleInterface.h"
#include "AnimationManipulator.h"
#include "CameraAnimator.h"
#include <nvutil/SmartPtr.h>

namespace avocado 
{
	class AvocadoAnimationManipulator : public AvocadoViewModule 
	{
	public:
		AvocadoAnimationManipulator ();
		virtual ~AvocadoAnimationManipulator ();
		virtual AvocadoViewModule* create ();

		virtual bool OnUnload ();
		virtual bool OnRegister ();

		void LoadAnimation();
		void SetAnimationStep(int step,bool &needRepaint);
			void SetVSAnimationStep(int step,bool &needRepaint);
		//void ToggleLoopMode();
		void Start();
		void Stop();
		void reachedAnimationEnd();

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint); //{ return false; }	// unused
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const  std::string &paramStr, bool &needRepaint);
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) { return false; }		// unused
		void OnCamAnimationEnd ();
		void OnPartAnimationEnd ();
		/*nvutil::SmartPtr<*/AnimationManipulator * /*>*/	m_animMan; 
		std::vector<nvsg::AnimationStateSharedPtr>  m_animationStates;
		std::vector<nvsg::AnimationStateSharedPtr>  m_vsAnimationStates;
		int m_step;
		bool m_timerState;
		CameraAnimator m_camAnimator;
		int m_vsstep;
		bool m_partsFinished;
		bool m_cameraFinished;
	};

}
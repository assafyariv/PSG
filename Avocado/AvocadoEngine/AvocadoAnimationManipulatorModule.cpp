/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoAnimationManipulatorModule.h"
#include <nvtraverser\SearchTraverser.h>
#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvutil/SmartPtr.h>
#include "CameraAnimator.h"
#include <nvsg/FrustumCamera.h>
#include <nvsg/PerspectiveCamera.h>
#include "SceneFunctions.h"

namespace avocado
{
	AvocadoAnimationManipulator::AvocadoAnimationManipulator (): AvocadoViewModule ("AnimationManipulatorModule")
	{
		m_timerState = false;
		m_cameraFinished = false;
		m_partsFinished = false;
	}

	AvocadoAnimationManipulator::~AvocadoAnimationManipulator ()
	{
		
	}

	AvocadoViewModule* AvocadoAnimationManipulator::create ()
	{
		return ( (AvocadoViewModule *) new AvocadoAnimationManipulator );
	}

	bool AvocadoAnimationManipulator::OnUnload ()
	{
		//if (m_camAnimator)
		{
			if (m_camAnimator.m_finishCallBack != 0)
			{
				delete m_camAnimator.m_finishCallBack;
			}
		}
		m_animMan->reset();
		delete m_animMan;
		return true;
	}

	class AnimationModuleFinishCallBack : public CameraAnimatorFinishCallBack
	{
	public:
		AnimationModuleFinishCallBack (AvocadoAnimationManipulator* module ) { m_module = module;}
		~AnimationModuleFinishCallBack () {}
	
		virtual void call () { m_module->OnCamAnimationEnd ();}
	private:
		AvocadoAnimationManipulator *m_module;
	};
	void AvocadoAnimationManipulator::OnCamAnimationEnd()
	{
		
		//if (m_vsAnimationStates.size()==0)
		{
			m_cameraFinished = true;
		}

//		m_vsAnimationStates.clear();
		//m_vsstep = 100;
		if (m_cameraFinished && m_vsAnimationStates.size()==0)
		{
			OnSendAvocadoDocGeneralStringMessage("NotifyCamAnimationEnd",0, "");
			m_cameraFinished = false;
			m_partsFinished = false;
		}
	}
	void AvocadoAnimationManipulator::OnPartAnimationEnd()
	{
		
		if (m_vsAnimationStates.size()==0)
		{
			m_partsFinished = true;
		}
		if (m_cameraFinished && m_partsFinished)
		{
			OnSendAvocadoDocGeneralStringMessage("NotifyCamAnimationEnd",0, "");
			m_cameraFinished = false;
			m_partsFinished = false;
		}
	}

	bool AvocadoAnimationManipulator::OnRegister ()
	{
		m_animMan = new AnimationManipulator ();
		m_animMan->setRenderTarget (m_renderTarget);
		m_animMan->setViewState (m_viewState);
		m_animMan->setAnimationFramesPerSecond(0.0);
		m_camAnimator.setViewState (m_viewState);
		AnimationModuleFinishCallBack* cb = new AnimationModuleFinishCallBack (this);
		m_camAnimator.m_finishCallBack = cb; 
		return true;
	}

	void AvocadoAnimationManipulator::LoadAnimation()
	{
		m_animationStates.clear();
		m_vsAnimationStates.clear();
		// get all the animation states from root
		nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
		st->setClassName("class nvsg::AnimationState");
		st->setBaseClassSearch(true);
		st->apply(nvsg::SceneReadLock((nvsg::ViewStateReadLock(m_viewState))->getScene())->getRootNode() );

		const std::vector<nvsg::ObjectWeakPtr> & vp = st->getResults();
		m_animationStates.reserve( vp.size() );
		for ( size_t i=0 ; i<vp.size() ; i++ )
		{
			NVSG_ASSERT( dynamic_cast<AnimationStateWeakPtr>(vp[i]) );
			nvsg::AnimationStateSharedPtr aniState  (static_cast<nvsg::AnimationStateWeakPtr>(vp[i]));
			if (AnimationStateReadLock(aniState)->getName ()!= string ("AvocadoViewStateAnimation"))
			    m_animationStates.push_back( static_cast<nvsg::AnimationStateWeakPtr>(vp[i]) );
			else
				m_vsAnimationStates.push_back (static_cast<nvsg::AnimationStateWeakPtr>(vp[i]));
		}
		m_vsstep = 1;
	}

	void AvocadoAnimationManipulator::SetAnimationStep(int step,bool &needRepaint)
	{
		for ( size_t i=0 ; i<m_animationStates.size() ; i++ )
		{
			needRepaint= nvsg::AnimationStateWriteLock(m_animationStates[i])->setCurrentFrame( step );
		}
	}
	
	void AvocadoAnimationManipulator::SetVSAnimationStep(int step,bool &needRepaint)
	{
		size_t stoppedCount = 0;
		for ( size_t i=0 ; i<m_vsAnimationStates.size() ; i++ )
		{
			needRepaint= nvsg::AnimationStateWriteLock(m_vsAnimationStates[i])->setCurrentFrame( step );
			if  ( (!(nvsg::AnimationStateWriteLock(m_vsAnimationStates[i])->isRunning())) || 
				(step > 60))
				stoppedCount++;
		}
		if (stoppedCount == m_vsAnimationStates.size())
		{
			m_vsAnimationStates.clear();
			OnPartAnimationEnd ();
		}
	}
	bool AvocadoAnimationManipulator::HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint)
	{
		bool ret = false;

		switch (msg)
		{
			case AVC_TIMER_TICK:
			{
				needRepaint = m_camAnimator.timerEvent ();
				if (m_timerState)
				{
					SetAnimationStep(m_step++,needRepaint);
				}
				if (m_vsAnimationStates.size()>0)
				{
					SetVSAnimationStep(m_vsstep++,needRepaint);
				}
				if (m_timerState || m_vsAnimationStates.size()>0)
				{
					if (needRepaint)
							OnSendAvocadoGeneralStringMessage("NotifyDocElementMove", m_viewId,"");
					
					needRepaint = true;
				}
				break;
			}
		}
		return ret;
	}
	bool AvocadoAnimationManipulator::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool &needRepaint)
	{
		bool ret = false;
		if (msg == "OrientCamera")
		{
			// here we get a sphere and set camera to sphere.
			ParamListSharedPtr pls = ParamList::createFromString(paramStr);
			float f1,f2,f3,f4;
			pls->PopFloat (f1);
			pls->PopFloat (f2);
			pls->PopFloat (f3);
			pls->PopFloat (f4);
			m_camAnimator.zoomTo (f3,f2,f1,f4);
			ret = true;
		}
		else if (msg == "ViewCamera_Orient")
		{
		
				SceneWriteLock swr (ViewStateWriteLock(m_viewState)->getScene());
				nvmath::Sphere3f sph = 	NodeWriteLock (swr->getRootNode ())->getBoundingSphere ();
		

			// here we get a sphere and set camera to sphere.
			//ParamListSharedPtr pls = ParamList::createFromString(paramStr);
			float f1,f2,f3,f4;
			float q1 = 0.0f;
			float q2 = 0.0f;
			float q3= 0.0f;
			float q4=0.0f;
			/*f1 = (sph.getCenter())[0];
			f2 = (sph.getCenter())[1];
			f3 = (sph.getCenter())[2];*/
			nvmath::Vec3f cent = sph.getCenter ();
			f1 = cent[0];
			f2 = cent[1];
			f3 = cent[2];
			f4 = sph.getRadius ();
			
			if (paramStr == "Right")
			{
					//nvmath::Quatf qq = nvmath::Quatf 
						nvmath::Quatf qq = nvmath::Quatf(nvmath::Vec3f(0.0f,1.0f,0.0f),-0.5f*3.14f);
						//	(0.0,1.0,0.0,0.0) * nvmath::Quatf (0.0,1.0,0.0,0.0)
							//* nvmath::Quatf (0.0,sqrt(0.5),0.0,sqrt(0.5));
							q1 = qq[0];
								q2 = qq[1];//1.0f;
								q3 = qq[2];//0.0f;
								q4 = qq[3];//1.0f;

			}
			else
				if (paramStr == "Back")
				{
				q1 = 0.0f;
				q2 = 1.0f;
				q3 = 0.0f;
				q4 = 0.0f;
				
				}
				else if (paramStr == "Top")
				{
			nvmath::Quatf qqq (0.0f,1.0f,0.0f,1.0f);
					qqq.normalize();
				q1 = qqq[0];
				q2 = qqq[1];
				q3 = qqq[2];
				q4 = qqq[3];

				}
				else if (paramStr == "Bottom")
				{
					nvmath::Quatf qqq (0.0,1.0f,sqrt(0.5f),1.0f);
					qqq.normalize();
				q1 = qqq[0];
				q2 = qqq[1];
				q3 = qqq[2];
				q4 = qqq[3];

				}
				else
					if (paramStr == "Left")
					{
							 
						nvmath::Quatf qq = nvmath::Quatf(nvmath::Vec3f(0.0f,-1.0f,0.0f),0.5f*3.14f);
						qq.normalize();
							q1 = qq[0]; 
								q2 = qq[1];//1.0f;
								q3 = qq[2];//0.0f;
								q4 = qq[3];//1.0f;

					}
					else if (paramStr == "Front")
					{
						nvmath::Quatf qq = nvmath::Quatf 
							(0.0,1.0,0.0,0.0) * nvmath::Quatf (0.0,1.0,0.0,0.0);
							q1 = qq[0];
								q2 = qq[1];//1.0f;
								q3 = qq[2];//0.0f;
								q4 = qq[3];//1.0f;

					}
			//pls->PopFloat (q1);
			//pls->PopFloat (q2);
			//pls->PopFloat (q3);
			//pls->PopFloat (q4);

			//pls->PopFloat (f1);
			//pls->PopFloat (f2);
			//pls->PopFloat (f3);
			//pls->PopFloat (f4);
		//	FrustumCameraWriteLock (ViewStateWriteLock (m_viewState)->getCamera())->setDirection (nvmath::Vec3f (q1,q2,q3));
			m_camAnimator.zoomToWithDirection (f1,f2,f3,f4,q1,q2,q3,q4);
			ret = true;
		}
		else if (msg == "MoveToCamera")
		{
			ParamListSharedPtr pls = ParamList::createFromString(paramStr);
			float ff[16];
			for (int i=15;i>=0;i--)
				pls->PopFloat (ff[i]);
			nvmath::Mat44f mat (ff[0],ff[1],ff[2],ff[3],ff[4],ff[5],ff[6],ff[7],ff[8],
								ff[9],ff[10],ff[11],ff[12],ff[13],ff[14],ff[15]);

			FrustumCameraSharedPtr targetCam = nvsg::PerspectiveCamera::create ();
			FrustumCameraWriteLock (targetCam)->setWorldToViewMatrix (mat);
			PerspectiveCameraWriteLock (targetCam)->setFieldOfView (AVOCADO_DEFAULT_FOV);
			m_camAnimator.moveToCamera (targetCam.get());
		}
		if (msg == "FitToPage")
		{
			m_camAnimator.zoomAll ();
			ret = true;
		} 
		else if (msg == "OnLoadAnimation")
		{
			LoadAnimation();
			//needRepaint = true;
			ret = true;
		}
		else
		if (msg == "OnPlayAnimation")
		{
			m_step = 1;
			m_timerState = true;
			ret = true;
			//double step = GetEngineTimer ();
/*
			while (m_animMan->nextFrame())
			m_animMan->
			m_animationStates
*/
		}
		else
		if (msg == "OnStopAnimation")
		{
			m_step = 0;
			m_timerState = false;
			ret = true;
		}
		else
		if (msg == "OnPauseAnimation")
		{
			m_timerState = !m_timerState;
			ret = true;
		}

		return ret;
	}
}
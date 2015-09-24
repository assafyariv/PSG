/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoManipulatorModule.h"
#include <nvgl/RenderContextGL.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include "SceneFunctions.h"
#include <nvsg/PerspectiveCamera.h>

namespace avocado
{
	AvocadoManipulator::AvocadoManipulator (): AvocadoViewModule ("ManipulatorModule")
	{
		m_manipulatiomMode = MANIPILATION_MODE_CAD;
	}

	AvocadoManipulator::~AvocadoManipulator ()
	{
	}

	AvocadoViewModule* AvocadoManipulator::create ()
	{
		return ( (AvocadoViewModule *) new AvocadoManipulator );
	}

	bool AvocadoManipulator::OnUnload ()
	{
		if (m_man)
		{
			ResetMan ();
			delete m_man;
		}
		return true;
	}

	bool AvocadoManipulator::OnRegister ()
	{
		m_man = new CylindricalCameraManipulator ();
		m_man->setRenderTarget (m_renderTarget);
		m_man->setViewState (m_viewState);
		m_mouseDown = false;
		ResetMan ();
		return true;
	}

	bool AvocadoManipulator::HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint)
	{

		bool ret = false;

		switch (msg)
		{
			case AVC_MOUSE_LDOWN:
			{
				if (m_manipulatiomMode == MANIPILATION_MODE_USER)
				{
					m_shiftState = ((zDelta & MK_SHIFT) == MK_SHIFT);
					m_ctrlState = ((zDelta & MK_CONTROL) == MK_CONTROL);
					ret = OnMouseMDown (x,y,needRepaint);
				}
				break;
			}
			case AVC_MOUSE_LUP:
			{
				if (m_manipulatiomMode == MANIPILATION_MODE_USER)
					ret = OnMouseMUp ();
					break;
			}
		case AVC_MOUSE_MDOWN:
		{
			if (m_manipulatiomMode == MANIPILATION_MODE_CAD)
			{
				m_shiftState = ((zDelta & MK_SHIFT) == MK_SHIFT);
				m_ctrlState = ((zDelta & MK_CONTROL) == MK_CONTROL);
			}
			else
			{
				m_shiftState = true;
			}
			ret = OnMouseMDown (x,y,needRepaint);
			break;
		}
		case AVC_MOUSE_MUP:
			{
				//if (m_manipulatiomMode == MANIPILATION_MODE_CAD)
					ret = OnMouseMUp ();
				break;
			}
		case AVC_MOUSE_MOVE:
			{
				if (m_manipulatiomMode != MANIPILATION_MODE_USER)
				{
					if (!(zDelta & AVC_MK_MBUTTON))
					{
						m_mouseDown = false;
					//	break;
					}
				} else
				{
					if (!(zDelta & AVC_MK_LBUTTON) && !(zDelta & AVC_MK_MBUTTON))
					{
						m_mouseDown = false;
					//	break;
					}
				}
				ret = OnMouseMove (x, y, needRepaint);
				break;
			}
		case AVC_MOUSE_WHEEL:
			{
				ret = OnMouseWheel (zDelta,x,y,needRepaint);
				break;
			}
		};

		return ret;
	}

	bool AvocadoManipulator::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string ("Module: " + m_name + string(" - ") + msg + string(" (")+paramStr+ string(")\n")).c_str());

		bool ret = false;
		if (msg == "OrientCamera")
		{
#if 0
			// here we get a sphere and set camera to sphere.
			ParamListSharedPtr pls = ParamList::createFromString(paramStr);
			float f1,f2,f3,f4;
			pls->PopFloat (f1);
			pls->PopFloat (f2);
			pls->PopFloat (f3);
			pls->PopFloat (f4);
			nvmath::Sphere3f zp;
			zp.setCenter (nvmath::Vec3f (f3,f2,f1));
			zp.setRadius (f4);
			CameraSharedPtr cam = ViewStateWriteLock (m_viewState)->getCamera ();
			PerspectiveCameraWriteLock lk (cam);
			lk->zoom (zp);
			needRepaint = true;
			ret = true;
#endif
		} else
		if (msg == "FitToPage")
		{
#if 0
			SceneSharedPtr scene = ViewStateReadLock(m_viewState)->getScene();
			nvutil::setupDefaultViewState(scene,m_viewState);
			//m_man->setCursorPosition(nvmath::Vec2i(0,0));
			//m_man->setPivot();
			ret = true;
			needRepaint = true;
#endif
		}
		else if ( msg == "SetViewParam")
		{
			string srcname;
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			if (pl->IsEmpty())
				return false;
			Param* p = pl->GetParam(0);
					string pval;
			((StringParam*)p)->GetName (srcname);
			((StringParam*)p)->GetValue (pval);
			if (srcname == "ManipulationMode")
			{
				if (pval == "1")
				{
					m_manipulatiomMode = MANIPILATION_MODE_CAD;
				} else if (pval == "2")
				{
					m_manipulatiomMode = MANIPILATION_MODE_USER;
				}
				else if (pval == "3")
				{
					m_manipulatiomMode = MANIPILATION_MODE_FLY;
				}
			}
		}
		return ret;
	}
	bool AvocadoManipulator::OnMouseMDown (int x, int y, bool &needRepaint)
	{
		m_man->setCursorPosition (nvmath::Vec2i (x, y));
		m_man->setSpincenter();
		m_mouseDown = true;
		OnSendAvocadoGeneralStringMessage("ManipulationStart",m_viewId,"");
		return true;
	} 

	bool AvocadoManipulator::OnMouseMUp ()
	{
		OnSendAvocadoGeneralStringMessage("ManipulationEnd",m_viewId,"");
		m_shiftState = false;
		m_ctrlState = false;
		m_mouseDown = false;
		return true;
	}

	bool AvocadoManipulator::OnMouseMove (int x, int y, bool &needRepaint)
	{
		if (!m_shiftState && !m_ctrlState)
		{
		   m_man->setMode (CylindricalCameraManipulator::MODE_ORBIT);
		}
		else if (m_shiftState)
		{ 
			m_man->setMode (CylindricalCameraManipulator::MODE_PAN);
		} else //if (_ctrlState)
		{
			m_man->setMode (CylindricalCameraManipulator::MODE_ROTATE_XY);
		}
		m_man->setCursorPosition (nvmath::Vec2i (x, y));
		
		if (m_mouseDown && m_man->updateFrame ( getETime() ))
		{
			OnSendAvocadoGeneralStringMessage("CameraChanged",m_viewId,"");
			needRepaint  = true;
		}
		return true;
	}

	bool AvocadoManipulator::OnMouseWheel (short zDelta , int x, int y, bool &needRepaint)
	{
		m_man->setMode(CylindricalCameraManipulator::MODE_ZOOM_FOV);
		if (zDelta != 0)
		{
			// Important not to divide by zero !
			m_wheel += 120 *(zDelta/abs(zDelta));
		}
		m_man->setWheelTicks (m_wheel);

		while (m_man->updateFrame( getETime() ))
		{
			needRepaint  = true;
			int frame = 0;
			while ( frame < 2 && (m_man->updateFrame( getETime() )) )
			{
				needRepaint  = true;
				frame++;
			}
	
			m_man->reset ();

			m_man->setWheelTicks (0);
			m_wheel = 0;
			if ( m_man->updateFrame( getETime() ) )
				needRepaint  = true;
		}
		if (needRepaint)
			OnSendAvocadoGeneralStringMessage("CameraChanged",m_viewId,"");
		return true;
	}

	void AvocadoManipulator::ResetMan ()
	{
		m_wheel = 0;
		m_man->reset ();
		m_man->setMode ( CylindricalCameraManipulator::MODE_ORBIT );
		m_man->setPivot ();
	}

	float AvocadoManipulator::getETime () 
	{
		double curTime = GetEngineTimer ();
		double diffTime = curTime - m_lastTime;

		m_lastTime = curTime;

		// gettime returns time in ms.  convert to s.
		diffTime /= 1000.0;

		return static_cast<float> ( diffTime ); 
	}
}

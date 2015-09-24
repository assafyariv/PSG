/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoPickerModule.h"
#include "AvocadoEngineObject.h"
#include <nvgl/RenderContextGL.h>
#include <nvtraverser/SearchTraverser.h>
#include <nvtraverser/RayIntersectTraverser.h>

#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvsg/FrustumCamera.h>
#include <nvsg/Transform.h>
//#include "nvutil/DbgNew.h" // this must be the last include

using namespace avocado;

namespace avocado
{
	AvocadoPicker::AvocadoPicker (): AvocadoViewModule ("PickerModule")
	{
		m_ticks =0;
		m_wait = false;
		m_lastX = 0;
		m_lastY = 0;
		m_prePickEnabled = true;
		m_preSelectionViewParam = false;
		m_manipulatiomMode = MANIPILATION_MODE_CAD;
	}

	AvocadoPicker::~AvocadoPicker ()
	{
	}

	AvocadoViewModule* AvocadoPicker::create ()
	{
		return ( (AvocadoViewModule *) new AvocadoPicker );
	}

	bool AvocadoPicker::OnUnload ()
	{
		return true;
	}

	bool AvocadoPicker::OnRegister ()
	{
		return true;
	}
	bool AvocadoPicker::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int docId,const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string ("Module: " + m_name + string(" - ") + msg + string(" (")+paramStr+ string(")\n")).c_str());

		bool ret = false;

		if (msg == "ManipulationStart")
		{
			m_prePickEnabled = false;
		} 
		else  if (msg == "ManipulationEnd")
		{
			m_prePickEnabled = true;
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
			if (srcname == "PreSelectionEnabled")
			{
				if (pval == "1")
				{
					m_preSelectionViewParam = true;
				} else
				{
					m_preSelectionViewParam = false;
				}
			} else if (srcname ==  "ManipulationMode")
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
	bool AvocadoPicker::HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint)
	{
		bool ret = false;

		switch (msg)
		{
		case AVC_TIMER_TICK:
		{
			if (!m_preSelectionViewParam)
				break;
			if (!m_prePickEnabled)
				break;
			m_ticks ++;
			
			if (m_wait && m_ticks> 5)
			{
				m_wait =false;
				m_ticks = 0;
				m_prePick = true;
				bool n;
			   OnMouseLDown (m_lastShift,m_lastX, m_lastY, n);
			}
			if (m_ticks > 10000)
				m_ticks = 0;
			break;
		}
		case AVC_MOUSE_LDOWN:
			{
				if (m_manipulatiomMode == MANIPILATION_MODE_CAD)
				{
				m_wait = false; // Not waiting for pre-pick now.
				// Force pre-pick to ensure that pre-pick is handled before pick.
				m_prePick = true;
				m_lastShift = zDelta;
				ret = OnMouseLDown (m_lastShift,x, y, needRepaint);
				m_prePick = false;
				ret = OnMouseLDown (m_lastShift,x, y, needRepaint);
				}
				break;

			}
			case AVC_MOUSE_RUP:
			{
				// Right mouse up is a picking in USER MODE only. and equals to left mouse down.
				if (m_manipulatiomMode == MANIPILATION_MODE_USER)
				{
				m_wait = false; // Not waiting for pre-pick now.
				// Force pre-pick to ensure that pre-pick is handled before pick.
				m_prePick = true;
				m_lastShift = zDelta;
				ret = OnMouseLDown (m_lastShift,x, y, needRepaint);
				m_prePick = false;
				ret = OnMouseLDown (m_lastShift,x, y, needRepaint);
				ret = true; // return true always for performance.
				}
				break;
			}
		case AVC_MOUSE_MOVE:
			{
				//ret = true;
				if (!m_preSelectionViewParam)
					break;
				if (!m_prePickEnabled)
				break;
				
				if (m_ticks == 0 || m_ticks > 50)
				{
					m_prePick = true;
				
				   ret = OnMouseLDown (zDelta,x, y, needRepaint);


					m_ticks = 0;
					m_wait = false;
				} else 
					{
						m_lastX = x;
					    m_lastY = y;
						m_wait = true;
						
				};
				

				break;
			}
		};
		return ret;
	}
	
	void AvocadoPicker::UnPick ()
	{
		ParamListSharedPtr pl = ParamList::createNew();
		ParamListWriteLock(pl)->PushBool ("prepick",m_prePick);
		ParamListWriteLock(pl)->PushInt ("vid",m_viewId);
		OnSendAvocadoDocGeneralStringMessage("OnPickNothing", m_parentDocId, ParamListWriteLock(pl)->SerializeList());		
	}

	void AvocadoPicker::Pick (int shiftstate, const	nvtraverser::Intersection &result)
	{	
		bool fromView = false;
		bool mainView = false;
		int viewElementID,elementId;
		const Path *pa = result.getPath ();
		DrawableSharedPtr drw(result.getDrawable());
		
		AvocadoEngineObject *el = 0;
		for (unsigned int i=0;i<pa->getLength();i++)
		{
			ObjectWeakPtr obj (pa->getFromHead (i));
			GroupWeakPtr parent = dynamic_cast< GroupWeakPtr >( obj );
			if (parent)
			{
				std::string name = GroupReadLock (parent)->getName();
				if (name == "AvocadoElement") {
					el = (AvocadoEngineDocElement*)(GroupReadLock (parent)->getUserData());
					if (el)
					{
						mainView = true;
						elementId = el->GetID ();
						((AvocadoEngineDocElement*)el)->m_subDrawable = drw;
					//elementType  = el->GetType();
					//m_currentElement = parent;
					break;
					} 
				} 
				else if (name == "AvocadoViewElement") {
					el = (AvocadoEngineViewElement*)(GroupReadLock (parent)->getUserData());
					if (el)
					{
						fromView = true;
						viewElementID = el->GetID ();
					//	elementType  = el->GetType();
						//m_currentElement = parent;
						break;
					} 
				} 
			}
		}

		if (fromView || mainView)
		{
			if (!fromView)
			{
				// windows specific inside engine.. bad stuff
				bool multi = ((shiftstate & MK_SHIFT) == MK_SHIFT);
				m_parentDocId = ((AvocadoEngineDocElement*)el)->m_docId;
				string owner =  ((AvocadoEngineDocElement*)el)->m_ownerModule;
				ParamListSharedPtr pl = ParamList::createNew();
				ParamListWriteLock(pl)->PushString("owner",owner);
				ParamListWriteLock(pl)->PushBool("prepick",m_prePick);
				ParamListWriteLock(pl)->PushInt("eid",elementId);
				ParamListWriteLock(pl)->PushInt ("vid",m_viewId);
				ParamListWriteLock(pl)->PushBool ("multi",multi);
				OnSendAvocadoDocGeneralStringMessage("OnPick", m_parentDocId, ParamListWriteLock(pl)->SerializeList());
			} else
			{
				ParamListSharedPtr pl = ParamList::createNew();
				ParamListWriteLock(pl)->PushBool("prepick",m_prePick);
				ParamListWriteLock(pl)->PushInt("eid",viewElementID);
				OnSendAvocadoGeneralStringMessage("OnViewPick", m_viewId, ParamListWriteLock(pl)->SerializeList());
			}
		}
	}

	bool AvocadoPicker::OnMouseLDown (int shiftstate,int x, int y, bool &needRepaint)
	{
		nvtraverser::Intersection result;
		bool front = false;
		if (  RayPick (x,y,result,front) )
		{	
			Pick (shiftstate,result);
			//needRepaint = true;
			return true;
		} 
		else
		{
			//needRepaint = true;
			UnPick ();
		}
		return false;
	}

	bool AvocadoPicker::RayPick (
								unsigned int screenX, unsigned int screenY,
								nvtraverser::Intersection & result , bool &frontLayer)
	{
		SceneSharedPtr layer0scene = ViewStateReadLock(m_frontViewState)->getScene();
		SceneSharedPtr doc_scene = ViewStateReadLock(m_viewState)->getScene();

		if (intersectObjectt (SceneReadLock(layer0scene)->getRootNode(),screenX,screenY,result,m_frontViewState ) )
		{
			frontLayer = true;
			return true;
		}
		if (intersectObjectt (SceneReadLock(doc_scene)->getRootNode(),screenX,screenY,result,m_viewState ) )
		{
			frontLayer = false;
			return true;
		}
		return false;
	}
	bool AvocadoPicker::intersectObjectt ( const NodeSharedPtr & baseSearch,
								unsigned int screenX, unsigned int screenY,
								nvtraverser::Intersection & result, const ViewStateSharedPtr &vs)
	{
		std::vector<ClipPlaneSharedPtr> cpVec;
		{
			Group::ClipPlaneConstIterator cit = GroupReadLock(baseSearch)->beginClipPlanes();
			ClipPlaneSharedPtr cp = 0;
			// Here we traverse all the clipping planes and create a list of all the enabled planes.
			// Later on , at the end of this function we set all the stored planes back to enable.
			while (cit != GroupReadLock(baseSearch)->endClipPlanes() && (*cit) != 0)
			{
				cp = (*cit);
				if (ClipPlaneWriteLock(cp)->isEnabled ())
				{
				    ClipPlaneWriteLock(cp)->setEnabled(false);
					cpVec.push_back (cp);
				}
				cit.operator++();
			}
		} 
		//GroupWriteLock(baseSearch)->clearClipPlanes();
		ViewStateWriteLock viewState( vs );
		// requires a camera attached to the ViewState
		if (isPtrTo<FrustumCamera>(viewState->getCamera()) )
		{
			FrustumCameraSharedPtr pCam(nvutil::sharedPtr_cast<FrustumCamera>(viewState->getCamera()) );

			// calculate ray origin and direction from the input point
			nvmath::Vec3f rayOrigin;
			nvmath::Vec3f rayDir;

			int y =  m_renderTarget->getHeight() - 1 - screenY; // adjust to bottom left origin
			//FrustumCameraReadLock((nvutil::ReadableObject)viewState->getCamera());
		
			FrustumCameraReadLock(pCam)->getPickRay(screenX, y,m_renderTarget->getWidth(), m_renderTarget->getHeight(), rayOrigin, rayDir);

			// run the intersect traverser for intersections with the given ray
			nvutil::SmartPtr<nvtraverser::RayIntersectTraverser> pick( new nvtraverser::RayIntersectTraverser );
			nvtraverser::RayIntersectTraverser * picker = pick.get();
			picker->setCamClipping (false);
			rayDir.normalize ();
			picker->setRay(rayOrigin, rayDir);
			picker->setViewState(vs);
			
			picker->setViewportSize( m_renderTarget->getWidth(),  m_renderTarget->getHeight() );
			//?picker->setCamClipping(false);
			picker->apply( baseSearch );
			//if (cp)
		     //	GroupWriteLock(baseSearch)->addClipPlane(cp);
		// restore clip planes..
			{
					//Group::ClipPlaneConstIterator cit = GroupReadLock(baseSearch)->beginClipPlanes();
					ClipPlaneSharedPtr cp = 0;
				//while (cit != GroupReadLock(baseSearch)->endClipPlanes() && (*cit) != 0)
				for (size_t kcp = 0 ;kcp<cpVec.size();kcp++)
				{
					cp = cpVec[kcp];
					ClipPlaneWriteLock(cp)->setEnabled(true);
					//cit.operator++();
				}
			}
		// end clip planes.
			
			if (picker->getNumberOfIntersections() > 0)
			{ 
				result = picker->getNearest();
				
				return true;
			}
			
		}

		return false;
	}
}
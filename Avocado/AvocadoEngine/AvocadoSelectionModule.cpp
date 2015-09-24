/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoSelectionModule.h"
#include "SceneFunctions.h"
#include "AvocadoParams.h"
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/ScenerendererGL2.h>
#include <nvtraverser\SearchTraverser.h>

namespace avocado
{

	AvocadoSelection::AvocadoSelection (): AvocadoDocModule ("SelectionModule")
	{
	
	}

	AvocadoSelection::~AvocadoSelection ()
	{
		//ClearDocFileElements();
	}

	AvocadoDocModule* AvocadoSelection::create ()
	{
		 return ( (AvocadoDocModule *) new AvocadoSelection);
	}

	bool AvocadoSelection::OnUnload()
	{
		return true;
	}

	bool AvocadoSelection::OnRegister()
	{
		return true;
	}

	bool AvocadoSelection::HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string ("Module: " + m_name + string(" - ") + msg + string(" (")+paramStr+ string(")\n")).c_str());

		bool ret = false;

		if (msg == "OnPick")
		{
			ret = true;
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int vid,eid;
			bool prepick;
			string owner;
			bool multi=false;
			
			ParamListWriteLock(pl)->PopBool(multi);
			ParamListWriteLock(pl)->PopInt(vid);
			ParamListWriteLock(pl)->PopInt(eid);
			ParamListWriteLock(pl)->PopBool(prepick);
			ParamListWriteLock(pl)->PopString(owner);

			if (prepick)
			{
					ParamListSharedPtr ppl = ParamList::createNew();
					ParamListWriteLock(ppl)->PushInt("eid",eid);
					ParamListWriteLock(ppl)->PushInt("vid",vid);
					OnSendAvocadoDocGeneralStringMessage("MouseOverElement", docId, ParamListWriteLock(ppl)->SerializeList(),owner);
					return true;
			}
			if (InSelectionList(eid,vid) &&  SelectionCountPerView (vid) == 1)
			{
					ParamListSharedPtr ppl = ParamList::createNew();
					ParamListWriteLock(ppl)->PushInt("secondary",eid);
					ParamListWriteLock(ppl)->PushInt("vid",vid);
					OnSendAvocadoDocGeneralStringMessage("ViewSelectionChanged", docId, ParamListWriteLock(ppl)->SerializeList(),owner);
			}
			else if (InSelectionList(eid,vid) && multi)
			{
					ParamListSharedPtr ppl = ParamList::createNew();
					ParamListWriteLock(ppl)->PushInt("removed",eid);
					ParamListWriteLock(ppl)->PushInt("vid",vid);
					RemoveFromSelectionList(eid,vid);
					OnSendAvocadoDocGeneralStringMessage("ViewSelectionChanged", docId, ParamListWriteLock(ppl)->SerializeList(),owner);
					// Notify to application
				vector<int> eids;
				for (int idxkX =0;idxkX < SelectionCountPerView (vid);idxkX ++)
					{
						int currentElementId = GetIndexSelectionInView(vid,idxkX);
						eids.push_back (currentElementId);
					}
				NotifySelectionChanged (vid,eids);

			} else
			{
				ParamListSharedPtr ppl = ParamList::createNew();
					
				if (!multi && SelectionCountPerView (vid) > 0)
				{
					
					//int currentElementId = GetFirstSelectionInView(vid);
					for (int idxk =0;idxk < SelectionCountPerView (vid);idxk ++)
					{
						int currentElementId = GetIndexSelectionInView(vid,idxk);
						ParamListWriteLock(ppl)->PushInt("removed",currentElementId);
					}
					ClearSelectionList (vid);
					ClearSubSelectionList (vid);
				}
				ClearSubSelectionList (vid);
				AddToSelectionList(eid,vid,owner);
				ParamListWriteLock(ppl)->PushInt("added",eid);
				ParamListWriteLock(ppl)->PushInt("vid",vid);
				OnSendAvocadoDocGeneralStringMessage("ViewSelectionChanged", docId, ParamListWriteLock(ppl)->SerializeList());
				// Notify to application
				vector<int> eids;
				for (int idxkX =0;idxkX < SelectionCountPerView (vid);idxkX ++)
					{
						int currentElementId = GetIndexSelectionInView(vid,idxkX);
						eids.push_back (currentElementId);
					}
				NotifySelectionChanged (vid,eids);
			}
		}
		else if (msg=="OnPickNothing")
		{
			ret = true;
			/* We get this message when the picker did not intersect with any geometry. */
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int vid,eid;
			bool prepick;
			ParamListWriteLock(pl)->PopInt(vid);
			ParamListWriteLock(pl)->PopBool(prepick);
			if (prepick)
			{
				OnSendAvocadoGeneralStringMessage("UnPreHightlightElement", vid,"");
				return true;
			}
			if (SelectionCountPerView (vid) > 0)
			{
				for (int idxk =0;idxk < SelectionCountPerView (vid);idxk ++)
					{
						int currentElementId = GetIndexSelectionInView(vid,idxk);

						eid = GetFirstSelectionInView(vid);
						string owner = GetEidOwner (currentElementId);

						ParamListSharedPtr ppl = ParamList::createNew();
						ParamListWriteLock(ppl)->PushInt("removed",currentElementId);
						ParamListWriteLock(ppl)->PushInt("vid",vid);
						OnSendAvocadoDocGeneralStringMessage("ViewSelectionChanged", docId, ParamListWriteLock(ppl)->SerializeList(),owner);
				}
				//Notify to application that nothing is selected
				std::vector<int> sels; 
				sels.push_back(-1);
				NotifySelectionChanged (vid,sels);
				ClearSubSelectionList (vid);
				ClearSelectionList (vid);
			}
		}
		return ret;
	}
}
/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include <nvutil/Handle.h>
#include "AvocadoParams.h"
#include "AvocadoEngineObject.h"
#include <nvsg/Group.h>
#include <nvutil/DbgNew.h>
namespace avocado
{
	AvocadoEngineDocElement *AvocadoEngineDocElement::GetElementFromParams (string paramStr)
	{
		void *ptr;
		ParamListSharedPtr pl = ParamList::createFromString(paramStr);
		ParamListWriteLock(pl)->PopPtr( & ptr);
		AvocadoEngineDocElement* docElem = (AvocadoEngineDocElement*)ptr;
		return docElem;
	}
	
	string AvocadoEngineDocElement::GetParamsFromElement (AvocadoEngineDocElement *e)
	{
		void *ptr = e;
		ParamListSharedPtr pl = ParamList::createNew();
		ParamListWriteLock(pl)->PushPtr("elemptr",ptr);
		return pl->SerializeList();
	}
	bool AvocadoEngineDocElement::SetVisibility (bool isVisible ) 
	{
		if (m_intr.isVisible != isVisible)
		{
			m_intr.isVisible = isVisible;
			if (m_elementRoot)
			{
				int oldtravmask = nvsg::GroupWriteLock (m_elementRoot)->getTraversalMask ();
				if (isVisible)
					oldtravmask = 1;//~0;
				else
					oldtravmask = 0;
				nvsg::GroupWriteLock (m_elementRoot)->setTraversalMask (oldtravmask);
			}
			return true;
		}
		return false;
	}
}
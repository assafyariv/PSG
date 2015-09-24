/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include <string>

#include <nvutil/Handle.h>

// Want to avoid nvidia includes here.. no time..
#include <nvmath/Matnnt.h>
#include "AvocadoAppInterface.h"
using namespace std;

namespace nvutil
{
	//class Handle;
	class HandledObject;
	CORE_TYPES_BASE( HandledObject );
}

namespace nvsg 
{
	class Object;
	class Drawable;
	class Node;
	class Group;
	class ViewState;
	class Scene;
	class StateSet;
	CORE_TYPES( Object, nvutil::HandledObject );
	CORE_TYPES( Node, Object );
	CORE_TYPES( StateSet, Object );
	CORE_TYPES( Drawable, Object );
	CORE_TYPES (Group,Node);
	CORE_TYPES( Scene, Object );
	CORE_TYPES( ViewState, nvutil::HandledObject );
}

namespace avocado
{

	enum AvcElemType {
		AVC_DOC_ELEM = 0,
		AVC_VIEW_ELEM  = 1
	};
	class AvocadoEngineObject
	{
	public:
		AvocadoEngineObject () {}
		AvocadoEngineObject(AvcElemType type) : m_type(type) {}
		~AvocadoEngineObject () {}
		int			GetID(){ return m_id; }
		void		SetID(int id){ m_id = id; }
		std::string	GetName(){ return m_name; }
		void		SetName(std::string name){ m_name = name; }
		AvcElemType	GetType(){ return m_type; }
		void		SetType(AvcElemType type){ m_type = type; }
	protected:
		int							m_id;
		std::string					m_name;
		AvcElemType					m_type;
	};

	class AvocadoEngineViewElement: public AvocadoEngineObject
	{
	public:
		AvocadoEngineViewElement (int viewId,string ownerModuleName) : AvocadoEngineObject(AVC_VIEW_ELEM) 
		{ 
			m_ownerModule = ownerModuleName;
			m_viewId = viewId;
		} 
		~AvocadoEngineViewElement(){}

		nvsg::GroupSharedPtr	m_elementRoot;
		int						m_viewId;
		string					m_ownerModule;
	};

	class AvocadoEngineDocElement: public AvocadoEngineObject
	{
	public:
		AvocadoEngineDocElement (int docId,string ownerModuleName) : AvocadoEngineObject(AVC_DOC_ELEM) 
		{
			m_docId = docId;
			m_id = -1;
			m_name=std::string ("");
			m_ownerModule = ownerModuleName;
			m_isPickable = true;
			m_isRef = false;
			m_refGeoNode =std::string("");
			m_refEID = -1;
			m_parentGroupID = -1;
			m_refDrawableIdx = 0;
			m_elementRoot = 0;
		}
		~AvocadoEngineDocElement () {}
		virtual bool ApplyStateSet (nvsg::StateSetSharedPtr newStateSet, bool cache,bool overwrite,bool convertFFP = false) ;
		virtual void attachmentMoved (int attID,nvsg::GroupSharedPtr &node,nvsg::SceneSharedPtr &scene) {};
		virtual void removeFromScene(nvsg::SceneSharedPtr &scene) {};
		virtual void createScene(nvsg::SceneSharedPtr &scene,std::string sessionFolder,bool skip_optimization = false) {}
		virtual string serialize() = 0;
		virtual bool  SetVisibility (bool isVisible ) ;

		static	AvocadoEngineDocElement *			GetElementFromParams (string paramStr);
		static  string								GetParamsFromElement (AvocadoEngineDocElement *e);

		nvsg::DrawableSharedPtr m_subDrawable;
		nvsg::DrawableSharedPtr m_cachedDrawable;
		nvsg::GroupSharedPtr	m_elementRoot;
		int						m_docId;
		string					m_ownerModule;
		bool					m_isPickable;
		bool					m_isVisible;
		bool					m_isRef;
		int						m_refEID;
		std::string				m_refGeoNode;
		int						 m_refDrawableIdx;
		int						m_parentGroupID;
		// We keep a list of removed geo nodes for each element.
		// this helps in the case when sub element was deleted and we load from file.
		std::vector<std::string> m_removedGeoNodes;

		// Application Interface.
		avocado::AvocadoElementInterface		m_intr;

		// RUN-TIME DATA
		nvsg::StateSetSharedPtr            m_cachedStateSet;
		std::string                            m_shaderFileName;
		nvmath::Mat44f							m_lastSavedLocation;

		// here we keep a list of attached elements for each element.
		// when the element moves (or something else) it will call attachmentMoved () 
		// on the attached element.
		std::vector<int>						m_attachments;
	};

}
#pragma once
#include "AvocadoAppInterface.h"
#include "AvocadoModuleInterface.h"
#include <nvutil/Handle.h>

namespace nvsg
{
	class Group;
	class Object;
	class Node;
	class ViewState;
	class Drawable;
	class Scene;
	CORE_TYPES( Object, nvutil::HandledObject );
	CORE_TYPES( Node, Object );
	CORE_TYPES( Group,  Node );
	CORE_TYPES( ViewState, nvutil::HandledObject );
	CORE_TYPES( Drawable, Object );
	CORE_TYPES( Scene, Object );
}

namespace nvtraverser 
{
	class Intersection;
}

namespace avocado {

	class AvocadoPicker : public AvocadoViewModule
	{
	public:
		AvocadoPicker ();
		virtual ~AvocadoPicker ();

		virtual AvocadoViewModule* create ();

		virtual bool OnUnload ();
		virtual bool OnRegister ();

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint);
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint) ;//{return false; };
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) { return false; }	// unused

		bool intersectObjectt( const NodeSharedPtr & baseSearch,
								unsigned int screenX, unsigned int screenY,
								nvtraverser::Intersection & result , const ViewStateSharedPtr &vs);

		bool OnMouseLDown(int shiftstate,int x, int y, bool &needRepaint);
		void UnPick ();
		void Pick (int shiftstate,const nvtraverser::Intersection &result);
		bool AvocadoPicker::RayPick (
								unsigned int screenX, unsigned int screenY,
								nvtraverser::Intersection & result , bool &frontLayer);
		SceneSharedPtr							m_scene;
		int												m_parentDocId;
		bool	m_prePick;
		int m_ticks;
		bool m_wait;
		int m_lastX;
		int m_lastY;
		bool  m_prePickEnabled;
		bool m_preSelectionViewParam;
		int m_lastShift;
		int								m_manipulatiomMode;
	};
}


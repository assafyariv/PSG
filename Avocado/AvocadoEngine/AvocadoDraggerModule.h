#pragma once
#include "AvocadoAppInterface.h"
#include "AvocadoModuleInterface.h"
#include <nvutil/Handle.h>

namespace nvsg
{
	class Object;
	class Node;
	class ViewState;
	class Drawable;
	class Scene;
	class Group;
	class Transform;
	CORE_TYPES( Object, nvutil::HandledObject );
	CORE_TYPES( Node, Object );
	CORE_TYPES( ViewState, nvutil::HandledObject );
	CORE_TYPES( Drawable, Object );
	CORE_TYPES( Scene, Object );
	CORE_TYPES( Group, Node );
	CORE_TYPES( Transform, Group );
}
namespace nvmath
{
//	class Vec3f;
}
namespace nvtraverser 
{
	class Intersection;
}

namespace avocado {
	class AvocadoEngineViewElement;
	class AvocadoDragger : public AvocadoViewModule 
	{
	public:
		AvocadoDragger ();
		virtual ~AvocadoDragger ();
		virtual AvocadoViewModule* create ();

		virtual bool OnUnload ();
		virtual bool OnRegister ();

		virtual bool HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint);
		virtual bool HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId, const std::string &paramStr, bool &needRepaint);
		virtual bool HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId, const std::string &paramStr, bool &needRepaint) { return false; }	// unused
		bool OnDraggerPrePick (int x,bool &needRepaint);
		bool OnDraggerUnPrePick (bool &needRepaint);
		nvmath::Vec3f GetElementCenter ();
		void GetDeltas (float *p_x,float *p_Y);
		void CreateDraggerRoot();
		void AddRotationAxisElement (nvmath::Quatf ori, nvmath::Vec3f color);
		void AddAxisElement (nvmath::Quatf ori, nvmath::Vec3f color);
		void CreateFrontViewCamera ();
		void ZoomCameraToDragger();
		bool FixRotationDir (float *p_x,float *p_y);
		void AddScaleElement (nvmath::Vec3f color);

		bool OnPick ();
		bool OnUnPick ();
		bool OnDraggerPick (int x);
		bool OnMouseMove (int x, int y, bool &needRepaint);
		bool OnMouseLUp ();
		void SetDraggerOrientation();
		nvmath::Vec3f SetDraggerOrientation2(nvmath::Vec3f invec);
		void UpdateVisibilities ();
		nvsg::DrawableSharedPtr			m_currentSelecion;
		nvsg::TransformSharedPtr       	m_currentElement;
		nvsg::SceneSharedPtr			m_scene;
		bool							m_draggerVisible;
		bool							m_draggerAxisPicked;
		bool							m_first;
		int								m_y;
		int								m_x;
		int								m_axis;
		nvsg::TransformSharedPtr		m_draggerRoot;
		int m_currentElementID ;
				int m_currentElementDocID ;
				string m_currentElementOwner;
				nvsg::DrawableSharedPtr m_currentSub;
		vector <AvocadoEngineViewElement*>  m_draggerElements;
		int		m_currentPrePicked;
		bool	m_enableRotate;
		bool	m_enableScale;
		bool	m_enableTranslate;

		
	};
}


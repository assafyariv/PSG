#include "pch.h"
#include <nvsg/Group.h>

namespace nvsg {
	Group::Group ()
	{
		m_objectCode = nvsg::OC_GROUP;
	}
	Group::~Group () 
	{
	}
	Group::Group (Group const &rh)
	{
	}
	GroupSharedPtr Group::create ()
	{
		return GroupSharedPtr (GroupHandle::create());
	}
	nvmath::Box3f Group::calculateBoundingBox () const
	{
		return nvmath::Box3f ();
	}

	nvmath::Sphere3f Group::calculateBoundingSphere () const
	{
		return nvmath::Sphere3f();
	}
	Group::ChildrenContainer::iterator Group::doInsertChild( const ChildrenContainer::iterator & gcci, const NodeSharedPtr & child )
	{
		preAddChild(m_children.size());
		ChildrenContainer::iterator it =m_children.insert (gcci,child);
		postAddChild (m_children.size());
		return it;
	};
	Group::ChildrenContainer::iterator 
		Group::doRemoveChild( const ChildrenContainer::iterator & cci )
	{
		preRemoveChild(m_children.size());
		ChildrenContainer::iterator it =m_children.erase (cci);
		postRemoveChild (m_children.size());
		return it;
	}
	Group::ChildrenContainer::iterator 
		Group::doReplaceChild( ChildrenContainer::iterator & cci, const NodeSharedPtr & newChild )
	{
		doRemoveChild (cci);
		ChildrenContainer::iterator it = doInsertChild (m_children.end(),newChild);
		return it;
	}
	Group::ClipPlaneContainer::iterator 
		Group::doRemoveClipPlane( const ClipPlaneContainer::iterator & cpci )
	{
		ClipPlaneContainer::iterator it = m_clipPlanes.erase (cpci);
		return it;
	}
	Group::LightSourceContainer::iterator 
		Group::doRemoveLightSource( const LightSourceContainer::iterator & lsci )
	{
		LightSourceContainer::iterator it = m_lightSources.erase (lsci);
		return it;
	}



	void Group::syncIncarnation ( class nvutil::Incarnation const & (__thiscall nvsg::Object::*)(void)const ) const
	{
	}
	bool Group::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void Group::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	unsigned int  Group::determineHintsContainment(unsigned int which) const {return 0;}
	bool  Group::determineShaderContainment() const {return false;}
	bool  Group::determineTransparencyContainment() const {return false;}
	bool  Group::determineAnimationContainment() const {return false;}
	bool  Group::determineLightContainment() const {return false;}
	bool  Group::determineLODContainment() const { return false;}
	void Group::preRemoveChild (unsigned int x) {}
	void Group::preAddChild (unsigned int x) {}
	void Group::postAddChild (unsigned int x) {}
	void Group::postRemoveChild (unsigned int x) {}
	void Group::initReflectionInfo () {}


}
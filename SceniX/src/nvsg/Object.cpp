#include "pch.h"
#include <nvsg/Object.h>
#include "D3DDal.h"

namespace nvsg {
	
	nvutil::Incarnation Object::m_dummyIncarnation;
	Object::Object()
	{
		m_objectCode = nvsg::OC_OBJECT;
		m_dalHost = D3DDalHost::create ();
	}
	Object::~Object()
	{
	}
	const DALHostSharedPtr &Object::getDALHost () const
	{
		return m_dalHost;
	}
	nvmath::Box3f Object::calculateBoundingBox () const
	{
		return nvmath::Box3f ();
	}
	nvmath::Sphere3f Object::calculateBoundingSphere () const
	{
		return nvmath::Sphere3f();
	}
	nvmath::Box3f Object::m_defaultBoundingBox;// = nvmath::Box3f();

	nvmath::Sphere3f Object::m_defaultBoundingSphere;
	Object::Object (const Object & bj)
	{
	}
	nvmath::Box3f const& Object::getBoundingBox (bool recal) const
	{
		return m_defaultBoundingBox;
	}
	nvmath::Sphere3f const& Object::getBoundingSphere (bool recalc) const
	{
		return m_defaultBoundingSphere;
	}
	nvmath::Box3f const& Object::getBoundingBox (void) const
	{
		return m_defaultBoundingBox;
	}
	nvmath::Sphere3f const& Object::getBoundingSphere (void) const
	{
		return m_defaultBoundingSphere;
	}
	unsigned int  Object::determineHintsContainment(unsigned int which) const {return 0;}
	bool  Object::determineShaderContainment() const {return false;}
	bool  Object::determineTransparencyContainment() const {return false;}
	bool  Object::determineAnimationContainment() const {return false;}
	bool  Object::determineLightContainment() const {return false;}
	bool  Object::determineLODContainment() const { return false;}
	bool Object::isDataShared() const {return false;}
	unsigned int Object::getHigherLevelObjectCode (unsigned int x) const {return 0;}
	nvsg::DataID Object::getDataID () const { return 0;}
	void Object::initReflectionInfo () {}
	void Object::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	void Object::syncIncarnation ( class nvutil::Incarnation const & (__thiscall nvsg::Object::*)(void)const ) const
	{
	}
	bool Object::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void Object::notifyChange (const nvutil::Subject *sub, unsigned int ui) const {}
	bool Object::containsTransparency() const { return false ; }
	  bool Object::containsAnimation() const { return false ; }
	 bool Object::containsLight() const { return false; }

	 // Static properties
	 nvutil::PropertyId Object::PID_Hints;
	 nvutil::PropertyId Object::PID_TraversalMask;
	 Object::CallbackContainer Object::m_dummyCallbackContainer;

}
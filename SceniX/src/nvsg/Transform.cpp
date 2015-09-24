#include "pch.h"
#include <nvsg/Transform.h>

namespace nvsg {
	Transform::Transform () {
		m_objectCode = OC_TRANSFORM;
	}
	Transform::Transform ( const Transform & d ) {}
	Transform::~Transform () {}
	TransformSharedPtr Transform::create () 
	{
		return TransformSharedPtr (TransformHandle::create());
	}
	void Transform::initReflectionInfo () {}
	bool Transform::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void Transform::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	nvmath::Box3f Transform::calculateBoundingBox () const
	{
		return nvmath::Box3f ();
	}

	void Transform::setTrafo (const nvmath::Trafo &tr) {m_trafo = tr;}

	nvmath::Sphere3f Transform::calculateBoundingSphere () const
	{
		return nvmath::Sphere3f();
	}
}
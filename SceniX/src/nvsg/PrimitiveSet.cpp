#include "pch.h"
#include <nvsg/PrimitiveSet.h>

namespace nvsg {
	
	
	/*----------------------*/
	PrimitiveSet::PrimitiveSet ()
	{
		// no object code	m_objectCode = nvsg::OC_TRIANGLES;
	}
	PrimitiveSet::~PrimitiveSet () 
	{
	}
	bool  PrimitiveSet::determineAnimationContainment() const {return false;}
	void PrimitiveSet::syncIncarnation ( class nvutil::Incarnation const & (__thiscall nvsg::Object::*)(void)const ) const
	{

	}
	bool PrimitiveSet::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void PrimitiveSet::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	void
		PrimitiveSet::initReflectionInfo ()
	{
	}

	void PrimitiveSet::calculateTexCoords (nvsg::TextureCoordType type, unsigned int uix, bool bbol)
	{
	}
	
}
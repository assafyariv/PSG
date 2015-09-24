#include "pch.h"
#include <nvsg/IndependentPrimitiveSet.h>

namespace nvsg {
	/*-------------------------*/
	IndependentPrimitiveSet::IndependentPrimitiveSet ()
	{
		// no object code	m_objectCode = nvsg::OC_TRIANGLES;
	}
	IndependentPrimitiveSet::~IndependentPrimitiveSet () 
	{
	}
	IndependentPrimitiveSet::IndependentPrimitiveSet (IndependentPrimitiveSet const &rh)
	{
	}

	nvmath::Box3f IndependentPrimitiveSet::calculateBoundingBox () const
	{
		return nvmath::Box3f ();
	}
	nvmath::Sphere3f IndependentPrimitiveSet::calculateBoundingSphere () const
	{
		return nvmath::Sphere3f();
	}
	bool IndependentPrimitiveSet::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void IndependentPrimitiveSet::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	bool IndependentPrimitiveSet::isDataShared () const
	{
		return false;
	}
	nvsg::DataID IndependentPrimitiveSet::getDataID () const
	{
		return 0;
	}
	void
		IndependentPrimitiveSet::initReflectionInfo ()
	{
	}
	void  IndependentPrimitiveSet::setIndices( const unsigned int * indices, unsigned int numIndices )
	{
	}

	void IndependentPrimitiveSet::setIndices( unsigned int pos, const unsigned int * indices, unsigned int numIndices ) 
	{
	}

}
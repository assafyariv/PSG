#include "pch.h"
#include <nvsg/Primitive.h>

namespace nvsg {
Primitive::Primitive (){ m_objectCode = nvsg::OC_PRIMITIVE;}
	Primitive::~Primitive () {}
	Primitive::Primitive (const Primitive &x) {}
	PrimitiveSharedPtr Primitive::create ()
	{
		return PrimitiveSharedPtr (PrimitiveHandle::create());	
	}

	void Primitive::setPrimitiveType (PrimitiveType t) { m_primitiveType = t;}
	void Primitive::setVertexAttributeSet ( const VertexAttributeSetSharedPtr & vash) { m_vertexAttributeSet =vash;}
	void Primitive::setIndexSet (const IndexSetSharedPtr & iset ) {m_indexSet = iset;}
	bool Primitive::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void Primitive::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}
	void Primitive::determinePrimitiveAndFaceCount() const {}

	void
		Primitive::initReflectionInfo ()
	{
	}
	bool Primitive::calculateNormals( bool rec )
	{
		return false;
	}
	void Primitive::calculateTangentSpace (unsigned int x, unsigned int y, unsigned int z, bool i)
	{
	}
	bool  Primitive::determineAnimationContainment() const {return false;}
	void Primitive::syncIncarnation ( class nvutil::Incarnation const & (__thiscall nvsg::Object::*)(void)const ) const {}
	nvmath::Box3f Primitive::calculateBoundingBox () const
	{
		return nvmath::Box3f ();
	}
	nvmath::Sphere3f Primitive::calculateBoundingSphere () const
	{
		return nvmath::Sphere3f();
	}
	void Primitive::calculateTexCoords (nvsg::TextureCoordType type, unsigned int uix, bool bbol)
	{
	}

}
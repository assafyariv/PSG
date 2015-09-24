#include "pch.h"
#include <nvsg/Drawable.h>

namespace nvsg {
		Drawable::Drawable ()
	{
		// no object code	m_objectCode = nvsg::OC_TRIANGLES;
	}
	Drawable::~Drawable () 
	{
	}
	nvmath::Box3f const& Drawable::getBoundingBox (bool recal) const
	{
		if (recal)
			m_boundingBox = calculateBoundingBox ();
		return m_boundingBox;
	}
	nvmath::Sphere3f const& Drawable::getBoundingSphere (bool recalc) const
	{
		if (recalc)
			m_boundingSphere = calculateBoundingSphere ();
		return m_boundingSphere;
	}
	bool Drawable::calculateNormals( bool rec )
	{
		return false;
	}
	void Drawable::calculateTangentSpace (unsigned int x, unsigned int y, unsigned int z, bool i)
	{
	}
	void
		Drawable::initReflectionInfo ()
	{
	}
	void Drawable::calculateTexCoords (nvsg::TextureCoordType type, unsigned int uix, bool bbol)
	{
	}
}
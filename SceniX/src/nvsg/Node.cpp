#include "pch.h"
#include <nvsg/Node.h>

namespace nvsg {

	Node::Node ()
	{
	}
	Node::~Node ()
	{
	}
	nvmath::Box3f const& Node::getBoundingBox (bool recal) const
	{
		if (recal)
			m_boundingBox = calculateBoundingBox ();
		return m_boundingBox;
	}
	nvmath::Sphere3f const& Node::getBoundingSphere (bool recalc) const
	{
		if (recalc)
			m_boundingSphere = calculateBoundingSphere ();
		return m_boundingSphere;
	}
	void Node::initReflectionInfo () {}
}
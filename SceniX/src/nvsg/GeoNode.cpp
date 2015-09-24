#include "pch.h"
#include <nvsg/GeoNode.h>
namespace nvsg {
	GeoNode::GeoNode ()
	{
		m_objectCode = nvsg::OC_GEONODE;
	}
	GeoNode::~GeoNode ()
	{
	}
	GeoNode::GeoNode (const GeoNode &x)
	{
	}
	GeoNodeSharedPtr GeoNode::create ()
	{
		return GeoNodeSharedPtr (GeoNodeHandle::create());
	}

	nvmath::Box3f GeoNode::calculateBoundingBox () const
	{
		return nvmath::Box3f ();
	}
	nvmath::Sphere3f GeoNode::calculateBoundingSphere () const
	{
		return nvmath::Sphere3f();
	}
	void GeoNode::syncIncarnation ( class nvutil::Incarnation const & (__thiscall nvsg::Object::*)(void)const ) const
	{
	}
	bool GeoNode::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void GeoNode::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}   
	bool  GeoNode::determineShaderContainment() const {return false;}
	bool  GeoNode::determineTransparencyContainment() const {return false;}
	bool  GeoNode::determineAnimationContainment() const {return false;}
	void GeoNode::initReflectionInfo () {}
	std::pair<GeoNode::StateSetIterator,GeoNode::DrawableIterator> 
		GeoNode::addDrawable (const StateSetSharedPtr &ss, const DrawableSharedPtr &drawable)
	{
		GeoNode::StateSetIterator geoit = beginStateSets ();//( m_geometries.begin() );
		while (geoit !=endStateSets ())
		{
			if (geoit.m_iter->m_stateSet == ss)
			{
				geoit.m_iter->m_drawables.push_back (drawable);
				return  std::pair<GeoNode::StateSetIterator,GeoNode::DrawableIterator> (geoit,GeoNode::DrawableIterator(geoit.m_iter->m_drawables.end()));
			}
			++geoit;
		}
		GeoNode::Geometry geom (ss);
		geom.m_drawables.push_back (drawable);
		m_geometries.push_back (geom);
		return  std::pair<GeoNode::StateSetIterator,GeoNode::DrawableIterator> (StateSetIterator(m_geometries.begin()),DrawableIterator (geom.m_drawables.begin()));
		//	m_geometries.insert ( std::pair <StateSetSharedPtr,DrawableSharedPtr> ());
	}

}
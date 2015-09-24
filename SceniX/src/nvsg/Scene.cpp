#include "pch.h"
//#include "Scene.h"

#include <nvutil/nvutil.h>
#include <nvutil/Allocator.h>
#include <nvutil/SWMRSync.h>
#include <nvsg/CoreTypes.h>
#include <nvsg/Group.h>
#include <nvutil/SmartPtr.h>
#include <nvsg/Scene.h>
#include <nvutil/RCObject.h>
#include <nvmath/nvmath.h>
#include <nvutil/DbgNew.h>
#include <nvsg/GeoNode.h>
#include <nvutil/HandledObject.h>
#include <nvsg/Object.h>
#include <nvutil/Incarnation.h>
#include <nvsg/Triangles.h>
#include "Direct3DBase.h"
#include <D3DBase.h>
#include <nvsg/VertexAttributeSet.h>
#include <nvsg/Transform.h>
#include <nvsg/IndexSet.h>
#include <nvsg/ViewState.h>
#include <nvui/RenderTarget.h>
#include <nvui/Renderer.h>
#include <nvui/SceneRenderer.h>

#include <nvd3d/SceneRendererD3D.h>
#include "D3DDal.h"

namespace nvutil
{

	Incarnation::Incarnation ()
	{
		m_incarnation = 0;
	}

	Incarnation::Incarnation (int x)
	{
		this->m_incarnation = x;
	}
}


namespace nvd3d {

	static NVD3DBase m_globalBase;
	NVD3DBase &GetNV3DBase() { return m_globalBase ; }
}
namespace nvsg {
	void nvsgInitialize ()
	{
		nvd3d::m_globalBase.CreateDeviceResources ();
		nvd3d::m_globalBase.CreateDefaultShaders ();
	}
	void nvsgTerminate ()
	{
	}
	DALData::~DALData ()
	{
	}
	DALData::DALData(DALDataCreator *creator)
	{
		this->m_creator = creator;
	}
	bool D3DVertexBufferData::test_func (D3DVertexBufferData *data) { return true; }
	bool D3DVertexBufferData::CreateVertexBuffer (unsigned int size, unsigned int type
		, const void * data, unsigned int strideInBytes, unsigned int count)
	{
		m_d3dBuffer = nvd3d::m_globalBase.CreateVertexBuffer (size,type,data,strideInBytes,count);
		if (!m_d3dBuffer)
			return false;
		m_count = size;
		return true;
	}
	bool D3DVertexBufferData::CreateIndexBuffer (unsigned int size, const void * data,const D3DDataSize &type)
	{
		int totalSize = 0;
		switch (type)
		{
		case D3DVertexBufferData::D3D_DATA_SIZE_UINT_16:
			totalSize = 2;
			break;
		case D3DVertexBufferData::D3D_DATA_SIZE_UINT_32:
			totalSize = 4;
			break;
		default:
			// assert 
			break;
		};
		m_d3dBuffer = nvd3d::m_globalBase.CreateIndexBuffer (size,data,totalSize);
		if (!m_d3dBuffer)
			return false;
		m_count = size;
		m_type = type;
		return true;
	}

	static D3DDataCreator g_dataCreator ;
	D3DDataCreator  &GetDataCreator () 
	{ 
		return g_dataCreator;
	}
	//#if 0
	DALHost::DALHost ()
	{
		m_hdal = HDAL_INVALID;
	}
	DALHost::~DALHost ()
	{
	}
	DALHost::DALHost (const DALHost &d)
	{
	}

}

namespace nvsg {

#if _WIN32_WINNT_WIN8
} // exit namespace
nvsg::DALServer nvutil::Singleton<nvsg::DALServer>::m_instance; 
namespace nvsg {
#endif

	// Triangles
	Triangles::Triangles ()
	{
		m_objectCode = nvsg::OC_TRIANGLES;
	}
	Triangles::~Triangles () 
	{
	}
	Triangles::Triangles (Triangles const &rh)

	{
	}

	TrianglesSharedPtr Triangles::create ()
	{

		return TrianglesSharedPtr (TrianglesHandle::create());

	}

	void
		Triangles::initReflectionInfo ()
	{
	}
	bool Triangles::calculateNormals( bool rec )
	{
		return false;
	}
	void Triangles::calculateTangentSpace (unsigned int x, unsigned int y, unsigned int z, bool i)
	{
	}

	Scene::Scene () {}
	Scene::~Scene () {}
	void Scene::initReflectionInfo () {}
	void Scene::setRootNode (const NodeSharedPtr &node)
	{
		m_root = node;
	}
	nvutil::PropertyId Scene::PID_BackColor;
	nvutil::PropertyId Scene::PID_AmbientColor;
	nvutil::PropertyId Scene::PID_FrameNumber;


	SceneSharedPtr
		Scene::create() { return SceneSharedPtr (SceneHandle::create()) ;}
	nvmath::Box3f const& Scene::getBoundingBox (bool recal) const
	{
		if (recal)
			m_boundingBox = calculateBoundingBox ();
		return m_boundingBox;
	}
	nvmath::Sphere3f const& Scene::getBoundingSphere (bool recalc) const
	{
		if (recalc)
			m_boundingSphere = calculateBoundingSphere ();
		return m_boundingSphere;
	}
}


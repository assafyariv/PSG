#include "pch.h"
#include "CubeRenderer.h"
#include "nvmath/Beziernt.h"
#include <nvsg/nvsg.h>
#include <nvsg/CoreTypes.h>
#include <nvsg/GeoNode.h>

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
using namespace Windows::UI::Core;

CubeRenderer2::CubeRenderer2() :
	m_loadingComplete(false)
	//m_indexCount(0)
{
	m_scene = 0;
	int *x;
	x = new int;
	(*x) = 10;
}

// Initialize the Direct3D resources required to run.
void CubeRenderer2::Initialize(CoreWindow^ window)
{
	m_window = window;

	CreateDeviceResources();
	CreateWindowSizeDependentResources();

}
void CubeRenderer2::Present ()
{
	if (this->m_d3dContext)
	{
		m_renderTarget->Present ();
	}
}
void CubeRenderer2::CreateDeviceResources()
{
	nvsg::nvsgInitialize ();
	nvsg::TransformSharedPtr mygroup = nvsg::Transform::create();
	nvmath::Trafo trafo;
	trafo.setMatrix (nvmath::Mat44f (true));
	nvsg::TransformWriteLock (mygroup)->setTrafo (trafo);
	nvsg::GroupSharedPtr mygroup2 = nvsg::Group::create();
	nvsg::NodeSharedPtr chilgroup (mygroup2.get());
	unsigned int l = nvsg::GroupWriteLock(mygroup)->getObjectCode ();
	nvsg::GroupWriteLock (mygroup)->addChild (mygroup2);
	nvsg::GeoNodeSharedPtr gron = nvsg::GeoNode::create();
	nvsg::StateSetSharedPtr ssptr = nvsg::StateSet::create ();
	//nvsg::TrianglesWriteLock(tris)->setFaces (0,&f4,1);
	//std::string name (nvsg::GroupWriteLock );//addChild (chilgroup); 
	nvsg::VertexAttributeSetSharedPtr vas = nvsg::VertexAttributeSet::create();
	std::vector<nvmath::Vec3f> verts;

	verts.push_back (nvmath::Vec3f (-0.5f,-0.5f,-0.5f)); verts.push_back (nvmath::Vec3f (0.77f,0.66f,0.44f));

	verts.push_back (nvmath::Vec3f (-0.5f,-0.5f,0.5f));verts.push_back (nvmath::Vec3f (0.77f,0.66f,0.44f));

	verts.push_back (nvmath::Vec3f (-0.5f,0.5f,-0.5f));verts.push_back (nvmath::Vec3f (0.77f,0.66f,0.44f));

	verts.push_back (nvmath::Vec3f (-0.5f,0.5f,0.5f));verts.push_back (nvmath::Vec3f (0.0f,0.66f,0.44f));

	verts.push_back (nvmath::Vec3f (0.5f,-0.5f,-0.5f));verts.push_back (nvmath::Vec3f (0.77f,0.66f,0.44f));

	verts.push_back (nvmath::Vec3f (0.5f,-0.5f,0.5f));verts.push_back (nvmath::Vec3f (0.77f,0.06f,0.99f));

	verts.push_back (nvmath::Vec3f (0.5f,0.5f,-0.5f));verts.push_back (nvmath::Vec3f (0.77f,0.66f,0.44f));

	verts.push_back (nvmath::Vec3f (0.5f,0.5f,0.5f));verts.push_back (nvmath::Vec3f (0.77f,0.66f,0.99f));


	nvsg::VertexAttributeSetWriteLock(vas)->setVertices (&verts[0],8 );

	std::vector<unsigned int> indices;
	indices.push_back (0);	indices.push_back (2);	indices.push_back (1);
	indices.push_back (1); indices.push_back (2); indices.push_back (3);
	indices.push_back (4);indices.push_back (5);indices.push_back (6);
	indices.push_back (5);	indices.push_back (7);	indices.push_back (6);
	indices.push_back (0);	indices.push_back (1);indices.push_back (5);
	indices.push_back (0); indices.push_back (5); indices.push_back (4);
	indices.push_back (2); indices.push_back (6); indices.push_back (7);
	indices.push_back (2); indices.push_back (7); indices.push_back (3);
	indices.push_back (0); indices.push_back (4); indices.push_back (6);
	indices.push_back (0); indices.push_back (6); indices.push_back (2);
	indices.push_back (1); indices.push_back (3); indices.push_back (7);
	indices.push_back (1); indices.push_back (7); indices.push_back (5);


	nvsg::IndexSetSharedPtr indexSet( nvsg::IndexSet::create() );
	nvsg::IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

	nvsg::PrimitiveSharedPtr primitivePtr = nvsg::Primitive::create();
	{
		nvsg::PrimitiveWriteLock primitive(primitivePtr);
		primitive->setPrimitiveType( nvsg::PRIMITIVE_TRIANGLES );
		primitive->setVertexAttributeSet( vas );
		primitive->setIndexSet( indexSet );
	}
	nvsg::GeoNodeWriteLock (gron)->addDrawable (ssptr,primitivePtr);
	nvsg::SceneSharedPtr scn = nvsg::Scene::create();
	if (m_scene == 0)
		m_scene = scn;
	nvsg::GroupWriteLock (mygroup)->addChild (gron);
	nvsg::SceneWriteLock (m_scene)->setRootNode (mygroup);
	m_root = mygroup;


}

void CubeRenderer2::CreateWindowSizeDependentResources()
{

	//init context, target , renderer and viewstate
	//nvd3d::SmartRenderContextD3D d3dContext;
	m_d3dContext = nvd3d::RenderContextD3D::CreateFromCoreWindow (m_window.Get());

	// nvd3d::SmartRenderTargetD3D d3dRenderTarget;
	m_renderTarget = nvd3d::RenderTargetD3D::create (m_d3dContext);

	// nvsg::ViewStateSharedPtr
	m_viewState  = nvsg::ViewState::create();

	m_renderer = nvd3d::SceneRendererD3D::create ();
	//  nvd3d::SmartSceneRendererD3D renderer;
	m_renderer->setRenderTarget (m_renderTarget);
	m_renderer->setViewState (m_viewState);
	m_renderer->setScene(m_scene);

}

void CubeRenderer2::Update(float timeTotal, float timeDelta)
{
	//m_renderer->UpdateTimer (timeTotal);
	(void) timeDelta; // Unused parameter.

	XMVECTOR eye = XMVectorSet(0.0f, 0.7f, 1.5f, 0.0f);
	XMVECTOR at = XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	DirectX::XMFLOAT4X4 matx;

	///	XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
	XMStoreFloat4x4(&matx, XMMatrixTranspose(XMMatrixRotationY(timeTotal * XM_PIDIV4)));
	nvmath::Trafo trafo;
	nvmath::Mat44f matnf;
	for (int i=0;i<4;i++)
		for (int k=0;k<4;k++)
			matnf[i][k] = matx.m[i][k];

	trafo.setMatrix(matnf);
	if (m_root)
		nvsg::TransformWriteLock(m_root)->setTrafo (trafo);
}

void CubeRenderer2::Render()
{
	m_renderer->render();
}

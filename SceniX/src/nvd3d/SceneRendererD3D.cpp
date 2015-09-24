

#include <pch.h>
#include <nvd3d/SceneRendererD3D.h>
#include "D3DDal.h"
#include "D3DBase.h"

namespace nvui {
	void SceneRenderer::beginRendering( const nvsg::ViewStateSharedPtr &viewState, const SmartRenderTarget &renderTarget ) {}
	void SceneRenderer::endRendering( const nvsg::ViewStateSharedPtr &viewState, const SmartRenderTarget &renderTarget ) {}

	void SceneRenderer::doRender( const SmartRenderTarget &renderTarget ){}

	/** \brief Interface for the actual rendering algorithm.
	\param viewState The nvsg::ViewState to use to render the frame.
	\param renderTarget The RenderTarget to use to render the frame.
	**/
	void SceneRenderer::doRender( const nvsg::ViewStateSharedPtr &viewState, const SmartRenderTarget &renderTarget ) {};

	void SceneRenderer::renderMapList( const nvsg::MapListSharedPtr& mapList, const SmartRenderTarget &renderTarget ) {}
	void SceneRenderer::beginRenderMapList( const nvsg::MapListSharedPtr& mapList, const SmartRenderTarget &renderTarget ) {}
	void SceneRenderer::endRenderMapList( const nvsg::MapListSharedPtr& mapList, const SmartRenderTarget &renderTarget ){}

	void SceneRenderer::renderMapElement( const nvsg::MapElementSharedPtr& mapElement, const SmartRenderTarget &renderTarget ) {};
	void SceneRenderer::renderMapArray( const nvsg::MapArraySharedPtr& mapArray, const SmartRenderTarget &renderTarget ) {};
	void SceneRenderer::renderMapMatrix( const nvsg::MapMatrixSharedPtr& mapMatrix, const SmartRenderTarget &renderTarget ) {};
	void SceneRenderer::renderMapObject( const nvsg::MapObjectSharedPtr& mapObject, const SmartRenderTarget &renderTarget ) {};
	void SceneRenderer::addRendererOptions( const RendererOptionsSharedPtr &rendererOptions ) {}
	void SceneRenderer::initReflectionInfo () {}
	SceneRenderer::SceneRenderer (const SmartRenderTarget &tar) {}
	Renderer::~Renderer() {}
	void Renderer::beginRendering(  const SmartRenderTarget &renderTarget ) {}
	void Renderer::endRendering( const SmartRenderTarget &renderTarget ) {}
	Renderer::Renderer (const SmartRenderTarget &tar) {}
	void Renderer::restartAccumulation () {}
	nvutil::Property * nvui::SceneRenderer::PID_TraversalMaskOverride;
	nvutil::Property * nvui::SceneRenderer::PID_PreserveTexturesAfterUpload;
	
}


namespace nvd3d {
	
		SceneRendererD3D::SceneRendererD3D (const SmartRenderTargetD3D &renderTarget) 
		{  
			m_renderTarget = renderTarget; 
		}
		SceneRendererD3D::~SceneRendererD3D () {}
	    nvutil::SmartPtr<SceneRendererD3D>  SceneRendererD3D::create ()
		{
			return nvutil::SmartPtr<SceneRendererD3D> (new SceneRendererD3D ());
		}
		void SceneRendererD3D::setRenderTarget (const SmartRenderTargetD3D & target) 
		{
			m_renderTarget = target;
		}

		//  void setViewState () { }
		void SceneRendererD3D::DrawRecursive (nvsg::NodeSharedPtr node,nvmath::Mat44f &mat)
		{
			nvsg::Group::ChildrenConstIterator it ;
			nvsg::GroupSharedPtr group ;
			nvsg::GeoNodeSharedPtr geop;
			nvsg::GeoNode::DrawableConstIterator dit;
			nvsg::GeoNode::StateSetConstIterator ssit;
			nvsg::DrawableSharedPtr drawable;
			nvmath::Mat44f matt2 ;
			nvmath::Trafo tr ;
			switch (nvsg::NodeReadLock (node)->getObjectCode() )
			{
			case nvsg::OC_GROUP:
				group = nvutil::smart_cast<nvsg::GroupHandle>  (node);
				it = nvsg::GroupReadLock (group)->beginChildren ();
				while (it != nvsg::GroupReadLock (group)->endChildren())
				{
					nvsg::NodeSharedPtr childNode = *it;
					DrawRecursive (childNode,mat);
					++it;
				}
				break;
			case nvsg::OC_TRANSFORM:
				group = nvutil::smart_cast<nvsg::TransformHandle>  (node);
				tr= nvsg::TransformReadLock (group)->getTrafo();
				matt2=tr.getMatrix();
				it = nvsg::TransformReadLock (group)->beginChildren ();
				while (it != nvsg::TransformReadLock (group)->endChildren())
				{
					nvsg::NodeSharedPtr childNode = *it;
					DrawRecursive (childNode,mat*matt2);
					++it;
				}
				break;
			case nvsg::OC_GEONODE:
				geop = nvutil::smart_cast<nvsg::GeoNodeHandle>(node);
				ssit = nvsg::GeoNodeReadLock (geop)->beginStateSets();
				while(ssit != nvsg::GeoNodeReadLock (geop)->endStateSets())
				{
					dit = nvsg::GeoNodeReadLock (geop)->beginDrawables (ssit);
					while (dit !=  nvsg::GeoNodeReadLock (geop)->endDrawables (ssit))
					{
						// move this to a draw callback..

						drawable = *dit;
						nvsg::D3DDalHostSharedPtr dalhost;
						ID3D11Buffer *vertexBuffer =0 ;
						ID3D11Buffer *indexBuffer = 0;

						nvsg::VertexAttributeSetSharedPtr vas= nvsg::PrimitiveReadLock (drawable)->getVertexAttributeSet();
						dalhost = nvsg::VertexAttributeSetReadLock(vas)->getDALHost();
						nvsg::D3DVertexBufferData *vbdata;
						nvsg::D3DDalHostReadLock (dalhost)->getDeviceAbstractionLinkData(0,vbdata,nvsg::D3DVertexBufferData::test_func);
						vertexBuffer = vbdata->GetBuffer();//m_d3dBuffer;


						nvsg::IndexSetSharedPtr isptr= nvsg::PrimitiveReadLock (drawable)->getIndexSet();//getVertexAttributeSet();
						dalhost = nvsg::IndexSetWriteLock(isptr)->getDALHost();
						nvsg::D3DVertexBufferData *ibdata;
						nvsg::D3DDalHostReadLock (dalhost)->getDeviceAbstractionLinkData(0,ibdata,nvsg::D3DVertexBufferData::test_func);
						indexBuffer = ibdata->GetBuffer();
						GetNV3DBase().SetModelMatrix (mat);
						GetNV3DBase().DrawBuffer ( indexBuffer, &vertexBuffer,ibdata->GetCount(),ibdata->GetFormat());
						++dit;
					}
					++ssit;
				}

				break;
			case nvsg::OC_DRAWABLE:
				break;
			case nvsg::OC_STATESET:
				break;
			default:
				break;
			};
		}
		void SceneRendererD3D::render () {
			nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->clearRenderTargetView ();
			nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->clearDepthStencilView ();
			unsigned int width = 0;
			unsigned int height= 0;
			nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->getSize (width,height);

			GetNV3DBase().SetTargets (  nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->GetRenderTargetView(),
				nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->GetDepthStencilView(),
				float(width),
				float(height));
			GetNV3DBase().SetDefaultShaders ();								
			nvsg::NodeSharedPtr root = nvsg::SceneReadLock (m_scene)->getRootNode ();
			nvmath::Mat44f m(true);
			DrawRecursive (root,m);
		}
		void SceneRendererD3D::setViewState ( nvsg::ViewStateSharedPtr vs) { m_viewState = vs;}
		void SceneRendererD3D::setScene (nvsg::SceneSharedPtr scene) { m_scene = scene; }
	
		void SceneRendererD3D::doRender (const nvsg::ViewStateSharedPtr &vs,const nvui::SmartRenderTarget &rt) {}

} // namespace nvsg

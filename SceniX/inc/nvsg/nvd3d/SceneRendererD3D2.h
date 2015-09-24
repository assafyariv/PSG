#pragma once
#ifndef SCENE_RENDERER_D3D
#define SCENE_RENDERER_D3D
#include <pch.h>
#include <nvd3d/D3DBase2.h>
#include <nvutil/SmartPtr.h>

#include <nvui/RenderTarget.h>
#include <nvui/SceneRenderer.h>
#include <nvutil/RCObject.h>
#include <nvsg/geoNode.h>
#include <nvsg/Transform.h>
#include <nvd3d/D3DDal2.h>
#include <nvsg/Primitive.h>
namespace nvd3d {
	class RenderContextD3D;
	typedef nvutil::SmartPtr<RenderContextD3D> SmartRenderContextD3D;
	class RenderContextD3D : public nvutil::RCObject
	{
	public:
		void Present (IDXGISwapChain1 *swapChain, ID3D11RenderTargetView 
			*rtv, ID3D11DepthStencilView *dsv)
		{
			nvsg::GetNV3DBase().Present (swapChain,rtv,dsv);
		}
		static SmartRenderContextD3D CreateFromCoreWindow ( Windows::UI::Core::CoreWindow ^  window)
		{
			RenderContextD3D *rcObj = new RenderContextD3D ();
			rcObj->m_window = window;
			SmartRenderContextD3D smartRC (rcObj);
			return smartRC;
		};
	protected:
		friend class RenderTargetD3D;
		void clearRTV ( ID3D11RenderTargetView *rtv) {
			nvsg::GetNV3DBase ().clearRTV (rtv);
		}
		void clearDSV ( ID3D11DepthStencilView *dsv) {
			nvsg::GetNV3DBase().clearDSV (dsv);
		}

		ID3D11RenderTargetView * CreateRenderTargetView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY) 
		{
			ID3D11RenderTargetView *res = nvsg::GetNV3DBase().CreateRenderTargetView (swapChain,targetX,targetY);
			return res;
		}
		ID3D11DepthStencilView * CreateDepthStencilView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY) 
		{
			ID3D11DepthStencilView *res = nvsg::GetNV3DBase().CreateDepthStencilView (swapChain,targetX,targetY);
			return res;
		}
		IDXGISwapChain1        * CreateSwapChain (unsigned int targetX, unsigned int targetY) 
		{
			IDXGISwapChain1 *swapChain = nvsg::GetNV3DBase().CreateSwapChain (targetX,targetY, m_window.Get());
			return swapChain;
		}
		void                    ResizeSwapChain (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY)
		{
			nvsg::GetNV3DBase().ResizeSwapChain (swapChain,targetX,targetY);
		};
		bool                    GetWindowBounds (float &width, float &height) 
		{
			width=m_window->Bounds.Width;
			height = m_window->Bounds.Height; 
			return true;
		}
		void makeCurrent () {}
	private:
		Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
	};

	class RenderTargetD3D : public nvui::RenderTarget{
	protected:
		RenderTargetD3D (SmartRenderContextD3D &context)
		{
			m_context = context;
			m_renderTargetWidth = 0;
			m_renderTargetHeight = 0;
			float i_windowWidth,i_windowHeight;
			context->GetWindowBounds (i_windowWidth,i_windowHeight);
				// Calculate the necessary swap chain and render target size in pixels.
			float windowWidth = nvsg::GetNV3DBase().ConvertDipsToPixels(i_windowWidth);
			float windowHeight = nvsg::GetNV3DBase().ConvertDipsToPixels(i_windowHeight);

			// The width and height of the swap chain must be based on the window's
			// landscape-oriented width and height. If the window is in a portrait
			// orientation, the dimensions must be reversed.
			Windows::Graphics::Display::DisplayOrientations m_orientation = Windows::Graphics::Display::DisplayProperties::CurrentOrientation;
			bool swapDimensions =
				m_orientation == Windows::Graphics::Display::DisplayOrientations::Portrait ||
				m_orientation == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped;
			m_renderTargetWidth = swapDimensions ? windowHeight : windowWidth;
			m_renderTargetHeight = swapDimensions ? windowWidth : windowHeight;
			m_pSwapChain = context->CreateSwapChain (m_renderTargetWidth,m_renderTargetHeight);
			m_pdsView = context->CreateDepthStencilView (m_pSwapChain,m_renderTargetWidth,m_renderTargetHeight);
			m_prtView = context->CreateRenderTargetView (m_pSwapChain,m_renderTargetWidth,m_renderTargetHeight);
		}
		~RenderTargetD3D () {}
		

	private:
		unsigned int m_renderTargetWidth;
		unsigned int m_renderTargetHeight;
		SmartRenderContextD3D m_context;
		ID3D11RenderTargetView *m_prtView;
		ID3D11DepthStencilView *m_pdsView;
		IDXGISwapChain1        *m_pSwapChain;
	public:
		bool isValid () { return true; }
		void setSize (unsigned int width, unsigned int height) ;
		void getSize( unsigned int &width,unsigned int &height) const ;
		nvsg::TextureHostSharedPtr getTextureHost (nvsg::Image::PixelFormat,nvsg::Image::PixelDataType);
		static nvutil::SmartPtr<RenderTargetD3D> create (SmartRenderContextD3D &context) { return nvutil::SmartPtr<RenderTargetD3D>(new RenderTargetD3D(context)); }
		void clearRenderTargetView ( )
		{
			if (m_context && m_prtView)
			{
				m_context->clearRTV (m_prtView);
			}
		}
		void clearDepthStencilView ()
		{
			if (m_context && m_pdsView)
			{
				m_context->clearDSV (m_pdsView);
			}

		}
		void Present ()
		{
			if (m_context)
			{
				m_context->Present (m_pSwapChain,m_prtView,m_pdsView);
			}
		}
		ID3D11RenderTargetView **GetRenderTargetView () { return &m_prtView; }
		ID3D11DepthStencilView *GetDepthStencilView () { return m_pdsView; }
	};
	 typedef nvutil::SmartPtr<RenderTargetD3D> SmartRenderTargetD3D;
	 inline void RenderTargetD3D::setSize (unsigned int width, unsigned int height) 
	 {
		 m_renderTargetHeight = height;
		 m_renderTargetWidth = width;
		 // recalc  dsv,rtv.
	 };
	 inline void RenderTargetD3D::getSize( unsigned int &width,unsigned int &height) const { width = m_renderTargetWidth; height = m_renderTargetHeight;}
	 inline nvsg::TextureHostSharedPtr RenderTargetD3D::getTextureHost (nvsg::Image::PixelFormat,nvsg::Image::PixelDataType){return 0;}
	 	/* nvui::SceneRenderer::SceneRenderer ( const SmartRenderTarget &renderTarget )
	 {
		 m_renderTarget = renderTarget;
	 }
	 void  nvui::Renderer::setRenderTarget (const SmartRenderTarget & target) 
					  {
						  m_renderTarget = target;
					  } */

	 class SceneRendererD3D : public nvui::SceneRenderer {
	 protected:

	SceneRendererD3D (const SmartRenderTargetD3D &renderTarget = SmartRenderTargetD3D()) 
	{  
		m_renderTarget = renderTarget; 
	}
		 ~SceneRendererD3D () {}
	 public:
		static nvutil::SmartPtr<SceneRendererD3D>  create ()
		 {
			 return nvutil::SmartPtr<SceneRendererD3D> (new SceneRendererD3D ());
		 }
	void setRenderTarget (const SmartRenderTargetD3D & target) 
					  {
						  m_renderTarget = target;
					  }

					//  void setViewState () { }
				void DrawRecursive (nvsg::NodeSharedPtr node,nvmath::Mat44f &mat)
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
								nvsg::GetNV3DBase().SetModelMatrix (mat);
								nvsg::GetNV3DBase().DrawBuffer ( indexBuffer, &vertexBuffer,ibdata->GetCount(),ibdata->GetFormat());
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
					  void render () {
						  nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->clearRenderTargetView ();
						  nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->clearDepthStencilView ();
						 
						  nvsg::GetNV3DBase().SetTargets (  nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->GetRenderTargetView(),
														nvutil::smart_cast<RenderTargetD3D>( m_renderTarget)->GetDepthStencilView());
						 nvsg::GetNV3DBase().SetDefaultShaders ();								
						  nvsg::NodeSharedPtr root = nvsg::SceneReadLock (m_scene)->getRootNode ();
						  nvmath::Mat44f m(true);
						  DrawRecursive (root,m);
					  }
	void setViewState ( nvsg::ViewStateSharedPtr vs) { m_viewState = vs;}
	void setScene (nvsg::SceneSharedPtr scene) { m_scene = scene; }
	 private:
		 nvsg::ViewStateSharedPtr m_viewState;
		 void doRender (const nvsg::ViewStateSharedPtr &vs,const nvui::SmartRenderTarget &rt) {}
	
	 };
	 typedef nvutil::SmartPtr<SceneRendererD3D> SmartSceneRendererD3D;

 } // namespace nvsg

#endif //SCENE_RENDERER_D3D
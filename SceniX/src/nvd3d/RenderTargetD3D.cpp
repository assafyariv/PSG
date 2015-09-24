#include "pch.h"
#include <nvd3d/RenderContextD3D.h>
#include <nvd3d/RenderTargetD3D.h>

#include "D3DBase.h"

namespace nvd3d {

		RenderTargetD3D::RenderTargetD3D (SmartRenderContextD3D &context)
		{
			m_context = context;
			m_renderTargetWidth = 0;
			m_renderTargetHeight = 0;
			float i_windowWidth,i_windowHeight;
			context->GetWindowBounds (i_windowWidth,i_windowHeight);
			// Calculate the necessary swap chain and render target size in pixels.
			float windowWidth = GetNV3DBase().ConvertDipsToPixels(i_windowWidth);
			float windowHeight = GetNV3DBase().ConvertDipsToPixels(i_windowHeight);

			// The width and height of the swap chain must be based on the window's
			// landscape-oriented width and height. If the window is in a portrait
			// orientation, the dimensions must be reversed.
#ifdef _PSG_WIN8_RT
			Windows::Graphics::Display::DisplayOrientations m_orientation = Windows::Graphics::Display::DisplayProperties::CurrentOrientation;
			bool swapDimensions =
				m_orientation == Windows::Graphics::Display::DisplayOrientations::Portrait ||
				m_orientation == Windows::Graphics::Display::DisplayOrientations::PortraitFlipped;
#else
			bool swapDimensions = false;
#endif;
			m_renderTargetWidth = (unsigned int)(swapDimensions ? windowHeight : windowWidth);
			m_renderTargetHeight = (unsigned int)(swapDimensions ? windowWidth : windowHeight);
			m_pSwapChain = context->CreateSwapChain (m_renderTargetWidth,m_renderTargetHeight);
			m_pdsView = context->CreateDepthStencilView (m_pSwapChain,m_renderTargetWidth,m_renderTargetHeight);
			m_prtView = context->CreateRenderTargetView (m_pSwapChain,m_renderTargetWidth,m_renderTargetHeight);
		}
		RenderTargetD3D::~RenderTargetD3D () {}


		bool RenderTargetD3D::isValid () { return true; }
	   nvutil::SmartPtr<RenderTargetD3D> RenderTargetD3D::create (SmartRenderContextD3D &context) { return nvutil::SmartPtr<RenderTargetD3D>(new RenderTargetD3D(context)); }
		void RenderTargetD3D::clearRenderTargetView ( )
		{
			if (m_context && m_prtView)
			{
				m_context->clearRTV (m_prtView);
			}
		}
		void RenderTargetD3D::clearDepthStencilView ()
		{
			if (m_context && m_pdsView)
			{
				m_context->clearDSV (m_pdsView);
			}

		}
		void RenderTargetD3D::Present ()
		{
			if (m_context)
			{
				m_context->Present (m_pSwapChain,m_prtView,m_pdsView);
			}
		}
		ID3D11RenderTargetView **RenderTargetD3D::GetRenderTargetView () { return &m_prtView; }
		ID3D11DepthStencilView *RenderTargetD3D::GetDepthStencilView () { return m_pdsView; }

	typedef nvutil::SmartPtr<RenderTargetD3D> SmartRenderTargetD3D;
	inline void RenderTargetD3D::setSize (unsigned int width, unsigned int height) 
	{
		m_renderTargetHeight = height;
		m_renderTargetWidth = width;
		// recalc  dsv,rtv.
	};
	inline void RenderTargetD3D::getSize( unsigned int &width,unsigned int &height) const { width = m_renderTargetWidth; height = m_renderTargetHeight;}
	inline nvsg::TextureHostSharedPtr RenderTargetD3D::getTextureHost (nvsg::Image::PixelFormat,nvsg::Image::PixelDataType){return 0;}
}
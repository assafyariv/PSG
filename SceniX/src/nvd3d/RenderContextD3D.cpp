#include "pch.h"
#include "D3DBase.h"
#include <nvd3d/RenderContextD3D.h>

namespace nvd3d {
	void RenderContextD3D::Present (IDXGISwapChain1 *swapChain, ID3D11RenderTargetView 
		*rtv, ID3D11DepthStencilView *dsv)
	{
		nvd3d::GetNV3DBase().Present (swapChain,rtv,dsv);
	}
#ifdef _PSG_WIN8_RT
	SmartRenderContextD3D RenderContextD3D::CreateFromCoreWindow ( Windows::UI::Core::CoreWindow ^  window)
#else
	SmartRenderContextD3D RenderContextD3D::CreateFromHWND ( HWND  window)
#endif
	{
		RenderContextD3D *rcObj = new RenderContextD3D ();
		rcObj->m_window = window;
		SmartRenderContextD3D smartRC (rcObj);
		return smartRC;
	};
	void RenderContextD3D::clearRTV ( ID3D11RenderTargetView *rtv) {
		GetNV3DBase ().clearRTV (rtv);
	}
	void RenderContextD3D::clearDSV ( ID3D11DepthStencilView *dsv) {
		GetNV3DBase().clearDSV (dsv);
	}

	ID3D11RenderTargetView * RenderContextD3D::CreateRenderTargetView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY) 
	{
		ID3D11RenderTargetView *res = GetNV3DBase().CreateRenderTargetView (swapChain,targetX,targetY);
		return res;
	}
	ID3D11DepthStencilView * RenderContextD3D::CreateDepthStencilView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY) 
	{
		ID3D11DepthStencilView *res = GetNV3DBase().CreateDepthStencilView (swapChain,targetX,targetY);
		return res;
	}
	IDXGISwapChain1        * RenderContextD3D::CreateSwapChain (unsigned int targetX, unsigned int targetY) 
	{
#ifdef _PSG_WIN8_RT
		IDXGISwapChain1 *swapChain = GetNV3DBase().CreateSwapChain (targetX,targetY, m_window.Get());
#else
		IDXGISwapChain1 *swapChain = GetNV3DBase().CreateSwapChain (targetX,targetY, m_window);
#endif
		return swapChain;
	}
	void                    RenderContextD3D::ResizeSwapChain (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY)
	{
		GetNV3DBase().ResizeSwapChain (swapChain,targetX,targetY);
	};
	bool                    RenderContextD3D::GetWindowBounds (float &width, float &height) 
	{
#if !WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_DESKTOP_APP)
		width=m_window->Bounds.Width;
		height = m_window->Bounds.Height; 
#else
		width = 0;
		height = 0;
#endif
		return true;
	}
	void RenderContextD3D::makeCurrent () {}
}
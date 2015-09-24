#pragma once
#include <nvutil/RCObject.h>
#include <nvutil/SmartPtr.h>

namespace nvd3d {
class RenderContextD3D;
	typedef nvutil::SmartPtr<RenderContextD3D> SmartRenderContextD3D;
	class RenderContextD3D : public nvutil::RCObject
	{
	public:
		NVSG_API void Present (IDXGISwapChain1 *swapChain, ID3D11RenderTargetView 
			*rtv, ID3D11DepthStencilView *dsv)
		;
#if ! WINAPI_FAMILY_PARTITION(WINAPI_FAMILY_DESKTOP_APP)
		NVSG_API static SmartRenderContextD3D CreateFromCoreWindow ( Windows::UI::Core::CoreWindow ^  window)
		;
#else
		NVSG_API static SmartRenderContextD3D CreateFromHWND ( HWND  window)
		;
#endif
	protected:
		friend class RenderTargetD3D;
		void clearRTV ( ID3D11RenderTargetView *rtv) ;
		
		void clearDSV ( ID3D11DepthStencilView *dsv) ;
		

		ID3D11RenderTargetView * CreateRenderTargetView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY) 
		;
		ID3D11DepthStencilView * CreateDepthStencilView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY) 
		;
		IDXGISwapChain1        * CreateSwapChain (unsigned int targetX, unsigned int targetY) 
		;
		void                    ResizeSwapChain (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY)
		;
		bool                    GetWindowBounds (float &width, float &height) 
		;
		void makeCurrent () ;
	private:
#ifdef _PSG_WIN8_RT
		Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
#else
		HWND m_window;
#endif
	};
}
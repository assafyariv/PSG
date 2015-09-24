#pragma once

#include "DirectXHelper.h"

namespace nvd3d {
	// Helper class that initializes DirectX APIs for 3D rendering.
	class Direct3DBase2 
	{
	public:
		Direct3DBase2();

	public:
#ifndef _PSG_WIN8_RT
		virtual void Initialize(HWND window);
#else
				virtual void Initialize(Windows::UI::Core::CoreWindow^ window);
#endif
		virtual void HandleDeviceLost();
		virtual void CreateDeviceResources() ;
		virtual void CreateWindowSizeDependentResources() {};
		virtual void UpdateForWindowSizeChange() {};
		virtual void Render() = 0;
		virtual void Present() {};
		virtual float ConvertDipsToPixels(float dips);

	protected:
		// Direct3D Objects.
		Microsoft::WRL::ComPtr<ID3D11Device1> m_d3dDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext1> m_d3dContext;
		//Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;
		//	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView;
		//	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView;

		// Cached renderer properties.
		D3D_FEATURE_LEVEL m_featureLevel;
		//	Windows::Foundation::Size m_renderTargetSize;
#ifdef _PSG_WIN8_RT
	    Windows::Foundation::Rect m_windowBounds;
		Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
		Windows::Graphics::Display::DisplayOrientations m_orientation;
#else
		RECT m_windowBounds;
		HWND m_window;
		int m_orientation;
#endif
		// Transform used for display orientation.
		DirectX::XMFLOAT4X4 m_orientationTransform3D;
	};
}
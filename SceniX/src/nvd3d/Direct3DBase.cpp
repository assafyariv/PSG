#include "pch.h"
#include "Direct3DBase.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace Windows::Foundation;
#ifdef _PSG_WIN8_RT
using namespace Windows::UI::Core;
using namespace Windows::Graphics::Display;
#endif
namespace nvd3d{
	// Constructor.
	Direct3DBase2::Direct3DBase2()
	{
	}
#ifdef _PSG_WIN8_RT
	// Initialize the Direct3D resources required to run.
	void Direct3DBase2::Initialize(CoreWindow^ window)
	{
		m_window = window;

		CreateDeviceResources();
		CreateWindowSizeDependentResources();
	}
#else 
void Direct3DBase2::Initialize(HWND window)
	{
		m_window = window;

		CreateDeviceResources();
		CreateWindowSizeDependentResources();
	}
#endif
	// Recreate all device resources and set them back to the current state.
	void Direct3DBase2::HandleDeviceLost()
	{
#ifdef _PSG_WIN8_RT
		// Reset these member variables to ensure that UpdateForWindowSizeChange recreates all resources.
		m_windowBounds.Width = 0;
		m_windowBounds.Height = 0;
		//	m_swapChain = nullptr;
#else
		m_windowBounds.bottom = 0;
		m_windowBounds.left = 0;
		m_windowBounds.right = 0;
		m_windowBounds.top = 0;
	
#endif
		CreateDeviceResources();
		//UpdateForWindowSizeChange();
	}

	// These are the resources that depend on the device.
	void Direct3DBase2::CreateDeviceResources()
	{
		// This flag adds support for surfaces with a different color channel ordering
		// than the API default. It is required for compatibility with Direct2D.
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

#if defined(_DEBUG)
		// If the project is in a debug build, enable debugging via SDK Layers with this flag.
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

		// This array defines the set of DirectX hardware feature levels this app will support.
		// Note the ordering should be preserved.
		// Don't forget to declare your application's minimum required feature level in its
		// description.  All applications are assumed to support 9.1 unless otherwise stated.
		D3D_FEATURE_LEVEL featureLevels[] = 
		{
			D3D_FEATURE_LEVEL_11_1,
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		// Create the Direct3D 11 API device object and a corresponding context.
		ComPtr<ID3D11Device> device;
		ComPtr<ID3D11DeviceContext> context;
		DX::ThrowIfFailed(
			D3D11CreateDevice(
			nullptr, // Specify nullptr to use the default adapter.
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags, // Set set debug and Direct2D compatibility flags.
			featureLevels, // List of feature levels this app can support.
			ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION, // Always set this to D3D11_SDK_VERSION for Windows Store apps.
			&device, // Returns the Direct3D device created.
			&m_featureLevel, // Returns feature level of device created.
			&context // Returns the device immediate context.
			)
			);

		// Get the Direct3D 11.1 API device and context interfaces.
		DX::ThrowIfFailed(
			device.As(&m_d3dDevice)
			);

		DX::ThrowIfFailed(
			context.As(&m_d3dContext)
			);
	}

	// Method to convert a length in device-independent pixels (DIPs) to a length in physical pixels.
	float Direct3DBase2::ConvertDipsToPixels(float dips)
	{
		static const float dipsPerInch = 96.0f;
#ifdef _PSG_WIN8_RT
		return floor(dips * DisplayProperties::LogicalDpi / dipsPerInch + 0.5f); // Round to nearest integer.
#else
		return floor(dips * 14.0f / dipsPerInch + 0.5f); // Round to nearest integer.
#endif
	}
}
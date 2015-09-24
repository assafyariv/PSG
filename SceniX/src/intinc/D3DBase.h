#pragma once

#include <DirectXMath.h>
#include "Direct3DBase.h"
#include <nvmath\Matnnt.h>
using namespace DirectX;
namespace nvd3d {
	struct ModelViewProjectionConstantBuffer
	{
		DirectX::XMFLOAT4X4 model;
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
	};

	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	class NVD3DBase : public nvd3d::Direct3DBase2
	{
	public:
		NVD3DBase () { m_vsLoaded= false;m_psLoaded = false;}
		Microsoft::WRL::ComPtr<ID3D11VertexShader> m_defaultVS;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> m_defaultPS;
		Microsoft::WRL::ComPtr<ID3D11InputLayout> m_defaultInputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
		//uint32 m_indexCount;
		ModelViewProjectionConstantBuffer m_constantBufferData;
		bool m_vsLoaded;
		bool m_psLoaded;
		void SetModelMatrix (nvmath::Mat44f modelMatrix )
		{
			DirectX::CXMMATRIX fmat = DirectX::XMMatrixSet(modelMatrix[0][0],modelMatrix[0][1],modelMatrix[0][2],modelMatrix[0][3],modelMatrix[1][0],modelMatrix[1][1],modelMatrix[1][2],
				modelMatrix[1][3],modelMatrix[2][0],modelMatrix[2][1],modelMatrix[2][2],modelMatrix[2][3],modelMatrix[3][0],modelMatrix[3][1],modelMatrix[3][2],modelMatrix[3][3]				);
			/*for (int i=0;i<4;i++)
			for (int k=0;k<4;k++)
			fmat.r[i][k] = modelMatrix[i][k];*/
			XMStoreFloat4x4(&m_constantBufferData.model, fmat);
			//			XMStoreFloat4x4(&m_constantBufferData.view, fmat);
			//	SetDefaultShaders();
			m_d3dContext->UpdateSubresource(
				m_constantBuffer.Get(),
				0,
				NULL,
				&m_constantBufferData,
				0,
				0
				);
		}
		void SetTargets (ID3D11RenderTargetView **rtv,ID3D11DepthStencilView *dsv, float width, float height)
		{
			// Set the rendering viewport to target the entire window.
			CD3D11_VIEWPORT viewport(
				0.0f,
				0.0f,
				width,
				height
				);

			m_d3dContext->RSSetViewports(1, &viewport);
			XMVECTOR eye = XMVectorSet(0.0f, 0.7f, 1.5f, 0.0f);
			XMVECTOR at = XMVectorSet(0.0f, -0.1f, 0.0f, 0.0f);
			XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

			XMStoreFloat4x4(&m_constantBufferData.view, XMMatrixTranspose(XMMatrixLookAtRH(eye, at, up)));
			XMStoreFloat4x4(&m_constantBufferData.model, XMMatrixTranspose(XMMatrixRotationY(60.05f * XM_PIDIV4)));


			float aspectRatio = width/height;
			float fovAngleY = 70.0f * 3.14f / 180.0f;

			// Note that the m_orientationTransform3D matrix is post-multiplied here
			// in order to correctly orient the scene to match the display orientation.
			// This post-multiplication step is required for any draw calls that are
			// made to the swap chain render target. For draw calls to other targets,
			// this transform should not be applied.
			//nvmath::Mat44f mat (true);

			XMStoreFloat4x4(
				&m_constantBufferData.projection,
				DirectX::XMMatrixTranspose(
				DirectX::XMMatrixMultiply(
				DirectX::XMMatrixPerspectiveFovRH(
				fovAngleY,
				aspectRatio,
				0.01f,
				100.0f
				),
				XMLoadFloat4x4(&m_orientationTransform3D)
				))
				);
			m_d3dContext->OMSetRenderTargets(
				1,
				rtv,
				dsv
				);

		}
		void clearRTV (ID3D11RenderTargetView *rtv) 
		{
			const float midnightBlue[] = { 0.698f, 0.098f, 0.439f, 1.000f };
			m_d3dContext->ClearRenderTargetView(
				rtv,
				midnightBlue
				);


		}
		void clearDSV (ID3D11DepthStencilView *dsv)
		{
			m_d3dContext->ClearDepthStencilView(
				dsv,
				D3D11_CLEAR_DEPTH,
				1.0f,
				0
				);
		}
		void Present (IDXGISwapChain1 *swapChain, ID3D11RenderTargetView 
			*rtv, ID3D11DepthStencilView *dsv)
		{
			// The application may optionally specify "dirty" or "scroll"
			// rects to improve efficiency in certain scenarios.
			DXGI_PRESENT_PARAMETERS parameters = {0};
			parameters.DirtyRectsCount = 0;
			parameters.pDirtyRects = nullptr;
			parameters.pScrollRect = nullptr;
			parameters.pScrollOffset = nullptr;

			// The first argument instructs DXGI to block until VSync, putting the application
			// to sleep until the next VSync. This ensures we don't waste any cycles rendering
			// frames that will never be displayed to the screen.
			HRESULT hr = swapChain->Present1(1, 0, &parameters);

			// Discard the contents of the render target.
			// This is a valid operation only when the existing contents will be entirely
			// overwritten. If dirty or scroll rects are used, this call should be removed.
			m_d3dContext->DiscardView(rtv);//.Get());

			// Discard the contents of the depth stencil.
			m_d3dContext->DiscardView(dsv);//.Get());

			// If the device was removed either by a disconnect or a driver upgrade, we 
			// must recreate all device resources.
			if (hr == DXGI_ERROR_DEVICE_REMOVED)
			{
				HandleDeviceLost();
			}
			else
			{
				DX::ThrowIfFailed(hr);
			}
		}

		void CreateDefaultShaders () 
		{
#ifdef _PSG_WIN8_RT
			auto loadVSTask = DX::ReadDataAsync("SimpleVertexShader.cso");
			auto loadPSTask = DX::ReadDataAsync("SimplePixelShader.cso");

			auto createVSTask = loadVSTask.then([this](Platform::Array<byte>^ fileData) {
				DX::ThrowIfFailed(
					m_d3dDevice->CreateVertexShader(
					fileData->Data,
					fileData->Length,
					nullptr,
					&m_defaultVS
					)
					);

				const D3D11_INPUT_ELEMENT_DESC vertexDesc[] = 
				{
					{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
					{ "COLOR",    0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				};

				DX::ThrowIfFailed(
					m_d3dDevice->CreateInputLayout(
					vertexDesc,
					ARRAYSIZE(vertexDesc),
					fileData->Data,
					fileData->Length,
					&m_defaultInputLayout
					)
					);
				m_vsLoaded = true;
			});

			auto createPSTask = loadPSTask.then([this](Platform::Array<byte>^ fileData) {
				DX::ThrowIfFailed(
					m_d3dDevice->CreatePixelShader(
					fileData->Data,
					fileData->Length,
					nullptr,
					&m_defaultPS
					)
					);

				CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelViewProjectionConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
				DX::ThrowIfFailed(
					m_d3dDevice->CreateBuffer(
					&constantBufferDesc,
					nullptr,
					&m_constantBuffer
					)
					);
				m_psLoaded = true;
			});
#endif
		}
		void DrawBuffer ( ID3D11Buffer *indexBuffer, ID3D11Buffer **vertexBuffer,unsigned int indexCount, unsigned int indexFormat)
		{

			UINT stride = sizeof(VertexPositionColor);
			UINT offset = 0;
			m_d3dContext->IASetVertexBuffers(
				0,
				1,
				vertexBuffer,
				&stride,
				&offset
				);

			m_d3dContext->IASetIndexBuffer(
				indexBuffer,
				//DXGI_FORMAT_R16_UINT,
				(DXGI_FORMAT)indexFormat,
				0
				);
			m_d3dContext->DrawIndexed(
				indexCount,
				0,
				0
				);
		}
		void SetDefaultShaders ()
		{

			while (!m_vsLoaded || !m_psLoaded)
			{

			}
			m_d3dContext->UpdateSubresource(
				m_constantBuffer.Get(),
				0,
				NULL,
				&m_constantBufferData,
				0,
				0
				);

			m_d3dContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

			m_d3dContext->IASetInputLayout(m_defaultInputLayout.Get());

			m_d3dContext->VSSetShader(
				m_defaultVS.Get(),
				nullptr,
				0
				);

			m_d3dContext->VSSetConstantBuffers(
				0,
				1,
				m_constantBuffer.GetAddressOf()
				);

			m_d3dContext->PSSetShader(
				m_defaultPS.Get(),
				nullptr,
				0
				);



		}
		ID3D11RenderTargetView * CreateRenderTargetView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY)
		{
			ID3D11RenderTargetView *i_rtv = NULL;
			// Create a render target view of the swap chain back buffer.
			Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
			DX::ThrowIfFailed(
				swapChain->GetBuffer(
				0,
				__uuidof(ID3D11Texture2D),
				&backBuffer
				)
				);

			DX::ThrowIfFailed(
				m_d3dDevice->CreateRenderTargetView(
				backBuffer.Get(),
				nullptr,
				&i_rtv
				)
				);
			return i_rtv;
		}
		ID3D11DepthStencilView *CreateDepthStencilView (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY)
		{
			ID3D11DepthStencilView *i_dsv = NULL;
			// Create a depth stencil view.
			CD3D11_TEXTURE2D_DESC depthStencilDesc(
				DXGI_FORMAT_D24_UNORM_S8_UINT, 
				static_cast<UINT>(targetX),
				static_cast<UINT>(targetY),
				1,
				1,
				D3D11_BIND_DEPTH_STENCIL
				);

			Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencil;
			DX::ThrowIfFailed(
				m_d3dDevice->CreateTexture2D(
				&depthStencilDesc,
				nullptr,
				&depthStencil
				)
				);

			CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
			DX::ThrowIfFailed(
				m_d3dDevice->CreateDepthStencilView(
				depthStencil.Get(),
				&depthStencilViewDesc,
				&i_dsv
				)
				);
			return i_dsv;
		}

		void   ResizeSwapChain (IDXGISwapChain1 *swapChain, unsigned int targetX, unsigned int targetY)
		{
			// If the swap chain already exists, resize it.
			DX::ThrowIfFailed(
				swapChain->ResizeBuffers(
				2, // Double-buffered swap chain.
				static_cast<UINT>(targetX),
				static_cast<UINT>(targetY),
				DXGI_FORMAT_B8G8R8A8_UNORM,
				0
				)
				);
		};
#ifdef _PSG_WIN8_RT
		IDXGISwapChain1 *CreateSwapChain (unsigned int targetX, unsigned int targetY,Windows::UI::Core::CoreWindow^ window )
		{
			IDXGISwapChain1 *swapChain;
			// Otherwise, create a new one using the same adapter as the existing Direct3D device.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
			swapChainDesc.Width = static_cast<UINT>(targetX); // Match the size of the window.
			swapChainDesc.Height = static_cast<UINT>(targetY);
			swapChainDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM; // This is the most common swap chain format.
			swapChainDesc.Stereo = false;
			swapChainDesc.SampleDesc.Count = 1; // Don't use multi-sampling.
			swapChainDesc.SampleDesc.Quality = 0;
			swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			swapChainDesc.BufferCount = 2; // Use double-buffering to minimize latency.
			swapChainDesc.Scaling = DXGI_SCALING_NONE;
			swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL; // All Windows Store apps must use this SwapEffect.
			swapChainDesc.Flags = 0;

			Microsoft::WRL::ComPtr<IDXGIDevice1>  dxgiDevice;
			DX::ThrowIfFailed(
				m_d3dDevice.As(&dxgiDevice)
				);

			Microsoft::WRL::ComPtr<IDXGIAdapter> dxgiAdapter;
			DX::ThrowIfFailed(
				dxgiDevice->GetAdapter(&dxgiAdapter)
				);

			Microsoft::WRL::ComPtr<IDXGIFactory2> dxgiFactory;
			DX::ThrowIfFailed(
				dxgiAdapter->GetParent(
				__uuidof(IDXGIFactory2), 
				&dxgiFactory
				)
				);

			//Windows::UI::Core::CoreWindow^ window = m_window.Get();
			DX::ThrowIfFailed(
				dxgiFactory->CreateSwapChainForCoreWindow(
				m_d3dDevice.Get(),
				reinterpret_cast<IUnknown*>(window),
				&swapChainDesc,
				nullptr, // Allow on all displays.
				&swapChain
				)
				);

			// Ensure that DXGI does not queue more than one frame at a time. This both reduces latency and
			// ensures that the application will only render after each VSync, minimizing power consumption.
			DX::ThrowIfFailed(
				dxgiDevice->SetMaximumFrameLatency(1));

			// Set the proper orientation for the swap chain, and generate the
			// 3D matrix transformation for rendering to the rotated swap chain.
			m_orientation = Windows::Graphics::Display::DisplayProperties::CurrentOrientation;
			DXGI_MODE_ROTATION rotation = DXGI_MODE_ROTATION_UNSPECIFIED;
			switch (m_orientation)
			{
			case Windows::Graphics::Display::DisplayOrientations::Landscape:
				rotation = DXGI_MODE_ROTATION_IDENTITY;
				m_orientationTransform3D = XMFLOAT4X4( // 0-degree Z-rotation
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;

			case Windows::Graphics::Display::DisplayOrientations::Portrait:
				rotation = DXGI_MODE_ROTATION_ROTATE270;
				m_orientationTransform3D = XMFLOAT4X4( // 90-degree Z-rotation
					0.0f, 1.0f, 0.0f, 0.0f,
					-1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;

			case Windows::Graphics::Display::DisplayOrientations::LandscapeFlipped:
				rotation = DXGI_MODE_ROTATION_ROTATE180;
				m_orientationTransform3D = XMFLOAT4X4( // 180-degree Z-rotation
					-1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, -1.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;

			case Windows::Graphics::Display::DisplayOrientations::PortraitFlipped:
				rotation = DXGI_MODE_ROTATION_ROTATE90;
				m_orientationTransform3D = XMFLOAT4X4( // 270-degree Z-rotation
					0.0f, -1.0f, 0.0f, 0.0f,
					1.0f, 0.0f, 0.0f, 0.0f,
					0.0f, 0.0f, 1.0f, 0.0f,
					0.0f, 0.0f, 0.0f, 1.0f
					);
				break;

			default:
				throw ref new Platform::FailureException();
			}
			DX::ThrowIfFailed(
				swapChain->SetRotation(rotation)
				);
			return swapChain;
		}
#else
IDXGISwapChain1 *CreateSwapChain (unsigned int targetX, unsigned int targetY,HWND window )
{
	return 0;
}
#endif
		//Microsoft::WRL::ComPtr<ID3D11Device> getDevice () { return m_d3dDevice;}
		ID3D11Buffer *CreateVertexBuffer (unsigned int ssize, unsigned int type
			, const void * data, unsigned int strideInBytes, unsigned int count)
		{
			ID3D11Buffer *pBuf = NULL;
			D3D11_SUBRESOURCE_DATA vertexBufferData = {0};
			vertexBufferData.pSysMem = data;
			vertexBufferData.SysMemPitch = 0;
			vertexBufferData.SysMemSlicePitch = 0;
			CD3D11_BUFFER_DESC vertexBufferDesc(ssize, D3D11_BIND_VERTEX_BUFFER);
			DX::ThrowIfFailed(
				m_d3dDevice->CreateBuffer(
				&vertexBufferDesc,
				&vertexBufferData,
				&pBuf)
				);
			return pBuf;
		}
		ID3D11Buffer *CreateIndexBuffer (unsigned int ssize, const void * sdata, const int &byteSize)
		{
			ID3D11Buffer *pBuf = NULL;
			D3D11_SUBRESOURCE_DATA indexBufferData = {0};
			indexBufferData.pSysMem = sdata;
			indexBufferData.SysMemPitch = 0;
			indexBufferData.SysMemSlicePitch = 0;
			CD3D11_BUFFER_DESC indexBufferDesc(ssize*byteSize, D3D11_BIND_INDEX_BUFFER);
			DX::ThrowIfFailed(
				m_d3dDevice->CreateBuffer(
				&indexBufferDesc,
				&indexBufferData,
				&pBuf
				)
				);
			return pBuf;
		}

		virtual	void Render () override {};
	};
	NVD3DBase &GetNV3DBase() ;
}
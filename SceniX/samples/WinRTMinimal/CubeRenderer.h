#pragma once

#include <nvsg\Scene.h>
#include <nvd3d/SceneRendererD3D.h>

// This class renders a simple spinning cube.
ref class CubeRenderer2 sealed// : public Direct3DBase
{
public:
	CubeRenderer2();
	void Present ();
	void Initialize(Windows::UI::Core::CoreWindow^ window);
	// Direct3DBase methods.
	virtual void CreateDeviceResources() ;//override;
	virtual void CreateWindowSizeDependentResources() ;//override;
	virtual void Render() ;//override;
	
	// Method for updating time-dependent objects.
	void Update(float timeTotal, float timeDelta);

private:
	bool m_loadingComplete;
	Platform::Agile<Windows::UI::Core::CoreWindow> m_window;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_indexBuffer;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	nvsg::TransformSharedPtr m_root;
	nvsg::SceneSharedPtr  m_scene;
	nvd3d::SmartSceneRendererD3D m_renderer;
	nvd3d::SmartRenderContextD3D m_d3dContext;
	nvd3d::SmartRenderTargetD3D m_renderTarget;
	nvsg::ViewStateSharedPtr m_viewState;
};

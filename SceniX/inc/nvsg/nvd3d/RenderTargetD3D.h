//#include "pch.h"
#include <nvui/RenderTarget.h>

namespace nvd3d{

	class RenderTargetD3D : public nvui::RenderTarget{
	protected:
		RenderTargetD3D (SmartRenderContextD3D &context)
		;
		~RenderTargetD3D ();


	private:
		unsigned int m_renderTargetWidth;
		unsigned int m_renderTargetHeight;
		SmartRenderContextD3D m_context;
		ID3D11RenderTargetView *m_prtView;
		ID3D11DepthStencilView *m_pdsView;
		IDXGISwapChain1        *m_pSwapChain;
	public:
		bool isValid () ;
		void setSize (unsigned int width, unsigned int height) ;
		void getSize( unsigned int &width,unsigned int &height) const ;
		nvsg::TextureHostSharedPtr getTextureHost (nvsg::Image::PixelFormat,nvsg::Image::PixelDataType);
		NVSG_API static nvutil::SmartPtr<RenderTargetD3D> create (SmartRenderContextD3D &context) ;
		void clearRenderTargetView ( );
		void clearDepthStencilView ();
		NVSG_API void Present ()
		;
		ID3D11RenderTargetView **GetRenderTargetView () ;
		ID3D11DepthStencilView *GetDepthStencilView () ;
	};
	typedef nvutil::SmartPtr<RenderTargetD3D> SmartRenderTargetD3D;
	}
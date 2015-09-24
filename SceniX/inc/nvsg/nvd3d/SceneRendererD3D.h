#pragma once

#include <nvutil/SmartPtr.h>

#include <nvui/RenderTarget.h>
#include <nvui/SceneRenderer.h>
#include <nvutil/RCObject.h>
#include <nvsg/geoNode.h>
#include <nvsg/Transform.h>

#include <nvsg/Primitive.h>
#include <nvd3d/RenderContextD3D.h>
#include <nvd3d/RenderTargetD3D.h>
namespace nvd3d {

	class SceneRendererD3D : public nvui::SceneRenderer {
	protected:

		SceneRendererD3D (const SmartRenderTargetD3D &renderTarget = SmartRenderTargetD3D()) 
		;
		~SceneRendererD3D () ;
	public:
		NVSG_API static nvutil::SmartPtr<SceneRendererD3D>  create ()
		;
		NVSG_API void setRenderTarget (const SmartRenderTargetD3D & target) 
		;

		//  void setViewState () { }
		NVSG_API void DrawRecursive (nvsg::NodeSharedPtr node,nvmath::Mat44f &mat)
		;
		NVSG_API void render () ;
		NVSG_API void setViewState ( nvsg::ViewStateSharedPtr vs) ;
		NVSG_API void setScene (nvsg::SceneSharedPtr scene) ;
	private:
		nvsg::ViewStateSharedPtr m_viewState;
		void doRender (const nvsg::ViewStateSharedPtr &vs,const nvui::SmartRenderTarget &rt);

	};
	typedef nvutil::SmartPtr<SceneRendererD3D> SmartSceneRendererD3D;

} // namespace nvsg


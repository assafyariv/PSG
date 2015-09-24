// Copyright NVIDIA Corporation 2010
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#pragma once

#include <nvsg/CoreTypes.h>
#include <nvui/SceneRenderer.h>
#include <nvui/RenderTarget.h>
#include <nvrt/SceneRendererRTBuiltin.h>
#include <nvrt/RenderTargetRTBuiltin.h>

namespace nvrt {
  class SceneRendererRTBuiltinGL : public nvrt::SceneRendererRTBuiltin
  {
  public:
    // FIXME add support for shared raytracing context once supported
    NVRT_API static nvutil::SmartPtr<SceneRendererRTBuiltinGL> create( const nvrt::RTContextSharedPtr & rtContext = nvrt::RTContextSharedPtr()
                                                                     , const nvui::SmartRenderTarget &renderTarget = nvui::SmartRenderTarget() );
    NVRT_API virtual ~SceneRendererRTBuiltinGL();

    NVRT_API SmartRenderTargetRTBuiltin getRenderTargetRT() const;
  protected:
    NVRT_API virtual void beginRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );
    NVRT_API virtual void endRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );

    NVRT_API virtual void doRender( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );

    /** \brief Constructor for a raytracing renderer with OpenGL output.
     \param rtContext If handle is valid it's used by the Renderer. Otherwise a new RTContext is being created.
     **/
    NVRT_API SceneRendererRTBuiltinGL( const nvrt::RTContextSharedPtr &rtContext, const nvui::SmartRenderTarget &renderTarget );

  private:
    bool m_initialized;
    nvrt::RTBufferGLTextureSharedPtr m_outputBuffer[2];

    nvrt::SmartRenderTargetRTBuiltin  m_renderTargetRT;
    nvrt::RTContextSharedPtr          m_rtContext; // should be passed from outside
  };

  typedef nvutil::SmartPtr<SceneRendererRTBuiltinGL> SmartSceneRendererRTBuiltinGL;
} // namespace nvrt

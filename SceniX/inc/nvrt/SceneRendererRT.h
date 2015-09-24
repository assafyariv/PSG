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

#include <nvui/SceneRenderer.h>
#include <nvrt/RenderTargetRT.h>
#include <nvrt/RTTraverser.h>

namespace nvrt
{

  class SceneRendererRT : public nvui::SceneRenderer
  {
  protected:
    NVRT_API SceneRendererRT( const SmartRenderTargetRT &renderTarget = SmartRenderTargetRT() );
  public:
    NVRT_API static nvutil::SmartPtr<SceneRendererRT> create( const SmartRenderTargetRT &renderTarget = SmartRenderTargetRT() );
    NVRT_API virtual ~SceneRendererRT();

    // Renderer interface
    NVRT_API void setEntryPointIndex( unsigned int entryPoint );
    NVRT_API unsigned int getEntryPointIndex() const;

    NVRT_API void setStackSize( RTsize stackSize );
    NVRT_API RTsize getStackSize() const;

    NVRT_API void setAccelerationCache( const nvrt::RTAccelerationCacheSharedPtr &ach );
  protected:
    NVRT_API virtual void beginRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );
    NVRT_API virtual void endRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );

    NVRT_API virtual void doRender( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );

    unsigned int                       m_entryPointIndex;
    nvrt::RTAccelerationCacheSharedPtr m_accelerationCache;
    RTsize                             m_stackSize;
  };

  typedef nvutil::SmartPtr<SceneRendererRT> SmartSceneRendererRT;

} // namespace nvrt

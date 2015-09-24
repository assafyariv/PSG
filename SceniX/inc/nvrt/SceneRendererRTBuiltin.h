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

#include <nvrt/SceneRendererRT.h>
#include <nvrt/RenderTargetRT.h>
#include <nvrt/RTTraverser.h>

namespace nvrt
{

    /** \Brief This SceneRenderer does work RenderTargetRTBuiltin RenderTargets **/
  class SceneRendererRTBuiltin : public nvrt::SceneRendererRT
  {
  public:
    NVRT_API static nvutil::SmartPtr<SceneRendererRTBuiltin> create( const SmartRenderTargetRT &renderTarget = SmartRenderTargetRT() );

    NVRT_API virtual ~SceneRendererRTBuiltin();

    /** \brief Signals the renderer that a complete new image is going to be rendered. **/
    NVRT_API virtual void restartAccumulation();

    NVRT_API void setSceneEpsilonFactor( float factor );
    NVRT_API float getSceneEpsilonFactor() const;
    NVRT_API void setMaxReflectionDepth( unsigned int depth );
    NVRT_API unsigned int getMaxReflectionDepth() const;
    NVRT_API void setMaxRefractionDepth( unsigned int depth );
    NVRT_API unsigned int getMaxRefractionDepth() const;
    NVRT_API void setMaxShadowDepth( unsigned int depth );
    NVRT_API unsigned int getMaxShadowDepth() const;
    NVRT_API void setIterationIndex( unsigned int index );
    NVRT_API unsigned int getIterationIndex() const;

  protected:
    NVRT_API virtual void beginRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );
    NVRT_API virtual void endRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );

    NVRT_API virtual void doRender( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );
  
    NVRT_API SceneRendererRTBuiltin( const SmartRenderTargetRT &renderTarget );

  private:
    float m_sceneEpsilonFactor;
    int   m_maxReflectionDepth;
    int   m_maxRefractionDepth;
    int   m_maxShadowDepth;
    unsigned int m_iterationIndex;

    RTsize m_stackSize;

    std::set<RenderTargetRT *> m_knownRenderTargets; //!< keep a list of prepared RenderTargets

  };

  typedef nvutil::SmartPtr<SceneRendererRTBuiltin> SmartSceneRendererRTBuiltin;

} // namespace nvrt

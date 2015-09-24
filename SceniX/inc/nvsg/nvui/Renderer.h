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
#include <nvsg/Scene.h>
#include <nvsg/ViewState.h>
#include <nvui/RenderTarget.h>
#include <nvutil/SmartPtr.h>

namespace nvui
{

  /** \brief Renderer is the base class for algorithms which operate on an nvui::RenderTarget.
  **/
  class Renderer : public nvutil::Reflection, public nvutil::RCObject
  {
  protected:
    NVSG_API Renderer( const SmartRenderTarget &renderTarget = SmartRenderTarget() );
  public:
    NVSG_API virtual ~Renderer();

    /** \brief Set the nvui::RenderTarget used by all subsequent render calls. 
        \param renderTarget The new nvui::RenderTarget.
    **/
    NVSG_API void setRenderTarget( const SmartRenderTarget &renderTarget );

    /** \brief Get the nvui::RenderTarget used by render calls.
        \return The current nvui::RenderTarget.
    **/
    NVSG_API SmartRenderTarget getRenderTarget() const;

    /** \brief Executes the rendering algorithm on the given nvui::RenderTarget. It calls beginRendering,
               doRender and endRendering with an nvui::RenderTarget.
        \param renderTarget nvui::RenderTarget to render on. If this parameter is specified
                            it overrides the nvui::RenderTarget set by setRenderTarget. Otherwise
                            the previously specified nvui::RenderTarget is used.
    **/
    NVSG_API void render( const SmartRenderTarget &renderTarget = SmartRenderTarget() );

    /** \brief Signals the renderer that a complete new image is going to be rendered. **/
    NVSG_API virtual void restartAccumulation();
  protected:
    /** \brief This function is called once per render call before the first doRender call
        \param nvui::renderTarget SmartRenderTarget which had been passed to the render call
    **/
    NVSG_API virtual void beginRendering( const SmartRenderTarget &renderTarget = SmartRenderTarget() );

    /** \brief This function is called once per render call after the last doRender call
        \param renderTarget nvui::SmartRenderTarget which had been passed to the render call
    **/
    NVSG_API virtual void endRendering( const SmartRenderTarget &renderTarget = SmartRenderTarget() );

    /** \brief Override this function to implement a rendering algorithm.
        \param renderTarget nvui::RenderTarget determined by the render call.
    **/
    NVSG_API virtual void doRender( const SmartRenderTarget &renderTarget ) = 0;
 // private:
    SmartRenderTarget m_renderTarget;
  };

  typedef nvutil::SmartPtr<Renderer> SmartRenderer;

}  // namespace nvui

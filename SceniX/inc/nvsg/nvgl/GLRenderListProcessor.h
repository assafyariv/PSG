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

#include <nvsg/IteratingRenderListProcessor.h>
#include <nvgl/RenderTargetGL.h>
#include <nvsg/ViewState.h>

#include <nvgl/GLObjectRenderer.h>

namespace nvgl
{

  

class GLRenderListProcessor;
typedef nvutil::SmartPtr< GLRenderListProcessor > GLRenderListProcessorSharedPtr;
class GLRenderListProcessor : public nvsg::IteratingRenderListProcessor
                            , public nvutil::RCObject
{
public:
  NVSG_API static GLRenderListProcessorSharedPtr create( const nvsg::ObjectRendererSharedPtr& );

public:
  NVSG_API virtual void setViewState( const nvsg::ViewStateSharedPtr& );

  NVSG_API virtual bool beginHandle( const nvsg::RenderList2SharedPtr& );
  NVSG_API virtual void endHandle  ( const nvsg::RenderList2SharedPtr& );

  NVSG_API virtual bool beginHandle( const nvsg::RenderModeGroupSharedPtr& );
  NVSG_API virtual void endHandle  ( const nvsg::RenderModeGroupSharedPtr& );

  NVSG_API virtual bool beginHandle        ( const nvsg::MaterialGroupSharedPtr& );
  NVSG_API virtual bool handleChildrenAgain( const nvsg::MaterialGroupSharedPtr& );
  NVSG_API virtual void endHandle          ( const nvsg::MaterialGroupSharedPtr& );
  
  NVSG_API virtual void doAddRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );
  NVSG_API virtual void doSetRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );

  NVSG_API virtual void handle( const nvsg::DrawableInstanceSharedPtr& );

protected:
  bool updateTransforms( nvsg::TransformTreeIndex transformIndex );

  friend struct nvutil::Holder<GLRenderListProcessor>;

  NVSG_API GLRenderListProcessor( const nvsg::ObjectRendererSharedPtr & );
  NVSG_API virtual ~GLRenderListProcessor();

  /*! Cache all passes of the given stateVariant in mgResouce **/
  NVSG_API void cacheMGResource( const nvsg::StateVariantSharedPtr& stateVariant, const MGResourceSharedPtr& mgResource );

  /*! Begin the current material pass of the given MGResource **/
  NVSG_API void beginRenderMGResource( const MGResourceSharedPtr& mgResource );

  /*! End the current material pass of the given MGResource **/
  NVSG_API void endRenderMGResource( const MGResourceSharedPtr& mgResource );

  // shortcuts to different render states
  nvsg::RenderState&    renderState()    { return *m_renderState; }
  nvsg::MaterialState&  materialState()  { return m_renderState->m_materialState; }
  nvsg::ShaderState&    shaderState()    { return m_renderState->m_materialState.m_shaderState; }
  nvsg::TransformState& transformState() { return m_renderState->m_transformState; }
  nvsg::LightState&     lightState()     { return m_renderState->m_lightState; }

protected:
  nvsg::ObjectRendererSharedPtr   m_objectRenderer;

private:
  void initOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );
  void initPropertyIds();

private:
  nvsg::RenderStateSharedPtr      m_renderState;
  nvsg::MaterialSharedPtr         m_defaultMaterial;

  nvsg::LightVector m_activeLights;
};

} // namespace nvgl




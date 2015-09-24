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

//#include <nvsg/nvsgcommon.h>
#include <nvsg/IteratingRenderListProcessor.h>
#include <nvgl/RenderTargetGL.h>
//#include <nvgl/GLContext.h>
#include <nvsg/ViewState.h>

//#include <GL/glu.h>
#include <map>

namespace nvgl
{
  /*
class ImmediateModeGLRenderListProcessor : public nvsg::IteratingRenderListProcessor
{
public:
  NVSG_API virtual void setRenderTarget( nvgl::SmartRenderTargetGL renderTarget );
  NVSG_API virtual void setViewState( const nvsg::ViewStateSharedPtr & viewState );

protected:
  NVSG_API virtual bool beginHandle( nvsg::SmartRenderList2 );
  NVSG_API virtual void endHandle  ( nvsg::SmartRenderList2 );

  NVSG_API virtual bool beginHandle( nvsg::SmartMaterialGroup );
  NVSG_API virtual bool handleAgain( nvsg::SmartMaterialGroup );
  NVSG_API virtual void endHandle  ( nvsg::SmartMaterialGroup );

  NVSG_API virtual bool beginHandle( nvsg::SmartDrawableInstance );
  NVSG_API virtual void endHandle  ( nvsg::SmartDrawableInstance );

  NVSG_API virtual bool beginHandle( nvsg::SmartTransformGroup );
  NVSG_API virtual void endHandle  ( nvsg::SmartTransformGroup );

  NVSG_API virtual void handle( nvsg::SmartGeometryInstance );

private:
  typedef std::map< nvsg::LightSourceSharedPtr, GLenum > LightMap;
  typedef LightMap::iterator LightMapIterator;
private:
  void renderCamera( const nvsg::Camera * cam );
  void renderPointLight( const nvsg::PointLight * light, bool enable );
  bool renderLightSource( const nvsg::LightSource * light, bool enable, GLenum & lightId );
  bool renderPass( const nvsg::StateVariantWeakPtr & stateVariant, unsigned int pass ); 

private:
  nvgl::SmartRenderTargetGL  m_renderTarget;
  nvsg::ViewStateSharedPtr m_viewState;

  LightMap     m_lights;
  unsigned int m_currentPass;
};
*/
} // namespace nvgl




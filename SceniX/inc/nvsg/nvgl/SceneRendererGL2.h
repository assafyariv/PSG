// Copyright NVIDIA Corporation 2010-2011
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

#include <nvgl/RenderTargetGL.h>
#include <nvgl/GLObjectRenderer.h>
#include <nvgl/GLRenderListProcessor.h>
#include <nvsg/RenderList2.h>
#include <nvutil/Reflection.h>
#include <nvutil/SmartPtr.h>

namespace nvgl
{
  /** \brief SceneRendererGL2 is an OpenGL 2.1 based renderer. The OpenGL context used during the first render call
            must not be changed for successive calls. It is necessary to remove all references to this object before
            the attached OpenGL context gets destroyed. Deleting this object after destroying the corresponding OpenGL
            context will result in a crash during resource cleanup. OpenGL resources like VBOs, display lists and 
            textures will be shared between multiple SceneRendererGL2 objects if the corresponding OpenGL contexts
            are being shared.
  **/
  class SceneRendererGL2 : public nvui::SceneRenderer
  {    
  protected:
    NVSG_API SceneRendererGL2( const SmartRenderTargetGL &renderTargetGL = SmartRenderTargetGL() );
    NVSG_API virtual ~SceneRendererGL2();

  public:
    /** \brief Create an instance of SceneRendererGL2
        \param renderTargetGL Default RenderTargetGL to use.
        \return An instance of a SceneRendererGL2 object.
    **/
    NVSG_API static nvutil::SmartPtr<SceneRendererGL2> create( const SmartRenderTargetGL &renderTargetGL = SmartRenderTargetGL() );

    /** \brief Add all supported options to the RendererOptions container.
        \param rendererOptions A container for RendererOptions 
    **/
    NVSG_API virtual void addRendererOptions( const nvui::RendererOptionsSharedPtr &rendererOptions );

    /*** RendererOptions ***/

    /** \brief CacheMode specifies the storage mode used for vertex and index data.\n
               ParameterType: nvgl::SceneRendererGL2::CacheMode\n
               Default Value: CACHEMODE_VBO
    **/
    NVSG_API static const std::string& getOptionStringCacheMode();

    /** \brief FaceCullMode specified which sides of a face should be culled.
               ParameterType: nvgl::SceneRendererGL2::FaceCullMode
               Default Value: CULLMODE_BACK
    **/
    NVSG_API static const std::string& getOptionStringFaceCullMode();
  
    /** \brief Enable OpenGL light model: "local viewer"\n
               ParameterType: bool\n
               Default Value: false
    **/
    NVSG_API static const std::string& getOptionStringLightModelLocalViewer();

    /** \brief Enable OpenGL light model: "two side"\n
               ParameterType: bool\n
               Default Value: false
    **/
    NVSG_API static const std::string& getOptionStringLightModelTwoSide();

    /** \brief Enable OpenGL light model: "separate specular color"\n
               ParameterType: bool\n
               Default Value: true
    **/
    NVSG_API static const std::string& getOptionStringSeparateSpecularColor();


    /** \brief Controls the depth mask when rendering transparent geometry.\n
               ParameterType: bool\n
               Default Value: false
    **/
    NVSG_API static const std::string& getOptionStringTransparencyDepthMask();

    /** \brief Enable frustum culling\n
               ParameterType: bool\n
               Default Value: true
    **/
    NVSG_API static const std::string& getOptionStringFrustumCulling();

    /** \brief Enable size culling\n
               ParameterType: bool\n
               Default Value: 
    **/
    NVSG_API static const std::string& getOptionStringSizeCulling();

    /** \brief Project size factor for size culling.\n
               ParameterType: float\n
               Default Value: 1.0
    **/
    NVSG_API static const std::string& getOptionStringSizeCullingFactor();

    /** \brief Set the RenderModeGroup to render. -1 specified all RenderModeGroups.\n
               ParameterType: int\n
               Default Value: -1
    **/
    NVSG_API static const std::string& getOptionStringRenderModeGroup();

    /** \brief TraversalMaskOverride will be or'ed to the traversal mask of each object during rendering.\n
               ParameterType: unsigned int\n
               Default Value: 0
    **/
    NVSG_API static const std::string& getOptionStringTraversalMaskOverride();

    /** \brief Enable tesselation hardware if supported.\n
               ParameterType: bool\n
               Default Value: true
    **/
    NVSG_API static const std::string& getOptionStringTessellationEnabled();

    /** \brief Choose Polygonmode.\n
               ParameterType: nvgl::SceneRenderer2::PolygonMode\n
               Default Value: POLYGONMODE_FACE
    **/
    NVSG_API static const std::string& getOptionStringPolygonMode();

    /** \brief Choose RenderTechnique\n
               ParameterType: nvgl::SceneRendererGL2::RenderTechnique\n
               Default Value: RENDERTECHNIQUE_EFFECTED
    **/
    NVSG_API static const std::string& getOptionStringRenderTechnique();

    /** \brief Tells the renderer to keep textures in host memory after uploading them to the GPU.\n
               ParameterType: bool\n
               Default Value: true
    **/
    NVSG_API static const std::string& getOptionStringPreserveTexturesAfterUpload();

  protected:
    /** \brief Delete all primitive caches. Call this function only if an OpenGL context is active since resources need
               to be deleted.
    **/
    NVSG_API void deletePrimitiveCaches();

    NVSG_API virtual void beginRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );
    NVSG_API virtual void endRendering( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );

    NVSG_API virtual void doRender( const nvsg::ViewStateSharedPtr &viewState, const nvui::SmartRenderTarget &renderTarget );

    NVSG_API virtual void beginRenderMapList( const nvsg::MapListSharedPtr& mapList, const nvui::SmartRenderTarget &renderTarget );
    NVSG_API virtual void endRenderMapList( const nvsg::MapListSharedPtr& mapList, const nvui::SmartRenderTarget &renderTarget );

    NVSG_API virtual void renderMapElement( const nvsg::MapElementSharedPtr& mapElement, const nvui::SmartRenderTarget &renderTarget );
    NVSG_API virtual void renderMapArray( const nvsg::MapArraySharedPtr& mapArray, const nvui::SmartRenderTarget &renderTarget );
    NVSG_API virtual void renderMapMatrix( const nvsg::MapMatrixSharedPtr& mapMatrix, const nvui::SmartRenderTarget &renderTarget );

  private:
    // helper function for map object rendering
    void drawImage( int x, int y, const nvsg::TextureHostSharedPtr & tih );

  private:
    // renderer info
    nvsg::NodeWeakPtr                    m_lastRoot;
    nvsg::RenderList2SharedPtr           m_renderList;
    nvgl::GLObjectRendererSharedPtr      m_objectRenderer;
    nvgl::GLRenderListProcessorSharedPtr m_renderListProcessor;

    GLObjectRenderer::CacheMode          m_lastCacheMode;

    nvgl::SmartRenderContextGL           m_userRenderContextGL;     // RenderContextGL provided by the user in the first render call
  };

  typedef nvutil::SmartPtr<SceneRendererGL2> SmartSceneRendererGL2;

}  // namespace nvgl

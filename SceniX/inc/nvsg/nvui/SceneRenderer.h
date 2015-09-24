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

#include <nvui/Renderer.h>

namespace nvui
{

  /** \brief SceneRenderer is the base class for all Renderers which visualize a scene. On a stereo
             nvui::RenderTarget stereo rendering is turned on automatically. SceniX supports a default implementation
             of nvui::SceneRenderer::StereoViewStateProvider which supports nvsg::PerspectiveCamera. For other
             cameras it is possible to specify the camera matrices for the left and right eye by implementing 
             a custom nvui::SceneRenderer::StereoViewStateProvider.
  **/
  class SceneRenderer : public Renderer
  {
  protected:
    /** \brief Constructor used by create **/
    NVSG_API SceneRenderer( const SmartRenderTarget &renderTarget = SmartRenderTarget() );
  public:
    /** \brief This class generates an nvsg::ViewState for the left and right eye based on a
               monoscopic nvsg::ViewState. 
    **/
    class StereoViewStateProvider : public RCObject
    {
    public:
      /** \brief Get an nvsg::ViewState for the given eye. Implement calculateViewState for custom behaviour.
          \param viewState Monoscopic nvsg::ViewState used as base for the stereo nvsg::ViewState.
          \param eye nvui::RenderTarget::LEFT or nvui::RenderTarget::RIGHT for the left or right eye, respectively.
          \return nvsg::ViewState for the given eye.
      **/
      NVSG_API nvsg::ViewStateSharedPtr getViewState( const nvsg::ViewStateSharedPtr &viewState, nvui::RenderTarget::StereoTarget eye );
    protected:
      /** \brief Calculate an nvsg::ViewState for the given eye. Override this function to implement custom behaviour.
          \param viewState Monoscopic nvsg::ViewState used as base for the stereo nvsg::ViewState.
          \param eye nvui::RenderTarget::LEFT or nvui::RenderTarget::RIGHT for the left or right eye.
          \return nvsg::ViewState for the given eye.
      **/
      NVSG_API virtual nvsg::ViewStateSharedPtr calculateViewState( const nvsg::ViewStateSharedPtr &viewState, nvui::RenderTarget::StereoTarget eye ) = 0;
    };

    /*! \brief Set an nvsg::ViewState to be used with this SceneRenderer.
     *  \param viewState The nvsg::ViewState to be used.
     *  \note The nvsg::ViewState's TraversalMask will be used in conjunction with any TraversalMaskOverride to direct
     *  scene traversal and rendering. See setTraversalMaskOverride for more info.
     *  \sa getViewState, setTraversalMaskOverride */
    NVSG_API void setViewState( const nvsg::ViewStateSharedPtr &viewState );

    /** \brief Get the default nvsg::ViewState used by render calls
        \return Default nvsg::ViewState used by render calls
    **/
    NVSG_API const nvsg::ViewStateSharedPtr & getViewState( ) const;

    /*! \brief Set the TraversalMask Override to be used with this SceneRenderer.
     *  \param mask The mask to be used.
     *  \remarks This method provides a way to set the traversal mask override for this SceneRenderer.
     *  \note \li The nvsg::ViewState's TraversalMask is used in conjuction with the OverrideTraversalMask and every scene graph node's 
     *  TraversalMask to determine whether nodes (and therefore possibly their entire subgraph) are rendered.  
     *  The scene renderer's override traversal mask is OR'd with the node's traversal mask and that result is ANDed with 
     *  the nvsg::ViewState's traversal mask. If the result is nonzero then the node is traversed/rendered, otherwise it is ignored.  IE:
     *  If ( ( (SceneRenderer::TraversalMaskOverride | Object::TraversalMask) & nvsg::ViewState::TraversalMask ) != 0 ) is true the node is
     *  rendered.
     *  \li Setting the nvsg::ViewState's TraversalMask to 0 will cause no nodes to be traversed.  Setting the nvsg::ViewState's TraversalMask 
     *  to ~0 and the TraversalMaskOverride to ~0 will cause all nodes to be traversed regardless of the Object::TraversalMask.
     *  \li The default traversal mask override is 0 so that it does not affect traversal/rendering.
     *  \sa getTraversalMaskOverride, nvsg::ViewState::setTraversalMask, Object::setTraversalMask */
    NVSG_API void setTraversalMaskOverride( unsigned int mask );

    /*! \brief Get the current TraversalMask override
     *  \return mask The mask in use.
     *  \sa setTraversalMaskOverride */
    NVSG_API unsigned int getTraversalMaskOverride() const;

    /**\brief Set the StereoViewState provider which should be used for stereo ViewState calculation.
       \param viewStateProvider A ViewStateProvider instance with desired behaviour.
    **/
    NVSG_API void setStereoViewStateProvider( const nvutil::SmartPtr<StereoViewStateProvider> &viewStateProvider );
    NVSG_API nvutil::SmartPtr<StereoViewStateProvider> getStereoViewStateProvider() const;

    /** \brief Render one frame with the given nvsg::ViewState and renderTarget. 
        \param viewState nvsg::ViewState to use to render the frame.
        \param renderTarget If renderTarget is valid it temporarily overrides the default set by Renderer::setRenderTarget.
    **/
    NVSG_API void render( const nvsg::ViewStateSharedPtr &viewState, const SmartRenderTarget &renderTarget = SmartRenderTarget(), RenderTarget::StereoTarget stereoTarget = RenderTarget::LEFT_AND_RIGHT );

    /** \brief Add all renderer options required by this renderer to the given nvui::RendererOptions object. It is possible
    *          to pass the same options object to multiple SceneRenderers as long as there are no property name collisions.
        \param rendererOptions Object which holds renderer options.
    **/
    NVSG_API virtual void addRendererOptions( const RendererOptionsSharedPtr &rendererOptions );

    REFLECTION_INFO_API( NVSG_API, SceneRenderer );
    BEGIN_DECLARE_STATIC_PROPERTIES
      NVSG_API DECLARE_STATIC_PROPERTY( PreserveTexturesAfterUpload );
      NVSG_API DECLARE_STATIC_PROPERTY( TraversalMaskOverride );
    END_DECLARE_STATIC_PROPERTIES

    using Renderer::render;
  protected:
    // preserve interface should be set using Reflection
    NVSG_API bool isPreserveTexturesAfterUpload() const;
    NVSG_API void setPreserveTexturesAfterUpload( bool onOff );

    NVSG_API virtual void beginRendering( const nvsg::ViewStateSharedPtr &viewState, const SmartRenderTarget &renderTarget );
    NVSG_API virtual void endRendering( const nvsg::ViewStateSharedPtr &viewState, const SmartRenderTarget &renderTarget );

    NVSG_API virtual void doRender( const SmartRenderTarget &renderTarget );

    /** \brief Interface for the actual rendering algorithm.
        \param viewState The nvsg::ViewState to use to render the frame.
        \param renderTarget The RenderTarget to use to render the frame.
    **/
    NVSG_API virtual void doRender( const nvsg::ViewStateSharedPtr &viewState, const SmartRenderTarget &renderTarget ) = 0;

    NVSG_API virtual void renderMapList( const nvsg::MapListSharedPtr& mapList, const SmartRenderTarget &renderTarget );
    NVSG_API virtual void beginRenderMapList( const nvsg::MapListSharedPtr& mapList, const SmartRenderTarget &renderTarget );
    NVSG_API virtual void endRenderMapList( const nvsg::MapListSharedPtr& mapList, const SmartRenderTarget &renderTarget );

    NVSG_API virtual void renderMapElement( const nvsg::MapElementSharedPtr& mapElement, const SmartRenderTarget &renderTarget );
    NVSG_API virtual void renderMapArray( const nvsg::MapArraySharedPtr& mapArray, const SmartRenderTarget &renderTarget );
    NVSG_API virtual void renderMapMatrix( const nvsg::MapMatrixSharedPtr& mapMatrix, const SmartRenderTarget &renderTarget );
    NVSG_API virtual void renderMapObject( const nvsg::MapObjectSharedPtr& mapObject, const SmartRenderTarget &renderTarget );

  protected:
    nvsg::SceneSharedPtr          m_scene;
    nvsg::ViewStateSharedPtr      m_viewState;
    bool                          m_preserveTexturesAfterUpload;
    nvui::RendererOptionsWeakPtr  m_rendererOptions;

  private:
    nvutil::SmartPtr<StereoViewStateProvider> m_stereoViewStateProvider;
    unsigned int m_traversalMaskOverride;
  };

  inline bool SceneRenderer::isPreserveTexturesAfterUpload() const
  {
    return m_preserveTexturesAfterUpload;
  }

  inline void SceneRenderer::setPreserveTexturesAfterUpload( bool onOff )
  {
    if ( m_preserveTexturesAfterUpload != onOff )
    {
      m_preserveTexturesAfterUpload = onOff;
      notify( PID_PreserveTexturesAfterUpload );
    }
  }

  inline unsigned int SceneRenderer::getTraversalMaskOverride() const
  {
    return m_traversalMaskOverride;
  }

  inline void SceneRenderer::setTraversalMaskOverride( unsigned int mask )
  {
    if ( m_traversalMaskOverride != mask )
    {
      m_traversalMaskOverride = mask;
      notify( PID_TraversalMaskOverride );
    }
  }

  inline void SceneRenderer::setStereoViewStateProvider( const nvutil::SmartPtr<SceneRenderer::StereoViewStateProvider> &viewStateProvider )
  {
    m_stereoViewStateProvider = viewStateProvider;
  }

  inline nvutil::SmartPtr<SceneRenderer::StereoViewStateProvider> SceneRenderer::getStereoViewStateProvider( ) const
  {
    return m_stereoViewStateProvider;
  }

  typedef nvutil::SmartPtr<SceneRenderer> SmartSceneRenderer;

} // namespace nvui

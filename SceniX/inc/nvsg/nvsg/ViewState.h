// Copyright NVIDIA Corporation 2002-2011
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
/** \file */

#include "nvsgcommon.h"
#include "nvsg/nvsg.h"
#include "nvsg/Camera.h"
#include "nvsg/MapList.h"
#include "nvsg/Scene.h"
#include "nvui/RenderTarget.h"
#include "nvutil/BackLinkObserver.h"
#include "nvutil/Reflection.h"

namespace nvsg
{
  /*! \brief The ViewState class encapsulates view-specific state. 
   *  \par Namespace: nvsg
   *  \remarks The ViewState encapsulates the view-specific information into one object.
   *  The ViewState defines how the user looks at the scene. It contains the camera and specific viewing
   *  settings like stereo view and does the bookkeeping of animation information.
   *  \n\n
   *  nvsg::ViewState and nvsg::Scene and nvui::RendererOptions are the three major components that need 
   *  to come together to produce a rendered image. 
   *  The scene contains basically the tree, the ViewState defines how to look at
   *  the tree, and the RendererOptions contains the parameters for the renderer which renders
   *  the final image.
   */
  class ViewState : public nvutil::HandledObject
  {
    public:
      NVSG_API static ViewStateSharedPtr create();

    public:
      /*! \brief Make a flat copy of another ViewState.
       *  \param rhs Source ViewState. */
      NVSG_API ViewState& operator=( const ViewState& rhs );   

      /*! \brief Test if there is currently an animation running.
       *  \return The return value of this function is true when an animation is currently running.
       *  \remarks This function can be used to determine the animation state of the view. 
       *  \sa setAnimating */
      NVSG_API bool isAnimating() const;
      
      /*! \brief Set the animation state of this view. 
       *  \param animating Parse in true to enable animation, otherwise false.
       *  \remarks This function controls the animation state of the view. Animation can be turned
       *  on and off.
       */
      NVSG_API void setAnimating( bool animating );

      /*! \brief Advance the animation to the next tick, using the specified step size.
       *  \param tick The tick for the next animation step.
       *  \param delta The number of steps to go from the previous tick.
       *  \return \c true, if animations currently are running, otherwise \c false.
       *  \remarks A ViewState feeds the \a tick and \a delta into the scene used.
       *  \sa getCurrentTick, getCurrentDelta */
      NVSG_API bool advanceAnimation( unsigned int tick, int delta = 1 );

      /*! \brief Get the current tick to use for animations.
       *  \return The current tick to feed into a scene for animation purposes.
       *  \remarks Each running Animation advances, when the current tick is different from its
       *  previous tick.
       *  \sa advanceAnimation, getCurrentDelta */
      NVSG_API unsigned int getCurrentTick() const;

      /*! \brief Get the current delta to use for animations.
       *  \return The current delta specifying the number of steps on a changing tick.
       *  \remarks Each running Animation advances by this delta steps per animation tick.
       *  \sa advanceAnimation, getCurrentTick */
      NVSG_API int getCurrentDelta() const;

      /*! \brief Sets the Camera for this view. 
       *  \param camera The Camera to set.
       *  \remarks The given camera is used for the rendering.
       *  \sa getCamera */
      NVSG_API void setCamera(const CameraSharedPtr & camera);

      /*! \brief Sets the RendererOptions for this view
          \param rendererOptions The RendererOptions to set
      */
      NVSG_API void setRendererOptions(const nvui::RendererOptionsSharedPtr &rendererOptions);

      /*! \brief Gets the RendererOptions for this view
          \param rendererOptions The RendererOptions to set
      */
      NVSG_API const nvui::RendererOptionsSharedPtr &getRendererOptions( ) const;

      /*! \brief Returns the current Camera. 
       *  \return The Camera which is used in this view. 
       *  If there is no Camera associated with this view, the function returns NULL.
       *  \sa setCamera */
      NVSG_API const CameraSharedPtr & getCamera() const;

      /*! \brief Get the distance to the target point.
       *  \return The distance from the current view point to the target point.
       *  \remarks The target point is    as the reference point for rotations of the camera.
       *  \sa setTargetDistance*/
      NVSG_API float getTargetDistance() const;

      /*! \brief Set the distance to the target point.
       *  \param dist distance between the view point (camera position) and the target point.
       *  \remarks The target point is used as the reference point for rotations of the camera.
       *  \sa getTargetDistance*/
      NVSG_API void setTargetDistance(float dist);

      /*! \brief Test on automatic eye distance calculation.   
       *  \return This function returns true when the distance is automatically calculated. 
       *  \remarks This function can be used to test if the automatic eye distance
       *  calculation for stereo is turned on. The automatic eye distance is calculated by  
       *  multiplying the adjustment factor by the focus distance of the camera.
       *  \sa setStereoAutomaticEyeDistanceAdjustment, getStereoAutomaticEyeDistanceFactor,
       *  setStereoAutomaticEyeDistanceFactor */
      NVSG_API bool isStereoAutomaticEyeDistanceAdjustment() const;

      /*! \brief Enable/Disable automatic eye distance adjustment.
       *  \param state Pass in true to enable automatic eye distance calculation. 
       *  \remarks The automatic eye distance is calculated by multiplying 
       *  the adjustment factor by the focus distance of the camera.
       *  \sa isStereoAutomaticEyeDistanceAdjustment, getStereoAutomaticEyeDistanceFactor,
       *  setStereoAutomaticEyeDistanceFactor */
      NVSG_API void setStereoAutomaticEyeDistanceAdjustment(bool state);

      /*! \brief Get the automatic eye distance adjustment factor.
       *  \return This function returns the eye distance adjustment factor. 
       *  \remarks The automatic eye distance is calculated by multiplying 
       *  the adjustment factor by the focus distance of the camera.\n 
       *  \par Example
       *  \sa setStereoAutomaticEyeDistanceAdjustment, getStereoAutomaticEyeDistanceFactor,
       *  setStereoAutomaticEyeDistanceFactor */
      NVSG_API float getStereoAutomaticEyeDistanceFactor() const;
      
      /*! \brief Set the automatic eye distance adjustment factor.
       *  \param factor Distance factor. 
       *  \remarks The automatic eye distance is calculated by multiplying 
       *  the adjustment factor by the focus distance of the camera.\n 
       *  The default value is 0.03 (3%). This value represents the following setup:\n
       *  A person with an eye distance of about six cm sitting in front of the monitor, 
       *  where the monitor is about one meter away. This setup leads to very natural stereo 
       *  impression.\n
       *  \sa isStereoAutomaticEyeDistanceAdjustment, getStereoAutomaticEyeDistanceFactor,
       *  setStereoAutomaticEyeDistanceFactor */
      NVSG_API void setStereoAutomaticEyeDistanceFactor(float factor);

      /*! \brief Get the eye distance for stereo rendering. 
       *  \return This function returns the eye distance. If the camera of this ViewState is not 
       *  valid, the behavior is undefined.
       *  \remarks The eye distance can be automatically calculated or manually set by the 
       *  application. Make sure that a valid camera is defined when asking for the eye distance 
       *  since the the automatic eye distance calculation is based on the focus distance of
       *  the camera of this ViewState.
       *  \sa setStereoEyeDistance, 
       *  isStereoAutomaticEyeDistanceAdjustment, isStereoAutomaticEyeDistanceAdjustment,
       *  getStereoAutomaticEyeDistanceFactor,  setStereoAutomaticEyeDistanceFactor */
      NVSG_API float getStereoEyeDistance() const;

      /*! \brief Set the eye distance for stereo rendering. 
       *  \param distance Distance between the left and the right eye. 
       *  \remarks This function manually sets the eye distance for stereo rendering.
       *  \sa setStereoEyeDistance, 
       *  isStereoAutomaticEyeDistanceAdjustment, isStereoAutomaticEyeDistanceAdjustment,
       *  getStereoAutomaticEyeDistanceFactor,  setStereoAutomaticEyeDistanceFactor */
      NVSG_API void setStereoEyeDistance(float distance);

      /*! \brief Function to reverse the left and the right eye for stereo rendering.
       *  \param state \c true puts the image for the left eye onto the right eye framebuffer and vice versa. 
       *  The default state is \c false.
       *  \sa isStereoReversedEyes */
      NVSG_API void setStereoReversedEyes( bool state );

      /*! \brief Test on reversed eyes in stereo rendering. 
       *  \return This function returns true when the left and the right eye are reversed. 
       *  \remarks If the eyes are reversed you will see the image for the left eye on the 
       *  right eye and vice versa.
       *  \sa setStereoReversedEyes */
      NVSG_API bool isStereoReversedEyes() const;
      
      /*! \brief Set the LOD range scale factor.
       *  \param factor The factor to scale the LOD scale ranges. The default value is 1.f, 
       *  so the unscaled ranges are used.
       *  \remarks This function sets a scaling factor for LOD ranges of the LOD nodes in the tree.\n
       *  The scale factor can be used to globally scale the ranges without changing the LOD node 
       *  ranges directly. This can be used, for example, for scenes that were initially created for viewing 
       *  on small monitors. You can use this scaling to fine-tune these scenes for large projection
       *  walls by scaling the LOD levels to switch later to a lower resolution representation. 
       *  \sa LOD::getLODToUse, getLODRangeScale */
      NVSG_API void setLODRangeScale(float factor);

      //! Get the LOD range scale factor
      /** Default value is 1.0, so the unscaled ranges are used. */
      /*! \brief Get the LOD range scale factor.
       *  \return The LOD range scale factor for all the LOD nodes in the tree. 
       *  By default this factor is 1.f, so unscaled ranges of the LOD node will be used.
       *  \remarks The scale factor can be used to globally scale the ranges without changing the LOD node 
       *  ranges directly. This can be used, for example, for scenes that were initially created for viewing 
       *  on small monitors. You can use this scaling to fine-tune these scenes for large projection
       *  walls by scaling the LOD levels to switch later to a lower resolution representation. 
       *  \sa LOD::getLODToUse, setLODRangeScale */
      NVSG_API float getLODRangeScale() const;

      /*! \brief Returns the current MapList.
       *  \return The MapList of this ViewState. 
       *  \remarks This function retrieves the current MapList.\n
       *  A MapList contains two-dimensional maps that can be used to render, for example, overlays 
       *  for Menus or textual information on top of the scene. 
       *  \par Example 
       *  See the MapListViewer tutorial on how to use this functionality.
       *  \sa MapList, MapObject, setMapList */
      NVSG_API const MapListSharedPtr & getMapList() const;

      /*! \brief Sets the MapList.
       *  \remarks This function assigns a MapList.\n
       *  A MapList contains two-dimensional maps that can be used to render, for example, overlays
       *  for Menus or textual information on top of the scene. 
       *  \par Example 
       *  See the MapListViewer tutorial on how to use this functionality.
       *  \sa MapList, MapObject, getMapList */
      NVSG_API void setMapList( const MapListSharedPtr & ml );

      /*! \brief Set a TraversalMask to be used with this ViewState.
       *  \param mask The mask to be used.
       *  \remarks The traversal mask is used in conjuction with Traverser-derived and Renderer-derived objects to
       *  determine whether nodes in the scene (and therefore possibly the entire subgraph) are traversed, and/or rendered.  Traversers
       *  and renderers will use the traversal mask stored in the ViewState along with their TraversalMaskOverride's.  See
       *  Traverser::setTraversalMask or SceneRenderer::setTraversalMask for more information.
       *  \note The default traversal mask is ~0 so that all objects will be traversed/rendered.  Setting the traversal mask to 0 
       *  will cause no nodes to be traversed/rendered.
       *  \sa getTraversalMask, Object::setTraversalMask, Traverser::setTraversalMask, SceneRenderer::setTraversalMask */
      NVSG_API void setTraversalMask( unsigned int mask );

      /*! \brief Returns the current traversal mask
       *  \return The current traversal mask.
       *  \sa setTraversalMask */
      NVSG_API unsigned int getTraversalMask() const;

      /*! \brief Returns the current Scene of the ViewState
       *  \return The Scene of this ViewState.
      */
      NVSG_API const SceneSharedPtr & getScene() const;

      /*! \brief Set the Scene of the ViewState
       *  \param scene The new Scene
      */
      NVSG_API void setScene( const SceneSharedPtr & scene );

      /*! \brief Query if automatic clip plane adjustment is on.
       *  \return \c true, if automatic clip plane adjustment is on, otherwise \c false.
       *  \sa setAutoClipPlanes, getCamera */
      NVSG_API bool getAutoClipPlanes() const;

      /*! \brief Enable/Disable automatic clip plane adjustment.
       *  \param enable \c true enables automatic clip plane adjustment, \c false disables it.
       *  \remarks When automatic clip plane adjustment is \c true, on calling getCamera the clip planes of this
       *  ViewState's camera are adjusted to ensure the whole depth of the scene is visible. If the user has
       *  additional information about the scene that might help clipping, this should be turned off.
       *  The default value is \c true.
       *  \sa getAutoClipPlanes, getCamera */
      NVSG_API void setAutoClipPlanes( bool enable );

      /*! \brief Returns the actual incarnation count of this ViewState.
       *  \return The function returns the actual incarnation count of this ViewState.
       *
       *  The returned incarnation count usually is used to test it against a related
       *  incarnation count maintained by some client to detect if changes have been
       *  made to the ViewState. */
      NVSG_API const nvutil::Incarnation& getIncarnation() const;

    // reflected properties
    public:
      REFLECTION_INFO_API( NVSG_API, ViewState );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( AutoClipPlanes );
          NVSG_API DECLARE_STATIC_PROPERTY( TargetDistance );
          NVSG_API DECLARE_STATIC_PROPERTY( StereoAutomaticEyeDistanceFactor );
          NVSG_API DECLARE_STATIC_PROPERTY( StereoEyeDistance );
          NVSG_API DECLARE_STATIC_PROPERTY( LODRangeScale );
          NVSG_API DECLARE_STATIC_PROPERTY( TraversalMask );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      friend struct nvutil::Holder<ViewState>;

      /*! \brief Default-constructs a ViewState. 
       *  \remarks After instantiation, the ViewState has the following properties:
       *   - no animation running 
       *   - no camera defined
       *   - no stereo 
       *   - no LOD range scaling
       *   - no cull information */
      NVSG_API ViewState();

      /*! \brief Copy-constructs a ViewState from another ViewState.
       *  \param rhs Source ViewState. */
      NVSG_API ViewState( const ViewState& rhs );

      /*! \brief Destructs a ViewState. */
      NVSG_API virtual ~ViewState();

    private:
      bool                           m_autoClipPlanes; //!< automatically adjust the clip planes for the camera
      CameraSharedPtr                m_camera;         //!< this camera renders the tree
      int                            m_currentDelta;   //!< current animation delta
      unsigned int                   m_currentTick;    //!< current animation tick
      nvui::RendererOptionsSharedPtr m_rendererOptions; //!< RenderOptions object for SceneRenderers
      unsigned int                   m_traversalMask; //!< Current Traversal mask
      
      float                         m_targetDistance;
      bool                          m_stereoAutomaticEyeDistanceAdjustment;
      float                         m_stereoAutomaticEyeDistanceFactor;
      float                         m_stereoEyeDistance;
      bool                          m_reversedEyes;
      float                         m_scaleLODRange;
      // the animation state might be modified by concurrently executing threads, 
      // declare it as volatile for this reason
      volatile bool                 m_animating;

      MapListSharedPtr  m_mapList;

      nvui::SmartRenderTarget m_renderTarget; //!< RenderTarget to render on
      nvsg::SceneSharedPtr    m_scene;        //!< Scene to render

      nvutil::SWMRSync            m_lock;
      mutable nvutil::Incarnation m_incarnation;  // will be incremented on edits
      mutable nvutil::Incarnation m_acpCameraIncarnation;             // needed to check for changes in getCamera()
      mutable bool                m_acpCameraIncarnationValid;
      mutable nvutil::Incarnation m_acpRootBoundingVolumeIncarnation; // needed to check for changes in getCamera()
      mutable bool                m_acpRootBoundingVolumeIncarnationValid;

      nvutil::BackLinkObserverSharedPtr m_cameraObserver;
      nvutil::Subject::SmartPayload     m_payload;
  };

  
  inline bool ViewState::isAnimating() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_animating );
  }

  inline void ViewState::setAnimating( bool animating )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_animating != animating )
    {
      m_animating = animating;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
    }
  }

  inline unsigned int ViewState::getCurrentTick() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_currentTick );
  }

  inline int ViewState::getCurrentDelta() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_currentDelta );
  }

  inline float ViewState::getTargetDistance() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_targetDistance );
  }

  inline void ViewState::setTargetDistance( float dist )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( dist != m_targetDistance )
    {
      m_targetDistance = dist;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
      notify( PID_TargetDistance );
    }
  }

  inline bool ViewState::isStereoAutomaticEyeDistanceAdjustment() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_stereoAutomaticEyeDistanceAdjustment );
  }

  inline void ViewState::setStereoAutomaticEyeDistanceAdjustment( bool state )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_stereoAutomaticEyeDistanceAdjustment != state ) 
    {
      m_stereoAutomaticEyeDistanceAdjustment = state;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
    }
  }

  inline float ViewState::getStereoAutomaticEyeDistanceFactor() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_stereoAutomaticEyeDistanceFactor );
  }

  inline void ViewState::setStereoAutomaticEyeDistanceFactor( float factor )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( factor != m_stereoAutomaticEyeDistanceFactor )
    {
      m_stereoAutomaticEyeDistanceFactor = factor;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation; 
      notify( PID_StereoAutomaticEyeDistanceFactor );
    }
  }

  inline float ViewState::getStereoEyeDistance() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    if ( m_stereoAutomaticEyeDistanceAdjustment && m_camera )
    {
      CameraReadLock camera(m_camera);
      return(m_stereoAutomaticEyeDistanceFactor * camera->getFocusDistance());
    }
    else
    {
      return(m_stereoEyeDistance);
    }
  }

  inline void ViewState::setStereoEyeDistance( float distance )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( distance != m_stereoEyeDistance )
    {
      m_stereoEyeDistance = distance;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
      notify( PID_StereoEyeDistance );
    }
  }

  inline void ViewState::setStereoReversedEyes( bool state )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_reversedEyes != state )
    {
      m_reversedEyes = state;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
    }
  }

  inline bool ViewState::isStereoReversedEyes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_reversedEyes );
  }

  inline void ViewState::setLODRangeScale(float factor)
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( factor != m_scaleLODRange )
    {
      m_scaleLODRange = factor;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
      notify( PID_LODRangeScale );
    }
  }
  
  inline float ViewState::getLODRangeScale() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_scaleLODRange;
  }

  inline const MapListSharedPtr & ViewState::getMapList() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_mapList );
  }

  inline void ViewState::setMapList( const MapListSharedPtr & ml )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_mapList != ml )
    {
      m_mapList = ml;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
    }
  }

  inline unsigned int ViewState::getTraversalMask() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_traversalMask );
  }

  inline void ViewState::setTraversalMask( unsigned int mask )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_traversalMask != mask )
    {
      m_traversalMask = mask;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
      notify( PID_TraversalMask );
    }
  }

  inline bool ViewState::getAutoClipPlanes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_autoClipPlanes );
  }

  inline void ViewState::setAutoClipPlanes( bool enable )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( enable != m_autoClipPlanes )
    {
      m_autoClipPlanes = enable;
      nvutil::AutoLock lock(m_lock);    // lock mutable m_incarnation
      ++m_incarnation;
      notify( PID_AutoClipPlanes );
    }
  }

} // namespace

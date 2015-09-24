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
#include "nvsg/CoreTypes.h"
#include "nvsg/Group.h"
#include "nvsg/Node.h"
#include "nvsg/SceneAttribute.h"
#include "nvsg/TextureHost.h"
#include "nvmath/Vecnt.h"
#include "nvutil/Reflection.h"

namespace nvsg
{
  /*! \brief Container class to hold all scene related information.
   *  \par Namespace: nvsg
   *  \remarks The Scene represents the loaded/created scene. It contains the ambient and back
   *  color, predefined cameras (views), predefined camera animations, and the tree of objects
   *  (the scene graph).
   *  \note The Scene holds a number of predefined cameras, but the ViewState holds the active
   *  camera.
   *  \sa Camera, SceneLoader, SceneSaver, ViewState */
  class Scene : public Object
  {
    public:
      /*! \brief The container type of the cameras */
      typedef std::vector<CameraSharedPtr>                                            CameraContainer;

      /*! \brief The iterator over the CameraContainer */
      typedef nvutil::SceniXIterator<Scene,CameraContainer::iterator>                 CameraIterator;

      /*! \brief The const iterator over the CameraContainer */
      typedef nvutil::SceniXIterator<Scene,CameraContainer::const_iterator>           CameraConstIterator;

      /*! \brief The container type of the camera animations */
      typedef std::vector<TrafoAnimationSharedPtr>                                    CameraAnimationContainer;

      /*! \brief The iterator over the CameraAnimationContainer */
      typedef nvutil::SceniXIterator<Scene,CameraAnimationContainer::iterator>        CameraAnimationIterator;

      /*! \brief The const iterator over the CameraAnimationContainer */
      typedef nvutil::SceniXIterator<Scene,CameraAnimationContainer::const_iterator>  CameraAnimationConstIterator;

      /*! \brief The container type of the attributes */
      typedef std::vector<SceneAttributeSharedPtr>                                    AttributeContainer;

      /*! \brief The iterator over the AttributeContainer */
      typedef nvutil::SceniXIterator<Scene,AttributeContainer::iterator>              AttributeIterator;

      /*! \brief The const iterator over the AttributeContainer */
      typedef nvutil::SceniXIterator<Scene,AttributeContainer::const_iterator>        AttributeConstIterator;

    public:
      NVSG_API static SceneSharedPtr create();

    public:
      /*! \brief Get the global ambient color.
       *  \return A reference to the constant ambient color.
       *  \remarks This color represents light that is not from any particular source. The default
       *  is light grey (0.2,0.2,0.2).
       *  \sa getBackColor, setAmbientColor */
      NVSG_API const nvmath::Vec3f & getAmbientColor() const;

      /*! \brief Set the global ambient color.
       *  \param color A reference to the constant ambient color to set.
       *  \remarks This color represents light that is not from any particular source. The default
       *  is light grey (0.2,0.2,0.2).
       *  \sa getAmbientColor, setBackColor */
      NVSG_API void setAmbientColor( const nvmath::Vec3f & color );

      /*! \brief Get the frame number
       *  \return The current frame number.  
       *  \remarks The frame number is monotonically incremented by
       *  SceniX's AppTraverser.  This value is published to
       *  shaders in the NVSGScene.frameNumber counter.  NOTE that the value
       *  is a SIGNED int, because Cg does not currently support unsigned ints.
       *  While the maximum value able to be returned from this routine is
       *  0x7fffffff, code using the frame number should assume that it can
       *  change or wrap to zero at any time.
       *  \sa setFrameNumber */
      NVSG_API int getFrameNumber() const;

      /*! \brief Set the frame number.
       *  \param frameNumber The current frame count for this scene.
       *  \remarks The frame number is monotonically incremented by
       *  SceniX's AppTraverser.  This value can be set or reset if
       *  necessary.  NOTE the value is a SIGNED int because Cg does not support
       *  unsigned ints.  Therefore, if the argument is < 0, it is clamped to 0
       *  when stored.  Also note that Cg will store the value as a float
       *  rather than an INT, and therefore the range will be further
       *  compromised.  Code using this value should assume it can change or
       *  wrap at any time.
       *  \sa getFrameNumber */
      NVSG_API void setFrameNumber( int frameNumber );

      /*! \brief Get the background color.
       *  \return A reference to the constant background color.
       *  \remarks This color is used to clear the viewport. The default is medium grey
       *  (0.4,0.4,0.4,1.0).
       *  \sa getAmbientColor, setBackColor */
      NVSG_API const nvmath::Vec4f & getBackColor()  const;

      /*! \brief Set the background color.
       *  \param color A reference to the constant background color to set.
       *  \remarks This color is used to clear the viewport with. The default is medium grey
       *  (0.4,0.4,0.4,1.0).
       *  \sa getBackColor, setAmbientColor, */
      NVSG_API void setBackColor( const nvmath::Vec4f & color );

       /*! \brief return the handle to background texture image.
      *  \return A TextureHost 
      *  \sa setBackImage */
      NVSG_API const TextureHostSharedPtr & getBackImage() const;

      /*! brief Set the background texture image
       * \param image The TextureHost to be set as background.
       * \remarks The reference count for \a image is incremented. The texture is applied to the entire viewport. 
       * \sa getBackImage */
      NVSG_API void setBackImage( const TextureHostSharedPtr & image );


      /*! \brief Get the number of cameras in this Scene.
       *  \return The number of cameras in this Scene.
       *  \sa beginCameras, endCameras, addCamera, removeCamera, clearCameras, findCamera */
      NVSG_API unsigned int getNumberOfCameras() const;

      /*! \brief Get a const iterator to the first camera in this Scene.
       *  \return A const iterator to the first camera in this Scene.
       *  \sa getNumberOfCameras, endCameras, addCamera, removeCamera, clearCameras, findCamera */
      NVSG_API CameraConstIterator beginCameras() const;

      /*! \brief Get an iterator to the first camera in this Scene.
       *  \return An iterator to the first camera in this Scene.
       *  \sa getNumberOfCameras, endCameras, addCamera, removeCamera, clearCameras, findCamera */
      NVSG_API CameraIterator beginCameras();

      /*! \brief Get a const iterator that points just beyond the end of the camera in this Scene.
       *  \return A const iterator that points just beyond the end of the camera in this Scene.
       *  \sa getNumberOfCameras, beginCameras, addCamera, removeCamera, clearCameras, findCamera */
      NVSG_API CameraConstIterator endCameras() const;

      /*! \brief Get an iterator that points just beyond the end of the camera in this Scene.
       *  \return An iterator that points just beyond the end of the camera in this Scene.
       *  \sa getNumberOfCameras, beginCameras, addCamera, removeCamera, clearCameras, findCamera */
      NVSG_API CameraIterator endCameras();

      /*! \brief Adds a camera to this Scene.
       *  \param camera Specifies the camera to add
       *  \return An iterator that points to the position where \a camera was added.
       *  \sa getNumberOfCameras, beginCameras, endCameras, removeCamera, clearCameras, findCamera */
      NVSG_API CameraIterator addCamera( const CameraSharedPtr & camera );

      /*! \brief Remove a camera from this Scene.
       *  \param camera The camera to remove from this Scene.
       *  \return \c true, if the camera has been removed from this Scene, otherwise \c false.
       *  \sa getNumberOfCameras, beginCameras, endCameras, addCamera, clearCameras, findCamera */
      NVSG_API bool removeCamera( const CameraSharedPtr & camera );

      /*! \brief Remove a camera from this Scene.
       *  \param sci An iterator to the camera to remove from this Scene.
       *  \return An iterator pointing to the new location of the camera that followed the one removed by
       *  this function call, which is endCameras() if the operation removed the last camera.
       *  \sa getNumberOfCameras, beginCameras, endCameras, addCamera, clearCameras, findCamera */
      NVSG_API CameraIterator removeCamera( const CameraIterator & sci );

      /*! \brief Remove all cameras from this Scene.
       *  \sa getNumberOfCameras, beginCameras, endCameras, addCamera, removeCamera, findCamera */
      NVSG_API void clearCameras();

      /*  \brief Find a specified camera in this Scene.
       *  \param camera The camera to find.
       *  \return A const iterator to the found camera in this Scene.
       *  \sa getNumberOfCameras, beginCameras, endCameras, addCamera, removeCamera, clearCameras */
      NVSG_API CameraConstIterator findCamera( const CameraSharedPtr & camera ) const;

      /*  \brief Find a specified camera in this Scene.
       *  \param camera The camera to find.
       *  \return An iterator to the found camera in this Scene.
       *  \sa getNumberOfCameras, beginCameras, endCameras, addCamera, removeCamera, clearCameras */
      NVSG_API CameraIterator findCamera( const CameraSharedPtr & camera );


      /*! \brief Get the number of camera animations in this Scene.
       *  \return The number of camera animations in this Scene.
       *  \sa beginCameraAnimations, endCameraAnimations, addCameraAnimation, removeCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API unsigned int getNumberOfCameraAnimations() const;

      /*! \brief Get a const iterator to the first camera in this Scene.
       *  \return A const iterator to the first camera in this Scene.
       *  \sa getNumberOfCameraAnimations, endCameraAnimations, addCameraAnimation, removeCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API CameraAnimationConstIterator beginCameraAnimations() const;

      /*! \brief Get an iterator to the first camera in this Scene.
       *  \return An iterator to the first camera in this Scene.
       *  \sa getNumberOfCameraAnimations, endCameraAnimations, addCameraAnimation, removeCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API CameraAnimationIterator beginCameraAnimations();

      /*! \brief Get a const iterator that points just beyond the end of the camera animation in this Scene.
       *  \return A const iterator that points just beyond the end of the camera animation in this Scene.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, addCameraAnimation, removeCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API CameraAnimationConstIterator endCameraAnimations() const;

      /*! \brief Get an iterator that points just beyond the end of the camera animation in this Scene.
       *  \return An iterator that points just beyond the end of the camera anitmation in this Scene.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, addCameraAnimation, removeCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API CameraAnimationIterator endCameraAnimations();

      /*! \brief Adds a camera animation to this Scene.
       *  \param animation Specifies the camera animation to add
       *  \return An iterator that points to the position where \a camera animation was added.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, endCameraAnimations, removeCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API CameraAnimationIterator addCameraAnimation( const TrafoAnimationSharedPtr & animation );

      /*! \brief Remove a camera animation from this Scene.
       *  \param animation The camera animation to remove from this Scene.
       *  \return \c true, if the camera animation has been removed from this Scene, otherwise \c false.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, endCameraAnimations, addCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API bool removeCameraAnimation( const TrafoAnimationSharedPtr & animation );

      /*! \brief Remove a camera animation from this Scene.
       *  \param scai An iterator to the camera animation to remove from this Scene.
       *  \return An iterator pointing to the new location of the camera animation that followed the one removed by
       *  this function call, which is endCameraAnimations() if the operation removed the last camera animation.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, endCameraAnimations, addCameraAnimation, clearCameraAnimations, findCameraAnimation */
      NVSG_API CameraAnimationIterator removeCameraAnimation( const CameraAnimationIterator & scai );

      /*! \brief Remove all camera animations from this Scene.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, endCameraAnimations, addCameraAnimation, removeCameraAnimation, findCameraAnimation */
      NVSG_API void clearCameraAnimations();

      /*  \brief Find a specified camera animation in this Scene.
       *  \param animation The camera animation to find.
       *  \return A const iterator to the found camera animation in this Scene.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, endCameraAnimations, addCameraAnimation, removeCameraAnimation, clearCameraAnimations */
      NVSG_API CameraAnimationConstIterator findCameraAnimation( const TrafoAnimationSharedPtr & animation ) const;

      /*  \brief Find a specified camera animation in this Scene.
       *  \param animation The camera animation to find.
       *  \return An iterator to the found camera animation in this Scene.
       *  \sa getNumberOfCameraAnimations, beginCameraAnimations, endCameraAnimations, addCameraAnimation, removeCameraAnimation, clearCameraAnimations */
      NVSG_API CameraAnimationIterator findCameraAnimation( const TrafoAnimationSharedPtr & animation );


      /*! \brief Get the number of attributes in this Scene.
       *  \return The number of attributes in this Scene.
       *  \sa beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API unsigned int getNumberOfAttributes() const;

      /*! \brief Get a const iterator to the first attribute in this Scene.
       *  \return A const iterator to the first attribute in this Scene.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeConstIterator beginAttributes() const;

      /*! \brief Get an iterator to the first attribute in this Scene.
       *  \return An iterator to the first attribute in this Scene.
       *  \sa getNumberOfAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator beginAttributes();

      /*! \brief Get a const iterator that points just beyond the end of the attribute in this Scene.
       *  \return A const iterator that points just beyond the end of the attribute in this Scene.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeConstIterator endAttributes() const;

      /*! \brief Get an iterator that points just beyond the end of the attribute in this Scene.
       *  \return An iterator that points just beyond the end of the attribute in this Scene.
       *  \sa getNumberOfAttributes, beginAttributes, addAttribute, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator endAttributes();

      /*! \brief Adds an attribute to this Scene.
       *  \param attribute Specifies the attribute to add
       *  \return An iterator that points to the position where \a attribute was added.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, removeAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator addAttribute( const SceneAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from this Scene.
       *  \param attribute The attribute to remove from this Scene.
       *  \return \c true, if the attribute has been removed from this Scene, otherwise \c false.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API bool removeAttribute( const SceneAttributeSharedPtr & attribute );

      /*! \brief Remove an attribute from this Scene.
       *  \param sai An iterator to the attribute to remove from this Scene.
       *  \return An iterator pointing to the new location of the attribute that followed the one removed by
       *  this function call, which is endAttributes() if the operation removed the last attribute.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, clearAttributes, findAttribute */
      NVSG_API AttributeIterator removeAttribute( const AttributeIterator & sai );

      /*! \brief Remove all attributes from this Scene.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, findAttribute */
      NVSG_API void clearAttributes();

      /*  \brief Find a specified attribute in this Scene.
       *  \param attribute The attribute to find.
       *  \return A const iterator to the found attribute in this Scene.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeConstIterator findAttribute( const SceneAttributeSharedPtr & attribute ) const;

      /*  \brief Find a specified attribute in this Scene.
       *  \param attribute The attribute to find.
       *  \return An iterator to the found attribute in this Scene.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeIterator findAttribute( const SceneAttributeSharedPtr & attribute );

      /*  \brief Find the attribute in this Scene specified by objectCode.
       *  \param objectCode The object code of the attribute to find.
       *  \return An iterator to the found attribute in this Scene.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeConstIterator findAttribute( unsigned int objectCode ) const;

      /*  \brief Find the attribute in this Scene specified by objectCode.
       *  \param objectCode The object code of the attribute to find.
       *  \return An iterator to the found attribute in this Scene.
       *  \sa getNumberOfAttributes, beginAttributes, endAttributes, addAttribute, removeAttribute, clearAttributes */
      NVSG_API AttributeIterator findAttribute( unsigned int objectCode );

      /*! \brief Returns the root Node of the Scene.
       *  \return The root Node of the Scene.
       *  \sa setRootNode */
      NVSG_API const NodeSharedPtr & getRootNode() const;

      /*! \brief Sets the root Node of the Scene.
       *  \param root The Node to set as the root node.
       *  \remarks The reference count of \a root is incremented, and the reference count of any
       *  previous root node is decremented.
       *  \sa getRootNode */
      NVSG_API void setRootNode( const NodeSharedPtr & root );

      /*! \brief Ask if the Scene contains any shader.
       *  \return \c true, if the Scene contains a CgFx or an RTFx attribute, otherwise \c false.
       *  \remarks Because of the special display handling of shaders, it might be of interest if
       *  there's a shader attribute in the Scene at all.
       *  \sa containsLight, containsTransparentGeometry */
      NVSG_API bool containsShader() const;

      /*! \brief Ask if this Scene contains any LightSource.
       *  \return \c true, if the Scene contains a LightSource, otherwise \c false.
       *  \sa containsCgFx, containsTransparentGeometry */
      NVSG_API bool containsLight() const;

      /*! \brief Ask if this Scene contains any transparent StateAttribute.
       *  \return \c true, if the Scene contains a transparent StateAttribute, otherwise \c false.
       *  \sa containsCgFx, containsLight */
      NVSG_API bool containsTransparency() const;

      /*! \brief Interface to determine if an Object contains an animation.
       *  \return \c true if there is a camera animation or the root node of the tree contains
       *  an Animation, otherwise c\ false. */
      NVSG_API bool containsAnimation() const;

      NVSG_API const nvutil::Incarnation& getIncarnation() const;

      using Object::getBoundingBox;
      using Object::getBoundingSphere;

#if !defined(NDEBUG)
      /*! \brief Ask if the specified node is part of this scene.
       *  \param node The node to check for being part of this scene.
       *  \return \c true, if \c node is part of this scene, otherwise \c false. */
      NVSG_API bool containsNode( const NodeSharedPtr & node ) const;
#endif

    protected:
      friend struct nvutil::Holder<Scene>;
	  friend class nvutil::Handle<Scene>;
      /*! \brief Default-constructs a Scene.
       *  \remarks The Scene initially has an ambient color of light grey (0.2, 0.2, 0.2), and a
       *  background color of medium grey (0.4, 0.4, 0.4). By default there are no cameras, 
       *  no camera animations, and no tree object attached to the Scene. */
      NVSG_API Scene();

      /*! \brief Destructs a Scene. */
      NVSG_API virtual ~Scene();

      NVSG_API void notifyChanged();

      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;
      NVSG_API virtual const nvmath::Box3f & getBoundingBox( bool recalculate ) const;
      NVSG_API virtual const nvmath::Sphere3f & getBoundingSphere( bool recalculate ) const;

    // reflected properties
    public:
      REFLECTION_INFO_API( NVSG_API, Scene );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( AmbientColor );
          NVSG_API DECLARE_STATIC_PROPERTY( FrameNumber );
          NVSG_API DECLARE_STATIC_PROPERTY( BackColor );
      END_DECLARE_STATIC_PROPERTIES

    private:
      mutable nvutil::Incarnation m_incarnation; // tracks changes
      mutable nvmath::Box3f       m_boundingBox;
      mutable nvmath::Sphere3f    m_boundingSphere;
      nvmath::Vec3f               m_ambientColor;
      nvmath::Vec4f               m_backColor;
      TextureHostSharedPtr        m_backImage;
      CameraContainer             m_cameras;
      CameraAnimationContainer    m_cameraAnimations;
      AttributeContainer          m_attributes;
      NodeSharedPtr               m_root;
      int                         m_frameNumber;
  };

  inline void Scene::setFrameNumber( int num )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    int newFrameNumber = (num<0) ? 0 : num;
    if ( newFrameNumber != m_frameNumber )
    {
      m_frameNumber = newFrameNumber;
      notify( PID_FrameNumber );
    }
  }

  inline int Scene::getFrameNumber() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_frameNumber;
  }

  inline unsigned int Scene::getNumberOfCameras() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_cameras.size()) );
  }

  inline Scene::CameraConstIterator Scene::beginCameras() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CameraConstIterator( m_cameras.begin() ) );
  }

  inline Scene::CameraIterator Scene::beginCameras()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( CameraIterator( m_cameras.begin() ) );
  }

  inline Scene::CameraConstIterator Scene::endCameras() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CameraConstIterator( m_cameras.end() ) );
  }

  inline Scene::CameraIterator Scene::endCameras()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( CameraIterator( m_cameras.end() ) );
  }

  inline Scene::CameraConstIterator Scene::findCamera( const CameraSharedPtr & camera ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( CameraConstIterator( find( m_cameras.begin(), m_cameras.end(), camera ) ) );
  }

  inline Scene::CameraIterator Scene::findCamera( const CameraSharedPtr & camera )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( CameraIterator( find( m_cameras.begin(), m_cameras.end(), camera ) ) );
  }

  inline void Scene::clearCameras()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    m_cameras.clear();
  }

  inline unsigned int Scene::getNumberOfCameraAnimations() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_cameraAnimations.size()) );
  }

  inline Scene::CameraAnimationConstIterator Scene::beginCameraAnimations() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CameraAnimationConstIterator( m_cameraAnimations.begin() ) );
  }

  inline Scene::CameraAnimationIterator Scene::beginCameraAnimations()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( CameraAnimationIterator( m_cameraAnimations.begin() ) );
  }

  inline Scene::CameraAnimationConstIterator Scene::endCameraAnimations() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( CameraAnimationConstIterator( m_cameraAnimations.end() ) );
  }

  inline Scene::CameraAnimationIterator Scene::endCameraAnimations()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( CameraAnimationIterator( m_cameraAnimations.end() ) );
  }

  inline Scene::CameraAnimationConstIterator Scene::findCameraAnimation( const TrafoAnimationSharedPtr & animation ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( CameraAnimationConstIterator( find( m_cameraAnimations.begin(), m_cameraAnimations.end(), animation ) ) );
  }

  inline Scene::CameraAnimationIterator Scene::findCameraAnimation( const TrafoAnimationSharedPtr & animation )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( CameraAnimationIterator( find( m_cameraAnimations.begin(), m_cameraAnimations.end(), animation ) ) );
  }

  inline void Scene::clearCameraAnimations()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    m_cameraAnimations.clear();
  }

  inline const NodeSharedPtr & Scene::getRootNode() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_root );
  }

  inline bool Scene::containsLight() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( isPtrTo<Group>(m_root)                     // Implicit check on root node
            ? GroupReadLock(nvutil::sharedPtr_cast<Group>(m_root))->containsLight()
            : false );
  }

  inline bool Scene::containsTransparency() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    if(m_root)                                              // First check if root node is valid before requesting read access!
    {
      NodeReadLock root(m_root);
      return( root->containsTransparency() );
    }
    return false;
  }

  inline bool Scene::containsAnimation() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    if(m_root)                                            // First check if root node is valid before requesting read access!
    {
      return( !m_cameraAnimations.empty() || NodeReadLock(m_root)->containsAnimation() );
    }
    return false;
  }

  inline const nvmath::Vec3f& Scene::getAmbientColor() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_ambientColor );
  }

  inline void Scene::setAmbientColor( const nvmath::Vec3f &color )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( color != m_ambientColor )
    {
      m_ambientColor = color;
      notify( PID_AmbientColor );
    }
  }

  inline const nvmath::Vec4f& Scene::getBackColor() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_backColor );
  }

  inline void Scene::setBackColor( const nvmath::Vec4f &color )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( color != m_backColor )
    {
      m_backColor = color;
      notify( PID_BackColor );
    }
  }

  inline const nvsg::TextureHostSharedPtr & Scene::getBackImage() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return(m_backImage);
  }

  inline unsigned int Scene::getNumberOfAttributes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return checked_cast<unsigned int>(m_attributes.size());
  }

  inline Scene::AttributeConstIterator Scene::beginAttributes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( AttributeConstIterator( m_attributes.begin() ) );
  }

  inline Scene::AttributeIterator Scene::beginAttributes()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( AttributeIterator( m_attributes.begin() ) );
  }

  inline Scene::AttributeConstIterator Scene::endAttributes() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( AttributeConstIterator( m_attributes.end() ) );
  }

  inline Scene::AttributeIterator Scene::endAttributes()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( AttributeIterator( m_attributes.end() ) );
  }

  inline Scene::AttributeConstIterator Scene::findAttribute( const SceneAttributeSharedPtr & attribute ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( AttributeConstIterator( find( m_attributes.begin(), m_attributes.end(), attribute ) ) );
  }

  inline Scene::AttributeIterator Scene::findAttribute( const SceneAttributeSharedPtr & attribute )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( AttributeIterator( find( m_attributes.begin(), m_attributes.end(), attribute ) ) );
  }

  inline nvmath::Box3f Scene::calculateBoundingBox() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_root ? NodeReadLock( m_root )->getBoundingBox() : nvmath::Box3f() );
  }

  inline nvmath::Sphere3f Scene::calculateBoundingSphere() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_root ? NodeReadLock( m_root )->getBoundingSphere() : nvmath::Sphere3f() );
  }

} //  namspace nvsg

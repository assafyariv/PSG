// Copyright NVIDIA Corporation 2002-2007
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
/** @file */

#include "nvmath/Quatt.h"
#include "nvsg/Object.h" // base class definition
#include "nvutil/HashGenerator.h"
#include "nvutil/SceniXIterator.h"

namespace nvsg
{
  /*! \brief Type to describe the cull code of an object.
   *  \par Namespace: nvsg
   *  \remarks With a CullCode, an object can be marked to be completely out (CC_OUT), completely
   *  in (CC_IN) or partially in and out (CC_PART) with respect to a view volume. */
  typedef enum
  {
    CC_OUT,   //!< an object is completely outside
    CC_IN,    //!< an object is completely inside
    CC_PART   //!< an object is partially in and partially out
  } CullCode;

  /*! \brief Abstract base class for parallel and perspective cameras.
   *  \par Namespace: nvsg
   *  \remarks Besides defining the interface for a camera, a Camera provides some basic
   *  functionalities needed or used in all Camera classes.\n
   *  A Camera has a focus, a near distance, and a far distance. These define planes that are orthogonal to the
   *  view direction. The near and far distance bind the volume
   *  seen by this Camera. The focus distance is somewhere in between the near and far distance. At
   *  that distance the window size is used, defining the sides of the viewing volume. For stereo
   *  vision, the focus distance determines the plane in focus.\n
   *  A Camera holds zero or more LightSource objects as head lights that are positioned relative
   *  to the Camera position. These LightSource objects are not part of a Scene, but part
   *  of the Camera.\n
   *  For deriving your own Camera, you have to overload the member functions clone(),
   *  getProjection(), and getInverseProjection(). You can also overload setFocusDistance().
   *  \sa Object, ParallelCamera, PerspectiveCamera */
  class Camera : public Object
  {
    public:
      /*! \brief The container type of the Camera's head lights */
      typedef std::vector<LightSourceSharedPtr>                                 HeadLightContainer;

      /*! \brief The iterator over the HeadLightContainer */
      typedef nvutil::SceniXIterator<Camera,HeadLightContainer::iterator>       HeadLightIterator;

      /*! \brief The const iterator over the HeadLightContainer */
      typedef nvutil::SceniXIterator<Camera,HeadLightContainer::const_iterator> HeadLightConstIterator;

    public:
      /*! \brief Destructs a Camera. */
      NVSG_API virtual ~Camera( void );

      /*! \brief Get the position of the Camera.
       *  \return A reference to the constant position of the Camera
       *  \remarks The position of the Camera is given in world coordinates. The default position
       *  of a Camera is at (0.0,0.0,1.0).
       *  \sa getDirection, getOrientation, getUpVector, setPosition */
      NVSG_API const nvmath::Vec3f & getPosition( void ) const;

      /*! \brief Set the position of the Camera.
       *  \param pos A reference to the constant position to set
       *  \remarks The position of the Camera is given in world coordinates. The default position
       *  of a Camera is at (0.0,0.0,1.0). Setting the position increases the Incarnation of the
       *  Camera.
       *  \sa getPosition, setDirection, setOrientation, setUpVector */
      NVSG_API void setPosition( const nvmath::Vec3f & pos );

      /*! \brief Get the orientation of the Camera.
       *  \return A reference to the constant orientation of the Camera
       *  \remarks The orientation quaternion describes the rotation of the Camera from its
       *  standard orientation. The standard orientation is the negative z-axis (0.0,0.0,-1.0) as
       *  the view direction and the y-axis (0.0,1.0,0.0) as the up vector.
       *  \sa getDirection, getUpVector, setOrientation */
      NVSG_API const nvmath::Quatf & getOrientation( void ) const;

      /*! \brief Set the orientation of the Camera by a Quaternion.
       *  \param quat A reference to the constant orientation of the Camera.
       *  \remarks The orientation of the Camera is the rotation from the default to the desired
       *  view direction and up vector. The default view direction is the negative z-axis
       *  (0.0,0.0,-1.0), and the default up vector is the y-axis (0.0,1.0,0.0). Setting the
       *  orientation is a simple way to set the view direction and the up vector in a consistent
       *  way. Setting the orientation increases the Incarnation.
       *  \sa getOrientation, setDirection, setUpVector */
      NVSG_API void setOrientation( const nvmath::Quatf & quat );

      /*! \brief Set the orientation of the Camera by a view direction and an up vector.
       *  \param dir  A reference to the constant view direction to set
       *  \param up   A reference to the constant up vector to set.
       *  \remarks Sets the orientation of the Camera by simultaneously setting the view direction
       *  and the up vector. The direction and the up vector have to be normalized and orthogonal
       *  to each other. Setting the orientation increases the Incarnation.
       *  \sa getOrientation, setDirection, setUpVector */
      NVSG_API void setOrientation( const nvmath::Vec3f & dir
                                  , const nvmath::Vec3f & up );

      /*! \brief Get the view direction.
       *  \return The view direction of the Camera
       *  \remarks The view direction is the vector in world space that defines the direction to which the
       *  Camera looks. The default view direction is the negative z-axis (0.0,0.0,-1.0).
       *  \sa getOrientation, getPosition, getUpVector, setDirection */
      NVSG_API nvmath::Vec3f getDirection( void ) const;

      /*! \brief Set the viewing direction.
       *  \param dir A reference to the constant view direction to set
       *  \remarks The view direction is the vector in world space that defines the direction to which the
       *  Camera looks. The default view direction is the negative z-axis (0.0,0.0,-1.0). Setting
       *  the view direction changes the orientation of the Camera and might also change the up
       *  vector. Setting the view direction also increases the Incarnation of the Camera.
       *  \sa getDirection, setOrientation, setPosition, setUpVector */
      NVSG_API void setDirection( const nvmath::Vec3f & dir );

      /*! \brief Get the up vector of the Camera.
       *  \return The up vector of the Camera
       *  \remarks The up vector is the vector in world space that defines the up direction of the
       *  Camera. The default up vector is the y-axis (0.0,1.0,0.0).
       *  \sa getDirection, getOrientation, getPosition, setUpVector */
      NVSG_API nvmath::Vec3f getUpVector( void ) const;

      /*! \brief Set the up vector of the Camera.
       *  \param up A reference to the constant up vector to set
       *  \remarks The up vector is the vector in world space that defines the up direction of the
       *  Camera. The default up vector is the y-axis (0.0,1.0,0.0). Setting the up vector changes
       *  the orientation of the Camera and might also change the view direction. Setting the up vector 
       *  also increases the Incarnation of the Camera.
       *  \sa getUpVector, setDirection, setOrientation, setPosition */
      NVSG_API void setUpVector( const nvmath::Vec3f & up );

      /*! \brief Set the world to view matrix of the Camera.
       *  \param m A reference to the constant matrix to set as the world to view transfomation.
       *  \remarks As SceniX holds the world to view transformation of a camera as a translation and
       *  a rotation, this convenience function determines those two elements out of the matrix \a m.
       *  As that calculation might not be exact, and takes some time to do, directly using setPosition
       *  and setOrientation should be preferred, if that data is available.
       *  getWorldToViewMatrix, setViewToWorldMatrix, setPosition, setOrientation */
      NVSG_API void setWorldToViewMatrix( const nvmath::Mat44f & m );

      /*! \brief Get the world-to-view transformation.
       *  \return The world-to-view transformation
       *  \remarks Based on the position and orientation of the Camera, the transformation as a
       *  nvmath::Mat44f is calculated.
       *  \sa getOrientation, getPosition, getViewToWorldMatrix */
      NVSG_API const nvmath::Mat44f& getWorldToViewMatrix( void ) const;

      /*! \brief Set the view to world matrix of the Camera.
       *  \param m A reference to the constant matrix to set as the view to world transfomation.
       *  \remarks As SceniX holds the world to view transformation of a camera as a translation and
       *  a rotation, this convenience function determines those two elements out of the matrix \a m.
       *  As that calculation might not be exact, and takes some time to do, directly using setPosition
       *  and setOrientation should be preferred, if that data is available.
       *  getWorldToViewMatrix, setViewToWorldMatrix, setPosition, setOrientation */
      NVSG_API void setViewToWorldMatrix( const nvmath::Mat44f & m );

      /*! \brief Get the view-to-world transformation.
       *  \return The view-to-world transformation.
       *  \remarks Based on the position and orientation of the Camera, the transformation as a
       *  nvmath::Mat44f is calculated.
       *  \sa getOrientation, getPosition, getWorldToViewMatrix */
      NVSG_API const nvmath::Mat44f& getViewToWorldMatrix( void ) const;

      /*! \brief Get the distance to the projection plane.
       *  \return The distance from the Camera position to the projection plane.
       *  \remarks The focus distance is the orthogonal distance between the Camera and the
       *  projection plane. The focus distance should be greater-than or equal-to the near
       *  distance and less-than or equal-to the far distance. The default focus distance is 1.0.
       *  \sa getFarDistance, getNearDistance, setFocusDistance*/
      NVSG_API float getFocusDistance( void ) const;

      /*! \brief Set the distance to the projection plane.
       *  \param fd Distance to the projection plane to set
       *  \remarks The focus distance is the orthogonal distance between the Camera and the
       *  projection plane. The focus distance should be greater-than or equal-to the near
       *  distance and less-than or equal-to the far distance. The default focus distance is 1.0.
       *  Setting the focus distance increases the Incarnation of the Camera.
       *  \sa getFocusDistance, setNearDistance, setFarDistance */
      NVSG_API virtual void setFocusDistance( float fd );

      /*! \brief Interface for getting the projection matrix of this Camera.
       *  \return The projection transformation
       *  \sa getInverseProjection, getWorldToViewMatrix, getViewToWorldMatrix */
      NVSG_API virtual nvmath::Mat44f getProjection( void ) const = 0;

      /*! \brief Interface for getting the inverse projection matrix of this Camera.
       *  \return The inverse projection transformation
       *  \sa getProjection,  getWorldToViewMatrix, getViewToWorldMatrix */
      NVSG_API virtual nvmath::Mat44f getInverseProjection( void ) const = 0;

      /*! \brief Get the number of headlights attached to this Camera.
       *  \return The number of headlights attached to this Camera
       *  \remarks A Camera can have zero or more headlights that are LightSource objects
       *  positioned relative to the Camera position.
       *  \sa beginHeadLights, endHeadLights, addHeadLight, replaceHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API unsigned int getNumberOfHeadLights( void ) const;

      /*! \brief Returns a const iterator to the camera’s first headlight.
       *  \return A const iterator to the camera’s first headlight.
       *  \sa getNumberOfHeadLights, addHeadLight, endHeadLights, replaceHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API HeadLightConstIterator beginHeadLights() const;

      /*! \brief Returns an iterator to the camera’s first headlight.
       *  \return An iterator to the camera’s first headlight.
       *  \sa getNumberOfHeadLights, addHeadLight, endHeadLights, replaceHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API HeadLightIterator beginHeadLights();

      /*! \brief Returns a const iterator that points past the end of the camera’s headlights.
       *  \return A const iterator that points past the end of the camera’s headlights. If
       *  Camera::beginHeadlights() == Camera::endHeadlights(), the camera has not been assigned any headlights.
       *  \sa getNumberOfHeadLights, beginHeadLights, addHeadLight, replaceHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API HeadLightConstIterator endHeadLights() const;

      /*! \brief Returns an iterator that points past the end of the camera’s headlights.
       *  \return An iterator that points past the end of the camera’s headlights. If
       *  Camera::beginHeadLights() == Camera::endHeadLights(), the camera has not been assigned any headlights.
       *  \sa getNumberOfHeadLights, beginHeadLights, addHeadLight, replaceHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API HeadLightIterator endHeadLights();

      /*! \brief Adds a LightSource as headlight to this Camera.
       *  \param light The LightSource to add as headlight.
       *  \returns An iterator to the newly added head light.
       *  \remarks A Camera can have zero or more headlights that are LightSource objects
       *  positioned relative to the Camera position. The default position and orientation of a
       *  headlight is the Camera position and orientation. Adding a headlight increases the
       *  Incarnation of the Camera.
       *  \sa getNumberOfHeadLights, beginHeadLights, endHeadLights, replaceHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API HeadLightIterator addHeadLight( const LightSourceSharedPtr & light );

      /*! \brief Replace a head light in this Camera by an other.
       *  \param newLight The light to replace.
       *  \param oldLight The head light to be replaced.
       *  \return \c true, if \a oldLight is different from \a newLight and is a head light of this camera,
       *  otherwise \c false.
       *  \sa getNumberOfHeadLights, beginHeadLights, endHeadLights, addHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API bool replaceHeadLight( const LightSourceSharedPtr & newLight, const LightSourceSharedPtr & oldLight );

      /*! \brief Replace a head light in this Camera by an other.
       *  \param newLight The light to replace.
       *  \param oldLight The iterator to the head light to be replaced.
       *  \return \c true, if \a oldLight is different from \a newLight and is a head light of this camera,
       *  otherwise \c false.
       *  \sa getNumberOfHeadLights, beginHeadLights, endHeadLights, addHeadLight, removeHeadLight, clearHeadLights */
      NVSG_API bool replaceHeadLight( const LightSourceSharedPtr & newLight, const HeadLightIterator & oldLight );

      /*! \brief Remove a headlight from this Camera.
       *  \param light The head light to be removed.
       *  \return \c true, if \a light has been removed as a head light of this Camera, otherwise \c false.
       *  \sa getNumberOfHeadLights, beginHeadLights, endHeadLights, addHeadLight, replaceHeadLight, clearHeadLights */
      NVSG_API bool removeHeadLight( const LightSourceSharedPtr & light );

      /*! \brief Remove a headlight from this Camera.
       *  \param light The head light to be removed.
       *  \return \c true, if \a light has been removed as a head light of this Camera, otherwise \c false.
       *  \sa getNumberOfHeadLights, beginHeadLights, endHeadLights, addHeadLight, replaceHeadLight, clearHeadLights */
      NVSG_API bool removeHeadLight( const HeadLightIterator & light );

      /*! \brief Clear the list of headlights.
       *  \sa getNumberOfHeadLights, beginHeadLights, endHeadLights, addHeadLight, replaceHeadLight, removeHeadLight */
      NVSG_API void clearHeadLights( void );

      /*! \brief Move the Camera.
       *  \param delta          A reference to the constant delta to move
       *  \param cameraRelative Optional boolean controlling Camera relative or world relative
       *  movement. Default is Camera relative.
       *  \remarks Moves the Camera by \a delta. If \a cameraRelative is true (the default), the
       *  Camera is moved the corresponding amounts along the local axes. Otherwise \a delta is
       *  just added to the position of the Camera (being in world space).\n
       *  Moving the Camera increases the Incarnation.
       *  \sa moveX, moveY, moveZ, orbit, rotate, zoom, */
      NVSG_API void move( const nvmath::Vec3f & delta
                        , bool cameraRelative = true );

      /*! \brief Orbit the Camera.
       *  \param axis           A reference to the normalized axis to orbit around in Camera
       *  relative coordinates.
       *  \param targetDistance Distance to target point to orbit around.
       *  \param angle          Angle in radians to orbit.
       *  \param cameraRelative Optional boolean controlling Camera relative or world relative
       *  rotation. Default is Camera relative.
       *  \remarks Orbiting a Camera performs a rotation by \a angle around the point at
       *  targetDistance along the view direction, using the \a axis of rotation. If \a
       *  cameraRelative is \c true (the default), \a axis is a vector in Camera local coordinates.
       *  This way, the Camera is moved on a great circle around the target point, modifying the
       *  view direction to keep that target point. If otherwise \a cameraRelative is \c false \a
       *  axis is in world coordinates.\n
       *  Orbiting the Camera increases the Incarnation.
       *  \sa move, orbitX, orbitY, orbitZ, rotate, */
      NVSG_API void orbit( const nvmath::Vec3f & axis
                         , float targetDistance
                         , float angle
                         , bool  cameraRelative = true );

      /*! \brief Rotate the Camera.
       *  \param axis   A reference to the constant normalized axis to rotate around.
       *  \param angle  Angle in radians to rotate
       *  \param cameraRelative Optional boolean controlling Camera relative or world relative
       *  rotation. Default is Camera relative.
       *  \remarks Rotates the Camera around its position using \a axis of rotation by \a angle
       *  radians. If \a cameraRelative is true (the default), \a axis is a vector in Camera local
       *  coordinates. Otherwise \a axis is in world coordinates.\n
       *  Rotating the Camera increases the Incarnation.
       *  \sa move, orbit, rotateX, rotateY, rotateZ */
      NVSG_API void rotate( const nvmath::Vec3f& axis
                          , float angle
                          , bool  cameraRelative = true );

      /*! \brief Zoom by a positive factor.
       *  \param factor   Factor to use in zooming
       *  \remarks Zooming by a factor changes the window size of the Camera. The window size is
       *  the size of the world that is viewed at the projection plane. This plane is at a target
       *  distance along the view direction and orthogonal to the view direction.\n
       *  When the factor is less than one, the window size is reduced, and thus the viewport shows
       *  less content in the same space. That is, it is zoomed in. When the factor is greater than
       *  one, it is zoomed out.\n
       *  Zooming the Camera increases the Incarnation.
       *  \sa move, orbit, rotate, zoom */
      NVSG_API void virtual zoom( float factor ) = 0;

      /*! \brief Convenience function to move to the right or left in Camera space.
       *  \param val  Amount to move to the right or left
       *  \remarks With positive \a val the Camera is moved to the right; with negative \a val it
       *  is moved to the left.
       *  \sa move */
      void moveX( float val );

      /*! \brief Convenience function to move up or down in Camera space.
       *  \param val  Amount to move up or down
       *  \remarks With positive \a val the Camera is moved up; with negative \a val it is moved
       *  down.
       *  \sa move */
      void moveY( float val );

      /*! \brief Convenience function to move backward or forward in Camera space.
       *  \param val  Amount to move backward or forward
       *  \remarks With positive \a val the Camera is moved backward; with negative \a val it is
       *  moved to the forward.
       *  \sa move */
      void moveZ( float val );

      /*! \brief Convenience function to orbit around the x-axis of the Camera.
       *  \param targetDistance Distance to target point to orbit around.
       *  \param angle          Angle in radians to orbit.
       *  \remarks With positive \a angle the Camera moves down while looking up; with negative
       *  \a angle it moves up while looking down.
       *  \sa orbit */
      void orbitX( float targetDistance
                 , float angle );

      /*! \brief Convenience function to orbit around the y-axis of the Camera.
       *  \param targetDistance Distance to target point to orbit around
       *  \param angle          Angle in radians to orbit
       *  \remarks With positive \a angle the Camera moves to the right while looking to the left;
       *  with negative \a angle the Camera moves to the left while looking to the right.
       *  \sa orbit */
      void orbitY( float targetDistance
                 , float angle );

      /*! \brief Convenience function to orbit around the z-axis of the Camera.
       *  \param targetDistance Distance to target point to orbit around
       *  \param angle          Angle in radians to orbit
       *  \remarks Orbiting around the z-axis of the Camera has the same effect as rotating around
       *  it (rolling). With positive \a angle it rolls to the left; with positive \a angle it
       *  rolls to the right.
       *  \sa orbit, rotateZ */
      void orbitZ( float targetDistance
                 , float angle );

      /*! \brief Convenience function to rotate around the x-axis of the Camera.
       *  \param angle  Angle in radians to rotate
       *  \remarks The Rotation around the x-axis of the Camera is also called Pitch. With positive
       *  \a angle the Camera looks up; with negative \a angle it looks down.
       *  \sa rotate */
      void rotateX( float angle );

      /*! \brief Convenience function to rotate around the y-axis of the Camera.
       *  \param angle  Angle in radians to rotate
       *  \remarks The Rotation around the y-axis of the Camera is also called Yaw. With positive
       *  \a angle the Camera looks to the left; with negative \a angle it looks to the right.
       *  \sa rotate */
      void rotateY( float angle );

      /*! \brief Convenience function to rotate around the z-axis of the Camera.
       *  \param angle  Angle in radians to rotate
       *  \remarks The Rotation around the y-axis of the Camera is also called Roll. With positive
       *  \a angle the Camera rolls to the left; with negative \a angle it rolls to the right. 
       *  \sa orbitZ, rotate */
      void rotateZ( float angle );

      /*! \brief Interface for determining the CullCode of a Sphere3f relative to the view volume.
       *  \param sphere A reference to the constant Sphere3f to determine the CullCode for.
       *  \return CC_IN, if the Sphere3f \a sphere is completely inside the view volume; CC_OUT if
       *  it is completely out of the view volume; otherwise CC_PART.
       *  \remarks Behavior is undefined if sphere is invalid. This can be checked using !nvmath::isValid() */
      NVSG_API virtual CullCode determineCullCode( const nvmath::Sphere3f &sphere ) const = 0;

    // reflected properties
    public:
      REFLECTION_INFO_API( NVSG_API, Camera );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( Position );
          NVSG_API DECLARE_STATIC_PROPERTY( Orientation );
          NVSG_API DECLARE_STATIC_PROPERTY( Direction );
          NVSG_API DECLARE_STATIC_PROPERTY( UpVector );
          NVSG_API DECLARE_STATIC_PROPERTY( FocusDistance );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      /*! \brief Protected constructor to prevent explicit creation.
       *  \remarks Because Camera is an abstract class, its constructor is never used explicitly,
       *  but in the constructor of derived classes.
       *  \sa ParallelCamera, PerspectiveCamera */
      NVSG_API Camera( void );

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \param rhs       Reference to the constant Camera to copy from
       *  \remarks Because Camera is an abstract class, its constructor is never used explicitly,
       *  but in the constructor of derived classes.
       *  \sa ParallelCamera, PerspectiveCamera */
      NVSG_API Camera( const Camera& rhs );

      /*! \brief Protected assignment operator 
       *  \param rhs  Reference to the constant Camera to copy from
       *  \return A reference to the assigned Camera.
       *  \remarks Besides assigning the Camera as an Object, each head light is copied as well as
       *  all Camera defining parameters.
       *  \sa LightSource, Object */
      NVSG_API Camera & operator=( const Camera & rhs );

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      void copyHeadLights(const Camera& rhs);
      bool doRemoveHeadLight( const HeadLightContainer::iterator & hlci );
      void removeHeadLights();

    private:
      HeadLightContainer  m_headLights;       //!< vector of the headlights

      nvmath::Quatf m_orientation;      //!< orientation of the Camera
      nvmath::Vec3f m_position;         //!< position of Camera

      float         m_focusDist;        //!< distance from position to projection plane

      mutable nvmath::Mat44f  m_viewToWorld;      //!< cached view to world matrix
      mutable bool            m_viewToWorldValid; //!< view to world matrix cache is valid
      mutable nvmath::Mat44f  m_worldToView;      //!< cached world to view matrix
      mutable bool            m_worldToViewValid; //!< world to view matrix cache is valid
  };

  inline nvmath::Vec3f Camera::getDirection( void ) const 
  { 
    NVSG_TRACE();  
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    //  NOTE: normalizing shouldn't be necessary, but rounding errors do catch us!
    nvmath::Vec3f dir = nvmath::Vec3f( 0.0f, 0.0f, -1.0f ) * m_orientation;
    dir.normalize();
    return( dir );
  }

  inline float Camera::getFocusDistance( void ) const
  { 
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_focusDist ); 
  }

  inline unsigned int Camera::getNumberOfHeadLights( void ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_headLights.size()) );
  }

  inline const nvmath::Quatf & Camera::getOrientation( void ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_orientation );
  }

  inline const nvmath::Vec3f & Camera::getPosition( void ) const 
  { 
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_position ); 
  }

  inline nvmath::Vec3f Camera::getUpVector( void ) const 
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_PRIVATE_ASSERT( isNormalized( m_orientation ) );
    //  NOTE: normalizing shouldn't be necessary, but rounding errors do catch us!
    nvmath::Vec3f up = nvmath::Vec3f( 0.0f, 1.0f, 0.0f ) * m_orientation;
    up.normalize();
    return( up );
  }

  inline void Camera::moveX( float val )
  { 
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    move( nvmath::Vec3f( val, 0.f, 0.f ) );
  }

  inline void Camera::moveY( float val )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    move( nvmath::Vec3f( 0.f, val, 0.f ) );
  }

  inline void  Camera::moveZ( float val )
  { 
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    move( nvmath::Vec3f( 0.f, 0.f, val ) ); 
  }

  inline void  Camera::orbitX( float targetDistance, float rad )
  { 
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    orbit( nvmath::Vec3f( 1.f, 0.f, 0.f ), targetDistance, rad );
  }

  inline void  Camera::orbitY( float targetDistance, float rad )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    orbit( nvmath::Vec3f( 0.f, 1.f, 0.f ), targetDistance, rad );
  }

  inline void  Camera::orbitZ( float targetDistance, float rad )
  { 
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    orbit( nvmath::Vec3f( 0.f, 0.f, 1.f ), targetDistance, rad ); 
  }

  inline void  Camera::rotateX( float rad )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    rotate( nvmath::Vec3f(1.f, 0.f, 0.f), rad );  
  }

  inline void  Camera::rotateY( float rad )
  { 
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    rotate( nvmath::Vec3f(0.f, 1.f, 0.f), rad );  
  }

  inline void  Camera::rotateZ( float rad )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    rotate( nvmath::Vec3f(0.f, 0.f, 1.f), rad ); 
  }

  inline Camera::HeadLightConstIterator Camera::beginHeadLights() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( HeadLightConstIterator( m_headLights.begin() ) );
  }

  inline Camera::HeadLightIterator Camera::beginHeadLights()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( HeadLightIterator( m_headLights.begin() ) );
  }

  inline Camera::HeadLightConstIterator Camera::endHeadLights() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( HeadLightConstIterator( m_headLights.end() ) );
  }

  inline Camera::HeadLightIterator Camera::endHeadLights()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( HeadLightIterator( m_headLights.end() ) );
  }

} // namespace

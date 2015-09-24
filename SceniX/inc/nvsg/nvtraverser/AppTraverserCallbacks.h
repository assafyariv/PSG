// Copyright NVIDIA Corporation 2002-2005
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

namespace nvtraverser {
  // A semaphore used to link two callbacks together for setting camera attributes
  class CameraUpdateSemaphore : public nvsg::Object
  {
    public:
      NVSG_API static CameraUpdateSemaphoreSharedPtr create();

    public:
      void setCamera( const nvsg::CameraSharedPtr & cam );
      void setCameraPositionAndRoll( const nvmath::Vec3f & pos, nvmath::Quatf & ori );
      void setTargetPosition( const nvmath::Vec3f & pos );

    protected:
      friend struct nvutil::Holder<CameraUpdateSemaphore>;

      CameraUpdateSemaphore();

    private:
      enum
      {
        UPDATE_CAMERA = BIT(0), 
        UPDATE_TARGET = BIT(1),
        UPDATE_COMPLETE = (UPDATE_CAMERA | UPDATE_TARGET)
      };

      void update( unsigned int flag );

      nvmath::Vec3f         m_cameraPosition;
      nvmath::Vec3f         m_targetPosition;
      nvmath::Quatf         m_orientation;
      nvmath::Vec3f         m_defaultX, m_defaultY, m_defaultZ;
      unsigned int          m_semaphore;
      nvsg::CameraSharedPtr m_camera;
      bool                  m_hasBeenCalled;

  };

  inline CameraUpdateSemaphoreSharedPtr CameraUpdateSemaphore::create()
  {
    return( nvutil::ObjectTraits<CameraUpdateSemaphore>::Handle::create() );
  }

  inline CameraUpdateSemaphore::CameraUpdateSemaphore()
     : m_semaphore(0)
     , m_hasBeenCalled( false )      
  {
    // initialize the default axes for camera orientation calculation
    m_defaultX = nvmath::Vec3f(1,0,0);
    m_defaultY = nvmath::Vec3f(0,1,0);
    m_defaultZ = nvmath::Vec3f(0,0,1);
  }

  inline void CameraUpdateSemaphore::setCamera( const nvsg::CameraSharedPtr & cam )    
  {
    m_camera = cam;
  }

  inline void CameraUpdateSemaphore::setCameraPositionAndRoll( const nvmath::Vec3f & pos, nvmath::Quatf & ori )
  {
    m_cameraPosition = pos;
    m_orientation = ori;
    update( UPDATE_CAMERA );
  }

  inline void CameraUpdateSemaphore::setTargetPosition( const nvmath::Vec3f & pos )
  {
    m_targetPosition = pos;
    update( UPDATE_TARGET );
  }

  class SpotLightUpdateSemaphore : public nvsg::Object
  {
    public:
      NVSG_API static SpotLightUpdateSemaphoreSharedPtr create();

    public:
      void setSpotLight( const nvsg::SpotLightSharedPtr & spot );
      void setSpotLightPosition( const nvmath::Vec3f & pos );
      void setTargetPosition( const nvmath::Vec3f & pos );

    protected:
      friend struct nvutil::Holder<SpotLightUpdateSemaphore>;

      SpotLightUpdateSemaphore();

    private:
      enum
      {
        UPDATE_SPOTLIGHT = BIT(0), 
        UPDATE_TARGET = BIT(1),
        UPDATE_COMPLETE = (UPDATE_SPOTLIGHT | UPDATE_TARGET)
      };

      void update( unsigned int flag );

      nvmath::Vec3f             m_spotLightPosition;
      nvmath::Vec3f             m_targetPosition;
      unsigned int              m_semaphore;
      nvsg::SpotLightSharedPtr  m_spotLight;
  };

  inline SpotLightUpdateSemaphoreSharedPtr SpotLightUpdateSemaphore::create()
  {
    return( nvutil::ObjectTraits<SpotLightUpdateSemaphore>::Handle::create() );
  }

  inline SpotLightUpdateSemaphore::SpotLightUpdateSemaphore( )
     : m_semaphore(0)
  {
  }

  inline void SpotLightUpdateSemaphore::setSpotLight( const nvsg::SpotLightSharedPtr & spot )
  {
    m_spotLight = spot;
  }

  inline void SpotLightUpdateSemaphore::setSpotLightPosition( const nvmath::Vec3f & pos )
  {
    m_spotLightPosition = pos;
    update( UPDATE_SPOTLIGHT );
  }

  inline void SpotLightUpdateSemaphore::setTargetPosition( const nvmath::Vec3f & pos )
  {
    m_targetPosition = pos;
    update( UPDATE_TARGET );
  } 

  /*! \brief Callback to link a Group to a Camera
   *  \remarks This kind of callback can be used to position and orient a camera based on a group's position and 
   *  orientation.  The AppTraverser will update the cameras position and orientation dynamically as the scene
   *  changes, possibly due to Transform or AnimatedTransform nodes.
   *  \sa Group, Callback, AppTraverser, LinkGroupToCameraAsTargetCallback */
  class LinkGroupToCameraCallback : public nvsg::Callback
  {
    public:
      /*! \brief Constructor of a LinkGroupToCameraCallback.
       *  \param camera The Camera to be used as the observer of this callback.
       *  \sa Camera, Group */
      NVSG_API explicit LinkGroupToCameraCallback( const nvsg::CameraSharedPtr & camera );

      /*! \brief The function operator() of this callback.
       *  \param subject A pointer to the read-locked object that is observed.
       *  \param data A pointer to constant callback specific data. This callback expects it to be a pointer to
       *  the AppTraverser using this callback.
       *  \remarks This function is called from AppTraverser::postTraverseObject for each object with app callbacks
       *  registered.
       *  \sa AppTraverser, Group, Camera */
      NVSG_API virtual void operator()( const nvsg::Object * subject, const void * data ) const;

      NVSG_API virtual unsigned int getClassID() const;
      NVSG_API virtual const char * getClassName() const;
  };

  inline LinkGroupToCameraCallback::LinkGroupToCameraCallback( const nvsg::CameraSharedPtr & camera )
    : Callback( camera )
  {}

  inline unsigned int LinkGroupToCameraCallback::getClassID() const
  {
    return *(unsigned int *)"GTCC"; // GroupToCameraCallback
  }

  inline const char * LinkGroupToCameraCallback::getClassName() const
  {
    return( "LinkGroupToCameraCallback" );
  }

  /*! \brief Callback to link a Group to a Camera as its target.
   *  \remarks This kind of callback can be used to observe a Group from a Camera. When used, for example, as an
   *  app callback, the camera's direction vector will be updated based on the current position of the Target.  The
   *  Camera is still responsible for setting an appropriate UP vector.  This link must be used in conjunction with a
   *  LinkTransformToCameraAsPositionAndUpVectorCallback.
   *  \sa Group, Callback, AppTraverser, LinkGroupToCameraCallback, LinkTransformToCameraAsPositionAndUpVectorCallback */
  class LinkGroupToCameraAsTargetCallback : public nvsg::Callback
  {
    public:
      /*! \brief Constructor of a LinkGroupToCameraCallback.
       *  \param camera The Camera to be used as the observer of this callback.
       *  \sa Camera, Group */
      NVSG_API explicit LinkGroupToCameraAsTargetCallback( const nvsg::CameraSharedPtr & camera );
      NVSG_API ~LinkGroupToCameraAsTargetCallback();

      /*! \brief The function operator() of this callback.
       *  \param subject A pointer to the read-locked object that is observed.
       *  \param data A pointer to constant callback specific data. This callback expects it to be a pointer to
       *  the AppTraverser using this callback.
       *  \remarks This function is called from AppTraverser::postTraverseObject for each object with app callbacks
       *  registered.
       *  \sa AppTraverser, Group, Camera */
      NVSG_API virtual void operator()( const nvsg::Object * subject, const void * data ) const;

      NVSG_API virtual void setSemaphore( const CameraUpdateSemaphoreSharedPtr & sem );

      NVSG_API virtual unsigned int getClassID() const;
      NVSG_API virtual const char * getClassName() const;

    private:
      // A pointer to this callback's cooresponding semaphore
      CameraUpdateSemaphoreSharedPtr m_sem;
  };

  inline LinkGroupToCameraAsTargetCallback::LinkGroupToCameraAsTargetCallback( const nvsg::CameraSharedPtr & camera )
    : Callback( camera )
  {}

  inline void LinkGroupToCameraAsTargetCallback::setSemaphore( const CameraUpdateSemaphoreSharedPtr & sem )
  {
    m_sem = sem;
  }

  inline unsigned int LinkGroupToCameraAsTargetCallback::getClassID() const
  {
    return *(unsigned int *)"GTCT"; // GroupToCameraTarget
  }

  inline const char * LinkGroupToCameraAsTargetCallback::getClassName() const
  {
    return( "LinkGroupToCameraAsTargetCallback" );
  }

  /*! \brief Callback to link a Transform to a Camera as its position and up vector.
   *  \remarks This kind of callback can be used to position and possibly orient a camera.  This callback will position
   *  the camera based on the current ModelView matrix, ignoring any rotations.  It will then decompose the orientation
   *  quaternion attached to the Transform, and extract the axis/angle from it.  It will assume that this is a "Roll" value
   *  around the (0,1,0) axis, but will apply the angle as the roll around the direction vector and will recompute the 
   *  direction and up vector.
   *  This callback must be used in conjunction with a LinkGroupToCameraAsTargetCallback to set the direction vector
   *  dynamically.
   *  \sa Group, Callback, AppTraverser, LinkGroupToCameraCallback */
  class LinkTransformToCameraAsPositionAndRollCallback : public nvsg::Callback
  {
    public:
      /*! \brief Constructor of a LinkGroupToCameraCallback.
       *  \param camera The Camera to be used as the observer of this callback.
       *  \sa Camera, Group */
      NVSG_API explicit LinkTransformToCameraAsPositionAndRollCallback( const nvsg::CameraSharedPtr & camera );

      NVSG_API ~LinkTransformToCameraAsPositionAndRollCallback();

      /*! \brief The function operator() of this callback.
       *  \param subject A pointer to the read-locked object that is observed.
       *  \param data A pointer to constant callback specific data. This callback expects it to be a pointer to
       *  the AppTraverser using this callback.
       *  \remarks This function is called from AppTraverser::postTraverseObject for each object with app callbacks
       *  registered.
       *  \sa AppTraverser, Group, Camera */
      NVSG_API virtual void operator()( const nvsg::Object * subject, const void * data ) const;

      NVSG_API virtual void setSemaphore( const CameraUpdateSemaphoreSharedPtr & sem );

      NVSG_API virtual unsigned int getClassID() const;
      NVSG_API virtual const char * getClassName() const;

    private:
      // A pointer to this callback's cooresponding semaphore
      CameraUpdateSemaphoreSharedPtr m_sem;
  };

  inline LinkTransformToCameraAsPositionAndRollCallback::LinkTransformToCameraAsPositionAndRollCallback( const nvsg::CameraSharedPtr & camera )
    : Callback( camera )
  {}

  inline void LinkTransformToCameraAsPositionAndRollCallback::setSemaphore( const CameraUpdateSemaphoreSharedPtr & sem )
  {
    m_sem = sem;
  }

  inline unsigned int LinkTransformToCameraAsPositionAndRollCallback::getClassID() const
  {
    return *(unsigned int *)"TTCS"; // Transform To Camera State
  }

  inline const char * LinkTransformToCameraAsPositionAndRollCallback::getClassName() const
  {
    return( "LinkTransformToCameraAsPositionAndRollCallback" );
  }

  /*! \brief Callback to link a Group to a LightSource
   *  \remarks This kind of callback can be used to observe a Group from a LightSource. When used, for example, as an
   *  app callback, the LightSource is notified whenever the group is traversed by the AppTraverser.
   *  \sa Group, Callback, AppTraverser */
  class LinkGroupToLightSourceCallback : public nvsg::Callback
  {
    public:
      /*! \brief Constructor of a LinkGroupToLightSourceCallback.
       *  \param camera The LightSource to be used as the observer of this callback.
       *  \sa LightSource, Group */
      NVSG_API explicit LinkGroupToLightSourceCallback( const nvsg::LightSourceSharedPtr & ls );

      /*! \brief The function operator() of this callback.
       *  \param subject A pointer to the read-locked object that is observed.
       *  \param data A pointer to constant callback specific data. This callback expects it to be a pointer to
       *  the AppTraverser using this callback.
       *  \remarks This function is called from AppTraverser::postTraverseObject for each object with app callbacks
       *  registered.
       *  \sa AppTraverser, Group, Camera */
      NVSG_API virtual void operator()( const nvsg::Object * subject, const void * data ) const;

      NVSG_API virtual unsigned int getClassID() const;
      NVSG_API virtual const char * getClassName() const;
  };

  inline LinkGroupToLightSourceCallback::LinkGroupToLightSourceCallback( const nvsg::LightSourceSharedPtr & ls )
    : Callback( ls )
  {}

  inline unsigned int LinkGroupToLightSourceCallback::getClassID() const
  {
    return *(unsigned int *)"GTLC"; // GroupToLightSourceCallback
  }

  inline const char * LinkGroupToLightSourceCallback::getClassName() const
  {
    return( "LinkGroupToLightSourceCallback" );
  }

  /*! \brief Callback to link a Group to a SpotLight as its target.
   *  \remarks This kind of callback can be used to direct the target position of a spotlight. When used, for example, as an
   *  app callback, the spotlight's direction vector will be updated based on the current position of the Target.  This link must be 
   *  used in conjunction with a LinkTransformToSpotLightAsPositionCallback.
   *  \sa Group, Callback, AppTraverser, LinkGroupToSpotLightCallback, LinkTransformToSpotLightAsPositionCallback */
  class LinkGroupToSpotLightAsTargetCallback : public nvsg::Callback
  {
    public:
      /*! \brief Constructor of a LinkGroupToCameraCallback.
       *  \param camera The SpotLight to be used as the observer of this callback.
       *  \sa Camera, Group */
      NVSG_API explicit LinkGroupToSpotLightAsTargetCallback( const nvsg::SpotLightSharedPtr & spot );

      NVSG_API ~LinkGroupToSpotLightAsTargetCallback();

      /*! \brief The function operator() of this callback.
       *  \param subject A pointer to the read-locked object that is observed.
       *  \param data A pointer to constant callback specific data. This callback expects it to be a pointer to
       *  the AppTraverser using this callback.
       *  \remarks This function is called from AppTraverser::postTraverseObject for each object with app callbacks
       *  registered.
       *  \sa AppTraverser, Group, SpotLight */
      NVSG_API virtual void operator()( const nvsg::Object * subject, const void * data ) const;

      NVSG_API virtual void setSemaphore( const SpotLightUpdateSemaphoreSharedPtr & sem );

      NVSG_API virtual unsigned int getClassID() const;
      NVSG_API virtual const char * getClassName() const;

    private:
      // A pointer to this callback's cooresponding semaphore
      SpotLightUpdateSemaphoreSharedPtr m_sem;
  };

  inline LinkGroupToSpotLightAsTargetCallback::LinkGroupToSpotLightAsTargetCallback( const nvsg::SpotLightSharedPtr & spot )
    : Callback( spot )
  {}

  inline void LinkGroupToSpotLightAsTargetCallback::setSemaphore( const SpotLightUpdateSemaphoreSharedPtr & sem )
  {
    m_sem = sem;
  }

  inline unsigned int LinkGroupToSpotLightAsTargetCallback::getClassID() const
  {
    return *(unsigned int *)"GTST"; // GroupToSpotLightTarget
  }

  inline const char * LinkGroupToSpotLightAsTargetCallback::getClassName() const
  {
    return( "LinkGroupToSpotLightAsTargetCallback" );
  }

    /*! \brief Callback to link a Transform to a SpotLight as its position.
   *  \remarks This kind of callback can be used to position a spotlight. 
   *  This callback must be used in conjunction with a LinkGroupToSpotLightAsTargetCallback to set the direction vector
   *  dynamically.
   *  \sa Group, Callback, AppTraverser, LinkGroupToLightSourceCallback, LinkGroupToSpotLightAsTargetCallback */
  class LinkTransformToSpotLightAsPositionCallback : public nvsg::Callback
  {
    public:
      /*! \brief Constructor of a LinkTransformToSpotLightAsPositionCallback.
       *  \param spot The SpotLight to be used as the observer of this callback.
       *  \sa SpotLight, Group */
      NVSG_API explicit LinkTransformToSpotLightAsPositionCallback( const nvsg::SpotLightSharedPtr & spot );

      NVSG_API ~LinkTransformToSpotLightAsPositionCallback();

      /*! \brief The function operator() of this callback.
       *  \param subject A pointer to the read-locked object that is observed.
       *  \param data A pointer to constant callback specific data. This callback expects it to be a pointer to
       *  the AppTraverser using this callback.
       *  \remarks This function is called from AppTraverser::postTraverseObject for each object with app callbacks
       *  registered.
       *  \sa AppTraverser, Group, SpotLight */
      NVSG_API virtual void operator()( const nvsg::Object * subject, const void * data ) const;

      NVSG_API virtual void setSemaphore( const SpotLightUpdateSemaphoreSharedPtr & sem );

      NVSG_API virtual unsigned int getClassID() const;
      NVSG_API virtual const char * getClassName() const;

    private:
      // A pointer to this callback's cooresponding semaphore
      SpotLightUpdateSemaphoreSharedPtr m_sem;
  };

  inline LinkTransformToSpotLightAsPositionCallback::LinkTransformToSpotLightAsPositionCallback( const nvsg::SpotLightSharedPtr & spot )
    : Callback( spot )
  {}

  inline void LinkTransformToSpotLightAsPositionCallback::setSemaphore( const SpotLightUpdateSemaphoreSharedPtr & sem )
  {
    m_sem = sem;
  }

  inline unsigned int LinkTransformToSpotLightAsPositionCallback::getClassID() const
  {
    return *(unsigned int *)"TTSS"; // Transform To Spotlight State
  }

  inline const char * LinkTransformToSpotLightAsPositionCallback::getClassName() const
  {
    return( "LinkTransformToSpotLightAsPositionCallback" );
  }

} // namespace nvtraverser

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
#include "nvmath/Matnnt.h"
#include "nvmath/Quatt.h"
#include "nvmath/Trafo.h"
#include "nvmath/Vecnt.h"
#include "nvsg/Group.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Transform node. 
   *  \par Namespace: nvsg
   *  \remarks A Transform is a special Group node. In addition to the children held by a
   *  Transform like a Group, it contains a transformation that is appended to the current
   *  transformation. The children then are positioned relative to this transformation.
   *
   *  Beginning with SceniX 7.2 each property P will send out only notify PID_P and PID_Matrix.
   *  PID_Invert is implied with all property changes.
   *  \sa Group */
  class Transform : public Group
  {
    friend class Skin;

    public:
      NVSG_API static TransformSharedPtr create();

    public:
      /*! \brief Get the transformation represented by this Transform.
       *  \return A reference to the constant nvmath::Trafo, representing the transformation.
       *  \sa nvmath::Trafo, setTrafo */
      NVSG_API virtual const nvmath::Trafo& getTrafo( void ) const;

      /*! \brief Set the transformation for this Transform.
       *  \param trafo A reference to the constant nvmath::Trafo to use.
       *  \remarks The transformation \a trafo is copied to the internal one, and the bounding
       *  sphere of the Transform is invalidated.
       *  \sa nvmath::Trafo, getTrafo */
      NVSG_API virtual void setTrafo( const nvmath::Trafo & trafo );

      /*! \brief Test if a Transform is a Joint.
       *  \return \c true, if this Transform is a Joint, otherwise \c false.
       *  \remarks When a Transform is added as a Joint to a Skin, its joint counter is incremented.
       *  If the joint counter of a Transform is positive, it is considered to be a Joint. */
      NVSG_API bool isJoint() const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Transform to copy from
       *  \return A reference to the assigned Transform
       *  \remarks The assignment operator calls the assignment operator of Group, and copies the
       *  transformation.
       *  \sa Group, nvmath::Trafo */
      NVSG_API Transform & operator=(const Transform & rhs);
      
      /*! \brief Test for equivalence with an other Transform. 
       *  \param p A reference to the constant Transform to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the Transform \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as a Group, they are equivalent if they have
       *  the same transformation.
       *  \sa Group, nvmath::Trafo */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, Transform );
    protected:
      friend struct nvutil::Holder<Transform>;

      /*! \brief Default-constructs a Transform. 
       */
      NVSG_API Transform(void);

      /*! \brief Constructs a Transform as a copy of another Transform.
      */
      NVSG_API Transform( const Transform &rhs );

      /*! \brief Destructs a Transform.
       */
      NVSG_API virtual ~Transform(void);

      /*! \brief Calculates the bounding box of this Transform.
       *  \return The axis-aligned bounding box of this Transform.
       *  \remarks This function is called by the framework when re-calculation
       *  of the bounding box is required for this Transform. */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;

      /*! \brief Calculate the bounding sphere of this Transform.
       *  \return A nvmath::Sphere3f that contains the complete Transform.
       *  \remarks This function is called by the framework to determine a sphere that completely
       *  contains the Transform. First, the bounding sphere of the Group is determined, then
       *  this sphere is transformed according to the transformation. */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;

      /*! \brief Increments the number of references as a joint.
       *  \remarks A Transform can be used as a joint by a Skin. For each Skin, that uses this
       *  Transform, its joint count is incremented. Whenever a joint is removed from a Skin, the
       *  joint count of the corresponding Transform is decremented. That is, when the joint count
       *  is larger than zero, this Transform is a joint, otherwise it isn't. This function is
       *  exclusively used by the Skin class.
       *  \sa decrementJointCount */
      NVSG_API void incrementJointCount();

      /*! \brief Decrements the number of references as a joint.
       *  \remarks A Transform can be used as a joint by a Skin. For each Skin, that uses this
       *  Transform, its joint count is incremented. Whenever a joint is removed from a Skin, the
       *  joint count of the corresponding Transform is decremented. That is, when the joint count
       *  is larger than zero, this Transform is a joint, otherwise it isn't. This function is
       *  exclusively used by the Skin class.
       *  \sa decrementJointCount */
      NVSG_API void decrementJointCount();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      void evalChange();

    private:
      nvmath::Trafo m_trafo;
      unsigned int  m_jointCount;
      unsigned int  m_changeCount;
      unsigned int  m_changeThreshold;

      // Property framework
    public:
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( Center );
          NVSG_API DECLARE_STATIC_PROPERTY( Orientation );
          NVSG_API DECLARE_STATIC_PROPERTY( ScaleOrientation );
          NVSG_API DECLARE_STATIC_PROPERTY( Scaling );
          NVSG_API DECLARE_STATIC_PROPERTY( Translation );
          NVSG_API DECLARE_STATIC_PROPERTY( Matrix );
          NVSG_API DECLARE_STATIC_PROPERTY( Inverse );
      END_DECLARE_STATIC_PROPERTIES      

      NVSG_API const nvmath::Vec3f&  getCenter( void ) const           { return getTrafo().getCenter(); }
      NVSG_API const nvmath::Quatf&  getOrientation( void ) const      { return getTrafo().getOrientation(); }
      NVSG_API const nvmath::Quatf&  getScaleOrientation( void ) const { return getTrafo().getScaleOrientation(); }
      NVSG_API const nvmath::Vec3f&  getScaling( void ) const          { return getTrafo().getScaling(); }
      NVSG_API const nvmath::Vec3f&  getTranslation( void ) const      { return getTrafo().getTranslation(); }
      NVSG_API       nvmath::Mat44f  getMatrix( void ) const           { return getTrafo().getMatrix(); }
      NVSG_API       nvmath::Mat44f  getInverse( void ) const          { return getTrafo().getInverse(); }

      NVSG_API void setCenter( const nvmath::Vec3f& value );
      NVSG_API void setOrientation( const nvmath::Quatf& value );
      NVSG_API void setScaleOrientation( const nvmath::Quatf& value );
      NVSG_API void setScaling( const nvmath::Vec3f& value );
      NVSG_API void setTranslation( const nvmath::Vec3f& value );
      NVSG_API void setMatrix( const nvmath::Mat44f& value);
  };

  inline const nvmath::Trafo& Transform::getTrafo( void ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_trafo );
  }

  inline bool Transform::isJoint() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( 0 < m_jointCount );
  }
}

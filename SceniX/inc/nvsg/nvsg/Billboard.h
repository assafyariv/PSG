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

#include "nvmath/Trafo.h"
#include "nvsg/Group.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Class that adds billboard properties to a Group.
   *  \par Namespace: nvsg
   *  \remarks The Billboard is a special transforming Group that performs a rotation either around
   *  and \a rotationAxis, or around the camera's \a upVector, such that the contents of the Group
   *  is either always oriented towards the viewer or screen aligned.
   *  \sa Group, Transform */
  class Billboard : public Group
  {
    public:
      NVSG_API static BillboardSharedPtr create();

    public:
      //! Enumeration type to describe the alignment of a Billboard.
      enum Alignment
      {
        BA_AXIS,    //!< Axis aligned Billboard: rotate around a specified axis.
        BA_VIEWER,  //!< Viewer aligned Billboard: align the Billboard to always face the viewer.
        BA_SCREEN   //!< Screen aligned Billboard: keep the Billboard aligned to the screen.
      };

    public:
      /*! \brief Get the current Trafo of the Billboard.
       *  \param cam Specifies the read-only Camera to use in aligning the Billboard.
       *  \param worldToModel Specifies the current mapping from world to model.
       *  \return An nvmath::Trafo, that represents the transformation of the Billboard.
       *  \remarks Depending on the alignment of the Billboard, a different Trafo is determined.
       *  \sa getAlignment, setAlignment */
      NVSG_API nvmath::Trafo getTrafo( const Camera & cam, const nvmath::Mat44f & worldToModel ) const;

      /*! \brief Get the alignment type of the Billboard.
       *  \return The enumeration value describing the alignment type.
       *  \sa Billboard::Alignment, setAlignment */
      NVSG_API Alignment getAlignment() const;

      /*! \brief Set the Billboard alignment type.
       *  \param ba The enumeration value describing the alignment type.
       *  \sa Billboard::Alignment, getAlignment */
      NVSG_API void setAlignment( Alignment ba );

      /*! \brief Get the rotation axis.
       *  \return A reference to the constant rotation axis.
       *  \remarks The rotation axis specifies which axis to use to perform the rotation. This
       *  axis is defined in the local coordinates of the Billboard. The default is the local
       *  y-axis, that is (0.0,1.0,0.0). The rotation axis is used only if the Billboard is not
       *  to be screen aligned. 
       *  \sa isViewerAligned, setRotationAxis */
      NVSG_API const nvmath::Vec3f & getRotationAxis() const;

      /*! \brief Set the rotation axis.
       *  \param axis A reference to the constant rotation axis.
       *  \remarks The rotation axis specifies which axis to use to perform the rotation. This
       *  axis is defined in the local coordinates of the Billboard. The default is the local
       *  y-axis, that is (0.0,1.0,0.0). The rotation axis is used only if the Billboard is not
       *  to be screen aligned. 
       *  \sa getRotationAxis, setViewerAligned */
      NVSG_API void setRotationAxis( const nvmath::Vec3f &axis );
    
      /*! \brief Assignment operator
       *  \param rhs Reference to the constant Billboard to copy from
       *  \return A reference to the assigned Billboard
       *  \remarks The assignment operator first calls the assignment operator of Group. Then the
       *  rotation axis and the screen alignment flag are copied.
       *  \sa Group */
      NVSG_API Billboard & operator=( const Billboard & rhs );
      
      /*! \brief Test for equivalence with another Billboard. 
       *  \param p            Pointer to the constant Billboard to test for equivalence with
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c
       *  true
       *  \param deepCompare  Optional parameter to perform a deep comparison; default is \c false
       *  \return \c true if the Billboard \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as a group, they are equivalent if they are
       *  both screen aligned, or if they are not screen aligned but have the same rotation axis.
       *  \note The behavior is undefined if \a p is not a Billboard nor derived from one.
       *  \sa Group, getRotationAxis, isViewerAligned */
      NVSG_API virtual bool isEquivalent( const Object *p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      // reflected properties
    public:
      REFLECTION_INFO_API( NVSG_API, Billboard );

      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( Alignment );
          NVSG_API DECLARE_STATIC_PROPERTY( RotationAxis );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      friend struct nvutil::Holder<Billboard>;

      /*! \brief Default-constructs a Billboard. 
       *  \remarks. The Billboard initially has the y-axis as rotation axis and is not screen 
       *  aligned. */
      NVSG_API Billboard();

      /*! \brief Copy-constructs a Billboard from another Billboard.
       *  \param rhs Source Billboard. */
      NVSG_API Billboard( const Billboard &rhs );

      /*! \brief Destructs a Billboard. */
      NVSG_API virtual ~Billboard();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      void getTrafoAxisAligned( const Camera & camera, const nvmath::Mat44f & worldToModel, nvmath::Trafo & trafo ) const;
      void getTrafoScreenAligned( const Camera & camera, const nvmath::Mat44f & worldToModel, nvmath::Trafo & trafo ) const;
      void getTrafoViewerAligned( const Camera & camera, const nvmath::Mat44f & worldToModel, nvmath::Trafo & trafo ) const;

    private:
      Alignment     m_alignment;
      nvmath::Vec3f m_rotationAxis;
      bool          m_viewerAligned;
  };

  inline const nvmath::Vec3f & Billboard::getRotationAxis() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_rotationAxis );
  }

  inline Billboard::Alignment Billboard::getAlignment() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_alignment );
  }

  inline void Billboard::setRotationAxis( const nvmath::Vec3f &rotationAxis )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_rotationAxis != rotationAxis )
    {
      m_rotationAxis = rotationAxis;
      notifyChange( this );
      notify( PID_RotationAxis );
    }
  }

  inline void Billboard::setAlignment( Alignment ba )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_alignment != ba )
    {
      m_alignment = ba;
      notifyChange( this );
      notify( PID_Alignment );
    }
  }
}

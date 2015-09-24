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
/** @file */

#include "nvsgcommon.h"
#include "nvsg/Drawable.h" // base class definition
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Base class for Drawable objects holding a VertexAttributeSet.
   *  \par Namespace: nvsg
   *  \remarks This class provides the common interface for Drawable object with a
   *  VertexAttributeSet, like IndependentPrimitiveSet or StrippedPrimitiveSet.
   *  \sa Drawable, IndependentPrimitiveSet, StrippedPrimitiveSet, VertexAttributeSet */
  class PrimitiveSet : public Drawable
  {
    public:
      /*! \brief Destructs a PrimitiveSet */
      NVSG_API virtual ~PrimitiveSet();

      /*! \brief Get a pointer to the constant VertexAttributeSet.
       *  \return A pointer to the constant VertexAttributeSet.
       *  \sa VertexAttributeSet */
      NVSG_API const VertexAttributeSetSharedPtr & getVertexAttributeSet() const;

      /*! \brief Set the VertexAttributeSet.
       *  \param svash A smart pointer to the VertexAttributeSet.
       *  \sa VertexAttributeSet */
      NVSG_API void setVertexAttributeSet( const VertexAttributeSetSharedPtr & svash );

      /*! \brief Get the Skin of this PrimitiveSet.
       *  \return The Skin of this PrimitiveSet.
       *  \remarks If a PrimitiveSet holds a Skin, the associated VertexAttributeSet is skinned
       *  using that Skin object.
       *  \sa setSkin, getSkinnedVertexAttributeSet, getVertexAttributeSet */
      NVSG_API const SkinSharedPtr & getSkin() const;

      /*! \brief Set the Skin of this PrimitiveSet.
       *  \param skin The Skin to set.
       *  \remarks If a PrimitiveSet holds a Skin, the associated VertexAttributeSet is skinned
       *  using that Skin object.
       *  \sa getSkin, getVertexAttributeSet */
      NVSG_API void setSkin( const SkinSharedPtr & skin );

      /*! \brief Test for equivalence with an other PrimitiveSet.
       *  \param p A reference to the constant PrimitiveSet to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the PrimitiveSet \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Drawable, they are equivalent if the
       *  VertexAttributeSet objects are equivalent. If \a deepCompare is \c true, a full
       *  equivalence test is performed on the VertexAttributeSet objects, otherwise they are
       *  considered to be equivalent if the pointers are equal.
       *  \note The behavior is undefined if \a p is not a PrimitiveSet nor derived from one.
       *  \sa Drawable, VertexAttributeSet */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, PrimitiveSet );
  protected:
      /*! \brief Protected default constructor to prevent explicit creation.
       *  \remarks This default constructor is accessible only by derived classes. By default, it
       *  holds no VertexAttributeSet. */
      NVSG_API PrimitiveSet();

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \param rhs A reference to the constant PrimitiveSet to copy from
       *  \remarks This copy constructor is accessible only by derived classes. The new
       *  PrimitiveSet holds a copy of the VertexAttributeSet of \a rhs. */
      NVSG_API PrimitiveSet( const PrimitiveSet& rhs );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant PrimitiveSet to copy from.
       *  \return A reference to the assigned PrimitiveSet.
       *  \remarks This assignment operator is accessible only by derived classes. It calls the
       *  assignment operator of PrimitiveSet. */
      NVSG_API PrimitiveSet & operator=( const PrimitiveSet & rhs );

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;
      NVSG_API virtual bool determineAnimationContainment() const;

      /*! \brief Override to specialize calculation of texture coords */
      NVSG_API virtual void calculateTexCoords(TextureCoordType type, unsigned int texcoords, bool overwrite);

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

  private:
      SkinSharedPtr               m_skin;
      VertexAttributeSetSharedPtr m_vertexAttributeSet;
  };

  NVSG_API void copy( const PrimitiveSetReadLock & src, const PrimitiveSetWriteLock & dst );

  inline const VertexAttributeSetSharedPtr & PrimitiveSet::getVertexAttributeSet() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_vertexAttributeSet );
  }

  inline const SkinSharedPtr & PrimitiveSet::getSkin() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_skin );
  }

} //  namespace nvsg

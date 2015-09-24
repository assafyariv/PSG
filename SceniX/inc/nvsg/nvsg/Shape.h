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
#include "nvsg/Drawable.h"
#include "nvsg/Primitive.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Abstract base class for shaped geometries like Boxes, Spheres, etc.
   *  \par Namespace: nvsg
   *  \remarks This class provides the common interface for shaped geometries, holding zero or
   *  more Primitive objects.
   *  \sa Drawable, Primitive */
  class Shape : public Drawable
  {
    public:
      /*! \brief The container type of the Primitives */
      typedef std::vector<PrimitiveSharedPtr>                                   PrimitiveContainer;

      /*! \brief The iterator over the PrimitiveContainer */
      typedef nvutil::SceniXIterator<Shape,PrimitiveContainer::iterator>        PrimitiveIterator;

      /*! \brief The const iterator over the PrimitiveContainer */
      typedef nvutil::SceniXIterator<Shape,PrimitiveContainer::const_iterator>  PrimitiveConstIterator;

    public:
      /*! \brief Destructs a Shape. */
      NVSG_API virtual ~Shape();

      /*! \brief Get the number of Primitives in this Shape.
       *  \return The number of Primitives in this Shape.
       *  \sa beginPrimitives, endPrimitives, addPrimitive, removePrimitive, clearPrimitives, findPrimitive */
      NVSG_API unsigned int getNumberOfPrimitives() const;

      /*! \brief Get a const iterator to the first Primitive in this Shape.
       *  \return A const iterator to the first Primitive in this Shape.
       *  \sa getNumberOfPrimitives, endPrimitives, addPrimitive, removePrimitive, clearPrimitives, findPrimitive */
      NVSG_API PrimitiveConstIterator beginPrimitives() const;

      /*! \brief Get an iterator to the first Primitive in this Shape.
       *  \return An iterator to the first Primitive in this Shape.
       *  \sa getNumberOfPrimitives, endPrimitives, addPrimitive, removePrimitive, clearPrimitives, findPrimitive */
      NVSG_API PrimitiveIterator beginPrimitives();

      /*! \brief Get a const iterator that points just beyond the end of the Primitive in this Shape.
       *  \return A const iterator that points just beyond the end of the Primitive in this Shape.
       *  \sa getNumberOfPrimitives, beginPrimitives, addPrimitive, removePrimitive, clearPrimitives, findPrimitive */
      NVSG_API PrimitiveConstIterator endPrimitives() const;

      /*! \brief Get an iterator that points just beyond the end of the Primitive in this Shape.
       *  \return An iterator that points just beyond the end of the Primitive in this Shape.
       *  \sa getNumberOfPrimitives, beginPrimitives, addPrimitive, removePrimitive, clearPrimitives, findPrimitive */
      NVSG_API PrimitiveIterator endPrimitives();

      /*  \brief Find a specified Primitive in this Shape.
       *  \param primitive The Primitive to find.
       *  \return A const iterator to the found Primitive in this Shape.
       *  \sa getNumberOfPrimitives, beginPrimitives, endPrimitives, addPrimitive, removePrimitive, clearPrimitives */
      NVSG_API PrimitiveConstIterator findPrimitive( const PrimitiveSharedPtr & primitive ) const;

      /*  \brief Find a specified Primitive in this Shape.
       *  \param primitive The Primitive to find.
       *  \return An iterator to the found Primitive in this Shape.
       *  \sa getNumberOfPrimitives, beginPrimitives, endPrimitives, addPrimitive, removePrimitive, clearPrimitives */
      NVSG_API PrimitiveIterator findPrimitive( const PrimitiveSharedPtr & primitive );

      /*! \brief Test for equivalence with another Shape.
       *  \param p A reference to the constant Shape to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the Shape \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Drawable, they are equivalent if their
       *  Primitive objects are pair-wise equivalent. If \a deepCompare is \c false, the
       *  Primitive objects are considered to be equivalent if they are pairwise the same
       *  pointers; otherwise, a full equivalence test on each pair is initiated.
       *  \sa Drawable, Primitive */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, Shape );
    protected:

      NVSG_API virtual void setDrawCallback(DrawCallback * cb);

      /*! \brief Protected default constructor to prevent explicit creation.
       *  \remarks This default constructor is accessible only by derived classes. By default, a
       *  Shape holds no Primitive objects. */
      NVSG_API Shape();

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \param rhs A reference to the constant Shape to copy from
       *  \remarks This copy constructor is accessible only by derived classes. The new Shape
       *  holds a copy of the Primitive objects of \a rhs. */
      NVSG_API Shape( const Shape& rhs );

      /*! \brief Adds a Primitive to this Shape.
       *  \param primitive Specifies the Primitive to add
       *  \return An iterator that points to the position where \a Primitive was added.
       *  \sa getNumberOfPrimitives, beginPrimitives, endPrimitives, removePrimitive, clearPrimitives, findPrimitive */
      NVSG_API PrimitiveIterator addPrimitive( const PrimitiveSharedPtr & primitive );

      /*! \brief Remove a Primitive from this Shape.
       *  \param primitive The Primitive to remove from this Shape.
       *  \return \c true, if the Primitive has been removed from this Shape, otherwise \c false.
       *  \sa getNumberOfPrimitives, beginPrimitives, endPrimitives, addPrimitive, clearPrimitives, findPrimitive */
      NVSG_API bool removePrimitive( const PrimitiveSharedPtr & primitive );

      /*! \brief Remove a Primitive from this Shape.
       *  \param spi An iterator to the Primitive to remove from this Shape.
       *  \return An iterator pointing to the new location of the Primitive that followed the one removed by
       *  this function call, which is endPrimitives() if the operation removed the last Primitive.
       *  \sa getNumberOfPrimitives, beginPrimitives, endPrimitives, addPrimitive, clearPrimitives, findPrimitive */
      NVSG_API PrimitiveIterator removePrimitive( const PrimitiveIterator & spi );

      /*! \brief Remove all Primitives from this Shape.
       *  \sa getNumberOfPrimitives, beginPrimitives, endPrimitives, addPrimitive, removePrimitive, findPrimitive */
      NVSG_API void clearPrimitives();

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Calculates the bounding box of this Shape.
       *  \return The axis-aligned bounding box of this Shape.
       *  \remarks This function is called by the framework when re-calculation
       *  of the bounding box is required for this Shape. */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;

      /*! \brief Calculate the bounding sphere of this Shape.
       *  \return The bounding sphere of this Shape.
       *  \remarks This function is called by the framework to determine the sphere to be used as
       *  the bounding sphere. It combines the bounding spheres of the Primitive objects of
       *  this Shape. If there are none, an invalid sphere is returned.
       *  \sa nvmath::Sphere3f, Primitive */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;

      /*! \brief Override to specialize normals calculation for Shapes */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Override to specialize calculation of texture coords for Shapes */
      NVSG_API virtual void calculateTexCoords(TextureCoordType type, unsigned int texcoords, bool overwrite);

      /*! \brief Override to specialize tangent space calculation for Shapes */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Shape to copy from.
       *  \return A reference to the assigned Shape.
       *  \remarks This assignment operator is accessible only by derived classes. It calls the
       *  assignment operator of Drawable, decreases the reference count of all its Primitive
       *  objects, and copies the Primitive objects from \a rhs. */
      NVSG_API Shape & operator=(const Shape & rhs);

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      NVSG_API PrimitiveContainer::iterator doRemovePrimitive( PrimitiveContainer::iterator pci );

    private:
      PrimitiveContainer m_primitives;
  };

  inline unsigned int Shape::getNumberOfPrimitives() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_primitives.size()) );
  }

  inline Shape::PrimitiveConstIterator Shape::beginPrimitives() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( PrimitiveConstIterator( m_primitives.begin() ) );
  }

  inline Shape::PrimitiveIterator Shape::beginPrimitives()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( PrimitiveIterator( m_primitives.begin() ) );
  }

  inline Shape::PrimitiveConstIterator Shape::endPrimitives() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( PrimitiveConstIterator( m_primitives.end() ) );
  }

  inline Shape::PrimitiveIterator Shape::endPrimitives()
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    return( PrimitiveIterator( m_primitives.end() ) );
  }

  inline Shape::PrimitiveConstIterator Shape::findPrimitive( const PrimitiveSharedPtr & camera ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    return( PrimitiveConstIterator( find( m_primitives.begin(), m_primitives.end(), camera ) ) );
  }

  inline Shape::PrimitiveIterator Shape::findPrimitive( const PrimitiveSharedPtr & camera )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    return( PrimitiveIterator( find( m_primitives.begin(), m_primitives.end(), camera ) ) );
  }

  inline void Shape::setDrawCallback(DrawCallback * cb)
  {
    NVSG_ASSERT(!"Draw Callbacks are not supported by the shape class");
  }

} //  namespace nvsg

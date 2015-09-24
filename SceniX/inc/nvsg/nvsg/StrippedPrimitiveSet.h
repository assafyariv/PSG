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
#include "nvsg/PrimitiveSet.h"
#include "nvsg/nvsg.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Abstract base class for PrimitiveSet objects with points organized as strips.
   *  \par Namespace: nvsg
   *  \remarks This class holds a vector of IndexLists used in stripped primitives like LineStrips,
   *  QuadStrips, and TriStrips. The strips are defined by a vector of IndexList elements. Each
   *  IndexList is a std::vector<unsigned int> and represents one strip.
   *  \sa IndependentPrimitiveSet, LineStrips, MeshedPrimitiveSet, QuadStrips, TriStrips */
  class StrippedPrimitiveSet : public PrimitiveSet
  {
    public:
      /*! \brief Destructs a StrippedPrimitiveSet. */
      NVSG_API virtual ~StrippedPrimitiveSet( void );

      /*! \brief Query whether the data of this StrippedPrimitiveSet is shared with other
       *  StrippedPrimitiveSet objects.
       *  \return \c true, if the data of this StrippedPrimitiveSet is shared with other
       *  StrippedPrimitiveSet objects, otherwise \c false.
       *  \remarks A StrippedPrimitiveSet object shares it's data with another
       *  StrippedPrimitiveSet object, if it was either instantiated as a copy of the other, using
       *  StrippedPrimitiveSet::clone, or if it was assigned to by the other object using the
       *  assignment operator. In that case, the two objects have the same DataID.
       *  \sa clone, getDataID, operator=() */
      NVSG_API virtual bool isDataShared( void ) const;

      /*! \brief Get the unique data identifier of this object.
       *  \return A 64-bit value that uniquely identifies the embedded data.
       *  \remarks A StrippedPrimitiveSet object shares it's data with another
       *  StrippedPrimitiveSet object, if it was either instantiated as a copy of the other, using
       *  StrippedPrimitiveSet::clone, or if it was assigned to by the other object using the
       *  assignment operator. In that case, the two objects have the same DataID.
       *  \sa clone, isDataShared, operator=() */
      NVSG_API virtual DataID getDataID( void ) const;

      /*! \brief Query if this StrippedPrimitiveSet has strips.
       *  \return \c true, if this StrippedPrimitiveSet has at least one IndexList, otherwise \c false.
       *  \sa getNumberOfStrips, getStrips, setStrips */
      NVSG_API bool hasStrips( void )  const;

      /*! \brief Get the number of strips.
       *  \return The number of strips of this StrippedPrimitiveSet.
       *  \sa hasStrips, getStrips, setStrips */
      NVSG_API unsigned int getNumberOfStrips( void ) const;

      /*! \brief Get a pointer to the strips.
       *  \return A pointer to the constant IndexList elements of this StrippedPrimitiveSet.
       *  \sa hasStrips, getNumberOfStrips, setStrips */
      NVSG_API const IndexList * getStrips() const;

      /*! \brief Set the strips of this StrippedPrimitiveSet.
       *  \param strips A pointer to the constant IndexList elements to use.
       *  \param numStrips The number of meshes.
       *  \remarks Copies \a numStrips strips from \a strips into this StrippedPrimitiveSet.
       *  \note The behaviour is undefined, if there are less than \a numStrips values of type
       *  IndexList located at \a strips.
       *  \sa hasStrips, getNumberOfStrips, getStrips */
      NVSG_API void setStrips( const IndexList * strips, unsigned int numStrips );

      /*! \brief Set a subset of strips of this StrippedPrimitiveSet.
       *  \param pos The start position inside the range of pre-existing strips.
       *  \param strips A pointer to the constant strips to use.
       *  \param numStrips The number of strips.
       *  \remarks Copies \a numStrips of IndexList elements from \a strips into this
       *  StrippedPrimitiveSet, starting at position \a pos inside the range of pre-existing
       *  strips.\n
       *  Pre-existing strips in the range [\a pos, \a pos + \a numStrips) will be replaced.
       *  Pre-existing strips outside this range remain untouched.\n
       *  If you specifiy (-1) for \a pos or \a pos specifies the number of strips currently
       *  stored, the strips pointed to by \a strips will be appended to the pre-existing strips.\n
       *  \note The behaviour is undefined, if \a pos is neither a valid position inside the range
       *  of pre-existing strips, nor the number of strips currently stored, nor (-1).
       *  \note The behaviour is undefined, if there are less than \a numStrips values of type
       *  IndexList located at \a strips.
       *  \sa hasStrips, getNumberOfStrips, getStrips */
      NVSG_API void setStrips( unsigned int pos, const IndexList * strips, unsigned int numStrips );

      /*! \brief Test for equivalence with an other StrippedPrimitiveSet.
       *  \param p A reference to the constant StrippedPrimitiveSet to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the StrippedPrimitiveSet \a p is equivalent to \c this, otherwise \c false.
       *  \remarks Two StrippedPrimitiveSet objects are equivalent, if they are equivalent as
       *  PrimitiveSet, and all their IndexList elements are pairwise equal.
       *  \note The behaviour is undefined if \a p is not an StrippedPrimitiveSet or derived from
       *  one.
       *  \sa PrimitiveSet */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare
                                        ) const;

      REFLECTION_INFO_API( NVSG_API, StrippedPrimitiveSet );
    protected:
      /*! \brief Protected constructor to prevent explicit creation.
       *  \remarks The newly created StrippedPrimitiveSet has no strips.
       *  \note A StrippedPrimitiveSet will not be instantiated directly, but through
       *  instantiating a derived object like QuadStrips for example.*/
      NVSG_API StrippedPrimitiveSet( void );

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \param rhs A reference to the constant StrippedPrimitiveSet to copy from.
       *  \remarks The newly created StrippedPrimitiveSet is copied from \a rhs.
       *  \note A StrippedPrimitiveSet will not be instantiated directly, but through
       *  instantiating a derived object like QuadStrips for example.*/
      NVSG_API StrippedPrimitiveSet( const StrippedPrimitiveSet& rhs );

      /*! \brief Calculates the bounding box of this StrippedPrimitiveSet.
       *  \return The axis-aligned bounding box of this StrippedPrimitiveSet.
       *  \remarks This function is called by the framework when re-calculation
       *  of the bounding box is required for this StrippedPrimitiveSet. */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;

      /*! \brief Calculate the bounding sphere of this StrippedPrimitiveSet.
       *  \return A nvmath::Sphere3f that contains the complete StrippedPrimitiveSet.
       *  \remarks This function is called by the framework to determine a sphere that completely
       *  contains the StrippedPrimitiveSet. */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant StrippedPrimitiveSet to copy from.
       *  \return A reference to the assigned StrippedPrimitiveSet.
       *  \remarks The assignment operator calls the assignment operator of PrimitiveSet and
       *  copies the IndexList elements. */
      NVSG_API StrippedPrimitiveSet & operator=(const StrippedPrimitiveSet & rhs);

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      nvutil::SmartPtr<nvutil::RCVector<IndexList> > m_strips;
  };

  NVSG_API void copy( const StrippedPrimitiveSetReadLock & src, const StrippedPrimitiveSetWriteLock & dst );

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline bool StrippedPrimitiveSet::hasStrips()  const 
  { 
    return( !m_strips->empty() ); 
  }

  inline unsigned int StrippedPrimitiveSet::getNumberOfStrips()  const
  {
    return( checked_cast<unsigned int>(m_strips->size()) );
  }

  inline const IndexList * StrippedPrimitiveSet::getStrips( void ) const 
  { 
    NVSG_TRACE();
    return( &(*m_strips)[0] ); 
  }
}

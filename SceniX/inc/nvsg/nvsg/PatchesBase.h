// Copyright NVIDIA Corporation 2009-2011
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
#include "nvsg/Face.h"
#include "nvsg/Primitive.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{  
  /*! \brief Base class for various Patches classes.
   *  \par Namespace: nvsg
   *  \remarks A PatchesBase is an IndependentPrimitiveSet that holds an array of index-tuples of a
   *  specified size. The size of the tuples is specified by the classes deriving form PatchesBase. Each
   *  of these tuples describe a patch with the indices pointing in the array of vertices. All vertex
   *  attributes (like normal, texture,...) are indexed with the same index.
   *  \sa IndependentPrimitiveSet, Points, Lines, Quads */
  class PatchesBase : public Primitive
  {
    public:
      /*! \brief Destructs a PatchesBase. */
      NVSG_API virtual ~PatchesBase();

      /*! \brief Query if this PatchesBase has patches.
       *  \return \c true, if this PatchesBase has at least the number of indices specified by \c
       *  setNumberOfVerticesPerPatch, otherwise \c false.
       *  \remarks Each such set of successive indices represent one patch.
       *  \sa setNumberOfVerticesPerPatch, getNumberOfVerticesPerPatch, getNumberOfPatches */
      NVSG_API bool hasPatches() const;

      /*! \brief Get the number of patches.
       *  \return The number of indices of this Patches divided by the number of indices per patch, as
       *  specified by \c setNumberOfVerticesPerPatch.
       *  \remarks Each such set of successive indices represent one patch.
       *  \sa setNumberOfVerticesPerPatch, getNumberOfVerticesPerPatch, hasPatches */
      NVSG_API unsigned int getNumberOfPatches() const;

      /*! \brief Test for equivalence with an other PatchesBase.
       *  \param p A reference to the constant PatchesBase to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the PatchesBase \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as IndependentPrimitiveSet and have the same number
       *  of vertices per patch.
       *  \note The behavior is undefined if \a p is not a PatchesBase nor derived from one.
       *  \sa IndependentPrimitiveSets */
      NVSG_API virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Returns the higher-level object code for a given object code.
       *  \param oc Object code for which to retrieve the associated higher-level object code.
       *  \return OC_PRIMITIVE, if \c oc is the object code of \a this, otherwise OC_INVALID.
       *  \note A PatchesBase essentially just is a Primitive. Thus, it can be handled as a Primitive,
       *  if the Traverser doesn't have an handler for the Patches.
       *  \sa getObjectCode */
      NVSG_API virtual unsigned int getHigherLevelObjectCode( unsigned int oc ) const;

      REFLECTION_INFO_API( NVSG_API, PatchesBase );
    protected:
      /*! \brief Default-constructs a PatchesBase. */
      NVSG_API PatchesBase();

      /*! \brief Constructs a PatchesBase as a copy from another PatchesBase. */
      NVSG_API PatchesBase( const PatchesBase &rhs );

      /*! \brief Set the number of vertices per patch.
       *  \param verticesPerPatch The number of vertices per patch.
       *  \remarks Each consecutive set of \a verticesPerPatch indices are treated as one patch.
       *  \sa getNumberOfVerticesPerPatch, hasPatches, getNumberOfPatches */
      NVSG_API void setNumberOfVerticesPerPatch( unsigned int verticesPerPatch );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant PatchesBase to copy from.
       *  \return A reference to the assigned PatchesBase.
       *  \remarks The assignment operator calls the assignment operator of IndependentPrimitiveSet. */
      NVSG_API PatchesBase & operator=( const PatchesBase & rhs );

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;
  };

  bool calculatePatchNormals( nvsg::VertexAttributeSet * vas, bool overwrite, const IndexSetSharedPtr & indexSet
                            , unsigned int numberOfPatches, unsigned int verticesPerPatch, unsigned int size );
  bool calculatePatchNormals( nvsg::VertexAttributeSet * vas, bool overwrite, const IndexSetSharedPtr & indexSet
                            , unsigned int numberOfPatches, unsigned int verticesPerPatch, unsigned int width, unsigned int height );
  void calculatePatchTangentSpace( nvsg::VertexAttributeSet * vas, unsigned int tc, unsigned int tg
                                 , unsigned int bn, bool overwrite, IndexSet::ConstIterator<unsigned int> & indices
                                 , unsigned int numberOfPatches, unsigned int verticesPerPatch
                                 , unsigned int size );
  void calculatePatchTangentSpace( nvsg::VertexAttributeSet * vas, unsigned int tc, unsigned int tg
                                 , unsigned int bn, bool overwrite, IndexSet::ConstIterator<unsigned int> & indices
                                 , unsigned int numberOfPatches, unsigned int verticesPerPatch
                                 , unsigned int width, unsigned int height );

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline bool PatchesBase::hasPatches()  const 
  { 
    NVSG_TRACE();
    NVSG_ASSERT( isIndexed() ); // all should be
    unsigned int numIndices = IndexSetReadLock(getIndexSet())->getNumberOfIndices();

    return( getNumberOfVerticesPerPatch() <= numIndices );
  }

  inline unsigned int PatchesBase::getNumberOfPatches()  const
  { 
    NVSG_TRACE();
 
    return getNumberOfPrimitives();
  }

  inline void PatchesBase::setNumberOfVerticesPerPatch( unsigned int verticesPerPatch )
  {
    Primitive::setNumberOfVerticesPerPatch( verticesPerPatch );
  }

  inline unsigned int PatchesBase::getHigherLevelObjectCode( unsigned int oc ) const
  {
    NVSG_TRACE();
    return( ( oc == getObjectCode() ) ? OC_PRIMITIVE : OC_INVALID );
  }
}

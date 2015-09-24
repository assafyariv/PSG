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
#include "nvmath/nvmath.h"
#include "nvsg/Face.h"
#include "nvsg/PatchesBase.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{  
  /*! \brief IndependentPrimitiveSet for triangular patches.
   *  \par Namespace: nvsg
   *  \remarks A TriPatches is an IndependentPrimitiveSet that holds an array of index-tuples of size
   *  (\a size * ( \a size + 1 ))/2. Each of these tuples describe a triangular patch with the indices
   *  pointing in the array of vertices. All vertex attributes (like normal, texture,...) are indexed with
   *  the same index.
   *  \sa IndependentPrimitiveSet, PatchesBase, Patches */
  class TriPatches : public PatchesBase
  {
    public:
      NVSG_API static TriPatchesSharedPtr create();

    public:
      /*! \brief Set the size of the triangular patches.
       *  \param size The number of vertices along an edge of the triangular patches.
       *  \remarks The number of vertices per patch equals (\a size * (\a size + 1))/2. Each such
       *  consecutive set of indices is treated as one patch.
       *  \sa getSize, getNumberOfVerticesPerPatch, hasPatches, getNumberOfPatches */
      NVSG_API void setSize( unsigned int size );

      /*! \brief Get the size of the triangular patches.
       *  \return The number of vertices along an edge of the triangular patches.
       *  \remarks The number of vertices per patch equals (\a size * (\a size + 1))/2. Each such
       *  consecutive set of indices is treated as one patch.
       *  \sa setSize, getNumberOfVerticesPerPatch, hasPatches, getNumberOfPatches */
      NVSG_API unsigned int getSize() const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant TriPatches to copy from.
       *  \return A reference to the assigned TriPatches.
       *  \remarks The assignment operator calls the assignment operator of IndependentPrimitiveSet. */
      NVSG_API TriPatches & operator=( const TriPatches & rhs );

      REFLECTION_INFO_API( NVSG_API, TriPatches );
    protected:
      friend struct nvutil::Holder<TriPatches>;

      /*! \brief Default-constructs a TriPatches. */
      NVSG_API TriPatches();

      /*! \brief Constructs a TriPatches as a copy from another TriPatches. */
      NVSG_API TriPatches( const TriPatches &rhs );

      /*! \brief Destructs a TriPatches. */
      NVSG_API virtual ~TriPatches();

      /*! \brief Override to specialize normals calculation for TriPatches */
      NVSG_API virtual bool calculateNormals( bool overwrite );

      /*! \brief Override to specialize tangent space calculation for TriPatches */
      NVSG_API virtual void calculateTangentSpace( unsigned int texcoords, unsigned int tangents
                                                 , unsigned int binormals, bool overwrite );

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      unsigned int  m_size;
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline void TriPatches::setSize( unsigned int size )
  {
    NVSG_TRACE();
    m_size = size;
    setNumberOfVerticesPerPatch( ( size * ( size + 1 ) ) / 2 );
  }

  inline unsigned int TriPatches::getSize() const
  {
    NVSG_TRACE();
    return( m_size );
  }
}

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
  /*! \brief IndependentPrimitiveSet for quadratic patches.
   *  \par Namespace: nvsg
   *  \remarks A QuadPatches is an IndependentPrimitiveSet that holds an array of index-tuples of size
   *  \a size x \a size. Each of these tuples describe a quadratic patch with the indices pointing in the
   *  array of vertices. All vertex attributes (like normal, texture,...) are indexed with the same index.
   *  \sa IndependentPrimitiveSet, PatchesBase, Patches */
  class QuadPatches : public PatchesBase
  {
    public:
      NVSG_API static QuadPatchesSharedPtr create();

    public:
      /*! \brief Set the size of the quadratic patches.
       *  \param size The number of vertices on the edges of the quadratic patches.
       *  \remarks The number of vertices per patch equals \a size x \a size. Each such consecutive set of
       *  indices is treated as one patch.
       *  \sa getSize, getNumberOfVerticesPerPatch, hasPatches, getNumberOfPatches */
      NVSG_API void setSize( unsigned int size );

      /*! \brief Get the size of the quadratic patches.
       *  \return The number of vertices on the edges of the quadratic patches.
       *  \remarks The number of vertices per patch equals \a size x \a size. Each such consecutive set of
       *  indices is treated as one patch.
       *  \sa setSize, getNumberOfVerticesperPatch, hasPatches, getNumberOfPatches */
      NVSG_API unsigned int getSize() const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant QuadPatches to copy from.
       *  \return A reference to the assigned QuadPatches.
       *  \remarks The assignment operator calls the assignment operator of IndependentPrimitiveSet. */
      NVSG_API QuadPatches & operator=( const QuadPatches & rhs );

      REFLECTION_INFO_API( NVSG_API, QuadPatches );
    protected:
      friend struct nvutil::Holder<QuadPatches>;

      /*! \brief Default-constructs a QuadPatches. */
      NVSG_API QuadPatches();

      /*! \brief Constructs a QuadPatches as a copy from another QuadPatches. */
      NVSG_API QuadPatches( const QuadPatches &rhs );

      /*! \brief Destructs a QuadPatches. */
      NVSG_API virtual ~QuadPatches();

      /*! \brief Override to specialize normals calculation for QuadPatches */
      NVSG_API virtual bool calculateNormals( bool overwrite );

      /*! \brief Override to specialize tangent space calculation for QuadPatches */
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

  inline void QuadPatches::setSize( unsigned int size )
  {
    NVSG_TRACE();
    m_size = size;
    setNumberOfVerticesPerPatch( size * size );
  }

  inline unsigned int QuadPatches::getSize() const
  {
    NVSG_TRACE();
    return( m_size );
  }
}

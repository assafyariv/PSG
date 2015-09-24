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

namespace nvsg
{  
  /*! \brief IndependentPrimitiveSet for triangular patches with four vertices per side.
   *  \par Namespace: nvsg
   *  \remarks A TriPatches4 is an IndependentPrimitiveSet that holds an array of index-tuples of size 10.
   *  They form a triangle with 4 vertices on each side, and one inner vertex. Each of these tuples
   *  describe a triangular patch with the indices pointing in the array of vertices. All vertex
   *  attributes (like normal, texture,...) are indexed with the same index.
   *  \sa IndependentPrimitiveSet, PatchesBase, Patches */
  class TriPatches4 : public PatchesBase
  {
    public:
      NVSG_API static TriPatches4SharedPtr create();

    public:
      /*! \brief Assignment operator
       *  \param rhs A reference to the constant TriPatches4 to copy from.
       *  \return A reference to the assigned TriPatches4.
       *  \remarks The assignment operator calls the assignment operator of IndependentPrimitiveSet. */
      NVSG_API TriPatches4 & operator=( const TriPatches4 & rhs );

      REFLECTION_INFO_API( NVSG_API, TriPatches4 );
    protected:
      friend struct nvutil::Holder<TriPatches4>;

      /*! \brief Default-constructs a TriPatches4. */
      NVSG_API TriPatches4();

      /*! \brief Constructs a TriPatches4 as a copy from another TriPatches4. */
      NVSG_API TriPatches4( const TriPatches4 &rhs );

      /*! \brief Destructs a TriPatches4. */
      NVSG_API virtual ~TriPatches4();

      /*! \brief Override to specialize normals calculation for TriPatches4 */
      NVSG_API virtual bool calculateNormals( bool overwrite );

      /*! \brief Override to specialize tangent space calculation for TriPatches4 */
      NVSG_API virtual void calculateTangentSpace( unsigned int texcoords, unsigned int tangents
                                                 , unsigned int binormals, bool overwrite );
  };

}

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
  /*! \brief IndependentPrimitiveSet for 4x4 patches.
   *  \par Namespace: nvsg
   *  \remarks A QuadPatches4x4 is an IndependentPrimitiveSet that holds an array of 4x4 index-tuples.
   *  Each of these tuples describe a quadratic patch with the indices pointing in the array of vertices.
   *  All vertex attributes (like normal, texture,...) are indexed with the same index.
   *  \sa IndependentPrimitiveSet, PatchesBase, Patches */
  class QuadPatches4x4 : public PatchesBase
  {
    public:
      NVSG_API static QuadPatches4x4SharedPtr create();

    public:
      /*! \brief Assignment operator
       *  \param rhs A reference to the constant QuadPatches4x4 to copy from.
       *  \return A reference to the assigned QuadPatches4x4.
       *  \remarks The assignment operator calls the assignment operator of IndependentPrimitiveSet. */
      NVSG_API QuadPatches4x4 & operator=( const QuadPatches4x4 & rhs );

      REFLECTION_INFO_API( NVSG_API, QuadPatches4x4 );
    protected:
      friend struct nvutil::Holder<QuadPatches4x4>;

      /*! \brief Default-constructs a QuadPatches4x4. */
      NVSG_API QuadPatches4x4();

      /*! \brief Constructs a QuadPatches4x4 as a copy from another QuadPatches4x4. */
      NVSG_API QuadPatches4x4( const QuadPatches4x4 &rhs );

      /*! \brief Destructs a QuadPatches4x4. */
      NVSG_API virtual ~QuadPatches4x4();

      /*! \brief Override to specialize normals calculation for QuadPatches4x4 */
      NVSG_API virtual bool calculateNormals( bool overwrite );

      /*! \brief Override to specialize tangent space calculation for QuadPatches4x4 */
      NVSG_API virtual void calculateTangentSpace( unsigned int texcoords, unsigned int tangents
                                                 , unsigned int binormals, bool overwrite );
  };

}

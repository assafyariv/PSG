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
#include "nvsg/StrippedPrimitiveSet.h"
#include "nvsg/nvsg.h"

namespace nvsg
{
  /*! \brief StrippedPrimitiveSet for triangle strips.
   *  \par Namespace: nvsg
   *  \remarks A TriStrips is a StrippedPrimitiveSet that represents the vertex attributes of the
   *  StrippedPrimitiveSet as triangle strips.\n
   *  If you have, for example, an array of eight vertices in the StrippedPrimitiveSet, one
   *  possibility to make them a cube is, to define three IndexList elements like that:
   *  \code
   *    set[0] = { 0, 1, 2, 3, 4, 5, 6, 7, 0, 1 };
   *    set[1] = { 1, 7, 3, 5 };
   *    set[2] = { 6, 0, 4, 2 };
   *  \endcode
   *  The ordering ensures that the triangles are all drawn with the same orientation so that the
   *  triangle strip correctly form part of a surface. Preserving the orientation is important for
   *  some operations, such as culling.\n
   *  The number of vertices per IndexList must be at least 3 for anything to be drawn.
   *  \sa LineStrips, StrippedPrimitiveSet, TriangleStrips */
  class TriStrips : public StrippedPrimitiveSet
  {
    public:
      NVSG_API static TriStripsSharedPtr create();

    public:
      /*! \brief Assignment operator
       *  \param rhs A reference to the constant TriStrips to copy from.
       *  \return A reference to the assigned TriStrips.
       *  \remarks The assignment operator calls the assignment operator of StrippedPrimitiveSet. */
      NVSG_API TriStrips & operator=(const TriStrips & rhs);

      REFLECTION_INFO_API( NVSG_API, TriStrips );
    protected:
      friend struct nvutil::Holder<TriStrips>;

      /*! \brief Default-constructs a TriStrips. */
      NVSG_API TriStrips();

      /*! \brief Constructs a TriStrips as a copy from another TriStrips. */
      NVSG_API explicit TriStrips( const TriStrips &rhs );

      /*! \brief Destructs a TriStrips. */
      NVSG_API virtual ~TriStrips();

      /*! \brief Override to specialize normals calculation for TriStrips */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Override to specialize tangent space calculation for TriStrips */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);
  };

}

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
#include "nvsg/nvsg.h"
#include "nvsg/StrippedPrimitiveSet.h"

namespace nvsg
{
  /*! \brief StrippedPrimitiveSet for quad strips.
   *  \par Namespace: nvsg
   *  \remarks A TriFans is a StrippedPrimitiveSet that represents the vertex attributes of the
   *  StrippedPrimitiveSet as triangle fans.\n
   *  The number of vertices per IndexList must be at least 3 for anything to be drawn.
   *  \sa LineStrips, StrippedPrimitiveSet, TriStrips */
  class TriFans : public StrippedPrimitiveSet
  {
    public:
      NVSG_API static TriFansSharedPtr create();

    public:
      /*! \brief Assignment operator
       *  \param rhs A reference to the constant TriFans to copy from.
       *  \return A reference to the assigned TriFans.
       *  \remarks The assignment operator calls the assignment operator of StrippedPrimitiveSet. */
      NVSG_API TriFans & operator=(const TriFans & rhs);

      REFLECTION_INFO_API( NVSG_API, TriFans );
    protected:
      friend struct nvutil::Holder<TriFans>;

      /*! \brief Default-constructs a TriFans. */
      NVSG_API TriFans();

      /*! \brief Constructs a TriFans as a copy from another TriFans. */
      NVSG_API explicit TriFans( const TriFans &rhs );

      /*! \brief Destructs a TriFans. */
      NVSG_API virtual ~TriFans();

      /*! \brief Override to specialize normals calculation for TriFans */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Override to specialize tangent space calculation for TriFans */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);
  };

}

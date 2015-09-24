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
#include "nvsg/MeshedPrimitiveSet.h"

namespace nvsg
{  
  /*! \brief MeshedPrimitiveSet for quad meshes.
   *  \par Namespace: nvsg
   *  \remarks A QuadMeshes is a MeshedPrimitiveSet that interprets the vertex attributes and the
   *  vector of IndexMesh objects of a MeshedPrimitiveSet as a mesh of quads.
   *  \sa IndependentPrimitiveSet, StrippedPrimitiveSet */
  class QuadMeshes : public MeshedPrimitiveSet
  {
    public:
      NVSG_API static QuadMeshesSharedPtr create();

    public:
      /*! \brief Assignment operator
       *  \param rhs A reference to the constant QuadMeshes to copy from.
       *  \return A reference to the assigned QuadMeshes.
       *  \remarks The assignment operator calls the assignment operator of MeshedPrimitiveSet. */
      NVSG_API QuadMeshes & operator=( const QuadMeshes & rhs );

      REFLECTION_INFO_API( NVSG_API, QuadMeshes );
    protected:
      friend struct nvutil::Holder<QuadMeshes>;

      /*! \brief Default-constructs a QuadMeshes.
       *  \remarks After instantiation the QuadMeshes initially is empty. */
      NVSG_API QuadMeshes();

      /*! \brief Copy-Constructs a QuadMeshes from another QuadMeshes.
       *  \param rhs Source QuadMeshes. */
      NVSG_API QuadMeshes( const QuadMeshes &rhs );

      /*! \brief Destructs a QuadMeshes. */
      NVSG_API virtual ~QuadMeshes();

      /*! \brief Override to specialize normals calculation for QuadMeshes */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Override to specialize tangent space calculation for QuadMeshes */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);
  };

}

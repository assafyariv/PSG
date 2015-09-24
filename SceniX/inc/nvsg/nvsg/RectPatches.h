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
#include "nvsg/PatchesBase.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{  
  /*! \brief IndependentPrimitiveSet for rectangular patches.
   *  \par Namespace: nvsg
   *  \remarks A RectPatches is an IndependentPrimitiveSet that holds an array of index-tuples of size
   *  width x height. Each of these tuples describe a rectangular patch with the indices pointing in the
   *  array of vertices. All vertex attributes (like normal, texture,...) are indexed with the same index.
   *  \sa IndependentPrimitiveSet, PatchesBase, Patches */
  class RectPatches : public PatchesBase
  {
    public:
      NVSG_API static RectPatchesSharedPtr create();

    public:
      /*! \brief Set width and height of the rectangular patches.
       *  \param width The number of vertices along the width of the patches.
       *  \param height The number of vertices along the height of the patches.
       *  \remarks The number of vertices per patch equals width x height. Each such consecutive set of
       *  indices is treated as one patch.
       *  \sa getWidth, getHeight, getNumberOfVerticesPerPatch, hasPatches, getNumberOfPatches */
      NVSG_API void setSize( unsigned int width, unsigned int height );

      /*! \brief Get the width of the rectangular patches.
       *  \return The number vertices along the width of the patches.
       *  \remarks The number of vertices per patch equals width x height. Each such consecutive set of
       *  indices is treated as one patch.
       *  \sa setSize, getNumberOfVerticesPerPatch, hasPatches, getNumberOfPatches */
      NVSG_API unsigned int getWidth() const;

      /*! \brief Get the height of the rectangular patches.
       *  \return The number vertices along the height of the patches.
       *  \remarks The number of vertices per patch equals width x height. Each such consecutive set of
       *  indices is treated as one patch.
       *  \sa setSize, getNumberOfVerticesPerPatch, hasPatches, getNumberOfPatches */
      NVSG_API unsigned int getHeight() const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant RectPatches to copy from.
       *  \return A reference to the assigned RectPatches.
       *  \remarks The assignment operator calls the assignment operator of PatchesBase. */
      NVSG_API RectPatches & operator=( const RectPatches & rhs );

      REFLECTION_INFO_API( NVSG_API, RectPatches );
    protected:
      friend struct nvutil::Holder<RectPatches>;

      /*! \brief Default-constructs a RectPatches. */
      NVSG_API RectPatches();

      /*! \brief Constructs a RectPatches as a copy from another RectPatches. */
      NVSG_API RectPatches( const RectPatches &rhs );

      /*! \brief Destructs a RectPatches. */
      NVSG_API virtual ~RectPatches();

      /*! \brief Override to specialize normals calculation for RectPatches */
      NVSG_API virtual bool calculateNormals( bool overwrite );

      /*! \brief Override to specialize tangent space calculation for RectPatches */
      NVSG_API virtual void calculateTangentSpace( unsigned int texcoords, unsigned int tangents
                                                 , unsigned int binormals, bool overwrite );

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      unsigned int  m_width;
      unsigned int  m_height;
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline void RectPatches::setSize( unsigned int width, unsigned int height )
  {
    NVSG_TRACE();
    m_width = width;
    m_height = height;
    setNumberOfVerticesPerPatch( width * height );
  }

  inline unsigned int RectPatches::getWidth() const
  {
    NVSG_TRACE();
    return( m_width );
  }

  inline unsigned int RectPatches::getHeight() const
  {
    NVSG_TRACE();
    return( m_height );
  }
}

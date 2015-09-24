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
#include "nvsg/Face.h"
#include "nvsg/IndependentPrimitiveSet.h"

namespace nvsg
{
  /*! \brief IndependentPrimitiveSet for quads.
   *  \par Namespace: nvsg
   *  \remarks A Quads is an IndependentPrimitiveSet that holds an array of index-quadruples. Each
   *  of these quadruples describe a quad with the indices pointing in the array of vertices. All
   *  vertex attributes (such as normal, texture,...) are indexed with the same index.
   *  \sa IndependentPrimitiveSet, Points, Lines, Triangles */
  class Quads : public IndependentPrimitiveSet
  {
    public:
      NVSG_API static QuadsSharedPtr create();

    public:
      /*! \brief Query if this Quads has faces.
       *  \return \c true, if this Quads has at least four indices, otherwise \c false.
       *  \remarks Each quadruple of successive indices represent one quadrangular face.
       *  \sa getNumberOfFaces, getFaces, setFaces */
      NVSG_API bool hasFaces() const;

      /*! \brief Get the number of quadrangular faces.
       *  \return The number of indices of this Quads divided by four.
       *  \remarks Each quadruple of successive indices represent one quadrangular face.
       *  \sa hasFaces, getFaces, setFaces */
      NVSG_API unsigned int getNumberOfFaces() const;

      /*! \brief Get a pointer to the faces.
       *  \return A pointer to the constant indices of this IndependentPrimitiveSet.
       *  \remarks Each quadruple of successive indices represent one quadrangular face.
       *  \sa hasFaces, getNumberOfFaces, setFaces */
      NVSG_API const Face4 * getFaces() const;

      /*! \brief Set the faces of this Quads.
       *  \param faces A pointer to the constant faces to use.
       *  \param numFaces The number of faces, with four indices per face.
       *  \remarks Copies four times \a numFaces indices from \a faces into this Quads by just
       *  calling the appropriate setIndices.
       *  \note The behavior is undefined if there are less than \a numFaces values of type
       *  nvsg::Face4 located at \a faces.
       *  \sa hasFaces, getNumberOfFaces, getFaces */
      NVSG_API void setFaces( const Face4 * faces, unsigned int numFaces );

      /*! \brief Set a subset of faces of this Quads.
       *  \param pos The start position inside the range of pre-existing faces.
       *  \param faces A pointer to the constant faces to use.
       *  \param numFaces The number of faces.
       *  \remarks Copies four times \a numFaces indices from \a faces into this Quads, starting
       *  at position four times \a pos inside the range of pre-existing indices.\n
       *  Pre-existing indices in the range [4*\a pos, 4*(\a pos + \a numFaces)) will be replaced.
       *  Pre-existing indices outside this range remain untouched.\n
       *  If you specifiy (-1) for \a pos or \a pos specifies the number of faces currently
       *  stored, the faces pointed to by \a faces will be appended to the pre-existing faces.\n
       *  \note The behavior is undefined if \a pos is neither a valid position inside the range
       *  of pre-existing faces, nor the number of faces currently stored, nor (-1).
       *  \note The behavior is undefined if there are less than \a numFaces values of type
       *  Face4 located at \a segments.
       *  \sa hasFaces, getNumberOfFaces, getFaces */
      NVSG_API void setFaces( unsigned int pos, const Face4 * faces, unsigned int numFaces );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Quads to copy from.
       *  \return A reference to the assigned Quads.
       *  \remarks The assignment operator calls the assignment operator of
       *  IndependentPrimitiveSet. */
      NVSG_API Quads & operator=( const Quads & rhs );


      REFLECTION_INFO_API( NVSG_API, Quads );
    protected:
      friend struct nvutil::Holder<Quads>;

      /*! \brief Default-constructs a Quads. */
      NVSG_API Quads();

      /*! \brief Constructs a Quads as a copy from another Quads. */
      NVSG_API Quads( const Quads &rhs );

      /*! \brief Destructs a Quads. */
      NVSG_API virtual ~Quads();

      /*! \brief Override to specialize normals calculation for QuadMeshes */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Override to specialize tangent space calculation for QuadMeshes */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline bool Quads::hasFaces() const
  { 
    NVSG_TRACE();
    return( 4 <= getNumberOfIndices() );
  }

  inline unsigned int Quads::getNumberOfFaces() const
  { 
    NVSG_TRACE();
    return( getNumberOfIndices() / 4 );
  }

  inline const Face4 * Quads::getFaces() const 
  { 
    NVSG_TRACE();
    return( (const Face4 *) getIndices() );
  }

  inline void Quads::setFaces( const Face4 * pFaces, unsigned int numFaces )
  {
    NVSG_TRACE();
    setIndices( (const unsigned int *) pFaces, 4 * numFaces );
  }

  inline void Quads::setFaces( unsigned int pos, const Face4* pFaces, unsigned int numFaces )
  {
    NVSG_TRACE();
    if ( pos != ~0 )
    {
      pos *= 4;
    }
    setIndices( pos, (const unsigned int *) pFaces, 4 * numFaces );
  }
}

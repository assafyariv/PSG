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
  /*! \brief IndependentPrimitiveSet for triangles.
   *  \par Namespace: nvsg
   *  \remarks A Triangles is an IndependentPrimitiveSet that holds an array of index-triples.
   *  Each of these triples describe a triangle with the indices pointing in the array of
   *  vertices. All vertex attributes (like normal, texture,...) are indexed with the same index.
   *  \sa IndependentPrimitiveSet, Points, Lines, Quads */
  class Triangles : public IndependentPrimitiveSet
  {
    public:
      NVSG_API static TrianglesSharedPtr create();

    public:
      /*! \brief Query if this Triangles has faces.
       *  \return \c true, if this Triangles has at least three indices, otherwise \c false.
       *  \remarks Each triple of successive indices represent one triangular face.
       *  \sa getNumberOfFaces, getFaces, setFaces */
      NVSG_API bool hasFaces() const;

      /*! \brief Get the number of triangular faces.
       *  \return The number of indices of this Triangles divided by three.
       *  \remarks Each triple of successive indices represent one triangular face.
       *  \sa hasFaces, getFaces, setFaces */
      NVSG_API unsigned int getNumberOfFaces() const;

      /*! \brief Get a pointer to the faces.
       *  \return A pointer to the constant indices of this IndependentPrimitiveSet.
       *  \remarks Each triple of successive indices represent one triangular face.
       *  \sa hasFaces, getNumberOfFaces, setFaces */
      NVSG_API const Face3 * getFaces() const;

      /*! \brief Set the faces of this Triangles.
       *  \param faces A pointer to the constant faces to use.
       *  \param numFaces The number of faces, with three indices per face.
       *  \remarks Copies three times \a numFaces indices from \a faces into this Triangles by
       *  just calling the appropriate setIndices.
       *  \note The behavior is undefined if there are less than \a numFaces values of type
       *  nvsg::Face3 located at \a faces.
       *  \sa hasFaces, getNumberOfFaces, getFaces */
      NVSG_API void setFaces( const Face3 * faces, unsigned int numFaces );

      /*! \brief Set a subset of faces of this Triangles.
       *  \param pos The start position inside the range of pre-existing faces.
       *  \param faces A pointer to the constant faces to use.
       *  \param numFaces The number of faces.
       *  \remarks Copies three times \a numFaces indices from \a faces into this Triangles,
       *  starting at position three times \a pos inside the range of pre-existing indices.\n
       *  Pre-existing indices in the range [3*\a pos, 3*(\a pos + \a numFaces)) will be replaced.
       *  Pre-existing indices outside this range remain untouched.\n
       *  If you specifiy (-1) for \a pos or \a pos specifies the number of faces currently
       *  stored, the faces pointed to by \a faces will be appended to the pre-existing faces.\n
       *  \note The behavior is undefined if \a pos is neither a valid position inside the range
       *  of pre-existing faces, nor the number of faces currently stored, nor (-1).
       *  \note The behaviour is undefined if there are less than \a numFaces values of type
       *  Face3 located at \a segments.
       *  \sa hasFaces, getNumberOfFaces, getFaces */
      NVSG_API void setFaces( unsigned int pos, const Face3 * faces, unsigned int numFaces );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Triangles to copy from.
       *  \return A reference to the assigned Triangles.
       *  \remarks The assignment operator calls the assignment operator of 
       *  IndependentPrimitiveSet. */
      NVSG_API Triangles & operator=(const Triangles & rhs);

      REFLECTION_INFO_API( NVSG_API, Triangles );
    protected:
      friend struct nvutil::Holder<Triangles>;

      /*! \brief Default-constructs a Triangles. */
      NVSG_API Triangles();

      /*! \brief Constructs a Triangles as a copy from another Triangles. */
      NVSG_API Triangles( const Triangles &rhs );

      /*! \brief Destructs a Triangles. */
      NVSG_API virtual ~Triangles();

      /*! \brief Override to specialize normals calculation for Triangles */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Override to specialize tangent space calculation for Triangles */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline bool Triangles::hasFaces()  const 
  { 
    NVSG_TRACE();
    return( 3 <= getNumberOfIndices() );
  }

  inline unsigned int Triangles::getNumberOfFaces()  const
  { 
    NVSG_TRACE();
    return( getNumberOfIndices() / 3 );
  }

  inline const Face3 * Triangles::getFaces() const 
  { 
    NVSG_TRACE();
    return( (const Face3 *) getIndices() );
  }

  inline void Triangles::setFaces( const Face3* pFaces, unsigned int numFaces )
  {
    NVSG_TRACE();
    setIndices( (const unsigned int *) pFaces, 3 * numFaces );
  }

  inline void Triangles::setFaces( unsigned int pos, const Face3* pFaces, unsigned int numFaces )
  {
    NVSG_TRACE();
    if ( pos != ~0 )
    {
      pos *= 3;
    }
    setIndices( pos, (const unsigned int *) pFaces, 3 * numFaces );
  }
}

// Copyright NVIDIA Corporation 2002-2009
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
/** @file */

#include "nvsg/nvsgcommon.h"
#include "nvsg/IndexSet.h"
#include <set>

namespace nvsg
{
  /*! \brief Helper class to determine face connectivity from triangle or quad soups.
   *  \remarks This class is used, for example, by the BezierInterpolationTraverser, and the
   *  StrippingTraverser.
   *  \sa BezierInterpolationTraverser, StrippingTraverser */
  class FaceConnections
  {
    public:
      /*! \brief Constructor of a FaceConnections object.
       *  \param p A pointer to the Primitive to handle.
       *  \remarks All the connectivity informations about the Primitive \a p is determined.
       *  Depending on the size of \a p, that might take a while.
       *  \sa Primitive */
      NVSG_API FaceConnections( const nvsg::Primitive * p );

      /*! \brief Disconnect a single face from the connectivity set.
       *  \param faceIndex The index of the face to disconnect.
       *  \remarks The connections from all faces adjacent to this face are removed, reducing the connections
       *  count of those faces accordingly. Essentially, the face \a faceIndex is removed. */
      NVSG_API void disconnectFace( unsigned int faceIndex );

      /*! \brief Disconnect an array of faces from the connectivity set.
       *  \param faceIndices A pointer to face indices to disconnect.
       *  \param faceCount The number of faces to disconnect.
       *  \remarks All faces from \a faceIndices[0] to \a faceIndices[\a faceCount-1] are disconnected.
       *  \sa disconnectFace */
      NVSG_API void disconnectFaces( const unsigned int * faceIndices, unsigned int faceCount );

      /*! \brief Disconnect a list of faces from the connectivity set.
       *  \param list A reference to a list of face indices to disconnect.
       *  \remarks All faces in the list \a list are disconnected.
       *  \sa disconnectFace */
      NVSG_API void disconnectFaces( const std::list<unsigned int> &list );

      /*! \brief Find the longest quad strip in the quad soup and append the respective vertex indices to \a stripIndices.
       *  \param indices A pointer to the indices of the quads.
       *  \param fi Index of the face to start the quad strip determination at.
       *  \param stripIndices A reference to a vector getting the vertex indices of the determined quad strip.
       *  \param stripFaces A reference to a list getting the face indices of the determined quad strip.
       *  \return Length of the longest quad strip found.
       *  \remarks Determines the 'horizontal' and the 'vertical' quad strip including the face \a fi and selects
       *  the longer one.
       *  \sa findLongestTriStrip */
      NVSG_API unsigned int findLongestQuadStrip( nvsg::IndexSet::ConstIterator<unsigned int> &, unsigned int fi
                                                , std::vector<unsigned int> & stripIndices
                                                , std::list<unsigned int> & stripFaces );

      /*! \brief Find the longest tri strip in the triangle soup and append the respective vertex indices to \a stripIndices.
       *  \param indices A pointer to the indices of the triangles.
       *  \param fi Index of the face to start the tri strip determination at.
       *  \param stripIndices A reference to a vector getting the vertex indices of the determined tri strip.
       *  \param stripFaces A reference to a list getting the face indices of the determined tri strip.
       *  \return Length of the longest tri strip found.
       *  \remarks Determines the three possible tri strip including the face \a fi, and selects the longest one.
       *  \fineLongestQuadStrip */
      NVSG_API unsigned int findLongestTriStrip( nvsg::IndexSet::ConstIterator<unsigned int> & indices
                                               , unsigned int fi, std::vector<unsigned int> & stripIndices
                                               , std::list<unsigned int> & stripFaces );

      /*! \brief Determine if a quad can be part of a connected set of 3 x 3 quads.
       *  \param indices A pointer to the indices of the quads.
       *  \param fi Index of the face to use for the test.
       *  \param patchIndices A reference to a vector getting the vertex indices of the determined set of 3 x 3 quads.
       *  \param patchFaces An array to get the 9 face indices forming the QuadPatch4x4.
       *  \return \c true if a set of quads was found, otherwise \c false.
       *  \remarks A QuadPatch4x4 consists of a set of 3 x 3 quads, or 4 x 4 vertices. This function simply checks
       *  the connectivities of the face \a fi and its neighbours, whether it forms such a set of quads.
       *  \sa findTriPatch4 */
      NVSG_API bool findQuadPatch4x4( nvsg::IndexSet::ConstIterator<unsigned int> & indices, unsigned int fi
                                    , std::vector<unsigned int> & patchIndices, unsigned int patchFaces[9] );

      /*! \brief Determine if a triangle can be part of a triangular connected set of 9 triangles.
       *  \param indices A pointer to the indices of the triangles.
       *  \param fi Index of the face to use for the test.
       *  \param patchIndices A reference to a vector getting the vertex indices of the determined set of triangles.
       *  \param patchFaces An array to get the 9 face indices forming the TriPatch4.
       *  \return \c true if a set of triangles was found, otherwise \c false.
       *  \remarks A TriPatch4 consists of a set of 9 triangles, that are arranged to form a triangular shape again.
       *  This function simply checks the connectivities of the face \a fi and it neighours, whether it forms such a
       *  set of triangles.
       *  \sa findQuadPatch4x4 */
      NVSG_API bool findTriPatch4( nvsg::IndexSet::ConstIterator<unsigned int> & indices, unsigned int fi
                                 , std::vector<unsigned int> & patchIndices, unsigned int patchFaces[9] );

      /*! \brief Get all the faces without any neighbours, and clear the list that holds them
       *  \param allIndices A pointer to the indices of the primitive set.
       *  \param zeroIndices A reference to a vector getting the vertex indices of the isolated primitives.
       *  \return The number of faces without any neighbours that were in the connectivity set.
       *  \remarks After having determined all strips or patches out of the primitive set, there still might be some
       *  primitives that had no neighbours, or were isolated in the stripping/patching process. You get all the
       *  vertex indices of those faces with this function.
       *  \sa findLongestQuadStrip, findLongestTriStrip, findQuadPatch4x4, findTriPatch4 */
      NVSG_API unsigned int getAndClearZeroConnectionIndices( nvsg::IndexSet::ConstIterator<unsigned int> & allIndices
                                                            , std::vector<unsigned int> & zeroIndices );

      /*! \brief Get the neighbours of a primitive.
       *  \param fi Index of the face to get the neighbours of.
       *  \param faces A reference to a vector getting the face indices of the neighbours of \a fi.
       *  \sa getNextFaceIndex */
      NVSG_API void getNeighbours( unsigned int fi, std::vector<unsigned int> & faces );

      /*! \brief Get the next face index with at least one neighbour.
       *  \param connectivity Optional parameter to get the connectivity of the next face index.
       *  \return The index of the next face to handle, or ~0 if there is none.
       *  \sa getNeighbours */
      NVSG_API unsigned int getNextFaceIndex( unsigned int * connectivity = NULL );

    private:
      void connectFaces( nvsg::IndexSet::ConstIterator<unsigned int> & indices, unsigned int faceIndex
                       , unsigned int verticesPerFace, unsigned int edgeIndex0, unsigned int edgeIndex1
                       , const std::vector<std::list<unsigned int> > & verticesToFaceLists );

    private:
      std::vector<unsigned int>             m_faceConnections;
      std::vector<unsigned int>             m_faceConnectionCounts;
      std::vector<std::set<unsigned int> >  m_faceSets;
  };
}


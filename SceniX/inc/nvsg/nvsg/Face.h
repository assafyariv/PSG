// Copyright NVIDIA Corporation 2002-2005
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

namespace nvsg
{
  /*! \brief Simple helper class for Lines, Quads, and Triangles.
   *  \par Namespace: nvsg
   *  \remarks A Face is a template class to hold \a n indices, with \a n being the template
   *  parameter. Interpreting these indices as indices into a vertex array, they form a face.
   *  \sa Lines, Quads, Triangles, Segment, Face3, Face4 */
  template<unsigned char n> class Face
  {
    public:
      /*! \brief Random access operator to the indices of the Face.
       *  \param i  The vertex of the Face to get the index for.
       *  \return A reference to the index of a vertex in a vertex array.
       *  \note The behavior is undefined if \a i is greater than or equal to the number of
       *  indices \a n of this Face. */
      template<typename T>
        unsigned int & operator[]( T i );

      /*! \brief Constant random access operator to the indices of the Face.
       *  \param i  The vertex of the Face to get the index for.
       *  \return A reference to the constant index of a vertex in a vertex array.
       *  \note The behavior is undefined if \a i is greater than or equal to the number of
       *  indices \a n of this Face. */
      template<typename T>
        const unsigned int & operator[]( T i ) const;

    public:
      unsigned int m_indices[n];  //!< Indices into a vertex array, forming the Face.
  };

  template<unsigned char n>
  template<typename T>
    inline unsigned int & Face<n>::operator[]( T i )
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    NVSG_ASSERT( i < n );
    return m_indices[i];
  }

  template<unsigned char n>
  template<typename T>
    inline const unsigned int & Face<n>::operator[]( T i ) const
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    NVSG_ASSERT( i < n );
    return m_indices[i];
  }

  typedef Face<2> Segment;  //!<  Shortcut for a 2-element Face (line segment).
  typedef Face<3> Face3;    //!<  Shortcut for a 3-element Face (triangle).
  typedef Face<4> Face4;    //!<  Shortcut for a 4-element Face (quad).
}

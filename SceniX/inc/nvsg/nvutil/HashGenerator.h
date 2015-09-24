// Copyright NVIDIA Corporation 2008
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR a PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#pragma once
/** \file */

#include "nvsg/nvsgapi.h"
#include <string>

namespace nvutil
{
  /*! \brief HashGenerator is the interface defining class for creating a hash out of some arbitrary data.
   *  \par Namespace: nvutil */
  class HashGenerator
  {
    public:
      /*! \brief Add data to the hash generator.
       *  \param input A pointer to the constant data to use hash.
       *  \param byteCount The number of bytes to process at \a input.
       *  \remarks To generate a hash, this function can be called an arbitrary amount of times, each time
       *  adding data to the hashing process. After all data to hash is added, a finalize determines the
       *  hash of all that data.
       *  \sa finalize */
      NVSG_API virtual void update( const unsigned char * input, unsigned int byteCount ) = 0;

      /*! \brief Add data to the hash generator.
       *  \param input A pointer to the constant data to use hash.
       *  \param elementSize size of one element in the input array
       *  \param stride stride in bytes between two elements in the input array
       *  \param elementCount The number of elements to process at \a input.
       *  \remarks To generate a hash, this function can be called an arbitrary amount of times, each time
       *  adding data to the hashing process. After all data to hash is added, a finalize determines the
       *  hash of all that data.
       *  \sa finalize */
      NVSG_API void update( const unsigned char * input, unsigned int elementSize, unsigned int stride, unsigned int elementCount );

      /*! \brief Get the size of the hash
       *  \return The size of the hash.
       *  \remarks A HashGenerator generates a hash of a specific size. This size should be independent of the
       *  data to hash. The function finalize( void * ) assumes, that \a hash points to some memory of the
       *  the required size, to get a copy of the hash.
       *  \sa update, finalize */
      NVSG_API virtual unsigned int getSizeOfHash() const = 0;

      /*! \brief Finalize the hash of the data added via update.
       *  \param hash A pointer to some memory to get the hash.
       *  \remarks To generate a hash, the \c update can be called an arbitrary amount of times, each time adding
       *  data to the hashing process. After all data to hash is added, this function determines the hash value of
       *  all that data. That value is copied to \a hash, while it is assumed, that there is at least as much
       *  memory reserved at that position as the \c getSizeOfHash tells.
       *  \sa getSizeOfHash, update */
      NVSG_API virtual void finalize( void * hash ) = 0;

      /*! \brief Finalize the hash of the data added via update, and get the hash as a string.
       *  \return The hash value converted to a string.
       *  \remarks To generate a hash, the \c update can be called an arbitrary amount of times, each time adding
       *  data to the hashing process. After all data to hash is added, this function determines the hash value of
       *  all that data. That value is converted to a string.
       *  \sa update */
      NVSG_API virtual std::string finalize() = 0;
  };

  inline void HashGenerator::update( const unsigned char * input, unsigned int elementSize, unsigned int stride, unsigned int elementCount )
  {
    if (stride != elementSize)
    {
      for ( unsigned int element = 0;element < elementCount;++element)
      {
        update( input, elementSize );
        input += stride;
      }
    }
    else
    {
      update( input, elementCount * elementSize );
    }
  }

}


// Copyright NVIDIA Corporation 2011
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR a PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

// Implementation of MurmurHash by Austin Appleby.
#pragma once

#include <nvutil/HashGenerator.h>

namespace nvutil
{
  class HashGeneratorMurMur : public HashGenerator
  {
    public:
      NVSG_API HashGeneratorMurMur( unsigned int seed = 0);
      NVSG_API virtual ~HashGeneratorMurMur() {};
   
      // import non-virtual update signature
      using HashGenerator::update;

      // update the hash with the input
      NVSG_API void update( const unsigned char * input, unsigned int byteCount );

      // get the size of the hash value
      NVSG_API virtual unsigned int getSizeOfHash() const { return 4; }

      // do the final hash calculation and get the hash value
      NVSG_API virtual void finalize( void * hash );

      NVSG_API std::string finalize();
  private:
    NVSG_API void updateTail( const unsigned char * & data, unsigned int & len );

    static const unsigned int m_m = 0x5bd1e995;
    static const int m_r = 24;

    static inline void mmix(unsigned int &h, unsigned int & k)
    {
      k *= m_m;
      k ^= k >> m_r;
      k *= m_m;
      h *= m_m;
      h ^= k;
    }

    unsigned int m_hash;
    unsigned int m_tail;
    unsigned int m_count;
    unsigned int m_size;
    unsigned int m_seed;
  };
} // namespace nvutil

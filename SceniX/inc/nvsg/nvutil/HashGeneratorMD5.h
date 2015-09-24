// Copyright NVIDIA Corporation 2002-2008
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

#include <string>
#include <nvutil/HashGenerator.h>
#include <nvutil/Assert.h>

namespace nvutil
{
  /*! \brief HashGenerator class implementing the MD5 algorithm. */
  class HashGeneratorMD5 : public HashGenerator
  {
    public:
      NVSG_API HashGeneratorMD5();
      NVSG_API virtual ~HashGeneratorMD5() {};
   
      // import non-virtual update signature
      using HashGenerator::update;

      // update the hash with the input
      NVSG_API void update( const unsigned char * input, unsigned int byteCount );

      // get the size of the hash value
      NVSG_API virtual unsigned int getSizeOfHash() const;

      // do the final hash calculation and get the hash value
      NVSG_API virtual void finalize( void * hash );

      // do the final hash calculation and get the hash value as a string
      NVSG_API virtual std::string finalize();

    private:
      void doFinalize();
      void init();
      inline void step1( unsigned int & a, unsigned int b, unsigned int c, unsigned int d
                       , unsigned int x, unsigned int s, unsigned int t );
      inline void step2( unsigned int & a, unsigned int b, unsigned int c, unsigned int d
                       , unsigned int x, unsigned int s, unsigned int t );
      inline void step3( unsigned int & a, unsigned int b, unsigned int c, unsigned int d
                       , unsigned int x, unsigned int s, unsigned int t );
      inline void step4( unsigned int & a, unsigned int b, unsigned int c, unsigned int d
                       , unsigned int x, unsigned int s, unsigned int t );
      void transform( const unsigned char block[64] );
   
    private:
      unsigned char m_inputBuffer[64];  // input buffer
      unsigned int m_bitCount[2];      // number of bits, modulo 2^64 (lsb first)
      unsigned int m_hash[4];          // MD5 hash
  };

}


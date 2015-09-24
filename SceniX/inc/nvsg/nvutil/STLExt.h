// Copyright NVIDIA Corporation 2002-2008
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

namespace nvutil
{
  /*! \brief Erases all elements in a set that match keys in the specified range
   *  \param theSet Specifies the set object from which to erase elements
   *  \param first Points to the beginning of the range of keys
   *  \param last Position just beyond the last key in the range
   *  \remarks
   *  This function provides the option of erasing elements from a set that match the keys in a specified range.
   *  This functionality is not provided by the C++ standard library.
   *  \par Example
   *  \code
   *  #include <set>
   *  #include "nvutil/STLExt.h"
   *  
   *  using namespace std;
   *  using namespace nvutil;
   *
   *  int main()
   *  {
   *    set<int> iset;
   *    for ( int i=0; i<10; i++ )
   *      iset.insert( i );
   *
   *    int keys[5] = {0, 2, 4, 6, 8};
   *    set_erase(iset, &keys[0], &keys[5]);
   *  }
   *  \endcode
   */
  template <typename SetType>
  inline void set_erase(SetType& theSet, typename SetType::key_type const * first, typename SetType::key_type const * last)
  { 
#if defined(_WIN32) & !defined (_AVOCADO_WIN8)
    // since the microsoft implementation of set inherits from tree, 
    // we can just use that API directly
    theSet.erase(first, last);
#else
    // others do not inherit from the red-black tree but uses it in their implementation.
    // there is no bypass accessible we could use directly
    for ( /**/ ; first != last; ++first )
      theSet.erase(*first);
#endif
  } 
}



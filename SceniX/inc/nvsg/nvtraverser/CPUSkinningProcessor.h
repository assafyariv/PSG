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
#include "AppTraverser.h"

namespace nvgl
{
  struct GLArrayBufferCache;
}

namespace nvtraverser
{
  //! CPUSkinningProcessor Class
  /** This class provides the mechanism to apply CPU skinning in the app 
   *  post processor. */
  class CPUSkinningProcessor : public AppTraverser::SkinningProcessor
  {
    public:
      //! Default constructor.
      NVSG_API CPUSkinningProcessor();

      //! apply to scene
      NVSG_API virtual void process( bool process );
      NVSG_API virtual AppTraverser::SkinningProcessorType getType();

      //! is this processor supported on the current arch.
      static NVSG_API bool isSupported( void );

    protected:
      NVSG_API virtual void processSkin( const nvsg::SkinSharedPtr & skinwp, const nvsg::VertexAttributeSetSharedPtr & vaswp );
      NVSG_API virtual ~CPUSkinningProcessor();
  };

} // namespace nvtraverser

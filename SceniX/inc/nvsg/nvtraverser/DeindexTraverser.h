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

#include <set>
#include <vector>

#include "nvsgcommon.h"
#include "nvsg/nvsg.h"
#include "nvsg/CoreTypes.h"
#include "nvsg/Primitive.h"
#include "nvtraverser/Traverser.h"

namespace nvtraverser
{  
  //! Traverser to convert indexed Primitive nodes to non-indexed Primitive nodes.
  // \note Use with care! This traverser expands VertexAttributeSets shared by multiple Primitives unconditionally.
  // The resulting Primitives will have their own non-indexed VertexAttributeSet after the DeindexTraverser has been applied.
  // Stripped primitives with primitive restart indices cannot be expanded to single Primitives, that is why
  // the DeindexTraverser::doApply() function will run a DestrippingTraverser over the scene first.
  // Skinned primitives will retain their IndexSet because skinning is only implemented for indexed primitives.
  class DeindexTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API DeindexTraverser(void);

    protected:
      //! Protected destructor to prevent instantiation of a DeindexTraverser.
      NVSG_API virtual ~DeindexTraverser(void);

      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );
      NVSG_API virtual void handlePrimitive( nvsg::Primitive *p );
  };

}

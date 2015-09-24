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

#include <list>
#include <map>
#include <set>
#include "nvsgcommon.h"
#include "nvsg/nvsg.h"
#include "nvsg/Face.h"
#include "nvsg/IndependentPrimitiveSet.h"
#include "nvtraverser/Traverser.h"

namespace nvtraverser
{  

  //! Traverser to convert Triangles to TriStrips, and Quads to QuadStrips
  /** It is strongly recommended to use a VertexUnifyTraverser before using this StrippingTraverser, because it 
    * works only on indices. */
  class StrippingTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API StrippingTraverser(void);

    protected:
      //! Protected destructor to prevent instantiation of a StrippingTraverser.
      NVSG_API virtual ~StrippingTraverser(void);

      //! Clears internal caches on multiple invocations
      /** This in particular is required if the same instance of a StrippingTraverser is 
        * invoked multiple times on different root nodes. */
      NVSG_API virtual void postApply( const nvsg::NodeSharedPtr & root );

      //! Convert each Primitive of type PRIMITIVE_QUADS or PRIMITIVE_TRIANGLES to PRIMITIVE_QUAD_STRIP and PRIMITIVE_TRIANGLE_STRIP, respectively.
      NVSG_API virtual void handlePrimitive( nvsg::Primitive * p );

      //! Set the complexVAS flag to true, to prevent stripping Primitives holding an AnimatedVertexAttributeSet.
      NVSG_API virtual void handleAnimatedVertexAttributeSet( nvsg::AnimatedVertexAttributeSet * p );

      //! Set the complexVAS flag to true, to prevent stripping Primitives holding a Skin.
      NVSG_API virtual void handleSkin( nvsg::Skin * p );

      /*! \brief Test whether this Object should be optimized
       *  \param p A pointer to the constant nvsg::Object to test. */
      NVSG_API virtual bool optimizationAllowed( const nvsg::Object *p );

    private:
      void changeToStrips( nvsg::Primitive * p );

    private:
      bool                                                      m_containsComplexVAS;
      std::map<nvsg::QuadsWeakPtr,nvsg::QuadStripsSharedPtr>    m_quadsToQuadStrips;
      std::map<nvsg::TrianglesWeakPtr,nvsg::TriStripsSharedPtr> m_trianglesToTriStrips;
      std::set<nvsg::GeoNodeWeakPtr>                            m_handledGeoNodes; 
  };
}

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
#include "nvsg/CoreTypes.h"
#include "nvsg/Primitive.h"
#include "nvtraverser/Traverser.h"

namespace nvtraverser
{  
  //! Traverser to convert Legacy primitive types to Primitive
  // \note Currently works with QUADS, TRIANGLES.
  class ConvertPrimitivesTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API ConvertPrimitivesTraverser(void);

    protected:
      //! Protected destructor to prevent instantiation of a ConvertPrimitivesTraverser.
      NVSG_API virtual ~ConvertPrimitivesTraverser(void);

      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );
      NVSG_API virtual void postApply( const nvsg::NodeSharedPtr & root );

      NVSG_API virtual void handleFlipbookAnimation( nvsg::FlipbookAnimation * p );
      NVSG_API virtual void handleLOD( nvsg::LOD * p );
      NVSG_API virtual void handleSwitch( nvsg::Switch * p );

      NVSG_API virtual void handleGeoNode( nvsg::GeoNode *p );

      // independent primitive sets
      NVSG_API virtual void handlePoints( nvsg::Points * p );
      NVSG_API virtual void handleLines( nvsg::Lines * p );
      NVSG_API virtual void handleTriangles( nvsg::Triangles *p );
      NVSG_API virtual void handleQuads( nvsg::Quads *p );

      // stripped primitive sets
      NVSG_API virtual void handleLineStrips( nvsg::LineStrips * p );
      NVSG_API virtual void handleTriStrips( nvsg::TriStrips * p );
      NVSG_API virtual void handleTriFans( nvsg::TriFans * p );
      NVSG_API virtual void handleQuadStrips( nvsg::QuadStrips * p );

      // meshed primitive sets
      NVSG_API virtual void handleQuadMeshes( nvsg::QuadMeshes * p );

      //! note, we currently don't support skinned primitives
      NVSG_API virtual void handleSkin( nvsg::Skin * p );

      NVSG_API nvsg::PrimitiveSharedPtr convertPrimitive( nvsg::PrimitiveType primitiveType, const nvsg::IndependentPrimitiveSetWeakPtr & ip );
      NVSG_API nvsg::PrimitiveSharedPtr convertPrimitive( nvsg::PrimitiveType primitiveType, const nvsg::StrippedPrimitiveSetWeakPtr & sp );
      NVSG_API nvsg::PrimitiveSharedPtr convertPrimitive( nvsg::PrimitiveType primitiveType, const nvsg::MeshedPrimitiveSetWeakPtr & sp );

    private:
      std::vector< std::pair<nvsg::PrimitiveType, nvsg::IndependentPrimitiveSetWeakPtr> > m_independentPrimitives;
      std::vector< std::pair<nvsg::PrimitiveType, nvsg::StrippedPrimitiveSetWeakPtr> >    m_strippedPrimitives;
      std::vector< std::pair<nvsg::PrimitiveType, nvsg::MeshedPrimitiveSetWeakPtr> >      m_meshedPrimitives;

      std::set<nvsg::GeoNodeWeakPtr>     m_handledGeoNodes; 
  };

}

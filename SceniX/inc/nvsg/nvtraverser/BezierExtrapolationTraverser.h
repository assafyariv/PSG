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
#include "nvsg/StateSet.h"
#include "nvtraverser/Traverser.h"
#include "nvutil/SmartPtr.h"
#include <map>
#include <set>

namespace nvtraverser
{  
  //! Traverser to convert Triangles to TriPatches4 and Quads to QuadPatches4x4.
  /** \remark This traverser converts single triangles or quads to entries in a TriPatches4 or a QuadPatches4x4,
    * respectively. It introduces new vertices and vertex attributes. The three vertices of a triangle are
    * extrapolated to 10 vertices of a cubic bezier triangle; the four vertices of a quad are extrapolated to 16
    * vertices of a bicubic bezier quad.
    * \note The patches need to have a CgFx attached that contain a TessellationControlProgram (TCP) and a
    * TessellationEvaluationProgram (TEP). Those programs can't be automatically derived from a given CgFx with just
    * a VertexProgram (VP), possibly a GeometryProgram (GP) and a FragmentProgram (FP).
    * \note This traverser can be used to exchange any triangles or quads not interpolated by the
    * BezierInterpolationTraverser to bezier patches, resulting in a scene containing only patches as geometry.
    * \note It's worth to run the UnifyTraverser with UnifyTarget UT_VERTICES on the extrapolated scene to unify
    * the newly introduced vertices on the edges of the patches. */
  class BezierExtrapolationTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API BezierExtrapolationTraverser(void);

      /*! \brief Set the CgFxs to be used with the newly created patches.
       *  \param triPatchesCgFx The CgFx to use with TriPatches4.
       *  \param quadPatchesCgFx The CgFx to use with QuadPatches4x4.
       *  \remarks The provided CgFxs will be used by the newly created patches of the corresponding types. */
      NVSG_API void setTessellationCgFxs( const nvsg::CgFxSharedPtr & triPatchesCgFx
                                        , const nvsg::CgFxSharedPtr & quadPatchesCgFx );

    protected:
      //! Protected destructor to prevent instantiation of a BezierExtrapolationTraverser.
      NVSG_API virtual ~BezierExtrapolationTraverser(void);

      //! Clears internal caches on multiple invocations
      /** This in particular is required if the same instance of a BezierExtrapolationTraverser is 
        * invoked multiple times on different root nodes. */
      NVSG_API virtual void postApply( const nvsg::NodeSharedPtr & root );

      //! Convert each Quads to a QuadStrips and a Quads, and each Triangles to a TriStrips and a Triangles.
      NVSG_API virtual void handleGeoNode( nvsg::GeoNode *p );

      //! Hold the Primitives to extrapolate in handleGeoNode.
      NVSG_API virtual void handlePrimitive( nvsg::Primitive * p );

      //! Hold the Quads to strip in handleGeoNode.
      NVSG_API virtual void handleQuads( nvsg::Quads *p );

      //! Hold the Triangles to strip in handleGeoNode.
      NVSG_API virtual void handleTriangles( nvsg::Triangles *p );

      //! Set the complexVAS flag to true, to prevent patchifying Primitives holding an AnimatedVertexAttributeSet.
      NVSG_API virtual void handleAnimatedVertexAttributeSet( nvsg::AnimatedVertexAttributeSet * p );

      //! Set the complexVAS flag to true, to prevent patchifying Primitives holding a Skin.
      NVSG_API virtual void handleSkin( nvsg::Skin * p );

    private:
      nvsg::CgFxSharedPtr                   m_cgfxQuadPatches;
      nvsg::CgFxSharedPtr                   m_cgfxTriPatches;
      bool                                  m_containsComplexVAS;
      std::set<nvsg::GeoNodeWeakPtr>        m_handledGeoNodes; 
      std::vector<nvsg::PrimitiveSharedPtr> m_primitives;
      std::vector<nvsg::QuadsSharedPtr>     m_quads;
      nvsg::StateSetReplacementMap          m_stateSetReplacementMap;
      std::vector<nvsg::TrianglesSharedPtr> m_triangles;
  };
}

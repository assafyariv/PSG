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
  //! Traverser to convert sets of Triangles to TriPatches4, and sets of Quads to QuadPatches4x4
  /** \remark This traverser identifies triangular sets of 10 triangles or quadrilateral sets of 9 quads and
    * converts each such set to a TriPatches4 or a QuadPatches4x4, respectively. The number of vertices does not
    * change by this conversion, but due to some triangles or quads not fitting in those sets of triangles or quads,
    * some vertices have to be duplicated.
    * \note The patches need to have a CgFx attached that contain a TessellationControlProgram (TCP) and a
    * TessellationEvaluationProgram (TEP). Those programs can't be automatically derived from a given CgFx with just
    * a VertexProgram (VP), possibly a GeometryProgram (GP) and a FragmentProgram (FP).
    * \note It is strongly recommended to use a VertexUnifyTraverser before using this BezierInterpolationTraverser,
    * because it works only on indices. */
  class BezierInterpolationTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API BezierInterpolationTraverser(void);

      /*! \brief Set the CgFxs to be used with the newly created patches.
       *  \param triPatchesCgFx The CgFx to use with TriPatches4.
       *  \param quadPatchesCgFx The CgFx to use with QuadPatches4x4.
       *  \remarks The provided CgFxs will be used by the newly created patches of the corresponding types. */
      NVSG_API void setTessellationCgFxs( const nvsg::CgFxSharedPtr & triPatchesCgFx
                                        , const nvsg::CgFxSharedPtr & quadPatchesCgFx );

    protected:
      //! Protected destructor to prevent instantiation of a BezierInterpolationTraverser.
      NVSG_API virtual ~BezierInterpolationTraverser(void);

      //! Clears internal caches on multiple invocations
      /** This in particular is required if the same instance of a BezierInterpolationTraverser is 
        * invoked multiple times on different root nodes. */
      NVSG_API virtual void postApply( const nvsg::NodeSharedPtr & root );

      //! Extract a QuadPatches4x4 or a TriPatches4 out of each Primitive of type PRIMITIVE_QUADS or PRIMITIVE_TRIANGLES, respectively.
      NVSG_API virtual void handleGeoNode( nvsg::GeoNode *p );

      //! Generate QuadPatches4x4 or TriPatches4 out of a Primitive of type PRIMITIVE_QUADS or PRIMITIVE_TRIANGLES, respectively.
      NVSG_API virtual void handlePrimitive( nvsg::Primitive * p );

      //! Get the currently active StateSet.
      NVSG_API virtual void handleStateSet( nvsg::StateSet * p );

      //! Set the complexVAS flag to true, to prevent patchifying Primitives holding an AnimatedVertexAttributeSet.
      NVSG_API virtual void handleAnimatedVertexAttributeSet( nvsg::AnimatedVertexAttributeSet * p );

      //! Set the complexVAS flag to true, to prevent pathifying Primitives holding a Skin.
      NVSG_API virtual void handleSkin( nvsg::Skin * p );

      /*! \brief Test whether this Object should be optimized
       *  \param p A pointer to the constant nvsg::Object to test. */
      NVSG_API virtual bool optimizationAllowed( const nvsg::Object *p );

    private:
      nvsg::QuadPatches4x4SharedPtr createQuadPatches4x4FromPrimitive( nvsg::Primitive * p );
      nvsg::TriPatches4SharedPtr    createTriPatches4FromPrimitive( nvsg::Primitive * p );

    private:
      nvsg::CgFxSharedPtr                                                         m_cgfxQuadPatches;
      nvsg::CgFxSharedPtr                                                         m_cgfxTriPatches;
      bool                                                                        m_containsComplexVAS;
      nvsg::StateSet                                                            * m_currentStateSet;
      std::vector<std::pair<nvsg::StateSetSharedPtr,nvsg::PrimitiveSharedPtr> >   m_patchesToAdd;
      std::map<nvsg::Primitive*,nvsg::QuadPatches4x4SharedPtr>                    m_primitivesToQuadPatches4x4;
      std::vector<nvsg::PrimitiveSharedPtr>                                       m_primitivesToRemove;
      std::map<nvsg::Primitive*,nvsg::TriPatches4SharedPtr>                       m_primitivesToTriPatches4;
      nvsg::StateSetReplacementMap                                                m_stateSetReplacementMap;
  };
}

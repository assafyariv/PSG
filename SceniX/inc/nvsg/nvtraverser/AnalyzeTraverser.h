// Copyright NVIDIA Corporation 2002-2007
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
#include "nvsg/IndependentPrimitiveSet.h"
#include "nvsg/StrippedPrimitiveSet.h"
#include "nvtraverser/Traverser.h"

#include <map>
#include <set>

namespace nvtraverser
{
  /*! \brief Base class of the analyzing results determined with the AnalyzeTraverser.
   *  \par Namespace: nvsg
   *  \remarks The AnalyzeResult holds the \a objectCode of the object that this result refers to,
   *  and the number of objects that have been encountered in the scene graph.
   *  \sa AnalyzeTraverser, CombinableResult, DenormalizedNormalsResult, EmptyResult,
   *  EquivalenceResult, IdentityResult, MissingResult, NullNormalsResult, RedundantIndexSetResult,
   *  SingleChildResult, ShortStrippedResult, UnusedVerticesResult */
  class AnalyzeResult
  {
    public:
      AnalyzeResult() : objectCount(0) {}

      /*! \brief Virtual Destructor */
      virtual ~AnalyzeResult() {}

    public:
      unsigned int  objectCode;   //!< The code of objects, this result refers to.
      unsigned int  objectCount;  //!< The number of objects of this type.
  };

  /*! \brief AnalyzeResult indicating that a number of Objects can be combined.
   *  \par Namespace: nvsg
   *  \remarks Reports if, and how many, combinable Objects are in the tree. It tells how many
   *  objects of the type \a objectCode are combinable, and by how many objects they could be
   *  replaced with. Currently, the following objects are checked for combinability: nvsg::GeoNode,
   *  nvsg::Lines, nvsg::LineStrips, nvsg::Points, nvsg::QuadMeshes, nvsg::Quads, nvsg::QuatStrips,
   *  nvsg::Triangles, nvsg::TriFans, and nvsg::TriStrips.
   *  \sa AnalyzeResult AnalyzeTraverser */
  class CombinableResult : public AnalyzeResult
  {
    public:
      virtual ~CombinableResult() {};

    public:
      unsigned int  combinableCount;  //!< The number of combinable objects of this type.
      unsigned int  reducedCount;     //!< The number of objects of this type, if combinable objects would be combined.
  };

  /*! \brief AnalyzeResult indicating a VertexAttributeSet with denormalized normals.
   *  \par Namespace: nvsg
   *  \remarks Reports if there is a VertexAttributeSet with denormalized normals.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class DenormalizedNormalsResult : public AnalyzeResult
  {
    public:
      virtual ~DenormalizedNormalsResult() {}

    public:
      unsigned int  denormalizedNormalsCount;   //!< The number of objects with denormalized normals.
      unsigned int  denormalizedNormalsNumber;  //!< The number of denormalized normals in those objects.
  };

  /*! \brief AnalyzeResult indicating an empty object.
   *  \par Namespace: nvsg
   *  \remarks All empty object in the tree (e.g. Groups without children, Triangles without
   *  indices,...) are reported with an EmptyResult.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class EmptyResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~EmptyResult() {}

    public:
      unsigned int  emptyCount;   //!< The number of empty objects of this type.
  };

  /*! \brief AnalyzeResult indicating equivalent objects.
   *  \par Namespace: nvsg
   *  \remarks All equivalent objects in the tree (e.g. StateAttributes with the same settings)
   *  are reported with an EquivalenceResult.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class EquivalenceResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~EquivalenceResult() {}

    public:
      unsigned int  equivalentCount;  //!< The number of equivalent objects of this type.
      unsigned int  reducedCount;     //!< The number of objects of this type, if equivalent objects would be combined.
  };

  /*! \brief AnalyzeResult indicating identity transformations.
   *  \par Namespace: nvsg
   *  \remarks All Transform objects in the tree that are in fact identity transforms are reported
   *  with an IdentityResult.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class IdentityResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~IdentityResult() {}

    public:
      unsigned int  identityCount;  //!< The number of identity Transforms.
  };

  /*! \brief AnalyzeResult indicating non-complete objects.
   *  \par Namespace: nvsg
   *  \remarks All Objects with missing information (e.g. an AnimatedTransform without an
   *  Animation) are reported with a MissingResult.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class MissingResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~MissingResult()  {}

    public:
      unsigned int  missingCount;   //!< The number of objects with missing information.
  };

  /*! \brief AnalyzeResult indicating a VertexAttributeSet with null normals.
   *  \par Namespace: nvsg
   *  \remarks Reports if there is a VertexAttributeSet with null normals.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class NullNormalsResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~NullNormalsResult() {}

    public:
      unsigned int  nullNormalsCount;   //!< The number of objects with null normals.
      unsigned int  nullNormalsNumber;  //!< The number of null normals in those objects.
  };

  /*! \brief AnalyzeResult indicating redundant IndexSets
   *  \par Namespace: nvsg
   *  \remarks An IndexSet that holds all indices between the smallest and the largest just once are reported with
   *  a RedundantIndexSetResult.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class RedundantIndexSetResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~RedundantIndexSetResult() {}

    public:
      unsigned int redundantIndexSetCount;    //!< The number of redundant index sets.
  };

  /*! \brief AnalyzeResult indicating LODs with redundant ranges
   *  \par Namespace: nvsg
   *  \remarks Two ranges of an LOD are redundant, if they are the same.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class RedundantLODRangesResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~RedundantLODRangesResult() {}

    public:
      unsigned int  lodRanges;          //!< The number of LOD ranges.
      unsigned int  redundantLODs;      //!< The number of LODs with redundant ranges.
      unsigned int  redundantLODRanges; //!< The number of redundant LOD ranges.
  };

  /*! \brief AnalyzeResult indicating a Group with a single child.
   *  \par Namespace: nvsg
   *  \remarks All Group (and Group-derived) objects with only one child are reported with a
   *  SingleChildResult.
   *  \sa AnalyzeResult, AnalyzeTraverser */
  class SingleChildResult : public AnalyzeResult
  {
    public:
      /*! \brief Virtual Destructor */
      virtual ~SingleChildResult()  {}

    public:
      unsigned int  singleChildCount;   //!< The number of objects with a single child.
  };

  /*! \brief AnalyzeResult indicating a StrippedPrimitiveSet with very short strips.
   *  \par Namespace: nvsg
   *  \remarks All QuadStrips with strips of length up to four, and all TriFans and TriStrips
   *  with fans or strips of length up to three are reported.
   *  \sa AnalyzeResult, AnalyzeTraverser, QuadStrips, TriFans, TriStrips */
  class ShortStrippedResult : public AnalyzeResult
  {
    public:
      virtual ~ShortStrippedResult() {}

    public:
      unsigned int  shortStrippedCount;    //!< The number of objects with short strips.
      unsigned int  shortStrippedNumber;   //!< The number of short strips in those objects.
  };

  /*! \brief AnalyzeResult indicating unused vertices.
   *  \par Namespace: nvsg
   *  \remarks: All VertexAttributeSets holding unused vertices are reported.
   *  \sa AnalyzeResult, AnalyzeTraverser, VertexAttributeSet */
  class UnusedVerticesResult : public AnalyzeResult
  {
    public:
      UnusedVerticesResult()
        : AnalyzeResult()
        , unusedVerticesCount(0)
      {
        objectCode = nvsg::OC_VERTEX_ATTRIBUTE_SET;
      }
      virtual ~UnusedVerticesResult() {}

    public:
      unsigned int  unusedVerticesCount;    //!< The number of unused vertices.
  };

  /*! \brief Traverser that analyzes a tree and reports about potential deficiencies.
   *  \par Namespace: nvsg
   *  \remarks The AnalyzeTraverser is a scene graph analyzing tool. It can give you hints on
   *  potential problems in your scene graph.
   *  \par Example
   *  To get the results of the AnalyzeTraverser on a given Scene, do something like that:
   *  \code
   *    SmartPtr<AnalyzeTraverser> analyzeTraverser( new AnalyzeTraverser );
   *    analyzeTraverser->apply( pScene );
   *    std::vector<AnalyzeResult *> results;
   *    unsigned int numberOfResults = analyzeTraverser->getAnalysis( results );
   *    for ( unsigned int i=0 ; i<numberOfResults ; i++ )
   *    {
   *      if ( dynamic_cast<EmptyResult *>( results[i] ) != NULL )
   *      {
   *        EmptyResult * emptyResult = dynamic_cast<EmptyResult *>( results[i] );
   *        // handle empty results
   *      }
   *      else if ( dynamic_cast<EquivalenceResult *>( results[i] ) != NULL )
   *      {
   *        EquivalenceResult * equivalenceResult = dynamic_cast<EquivalenceResult *>( results[i] );
   *        // handle equivalence results
   *      }
   *      else if ( ... )
   *      // handle other known results
   *      // ...
   *      else
   *      {
   *        // handle any unknown AnalyzeResult
   *      }
   *      // delete the result after interpretation
   *      delete results[i];
   *    }
   *  \endcode
   *  \sa AnalyzeResult, Traverser */
  class AnalyzeTraverser : public SharedTraverser
  {
    public:
      /*! \brief Default Constructor */
      NVSG_API AnalyzeTraverser( void );

      /*! \brief Get the analysis results of the latest traversal.
       *  \param results A reference to the vector pointers to an AnalyzeResult object to fill.
       *  \return The number of AnalyzeResult objects generated.
       *  \remarks On return, each element of the vector \a results holds a pointer to an
       *  AnalyzeResult. Those objects are owned by the requester, then. There can be multiple
       *  AnalyzeResult objects of the same type, but referring to different types of objects in
       *  the Scene.
       *  \sa AnalyzeResult */
      NVSG_API unsigned int getAnalysis( std::vector<AnalyzeResult*> &results ) const;

    protected:
      /*! \brief Protected destructor to prevent explicit creation.
       *  \remarks The destructor is never used explicitly, but only when the reference count of
       *  the AnalyzeTraverser is decremented to zero. */
      NVSG_API virtual ~AnalyzeTraverser( void );

      /*! \brief Override of the doApply method.
       *  \param root A pointer to the read-locked root node of the tree to analyze.
       *  \remarks The doApply method is the traversal entry point of a Traverser. The local data
       *  is cleared and Traverser::doApply() is called to start traversal.
       *  \sa nvsg::Scene, Traverser */
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Analyze an nvsg::AnimatedTransform.
       *  \param p A pointer to the read-locked nvsg::AnimatedTransform to analyze.
       *  \remarks An nvsg::AnimatedTransform is tested for emptiness (no children), equivalence
       *  with any previously encountered nvsg::AnimatedTransform, and for a missing nvsg::Animation.
       *  \sa nvsg::AnimatedTransform, EmptyResult, EquivalenceResult, MissingResult */
      NVSG_API virtual void handleAnimatedTransform( const nvsg::AnimatedTransform *p );

      /*! \brief Analyze an nvsg::AnimatedVertexAttributeSet.
       *  \param p A pointer to the read-locked nvsg::AnimatedVertexAttributeSet to analyze.
       *  \remarks An nvsg::AnimatedVertexAttributeSet is tested for emptiness (no vertices),
       *  equivalence with any previously encountered nvsg::AnimatedVertexAttributSet, and if there
       *  are any unnormalized normals.
       *  \sa nvsg::AnimatedVertexAttributeSet, EmptyResult, EquivalenceResult, NullNormalsResult,
       *  DenormalizedNormalsResult */
      NVSG_API virtual void handleAnimatedVertexAttributeSet( const nvsg::AnimatedVertexAttributeSet * p );

      /*! \brief Analyze a nvsg::Billboard.
       *  \param p A pointer to the read-locked nvsg::Billboard to analyze.
       *  \remarks A nvsg::Billboard is tested for emptiness (no children), and equivalence with
       *  any previously encountered nvsg::Billboard.
       *  \sa nvsg::Billboard, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleBillboard( const nvsg::Billboard * p );

      /*! \brief Analyze a nvsg::BlendAttribute.
       *  \param p A pointer to the read-locked nvsg::BlendAttribute to analyze.
       *  \remarks A nvsg::BlendAttribute is tested for equivalence with any previously
       *  encountered nvsg::BlendAttribute.
       *  \sa nvsg::BlendAttribute, EquivalenceResult */
      NVSG_API virtual void handleBlendAttribute( const nvsg::BlendAttribute * p );

      /*! \brief Analyze a nvsg::CgFx.
       *  \param p A pointer to the read-locked nvsg::CgFx to analyze.
       *  \remarks A nvsg::CgFx is tested for equivalence with any previously encountered nvsg::CgFx.
       *  \sa nvsg::CgFx, EquivalenceResult */
      NVSG_API virtual void handleCgFx( const nvsg::CgFx * p );

      /*! \brief Analyze a nvsg::DepthAttribute.
       *  \param p A pointer to the read-locked nvsg::DepthAttribute to analyze.
       *  \remarks A nvsg::DepthAttribute is tested for equivalence with any previously
       *  encountered nvsg::DepthAttribute.
       *  \sa nvsg::DepthAttribute, EquivalenceResult */
      NVSG_API virtual void handleDepthAttribute( const nvsg::DepthAttribute * p );

      /*! \brief Analyze a nvsg::DirectedLight.
       *  \param p A pointer to the read-locked nvsg::DirectedLight to analyze.
       *  \remarks A nvsg::DirectedLight is tested for equivalence with any previously encountered
       *  nvsg::DirectedLight.
       *  \sa nvsg::DirectedLight, EquivalenceResult */
      NVSG_API virtual void handleDirectedLight( const nvsg::DirectedLight * p );

      /*! \brief Analyze a nvsg::FaceAttribute.
       *  \param p A pointer to the read-locked nvsg::FaceAttribute to analyze.
       *  \remarks A nvsg::FaceAttribute is tested for equivalence with any previously encountered
       *  nvsg::FaceAttribute.
       *  \sa nvsg::FaceAttribute, EquivalenceResult */
      NVSG_API virtual void handleFaceAttribute( const nvsg::FaceAttribute * p );

      /*! \brief Analyze a nvsg::FlipbookAnimation.
       *  \param p A pointer to the read-locked nvsg::FlipbookAnimation to analyze.
       *  \remarks A nvsg::FlipbookAnimation is tested for emptiness (no children), equivalence
       *  with any previously encountered nvsg::FlipbookAnimation, and for a missing nvsg::Animation.
       *  \sa nvsg::AnimatedTransform, EmptyResult, EquivalenceResult, MissingResult */
      NVSG_API virtual void handleFlipbookAnimation( const nvsg::FlipbookAnimation *p );

      /*! \brief Analyze a nvsg::FramedAnimationDescription of type unsigned int.
       *  \param p A pointer to the read-locked nvsg::FramedAnimationDescription of type unsigned int to analyze.
       *  \remarks A nvsg::FramedAnimationDescription of type unsigned int is tested for emptiness (no
       *  frames) and for equivalence with any previously encountered nvsg::FramedAnimationDescription of
       *  type unsigned int.
       *  \sa nvsg::FramedAnimationDescription, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleFramedIndexAnimationDescription( const nvsg::FramedAnimationDescription<unsigned int> * p );

      /*! \brief Analyze a nvsg::FramedAnimationDescription of type nvmath::Trafo.
       *  \param p A pointer to the read-locked nvsg::FramedAnimationDescription of type nvmath::Trafo to analyze.
       *  \remarks A nvsg::FramedAnimationDescription of type nvmath::Trafo is tested for emptiness (no
       *  frames) and for equivalence with any previously encountered nvsg::FramedAnimationDescription of
       *  type nvmath::Trafo.
       *  \sa nvsg::FramedAnimationDescription, nvmath::Trafo, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleFramedTrafoAnimationDescription( const nvsg::FramedAnimationDescription<nvmath::Trafo> * p );

      /*! \brief Analyze a nvsg::FramedAnimationDescription of type nvsg::VertexAttribute.
       *  \param p A pointer to the read-locked nvsg::FramedAnimationDescription of type nvsg::VertexAttribute
       *  to analyze.
       *  \remarks A nvsg::FramedAnimationDescription of type nvsg::VertexAttribute is tested for emptiness (no
       *  frames) and for equivalence with any previously encountered nvsg::FramedAnimationDescription of
       *  type nvsg::VertexAttribute.
       *  \sa nvsg::FramedAnimationDescription, nvsg::VertexAttribute, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleFramedVertexAttributeAnimationDescription( const nvsg::FramedAnimationDescription<nvsg::VertexAttribute> * p );

      /*! \brief Analyze a nvsg::GeoNode.
       *  \param p A pointer to the read-locked nvsg::GeoNode to analyze.
       *  \remarks A nvsg::GeoNode is tested for emptiness (no geometries) and for equivalence
       *  with any previously encountered nvsg::GeoNode.
       *  \sa nvsg::GeoNode, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleGeoNode( const nvsg::GeoNode * p );

      /*! \brief Analyze a nvsg::Group.
       *  \param p A pointer to the read-locked nvsg::Group to analyze.
       *  \remarks A nvsg::Group is tested for emptiness (no children), for equivalence with any
       *  previously encountered nvsg::Group, and for holding a single child only.
       *  \sa nvsg::Group, EmptyResult, EquivalenceResult, SingleChildResult */
      NVSG_API virtual void handleGroup( const nvsg::Group * p );

      /*! \brief Analyze an nvsg::Animation of type unsigned int.
       *  \param anim A pointer to the read-locked Animation being traversed.
       *  \remarks An Animation of type unsigned int is tested for emptiness
       *  (no AnimationDescription) and for equivalence with any previously encounted Animation of
       *  type unsigned int.
       *  \sa nvsg::AnimationState, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleIndexAnimation( const nvsg::Animation<unsigned int> * p );

      /*! \brief Analyze a nvsg::LinearInterpolatedAnimationDescription of type nvmath::Trafo.
       *  \param p A pointer to the read-locked nvsg::LinearInterpolatedAnimationDescription of type
       *  nvmath::Trafo to analyze.
       *  \remarks A nvsg::LinearInterpolatedAnimationDescription of type nvmath::Trafo is tested for
       *  emptiness (no keys) and for equivalence with any previously encountered
       *  nvsg::LinearInterpolatedAnimationDescription of type nvmath::Trafo.
       *  \sa nvsg::LinearInterpolatedAnimationDescription, nvmath::Trafo, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleLinearInterpolatedTrafoAnimationDescription( const nvsg::LinearInterpolatedAnimationDescription<nvmath::Trafo> * p );

      /*! \brief Analyze a nvsg::LinearInterpolatedAnimationDescription of type nvsg::VertexAttribute.
       *  \param p A pointer to the read-locked nvsg::LinearInterpolatedAnimationDescription of type
       *  nvsg::VertexAttribute to analyze.
       *  \remarks A nvsg::LinearInterpolatedAnimationDescription of type nvsg::VertexAttribute is tested for
       *  emptiness (no keys) and for equivalence with any previously encountered
       *  nvsg::LinearInterpolatedAnimationDescription of type nvsg::VertexAttribute.
       *  \sa nvsg::LinearInterpolatedAnimationDescription, nvsg::VertexAttribute, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleLinearInterpolatedVertexAttributeAnimationDescription( const nvsg::LinearInterpolatedAnimationDescription<nvsg::VertexAttribute> * p );

      /*! \brief Analyze a nvsg::LineAttribute.
       *  \param p A pointer to the read-locked nvsg::LineAttribute to analyze.
       *  \remarks A nvsg::LineAttribute is tested for equivalence with any previously encountered
       *  nvsg::LineAttribute.
       *  \sa nvsg::LineAttribute, EquivalenceResult */
      NVSG_API virtual void handleLineAttribute( const nvsg::LineAttribute * p );

      /*! \brief Analyze a nvsg::Lines.
       *  \param p A pointer to the read-locked nvsg::Lines to analyze.
       *  \remarks A nvsg::Lines is tested for emptiness (no indices) and for equivalence with any
       *  previously encountered nvsg::Lines.
       *  \sa nvsg::Lines, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleLines( const nvsg::Lines * p );

      /*! \brief Analyze a nvsg::LineStrips.
       *  \param p A pointer to the read-locked nvsg::LineStrips to analyze.
       *  \remarks A nvsg::LineStrips is tested for emptiness (no strips) and for equivalence with
       *  any previously encountered nvsg::LineStrips.
       *  \sa nvsg::LineStrips, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleLineStrips( const nvsg::LineStrips * p );

      /*! \brief Analyze a nvsg::LOD.
       *  \param p A pointer to the read-locked nvsg::LOD to analyze.
       *  \remarks A nvsg::LOD is tested for emptiness (no children), for equivalence with any
       *  previously encountered nvsg::LOD, and for holding a single child only.
       *  \sa nvsg::LOD, EmptyResult, EquivalenceResult, RedundantLODRangesResult, SingleChildResult */
      NVSG_API virtual void handleLOD( const nvsg::LOD * p );

      /*! \brief Analyze a nvsg::Material.
       *  \param p A pointer to the read-locked nvsg::Material to analyze.
       *  \remarks A nvsg::Material is tested for equivalence with any previously encountered
       *  nvsg::Material.
       *  \sa nvsg::Material, EquivalenceResult */
      NVSG_API virtual void handleMaterial( const nvsg::Material * p );

      /*! \brief Analyze a nvsg::PointAttribute.
       *  \param p A pointer to the read-locked nvsg::PointAttribute to analyze.
       *  \remarks A nvsg::PointAttribute is tested for equivalence with any previously
       *  encountered nvsg::PointAttribute.
       *  \sa nvsg::PointAttribute, EquivalenceResult */
      NVSG_API virtual void handlePointAttribute( const nvsg::PointAttribute * p );

      /*! \brief Analyze a nvsg::PointLight.
       *  \param p A pointer to the read-locked nvsg::PointLight to analyze.
       *  \remarks A nvsg::PointLight is tested for equivalence with any previously encountered
       *  nvsg::PointLight.
       *  \sa nvsg::PointLight, EquivalenceResult */
      NVSG_API virtual void handlePointLight( const nvsg::PointLight * p );

      /*! \brief Analyze a nvsg::Points.
       *  \param p A pointer to the read-locked nvsg::Points to analyze.
       *  \remarks A nvsg::Points is tested for emptiness (no indices) and for equivalence with
       *  any previously encountered nvsg::Points.
       *  \sa nvsg::Points, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handlePoints( const nvsg::Points * p );

      /*! \brief Analyze a nvsg::QuadMeshes.
       *  \param p A pointer to the read-locked nvsg::QuadMeshes to analyze.
       *  \remarks A nvsg::QuadMeshes is tested for emptiness (no meshes) and for equivalence with
       *  any previously encountered nvsg::QuadMeshes.
       *  \sa nvsg::QuadMeshes, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleQuadMeshes( const nvsg::QuadMeshes * p );

      /*! \brief Analyze a nvsg::Quads.
       *  \param p A pointer to the read-locked nvsg::Quads to analyze.
       *  \remarks A nvsg::Quads is tested for emptiness (no indices) and for equivalence with any
       *  previously encountered nvsg::Quads.
       *  \sa nvsg::Quads, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleQuads( const nvsg::Quads * p );

      /*! \brief Analyze a nvsg::QuadStrips.
       *  \param p A pointer to the read-locked nvsg::QuadStrips to analyze.
       *  \remarks A nvsg::QuadStrips is tested for emptiness (no strips), for equivalence with
       *  any previously encountered nvsg::Quads, and for holding strips of length up to only four.
       *  \sa nvsg::QuadStrips, EmptyResult, EquivalenceResult, ShortStrippedResult */
      NVSG_API virtual void handleQuadStrips( const nvsg::QuadStrips * p );

      NVSG_API virtual void handleRTFx( const nvsg::RTFx * p );

      NVSG_API virtual void handleRTFxProgram( const nvsg::RTFxProgram * p );

      /*! \brief Analyze a nvsg::Shape.
       *  \param p A pointer to the read-locked nvsg::Shape to analyze.
       *  \remarks A nvsg::Shape is tested for emptiness (no primitive sets) and for equivalence
       *  with any previously encountered nvsg::Shape.
       *  \sa nvsg::Shape, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleShape( const nvsg::Shape * p );

      /*! \brief Analyze a nvsg::Skin.
       *  \param p A pointer to the read-locked nvsg::Skin to analyze.
       *  \remarks A nvsg::Skin is tested for emptiness (no joints) and equivalence with any
       *  previously encountered nvsg::Skin.
       *  \sa nvsg::Skin, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleSkin( const nvsg::Skin * p );

      /*! \brief Analyze a nvsg::SpotLight.
       *  \param p A pointer to the read-locked nvsg::SpotLight to analyze.
       *  \remarks A nvsg::SpotLight is tested for equivalence with any previously encountered
       *  nvsg::SpotLight.
       *  \sa nvsg::SpotLight, EquivalenceResult */
      NVSG_API virtual void handleSpotLight( const nvsg::SpotLight * p );

      /*! \brief Analyze a nvsg::StatePass.
       *  \param p A pointer to the read-locked nvsg::StatePass to analyze.
       *  \remarks A nvsg::StatePass is tested for emptiness (no attributes) and for equivalence
       *  with any previously encountered nvsg::StatePass.
       *  \sa nvsg::StatePass, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleStatePass( const nvsg::StatePass * p );

      /*! \brief Analyze a nvsg::StateSet.
       *  \param p A pointer to the read-locked nvsg::StateSet to analyze.
       *  \remarks A nvsg::StateSet is tested for emptiness (no variants) and for equivalence with
       *  any previously encountered nvsg::StateSet.
       *  \sa nvsg::StateSet, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleStateSet( const nvsg::StateSet * p );

      /*! \brief Analyze a nvsg::StateVariant.
       *  \param p A pointer to the read-locked nvsg::StateVariant to analyze.
       *  \remarks A nvsg::StateVariant is tested for emptiness (no passes) and for equivalence
       *  with any previously encountered nvsg::StateVariant.
       *  \sa nvsg::StateVariant, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleStateVariant( const nvsg::StateVariant * p );

      /*! \brief Analyze a nvsg::Switch.
       *  \param p A pointer to the read-locked nvsg::Switch to analyze.
       *  \remarks A nvsg::Switch is tested for emptiness (no children) and for equivalence with
       *  any previously encountered nvsg::Switch.
       *  \sa nvsg::Switch, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleSwitch( const nvsg::Switch * p );

      /*! \brief Analyze a nvsg::TextureAttribute.
       *  \param p A pointer to the read-locked nvsg::TextureAttribute to analyze.
       *  \remarks A nvsg::TextureAttribute is tested for emptiness (no items) and for equivalence
       *  with any previously encountered nvsg::TextureAttribute.
       *  \sa nvsg::TextureAttribute, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleTextureAttribute( const nvsg::TextureAttribute * p );

      /*! \brief Analyze a nvsg::TextureAttributeItem.
       *  \param p A pointer to the read-locked nvsg::TextureAttributeItem to analyze.
       *  \remarks A nvsg::TextureAttributeItem is tested for equivalence with any previously
       *  encountered nvsg::TextureAttributeItem.
       *  \sa nvsg::TextureAttributeItem, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleTextureAttributeItem( const nvsg::TextureAttributeItem * p );

      /*! \brief Analyze an nvsg::Animation of type nvmath::Trafo.
       *  \param anim A pointer to the read-locked Animation being traversed.
       *  \remarks An Animation of type nvmath::Trafo is tested for emptiness (no
       *  AnimationDescription) and for equivalence with any previously encounted Animation of
       *  type nvmath::Trafo.
       *  \sa nvsg::AnimationState, nvmath::Trafo, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleTrafoAnimation( const nvsg::Animation<nvmath::Trafo> * p );

      /*! \brief Analyze a nvsg::Transform.
       *  \param p A pointer to the read-locked nvsg::Transform to analyze.
       *  \remarks A nvsg::Transform is tested for emptiness (no children), for equivalence with
       *  any previously encountered nvsg::Transform, and for being the identity transform.
       *  \sa nvsg::Transform, EmptyResult, EquivalenceResult, IdentityResult */
      NVSG_API virtual void handleTransform( const nvsg::Transform * p );

      /*! \brief Analyze a nvsg::Triangles.
       *  \param p A pointer to the read-locked nvsg::Triangles to analyze.
       *  \remarks A nvsg::Triangles is tested for emptiness (no indices) and for equivalence with
       *  any previously encountered nvsg::Triangles.
       *  \sa nvsg::Triangles, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleTriangles( const nvsg::Triangles * p );

      /*! \brief Analyze a nvsg::TriFans.
       *  \param p A pointer to the read-locked nvsg::TriFans to analyze.
       *  \remarks A nvsg::TriFans is tested for emptiness (no strips), for equivalence with any
       *  previously encountered nvsg::TriFans, and for holding fans of length up to only three.
       *  \sa nvsg::TriFans, EmptyResult, EquivalenceResult, ShortStrippedResult */
      NVSG_API virtual void handleTriFans( const nvsg::TriFans * p );

      /*! \brief Analyze a nvsg::TriStrips.
       *  \param p A pointer to the read-locked nvsg::TriStrips to analyze.
       *  \remarks A nvsg::TriStrips is tested for emptiness (no strips), for equivalence with any
       *  previously encountered nvsg::TriStrips, and for holding strips of length up to only three.
       *  \sa nvsg::TriStrips, EmptyResult, EquivalenceResult, ShortStrippedResult */
      NVSG_API virtual void handleTriStrips( const nvsg::TriStrips * p );

      /*! \brief Analyze an nvsg::Animation of type VertexAttribute.
       *  \param anim A pointer to the read-locked Animation being traversed.
       *  \remarks An Animation of type VertexAttribute is tested for emptiness (no
       *  AnimationDescription) and for equivalence with any previously encounted Animation of
       *  type VertexAttributeSet.
       *  \sa nvsg::AnimationState, VertexAttribute, EmptyResult, EquivalenceResult */
      NVSG_API virtual void handleVertexAttributeAnimation( const nvsg::Animation<nvsg::VertexAttribute> * p );

      /*! \brief Analyze a nvsg::VertexAttributeSet.
       *  \param p A pointer to the read-locked nvsg::VertexAttributeSet to analyze.
       *  \remarks A nvsg::VertexAttributeSet is tested for emptiness (no vertices), for
       *  equivalence with any previously encountered nvsg::VertexAttributeSet, and for holding
       *  denormalized normals.
       *  \sa nvsg::VertexAttributeSet, EmptyResult, EquivalenceResult, DenormalizedNormalsResult,
       *  NullNormalsResult */
      NVSG_API virtual void handleVertexAttributeSet( const nvsg::VertexAttributeSet * p );

      /*! \brief Analyze an nvsg::Primitive.
       *  \param p A poitner to the read-locked nvsg::Primitive to analyze.
       *  \remarks An nvsg::Primitive is tested for combinability, emptiness, equivalence containing a
       *  redundant nvsg::IndexSet, and holding a VertexAttributeSet with unused vertices.
       *  \sa CombinableResult, EmptyResult, EquivalenceResult, RedundantIndexSetResult, UnusedVerticesResult */
      NVSG_API virtual void traversePrimitive( const nvsg::Primitive * p );

    private:
      class EquivalenceInfo
      {
        public :
          std::vector<nvsg::ObjectWeakPtr> uniqueObjects;
          std::set<nvsg::ObjectWeakPtr>    equivalentObjects;
      };

      template<typename T> class CombinableMap : public std::map<const nvsg::StateSet*,std::vector<std::pair<const T *,unsigned int> > >
      {
      };

      typedef std::map<nvsg::VertexAttributeSetWeakPtr,std::set<unsigned int> > VertexUseMap;

    private:
      template<typename T> void analyzeCombinable( const T * p, CombinableMap<T> & mvpT );
      void analyzeCombinable( const nvsg::Group * p );
      void analyzeEmpty( const nvsg::Object * p, unsigned int numberOfElements );
      void analyzeEquivalent( const nvsg::Object *p );
      void analyzeNormalsNormalized( nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type normals, unsigned int non
                                   , unsigned int &nullNormals, unsigned int &denormalizedNormals );
      void analyzeMissing( const nvsg::Object * p, const void * ptr );
      void analyzeRedundantIndexSet( const nvsg::IndexSetSharedPtr & p, unsigned int offset, unsigned int count );
      void analyzeRedundantLODRanges( const nvsg::LOD * p );
      void analyzeShortStripped( const nvsg::StrippedPrimitiveSet * p, unsigned int minCount );
      void analyzeSingleChild( const nvsg::Group * p );
      void analyzeUnusedVertices( const nvsg::IndexSetSharedPtr & isSP
                                , const nvsg::VertexAttributeSetSharedPtr & vasSP
                                , unsigned int offset, unsigned int count );
      template<typename T>
        void extractCombinableInfo( unsigned int oc
                                  , std::map<const nvsg::StateSet*,std::vector<std::pair<const T*,unsigned int> > > &mvpT );
      bool isChildOfCurrentLOD( const nvsg::Object * p );
      bool isToBeHandled( const nvsg::Object *p );
      void testVertexAttributeSet( const nvsg::VertexAttributeSet * p );

    private:
      std::map<unsigned int,unsigned int>                           m_countMap;
      std::stack<std::vector<const nvsg::GeoNode*> >                m_combinableGeoNodes;
      std::map<unsigned int,std::pair<unsigned int,unsigned int> >  m_combinableInfo;
      CombinableMap<nvsg::Lines>                                    m_combinableLines;
      CombinableMap<nvsg::LineStrips>                               m_combinableLineStrips;
      CombinableMap<nvsg::Points>                                   m_combinablePoints;
      CombinableMap<nvsg::Primitive>                                m_combinablePrimitive;
      CombinableMap<nvsg::QuadMeshes>                               m_combinableQuadMeshes;
      CombinableMap<nvsg::QuadPatches>                              m_combinableQuadPatches;
      CombinableMap<nvsg::QuadPatches4x4>                           m_combinableQuadPatches4x4;
      CombinableMap<nvsg::Quads>                                    m_combinableQuads;
      CombinableMap<nvsg::QuadStrips>                               m_combinableQuadStrips;
      CombinableMap<nvsg::RectPatches>                              m_combinableRectPatches;
      CombinableMap<nvsg::Triangles>                                m_combinableTriangles;
      CombinableMap<nvsg::TriFans>                                  m_combinableTriFans;
      CombinableMap<nvsg::TriPatches>                               m_combinableTriPatches;
      CombinableMap<nvsg::TriPatches4>                              m_combinableTriPatches4;
      CombinableMap<nvsg::TriStrips>                                m_combinableTriStrips;
      const nvsg::LOD *                                             m_currentLOD;
      const nvsg::StateSet *                                        m_currentStateSet;
      std::pair<unsigned int,unsigned int>                          m_denormalizedNormalsLIVAAD;
      std::pair<unsigned int,unsigned int>                          m_denormalizedNormalsVAS;
      std::map<unsigned int, unsigned int>                          m_emptyMap;
      std::map<unsigned int, EquivalenceInfo>                       m_equivalenceMap;
      unsigned int                                                  m_identityCount;
      unsigned int                                                  m_lodRanges;
      std::map<unsigned int, unsigned int>                          m_missingMap;
      std::pair<unsigned int,unsigned int>                          m_nullNormalsLIVAAD;
      std::pair<unsigned int,unsigned int>                          m_nullNormalsVAS;
      unsigned int                                                  m_redundantIndexSets;
      unsigned int                                                  m_redundantLODs;
      unsigned int                                                  m_redundantLODRanges;
      std::set<const nvsg::Object *>                                m_sharedObjects;
      std::map<unsigned int, unsigned int>                          m_singleChildMap;
      std::map<unsigned int, std::pair<unsigned int,unsigned int> > m_shortStripped;
      VertexUseMap                                                  m_vertexUseMap;
  };
}

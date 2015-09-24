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

#include <list>
#include <vector>
#include <utility>

#include "nvsgcommon.h"
#include "nvsg/Primitive.h"
#include "nvtraverser/OptimizeTraverser.h"

namespace nvtraverser
{
  /*! \brief OptimizeTraverser that can unify identical objects in a scene.
   *  \remarks The UnifyTraverser can unify \link nvsg::GeoNode GeoNodes, \endlink \link nvsg::Group
   *  Groups, \endlink \link nvsg::LOD LODs, \endlink \link nvsg::PrimitiveSet PrimitiveSets,
   *  \endlink \link nvsg::StateAttribute StateAttributes, \endlink \link nvsg::StatePass
   *  StatePasses, \endlink \link nvsg::StateSet StateSets, \endlink \link nvsg::StateVariant
   *  StateVariants, \endlink \link nvsg::TextureAttributeItem TextureAttributeItems, \endlink \link
   *  nvsg::VertexAttributeSet VertexAttributeSet, \endlink and vertices. Unifying in this
   *  context means, if there are two separate but identical objects in the scene, each reference to
   *  one of them is replaced by a reference of the other. That way, after unifying, there will be
   *  no separate but identical objects in the scene. For vertices, unification is performed within
   *  \link nvsg::PrimitiveSet PrimitiveSets. \endlink\n
   *  The types of objects to unify can be selected by \link UnifyTraverser::setUnifyTargets
   *  setUnifyTargets. \endlink By default, each object type listed above is unified.\n
   *  The accepted epsilon used in comparing the components of vertices can be set by
   *  \link UnifyTraverser::setEpsilon setEpsilon. \endlink By default, epsilon is FLT_EPSILON.\n
   *  As with every OptimizeTraverser, identical objects with different names can be considered to
   *  be equal. This can be set with \link OptimizeTraverser::setIgnoreNames setIgnoreNames. \endlink
   *  By default, this is set to \c true.\n
   *  After unifying a scene, the UnifyTraverser can be queried if the latest traversal did modify
   *  the scene, using \link OptimizeTraverser::getTreeModified getTreeModified. \endlink
   *  \sa OptimizeTraverser */
  class UnifyTraverser : public OptimizeTraverser
  {
    public:
      enum
      {
        UT_GEONODE                = BIT0,   //!< UnifyTarget nvsg::GeoNode: unify identical GeoNode objects into one.
        UT_GROUP                  = BIT1,   //!< UnifyTarget nvsg::Group: unify identical Group objects into one.
        UT_LOD                    = BIT2,   //!< UnifyTarget nvsg::LOD: unify identical LOD objects into one.
        UT_PRIMITIVE_SET          = BIT3,   //!< UnifyTarget nvsg::PrimitiveSet: unify identical PrimitiveSet objects into one.
        UT_PRIMITIVE              = UT_PRIMITIVE_SET,
        UT_STATE_ATTRIBUTE        = BIT4,   //!< UnifyTarget nvsg::StateAttribute: unify identical StateAttribute objects into one.
        UT_STATE_PASS             = BIT5,   //!< UnifyTarget nvsg::StatePass: unify identical StatePass objects into one.
        UT_STATE_SET              = BIT6,   //!< UnifyTarget nvsg::StateSet: unify identical StateSet objects into one.
        UT_STATE_VARIANT          = BIT7,   //!< UnifyTarget nvsg::StateVariant: unify identical StateVariant objects into one.
        UT_TEXTURE                = BIT8,   //!< UnifyTarget nvsg::Texture: unify identical Texture objects into one.
        UT_TEXTURE_ATTRIBUTE_ITEM = BIT9,   //!< UnifyTarget nvsg::TextureAttributeItem: unify identical TextureAttributeItem objects into one.
        UT_TRAFO_ANIMATION        = BIT10,  //!< UnifyTarget nvsg::Animation<nvmath::Trafo>: unify identical Animations on Trafo into one.
        UT_VERTEX_ATTRIBUTE_SET   = BIT12,  //!< UnifyTarget nvsg::VertexAttributeSet: unify identical VertexAttributeSet objects into one.
        UT_VERTICES               = BIT13,  //!< UnifyTarget Vertices: unify identical Vertices (with an epsilon) into one.
        UT_ALL_TARGETS_MASK       = ( UT_GEONODE | UT_GROUP | UT_LOD | UT_PRIMITIVE_SET
                                    | UT_STATE_ATTRIBUTE | UT_STATE_PASS | UT_STATE_SET
                                    | UT_STATE_VARIANT | UT_TEXTURE | UT_TEXTURE_ATTRIBUTE_ITEM
                                    | UT_TRAFO_ANIMATION | UT_VERTEX_ATTRIBUTE_SET | UT_VERTICES )
      } UnifyTarget;                        //!< Enum to specify the object types to unify.

    public:
      /*! \brief Default constructor of an UnifyTraverser.
       *  \remarks Creates a UnifyTraverser with an epsilon for vertex components comparison of
       *  FLT_EPSILON, and the targets to unify set to all.
       *  \sa setEpsilon, setUnifyTargets */
      NVSG_API UnifyTraverser( void );

      /*! \brief Get the bitmask describing the targets to unify.
       *  \return A bitmask describing the targets to unify. */
      NVSG_API unsigned int getUnifyTargets() const;

      /*! \brief Set the bitmask describing the targets to unify.
       *  \param mask The bitmask describing the targets to unify. */
      NVSG_API void setUnifyTargets( unsigned int mask );

      /*! \brief Get the epsilon used for compares in vertex unification.
       *  \return The epsilon used on component of a vertex to determine equality. */
      NVSG_API float getEpsilon() const;

      /*! \brief Set the epsilon used for compares in vertex unification.
       *  \param eps The epsilon used on component of a vertex to determine equality.
       *  \note The unification of vertices undefined, if eps is not positive. */
      NVSG_API void setEpsilon( float eps );

      REFLECTION_INFO_API( NVSG_API, UnifyTraverser );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( UnifyTargets );
          NVSG_API DECLARE_STATIC_PROPERTY( Epsilon );
      END_DECLARE_STATIC_PROPERTIES
    protected:
      /*! \brief Protected destructor to prevent instantiation of a UnifyTraverser on stack. */
      NVSG_API virtual ~UnifyTraverser( void );

      /*! \brief Overload of the \link OptimizeTraverser::doApply doApply \endlink method.
       *  \remarks After scene traversal, temporarily allocated storage is freed again. */
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Overload of the \link ExclusiveTraverser::handleBillboard Billboard \endlink method.
       *  \param billboard A pointer to the write-locked \link nvsg::Billboard Billboard \endlink to handle.
       *  \remarks After traversal of the Billboard, identical children are unified. */
      NVSG_API virtual void handleBillboard( nvsg::Billboard * billboard );

      /*! \brief Overload of the \link ExclusiveTraverser::handleGroup Group \endlink method.
       *  \param group A pointer to the write-locked \link nvsg::Group Group \endlink to handle.
       *  \remarks After traversal of the Group, identical children are unified. */
      NVSG_API virtual void handleGroup( nvsg::Group * group );

      /*! \brief Overload of the \link ExclusiveTraverser::handleLOD LOD \endlink method.
       *  \param lod A pointer to the write-locked \link nvsg::LOD LOD \endlink to handle.
       *  \remarks After traversal of the LOD as a Group, identical children are unified. */
      NVSG_API virtual void handleLOD( nvsg::LOD * lod );

      /*! \brief Overload of the \link ExclusiveTraverser::handleSwitch Switch \endlink method.
       *  \param swtch A pointer to the write-locked \link nvsg::Switch Switch \endlink to handle.
       *  \remarks After traversal of the Switch as a Group, identical children are unified. */
      NVSG_API virtual void handleSwitch( nvsg::Switch * swtch );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTransform Transform \endlink method.
       *  \param transform A pointer to the write-locked \link nvsg::Transform Transform \endlink to handle.
       *  \remarks After traversal of the Transform, identical children are unified. */
      NVSG_API virtual void handleTransform( nvsg::Transform * transform );

      /*! \brief Overload of the \link ExclusiveTraverser::handleFlipbookAnimation handleFlipbookAnimation \endlink method.
       *  \param anim A pointer to the write-locked \link nvsg::FlipbookAnimation FlipbookAnimation
       *  \endlink to handle.
       *  \remarks For unifying purposes, \a anim is handled like a \link nvsg::Group Group. \endlink */
      NVSG_API virtual void handleFlipbookAnimation( nvsg::FlipbookAnimation * anim );

      /*! \brief Overload of the \link ExclusiveTraverser::handleAnimatedTransform handleAnimatedTransform \endlink method.
       *  \param trafo A pointer to the write-locked \link nvsg::AnimatedTransform AnimatedTransform
       *  \endlink to handle.
       *  \remarks If \link nvsg::GeoNode GeoNodes, \endlink \link nvsg::Group Groups, \endlink or
       *  \link nvsg::LOD LODs \endlink are to be unified, \a trafo is handled like a \link
       *  nvsg::Transform Transform. \endlink Otherwise \a trafo is just traversed. */
      NVSG_API virtual void handleAnimatedTransform( nvsg::AnimatedTransform * trafo );

      /*! \brief Overload of the \link ExclusiveTraverser::handleGeoNode handleGeoNode \endlink method.
       *  \param gnode A pointer to the write-locked \link nvsg::GeoNode GeoNode \endlink to handle.
       *  \remarks After traversing \a gnode, it's \link nvsg::StateSet StateSets \endlink are
       *  unified, if requested. */
      NVSG_API virtual void handleGeoNode( nvsg::GeoNode * gnode );

      /*! \brief Overload of the \link ExclusiveTraverser::handleLines handleLines \endlink method.
       *  \param lines A pointer to the write-locked \link nvsg::Lines Lines \endlink to handle.
       *  \remarks After traversing \a lines, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleLines( nvsg::Lines * lines );

      /*! \brief Overload of the \link ExclusiveTraverser::handleLineStrips handleLineStrips \endlink method.
       *  \param strips A pointer to the write-locked \link nvsg::LineStrips LineStrips \endlink to
       *  handle.
       *  \remarks After traversing \a strips, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleLineStrips( nvsg::LineStrips * strips );

      /*! \brief Overload of the \link ExclusiveTraverser::handlePoints handlePoints \endlink method.
       *  \param points A pointer to the write-locked \link nvsg::Points Points \endlink to handle.
       *  \remarks After traversing \a points, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handlePoints( nvsg::Points *points );

      /*! \brief Overload of the \link ExclusiveTraverser::handlePrimitive handlePrimitive \endlink method.
       *  \param primitive A pointer to the write-locked \link nvsg::Primitive Primitive \endlink to handle.
       *  \remarks After traversing \a primitive, it is unified if unification of \link
       *  nvsg::Primitive Primitive \endlink is requested. */
      NVSG_API virtual void handlePrimitive( nvsg::Primitive * primitive );

      /*! \brief Overload of the \link ExclusiveTraverser::handleQuadMeshes handleQuadMeshes \endlink method.
       *  \param meshes A pointer to the write-locked \link nvsg::QuadMeshes QuadMeshes \endlink to
       *  handle.
       *  \remarks After traversing \a meshes, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleQuadMeshes( nvsg::QuadMeshes * meshes );

      /*! \brief Overload of the \link ExclusiveTraverser::handleQuadPatches handleQuadPatches \endlink method.
       *  \param quadPatches A pointer to the write-locked \link nvsg::QuadPatches QuadPatches \endlink to handle.
       *  \remarks After traversing \a quadPatches, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleQuadPatches( nvsg::QuadPatches * quadPatches );

      /*! \brief Overload of the \link ExclusiveTraverser::handleQuadPatches4x4 handleQuadPatches4x4 \endlink method.
       *  \param quadPatches4x4 pointer to the write-locked \link nvsg::QuadPatches4x4 QuadPatches4x4 \endlink to handle.
       *  \remarks After traversing \a quadPatches4x4, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleQuadPatches4x4( nvsg::QuadPatches4x4 * quadPatches4x4 );

      /*! \brief Overload of the \link ExclusiveTraverser::handleQuads handleQuads \endlink method.
       *  \param quads A pointer to the write-locked \link nvsg::Quads Quads \endlink to handle.
       *  \remarks After traversing \a quads, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleQuads( nvsg::Quads * quads );

      /*! \brief Overload of the \link ExclusiveTraverser::handleQuadStrips handleQuadStrips \endlink method.
       *  \param strips A pointer to the write-locked \link nvsg::QuadStrips QuadStrips \endlink to
       *  handle.
       *  \remarks After traversing \a strips, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleQuadStrips( nvsg::QuadStrips * strips );

      /*! \brief Overload of the \link ExclusiveTraverser::handleRectPatches handleRectPatches \endlink method.
       *  \param rectPatches A pointer to the write-locked \link nvsg::RectPatches RectPatches \endlink to handle.
       *  \remarks After traversing \a rectPatches, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleRectPatches( nvsg::RectPatches * rectPatches );

      NVSG_API virtual void handleRTFx( nvsg::RTFx * rtfx );

      /*! \brief Overload of the \link ExclusiveTraverser::handleStatePass handleStatePass \endlink method.
       *  \param pass A pointer to the write-locked \link nvsg::StatePass StatePass \endlink to
       *  handle.
       *  \remarks After traversing \a pass, it's StateAttributes are unified if unification of
       *  \link nvsg::StateAttribute StateAttributes \endlink is requested. */
      NVSG_API virtual void handleStatePass( nvsg::StatePass * pass );

      /*! \brief Overload of the \link ExclusiveTraverser::handleStateSet handleStateSet \endlink method.
       *  \param stateSet A pointer to the write-locked \link nvsg::StateSet StateSet \endlink to
       *  handle.
       *  \remarks If the unification of \link nvsg::StateVariant StateVariants, \endlink \link
       *  nvsg::StatePass StatePasses, \endlink \link nvsg::StateAttribute StateAttributes, \endlink
       *  or \link nvsg::TextureAttributeItem TextureAttributeItems \endlink is requested, a
       *  non-standard traversal of each \link nvsg::StateVariant StateVariant \endlink is
       *  performed, and then the \link nvsg::StateVariant StateVariants \endlink of this \link
       *  nvsg::StateSet StateSet \endlink are unified their unification is requested. Otherwise,
       *  \a stateSet is just traversed. */
      NVSG_API virtual void handleStateSet( nvsg::StateSet * stateSet );

      /*! \brief Overload of the \link ExclusiveTraverser::handleStateVariant handleStateVariant \endlink method.
       *  \param variant A pointer to the write-locked \link nvsg::StateVariant StateVariant
       *  \endlink to handle.
       *  \remarks After traversing \a variant, it's \link nvsg::StatePass StatePasses \endlink are
       *  unified if unification of \link nvsg::StatePass StatePasses \endlink is requested. */
      NVSG_API virtual void handleStateVariant( nvsg::StateVariant * variant );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTextureAttribute handleTextureAttribute \endlink method.
       *  \param texAttrib A pointer to the write-locked \link nvsg::TextureAttribute
       *  TextureAttribute \endlink to handle.
       *  \remarks After traversing \a texAttrib, it's \link nvsg::TextureAttributeItem
       *  TextureAttributeItems \endlink are unified if unification of \link
       *  nvsg::TextureAttributeItem TextureAttributeItems \endlink is requested. */
      NVSG_API virtual void handleTextureAttribute( nvsg::TextureAttribute * texAttrib );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTextureAttributeItem handleTextureAttributeItem \endlink method.
       *  \param texAttribItem A pointer to the write-locked \link nvsg::TextureAttributeItem TextureAttributeItem \endlink to handle.
       *  \remarks This function is called from the framework whenever a TextureAttributeItem is
       *  encountered on traversal. The TextureAttributeItem \a texAttribItem is already read-locked.\n
       *  The base implementation just does nothing.
       *  \sa getCurrentTextureUnit */
      NVSG_API virtual void handleTextureAttributeItem( nvsg::TextureAttributeItem * texAttribItem );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTriangles handleTriangles \endlink method.
       *  \param triangles A pointer to the write-locked \link nvsg::Triangles Triangles \endlink to
       *  handle.
       *  \remarks After traversing \a triangles, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleTriangles( nvsg::Triangles * triangles );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTriFans handleTriFans \endlink method.
       *  \param fans A pointer to the write-locked \link nvsg::TriFans TriFans \endlink to handle.
       *  \remarks After traversing \a fans, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleTriFans( nvsg::TriFans * fans );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTriPatches handleTriPatches \endlink method.
       *  \param triPatches A pointer to the write-locked \link nvsg::TriPatches TriPatches \endlink to handle.
       *  \remarks After traversing \a triPatches, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleTriPatches( nvsg::TriPatches * triPatches );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTriPatches4 handleTriPatches4 \endlink method.
       *  \param triPatches4 A pointer to the write-locked \link nvsg::TriPatches4 TriPatches4 \endlink to handle.
       *  \remarks After traversing \a triPatches4, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleTriPatches4( nvsg::TriPatches4 * triPatches4 );

      /*! \brief Overload of the \link ExclusiveTraverser::handleTriStrips handleTriStrips \endlink method.
       *  \param strips A pointer to the write-locked \link nvsg::TriStrips TriStrips \endlink to
       *  handle.
       *  \remarks After traversing \a strips, it is unified if unification of \link
       *  nvsg::PrimitiveSet PrimitiveSets \endlink is requested. */
      NVSG_API virtual void handleTriStrips( nvsg::TriStrips * strips );

      /*! \brief Overload of the \link ExclusiveTraverser::handleVertexAttributeSet handleVertexAttributeSet \endlink method.
       *  \param vas A pointer to the write-locked \link nvsg::VertexAttributeSet VertexAttributeSet
       *  \endlink to handle.
       *  \remarks After traversing \a vas, the vertices inside are unified if unification of
       *  vertices is requested. */
      NVSG_API virtual void handleVertexAttributeSet( nvsg::VertexAttributeSet * vas );

      /*! \brief Overload of the \link ExclusiveTraverser::handleAnimatedVertexAttributeSet handleAnimatedVertexAttributeSet \endlink method.
       *  \param avas A pointer to the write-locked \link nvsg::AnimatedVertexAttributeSet
       *  AnimatedVertexAttributeSet \endlink to handle.
       *  \remarks The vertices of an \link nvsg::AnimatedVertexAttributeSet
       *  AnimatedVertexAttributeSet \endlink are not unified by this UnifyTraverser. That is, only
       *  if no vertices are to be unified, \a avas is traversed. */
      NVSG_API virtual void handleAnimatedVertexAttributeSet( nvsg::AnimatedVertexAttributeSet * avas );

      /*! \brief Overload of the \link ExclusiveTraverser::traversePrimitiveSet traversePrimitiveSet \endlink method.
       *  \param pset A pointer to the write-locked \link nvsg::PrimitiveSet PrimitiveSets \endlink
       *  to handle.
       *  \remarks After traversing \a pset, it's \link nvsg::VertexAttributeSet VertexAttributeSets
       *  \endlink are unified if unification of \link nvsg::VertexAttributeSet VertexAttributeSets
       *  \endlink is requested. */
      NVSG_API virtual void traversePrimitiveSet( nvsg::PrimitiveSet * pset );

    private:
      void unifyChildren( nvsg::Group *p );
      void unifyGeoNodes( nvsg::Group *p );
      void unifyGroups( nvsg::Group *p );
      void unifyLODs( nvsg::Group *p );
      template<typename T, typename TH> void unifyPrimitiveSet( std::multimap<nvsg::HashKey,nvutil::SmartPtr<TH> > &v, T *pT );
      void unifyPrimitive( std::map<nvsg::PrimitiveType, std::multimap<nvsg::HashKey,nvsg::PrimitiveSharedPtr> > & v, nvsg::Primitive * p );

      typedef std::set<nvsg::DrawableSharedPtr>             DrawableSet;
      typedef std::map<nvsg::StateSetSharedPtr,DrawableSet> GeometryMap;

    private:
      GeometryMap                                                       m_currentGeometries;
      bool                                                              m_currentGeometriesChanged;
      nvsg::StateSetWeakPtr                                             m_currentStateSet;
      float                                                             m_epsilon;
      std::vector<nvsg::TrafoAnimationSharedPtr>                        m_trafoAnimations;
      std::multimap<nvsg::HashKey,nvsg::GeoNodeSharedPtr>               m_geoNodes;
      std::multimap<nvsg::HashKey,nvsg::GroupSharedPtr>                 m_groups;
      std::multimap<nvsg::HashKey,nvsg::LinesSharedPtr>                 m_lines;
      std::multimap<nvsg::HashKey,nvsg::LineStripsSharedPtr>            m_lineStrips;
      std::vector<nvsg::LODSharedPtr>                                   m_LODs;
      std::multimap<nvsg::HashKey,nvsg::PointsSharedPtr>                m_points;
      std::map<nvsg::PrimitiveType,std::multimap<nvsg::HashKey,nvsg::PrimitiveSharedPtr> >  m_primitives;
      std::multimap<nvsg::HashKey,nvsg::QuadMeshesSharedPtr>            m_quadMeshes;
      std::multimap<nvsg::HashKey,nvsg::QuadPatchesSharedPtr>           m_quadPatches;
      std::multimap<nvsg::HashKey,nvsg::QuadPatches4x4SharedPtr>        m_quadPatches4x4;
      std::multimap<nvsg::HashKey,nvsg::QuadsSharedPtr>                 m_quads;
      std::multimap<nvsg::HashKey,nvsg::QuadStripsSharedPtr>            m_quadStrips;
      std::multimap<nvsg::HashKey,nvsg::RectPatchesSharedPtr>           m_rectPatches;
      std::set<nvsg::RTFxProgramSharedPtr>                              m_rtfxPrograms;
      std::map<unsigned int, std::multimap<nvsg::HashKey,nvsg::StateAttributeSharedPtr> >  m_stateAttributes;
      std::multimap<nvsg::HashKey,nvsg::StatePassSharedPtr>             m_statePasses;
      std::multimap<nvsg::HashKey,nvsg::StateSetSharedPtr>              m_stateSets;
      std::multimap<nvsg::HashKey,nvsg::StateVariantSharedPtr>          m_stateVariants;
      std::multimap<nvsg::HashKey,nvsg::TextureAttributeItemSharedPtr>  m_textureAttributeItems;
      std::multimap<nvsg::HashKey,nvsg::TextureSharedPtr>               m_textures;
      std::multimap<nvsg::HashKey,nvsg::TrianglesSharedPtr>             m_triangles;
      std::multimap<nvsg::HashKey,nvsg::TriFansSharedPtr>               m_triFans;
      std::multimap<nvsg::HashKey,nvsg::TriPatchesSharedPtr>            m_triPatches;
      std::multimap<nvsg::HashKey,nvsg::TriPatches4SharedPtr>           m_triPatches4;
      std::multimap<nvsg::HashKey,nvsg::TriStripsSharedPtr>             m_triStrips;
      unsigned int                                                      m_unifyTargets;
      std::multimap<nvsg::HashKey,nvsg::VertexAttributeSetSharedPtr>    m_vertexAttributeSets;

      std::set<nvsg::VertexAttributeSetSharedPtr>                       m_multiOwnedHandledVAS;
      std::vector<nvsg::VertexAttributeSetSharedPtr>                    m_removedVAS;
  };

  inline unsigned int UnifyTraverser::getUnifyTargets() const
  {
    return( m_unifyTargets );
  }

  inline void UnifyTraverser::setUnifyTargets( unsigned int mask )
  {
    if ( mask != m_unifyTargets )
    {
      m_unifyTargets = mask;
      notify( PID_UnifyTargets );
    }
  }

  inline float UnifyTraverser::getEpsilon() const
  {
    return( m_epsilon );
  }

  inline void UnifyTraverser::setEpsilon( float eps )
  {
    NVSG_ASSERT( 0 < eps );
    if ( m_epsilon != eps )
    {
      m_epsilon = eps;
      notify( PID_Epsilon );
    }
  }

}



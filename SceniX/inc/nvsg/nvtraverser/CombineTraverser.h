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
#pragma warning( disable : 4503 )   // decorated name length exceeded, name was truncated

#include <list>
#include "nvsgcommon.h"
#include "nvsg/Primitive.h"
#include "nvtraverser/OptimizeTraverser.h"

namespace nvtraverser
{
  /*! \brief Traverser that combines a number of Objects, if appropriate.
   *  \remarks There are a number of different objects types combinable:\n
   *  - All geometries of all GeoNodes under one Group are combined into a single GeoNode.
   *  - All LODs with the same ranges under one Group into a single LOD.
   *  - Consecutive ranges in an LOD that reference the same Node into one range.
   *  - All PrimitveSets of the same type bound to the same StateSet are combined into one. */
  class CombineTraverser : public OptimizeTraverser
  {
    public:
      enum
      {
        CT_GEONODE          = BIT0,   //!< CombineTarget GeoNode: combine compatible GeoNode objects into one.
        CT_LOD              = BIT1,   //!< CombineTarget LOD: combine compatible LOD objects into one.
        CT_LOD_RANGES       = BIT2,   //!< CombineTarget LOD Range: combine identical adjacent LOD levels into one.
        CT_PRIMITIVE_SET    = BIT3,   //!< CombineTarget PrimitiveSet: combine compatible PrimitiveSet objects into one.
        CT_PRIMITIVE        = CT_PRIMITIVE_SET,
        CT_TRANSFORM        = BIT4,   //!< CombineTarget Transform: combine compatible Transform objects into one.
        CT_ALL_TARGETS_MASK = ( CT_GEONODE | CT_LOD | CT_LOD_RANGES | CT_PRIMITIVE_SET | CT_TRANSFORM )
      } CombineTarget;

    public:
      NVSG_API static bool areCombinable( const nvsg::Drawable * p0, const nvsg::Drawable * p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::Group *p0, const nvsg::Group *p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::LOD * p0, const nvsg::LOD * p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::Object * p0, const nvsg::Object * p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::Primitive * p0, const nvsg::Primitive * p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::QuadPatches *p0, const nvsg::QuadPatches *p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::RectPatches *p0, const nvsg::RectPatches *p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::Skin * p0, const nvsg::Skin * p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::Transform * p0, const nvsg::Transform * p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::TriPatches *p0, const nvsg::TriPatches *p1, bool ignoreNames );
      NVSG_API static bool areCombinable( const nvsg::VertexAttributeSet * p0, const nvsg::VertexAttributeSet * p1, bool ignoreNames );

    public:
      //! Constructor
      NVSG_API CombineTraverser( void );

      /*! \brief Get the bitmask describing the targets to combine.
       *  \return A bitmask describing the targets to combine. */
      NVSG_API unsigned int getCombineTargets() const;

      /*! \brief Set the bitmask describing the targets to combine.
       *  \param mask The bitmask describing the targets to combine. */
      NVSG_API void setCombineTargets( unsigned int mask );

      //! Get the 'ignore acceleration builder' hint.
      /** If the 'ignore acceleration builder' hint is set, combinable objects with different hints are
        * still considered for combining.
        * \return true if the names will be ignored, otherwise false */
      NVSG_API bool getIgnoreAccelerationBuilderHints() const;

      //! Set the 'ignore acceleration builder' hint.
      /** If the 'ignore acceleration builder' hint is set, combinable objects with different hints are
        * still considered for combining. */
      NVSG_API void setIgnoreAccelerationBuilderHints( bool ignore );

      REFLECTION_INFO_API( NVSG_API, CombineTraverser );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( CombineTargets );
          NVSG_API DECLARE_STATIC_PROPERTY( IgnoreAccelerationBuilderHints );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      //! Protected destructor to prevent instantiation of a CombineTraverser on stack.
      NVSG_API virtual ~CombineTraverser( void );

      //! Combine all GeoNodes and LODs directly underneath this AnimatedTransform.
      NVSG_API virtual void handleAnimatedTransform( nvsg::AnimatedTransform *p );

      //! Combine all GeoNodes and LODs directly underneath this Billboard.
      NVSG_API virtual void handleBillboard( nvsg::Billboard * p );

      //! Combine all PrimitiveSets of the same type bound to the same StateSet.
      NVSG_API virtual void handleGeoNode( nvsg::GeoNode * p );

      //! Combine all GeoNodes and LODs directly underneath this Group.
      NVSG_API virtual void handleGroup( nvsg::Group * p );

      //! Gather Lines to combine PrimitiveSets.
      NVSG_API virtual void handleLines( nvsg::Lines * p );

      //! Gather LineStrips to combine PrimitiveSets.
      NVSG_API virtual void handleLineStrips( nvsg::LineStrips * p );

      //! Combine all consecutive LOD ranges referencing the same Node.
      NVSG_API virtual void handleLOD( nvsg::LOD * p );

      //! Gather Points to combine PrimitiveSets.
      NVSG_API virtual void handlePoints( nvsg::Points * p );

      //! Gather Primitives to combine.
      NVSG_API virtual void handlePrimitive( nvsg::Primitive * p );

      //! Gather QuadMeshes to combine PrimitiveSets.
      NVSG_API virtual void handleQuadMeshes( nvsg::QuadMeshes * p );

      //! Gather QuadPatches to combine Primitive.
      NVSG_API virtual void handleQuadPatches( nvsg::QuadPatches * p );

      //! Gather QuadPatches4x4 to combine Primitive.
      NVSG_API virtual void handleQuadPatches4x4( nvsg::QuadPatches4x4 * p );

      //! Gather Quads to combine PrimitiveSets.
      NVSG_API virtual void handleQuads( nvsg::Quads * p );

      //! Gather QuadStrips to combine PrimitiveSets.
      NVSG_API virtual void handleQuadStrips( nvsg::QuadStrips * p );

      //! Gather RectPatches to combine Primitive.
      NVSG_API virtual void handleRectPatches( nvsg::RectPatches * p );

      //! Combine all GeoNodes and LODs directly underneath this Transform.
      NVSG_API virtual void handleTransform( nvsg::Transform * p );

      //! Gather Triangles to combine PrimitiveSets.
      NVSG_API virtual void handleTriangles( nvsg::Triangles * p );

      //! Gather TriFans to combine PrimitiveSets.
      NVSG_API virtual void handleTriFans( nvsg::TriFans * p );

      //! Gather TriPatches to combine Primitive.
      NVSG_API virtual void handleTriPatches( nvsg::TriPatches * p );

      //! Gather TriPatches4 to combine Primitive.
      NVSG_API virtual void handleTriPatches4( nvsg::TriPatches4 * p );

      //! Gather TriStrips to combine PrimitiveSets.
      NVSG_API virtual void handleTriStrips( nvsg::TriStrips * p );
      
      //! Keep track of the current StateSet.
      NVSG_API virtual bool preTraverseDrawables( const nvsg::GeoNode* gnode, nvsg::GeoNode::StateSetConstIterator ssci );

    private:
      template<typename T> class PrimitiveContainer : public std::vector<typename nvutil::ObjectTraits<T>::SharedPtr> {};
      template<typename T> class PrimitiveContainerContainer : public std::vector<PrimitiveContainer<T> > {};
      template<typename T> class PrimitiveMap : public std::map<nvsg::StateSetSharedPtr,PrimitiveContainerContainer<T> > {};

    private:
      void combineGeoNodes( nvsg::Group *p );
      void combineLODs( nvsg::Group *p );
      void combineLODRanges( nvsg::LOD *p );
      void combineTransforms( nvsg::Group *p );

      // for PrimitiveSet combining:
      void combine( const nvsg::PrimitiveSharedPtr & p0, const nvsg::PrimitiveSharedPtr & p1 );
      void combine( nvsg::PrimitiveSet *p0, const nvsg::PrimitiveSet *p1 );
      void combine( const nvsg::IndependentPrimitiveSetSharedPtr & p0, const nvsg::IndependentPrimitiveSetSharedPtr & p1 );
      void combine( const nvsg::MeshedPrimitiveSetSharedPtr & p0, const nvsg::MeshedPrimitiveSetSharedPtr & p1 );
      void combine( const nvsg::StrippedPrimitiveSetSharedPtr & p0, const nvsg::StrippedPrimitiveSetSharedPtr & p1 );
      void combine( const nvsg::VertexAttributeSetSharedPtr & vash0, const nvsg::VertexAttributeSetSharedPtr & vash1 );
      void combine( const nvsg::AnimatedVertexAttributeSetSharedPtr & avash0, const nvsg::AnimatedVertexAttributeSetSharedPtr & avash1 );
      void combine( const nvsg::FramedVertexAttributeAnimationDescriptionSharedPtr & fvaah0, const nvsg::FramedVertexAttributeAnimationDescriptionSharedPtr & fvaah1 );
      void combine( const nvsg::LinearInterpolatedVertexAttributeAnimationDescriptionSharedPtr & livaah0, const nvsg::LinearInterpolatedVertexAttributeAnimationDescriptionSharedPtr & livaah1 );
      void combine( const nvsg::SkinSharedPtr & sh0, const nvsg::SkinSharedPtr & sh1, unsigned int offset );
      template<typename T> void checkCombinable( PrimitiveMap<T> & pm );
      void combineDrawables( nvsg::GeoNode * p );
      template<typename T> void combineDrawables( nvsg::GeoNode *p, PrimitiveMap<T> & pm );
      void combineDrawables( nvsg::GeoNode *p, PrimitiveMap<nvsg::Primitive> & pm );
      void reduceVertexAttributeSet( const nvsg::IndependentPrimitiveSetSharedPtr & p );
      void reduceVertexAttributeSet( const nvsg::MeshedPrimitiveSetSharedPtr & p );
      void reduceVertexAttributeSet( const nvsg::PrimitiveSharedPtr & p );
      void reduceVertexAttributeSet( const nvsg::StrippedPrimitiveSetSharedPtr & p );
      nvsg::VertexAttributeSetSharedPtr reduceVertexAttributeSet( const nvsg::VertexAttributeSetSharedPtr & p, std::map<unsigned int, unsigned int> &indexMap );
      nvsg::VertexAttributeSetSharedPtr reduceVertexAttributeSet( const nvsg::VertexAttributeSetSharedPtr & p, unsigned int offset, unsigned int count );
      void removeDrawables( nvsg::GeoNode * p );
      template<typename T> void removeDrawables( nvsg::GeoNode * p, PrimitiveMap<T> & pm );
      template<typename T> void storeDrawable( T *p, PrimitiveMap<T> &pm );
      void storeDrawable( nvsg::Primitive *p, PrimitiveMap<nvsg::Primitive> &pm );
      nvsg::IndependentPrimitiveSetSharedPtr simpleCombine( const nvsg::IndependentPrimitiveSetSharedPtr & p0
                                                          , const nvsg::IndependentPrimitiveSetSharedPtr & p1 );
      nvsg::MeshedPrimitiveSetSharedPtr simpleCombine( const nvsg::MeshedPrimitiveSetSharedPtr & p0
                                                     , const nvsg::MeshedPrimitiveSetSharedPtr & p1 );
      nvsg::PrimitiveSharedPtr simpleCombine( const nvsg::PrimitiveSharedPtr & p0
                                            , const nvsg::PrimitiveSharedPtr & p1 );
      nvsg::StrippedPrimitiveSetSharedPtr simpleCombine( const nvsg::StrippedPrimitiveSetSharedPtr & p0
                                                       , const nvsg::StrippedPrimitiveSetSharedPtr & p1 );

      using OptimizeTraverser::optimizationAllowed;
      bool optimizationAllowed( const nvsg::PrimitiveSet * obj );

    private:
      unsigned int m_combineTargets;
      bool         m_ignoreAccelerationBuilderHints;

      // for PrimitiveSet combining
      nvsg::StateSetSharedPtr                     m_currentStateSet;
      //  For each primitive type, hold a map from the current StateSet to a vector of vector of primitives.
      //  The Primitives in the second level vector are compatible to each of them.
      PrimitiveMap<nvsg::IndependentPrimitiveSet> m_lines;
      PrimitiveMap<nvsg::StrippedPrimitiveSet>    m_lineStrips;
      bool                                        m_optimizeGeoNode;
      PrimitiveMap<nvsg::IndependentPrimitiveSet> m_points;
      std::map<nvsg::PrimitiveType, PrimitiveMap<nvsg::Primitive> > m_primitives;
      PrimitiveMap<nvsg::MeshedPrimitiveSet>      m_quadMeshes;
      PrimitiveMap<nvsg::Primitive>               m_quadPatches;
      PrimitiveMap<nvsg::Primitive>               m_quadPatches4x4;
      PrimitiveMap<nvsg::IndependentPrimitiveSet> m_quads;
      PrimitiveMap<nvsg::StrippedPrimitiveSet>    m_quadStrips;
      PrimitiveMap<nvsg::Primitive>               m_rectPatches;
      PrimitiveMap<nvsg::IndependentPrimitiveSet> m_triangles;
      PrimitiveMap<nvsg::StrippedPrimitiveSet>    m_triFans;
      PrimitiveMap<nvsg::Primitive>               m_triPatches;
      PrimitiveMap<nvsg::Primitive>               m_triPatches4;
      PrimitiveMap<nvsg::StrippedPrimitiveSet>    m_triStrips;
  };

  inline unsigned int CombineTraverser::getCombineTargets() const
  {
    return( m_combineTargets );
  }

  inline void CombineTraverser::setCombineTargets( unsigned int mask )
  {
    if ( mask != m_combineTargets )
    {
      m_combineTargets = mask;
      notify( PID_CombineTargets );
    }
  }

  inline bool CombineTraverser::getIgnoreAccelerationBuilderHints() const
  {
    return( m_ignoreAccelerationBuilderHints );
  }

  inline void CombineTraverser::setIgnoreAccelerationBuilderHints( bool ignore )
  {
    if ( ignore != m_ignoreAccelerationBuilderHints )
    {
      m_ignoreAccelerationBuilderHints = ignore;
      notify( PID_IgnoreAccelerationBuilderHints );
    }
  }
}

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

#include "nvsgcommon.h"
#include "nvtraverser/Traverser.h"

namespace nvsg
{
  class IndependentPrimitiveSet;
}

namespace nvtraverser
{
  class DestrippingTraverser;

  //! Traverser that can smooth some drawables.
  /** Currently, this SmoothTraverser supports Triangles only. If these Triangles are non-trivially indexed, they are
    * changed to be trivially indexed.  */
  class SmoothTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API SmoothTraverser(void);

      //! Set the crease angle for smoothing.
      NVSG_API void  setCreaseAngle( float creaseAngle );

      //! Get the crease angle for smoothing.
      NVSG_API float getCreaseAngle( ) const;

      REFLECTION_INFO_API( NVSG_API, SmoothTraverser );
      BEGIN_REFLECT_STATIC_PROPERTIES( SmoothTraverser )
        ADD_PROPERTY_RW(CreaseAngle, float, SEMANTIC_VALUE, value, value );
      END_REFLECT_STATIC_PROPERTIES()

    protected:
      //! Protected destructor to prevent instantiation of a SmoothTraverser.
      NVSG_API virtual ~SmoothTraverser(void);

      //! doApply override
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

      //! Handle a GeoNode object.
      /** The GeoNode is the primary object to smooth.  */
      NVSG_API virtual void  handleGeoNode( nvsg::GeoNode *p              //!<  GeoNode to handle
                                          );

      //! Handle a QuadPatches object.
      NVSG_API virtual void  handleQuadPatches( nvsg::QuadPatches *p      //!<  QuadPatches to handle
                                        );

      //! Handle a QuadPatches4x4 object.
      NVSG_API virtual void  handleQuadPatches4x4( nvsg::QuadPatches4x4 *p  //!<  QuadPatches4x4 to handle
                                        );

      //! Handle a Quads object.
      NVSG_API virtual void  handleQuads( nvsg::Quads *p                  //!<  Quads to handle
                                        );

      //! Handle a RectPatches object.
      NVSG_API virtual void  handleRectPatches( nvsg::RectPatches *p      //!<  RectPatches to handle
                                        );

      //! Handle a Triangles object.
      NVSG_API virtual void  handleTriangles( nvsg::Triangles *p          //!<  Triangles to handle
                                            );

      //! Handle a TriPatches object.
      NVSG_API virtual void  handleTriPatches( nvsg::TriPatches *p      //!<  TriPatches to handle
                                        );

      //! Handle a TriPatches4 object.
      NVSG_API virtual void  handleTriPatches4( nvsg::TriPatches4 *p      //!<  TriPatches4 to handle
                                        );

      //! Handle a Primitive object.
      NVSG_API virtual void  handlePrimitive( nvsg::Primitive *p        //!<  Primitive to handle
                                        );

#if !defined(NDEBUG)
      //! Handle a QuadStrips object.
      /** Just assert here; shouldn't happen at all! */
      NVSG_API virtual void  handleQuadStrips( nvsg::QuadStrips *p        //!<  QuadStrips to handle
                                             );

      //! Handle a TriFans object.
      /** Just assert here; shouldn't happen at all! */
      NVSG_API virtual void  handleTriFans( nvsg::TriFans *p              //!<  TriFans to handle
                                          );

      //! Handle a TriStrips object.
      /** Just assert here; shouldn't happen at all! */
      NVSG_API virtual void  handleTriStrips( nvsg::TriStrips *p          //!<  TriStrips to handle
                                            );
#endif

      /*! \brief Test whether this Object should be optimized
       *  \param p A pointer to the constant nvsg::Object to test. */
      NVSG_API virtual bool optimizationAllowed( const nvsg::PrimitiveSet *p );
      NVSG_API virtual bool optimizationAllowed( const nvsg::Primitive *p );

    private:
      void flattenIndependentPrimitiveSet( nvsg::IndependentPrimitiveSet *p );
      void flattenPrimitive( nvsg::Primitive *p );

    private:
      float                                     m_creaseAngle;
      nvutil::SmartPtr<DestrippingTraverser>    m_destrippingTraverser;
      std::vector<nvsg::PrimitiveSetSharedPtr>  m_primitiveSets;
      std::vector<nvsg::PrimitiveSharedPtr>     m_primitives;
  };
}

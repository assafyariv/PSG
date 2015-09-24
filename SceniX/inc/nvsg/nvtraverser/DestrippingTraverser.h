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
#include "nvtraverser/Traverser.h"

namespace nvtraverser
{
  /*! \brief Traverser to destripify stripped primitives.
   *  \par Namespace: nvsg
   *  \remarks This Traverser destripifies nvsg::TriFans and nvsg::TriStrips to nvsg::Triangles,
   *  nvsg::QuadStrips to nvsg::Quads, and nvsg::LineStrips to nvsg::Lines.\n
   *  Using good nvsg::TriStrips usually is more efficient than using nvsg::Triangles, but often
   *  it is more efficient to use nvsg::Triangles instead of badly stripped nvsg::TriStrips. That
   *  is, it might be worth to try to destrip all the nvsg::TriStrips to nvsg::Triangles. The same
   *  holds for the other primitive types.
   *  \sa OptimizeTraverser */
  class DestrippingTraverser : public ExclusiveTraverser
  {
    public:
      /*! \brief Default Constructor
       *  \remarks A DestrippingTraverser potentially modifies the tree of the scene. */
      NVSG_API DestrippingTraverser(void);

    protected:
      /*! \brief Protected destructor to prevent explicit creation.
       *  \remarks The destructor is never used explicitly, but only when the reference count of
       *  the DestrippingTraverser is decremented to zero. */
      NVSG_API virtual ~DestrippingTraverser(void);

      /*! \brief Override of the doApply method.
       *  \param root The Node to destripify.
       *  \remarks The doApply method is the traversal entry point of a Traverser. Clears
       *  temporarily used data structures after traversing.
       *  \sa nvsg::Scene, Traverser */
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

      /*! \brief Destripify the strips of this nvsg::GeoNode.
       *  \param p A pointer to the constant nvsg::GeoNode whose strips are to be destripified.
       *  \remarks All nvsg::TriFans and nvsg::TriStrips of this nvsg::GeoNode are destripified to
       *  nvsg::Triangles, all nvsg::QuadStrips to nvsg::Quads, and all nvsg::LineStrips to
       *  nvsg::Lines.
       *  \sa handleLineStrips, handleQuadStrips, handleTriFans, handleTriStrips */
      NVSG_API virtual void handleGeoNode( nvsg::GeoNode * p );

      /*! \brief Gather the nvsg::LineStrips to destrip. 
       *  \param p A pointer to the constant nvsg::LineStrip to destrip.
       *  \sa handleQuadStrips, handleTriFans, handleTriStrips */
      NVSG_API virtual void handleLineStrips( nvsg::LineStrips *p );

      /*! \brief Change stripped Primitives to non-stripped Primitives
       *  \param p A pointer to the nvsg::Primitive to handle. */
      NVSG_API virtual void handlePrimitive( nvsg::Primitive * p );

      /*! \brief Gather the nvsg::QuadStrips to destrip. 
       *  \param p A pointer to the constant nvsg::QuadStrips to destrip.
       *  \sa handleLineStrips, handleTriFans, handleTriStrips */
      NVSG_API virtual void handleQuadStrips( nvsg::QuadStrips *p );

      /*! \brief Gather the nvsg::TriFans to destrip. 
       *  \param p A pointer to the constant nvsg::TriFans to destrip.
       *  \sa handleLineStrips, handleQuadStrips, handleTriStrips */
      NVSG_API virtual void handleTriFans( nvsg::TriFans *p );

      /*! \brief Gather the nvsg::TriStrips to destrip. 
       *  \param p A pointer to the constant nvsg::TriStrips to destrip.
       *  \sa handleLineStrips, handleQuadStrips, handleTriFans */
      NVSG_API virtual void handleTriStrips( nvsg::TriStrips *p );

      /*! \brief Test whether this Object should be optimized
       *  \param p A pointer to the constant nvsg::Object to test. */
      NVSG_API virtual bool optimizationAllowed( const nvsg::Object *p );

    private:
      nvsg::LinesSharedPtr      createLinesFromLineStrips( const nvsg::LineStripsWeakPtr & ls );
      nvsg::QuadsSharedPtr      createQuadsFromQuadStrips( const nvsg::QuadStripsWeakPtr & qs );
      nvsg::TrianglesSharedPtr  createTrianglesFromTriFans( const nvsg::TriFansWeakPtr & tf );
      nvsg::TrianglesSharedPtr  createTrianglesFromTriStrips( const nvsg::TriStripsWeakPtr & ts );

    private:
      std::vector<nvsg::LineStripsWeakPtr>                      m_lineStrips;
      std::map<nvsg::LineStripsWeakPtr,nvsg::LinesSharedPtr>    m_lineStripsToLines;
      std::vector<nvsg::QuadStripsWeakPtr>                      m_quadStrips;
      std::map<nvsg::QuadStripsWeakPtr,nvsg::QuadsSharedPtr>    m_quadStripsToQuads;
      std::vector<nvsg::TriFansWeakPtr>                         m_triFans;
      std::map<nvsg::TriFansWeakPtr,nvsg::TrianglesSharedPtr>   m_triFansToTriangles;
      std::vector<nvsg::TriStripsWeakPtr>                       m_triStrips;
      std::map<nvsg::TriStripsWeakPtr,nvsg::TrianglesSharedPtr> m_triStripsToTriangles;
  };
}

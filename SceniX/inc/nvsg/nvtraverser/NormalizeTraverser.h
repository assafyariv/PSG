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
#include "nvsgcommon.h"
#include "nvtraverser/Traverser.h"

namespace nvtraverser
{  

  //! Traverser that normalizes all normals.
  class NormalizeTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API NormalizeTraverser(void);

      //! Get the index of the VertexAttributeSet that is to be normalized.
      /** The default index for normalizing is NORMAL. */
      NVSG_API unsigned int getVertexAttributeIndex() const;

      //! Set the index of the VertexAttributeSet that is to be normalized.
      /** The default index for normalizing is NORMAL. */
      NVSG_API void setVertexAttributeIndex( unsigned int attrib );

      REFLECTION_INFO_API( NVSG_API, NormalizeTraverser );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( VertexAttributeIndex );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      //! Protected destructor to prevent instantiation of a NormalizeTraverser on stack.
      NVSG_API virtual ~NormalizeTraverser(void);

      //! Initiate traversal of the scene.
      NVSG_API virtual void  doApply( const nvsg::NodeSharedPtr & root );

      //! Normalize VertexAttributeSets.
      NVSG_API virtual void handleVertexAttributeSet( nvsg::VertexAttributeSet * p );

      //! Normalize AnimatedVertexAttributeSet.
      NVSG_API virtual void handleAnimatedVertexAttributeSet( nvsg::AnimatedVertexAttributeSet * p );

      //! Normalize FramedAnimationDescription<VertexAttribute>.
      NVSG_API virtual void handleFramedVertexAttributeAnimationDescription( nvsg::FramedAnimationDescription<nvsg::VertexAttribute> * p );

      //! Normalize LinearInterpolatedAnimationDescription<VertexAttribute>.
      NVSG_API virtual void handleLinearInterpolatedVertexAttributeAnimationDescription( nvsg::LinearInterpolatedAnimationDescription<nvsg::VertexAttribute> * p );

    private:
      template<unsigned char N, typename T>
        void  normalizeData( nvsg::VertexAttributeSet * p );

    private:
      unsigned int                          m_attrib;
      std::set<nvsg::VertexAttributeSet *>  m_multiOwnedVAS;
  };

  inline unsigned int NormalizeTraverser::getVertexAttributeIndex() const
  {
    return( m_attrib );
  }

  inline void NormalizeTraverser::setVertexAttributeIndex( unsigned int attrib )
  {
    if ( attrib != m_attrib )
    {
      m_attrib = attrib;
      notify( PID_VertexAttributeIndex );
    }
  }
}

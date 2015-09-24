// Copyright NVIDIA Corporation 2010
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

#include <nvtraverser/ModelViewTraverser.h>
#include <nvsg/RenderList2.h>
#include <nvsg/GeneratorState.h>

namespace nvtraverser
{

class RenderListGenerator : public SharedTraverser
{
public:
  NVSG_API RenderListGenerator( const nvsg::RenderList2WeakPtr& renderList );
  NVSG_API ~RenderListGenerator() { /* NOP */ }

  NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

  // functions for renderlist update
  NVSG_API void setCurrentTransformTreeData( nvsg::TransformTreeIndex parentIndex, nvsg::TransformTreeIndex siblingIndex );
  NVSG_API void setCurrentObjectTreeData( nvsg::ObjectTreeIndex parentIndex, nvsg::ObjectTreeIndex siblingIndex );  

  NVSG_API void addLight( const nvsg::LightSourceWeakPtr& l );
  NVSG_API void addClipPlane( const nvsg::ClipPlaneWeakPtr& l );

protected:  
  NVSG_API virtual bool preTraverseGroup( const nvsg::Group *p );
  NVSG_API virtual void postTraverseGroup( const nvsg::Group *p );
  
  NVSG_API virtual void handleTransform( const nvsg::Transform * p );

  NVSG_API virtual void handleAnimatedTransform( const nvsg::AnimatedTransform *p );
  NVSG_API virtual void handleBillboard( const nvsg::Billboard *p );
  
  NVSG_API virtual void handleLOD( const nvsg::LOD *p );
  NVSG_API virtual void handleSwitch( const nvsg::Switch *p );  
  NVSG_API virtual void handleFlipbookAnimation( const nvsg::FlipbookAnimation * anim );

  NVSG_API virtual void handleStateVariant( const nvsg::StateVariant * p );

  NVSG_API virtual void handleGeoNode( const nvsg::GeoNode *p );
  NVSG_API virtual bool preTraverseDrawables ( const nvsg::GeoNode * p, nvsg::GeoNode::StateSetConstIterator ssci );

  NVSG_API virtual void traversePrimitive( const nvsg::Primitive *p ); 
  NVSG_API virtual void traversePrimitiveSet( const nvsg::PrimitiveSet *p ); 

private:
  nvsg::RenderList2WeakPtr      m_renderList;
  nvsg::GeneratorStateSharedPtr m_generatorState;
};

}

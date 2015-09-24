// Copyright NVIDIA Corporation 2010-2011
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

#include <nvsg/Camera.h>
#include <nvsg/RenderListProcessor.h>
#include <nvsg/ViewState.h>
#include <nvui/RendererOptions.h>

namespace nvsg 
{
class IteratingRenderListProcessor : public RenderListProcessor
                                   , public nvui::RendererOptionsProvider
{
public:
  NVSG_API static const std::string & getOptionStringFrustumCulling()
  {
    static std::string s( "IteratingRenderListProcessor::FrustumCulling" );
    return( s );
  }

  NVSG_API static const std::string & getOptionStringSizeCulling()
  {
    static std::string s( "IteratingRenderListProcessor::SizeCulling" );
    return( s );
  }

  NVSG_API static const std::string & getOptionStringSizeCullingFactor()
  {
    static std::string s( "IteratingRenderListProcessor::SizeCullingFactor" );
    return( s );
  }

  NVSG_API static const std::string & getOptionStringRenderModeGroup()
  {
    static std::string s( "IteratingRenderListProcessor::RenderModeGroup" );
    return( s );
  }

  NVSG_API static const std::string & getOptionStringTraversalMaskOverride()
  {
    static std::string s( "IteratingRenderListProcessor::TraversalMaskOverride" );
    return( s );
  }

public:
  NVSG_API IteratingRenderListProcessor();
  NVSG_API virtual ~IteratingRenderListProcessor()  { /*NOP*/ };

  NVSG_API virtual RenderList2SharedPtr process( const RenderList2SharedPtr& );

  NVSG_API virtual void setRenderTarget( const nvui::SmartRenderTarget & renderTarget ) { m_renderTarget = renderTarget; }
  NVSG_API virtual const nvui::SmartRenderTarget & getRenderTarget() const              { return( m_renderTarget ); }
  NVSG_API virtual void setViewState( const ViewStateSharedPtr & viewState )            { m_viewState = viewState; }
  NVSG_API virtual const ViewStateSharedPtr & getViewState() const                      { return( m_viewState ); }

  NVSG_API virtual void doAddRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );
  NVSG_API virtual void doSetRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );

  REFLECTION_INFO_API( NVSG_API, IteratingRenderListProcessor );
  BEGIN_DECLARE_STATIC_PROPERTIES
  END_DECLARE_STATIC_PROPERTIES
protected:
  // call these functions in derived implementations if you want control flow
  virtual bool beginHandle( const RenderList2SharedPtr& );
  virtual bool beginHandle( const RenderModeGroupSharedPtr& );
  virtual bool beginHandle( const MaterialGroupSharedPtr& );
  
  // no need to call these in derived implementations
  virtual bool handleChildren( const RenderList2SharedPtr& );
  virtual bool handleChildren( const RenderModeGroupSharedPtr& );
  virtual bool handleChildren( const MaterialGroupSharedPtr& );

  virtual bool handleChildrenAgain( const RenderList2SharedPtr& );
  virtual bool handleChildrenAgain( const RenderModeGroupSharedPtr& );
  virtual bool handleChildrenAgain( const MaterialGroupSharedPtr& );

  virtual void endHandle( const RenderList2SharedPtr& );
  virtual void endHandle( const RenderModeGroupSharedPtr& );
  virtual void endHandle( const MaterialGroupSharedPtr& );

  virtual void handle( const DrawableInstanceSharedPtr& );

  nvui::SmartRenderTarget & renderTarget()  { return( m_renderTarget ); }
  ViewStateSharedPtr & viewState()          { return( m_viewState ); }

protected:
  // property ids
  nvutil::PropertyId      m_frustumCulling;
  nvutil::PropertyId      m_sizeCulling;
  nvutil::PropertyId      m_sizeCullingFactor;
  nvutil::PropertyId      m_selectedRenderMode;
  nvutil::PropertyId      m_traversalMaskOverride;

  // property value caches
  bool                    m_currentFrustumCulling;
  bool                    m_currentSizeCulling;
  float                   m_currentSizeCullingFactor;
  int                     m_currentSelectedRenderMode;
  unsigned int            m_currentTraversalMaskOverride;

  // data caches
  nvmath::Mat44f          m_worldToViewMatrix;
  unsigned int            m_traversalMask;
  nvmath::Vec4f           m_projectionSizeVector;


private:
  template< typename T > void doProcess( const nvutil::SmartPtr<T>&, CullCode );
  void doProcess( const DrawableInstanceSharedPtr&, CullCode );
  void initOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );
  void initPropertyIds();

private:
  nvui::SmartRenderTarget m_renderTarget;
  ViewStateSharedPtr      m_viewState;
};

template< typename T >
void IteratingRenderListProcessor::doProcess( const nvutil::SmartPtr<T>& item, CullCode cc ) 
{
  NVSG_ASSERT(item);
  NVSG_ASSERT( cc != CC_OUT );

  if( beginHandle( item ) )
  {
    if( handleChildren( item ) )
    {
      do
      {
        typename T::ChildIterator it;
        typename T::ChildIterator it_end = item->end();
        if ( cc == CC_IN )
        {
          if ( ! m_currentSizeCulling )
          {
            // the simple path: item is trivial in, and no size culling
            for ( it = item->begin(); it != it_end; ++it )
            {
              doProcess( *it, CC_IN );
            }
          }
          else
          {
            // less simple path: item is trivial in, but size culling might cull some things
            for ( it = item->begin(); it != it_end; ++it )
            {
              // check if a DrawableInstance below this RenderList node has NVSG_HINT_ALWAYS_VISIBLE set
              if( (*it)->containsHints( Object::NVSG_HINT_ALWAYS_VISIBLE ) )
              {
                doProcess( *it, CC_IN );
              }
              else
              {
                nvmath::Sphere3f bs = (*it)->getBoundingSphere();
                if ( isValid( bs ) )
                {
                  float minRadius = ( nvmath::Vec4f( bs.getCenter(), 1.0f ) * m_projectionSizeVector ) * m_currentSizeCullingFactor;
                  if ( minRadius <= bs.getRadius() )
                  {
                    doProcess( *it, CC_IN );
                  }
                }
              }
            }
          }
        }
        else
        {
          // the complex path: item is partial (that is: frustum culling is on),
          // and frustum or size culling might cull some things
          NVSG_ASSERT( m_currentFrustumCulling );
          CameraReadLock camera( ViewStateReadLock( m_viewState )->getCamera() );
          for ( it = item->begin(); it != it_end; ++it )
          {
            nvmath::Sphere3f bs = (*it)->getBoundingSphere();
            if ( isValid( bs ) )
            {
              nvmath::Vec4f bsc = nvmath::Vec4f( bs.getCenter(), 1.0f );
              bs.setCenter( nvmath::Vec3f( bsc * m_worldToViewMatrix ) );
              cc = camera->determineCullCode( bs );
              if ( ( cc != CC_OUT ) && m_currentSizeCulling )
              {
                // m_projectionSizeVector needs to be used with world-space bounding sphere center
                float minRadius = ( bsc * m_projectionSizeVector ) * m_currentSizeCullingFactor;
                if ( bs.getRadius() < minRadius )
                {
                  cc = CC_OUT;
                }
              }

              // check if a DrawableInstance below this RenderList node has NVSG_HINT_ALWAYS_VISIBLE set
              if( cc == CC_OUT && (*it)->containsHints( Object::NVSG_HINT_ALWAYS_VISIBLE ) )
              {
                cc = CC_PART;
              }

              if ( cc != CC_OUT )
              {
                doProcess( *it, cc );
              }
            }
          }
        }
      } while( handleChildrenAgain( item ) );
    }
    endHandle( item );
  }
}

} // namespace nvsg




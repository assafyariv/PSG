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
/** \file */

#include <nvsg/CgFx.h>
#include <nvsg/CoreTypes.h>
#include <nvsg/Object.h>
#include <nvsg/Resource.h>
#include <nvsg/RenderList2.h>
#include <nvutil/MemFunTbl.h>
#include <nvui/RendererOptions.h>
#include <memory>

namespace nvsg
{

  /*** RenderObject ***/
  class RenderObject : public NodeExecutionObject
  {
  };

  typedef nvutil::SmartPtr<RenderObject> RenderObjectSharedPtr;

  /*** RenderObjectCreator ***/
  class RenderObjectCreator : public nvutil::RCObject
  {
  public:
    virtual RenderObjectSharedPtr createRenderObject( const ObjectSharedPtr& o ) = 0;
  };
  typedef nvutil::SmartPtr< RenderObjectCreator > RenderObjectCreatorSharedPtr;

  /*** RenderObjectTmpl ***/
  template< typename ObjectRendererClass, typename ObjectType >
  class RenderObjectTmpl : public RenderObject
  {
  public:
    typedef void(ObjectRendererClass::*FunctionPtr)(const typename nvutil::ObjectTraits<ObjectType>::SharedPtr& obj, const nvsg::ResourceSharedPtr& );
    typedef void(ObjectRendererClass::*FunctionPtr2)(const typename nvutil::ObjectTraits<ObjectType>::SharedPtr& obj, nvsg::ResourceSharedPtr& );

  public:
    RenderObjectTmpl( ObjectRendererClass* objectRenderer,
                      FunctionPtr beginFunc, 
                      FunctionPtr endFunc, 
                      FunctionPtr2 updateFunc, 
                      const typename nvutil::ObjectTraits<ObjectType>::SharedPtr& obj )
      : m_objectRenderer( objectRenderer )
      , m_beginFunc( beginFunc )
      , m_endFunc( endFunc )
      , m_updateFunc( updateFunc )
      , m_obj( obj )
    {
      NVSG_ASSERT( beginFunc );
      NVSG_ASSERT( endFunc );
      NVSG_ASSERT( updateFunc );

      (m_objectRenderer->*m_updateFunc)(m_obj, m_resource ); 
    }

    virtual void callBeginFunc( )
    {
      (m_objectRenderer->*m_beginFunc)(m_obj, m_resource ); 
    }

    virtual void callEndFunc( )
    {
      (m_objectRenderer->*m_endFunc)(m_obj, m_resource ); 
    }

    virtual void callUpdateFunc( )
    {
      (m_objectRenderer->*m_updateFunc)(m_obj, m_resource ); 
    }

  private:
    RenderObjectTmpl(){}

  private:
    ObjectRendererClass*                                 m_objectRenderer;
    FunctionPtr                                          m_beginFunc;
    FunctionPtr                                          m_endFunc;
    FunctionPtr2                                         m_updateFunc;
    typename nvutil::ObjectTraits<ObjectType>::SharedPtr m_obj;
    nvsg::ResourceSharedPtr                              m_resource;
  };

  /*** RenderObjectCreatorTmpl ***/
  template< typename ObjectRendererClass, typename ObjectType, typename RenderObjectClass = RenderObjectTmpl<ObjectRendererClass, ObjectType> >
  class RenderObjectCreatorTmpl : public RenderObjectCreator
  {
  public:
    // define 'FunctionPtr' function pointer type
    typedef void(ObjectRendererClass::*FunctionPtr)(const typename nvutil::ObjectTraits<ObjectType>::SharedPtr& obj, const nvsg::ResourceSharedPtr& );
    typedef void(ObjectRendererClass::*FunctionPtr2)(const typename nvutil::ObjectTraits<ObjectType>::SharedPtr& obj, nvsg::ResourceSharedPtr& );

  protected:
    RenderObjectCreatorTmpl( ObjectRendererClass* objectRenderer, FunctionPtr funcBegin, FunctionPtr funcEnd, FunctionPtr2 funcUpdate )
      : m_objectRenderer( objectRenderer )
      , m_funcBegin( funcBegin )
      , m_funcEnd( funcEnd )
      , m_funcUpdate( funcUpdate )
    {}

  public:
    static RenderObjectCreatorSharedPtr create( ObjectRendererClass* objectRenderer, FunctionPtr funcBegin, FunctionPtr funcEnd, FunctionPtr2 funcUpdate  )
    {
      return new RenderObjectCreatorTmpl( objectRenderer, funcBegin, funcEnd, funcUpdate );
    }  

    // Override RenderObjectCreator::createRenderObject with same signature
    virtual RenderObjectSharedPtr createRenderObject( const ObjectSharedPtr& o )
    {
      // cast o to the correct shared ptr type
      const typename nvutil::ObjectTraits< ObjectType >::SharedPtr obj(
        nvutil::smart_cast< typename nvutil::ObjectTraits<ObjectType>::Handle >(o) );
      return new RenderObjectClass( m_objectRenderer, m_funcBegin, m_funcEnd, m_funcUpdate, obj );
    }

  protected:
    ObjectRendererClass* m_objectRenderer;
    FunctionPtr          m_funcBegin;
    FunctionPtr          m_funcEnd;
    FunctionPtr2         m_funcUpdate;
  };



  /*** ObjectRenderer ***/
  class ObjectRenderer;
  typedef nvutil::SmartPtr< ObjectRenderer > ObjectRendererSharedPtr;
  class ObjectRenderer : public nvutil::RCObject
                       , public nvui::RendererOptionsProvider    
  {
  public:
    enum PolygonMode
    {
      POLYGONMODE_POINT,
      POLYGONMODE_LINE,
      POLYGONMODE_FACE
    };

  public:
    NVSG_API ObjectRenderer();
    NVSG_API virtual ~ObjectRenderer()  { /* NOP */ };

    NVSG_API void setRenderState(const RenderStateSharedPtr& renderState)
    {
      m_renderState = renderState;
    }

    NVSG_API bool isSupportedOC( unsigned int objectCode );
    
    void beginRender( const nvsg::ObjectSharedPtr& oh );
    void endRender( const nvsg::ObjectSharedPtr& oh );
    
    template <typename T>
    void beginRender( const nvsg::ObjectSharedPtr& oh, T& arg );
    template <typename T>
    void endRender( const nvsg::ObjectSharedPtr& oh, T& arg );

    template <typename T>
    void beginRenderOC( const nvsg::ObjectSharedPtr& oh, unsigned int objectCode, T& arg );
    template <typename T>
    void endRenderOC( const nvsg::ObjectSharedPtr& oh, unsigned int objectCode, T& arg );

    template <typename T, typename U>
    void beginRender( const nvsg::ObjectSharedPtr& oh, T& arg0, U& arg1 );
    template <typename T, typename U>
    void endRender( const nvsg::ObjectSharedPtr& oh, T& arg0, U& arg1 );

    void beginRenderFrame();
    void endRenderFrame();

    void beginRenderMode( unsigned int mode );
    void endRenderMode( unsigned int mode );

    void enableClipPlanes();
    void disableClipPlanes();
    
    std::map< unsigned int, RenderObjectCreatorSharedPtr > m_objectRendererCreators;

    inline void addObjectRendererCreator( unsigned int objectCode, const RenderObjectCreatorSharedPtr & creator )
    {
      m_objectRendererCreators[ objectCode ] = creator;
    }

    template< typename ObjectType, 
      void (ObjectRenderer::*fnBegin) (const ObjectType*, const nvsg::ResourceSharedPtr&), 
      void (ObjectRenderer::*fnEnd) (const ObjectType*, const nvsg::ResourceSharedPtr&) >
    RenderObjectSharedPtr create( const ObjectSharedPtr& o )
    {
      return new RenderObjectTmpl< ObjectRenderer, ObjectType >( this, fnBegin, fnEnd, o );
    }

    RenderObjectSharedPtr createRenderObject( const ObjectSharedPtr& o )
    {
      unsigned int oc = getObjectRenderCode(ObjectReadLock(o));
      if ( nvsg::OC_INVALID != oc ) 
      {
        return m_objectRendererCreators[oc]->createRenderObject(o);
      }
      NVSG_ASSERT(!"Encountered invalid object code!");
      return nullptr;
    }

    NVSG_API static const std::string& getOptionStringTessellationEnabled() 
    { 
      static std::string s("nvsg::ObjectRenderer::TessellationEnabled");
      return s;
    }

    NVSG_API static const std::string& getOptionStringPolygonMode()
    { 
      static std::string s("nvsg::ObjectRenderer::PolygonMode" );
      return s;
    }

    enum RenderTechnique
    {
      RENDERTECHNIQUE_UNLIT,
      RENDERTECHNIQUE_SHADED,
      RENDERTECHNIQUE_TEXTURED,
      RENDERTECHNIQUE_EFFECTED
    };

    NVSG_API static const std::string& getOptionStringRenderTechnique() 
    { 
      static std::string s("nvsg::ObjectRenderer::RenderTechnique");
      return s;
    }

    NVSG_API static const std::string& getOptionStringPreserveTexturesAfterUpload()
    {
      static std::string s("nvsg::ObjectRenderer::PreserveTexturesAfterUpload");
      return s;
    }

  protected:
    NVSG_API virtual void doAddRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );
    NVSG_API virtual void doSetRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );
    
    template<typename RendererType>
    void addObjectBeginRenderer( unsigned int objectCode, RendererType renderer );

    template<typename RendererType>
    void addObjectEndRenderer( unsigned int objectCode, RendererType renderer );

    unsigned int getObjectRenderCode( const nvsg::Object * obj );
    
    void render( const nvsg::ObjectSharedPtr& oh, nvutil::MemFunTbl<ObjectRenderer>& funTbl );
    
    template <typename T>
    void render( const nvsg::ObjectSharedPtr& oh, nvutil::MemFunTbl<ObjectRenderer>& funTbl, T& arg );

    template <typename T>
    void renderOC( const nvsg::ObjectSharedPtr& oh, unsigned int objectCode, nvutil::MemFunTbl<ObjectRenderer>& funTbl, T& arg );

    template <typename T, typename U>
    void render( const nvsg::ObjectSharedPtr& oh, nvutil::MemFunTbl<ObjectRenderer>& funTbl, T& arg0, U& arg1 );

  protected:
    NVSG_API virtual void doBeginRenderFrame();
    NVSG_API virtual void doEndRenderFrame();

    NVSG_API virtual void doBeginRenderMode( unsigned int mode );
    NVSG_API virtual void doEndRenderMode( unsigned int mode );

    NVSG_API virtual void doEnableClipPlanes();
    NVSG_API virtual void doDisableClipPlanes();

  public:
    // these must be called in derived functions!
    NVSG_API virtual void beginRenderBlendAttribute( const nvsg::BlendAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderCgFx( const nvsg::CgFxSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderClipPlane( const nvsg::ClipPlaneSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderDirectedLight( const nvsg::DirectedLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderFaceAttribute( const nvsg::FaceAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderMatrixCamera( const nvsg::MatrixCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderParallelCamera( const nvsg::ParallelCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderPerspectiveCamera( const nvsg::PerspectiveCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderPointLight( const nvsg::PointLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderSpotLight( const nvsg::SpotLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderStatePass( const nvsg::StatePassSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderTextureAttribute( const nvsg::TextureAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );

    NVSG_API virtual void endRenderBlendAttribute( const nvsg::BlendAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderCgFx( const nvsg::CgFxSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderClipPlane( const nvsg::ClipPlaneSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderDirectedLight( const nvsg::DirectedLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderFaceAttribute( const nvsg::FaceAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderPointLight( const nvsg::PointLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderSpotLight( const nvsg::SpotLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderStatePass( const nvsg::StatePassSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderTextureAttribute( const nvsg::TextureAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );

    NVSG_API virtual void updateRenderBlendAttribute( const nvsg::BlendAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderCgFx( const nvsg::CgFxSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderClipPlane( const nvsg::ClipPlaneSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderDirectedLight( const nvsg::DirectedLightSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderFaceAttribute( const nvsg::FaceAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderPointLight( const nvsg::PointLightSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderSpotLight( const nvsg::SpotLightSharedPtr &p, nvsg::ResourceSharedPtr& res );

    // these don't need to be called in derived functions
    NVSG_API virtual void beginRenderAlphaTestAttribute( const nvsg::AlphaTestAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderDepthAttribute( const nvsg::DepthAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderLightingAttribute( const nvsg::LightingAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderLineAttribute( const nvsg::LineAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderMaterial( const nvsg::MaterialSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderPointAttribute( const nvsg::PointAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderPrimitive( const nvsg::PrimitiveSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderRTFx( const nvsg::RTFxSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderShape( const nvsg::ShapeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderStencilAttribute( const nvsg::StencilAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderTextureAttributeItem( const nvsg::TextureAttributeItemSharedPtr &p, const unsigned int& tu, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderUnlitColorAttribute( const nvsg::UnlitColorAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    
    NVSG_API virtual void endRenderAlphaTestAttribute( const nvsg::AlphaTestAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderDepthAttribute( const nvsg::DepthAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderLightingAttribute( const nvsg::LightingAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderLineAttribute( const nvsg::LineAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderMaterial( const nvsg::MaterialSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderMatrixCamera( const nvsg::MatrixCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderParallelCamera( const nvsg::ParallelCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderPerspectiveCamera( const nvsg::PerspectiveCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderPointAttribute( const nvsg::PointAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderPrimitive( const nvsg::PrimitiveSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderRTFx( const nvsg::RTFxSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderShape( const nvsg::ShapeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderStencilAttribute( const nvsg::StencilAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderTextureAttributeItem( const nvsg::TextureAttributeItemSharedPtr &p, const unsigned int& tu, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderUnlitColorAttribute( const nvsg::UnlitColorAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    
    NVSG_API virtual void updateRenderAlphaTestAttribute( const nvsg::AlphaTestAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderDepthAttribute( const nvsg::DepthAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderLightingAttribute( const nvsg::LightingAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderLineAttribute( const nvsg::LineAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderMaterial( const nvsg::MaterialSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderMatrixCamera( const nvsg::MatrixCameraSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderParallelCamera( const nvsg::ParallelCameraSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderPerspectiveCamera( const nvsg::PerspectiveCameraSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderPointAttribute( const nvsg::PointAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderPrimitive( const nvsg::PrimitiveSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderRTFx( const nvsg::RTFxSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderShape( const nvsg::ShapeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderStatePass( const nvsg::StatePassSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderStencilAttribute( const nvsg::StencilAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderTextureAttribute( const nvsg::TextureAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderTextureAttributeItem( const nvsg::TextureAttributeItemSharedPtr &p, unsigned int& tu, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void updateRenderUnlitColorAttribute( const nvsg::UnlitColorAttributeSharedPtr &p, nvsg::ResourceSharedPtr& res );
  protected:  

    // Renderer options
    nvutil::PropertyId m_polygonMode;
    nvutil::PropertyId m_tessellationEnabled;
    nvutil::PropertyId m_renderTechnique;
    nvutil::PropertyId m_preserveTexturesAfterUpload;

    PolygonMode     m_currentDefaultPolygonMode;
    RenderTechnique m_currentRenderTechnique;
    bool            m_currentTessellationEnabled;
    bool            m_currentPreserveTexturesAfterUpload;

  protected:
    RenderState&    renderState()    { return *m_renderState; }
    MaterialState&  materialState()  { return m_renderState->m_materialState; }
    ShaderState&    shaderState()    { return m_renderState->m_materialState.m_shaderState; }
    TransformState& transformState() { return m_renderState->m_transformState; }
    LightState&     lightState()     { return m_renderState->m_lightState; }
    ClipPlaneState& clipPlaneState() { return m_renderState->m_clipPlaneState; }

  private:
    void addLight( const nvsg::LightSourceSharedPtr& lightHdl );
    void removeLight( const nvsg::LightSourceSharedPtr& lightHdl );

    void addClipPlane( const nvsg::ClipPlaneSharedPtr& clipPlaneHdl );
    void removeClipPlane( const nvsg::ClipPlaneSharedPtr& clipPlaneHdl );

    void setCamera(const nvsg::Camera * p);
  protected:
    // current render state
    RenderStateSharedPtr              m_renderState;
    
  private:
    // function pointers to begin and end functions
    nvutil::MemFunTbl<ObjectRenderer> m_beginFunctions;
    nvutil::MemFunTbl<ObjectRenderer> m_endFunctions;  
  };

  inline void ObjectRenderer::beginRender(  const nvsg::ObjectSharedPtr& oh )
  {
    render( oh, m_beginFunctions );
  }

  inline void ObjectRenderer::endRender(  const nvsg::ObjectSharedPtr& oh )
  {
    render( oh, m_endFunctions );
  }

  template <typename T>
  inline void ObjectRenderer::beginRender(  const nvsg::ObjectSharedPtr& oh, T& arg )
  {
    render( oh, m_beginFunctions, arg );
  }

  template <typename T>
  inline void ObjectRenderer::endRender(  const nvsg::ObjectSharedPtr& oh, T& arg )
  {
    render( oh, m_endFunctions, arg );
  }

  template <typename T>
  inline void ObjectRenderer::beginRenderOC(  const nvsg::ObjectSharedPtr& oh, unsigned int objectCode, T& arg )
  {
    renderOC( oh, objectCode, m_beginFunctions, arg );
  }

  template <typename T>
  inline void ObjectRenderer::endRenderOC(  const nvsg::ObjectSharedPtr& oh, unsigned int objectCode, T& arg )
  {
    renderOC( oh, objectCode, m_endFunctions, arg );
  }
  
  template <typename T, typename U>
  inline void ObjectRenderer::beginRender(  const nvsg::ObjectSharedPtr& oh, T& arg0, U& arg1 )
  {
    render( oh, m_beginFunctions, arg0, arg1 );
  }

  template <typename T, typename U>
  inline void ObjectRenderer::endRender(  const nvsg::ObjectSharedPtr& oh, T& arg0, U& arg1 )
  {
    render( oh, m_endFunctions, arg0, arg1 );
  }

  inline void ObjectRenderer::beginRenderFrame()
  {
    {
      // cache current renderer options
      nvui::RendererOptionsReadLock options( m_rendererOptions );
      m_currentDefaultPolygonMode           = options->getValue<PolygonMode>(m_polygonMode);
      m_currentTessellationEnabled          = options->getValue<bool>(m_tessellationEnabled);
      m_currentRenderTechnique              = options->getValue<RenderTechnique>(m_renderTechnique);
      m_currentPreserveTexturesAfterUpload  = options->getValue<bool>(m_preserveTexturesAfterUpload);
    }

    doBeginRenderFrame();
  }

  inline void ObjectRenderer::endRenderFrame()
  {
    doEndRenderFrame();
  }

  inline void ObjectRenderer::beginRenderMode( unsigned int mode )
  {
    doBeginRenderMode( mode );
  }

  inline void ObjectRenderer::endRenderMode( unsigned int mode )
  {
    doEndRenderMode( mode );
  }

  inline void ObjectRenderer::enableClipPlanes()
  {
    clipPlaneState().m_clipPlanesEnabled = true;
    doEnableClipPlanes();
  }

  inline void ObjectRenderer::disableClipPlanes()
  {
    clipPlaneState().m_clipPlanesEnabled = false;
    doDisableClipPlanes();
  }

  inline void ObjectRenderer::render( const nvsg::ObjectSharedPtr& oh, nvutil::MemFunTbl<ObjectRenderer>& funTbl )
  {
    NVSG_TRACE();
      
    // share-lock the object while being rendered
    ObjectReadLock obj(oh);
      
    unsigned int oc = getObjectRenderCode( obj );
    if ( nvsg::OC_INVALID != oc ) 
    {

  #if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      void * __esp__0;
      void * __esp__1;
      __asm mov __esp__0, esp
  #endif

      (this->*(void (ObjectRenderer::*)(const nvsg::Object*))(funTbl)[oc])(obj);

  #if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      __asm mov __esp__1, esp
      NVSG_ASSERT(__esp__1==__esp__0);
  #endif

    }
  }

  template <typename T>
  inline void ObjectRenderer::render( const nvsg::ObjectSharedPtr& oh, nvutil::MemFunTbl<ObjectRenderer>& funTbl, T& arg )
  {
    NVSG_TRACE();
      
    // share-lock the object while being rendered
    //ObjectReadLock obj(oh);
      
    unsigned int oc = getObjectRenderCode( ObjectReadLock(oh) );
    if ( nvsg::OC_INVALID != oc ) 
    {
          
  #if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      void * __esp__0;
      void * __esp__1;
      __asm mov __esp__0, esp
  #endif

      (this->*(void (ObjectRenderer::*)(const nvsg::ObjectSharedPtr &, T&))(funTbl)[oc])(oh, arg);

  #if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      __asm mov __esp__1, esp
      NVSG_ASSERT(__esp__1==__esp__0);
  #endif

    }
  }

  template <typename T>
  inline void ObjectRenderer::renderOC( const nvsg::ObjectSharedPtr& oh, unsigned int objectCode, nvutil::MemFunTbl<ObjectRenderer>& funTbl, T& arg )
  {
    NVSG_TRACE();
    NVSG_ASSERT( nvsg::OC_INVALID != objectCode );
          
#if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
    void * __esp__0;
    void * __esp__1;
    __asm mov __esp__0, esp
#endif

    (this->*(void (ObjectRenderer::*)(const nvsg::ObjectSharedPtr &, T&))(funTbl)[objectCode])(oh, arg);

#if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
    __asm mov __esp__1, esp
    NVSG_ASSERT(__esp__1==__esp__0);
#endif
  }

  template <typename T, typename U>
  inline void ObjectRenderer::render( const nvsg::ObjectSharedPtr& oh, nvutil::MemFunTbl<ObjectRenderer>& funTbl, T& arg0, U& arg1 )
  {
    NVSG_TRACE();
      
    // share-lock the object while being rendered
    ;
      
    unsigned int oc = getObjectRenderCode( ObjectReadLock(oh) );
    if ( nvsg::OC_INVALID != oc ) 
    {
          
  #if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      void * __esp__0;
      void * __esp__1;
      __asm mov __esp__0, esp
  #endif

      (this->*(void (ObjectRenderer::*)(const nvsg::ObjectSharedPtr &, T&, U&))(funTbl)[oc])(oh, arg0, arg1);

  #if !defined(_WIN64) && defined(_WIN32) && !defined(NDEBUG)
      __asm mov __esp__1, esp
      NVSG_ASSERT(__esp__1==__esp__0);
  #endif

    }
  }

  template<typename RendererType>
    inline void ObjectRenderer::addObjectBeginRenderer( unsigned int objectCode, RendererType renderer )
  {
    m_beginFunctions.addEntry( objectCode, renderer ); 
  }

  template<typename RendererType>
    inline void ObjectRenderer::addObjectEndRenderer( unsigned int objectCode, RendererType renderer )
  {
    m_endFunctions.addEntry( objectCode, renderer ); 
  }
    
  inline unsigned int ObjectRenderer::getObjectRenderCode(const nvsg::Object * obj)
  {
    NVSG_TRACE();

    unsigned int oc = obj->getObjectCode();
    NVSG_ASSERT(oc!=nvsg::OC_INVALID);

    if ( !m_beginFunctions.testEntry(oc) )
    {
      unsigned int orgOC = oc;
      while ( !m_beginFunctions.testEntry(oc) )
      { // came across an unknown object
        // move up the object's hierarchy to find an appropriate handler
        oc = obj->getHigherLevelObjectCode(oc);
        if ( nvsg::OC_INVALID==oc )
        { // proceed immediately without handling the object
          return oc; 
        }
      }
              
      // found an appropriate renderer if we get here
      // don't loop again for this object - register the handler
      addObjectRendererCreator(orgOC, (m_objectRendererCreators)[oc]);
      addObjectBeginRenderer(orgOC, (m_beginFunctions)[oc]);
      addObjectEndRenderer(orgOC, (m_endFunctions)[oc]);
    }
    return oc;
  }

} // namespace nvsg

namespace nvutil
{
  template <> struct TypedPropertyEnum<nvsg::ObjectRenderer::PolygonMode>
  {
    enum { type = Property::TYPE_INT };
  };

  template <> struct TypedPropertyEnum<nvsg::ObjectRenderer::RenderTechnique>
  {
    enum { type = Property::TYPE_INT };
  };
} // namespace nvutil

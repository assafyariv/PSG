// Copyright NVIDIA Corporation 2011
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

#include <Cg/cgGL.h>

#include <nvsg/ObjectRenderer.h>

#include <nvgl/BufferGL.h>
#include <nvgl/TextureGL.h>
#include <nvgl/RenderContextGL.h>
#include <nvgl/DisplayListGL.h>

#include <nvsg/FaceAttribute.h>
#include <nvsg/Material.h>
#include <nvsg/Primitive.h>
#include <nvsg/RenderList2.h>
#include <nvmath/Vecnt.h>
#include <nvmath/Matnnt.h>

#include <nvui/RendererOptions.h>

namespace nvgl
{
  enum
  {
    RES_VERTEX_BUFFER,
    RES_INDEX_BUFFER, 
    RES_DISPLAY_LIST,
    RES_TEXTURE
  };

  enum
  {
    NVSG_MAX_TEXTURE_IMAGES = 32
  };

  const unsigned int MAX_CACHED_TEXTUREATTRIBUTE_ITEMS = 4;

  class GLObjectRenderer;
  typedef nvutil::SmartPtr< GLObjectRenderer > GLObjectRendererSharedPtr;

  /* \brief The GLObjectRendererGLStateCache manages some OpenGL states in an abstract way. It reduces the number of
            OpenGL calls as much as possible.
  **/
  class GLObjectRendererGLStateCache
  {
  public:
    GLObjectRendererGLStateCache();

    /* \brief Reset all cached values to defaults. */
    NVSG_API void reset();

    /* \brief Flag all cached values as invalid. The next call to a set function then performs a reset(). */
    NVSG_API void invalidate();

    // TODO Support caching including VAO state.
    /* \brief Enable or disable all attribute arrays corresponding to the given mask.
       \param clientAttribState A bitmask specifiying which attributes should be enabled. 
              Bits 0-15 enable fixed function attributes and bits 16-31 enable the generic attributes.
    **/
    NVSG_API void setClientAttribState( unsigned int clientAttribState );

    NVSG_API void setPrimitiveRestartState( bool enabled, GLuint restartIndex );

  private:
    bool m_cacheValid;

    GLuint m_currentClientAttribState;

    bool   m_primitiveRestartEnabled;
    GLuint m_primitiveRestartIndex;
  };
  
  class GLObjectRendererTextureCache;
  CORE_TYPES(GLObjectRendererTextureCache, nvutil::Observer );
  class GLObjectRendererTextureCache : public nvutil::Observer
  {
    friend struct nvutil::Holder<GLObjectRendererTextureCache>;

  public:
    class Payload : public Subject::Payload
    {
    public:
      Payload( nvsg::TextureWeakPtr tex )
        : m_tex( tex )
      {
      }

      nvsg::TextureWeakPtr m_tex;
    };
    typedef nvutil::SmartPtr<Payload> SmartPayload;

  protected:
    NVSG_API GLObjectRendererTextureCache();
    NVSG_API ~GLObjectRendererTextureCache();

  public:
    NVSG_API static GLObjectRendererTextureCacheSharedPtr create();

    NVSG_API nvgl::TextureGLSharedPtr getGLTexture( nvsg::TextureSharedPtr tex );

    NVSG_API virtual void onDestroyed( const nvutil::Subject* subject, const nvutil::Subject::SmartPayload &payload );

  private:
    typedef std::map< nvsg::TextureWeakPtr, std::pair<nvgl::TextureGLSharedPtr, SmartPayload> > Textures;
    Textures m_textures;
  };

  class RLResource;
  typedef nvutil::SmartPtr< RLResource > RLResourceSharedPtr;
  class RLResource : public nvsg::Resource
  {
  protected:
    RLResource() {}

  public:
    static RLResourceSharedPtr create() { return new RLResource; }
  };

  class RMGResource;
  typedef nvutil::SmartPtr< RMGResource > RMGResourceSharedPtr;
  class RMGResource : public nvsg::Resource
  {
  protected:
    RMGResource() {}

  public:
    static RMGResourceSharedPtr create() { return new RMGResource; }
  };

  class TextureInfo;
  typedef nvutil::SmartPtr< TextureInfo > TextureInfoSharedPtr;
  class TextureInfo : public nvutil::RCObject
  {
  protected:
    TextureInfo()
    {}
    TextureInfo(const TextureInfo& rhs) {} // prevent copy by accident

  public:
    static TextureInfoSharedPtr create() { return new TextureInfo; }

  public:
    nvgl::TextureGLSharedPtr texture;
    GLuint                   textureID;
    GLenum                   textureTarget;
    bool                     originIsHost;
  };

  class MGResource;
  typedef nvutil::SmartPtr< MGResource > MGResourceSharedPtr;
  class MGResource : public nvsg::Resource
  {
  protected:
    MGResource()
    {}

  public:
    static MGResourceSharedPtr create() { return new MGResource; }

    struct TexInfo
    {
      TextureInfoSharedPtr info;
      SmartShareGroupGLResource resource;
    };

    typedef std::vector< std::pair<nvsg::StateAttributeSharedPtr, unsigned int> > StateAttributeVector;

    struct PassInfo
    {
      nvsg::StatePassSharedPtr m_statePass;
      StateAttributeVector m_statePassAttributes;

      // reserve for NVSG_MAX_TEXTURE_IMAGES texture images
      TexInfo m_texture[NVSG_MAX_TEXTURE_IMAGES];
      std::map<CGparameter, TexInfo> m_sampler;

      // CgFx
      nvsg::CgFxSharedPtr m_cgfx;

      // storage for fixed function material
      nvmath::Vec4f m_backAmbientColor;
      nvmath::Vec4f m_backDiffuseColor;
      nvmath::Vec4f m_backEmissiveColor;
      nvmath::Vec4f m_backSpecularColor;
      float         m_backShininess;

      nvmath::Vec4f m_frontAmbientColor;
      nvmath::Vec4f m_frontDiffuseColor;
      nvmath::Vec4f m_frontEmissiveColor;
      nvmath::Vec4f m_frontSpecularColor;
      float         m_frontShininess;

      nvmath::Vec4f m_unlitColor;
      bool m_lightingAttribute;
      bool m_isLightingAttributeEnabled;

      // storage for stencil attribute
      bool m_isStencilEnabled;
      bool m_isStencilSeparateEnabled;

      unsigned int m_stencilFrontWriteMask;       // indicates which bit planes to write to
      GLenum       m_stencilFrontFunction;        // stencil function
      GLint        m_stencilFrontReferenceValue;  // reference value
      GLuint       m_stencilFrontValueMask;       // apply reference value comparison only to those bits set in this mask
      GLenum       m_stencilFrontFail;            // operation performed if fragment fails the stencil test
      GLenum       m_stencilFrontZFail;           // operation performed if fragment passes the test, and fails the depth test
      GLenum       m_stencilFrontZPass;           // operation performed if fragment passes the test, and passes the depth test

      unsigned int m_stencilBackWriteMask;       // indicates which bit planes to write to
      GLenum       m_stencilBackFunction;        // stencil function
      GLint        m_stencilBackReferenceValue;  // reference value
      GLuint       m_stencilBackValueMask;       // apply reference value comparison only to those bits set in this mask
      GLenum       m_stencilBackFail;            // operation performed if fragment fails the stencil test
      GLenum       m_stencilBackZFail;           // operation performed if fragment passes the test, and fails the depth test
      GLenum       m_stencilBackZPass;           // operation performed if fragment passes the test, and passes the depth test

      // storage for point attribute
      bool  m_isPointAttributeEnabled;

      bool  m_pointAttributeAntiAliasing;
      float m_pointAttributeSize;

      // storage for blend attribute
      bool  m_isBlendAttributeEnabled;

      GLenum m_blendSourceFunction;
      GLenum m_blendDestinationFunction;

      // storage for texture attribute
      unsigned char                       m_textureAttributeItemCount; // 4 bits are enough here
      unsigned int                        m_textureAttributeItemUnits[MAX_CACHED_TEXTUREATTRIBUTE_ITEMS];
      nvsg::TextureAttributeItemSharedPtr m_textureAttributeItems[MAX_CACHED_TEXTUREATTRIBUTE_ITEMS];
    };

    std::vector<PassInfo> m_info;
  };

  class TGResource;
  typedef nvutil::SmartPtr< TGResource > TGResourceSharedPtr;
  class TGResource : public nvsg::Resource
  {
  protected:
    TGResource() {}
  public:
    static TGResourceSharedPtr create() { return new TGResource; }

  public:
    nvsg::RenderObjectSharedPtr m_renderObject;
  };

  struct VertexAttributeFormat
  {
    enum
    {
      IS_NORMALIZED = BIT0,
      IS_INTEGER    = BIT1
    };

    VertexAttributeFormat()
      : numComponents(0)
      , type(GL_INVALID_VALUE)
      , offset(0)
      , stride(0)
      , formatFlags(0)
    {}

    bool operator==( const VertexAttributeFormat& rhs ) const
    {
      return numComponents == rhs.numComponents
          && type == rhs.type
          && offset == rhs.offset
          && stride == rhs.stride
          && formatFlags == rhs.formatFlags;
    }
    bool operator!=( const VertexAttributeFormat& rhs ) const
    {
       return !(*this==rhs);
    }

    GLint        numComponents; // attribute size (i.e. # components)
    GLenum       type;          // attribute type in GL land
    GLsizeiptr   offset;        // offset inside buffer
    unsigned int stride;        // attribute stride
    unsigned int formatFlags;   // attribute format flags
  };

  class VertexDeclaration;
  typedef nvutil::SmartPtr< VertexDeclaration > VertexDeclarationSharedPtr;
  class VertexDeclaration : public nvutil::RCObject
  {
  protected:
    VertexDeclaration()
      : size(0)
      , containsUserBuffers(false)
      , enabledMask(0)
    {};
  public:
    static VertexDeclarationSharedPtr create() { return new VertexDeclaration; }

    bool operator==( const VertexDeclaration& rhs )
    {
      for( unsigned int i=0; i < nvsg::VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT; ++i )
      {
        if( attributes[i] != rhs.attributes[i] )
        {
          return false;
        }
      }
      return true;
    }

    bool operator!=( const VertexDeclaration& rhs )
    {
      return !(*this==rhs);
    }

  public:
    VertexAttributeFormat     attributes[nvsg::VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT];
    BufferGLSharedPtr         bufferGLs[nvsg::VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT];
    GLuint                    bufferGLIds[nvsg::VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT];
    GLint                     size;

    bool                      containsUserBuffers;  // true if buffers may not be reused during upload
    unsigned int              enabledMask;          // enabled mask for attributes
    std::vector<unsigned int> enabledAttributes;    // list of enabled attributes 
  };

  class VertexBufferInfo;
  typedef nvutil::SmartPtr< VertexBufferInfo > VertexBufferInfoSharedPtr;
  class VertexBufferInfo : public nvutil::RCObject
  {
  protected:
    VertexBufferInfo()
      : vertexAttribOffset( 0 )
      , vertexCount( 0 )
    {}
    VertexBufferInfo(const VertexBufferInfo& rhs) {} // prevent copy by accident

  public:
    static VertexBufferInfoSharedPtr create() { return new VertexBufferInfo; }

  public:
    VertexDeclarationSharedPtr vertexDeclaration;
    GLsizeiptr                 vertexAttribOffset;
    unsigned int               vertexCount;
    nvutil::Incarnation        vasIncarnation; // incarnation count of the underlying vertex attribute set
  };

  class IndexBufferInfo;
  typedef nvutil::SmartPtr< IndexBufferInfo > IndexBufferInfoSharedPtr;
  class IndexBufferInfo : public nvutil::RCObject
  {
  protected:
    IndexBufferInfo()
      : indexBufferID( 0 )
      , indexBufferDataType( 0 )
      , primitiveRestartIndex( 0 )
      , indexCount( 0 )
    {}
    IndexBufferInfo(const IndexBufferInfo& rhs) {} // prevent copy by accident

  public:
    static IndexBufferInfoSharedPtr create() { return new IndexBufferInfo; }

  public:
    nvgl::BufferGLSharedPtr    indexBuffer;
    GLuint                     indexBufferID;
    unsigned int               indexBufferDataType;
    unsigned int               primitiveRestartIndex;
    unsigned int               indexCount;
    nvutil::Incarnation        indexSetIncarnation; // incarnation of indexset for gl buffer
  };

  class DisplayListInfo;
  typedef nvutil::SmartPtr< DisplayListInfo > DisplayListInfoSharedPtr;
  class DisplayListInfo : public nvutil::RCObject
  {
  protected:
    DisplayListInfo( ) {}
    DisplayListInfo(const DisplayListInfo& rhs) {} // prevent copy by accident

  public:
    static DisplayListInfoSharedPtr create( ) { return new DisplayListInfo( ); }
    
  public:
    DisplayListGLSharedPtr displayList;  
    nvutil::Incarnation    primitiveIncarnation; // incarnation of primitive used to create this DL
  };

  class DIResource;
  typedef nvutil::SmartPtr< DIResource > DIResourceSharedPtr;
  typedef DIResource* DIResourceWeakPtr;
  class DIResource : public nvsg::Resource
  {
  protected:
    DIResource();
    ~DIResource();

  public:
    static DIResourceSharedPtr create();

    class SetupFunction : public nvutil::IAllocator
    {
    public:
      NVSG_API virtual ~SetupFunction() {};
      NVSG_API virtual void operator()() const = 0;
    };

    // Nullary function which setups the VAS state
    SetupFunction             *bufferSetup; // setup VAS and Indices

    // Primitive Information
    GLenum                    glMode;
    unsigned int              numberOfVerticesPerPatch;
    unsigned int              elementCount;
    unsigned int              elementOffset;
    unsigned int              indexBufferDataType; // copy from IndexBuffer for faster access
    unsigned int              renderFlags;
    bool                      hasDrawCallback;
    bool                      hasVertexColors;
    unsigned int              instanceCount;

    // Vertex Attrib Buffer
    VertexBufferInfoSharedPtr vertexBufferInfo;
    SmartShareGroupGLResource vertexBufferResource;

    // Index Buffer
    IndexBufferInfoSharedPtr  indexBufferInfo;
    SmartShareGroupGLResource indexBufferResource;
    nvutil::STLVector<unsigned int> indexStripCount; // number of elements per strip in indexbuffer.

    // Display List
    DisplayListInfoSharedPtr  displayListInfo;
    DisplayListGL*            displayList;
    SmartShareGroupGLResource displayListResource;

    unsigned int m_enableFlags[nvsg::VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT];
    unsigned int m_isNormalized[nvsg::VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT];

    nvutil::Incarnation       primitiveIncarnation;
  };

  class GLObjectRenderer : public nvsg::ObjectRenderer
  {
  public:
    enum CacheMode
    {
      CACHEMODE_UNCACHED,
      CACHEMODE_DL,
      CACHEMODE_VBO,
      CACHEMODE_NUM_CACHEMODES
    };

    // RendererOptions related defines and functions
    enum FaceCullMode
    {
      CULLMODE_FRONT,
      CULLMODE_BACK,
      CULLMODE_FRONT_AND_BACK,
      CULLMODE_NONE
    };

    NVSG_API static const std::string& getOptionStringCacheMode() 
    { 
      static std::string s("nvgl::GLObjectRenderer::CacheMode"); 
      return s;
    }
    NVSG_API static const std::string& getOptionStringFaceCullMode()
    {
      static std::string s( "nvgl::GLObjectRenderer::FaceCullMode" );
      return( s );
    }
    NVSG_API static const std::string& getOptionStringLightModelLocalViewer()
    {
      static std::string s("nvgl::GLObjectRenderer::LightModelLocalViewer");
      return( s );
    }
    NVSG_API static const std::string& getOptionStringLightModelTwoSide()
    {
      static std::string s("nvgl::GLObjectRenderer::LightModelTwoSide");
      return( s );
    }
    NVSG_API static const std::string& getOptionStringSeparateSpecularColor()
    {
      static std::string s("nvgl::GLObjectRenderer::SeparateSpecularColor");
      return( s );
    }
    NVSG_API static const std::string& getOptionStringTransparencyDepthMask()
    {
      static std::string s( "nvgl::GLObjectRenderer::TransparencyDepthMask" );
      return( s );
    }

  protected:
    NVSG_API GLObjectRenderer();

  public:
    NVSG_API virtual ~GLObjectRenderer();

    static GLObjectRendererSharedPtr create()
    {
      return new GLObjectRenderer;
    }

  public:
    NVSG_API virtual void doAddRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );
    NVSG_API virtual void doSetRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );

    NVSG_API virtual void doBeginRenderFrame();
    NVSG_API virtual void doEndRenderFrame();

    NVSG_API virtual void doBeginRenderMode( unsigned int mode );
    NVSG_API virtual void doEndRenderMode( unsigned int mode );

    NVSG_API virtual void doEnableClipPlanes();
    NVSG_API virtual void doDisableClipPlanes();

    NVSG_API virtual void beginRenderAlphaTestAttribute( const nvsg::AlphaTestAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderCgFx( const nvsg::CgFxSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderClipPlane( const nvsg::ClipPlaneSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderDepthAttribute( const nvsg::DepthAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderDirectedLight( const nvsg::DirectedLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderFaceAttribute( const nvsg::FaceAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderLightingAttribute( const nvsg::LightingAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderLineAttribute( const nvsg::LineAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderMatrixCamera( const nvsg::MatrixCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderParallelCamera( const nvsg::ParallelCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderPerspectiveCamera( const nvsg::PerspectiveCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderPointLight( const nvsg::PointLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderPrimitive( const nvsg::PrimitiveSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderSpotLight( const nvsg::SpotLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderStatePass( const nvsg::StatePassSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void beginRenderTextureAttributeItem( const nvsg::TextureAttributeItemSharedPtr &p , const unsigned int& tu, const nvsg::ResourceSharedPtr& res );

    NVSG_API virtual void endRenderAlphaTestAttribute( const nvsg::AlphaTestAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderCgFx( const nvsg::CgFxSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderClipPlane( const nvsg::ClipPlaneSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderDepthAttribute( const nvsg::DepthAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderDirectedLight( const nvsg::DirectedLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderFaceAttribute( const nvsg::FaceAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderLightingAttribute( const nvsg::LightingAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderLineAttribute( const nvsg::LineAttributeSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderMatrixCamera( const nvsg::MatrixCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderParallelCamera( const nvsg::ParallelCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderPerspectiveCamera( const nvsg::PerspectiveCameraSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderPointLight( const nvsg::PointLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderPrimitive( const nvsg::PrimitiveSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderSpotLight( const nvsg::SpotLightSharedPtr &p, const nvsg::ResourceSharedPtr& res );
    NVSG_API virtual void endRenderTextureAttributeItem( const nvsg::TextureAttributeItemSharedPtr &p , const unsigned int& tu, const nvsg::ResourceSharedPtr& res );

    NVSG_API virtual void updateRenderPrimitive( const nvsg::PrimitiveSharedPtr &p, nvsg::ResourceSharedPtr& res );
  protected:
    // primitive rendering helper functions
    NVSG_API void uploadVertexData( const nvsg::VertexAttributeSet* p, VertexBufferInfoSharedPtr &vboBufInfo );
    NVSG_API VertexBufferInfoSharedPtr useGLBuffer( const nvsg::VertexAttributeSet* p );
    NVSG_API void buildVertexBufferCache( const DIResourceSharedPtr& resource, const nvsg::VertexAttributeSet * p);
    NVSG_API void buildIndexBufferCache( const DIResourceSharedPtr& resource, const nvsg::IndexSet *indices );
    NVSG_API void beginRenderPrimitiveUncached( const nvsg::PrimitiveSharedPtr &p, const DIResourceSharedPtr& res );
    NVSG_API void beginRenderPrimitiveDL( const nvsg::PrimitiveSharedPtr &p, const DIResourceSharedPtr& res );
    NVSG_API void beginRenderPrimitiveVBO( const nvsg::PrimitiveSharedPtr &p, const DIResourceSharedPtr& res );
    NVSG_API void updateRenderPrimitiveDL( const nvsg::PrimitiveSharedPtr &p, const DIResourceSharedPtr& diRes );
    NVSG_API void updateRenderPrimitiveVBO( const nvsg::PrimitiveSharedPtr &p, const DIResourceSharedPtr& diRes );

    // camera rendering helper functions
    NVSG_API void beginRenderCamera( const nvsg::CameraSharedPtr &p );
    NVSG_API void endRenderCamera( const nvsg::CameraSharedPtr &p );

    NVSG_API void setConstantFramerate( bool on );

  protected:
    // property ids of needed renderer options, pointing to the current ObjectRenderer::m_rendererOptions    
    nvutil::PropertyId m_cacheMode;
    nvutil::PropertyId m_faceCullMode;
    nvutil::PropertyId m_lightModelLocalViewer;
    nvutil::PropertyId m_lightModelTwoSide;
    nvutil::PropertyId m_separateSpecularColor;
    nvutil::PropertyId m_transparencyDepthMask;

    // property caches for renderer options
    CacheMode     m_currentCacheMode;
    FaceCullMode  m_currentDefaultFaceCullMode;
    bool          m_currentLightModelLocalViewer;
    bool          m_currentLightModelTwoSide;
    bool          m_currentSeparateSpecularColor;
    bool          m_currentTransparencyDepthMask;

    // derived caches for easier access
    bool m_currentFrontFaceCulling;
    bool m_currentBackFaceCulling;

    // cache for gl states
    GLObjectRendererGLStateCache m_glStateCache;

    // cache for TextureGL objects
    GLObjectRendererTextureCacheSharedPtr m_textureCache; // shared ptr so observer is instantiated correctly

  private:
    bool addGLLight( const nvsg::LightSourceSharedPtr& lightHdl, GLenum &lightId );
    void removeGLLight( const nvsg::LightSourceSharedPtr& lightHdl );

    bool addGLClipPlane( const nvsg::ClipPlaneSharedPtr& clipPlaneHdl, GLenum &clipPlaneId );
    void removeGLClipPlane( const nvsg::ClipPlaneSharedPtr& clipPlaneHdl );

    void setPolygonMode( GLenum face, nvsg::FaceMode mode, const nvsg::FaceAttributeSharedPtr &p = nullptr );

    nvsg::PrimitiveSharedPtr triPatchFallback( const nvsg::PatchesBase * p, unsigned int size );
    nvsg::PrimitiveSharedPtr rectPatchFallback( const nvsg::PatchesBase * p, unsigned int width, unsigned int height );

    void updateTextureInfo( const nvsg::TextureHostSharedPtr &tex, const nvgl::TextureInfoSharedPtr &textureInfo);
    void checkTextureInfoForUpdate(const nvsg::TextureSharedPtr &tex, const nvgl::TextureInfoSharedPtr &textureInfo);
    void createTextureInfo(const nvsg::TextureSharedPtr &texturePtr, SmartShareGroupGLResource &res, TextureInfoSharedPtr &info);

    unsigned int calculateEnabledMask( const nvsg::VertexAttributeSet *vas );

  private:    
    size_t m_maxGLLights;
    size_t m_numGLLights;
    std::vector< nvsg::LightSourceSharedPtr > m_GLLights;

    size_t m_maxGLClipPlanes;
    size_t m_numGLClipPlanes;
    std::vector< nvsg::ClipPlaneSharedPtr > m_GLClipPlanes;

    nvutil::TransformStack m_transformStack;

    unsigned int m_currentEnabledMask;
    unsigned int m_currentUniformVertexAttributeFormat[ nvsg::VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT ]; // id for current vertex attribute format;
    bool m_anisotropyAvailable;
    bool m_tessellationAvailable;
    bool m_renderTesselated;  // enabled if tesselation is supported by hw and shader
    bool m_bindlessAvailable;
    bool m_bindlessActive;
    bool m_primitiveRestartAvailable;

    bool m_VAOAvailable;  // VAOs will be used for VertexAttributeSetup.
  };

} // namespace nvgl

namespace nvutil
{
  template <> struct TypedPropertyEnum<nvgl::GLObjectRenderer::CacheMode>
  {
    enum { type = Property::TYPE_INT };
  };

  template <> struct TypedPropertyEnum<nvgl::GLObjectRenderer::FaceCullMode>
  {
    enum { type = Property::TYPE_INT };
  };
} // namespace nvutil

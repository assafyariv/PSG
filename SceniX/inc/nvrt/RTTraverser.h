// Copyright NVIDIA Corporation 2009-2011
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

#include <vector>
#include <nvsgcommon.h> // common used stuff 
#include <nvmath/Matnnt.h>
#include <nvrt/RTAcceleration.h>
#include <nvrt/RTAccelerationCache.h>
#include <nvrt/RTDALData.h>
#include <nvtraverser/ModelViewTraverser.h> // base class definition
#include <nvutil/Observer.h>

namespace nvrt
{
  class RenderTargetRT;
  class RTVariable;
  class RenderTargetRT;
  typedef nvutil::SmartPtr<RenderTargetRT> SmartRenderTargetRT;
};

namespace nvsg
{
  class Material;
  class TextureAttributeItem;
  class LOD;
  class RTFx;
  class RTFxBase;
  class RTFxSceneAttribute;
  class Scene;
};

namespace nvtraverser
{

//! RTTraverser generates NVIRT API data structures
#define BASECLASS ExclusiveModelViewTraverser

class RTTraverser : public nvtraverser::BASECLASS
{

  private:
    // ShaderLightSource must be kept in sync with the built-in 
    // CUDA C programs' LightSource.h and the DAELoader profile_COMMON.rtfx!
    struct ShaderLightSource
    {
      float ambientColor[3];
      float diffuseColor[3];
      float specularColor[3];
      float position[3];
      float direction[3];
      float attenuation[3];
      float cosCutoff;
      float exponent;
      int   type;
      int   flags;  // New field added after SceniX 7.0.9.0.2.
    };

    // ShaderLightSource.flags bitfield definitions.
    enum ShaderLightSourceFlags
    {
      SLSF_IS_SHADOW_CASTING = 1 // flags bit 0 set when LightSource::isShadowCasting() is true (default).
    };

    /* From CUDAPrograms/Material.h - must be kept in sync!!
     * struct Material
     * {
     *   float4 ambientColor;
     *   float4 diffuseColor;
     *   float4 emissiveColor;
     *   float4 opacityColor;
     *   float4 reflectivityColor;
     *   float4 specularColor;
     *   float  reflectivity;
     *   float  specularExponent;
     *   float  ior;
     *   float  opacity;
     * };
    */

    struct ShaderMaterial
    {
      float ambientColor[4];
      float diffuseColor[4];
      float emissiveColor[4];
      float opacityColor[4];
      float reflectivityColor[4];
      float specularColor[4];
      float reflectivity;
      float specularExponent;
      float ior;
      float opacity;
    };

    // special Observer for RTTraverser, to enable observing without being a Subject
    class RTTraverserObserver;
    CORE_TYPES( RTTraverserObserver, nvutil::Observer );

    class RTTraverserObserver : public nvutil::Observer
    {
      public:
        NVRT_API static RTTraverserObserverSharedPtr create()
        {
          return( RTTraverserObserverHandle::create() );
        }

      public:
        RTTraverserObserver() : m_traverser(NULL) {}

        virtual void onDestroyed( const Subject *subject, const nvutil::Subject::SmartPayload &payload )
        {
          NVSG_ASSERT( m_traverser );
          m_traverser->onDestroyed( subject, payload );
        }

        void setTraverser( RTTraverser * traverser )
        {
          NVSG_ASSERT( !m_traverser && traverser );
          m_traverser = traverser;
        }

      private:
        RTTraverser * m_traverser;    // back-link to the RTTraverser
    };

    friend class RTTraverserObserver;

  public:
    //! Default constructor
    RTTraverser( const nvrt::RTContextWeakPtr & context );

    virtual void setViewState( const nvsg::ViewStateSharedPtr &viewState );
    void setSceneFileName( std::string & fileName );

    void setRenderTargetRT( const nvrt::SmartRenderTargetRT &renderTargetRT );
    nvrt::SmartRenderTargetRT getRenderTargetRT() const;

    NVRT_API void setAccelerationCache( const nvrt::RTAccelerationCacheWeakPtr & ach );
    NVRT_API nvrt::RTAccelerationCacheWeakPtr getAccelerationCache() const;

    NVRT_API void setEntryPointIndex( unsigned int idx );
    NVRT_API unsigned int getEntryPointIndex() const;

    NVRT_API void storeAccelCaches();

    // reflection interface
    NVRT_API void setOptions( nvutil::Reflection * opt );
    NVRT_API nvutil::Reflection * getOptions() const;

  protected:    
    //! Default-instantiates a RayTracingTraverser object.
    virtual ~RTTraverser(void);

    virtual void doApply( const nvsg::NodeSharedPtr & root );
    virtual void postTraversalTrace();

    virtual void handleGroup( nvsg::Group * p );                          // create an RTGroup
    virtual void handleFlipbookAnimation( nvsg::FlipbookAnimation * p );  // ???
    virtual bool preTraverseTransform( const nvmath::Trafo * p );         // store trafo
    virtual void handleTransform( nvsg::Transform *p );                   // create an RTTransform
    virtual void handleAnimatedTransform( nvsg::AnimatedTransform *p );   // handle just like a Transform
    virtual void handleLOD( nvsg::LOD * p );                              // traverses highest level only; produced no RTNode !
    virtual void handleSwitch( nvsg::Switch * p );                        // currently simulated by an RTGroup !!
    virtual void handleBillboard( nvsg::Billboard * billboard );          // ???

    virtual void handleGeoNode( nvsg::GeoNode * p );                      // create an RTGeometryGroup
    virtual void handlePrimitive( nvsg::Primitive * p );                  // create an RTGeometryGroup and an RTGeometryInstance
    virtual void handleQuads( nvsg::Quads * p );                          // create an RTGeometry and an RTGeometryInstance
    virtual void handleTriangles( nvsg::Triangles * p );                  // create an RTGeometry and an RTGeometryInstance
    virtual void handleQuadMeshes( nvsg::QuadMeshes * p );                // create an RTGeometryGroup
    virtual void handleQuadStrips( nvsg::QuadStrips * p );                // create an RTGeometryGroup
    virtual void handleTriFans( nvsg::TriFans * p );                      // create an RTGeometryGroup
    virtual void handleTriStrips( nvsg::TriStrips * p );                  // create an RTGeometryGroup

    virtual void handleStateSet( nvsg::StateSet * p );
    virtual void handleRTBufferAttribute( nvsg::RTBufferAttribute * p );
    virtual void handleRTFx( nvsg::RTFx * p );
    virtual void handleMaterial( nvsg::Material * p );
    virtual void handleTextureAttributeItem( nvsg::TextureAttributeItem * p );

    virtual void handleVertexAttributeSet( nvsg::VertexAttributeSet * p );  // create an RTBuffer as the vertexAttributeBuffer


    virtual void handleAnimatedVertexAttributeSet( nvsg::AnimatedVertexAttributeSet * p );
    virtual void handleSkin( nvsg::Skin * p );

    virtual void traverseSceneAttributes( nvsg::Scene * scene );
    virtual void createScenePrograms( nvsg::RTFxSceneAttribute * rtfx, nvrt::RTFxSceneAttributeCache * data, bool recreate = false );

    virtual void traverseCamera( nvsg::Camera * cam );
    virtual bool preTraverseGroup( const nvsg::Group * grp );
    virtual void postTraverseGroup( const nvsg::Group * grp );
    virtual void postTraverseTransform( const nvmath::Trafo * trafo );    // needed for Billboard calculations
    virtual bool preTraverseDrawables( const nvsg::GeoNode* gnode, nvsg::GeoNode::StateSetConstIterator ssci );

    // special function called from the RTTraverserObserver, if an observed subject is going to be destroyed
    void onDestroyed( const Subject *subject, const nvutil::Subject::SmartPayload &payload );

    // update the running traversal mask
    unsigned int updateTraversalMask( const nvsg::Object * obj );

  private:
    typedef std::map< size_t, size_t > AttributeMap;

  private:
    nvsg::TextureAttributeItemWeakPtr getTextureOnTextureUnit( unsigned int which );
    void                      setVariable( nvrt::RTVariable *, const nvsg::RTFxParameter & param );
    unsigned int              getAttributeMask( const AttributeMap & attributes );
    void                      createDefaultTexture( void );
    void                      addLightSources( void );
    nvrt::RTGroupSharedPtr    createGroup( const nvsg::Group * p );
    nvrt::RTBufferSharedPtr   createIndexBuffer( const nvsg::QuadMeshes * p );
    nvrt::RTBufferSharedPtr   createIndexBuffer( const nvsg::Quads * p );
    nvrt::RTBufferSharedPtr   createIndexBuffer( const nvsg::QuadStrips * p );
    nvrt::RTBufferSharedPtr   createIndexBuffer( const nvsg::Triangles * p );
    nvrt::RTBufferSharedPtr   createIndexBuffer( const nvsg::TriFans * p );
    nvrt::RTBufferSharedPtr   createIndexBuffer( const nvsg::TriStrips * p );
    nvrt::RTBufferSharedPtr   createIndexBuffer( const nvsg::Primitive * p, unsigned int & numPrims );
    void                      createIntersectionPrograms( const AttributeMap & attributes, nvrt::RTGeometry * geom );
    nvrt::RTMaterialSharedPtr createRTMaterial( const nvrt::RTMaterialSharedPtr & src = nvrt::RTMaterialSharedPtr() );
    void                      createHitandAnyHitPrograms( const nvsg::RTFxWeakPtr & rtfx, const nvrt::RTMaterialSharedPtr & material );
    void                      determineAccelerationTypes( const nvsg::GeoNode * p
                                                        , nvrt::RTAcceleration::RTBuilderType & rtbt
                                                        , nvrt::RTAcceleration::RTTraverserType & rttt );
    void                      determineAccelerationTypes( const nvsg::Group * p
                                                        , nvrt::RTAcceleration::RTBuilderType & rtbt
                                                        , nvrt::RTAcceleration::RTTraverserType & rttt );
    void                      getShaderMaterial( const nvsg::Material * p, ShaderMaterial & sm );
    void                      setMaterialComponents( const nvsg::Material * p, const nvrt::RTMaterialSharedPtr & rtm );
    void                      initAccelerationGeometry( const nvrt::RTAccelerationWeakPtr & accelerationHdl
                                                      , nvrt::RTAcceleration::RTBuilderType rtbt
                                                      , nvrt::RTAcceleration::RTTraverserType rttt );
    void                      initAccelerationHierarchy( const nvrt::RTAccelerationWeakPtr & accelerationHdl
                                                       , nvrt::RTAcceleration::RTBuilderType rtbt
                                                       , nvrt::RTAcceleration::RTTraverserType rttt );
    bool                      isChildrenBarrier() const;
    nvrt::RTGroupCache      * processGroup( const nvsg::Group * p );
    template<typename T>
    void                      processPrimitiveSet( T * p );
    void                      processTextures( const nvsg::RTFxBase * rtfx, nvrt::RTVariablesManager * vholder );
    void                      processBuffers( const nvsg::RTFxBase * rtfx, nvrt::RTVariablesManager * vholder );
    void                      pushLightType( const nvsg::LightSourceSharedPtr & lsh, const nvmath::Mat44f&, const nvmath::Mat44f& );
    void                      setGlobalVariables( const nvsg::RTFxBase * p, const nvrt::RTContextWeakPtr & ch );
    void                      updateTextureHosts( const nvsg::RTFxBase * p );
    void                      setLocalVariables( const nvsg::RTFxBase * p, const nvrt::RTMaterialSharedPtr & mh );
    void                      setLocalVariables( const nvsg::RTFxBase * p, const nvrt::RTGeometryWeakPtr & gh );
    void                      updateChildren( const nvrt::RTGroupWeakPtr & groupHdl );
    void                      updateTopLevelGroup( );
    void                      updateLightSources( void );
    void                      traceCamera( nvrt::RTContext *, const nvsg::Camera * );

    void interleaveVASData( nvrt::RTBuffer* buffer, nvrt::RTBuffer* vertexBuffer, nvsg::VertexAttributeSet * p );

    enum ProgramFormat
    {
      PFORMAT_DEFAULT,
      PFORMAT_CUDA_FILE,
      PFORMAT_PTX
    };

    struct LibraryEntry
    {
      const char * identifier;
      const char * entryPoint;
      // must be unsigned because of bin2c
      const unsigned char * codeTextPTX;
      const unsigned char * codeTextPTX_64;
      const char * codeFileCUDA;
      ProgramFormat flags;
    };

    void                      initProgramLibrary();
    nvrt::RTProgramSharedPtr  getCompiledProgram( const std::string & id );
    const char              * getProgramString( const std::string & id, ProgramFormat & fmtInOut, std::string & entry );

    // RT DAL Data handling
    nvrt::RTSelectorCache *genSelectorCache(nvsg::DALHost *host, nvsg::Object *p);
    nvrt::RTSelectorCache *getSelectorCache(const nvsg::DALHost *host) const;

    nvrt::RTTransformCache *genTransformCache(nvsg::DALHost *host, nvsg::Group *p, const nvmath::Trafo &trafo);
    nvrt::RTTransformCache *getTransformCache(const nvsg::DALHost *host) const;
    void updateTransformCache(nvrt::RTTransformCache *cache, nvsg::Group *p, const nvmath::Trafo &trafo);

    nvrt::RTGeometryGroupCache *genGeometryGroupCache(nvsg::DALHost *host, nvsg::Object *p);
    nvrt::RTGeometryGroupCache *getGeometryGroupCache(const nvsg::DALHost *host) const;

    nvrt::RTMaterialCache *genMaterialCache(nvsg::DALHost *host, nvsg::Object *p);
    nvrt::RTMaterialCache *getMaterialCache(const nvsg::DALHost *host) const;
    void                   regenMaterialCache(nvrt::RTMaterialCache * cache );

    nvrt::RTBufferCache *genBufferCache(nvsg::DALHost *host, nvsg::Object *p);
    nvrt::RTBufferCache *getBufferCache(const nvsg::DALHost *host) const;
    nvrt::RTBufferCache *getOrGenBufferCache(nvsg::RTBufferAttribute *p);
    void updateRTBuffer( nvrt::RTBufferCache *c, nvsg::RTBufferAttribute *p );
    
    nvrt::RTTextureSamplerCache *genTextureSamplerCache(nvsg::DALHost *host, nvsg::Object *p, const nvsg::TextureAttributeItemSharedPtr & taih);
    nvrt::RTTextureSamplerCache *getTextureSamplerCache(const nvsg::DALHost *host) const;
    nvrt::RTTextureSamplerCache *getOrGenTextureSamplerCache(const nvsg::TextureAttributeItemSharedPtr & taih);
    void updateTextureSampler( nvrt::RTTextureSamplerCache *c, const nvsg::TextureAttributeItemSharedPtr & p );
    
    nvrt::RTVASBufferCache *genVASBufferCache(nvsg::DALHost *host, nvsg::Object *p);
    nvrt::RTVASBufferCache *getVASBufferCache(const nvsg::DALHost *host) const;
    
    nvrt::RTFxSceneAttributeCache *genRTFxSceneAttributeCache(nvsg::DALHost *host, nvsg::Object *p);
    nvrt::RTFxSceneAttributeCache *getRTFxSceneAttributeCache(const nvsg::DALHost *host) const;

    nvrt::RTGroupCache *genGroupCache(nvsg::DALHost *host, const nvsg::Group *p);
    nvrt::RTGroupCache *getGroupCache(const nvsg::DALHost *host) const;

    nvrt::RTGeometryCache *genGeometryCache(nvsg::DALHost *host, nvsg::Object *p);
    nvrt::RTGeometryCache *getGeometryCache(const nvsg::DALHost *host) const;

  private:
    // set this here for now
    enum { NVRT_MAX_TEXTURE_UNITS = 8 };

    // RenderTarget must be the first variable to ensure that it's destroyed as last object.
    // m_renderTargetRT keeps a reference to the RTConext used for all the nvrt-objects
    // created within the scope of an instance of this class. If the RTcontext is being
    // destroyed before all nvrt objects have been destroyed OptiX will crash when destroying
    // an object whose context had been destroyed before.

#if 0
    nvrt::SmartRenderTargetRT  m_renderTargetRT;
    nvrt::SmartRenderTargetRT  m_lastRenderTargetRT; // RenderTargetRT used for last rendercall
#else
    nvrt::RenderTargetRT *m_renderTargetRT;     // Store only weak references to RenderTarget to break cycle RTContext/RenderTarget/RTTraverser
    nvrt::RenderTargetRT *m_lastRenderTargetRT; // RenderTargetRT used for last rendercall
#endif
    
    typedef std::map< nvsg::SceneWeakPtr, nvrt::RTGroupSharedPtr > SceneGroupMap;
    typedef std::map< nvutil::Subject *, nvutil::Subject::SmartPayload > SubjectMap;

    nvsg::RTFxSceneAttributeSharedPtr     m_defaultRTFxSceneAttribute;
    nvrt::RTAccelerationCacheSharedPtr    m_rtCache;
    nvrt::RTDALDataCreator              * m_DALDataCreator; 
    AttributeMap                          m_currentAttributes;
    std::vector<nvrt::RTNodeWeakPtr>      m_currentChildren;    //! a list of all children for the current object
    std::vector<nvrt::RTNodeWeakPtr>      m_topLevelChildren;    //! a list of all children for the toplevel object
    bool                                  m_parentTransformChanged;  //! true if the transform for any parent children has been changed
    std::vector<std::pair<nvrt::RTMaterialWeakPtr,nvrt::RTGeometryWeakPtr> > m_currentGeometries;
    nvrt::RTGroupWeakPtr                  m_currentGroup;
    unsigned int                          m_currentGroupID;
    nvmath::Mat44f                        m_currentTransformation;
    nvsg::MaterialWeakPtr                 m_currentMaterial;
    nvsg::RTFxWeakPtr                     m_currentRTFx;
    nvrt::RTMaterialWeakPtr               m_currentRTMaterial;
    nvsg::RTBufferAttributeWeakPtr        m_currentRTBufferAttribute;
    nvrt::RTProgramWeakPtr                m_currentShader;
    nvsg::TextureAttributeItemWeakPtr     m_currentTextureAttributeItems[NVRT_MAX_TEXTURE_UNITS];
    nvrt::RTMaterialSharedPtr             m_defaultRTMaterial;
    nvrt::RTBufferSharedPtr               m_lightSourceBuffer;
    std::vector< ShaderLightSource >      m_lightSources;
    nvsg::Scene                         * m_previousScene;
    nvrt::RTGroupSharedPtr                m_topLevelGroup;      //!< top level node that everything is added to
    SceneGroupMap                         m_topLevelGroups;  //! keep one top level group per scene to avoid recompiles
    std::map< const nvutil::Subject *, std::string> m_subjectVariable; //! Mapping from Subject to variable which needs to be destroyed on destruction of the subject
    std::map< const nvutil::Subject *, nvsg::SceneWeakPtr> m_subjectToScene; //! Mapping from Subject to Scene
    nvrt::RTTextureSamplerCache         * m_defaultTextureSamplerCache;
    nvsg::TextureAttributeItemSharedPtr   m_defaultTextureAttributeItem;
    bool                                  m_hierarchyModified;  //! true if scene hierarchy changed, or a compile is needed
    bool                                  m_vertexBufferDataModified; //! true if the buffer data in a VAS was modified
    std::set<nvsg::Billboard *>           m_billboards;         // billboards encountered on traversal; used to detect multiple instances
    std::vector<void *>                   m_currentPathID;      // unique path identification from root to the current Billboard; used to distinguish multiple instances
    nvsg::Billboard                     * m_currentBillboard;
    unsigned int                          m_entryPointIndex;
    std::map< std::string, LibraryEntry > m_libraryEntries;
    std::vector<int>                      m_currentHints;         // Need a stack to store the hints over pre/postTraverse limits.
    std::vector<unsigned int>             m_currentTraversalMask; // Need a stack to store the TM over pre/postTraverse limits.
    unsigned int                          m_oldTraversalMaskOverride;
    unsigned int                          m_viewStateTraversalMask;

    nvrt::RTContextWeakPtr                m_rtContext;
    SubjectMap                            m_subjects; // Subjects attached to.

    nvrt::RTProgramSharedPtr  m_anyHitShadow;
    nvrt::RTProgramSharedPtr  m_anyHitShadowTex;
    nvrt::RTProgramSharedPtr  m_anyHit;
    nvrt::RTProgramSharedPtr  m_anyHitTex;
    nvrt::RTProgramSharedPtr  m_matHitProgram;
    nvrt::RTProgramSharedPtr  m_texHitProgram;
    nvrt::RTProgramSharedPtr  m_visitFlipbookProgram;
    nvrt::RTProgramSharedPtr  m_visitSwitchProgram;

    nvrt::RTProgramSharedPtr  m_boundProgram;
    nvrt::RTProgramSharedPtr  m_intersectProgram;

    bool                    m_preserveTexturesAfterUpload;

    nvutil::Reflection    * m_options;

    RTTraverserObserverSharedPtr  m_observer;   // special Observer to enable observing even without being derived from Subject
    bool                      m_inSkin;
};

inline unsigned int RTTraverser::updateTraversalMask( const nvsg::Object * p )
{
  unsigned int mask = ( p->getTraversalMask() | m_oldTraversalMaskOverride ) & m_viewStateTraversalMask;
  return (m_currentTraversalMask.back() & mask) ? ~0 : 0;
}

inline nvutil::Reflection * RTTraverser::getOptions() const
{
  return m_options;
}

inline nvrt::RTAccelerationCacheWeakPtr RTTraverser::getAccelerationCache() const
{
  NVSG_TRACE();
  return( m_rtCache.get() );
}

inline void RTTraverser::setEntryPointIndex( unsigned int index )
{
  m_entryPointIndex = index;
}

inline unsigned int RTTraverser::getEntryPointIndex() const
{
  return( m_entryPointIndex );
}

} // end namespace nvtraverser

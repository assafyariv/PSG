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

#include <nvsg/CgFx.h>
#include <nvsg/CoreTypes.h>
#include <nvsg/FlipbookAnimation.h>
#include <nvsg/GeneratorState.h>
#include <nvsg/GeoNode.h>
#include <nvsg/LOD.h>
#include <nvsg/Object.h>
#include <nvsg/Switch.h>
#include <nvsg/Transform.h>
#include <nvsg/Resource.h>

#include <nvutil/Handle.h>
#include <nvutil/HandledObject.h>
#include <nvutil/Observer.h>
#include <nvutil/Subject.h>

#include <nvmath/Trafo.h>

#include <vector>
#include <stack>
#include <algorithm>
#include <memory>

namespace nvsg
{

  /*===========================================================================*/
  struct RenderState;
  typedef nvutil::SmartPtr< RenderState > RenderStateSharedPtr;
  struct RenderState : public nvutil::RCObject
  {
    RenderState()
      : m_renderMode(0)
      , m_blendAttributeEnabled(false)
      , m_faceWindingCCW(true)
      , m_lightingEnabled(true)
    {}

    struct TransformState
    {
      nvmath::Mat44f m_viewToClip;
      nvmath::Mat44f m_clipToView;
      nvmath::Mat44f m_worldToView;
      nvmath::Mat44f m_viewToWorld;
      nvmath::Mat44f m_worldToModel;
      nvmath::Mat44f m_modelToWorld;
      nvmath::Vec4f  m_projSizeVector; // per-frame projection size vector
    };

    struct LightState
    {
      LightState() 
        : m_processingHeadlight(false)
      {}

      class LightEntity
      {
      public:
        LightEntity( nvsg::LightSourceWeakPtr lightSource, nvmath::Mat44f transform, bool headlight )
          : m_lightSource(lightSource)
          , m_transform(transform)
          , m_isHeadlight(headlight)
        {}

        nvsg::LightSourceWeakPtr m_lightSource;
        nvmath::Mat44f           m_transform;
        bool                     m_isHeadlight;
      };

      std::vector< LightEntity > m_lights;
      bool                       m_processingHeadlight;
    };

    struct ClipPlaneState
    {
      ClipPlaneState()
        : m_clipPlanesEnabled( true )
      {}

      class ClipPlaneEntity
      {
      public:
        ClipPlaneEntity( nvsg::ClipPlaneWeakPtr clipPlane, nvmath::Mat44f transform )
          : m_clipPlane(clipPlane)
          , m_transform(transform)
        {
        }

        nvsg::ClipPlaneWeakPtr m_clipPlane;
        nvmath::Mat44f         m_transform;
      };

      std::vector< ClipPlaneEntity > m_clipPlanes;
      bool m_clipPlanesEnabled;
    };

    struct MaterialState
    {
      struct ShaderState
      {
        ShaderState()
        {
          reset();
        }

        void reset()
        {
          m_lockedFx.reset(NULL);
          m_technique = NULL;
          m_pass = ~0;
          m_passCnt = 0;
        }

        std::auto_ptr<CgFxEffectWriteLock> m_lockedFx;
        CgFxTechnique m_technique;
        unsigned int m_pass; // current pass
        unsigned int m_passCnt; // absolut pass count for current technique
      };

      unsigned int m_pass; // current material state pass
      unsigned int m_passCnt; // absolute material state pass count
      ShaderState  m_shaderState;
    };

    RenderList2WeakPtr m_renderList;
    TransformState     m_transformState;
    LightState         m_lightState;
    ClipPlaneState     m_clipPlaneState;
    MaterialState      m_materialState;
    unsigned int       m_renderMode;
    bool               m_blendAttributeEnabled;
    bool               m_lightingEnabled;
    bool               m_faceWindingCCW;
    nvmath::Vec4f      m_sceneAmbient;
    int                m_sceneFrame;
  };

  typedef RenderState::TransformState             TransformState;
  typedef RenderState::LightState                 LightState;
  typedef RenderState::ClipPlaneState             ClipPlaneState;
  typedef RenderState::MaterialState              MaterialState;
  typedef RenderState::MaterialState::ShaderState ShaderState;

  typedef RenderState::LightState::LightEntity    LightEntity;

  /*===========================================================================*/
  class NodeExecutionObject : public nvutil::RCObject
  {
  public:
    virtual void callBeginFunc() {};
    virtual void callEndFunc() {};
    virtual void callUpdateFunc() {};
  };

  typedef nvutil::SmartPtr<NodeExecutionObject> SmartNodeExecutionObject;

  /*===========================================================================*/
  template < class ParentClass >
  class RenderListChildClass
  {
  protected:
    RenderListChildClass()
      : m_parent( nullptr )
      , m_dirty( false )
      , m_id(m_counter++)
    {}

  public:
    void setParent( const ParentClass &parent )
    {
      m_parent = parent;
      if( m_parent && m_dirty )
      {
        m_parent->setDirty(true);
      }
    }
    const ParentClass &getParent() { return m_parent; }

    void setDirty( bool dirty ) 
    {
      if( dirty && !m_dirty && m_parent )
      {
        m_parent->setDirty(true);
      }
      m_dirty = dirty; 
    }
    bool isDirty() const { return m_dirty; }

    unsigned int getID() { return m_id; };

  private:
    ParentClass         m_parent;
    bool                m_dirty;
    static unsigned int m_counter;
    unsigned int        m_id;
  };

  template<class ParentClass> unsigned int RenderListChildClass<ParentClass>::m_counter = 0;

  /*===========================================================================*/
  template < typename ChildClass >
  class RenderListParentClass
  {
  public:
    typedef nvutil::STLVector<ChildClass>         Children;
    typedef ChildClass                          * ChildIterator;
    typedef const ChildClass                    * ConstChildIderator;
    typedef nvutil::STLMap<ChildClass, size_t>    ChildrenMap;

  protected:
    RenderListParentClass()
    {
    }

    virtual ~RenderListParentClass()
    {
    }

  public:
    void addChild( const ChildClass& child )
    {
      NVSG_ASSERT( m_childrenMap.find( child ) == m_childrenMap.end() );

      m_childrenMap[child] = m_children.size();
      m_children.push_back( child );
      m_firstChild = &m_children[0];
    }

    void removeChild( const ChildClass& child ) 
    {
      typename ChildrenMap::iterator it = m_childrenMap.find( child );
      if( it != m_childrenMap.end() )
      {
        size_t idx = it->second;
        size_t lastIndex = m_children.size() - 1;

        m_childrenMap.erase(it);
        // any element except the last one had been removed, do the fast swapping
        if ( idx != lastIndex )
        {
          m_children[idx] = m_children[lastIndex];
          if ( lastIndex )
          {
            m_childrenMap[m_children[idx]] = idx;
          }
        }
        m_children.resize( lastIndex );
      }
    }
    size_t numChildren()        { return m_children.size(); }
    ChildIterator begin()       { return m_firstChild; }
    ChildIterator end()         { return m_firstChild + numChildren(); }

    template< typename Compare >
    void sortChildren( const Compare& c )
    {
      std::sort( m_children.begin(), m_children.end(), c );

      for ( size_t idx = 0;idx < numChildren(); ++idx )
      {
        m_childrenMap[ m_children[idx] ] = idx;
      }
    }

  protected:
    nvmath::Sphere3f combineBoundingSpheres()
    {
      nvmath::Sphere3f bs;
      for ( typename Children::iterator it = m_children.begin() ; it != m_children.end() ; ++it )
      {
        bs = boundingSphere( bs, (*it)->getBoundingSphere() );
      }
      return( bs );
    }

    unsigned int combineHints()
    {
      unsigned int hints( 0 );
      for ( typename Children::iterator it = m_children.begin() ; it != m_children.end() ; ++it )
      {
        hints |= (*it)->getHints();
      }
      return( hints );
    }

  private:
    Children    m_children;
    ChildrenMap m_childrenMap;
    ChildClass* m_firstChild;
  };  

  /*===========================================================================*/

  class RenderListNodeClass : public nvutil::RCObject
  {
  protected: 
    RenderListNodeClass()
    {}

  public:
    void setResource( const ResourceSharedPtr& res ) { m_resource = res; }
    const ResourceSharedPtr &getResource() const     { return m_resource; }

    void setNodeExecutionObject( const SmartNodeExecutionObject & obj ) { m_executionObject = obj; }
    const SmartNodeExecutionObject &getNodeExecutionObject() const { return m_executionObject; }

  private:
    ResourceSharedPtr          m_resource;
    SmartNodeExecutionObject   m_executionObject;
  };

  /*===========================================================================*/
  enum RenderMode
  {
    RENDER_OPAQUE              = 0,
    RENDER_TRANSPARENT         = BIT0,                                // 1 = transparent, 0 = opaque
    RENDER_OVERLAY             = BIT1,                                // 1 = render to overlay   
    RENDER_OVERLAY_OPAQUE      = RENDER_OVERLAY,                      
    RENDER_OVERLAY_TRANSPARENT = RENDER_OVERLAY | RENDER_TRANSPARENT,
    RENDER_BITS                = RENDER_OVERLAY | RENDER_TRANSPARENT, // all possible render bits
  };

  /*===========================================================================*/
  class RenderList2 : public RenderListParentClass< RenderModeGroupSharedPtr >
    , public RenderListNodeClass
  { 
  protected:
    RenderList2(const NodeWeakPtr& root);
    virtual ~RenderList2();

  public:
    template <typename IndexType> class Observer;

    class SwitchObserver;
    CORE_TYPES( SwitchObserver, nvutil::Observer );

    class GeoNodeObserver;
    CORE_TYPES( GeoNodeObserver, nvutil::Observer );

    class TransformObserver;
    CORE_TYPES( TransformObserver, nvutil::Observer );

    class ObjectObserver;
    CORE_TYPES( ObjectObserver, nvutil::Observer );

    class RootNodeObserver;
    CORE_TYPES( RootNodeObserver, nvutil::Observer );

  public:
    NVSG_API static RenderList2SharedPtr create( const NodeWeakPtr& root );

    NVSG_API void addSubTree( const NodeWeakPtr& root, 
                              nvsg::ObjectTreeIndex parentIndex, nvsg::ObjectTreeIndex leftSibling,
                              nvsg::TransformTreeIndex parentTransform, nvsg::TransformTreeIndex leftSiblingTransform );

    /** \brief Replace a subtree by a another subtree.
        \param root root node of the new subtree to put into the renderlist
        \param nodeIndex index of the node which should be replaced by the new subtree
    **/
    NVSG_API void replaceSubTree( const NodeWeakPtr &root, nvsg::ObjectTreeIndex nodeIndex );

    //! Add all renderer options required by this renderer to the given rendererOptions object.
    NVSG_API void addRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions );

    //! Add a DrawableInstance to the RenderList by sorting it into the correct groups.
    NVSG_API void addDrawable( const DrawableInstanceSharedPtr& drawable );

    //! Add a GeoNode to the list of dirty GeoNodes
    NVSG_API void dirtyGeoNode( ObjectTreeIndex index );

    //! Add a DrawableInstance to the List of dirty DrawableInstances
    NVSG_API void dirtyDrawable( const DrawableInstanceSharedPtr& drawable );

    //! Update a DrawableInstance during the next rendering step
    NVSG_API void updateDrawable( const DrawableInstanceSharedPtr& drawable ) { m_updateDIs.push_back( drawable ); }

    //! Transform Tree access function
    NVSG_API const nvmath::Mat44f& getTransformMatrix( TransformTreeIndex index ) const { return m_transformTree[index].m_worldMatrix; }
    NVSG_API bool isMirrorTransform( TransformTreeIndex index ) const { return m_transformTree.operator[](index).m_worldBits & TransformTreeNode::ISMIRRORTRANSFORM; }

    //! Object Tree access functions
    NVSG_API bool         isActive( ObjectTreeIndex index ) const { return m_objectTree[index].m_worldActive; }
    NVSG_API unsigned int getHints( ObjectTreeIndex index ) const { return m_objectTree[index].m_worldHints; }
    NVSG_API unsigned int getMask( ObjectTreeIndex index ) const  { return m_objectTree[index].m_worldMask; }

    //! Get RenderModeGroup with renderMode. Will add to self if not already there.
    NVSG_API RenderModeGroupSharedPtr getRenderModeGroup( const DrawableInstanceSharedPtr& drawable );
    NVSG_API void remove( const RenderModeGroupSharedPtr &p )
    {
      invalidateBoundingSphere();
      invalidateHints();
      removeChild( p );
    }

    NVSG_API bool needsRebuild();
    NVSG_API void triggerRebuild();
    NVSG_API void setDirty( bool dirty ) { m_dirty = dirty; }
    NVSG_API bool isDirty()              { return m_dirty; }
    NVSG_API void update( const nvsg::ViewStateSharedPtr& ); 

    // TODO documentation.
    // adders - no not the snakes.
    NVSG_API TransformTreeIndex addTransform( TransformTreeNode node, TransformTreeIndex parentIndex, TransformTreeIndex siblingIndex );
    NVSG_API ObjectTreeIndex addObject( ObjectTreeNode node, ObjectTreeIndex parentIndex, ObjectTreeIndex siblingIndex );

    // special function to mark certain transform tree indices as dynamic
    NVSG_API void markTransformDynamic( TransformTreeIndex index );

    // special functions to mark certain object tree indices as certain special nodes
    NVSG_API void addFlipbookAnimation( const nvsg::FlipbookAnimationWeakPtr& fba, ObjectTreeIndex index );
    NVSG_API void addLOD(               const nvsg::LODWeakPtr& lod, ObjectTreeIndex index );
    NVSG_API void addSwitch(            const nvsg::SwitchWeakPtr& s, ObjectTreeIndex index );

    // remove an index and the tree below it from the object tree. removes all referenced DIs, detaches affected objects from
    // the ObjectObserver and removes all affected transforms
    // note: doesnt work for the root node, as this requires a renderlist rebuild
    NVSG_API void removeObjectTreeIndex( ObjectTreeIndex index );

    // remove an index and the tree below it from the object tree.
    // detaches affected transforms from the TransformTraverser
    // note: doesnt work for the root node, as this requires a renderlist rebuild
    NVSG_API void removeTransformTreeIndex( TransformTreeIndex index );

    NVSG_API TransformTree& getTransformTree();
    NVSG_API ObjectTree& getObjectTree();

    NVSG_API TransformTreeNode& getTransformTreeNode( TransformTreeIndex index );
    NVSG_API ObjectTreeNode& getObjectTreeNode( ObjectTreeIndex index );

    NVSG_API const nvmath::Sphere3f & getBoundingSphere();
    NVSG_API void invalidateBoundingSphere();

    NVSG_API unsigned int getHints();
    NVSG_API bool containsHints( unsigned int hints ) { return !!(getHints() & hints); }
    NVSG_API void invalidateHints();

    NVSG_API void lockObservers();
    NVSG_API void unlockObservers();
  protected:
    void updateTransformTree( const nvsg::ViewStateSharedPtr& );
    void updateObjectTree( const nvsg::ViewStateSharedPtr& );

  private:
    bool m_dirty;
    bool m_doRebuild; // trigger renderlist rebuild

    std::vector< DrawableInstanceSharedPtr > m_dirtyDIs;
    std::vector< DrawableInstanceSharedPtr > m_updateDIs;
    std::set   < ObjectTreeIndex >           m_dirtyGeoNodes;

    //TODO0 check switchobserver for shared switches!
    SwitchObserverSharedPtr                  m_switchObserver;
    TransformObserverSharedPtr               m_transformObserver;
    ObjectObserverSharedPtr                  m_objectObserver;
    GeoNodeObserverSharedPtr                 m_geoNodeObserver;
    RootNodeObserverSharedPtr                m_rootNodeObserver;

    // ObserverLocks
    nvutil::ObserverWriteLock*               m_lockSwitchObserver;
    nvutil::ObserverWriteLock*               m_lockTransformObserver;
    nvutil::ObserverWriteLock*               m_lockObjectObserver;
    nvutil::ObserverWriteLock*               m_lockGeoNodeObserver;
    nvutil::ObserverWriteLock*               m_lockRootNodeObserver;

    TransformTree                            m_transformTree;
    TransformTreeIndexSet                    m_dynamicTransformIndices;
     
    ObjectTree                               m_objectTree;
    std::vector< TransformTreeIndex >        m_transformIndexStack; // temp variable
    std::vector< ObjectTreeIndex >           m_objectIndexStack;  // temp variable

    nvmath::Sphere3f m_boundingSphere;
    bool             m_hintsValid;
    unsigned int     m_hints;
  };

  /*===========================================================================*/
  class RenderModeGroup : public RenderListParentClass< MaterialGroupSharedPtr >
    , public RenderListNodeClass
    , public RenderListChildClass< RenderList2WeakPtr >
  {
  protected:
    RenderModeGroup( unsigned int mode )
      : m_renderMode( mode )
      , m_hintsValid( false )
    {}

  public:
    NVSG_API static RenderModeGroupSharedPtr create( unsigned int mode )
    {
      return new RenderModeGroup( mode );
    }

    NVSG_API void setRenderMode( unsigned int mode ) { m_renderMode = mode; }
    NVSG_API unsigned int getRenderMode() const { return m_renderMode; }

    // Get a MaterialGroup with the given material and lightVector. Will add to self if not already there.
    NVSG_API MaterialGroupSharedPtr getMaterialGroup( const DrawableInstanceSharedPtr& drawable );
    NVSG_API void remove( const MaterialGroupSharedPtr &p );

    NVSG_API const nvmath::Sphere3f & getBoundingSphere();
    NVSG_API void invalidateBoundingSphere();

    NVSG_API unsigned int getHints();
    NVSG_API bool containsHints( unsigned int hints ) { return !!(getHints() & hints); }
    NVSG_API void invalidateHints();

  private:
    RenderModeGroup() {};

  private:
    unsigned int      m_renderMode;

    nvmath::Sphere3f  m_boundingSphere;
    bool              m_hintsValid;
    unsigned int      m_hints;

    std::multimap< HashKey, MaterialGroupWeakPtr > m_hashMap;
  };

  /*===========================================================================*/
  class MaterialGroup : public RenderListParentClass< DrawableInstanceSharedPtr >
    , public RenderListNodeClass
    , public RenderListChildClass< RenderModeGroupWeakPtr >
  {
  protected:
    MaterialGroup()
      : m_hintsValid( false )
    {}

  public:  
    NVSG_API static MaterialGroupSharedPtr create()
    {
      return new MaterialGroup;
    }

    NVSG_API void setLightVector( LightVector lightVector )               { m_lightVector = lightVector; }
    NVSG_API void addLightInstance( LightInstanceSharedPtr instance )     { m_lightVector.push_back( instance ); }
    NVSG_API size_t numLightInstances()                                   { return m_lightVector.size(); }
    NVSG_API const LightInstanceSharedPtr& getLightInstance( size_t i )   { return m_lightVector.at(i); } 
    NVSG_API const LightVector& getLightVector()                          { return m_lightVector; }

    NVSG_API void setClipPlaneVector( ClipPlaneVector clipPlaneVector )         { m_clipPlaneVector = clipPlaneVector; }
    NVSG_API void addClipPlaneInstance( ClipPlaneInstanceSharedPtr instance )   { m_clipPlaneVector.push_back( instance ); }
    NVSG_API size_t numClipPlaneInstances()                                     { return m_clipPlaneVector.size(); }
    NVSG_API const ClipPlaneInstanceSharedPtr& getClipPlaneInstance( size_t i ) { return m_clipPlaneVector.at(i); } 
    NVSG_API const ClipPlaneVector& getClipPlaneVector()                        { return m_clipPlaneVector; }

    NVSG_API void setStateVariant( StateVariantSharedPtr stateVariant ) { m_stateVariant = stateVariant; }
    NVSG_API const StateVariantSharedPtr& getStateVariant()             { return m_stateVariant; }

    NVSG_API void addDrawableInstance( const DrawableInstanceSharedPtr &p );
    NVSG_API void remove( const DrawableInstanceSharedPtr &p )
    {
      NVSG_ASSERT( getParent() );

      invalidateBoundingSphere();
      invalidateHints();
      removeChild( p );
      if ( numChildren() == 0 )
      {
        getParent()->remove( this );
      }
    }

    NVSG_API const nvmath::Sphere3f & getBoundingSphere();
    NVSG_API void invalidateBoundingSphere();

    // for faster sorting
    NVSG_API void updateSquaredDistanceToCamera( const nvmath::Mat44f &worldToView )
    {
      const nvmath::Sphere3f& bs( getBoundingSphere() );
      m_squaredDistanceToCamera = lengthSquared(nvmath::Vec4f(bs.getCenter(), 1.0f ) * worldToView);
    }

    NVSG_API float getSquaredDistanceToCamera() const
    {
      return m_squaredDistanceToCamera;
    }

    NVSG_API unsigned int getHints();
    NVSG_API bool containsHints( unsigned int hints ) { return !!(getHints() & hints); }
    NVSG_API void invalidateHints();

    NVSG_API void setHashKey( HashKey hashKey ) { m_hashKey = hashKey; }
    NVSG_API HashKey getHashKey( ) const { return m_hashKey; }

  private:
    // TODO: add clip planes
    LightVector           m_lightVector;
    ClipPlaneVector       m_clipPlaneVector;
    StateVariantSharedPtr m_stateVariant;
    nvmath::Sphere3f      m_boundingSphere;
    float                 m_squaredDistanceToCamera;
    bool                  m_hintsValid;
    unsigned int          m_hints;
    HashKey               m_hashKey;
  };

  /*===========================================================================*/
  class DrawableInstance : public RenderListNodeClass
    , public RenderListChildClass< MaterialGroupWeakPtr >
  {
  protected:
    NVSG_API DrawableInstance();
  public:
    NVSG_API static DrawableInstanceSharedPtr create();

    // access methods for concrete data
    NVSG_API void setGeoNode( const GeoNodeWeakPtr & geoNode );
    NVSG_API const GeoNodeWeakPtr getGeoNode() const                           { return m_geoNode; }

    NVSG_API void setStateSet( const StateSetWeakPtr &sswp );
    NVSG_API const StateSetWeakPtr &getStateSetIterator() const { return( m_stateSet ); }

    NVSG_API void setLightVector( const LightVector& lightVector )             { m_lightVector = lightVector; }
    NVSG_API const LightVector& getLightVector() const                         { return m_lightVector; }

    NVSG_API void setClipPlaneVector( const ClipPlaneVector& clipPlaneVector ) { m_clipPlaneVector = clipPlaneVector; }
    NVSG_API const ClipPlaneVector& getClipPlaneVector() const                 { return m_clipPlaneVector; }

    NVSG_API void setTransformIndex( TransformTreeIndex index )                { m_transformIndex = index; }
    NVSG_API const TransformTreeIndex getTransformIndex() const                { return m_transformIndex; }

    NVSG_API void setObjectTreeIndex( ObjectTreeIndex index )                  { m_objectTreeIndex = index; }
    NVSG_API const ObjectTreeIndex getObjectTreeIndex() const                  { return m_objectTreeIndex; }

    NVSG_API void setDrawable( const DrawableWeakPtr & drawable );
    NVSG_API const DrawableWeakPtr getDrawable() const                         { return m_drawable; }

    // access methods for grouping data
    NVSG_API const unsigned int getRenderMode() const;

    NVSG_API const StateVariantSharedPtr & getMaterial() const;

    NVSG_API void setRenderList2( const RenderList2WeakPtr rl );
    NVSG_API const RenderList2WeakPtr getRenderList()                          { return m_renderList; }

    NVSG_API void remove();

    NVSG_API const nvmath::Sphere3f & getBoundingSphere();
    NVSG_API void invalidateBoundingSphere();

    NVSG_API unsigned int getHints();
    NVSG_API bool containsHints( unsigned int hints ) { return !!(getHints() & hints); }
    NVSG_API void invalidateHints();

    NVSG_API void setActive( bool active ) { m_active = active; }
    NVSG_API bool isActive() const { return m_active; }

    NVSG_API void setMask( unsigned int mask ) { m_mask = mask; }
    NVSG_API unsigned int getMask() const { return m_mask; }

    NVSG_API void setValid( bool valid ) { m_isValid = valid; }
    NVSG_API bool isValid() const { return m_isValid; }
  private:
    // data caches
    nvmath::Sphere3f m_boundingSphere;
    bool             m_active;
    bool             m_hintsValid;
    unsigned int     m_hints;
    unsigned int     m_mask;

    // concrete data
    TransformTreeIndex              m_transformIndex;
    GeoNodeWeakPtr                  m_geoNode;
    StateSetWeakPtr                 m_stateSet;
    LightVector                     m_lightVector;
    ClipPlaneVector                 m_clipPlaneVector;
    ObjectTreeIndex                 m_objectTreeIndex;
    DrawableWeakPtr                 m_drawable;
    bool                            m_isValid;

    // the RenderList2 we're under
    RenderList2WeakPtr m_renderList;
  };

  /*===========================================================================*/
  class LightInstance : public nvutil::RCObject
  {
  protected:
    LightInstance() {}

  public:
    static LightInstanceSharedPtr create() { return new LightInstance; }

  public:
    bool operator<(const LightInstance &rhs) const
    {
      return m_transformIndex < rhs.m_transformIndex || ( m_transformIndex == rhs.m_transformIndex && m_lightSource < rhs.m_lightSource);
    }

    bool operator==(const LightInstance &rhs) const
    {
      return m_transformIndex == rhs.m_transformIndex && m_lightSource == rhs.m_lightSource;
    }

    TransformTreeIndex m_transformIndex;
    LightSourceWeakPtr m_lightSource;
  };

  /*===========================================================================*/
  class ClipPlaneInstance : public nvutil::RCObject
  {
  protected:
    ClipPlaneInstance() {}

  public:
    static ClipPlaneInstanceSharedPtr create() { return new ClipPlaneInstance; }

  public:
    bool operator<(const ClipPlaneInstance &rhs) const
    {
      return m_transformIndex < rhs.m_transformIndex || ( m_transformIndex == rhs.m_transformIndex && m_clipPlane < rhs.m_clipPlane);
    }

    bool operator==(const ClipPlaneInstance &rhs) const
    {
      return m_transformIndex == rhs.m_transformIndex && m_clipPlane == rhs.m_clipPlane;
    }

    TransformTreeIndex m_transformIndex;
    ClipPlaneSharedPtr m_clipPlane;
  };

} // namespace nvsg




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

#include <nvsg/CoreTypes.h>
#include <nvsg/GeoNode.h>
#include <nvutil/TransformStack.h>
#include <deque>

namespace nvsg
{
  class RenderList2;
  class RenderModeGroup;
  class MaterialGroup;
  class DrawableInstance;
  class LightInstance;
  class ClipPlaneInstance;

  typedef nvutil::SmartPtr< RenderList2 >               RenderList2SharedPtr;
  typedef nvutil::SmartPtr< RenderModeGroup >           RenderModeGroupSharedPtr;
  typedef nvutil::SmartPtr< MaterialGroup >             MaterialGroupSharedPtr;
  typedef nvutil::SmartPtr< DrawableInstance >          DrawableInstanceSharedPtr;
  typedef nvutil::SmartPtr< LightInstance >             LightInstanceSharedPtr;
  typedef nvutil::SmartPtr< ClipPlaneInstance >         ClipPlaneInstanceSharedPtr;

  typedef RenderList2*      RenderList2WeakPtr;
  typedef RenderModeGroup*  RenderModeGroupWeakPtr;
  typedef MaterialGroup*    MaterialGroupWeakPtr;
  typedef DrawableInstance* DrawableInstanceWeakPtr;

  typedef size_t TransformTreeIndex;
  typedef size_t ObjectTreeIndex;
  

  template< class NodeClass, class IndexClass >
  class TreeBaseClass
  {
  public:
    // typedefs for traversers
    typedef NodeClass  NodeType;
    typedef IndexClass IndexType;

    TreeBaseClass()
    {
      // initialize object tree with some free indices
      size_t n( 65536 );
      m_tree.resize( n );
      // generate a chain of free objects, leave last index as initialized (~0)
      for( size_t i=0; i<n-1; ++i )
      {
        m_tree[i].m_nextSibling = i+1;
      }
      m_firstFreeIndex = 0;
    }

    IndexClass getFreeNode()
    {
      // check if this is the last free index -> allocate more
      if( m_tree[m_firstFreeIndex].m_nextSibling == ~0 )
      {
        size_t size = m_tree.size();
        IndexClass firstNew = size;

        // resize to factor of old size
        m_tree.resize( checked_cast<size_t>(size * 1.5f) );
        size_t newSize = m_tree.size();

        // generate a new chain of free objects, with the last one pointing to ~0
        m_tree[m_firstFreeIndex].m_nextSibling = firstNew;
        for( size_t i=firstNew; i<newSize-1; ++i )
        {
          m_tree[i].m_nextSibling = i+1;
        }
      }

      IndexClass index = m_firstFreeIndex;
      m_firstFreeIndex = m_tree[m_firstFreeIndex].m_nextSibling;
      return index;
    }

    IndexClass insertNode( NodeClass node, IndexClass parentIndex, IndexClass prevSiblingIndex )
    {
      IndexClass index = getFreeNode();

      // connect node in tree
      node.m_parentIndex = parentIndex;

      if( parentIndex != ~0 )
      {
        // first child below a node
        if ( m_tree[parentIndex].m_firstChild == ~0 )
        {
          m_tree[parentIndex].m_firstChild = index;
        }
        // insert in the beginning of a node
        else if ( prevSiblingIndex == ~0 )
        {
          node.m_nextSibling = m_tree[parentIndex].m_firstChild;
          m_tree[parentIndex].m_firstChild = index;
        }
        // insert between two childs or at the end
        else
        {
          node.m_nextSibling = m_tree[prevSiblingIndex].m_nextSibling;
          m_tree[prevSiblingIndex].m_nextSibling = index;
        }
      }

      // put node into tree
      m_tree[index] = node;

      markDirty( index, ~0 );

      return index;
    }

    void deleteNode( IndexClass index )
    {
      NVSG_ASSERT( index != ~0 );
      NodeClass& node = m_tree[index];

      // sever family ties
      if( node.m_parentIndex != ~0 )
      {
        NodeClass& parent = m_tree[node.m_parentIndex];

        // disconnect node from previous sibling
        IndexClass current = parent.m_firstChild;
        while( current != ~0 )
        {
          if( m_tree[current].m_nextSibling == index )
          {
            // thou art not me brother!
            m_tree[current].m_nextSibling = node.m_nextSibling;
            break;
          }
          current = m_tree[current].m_nextSibling;
        }

        // disconnect node from parent
        if( parent.m_firstChild == index )
        {
          // you are not my child anymore!
          parent.m_firstChild = node.m_nextSibling;
        }

        // I don't have parents!
        node.m_parentIndex = ~0;
      }

      // free node and all nodes below by putting them into the free list
      IndexClass lastIndex = ~0;
      std::deque< ObjectTreeIndex > queue;
      queue.push_back( index );
      while( !queue.empty() )
      {
        // take front node from queue
        IndexClass currentIndex = queue.front();
        NodeClass& current      = m_tree[currentIndex];
        queue.pop_front();

        // insert all children into queue
        IndexClass childIndex = current.m_firstChild;
        while( childIndex != ~0 )
        {
          queue.push_back( childIndex );
          childIndex = m_tree[childIndex].m_nextSibling;
        }

        // disconnect node from parents and children
        current.m_firstChild  = ~0;
        current.m_parentIndex = ~0;

        // put node into free list
        if( lastIndex != ~0 )
        {
          m_tree[lastIndex].m_nextSibling = currentIndex;
        }
        lastIndex = currentIndex;
      }
      m_tree[lastIndex].m_nextSibling = m_firstFreeIndex;
      m_firstFreeIndex = index;
    }

    NodeClass& operator[]( const IndexClass index )
    {
      return m_tree[index];
    }

    const NodeClass& operator[]( const IndexClass index ) const
    {
      return m_tree[index];
    }

    size_t size() const
    {
      return m_tree.size();
    }

    void markDirty( const IndexClass index, unsigned int bits )
    {
      if ( !m_tree[index].m_dirtyBits )
      {
        m_dirtyObjects.push_back( index );
      }
      m_tree[index].m_dirtyBits |= bits;
    }

    IndexClass               m_firstFreeIndex;
    std::vector< NodeClass > m_tree;
    std::vector< IndexClass> m_dirtyObjects;
  };

  class TreeNodeBaseClass
  {
  public:
    TreeNodeBaseClass()
      : m_parentIndex( ~0 )
      , m_nextSibling( ~0 )
      , m_firstChild( ~0 )
      , m_dirtyBits( 0 )
    {}

  public:
    ObjectTreeIndex m_parentIndex; // index of this node's parent
    ObjectTreeIndex m_nextSibling; // index of this node's next sibling (for child iteration)
    ObjectTreeIndex m_firstChild;  // index of this node's first child (for child iteration)
    unsigned int    m_dirtyBits;   // vector of dirty bits (bit definition in derived classes)
  };

  struct TransformTreeNode : public TreeNodeBaseClass
  {
    enum DirtyBits
    {
      DEFAULT_DIRTY = BIT(0) // default dirty flag: update transform and bits
    };

    enum NodeBits
    {
      ISMIRRORTRANSFORM = BIT(0)
    };

    TransformTreeNode()
      : m_transform( nullptr )
      , m_billboard( nullptr )
      , m_localBits( 0 )
      , m_worldBits( 0 )
    {}

    inline void setLocalBits( unsigned int bits, bool value )
    {
      m_localBits = value ? m_localBits |  bits
                          : m_localBits & ~bits;
    }

    inline void setWorldBits( unsigned int bits, bool value )
    {
      m_worldBits = value ? m_worldBits |  bits
                          : m_worldBits & ~bits;
    }
    
    TransformWeakPtr   m_transform;
    BillboardWeakPtr   m_billboard;
    nvmath::Mat44f     m_localMatrix;
    nvmath::Mat44f     m_worldMatrix;
    unsigned int       m_localBits;
    unsigned int       m_worldBits;
    ObjectTreeIndex    m_objectTreeIndex;
  };

  struct ObjectTreeNode : public TreeNodeBaseClass
  {
    enum DirtyBits
    {
      DEFAULT_DIRTY = BIT(0),  // default dirty flag: update hints, mask and active flag
      BV_DIRTY      = BIT(1),  // bounding volume dirty flag: invalidate BVs of DIs found
      GEONODE_DIRTY = BIT(2)   // GeoNode marked dirty for replacement
    };

    ObjectTreeNode()
      : m_object( nullptr )
      , m_di( nullptr )
      , m_localHints( 0 )
      , m_worldHints( 0 )
      , m_localActive( true )
      , m_worldActive( true )
      , m_localMask( ~0 )
      , m_worldMask( ~0 )
    {}
    
    TransformTreeIndex       m_transformIndex; // index of the current parent transform
    ObjectWeakPtr            m_object;         // the node's object in the tree
    DrawableInstanceWeakPtr  m_di;             // the DrawableInstance of this node, if it's object is a drawable
    unsigned int             m_localHints;     // the hints the corresponding node/primitive provides
    unsigned int             m_worldHints;     // the resulting hints for this node
    unsigned int             m_localMask;      // mask of the node's object
    unsigned int             m_worldMask;      // resulting mask
    bool                     m_localActive;    // false iff node is hidden due to Switch/LOD/FBA (can only be child of one at a time -> bool)
    bool                     m_worldActive;    // false iff node is hidden due to !m_localActive or !parent.m_localActive
  };

  class TransformTree : public TreeBaseClass< TransformTreeNode, TransformTreeIndex >
  {};
  typedef std::set< TransformTreeIndex > TransformTreeIndexSet;

  class ObjectTree : public TreeBaseClass< ObjectTreeNode, ObjectTreeIndex >
  {
  public:
    std::map< ObjectTreeIndex, SwitchWeakPtr >            m_switchNodes;
    std::map< ObjectTreeIndex, FlipbookAnimationWeakPtr > m_flipbookAnimations;
    std::map< ObjectTreeIndex, LODWeakPtr >               m_LODs;
  };
  typedef std::set< ObjectTreeIndex >                     ObjectTreeIndexSet;

  typedef nvutil::STLVector<LightInstanceSharedPtr>       LightVector;
  typedef nvutil::STLVector<ClipPlaneInstanceSharedPtr>   ClipPlaneVector;

  class GeneratorState;
  typedef nvutil::SmartPtr< GeneratorState > GeneratorStateSharedPtr;
  class GeneratorState : public nvutil::RCObject
  {
  public:
    NVSG_API static GeneratorStateSharedPtr create( const RenderList2WeakPtr& renderList )
    {
      return new GeneratorState( renderList );
    }

  public:
    // set current environment for transform and object tree
    NVSG_API void setCurrentTransformTreeData( nvsg::TransformTreeIndex parentIndex, nvsg::TransformTreeIndex siblingIndex );
    NVSG_API void setCurrentObjectTreeData( nvsg::ObjectTreeIndex parentIndex, nvsg::ObjectTreeIndex siblingIndex );  

    // build up transform tree
    NVSG_API void pushTransform( const TransformWeakPtr& t );
    NVSG_API void pushTransform( const BillboardWeakPtr& bb );
    NVSG_API void popTransform();
    NVSG_API TransformTreeIndex getParentTransformIndex() const;
    NVSG_API TransformTreeIndex getSiblingTransformIndex() const;

    NVSG_API void addDynamicTransformIndex( TransformTreeIndex index );

    // build up node tree
    NVSG_API void pushObject( const GroupWeakPtr& g );
    NVSG_API void pushObject( const FlipbookAnimationWeakPtr& fba );
    NVSG_API void pushObject( const LODWeakPtr& lod );
    NVSG_API void pushObject( const SwitchWeakPtr& s );
    NVSG_API void pushObject( const GeoNodeWeakPtr& gn );
    NVSG_API void popObject();    
    // add primitive as leaf under node tree, returns the index of the primitive
    NVSG_API ObjectTreeIndex addPrimitive( const PrimitiveWeakPtr& p, const DrawableInstanceWeakPtr& di );
    // data access functions
    NVSG_API ObjectTreeIndex getParentObjectIndex() const;
    NVSG_API ObjectTreeIndex getSiblingObjectIndex() const;
        
    NVSG_API void pushLightSet();
    NVSG_API void popLightSet();
    NVSG_API void addLight( const LightInstanceSharedPtr& l );
    NVSG_API size_t getNumberOfLights() const;
    NVSG_API const LightInstanceSharedPtr& getLight( unsigned int i ) const;
    NVSG_API const LightVector& getLightVector();

    NVSG_API void pushClipPlaneSet();
    NVSG_API void popClipPlaneSet();
    NVSG_API void addClipPlane( const ClipPlaneInstanceSharedPtr& l );
    NVSG_API size_t getNumberOfClipPlanes() const;
    NVSG_API const ClipPlaneInstanceSharedPtr& getClipPlane( unsigned int i ) const;
    NVSG_API const ClipPlaneVector& getClipPlaneVector();

    // TODO: remove this API, redundant due to ObjectTree
    NVSG_API void setGeoNode( const GeoNodeWeakPtr & g );
    NVSG_API const GeoNodeWeakPtr& getGeoNode() const;

    NVSG_API void setStateSetIterator( const GeoNode::StateSetConstIterator& ssci );
    NVSG_API GeoNode::StateSetConstIterator getStateSetIterator() const;

    NVSG_API void setMaterial( const StateVariantWeakPtr& s );
    NVSG_API const StateVariantWeakPtr& getMaterial() const;

  protected:
    friend struct nvutil::Holder<GeneratorState>;

    GeneratorState( const RenderList2WeakPtr& renderList );

  private:
    typedef std::stack< std::pair< TransformTreeIndex, TransformTreeIndex> > TransformParentSiblingStack;
    typedef std::stack< std::pair< ObjectTreeIndex,    ObjectTreeIndex > >   ObjectParentSiblingStack;

  private:
    ObjectTreeIndex insertNode( const ObjectWeakPtr& g );

  private:
    RenderList2WeakPtr             m_renderList;

    TransformParentSiblingStack    m_transformParentSiblingStack; // A stack of indices of the current parent and sibling nodes
    ObjectParentSiblingStack       m_objectParentSiblingStack;    // A stack of indices of the current parent and sibling nodes

    struct LightVectorStackEntry
    {
      LightVectorStackEntry( )
        : m_sorted( false )
      {
      }

      LightVectorStackEntry( const LightVectorStackEntry &rhs )
        : m_lightVector( rhs.m_lightVector )
        , m_sorted( rhs.m_sorted )
      {
      }

      LightVector m_lightVector;
      bool        m_sorted;
    };

    struct ClipPlaneVectorStackEntry
    {
      ClipPlaneVectorStackEntry( )
        : m_sorted( false )
      {
      }

      ClipPlaneVectorStackEntry( const ClipPlaneVectorStackEntry &rhs )
        : m_clipPlaneVector( rhs.m_clipPlaneVector )
        , m_sorted( rhs.m_sorted )
      {
      }

      ClipPlaneVector m_clipPlaneVector;
      bool            m_sorted;
    };

    //TODO0 initialize these two before inserting
    std::vector< LightVectorStackEntry >     m_lightVector;                 // A vector of vectors with sort status since an arbitrary amount can be added per Group
    std::vector< ClipPlaneVectorStackEntry > m_clipPlaneVector;             // A vector of vectors with sort status since an arbitrary amount can be added per Group

    StateVariantWeakPtr            m_material;                    // Current material
    GeoNodeWeakPtr                 m_geoNode;                     // The current GeoNode we're under
    GeoNode::StateSetConstIterator m_stateSetIterator;            // The iterator of the current StateSet in the GeoNode
  };

} // namespace nvtraverser

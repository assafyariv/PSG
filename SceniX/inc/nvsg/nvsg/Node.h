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
#include "nvsg/OwnedObject.h"
#include "nvsg/Drawable.h"
#include "nvsg/StateSet.h"
#include "nvmath/Boxnt.h"
#include "nvmath/Spherent.h"
#include "nvutil/BitMask.h"
#include <vector>

namespace nvsg
{
/*! \brief Serves as base class for all tree nodes
  * \par Namespace: nvsg
  */
class Node : public OwnedObject<Group>
{
  public:
    /*! \brief Default-constructs a Node.
      */
    NVSG_API Node();

    /*! \brief Constructs a Node as a copy of another Node.
      */
    NVSG_API Node(const Node& rhs);

    /*! \brief Destructs a Node. 
      */
    NVSG_API virtual ~Node();

    using Object::getBoundingBox;
    using Object::getBoundingSphere;

    /*! \brief Returns the number of parent Group nodes for this Node.
      * \returns The actual number of Group nodes that have this Node as child node. */
    NVSG_API size_t getNumberOfParents() const;

    REFLECTION_INFO_API( NVSG_API, Node );
  protected:
    NVSG_API virtual const nvmath::Box3f & getBoundingBox( bool recalculate ) const;
    NVSG_API virtual const nvmath::Sphere3f & getBoundingSphere( bool recalculate ) const;
    NVSG_API virtual void incrementHintsIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryHintsIncarnation() const;
    NVSG_API virtual void incrementIndexSetIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryIndexSetIncarnation() const;
    NVSG_API virtual void incrementMaterialIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryMaterialIncarnation() const;
    NVSG_API virtual void incrementPrimitiveIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryPrimitiveIncarnation() const;
    NVSG_API virtual void incrementVertexAttributeSetIncarnation() const;
    NVSG_API virtual const nvutil::Incarnation & queryVertexAttributeSetIncarnation() const;

    /*! \brief Assigns new content from another Node. 
      * \param
      * rhs Reference to a Node from which to assign the new content.
      * \return
      * A reference to this object.
      * \remarks
      * The assignment operator unreferences the old content before assigning the new content. The new
      * content will be a deep-copy of the content of right-hand-sided object. 
      */
    NVSG_API Node & operator=(const Node & rhs);

  private:
    mutable nvmath::Box3f       m_boundingBox;
    mutable nvmath::Sphere3f    m_boundingSphere;
    mutable nvutil::Incarnation m_hintsIncarnation; // tracks unspecified hints changes
    mutable nvutil::Incarnation m_indexSetIncarnation; // tracks unspecified indexset changes
    mutable nvutil::Incarnation m_materialIncarnation; // tracks unspecified material changes
    mutable nvutil::Incarnation m_primitiveIncarnation; // tracks unspecified primitive changes
    mutable nvutil::Incarnation m_vertexAttributeSetIncarnation; // tracks unspecified vertexattributeset changes
};

//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

inline size_t Node::getNumberOfParents() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  return getNumberOfOwners();
}

inline void Node::incrementIndexSetIncarnation() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
  m_indexSetIncarnation++;
}

inline const nvutil::Incarnation & Node::queryHintsIncarnation() const
{
  return( m_hintsIncarnation );
}

inline void Node::incrementHintsIncarnation() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
  m_hintsIncarnation++;
}

inline const nvutil::Incarnation & Node::queryIndexSetIncarnation() const
{
  return( m_indexSetIncarnation );
}

inline void Node::incrementMaterialIncarnation() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
  m_materialIncarnation++;
}

inline const nvutil::Incarnation & Node::queryMaterialIncarnation() const
{
  return( m_materialIncarnation );
}

inline void Node::incrementPrimitiveIncarnation() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
  m_primitiveIncarnation++;
}

inline const nvutil::Incarnation & Node::queryPrimitiveIncarnation() const
{
  return( m_primitiveIncarnation );
}

inline void Node::incrementVertexAttributeSetIncarnation() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
  m_vertexAttributeSetIncarnation++;
}

inline const nvutil::Incarnation & Node::queryVertexAttributeSetIncarnation() const
{
  return( m_vertexAttributeSetIncarnation );
}


// convenient helper

/*! \brief Tests whether the bounding sphere of the specified Node is valid.
  * \param node The Node to check.
  * \return \c true if the bounding sphere is valid, \c false otherwise.
  * \remarks
  * The function temporary read-locks the passed Node to query its bounding sphere. 
  */
inline bool isValidBoundingSphere( const NodeSharedPtr & node )
{
  NVSG_TRACE();
  return nvmath::isValid(NodeReadLock(node)->getBoundingSphere());
}

/*! \brief Returns a copy of the specified Node's bounding sphere.
  * \param node The Node to get the bounding sphere from.
  * \return A copy of the node's bounding sphere.
  * \remarks
  * The function temporary read-locks the passed Node to access its bounding sphere. 
  */
inline nvmath::Sphere3f getBoundingSphere( const NodeSharedPtr & node )
{
  NVSG_TRACE();
  return NodeReadLock(node)->getBoundingSphere();
}

} // namespace

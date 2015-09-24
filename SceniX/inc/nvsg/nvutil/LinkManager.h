// Copyright NVIDIA Corporation 2009
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

#include <list>
#include <set>
#include <map>

#include "Reflection.h"

#include "nvsg/CoreTypes.h"
#include "nvsg/Object.h"

namespace nvutil {

  struct Link {
    NVSG_API Link( const nvsg::ObjectWeakPtr & srcObject, PropertyId srcProperty, const nvsg::ObjectWeakPtr & dstObject, PropertyId  dstProperty)
      : m_srcObject(srcObject)
      , m_srcProperty(srcProperty)
      , m_dstObject(dstObject)
      , m_dstProperty(dstProperty)
      {}

    NVSG_API virtual ~Link() {}

    PropertyId  m_srcProperty;
    PropertyId  m_dstProperty;
    nvsg::ObjectWeakPtr m_srcObject;
    nvsg::ObjectWeakPtr m_dstObject;

    virtual void sync() const = 0;
  };

  typedef const Link* LinkId;

  template <typename T>
  struct LinkImpl : public Link {
    NVSG_API LinkImpl( const nvsg::ObjectWeakPtr & srcObject, PropertyId srcProperty, const nvsg::ObjectWeakPtr & dstObject, PropertyId  dstProperty)
    : Link(srcObject, srcProperty, dstObject, dstProperty)
    {
    }

    NVSG_API virtual ~LinkImpl() {}

    NVSG_API virtual void sync() const
    {
      nvsg::ObjectReadLock srcObjectLock(m_srcObject);
      nvsg::ObjectWriteLock dstObjectLock(m_dstObject);
      dstObjectLock->setValue<T>(m_dstProperty, srcObjectLock->getValue<T>(m_srcProperty));
    }
  };

  struct LinkInfo {

    NVSG_API LinkInfo( const nvsg::ObjectWeakPtr & object ) // src reflection object
      : m_reflection(object)
      {
      }

    NVSG_API void addLink( LinkId link );
    NVSG_API void removeLink( LinkId link );

    NVSG_API bool isTopLevelObject() {
      return m_incomingLinks.empty();
    }

    NVSG_API nvsg::ObjectWeakPtr getObject()
    {
      return m_reflection;
    }

    NVSG_API bool isEmpty()
    {
      return m_outgoingLinks.empty() && m_incomingLinks.empty();
    }
  //protected:
    nvsg::ObjectWeakPtr         m_reflection;
    std::list<const Link *>     m_outgoingLinks; // outgoing links
    std::list<const Link *>     m_incomingLinks; // incoming links
  };

  typedef std::map<nvsg::ObjectWeakPtr, LinkInfo> LinkInfoMap;
  typedef std::set<nvsg::ObjectWeakPtr> CalcList;
  typedef std::set<nvsg::ObjectWeakPtr> ReflectionSet;

  struct LinkRuntime;

  class LinkManager {
  public:
    NVSG_API LinkManager();
    NVSG_API ~LinkManager();

    // link with known type -> runtime check for correct type
    template <typename T> LinkId link( const nvsg::ObjectWeakPtr & srcObject, PropertyId srcProperty, const nvsg::ObjectWeakPtr & dstObject, PropertyId dstProperty);
    template <typename T> LinkId link( const nvsg::ObjectWeakPtr & srcObject, const char *srcProperty, const nvsg::ObjectWeakPtr & dstObject, const char *dstProperty);

    // link with unknown type -> runtime check for same type on both src and dst
    NVSG_API LinkId link( const nvsg::ObjectWeakPtr & srcObject, PropertyId srcProperty, const nvsg::ObjectWeakPtr & dstObject, PropertyId  dstProperty);
    NVSG_API LinkId link( const nvsg::ObjectWeakPtr & srcObject, const char *srcProperty, const nvsg::ObjectWeakPtr & dstObject, const char *dstProperty);

    NVSG_API void unlink(LinkId link);

    /** unlink all incoming and outgoing links of this object **/
    NVSG_API void unlink( const nvsg::ObjectWeakPtr & object );

    NVSG_API bool processLinks();

  protected:

    /** Update processing order of objects if necessary **/
    bool updateLinkOrder();

    void storeLink(LinkId link);

    bool isSourceCalculationDone( const nvsg::ObjectWeakPtr & object );
    bool isGathered( const nvsg::ObjectWeakPtr & object ); // has been added for ordering
    bool isOrdered( const nvsg::ObjectWeakPtr & object ); // has been ordered

    void gather( const nvsg::ObjectWeakPtr & object )
    {
      m_gatheredObjects.insert(object);
    }

    std::list<nvsg::ObjectWeakPtr> getTopLevelObjects();

    LinkInfoMap linkInfos;

    ReflectionSet m_gatheredObjects; // objects which have been gathered, but not been place into the ordered list
    ReflectionSet m_orderedObjects; // objects which have been places in the ordered lsit

    std::vector<nvsg::ObjectWeakPtr> m_objectOrder; // objects sorted by dependencies

    bool m_linkOrderDone;
    bool m_cycleDetected;
  };

  template <typename T>
  LinkId LinkManager::link( const nvsg::ObjectWeakPtr & srcObject, PropertyId srcProperty, const nvsg::ObjectWeakPtr & dstObject, PropertyId  dstProperty )
  {
    nvsg::ObjectReadLock srcObjectLock(srcObject);
    nvsg::ObjectReadLock dstObjectLock(dstObject);

    NVSG_ASSERT( srcObjectLock->getPropertyType(srcProperty) == static_cast<Property::Type>(TypedPropertyEnum<T>::type) );
    NVSG_ASSERT( dstObjectLock->getPropertyType(dstProperty) == static_cast<Property::Type>(TypedPropertyEnum<T>::type) );

    LinkId link = new LinkImpl<T>(srcObjectLock, srcProperty, dstObjectLock, dstProperty);
    storeLink(link);
    return link;
  }

  template <typename T>
  LinkId LinkManager::link( const nvsg::ObjectWeakPtr & srcObject, const char *srcProperty, const nvsg::ObjectWeakPtr & dstObject, const char *dstProperty )
  {
    nvsg::ObjectReadLock srcObjectLock(srcObject);
    nvsg::ObjectReadLock dstObjectLock(dstObject);

    return link<T>(srcObjectLock, srcObjectLock->getProperty(srcProperty), dstObjectLock, dstObjectLock->getProperty(dstProperty));
  }

}

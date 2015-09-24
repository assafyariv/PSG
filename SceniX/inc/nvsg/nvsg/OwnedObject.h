// Copyright NVIDIA Corporation 2002-2007
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
/** @file */

#include "nvsg/Object.h" // base class definition
#include "nvutil/Handle.h"
#include "nvutil/STLAllocator.h"
#include <vector>

namespace nvsg
{
  /*! \brief Helper class to hold a vector of owners of an Object.
   *  \remarks Some classes, like Drawable, LightSource, or StateAttribute, need to know all the
   *  objects that "own" them, i.e. the objects that are referencing them in the tree hierarchy.
   *  This knowledge is needed to carry information from such objects that are not derived from Node
   *  "up-tree" to the Nodes that hold them. That is, the "owner-relationship" is similar to the
   *  "parent-relationship" of the Nodes. Each OwnedObject can only be owned by one specific type of
   *  owners, specified as the template parameter "OwnerType".
   *  \sa Node */
  template<typename OwnerType>
  class OwnedObject : public Object
  {
    template <typename T, typename U> friend void addAsOwnerTo(T *, const nvutil::SmartPtr<U> &);
    template <typename T, typename U> friend void removeAsOwnerFrom(T *, const nvutil::SmartPtr<U> &);

  public:
    typedef nvutil::STLMultiset<typename nvutil::ObjectTraits<OwnerType>::WeakPtr>  OwnerContainer;
    typedef typename OwnerContainer::const_iterator                                 OwnerIterator;

    public:
      /*! \brief Get the number of owners of this OwnedObject.
       *  \return Number of owners of this OwnedObject.
       *  \remarks An OwnedObject that is part of one or more OwnerType objects holds a pointer
       *  back to each of those OwnerType objects. These are called the owners. So the number
       *  of owners tells you how often this OwnedObject is referenced in a scene tree.
       *  \sa getOwner */
      size_t getNumberOfOwners() const;

      /*! \brief Get the owning OwnerType at position \a index.
       *  \param index Position of the OwnerType to get in the list of owners.
       *  \return The OwnerType that owns this OwnedObject.
       *  \remarks An OwnedObject that is part of one or more OwnerType objects, holds a
       *  pointer back to each of those OnwerType objects. These are called the owners.
       *  \note The behavior is undefined if \a index is greater than or equal to the number of
       *  owners.
       *  \sa getNumberOfOwners */
      typename nvutil::ObjectTraits<OwnerType>::WeakPtr getOwner( OwnerIterator ) const;
      OwnerIterator ownersBegin() const { return m_owners.begin(); }
      OwnerIterator ownersEnd() const { return m_owners.end(); }

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant OwnedObject to copy from
       *  \return A reference to the assigned OwnedObject
       *  \remarks The assignment operator calls the assignment operator of Object.
       *  \note The owners of rhs are not copied.
       *  \sa Object */
      OwnedObject<OwnerType>& operator=(const OwnedObject<OwnerType> & rhs);

      virtual void notify( const Subject * originator, unsigned int state ) const;
      virtual void notify( nvutil::PropertyId propertyId ) const;

    protected:

      /*! \brief Protected default constructor to prevent instantiation of an OwnedObject.
       *  \remarks An OwnedObject is not intended to be instantiated, but only classes derived from
       *  it. */
      OwnedObject();

      /*! \brief Protected copy constructor from an Object.
       *  \remarks An OwnedObject is not intended to be instantiated, but only classes derived from
       *  it. */
      OwnedObject( const Object & rhs );

      /*! \brief Protected copy constructor from an OwnedObject.
       *  \remarks An OwnedObject is not intended to be instantiated, but only classes derived from
       *  it. */
      OwnedObject( const OwnedObject<OwnerType> &rhs );

      /*! \brief Protected destructor of an OwnedObject.
       *  \remarks An OwnedObject is not intended to be instantiated, but only classes derived from
       *  it. */
      ~OwnedObject();

      void addOwner( OwnerType * owner);
      void removeOwner( OwnerType * owner);

    private:
      OwnerContainer  m_owners;
  };

  template<typename OwnerType>
  OwnedObject<OwnerType>::OwnedObject()
  : Object()
  {
  }

  template<typename OwnerType>
  OwnedObject<OwnerType>::OwnedObject( const Object &rhs )
  : Object(rhs)
  {
    // must not copy owners!!!
    // this is what this implementation of the cpy ctor is for!
    NVSG_ASSERT( m_owners.empty() );
  }

  template<typename OwnerType>
  OwnedObject<OwnerType>::OwnedObject( const OwnedObject<OwnerType> &rhs )
  : Object(rhs)
  {
    // must not copy owners!!!
    // this is what this implementation of the cpy ctor is for!
    NVSG_ASSERT( m_owners.empty() );
  }

  template<typename OwnerType>
  OwnedObject<OwnerType>::~OwnedObject()
  {
    // if this fires, there was no proper cleanup of owner relationship elsewhere
    NVSG_ASSERT( m_owners.empty() );
  }

  template<typename OwnerType>
  inline size_t OwnedObject<OwnerType>::getNumberOfOwners( void ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_owners.size() );
  }

  template<typename OwnerType>
  inline typename nvutil::ObjectTraits<OwnerType>::WeakPtr OwnedObject<OwnerType>::getOwner( OwnerIterator i ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( *i );
  }

  template<typename OwnerType>
  inline OwnedObject<OwnerType> & OwnedObject<OwnerType>::operator=( const OwnedObject<OwnerType> &rhs )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    Object::operator=( rhs );
    return( *this );
  }

  template<typename OwnerType>
  inline void OwnedObject<OwnerType>::addOwner( OwnerType * obj )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    // do not add an owner twice
    m_owners.insert( nvutil::getWeakPtr<OwnerType>(obj) );
  }

  template<typename OwnerType>
  inline void OwnedObject<OwnerType>::removeOwner( OwnerType * obj )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);

    m_owners.erase( nvutil::getWeakPtr<OwnerType>(obj) );
  }

  template<typename OwnerType>
  inline void OwnedObject<OwnerType>::notify( const Subject * originator, unsigned int state ) const
  {
    Object::notify( originator, state );
    for ( OwnerIterator oi = m_owners.begin() ; oi != m_owners.end() ; ++oi )
    {
      ObjectReadLock( ObjectSharedPtr( *oi ) )->notifyChange( this, state );
    }
  }

  template<typename OwnerType>
  inline void OwnedObject<OwnerType>::notify( nvutil::PropertyId propertyId ) const
  {
    Object::notify( propertyId );
    for ( OwnerIterator oi = m_owners.begin() ; oi != m_owners.end() ; ++oi )
    {
      ObjectReadLock( ObjectSharedPtr( *oi ) )->notify( propertyId );
    }
  }

  template<typename OwnerType, typename OwnedHandleType>
  inline void addAsOwnerTo(OwnerType * owner, const nvutil::SmartPtr<OwnedHandleType> & owned)
  {
    if ( owned )
    {
      nvutil::WritableObject<typename OwnedHandleType::ObjectType>(owned)->addOwner( owner );
    }
  }

  template<typename OwnerType, typename OwnedHandleType>
  inline void removeAsOwnerFrom(OwnerType * owner, const nvutil::SmartPtr<OwnedHandleType> & owned)
  {
    if ( owned )
    {
      nvutil::WritableObject<typename OwnedHandleType::ObjectType>(owned)->removeOwner( owner );
    }
  }

  template<typename OwnerType>
  inline void copy( typename nvutil::ObjectTraits<OwnedObject<OwnerType> >::WriteLock & dst
                  , typename nvutil::ObjectTraits<OwnedObject<OwnerType> >::ReadLock & src )
  {
  }

} // namespace nvsg

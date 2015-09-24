// Copyright NVIDIA Corporation 2002-2005
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

// rcobject.h
//
// Support for reference counted object
//
#pragma once
/** \file */
#ifndef __RC_OBBBB
#define __RC_OBBBB

#include "nvsgcommon.h"

#include "nvutil/Allocator.h" // for IAllocator interface
#include "nvutil/SWMRSync.h"

namespace nvutil
{
  class RCObject;
  template<typename T> class SmartPtr;

#if !defined(DOXYGEN_IGNORE)
  // RefCount abstracts a reference counter and provides mutual exclusive access to the internal counter.
  template <typename T>
  class RefCount
  {
  public:
    // initializes the internal counter with the value of the passed 'initialValue' parameter. 
    // initializes the internal counter with 0, if the parameter is omitted.
    // the mutex object will always be default constructed (see SWMRSync::SWMRSync)
    RefCount(T initialValue = 0) : m_cnt(initialValue) {}

    // increments (increases by one) the internal counter.
    // this operator prevents more than one thread from accessing the internal counter simultaneously.
    T operator++() 
    {
      m_mutex.lockExclusive();
      T res = ++m_cnt;
      m_mutex.unlockExclusive();
      return res;
    }

    // decrements (decreases by one) the internal counter.
    // this operator prevents more than one thread from accessing the internal counter simultaneously.
    T operator--()
    {
      m_mutex.lockExclusive();
      NVSG_ASSERT(m_cnt>0); // must never turn negative
      T res = --m_cnt;
      m_mutex.unlockExclusive();
      return res;
    }

    // 'greater than' comparison of the internal counter.
    // this operator prevents more than one thread from accessing the internal counter simultaneously.
    bool operator>(const T& rhs) const
    {
      m_mutex.lockExclusive();
      bool res = m_cnt > rhs;
      m_mutex.unlockExclusive();
      return res;
    }

    // hide the built-in assignment operator!
    // the built-in assignment operator would assign the mutex, which is not allowed!
    // this operator prevents more than one thread from accessing the internal counter simultaneously.
    RefCount& operator=(const RefCount &rhs)
    {
      if ( this != &rhs )
      {
        // never copy a mutex! keep the original one instead!
        m_mutex.lockExclusive();
        m_cnt = rhs.m_cnt;
        m_mutex.unlockExclusive();
      }
      return *this;
    }

  private:
    SWMRSync m_mutex; // to realize mutual exclusive access to m_cnt
    T m_cnt; // holds the counter value 
  };
#endif

  /*! \brief Type definition for the callback on destruction.
   *  \par Namespace: nvsg
   *  \remarks An RCObject can have callbacks to be called, just before it is destroyed. This is
   *  the definition of the interface of these callbacks. The complete \a rcObject can be read at
   *  that moment, but not written.
   *  \note The behavior is undefined, if \a rcObject is written to.
   *  \sa RCObject::addOnDestroyCallback, RCObject::removeOnDestroyCallback */
  typedef void DestroyCallback( const RCObject * rcObject, void * data );

  /*! \brief Provides an interface for reference counted objects. 
    * \par Namespace: nvutil
    * \remarks
    * The class provides a protected interface for managing reference counted objects. RCObject inherits
    * from IAllocator to utilize an optimized heap manager for small object allocation.
    * \n\n
    * \note An RCObject can only be constructed on heap. The compiler will reject any
    * attempt to construct an RCObject on stack.
    * \note It is prohibited to explicitly delete an RCObject by calling \c delete on a pointer
    * to an RCObject received from a previous call to \c new. The compiler will complain if any 
    * client code attempts to do so. If client code creates an RCObject, it must pass it to a SmartPtr. That
    * SmartPtr then manages the reference count of this RCObject.
    * \note The reference count of a newly created RCObject is initially zero.
    * \sa nvutil::SmartPtr
    */
  class RCObject : public IAllocator
  {
    template<typename T> friend class SmartPtr;

    public:
      /*! \brief Marks the object as not shareable.
        * \remarks
        * The function marks the object as not shareable. The shareable flag of an RCObject will be 
        * evaluated if a managing SmartPtr (reference counter pointer) will be copied. If the managed 
        * RCObject is flagged as shareable (default), only the reference count of the RCObject will
        * be incremented, indicating that two or more SmartPtr objects share the same RCObject. The
        * managed RCObject will be copied not before the first write access (lazy copying). \n
        * If instead the RCObject is flagged as not shareable, the managed RCObject will be copied 
        * as the managing SmartPtr object will be copied (immediate copying).
        * \sa RCObject::isShareable
        */
      NVSG_API void markUnshareable();
      
      /*! \brief Tests whether this particular object is flagged shareable.
        * \return
        * \c true if shareable, \c false otherwise.
        * \remarks
        * Marking an RCObject as shareable enables lazy copying of the RCObject if managed by a
        * SmartPtr (reference counted pointer) object. That is, a SmartPtr object can share its 
        * referenced RCObject with other SmartPtr objects as long as no write access happens.
        * \sa RCObject::markUnshareable
        */
      NVSG_API bool isShareable()       const; 

      /*! \brief Tests whether this RCobject is shared.
        * \return
        * \c true if the internal reference count is greater than one, \c false otherwise.
        * \remarks
        */
      NVSG_API bool isShared()          const; 

      /*! \brief Add a callback to be called on destruction.
       *  \param dcb A pointer to the DestroyCallback to add.
       *  \param data A pointer to be used as the second argument of the DestroyCallback.
       *  \note The behavior is undefined, if the same combination of DestroyCallback and void *
       *  is added more than once.
       *  \par Example
       *  \code
       *    ObjectWriteLock(myObject)->addOnDestroyCallback( myDestroyCallback, NULL );
       *  \endcode
       *  \sa DestroyCallback, removeOnDestroyCallback */
      NVSG_API void addOnDestroyCallback( DestroyCallback *dcb, void * data );

      /*! \brief Remove a callback from the list of callbacks called on destruction.
       *  \param dcb A pointer to the DestroyCallback to remove.
       *  \param data The pointer that is used as the data pointer in the corresponding
       *  addOnDestroyCallback.
       *  \note The behavior is undefined if the combination of \a dcb and \a data is not
       *  registered as a callback on destruction via addOnDestroyCallback.
       *  \par Example
       *  \code
       *    ObjectWriteLock(myObject)->removeOnDestroyCallback( myDestroyCallback, NULL );
       *  \endcode
       *  \sa DestroyCallback, addOnDestroyCallback */
      NVSG_API void removeOnDestroyCallback( DestroyCallback * dcb, void * data );

    protected:
      /*! \brief Increments the object's reference count.
        * \remarks
        * For user code to ensure that the data of a reference counted object is valid as long as it
        * uses the object, it should first increment the objects reference count. After usage, 
        * client code should decrement the objects reference count to avoid resource leaks.
        *
        * \note The reference count of a newly created RCObject initially is zero.
        * \sa RCObject::removeRef
        */
      NVSG_API void addRef()            const;

      /*! \brief Decrements the object's reference count. 
        * \remarks
        * It is prohibited to explicitly delete an RCObject. 
        * The object will be automatically deleted, if the reference count changes from one to zero.
        * \sa RCObject::addRef
        */
      NVSG_API void removeRef()         const;

      /*! \brief Constructs an RCObject.
        * \remarks
        * Initializes the reference count to zero. The creator is responsible for setting the
        * reference count to its proper value.
        * \n\n
        * Initially the RCObject is marked as shareable.
        * \n\n
        * An RCObject is intended to serve as base class only. That is, an RCObject can be 
        * instantiated only as an object of a class derived from RCObject.
        */
      RCObject();

      /*! \brief Constructs an RCObject.
        * \remarks
        * A new RCObject is created, and hence, the reference count will be initialized to zero 
        * because no one but the creator is currently referencing this object, and the
        * creator is responsible for setting the reference count to its proper value.
        * \n\n
        * Initially the RCObject is marked as shareable.
        * \n\n
        * An RCObject is intended to serve as base class only. That is, an RCObject can be 
        * instantiated only as an object of a class derived from RCObject.
        */
      RCObject(const RCObject& rhs);

      /*! \brief Assignment operator.
       * \remarks
       * Assigning new content from another object does not change the number of users referencing
       * the object nor does it change its shareable state. Hence, this assignment operator leaves
       * the reference count and the shareable state unchanged.
       */
      RCObject& operator=(const RCObject& rhs) { return *this; }

      /*! \brief Destructs an RCObject.
        * \remarks
        * An RCObject will be automatically deleted when its reference count changes from one to 
        * zero.
        * \sa RCObject::removeRef
        */
      virtual ~RCObject();

    private:
      typedef std::vector<std::pair<DestroyCallback *,void *> > DestroyCallbackContainer;

    private:
      /** m_refcnt must be mutable, so we can also call addRef() / removeRef() from const pointers. */
      mutable RefCount<int>       m_refcnt;
      bool                        m_shareable;
      DestroyCallbackContainer  * m_destroyCallbacks;
  };

  typedef nvutil::SmartPtr< RCObject > SmartRCObject; //!< Type definition for a SmartPtr of type RCObject

  /*! \brief A reference counted vector.
    * \par Namespace : nvutil
    * \remarks
    * A RCVector is simply a STL vector providing the RCObject interface. A RCVector, once 
    * constructed using the limited set of available constructors, can be used same as a STL vector. 
    */
  template<typename T>
  class RCVector : public std::vector<T>, public RCObject
  {
    public:
      /*! \brief Constructs an initially empty RCVector
       */
      RCVector() {}

      /*! \brief Copy constructor of an RCVector.
       *  \param rhs The RCVector to copy from. */
      RCVector( const RCVector<T> & rhs ) : std::vector<T>(rhs) {}

      /*! \brief Constructs a RCVector of the specified size.
        * \param
        * n Number of elements in the constructed vector.
        * \remarks
        * The constructor specifies a repetition of the specified number \a n of elements of the 
        * default value for type T.
        * \sa std::vector
        */
      explicit RCVector(unsigned int n) : std::vector<T>(n) {}
  };

  //----------------------------------------------------------------------------
  // inline implementations following
  //----------------------------------------------------------------------------

  inline RCObject::RCObject()
    : m_destroyCallbacks(nullptr)
    , m_shareable(true)
  {
  }

  inline RCObject::RCObject( const RCObject & rhs )
    : m_destroyCallbacks(nullptr)
    , m_shareable(true)
  {
  }

  inline RCObject::~RCObject()
  {
    delete m_destroyCallbacks;
  }

  inline void RCObject::addRef() const 
  { 
    ++m_refcnt; 
  } 

 inline  void RCObject::removeRef() const
  { 
    if ( !--m_refcnt ) 
    {
      if ( m_destroyCallbacks )
      {
        for ( size_t i=0 ; i<m_destroyCallbacks->size() ; i++ )
        {
          (*(*m_destroyCallbacks)[i].first)( this, (*m_destroyCallbacks)[i].second );
        }
      }
      delete this; 
    } 
  }

  inline void RCObject::markUnshareable() 
  { 
    m_shareable=false; 
  }

  inline bool RCObject::isShareable() const 
  { 
    return m_shareable==true; 
  }

  inline bool RCObject::isShared() const 
  { 
    return m_refcnt>1;
  }

  inline void RCObject::addOnDestroyCallback( DestroyCallback * dcb, void * data )
  {
    NVSG_ASSERT( !m_destroyCallbacks
              || (  std::find( m_destroyCallbacks->begin(), m_destroyCallbacks->end(), std::make_pair( dcb, data ) )
                    == m_destroyCallbacks->end() ) );
    if ( !m_destroyCallbacks )
    {
      m_destroyCallbacks = new DestroyCallbackContainer;
    }
    m_destroyCallbacks->push_back( std::make_pair( dcb, data ) );
  }

  inline void RCObject::removeOnDestroyCallback( DestroyCallback * dcb, void * data )
  {
    if ( m_destroyCallbacks )
    {
      DestroyCallbackContainer::iterator it = std::find( m_destroyCallbacks->begin(), m_destroyCallbacks->end(), std::make_pair( dcb, data ) );
      NVSG_ASSERT( it != m_destroyCallbacks->end() );
      m_destroyCallbacks->erase( it );
      if ( m_destroyCallbacks->empty() )
      {
        delete m_destroyCallbacks;
        m_destroyCallbacks = nullptr;
      }
    }
  }

} // namespace nvutil
#endif
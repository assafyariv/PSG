// Copyright NVIDIA Corporation 2002-2010
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

/*! \file */
#pragma once

#if defined(_WIN32)
# include <basetsd.h>
#endif
#if defined(LINUX)
# include <stdint.h>
typedef uintptr_t UINT_PTR;   //!< Linux specific type definition for UINT_PTR, as it's known from MSVC.
#endif

#include "nvutil/Allocator.h"
#include "nvutil/Assert.h"
#include "nvutil/Conversion.h"
#include "nvutil/RCObject.h"
#include "nvutil/SmartPtr.h"
#include "nvutil/SWMRSync.h"

namespace nvutil
{

#if !defined(DOXYGEN_IGNORE)
# define _new_ new

#if defined(_MSC_VER)
  // the ms compilers are less compliant to the C++ standard than other compilers
# define TEMPLATE
#else 
  // C++ standard compliant 
# define TEMPLATE  template 
#endif

// special 'null' type used to build Handle class hierarchy
class NullType;

// will be friends to access [begin|end][Read|Edit]
template<typename T> class ReadableObject;
template<typename T> class WritableObject;

// enables back-referencing an object to its handle 
// and serves locking purposes
template <typename T>
struct Holder : public nvutil::IAllocator 
{
  // both, the default constructor and the copy constructor use the placement new 
  // to create an object of type T at &rawmem[0]
  Holder(UINT_PTR hdl) 
  {
    // early assign the handle!
    // note: 
    // need to early assign handle and signature here because some object 
    // constructors require a valid handle available at construction-time
    ptr = (T*)&rawmem[0];
    ptr->m_signature = SIGNATURE;  
    ptr->m_handle = hdl;  
    // construct the object at specified memory
    // note:
    // the following does not override already assigned handle and signature!
    // see HandleObject constructor for more details
    ptr = new(&rawmem[0]) T(); 
  }

  // construct with one parameter
  template <typename P1>
  Holder( UINT_PTR hdl, P1 p1 ) 
  {
    // early assign the handle!
    // note: 
    // need to early assign handle and signature here because some object 
    // constructors require a valid handle available at construction-time
    ptr = (T*)&rawmem[0];
    ptr->m_signature = SIGNATURE;  
    ptr->m_handle = hdl;  
    // construct the object at specified memory
    // note:
    // the following does not override already assigned handle and signature!
    // see HandleObject constructor for more details
    ptr = new(&rawmem[0]) T( p1 ); 
  }

  Holder(const Holder<T>& rhs, UINT_PTR hdl) 
  {
    // early assign the handle!
    // note: 
    // need to early assign handle and signature here because some object 
    // constructors require a valid handle available at construction-time
    ptr = (T*)&rawmem[0];
    ptr->m_signature = SIGNATURE;  
    ptr->m_handle = hdl;  
    // copy-construct the object at specified memory
    // note:
    // the following does not override already assigned handle and signature!
    // see HandleObject constructor for more details
    ptr = new(&rawmem[0]) T(*rhs.ptr); 
  }

  template <typename U>
  Holder(const Holder<U>& rhs, UINT_PTR hdl)
  {
    // early assign the handle!
    // note: 
    // need to early assign handle and signature here because some object 
    // constructors require a valid handle available at construction-time
    ptr = (T*)&rawmem[0];
    ptr->m_signature = SIGNATURE;  
    ptr->m_handle = hdl;  
    // copy-construct the object at specified memory
    // note:
    // the following does not override already assigned handle and signature!
    // see HandleObject constructor for more details
    ptr = new(&rawmem[0]) T(*rhs.ptr);
  }
  // the destructor explicitely calls T's destructor
  ~Holder() { ptr->~T(); }

  nvutil::SWMRSync lock; // handles exclusive and shared locking
  T * ptr; // points to the object created at &rawmem[sizeof(UINT_PTR)]

  // note: this array must reside at the very end of this structure
  // as its size varies for different types T and hence would alter
  // offsets for subsequent members!
  char rawmem[sizeof(T)]; 
};
#endif  // DOXYGEN_IGNORE

// general definition helps to limit certain hierarchies, that is - 
// (1) a Handle must either be a stand-alone class (not derived from any other class) or 
// (2) must be derived from another Handle class

/*! \class Handle
    \brief Controls access to the the handled object.
    \param T Specifies the type of the handled object.
    \param U Specifies an optional base class for the particular Handle.

    The purpose of a Handle is to limit direct access to the handled object. 
    The user can access the interface of the handled object through the interface of the Handle.

    A Handle not only controls access to the handled object but also controls the lifetime of the
    handled object. The handled object will be created at Handle instantiation and will be 
    automatically destroyed when the Handle's lifetime ends.

    The instantiation of a Handle is limited. A Handle can live on heap only. 
    Attempts to create a Handle on stack will be rejected by the compiler. 
    It is also prohibited to explicitely call new or malloc on a Handle.
    The only ways possible to instantiate a Handle are  

    -# by instantiating a Handle as a copy from another Handle by invoking the Handle's clone member.
  
    The special design of the Handle template helps to limit the definition of different Handle
    types as folows:

    - A Handle type must either be a stand-alone class, not derived from any other class, or
    - must have another Handle type as base class.

    Examples:
    \code
        // valid typedefs
        typedef nvutil::Handle<nvsg::Object> ObjectHandle;
        typedef nvutil::Handle<nvsg::Node, ObjectHandle> NodeHandle;
    \endcode

    The purpose of the Handle template for the SceniX core is to have the compiler generate a particular
    hierarchy of Handles. This enables us to create a kind of parallel hierarchy of Handles that
    reflect the same class hierarchy that we have for the SceniX core objects. That is, in the same
    way nvsg::Node inherits from nvsg::Object, NodeHandle inherits from ObjectHandle, and so on. 
    This special Handle types thus carry the associated SceniX core objects and protect the core
    objects from direct access.

    \sa Handle<T, NullType>, Handle<T, Handle<U,V> >
*/
template <typename T, typename U=NullType>
class Handle;
class HandledObject;

/*! \brief Specialized stand-alone Handle
 
    This specialized Handle is used as a template for stand-alone Handle types. 
    Stand-alone Handle types are types that do not inherit from other types.

    \sa Handle
*/
template <typename T>
class Handle<T, NullType> : public nvutil::RCObject
{
  template<typename S> friend class ReadableObject;
  template<typename S> friend class WritableObject;

public:
  typedef T ObjectType;   //!< class internal type definition of the handled type.

public:
  // deep copy
  // NOTE: For this function to work as expected, it needs to be 'virtual'
  // However, declaring it virtual disables lazy instantiation for this function.
  // That is - the compiler would fully instantiate the definition and not only
  // as much as is really needed. For a generated class hierarchy for which the wrapped
  // type is part of a class hierarchy with an abstract base class, for example, this would 
  // end up in a compiler error, because abstract base classes cannot be instantiated.
  // A workaround for this is to implement a pseudo-virtual call mechanism for the clone
  // function via a function pointer which will be overridden for each 'real' Handle instance. 

  /*! \brief Returns a copy of this Handle
      \return
      The function returns a pointer to the Handle that was instantiated as a deep-copy
      of this Handle.

      Use this function if you want to instantiate a real clone of this Handle. 
  */
  Handle * clone() const; 

  /*! \brief Check if notfication is allowed.
   *  \remark Forwarding notifications is not allowed while in the constructor or destructor. */
  bool isNotifyAllowed();

#if !defined(DOXYGEN_IGNORE)

public:
  static Handle * create();

#if !defined(NDEBUG)
  void assertWriteLocked() const;
  void assertReadLocked() const;
#endif

protected:
  // default constructor will be invoked from derived class's constructors only
  Handle(); 
  virtual ~Handle();

  /*! \brief Provides write access to the handle object.
      \return 
      The function returns a pointer to the handled object, through which the user can access the 
      objects interface.

      If the SceniX runtime was configured to run in a multithreaded environment, the function locks the
      handled object for exclusive write access, before returning a pointer to that object.

      The object remains locked until the write-lock will be released through endEdit.

      Attempts to further use the returned pointer after the write-lock was released is an error and 
      results in undefined runtime behavior.
  */
  T * beginEdit(); 

  /*! \brief Releases write access to the handle object.

      The function releases the write access to the handled object that was previously obtained
      through beginEdit.
  */
  void endEdit();        

  /*! \brief Provides read-only access to the handle object.
      \return 
      The function returns a pointer to the handled object, through which the user can access the 
      objects const, thus read only interface.

      If the SceniX runtime was configured to run in a multithreaded environment, the function locks the
      handled object for shared read-only access, before returning a pointer to that const object.

      The object remains locked until the read-lock will be released through endRead.

      Attempts to further use the returned pointer after the read-lock was released is an error and 
      results in undefined runtime behavior.
  */
  const T * beginRead(); 

  /*! \brief Releases read-only access to the handle object.

      The function releases the read-only access to the handled object that was previously obtained
      through beginRead.

      \note 
      Attempts to write-lock an object that already was read-locked by the same thread dead-locks
      your application!
  */
  void endRead();       

public:
  // helper to turn the type-less pointed-to holder into an object
  // of type Holder<U>

  // NOTE: from a derived, that is - dependent Handle, this
  // function must be called using the template keyword as follows:
  //
  // Handle<T, U> h; 
  // // h.holder<T>(); // this throws a compile error!
  // h.TEMPLATE holder<T>(); // correct syntax!
  //
  // since h is dependent, the compiler is not expecting holder to be
  // a template and so parses holder as a non-template if the template
  // keyword was not specified as shown above!
  template <typename U>
  const Holder<U>& holder() const;

protected:
  // function pointer through which to call the pseudo-virtual clone function.
  Handle * (Handle::*m_pfnVirtualClone)(void) const;

  // type-less pointer to a Holder<T>
  //
  // use a Holder<HandledObject>* here to avoid violation of strict-aliasing
  // rules, where the compiler can assume that objects of different types
  // never reside at the same memory address.
  //
  // since the type of the pointed-to object changes per derived Handle, the
  // usage of Holder<T>* instead would violate strict-aliasing rules, which
  // in turn can result in unexpected runtime behavior!
  Holder<HandledObject> * m_holder;
  bool                    m_notifyAllowed;

#if !defined(NDEBUG)
  SWMRSync m_dbgLock; 
  int m_dbgWriteCount;
  int m_dbgReadCount;
  bool m_dbgInsideConstructor;
  bool m_dbgInsideDestructor;
#endif

private:
  // copy constructor will be invoked by clone
  Handle(const Handle& rhs);
  // init on default construction; will be invoked by create
  void default_init();
  // init on construction with one parameter; will be invoked by create
  template <typename P1> void default_init( P1 p1 );
  // implementation of clone, called through m_pfnVirtualClone
  Handle * clone_impl() const;

  // not permitted ...
  // ... assignment
  Handle& operator=(const Handle& rhs);
  // ... comparison 
  bool operator==(const Handle& rhs) const;
  bool operator!=(const Handle& rhs) const;

#endif // DOXYGEN_IGNORE
};

/*! \brief Specialization template for Handle-derived Handle types
 
    This specialized Handle is used as a template for Handle-derived handle types. 

    \sa Handle, Handle<T, NullType>
*/
template <typename T, typename U, typename V>
class Handle<T, Handle<U,V> > : public Handle<U,V>
{
  template<typename S> friend class ReadableObject;
  template<typename S> friend class WritableObject;

public:
  typedef T ObjectType;   //!< class internal type definition of the handled type.

public:
  /*! \brief Returns a copy of this Handle
      \return
      The function returns a pointer to the Handle that was instantiated as a deep-copy
      of this Handle.

      Use this function if you want to instantiate a real clone of this Handle. 
  */
  Handle * clone() const; // hides! base class implementation

#if !defined(DOXYGEN_IGNORE)
public:
  // instantiation
  static Handle * create();
  template <typename P1> static Handle * create( P1 p1 );
  static Handle * createFromBase(const Handle<U,V>& base);
#if !defined(NDEBUG)
  static Handle * create(const char * file, unsigned int line);
  static Handle * createFromBase(const char * file, unsigned int line, const Handle<U,V>& base);
#endif
  
protected:
  Handle();
  virtual ~Handle();

  // accessors  
  T * beginEdit();       // hides! base class implementation
  const T * beginRead(); // hides! base class implementation
  // no need to implement end[Edit/Read] functions

private:
  // private typedef used in private constructors below  
  typedef Handle* (Handle::*PFNVIRTUALCLONE) (void) const;
  // copy constructor, called through clone
  Handle(const Handle& rhs);
  // construct from a base Handle
  explicit Handle(const Handle<U,V>& base);
  // init on default construction; will be invoked by create
  void default_init();
  // init on construction with one parameter; will be invoked by create
  template <typename P1> void default_init( P1 p1 );
  // implementation of clone, called via m_pfnVirtualClone
  Handle * clone_impl() const;

  // not permitted ...
  // ... assignment
  Handle& operator=(const Handle& rhs);
  // ... comparison 
  bool operator==(const Handle& rhs) const;
  bool operator!=(const Handle& rhs) const;

#endif // DOXYGEN_IGNORE
};

//
// implementation of member functions
//

#if !defined(DOXYGEN_IGNORE)
template <typename T>
inline Handle<T, NullType> * Handle<T, NullType>::create() 
{ 
  Handle<T, NullType> * hdl = new Handle<T, NullType>();
  hdl->default_init();
  return hdl;
}

template <typename T>
inline Handle<T, NullType>::Handle() 
: m_notifyAllowed(false)
#if !defined(NDEBUG)
, m_dbgWriteCount(0)
, m_dbgReadCount(0)
, m_dbgInsideConstructor(true)
, m_dbgInsideDestructor(false)
#endif
{
}

template <typename T>
inline Handle<T, NullType>::Handle(const Handle<T,NullType>& rhs) 
: m_pfnVirtualClone(&Handle<T, NullType>::clone_impl)
, m_notifyAllowed(false)
#if !defined(NDEBUG)
, m_dbgWriteCount(0)
, m_dbgReadCount(0)
, m_dbgInsideConstructor(true)
, m_dbgInsideDestructor(false)
#endif
{ 
  // read-lock source while being copied
  Handle<T,NullType> * prhs = const_cast<Handle<T,NullType>*>(&rhs);
  prhs->beginRead(); 
  m_holder = reinterpret_cast<Holder<HandledObject>*>(_new_ Holder<T>(rhs.holder<T>(), (UINT_PTR)this));
  prhs->endRead();

  // Allow notification again now the wrapped object has been fully constructed.
  m_notifyAllowed = true;
#if !defined(NDEBUG)
  m_dbgInsideConstructor = false;
#endif
}

template <typename T>
inline Handle<T, NullType>::~Handle() 
{ 
#if !defined(NDEBUG)
  NVSG_ASSERT(m_dbgWriteCount==0); // somebody still writing the object ->undefined behavior
  NVSG_ASSERT(m_dbgReadCount==0); // somebody still reading the object ->undefined behavior
  m_dbgInsideDestructor = true;
#endif
  m_notifyAllowed = false;
  delete (Holder<T>*)m_holder; 
}

template <typename T>
inline void Handle<T, NullType>::default_init()
{
  NVSG_ASSERT(m_dbgInsideConstructor);
  m_pfnVirtualClone = &Handle<T, NullType>::clone_impl;
  m_holder = reinterpret_cast<Holder<HandledObject>*>(_new_ Holder<T>((UINT_PTR)this));

  m_notifyAllowed = true;
#if !defined(NDEBUG)
  m_dbgInsideConstructor = false;
#endif
}

template <typename T>
template <typename P1>
inline void Handle<T, NullType>::default_init( P1 p1 )
{
  NVSG_ASSERT(m_dbgInsideConstructor);
  m_pfnVirtualClone = &Handle<T, NullType>::clone_impl;
  m_holder = reinterpret_cast<Holder<HandledObject>*>(_new_ Holder<T>((UINT_PTR)this, p1));

  m_notifyAllowed = true;
#if !defined(NDEBUG)
  m_dbgInsideConstructor = false;
#endif
}
#endif // DOXYGEN_IGNORE

template <typename T>
inline Handle<T, NullType> * Handle<T, NullType>::clone() const 
{ // pseudo-virtual call via member function pointer solves problems with 
  // the compiler's instantiation policy for virtual functions 
  return (this->*m_pfnVirtualClone)();
} 

#if !defined(DOXYGEN_IGNORE)
template <typename T>
inline Handle<T, NullType> * Handle<T, NullType>::clone_impl() const 
{
  return _new_ Handle<T, NullType>(*this);
}

template <typename T>
inline T * Handle<T, NullType>::beginEdit()       
{
  // the following waits until exclusive access was given
  holder<T>().lock.lockExclusive(); 
#if !defined(NDEBUG)
  m_dbgWriteCount++;
#endif
  return holder<T>().ptr; 
}

template <typename T>
inline void Handle<T, NullType>::endEdit()       
{
  // release exclusive access
#if !defined(NDEBUG)
  // catch endEdit without prior beginEdit
  NVSG_ASSERT(m_dbgWriteCount>0); 
  m_dbgWriteCount--;
#endif
  holder<T>().lock.unlockExclusive();
}

template <typename T>
inline const T * Handle<T, NullType>::beginRead()       
{ 
  // the following waits until shared access was given
  holder<T>().lock.lockShared(); 
#if !defined(NDEBUG)
  {
    // we need exclusive access to increment this debug counter
    // note: this is not given by the previous shared lock
    AutoLock dbgLock(m_dbgLock);
    m_dbgReadCount++;
  }
#endif
  return holder<T>().ptr; 
}

template <typename T>
inline void Handle<T, NullType>::endRead()       
{
  // release shared access
#if !defined(NDEBUG)
  {
    AutoLock dbgLock(m_dbgLock);
    // catch endRead without prior beginRead
    NVSG_ASSERT(m_dbgReadCount>0); 
    m_dbgReadCount--;
  }
#endif
  holder<T>().lock.unlockShared();
}
#endif // DOXYGEN_IGNORE

template <typename T>
inline bool Handle<T,NullType>::isNotifyAllowed()
{
  return( m_notifyAllowed );
}

#if !defined(DOXYGEN_IGNORE)
template <typename T>
template <typename U>
inline const Holder<U>& Handle<T, NullType>::holder() const
{
  return *((Holder<U>*)m_holder);
}

#if !defined(NDEBUG)
template <typename T>
inline void Handle<T, NullType>::assertWriteLocked() const 
{ 
  // NOTE: Actually, this needs additional thread info.
  // That is - this is not 100% correct, for example,
  // when the object is write-locked from thread A and
  // a write access without locking is performed from 
  // thread B!
  NVSG_ASSERT(  m_dbgInsideConstructor 
             || m_dbgInsideDestructor 
             || m_dbgWriteCount>0 ); 
}
template <typename T>
inline void Handle<T, NullType>::assertReadLocked() const 
{ 
  // NOTE: Actually, this needs additional thread info!
  NVSG_ASSERT(  m_dbgInsideConstructor 
             || m_dbgInsideDestructor 
             || m_dbgReadCount>0 
             || m_dbgWriteCount>0 ); 
}
#endif

template <typename T, typename U, typename V>
inline Handle<T, Handle<U,V> > * Handle<T, Handle<U,V> >::create() 
{ 
  Handle<T,Handle<U,V> > * hdl = new Handle<T,Handle<U,V> >();
  hdl->default_init();
  return hdl;
}

template <typename T, typename U, typename V>
template <typename P1>
inline Handle<T, Handle<U,V> > * Handle<T, Handle<U,V> >::create( P1 p1 )
{ 
  Handle<T,Handle<U,V> > * hdl = new Handle<T,Handle<U,V> >();
  hdl->default_init( p1 );
  return hdl;
}

template <typename T, typename U, typename V>
inline Handle<T, Handle<U,V> > * Handle<T, Handle<U,V> >::createFromBase(const Handle<U,V>& base)
{
  return new Handle<T,Handle<U,V> >(base);
}

#if !defined(NDEBUG)
template <typename T, typename U, typename V>
inline Handle<T, Handle<U,V> > * Handle<T, Handle<U,V> >::create(const char* file, unsigned int line) 
{ 
  Handle<T,Handle<U,V> > * hdl = new(file, line) Handle<T,Handle<U,V> >();
  hdl->default_init();
  return hdl;
}

template <typename T, typename U, typename V>
inline Handle<T, Handle<U,V> > * Handle<T, Handle<U,V> >::createFromBase(const char* file, unsigned int line, const Handle<U,V>& base)
{
  return new(file, line) Handle<T,Handle<U,V> >(base);
}
#endif

template <typename T, typename U, typename V>
inline Handle<T,Handle<U,V> >::Handle() 
{ 
  NVSG_ASSERT((Handle<U,V>::m_dbgInsideConstructor));
  /* do nothing ! */ 
}

template <typename T, typename U, typename V>
inline Handle<T,Handle<U,V> >::Handle(const Handle<T,Handle<U,V> >& rhs) 
// note: must not call base classes copy constructor here!!
{ 
  NVSG_ASSERT((Handle<U,V>::m_dbgInsideConstructor));
  // override function pointer! 
  *((PFNVIRTUALCLONE*)&this->m_pfnVirtualClone) = &Handle<T,Handle<U,V> >::clone_impl;
  // read-lock source while being copied
  Handle<T,Handle<U,V> > * prhs = const_cast<Handle<T,Handle<U,V> >*>(&rhs);
  prhs->beginRead();
  this->m_holder = reinterpret_cast<Holder<HandledObject>*>(_new_ Holder<T>(rhs.TEMPLATE holder<T>(), (UINT_PTR)this));
  prhs->endRead();

  // Allow notification again now the wrapped object has been fully constructed.
  this->m_notifyAllowed = true;
#if !defined(NDEBUG)
  Handle<U,V>::m_dbgInsideConstructor = false;
#endif
} 

template <typename T, typename U, typename V>
inline Handle<T,Handle<U,V> >::Handle(const Handle<U,V>& base) 
// note: must not call base classes copy constructor here!!
{ 
  NVSG_ASSERT((Handle<U,V>::m_dbgInsideConstructor));
  // override function pointer! 
  *((PFNVIRTUALCLONE*)&this->m_pfnVirtualClone) = &Handle<T,Handle<U,V> >::clone_impl;
  this->m_holder = reinterpret_cast<Holder<HandledObject>*>(_new_ Holder<T>(base.TEMPLATE holder<U>(), (UINT_PTR)this));

  // Allow notification again now the wrapped object has been fully constructed.
  this->m_notifyAllowed = true;
#if !defined(NDEBUG)
  Handle<U,V>::m_dbgInsideConstructor = false;
#endif
} 

template <typename T, typename U, typename V>
inline Handle<T,Handle<U,V> >::~Handle() 
{ 
  // cleanup will be managed by uppermost base class!
}

template <typename T, typename U, typename V>
inline void Handle<T,Handle<U,V> >::default_init()
{
  NVSG_ASSERT((Handle<U,V>::m_dbgInsideConstructor));
  // override function pointer! 
  *((PFNVIRTUALCLONE*)&this->m_pfnVirtualClone) = &Handle<T,Handle<U,V> >::clone_impl;
  this->m_holder = reinterpret_cast<Holder<HandledObject>*>(_new_ Holder<T>((UINT_PTR)this));
  this->m_notifyAllowed = true;
#if !defined(NDEBUG)
  Handle<U,V>::m_dbgInsideConstructor = false;
#endif
}

template <typename T, typename U, typename V>
template <typename P1>
inline void Handle<T,Handle<U,V> >::default_init( P1 p1 )
{
  NVSG_ASSERT((Handle<U,V>::m_dbgInsideConstructor));
  // override function pointer! 
  *((PFNVIRTUALCLONE*)&this->m_pfnVirtualClone) = &Handle<T,Handle<U,V> >::clone_impl;
  this->m_holder = reinterpret_cast<Holder<HandledObject>*>(_new_ Holder<T>((UINT_PTR)this, p1));
  this->m_notifyAllowed = true;
#if !defined(NDEBUG)
  Handle<U,V>::m_dbgInsideConstructor = false;
#endif
}
#endif // DOXYGEN_IGNORE

template <typename T, typename U, typename V>
inline Handle<T,Handle<U,V> > * Handle<T,Handle<U,V> >::clone() const 
{ // pseudo-virtual call via member function pointer solves problems with 
  // the compiler's instantiation policy for virtual functions   
  return (Handle<T,Handle<U,V> >*)(this->*Handle::m_pfnVirtualClone)();
} 

#if !defined(DOXYGEN_IGNORE)
template <typename T, typename U, typename V>
inline Handle<T,Handle<U,V> > * Handle<T,Handle<U,V> >::clone_impl() const 
{ 
  return _new_ Handle<T,Handle<U,V> >(*this);
} 

template <typename T, typename U, typename V>
inline T * Handle<T,Handle<U,V> >::beginEdit()       
{ 
  // the following waits until exclusive access was given
  this->TEMPLATE holder<T>().lock.lockExclusive(); 
#if !defined(NDEBUG)
  Handle<U,V>::m_dbgWriteCount++;
#endif
  return this->TEMPLATE holder<T>().ptr; 
}

template <typename T, typename U, typename V>
inline const T * Handle<T,Handle<U,V> >::beginRead() 
{ 
  // the following waits until exclusive access was given
  this->TEMPLATE holder<T>().lock.lockShared(); 
#if !defined(NDEBUG)
  {
    AutoLock dbgLock(Handle<U,V>::m_dbgLock);
    Handle<U,V>::m_dbgReadCount++;
  }
#endif
  return this->TEMPLATE holder<T>().ptr; 
}
#endif // DOXYGEN_IGNORE

//
// implementation of convenient helper functions and structs
//

/*! \brief Functor class to clone an object
 *  \remark This Functor can be used, for example, to clone all objects of a STL container, using the
 *  STL algorithm transform. */
struct CloneObject
{
  /*! \brief The function call operator of this functor.
   *  \param src A reference to the SmartPtr to clone.
   *  \return A clone of the parameter \a src. */
  template<typename T>
  SmartPtr<T> operator()( const SmartPtr<T> & src )
  {
    return( SmartPtr<T>(src.get()->clone()) );
  }
};

#undef TEMPLATE
#undef _new_

/*! \brief Typedefs for objects
   The following typedefs should be defined for an object.
   Handle
   SharedPtr
   WeakPtr
   ReadLock
   WriteLock
**/
template <typename ObjectType> struct ObjectTraits {};

/*! \brief Special casting operator for const SharedPtr.
 *  \param rhs The const SharedPtr to cast.
 *  \return \a rhs casted from type \c const \c USharedPtr to type \c const \c SharedPtr<T>.
 *  \remark A sharedPtr_cast can be used to do a downcast a SharedPtr. In debug builds, it is checked whether
 *  that downcast is allowed. */
template<typename T, typename USharedPtr>
inline const typename ObjectTraits<T>::SharedPtr & sharedPtr_cast( const USharedPtr & rhs )
{
  NVSG_ASSERT( !rhs || isSmartPtrOf<typename ObjectTraits<T>::Handle>(rhs) );
  return( *reinterpret_cast<const typename ObjectTraits<T>::SharedPtr *>(&rhs) );
}

/*! \brief Special casting operator for SharedPtr.
 *  \param rhs The SharedPtr to cast.
 *  \return \a rhs casted from type \c USharedPtr to type \c SharedPtr<T>.
 *  \remark A sharedPtr_cast can be used to do a downcast of a SharedPtr. In debug builds, it is checked whether
 *  that downcast is allowed. */
template<typename T, typename USharedPtr>
inline typename ObjectTraits<T>::SharedPtr & sharedPtr_cast( USharedPtr & rhs )
{
  NVSG_ASSERT( !rhs || isSmartPtrOf<typename ObjectTraits<T>::Handle>(rhs) );
  return( *reinterpret_cast<typename ObjectTraits<T>::SharedPtr *>(&rhs) );
}

/*! \brief Special casting operator for const WeakPtr.
 *  \param rhs The const WeakPtr to cast.
 *  \return \a rhs casted from type \c const \c UWeakPtr to type \c const \c WeakPtr<T>.
 *  \remark A weakPtr_cast can be used to do a downcast a WeakPtr. In debug builds, it is checked whether
 *  that downcast is allowed. */
template<typename T, typename UWeakPtr>
inline const typename ObjectTraits<T>::WeakPtr & weakPtr_cast( const UWeakPtr & rhs )
{
  NVSG_ASSERT( !rhs || dynamic_cast<typename ObjectTraits<T>::WeakPtr>(rhs) );
  return( *reinterpret_cast<const typename ObjectTraits<T>::WeakPtr *>(&rhs) );
}

/*! \brief Special casting operator for WeakPtr.
 *  \param rhs The WeakPtr to cast.
 *  \return \a rhs casted from type \c UWeakPtr to type \c WeakPtr<T>.
 *  \remark A weakPtr_cast can be used to do a downcast a WeakPtr. In debug builds, it is checked whether
 *  that downcast is allowed. */
template<typename T, typename UWeakPtr>
inline typename ObjectTraits<T>::WeakPtr & weakPtr_cast( UWeakPtr & rhs )
{
  NVSG_ASSERT( !rhs || dynamic_cast<typename ObjectTraits<T>::WeakPtr>(rhs) );
  return( *reinterpret_cast<typename ObjectTraits<T>::WeakPtr *>(&rhs) );
}

} // namespace nvutil


/*! \brief Macro to define the five standard types for a base type T.
 *  \remark For convenience, for each class T, we define the types THandle, TSharedPtr, TWeakPtr, TReadLock and
 *  TWriteLock. */
#define CORE_TYPES_BASE(T)                            \
  typedef nvutil::Handle<T>           T##Handle;      \
  typedef nvutil::SmartPtr<T##Handle> T##SharedPtr;   \
  typedef T##Handle *                 T##WeakPtr;     \
  typedef nvutil::ReadableObject<T>   T##ReadLock;    \
  typedef nvutil::WritableObject<T>   T##WriteLock

/*! \brief Macro to define the five standard types for a type T, whose base type is BT.
 *  \remark For convenience, for each class T, we define the types THandle, TSharedPtr, TWeakPtr, TReadLock, and
 *  TWriteLock. */
#define CORE_TYPES(T,BT)                              \
  typedef nvutil::Handle<T,BT##Handle>  T##Handle;    \
  typedef nvutil::SmartPtr<T##Handle>   T##SharedPtr; \
  typedef T##Handle *                   T##WeakPtr;   \
  typedef nvutil::ReadableObject<T>     T##ReadLock;  \
  typedef nvutil::WritableObject<T>     T##WriteLock

/*! \brief Macro to define ObjectType and our five standard types of a base type T as part of a templated struct.
 *  \remark Using this struct, the standard types Handle, SharedPtr, WeakPtr, ReadLock, and WriteLock, as well as
 *  the ObjectType itself, are easily available within a template context. */
#define OBJECT_TRAITS_BASE(T)                         \
template <> struct ObjectTraits<T>                    \
{                                                     \
  typedef T                               ObjectType; \
  typedef nvutil::Handle<T>               Handle;     \
  typedef nvutil::SmartPtr<T##Handle>     SharedPtr;  \
  typedef T##Handle *                     WeakPtr;    \
  typedef nvutil::ReadableObject<T>       ReadLock;   \
  typedef nvutil::WritableObject<T>       WriteLock;  \
}

/*! \brief Macro to define ObjectType and our five standard types of a type T, with base type BT, as part of a templated struct.
 *  \remark Using this struct, the standard types Handle, SharedPtr, WeakPtr, ReadLock, and WriteLock, as well as
 *  the ObjectType itself, are easily available within a template context. */
#define OBJECT_TRAITS(T, BT)                          \
template <> struct ObjectTraits<T>                    \
{                                                     \
  typedef T                               ObjectType; \
  typedef BT                              Base;       \
  typedef nvutil::Handle<T,BT##Handle>    Handle;     \
  typedef nvutil::SmartPtr<T##Handle>     SharedPtr;  \
  typedef T##Handle *                     WeakPtr;    \
  typedef nvutil::ReadableObject<T>       ReadLock;   \
  typedef nvutil::WritableObject<T>       WriteLock;  \
}

namespace nvutil
{
  class HandledObject;
  class Observer;
  class BackLinkObserver;

  CORE_TYPES_BASE( HandledObject );         //!< Type definitions for HandledObject
  CORE_TYPES( Observer, HandledObject );    //!< Type definitions for Observer
  CORE_TYPES( BackLinkObserver, Observer ); //!< Type definitions for BackLinkObserver

  OBJECT_TRAITS_BASE( HandledObject );          //!< Trait definitions for HandledObject
  OBJECT_TRAITS( Observer, HandledObject );     //!< Trait definitions for Observer
  OBJECT_TRAITS( BackLinkObserver, Observer );  //!< Trait definitions for BackLinkObserver
}

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

#if defined(_WIN32)
# include "nvsgwindefs.h"
# ifndef NOMINMAX
#  define NOMINMAX // avoid problems with Microsoft definitions in windef.h and min/max from stl
# endif
//# include <windows.h>
//# include <winbase.h>
# undef NOMINMAX
#endif

#if defined(LINUX)
# include <pthread.h>  
#endif

#include <map>

//! nvutil namespace 
namespace nvutil
{
  /*! brief Platform independent type definition of a thread synchronization object. */
#if defined(_WIN32)
  typedef CRITICAL_SECTION SWMRCriticalSection;
#elif defined(LINUX)
  typedef pthread_mutex_t  SWMRCriticalSection;
#endif

  /*! \brief Enable locking of the SWMRSync class.
   *  \remarks This function is called from within nvsgInitialize if the argument \a threadingModel
   *  is \c NVSG_MULTITHREADED. */
  NVSG_API extern void EnableLocking();

#if !defined(SINGLE_THREADED_LOCKING)
  /*! \brief Interface class for SWMRSync **/
  class SWMRSyncImpl {
  public:
    NVSG_API virtual ~SWMRSyncImpl();

    /*! \brief Lock the object for exclusive access.
     *  \return \c true, if exclusive access is granted.
     *  \remarks This function waits until it gets exclusive access to the object. After getting
     *  exclusive access, it is safe to write to the object, and no other thread has read or write
     *  access to that object.
     *  \sa unlockExclusive, lockShared */
    NVSG_API virtual bool lockExclusive()   const = 0;

    /*! \brief Unlock the object from exclusive access.
     *  \remarks After unlocking the object from exclusive access, it is unsafe to have read or
     *  write access to that object.
     *  \sa lockExclusive, unlockShared */
    NVSG_API virtual void unlockExclusive() const = 0;

    /*! \brief Lock the object for shared access.
     *  \return \c true, if shared access is granted.
     *  \remarks This function waits until it gets shared access to the object. After getting shared
     *  access, it is safe to read from the object, but not to write to it. No other thread has
     *  write access to that object then.
     *  \sa unlockShared, lockExclusive */
    NVSG_API virtual bool lockShared()      const = 0;

    /*! \brief Unlock the object from shared access.
     *  \remarks After unlocking the object from shared access, it is unsafe to have read or write
     *  access to that object. Any other thread, trying to get exclusive access, can now get it.
     *  \sa lockShared, unlockExclusive */
    NVSG_API virtual void unlockShared()    const = 0;

  protected:
#if !defined(DOXYGEN_IGNORE)
#if !defined(NDEBUG)
    NVSG_API void DBG_INCR_THREADS_SHARED_LOCK_CNT() const;
    NVSG_API void DBG_DECR_THREADS_SHARED_LOCK_CNT() const;
    NVSG_API int  DBG_GET_THREADS_SHARED_LOCK_CNT() const;

#if defined(_WIN32)
    typedef std::map<DWORD,int> SWMRPerThreadSharedLockCountMap;
#elif defined(LINUX)
    typedef std::map<pthread_t,int> SWMRPerThreadSharedLockCountMap;
#endif
      // accessed only when inside critical section
  mutable SWMRPerThreadSharedLockCountMap m_perThreadSharedLockCount; 
#else
# define DBG_INCR_THREADS_SHARED_LOCK_CNT() static_cast<void*>(0)
# define DBG_DECR_THREADS_SHARED_LOCK_CNT() static_cast<void*>(0)
# define DBG_GET_THREADS_SHARED_LOCK_CNT()  static_cast<void*>(0)
#endif // !NDEBUG
#endif // DOXYGEN_IGNORE
  };
#endif

  /*! \brief Single write, multiple read synchronization class.
   *  \remarks This class provides the functionality for locking to ensure a single write, multiple
   *  read synchronization. That is, an unlimited number of clients can have read access, but only one
   *  client can have write access, and when a client has write access, no other client can get read
   *  access. An Object, for example, holds a member of type SWMRSync to manage the read/write
   *  operations. */
  class SWMRSync
  {
    public:
      /*! \brief Default constructor.
       *  \remarks Performs initialization of the internal mutex object. */
      NVSG_API SWMRSync();

      /*! \brief Copy constructor.
        * \remarks Performs initialization of the internal mutex object. Does not copy data from the
        * source object. */
      NVSG_API SWMRSync( const SWMRSync & rhs );

      /*! \brief Default destructor.
       *  \remarks Performs deletion of the internal mutex object. */
      NVSG_API ~SWMRSync(void);

      /*! \brief Lock the object for exclusive access.
       *  \return \c true, if exclusive access is granted.
       *  \remarks This function waits until it gets exclusive access to the object. After getting
       *  exclusive access, it is safe to write to the object, and no other thread has read or write
       *  access to that object.
       *  \sa unlockExclusive, lockShared */
      NVSG_API bool lockExclusive()   const;

      /*! \brief Unlock the object from exclusive access.
       *  \remarks After unlocking the object from exclusive access, it is unsafe to have read or
       *  write access to that object.
       *  \sa lockExclusive, unlockShared */
      NVSG_API void unlockExclusive() const;

      /*! \brief Lock the object for shared access.
       *  \return \c true, if shared access is granted.
       *  \remarks This function waits until it gets shared access to the object. After getting shared
       *  access, it is safe to read from the object, but not to write to it. No other thread has
       *  write access to that object then.
       *  \sa unlockShared, lockExclusive */
      NVSG_API bool lockShared()      const;

      /*! \brief Unlock the object from shared access.
       *  \remarks After unlocking the object from shared access, it is unsafe to have read or write
       *  access to that object. Any other thread, trying to get exclusive access, can now get it.
       *  \sa lockShared, unlockExclusive */
      NVSG_API void unlockShared()    const;

    private:
#if !defined(SINGLE_THREADED_LOCKING)
      SWMRSyncImpl *m_impl;
#elif !defined( NDEBUG )
      mutable int                 m_sharedLockCount; // counts concurrent readers, accessed only when inside critical section
#endif
  };

  /*! \brief Helper class to provide a scoped usage of an SWMRSync lock. */
  class AutoLock
  {
  public:
    /*! \brief Constructor of an AutoLock
     *  \param lock An SWMRSync to be exclusively locked for the lifetime of this AutoLock object. */
    explicit AutoLock(const SWMRSync& lock) : m_lock(lock) 
    {
      m_lock.lockExclusive();
    }

    /*! \brief Destructor of an AutoLock
     *  \remarks On desctruction of this object, the exclusive lock from the constructor is released. */
    ~AutoLock() 
    {
      m_lock.unlockExclusive();
    }

  private:
    // don't permit copying and assignment
    AutoLock(const AutoLock&);
    AutoLock& operator=(const AutoLock&);

    const SWMRSync& m_lock; // reference to the lock object passed at instantiation
  };

  inline bool SWMRSync::lockExclusive() const
  {
    NVSG_TRACE();
#if !defined(SINGLE_THREADED_LOCKING)
    return m_impl->lockExclusive();
#elif !defined(NDEBUG)
  NVSG_ASSERT( m_sharedLockCount == 0 );
  return m_sharedLockCount == 0;
#else
  return true;
#endif
  }

  inline void SWMRSync::unlockExclusive() const
  {
    NVSG_TRACE();
#if !defined(SINGLE_THREADED_LOCKING)
    m_impl->unlockExclusive();
#elif !defined(NDEBUG)
  NVSG_ASSERT( m_sharedLockCount == 0 );
#endif
  }

  inline bool SWMRSync::lockShared() const
  {
    NVSG_TRACE();
#if !defined(SINGLE_THREADED_LOCKING)
    return m_impl->lockShared();
#elif !defined(NDEBUG)
    ++m_sharedLockCount;
#endif
    return true;
  }

  inline void SWMRSync::unlockShared () const
  {
    NVSG_TRACE();
#if !defined(SINGLE_THREADED_LOCKING)
    m_impl->unlockShared();
#elif !defined(NDEBUG)
    --m_sharedLockCount;
#endif
  }
} // namespace nvutil


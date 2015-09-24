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

#include <nvsgcommon.h>
#include <nvsg/CoreTypes.h>
#include <nvsg/OwnedObject.h>
#include <nvutil/Incarnation.h>
#include <nvutil/ReadableObject.h>
#include <nvutil/WritableObject.h>
#include <nvutil/StridedIterator.h>
#include <nvutil/SWMRSync.h>

namespace nvsg
{
  class ReadLockImpl;
  class WriteLockImpl;

  /** \brief Interface for buffers used as storage for SceniX. 
   *  \sa BufferHost, BufferGL
  **/
  class Buffer : public OwnedObject<Object>
  {
  public:
    // FIXME enforce ctor with managedBySystem state
    NVSG_API Buffer() : m_lockCount(0), m_managedBySystem(true) {}
    NVSG_API virtual ~Buffer();
    
    enum MapMode { MAP_NONE = 0, MAP_READ = 1, MAP_WRITE = 2, MAP_READWRITE = MAP_READ | MAP_WRITE };


    /** \brief copy the continuous memory from another buffer to self
    **/
    NVSG_API virtual void setData( size_t dst_offset, size_t length, const void* src_data );
    NVSG_API virtual void setData( size_t dst_offset, size_t length, const BufferSharedPtr &src_buffer , size_t src_offset );

    /** \brief copy the continuous memory to another buffer
    **/
    NVSG_API virtual void getData( size_t src_offset, size_t length, void* dst_data ) const;
    NVSG_API virtual void getData( size_t src_offset, size_t length, const BufferSharedPtr &dst_buffer , size_t dst_offset ) const;
    

    /** \brief Retrieve the size of the Buffer
     **/
    NVSG_API virtual size_t getSize() const = 0;
    
    /** \brief Resize the Buffer. Any data currently stored in this buffer will be lost.
     \  \param New size for the buffer
    **/
    NVSG_API virtual void setSize( size_t size ) = 0;

    /** \brief Resize the Buffer. This function keeps data currently stored in the buffer if possible.
     \  \param New size for the buffer
    **/
    NVSG_API virtual void resize( size_t size );

    /** \brief Get the incarnation count of the buffer
    **/
    NVSG_API virtual nvutil::Incarnation getIncarnation() const = 0;

    /** \brief Get whether buffer is managed by internal system or user
    **/
    NVSG_API bool isManagedBySystem() const;

    /** \brief Object to acquire a thread-safe read access to the buffer's data. 
     *  \sa WriteLock
    **/
    class DataReadLock {
    public:

      /**
       *  \brief Prepare thread-safe access to a range within buffer data. Only use the pointers the way the MapMode describes it!
          \param mode desired MapMode to the buffer
          \param offset in bytes to start map from
          \param length in bytes for the mapped range
      **/
      NVSG_API DataReadLock() {};
      NVSG_API DataReadLock( const BufferSharedPtr &buffer) : m_lock( new ReadLockImpl( buffer ) ) {}
      NVSG_API DataReadLock( const BufferSharedPtr &buffer, size_t offset, size_t length) 
                         : m_lock( new ReadLockImpl( buffer , offset, length) ) {}
      NVSG_API DataReadLock( const DataReadLock &rhs ) { m_lock = rhs.m_lock; }
      NVSG_API DataReadLock &operator=( const DataReadLock &rhs ) { m_lock = rhs.m_lock; return *this;}

      void reset() { m_lock.reset(); }

      /** \brief Returns the pointer to the mapped range within the buffer.
      **/
      template <typename ValueType> const ValueType *getPtr() const { return reinterpret_cast<const ValueType *>(m_lock->m_ptr); }
      NVSG_API const void *getPtr() const { return m_lock->m_ptr; }
    protected:
      class ReadLockImpl : public nvutil::RCObject
      {
      public:
        NVSG_API ReadLockImpl( const BufferSharedPtr &buffer);
        NVSG_API ReadLockImpl( const BufferSharedPtr &buffer, size_t offset, size_t length);
        NVSG_API ~ReadLockImpl();

        BufferSharedPtr m_buffer;
        BufferReadLock  m_bufferLock;
        const void *m_ptr;
      };
    private:
      nvutil::SmartPtr<ReadLockImpl> m_lock;
    };


    /** \brief Object to acquire a thread-safe write, read or read-write access to the buffer's data. 
     *  \sa ReadLock
    **/
    class DataWriteLock {
    public:

      /**
       *  \brief Prepare thread-safe access to a range within buffer data. Only use the pointers the way the MapMode describes it!
          \param mapMode desired MapMode to the buffer.
          \param offset in bytes to start map from
          \param length in bytes for the mapped range
          \note Mapping with \c MAP_WRITE must not be used to retrieve data, and when using \c MAP_READ 
          you must not write data. Both operations are legal on the C++ side as you get a non-const pointer
          when mapping, but are illegal for the system's integrity. Buffer implementations will have undefined behavior
          when you use the MapMode wrongly.
          \sa getPtr
      **/
      NVSG_API DataWriteLock() {};
      NVSG_API DataWriteLock( const BufferSharedPtr &buffer, Buffer::MapMode mapMode )
                          : m_lock(new WriteLockImpl( buffer, mapMode ) ) { }
      NVSG_API DataWriteLock( const BufferSharedPtr &buffer, Buffer::MapMode mapMode, size_t offset, size_t length )
                          : m_lock(new WriteLockImpl( buffer, mapMode, offset, length ) ) { }
      NVSG_API DataWriteLock( const DataWriteLock &rhs ) { m_lock = rhs.m_lock; }
      NVSG_API DataWriteLock &operator=( const DataWriteLock &rhs ) { m_lock = rhs.m_lock; return *this; }

      NVSG_API void reset() { m_lock.reset(); }

      /** \brief Returns the pointer to the mapped range within the buffer. Make sure you only use it according to MapMode.
      **/
      NVSG_API void*getPtr() const { return m_lock->m_ptr; }
      template <typename ValueType> ValueType *getPtr() const { return reinterpret_cast<ValueType *>(m_lock->m_ptr); }
    protected:
      class WriteLockImpl : public nvutil::RCObject
      {
      public:
        NVSG_API WriteLockImpl( const BufferSharedPtr &buffer, Buffer::MapMode mapMode );
        NVSG_API WriteLockImpl( const BufferSharedPtr &buffer, Buffer::MapMode mapMode, size_t offset, size_t length );
        NVSG_API ~WriteLockImpl();

        BufferSharedPtr m_buffer;
        BufferWriteLock m_bufferLock;
        void *m_ptr;
      };
    private:
      nvutil::SmartPtr<WriteLockImpl> m_lock;
    };

    // FIXME rename to WriteLockIterator
    template <typename ValueType>
    struct Iterator
    {
      typedef typename nvutil::StridedIterator<ValueType, DataWriteLock> Type;
    };

    // FIXME rename to ReadLockIterator
    template <typename ValueType>
    struct ConstIterator
    {
      typedef typename nvutil::StridedConstIterator<ValueType, DataReadLock> Type;
    };

    /** \brief Returns an StridedIterator to access the buffer. The buffer stays write-locked as long as an iterator constructed from
               the returned one exist.
        \param mapMode Access mode to this buffer
        \param offset offset in bytes of the first element to access
        \param strideInBytes Stride between two elements in the buffer in bytes. A stride of 0 will use siezof(ValueType) as stride.
     **/
    template <typename ValueType>
    typename Buffer::Iterator<ValueType>::Type getIterator( MapMode mapMode, size_t offset = 0, int strideInBytes = 0);

    /** \brief Returns an StridedConstIterator to access the buffer. The buffer stays read locked as long as an iterator constructed from
               the returned one exist.
        \param offset offset in bytes of the first element to access
        \param strideInBytes Stride between two elements in bytes. A stride of 0 will use siezof(ValueType) as stride.
     **/
    template <typename ValueType>
    typename ConstIterator<ValueType>::Type getConstIterator( size_t offset = 0, int strideInBytes = 0) const;

  protected:
    /**
     * \brief Retrieve pointer to a range within buffer data. Only use the pointers the way the MapMode describes it!
        \param mode desired MapMode to the buffer
        \param offset in bytes to start map from
        \param length in bytes for the mapped range
        \note Mapping with \c MAP_WRITE must not be used to retrieve data, and when using \c MAP_READ 
        you must not write data. Both operations are legal on the C++ side as you get a non-const pointer
        when mapping, but are illegal for the system's integrity. Buffer implementations will have undefined behavior
        when you use the MapMode wrongly.
        \sa unmap
     **/
    NVSG_API virtual void *map( MapMode mode ); // maps entire range
    NVSG_API virtual void *map( MapMode mode, size_t offset, size_t length ) = 0;
    NVSG_API virtual const void *mapRead() const; // map entire range on const object
    NVSG_API virtual const void *mapRead( size_t offset, size_t length ) const = 0; // map on const object

  
    /** \Brief Unmap the previously mapped buffer
     *  \sa map
     **/
    NVSG_API virtual void unmap( ) = 0;
    NVSG_API virtual void unmapRead( ) const = 0; // unmap on const object

    void *lock( MapMode mapMode );
    void *lock( MapMode mode, size_t offset, size_t length );
    const void *lockRead() const;
    const void *lockRead( size_t offset, size_t length ) const;
    void unlock();
    void unlockRead() const;

  private:
    mutable int m_lockCount;
    mutable void *m_mappedPtr;
    bool          m_managedBySystem;
  };

  inline bool Buffer::isManagedBySystem() const
  {
    return m_managedBySystem;
  }

  inline void* Buffer::map(MapMode mode)
  {
    return map( mode, 0, getSize() );
  }

  inline const void* Buffer::mapRead() const
  {
    return mapRead( 0, getSize() );
  }

  inline void Buffer::resize( size_t newSize )
  {
    size_t oldSize = getSize();
    if ( oldSize && newSize != oldSize )
    {
      size_t tmpSize = std::min<size_t>( getSize(), newSize );
      char * tmpData = (char *)malloc( tmpSize );
	  if (tmpData)
	  {
		  const char *oldData = reinterpret_cast<const char *>(mapRead());
		  memcpy( tmpData, oldData, tmpSize );
		  unmapRead();

		  setSize( newSize );

		  char *newData = reinterpret_cast<char *>(map( MAP_WRITE ));
		  memcpy( newData, tmpData, tmpSize );
		  unmap();

		  free( tmpData );
	  }
    }
    else
    {
      setSize( newSize );
    }
  }

  inline void *Buffer::lock( Buffer::MapMode mapMode )
  {
    nvutil::AutoLock lock(m_mutableLock);
    if (!m_lockCount)
    {
      m_mappedPtr = map( mapMode );
    }
    ++m_lockCount;
    return m_mappedPtr;
  }

  inline void *Buffer::lock( Buffer::MapMode mapMode, size_t offset, size_t length )
  {
    nvutil::AutoLock lock(m_mutableLock);
    if (!m_lockCount)
    {
      m_mappedPtr = map( mapMode, 0, getSize() );
    }
    ++m_lockCount;
    return (char *)m_mappedPtr + offset;
  }

  inline const void *Buffer::lockRead() const
  {
    nvutil::AutoLock lock(m_mutableLock);
    if (!m_lockCount)
    {
      m_mappedPtr = const_cast<void*>(mapRead( ));
    }
    ++m_lockCount;
    return m_mappedPtr;
  }

  inline const void *Buffer::lockRead( size_t offset, size_t length ) const
  {
    nvutil::AutoLock lock(m_mutableLock);
    if (!m_lockCount)
    {
      m_mappedPtr = const_cast<void*>(mapRead( 0, getSize() ));
    }
    ++m_lockCount;
    return (const char *)m_mappedPtr + offset;
  }

  inline void Buffer::unlock()
  {
    nvutil::AutoLock lock(m_mutableLock);
    --m_lockCount;
    if (!m_lockCount)
    {
      unmap();
      m_mappedPtr = 0;
    }
  }

  inline void Buffer::unlockRead() const
  {
    nvutil::AutoLock lock(m_mutableLock);
    --m_lockCount;
    if (!m_lockCount)
    {
      unmapRead();
      m_mappedPtr = 0;
    }
  }

  inline Buffer::DataReadLock::ReadLockImpl::ReadLockImpl( const BufferSharedPtr &buffer )
    : m_buffer( buffer )
    , m_bufferLock( buffer )
    , m_ptr(0)
  {
    NVSG_ASSERT( buffer );
    m_ptr = m_bufferLock->lockRead( );
  }

  inline Buffer::DataReadLock::ReadLockImpl::ReadLockImpl( const BufferSharedPtr &buffer , size_t offset, size_t length)
    : m_buffer( buffer )
    , m_bufferLock( buffer )
    , m_ptr(0)
  {
    NVSG_ASSERT( buffer );
    m_ptr = m_bufferLock->lockRead( offset, length );
  }

  inline Buffer::DataReadLock::ReadLockImpl::~ReadLockImpl()
  {
    m_bufferLock->unlockRead();
  }

  inline Buffer::DataWriteLock::WriteLockImpl::WriteLockImpl( const BufferSharedPtr &buffer, Buffer::MapMode mapMode )
    : m_buffer( buffer )
    , m_bufferLock( buffer )
    , m_ptr(0)
  {
    NVSG_ASSERT( buffer );
    m_ptr = m_bufferLock->lock( mapMode );
  }

  inline Buffer::DataWriteLock::WriteLockImpl::WriteLockImpl( const BufferSharedPtr &buffer, Buffer::MapMode mapMode, 
                                                          size_t offset, size_t length)
    : m_buffer( buffer )
    , m_bufferLock( buffer )
    , m_ptr(0)
  {
    NVSG_ASSERT( buffer );
    m_ptr = m_bufferLock->lock( mapMode, offset, length );
  }

  inline Buffer::DataWriteLock::WriteLockImpl::~WriteLockImpl()
  {
    m_bufferLock->unlock();
  }

  template <typename ValueType>
  typename Buffer::Iterator<ValueType>::Type Buffer::getIterator( MapMode mapMode, size_t offset, int strideInBytes )
  {
    DataWriteLock writeLock( nvutil::getWeakPtr<Buffer>(this), mapMode );
    char *basePtr = reinterpret_cast<char*>(writeLock.getPtr()) + offset;
    return typename Buffer::Iterator<ValueType>::Type( reinterpret_cast<ValueType *>(basePtr), strideInBytes ? strideInBytes : sizeof(ValueType), writeLock );
  }

  /** \brief Returns an StridedConstIterator to access the buffer. The buffer stays write-locked as long as an iterator constructed from
             the returned one exist.
   **/
  template <typename ValueType>
  typename Buffer::ConstIterator<ValueType>::Type Buffer::getConstIterator( size_t offset, int strideInBytes ) const
  {
    DataReadLock readLock( nvutil::getWeakPtr<Buffer>(this) );
    const char *basePtr = reinterpret_cast<const char*>(readLock.getPtr()) + offset;
    return ConstIterator<ValueType>::Type( reinterpret_cast<const ValueType *>(basePtr), strideInBytes ? strideInBytes : sizeof(ValueType), readLock );
  }

  /** \brief Copy elements from a strided source to a strided destination
   *  \param dst Destination for data
   *  \param dstOffset Offset in bytes relative to dst of the first element to write
   *  \param dstStride Stride between two elements in destination data
   *  \param src Source of data
   *  \param srcOffset Offset in bytes relative to src of the first element to read
   *  \param srcStride Stride between two elements in source data
   *  \param elementSize Size of a single data element to copy
   *  \param elementCount Number of elements to copy
   *  \remarks The result is undefined if src and dst overlap.
   **/
  NVSG_API void stridedMemcpy( void *dst, size_t dstOffset, size_t dstStride, const void *src, size_t srcOffset, size_t srcStride, size_t elementSize, size_t elementCount );
}

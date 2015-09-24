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
/** @file */

#include "nvsgcommon.h"
#include "nvsg/CoreTypes.h"
#include "nvsg/Types.h"
#include "nvutil/HashGenerator.h"
#include "nvutil/RCObject.h" // base class definition
#include "nvsg/Buffer.h"
#include "nvsg/Primitive.h"

namespace nvsg
{
  /*! \brief Provides an interface to handle indices to be attached to a Primitive
   *  \sa Primitive */
  class IndexSet : public OwnedObject<Primitive>
  {
    public:
      /*! \brief Creates a new IndexSet */
      static NVSG_API IndexSetSharedPtr create();

      /*! \brief Default constructor. */
      NVSG_API IndexSet();

      /*! \brief Destructs a IndexSet */
      NVSG_API ~IndexSet();

      /*! \brief Copy constructor.
       *  \param rhs A reference to the constant IndexSet to copy from
       */
      NVSG_API IndexSet( const IndexSet& rhs );

      /*! \brief Convenience function to set the indices.
       *  \param indices A pointer to the constant indices to use.
       *  \param count The number of indices.
       *  \param primitiveRestartIndex The primitiveRestartIndex - see setPrimitiveRestartIndex for info.
       *  \remarks Copies \a count elements from \a indices into this IndexSet, and sets the data type, primitive restart index, and number of
       *  indices.
       *  \note SceniX does not make a separate copy of the data, it stores the data in the attached buffer.  If no buffer exists,
       *  one will be created.  To extract the indices, the application must manipulate the buffer or call getData().
       *  \note The behavior is undefined if there are less than \a count * sizeof( *indices ) bytes of data located at \a indices.
       *  \note This is a convenience function to reference the attached Buffer.  If no buffer exists, one will be allocated, resized and
       *  the data will be written to the buffer.  Beware that if a buffer does exist, it will be resized and data will be written to the buffer.  
       *  See setBuffer for more details.
       *  \note Since this method makes assumptions about buffer sizes, it may be faster to manipulate the buffer directly.
       *  \sa getNumberOfIndices, appendIndexData, setBuffer 
       */
      NVSG_API void setData( const unsigned int   * indices, unsigned int count, unsigned int primitiveRestartIndex = ~0 );
      NVSG_API void setData( const unsigned short * indices, unsigned int count, unsigned int primitiveRestartIndex = ~0 );
      NVSG_API void setData( const unsigned char  * indices, unsigned int count, unsigned int primitiveRestartIndex = ~0 );
      NVSG_API void setData( const void * indices, unsigned int count, unsigned int type = nvsg::NVSG_UNSIGNED_INT, 
                                                                       unsigned int primitiveRestartIndex = ~0 );

      /*! \brief set the buffer object to use for this IndexSet
       *  \param buffer The buffer to assign to this IndexSet.
       *  \param count The number of indices packed in the buffer.
       *  \param type Specifies the data type of each index in the buffer.  Valid values are  NVSG_UNSIGNED_BYTE, NVSG_UNSIGNED_SHORT, or NVSG_UNSIGNED_INT.
       *  \param primitiveRestartIndex The primitiveRestartIndex - see setPrimitiveRestartIndex for info.
       *  \remarks Copies \a count elements from \a indices into this IndexSet, and sets the data type, primitive restart index, and number of
       *  indices.
       *  \remarks Applications are free to use the convenience function setData() to allocate and fill a buffer, or they may fill a buffer directly 
       *  and attach it to this IndexSet.
       *  \sa setData
       */
      NVSG_API void setBuffer( const BufferSharedPtr &buffer, unsigned int count,
                                                              unsigned int type = nvsg::NVSG_UNSIGNED_INT,
                                                              unsigned int primitiveRestartIndex = ~0 );

      /*! \brief Get the buffer attached to this IndexSet 
       *  \return The buffer
       */
      NVSG_API const BufferSharedPtr & getBuffer() const;

      /*! \brief Convenience function to get the indices.
       *  \param destination A pointer to a destination buffer.
       *  \return \c true if there was an attached buffer and any data has been copied into \a destination; \c false otherwise.
       *  \remarks Copies all data from attached buffer into \a destination pointer.  Results are undefined if \a destination is
       *  not large enough to hold all the data.
       *  \note In order to determine the required buffer size, multiply the number of indices in the buffer by the byte size of
       *  the index data type, ie: 
       *
       *  size_t bytes = iset->getNumberOfIndices() * nvsg::sizeOfType( iset->getIndexDataType() );
       */
      NVSG_API bool getData( void * destination ) const;

      /*! \brief Get the number of indices that have been attached to this IndexSet 
       *  \return The count of indices.
       */
      NVSG_API unsigned int getNumberOfIndices() const;

      /*! \brief Set the "Primitive Restart" index value
       * \param index The index value to use.  At runtime, the value will be masked to use only the number of bits in the index data type 
       * (int, short, byte) before being used by the renderer.  Typically the application will use ~0 as the restart index to avoid interfering 
       * with an actual index.
       * \remarks Primitive Restart is typically used with [LINE|TRIANGLE|QUAD]_STRIP[_ADJACENCY], LINE_LOOP, and TRIANGLE_FAN primitive types, 
       * in order to specify multiple primitives at once.  While Primitive Restart can be used with other primitive types, it will have no benefit.\n
       * When this index is encountered, the primitive type will be restarted as if a second primitive were specified.
       * \note In order to use Primitive Restart, an index must be selected using this method, and the buffer must be packed appropriately 
       * with that Primitive Restart index.
       * \note The default primitive restart index is 0xFFFFFFFF.
       */
      NVSG_API void setPrimitiveRestartIndex( unsigned int index );
      NVSG_API unsigned int getPrimitiveRestartIndex() const;

      /* Set index data type
       *  \param type Specifies the data type of each index in the input data array.  Valid values are  NVSG_UNSIGNED_BYTE, 
       *  NVSG_UNSIGNED_SHORT, or NVSG_UNSIGNED_INT.
       *  \note The default format is NVSG_UNSIGNED_INT
       *  \sa setPrimitiveRestartIndex, getPrimitiveRestartIndex
       */
      NVSG_API void setIndexDataType( unsigned int type );
      NVSG_API unsigned int getIndexDataType() const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant IndexSet to copy from.
       *  \return A reference to the assigned IndexSet.
       */
      NVSG_API IndexSet & operator=( const IndexSet & rhs );

      /*! \brief Test for equivalence with an other IndexSet.
       *  \param p A pointer to the constant IndexSet to test for equivalence with.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the IndexSet \a p is equivalent to \c this, otherwise \c false.
       *  \remarks Two IndexSets are equivalent if their data type, index count, and primitive restart index are equal;  and
       *  if \a deepCompare is \c true, their buffer size and contents are also equal.
       *  \sa Primitive, Buffer */
      NVSG_API bool isEquivalent( const IndexSet *p, bool deepCompare ) const;

      NVSG_API virtual void notifyChange( const Subject * originator, unsigned int state = 0 ) const;

      /*! \brief ConstIterator to iterate the indices as a particular index type
       */
      template <typename T>
      class ConstIterator
      {
        public:
          ConstIterator( const nvsg::IndexSetSharedPtr & iset, unsigned int offset = 0 );
          ConstIterator( const ConstIterator& rhs );
          ConstIterator& operator=( const ConstIterator& rhs );
          ~ConstIterator() {}

          T operator*() const;
          T operator[](size_t index) const;
          ConstIterator& operator++();   // pre-increment
          ConstIterator  operator++(int) const; // post-increment
          ConstIterator  operator+(int offset) const;

        protected:
          Buffer::DataReadLock  m_readlock;
          const void          * m_basePtr;
          unsigned int          m_type;
          unsigned int          m_offset;
      };

      REFLECTION_INFO_API( NVSG_API, IndexSet );
    protected:
      NVSG_API void copyDataToBuffer( const void * ptr, unsigned int count );

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

      NVSG_API virtual void incrementIndexSetIncarnation() const;
      NVSG_API virtual const nvutil::Incarnation & queryIndexSetIncarnation() const;

    private:
      unsigned int    m_dataType;
      unsigned int    m_primitiveRestartIndex;
      unsigned int    m_numberOfIndices;
      BufferSharedPtr m_buffer;
      const Subject  *m_bufferSubject;

      mutable nvutil::Incarnation m_indexSetIncarnation; // tracks unspecified indexset changes
  };

  inline unsigned int IndexSet::getNumberOfIndices() const
  {
    NVSG_TRACE();
    return m_numberOfIndices;
  }

  inline unsigned int IndexSet::getIndexDataType() const
  {
    NVSG_TRACE();
    return m_dataType;
  }

  inline unsigned int IndexSet::getPrimitiveRestartIndex() const
  {
    NVSG_TRACE();
    return m_primitiveRestartIndex;
  }

  inline const BufferSharedPtr & IndexSet::getBuffer() const
  {
    NVSG_TRACE();
    return m_buffer;
  }

  template< typename T >
  inline IndexSet::ConstIterator<T>::ConstIterator( const IndexSetSharedPtr & iset, unsigned int offset )
    : m_readlock( IndexSetReadLock(iset)->getBuffer() )
    , m_type( IndexSetReadLock(iset)->getIndexDataType() )
  {
    // compute offset from base - no reason to store the offset
    m_basePtr = m_readlock.getPtr<unsigned char>() + (offset * nvsg::sizeOfType( m_type ));
  }

  template< typename T >
  inline IndexSet::ConstIterator<T>::ConstIterator( const IndexSet::ConstIterator<T> & rhs )
    : m_readlock( rhs.m_readlock )
    , m_type( rhs.m_type )
    , m_basePtr( rhs.m_basePtr )
  {
  }

  template< typename T >
  inline IndexSet::ConstIterator<T>& IndexSet::ConstIterator<T>::operator=( const IndexSet::ConstIterator<T> & rhs )
  {
    m_readlock = rhs.m_readlock;
    m_type     = rhs.m_type;
    m_basePtr  = rhs.m_basePtr;
    m_offset   = rhs.m_offset;

    return *this;
  }

  template< typename T >
  inline IndexSet::ConstIterator<T> IndexSet::ConstIterator<T>::operator+( int offset ) const
  {
    ConstIterator<T> tn( *this );

    tn.m_basePtr = reinterpret_cast< const unsigned char * >( m_basePtr ) + (offset * nvsg::sizeOfType( m_type ));

    return tn;
  }

  template< typename T >
  inline IndexSet::ConstIterator<T> IndexSet::ConstIterator<T>::operator++(int) const
  {
    return *this + 1;
  }

  template< typename T >
  inline IndexSet::ConstIterator<T>& IndexSet::ConstIterator<T>::operator++()
  {
    m_basePtr = reinterpret_cast< const unsigned char * >( m_basePtr ) + nvsg::sizeOfType( m_type );

    return *this;
  }

  template< typename T >
  inline T IndexSet::ConstIterator<T>::operator*() const
  {
    switch( m_type )
    {
      case NVSG_UNSIGNED_INT:
        return checked_cast<T>( *reinterpret_cast<const unsigned int *>(m_basePtr) );
      case NVSG_UNSIGNED_SHORT:
        return checked_cast<T>( *reinterpret_cast<const unsigned short *>(m_basePtr) );
      case NVSG_UNSIGNED_BYTE:
        return checked_cast<T>( *reinterpret_cast<const unsigned char *>(m_basePtr) );
      default:
        NVSG_ASSERT(0);
        return (T)(~0);
    }
  }

  template< typename T >
  inline T IndexSet::ConstIterator<T>::operator[]( size_t index ) const
  {
    switch( m_type )
    {
      case NVSG_UNSIGNED_INT:
        return checked_cast<T>( reinterpret_cast<const unsigned int *>(m_basePtr)[index] );
      case NVSG_UNSIGNED_SHORT:
        return checked_cast<T>( reinterpret_cast<const unsigned short *>(m_basePtr)[index] );
      case NVSG_UNSIGNED_BYTE:
        return checked_cast<T>( reinterpret_cast<const unsigned char *>(m_basePtr)[index] );
      default:
        NVSG_ASSERT(0);
        return (T)(~0);
    }
  }

  inline void IndexSet::incrementIndexSetIncarnation() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
    m_indexSetIncarnation++;
  }

  inline const nvutil::Incarnation & IndexSet::queryIndexSetIncarnation() const
  {
    return( m_indexSetIncarnation );
  }

} //  namespace nvsg


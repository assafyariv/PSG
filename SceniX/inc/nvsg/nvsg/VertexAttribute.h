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

#pragma once
/** @file */

#include "nvsgcommon.h"
#include "nvutil/HashGenerator.h"
#include "nvsg/Buffer.h"

namespace nvsg
{
  /*! \brief Class to hold the data of one vertex attribute.
   *  \remarks A VertexAttribute holds \c VertexDataCount elements, each one with \c VertexDataSize
   *  components or coordinates, each one in turn of type \c VertexDataType. For a set of
   *  normals for example, \c VertexDataSize is three and the \c VertexDataType is NVSG_FLOAT.
   *  \sa VertexAttributeSet */
  class VertexAttribute
  {
    public:
      typedef unsigned char mem_t;
      typedef mem_t * memptr_t;

    public:
      /*! \brief Default constructor for an empty VertexAttribute. */
      NVSG_API VertexAttribute();

      /*! \brief Copy Constructor.
       *  \param rhs VertexAttribute to copy from. */
      NVSG_API VertexAttribute(const VertexAttribute& rhs);

      /*! \brief Destructor of a VertexAttribute. */
      NVSG_API ~VertexAttribute();

      /*! \brief Exchanges the data of two VertexAttributes.
       *  \param rhs A VertexAttribute whose data is to be exchanged with this VertexAttribute. 
       *  \remarks You should always prefer it over an assignment if you no longer need the assigned
       *  VertexAttribute \a rhs. It offers much better efficiency because it swaps only the internal
       *  data of the two VertexAttributes and is guaranteed to have only constant complexity, 
       *  instead of the linear complexity of an assignment. */
      NVSG_API void swapData(VertexAttribute& rhs);

      /*! \brief Reserves space for data in this VerexAttribute.
       *  \param size Specifies the number of coordinates per vertex; must be 1, 2, 3, or 4.
       *  \param type Specifies the data type of each coordinate in the input data array. In general,
       *  sympbolic constants NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT, NVSG_UNSIGNED_SHORT,
       *  NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted.
       *  \param count Specifies the number of vertex data to copy to reserve space for.
       *  \remarks If no buffer has been allocated before a new buffer will be allocated. If the buffer size
       *  is less than the given size the buffer will be resized accordingly. Otherwise the size of the buffer
       *  remains unchanged.
       *  \sa getData, setData, removeData, getVertexDataCount, getVertexDataSize, getVertexDataType,
       *  getVertexDataBytes */
      NVSG_API void reserveData( unsigned int size, unsigned int type, unsigned int count );

      /*! \brief Copies data into this VertexAttribute.
       *  \param size Specifies the number of coordinates per vertex; must be 1, 2, 3, or 4.
       *  \param type Specifies the data type of each coordinate in the input data array. In general,
       *  sympbolic constants NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT, NVSG_UNSIGNED_SHORT,
       *  NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted.
       *  \param data Specifies the start address of the input data array.
       *  \param strideInBytes Specifies the stride between two elements in data. A stride of 0 assumes packed data.
       *  \param count Specifies the number of vertex data to copy.
       *  \remarks This function copies data from the specified location and according to the
       *  indicated data format. Data that previously was specified will be released prior to copy
       *  the new data.
       *  \sa getData, removeData, reserveData, getVertexDataCount, getVertexDataSize,
       *  getVertexDataType, getVertexDataBytes */
      NVSG_API void setData(unsigned int size, unsigned int type, const void * data, unsigned int strideInBytes, unsigned int count);

      /*! \brief Overwrites data in this VertexAttribute.
       *  \param pos Marks the position of the first vertex inside this VertexAttribute, where
       *  overwriting should start. If the magic value ~0 is specified, the input data will be
       *  appended to the array of vertex data.
       *  \param size Specifies the number of coordinates per vertex; must be 1, 2, 3, or 4.
       *  \param type Specifies the data type of each coordinate in the input data array. In general,
       *  symbolic constants NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT, NVSG_UNSIGNED_SHORT,
       *  NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted.
       *  \param data Specifies the start address of the input data array.
       *  \param strideInBytes Specifies the stride between two elements in data. A stride of 0 assumes packed data.
       *  \param count Specifies the number of vertex data to copy.
       *  \remarks This function overload re-specifies previously copied vertex data starting at the
       *  indicated position. If the magic value ~0 is specified for \a pos, the input vertex data
       *  will be appended to the previously copied vertex data. The underlying buffer will be resized to
       *  #elements * stride of VA + offset. The behaviour is undefined if this function will be invoked on VertexAttribute objects,
       *  where the underlying buffer contains data for multiple attributes which are not interleaved.
       *  It is safe to use this function if there is only one chunk of interleaved data in this buffer.
       *  \sa getData, removeData, reserveData, getVertexDataCount, getVertexDataSize,
       *  getVertexDataType, getVertexDataBytes */
      NVSG_API void setData(unsigned int pos, unsigned int size, unsigned int type, const void * data, unsigned int strideInBytes, unsigned int count);

      /*! \brief Sets the buffer of this vertex attribute
       *  \param size Specifies the number of coordinates per vertex; must be 1, 2, 3, or 4.
       *  \param type Specifies the data type of each coordinate in the input data array. In general,
       *  sympbolic constants NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT, NVSG_UNSIGNED_SHORT,
       *  NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted.
       *  \param buffer Specifies the buffer to use for this VertexAttribute.
       *  \param offset Offset of the first element in the buffer in bytes
       *  \param strideInBytes stride between two vertex elements in the buffer.
       *         A stride of 0 will calculate the stride depending on the size and type of an vertex element.
       *  \param count Specifies the number of vertex data to copy.
       *  \remarks This function uses the given buffer as VertexData using the indicated data format.
       *           No data is being copied in this function call.
       *  \sa getData, removeData, reserveData, getVertexDataCount, getVertexDataSize,
       *  getVertexDataType, getVertexDataBytes */
      NVSG_API void setData(unsigned int size, unsigned int type, const BufferSharedPtr &buffer, unsigned int offset, unsigned int strideInBytes, unsigned int count);

      /*! \brief Get a constant pointer to the vertex data of this VertexAttribute.
       *  \return The functions returns a type-less pointer to the vertex data of this
       *  VertexAttribute. If no vertex data was specified before, the function returns NULL.
       *  \remarks The function returns a type-less pointer to the stored vertex data. To access
       *  the vertex data from this pointer it requires the determination of additional vertex data
       *  traits: Use getVertexDataType to determine the per-coordinate type for each vertex in the
       *  vertex array, getVertexDataSize retrieves the number of coordinates per vertex, and
       *  getVertexDataCount returns the number of vertices in the vertex array.
       *  \sa setData, removeData, getVertexDataCount, getVertexDataSize, getVertexDataType,
       *  getVertexDataBytes */
      template <typename ValueType>
      typename Buffer::ConstIterator<ValueType>::Type getData() const;

      /*! \brief Get a pointer to the vertex data of this VertexAttribute.
       *  \return The functions returns a type-less pointer to the vertex data of this
       *  VertexAttribute. If no vertex data was specified before, the function returns NULL.
       *  \remarks The function returns a type-less pointer to the stored vertex data. To access
       *  the vertex data from this pointer it requires the determination of additional vertex data
       *  traits: Use getVertexDataType to determine the per-coordinate type for each vertex in the
       *  vertex array, getVertexDataSize retrieves the number of coordinates per vertex, and
       *  getVertexDataCount returns the number of vertices in the vertex array.
       *  \sa setData, removeData, getVertexDataCount, getVertexDataSize, getVertexDataType,
       *  getVertexDataBytes */
      template <typename ValueType>
      typename Buffer::Iterator<ValueType>::Type getData();

      /*! \brief Get an iterator to the vertex data of this attribute
          \return Iterator to the first element of this attribute
      **/
      template <typename ValueType>
      typename Buffer::Iterator<ValueType>::Type begin();

      /*! \brief Get an const iterator to the vertex data of this attribute.
          \return Iterator to the first element of this attribute
      **/
      template <typename ValueType>
      typename Buffer::ConstIterator<ValueType>::Type beginRead() const;

      /*! \brief Get an iterator to the vertex data of this attribute.
          \return Iterator behind the last element of this attribute
      **/
      template <typename ValueType>
      typename Buffer::Iterator<ValueType>::Type end();

      /*! \brief Get an const iterator to the vertex data of this attribute.
          \return Iterator behind the last element of this attribute
      **/
      template <typename ValueType>
      typename Buffer::ConstIterator<ValueType>::Type endRead() const;

      /*! \brief Remove the vertex data assigned to this VertexAttribute.
       *  \remarks The function releases the vertex data previously specified by setData.
       *  \sa setData, getData */
      NVSG_API void removeData();

      /*! \brief Get the number of vertex data elements in this VertexAttribute.
       *  \sa setData, getVertexDataSize, getVertexDataType, getVertexDataBytes */
      NVSG_API unsigned int getVertexDataCount() const;

      /*! \brief Get the size of one vertex data element in this VertexAttribute.
       *  \return The function returns the number of coordinates per vertex data element. This can
       *  be 1, 2, 3, or 4. If no vertex data was specified before, the function returns 0.
       *  \sa setData, getVertexDataCount, getVertexDataType, getVertexDataBytes */
      NVSG_API unsigned int getVertexDataSize()  const;

      /*! \brief Get the type identifier of the coordinates of the vertex data elements.
       *  \return The function returns a symbolic constant indicating the type of each coordinate of
       *  the vertex data elements. If no vertex data was specified, the function returns
       *  NVSG_UNSUPPORTED_TYPE.
       *  \remarks The function returns the type specifier that was used with a corresponding call
       *  to setData. Use getVertexDataSize to query the number of coordinates for each vertex data.
       *  You can use the nvsg::sizeOfType convenient function to determine the size in bytes of the
       *  type returned by this function.
       *  \sa setData, getVertexDataCount, getVertexDataSize, getVertexDataBytes, nvsg::sizeOfType */
      NVSG_API unsigned int getVertexDataType()  const;

      /*! \brief Get the size of one vertex data element in bytes.
       *  \return The function returns the size of a vertex data element in bytes.
       *  \remarks This is simply the cached result of getVertexDataSize * sizeOfType(getVertexDataType).
       *  \sa setData, getVertexDataCount, getVertexDataSize, getVertexDataType, nvsg::sizeOfType */
      NVSG_API unsigned int getVertexDataBytes() const;

      /*! \brief Get the stride between two vertex data elements in the buffer
       *  \return Stride in bytes
       */
      NVSG_API unsigned int getVertexDataStrideInBytes() const;

      /*! \brief Get the offset of the first element in the buffer in bytes
       *  \return Offset in bytes
       */
      NVSG_API unsigned int getVertexDataOffsetInBytes() const;

      /*! \brief Get the buffer which is used to store vertex data
       *  \return The Buffer of this VertexAttribute
      **/
      NVSG_API const BufferSharedPtr & getBuffer() const;

      /*! \brief Check whether the vertex attribute is stored in contiguous memory.
       *  \return Returns \c true if stride is equal to vertex size, otherwise \c false,
       *  which may mean the data is interleaved.
       */
      NVSG_API bool isContiguous() const;

      /*! \brief Assignment operator.
       *  \param rhs The VertexAttribute to assign \c this with.
       *  \return A reference to \c this.
       *  \remarks The complete data of \a rhs is copied over to \c this. */
      NVSG_API VertexAttribute & operator=( const VertexAttribute & rhs );

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \remarks Floating point data of a VertexAttribute is not used exactly to update the hash string.
       *  It's just the first three (seven) bytes of each value used for float (double) data. That way,
       *  VertexAttributes, that are only approximately equal get the same hash string. That is, you have
       *  to explicitly check for equality, even if the hash string is equal.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      //initiatialize m_size, m_type and m_bytes
      void initData( unsigned int size, unsigned int type );

    private:      
      unsigned int    m_count;  // # vertex data
      unsigned int    m_size;   // # coordinates per vertex 
      unsigned int    m_type;   // symbolic constant indicating the type of coordinates
      unsigned int    m_bytes;  // size of vertex in bytes
      unsigned int    m_offset;
      unsigned int    m_strideInBytes; // stride in bytes between two elements in the buffer
      BufferSharedPtr m_buffer;
  };

  /*! \brief Normalize all vertex data elements of a VertexAttribute.
   *  \param va The VertexAttribute to normalize.
   *  \remarks The function \c normalize is called for every vertex data element. */
  NVSG_API void normalize( VertexAttribute & va );

  /*! \brief Equality operator.
   *  \param va0 First VertexAttribute to compare.
   *  \param va1 Second VertexAttribute to compare.
   *  \return \c true, if \a va0 and \a va1 are equal, otherwise \c false.
   *  \sa operator!= */
  NVSG_API bool operator==( const VertexAttribute & va0, const VertexAttribute & va1 );

  /*! \brief Inequality operator.
   *  \param va0 First VertexAttribute to compare.
   *  \param va1 Second VertexAttribute to compare.
   *  \return \c true, if \a va0 and \a va1 are not equal, otherwise \c false.
   *  \sa operator== */
  NVSG_API bool operator!=( const VertexAttribute & va0, const VertexAttribute & va1 );


  template <typename ValueType>
  inline typename Buffer::ConstIterator<ValueType>::Type VertexAttribute::getData() const
  {
    return m_buffer ? beginRead<ValueType>() : Buffer::ConstIterator<ValueType>();
  }

  template <typename ValueType>
  inline typename Buffer::Iterator<ValueType>::Type VertexAttribute::getData()
  {
    return m_buffer ? begin<ValueType>() : Buffer::Iterator<ValueType>();
  }

  inline unsigned int VertexAttribute::getVertexDataCount() const
  {
    return( m_count );
  }

  inline unsigned int VertexAttribute::getVertexDataSize() const
  {
    return( m_size );
  }

  inline unsigned int VertexAttribute::getVertexDataType() const
  {
    return( m_type );
  }

  inline unsigned int VertexAttribute::getVertexDataBytes() const
  {
    return( m_bytes );
  }

  inline unsigned int VertexAttribute::getVertexDataStrideInBytes() const
  {
    return( m_strideInBytes );
  }

  inline unsigned int VertexAttribute::getVertexDataOffsetInBytes() const
  {
    return( m_offset );
  }

  inline const BufferSharedPtr & VertexAttribute::getBuffer() const
  {
    return( m_buffer );
  }

  inline bool VertexAttribute::isContiguous() const
  {
    return( m_strideInBytes == m_bytes );
  }

  inline bool operator==( const VertexAttribute & va0, const VertexAttribute & va1 )
  {
    return(     ( va0.getVertexDataBytes() == va1.getVertexDataBytes() )
            &&  ( va0.getVertexDataStrideInBytes() == va1.getVertexDataStrideInBytes() )
            &&  ( va0.getVertexDataOffsetInBytes() == va1.getVertexDataOffsetInBytes() )
            &&  ( va0.getVertexDataCount() == va1.getVertexDataCount() )
            &&  ( va0.getVertexDataSize()  == va1.getVertexDataSize() )
            &&  ( va0.getVertexDataType()  == va1.getVertexDataType() )
            &&  ( va0.getBuffer() == va1.getBuffer() )
           );
  }

  inline bool operator!=( const VertexAttribute & va0, const VertexAttribute & va1 )
  {
    return( ! ( va0 == va1 ) );
  }

  template <typename ValueType>
  typename Buffer::Iterator<ValueType>::Type VertexAttribute::begin()
  {
    Buffer::DataWriteLock lock = Buffer::DataWriteLock( m_buffer, Buffer::MAP_READWRITE, m_offset, m_count * m_strideInBytes );
    return typename Buffer::Iterator<ValueType>::Type( lock.getPtr<ValueType>(), m_strideInBytes, lock );
  }

  template <typename ValueType>
  typename Buffer::ConstIterator<ValueType>::Type VertexAttribute::beginRead() const
  {
    Buffer::DataReadLock lock = Buffer::DataReadLock( m_buffer, m_offset, m_count * m_strideInBytes );
    return typename Buffer::ConstIterator<ValueType>::Type( lock.getPtr<ValueType>(), m_strideInBytes, lock );
  }

  template <typename ValueType>
  typename Buffer::Iterator<ValueType>::Type VertexAttribute::end()
  {
    Buffer::DataWriteLock lock = Buffer::DataWriteLock( m_buffer, Buffer::MAP_READWRITE, m_offset, m_count * m_strideInBytes );
    void *end = lock.getPtr<char>() + m_count * m_strideInBytes;
    return typename Buffer::Iterator<ValueType>::Type( reinterpret_cast<ValueType*>(end) , m_strideInBytes, lock );
  }

  template <typename ValueType>
  typename Buffer::ConstIterator<ValueType>::Type VertexAttribute::endRead() const
  {
    Buffer::DataReadLock lock = Buffer::DataReadLock( m_buffer, m_offset, m_count * m_strideInBytes );
    const void *end = lock.getPtr<char>() + m_count * m_strideInBytes;
    return typename Buffer::ConstIterator<ValueType>::Type( reinterpret_cast<const ValueType*>(end) , m_strideInBytes, lock );
  }
} //  namespace nvsg


namespace nvmath
{
  /*! \brief Linear interpolation between two VertexAttributes.
   *  \param alpha The interpolation parameter.
   *  \param va0 The starting value for the interpolation.
   *  \param va1 The ending value for the interpolation.
   *  \param var The resulting value of the interpolation.
   *  \remarks Two VertexAttributes are linearly interpolated by linearly interpolating each of the
   *  corresponding vertex data elements. */
  NVSG_API void lerp( float alpha, const nvsg::VertexAttribute & va0
                    , const nvsg::VertexAttribute & va1, nvsg::VertexAttribute & var );
} //  namespace nvmath

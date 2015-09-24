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

#include <iterator>

namespace nvutil
{

  struct DummyPayload {};

  /*******************/
  /* StridedIterator */
  /*******************/
  template <typename ValueType, typename Payload = DummyPayload>
  class StridedIterator : public std::iterator< std::input_iterator_tag, ValueType >
  {
  public:
    StridedIterator();
    StridedIterator( ValueType *basePtr, size_t strideInBytes, Payload = Payload() );
    StridedIterator( const StridedIterator &rhs );
    StridedIterator &operator=( const StridedIterator &rhs );
    virtual ~StridedIterator() {}

    ValueType &operator*();
    ValueType &operator[](size_t index);
    bool operator==(const StridedIterator &rhs) const; // compares only ptr!
    bool operator!=(const StridedIterator &rhs) const; // compares only ptr!
    StridedIterator operator+(int offset) const;
    StridedIterator &operator++();  // pre-increment
    StridedIterator operator++(int);  //post-increment

    typedef std::input_iterator_tag iterator_category;
    typedef ValueType value_type;
  protected:
    ValueType *m_basePtr;
    size_t     m_strideInBytes;
    Payload    m_payload;
  };

  template <typename ValueType, typename Payload>
  inline StridedIterator<ValueType, Payload>::StridedIterator( ) 
    : m_basePtr(0)
    , m_strideInBytes(0)
  {
  }

  template <typename ValueType, typename Payload>
  inline StridedIterator<ValueType, Payload>::StridedIterator( ValueType *basePtr, size_t strideInBytes, Payload payload )
    : m_basePtr( basePtr )
    , m_strideInBytes( strideInBytes )
    , m_payload( payload )
  {
  }

  template <typename ValueType, typename Payload>
  inline StridedIterator<ValueType,Payload>::StridedIterator( const StridedIterator &rhs )
    : m_basePtr( rhs.m_basePtr )
    , m_strideInBytes( rhs.m_strideInBytes )
    , m_payload( rhs.m_payload )
  {
  }

  template <typename ValueType, typename Payload>
  inline StridedIterator<ValueType,Payload> &StridedIterator<ValueType,Payload>::operator=( const StridedIterator &rhs )
  {
    m_basePtr = rhs.m_basePtr;
    m_strideInBytes = rhs.m_strideInBytes;
    m_payload = rhs.m_payload;

    return *this;
  }

  template <typename ValueType, typename Payload>
  ValueType &StridedIterator<ValueType,Payload>::operator*()
  {
    return *reinterpret_cast<ValueType *>(m_basePtr);
  }

  template <typename ValueType, typename Payload>
  ValueType &StridedIterator<ValueType,Payload>::operator[](size_t index)
  {
    char *ptr = reinterpret_cast<char *>(m_basePtr);
    ptr += m_strideInBytes * index;
    return *reinterpret_cast<ValueType *>(ptr);
  }

  template <typename ValueType, typename Payload>
  StridedIterator<ValueType,Payload> StridedIterator<ValueType,Payload>::operator+(int offset) const
  {
    char *ptr = reinterpret_cast<char *>(m_basePtr);
    ptr += offset * m_strideInBytes;
    return StridedIterator<ValueType, Payload>( ptr, m_strideInBytes, m_payload );
  }

  template <typename ValueType, typename Payload>
  StridedIterator<ValueType,Payload> &StridedIterator<ValueType,Payload>::operator++()
  {
    char *ptr = reinterpret_cast<char *>(m_basePtr);
    ptr += m_strideInBytes;
    m_basePtr = reinterpret_cast<ValueType *>(ptr);
    return *this;
  }

  template <typename ValueType, typename Payload>
  StridedIterator<ValueType,Payload> StridedIterator<ValueType,Payload>::operator++(int)
  {
    StridedIterator copy(*this);
    ++(*this);
    return copy;
  }

  template <typename ValueType, typename Payload>
  bool StridedIterator<ValueType,Payload>::operator==( const StridedIterator<ValueType, Payload> &rhs ) const
  {
    return m_basePtr == rhs.m_basePtr;
  }

  template <typename ValueType, typename Payload>
  bool StridedIterator<ValueType,Payload>::operator!=( const StridedIterator<ValueType, Payload> &rhs ) const
  {
    return m_basePtr != rhs.m_basePtr;
  }

  /************************/
  /* StridedConstIterator */
  /************************/
  template <typename ValueType, typename Payload = DummyPayload>
  class StridedConstIterator : public std::iterator< std::input_iterator_tag, ValueType >
  {
  public:
    StridedConstIterator( );
    StridedConstIterator( const ValueType *basePtr, size_t strideInBytes, Payload = Payload() );
    StridedConstIterator( const StridedConstIterator &rhs );
    StridedConstIterator &operator=( const StridedConstIterator &rhs );
    virtual ~StridedConstIterator() {}

    const ValueType &operator*() const;
    const ValueType &operator[](size_t index) const;
    bool operator==(const StridedConstIterator &rhs) const; // compares only ptr!
    bool operator!=(const StridedConstIterator &rhs) const; // compares only ptr!
    StridedConstIterator operator+(size_t offset) const;
    StridedConstIterator &operator++();  // pre-increment
    StridedConstIterator operator++(int);  //post-increment
  protected:
    const ValueType *m_basePtr;
    size_t           m_strideInBytes;
    Payload          m_payload;
  };

  template <typename ValueType, typename Payload>
  inline StridedConstIterator<ValueType, Payload>::StridedConstIterator( )
    : m_basePtr(0)
    , m_strideInBytes(0)
  {
  }

  template <typename ValueType, typename Payload>
  inline StridedConstIterator<ValueType, Payload>::StridedConstIterator( const ValueType *basePtr, size_t strideInBytes, Payload payload )
    : m_basePtr( basePtr )
    , m_strideInBytes( strideInBytes )
    , m_payload( payload )
  {
  }

  template <typename ValueType, typename Payload>
  inline StridedConstIterator<ValueType,Payload>::StridedConstIterator( const StridedConstIterator &rhs )
    : m_basePtr( rhs.m_basePtr )
    , m_strideInBytes( rhs.m_strideInBytes )
    , m_payload( rhs.m_payload )
  {
  }

  template <typename ValueType, typename Payload>
  inline StridedConstIterator<ValueType,Payload> &StridedConstIterator<ValueType,Payload>::operator=( const StridedConstIterator &rhs )
  {
    m_basePtr = rhs.m_basePtr;
    m_strideInBytes = rhs.m_strideInBytes;
    m_payload = rhs.m_payload;

    return *this;
  }

  template <typename ValueType, typename Payload>
  const ValueType &StridedConstIterator<ValueType,Payload>::operator*() const
  {
    return *reinterpret_cast<const ValueType *>(m_basePtr);
  }

  template <typename ValueType, typename Payload>
  const ValueType &StridedConstIterator<ValueType,Payload>::operator[](size_t index) const
  {
    const char *ptr = reinterpret_cast<const char *>(m_basePtr);
    ptr += m_strideInBytes * index;
    return *reinterpret_cast<const ValueType *>(ptr);
  }

  template <typename ValueType, typename Payload>
  StridedConstIterator<ValueType,Payload> StridedConstIterator<ValueType,Payload>::operator+( size_t offset ) const
  {
    const char *ptr = reinterpret_cast<const char *>(m_basePtr);
    ptr += offset * m_strideInBytes;
    return StridedConstIterator<ValueType, Payload>( reinterpret_cast<const ValueType *>(ptr), m_strideInBytes, m_payload );
  }

  template <typename ValueType, typename Payload>
  StridedConstIterator<ValueType,Payload> &StridedConstIterator<ValueType,Payload>::operator++()
  {
    const char *ptr = reinterpret_cast<const char *>(m_basePtr);
    ptr += m_strideInBytes;
    m_basePtr = reinterpret_cast<const ValueType *>(ptr);
    return *this;
  }

  template <typename ValueType, typename Payload>
  StridedConstIterator<ValueType,Payload> StridedConstIterator<ValueType,Payload>::operator++(int)
  {
    StridedConstIterator copy(*this);
    ++(*this);
    return copy;
  }

  template <typename ValueType, typename Payload>
  bool StridedConstIterator<ValueType,Payload>::operator==( const StridedConstIterator<ValueType, Payload> &rhs ) const
  {
    return m_basePtr == rhs.m_basePtr;
  }

  template <typename ValueType, typename Payload>
  bool StridedConstIterator<ValueType,Payload>::operator!=( const StridedConstIterator<ValueType, Payload> &rhs ) const
  {
    return m_basePtr != rhs.m_basePtr;
  }

//namespace nvutil
}

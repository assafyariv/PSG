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
/** @file */

#include <nvutil/Handle.h>

namespace nvutil
{

  /*! \brief Iterator base class for various iterators in SceniX
   *  \param FriendType The class being a friend of this SceniXIterator.
   *  \param IteratorType The iterator type to wrap.
   *  \remark This class is used to wrap an arbitrary iterator, but provide only const access to it, while
   *  one of the template arguments specifies a class to be a friend of this SceniXIterator, thus allowing
   *  unlimited access to just that class.
   *  \note There is no post-increment or post-decrement operator declared.
   *  \sa Camera::HeadLightIterator, GeoNode::StateSetIterator, GeoNode::DrawableIterator */
  template<typename FriendType, typename IteratorType>
  class SceniXIterator : public std::iterator<std::input_iterator_tag, typename IteratorType::value_type>
  {
#if defined(LINUX)
    friend class nvutil::ObjectTraits<FriendType>::ObjectType;
#else
    friend typename nvutil::ObjectTraits<FriendType>::ObjectType;
#endif

    public:
      /*  \brief Default constructor */
      SceniXIterator();

      /*! \brief Construct a SceniXIterator out of an iterator
       *  \param it An iterator of the same type being wrapped by this SceniXIterator */
      explicit SceniXIterator( const IteratorType & it );

      /*! \brief Constant indirection operator, providing a const reference to the variable
       *  \return A constant reference to the variable */
      const typename IteratorType::value_type & operator*() const;

      /*! \brief Constant member operator, providing a const pointer to the variable
       *  \return A constant pointer to the variable */
      const typename IteratorType::value_type * operator->() const;

      /*! \brief Equal operator
       *  \param di The SceniXIterator to compare with.
       *  \return \c true, if \a this and \a di are equal, otherwise \c false. */
      bool operator==( const SceniXIterator & di ) const;

      /*! \brief Not equal operator
       *  \param di The SceniXIterator to compare with.
       *  \return \c true, if \a this and \a di are not equal, otherwise \c false. */
      bool operator!=( const SceniXIterator & di ) const;

      /*! \brief Prefix increment operator
       *  \return A reference to the SceniXIterator after incrementing. */
      SceniXIterator & operator++();

      /*! \brief Prefix decrement operator
       *  \return A reference to the SceniXIterator after decrementing. */
      SceniXIterator & operator--();

    protected:
      IteratorType m_iter;    //!< The wrapped iterator
  };

  template<typename FriendType, typename IteratorType>
  inline SceniXIterator<FriendType,IteratorType>::SceniXIterator()
  {
  }

  template<typename FriendType, typename IteratorType>
  inline SceniXIterator<FriendType,IteratorType>::SceniXIterator( const IteratorType & ti )
    : m_iter( ti )
  {
  }

  template<typename FriendType, typename IteratorType>
  inline const typename IteratorType::value_type & SceniXIterator<FriendType,IteratorType>::operator*() const
  {
    return( m_iter.operator*() );
  }

  template<typename FriendType, typename IteratorType>
  inline const typename IteratorType::value_type * SceniXIterator<FriendType,IteratorType>::operator->() const
  {
    return( m_iter.operator->() );
  }

  template<typename FriendType, typename IteratorType>
  inline bool SceniXIterator<FriendType,IteratorType>::operator==( const SceniXIterator & di ) const
  {
    return( m_iter == di.m_iter );
  }

  template<typename FriendType, typename IteratorType>
  inline bool SceniXIterator<FriendType,IteratorType>::operator!=( const SceniXIterator & di ) const
  {
    return( m_iter != di.m_iter );
  }

  template<typename FriendType, typename IteratorType>
  inline SceniXIterator<FriendType,IteratorType> & SceniXIterator<FriendType,IteratorType>::operator++()
  {
    ++m_iter;
    return( *this );
  }

  template<typename FriendType, typename IteratorType>
  inline SceniXIterator<FriendType,IteratorType> & SceniXIterator<FriendType,IteratorType>::operator--()
  {
    --m_iter;
    return( *this );
  }
}

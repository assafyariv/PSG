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

#include "nvutil/RCObject.h"


namespace nvutil
{
  /*! \brief Pointer object to auto-support lifetime management for reference-counted objects.
   */
  template <typename RCType>
  class SmartPtr
  {
    // Types used to implement the safe_bool idiom.
    struct _Hidden_type { _Hidden_type* _M_bool; };
    typedef _Hidden_type* _Hidden_type::* _Safe_bool;

    typedef RCType* pointer;
  public:
    /*! \brief Constructor of a SmartPtr.
     *  \param ptr Optional pointer to manage with this SmartPtr. */
    SmartPtr(RCType * ptr = NULL) : m_ptr(ptr) { check_ref(); }

    /*! \brief Constructor of a SmartPtr.
     *  \param ptr Plain pointer to a type derived from the SmartPtr type. */
    template <typename RCTypeDerived>
    SmartPtr(RCTypeDerived *ptr)
      : m_ptr(ptr)
    {
      check_ref();
    }

    /*! \brief Copy constructor of a SmartPtr.
     *  \param rhs A SmartPtr to copy.
     *  \remarks The copy constructor does not copy the contained object, but just the pointer to it, and increases
     *  its reference count. */
    SmartPtr(const SmartPtr& rhs) : m_ptr(rhs.m_ptr) { check_ref(); }

    /*! \brief Assignment operator of a SmartPtr.
     *  \param rhs the SmartPtr to assign.
     *  \returns The newly assigned SmartPtr.
     *  \remarks If \c this already manages a pointer, its reference count is decreased. Then the managed pointer of
     *  \a rhs is copied and its reference count is increased. */
    SmartPtr& operator=(const SmartPtr& rhs)
    {
      // catch attempts of self assignment 
      if ( *this != rhs ) {
        check_unref();
        m_ptr = rhs.get();
        check_ref();
      }
      return *this;
    }

    /*! \brief Assignement operator of a SmartPtr.
     *  \param rhs A SmartPtr of a type, which is derived from the type of the SmartPtr to assign.
     *  \returns The newly assigned SmartPtr.
     *  \remarks If \c this already manages a pointer, its reference count is decreased. Then the managed pointer
     *  of \a rhs is copied and its reference count is increased.
     *  \note In debug mode it is asserted that the managed pointer of \a rhs can be dynamically cast to the
     *  managed pointer type of \c this. */
    template <typename RCTypeBase>
    SmartPtr& operator=(const SmartPtr<RCTypeBase>& rhs)
    {
      NVSG_ASSERT( !rhs.get() || dynamic_cast<RCType*>(rhs.get()) );
      // catch attempts of self assignment 
      if ( *this != rhs ) {
        check_unref();
        m_ptr = dynamic_cast<RCType*>(rhs.get());
        check_ref();
      }
      return *this;
    }

    /*! \brief Cast operator to perform upcasts
     *  \returns A SmartPtr of a base type of the original type. */
    template <typename RCTypeBase> operator SmartPtr<RCTypeBase>() const { return SmartPtr<RCTypeBase>(m_ptr); }

    /*! \brief Destructor of a SmartPtr
     *  \remarks The reference count of the managed pointer is decreased. */
    ~SmartPtr() { check_unref(); }

    /*! \brief The member through pointer operator
     *  \returns The pointer managed by this SmartPtr.
     *  \sa get */
    RCType * operator->() const { NVSG_ASSERT(m_ptr); return m_ptr; }

    /*! \brief The member through reference operator
     *  \returns A reference to the object pointed to by the pointer, managed by this SmartPtr. */
    RCType& operator*() const { NVSG_ASSERT(m_ptr); return *m_ptr; }

    /*! \brief Function to get the pointer managed by this SmartPtr.
     *  \returns The pointer managed by this SmartPtr.
     *  \sa operator->() */ 
    RCType * get() const { return m_ptr; } 

    /*! \brief reset this SmartPtr by dereferencing its pointer and setting it to nullptr. */
    void reset() { check_unref(); m_ptr = nullptr; }

    /*! \brief Comparison operator with an other SmartPtr.
     *  \param rhs The SmartPtr to compare with.
     *  \returns \c true, if the managed pointers of \c this and \a rhs are equal, otherwise \c false */
    template <typename RCTypeBase>
    bool operator==(const SmartPtr<RCTypeBase>& rhs) const
    {
      return( m_ptr==rhs.get() );
    }

    /*! \brief Comparison operator with an other SmartPtr.
     *  \param rhs The SmartPtr to compare with.
     *  \returns \c true, if the managed pointers of \c this and \a rhs are different, otherwise \c false */
    template <typename RCTypeBase>
    bool operator!=(const SmartPtr<RCTypeBase>& rhs) const
    {
      return( m_ptr!=rhs.get() );
    }

    /*! \brief Less-than operator
     *  \param rhs The SmartPtr to compare with.
     *  \return \c true, if the pointer managed by \c this is less than the pointer managed by \a rhs,
     *  otherwise \c false. */
    template <typename RCTypeBase>
    bool operator<(const SmartPtr<RCTypeBase>& rhs) const
    {
      return( m_ptr<rhs.get() );
    }

    /*! \brief Comparison operator with a pointer.
     *  \param rhs A pointer to compare with.
     *  \return \c true, if the pointer managed by \c this equals \a rhs, otherwise \c false. */
    bool operator==(const RCType * rhs)  const { return m_ptr==rhs; }

    /*! \brief Comparison operator with a pointer.
     *  \param rhs A pointer to compare with.
     *  \return \c true, if the pointer managed by \c this is not equal to \a rhs, otherwise \c false. */
    bool operator!=(const RCType * rhs)  const { return m_ptr!=rhs; }

    /*! \brief Cast operator providing a safe way to check a SmartPtr for validity
     *  \return A boolean testable value, which is \c true if \c this manages a pointer, otherwise \c false. */
    operator _Safe_bool() const { return (nullptr == m_ptr ? 0 : &_Hidden_type::_M_bool); }

  private:
    void check_ref() { if ( m_ptr ) { m_ptr->addRef(); } }
    void check_unref() { if ( m_ptr)  { m_ptr->removeRef(); } }
    RCType * m_ptr;
  };

  /*! \brief Compare a pointer with a SmartPtr of the same type.
   *  \param lhs A pointer.
   *  \param rhs A SmartPtr of the same type as \a lhs.
   *  \return \c true, if \a lhs is the same pointer as the one managed by \a rhs, otherwise \c false. */
  template <typename T>
  bool operator==( const T * lhs, SmartPtr<T> & rhs )
  {
    return( rhs == lhs );
  }

  /*! \brief Compare a pointer with a SmartPtr of the same type.
   *  \param lhs A pointer.
   *  \param rhs A SmartPtr of the same type as \a lhs.
   *  \return \c true, if \a lhs is the a different pointer as the one managed by \a rhs, otherwise \c false. */
  template <typename T>
  bool operator!=( const T * lhs, SmartPtr<T> & rhs )
  {
    return( rhs != lhs );
  }

  /*! \brief Check if a SmartPtr of type \c U also is a SmartPtr of type \c T.
   *  \param p The SmartPtr to check.
   *  \return \c true, if the pointer managed by \a p can be dynamically casted to a pointer to a \a T,
   *  otherwise \c false. */
  template<typename T, typename U> 
  inline bool isSmartPtrOf( const SmartPtr<U> &p )
  {
    return( dynamic_cast<T *>(p.get()) != nullptr );
  }

  /*! \brief Cast a const SmartPtr of type \c U to a const SmartPtr of type \c T.
   *  \param rhs The const SmartPtr to cast.
   *  \return \a rhs reinterpreted as a const SmartPtr of type \c T.
   *  \note In debug mode, it is asserted that \a rhs in fact is a const SmartPtr of type \c T. */
  template<typename T, typename U>
  inline const SmartPtr<T> & smart_cast( const SmartPtr<U> & rhs )
  {
    NVSG_ASSERT( !rhs || isSmartPtrOf<T>(rhs) );
    return( *reinterpret_cast<const SmartPtr<T>*>(&rhs) );
  }

  /*! \brief Cast a SmartPtr of type \c U to a SmartPtr of type \c T.
   *  \param rhs The SmartPtr to cast.
   *  \return \a rhs reinterpreted as a SmartPtr of type \c T.
   *  \note In debug mode, it is asserted that \a rhs in fact is a SmartPtr of type \c T. */
  template<typename T, typename U>
  inline SmartPtr<T> & smart_cast( SmartPtr<U> & rhs )
  {
    NVSG_ASSERT( !rhs || isSmartPtrOf<T>(rhs) );
    return( *reinterpret_cast<SmartPtr<T>*>(&rhs) );
  }

  /*! \brief Helper function to clone a SmartPtr if it is shared.
   *  \param sp The SmartPtr to clone, if it is shared.
   *  \remarks If \a sp is shared, it is cloned and sp then points to the newly cloned object.
   *  Otherwise sp isn't changed. */
  template <typename T>
  inline void cloneIfShared( SmartPtr<T> & sp )
  {
    if ( sp->isShared() )
    {
      sp = new T(*sp.get());
    }
  }

}



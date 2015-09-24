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

#include <nvutil/Reflection.h>

namespace nvutil
{
  // serves as base class for 'handled' objects
  class HandledObject : public Reflection
  {
    public:
      NVSG_API virtual unsigned int onUpdate( const Subject *subject, unsigned int oldState, unsigned int updateState, const nvutil::Subject::SmartPayload &payload ) const;
      NVSG_API virtual void onUpdate( const Subject *subject, PropertyId property, const nvutil::Subject::SmartPayload &payload ) const;

      HandledObject& operator=(const HandledObject & rhs);

    protected:
      HandledObject();
      HandledObject( const HandledObject & );
      virtual ~HandledObject();

      bool isHandled() const;

  private:
      template <typename T> friend struct Holder;
      template <typename T> friend typename ObjectTraits<T>::WeakPtr getWeakPtr( const HandledObject * obj );
      int m_signature; // initialized with SIGNATURE if the object is handled
      UINT_PTR m_handle; // holds the address of the corresponding handle
  };

  inline HandledObject::HandledObject()
  {
    /* do nothing! */
  }

  inline HandledObject::HandledObject( const HandledObject &rhs )
    : Reflection( rhs )
  {
    /* do nothing! */
  }

  inline unsigned int HandledObject::onUpdate( const Subject *subject, unsigned int oldState, unsigned int updateState, const nvutil::Subject::SmartPayload &payload ) const
  {
    notify( this, updateState );
    return oldState | updateState;
  }

  inline void HandledObject::onUpdate( const Subject *subject, PropertyId property, const nvutil::Subject::SmartPayload &payload ) const
  {
    notify( property );
  }

  inline HandledObject::~HandledObject()
  {
#if !defined(NDEBUG)
    m_signature = 0xDEADF00D;
    m_handle = 0xDEADF00D;
#endif
  }

  inline bool HandledObject::isHandled() const
  {
    return SIGNATURE == m_signature;
  }

  inline HandledObject& HandledObject::operator=( const HandledObject &rhs )
  {
    NVSG_TRACE();
    Reflection::operator=( rhs );
    return( *this );
  }

  // returns the object's corresponding handle
  template <typename T>
  inline typename ObjectTraits<T>::WeakPtr getWeakPtr( const HandledObject * obj )
  {
    NVSG_ASSERT( obj->isHandled() );
    return( reinterpret_cast<typename ObjectTraits<T>::WeakPtr>(obj->m_handle) );
  }

} // namespace nvutil

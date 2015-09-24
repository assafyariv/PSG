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

namespace nvutil
{
  template<typename T>
  class WritableObject
  {
    public:
      template <typename U> explicit WritableObject( U * h )
      {
        NVSG_ASSERT(h);
        obj = static_cast<T *>(h->beginEdit());
      }

      template <typename U> explicit WritableObject( const SmartPtr<U> &h )
      {
        NVSG_ASSERT(h.get());
        obj = static_cast<T *>(h.get()->beginEdit());
      }

      ~WritableObject()
      {
        HandledObjectWeakPtr ptr(getWeakPtr<HandledObject>(obj));
        ptr->endEdit();
      }

      // make it behave like a pointer
      T * operator->() const
      {
        return obj;
      }

      T & operator*() const
      {
        return *obj;
      }

      // implicit conversion to T*
      operator T *() const
      {
        return obj;
      }

    private:
      WritableObject(const WritableObject&);            // copying not permitted!
      WritableObject& operator=(const WritableObject&); // assigning a WritableObject not permitted!
      WritableObject& operator=( const T& rhs );        // assigning an object of type T not permitted!

    private:
      T * obj;
  };

} // namespace nvutil

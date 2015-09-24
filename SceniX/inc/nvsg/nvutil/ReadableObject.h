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
  class ReadableObject
  {
    public:
      template <typename U> explicit ReadableObject( U * h )
      {
        NVSG_ASSERT(h);
        obj = static_cast<const T*>(h->beginRead());
      }

      template <typename U> explicit ReadableObject( const SmartPtr<U> &h )
      {
        NVSG_ASSERT(h.get());
        obj = static_cast<const T *>(h.get()->beginRead());
      }

      ~ReadableObject()
      {
        HandledObjectWeakPtr ptr(getWeakPtr<HandledObject>(obj));
        ptr->endRead();
      }

      // make it behave like a pointer
      const T * operator->() const
      {
        return obj;
      }

      const T & operator*() const
      {
        return *obj;
      }

      // implicit conversion to T*
      operator const T *() const
      {
        return obj;
      }

    private:
      ReadableObject(const ReadableObject&);            // copying not permitted!
      ReadableObject& operator=(const ReadableObject&); // assigning a ReadableObject not permitted!
      ReadableObject& operator=( const T& rhs );        // assigning an object of type T not permitted!

    private:
      const T                 * obj;
  };

} // namespace nvutil

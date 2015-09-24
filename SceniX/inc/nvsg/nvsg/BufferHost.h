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

#include "nvsg/Buffer.h"

namespace nvsg {
  /** \brief Buffer implementation using memory on the host as storage.
   *  \sa Buffer
  **/
  class BufferHost : public Buffer
  {
  public:
    NVSG_API static BufferHostSharedPtr create();

#if 0
    NVSG_API static BufferHostSharedPtr create( size_t sizeInBytes );
#endif

  public:
    NVSG_API virtual void setUnmanagedDataPtr( void *data );

    NVSG_API virtual void setSize(size_t size);
    NVSG_API virtual size_t getSize() const;

    NVSG_API virtual nvutil::Incarnation getIncarnation() const;

  protected:
    friend struct nvutil::Holder<BufferHost>;

    NVSG_API BufferHost( );
    NVSG_API virtual ~BufferHost();

    using Buffer::map;
    NVSG_API virtual void *map( MapMode mode, size_t offset, size_t length );
    NVSG_API virtual void unmap( );

    using Buffer::mapRead;
    NVSG_API virtual const void *mapRead(size_t offset, size_t length ) const;
    NVSG_API virtual void unmapRead() const;

    mutable Buffer::MapMode m_mapMode;

    size_t              m_sizeInBytes;
    char*               m_data;
    nvutil::Incarnation m_incarnation;
    bool                m_managed;
  };
  
} // namespace nvsg

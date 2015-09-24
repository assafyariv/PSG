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

#include <nvsg/Buffer.h>
#include <nvgl/RenderContextGL.h>

namespace nvgl {

  /*! \brief This class uses an OpenGL buffer object as storage for the data of an nvsg::Buffer. For
             all operations on an nvgl::BufferGL object it is necessary to have a valid OpenGL context current.
  **/
  class BufferGL : public nvsg::Buffer
  {
    public:
      /*! \brief Create a new nvgl::BufferGL object. It is required that an OpenGL context is current
                 when calling this function.
          \return An nvgl::BufferGLSharedPtr to the created object.
      !*/
      NVSG_API static BufferGLSharedPtr create();

  public:
    enum BufferState{
      STATE_MANAGED                 = BIT0,
      STATE_CAPABILITY_RANGE        = BIT1,
      STATE_CAPABILITY_COPY         = BIT2,
      STATE_CAPABILITY_RANGEANDCOPY = STATE_CAPABILITY_RANGE | STATE_CAPABILITY_COPY,
    };
  
    /*! \brief Bind the buffer to the target specified by the setTarget function !*/
    NVSG_API void bind();

    /*! \brief Bind the buffer to the given target
        \param target OpenGL target (GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, ...)
    !*/
    NVSG_API void bind( GLenum target );

    /*! \brief Unbind the buffer from the target specified by the setTarget function !*/
    NVSG_API void unbind();

    /*! \brief Bind the buffer from the given target
        \param target OpenGL target (GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, ...)
    !*/
    NVSG_API void unbind( GLenum target);

    using Buffer::setData;
    using Buffer::getData;

    /*! \brief Copy data from this buffer into the given buffer.
        \param srcOffset Offset in source buffer in bytes.
        \param length Number of bytes to copy from source buffer to destination buffer.
        \param dstBuffer Buffer which should receive the data.
        \param dstOffset Offset in destination buffer in bytes.
    !*/
    NVSG_API virtual void getData( size_t srcOffset, size_t length, const nvsg::BufferSharedPtr &dstBuffer , size_t dstOffset) const;

    /*! \brief Copy data from a given buffer into this buffer.
        \param srcOffset Offset in this buffer in bytes.
        \param length Number of bytes to copy from source buffer into this buffer.
        \param srcBuffer Source for copy operation.
        \param srcOffset Offset in source buffer in bytes.
    !*/
    NVSG_API virtual void setData( size_t dstOffset, size_t length, const nvsg::BufferSharedPtr &srcBuffer , size_t srcOffset);

    /*! \brief Resize the buffer. This function will resize the underlying storage. All data stored
               in this buffer will be lost.
        \remarks Use resize( size_t ) to change the size without losing data in this buffer.
    !*/
    NVSG_API virtual void setSize(size_t size);

    /*! \brief Query the size of this buffer.
        \return Size of this buffer.
    !*/
    NVSG_API virtual size_t getSize() const;

    /*! \brief Sets the OpenGL target this buffer will be bound to.
     *  \param target OpenGL target (GL_ARRAY_BUFFER, GL_ELEMENT_ARRAY_BUFFER, ...)
    !*/
    NVSG_API void setTarget( GLenum target );

    /*! \brief Query the OpenGL target of this buffer.
        \return OpenGL target of this buffer.
    !*/
    NVSG_API GLenum getTarget() const;

    /*! \brief Sets the OpenGL access usage of this object. Becomes effective with the next resize of setSize operation.
     *  \param usage (GL_STATIC_DRAW, GL_DYNAMIC_READ, ...).
    !*/
    NVSG_API void setUsage( GLenum usage );

    /*! \brief Query the OpenGL access usage of this buffer.
        \return Usage of this buffer (GL_STATIC_DRAW, GL_DYNAMIC_READ, ...).
    **/
    NVSG_API GLenum getUsage() const;

    /*! \brief Query the OpenGL buffer object name of this buffer.
        \return OpenGL buffer object name.
    **/
    NVSG_API GLuint getGLId() const;

    NVSG_API virtual nvutil::Incarnation getIncarnation() const { return m_incarnation; }

  protected:
    friend struct nvutil::Holder<BufferGL>;

    /*! \brief Default constructor !*/
    NVSG_API BufferGL( );

    /*! \brief Destructor !*/
    NVSG_API virtual ~BufferGL();

    using Buffer::map;
    NVSG_API virtual void *map( MapMode mode, size_t offset, size_t length);
    NVSG_API virtual void unmap( );

    using Buffer::mapRead;
    NVSG_API virtual const void *mapRead( size_t offset, size_t length ) const;
    NVSG_API virtual void unmapRead( ) const;

    mutable Buffer::MapMode m_mapMode;

    GLuint          m_glId;
    GLenum          m_target;
    GLenum          m_usage;
    size_t          m_sizeInBytes;
    unsigned int    m_stateFlags;
    
    nvutil::Incarnation m_incarnation;

    ShareGroupGLSharedPtr m_shareGroupGL;
  };

  inline void BufferGL::setTarget( GLenum target )
  {
    NVSG_ASSERT( m_mapMode == MAP_NONE );
    m_target = target;
  }

  inline GLenum BufferGL::getTarget() const
  {
    return m_target;
  }

  inline void BufferGL::setUsage( GLenum usage )
  {
    NVSG_ASSERT( m_mapMode == MAP_NONE );
    m_usage = usage;
  }

  inline GLenum BufferGL::getUsage() const
  {
    return m_usage;
  }

}

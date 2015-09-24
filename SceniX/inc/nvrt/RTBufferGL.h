// Copyright NVIDIA Corporation 2002-2008
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
/** \file */

#include <nvrt/RTBuffer.h>
#include <nvgl/BufferGL.h>

namespace nvrt
{
  /*! \brief Special RTBuffer to also manage an OpenGL specific buffer id.
   *  \par Namespace: nvrt
   *  \remarks An RTBufferGL is an RTBuffer, that in addition manages an OpenGL specific buffer id.
   *  Besides that, it can be used like any other RTBuffer.
   *  \sa nvrt::RTBuffer, nvrt::RTContext */
  class RTBufferGL : public RTBuffer
  {
    friend class RTContext;

    public:
      NVRT_API static RTBufferGLSharedPtr create();

    public:
      /*! \brief Default-constructor of RTBufferGL */
      NVRT_API RTBufferGL();

      /*! \brief Destructor of RTBufferGL */
      NVRT_API virtual ~RTBufferGL();

      /*! \brief Set the format of the elements of this RTBufferGL and update the OpenGL specific data.
       *  \param format Enumeration value describing the format of the elements of this RTBufferGL.
       *  \return \c true, if the format has changed, otherwise \c false.
       *  \sa RTBuffer::setFormat */
      NVRT_API virtual bool setFormat( RTformat format );

      /*! \brief Set the element size for the elements of this RTBufferGL and update the OpenGL specific data.
       *  \param size The size of the elements of this RTBufferGL.
       *  \return \c true, if the element size has changed, otherwise \c false.
       *  \sa RTBuffer::setElementSize */
      NVRT_API virtual bool setElementSize( unsigned int size );

      /*! \brief Set the size of this RTBufferGL and update the OpenGL specific data.
       *  \param width The width of this RTBufferGL. It has to be positive.
       *  \param height The optional height of this RTBufferGL. Its default value is zero.
       *  \param depth The optional depth of this RTBufferGL. Its default value is zero.
       *  \return \c true, if the any size has changed, otherwise \c false.
       *  \sa RTBuffer::setSize */
      NVRT_API virtual bool setSize( unsigned int width, unsigned int height = 0, unsigned int depth = 0 );

      /*! \brief Get the OpenGL specific id of the buffer data.
       *  \return The OpenGL specific id of the buffer data.
       */
      NVRT_API GLuint getGLId() const;

    protected :
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the acceleration structure with.
       *  \param type Enumeration to specify the buffer type.
       *  \remarks This function is called from RTContext::newBuffer to initialize the new buffer
       *  structure. The enumeration values for \a type are RT_BUFFER_INPUT, RT_BUFFER_OUTPUT, and
       *  RT_BUFFER_INPUT_OUTPUT
       *  \sa nvrt::RTContext */
      NVRT_API virtual void init( RTcontext context, RTbuffertype type );

    private:
      void updateGLObject();

    private:
      nvgl::BufferGLSharedPtr m_bufferGL;
  };
}


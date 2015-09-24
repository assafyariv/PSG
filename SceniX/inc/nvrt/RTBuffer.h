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

#include <nvrt/nvsgrtapi.h>
#include <nvrt/RTBase.h>
#include <nvrt/nvirt/optix.h>

namespace nvrt
{
  /*! \brief Wrapper class for a nvirt RTbuffer object.
   *  \par Namespace: nvrt
   *  \remarks An RTBuffer is a class to hold some arbirtrary one-, two-, or three-dimensional data
   *  to be used in a ray tracing program. A RTBuffer can be associated with a RTVariableBuffer,
   *  which in turn can be set to a class derived from RTVariablesManager.
   *  \sa nvrt::RTBase, nvrt::RTContext, nvrt::VariableBuffer */
  class RTBuffer : public RTBase
  {
    friend class RTContext;
    friend class RTTextureSampler;
    friend class RTVariableType<RTBuffer>;

    public:
      NVRT_API static RTBufferSharedPtr create();

    public:
      /*! \brief Default-constructor of RTBuffer */
      NVRT_API RTBuffer();

      /*! \brief Destructor of RTBuffer */
      NVRT_API virtual ~RTBuffer();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_BUFFER. */
      NVRT_API virtual RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Set the format of the elements of this RTBuffer.
       *  \param format Enumeration value describing the format of the elements of this RTBuffer.
       *  \return \c true, if the format has changed, otherwise \c false.
       *  \remarks The format describes the data type of the elements of the RTBuffer. It can either
       *  describe a specific type, like RT_FORMAT_FLOAT, RT_FORMAT_BYTE, etc., or a user defined
       *  type: RT_FORMAT_USER. When using a specific type, the size of the elements is implicitly
       *  defined. When using RT_FORMAT_USER, the size of the elements has to be set explicitly.\n
       *  The supported enumeration values for \a format are RT_FORMAT_BYTE, RT_FORMAT_BYTE2,
       *  RT_FORMAT_BYTE3, RT_FORMAT_BYTE4, RT_FORMAT_UNSIGNED_BYTE, RT_FORMAT_UNSIGNED_BYTE2,
       *  RT_FORMAT_UNSIGNED_BYTE3, RT_FORMAT_UNSIGNED_BYTE4, RT_FORMAT_SHORT, RT_FORMAT_SHORT2,
       *  RT_FORMAT_SHORT3, RT_FORMAT_SHORT4, RT_FORMAT_UNSIGNED_SHORT, RT_FORMAT_UNSIGNED_SHORT2,
       *  RT_FORMAT_UNSIGNED_SHORT3, RT_FORMAT_UNSIGNED_SHORT4, RT_FORMAT_INT, RT_FORMAT_INT2,
       *  RT_FORMAT_INT3, RT_FORMAT_INT4, RT_FORMAT_UNSIGNED_INT, RT_FORMAT_UNSIGNED_INT2,
       *  RT_FORMAT_UNSIGNED_INT3, RT_FORMAT_UNSIGNED_INT4, RT_FORMAT_FLOAT, RT_FORMAT_FLOAT2,
       *  RT_FORMAT_FLOAT3, RT_FORMAT_FLOAT4, RT_FORMAT_USER.
       *  \sa getFormat, setElementSize, getElementSize */
      NVRT_API virtual bool setFormat( RTformat format );

      /*! \brief Get the format of the elements of this RTBuffer.
       *  \return The enumeration value specifying the format of the elements of this RTBuffer.
       *  \remarks The format describes the data type of the elements of the RTBuffer. It can either
       *  describe a specific type, like RT_FORMAT_FLOAT, RT_FORMAT_BYTE, etc., or a user defined
       *  type: RT_FORMAT_USER.
       *  \sa setFormat, setElementSize, getElementSize */
      NVRT_API RTformat getFormat() const;

      /*! \brief Set the element size for the elements of this RTBuffer.
       *  \param size The size of the elements of this RTBuffer.
       *  \return \c true, if the element size has changed, otherwise \c false.
       *  \remarks If the format of this RTBuffer is RT_FORMAT_USER, the element size has to be
       *  specified explicitly; otherwise it is implicitly specified by the format, and this
       *  function should not be used.
       *  \sa setFormat, getFormat, getElementSize */
      NVRT_API virtual bool setElementSize( unsigned int size );

      /*! \brief Get the element size of the elements of this RTBuffer.
       *  \return The element size of the elements of this RTBuffer.
       *  \remarks The size of the elements of this RTBuffer is either implicitly specified by its
       *  format, or explicitly specified by using setElementSize if the format is RT_FORMAT_USER.
       *  \sa setFormat, getFormat, setElementSize */
      NVRT_API unsigned int getElementSize() const;

      /*! \brief Set the size of this RTBuffer.
       *  \param width The width of this RTBuffer. If the width is 0 the buffer has a dimensionality of 1.
       *  \param height The optional height of this RTBuffer. Its default value is zero.
       *  \param depth The optional depth of this RTBuffer. Its default value is zero.
       *  \return \c true, if the any size has changed, otherwise \c false.
       *  \remarks This function specifies the size, as well as the dimensionality of this RTBuffer.
       *  A three dimensional RTBuffer is specified when width, height, and depth are greater than zero.
       *  A two dimensional RTBuffer is specified if width and height are greater than zero and depth is zero.
       *  A one dimensional RTBuffer is specified if height and depth are zero.
       *  All three size values, width, height, and depth, specify the number of elements for this RTBuffer.
       *  \sa setFormat, getDimensionality, getWidth, getHeight, getDepth */
      NVRT_API virtual bool setSize( unsigned int width, unsigned int height = 0, unsigned int depth = 0 );

      /*! \brief Get the dimensionality of this RTBuffer.
       *  \return The dimensionality of this RTBuffer.
       *  \remarks The dimensionality of this RTBuffer is specified by a previous call to setSize.
       *  If width, height, and depth are greater than zero, the dimensionality is three; if width and height
       *  are greater than zero while depth is zero, the dimensionality is two; if height and depth are zero,
       *  the dimensionality is one. A dimensionality of zero or larger than three is not possible.
       *  \sa setSize, getWidth, getHeight, getDepth */
      NVRT_API unsigned int getDimensionality() const;

      /*! \brief Get the width of this RTBuffer.
       *  \return The width of this RTBuffer.
       *  \remarks The width of this RTBuffer is the number of elements for the first dimension. If
       *  setSize has been called before, it is assured to be positive.
       *  \sa setSize, getDimensionality, getHeight, getDepth */
      NVRT_API unsigned int getWidth() const;

      /*! \brief Get the height of this RTBuffer.
       *  \return The height of this RTBuffer.
       *  \remarks If this RTBuffer is at least two dimensional, the height is positive. The height
       *  is the number of elements for the second dimension.
       *  \sa setSize, getDimensionality, getWidth, getDepth */
      NVRT_API unsigned int getHeight() const;

      /*! \brief Get the depth of this RTBuffer.
       *  \return The depth of this RTBuffer.
       *  \remarks If this RTBuffer is three dimensional, the depth is positive. The depth is the
       *  number of elements for the third dimension.
       *  \sa setSize, getDimensionality, getWidth, getHeight */
      NVRT_API unsigned int getDepth() const;

      /*! \brief Get the size of the RTBuffer in bytes.
       *  \return The size of the RTBuffer in bytes.
       *  \sa setFormat, setSize, getDimensionality, getWidth, getHeight, getDepth */
      NVRT_API unsigned int getSizeInBytes() const;

      /*! \brief Get a constant pointer to the buffer data.
       *  \return A constant pointer to the buffer data.
       *  \remarks This function allows direct reading of the buffer data. After finishing the
       *  operations, one should call unmap.
       *  \sa map, unmap */
      NVRT_API const void * map() const;

      /*! \brief Get a pointer to the buffer data.
       *  \return A pointer to the buffer data.
       *  \remarks This function allows direct reading and writing of the buffer data. After
       *  finishing the operations, one should call unmap.
       *  \sa map, unmap */
      NVRT_API void * map();

      /*! \brief Unmap the buffer data.
       *  \remarks After finishing operations on the buffer data previously mapped with map, this
       *  function should be called. After unmapping the buffer, the pointer get by map is invalid.
       *  \sa map, map */
      NVRT_API void unmap() const;

      /*! \brief Clear the buffer data.
       *  \remarks This is a convenience function to easily clear all the buffer data by filling it
       *  with zeros.
       *  \sa map, unmap, setData, setSubData */
      NVRT_API void clear();

      /*! \brief Set the complete buffer data.
       *  \param data A constant pointer to the data to copy into this RTBuffer.
       *  \remarks The size of the data has to be at least as large as the RTBuffer.
       *  \sa getSizeInBytes, setSubData */
      NVRT_API void setData( const void * data );

      /*! \brief Set the data in some specified part of this RTBuffer.
       *  \param data A constant pointer to the data to copy int the specified part of this RTBuffer.
       *  \param xOffset Offset for the first dimension to start the area to copy to.
       *  \param width Size of the data to copy in the first dimension.
       *  \param yOffset Optional offset for the second dimension. The default value is zero.
       *  \param height Optional size of the data to copy in the second dimension. The default value
       *  is zero.
       *  \param zOffset Optional offset for the third dimension. The default value is zero.
       *  \param depth Optional size of the data to copy in the third dimension. The default value
       *  is zero.
       *  \remarks Using this function, a one-, two- or three-dimensional part of the buffer can
       *  be set. For each dimension, the sum of offset and size (width, height, or depth) is not
       *  allowed to be larger than the corresponding size of the RTBuffer. All offsets and sizes
       *  are in number of elements, not in bytes. The size of the data to copy has to be at least
       *  as large as the sub data described by the offsets and sizes.
       *  \sa setData */
      NVRT_API void setSubData( const void * data, unsigned int xOffset, unsigned int width,
                                  unsigned int yOffset = 0, unsigned int height = 0,
                                  unsigned int zOffset = 0, unsigned int depth = 0 );

    protected:
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the acceleration structure with.
       *  \param type Enumeration to specify the buffer type.
       *  \remarks This function is called from RTContext::newBuffer to initialize the new buffer
       *  structure. The enumeration values for \a type are RT_BUFFER_INPUT, RT_BUFFER_OUTPUT, and
       *  RT_BUFFER_INPUT_OUTPUT
       *  \sa nvrt::RTContext */
      NVRT_API virtual void init( RTcontext context, RTbuffertype type );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTBuffer wraps the functionality of an nvirt::RTbuffer. This function is used
       *  internally for accessing that resource. */
      NVRT_API RTbuffer getResource() const;

    protected :
      RTbuffer      m_buffer;   // This needs to be protected to allow access from RTBufferGL !

    private:
      RTformat      m_format;
      unsigned int  m_dimensionality;
      unsigned int  m_elementSize, m_bufferSize;
      unsigned int  m_width, m_height, m_depth;
  };
}


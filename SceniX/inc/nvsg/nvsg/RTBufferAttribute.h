// Copyright NVIDIA Corporation 2002-2011
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

#include "nvsgcommon.h"
#include "nvsg/StateAttribute.h"
#include "nvrt/RTBuffer.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief StateAttribute that defines an RTBuffer used for Ray Tracing
   *  \par Namespace: nvsg
   *  \remarks an RTBuffer used to publish generic data to an RTFx.  This StateAttribute maintains a host-memory buffer 
   *  that is published to the RTFx when it is updated by the application.  Only the application may read or write data to the 
   *  buffer, but the CUDA/RT code may only read the buffer.
   *  Buffers can be shared between multiple RTFx instances in the scene.
   *  \sa StateAttribute,  */
  class RTBufferAttribute : public StateAttribute
  {
    public:
      NVSG_API static RTBufferAttributeSharedPtr create();

    public:

      /*! \brief Buffer Access modes
       */
      enum
      {
        MAP_READ    = 0x01,   //! Map buffer for reading
        MAP_WRITE   = 0x02    //! Map buffer for writing (updates incarnation count)
      };

      /*! \brief Set the format of a buffer.
       *  \param format       The format of the given buffer.  Must be one of the formats in optix.h, currently:
       *                      RT_FORMAT_FLOAT[234]
       *                      RT_FORMAT_BYTE[234]
       *                      RT_FORMAT_UNSIGNED_BYTE[234]
       *                      RT_FORMAT_SHORT[234]
       *                      RT_FORMAT_UNSIGNED_SHORT[234]
       *                      RT_FORMAT_INT[234]
       *                      RT_FORMAT_UNSIGNED_INT[234]
       *                      or RT_FORMAT_USER.
       *  \param elementSize  Size of data structure if format == RT_FORMAT_USER.
       *  \remarks Sets the format for the buffer.  If format == RT_FORMAT_USER then elementSize must also be specified.  Typically
       *  RT_FORMAT_USER is used to publish a data structure.  In this case format would be RT_FORMAT_USER and elementSize would
       *  be sizeof( DataStructure ).
       *  This method will allocate host memory for the buffer if setSize has been called.
       *  \sa RTBufferAttribute::getFormat, RTBufferAttribute::getElementSize
       */
      NVSG_API virtual void setFormat( RTformat format, unsigned int elementSize = 0 );

      /*! \brief Get the format of a buffer.
       *  \return RTformat representing the buffer's format.  See RTBufferAttribute::setFormat for a list of possible values.
       *  \sa RTBufferAttribute::getFormat, RTBufferAttribute::getElementSize
       */
      NVSG_API RTformat getFormat() const;

      /*! \brief Gets the buffers element size.
       *  \return The size of each element in the buffer, or zero if the buffer has not yet been configured.
       *  \remarks Returns the number of bytes per element based on the buffer's format.  The buffer must be previously configured by a call to 
       *  RTBufferAttribute::setFormat.
       *  \sa RTBufferAttribute::getFormat, RTBufferAttribute::getElementSize
       */
      NVSG_API unsigned int getElementSize() const;

      /*! \brief Gets the buffers size.
       *  \return The total size, in bytes, of the buffer.
       *  \remarks This method uses the following formula to calculate the buffer size:
       *  <code>
       *  ( width * max<unsigned int>( 1, height ) * max<unsigned int>( 1, depth ) * elementSize )
       *  </code>
       *  \sa RTBufferAttribute::getElementSize
       */
      NVSG_API size_t getBufferSize() const;

      /*! \brief Sets the buffers size.
       *  \param width For a one- two- or three-dimensional buffer, this is the first dimension's size.
       *  \param height For a two- or three-dimensional buffer, this is the second dimension's size.  Defaults to 0.
       *  \param depth For a three-dimensional buffer, this is the third dimension's size.  Defaults to 0.
       *  \remarks Width, height, and depth supply the number of elements in the buffer, not the number of
       *  "bytes" in the buffer.  Therefore, if format is RT_FORMAT_FLOAT3 and there is 1 float3 to be published, width 
       *  should be equal to '1'.
       *  This method will allocate host memory for the buffer if setFormat has been called. 
       *  \sa RTBufferAttribute::getBufferSize
       */
      NVSG_API virtual void setSize( unsigned int width, unsigned int height = 0, unsigned int depth = 0 );

      /*! \brief Gets the buffers dimensionality.
       *  \returns The number of dimensions this buffer contains.  (1 = 1D, 2 = 2D, 3 = 3D).
       *  \remarks Dimensionality of the buffer is set by a call to setSize.
       *  \sa RTBufferAttribute::setSize
       */
      NVSG_API unsigned int getDimensionality() const;

      /*! \brief Gets the buffer's width.
       *  \returns The width of the buffer.
       *  \remarks Dimensionality of the buffer is set by a call to setSize.
       *  \sa RTBufferAttribute::setSize
       */
      NVSG_API unsigned int getWidth() const;

      /*! \brief Gets the buffer's height.
       *  \returns The height of the buffer.
       *  \remarks Dimensionality of the buffer is set by a call to setSize.
       *  \sa RTBufferAttribute::setSize
       */
      NVSG_API unsigned int getHeight() const;
     
      /*! \brief Gets the buffer's depth.
       *  \returns The depth of the buffer.
       *  \remarks Dimensionality of the buffer is set by a call to setSize.
       *  \sa RTBufferAttribute::setSize
       */
      NVSG_API unsigned int getDepth() const;

      /*! \brief Maps the buffer to host memory for reading.
       *  \returns A pointer to the host memory where the buffer was mapped.
       *  \remarks "Maps" the memory READ_ONLY so that it can be read by the application.  Note that the implementation of this
       *  method simply returns a pointer to the internal buffer.  Calls to map() should be paired with calls to unmap().  
       *  \note Do not write to the mapped memory, or internal incarnation counters will become invalid.
       *  \sa RTBufferAttribute::unmap
       */
      NVSG_API const void * map() const;

      /*! \brief Maps the buffer to host memory.
       *  \param mode A combination of MAP_READ and MAP_WRITE to declare how the buffer will be accessed.
       *  \returns A pointer to the host memory where the buffer was mapped.
       *  \remarks "Maps" the memory so that it can be read or written by the application.  Note that the implementation of this
       *  method simply returns a pointer to the internal buffer.  Calls to map() should be paired with calls to unmap().
       *  \sa RTBufferAttribute::unmap
       */
      NVSG_API void * map( unsigned int mode = MAP_READ|MAP_WRITE );

      /*! \brief Unmaps the buffer from host memory.
       *  \remarks "Unmaps" the memory so that it can no longer be read or written by the application.  Calls to map() should be
       *  paired with calls to unmap().
       *  \sa RTBufferAttribute::map
       */
      NVSG_API void unmap() const;

      /*! \brief Clears the contents of the memory buffer.
       *  \remarks Zeroes the contents of the buffer.
       *  \sa RTBufferAttribute::unmap
       */
      NVSG_API void clear();

      /*! \brief Sets the buffers data.
       *  \param data A pointer to the data to copy into the buffer.
       *  \remarks Copies the data from the given pointer into the host memory buffer.  The size of the data pointed to must be
       *  at least as large as the buffer.
       *  \sa RTBufferAttribute::map, RTBufferAttribute::unmap
       */
      NVSG_API void setData( const void * data );

      /*! \brief Fills a portion of the buffers data.
       *  \param data A pointer to the data to copy into the buffer.
       *  \param xOffset The 1D element offset to begin writing data into the buffer.
       *  \param yOffset The 2D element offset to begin writing data into the buffer.
       *  \param zOffset The 3D element offset to begin writing data into the buffer.
       *  \param width The width of the data buffer.
       *  \param height For a two-dimensional data buffer, this is the second dimension's size.  Defaults to 0.
       *  \param depth For a three-dimensional data buffer, this is the third dimension's size.  Defaults to 0.
       *  \remarks Copies the data from the given pointer into a portion of the host memory buffer.  The host memory buffer must be greater
       *  than or equal to in size and dimension as the data to be copied.
       *  \sa RTBufferAttribute::map, RTBufferAttribute::setData
       */
      NVSG_API void setSubData( const void * data, unsigned int xOffset, unsigned int width,
                                  unsigned int yOffset = 0, unsigned int height = 0,
                                  unsigned int zOffset = 0, unsigned int depth = 0 );

      /*! \brief Sets the CUDA/RT variable name to bind the buffer to.
       *  \param name The name of the buffer in the CUDA/RT code to receive this data.
       *  \remarks Binds the data from this buffer to an RTBuffer in the CUDA code of the given name.  The RTTraverser will
       *  determine if there are any RTFx's in the current nvsg::StateSet, and if so, will bind this buffer to them in all 
       *  domains to make the data available.
       *  Note that this method will create a buffer of the given name and type on the RTFx if no variable name matches exactly.
       *  Therefore, there are no failure conditions at runtime related to buffers.
       *  \sa RTBufferAttribute::getVariableName
       */
      NVSG_API void setVariableName( const std::string & name);

      /*! \brief Gets the CUDA/RT variable name this buffer will be bound to.
       *  \returns The name the buffer will be bound to.
       *  \sa RTBufferAttribute::setVariableName
       */
      NVSG_API const std::string & getVariableName() const;

      /*! \brief Test for equivalence with another RTBufferAttribute.
       *  \param p            Pointer to the constant RTBufferAttribute to test for equivalence with.
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c true
       *  \param deepCompare  Optional parameter to perform a deep comparsion; default is \c false
       *  \return \c true if the RTBufferAttribute \a p is equivalent to \c this, otherwise \c false.
       *  \sa StateAttribute, RTBuffer */
      NVSG_API virtual bool isEquivalent( const Object *p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      /*! \brief Assignment operator
      *  \param rhs A reference to the constant RTBufferAttribute to copy from
      *  \return A reference to the assigned RTBufferAttribute
      *  \remarks The assignment operator calls the assignment operator of StateAttribute and then
      *  copies the RTBufferAttribute specific data.
      *  \sa PrimitiveAttribute */
      NVSG_API RTBufferAttribute & operator=(const RTBufferAttribute & rhs);

      REFLECTION_INFO_API( NVSG_API, RTBufferAttribute );
    protected:
      friend struct nvutil::Holder<RTBufferAttribute>;

      /*! \brief Default-constructs a RTBufferAttribute. 
       */
      NVSG_API RTBufferAttribute();

      /*! \brief Constructs a RTBufferAttribute as a copy from another RTBufferAttribute.
       *  \param rhs Source RTBufferAttribute. */ 
      NVSG_API RTBufferAttribute( const RTBufferAttribute &rhs );

      /*! \brief Destructs a RTBufferAttribute. */
      NVSG_API virtual ~RTBufferAttribute();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      std::string     m_variableName; // name of variable in CUDA code
      unsigned char * m_buffer;
      RTformat        m_format;
      unsigned int    m_elementSize; 
      size_t          m_size;
      unsigned int    m_width, m_height, m_depth;
  };

inline void RTBufferAttribute::setVariableName( const std::string & name)
{
  m_variableName = name;
}

inline const std::string & RTBufferAttribute::getVariableName() const
{
  return m_variableName;
}

} // namespace nvsg

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

#include <nvrt/RTBufferGL.h>

namespace nvrt {

  /*! \brief Special RTBuffer which can synchronize data with an OpenGL texture.
   *  \par Namespace: nvrt
   *  \remarks The OpenGL texture does get updated with an call to updateTexture(). 
   *           It uses GL_TEXTURE_1D for 1D textures, GL_TEXTURE_2D for 2D textures 
   *           and GL_TEXTURE_3D for 3D textures.
   *           Besides that, it can be used like any other RTBuffer.
   *  \sa nvrt::RTBufferGL, nvrt::RTContext */
  class RTBufferGLTexture : public RTBufferGL
  {
    friend class RTContext;
  public:

    public:
      NVRT_API static RTBufferGLTextureSharedPtr create();

    /*! \brief Default-constructor of RTBufferGLTexture */
    NVRT_API RTBufferGLTexture();

    /*! \brief Destructor of RTBufferGLTexture */
    NVRT_API virtual ~RTBufferGLTexture();

    /*! \brief Retrieve the OpenGL texture id for this buffer. Do not cache this value 
               since it may change on each updateTexture() call
     *  \return OpenGL textureid
     \  \sa updateTexture()
    */
    NVRT_API GLuint getTextureId() const;

    /*! \brief Synchronize the content of the PBO to the texture. 
        \remarks It is possible that this may change the texture id, 
                 i.e. if the dimensionality of this buffer has changed
                 since the last call to this function. Due to this do not
                 cache the texture id.
    */
    NVRT_API void updateTexture();

    /*! \brief Retrieve the OpenGL texture target which has been used by this texture
    */
    NVRT_API GLenum getTextureTarget() const;

  protected:
    /*! \brief Initialize this object.
     *  \param context The RTContext to create the acceleration structure with.
     *  \param type Enumeration to specify the buffer type.
     *  \param internalFormat The internal OpenGL format for this texture
     *  \param format The OpenGL color format of the PBO for this texture
     *  \param dataType The OpenGL datatype of the PBO for this texture
     *  \remarks This function is called from RTContext::newBufferGLTexture to initialize the new buffer
     *  structure. The enumeration values for \a type are RT_BUFFER_INPUT, RT_BUFFER_OUTPUT, and
     *  RT_BUFFER_INPUT_OUTPUT
     *  \sa nvrt::RTContext */
    NVRT_API virtual void init( RTcontext context, RTbuffertype type, GLenum internalFormat, GLenum format, GLenum dataType);

    unsigned int m_textureDimensionality; //< The current texture dimensionality

    GLuint m_textureId;                   //< OpenGL texture id managed by this buffer

    GLenum m_internalFormat;              //< OpenGL internalFormat of the managed texture
    GLenum m_format;                      //< OpenGL format of the texture input data
    GLenum m_dataType;                    //< OpenGL datatype for the texture input data
  };

}

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

#include <nvsg/Texture.h>
#include <nvgl/RenderContextGL.h>


namespace nvgl
{

  /*! \brief Base class to represent an OpenGL texture.
   *  \remarks Various sub classes exist that represent specific OpenGL textures.
   */
  class TextureGL : public nvsg::Texture
  {
  public:
    /*! \brief Creates an appropriate child class of TextureGL based on the content of the TextureHost
     *  \param smTexImage Specifies the image data to be used. 
    **/
    NVSG_API static TextureGLSharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage);

  public:
    /*! \brief Returns the OpenGL texture object.
    **/
    NVSG_API GLuint  getGLId() const;

    /*! \brief Returns the OpenGL target of the texture (e.g. GL_TEXTURE_2D).
    **/
    NVSG_API GLenum  getTarget() const;

    /*! \brief Returns the OpenGL binding target of the texture (e.g. GL_TEXTURE_BINDING_2D).
    **/
    NVSG_API GLenum  getBindingTarget() const;

    /*! \brief Returns the OpenGL internal format of the texture (e.g. GL_RGBA8).
    **/
    NVSG_API GLenum  getInternalFormat() const;

   /*! \brief Returns the OpenGL client data format of the texture (e.g. GL_RGBA).
       \note The value is initially set in the constructor and is overwritten in setData functions.
       Use this function if you want to retrieve data in the format, which client data was
       last specified.
    **/
    NVSG_API GLenum  getClientDataFormat() const;

   /*! \brief Returns the OpenGL client data type of the texture (e.g. GL_UNSGINED_BYTE).
       \note The value is initially set in the constructor and is overwritten in setData functions.
       Use this function if you want to retrieve data in the type, which client data was
       last specified.
    **/
    NVSG_API GLenum  getClientDataType() const;

    /*! \brief Returns the border size of the texture.
    **/
    NVSG_API GLsizei getBorder() const;

    /*! \brief Returns whether the texture level has been defined.
     *  \note The levels can be defined through setData, create from TextureHost or generateMipMap.<br> Level 0 is also defined through resize operations.
    **/
    NVSG_API bool    isMipMapLevelDefined( GLuint level ) const;

    /*! \brief Returns whether the texture level can be used at all.
        \note This operation is the same as level <= getMaxLevel
    **/
    NVSG_API bool    isMipMapLevelValid( GLuint level ) const;

    /*! \brief Returns whether all necessary mipmap levels have been supplied. Once complete, mipmapped texture filtering can be used.
    **/
    NVSG_API bool    isMipMapComplete() const;

    /*! \brief Get the highest level the texture can have, based on its current size.
    **/
    NVSG_API GLuint  getMaxLevel() const;

    /*! \brief Binds texture and stores previous binding.
    **/
    NVSG_API void bind();

    /*! \brief Restores previous binding stored at bind.
    **/
    NVSG_API void unbind();

    /*! \brief Generates all mipmaps based on level of detail 0 using hardware functionality and sets all mipmap levels as defined.
     *  \note Only supported when EXT/ARB_framebuffer_object exist and if the texture has m_maxLevel > 0.
     *  The function also sets the texture base level to 0.
    **/
    NVSG_API void generateMipMap();

    /*! \brief Checks whether the specified dimensions are valid for the target.
     *  \return \c true if valid, otherwise \c false.
    **/
    NVSG_API static bool sizeValid( GLenum target, GLsizei w, GLsizei h, GLsizei d, GLsizei layers = 0 );

    /*! \brief Convenience function to compute the size at a given mipmap level.
    **/
    NVSG_API static GLsizei getMipMapSize( GLsizei sz, GLuint level );

    /*! \brief Returns the maximum size along one dimension (not layers) for the texture target.
     *  \note Use subclass implementation specific queries for additional data.
    **/
    NVSG_API static GLsizei getMaximumSize( GLenum target );

    /*! \brief Returns \c true if the texture target supports layers.
    **/
    NVSG_API static bool hasLayers( GLenum target );
    
    /*! \brief Returns \c true if the byte size of the compressed format can be computed.
    **/
    NVSG_API static bool isCompressedSizeKnown( GLenum format );

    /*! \brief Returns \c true if the texture format uses compressed data.
    **/
    NVSG_API static bool isCompressedFormat( GLenum format );

    /*! \brief Returns \c true if the texture internal format is interpreted as integers.
    **/
    NVSG_API static bool isIntegerInternalFormat( GLenum format );

    /*! \brief Calculates the byte size for the image dimensions.
     *  \return Returns memory size in bytes or \c 0 if the value can not be computed.
    **/
    NVSG_API static size_t getCompressedSize( GLenum format, GLsizei w, GLsizei h, GLsizei d, GLsizei layers = 0 );

    /*! \brief Calculates the byte size for the image dimensions.
     *  \return Returns memory size in bytes or \c 0 if the value can not be computed.
    **/
    NVSG_API static size_t  getDataSize(GLenum format, GLenum type, GLsizei w, GLsizei h, GLsizei d, GLsizei layers = 0 );

    /*! \brief Overrides Texture::isEquivalent.
      * \param p Pointer to the Texture to test for equivalence with this TextureGL object.
      * \param deepCompare The function performs a deep-compare instead of a shallow compare if this is \c true.
      * \return The function returns \c true if the Texture pointed to by \a p is detected to be equivalent
      * to this TextureGL object.
      * \remarks Two TextureGL are considered to be equivalent, if they are equivalent as Textures and have
      * the same GLId.
      * \sa Texture::isEquivalent */
    NVSG_API virtual bool isEquivalent( const Texture * p, bool deepCompare ) const;

  protected:
    friend struct nvutil::Holder<TextureGL>;

    /*! \brief Constructor for base class.
     *  \note By default all textures are set to nearest filtering without mipmaps to be renderable
     *  once first texture level is specified independent of all internal formats. Any other texture
     *  states, such as wrap modes, are in their original default state defined by OpenGL.
     */
    NVSG_API TextureGL();

    NVSG_API virtual ~TextureGL();

    /*! \brief Initialize this TextureGL.
     *  \param target The OpenGL target of the texture (e.g. GL_TEXTURE_2D).
     *  \param bindingTarget The OpenGL binding target of the texture (e.g. GL_TEXTURE_BINDING_2D).
     *  \param internalFormat The GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \param nvsgTarget The TextureTarget of this TextureGL (e.g. NVSG_TEXTURE_2D). */
    NVSG_API void init( GLenum target, GLenum bindingTarget, GLenum internalFormat, GLsizei border, 
                        GLenum format, GLenum type, nvsg::TextureTarget nvsgTarget = nvsg::NVSG_UNSPECIFIED_TEXTURE_TARGET );

    /*! \brief Feed the data of this object into the provied HashGenerator.
     *  \param hg The HashGenerator to update with the data of this object.
     *  \sa getHashKey */
    NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

  protected:
    std::vector<GLint>    m_bindingStack;     //!< A stack of bindings of this binding target
    GLuint                m_id;               //!< OpenGL id
    GLenum                m_target;           //!< The OpenGL target of the texture (e.g. GL_TEXTURE_2D)
    GLenum                m_bindingTarget;    //!< The OpenGL binding target of the texture (e.g. GL_TEXTURE_BINDING_2D)
    GLenum                m_internalFormat;   //!< The GL format used to store the data on the OpenGL server (e.g. GL_RGBA8)
    GLsizei               m_border;           //!< The texture border size
    GLenum                m_clientDataFormat; //!< The OpenGL client data format of the texture (e.g. GL_RGBA)
    GLenum                m_clientDataType;   //!< The OpenGL client data type of the texture (e.g. GL_UNSGINED_BYTE)
    GLbitfield            m_definedLevels;    //!< bitfield encoding which levels have been created already
    GLuint                m_maxLevel;         //!< must be specified when size of texture is changed
    ShareGroupGLSharedPtr m_shareGroupGL;     //!< The share group holding this texture
  };




  inline GLenum TextureGL::getGLId() const
  {
    return m_id;
  }

  inline GLenum TextureGL::getTarget() const
  {
    return m_target;
  }

  inline GLenum TextureGL::getBindingTarget() const
  {
    return m_bindingTarget;
  }

  inline GLenum TextureGL::getInternalFormat() const
  {
    return m_internalFormat;
  }

  inline GLenum TextureGL::getClientDataFormat() const
  {
    return m_clientDataFormat;
  }

  inline GLenum TextureGL::getClientDataType() const
  {
    return m_clientDataType;
  }

  inline GLsizei TextureGL::getBorder() const
  {
    return m_border;
  }

  inline GLuint TextureGL::getMaxLevel() const
  {
    return m_maxLevel;
  }

  inline bool TextureGL::isMipMapLevelValid( GLuint level ) const
  {
    return level <= m_maxLevel;
  }

  inline bool TextureGL::isMipMapLevelDefined( GLuint level ) const
  {
    return !!( m_definedLevels & ( 1 << level ));
  }

  inline bool TextureGL::isMipMapComplete() const
  {
    return (( 1 << (m_maxLevel + 1)) - 1) == m_definedLevels;
  }

  inline GLsizei TextureGL::getMipMapSize( GLsizei sz, GLuint level )
  {
    sz = (sz >> level);
    return sz ? sz : 1;
  }

  inline void TextureGL::feedHashGenerator( nvutil::HashGenerator & hg ) const
  {
    Texture::feedHashGenerator( hg );
    // based only on the m_id!
    hg.update( reinterpret_cast<const unsigned char *>(&m_id), sizeof(m_id) );
  }

  inline bool TextureGL::isEquivalent( const Texture * p, bool deepCompare ) const
  {
    bool equi = Texture::isEquivalent( p, deepCompare );
    if ( equi )
    {
      const TextureGL * tgl = dynamic_cast<const TextureGL *>( p );
      equi =  ( tgl != nullptr )
          &&  ( m_id == tgl->m_id );
    }
    return( equi );
  }

#if !defined( DOXYGEN_IGNORE )
  /*!*************/
  /* TextureGL1D */
  /*!*************/
  class TextureGL1D;
  CORE_TYPES( TextureGL1D, nvgl::TextureGL );
#endif

  /*! \brief Class for 1D OpenGL textures.
   */
  class TextureGL1D : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL1DSharedPtr create( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );

    /*! \brief Create a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width.
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL1DSharedPtr create( GLenum internalFormat, GLsizei width, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \note In debug the function asserts on fail, but in release it will return a nullptr
     */
    NVSG_API static TextureGL1DSharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
     */
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 );

    /*! \brief Transfers data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data to transfer.
     *  \param format The OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type The OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel The texture level (0 being base-level) the data is stored to.
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type, GLuint mipLevel = 0);

    /*! \brief Retrieves the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Retrieves the OpenGL texture data.
     *  \param data A pointer to the area to transfer the texture data to.
     *  \param format The OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type The OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Optional texture level (0 being base level) that data is transfered from. Default is 0.
     *  \note It is assumed that there's enough memory allocated at \a data to hold the transfered data.
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize(GLsizei width);

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

  protected:
    friend struct nvutil::Holder<TextureGL1D>;

    NVSG_API TextureGL1D() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei border, GLenum format, GLenum type );

  protected:
    int m_width;
  };


  inline GLsizei TextureGL1D::getWidth() const
  {
    return m_width;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!******************/
  /* TextureGL1DArray */
  /*!******************/
  class TextureGL1DArray;
  CORE_TYPES( TextureGL1DArray, nvgl::TextureGL );
#endif

  /*! \brief Class for 1D array OpenGL textures.
   *  \remarks This texture type requires additional hardware support.
   *  Array textures store multiple 1D textures in stacked layers.
   *  Texture interpolation is done within layers, but not between.
   *  \sa isSupported
   */
  class TextureGL1DArray : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL1DArraySharedPtr create( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width.
     *  \param layers The amount of texture layers.
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL1DArraySharedPtr create( GLenum internalFormat, GLsizei width, GLsizei layers, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \note In debug the function asserts on fail, but in release it will return a nullptr
     */
    NVSG_API static TextureGL1DArraySharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
     */
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param layer The layer that is updated.
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLint layer, GLuint mipLevel = 0 );

    /*! \brief Transfers data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data that is read.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param layer The layer that is updated.
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type, GLint layer, GLuint mipLevel = 0);

    /*! \brief Retrieves all layers of the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Retrieves all layers of the OpenGL texture data.
     *  \param data A pointer for data storage.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize( GLsizei width, GLsizei numLayers );

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getLayers() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

    /*! \brief Returns the maximum texture layers allowed in the current OpenGL context.
    **/
    NVSG_API static GLsizei getMaximumLayers();

    /*! \brief Returns \c true if this texture class is supported in the current OpenGL context.
    **/
    NVSG_API static bool isSupported();

  protected:
    friend struct nvutil::Holder<TextureGL1DArray>;

    NVSG_API TextureGL1DArray() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei layers, GLsizei border, GLenum format, GLenum type );

  protected:
    int m_width;
    int m_layers;
  };



  inline GLsizei TextureGL1DArray::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGL1DArray::getLayers() const
  {
    return m_layers;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!*************/
  /* TextureGL2D */
  /*!*************/
  class TextureGL2D;
  CORE_TYPES( TextureGL2D, nvgl::TextureGL );
#endif

  /*! \brief Class for 2D OpenGL textures.
   */
  class TextureGL2D : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL2DSharedPtr create( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width.
     *  \param height The texture height.
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL2DSharedPtr create( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \note In debug the function asserts on fail, but in release it will return a nullptr
     */
    NVSG_API static TextureGL2DSharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
     */
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data that is read.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type, GLuint mipLevel = 0);

    /*! \brief Retrieves the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Retrieves the OpenGL texture data.
     *  \param data A pointer for data storage.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize( GLsizei width, GLsizei height );

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

  protected:
    friend struct nvutil::Holder<TextureGL2D>;

    NVSG_API TextureGL2D() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei border, GLenum format, GLenum type );

  protected:
    int m_width;
    int m_height;
  };



  inline GLsizei TextureGL2D::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGL2D::getHeight() const
  {
    return m_height;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!********************/
  /* TextureGLRectangle */
  /*!********************/
  class TextureGLRectangle;
  CORE_TYPES( TextureGLRectangle, nvgl::TextureGL );
#endif

  /*! \brief Class for rectangle OpenGL textures.
   *  \remarks Rectangle textures do not support borders or mip map levels and
   *  are sampled using unnormalized texture coordinates.
   */
  class TextureGLRectangle : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGLRectangleSharedPtr create( GLenum internalFormat, GLenum format, GLenum type );

    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width.
     *  \param height The texture height.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGLRectangleSharedPtr create( GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \note In debug the function asserts on fail, but in release it will return a nullptr
     */
    NVSG_API static TextureGLRectangleSharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
    **/
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type );

    /*! \brief Transfers data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data that is read.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type );

    /*! \brief Retrieves the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr &buffer, size_t bufferOffset, GLenum format, GLenum type ) const;

    /*! \brief Retrieves the OpenGL texture data.
     *  \param data A pointer for data storage.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize(GLsizei width, GLsizei height);

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_RECTANGLE_TEXTURE_SIZE_ARB for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

  protected:
    friend struct nvutil::Holder<TextureGLRectangle>;

    NVSG_API TextureGLRectangle() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei height, GLenum format, GLenum type );

  protected:
    int m_width;
    int m_height;
  };



  inline GLsizei TextureGLRectangle::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGLRectangle::getHeight() const
  {
    return m_height;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!******************/
  /* TextureGL2DArray */
  /*!******************/
  class TextureGL2DArray;
  CORE_TYPES( TextureGL2DArray, nvgl::TextureGL );
#endif

  /*! \brief Class for 2D array OpenGL textures.
   *  \remarks This texture type requires additional hardware support.
   *  Array textures store multiple 2D textures in stacked layers.
   *  Texture interpolation is done within layers, but not between.
   *  \sa isSupported
   */
  class TextureGL2DArray : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL2DArraySharedPtr create( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width.
     *  \param height The texture height.
     *  \param layers The amount of texture layers.
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL2DArraySharedPtr create( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei layers, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \note In debug the function asserts on fail, but in release it will return a nullptr
     */
    NVSG_API static TextureGL2DArraySharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
     */
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param layer The layer that is updated.
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLint layer, GLuint mipLevel = 0 );

    /*! \brief Transfers data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data that is read.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param layer The layer that is updated.
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type, GLint layer, GLuint mipLevel = 0);

    /*! \brief Retrieves all layers of the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Retrieves all layers of the OpenGL texture data.
     *  \param data A pointer for data storage.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize( GLsizei width, GLsizei height, GLsizei layers );

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the texture layers.
    **/
    NVSG_API GLsizei getLayers() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

    /*! \brief Returns the maximum texture layers allowed in the current OpenGL context.
    **/
    NVSG_API static GLsizei getMaximumLayers();

    /*! \brief Returns \c true if this texture class is supported in the current OpenGL context.
    **/
    NVSG_API static bool isSupported();

  protected:
    friend struct nvutil::Holder<TextureGL2DArray>;

    NVSG_API TextureGL2DArray() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei layers, GLsizei border, GLenum format, GLenum type );

  protected:
    int m_width;
    int m_height;
    int m_layers;
  };

  inline GLsizei TextureGL2DArray::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGL2DArray::getHeight() const
  {
    return m_height;
  }

  inline GLsizei TextureGL2DArray::getLayers() const
  {
    return m_layers;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!*************/
  /* TextureGL3D */
  /*!*************/
  class TextureGL3D;
  CORE_TYPES( TextureGL3D, nvgl::TextureGL );
#endif

  /*! \brief Class for 3D OpenGL textures.
   *  \remarks This type is mostly used for volume rendering.
   */
  class TextureGL3D : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL3DSharedPtr create( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width.
     *  \param height The texture height.
     *  \param depth The texture depth.
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGL3DSharedPtr create( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \note In debug the function asserts on fail, but in release it will return a nullptr
     */
    NVSG_API static TextureGL3DSharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
     */
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 );

    /*! \brief Transfers data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data that is read.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type, GLuint mipLevel = 0);

    /*! \brief Retrieves the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Retrieves the OpenGL texture data.
     *  \param data A pointer for data storage.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize(GLsizei width, GLsizei height, GLsizei depth);

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the texture depth.
    **/
    NVSG_API GLsizei getDepth() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_3D_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

  protected:
    friend struct nvutil::Holder<TextureGL3D>;

    NVSG_API TextureGL3D() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLsizei border, GLenum format, GLenum type );

  protected:
    int m_width;
    int m_height;
    int m_depth;
  };

  inline GLsizei TextureGL3D::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGL3D::getHeight() const
  {
    return m_height;
  }

  inline GLsizei TextureGL3D::getDepth() const
  {
    return m_depth;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!******************/
  /* TextureGLCubemap */
  /*!******************/
  class TextureGLCubemap;
  CORE_TYPES( TextureGLCubemap, nvgl::TextureGL );
#endif

  /*! \brief Class for cubemap OpenGL textures.
   *  \remarks Cubemaps represent the six inner faces of a cube and are mostly used
   *  for environment effects (e.g. reflections).
   *  The cube faces are square and stored in the order +X,-X,+Y,-Y,+Z,-Z.
   */
  class TextureGLCubemap : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGLCubemapSharedPtr create( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width. Width and height must match.
     *  \param height The texture height. Width and height must match.
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGLCubemapSharedPtr create( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \param smTexImage The texture image used for the data. It must have 6 square 2D images. 
     *  \note In debug the function asserts on fail, but in release it will return a nullptr.
     */
    NVSG_API static TextureGLCubemapSharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \param smTexImage The texture image used for the data. It must have 6 square 2D images. 
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
     */
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param face Specifies the face of the cube from 0 to 5 (+X to -Z).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, int face, GLuint mipLevel = 0 );

    /*! \brief Transfers data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data that is read.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param face Specifies the face of the cube from 0 to 5 (+X to -Z).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type, int face, GLuint mipLevel = 0 );

    /*! \brief Retrieves the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param face Specifies the face of the cube from 0 to 5 (+X to -Z).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, int face, GLuint mipLevel = 0 ) const;

    /*! \brief Retrieves the OpenGL texture data.
     *  \param data A pointer for data storage.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param face Specifies the face of the cube from 0 to 5 (+X to -Z).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type, int face, GLuint mipLevel = 0 ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize(GLsizei width, GLsizei height);

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_CUBE_MAP_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

  protected:
    friend struct nvutil::Holder<TextureGLCubemap>;

    NVSG_API TextureGLCubemap() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei border, GLenum format, GLenum type );

  protected:
    int m_width;
    int m_height;
  };

  inline GLsizei TextureGLCubemap::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGLCubemap::getHeight() const
  {
    return m_height;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!***********************/
  /* TextureGLCubemapArray */
  /*!***********************/
  // TextureGLCubemapArray must have a layer count that is multiple of 6 (every 6 layers are a cubemap)
  class TextureGLCubemapArray;
  CORE_TYPES( TextureGLCubemapArray, nvgl::TextureGL );
#endif

  /*! \brief Class for cubemap array OpenGL textures.
   *  \remarks Cubemaps represent the six inner faces of a cube and are mostly used
   *  for environment effects (e.g. reflections).
   *  The cube faces are stored in the order +X,-X,+Y,-Y,+Z,-Z.
   *  Each layer encodes one cube face, so every six layers a new cubemap
   *  starts. This texture class requires additional hardware support.
   * \sa isSupported
   */
  class TextureGLCubemapArray : public TextureGL
  {
  public:
    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGLCubemapArraySharedPtr create( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture based on the given GL parameters.
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param width The texture width. Width and height must match.
     *  \param height The texture height. Width and height must match.
     *  \param layers The amount of texture layers.
     *  \param border The texture border size.
     *  \param format The OpenGL texture client format used in resize operations (e.g. GL_RGBA).
     *  \param type The OpenGL texture client type used in resize operations (e.g. GL_UNSIGNED_BYTE).
     *  \sa setData, resize */
    NVSG_API static TextureGLCubemapArraySharedPtr create( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei layers, GLsizei border, GLenum format, GLenum type );

    /*! \brief Creates a texture from a texture image.
     *  \param smTexImage The texture image used for the data. It must have an image count that is a multiple of 6.
     *  \note In debug the function asserts on fail, but in release it will return a nullptr
     */
    NVSG_API static TextureGLCubemapArraySharedPtr create( const nvsg::TextureHostSharedPtr &smTexImage );

  public:
    /*! \brief Sets the internal format, size and data from the texture image.
     *  \param smTexImage The texture image used for the data. It must have an image count that is a multiple of 6.
     *  \return Returns \c true on success, otherwise \c false (which triggers an assertion in debug build).
     */
    NVSG_API bool setData( const nvsg::TextureHostSharedPtr &smTexImage );

    /*! \brief Transfers the buffer data to the OpenGL texture, keeping current internal format and size.
     *  \param buffer The Buffer used to read the texture data from.
     *  \param bufferOffset The offset in bytes into the buffer where reading is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param layer The amount of texture layers.
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa getData */
    NVSG_API void setData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLint layer, GLuint mipLevel = 0 );

    /*! \brief Transfers data to the OpenGL texture, keeping current internal format and size.
     *  \param data A pointer to the data that is read.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param layer The amount of texture layers.
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa getData */
    NVSG_API void setData( const void *data, GLenum format, GLenum type, GLint layer, GLuint mipLevel = 0);

    /*! \brief Retrieves all layers of the OpenGL texture data into a buffer.
     *  \param buffer The Buffer used to write the texture data to.
     *  \param bufferOffset The offset in bytes into the buffer where writing is started from.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \note No checks on the buffer's size validity are performed.
     *  \sa setData */
    NVSG_API void getData( const nvsg::BufferSharedPtr & buffer, size_t bufferOffset, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Retrieves all layers of the OpenGL texture data.
     *  \param data A pointer for data storage.
     *  \param format Specifies the OpenGL client format of the data (e.g. GL_RGBA).
     *  \param type Specifies the OpenGL client type of the data (e.g. GL_FLOAT).
     *  \param mipLevel Specifies the texture level (0 being base-level) the data is stored to.
     *  \sa setData */
    NVSG_API void getData( void *data, GLenum format, GLenum type, GLuint mipLevel = 0 ) const;

    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize(GLsizei width, GLsizei height, GLsizei layers );

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the texture layers.
    **/
    NVSG_API GLsizei getLayers() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_CUBE_MAP_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

    /*! \brief Returns the maximum texture layers allowed in the current OpenGL context.
    **/
    NVSG_API static GLsizei getMaximumLayers();

    /*! \brief Returns \c true if this texture class is supported in the current OpenGL context.
    **/
    NVSG_API static bool isSupported();

  protected:
    friend struct nvutil::Holder<TextureGLCubemapArray>;

    NVSG_API TextureGLCubemapArray() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei border, GLenum format, GLenum type );
    NVSG_API void init( GLenum internalFormat, GLsizei width, GLsizei height, GLsizei layers, GLsizei border, GLenum format, GLenum type );

  protected:
    int m_width;
    int m_height;
    int m_layers;
  };

  inline GLsizei TextureGLCubemapArray::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGLCubemapArray::getHeight() const
  {
    return m_height;
  }

  inline GLsizei TextureGLCubemapArray::getLayers() const
  {
    return m_layers;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!*****************/
  /* TextureGLBuffer */
  /*!*****************/
  class TextureGLBuffer;
  CORE_TYPES( TextureGLBuffer, nvgl::TextureGL );
#endif

  /*! \brief Class for buffer OpenGL textures.
   *  \remarks This texture type requires dedicated hardware support.
   *  Buffer textures do not support texture filtering, instead
   *  the associated buffer's data is treated as large 1D array.
   *  The internal format defines how the array elements are interpreted.
   *  Therefore, the size of the texture depends on the buffer's
   *  size and the internal format.
   *  The data is directly sampled from the associated BufferGL,
   *  so any updates to it, will change the sampling result.
   *  \sa isSupported, isFormatValid, computeSize
   */
  class TextureGLBuffer : public TextureGL
  {
  public:
    /*! \brief Creates a texture that allows to address a BufferGL and interpret its data in the given format.
     *  \param smBufferGL The buffer whose data is referenced
     *  \param internalFormat Specifies how the buffer's data is interpreted when the texture is sampled (e.g. GL_RGBA32F)
     *  \sa setBuffer, getBuffer
    **/
    NVSG_API static TextureGLBufferSharedPtr create( const BufferGLSharedPtr &smBufferGL, GLenum internalFormat );

  public:
    /*! \brief Sets the BufferGL and the format of its data, that this texture addresses directly
     *  \param smBufferGL The buffer whose data is referenced
     *  \param internalFormat Specifies how the buffer's data is interpreted when the texture is sampled (e.g. GL_RGBA32F)
     *  \sa getBuffer */
    NVSG_API void setBuffer( const BufferGLSharedPtr &smBufferGL, GLenum internalFormat );

    /*! \brief Returns the BufferGL that is assigned to this Texture
     *  \sa setBuffer
     */
    NVSG_API const BufferGLSharedPtr & getBuffer() const;

    /*! \brief Computes the current size (amount of addressable texels) 
     *  based on the internal format and the buffer's size.
     *  \sa setBuffer
     */
    NVSG_API GLsizei getSize() const;

    /*! \brief Returns whether the internal format is supported by the current OpenGL context
    **/
    NVSG_API static bool isFormatValid( GLenum internalFormat );

    /*! \brief Returns \c true if this texture class is supported in the current OpenGL context.
    **/
    NVSG_API static bool isSupported();

    /*! \brief Compute the size (amount of addressable texels) depending on format and byte size of the buffer
    **/
    NVSG_API static GLsizei computeSize( size_t bytes, GLenum internalFormat );

    /*! \brief Returns the maximum size (amount of addressable texels) in the current OpenGL context.
    **/
    NVSG_API static GLsizei getMaximumSize();

  protected:
    friend struct nvutil::Holder<TextureGLBuffer>;

    NVSG_API TextureGLBuffer() {}

  private:
    NVSG_API void init();

  protected:
    BufferGLSharedPtr m_buffer;
  };

  inline const BufferGLSharedPtr & TextureGLBuffer::getBuffer() const
  {
    return m_buffer;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!************************/
  /* TextureGL2DMultisample */
  /*!************************/
  class TextureGL2DMultisample;
  CORE_TYPES( TextureGL2DMultisample, nvgl::TextureGL );
#endif

  /*! \brief Class for 2D multisample OpenGL textures.
   *  \remarks This texture type requires additional hardware support.
   *  Multisample textures store multiple samples per texel and
   *  are used as attachments for RenderTargetGLFBO.
   *  There is no support mipmapping nor texture filtering,
   *  however shaders can directly access samples individually.
   *  \sa isSupported
   */
  class TextureGL2DMultisample : public TextureGL
  {
  public:
    /*! \brief Creates a multi-sampled texture based on the given GL parameters 
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param samples The number of samples per pixel
     *  \sa resize, setSamples */
    NVSG_API static TextureGL2DMultisampleSharedPtr create( GLenum internalFormat, GLsizei samples=1 );

    /*! \brief Creates a multi-sampled texture based on the given GL parameters 
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param samples The number of samples per pixel
     *  \param width The texture width.
     *  \param height The texture height.
     *  \param fixedLocations When set to true, the location of the samples are the same for all internalFormats and depend only on sample count.
     *  Otherwise they can vary with each internalFormat.
     *  \sa resize, setSamples */
    NVSG_API static TextureGL2DMultisampleSharedPtr create( GLenum internalFormat, GLsizei samples, GLsizei width, GLsizei height, bool fixedLocations );

  public:
    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize(GLsizei width, GLsizei height);

    /*! \brief Changes the sample count of the texture. All content and mipmap levels are lost if the sample count is different from current state.
    **/
    NVSG_API void setSamples(GLsizei samples);

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the number of samples per pixel.
    **/
    NVSG_API GLsizei getSamples() const;

    /*! \brief Returns whether the texture uses fixed sample locations when being rendered to.
    **/
    NVSG_API bool getFixedLocations() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context.
    **/
    NVSG_API static GLsizei getMaximumSamples();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context, when a integer internal format is used.
    **/
    NVSG_API static GLsizei getMaximumIntegerSamples();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context, when a color-renderable internal format is used.
    **/
    NVSG_API static GLsizei getMaximumColorSamples();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context, when a depth-renderable internal format is used.
    **/
    NVSG_API static GLsizei getMaximumDepthSamples();

    /*! \brief Returns \c true if this texture class is supported in the current OpenGL context.
    **/
    NVSG_API static bool isSupported();

  protected:
    friend struct nvutil::Holder<TextureGL2DMultisample>;

    NVSG_API TextureGL2DMultisample() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei samples );
    NVSG_API void init( GLenum internalFormat, GLsizei samples, GLsizei width, GLsizei height, bool fixedLocations );

  protected:
    int m_width;
    int m_height;
    int m_samples;
    bool m_fixedLocations;
  };


  inline GLsizei TextureGL2DMultisample::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGL2DMultisample::getHeight() const
  {
    return m_height;
  }

  inline GLsizei TextureGL2DMultisample::getSamples() const
  {
    return m_samples;
  }

  inline bool TextureGL2DMultisample::getFixedLocations() const
  {
    return m_fixedLocations;
  }

#if !defined( DOXYGEN_IGNORE )
  /*!*****************************/
  /* TextureGL2DMultisampleArray */
  /*!*****************************/
  class TextureGL2DMultisampleArray;
  CORE_TYPES( TextureGL2DMultisampleArray, nvgl::TextureGL );
#endif

  /*! \brief Class for 2D multisample array OpenGL textures.
   *  \remarks This texture type requires additional hardware support.
   *  Multisample textures store multiple samples per texel and
   *  are used as attachments for RenderTargetGLFBO.
   *  There is no support mipmapping nor texture filtering,
   *  however shaders can directly access samples individually.
   *  Array textures store multiple textures in stacked layers.
   *  \sa isSupported
   */
  class TextureGL2DMultisampleArray : public TextureGL
  {
  public:
    /*! \brief Creates a multi-sampled texture based on the given GL parameters 
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param samples The number of samples per pixel
     *  Otherwise they can vary with each internalFormat.
     *  \sa resize, setSamples */
    NVSG_API static TextureGL2DMultisampleArraySharedPtr create( GLenum internalFormat, GLsizei samples=1 );

    /*! \brief Creates a multi-sampled texture based on the given GL parameters 
     *  \param internalFormat Specifies the GL format used to store the data on the OpenGL server (e.g. GL_RGBA8).
     *  \param samples The number of samples per pixel
     *  \param width The texture width.
     *  \param height The texture height.
     *  \param layers The amount of texture layers.
     *  \param fixedLocations When set to true, the location of the samples are the same for all internalFormats and depend only on sample count.
     *  Otherwise they can vary with each internalFormat.
     *  \sa resize, setSamples */
    NVSG_API static TextureGL2DMultisampleArraySharedPtr create( GLenum internalFormat, GLsizei samples, GLsizei width, GLsizei height, GLsizei layers, bool fixedLocations );

  public:
    /*! \brief Resizes the texture. All content and mipmap levels are lost if the size is different from current state.
    **/
    NVSG_API void resize(GLsizei width, GLsizei height, GLsizei layers);

    /*! \brief Changes the sample count of the texture. All content and mipmap levels are lost if the sample count is different from current state.
    **/
    NVSG_API void setSamples(GLsizei samples);

    /*! \brief Returns the texture width.
    **/
    NVSG_API GLsizei getWidth() const;

    /*! \brief Returns the texture height.
    **/
    NVSG_API GLsizei getHeight() const;

    /*! \brief Returns the texture layers.
    **/
    NVSG_API GLsizei getLayers() const;

    /*! \brief Returns the number of samples per pixel.
    **/
    NVSG_API GLsizei getSamples() const;

    /*! \brief Returns whether the texture uses fixed sample locations when being rendered to.
    **/
    NVSG_API bool getFixedLocations() const;

    /*! \brief Returns the maximum texture size allowed in the current OpenGL context.
     *  \note Uses GL_MAX_TEXTURE_SIZE for the query.
    **/
    NVSG_API static GLsizei getMaximumSize();

    /*! \brief Returns the maximum texture layers allowed in the current OpenGL context.
    **/
    NVSG_API static GLsizei getMaximumLayers();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context.
    **/
    NVSG_API static GLsizei getMaximumSamples();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context, when a integer internal format is used.
    **/
    NVSG_API static GLsizei getMaximumIntegerSamples();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context, when a color-renderable internal format is used.
    **/
    NVSG_API static GLsizei getMaximumColorSamples();

    /*! \brief Returns the maximum texture samples allowed in the current OpenGL context, when a depth-renderable internal format is used.
    **/
    NVSG_API static GLsizei getMaximumDepthSamples();

    /*! \brief Returns \c true if this texture class is supported in the current OpenGL context.
    **/
    NVSG_API static bool isSupported();

  protected:
    friend struct nvutil::Holder<TextureGL2DMultisampleArray>;

    NVSG_API TextureGL2DMultisampleArray() {}

  private:
    NVSG_API void init( GLenum internalFormat, GLsizei samples );
    NVSG_API void init( GLenum internalFormat, GLsizei samples, GLsizei width, GLsizei height, GLsizei layers, bool fixedLocations );

  protected:
    int m_width;
    int m_height;
    int m_samples;
    int m_layers;
    bool m_fixedLocations;
  };

  inline GLsizei TextureGL2DMultisampleArray::getWidth() const
  {
    return m_width;
  }

  inline GLsizei TextureGL2DMultisampleArray::getHeight() const
  {
    return m_height;
  }

  inline GLsizei TextureGL2DMultisampleArray::getLayers() const
  {
    return m_layers;
  }

  inline GLsizei TextureGL2DMultisampleArray::getSamples() const
  {
    return m_samples;
  }

  inline bool TextureGL2DMultisampleArray::getFixedLocations() const
  {
    return m_fixedLocations;
  }

  inline bool TextureGL::sizeValid( GLenum target, GLsizei w, GLsizei h, GLsizei d, GLsizei layers )
  {
    // init to zero so comparison at bottom will fail on unknown texture
    GLsizei maxw = 0;
    GLsizei maxh = 0;
    GLsizei maxd = 0;
    GLsizei maxl = 0;
    bool powerOfTwoCheck = true;

    switch( target )
    {
      case GL_TEXTURE_1D: 
        maxw = TextureGL1D::getMaximumSize();
        maxh = 1;
        maxd = 1;
        maxl = 0;
        break;

      case GL_TEXTURE_2D: 
        maxw = TextureGL2D::getMaximumSize();
        maxh = maxw;
        maxd = 1;
        maxl = 0;
        break;

      case GL_TEXTURE_RECTANGLE_ARB:
        maxw = TextureGLRectangle::getMaximumSize();
        maxh = maxw;
        maxd = 1;
        powerOfTwoCheck = false;
        maxl = 0;
        break;

      case GL_TEXTURE_3D:
        maxw = TextureGL3D::getMaximumSize();
        maxh = maxw;
        maxd = maxh;
        maxl = 0;
        break;

      case GL_TEXTURE_CUBE_MAP: 
        maxw = TextureGLCubemap::getMaximumSize();
        maxh = maxw;
        maxd = 1;
        maxl = 0;
        break;

      case GL_TEXTURE_1D_ARRAY_EXT:
        maxw = TextureGL1DArray::getMaximumSize();
        maxh = 1;
        maxd = 1;
        maxl = TextureGL1DArray::getMaximumLayers();
        if (!layers)
        {
          return false;
        }
        break;

      case GL_TEXTURE_2D_ARRAY_EXT: 
        maxw = TextureGL2DArray::getMaximumSize();
        maxh = maxw;
        maxd = 1;
        maxl = TextureGL2DArray::getMaximumLayers();
        if (!layers)
        {
          return false;
        }
        break;

      case GL_TEXTURE_CUBE_MAP_ARRAY: 
        maxw = TextureGLCubemapArray::getMaximumSize();
        maxh = maxw;
        maxd = 1;
        maxl = TextureGLCubemapArray::getMaximumLayers();
        if (!layers || (layers % 6 != 0))
        {
          return false;
        }
        break;

      case GL_TEXTURE_BUFFER:
        maxw = TextureGLBuffer::getMaximumSize();
        maxh = 1;
        maxd = 1;
        maxl = 0;
        break;

      case GL_TEXTURE_2D_MULTISAMPLE:
        maxw = TextureGL2DMultisample::getMaximumSize();
        maxh = maxw;
        maxd = 1;
        maxl = 0;
        break;

      case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        maxw = TextureGL2DMultisampleArray::getMaximumSize();
        maxh = maxw;
        maxd = 1;
        maxl = TextureGL2DMultisampleArray::getMaximumLayers();
        break;

      default:
        NVSG_ASSERT( !"texture target unknown!" );
        break;
    }

    // either we dont require a check, or we are only valid
    // when extension exists, or we are power of two
    powerOfTwoCheck = powerOfTwoCheck ? 
      (GLIsExtensionAvailable("GL_ARB_texture_non_power_of_two") || 
       (nvmath::isPowerOfTwo(w) && nvmath::isPowerOfTwo(h) && nvmath::isPowerOfTwo(d))) : true;


    return ( w <= maxw && h <= maxh && d <= maxd && layers <= maxl) && powerOfTwoCheck;
  }

  inline GLsizei TextureGL::getMaximumSize( GLenum target )
  {
    switch( target )
    {
      case GL_TEXTURE_1D: 
        return TextureGL1D::getMaximumSize();

      case GL_TEXTURE_2D: 
        return TextureGL2D::getMaximumSize();

      case GL_TEXTURE_RECTANGLE_ARB:
        return TextureGLRectangle::getMaximumSize();

      case GL_TEXTURE_3D:
        return TextureGL3D::getMaximumSize();

      case GL_TEXTURE_CUBE_MAP: 
        return TextureGLCubemap::getMaximumSize();

      case GL_TEXTURE_1D_ARRAY_EXT:
        return TextureGL1DArray::getMaximumSize();

      case GL_TEXTURE_2D_ARRAY_EXT: 
        return TextureGL2DArray::getMaximumSize();
      
      case GL_TEXTURE_CUBE_MAP_ARRAY:
        return TextureGLCubemapArray::getMaximumSize();

      case GL_TEXTURE_BUFFER:
        return TextureGLBuffer::getMaximumSize();

      case GL_TEXTURE_2D_MULTISAMPLE:
        return TextureGL2DMultisample::getMaximumSize();

      case GL_TEXTURE_2D_MULTISAMPLE_ARRAY:
        return TextureGL2DMultisampleArray::getMaximumSize();

      default:
        NVSG_ASSERT( !"texture target unknown!" );
        return 0;
        break;
    }
  }

} // namespace nvgl

#if !defined( DOXYGEN_IGNORE )
namespace nvutil {
  OBJECT_TRAITS( nvgl::TextureGL1D, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGL2D, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGLRectangle, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGL3D, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGLCubemap, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGL1DArray, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGL2DArray, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGLCubemapArray, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGLBuffer, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGL2DMultisample, nvgl::TextureGL );
  OBJECT_TRAITS( nvgl::TextureGL2DMultisampleArray, nvgl::TextureGL );
} // namespace nvutil
#endif

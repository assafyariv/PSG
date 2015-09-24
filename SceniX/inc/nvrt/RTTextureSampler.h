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

#include "optix.h"
#include "nvrt/RTBase.h"
#include "nvsg/CoreTypes.h"
#include "nvsgrtapi.h"
#include "nvsg/TextureHost.h"

namespace nvrt
{
  /*! \brief Wrapper class for an nvirt RTtexturesampler object.
   *  \par Namespace: nvrt
   *  \remarks An RTtexturesampler manages an array potentially mip-mapped images, where each
   *  mip level is represented by a buffer. Additionally, it holds some texture parameters like
   *  filtering modes, indexing mode, maximum anisotropy, read mode, and wrap mode.
   *  \note Currently, mip-mapping is not supported.
   *  \sa RTBase, RTBuffer */
  class RTTextureSampler : public RTBase
  {
    friend class RTContext;
    friend class RTVariableType<RTTextureSampler>;

    enum WrapCoord
    {
      WRAP_S = 0,
      WRAP_T = 1,
      WRAP_R = 2
    };

    public:
      NVRT_API static RTTextureSamplerSharedPtr create();

    public:
      /*! \brief Default-constructor of RTTextureSampler */
      NVRT_API RTTextureSampler();

      /*! \brief Destructor of RTTextureSampler */
      NVRT_API virtual ~RTTextureSampler();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_TEXTURE_SAMPLER. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Add a buffer as a texture.
       *  \param buffer A pointer to a buffer to use as a texture.
       *  \return The index of the added texture in this RTTextureSampler.
       *  \remarks The \a buffer will become the first level of the last texture image in this
       *  RTTextureSampler. No mip-maps are created.
       *  \sa addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTexture, removeTextureArray, removeTextureLevel,
       *  getNumberOfTextures, getNumberOfMipLevels, getTextureBuffer */
      NVRT_API unsigned int addTexture( const RTBufferWeakPtr & buffer );

      /*! \brief Add an array of buffers as a mip-mapped texture.
       *  \param count The number of buffers to add as one mip-mapped texture.
       *  \param buffers A pointer to an array of pointers to the buffers to add as one mip-mapped
       *  texture.
       *  \return The index of the added texture in this RTTextureSampler.
       *  \remarks The buffers will be used as one mip-mapped texture.
       *  \sa addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTexture, removeTextureArray, removeTextureLevel,
       *  getNumberOfTextures, getNumberOfMipLevels, getTextureBuffer */
      NVRT_API unsigned int addTexture( unsigned int count, const RTBufferWeakPtr * buffers );

      /*! \brief Add an array of buffers as a number of not mip-mapped textures.
       *  \param count The number of buffers to add as not mip-mapped textures.
       *  \param buffers A pointer to an array of pointers to the buffers to add as not mip-mapped
       *  textures.
       *  \return The index of the last added texture in this RTTextureSampler.
       *  \remarks Each buffer in \a buffers will become the first level of a texture image in this
       *  RTTextureSampler. No mip-maps are created.
       *  \sa addTexture, addTextureLevel, replaceTexture, replaceTextureArray, replaceTextureLevel,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API unsigned int addTextureArray( unsigned int count, const RTBufferWeakPtr * buffers );

      /*! \brief Add an array of array of buffers as a number of mip-mapped textures.
       *  \param count The number of arrays of buffers to add as mip-mapped textures.
       *  \param levelsCount A pointer to the array containing the number of levels per mip-mapped
       *  texture.
       *  \param buffers A pointer to an array of array of pointers to the buffers to add as
       *  mip-mapped textures.
       *  \return The index of the last added texture in this RTTextureSampler.
       *  \remarks The buffers will be used as mip-mapped texture.
       *  \sa addTexture, addTextureLevel, replaceTexture, replaceTextureArray, replaceTextureLevel,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API unsigned int addTextureArray( unsigned int count, unsigned int * levelsCount
                                           , const RTBufferWeakPtr ** buffers );

      /*! \brief Add a texture mip level to a texture image.
       *  \param textureIdx The index of the texture image to add a mip level to.
       *  \param buffer A pointer to the buffer to add as one mip level.
       *  \return The mip level of the added buffer.
       *  \note The behavior is undefined, if \a textureIdx is larger than or equal to the number of
       *  textures in this RTTextureSampler.
       *  \sa addTexture, replaceTexture, replaceTextureArray, replaceTextureLevel,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API unsigned int addTextureLevel( unsigned int textureIdx, const RTBufferWeakPtr & texture );

      /*! \brief Replace a complete texture image by a not mip-mapped buffer.
       *  \param textureIdx The index of the texture image to be replaced.
       *  \param buffer A pointer to the buffer to replace a complete texture image as a not mip-mapped texture.
       *  \return \c true, if the specified complete texture image was replaced by \a buffer as a
       *  not mip-mapped texture. Otherwise \c false.
       *  \note The behavior is undefined, if \a textureIdx is larger than or equal to the number of
       *  textures in this RTTextureSampler.
       *  \sa addTexture, addTextureLevel, replaceTextureArray, replaceTextureLevel,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API bool replaceTexture( unsigned int textureIdx, const RTBufferWeakPtr & texture );

      /*! \brief Replace a complete texture image by an array of buffers as a mip-mapped texture.
       *  \param textureIdx The index of the texture image to be replaced.
       *  \param count The number of buffers to be used as one mip-mapped texture.
       *  \param buffers A pointer to an array of pointers to the buffers to be used as one mip-mapped
       *  texture.
       *  \return \c true, if the specified complete texture image was replaced by \a buffers as a
       *  mip-mapped texture. Otherwise \c false.
       *  \note The behavior is undefined, if \a textureIdx is larger than or equal to the number of
       *  textures in this RTTextureSampler.
       *  \sa addTexture, addTextureLevel, replaceTextureArray, replaceTextureLevel,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API bool replaceTexture( unsigned int textureIdx, unsigned int count
                                  , const RTBufferWeakPtr * buffers );

      /*! \brief Replace some complete texture images by an array of buffers as that number of not
       *  mip-mapped textures.
       *  \param textureIdx The index of the first texture image to be replaced.
       *  \param count The number of buffers to replace complete texture images as not mip-mapped
       *  textures.
       *  \param buffers A pointer to an array of pointers to the buffers to replace complete
       *  texture images as not mip-mapped textures.
       *  \return \c true, if the specified complete texture images were replaced by \a buffers as
       *  not mip-mapped textures. Otherwise \c false.
       *  \remarks Each buffer in \a buffers will become the first level of a texture image in this
       *  RTTextureSampler. No mip-maps are created.
       *  \note The behavior is undefined, if the sum of \a textureIdx and \a count is larger than
       *  or equal to the number of textures in this RTTextureSampler.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureLevel,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API bool replaceTextureArray( unsigned int textureIdx, unsigned int count
                                       , const RTBufferWeakPtr * buffers );

      /*! \brief Replace some complete texture images by an array of arrays of buffers as that
       *  number of mip-mapped textures.
       *  \param textureIdx The index of the first texture image to be replaced.
       *  \param count The number of arrays of buffers to use as mip-mapped textures.
       *  \param levelsCount A pointer to the array containing the number of levels per mip-mapped
       *  texture.
       *  \param buffers A pointer to an array of array of pointers to the buffers to use as
       *  mip-mapped textures.
       *  \return \c true, if the specified complete texture images were replaced by \a buffers as
       *  mip-mapped textures. Otherwise \c false.
       *  \note The behavior is undefined, if the sum of \a textureIdx and \a count is larger than
       *  or equal to the number of textures in this RTTextureSampler.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureLevel,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API bool replaceTextureArray( unsigned int textureIdx, unsigned int count
                                       , unsigned int * levelsCount, const RTBufferWeakPtr ** buffers );

      /*! \brief Replace a single mip level of one texture image.
       *  \param textureIdx The index of the texture to replace a mip level in.
       *  \param mipLevel The index of the mip level to be replaced.
       *  \param buffer A pointer to the buffer to replace the specified mip level.
       *  \return \c true, if the mip level could be replaced. Otherwise \c false.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  removeTexture, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API bool replaceTextureLevel( unsigned int textureIdx, unsigned int mipLevel
                                       , const RTBufferWeakPtr & buffer );

      /*! \brief Remove a complete texture image.
       *  \param textureIdx The index of the texture image to remove.
       *  \remarks Any textures images after \a textureIdx are shifted one index down.
       *  \note The behavior is undefined, if \a textureIdx is larger than or equal to the number of
       *  textures in this RTTextureSampler.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTextureArray, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API void removeTexture( unsigned int textureIdx );

      /*! \brief Remove a set of consecutive textures
       *  \param textureIdx The index of the first texture to remove.
       *  \param count The number if textures to remove.
       *  \note The behavior is undefined, if the sum of \a textureIdx and \a count is larger than
       *  or equal to the number of textures in this RTTextureSampler.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTexture, removeTextureLevel, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API void removeTextureArray( unsigned int textureId, unsigned int count );

      /*! \brief Remove a single mip level of one texture image.
       *  \param textureIdx The index of the texture image to remove a mip level from.
       *  \param mipLevel The index of the mip level to remove.
       *  \remarks Any mip level in the specified texture following the removed level is shifted
       *  one level down. If this was the last level of the texture, any texture following this is
       *  shifted one index down.
       *  \note The behavior is undefined, if \a textureIdx is larger than or equal to the number of
       *  textures in this RTTextureSampler, or if \a mipLevel is larger than or equal to the number
       *  of mip levels of the specified texture.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTexture, removeTextureArray, getNumberOfTextures,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API void removeTextureLevel( unsigned int textureIdx, unsigned int mipLevel );

      /*! \brief Get the number of texture images in this RTTextureSampler.
       *  \return The number of texture images.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTexture, removeTextureArray, removeTextureLevel,
       *  getNumberOfMipLevels, getTextureBuffer */
      NVRT_API unsigned int getNumberOfTextures() const;

      /*! \brief Get the number of mip levels in the specified texture.
       *  \param textureIdx Optional index of the texture image. Default value is 0.
       *  \note The behavior is undefined, if \a textureIdx is larger than or equal to the number of
       *  textures in this RTTextureSampler.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTexture, removeTextureArray, removeTextureLevel,
       *  getNumberOfTextures, getTextureBuffer */
      NVRT_API unsigned int getNumberOfMipLevels( unsigned int textureIdx = 0 ) const;

      /*! \brief Get the buffer of a specified mip level of a specified texture.
       *  \param textureIdx Optional index of the texture image. Default value is 0.
       *  \param level Optional index of the mip level of the texture image. Default value is 0.
       *  \note The behavior is undefined, if \a textureIdx is larger than or equal to the number of
       *  textures in this RTTextureSampler, of if \a level is larger than or equal to the number o
       *  mip levels in the specified texture.
       *  \sa addTexture, addTextureArray, addTextureLevel, replaceTexture, replaceTextureArray,
       *  replaceTextureLevel, removeTexture, removeTextureArray, removeTextureLevel,
       *  getNumberOfTextures, getNumberOfMipLevels */
      NVRT_API RTBufferWeakPtr getTextureBuffer( unsigned int textureIdx = 0, unsigned int level = 0 ) const;

      /*! \brief Set the wrap mode for this RTTextureSampler.
       *  \param mode The wrap mode to set.
       *  \sa getWrapMode */
      NVRT_API void setWrapMode( WrapCoord wc, RTwrapmode mode );

      /*! \brief Get the wrap mode of this RTTextureSampler.
       *  \return The wrap mode of this RTTextureSampler.
       *  \sa setWrapMode */
      NVRT_API RTwrapmode getWrapMode( WrapCoord wc ) const;

      /*! \brief Set the filtering modes for this RTTextureSampler.
       *  \param minFilter The minification filter mode to set.
       *  \param magFilter The magnification filter mode to set.
       *  \param mipFilter The mip filter mode to set.
       *  \sa getFilteringModes */
      NVRT_API void setFilteringModes( RTfiltermode minFilter, RTfiltermode magFilter, RTfiltermode mipFilter );

      /*! \brief Get the filtering modes of this RTTextureSampler.
       *  \param minFilter A reference to get the minification filter mode.
       *  \param magFilter A reference to get the magnification filter mode.
       *  \param mipFilter A reference to get the mip filter mode.
       *  \sa setFilteringModes */
      NVRT_API void getFilteringModes( RTfiltermode &minFilter, RTfiltermode &magFilter, RTfiltermode &mipFilter ) const;

      /*! \brief Set the maximal anisotropy for this RTTextureSampler.
       *  \param value The maximal anistropy to set.
       *  \sa getMaxAnisotropy */
      NVRT_API void setMaxAnisotropy( float value );

      /*! \brief Get the maximal anisotropy of this RTTextureSampler.
       *  \return The maximal anisotropy of this RTTextureSampler.
       *  \sa setMaxAnisotropy */
      NVRT_API float getMaxAnisotropy() const;

      /*! \brief Set the read mode for this RTTextureSampler.
       *  \param readMode The read mode to set.
       *  \sa getReadMode */
      NVRT_API void setReadMode( RTtexturereadmode readMode );

      /*! \brief Get the read mode of this RTTextureSampler.
       *  \return The read mode of this RTTextureSampler.
       *  \sa setReadMode */
      NVRT_API RTtexturereadmode getReadMode() const;

      /*! \brief Set the indexing mode for this RTTextureSampler.
       *  \param indexMode The indexing mode to set.
       *  \sa getIndexingMode */
      NVRT_API void setIndexingMode( RTtextureindexmode indexMode );

      /*! \brief Get the indexing mode of this RTTextureSampler.
       *  \return The indexing mode of this RTTextureSampler.
       *  \sa setIndexingMode */
      NVRT_API RTtextureindexmode getIndexingMode() const;

      /*! \brief Update a RTTextureSampler from a TextureAttributeItem.
       *  \param taih The TextureAttributeItem to use to update the sampler.
       *  \param textureIdx Optional index of the texture image. Default value is 0.
       *  \param level Optional index of the mip level of the texture image. Default value is 0.
       *  \param preserveTexture If this flag is \c true, then the TextureHost attached to the TextureAttributeItem will not
       *         be freed after uploading, regardless of whether the TextureHost has the F_PRESERVE_IMAGE_AFTER_UPLOAD flag set.
       *  \remarks This function updates the buffer contents and all modes for the sampler based on the provided TextureAttributeItem.
       *  \sa nvrt::initFromTextureAttributeItem */
      NVRT_API bool updateFromTextureAttributeItem( const nvsg::TextureAttributeItemWeakPtr & taih
                                                  , unsigned int textureIdx = 0
                                                  , unsigned int textureLevel = 0
                                                  , bool preserveTexture = false );

      /*! \brief Update a RTTextureSampler from a TextureAttributeItem.
       *  \param taih The TextureAttributeItem to use to update the sampler.
       *  \param textureIdx Optional index of the texture image. Default value is 0.
       *  \param level Optional index of the mip level of the texture image. Default value is 0.
       *  \param preserveTexture If this flag is \c true, then the TextureHost attached to the TextureAttributeItem will not
       *         be freed after uploading, regardless of whether the TextureHost has the F_PRESERVE_IMAGE_AFTER_UPLOAD flag set.
       *  \remarks This function updates the buffer contents and all modes for the sampler based on the provided TextureAttributeItem.
       *  \sa nvrt::initFromTextureAttributeItem */
      NVRT_API bool updateFromTextureAttributeItem( const nvsg::TextureAttributeItemSharedPtr & taih
                                                  , unsigned int textureIdx = 0, unsigned int textureLevel = 0
                                                  , bool preserveTexture = false );

    protected :
      /*! \brief Initialize this object.
       *  \param context The RTcontext to create the RTTextureSampler with.
       *  \remarks This function is called from RTContext::newTextureSampler to initialize the new
       *  texture sampler.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Initialize this RTTextureSampler from a TextureAttributeItem.
       *  \param context A pointer to the RTContext to create the RTTextureSampler with.
       *  \param texture A pointer to the TextureAttributeItem to use to fill the values of this RTTextureSampler.
       *  \remarks This function is called from RTContext::newTextureSampler to initialize the new
       *  texture sampler.
       *  \sa nvrt::RTContext */
      NVRT_API void initFromTextureAttributeItem( RTContext * context, const nvsg::TextureAttributeItem * texture, bool preserveTexture = false );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTTextureSampler wraps the functionality of an nvirt::RTtexturesampler.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTtexturesampler getResource() const;

    private:
      NVRT_API void initCaches();
      NVRT_API void setTextureParameters( const nvsg::TextureAttributeItem * item, bool isInteger );
      NVRT_API bool updateBufferFromTextureHost( const nvrt::RTBufferWeakPtr & buf
                                                , const nvsg::TextureHostSharedPtr & tex, bool & isInteger
                                                , bool preserveTexture );

    private:
      unsigned int                                  m_maxTextureDepth;
      std::vector<std::vector<RTBufferSharedPtr> >  m_textureBuffers;
      RTtexturesampler                              m_textureSampler;
      RTwrapmode                                    m_wrapMode[3];
      RTfiltermode                                  m_minFilter;
      RTfiltermode                                  m_magFilter;
      RTfiltermode                                  m_mipFilter;
      float                                         m_maxAnisotropy;
      RTtexturereadmode                             m_readMode;
      RTtextureindexmode                            m_indexMode;
  };
}


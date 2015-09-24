// Copyright NVIDIA Corporation 2012
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

#include <nvsgcommon.h>
#include <nvsg/CoreTypes.h>
#include <nvutil/HandledObject.h>
#include <nvutil/HashGeneratorMurMur.h>
#include <nvutil/Incarnation.h>
#include <nvutil/ReadableObject.h>
#include <nvutil/WritableObject.h>

namespace nvsg
{
  //! Texture target specifier
  enum TextureTarget
  {
    NVSG_UNSPECIFIED_TEXTURE_TARGET = ~0, //!< indicates an unspecified target
    NVSG_TEXTURE_1D = 0,                  //!< indicates a 1-dimensional texture
    NVSG_TEXTURE_2D,                      //!< indicates a 2-dimensional texture
    NVSG_TEXTURE_3D,                      //!< indicates a 3-dimensional texture
    NVSG_TEXTURE_CUBE,                    //!< indicates a cube map
    NVSG_TEXTURE_1D_ARRAY,                //!< indicates a 1d texture array
    NVSG_TEXTURE_2D_ARRAY,                //!< indicates a 2d texture array
    NVSG_TEXTURE_RECTANGLE,               //!< indicates a non-power-of-two, rectangular texture
    NVSG_TEXTURE_CUBE_ARRAY,              //!< indicates a cube map array
    NVSG_TEXTURE_BUFFER,                  //!< indicates a buffer texture
    NVSG_TEXTURE_2D_MULTISAMPLE,          //!< indicates a 2-dimensional multisample texture
    NVSG_TEXTURE_2D_MULTISAMPLE_ARRAY,    //!< indicates a 2-dimensional multisample texture array
    NVSG_TEXTURE_TARGET_COUNT             //!< specifies the count of known texture targets
  };

  /** \brief Interface for textures used as storage for SceniX. 
   *  \sa TextureHost, TextureGL
  **/
  class Texture : public nvutil::HandledObject
  {
  public:
    NVSG_API Texture();
    NVSG_API virtual ~Texture();

    NVSG_API TextureTarget getTextureTarget() const;

    /*! \brief Get the hash key of this Object.
     *  \return The hash key of this Object.
     *  \remarks If the hash key is not valid, the virtual function feedHashGenerator() is called to
     *  recursively determine a hash string, using a HashGenerator, which then is converted into
     *  a HashKey.
     *  \sa feedHashGenerator */
    NVSG_API HashKey getHashKey() const;

    /*! \brief Tests whether this Texture is equivalent to another Texture.
      * \param p Pointer to the Texture to test for equivalence with this Texture object.
      * \param deepCompare The function performs a deep-compare instead of a shallow compare if this is \c true.
      * \return The function returns \c true if the Texture pointed to by \a p is detected to be equivalent
      * to this Texture object. */
    NVSG_API virtual bool isEquivalent( const Texture * p, bool deepCompare ) const;

  protected:
    /*! \brief Feed the data of this object into the provied HashGenerator.
     *  \param hg The HashGenerator to update with the data of this object.
     *  \sa getHashKey */
    NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    /*! \brief Invalidate the HashKey.
     *  \sa getHashKey, feedHashGenerator */
    NVSG_API void invalidateHashKey();

#if !defined(NDEBUG)
    NVSG_API bool isHashKeyValid() const;
#endif

  protected:
    TextureTarget     m_textureTarget;

  private:
    nvutil::SWMRSync  m_lock;      // helps managing exclusive access to mutable members while reading the object 
    mutable HashKey   m_hashKey;
    mutable bool      m_hashKeyValid;
  };

  inline Texture::Texture()
    : m_textureTarget( NVSG_UNSPECIFIED_TEXTURE_TARGET )
    , m_hashKeyValid(false)
  {
  }

  inline Texture::~Texture()
  {
  }

  inline TextureTarget Texture::getTextureTarget() const
  {
    return m_textureTarget;
  }

  inline HashKey Texture::getHashKey() const
  {
    nvutil::AutoLock lock(m_lock); // secure exclusive write access to mutable members!
    if ( ! m_hashKeyValid )
    {
      nvutil::HashGeneratorMurMur hg;
      feedHashGenerator( hg );
      hg.finalize( (unsigned int *)&m_hashKey );
      m_hashKeyValid = true;
    }
    return( m_hashKey );
  }

  inline bool Texture::isEquivalent( const Texture * p, bool deepCompare ) const
  {
    return( m_textureTarget == p->m_textureTarget );
  }

  inline void Texture::feedHashGenerator( nvutil::HashGenerator & hg ) const
  {
    hg.update( reinterpret_cast<const unsigned char *>(&m_textureTarget), sizeof(m_textureTarget) );
  }

  inline void Texture::invalidateHashKey()
  {
    m_hashKeyValid = false;
  }

#if !defined(NDEBUG)
  inline bool Texture::isHashKeyValid() const
  {
    return( m_hashKeyValid );
  }
#endif

} // namespace nvsg

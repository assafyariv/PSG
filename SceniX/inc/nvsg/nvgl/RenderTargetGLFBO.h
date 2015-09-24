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

#include <nvgl/RenderTargetGL.h>
#include <nvgl/RenderTargetGLFB.h>
#include <nvgl/TextureGL.h>
#include <nvgl/RenderbufferGL.h>

#include <map>
#include <vector>

namespace nvgl
{
  class RenderTargetGLFBO;
  typedef nvutil::SmartPtr<RenderTargetGLFBO> SmartRenderTargetGLFBO;

  /** \brief This RenderTarget supports OpenGL offscreen rendering with FBOs. It is possible to render to
             nvgl::TextureGL objects for streaming textures and nvgl::RenderbufferGL objects. Stereo is also
             supported by attaching separate objects to the left and right eye targets.

             RenderTargetGL renders to Attachments. Currently there are two types of Attachments available,
             nvgl::RenderTargetGLFBO::AttachmentTexture for rendering to OpenGL textures and
             nvgl::RenderTargetGLFBO::AttachmentRenderbuffer for rendering on OpenGL renderbuffers.

             Attachments can be attached with the setAttachment methods. It is possible to share attachments between
             multiple RenderTargetGLFBO classes or the left and right eye.

             It is not necessary to resize attachments to the size of the RenderTarget. Attachments are resized
             automatically during RenderTargetGLFBO::beginRendering.
   **/
  class RenderTargetGLFBO : public RenderTargetGL
  {
  public:
    /***************/
    /* Attachment */
    /***************/
    /** \brief Base class for all attachments of nvgl::RenderTargetGLFBO.
        \sa    nvgl::RenderTargetGLFBO::AttachmentTexture and nvgl::RenderTargetGLFBO::AttachmentRenderbuffer **/
    class Attachment : public nvutil::RCObject {
    protected:
      friend class RenderTargetGLFBO;

      /** \brief Interface function to resize the attachment.
          \param width New width for the attachment.
          \param height New height for the attachment.
      **/
      NVSG_API virtual void resize(int width, int height) = 0;

      /** \brief Bind the attachment to the current framebuffer.
          \param target Target to bind the attachment to.
      **/
      NVSG_API virtual void bind( GLenum target ) = 0;

      /** \brief Remove the binding for the given target.
          \param param target The binding for the given target will be removed.
      **/
      NVSG_API virtual void unbind( GLenum target ) = 0;
    };

    typedef nvutil::SmartPtr<Attachment> SmartAttachment;

    /*********************/
    /* AttachmentTexture */
    /*********************/
    /** \brief Class to attach an TextureGL object to a RenderTargetFBO object.
        \sa nvgl::RenderTargetGLFBO::setAttachment */
    class AttachmentTexture : public Attachment
    {
    public:
      /** \brief Constructor for 1D textures **/
      NVSG_API AttachmentTexture( const TextureGL1DSharedPtr &texture, int level = 0 );

      /** \brief Constructor for 2D textures **/
      NVSG_API AttachmentTexture( const TextureGL2DSharedPtr &texture, int level = 0 );

      /** \brief Constructor for 3D textures **/
      NVSG_API AttachmentTexture( const TextureGL3DSharedPtr &texture, int zoffset, int level = 0 );

      /** \brief Constructor for 1D array textures **/
      NVSG_API AttachmentTexture( const TextureGL1DArraySharedPtr &texture, int layer, int level = 0 );

      /** \brief Constructor for 2D array textures **/
      NVSG_API AttachmentTexture( const TextureGL2DArraySharedPtr &texture, int layer, int level = 0);

      /** \brief Constructor for cubemap textures **/
      NVSG_API AttachmentTexture( const TextureGLCubemapSharedPtr &texture, int face, int level = 0 );

      /** \brief Constructor for rectangle textures **/
      NVSG_API AttachmentTexture( const TextureGLRectangleSharedPtr &texture );

      /** \brief Constructor for 2D multisample textures **/
      NVSG_API AttachmentTexture( const TextureGL2DMultisampleSharedPtr &texture );

      /** \brief Constructor for 2D multisample array textures **/
      NVSG_API AttachmentTexture( const TextureGL2DMultisampleArraySharedPtr &texture, int level = 0 );

      /** \brief Get the attached textureGL object.
          \return The attached textureGL object.
      **/
      NVSG_API TextureGLSharedPtr getTextureGL() const;

    protected:
      /** \brief Resize the texture to the given size.
          \param width New width for the texture.
          \param height New height for the texture.
      **/
      NVSG_API virtual void resize(int width, int height);

      /** \brief Bind the texture to the given target of the framebuffer.
          \param param target. The texture will be bound to the given target of the current framebuffer.
      **/
      NVSG_API virtual void bind( GLenum target );

      /** \brief Remove the texture binding for given a target of the current framebuffer.
          \param param target The binding for the given target will be removed.
      ´**/
      NVSG_API virtual void unbind( GLenum target );

      NVSG_API void init( const TextureGLSharedPtr &texture, GLenum target, GLenum level, GLenum zoffset );

      /** \brief Resize function for 1D textures. **/
      NVSG_API void resizeTexture1D( int width, int height );

      /** \brief Resize function for 2D textures. **/
      NVSG_API void resizeTexture2D( int width, int height );

      /** \brief Resize function for 3D textures. **/
      NVSG_API void resizeTexture3D( int width, int height );

      /** \brief Resize function for 1D Array textures. **/
      NVSG_API void resizeTexture1DArray( int width, int height );

      /** \brief Resize function for 2D array textures. **/
      NVSG_API void resizeTexture2DArray( int width, int height );

      /** \brief Resize function for cubemap textures. **/
      NVSG_API void resizeTextureCubemap( int width, int height );

      /** \brief Resize function for 2D multisample textures. **/
      NVSG_API void resizeTexture2DMultisample( int width, int height );

      /** \brief Resize function for 2D multisample array textures. **/
      NVSG_API void resizeTexture2DMultisampleArray( int width, int height );

      /** \brief Bind a 1D texture to an attachment.
          \param attachment Attachment to bind the texture to.
          \param textureId OpenGL texture name which to bind.
      **/
      NVSG_API void bind1D( GLenum attachment, GLuint textureId );

      /** \brief Bind a 2D texture to an attachment.
          \param attachment Attachment to bind the texture to.
          \param textureId OpenGL texture name which to bind.
      **/
      NVSG_API void bind2D( GLenum attachment, GLuint textureId );

      /** \brief Bind a 3D texture to an attachment.
          \param attachment Attachment to bind the texture to.
          \param textureId OpenGL texture name which to bind.
      **/
      NVSG_API void bind3D( GLenum attachment, GLuint textureId );

      /** \brief Bind a layered texture to an attachment.
          \param attachment Attachment to bind the texture to.
          \param textureId OpenGL texture name to bind.
      **/
      NVSG_API void bindLayer( GLenum attachment, GLuint textureId );

    private:
      /** \brief Function pointer to resize the attached texture type **/
      void (AttachmentTexture::*m_resizeFunc)(int width, int height);

      /** \brief Function pointer to bind the attached texture type to the framebuffer. **/
      void (AttachmentTexture::*m_bindFunc)( GLenum target, GLuint textureId );

      GLenum m_textureTarget;
      GLuint m_level;
      GLuint m_zoffset;
      TextureGLSharedPtr m_textureGL;
    };

    typedef nvutil::SmartPtr<AttachmentTexture> SmartAttachmentTexture;

    /**************************/
    /* AttachmentRenderbuffer */
    /**************************/
    /** \brief Class to attach an OpenGL renderbuffer to an nvgl::RenderTargetGLFBO.
        \sa nvgl::RenderTargetGLFBO::setAttachment */
    class AttachmentRenderbuffer : public Attachment
    {
    public:
      /** \brief Constructor for an Attachment with a renderbuffer.
          \param renderbufferGL RenderbufferGL to use for this attachment.
      **/
      NVSG_API AttachmentRenderbuffer( SmartRenderbufferGL renderbufferGL );
      NVSG_API virtual ~AttachmentRenderbuffer();

    protected:
      /** \brief Resize the renderbuffer.
          \param width New width for the renderbuffer.
          \param height New height for the renderbuffer.
      **/
      NVSG_API virtual void resize(int width, int height);

      /** \brief Bind the renderbuffer to the current framebuffer.
          \param param target. The renderbuffer will be bound to the given target of the current framebuffer.
      ´**/
      NVSG_API virtual void bind( GLenum target );

      /** \brief Remove the renderbuffer binding for given a target of the current framebuffer.
          \param param target The binding for the given target will be removed.
      ´**/
      NVSG_API virtual void unbind( GLenum target );

      /** \brief Get the RenderBufferGL object of this attachment.
          \return SmartRenderbufferGL object used by this attachment.
      **/
      NVSG_API SmartRenderbufferGL getRenderbufferGL() const;
    private:
      SmartRenderbufferGL m_renderbufferGL;
    };

    typedef nvutil::SmartPtr<AttachmentRenderbuffer> SmartAttachmentRenderbuffer;

    // RenderTargetGL interface
    enum {
       COLOR_ATTACHMENT0           = GL_COLOR_ATTACHMENT0_EXT
      ,COLOR_ATTACHMENT1           = GL_COLOR_ATTACHMENT1_EXT
      ,COLOR_ATTACHMENT2           = GL_COLOR_ATTACHMENT2_EXT
      ,COLOR_ATTACHMENT3           = GL_COLOR_ATTACHMENT3_EXT
      ,COLOR_ATTACHMENT4           = GL_COLOR_ATTACHMENT4_EXT
      ,COLOR_ATTACHMENT5           = GL_COLOR_ATTACHMENT5_EXT
      ,COLOR_ATTACHMENT6           = GL_COLOR_ATTACHMENT6_EXT
      ,COLOR_ATTACHMENT7           = GL_COLOR_ATTACHMENT7_EXT
      ,COLOR_ATTACHMENT8           = GL_COLOR_ATTACHMENT8_EXT
      ,COLOR_ATTACHMENT9           = GL_COLOR_ATTACHMENT9_EXT
      ,COLOR_ATTACHMENT10          = GL_COLOR_ATTACHMENT10_EXT
      ,COLOR_ATTACHMENT11          = GL_COLOR_ATTACHMENT11_EXT
      ,COLOR_ATTACHMENT12          = GL_COLOR_ATTACHMENT12_EXT
      ,COLOR_ATTACHMENT13          = GL_COLOR_ATTACHMENT13_EXT
      ,COLOR_ATTACHMENT14          = GL_COLOR_ATTACHMENT14_EXT
      ,COLOR_ATTACHMENT15          = GL_COLOR_ATTACHMENT15_EXT
      ,DEPTH_ATTACHMENT            = GL_DEPTH_ATTACHMENT_EXT
      ,STENCIL_ATTACHMENT          = GL_STENCIL_ATTACHMENT_EXT
      ,DEPTH_STENCIL_ATTACHMENT    = GL_DEPTH_STENCIL_ATTACHMENT
    };

    enum {
       COLOR_BUFFER_BIT            = GL_COLOR_BUFFER_BIT
      ,DEPTH_BUFFER_BIT            = GL_DEPTH_BUFFER_BIT
      ,STENCIL_BUFFER_BIT          = GL_STENCIL_BUFFER_BIT
    };

    enum {
       NEAREST                     = GL_NEAREST
      ,LINEAR                      = GL_LINEAR
    };

    typedef unsigned int AttachmentTarget;
    typedef unsigned int BlitMask;
    typedef unsigned int BlitFilter;

  protected:
    NVSG_API RenderTargetGLFBO( const SmartRenderContextGL &glContext );
  public:
    /** \brief Create a new RenderTargetFBO object using the given context */
    NVSG_API static nvutil::SmartPtr<RenderTargetGLFBO> create( const SmartRenderContextGL &glContext );

    NVSG_API virtual ~RenderTargetGLFBO();
 
    NVSG_API virtual nvsg::TextureHostSharedPtr getTextureHost( nvsg::Image::PixelFormat pixelFormat = nvsg::Image::IMG_BGR, nvsg::Image::PixelDataType = nvsg::Image::IMG_UNSIGNED_BYTE );

    NVSG_API virtual bool isValid();

    // Stereo API
    /** \brief Change stereo state on this RenderTargetGLFBO.
        \param stereoEnabled Stereo will be enabled if stereoEnabled is true. Otherwise it'll be disabled.
    **/
    NVSG_API virtual void setStereoEnabled( bool stereoEnabled);
    NVSG_API virtual bool isStereoEnabled() const;

    NVSG_API virtual bool setStereoTarget( StereoTarget target );
    NVSG_API virtual RenderTargetGL::StereoTarget getStereoTarget() const;

    /** \brief Remove all attachments for the given stereo target.
        \param stereoTarget All attachments of the given stereoTarget will be removed.
    **/
    NVSG_API void clearAttachments( StereoTarget stereoTarget = RenderTargetGL::LEFT_AND_RIGHT );

    /** \brief Sets the attachment for a given target.
        \param target The attachment will be attached to the given target.
        \param attachment Attachment to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const SmartAttachment &attachment, StereoTarget stereoTarget = RenderTargetGL::LEFT );

    // convenience functions to set an attachment
    /** \brief Attach a 1d texture.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \param level Mipmap level to use for the attachment
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGL1DSharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT, int level = 0 );

    /** \brief Attach a 2d texture.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \param level Mipmap level to use for the attachment
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGL2DSharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT, int level = 0 );

    /** \brief Attach a 3d texture.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \param zoffset zoffset of the 3d texture to use for the attachment.
        \param level Mipmap level to use for the attachment.
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGL3DSharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT, int zoffset = 0, int level = 0 );

    /** \brief Attach a 1d texture array.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \param layer Layer of the array to use for the attachment
        \param level Mipmap level to use for the attachment.
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGL1DArraySharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT, int layer = 0, int level = 0 );

    /** \brief Attach a 2d texture array.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \param layer Layer of the array to use for the attachment
        \param level Mipmap level to use for the attachment.
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGL2DArraySharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT, int layer = 0, int level = 0 );

    /** \brief Attach a cubemap.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \param face Face of the cubemap to use for the attachment
        \param level Mipmap level to use for the attachment.
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGLCubemapSharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT, int face = 0, int level = 0 );

    /** \brief Attach a 2d rectangular texture.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGLRectangleSharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT );

    /** \brief Attach a 2D multisample texture.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \return true if the operation was successful, false otherwise.
        **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGL2DMultisampleSharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT );

    /** \brief Attach a 2D multisample array texture.
        \param target The attachment will be attached to the given target.
        \param texture Texture to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \return true if the operation was successful, false otherwise.
        **/
    NVSG_API bool setAttachment( AttachmentTarget target, const TextureGL2DMultisampleArraySharedPtr &texture, StereoTarget stereoTarget = RenderTargetGL::LEFT, int layer = 0 );

    /** \brief Attach a renderbuffer.
        \param target The attachment will be attached to the given target.
        \param buffer Renderbuffer to attach.
        \param stereoTarget For stereo rendering it's possible to assign the attachment to the LEFT, RIGHT or LEFT_AND_RIGHT eye.
        \return true if the operation was successful, false otherwise.
    **/
    NVSG_API bool setAttachment( AttachmentTarget target, const SmartRenderbufferGL &buffer, StereoTarget stereoTarget = RenderTargetGL::LEFT );

    /** \brief Get the attachment for a given target
        \param target Target of the FramebufferObject for the query.
        \param stereoTarget LEFT or RIGHT for the eye.
        \return Attachment for the given parameters.
    **/
    NVSG_API SmartAttachment getAttachment( AttachmentTarget target, StereoTarget stereoTarget = RenderTargetGL::LEFT );

    /** \brief Set which targets of the framebuffer object should be active
        \param drawBuffers Vector of GLenums with attachment names
    **/
    NVSG_API void setDrawBuffers( const std::vector<GLenum> &drawBuffers );

    /** \brief Select the attachment which should be used for read operations on the framebuffer object
        \param readBuffer attachment name of the buffer to read.
    **/
    NVSG_API void setReadBuffer( GLenum readBuffer );

    struct BlitRegion
    {
      BlitRegion( int x, int y, int width, int height ) :
        x(x),y(y),width(width),height(height)
      {
      }
      BlitRegion() :
        x(0), y(0), width(0), height(0)
      {
      }
      
      int x;
      int y;
      int width;
      int height;
    };

   
    NVSG_API void blit( const SmartRenderTargetGLFBO & destination, const BlitMask & mask = COLOR_BUFFER_BIT, 
                        const BlitFilter & filter = NEAREST );
    NVSG_API void blit( const SmartRenderTargetGLFB & destination, const BlitMask & mask = COLOR_BUFFER_BIT, 
                        const BlitFilter & filter = NEAREST );
    NVSG_API void blit( const SmartRenderTargetGLFBO & destination, const BlitMask & mask, 
                        const BlitFilter & filter, const BlitRegion & destRegion, 
                        const BlitRegion & srcRegion );
    NVSG_API void blit( const SmartRenderTargetGLFB & destination, const BlitMask & mask, 
                        const BlitFilter & filter, const BlitRegion & destRegion, 
                        const BlitRegion & srcRegion );

    /** \brief Get the OpenGL framebuffer name of this object.
        \return OpenGL name of this object.
    **/
    NVSG_API GLuint getFramebufferId( void ) const { return m_framebuffer; }

    /** \brief Test if framebuffer objects are supported.
        \return true if framebuffer objects are supported, false otherwise.
    **/
    NVSG_API static bool isSupported();
    
    /** \brief Test if multiple rendertargets are supported.
        \return true if multiple rendertargets are supported, false otherwise.
    **/
    NVSG_API static bool isMultiTargetSupported();

    /** \brief Test if blitting between two framebuffers is supported.
        \return true if blitting is supported, false otherwise.
    **/
    NVSG_API static bool isBlitSupported();
  protected:
    NVSG_API int getStereoTargetId( StereoTarget stereoTarget ) const; // 0 -> left eye/mono, 1 -> right eye, assert on LEFT_AND_RIGHT

    NVSG_API void blit( const int & framebufferId, const BlitMask & mask, const BlitFilter & filter,
                        const BlitRegion & destRegion, const BlitRegion & srcRegion );

    NVSG_API virtual void makeCurrent();
    NVSG_API virtual void makeNoncurrent();

    NVSG_API void bindAttachments( StereoTarget stereoTarget );
    NVSG_API void resizeAttachments( StereoTarget stereoTarget );
    NVSG_API bool isFramebufferComplete();

    GLuint m_framebuffer;

    std::vector<GLenum> m_drawBuffers; //!< List of drawbuffers to activate for rendering
    GLenum              m_readBuffer;  //!< read buffer to activate for rendering
    std::vector<GLint> m_bindingStack;    //!< Bind stack for FBO

    typedef std::map<GLenum,SmartAttachment> AttachmentMap;

    AttachmentMap m_attachments[2]; //<! left/right eye attachments
    AttachmentMap m_attachmentChanges[2];

    // Stereo API
    bool         m_stereoEnabled;
    StereoTarget m_stereoTarget;
    int          m_currentlyBoundAttachments;
  };
}

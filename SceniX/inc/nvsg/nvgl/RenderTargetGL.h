// Copyright NVIDIA Corporation 2010-2011
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

#include <nvui/RenderTarget.h>
#include <nvgl/RenderContextGL.h>
#include <nvsg/Buffer.h>

namespace nvgl
{

  /** \brief RenderTargetGL is the base class for OpenGL based RenderTargets like nvgl::RenderTargetGLFB
             and nvgl::RenderTargetGLFBO. It contains basic OpenGL functionality like OpenGL context 
             managment, framebuffer grabbing and clearing the background.
  **/
  class RenderTargetGL : public nvui::RenderTarget
  {
    // RenderTargetGL interface
  public:
    NVSG_API virtual ~RenderTargetGL();

    /** \brief Make the OpenGL context attached to this target current and clear the surface with
               the clearMask set by setClearMask. beginRendering and endRendering cannot be nested.
        \return false If rendering on the target is not possible.
    **/
    NVSG_API virtual bool beginRendering();

    /** \brief Make the current OpenGL context noncurrent.
    **/
    NVSG_API virtual void endRendering();

    /** \brief Copy the content of the framebuffer attached to this RenderTargetGL into an nvsg::Buffer.
        \param mode OpenGL color buffer mode (i.e. GL_FRONT_LEFT or GL_COLOR_ATTACHMENT_0 ).
        \param pixelFormat SceniX Format to use for pixels in the resulting buffer.
        \param pixelDataType SceniX datatype to use for pixels in the resulting buffer.
        \param buffer A SceniX buffer which is being used for the resulting pixels.
        \return true If operation succeeded, false otherwise.
    **/
    NVSG_API virtual bool copyToBuffer( GLenum  mode, nvsg::Image::PixelFormat pixelFormat, nvsg::Image::PixelDataType pixelDataType, const nvsg::BufferSharedPtr &buffer );

    // clear interface
    /** \brief Sets the OpenGL clear mask used by beginRendering
        \param clearMask clearMask used for glClear calls.
        \remarks The initial value for the clear mask is 0.
    **/
    NVSG_API void setClearMask( GLenum clearMask );

    /** \brief Get the current OpenGL clear mask
        \return Current clear mask used for glClear calls.
    **/
    NVSG_API GLenum getClearMask();

    /** \brief Set the background color for glClear calls
        \param r red value
        \param g green value
        \param b blue value
        \param a alpha value
        \remarks The initial values for all components are 0.0.
    */
    NVSG_API void setClearColor( GLclampf r, GLclampf g, GLclampf b, GLclampf a );

    /** \brief Set clear depth for glClear calls
        \param depth Depth value used to fill the depth buffer when clearing the target
        \remarks The initial value for the depth is 1.0.
    **/
    NVSG_API void setClearDepth( GLclampd depth );

    /** \brief Set clear stencil value for glClear calls.
        \param stencil stencil value used to fill the stencil buffer when clearing the target.
        \remarks The initial value for the stencil is 0.
    **/
    NVSG_API void setClearStencil( GLint stencil );

    // RenderTarget interface
    NVSG_API virtual void setSize( unsigned int  width, unsigned int  height );
    NVSG_API virtual void getSize( unsigned int &width, unsigned int &height ) const;

    /** \brief Get width of the RenderTargetGL.
        \return width of the RenderTargetGL.
    **/
    NVSG_API unsigned int getWidth() { return m_width; }

    /** \brief Get height of the RenderTargetGL.
        \return width of the RenderTargetGL.
    **/
    NVSG_API unsigned int getHeight() { return m_height; }

    NVSG_API virtual void setPosition( int  x, int  y ); //!< Set the lower left point of the viewport
    NVSG_API virtual void getPosition( int &x, int &y ) const; //!< Get the lower left point of the viewport

    // "screenshot"
    /** \brief Grab a screenshot of the current color buffer.
        \param pixelFormat PixelFormat to use when grabbing the pixels.
        \param pixelDataType DataType to use for each pixel component.
        \return A TextureHostSharedPtr containing a texture with the content of the surface.
    */
    NVSG_API virtual nvsg::TextureHostSharedPtr getTextureHost( nvsg::Image::PixelFormat pixelFormat = nvsg::Image::IMG_BGR, nvsg::Image::PixelDataType = nvsg::Image::IMG_UNSIGNED_BYTE );

    /** \brief Grab a screenshot of the specified color buffer.
        \param mode GL color buffer target to grab (i.e. GL_FRONT_LEFT or GL_COLOR_ATTACHMENT0).
        \param pixelFormat PixelFormat to use when grabbing the pixels.
        \param pixelDataType DataType to use for each pixel component.
        \return A TextureHostSharedPtr containing a texture with the content of the surface.
    **/
    NVSG_API virtual nvsg::TextureHostSharedPtr getTargetAsTextureHost(GLenum mode, nvsg::Image::PixelFormat pixelFormat = nvsg::Image::IMG_BGR, nvsg::Image::PixelDataType = nvsg::Image::IMG_UNSIGNED_BYTE );

    NVSG_API virtual bool isValid();

    /** \brief Get the OpenGL context used by this RenderTarget
        \return RenderContextGL used by this RenderTarget
    **/
    NVSG_API SmartRenderContextGL getRenderContextGL();
  protected:
    NVSG_API RenderTargetGL( const SmartRenderContextGL &glContext );

    /** \brief Create a side-by-side stereo image out of two images
        \param textureImageHandleLeft Image for the left eye
        \param textureImageHandleRight Image for the right eye
        \return TextureHost with left eye on the right side and right eye on the left side
    **/
    NVSG_API nvsg::TextureHostSharedPtr createStereoTextureHost( const nvsg::TextureHostSharedPtr &extureImageHandleLeft, 
                                                                     const nvsg::TextureHostSharedPtr &textureImageHandleRight );

    /** \brief Check if this RenderTargetGL is current
        \return true if this RenderTargetGL is current, false otherwise.
    **/
    NVSG_API virtual bool isCurrent();

    // make context temporary current for exception safety
    class TmpCurrent
    {
    public:
      TmpCurrent(RenderTargetGL *target)
      {
        m_target = target;
        target->makeCurrent();
      }

      ~TmpCurrent()
      {
        m_target->makeNoncurrent();
      }
    private:
      RenderTargetGL *m_target;
    };

    friend class TmpCurrent;

    NVSG_API virtual void makeCurrent();
    NVSG_API virtual void makeNoncurrent();

    // clear interface
    GLbitfield m_clearMask;
    GLclampf   m_clearColorR, m_clearColorG, m_clearColorB, m_clearColorA;
    GLclampd   m_clearDepth;
    GLuint     m_clearStencil;

  private:
    bool                 m_current;
    SmartRenderContextGL m_renderContextGL;
    int                  m_x;
    int                  m_y;
    unsigned int         m_width;
    unsigned int         m_height;

    RenderContextGLStack m_contextStack;
  };

  typedef nvutil::SmartPtr<RenderTargetGL> SmartRenderTargetGL;

  inline SmartRenderContextGL RenderTargetGL::getRenderContextGL()
  {
    return m_renderContextGL;
  }

}  // namespace nvgl

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

#include <nvgl/GLContext.h>
#include <nvutil/SmartPtr.h>
#include <vector>

namespace nvgl
{
  /** \brief RenderbufferGL is a wrapper around OpenGL renderbuffers. It can be used as attachment
             by nvgl::RenderTargetGLFBO or as standalone object. If the buffer is used as attachment
             the size is managed by the RenderTarget.
  **/
  class RenderbufferGL : public nvutil::RCObject
  {
  public:
    struct MSAA {
      MSAA( GLsizei colorSamples ) 
        : m_colorSamples(colorSamples)
      {
      }

      unsigned int m_colorSamples;
    };

    struct CSAA {
      CSAA( GLsizei coverageSamples, GLsizei colorSamples ) 
        : m_colorSamples(colorSamples)
        , m_coverageSamples(coverageSamples)
      {
      }

      unsigned int m_colorSamples;
      unsigned int m_coverageSamples;
    };

  public:
    /** \brief Create a new RenderbufferGL.
        \param internalFormat OpenGL format of the newly created renderbuffer.
        \param width Width of the newly created Renderbuffer.
        \param height Height of the newly created Renderbuffer.
        \return RefCounted RenderbufferGL.
    **/
    NVSG_API static nvutil::SmartPtr<RenderbufferGL> create(GLenum internalFormat, int width = 1, int height = 1);

    /** \brief Create a new Renderbuffer with MSAA enabled.
        \param msaa MSAA settings of the newly created Renderbuffer.
        \param internalFormat OpenGL format of the newly created renderbuffer.
        \param width Width of the newly created Renderbuffer.
        \param height Height of the newly created Renderbuffer.
        \return RefCounted RenderbufferGL.
    **/
    NVSG_API static nvutil::SmartPtr<RenderbufferGL> create(const MSAA &msaa, GLenum internalFormat, int width = 1, int height = 1);

    /** \brief Create a new Renderbuffer with CSAA enabled.
        \param csaa CSAA settings of the newly created Renderbuffer.
        \param internalFormat OpenGL format of the newly created renderbuffer.
        \param width Width of the newly created Renderbuffer.
        \param height Height of the newly created Renderbuffer.
        \return RefCounted RenderbufferGL.
    **/
    NVSG_API static nvutil::SmartPtr<RenderbufferGL> create(const CSAA &csaa, GLenum internalFormat, int width = 1, int height = 1);

    NVSG_API virtual ~RenderbufferGL();

    /** \brief Change the size of the renderbuffer.
        \param width New width of the renderbuffer.
        \param height New height of the renderbuffer.
        \note It's not necessary to call this function if the renderbuffer is used as nvgl::RenderTargetGLFBO::Attachment.
              The size of attachments is managed by nvgl::RenderTargetGLFBO.
    **/
    NVSG_API void resize( int width, int height );

    /** \brief Get the OpenGL name of the renderbuffer.
        \return OpenGL name of the renderbuffer.
    **/
    NVSG_API GLuint getRenderbufferId() const;

    /** \brief Test if MSAA is available for the current active context.
        \return true if MSAA is supported, false otherwise.
    **/
    NVSG_API static bool isMSAAAvailable();

    /** \brief Test if CSAA is available for the current active context.
        \return true if CSAA is supported, false otherwise.
    **/
    NVSG_API static bool isCSAAAvailable();

    /** \brief Determine the maximum number of MSAA samples supported by the current active context.
        \return Maximum number of supported MSAA samples.
    **/
    NVSG_API static GLint getMaxMSAASamples();

    /** \brief Get a list of all supported CSAA modes supported by the current active context.
        \return vector of all supported CSAA modes
    **/
    NVSG_API static std::vector<CSAA> getAvailableCSAAModes();
  protected:
    void init( int width, int height );

  private:
    NVSG_API RenderbufferGL(GLenum internalFormat, int width = 1, int height = 1);
    NVSG_API RenderbufferGL(const MSAA &msaa, GLenum internalFormat, int width = 1, int height = 1);
    NVSG_API RenderbufferGL(const CSAA &csaa, GLenum internalFormat, int width = 1, int height = 1);

  private:
    void resizeNoAA( );
    void resizeMSAA( );
    void resizeCSAA( );

    GLuint  m_renderbuffer;
    GLsizei m_width;
    GLsizei m_height;
    GLenum  m_internalFormat;

    GLsizei m_colorSamples;     // will be 0 when msaa and csaa are not being used
    GLsizei m_coverageSamples;  // will be 0 for msaa and no multisampling
    void (RenderbufferGL::*m_resizeFunc)( );
  };

  inline GLuint RenderbufferGL::getRenderbufferId() const
  {
    return m_renderbuffer;
  }

  typedef nvutil::SmartPtr<RenderbufferGL> SmartRenderbufferGL;

} // namespace nvgl

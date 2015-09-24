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

#include <nvgl/RenderTargetGL.h>
#include <nvgl/RenderContextGL.h>

namespace nvgl
{

  /** \brief RenderTarget for the OpenGL framebuffer. If the attached OpenGL context has a stereo 
             pixelformat stereo will be enabled automatically.
  **/
  class RenderTargetGLFB : public nvgl::RenderTargetGL
  {
  public:
    // RenderTargetGL interface
    NVSG_API static nvutil::SmartPtr<RenderTargetGLFB> create( const SmartRenderContextGL &glContext );

    NVSG_API virtual ~RenderTargetGLFB();

    NVSG_API virtual bool isValid();

    /** \brief Grab a screenshot of the current framebuffer. If stereo is enabled a side-by-side stereo image
               will be created.
        \return A TextureHost with the content of the color buffer.
    **/
    NVSG_API virtual nvsg::TextureHostSharedPtr getTextureHost();

    // stereo api
    NVSG_API virtual bool isStereoEnabled() const;
    NVSG_API virtual bool setStereoTarget( StereoTarget target );
    NVSG_API virtual StereoTarget getStereoTarget() const;

  protected:
    // make context current
    NVSG_API virtual void makeCurrent();
    // make context uncurrent
    NVSG_API virtual void makeNoncurrent();

    GLenum getStereoTargetBuffer( StereoTarget stereoTarget, bool backbuffer );
    void setDrawBuffer( StereoTarget stereoTarget );

    NVSG_API RenderTargetGLFB( const SmartRenderContextGL &glContext );

    StereoTarget m_stereoTarget;    //!< The currently active stereo target
    GLint        m_oldDrawBuffer;   //!< Keeps the drawbuffer which had been current before making this target current
    bool         m_stereoEnabled; //!< True is stereo is supported on the OpenGL context
  };

  typedef nvutil::SmartPtr<RenderTargetGLFB> SmartRenderTargetGLFB;

}  // namespace nvgl

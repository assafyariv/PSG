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

#include <nvutil/SmartPtr.h>
#include <nvsg/CoreTypes.h>
#include <nvsg/TextureHost.h>

namespace nvui {

  /** \brief An nvui::RenderTarget specifies a render surface to be used in conjunction with an nvui::Renderer.
             SceniX supports RenderTargets for OptiX (nvrt::RenderTargetRT, nvrt::RenderTargetRTBuiltin), for
             OpenGL (nvgl::RenderTargetGLFB, nvgl::RenderTargetGLFBO) .

             After a Renderer's render command has been executed, the rendered image can be queried from the 
             RenderTarget as an nvsg::TextureHost by calling RenderTarget::getTextureHost.

             A RenderTarget is also prepared to support stereoscopic rendering in conjunction with nvui::SceneRenderer.
      \remarks For mono rendering the LEFT surface is used as default. 
  **/
  class RenderTarget : public nvutil::Reflection
                     , public nvutil::RCObject
  {
  public:
    enum StereoTarget { LEFT = 0,         //!< Left eye only. Mono is aliased to the left eye.
                        RIGHT,            //!< Right eye only
                        LEFT_AND_RIGHT    //!< Left and right eye at the same time, may not work on all targets
                      };

    NVSG_API virtual ~RenderTarget() {};

    /** \brief This function will be called by renderers when they start to render a frame.
        \return true If RenderTarget is ready for rendering, false otherwise.
    **/
    NVSG_API virtual bool beginRendering()  { return( true ); }

    /** \brief This function will be called by renderers when they finished a frame. **/
    NVSG_API virtual void endRendering()    {}

    /** \brief Update the dimensions of the RenderTarget
        \param width New width of the RenderTarget
        \param height New height of the RenderTarget
    **/
    NVSG_API virtual void setSize( unsigned int width, unsigned int height ) = 0;

    /** \brief Retrieve the dimensions of the RenderTarget.
        \param width Returns current width of the RenderTarget.
        \param height Returns current height of the RenderTarget.
    **/
    NVSG_API virtual void getSize( unsigned int &width, unsigned int &height ) const = 0;

    /** \brief Get the current width of the RenderTarget
        \return current width of the RenderTarget
    **/
    NVSG_API unsigned int getWidth() const;

    /** \brief Get the current heightof the RenderTarget
        \return current heightof the RenderTarget
    **/
    NVSG_API unsigned int getHeight() const;

    /** \brief Get the current aspect ratio of the RenderTarget. The default implementation
               returns width/height. If a derived class supports a RenderTarget with non-square
               pixels this function should be overriden.
        \return Aspect ratio of the surface
    **/
    NVSG_API virtual float getAspectRatio() const;

    /** \brief Fetch pixels of the surface in a TextureHost.
        \param pixelFormat Pixel format to use when grabbing the pixels.
        \param pixelDataType Data type to use for each pixel component.
        \return A TextureHostSharedPtr containing a texture with the content of the surface.
        \remarks If a RenderTarget cannot support this operation it returns a null object.
    **/
    NVSG_API virtual nvsg::TextureHostSharedPtr getTextureHost( 
          nvsg::Image::PixelFormat pixelFormat = nvsg::Image::IMG_BGR, 
          nvsg::Image::PixelDataType pixelDataType = nvsg::Image::IMG_UNSIGNED_BYTE ) = 0;

    /** \brief Check if the RenderTarget is valid and can be used for rendering.
        \return true If the RenderTarget is ready for rendering, false otherwise.
    **/
    NVSG_API virtual bool isValid() = 0;

    /** \brief Check if stereo is enabled.
        \return true If the RenderTarget has stereo enabled, false otherwise.
    **/
    NVSG_API virtual bool isStereoEnabled() const { return false; }

    /** \brief Choose stereo surface to render on.
        \param target LEFT, RIGHT or LEFT_AND_RIGHT for the corresponding surface.
        \return true If the surface had been selected and false otherwise.
    **/
    NVSG_API virtual bool setStereoTarget( StereoTarget target ) { return target == LEFT; }

    /** \brief Retrieve which stereo surface is currently active.
        \return Currently active surface for stereo rendering.
    **/
    NVSG_API virtual StereoTarget getStereoTarget() const { return LEFT;}
  };

  inline unsigned int RenderTarget::getWidth() const
  {
    unsigned int width, height;
    getSize(width, height);
    return width;
  }

  inline unsigned int RenderTarget::getHeight() const
  {
    unsigned int width, height;
    getSize(width, height);
    return height;
  }

  inline float RenderTarget::getAspectRatio() const
  {
    unsigned int width, height;
    getSize(width, height);
    float ratio = 1.0;
    if ( width && height )
    {
      ratio = float(width) / float(height);
    }
    return ratio;
  }

  typedef nvutil::SmartPtr<RenderTarget> SmartRenderTarget;
} // namespace nvui

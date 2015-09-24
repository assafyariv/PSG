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
#include <set>

namespace nvgl
{

  class RenderContextGLFormat
  {
  public:
    NVSG_API RenderContextGLFormat();
    NVSG_API virtual ~RenderContextGLFormat();

    NVSG_API void setStereo(bool stereo);
    NVSG_API bool isStereo() const;

    NVSG_API void setThirtyBit(bool thirtyBit);
    NVSG_API bool isThirtyBit() const;

    NVSG_API void setSRGB(bool sRGB);
    NVSG_API bool isSRGB() const;

    NVSG_API void setMultisample( unsigned int samples ); //<! 0 -> do not request multisampling fb
    NVSG_API unsigned int getMultisample( ) const;

    // requires NV_multisample_coverage extension
    NVSG_API void setMultisampleCoverage( unsigned int colorSamples, unsigned int coverageSamples );
    NVSG_API void getMultisampleCoverage( unsigned int & colorSamples, unsigned int & coverageSamples ) const;

    NVSG_API void setStencil( bool stencil );
    NVSG_API bool isStencil() const;

    struct FormatInfo
    {
      FormatInfo()
        : stereoSupported( false )
        , stencilSupported( false )
        , thirtyBitSupported( false )
        , sRGBSupported( false )
        , msSupported( false )
        , msCoverageSupported( false )
      {}

      bool stereoSupported;
      bool stencilSupported;
      bool thirtyBitSupported;
      bool sRGBSupported;
      bool msSupported;
      bool msCoverageSupported;
      std::set< std::pair < unsigned int, unsigned int > > multisampleModesSupported;
    };

    //
    // NOTE: both of these require a current context!
    //
    NVSG_API bool getFormatInfo( FormatInfo & info ) const;
    NVSG_API bool isAvailable() const;

    NVSG_API bool operator==( const RenderContextGLFormat &rhs ) const;
    NVSG_API bool operator!=( const RenderContextGLFormat &rhs ) const;

#if defined(WIN32)
    NVSG_API int getPixelFormat() const;
    NVSG_API void syncFormat( HDC hdc );
#elif defined(LINUX)
    NVSG_API GLXFBConfig getGLXFBConfig( Display *display, int screen ) const;
    NVSG_API static GLXFBConfig getGLXFBConfig( Display *display, GLXContext context );
    NVSG_API static int getGLXFBConfigAttrib( Display *display, GLXFBConfig config, int attribute );
    NVSG_API void syncFormat( Display *display, GLXContext context );
#endif

  protected:
    bool m_sRGB;
    bool m_stereo;
    bool m_thirtyBit;
    unsigned m_msColorSamples;
    unsigned m_msCoverageSamples;
    bool m_stencil;
  };

  inline void RenderContextGLFormat::setStereo(bool stereo)
  {
    m_stereo = stereo;
  }

  inline bool RenderContextGLFormat::isStereo() const
  {
    return m_stereo;
  }

  inline void RenderContextGLFormat::setThirtyBit(bool thirtyBit)
  {
    m_thirtyBit = thirtyBit;
  }

  inline bool RenderContextGLFormat::isThirtyBit() const
  {
    return m_thirtyBit;
  }

  inline void RenderContextGLFormat::setSRGB(bool sRGB)
  {
    m_sRGB = sRGB;
  }

  inline bool RenderContextGLFormat::isSRGB() const
  {
    return m_sRGB;
  }

  inline void RenderContextGLFormat::setMultisample( unsigned int multisample )
  {
    m_msColorSamples    = multisample;
    m_msCoverageSamples = multisample;
  }

  inline unsigned int RenderContextGLFormat::getMultisample( ) const
  {
    return m_msColorSamples;
  }

  inline void RenderContextGLFormat::setMultisampleCoverage( unsigned int color, unsigned int coverage )
  {
    m_msColorSamples = color;

    // can't have less coverage samples than color samples
    m_msCoverageSamples = std::max( color, coverage );
  }

  inline void RenderContextGLFormat::getMultisampleCoverage( unsigned int & color, unsigned int & coverage ) const
  {
    color    = m_msColorSamples;
    coverage = m_msCoverageSamples;
  }

  inline void RenderContextGLFormat::setStencil( bool stencil )
  {
    m_stencil = stencil;
  }

  inline bool RenderContextGLFormat::isStencil() const
  {
    return m_stencil;
  }

} // namespace nvgl

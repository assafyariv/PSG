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

#include "nvrt/RenderTargetRT.h"
#include "nvrt/RTBuffer.h"

namespace nvrt
{
  class RenderTargetRTBuiltin : public RenderTargetRT
  {
  public:

    enum OutputBufferStorage
    {
      OBS_HOST,
      OBS_GL_BO,
      OBS_GL_TEXTURE
    };

    NVRT_API static nvutil::SmartPtr<RenderTargetRTBuiltin> create( const nvrt::RTContextSharedPtr &rtContext );

    NVRT_API virtual ~RenderTargetRTBuiltin();

    NVRT_API virtual bool beginRendering();
    NVRT_API virtual void endRendering();

    NVRT_API virtual void setColorFormat( ColorFormat colorFormat );

    NVRT_API void setBufferType( RTbuffertype bufferType);
    NVRT_API RTbuffertype getBufferType( ) const;

    NVRT_API void setOutputBufferStorage( OutputBufferStorage outputBufferStorage );
    NVRT_API OutputBufferStorage getOutputBufferStorage( ) const;

    NVRT_API virtual void setStereoEnabled(bool stereoEnabled);
  protected:
    void updateOutputBuffer(); //! update the current outputbuffer to match the given parameters
    virtual RTBufferSharedPtr createOutputBuffer();

    NVRT_API RenderTargetRTBuiltin( const nvrt::RTContextSharedPtr &rtContext );

  private:
    RTbuffertype        m_bufferType;
    OutputBufferStorage m_outputBufferStorage;
  };

  typedef nvutil::SmartPtr<RenderTargetRTBuiltin> SmartRenderTargetRTBuiltin;

} // namespace nvrt

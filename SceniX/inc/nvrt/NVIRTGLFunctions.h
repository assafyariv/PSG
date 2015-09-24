// AUTOMATICALLY GENERATED - DO NOT EDIT

#pragma once

#include <nvrt/nvsgrtapi.h>
#include <nvrt/nvirt/optix.h>
#include <nvrt/nvirt/optixu/optixu.h>

#include <nvsgcommon.h>
#include <nvgl/GLContext.h>

#include <string>

struct NVIRTGLFunctions
{
  RTresult (* RTBufferCreateFromGLBO)( RTcontext , unsigned int , unsigned int , RTbuffer* );
  RTresult (* RTTextureSamplerCreateFromGLImage)( RTcontext , unsigned int , RTgltarget , RTtexturesampler* );
  RTresult (* RTBufferGetGLBOId)( RTbuffer , unsigned int* );
  RTresult (* RTTextureSamplerGetGLImageId)( RTtexturesampler , unsigned int* );
  RTresult (* RTBufferGLRegister)( RTbuffer );
  RTresult (* RTBufferGLUnregister)( RTbuffer );
  RTresult (* RTTextureSamplerGLRegister)( RTtexturesampler );
  RTresult (* RTTextureSamplerGLUnregister)( RTtexturesampler );
#ifdef _WIN32
  RTresult (* RTDeviceGetWGLDevice)( int* , HGPUNV );
#endif
};

extern struct NVIRTGLFunctions _NVIRTGLFunctions;

bool InitNVIRTGLFunctions( void * (*DLL_FUNC)( void *, const std::string & ), void * DLL_ARG );
RTresult NVRT_API RTBufferCreateFromGLBO(  RTcontext context, unsigned int bufferdesc, unsigned int glId,  RTbuffer* buffer );
RTresult NVRT_API RTTextureSamplerCreateFromGLImage(  RTcontext context, unsigned int glId, RTgltarget target, RTtexturesampler* textureSampler );
RTresult NVRT_API RTBufferGetGLBOId(  RTbuffer buffer, unsigned int* glId );
RTresult NVRT_API RTTextureSamplerGetGLImageId(  RTtexturesampler textureSampler, unsigned int* glId );
RTresult NVRT_API RTBufferGLRegister(  RTbuffer buffer );
RTresult NVRT_API RTBufferGLUnregister(  RTbuffer buffer );
RTresult NVRT_API RTTextureSamplerGLRegister(  RTtexturesampler textureSampler );
RTresult NVRT_API RTTextureSamplerGLUnregister(  RTtexturesampler textureSampler );
#ifdef _WIN32
RTresult NVRT_API RTDeviceGetWGLDevice( int* device, HGPUNV gpu);
#endif


inline RTresult RTBufferCreateFromGLBO(  RTcontext context, unsigned int bufferdesc, unsigned int glId,  RTbuffer* buffer  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTBufferCreateFromGLBO );
  return _NVIRTGLFunctions.RTBufferCreateFromGLBO( context, bufferdesc, glId, buffer );
}

inline RTresult RTTextureSamplerCreateFromGLImage(  RTcontext context, unsigned int glId, RTgltarget target, RTtexturesampler* textureSampler  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTTextureSamplerCreateFromGLImage );
  return _NVIRTGLFunctions.RTTextureSamplerCreateFromGLImage( context, glId, target, textureSampler );
}

inline RTresult RTBufferGetGLBOId(  RTbuffer buffer, unsigned int* glId  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTBufferGetGLBOId );
  return _NVIRTGLFunctions.RTBufferGetGLBOId( buffer, glId );
}

inline RTresult RTTextureSamplerGetGLImageId(  RTtexturesampler textureSampler, unsigned int* glId  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTTextureSamplerGetGLImageId );
  return _NVIRTGLFunctions.RTTextureSamplerGetGLImageId( textureSampler, glId );
}

inline RTresult RTBufferGLRegister(  RTbuffer buffer  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTBufferGLRegister );
  return _NVIRTGLFunctions.RTBufferGLRegister( buffer );
}

inline RTresult RTBufferGLUnregister(  RTbuffer buffer  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTBufferGLUnregister );
  return _NVIRTGLFunctions.RTBufferGLUnregister( buffer );
}

inline RTresult RTTextureSamplerGLRegister(  RTtexturesampler textureSampler  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTTextureSamplerGLRegister );
  return _NVIRTGLFunctions.RTTextureSamplerGLRegister( textureSampler );
}

inline RTresult RTTextureSamplerGLUnregister(  RTtexturesampler textureSampler  )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTTextureSamplerGLUnregister );
  return _NVIRTGLFunctions.RTTextureSamplerGLUnregister( textureSampler );
}

#ifdef _WIN32
inline RTresult RTDeviceGetWGLDevice( int* device, HGPUNV gpu )
{
  NVSG_ASSERT( _NVIRTGLFunctions.RTDeviceGetWGLDevice );
  return _NVIRTGLFunctions.RTDeviceGetWGLDevice( device, gpu );
}
#endif


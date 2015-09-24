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

#include <nvrt/nvsgrtapi.h>
#include <nvutil/BitMask.h>

namespace nvrt
{
  //* RTInit status codes
  enum RTInitStatus
  {
    RT_INITIALIZED = 0,                   //!< No error occured, init succeeded
    RT_NOT_INITIALIZED,                   //!< RT not initialized yet
    RT_ERROR_DLL_OPEN_FAILED,             //!< Opening one of the needed OptiX DLLs failed (Windows: optix.1.dll, optixu.1.dll Linux: liboptix.1.so, liboptixu.1.so)
    RT_ERROR_INCOMPATIBLE_OPTIX_VERSION,  //!< One of the OptiX DLLs is incompatible (Windows: optix.1.dll, optixu.1.dll Linux: liboptix.1.so, liboptixu.1.so)
    RT_ERROR_GET_DEVICE_COUNT_FAILED,     //!< An error occured during enumerating RT capable devices
    RT_ERROR_NO_RT_DEVICE_FOUND           //!< No RT capable device found
  };

  //* Initialize the RT libraries and functions - used internally
  NVRT_API RTInitStatus RTInit( void );
  //* Shutdown the RT libraries and functions - used internally
  NVRT_API void RTShutdown( void );
  //* Check to see if Ray Tracing is supported in SceniX - internal and app use too.
  NVRT_API bool RTIsSupported( void );
  //* Check RT initialization status
  NVRT_API RTInitStatus RTGetInitStatus( void );
  //* Get a string corresponding to a given status
  NVRT_API const char * RTGetInitStatusString( RTInitStatus status );
}


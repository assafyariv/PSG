// Copyright NVIDIA Corporation 2002-2010
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

#include <nvutil/Handle.h>
#include <nvutil/SmartPtr.h>

#ifdef _WIN32
// microsoft specific storage-class defines
# ifdef NVRT_EXPORTS
#  define NVRT_API __declspec(dllexport)
# else
#  define NVRT_API __declspec(dllimport)
# endif
#else
# define NVRT_API
#endif

namespace nvrt
{
  class RTAccelerationCache;
    class RTMultiFileCache;
  class RTBase;
    class RTAcceleration;
    class RTBuffer;
      class RTBufferGL;
        class RTBufferGLTexture;
    class RTContext;
    class RTGeometry;
    class RTGeometryInstance;
    class RTMaterial;
    class RTNode;
      class RTGeometryGroup;
      class RTGroup;
      class RTSelector;
      class RTTransform;
    class RTProgram;
    class RTTextureSampler;
  class RTVariable;
    template<unsigned char N, typename T> class RTVariableNT;
    template<unsigned char N, unsigned char M> class RTVariableNxMf;
    template<typename Type> class RTVariableType;
  class RTVariableSet;

  CORE_TYPES( RTAccelerationCache, nvutil::HandledObject );
  CORE_TYPES( RTMultiFileCache, RTAccelerationCache );
  CORE_TYPES( RTBase, nvutil::HandledObject );
  CORE_TYPES( RTAcceleration, RTBase );
  CORE_TYPES( RTBuffer, RTBase );
  CORE_TYPES( RTBufferGL, RTBuffer );
  CORE_TYPES( RTBufferGLTexture, RTBufferGL );
  CORE_TYPES( RTContext, RTBase );
  CORE_TYPES( RTGeometry, RTBase );
  CORE_TYPES( RTGeometryInstance, RTBase );
  CORE_TYPES( RTMaterial, RTBase );
  CORE_TYPES( RTNode, RTBase );
  CORE_TYPES( RTGeometryGroup, RTNode );
  CORE_TYPES( RTGroup, RTNode );
  CORE_TYPES( RTSelector, RTNode );
  CORE_TYPES( RTTransform, RTNode );
  CORE_TYPES( RTProgram, RTBase );
  CORE_TYPES( RTTextureSampler, RTBase );
}

namespace nvutil
{
  OBJECT_TRAITS( nvrt::RTAccelerationCache, nvutil::HandledObject );
  OBJECT_TRAITS( nvrt::RTMultiFileCache, nvrt::RTAccelerationCache );
  OBJECT_TRAITS( nvrt::RTBase, nvutil::HandledObject );
  OBJECT_TRAITS( nvrt::RTAcceleration, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTBuffer, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTBufferGL, nvrt::RTBuffer );
  OBJECT_TRAITS( nvrt::RTBufferGLTexture, nvrt::RTBufferGL );
  OBJECT_TRAITS( nvrt::RTContext, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTGeometry, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTGeometryInstance, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTMaterial, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTNode, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTGeometryGroup, nvrt::RTNode );
  OBJECT_TRAITS( nvrt::RTGroup, nvrt::RTNode );
  OBJECT_TRAITS( nvrt::RTSelector, nvrt::RTNode );
  OBJECT_TRAITS( nvrt::RTTransform, nvrt::RTNode );
  OBJECT_TRAITS( nvrt::RTProgram, nvrt::RTBase );
  OBJECT_TRAITS( nvrt::RTTextureSampler, nvrt::RTBase );
}

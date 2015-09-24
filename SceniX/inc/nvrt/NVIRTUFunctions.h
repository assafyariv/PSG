// AUTOMATICALLY GENERATED - DO NOT EDIT

#pragma once

#include <nvrt/nvsgrtapi.h>
#include <nvrt/nvirt/optix.h>
#include <nvrt/nvirt/optixu/optixu.h>

#include <nvsgcommon.h>
#include <nvgl/GLContext.h>

#include <string>

struct NVIRTUFunctions
{
  RTresult (* RTUNameForType)( RTobjecttype , char* , RTsize );
  RTresult (* RTUGetSizeForRTformat)( RTformat , size_t* );
  RTresult (* RTUCUDACompileString)( const char* , const char** , unsigned int , RTsize* , RTsize* );
  RTresult (* RTUCUDACompileFile)( const char* , const char** , unsigned int , RTsize* , RTsize* );
  RTresult (* RTUCUDAGetCompileResult)( char* , char* );
  RTresult (* RTUCreateClusteredMesh)( RTcontext , unsigned int , RTgeometry* , unsigned int , const float* , unsigned int , const unsigned* , const unsigned* );
  RTresult (* RTUCreateClusteredMeshExt)( RTcontext , unsigned int , RTgeometry* , unsigned int , const float* , unsigned int , const unsigned* , const unsigned* , RTbuffer , const unsigned* , RTbuffer , const unsigned* );
};

extern struct NVIRTUFunctions _NVIRTUFunctions;

bool InitNVIRTUFunctions( void * (*DLL_FUNC)( void *, const std::string & ), void * DLL_ARG );
RTresult NVRT_API RTUNameForType(  RTobjecttype type, char* buffer, RTsize bufferSize );
RTresult NVRT_API RTUGetSizeForRTformat(  RTformat format, size_t* size);
RTresult NVRT_API RTUCUDACompileString(  const char* source, const char** preprocessorArguments, unsigned int numPreprocessorArguments, RTsize* resultSize, RTsize* errorSize );
RTresult NVRT_API RTUCUDACompileFile(  const char* filename, const char** preprocessorArguments, unsigned int numPreprocessorArguments, RTsize* resultSize, RTsize* errorSize );
RTresult NVRT_API RTUCUDAGetCompileResult(  char* result, char* error );
RTresult NVRT_API RTUCreateClusteredMesh(  RTcontext context, unsigned int usePTX32InHost64, RTgeometry* mesh, unsigned int num_verts, const float* verts, unsigned int num_tris, const unsigned* indices, const unsigned* mat_indices );
RTresult NVRT_API RTUCreateClusteredMeshExt(  RTcontext context, unsigned int usePTX32InHost64, RTgeometry* mesh, unsigned int num_verts, const float* verts, unsigned int num_tris, const unsigned* indices, const unsigned* mat_indices, RTbuffer norms, const unsigned* norm_indices, RTbuffer tex_coords, const unsigned* tex_indices );


inline RTresult RTUNameForType(  RTobjecttype type, char* buffer, RTsize bufferSize  )
{
  NVSG_ASSERT( _NVIRTUFunctions.RTUNameForType );
  return _NVIRTUFunctions.RTUNameForType( type, buffer, bufferSize );
}

inline RTresult RTUGetSizeForRTformat(  RTformat format, size_t* size )
{
  NVSG_ASSERT( _NVIRTUFunctions.RTUGetSizeForRTformat );
  return _NVIRTUFunctions.RTUGetSizeForRTformat( format, size );
}

inline RTresult RTUCUDACompileString(  const char* source, const char** preprocessorArguments, unsigned int numPreprocessorArguments, RTsize* resultSize, RTsize* errorSize  )
{
  NVSG_ASSERT( _NVIRTUFunctions.RTUCUDACompileString );
  return _NVIRTUFunctions.RTUCUDACompileString( source, preprocessorArguments, numPreprocessorArguments, resultSize, errorSize );
}

inline RTresult RTUCUDACompileFile(  const char* filename, const char** preprocessorArguments, unsigned int numPreprocessorArguments, RTsize* resultSize, RTsize* errorSize  )
{
  NVSG_ASSERT( _NVIRTUFunctions.RTUCUDACompileFile );
  return _NVIRTUFunctions.RTUCUDACompileFile( filename, preprocessorArguments, numPreprocessorArguments, resultSize, errorSize );
}

inline RTresult RTUCUDAGetCompileResult(  char* result, char* error  )
{
  NVSG_ASSERT( _NVIRTUFunctions.RTUCUDAGetCompileResult );
  return _NVIRTUFunctions.RTUCUDAGetCompileResult( result, error );
}

inline RTresult RTUCreateClusteredMesh(  RTcontext context, unsigned int usePTX32InHost64, RTgeometry* mesh, unsigned int num_verts, const float* verts, unsigned int num_tris, const unsigned* indices, const unsigned* mat_indices  )
{
  NVSG_ASSERT( _NVIRTUFunctions.RTUCreateClusteredMesh );
  return _NVIRTUFunctions.RTUCreateClusteredMesh( context, usePTX32InHost64, mesh, num_verts, verts, num_tris, indices, mat_indices );
}

inline RTresult RTUCreateClusteredMeshExt(  RTcontext context, unsigned int usePTX32InHost64, RTgeometry* mesh, unsigned int num_verts, const float* verts, unsigned int num_tris, const unsigned* indices, const unsigned* mat_indices, RTbuffer norms, const unsigned* norm_indices, RTbuffer tex_coords, const unsigned* tex_indices  )
{
  NVSG_ASSERT( _NVIRTUFunctions.RTUCreateClusteredMeshExt );
  return _NVIRTUFunctions.RTUCreateClusteredMeshExt( context, usePTX32InHost64, mesh, num_verts, verts, num_tris, indices, mat_indices, norms, norm_indices, tex_coords, tex_indices );
}


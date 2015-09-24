// AUTOMATICALLY GENERATED - DO NOT EDIT

#pragma once

#include <nvrt/nvsgrtapi.h>
#include <nvrt/nvirt/optix.h>
#include <nvrt/nvirt/optixu/optixu.h>

#include <nvsgcommon.h>
#include <nvgl/GLContext.h>

#include <string>

struct NVIRTFunctions
{
  RTresult (* RTGetVersion)( unsigned int* );
  RTresult (* RTDeviceGetDeviceCount)( unsigned int* );
  RTresult (* RTDeviceGetAttribute)( int , RTdeviceattribute , RTsize , void* );
  RTresult (* RTVariableSet1f)( RTvariable , float );
  RTresult (* RTVariableSet2f)( RTvariable , float , float );
  RTresult (* RTVariableSet3f)( RTvariable , float , float , float );
  RTresult (* RTVariableSet4f)( RTvariable , float , float , float , float );
  RTresult (* RTVariableSet1fv)( RTvariable , const float* );
  RTresult (* RTVariableSet2fv)( RTvariable , const float* );
  RTresult (* RTVariableSet3fv)( RTvariable , const float* );
  RTresult (* RTVariableSet4fv)( RTvariable , const float* );
  RTresult (* RTVariableSet1i)( RTvariable , int );
  RTresult (* RTVariableSet2i)( RTvariable , int , int );
  RTresult (* RTVariableSet3i)( RTvariable , int , int , int );
  RTresult (* RTVariableSet4i)( RTvariable , int , int , int , int );
  RTresult (* RTVariableSet1iv)( RTvariable , const int* );
  RTresult (* RTVariableSet2iv)( RTvariable , const int* );
  RTresult (* RTVariableSet3iv)( RTvariable , const int* );
  RTresult (* RTVariableSet4iv)( RTvariable , const int* );
  RTresult (* RTVariableSet1ui)( RTvariable , unsigned int );
  RTresult (* RTVariableSet2ui)( RTvariable , unsigned int , unsigned int );
  RTresult (* RTVariableSet3ui)( RTvariable , unsigned int , unsigned int , unsigned int );
  RTresult (* RTVariableSet4ui)( RTvariable , unsigned int , unsigned int , unsigned int , unsigned int );
  RTresult (* RTVariableSet1uiv)( RTvariable , const unsigned int* );
  RTresult (* RTVariableSet2uiv)( RTvariable , const unsigned int* );
  RTresult (* RTVariableSet3uiv)( RTvariable , const unsigned int* );
  RTresult (* RTVariableSet4uiv)( RTvariable , const unsigned int* );
  RTresult (* RTVariableSetMatrix2x2fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix2x3fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix2x4fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix3x2fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix3x3fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix3x4fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix4x2fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix4x3fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetMatrix4x4fv)( RTvariable , int , const float* );
  RTresult (* RTVariableSetObject)( RTvariable , RTobject );
  RTresult (* RTVariableSetUserData)( RTvariable , RTsize , const void* );
  RTresult (* RTVariableGet1f)( RTvariable , float* );
  RTresult (* RTVariableGet2f)( RTvariable , float* , float* );
  RTresult (* RTVariableGet3f)( RTvariable , float* , float* , float* );
  RTresult (* RTVariableGet4f)( RTvariable , float* , float* , float* , float* );
  RTresult (* RTVariableGet1fv)( RTvariable , float* );
  RTresult (* RTVariableGet2fv)( RTvariable , float* );
  RTresult (* RTVariableGet3fv)( RTvariable , float* );
  RTresult (* RTVariableGet4fv)( RTvariable , float* );
  RTresult (* RTVariableGet1i)( RTvariable , int* );
  RTresult (* RTVariableGet2i)( RTvariable , int* , int* );
  RTresult (* RTVariableGet3i)( RTvariable , int* , int* , int* );
  RTresult (* RTVariableGet4i)( RTvariable , int* , int* , int* , int* );
  RTresult (* RTVariableGet1iv)( RTvariable , int* );
  RTresult (* RTVariableGet2iv)( RTvariable , int* );
  RTresult (* RTVariableGet3iv)( RTvariable , int* );
  RTresult (* RTVariableGet4iv)( RTvariable , int* );
  RTresult (* RTVariableGet1ui)( RTvariable , unsigned int* );
  RTresult (* RTVariableGet2ui)( RTvariable , unsigned int* , unsigned int* );
  RTresult (* RTVariableGet3ui)( RTvariable , unsigned int* , unsigned int* , unsigned int* );
  RTresult (* RTVariableGet4ui)( RTvariable , unsigned int* , unsigned int* , unsigned int* , unsigned int* );
  RTresult (* RTVariableGet1uiv)( RTvariable , unsigned int* );
  RTresult (* RTVariableGet2uiv)( RTvariable , unsigned int* );
  RTresult (* RTVariableGet3uiv)( RTvariable , unsigned int* );
  RTresult (* RTVariableGet4uiv)( RTvariable , unsigned int* );
  RTresult (* RTVariableGetMatrix2x2fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix2x3fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix2x4fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix3x2fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix3x3fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix3x4fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix4x2fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix4x3fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetMatrix4x4fv)( RTvariable , int , float* );
  RTresult (* RTVariableGetObject)( RTvariable , RTobject* );
  RTresult (* RTVariableGetUserData)( RTvariable , RTsize , void* );
  RTresult (* RTVariableGetName)( RTvariable , const char** );
  RTresult (* RTVariableGetAnnotation)( RTvariable , const char** );
  RTresult (* RTVariableGetType)( RTvariable , RTobjecttype* );
  RTresult (* RTVariableGetContext)( RTvariable , RTcontext* );
  RTresult (* RTVariableGetSize)( RTvariable , RTsize* );
  RTresult (* RTContextCreate)( RTcontext* );
  RTresult (* RTContextDestroy)( RTcontext );
  RTresult (* RTContextValidate)( RTcontext );
  void (* RTContextGetErrorString)( RTcontext , RTresult , const char** );
  RTresult (* RTContextSetAttribute)( RTcontext , RTcontextattribute , RTsize , void* );
  RTresult (* RTContextGetAttribute)( RTcontext , RTcontextattribute , RTsize , void* );
  RTresult (* RTContextSetDevices)( RTcontext , unsigned int , const int* );
  RTresult (* RTContextGetDevices)( RTcontext , int* );
  RTresult (* RTContextGetDeviceCount)( RTcontext , unsigned int* );
  RTresult (* RTContextSetStackSize)( RTcontext , RTsize );
  RTresult (* RTContextGetStackSize)( RTcontext , RTsize* );
  RTresult (* RTContextSetTimeoutCallback)( RTcontext , RTtimeoutcallback , double );
  RTresult (* RTContextSetEntryPointCount)( RTcontext , unsigned int );
  RTresult (* RTContextGetEntryPointCount)( RTcontext , unsigned int* );
  RTresult (* RTContextSetRayGenerationProgram)( RTcontext , unsigned int , RTprogram );
  RTresult (* RTContextGetRayGenerationProgram)( RTcontext , unsigned int , RTprogram* );
  RTresult (* RTContextSetExceptionProgram)( RTcontext , unsigned int , RTprogram );
  RTresult (* RTContextGetExceptionProgram)( RTcontext , unsigned int , RTprogram* );
  RTresult (* RTContextSetExceptionEnabled)( RTcontext , RTexception , int );
  RTresult (* RTContextGetExceptionEnabled)( RTcontext , RTexception , int* );
  RTresult (* RTContextSetRayTypeCount)( RTcontext , unsigned int );
  RTresult (* RTContextGetRayTypeCount)( RTcontext , unsigned int* );
  RTresult (* RTContextSetMissProgram)( RTcontext , unsigned int , RTprogram );
  RTresult (* RTContextGetMissProgram)( RTcontext , unsigned int , RTprogram* );
  RTresult (* RTContextCompile)( RTcontext );
  RTresult (* RTContextLaunch1D)( RTcontext , unsigned int , RTsize );
  RTresult (* RTContextLaunch2D)( RTcontext , unsigned int , RTsize , RTsize );
  RTresult (* RTContextLaunch3D)( RTcontext , unsigned int , RTsize , RTsize , RTsize );
  RTresult (* RTContextGetRunningState)( RTcontext , int* );
  RTresult (* RTContextSetPrintEnabled)( RTcontext , int );
  RTresult (* RTContextGetPrintEnabled)( RTcontext , int* );
  RTresult (* RTContextSetPrintBufferSize)( RTcontext , RTsize );
  RTresult (* RTContextGetPrintBufferSize)( RTcontext , RTsize* );
  RTresult (* RTContextSetPrintLaunchIndex)( RTcontext , int , int , int );
  RTresult (* RTContextGetPrintLaunchIndex)( RTcontext , int* , int* , int* );
  RTresult (* RTContextDeclareVariable)( RTcontext , const char* , RTvariable* );
  RTresult (* RTContextQueryVariable)( RTcontext , const char* , RTvariable* );
  RTresult (* RTContextRemoveVariable)( RTcontext , RTvariable );
  RTresult (* RTContextGetVariableCount)( RTcontext , unsigned int* );
  RTresult (* RTContextGetVariable)( RTcontext , unsigned int , RTvariable* );
  RTresult (* RTProgramCreateFromPTXString)( RTcontext , const char* , const char* , RTprogram* );
  RTresult (* RTProgramCreateFromPTXFile)( RTcontext , const char* , const char* , RTprogram* );
  RTresult (* RTProgramDestroy)( RTprogram );
  RTresult (* RTProgramValidate)( RTprogram );
  RTresult (* RTProgramGetContext)( RTprogram , RTcontext* );
  RTresult (* RTProgramDeclareVariable)( RTprogram , const char* , RTvariable* );
  RTresult (* RTProgramQueryVariable)( RTprogram , const char* , RTvariable* );
  RTresult (* RTProgramRemoveVariable)( RTprogram , RTvariable );
  RTresult (* RTProgramGetVariableCount)( RTprogram , unsigned int* );
  RTresult (* RTProgramGetVariable)( RTprogram , unsigned int , RTvariable* );
  RTresult (* RTGroupCreate)( RTcontext , RTgroup* );
  RTresult (* RTGroupDestroy)( RTgroup );
  RTresult (* RTGroupValidate)( RTgroup );
  RTresult (* RTGroupGetContext)( RTgroup , RTcontext* );
  RTresult (* RTGroupSetAcceleration)( RTgroup , RTacceleration );
  RTresult (* RTGroupGetAcceleration)( RTgroup , RTacceleration* );
  RTresult (* RTGroupSetChildCount)( RTgroup , unsigned int );
  RTresult (* RTGroupGetChildCount)( RTgroup , unsigned int* );
  RTresult (* RTGroupSetChild)( RTgroup , unsigned int , RTobject );
  RTresult (* RTGroupGetChild)( RTgroup , unsigned int , RTobject* );
  RTresult (* RTGroupGetChildType)( RTgroup , unsigned int , RTobjecttype* );
  RTresult (* RTSelectorCreate)( RTcontext , RTselector* );
  RTresult (* RTSelectorDestroy)( RTselector );
  RTresult (* RTSelectorValidate)( RTselector );
  RTresult (* RTSelectorGetContext)( RTselector , RTcontext* );
  RTresult (* RTSelectorSetVisitProgram)( RTselector , RTprogram );
  RTresult (* RTSelectorGetVisitProgram)( RTselector , RTprogram* );
  RTresult (* RTSelectorSetChildCount)( RTselector , unsigned int );
  RTresult (* RTSelectorGetChildCount)( RTselector , unsigned int* );
  RTresult (* RTSelectorSetChild)( RTselector , unsigned int , RTobject );
  RTresult (* RTSelectorGetChild)( RTselector , unsigned int , RTobject* );
  RTresult (* RTSelectorGetChildType)( RTselector , unsigned int , RTobjecttype* );
  RTresult (* RTSelectorDeclareVariable)( RTselector , const char* , RTvariable* );
  RTresult (* RTSelectorQueryVariable)( RTselector , const char* , RTvariable* );
  RTresult (* RTSelectorRemoveVariable)( RTselector , RTvariable );
  RTresult (* RTSelectorGetVariableCount)( RTselector , unsigned int* );
  RTresult (* RTSelectorGetVariable)( RTselector , unsigned int , RTvariable* );
  RTresult (* RTTransformCreate)( RTcontext , RTtransform* );
  RTresult (* RTTransformDestroy)( RTtransform );
  RTresult (* RTTransformValidate)( RTtransform );
  RTresult (* RTTransformGetContext)( RTtransform , RTcontext* );
  RTresult (* RTTransformSetMatrix)( RTtransform , int , const float* , const float* );
  RTresult (* RTTransformGetMatrix)( RTtransform , int , float* , float* );
  RTresult (* RTTransformSetChild)( RTtransform , RTobject );
  RTresult (* RTTransformGetChild)( RTtransform , RTobject* );
  RTresult (* RTTransformGetChildType)( RTtransform , RTobjecttype* );
  RTresult (* RTGeometryGroupCreate)( RTcontext , RTgeometrygroup* );
  RTresult (* RTGeometryGroupDestroy)( RTgeometrygroup );
  RTresult (* RTGeometryGroupValidate)( RTgeometrygroup );
  RTresult (* RTGeometryGroupGetContext)( RTgeometrygroup , RTcontext* );
  RTresult (* RTGeometryGroupSetAcceleration)( RTgeometrygroup , RTacceleration );
  RTresult (* RTGeometryGroupGetAcceleration)( RTgeometrygroup , RTacceleration* );
  RTresult (* RTGeometryGroupSetChildCount)( RTgeometrygroup , unsigned int );
  RTresult (* RTGeometryGroupGetChildCount)( RTgeometrygroup , unsigned int* );
  RTresult (* RTGeometryGroupSetChild)( RTgeometrygroup , unsigned int , RTgeometryinstance );
  RTresult (* RTGeometryGroupGetChild)( RTgeometrygroup , unsigned int , RTgeometryinstance* );
  RTresult (* RTAccelerationCreate)( RTcontext , RTacceleration* );
  RTresult (* RTAccelerationDestroy)( RTacceleration );
  RTresult (* RTAccelerationValidate)( RTacceleration );
  RTresult (* RTAccelerationGetContext)( RTacceleration , RTcontext* );
  RTresult (* RTAccelerationSetBuilder)( RTacceleration , const char* );
  RTresult (* RTAccelerationGetBuilder)( RTacceleration , const char** );
  RTresult (* RTAccelerationSetTraverser)( RTacceleration , const char* );
  RTresult (* RTAccelerationGetTraverser)( RTacceleration , const char** );
  RTresult (* RTAccelerationSetProperty)( RTacceleration , const char* , const char* );
  RTresult (* RTAccelerationGetProperty)( RTacceleration , const char* , const char** );
  RTresult (* RTAccelerationGetDataSize)( RTacceleration , RTsize* );
  RTresult (* RTAccelerationGetData)( RTacceleration , void* );
  RTresult (* RTAccelerationSetData)( RTacceleration , const void* , RTsize );
  RTresult (* RTAccelerationMarkDirty)( RTacceleration );
  RTresult (* RTAccelerationIsDirty)( RTacceleration , int* );
  RTresult (* RTGeometryInstanceCreate)( RTcontext , RTgeometryinstance* );
  RTresult (* RTGeometryInstanceDestroy)( RTgeometryinstance );
  RTresult (* RTGeometryInstanceValidate)( RTgeometryinstance );
  RTresult (* RTGeometryInstanceGetContext)( RTgeometryinstance , RTcontext* );
  RTresult (* RTGeometryInstanceSetGeometry)( RTgeometryinstance , RTgeometry );
  RTresult (* RTGeometryInstanceGetGeometry)( RTgeometryinstance , RTgeometry* );
  RTresult (* RTGeometryInstanceSetMaterialCount)( RTgeometryinstance , unsigned int );
  RTresult (* RTGeometryInstanceGetMaterialCount)( RTgeometryinstance , unsigned int* );
  RTresult (* RTGeometryInstanceSetMaterial)( RTgeometryinstance , unsigned int , RTmaterial );
  RTresult (* RTGeometryInstanceGetMaterial)( RTgeometryinstance , unsigned int , RTmaterial* );
  RTresult (* RTGeometryInstanceDeclareVariable)( RTgeometryinstance , const char* , RTvariable* );
  RTresult (* RTGeometryInstanceQueryVariable)( RTgeometryinstance , const char* , RTvariable* );
  RTresult (* RTGeometryInstanceRemoveVariable)( RTgeometryinstance , RTvariable );
  RTresult (* RTGeometryInstanceGetVariableCount)( RTgeometryinstance , unsigned int* );
  RTresult (* RTGeometryInstanceGetVariable)( RTgeometryinstance , unsigned int , RTvariable* );
  RTresult (* RTGeometryCreate)( RTcontext , RTgeometry* );
  RTresult (* RTGeometryDestroy)( RTgeometry );
  RTresult (* RTGeometryValidate)( RTgeometry );
  RTresult (* RTGeometryGetContext)( RTgeometry , RTcontext* );
  RTresult (* RTGeometrySetPrimitiveCount)( RTgeometry , unsigned int );
  RTresult (* RTGeometryGetPrimitiveCount)( RTgeometry , unsigned int* );
  RTresult (* RTGeometrySetBoundingBoxProgram)( RTgeometry , RTprogram );
  RTresult (* RTGeometryGetBoundingBoxProgram)( RTgeometry , RTprogram* );
  RTresult (* RTGeometrySetIntersectionProgram)( RTgeometry , RTprogram );
  RTresult (* RTGeometryGetIntersectionProgram)( RTgeometry , RTprogram* );
  RTresult (* RTGeometryMarkDirty)( RTgeometry );
  RTresult (* RTGeometryIsDirty)( RTgeometry , int* );
  RTresult (* RTGeometryDeclareVariable)( RTgeometry , const char* , RTvariable* );
  RTresult (* RTGeometryQueryVariable)( RTgeometry , const char* , RTvariable* );
  RTresult (* RTGeometryRemoveVariable)( RTgeometry , RTvariable );
  RTresult (* RTGeometryGetVariableCount)( RTgeometry , unsigned int* );
  RTresult (* RTGeometryGetVariable)( RTgeometry , unsigned int , RTvariable* );
  RTresult (* RTMaterialCreate)( RTcontext , RTmaterial* );
  RTresult (* RTMaterialDestroy)( RTmaterial );
  RTresult (* RTMaterialValidate)( RTmaterial );
  RTresult (* RTMaterialGetContext)( RTmaterial , RTcontext* );
  RTresult (* RTMaterialSetClosestHitProgram)( RTmaterial , unsigned int , RTprogram );
  RTresult (* RTMaterialGetClosestHitProgram)( RTmaterial , unsigned int , RTprogram* );
  RTresult (* RTMaterialSetAnyHitProgram)( RTmaterial , unsigned int , RTprogram );
  RTresult (* RTMaterialGetAnyHitProgram)( RTmaterial , unsigned int , RTprogram* );
  RTresult (* RTMaterialDeclareVariable)( RTmaterial , const char* , RTvariable* );
  RTresult (* RTMaterialQueryVariable)( RTmaterial , const char* , RTvariable* );
  RTresult (* RTMaterialRemoveVariable)( RTmaterial , RTvariable );
  RTresult (* RTMaterialGetVariableCount)( RTmaterial , unsigned int* );
  RTresult (* RTMaterialGetVariable)( RTmaterial , unsigned int , RTvariable* );
  RTresult (* RTTextureSamplerCreate)( RTcontext , RTtexturesampler* );
  RTresult (* RTTextureSamplerDestroy)( RTtexturesampler );
  RTresult (* RTTextureSamplerValidate)( RTtexturesampler );
  RTresult (* RTTextureSamplerGetContext)( RTtexturesampler , RTcontext* );
  RTresult (* RTTextureSamplerSetMipLevelCount)( RTtexturesampler , unsigned int );
  RTresult (* RTTextureSamplerGetMipLevelCount)( RTtexturesampler , unsigned int* );
  RTresult (* RTTextureSamplerSetArraySize)( RTtexturesampler , unsigned int );
  RTresult (* RTTextureSamplerGetArraySize)( RTtexturesampler , unsigned int* );
  RTresult (* RTTextureSamplerSetWrapMode)( RTtexturesampler , unsigned int , RTwrapmode );
  RTresult (* RTTextureSamplerGetWrapMode)( RTtexturesampler , unsigned int , RTwrapmode* );
  RTresult (* RTTextureSamplerSetFilteringModes)( RTtexturesampler , RTfiltermode , RTfiltermode , RTfiltermode );
  RTresult (* RTTextureSamplerGetFilteringModes)( RTtexturesampler , RTfiltermode* , RTfiltermode* , RTfiltermode* );
  RTresult (* RTTextureSamplerSetMaxAnisotropy)( RTtexturesampler , float );
  RTresult (* RTTextureSamplerGetMaxAnisotropy)( RTtexturesampler , float* );
  RTresult (* RTTextureSamplerSetReadMode)( RTtexturesampler , RTtexturereadmode );
  RTresult (* RTTextureSamplerGetReadMode)( RTtexturesampler , RTtexturereadmode* );
  RTresult (* RTTextureSamplerSetIndexingMode)( RTtexturesampler , RTtextureindexmode );
  RTresult (* RTTextureSamplerGetIndexingMode)( RTtexturesampler , RTtextureindexmode* );
  RTresult (* RTTextureSamplerSetBuffer)( RTtexturesampler , unsigned int , unsigned int , RTbuffer );
  RTresult (* RTTextureSamplerGetBuffer)( RTtexturesampler , unsigned int , unsigned int , RTbuffer* );
  RTresult (* RTBufferCreate)( RTcontext , unsigned int , RTbuffer* );
  RTresult (* RTBufferDestroy)( RTbuffer );
  RTresult (* RTBufferValidate)( RTbuffer );
  RTresult (* RTBufferGetContext)( RTbuffer , RTcontext* );
  RTresult (* RTBufferSetFormat)( RTbuffer , RTformat );
  RTresult (* RTBufferGetFormat)( RTbuffer , RTformat* );
  RTresult (* RTBufferSetElementSize)( RTbuffer , RTsize );
  RTresult (* RTBufferGetElementSize)( RTbuffer , RTsize* );
  RTresult (* RTBufferSetSize1D)( RTbuffer , RTsize );
  RTresult (* RTBufferGetSize1D)( RTbuffer , RTsize* );
  RTresult (* RTBufferSetSize2D)( RTbuffer , RTsize , RTsize );
  RTresult (* RTBufferGetSize2D)( RTbuffer , RTsize* , RTsize* );
  RTresult (* RTBufferSetSize3D)( RTbuffer , RTsize , RTsize , RTsize );
  RTresult (* RTBufferGetSize3D)( RTbuffer , RTsize* , RTsize* , RTsize* );
  RTresult (* RTBufferSetSizev)( RTbuffer , unsigned int , const RTsize* );
  RTresult (* RTBufferGetSizev)( RTbuffer , unsigned int , RTsize* );
  RTresult (* RTBufferGetDimensionality)( RTbuffer , unsigned int* );
  RTresult (* RTBufferMap)( RTbuffer , void** );
  RTresult (* RTBufferUnmap)( RTbuffer );
};

extern struct NVIRTFunctions _NVIRTFunctions;

bool InitNVIRTFunctions( void * (*DLL_FUNC)( void *, const std::string & ), void * DLL_ARG );
RTresult NVRT_API RTGetVersion( unsigned int* version);
RTresult NVRT_API RTDeviceGetDeviceCount( unsigned int* count);
RTresult NVRT_API RTDeviceGetAttribute( int ordinal, RTdeviceattribute attrib, RTsize size, void* p);
RTresult NVRT_API RTVariableSet1f( RTvariable v, float f1);
RTresult NVRT_API RTVariableSet2f( RTvariable v, float f1, float f2);
RTresult NVRT_API RTVariableSet3f( RTvariable v, float f1, float f2, float f3);
RTresult NVRT_API RTVariableSet4f( RTvariable v, float f1, float f2, float f3, float f4);
RTresult NVRT_API RTVariableSet1fv( RTvariable v, const float* f);
RTresult NVRT_API RTVariableSet2fv( RTvariable v, const float* f);
RTresult NVRT_API RTVariableSet3fv( RTvariable v, const float* f);
RTresult NVRT_API RTVariableSet4fv( RTvariable v, const float* f);
RTresult NVRT_API RTVariableSet1i( RTvariable v, int i1);
RTresult NVRT_API RTVariableSet2i( RTvariable v, int i1, int i2);
RTresult NVRT_API RTVariableSet3i( RTvariable v, int i1, int i2, int i3);
RTresult NVRT_API RTVariableSet4i( RTvariable v, int i1, int i2, int i3, int i4);
RTresult NVRT_API RTVariableSet1iv( RTvariable v, const int* i);
RTresult NVRT_API RTVariableSet2iv( RTvariable v, const int* i);
RTresult NVRT_API RTVariableSet3iv( RTvariable v, const int* i);
RTresult NVRT_API RTVariableSet4iv( RTvariable v, const int* i);
RTresult NVRT_API RTVariableSet1ui( RTvariable v, unsigned int u1);
RTresult NVRT_API RTVariableSet2ui( RTvariable v, unsigned int u1, unsigned int u2);
RTresult NVRT_API RTVariableSet3ui( RTvariable v, unsigned int u1, unsigned int u2, unsigned int u3);
RTresult NVRT_API RTVariableSet4ui( RTvariable v, unsigned int u1, unsigned int u2, unsigned int u3, unsigned int u4);
RTresult NVRT_API RTVariableSet1uiv( RTvariable v, const unsigned int* u);
RTresult NVRT_API RTVariableSet2uiv( RTvariable v, const unsigned int* u);
RTresult NVRT_API RTVariableSet3uiv( RTvariable v, const unsigned int* u);
RTresult NVRT_API RTVariableSet4uiv( RTvariable v, const unsigned int* u);
RTresult NVRT_API RTVariableSetMatrix2x2fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix2x3fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix2x4fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix3x2fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix3x3fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix3x4fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix4x2fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix4x3fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetMatrix4x4fv( RTvariable v, int transpose, const float* m);
RTresult NVRT_API RTVariableSetObject( RTvariable v, RTobject object);
RTresult NVRT_API RTVariableSetUserData( RTvariable v, RTsize size, const void* ptr);
RTresult NVRT_API RTVariableGet1f( RTvariable v, float* f1);
RTresult NVRT_API RTVariableGet2f( RTvariable v, float* f1, float* f2);
RTresult NVRT_API RTVariableGet3f( RTvariable v, float* f1, float* f2, float* f3);
RTresult NVRT_API RTVariableGet4f( RTvariable v, float* f1, float* f2, float* f3, float* f4);
RTresult NVRT_API RTVariableGet1fv( RTvariable v, float* f);
RTresult NVRT_API RTVariableGet2fv( RTvariable v, float* f);
RTresult NVRT_API RTVariableGet3fv( RTvariable v, float* f);
RTresult NVRT_API RTVariableGet4fv( RTvariable v, float* f);
RTresult NVRT_API RTVariableGet1i( RTvariable v, int* i1);
RTresult NVRT_API RTVariableGet2i( RTvariable v, int* i1, int* i2);
RTresult NVRT_API RTVariableGet3i( RTvariable v, int* i1, int* i2, int* i3);
RTresult NVRT_API RTVariableGet4i( RTvariable v, int* i1, int* i2, int* i3, int* i4);
RTresult NVRT_API RTVariableGet1iv( RTvariable v, int* i);
RTresult NVRT_API RTVariableGet2iv( RTvariable v, int* i);
RTresult NVRT_API RTVariableGet3iv( RTvariable v, int* i);
RTresult NVRT_API RTVariableGet4iv( RTvariable v, int* i);
RTresult NVRT_API RTVariableGet1ui( RTvariable v, unsigned int* u1);
RTresult NVRT_API RTVariableGet2ui( RTvariable v, unsigned int* u1, unsigned int* u2);
RTresult NVRT_API RTVariableGet3ui( RTvariable v, unsigned int* u1, unsigned int* u2, unsigned int* u3);
RTresult NVRT_API RTVariableGet4ui( RTvariable v, unsigned int* u1, unsigned int* u2, unsigned int* u3, unsigned int* u4);
RTresult NVRT_API RTVariableGet1uiv( RTvariable v, unsigned int* u);
RTresult NVRT_API RTVariableGet2uiv( RTvariable v, unsigned int* u);
RTresult NVRT_API RTVariableGet3uiv( RTvariable v, unsigned int* u);
RTresult NVRT_API RTVariableGet4uiv( RTvariable v, unsigned int* u);
RTresult NVRT_API RTVariableGetMatrix2x2fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix2x3fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix2x4fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix3x2fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix3x3fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix3x4fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix4x2fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix4x3fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetMatrix4x4fv( RTvariable v, int transpose, float* m);
RTresult NVRT_API RTVariableGetObject( RTvariable v, RTobject* object);
RTresult NVRT_API RTVariableGetUserData( RTvariable v, RTsize size, void* ptr);
RTresult NVRT_API RTVariableGetName( RTvariable v, const char** name_return);
RTresult NVRT_API RTVariableGetAnnotation( RTvariable v, const char** annotation_return);
RTresult NVRT_API RTVariableGetType( RTvariable v, RTobjecttype* type_return);
RTresult NVRT_API RTVariableGetContext( RTvariable v, RTcontext* context);
RTresult NVRT_API RTVariableGetSize( RTvariable v, RTsize* size);
RTresult NVRT_API RTContextCreate( RTcontext* context);
RTresult NVRT_API RTContextDestroy( RTcontext  context);
RTresult NVRT_API RTContextValidate( RTcontext  context);
void NVRT_API RTContextGetErrorString( RTcontext context, RTresult code, const char** return_string);
RTresult NVRT_API RTContextSetAttribute( RTcontext context, RTcontextattribute attrib, RTsize size, void* p);
RTresult NVRT_API RTContextGetAttribute( RTcontext context, RTcontextattribute attrib, RTsize size, void* p);
RTresult NVRT_API RTContextSetDevices( RTcontext context, unsigned int count, const int* devices);
RTresult NVRT_API RTContextGetDevices( RTcontext context, int* devices);
RTresult NVRT_API RTContextGetDeviceCount( RTcontext context, unsigned int* count);
RTresult NVRT_API RTContextSetStackSize( RTcontext context, RTsize  stack_size_bytes);
RTresult NVRT_API RTContextGetStackSize( RTcontext context, RTsize* stack_size_bytes);
RTresult NVRT_API RTContextSetTimeoutCallback( RTcontext context, RTtimeoutcallback callback, double min_polling_seconds);
RTresult NVRT_API RTContextSetEntryPointCount( RTcontext context, unsigned int  num_entry_points);
RTresult NVRT_API RTContextGetEntryPointCount( RTcontext context, unsigned int* num_entry_points);
RTresult NVRT_API RTContextSetRayGenerationProgram( RTcontext context, unsigned int entry_point_index, RTprogram  program);
RTresult NVRT_API RTContextGetRayGenerationProgram( RTcontext context, unsigned int entry_point_index, RTprogram* program);
RTresult NVRT_API RTContextSetExceptionProgram( RTcontext context, unsigned int entry_point_index, RTprogram  program);
RTresult NVRT_API RTContextGetExceptionProgram( RTcontext context, unsigned int entry_point_index, RTprogram* program);
RTresult NVRT_API RTContextSetExceptionEnabled( RTcontext context, RTexception exception, int  enabled );
RTresult NVRT_API RTContextGetExceptionEnabled( RTcontext context, RTexception exception, int* enabled );
RTresult NVRT_API RTContextSetRayTypeCount( RTcontext context, unsigned int  num_ray_types);
RTresult NVRT_API RTContextGetRayTypeCount( RTcontext context, unsigned int* num_ray_types);
RTresult NVRT_API RTContextSetMissProgram( RTcontext context, unsigned int ray_type_index, RTprogram  program);
RTresult NVRT_API RTContextGetMissProgram( RTcontext context, unsigned int ray_type_index, RTprogram* program);
RTresult NVRT_API RTContextCompile( RTcontext context);
RTresult NVRT_API RTContextLaunch1D( RTcontext context, unsigned int entry_point_index, RTsize image_width);
RTresult NVRT_API RTContextLaunch2D( RTcontext context, unsigned int entry_point_index, RTsize image_width, RTsize image_height);
RTresult NVRT_API RTContextLaunch3D( RTcontext context, unsigned int entry_point_index, RTsize image_width, RTsize image_height, RTsize image_depth);
RTresult NVRT_API RTContextGetRunningState( RTcontext context, int* running);
RTresult NVRT_API RTContextSetPrintEnabled( RTcontext context, int  enabled);
RTresult NVRT_API RTContextGetPrintEnabled( RTcontext context, int* enabled);
RTresult NVRT_API RTContextSetPrintBufferSize( RTcontext context, RTsize  buffer_size_bytes);
RTresult NVRT_API RTContextGetPrintBufferSize( RTcontext context, RTsize* buffer_size_bytes);
RTresult NVRT_API RTContextSetPrintLaunchIndex( RTcontext context, int  x, int  y, int  z);
RTresult NVRT_API RTContextGetPrintLaunchIndex( RTcontext context, int* x, int* y, int* z);
RTresult NVRT_API RTContextDeclareVariable( RTcontext context, const char* name, RTvariable* v);
RTresult NVRT_API RTContextQueryVariable( RTcontext context, const char* name, RTvariable* v);
RTresult NVRT_API RTContextRemoveVariable( RTcontext context, RTvariable v);
RTresult NVRT_API RTContextGetVariableCount( RTcontext context, unsigned int* count);
RTresult NVRT_API RTContextGetVariable( RTcontext context, unsigned int index, RTvariable* v);
RTresult NVRT_API RTProgramCreateFromPTXString( RTcontext context, const char* ptx, const char* program_name, RTprogram* program);
RTresult NVRT_API RTProgramCreateFromPTXFile( RTcontext context, const char* filename, const char* program_name, RTprogram* program);
RTresult NVRT_API RTProgramDestroy( RTprogram program);
RTresult NVRT_API RTProgramValidate( RTprogram program);
RTresult NVRT_API RTProgramGetContext( RTprogram program, RTcontext* context);
RTresult NVRT_API RTProgramDeclareVariable( RTprogram program, const char* name, RTvariable* v);
RTresult NVRT_API RTProgramQueryVariable( RTprogram program, const char* name, RTvariable* v);
RTresult NVRT_API RTProgramRemoveVariable( RTprogram program, RTvariable v);
RTresult NVRT_API RTProgramGetVariableCount( RTprogram program, unsigned int* count);
RTresult NVRT_API RTProgramGetVariable( RTprogram program, unsigned int index, RTvariable* v);
RTresult NVRT_API RTGroupCreate( RTcontext context, RTgroup* group);
RTresult NVRT_API RTGroupDestroy( RTgroup group);
RTresult NVRT_API RTGroupValidate( RTgroup group);
RTresult NVRT_API RTGroupGetContext( RTgroup group, RTcontext* context);
RTresult NVRT_API RTGroupSetAcceleration( RTgroup group, RTacceleration  acceleration);
RTresult NVRT_API RTGroupGetAcceleration( RTgroup group, RTacceleration* acceleration);
RTresult NVRT_API RTGroupSetChildCount( RTgroup group, unsigned int  count);
RTresult NVRT_API RTGroupGetChildCount( RTgroup group, unsigned int* count);
RTresult NVRT_API RTGroupSetChild( RTgroup group, unsigned int index, RTobject  child);
RTresult NVRT_API RTGroupGetChild( RTgroup group, unsigned int index, RTobject* child);
RTresult NVRT_API RTGroupGetChildType( RTgroup group, unsigned int index, RTobjecttype* type);
RTresult NVRT_API RTSelectorCreate( RTcontext context, RTselector* selector);
RTresult NVRT_API RTSelectorDestroy( RTselector selector);
RTresult NVRT_API RTSelectorValidate( RTselector selector);
RTresult NVRT_API RTSelectorGetContext( RTselector selector, RTcontext* context);
RTresult NVRT_API RTSelectorSetVisitProgram( RTselector selector, RTprogram  program);
RTresult NVRT_API RTSelectorGetVisitProgram( RTselector selector, RTprogram* program);
RTresult NVRT_API RTSelectorSetChildCount( RTselector selector, unsigned int  count);
RTresult NVRT_API RTSelectorGetChildCount( RTselector selector, unsigned int* count);
RTresult NVRT_API RTSelectorSetChild( RTselector selector, unsigned int index, RTobject  child);
RTresult NVRT_API RTSelectorGetChild( RTselector selector, unsigned int index, RTobject* child);
RTresult NVRT_API RTSelectorGetChildType( RTselector selector, unsigned int index, RTobjecttype* type);
RTresult NVRT_API RTSelectorDeclareVariable( RTselector selector, const char* name, RTvariable* v);
RTresult NVRT_API RTSelectorQueryVariable( RTselector selector, const char* name, RTvariable* v);
RTresult NVRT_API RTSelectorRemoveVariable( RTselector selector, RTvariable v);
RTresult NVRT_API RTSelectorGetVariableCount( RTselector selector, unsigned int* count);
RTresult NVRT_API RTSelectorGetVariable( RTselector selector, unsigned int index, RTvariable* v);
RTresult NVRT_API RTTransformCreate( RTcontext context, RTtransform* transform);
RTresult NVRT_API RTTransformDestroy( RTtransform transform);
RTresult NVRT_API RTTransformValidate( RTtransform transform);
RTresult NVRT_API RTTransformGetContext( RTtransform transform, RTcontext* context);
RTresult NVRT_API RTTransformSetMatrix( RTtransform transform, int transpose, const float* matrix, const float* inverse_matrix);
RTresult NVRT_API RTTransformGetMatrix( RTtransform transform, int transpose, float* matrix, float* inverse_matrix);
RTresult NVRT_API RTTransformSetChild( RTtransform transform, RTobject  child);
RTresult NVRT_API RTTransformGetChild( RTtransform transform, RTobject* child);
RTresult NVRT_API RTTransformGetChildType( RTtransform transform, RTobjecttype* type);
RTresult NVRT_API RTGeometryGroupCreate( RTcontext context, RTgeometrygroup* geometrygroup);
RTresult NVRT_API RTGeometryGroupDestroy( RTgeometrygroup geometrygroup);
RTresult NVRT_API RTGeometryGroupValidate( RTgeometrygroup geometrygroup);
RTresult NVRT_API RTGeometryGroupGetContext( RTgeometrygroup geometrygroup, RTcontext* context);
RTresult NVRT_API RTGeometryGroupSetAcceleration( RTgeometrygroup geometrygroup, RTacceleration  acceleration);
RTresult NVRT_API RTGeometryGroupGetAcceleration( RTgeometrygroup geometrygroup, RTacceleration* acceleration);
RTresult NVRT_API RTGeometryGroupSetChildCount( RTgeometrygroup geometrygroup, unsigned int  count);
RTresult NVRT_API RTGeometryGroupGetChildCount( RTgeometrygroup geometrygroup, unsigned int* count);
RTresult NVRT_API RTGeometryGroupSetChild( RTgeometrygroup geometrygroup, unsigned int index, RTgeometryinstance  geometryinstance);
RTresult NVRT_API RTGeometryGroupGetChild( RTgeometrygroup geometrygroup, unsigned int index, RTgeometryinstance* geometryinstance);
RTresult NVRT_API RTAccelerationCreate( RTcontext context, RTacceleration* acceleration);
RTresult NVRT_API RTAccelerationDestroy( RTacceleration acceleration);
RTresult NVRT_API RTAccelerationValidate( RTacceleration acceleration);
RTresult NVRT_API RTAccelerationGetContext( RTacceleration acceleration, RTcontext* context);
RTresult NVRT_API RTAccelerationSetBuilder( RTacceleration acceleration, const char* builder);
RTresult NVRT_API RTAccelerationGetBuilder( RTacceleration acceleration, const char** return_string);
RTresult NVRT_API RTAccelerationSetTraverser( RTacceleration acceleration, const char* traverser);
RTresult NVRT_API RTAccelerationGetTraverser( RTacceleration acceleration, const char** return_string);
RTresult NVRT_API RTAccelerationSetProperty( RTacceleration acceleration, const char* name, const char* value);
RTresult NVRT_API RTAccelerationGetProperty( RTacceleration acceleration, const char* name, const char** return_string);
RTresult NVRT_API RTAccelerationGetDataSize( RTacceleration acceleration, RTsize* size);
RTresult NVRT_API RTAccelerationGetData( RTacceleration acceleration, void* data);
RTresult NVRT_API RTAccelerationSetData( RTacceleration acceleration, const void* data, RTsize size);
RTresult NVRT_API RTAccelerationMarkDirty( RTacceleration acceleration);
RTresult NVRT_API RTAccelerationIsDirty( RTacceleration acceleration, int* dirty);
RTresult NVRT_API RTGeometryInstanceCreate( RTcontext context, RTgeometryinstance* geometryinstance);
RTresult NVRT_API RTGeometryInstanceDestroy( RTgeometryinstance geometryinstance);
RTresult NVRT_API RTGeometryInstanceValidate( RTgeometryinstance geometryinstance);
RTresult NVRT_API RTGeometryInstanceGetContext( RTgeometryinstance geometryinstance, RTcontext* context);
RTresult NVRT_API RTGeometryInstanceSetGeometry( RTgeometryinstance geometryinstance, RTgeometry  geometry);
RTresult NVRT_API RTGeometryInstanceGetGeometry( RTgeometryinstance geometryinstance, RTgeometry* geometry);
RTresult NVRT_API RTGeometryInstanceSetMaterialCount( RTgeometryinstance geometryinstance, unsigned int  count);
RTresult NVRT_API RTGeometryInstanceGetMaterialCount( RTgeometryinstance geometryinstance, unsigned int* count);
RTresult NVRT_API RTGeometryInstanceSetMaterial( RTgeometryinstance geometryinstance, unsigned int idx, RTmaterial  material);
RTresult NVRT_API RTGeometryInstanceGetMaterial( RTgeometryinstance geometryinstance, unsigned int idx, RTmaterial* material);
RTresult NVRT_API RTGeometryInstanceDeclareVariable( RTgeometryinstance geometryinstance, const char* name, RTvariable* v);
RTresult NVRT_API RTGeometryInstanceQueryVariable( RTgeometryinstance geometryinstance, const char* name, RTvariable* v);
RTresult NVRT_API RTGeometryInstanceRemoveVariable( RTgeometryinstance geometryinstance, RTvariable v);
RTresult NVRT_API RTGeometryInstanceGetVariableCount( RTgeometryinstance geometryinstance, unsigned int* count);
RTresult NVRT_API RTGeometryInstanceGetVariable( RTgeometryinstance geometryinstance, unsigned int index, RTvariable* v);
RTresult NVRT_API RTGeometryCreate( RTcontext context, RTgeometry* geometry);
RTresult NVRT_API RTGeometryDestroy( RTgeometry geometry);
RTresult NVRT_API RTGeometryValidate( RTgeometry geometry);
RTresult NVRT_API RTGeometryGetContext( RTgeometry geometry, RTcontext* context);
RTresult NVRT_API RTGeometrySetPrimitiveCount( RTgeometry geometry, unsigned int  num_primitives);
RTresult NVRT_API RTGeometryGetPrimitiveCount( RTgeometry geometry, unsigned int* num_primitives);
RTresult NVRT_API RTGeometrySetBoundingBoxProgram( RTgeometry geometry, RTprogram  program);
RTresult NVRT_API RTGeometryGetBoundingBoxProgram( RTgeometry geometry, RTprogram* program);
RTresult NVRT_API RTGeometrySetIntersectionProgram( RTgeometry geometry, RTprogram  program);
RTresult NVRT_API RTGeometryGetIntersectionProgram( RTgeometry geometry, RTprogram* program);
RTresult NVRT_API RTGeometryMarkDirty( RTgeometry geometry);
RTresult NVRT_API RTGeometryIsDirty( RTgeometry geometry, int* dirty);
RTresult NVRT_API RTGeometryDeclareVariable( RTgeometry geometry, const char* name, RTvariable* v);
RTresult NVRT_API RTGeometryQueryVariable( RTgeometry geometry, const char* name, RTvariable* v);
RTresult NVRT_API RTGeometryRemoveVariable( RTgeometry geometry, RTvariable v);
RTresult NVRT_API RTGeometryGetVariableCount( RTgeometry geometry, unsigned int* count);
RTresult NVRT_API RTGeometryGetVariable( RTgeometry geometry, unsigned int index, RTvariable* v);
RTresult NVRT_API RTMaterialCreate( RTcontext context, RTmaterial* material);
RTresult NVRT_API RTMaterialDestroy( RTmaterial material);
RTresult NVRT_API RTMaterialValidate( RTmaterial material);
RTresult NVRT_API RTMaterialGetContext( RTmaterial material, RTcontext* context);
RTresult NVRT_API RTMaterialSetClosestHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram  program);
RTresult NVRT_API RTMaterialGetClosestHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram* program);
RTresult NVRT_API RTMaterialSetAnyHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram  program);
RTresult NVRT_API RTMaterialGetAnyHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram* program);
RTresult NVRT_API RTMaterialDeclareVariable( RTmaterial material, const char* name, RTvariable* v);
RTresult NVRT_API RTMaterialQueryVariable( RTmaterial material, const char* name, RTvariable* v);
RTresult NVRT_API RTMaterialRemoveVariable( RTmaterial material, RTvariable v);
RTresult NVRT_API RTMaterialGetVariableCount( RTmaterial material, unsigned int* count);
RTresult NVRT_API RTMaterialGetVariable( RTmaterial material, unsigned int index, RTvariable* v);
RTresult NVRT_API RTTextureSamplerCreate( RTcontext context, RTtexturesampler* texturesampler);
RTresult NVRT_API RTTextureSamplerDestroy( RTtexturesampler texturesampler);
RTresult NVRT_API RTTextureSamplerValidate( RTtexturesampler texturesampler);
RTresult NVRT_API RTTextureSamplerGetContext( RTtexturesampler texturesampler, RTcontext* context);
RTresult NVRT_API RTTextureSamplerSetMipLevelCount( RTtexturesampler texturesampler, unsigned int  num_mip_levels);
RTresult NVRT_API RTTextureSamplerGetMipLevelCount( RTtexturesampler texturesampler, unsigned int* num_mip_levels);
RTresult NVRT_API RTTextureSamplerSetArraySize( RTtexturesampler texturesampler, unsigned int  num_textures_in_array);
RTresult NVRT_API RTTextureSamplerGetArraySize( RTtexturesampler texturesampler, unsigned int* num_textures_in_array);
RTresult NVRT_API RTTextureSamplerSetWrapMode( RTtexturesampler texturesampler, unsigned int dimension, RTwrapmode wrapmode);
RTresult NVRT_API RTTextureSamplerGetWrapMode( RTtexturesampler texturesampler, unsigned int dimension, RTwrapmode* wrapmode);
RTresult NVRT_API RTTextureSamplerSetFilteringModes( RTtexturesampler texturesampler, RTfiltermode  minification, RTfiltermode  magnification, RTfiltermode  mipmapping);
RTresult NVRT_API RTTextureSamplerGetFilteringModes( RTtexturesampler texturesampler, RTfiltermode* minification, RTfiltermode* magnification, RTfiltermode* mipmapping);
RTresult NVRT_API RTTextureSamplerSetMaxAnisotropy( RTtexturesampler texturesampler, float value);
RTresult NVRT_API RTTextureSamplerGetMaxAnisotropy( RTtexturesampler texturesampler, float* value);
RTresult NVRT_API RTTextureSamplerSetReadMode( RTtexturesampler texturesampler, RTtexturereadmode  readmode);
RTresult NVRT_API RTTextureSamplerGetReadMode( RTtexturesampler texturesampler, RTtexturereadmode* readmode);
RTresult NVRT_API RTTextureSamplerSetIndexingMode( RTtexturesampler texturesampler, RTtextureindexmode  indexmode);
RTresult NVRT_API RTTextureSamplerGetIndexingMode( RTtexturesampler texturesampler, RTtextureindexmode* indexmode);
RTresult NVRT_API RTTextureSamplerSetBuffer( RTtexturesampler texturesampler, unsigned int texture_array_idx, unsigned int mip_level, RTbuffer  buffer);
RTresult NVRT_API RTTextureSamplerGetBuffer( RTtexturesampler texturesampler, unsigned int texture_array_idx, unsigned int mip_level, RTbuffer* buffer);
RTresult NVRT_API RTBufferCreate( RTcontext context, unsigned int bufferdesc, RTbuffer* buffer);
RTresult NVRT_API RTBufferDestroy( RTbuffer buffer);
RTresult NVRT_API RTBufferValidate( RTbuffer buffer);
RTresult NVRT_API RTBufferGetContext( RTbuffer buffer, RTcontext* context);
RTresult NVRT_API RTBufferSetFormat( RTbuffer buffer, RTformat  format);
RTresult NVRT_API RTBufferGetFormat( RTbuffer buffer, RTformat* format);
RTresult NVRT_API RTBufferSetElementSize( RTbuffer buffer, RTsize  size_of_element);
RTresult NVRT_API RTBufferGetElementSize( RTbuffer buffer, RTsize* size_of_element);
RTresult NVRT_API RTBufferSetSize1D( RTbuffer buffer, RTsize  width);
RTresult NVRT_API RTBufferGetSize1D( RTbuffer buffer, RTsize* width);
RTresult NVRT_API RTBufferSetSize2D( RTbuffer buffer, RTsize  width, RTsize  height);
RTresult NVRT_API RTBufferGetSize2D( RTbuffer buffer, RTsize* width, RTsize* height);
RTresult NVRT_API RTBufferSetSize3D( RTbuffer buffer, RTsize  width, RTsize  height, RTsize  depth);
RTresult NVRT_API RTBufferGetSize3D( RTbuffer buffer, RTsize* width, RTsize* height, RTsize* depth);
RTresult NVRT_API RTBufferSetSizev( RTbuffer buffer, unsigned int dimensionality, const RTsize* dims);
RTresult NVRT_API RTBufferGetSizev( RTbuffer buffer, unsigned int dimensionality,       RTsize* dims);
RTresult NVRT_API RTBufferGetDimensionality( RTbuffer buffer, unsigned int* dimensionality);
RTresult NVRT_API RTBufferMap( RTbuffer buffer, void** user_pointer);
RTresult NVRT_API RTBufferUnmap( RTbuffer buffer);


inline RTresult RTGetVersion( unsigned int* version )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGetVersion );
  return _NVIRTFunctions.RTGetVersion( version );
}

inline RTresult RTDeviceGetDeviceCount( unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTDeviceGetDeviceCount );
  return _NVIRTFunctions.RTDeviceGetDeviceCount( count );
}

inline RTresult RTDeviceGetAttribute( int ordinal, RTdeviceattribute attrib, RTsize size, void* p )
{
  NVSG_ASSERT( _NVIRTFunctions.RTDeviceGetAttribute );
  return _NVIRTFunctions.RTDeviceGetAttribute( ordinal, attrib, size, p );
}

inline RTresult RTVariableSet1f( RTvariable v, float f1 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet1f );
  return _NVIRTFunctions.RTVariableSet1f( v, f1 );
}

inline RTresult RTVariableSet2f( RTvariable v, float f1, float f2 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet2f );
  return _NVIRTFunctions.RTVariableSet2f( v, f1, f2 );
}

inline RTresult RTVariableSet3f( RTvariable v, float f1, float f2, float f3 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet3f );
  return _NVIRTFunctions.RTVariableSet3f( v, f1, f2, f3 );
}

inline RTresult RTVariableSet4f( RTvariable v, float f1, float f2, float f3, float f4 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet4f );
  return _NVIRTFunctions.RTVariableSet4f( v, f1, f2, f3, f4 );
}

inline RTresult RTVariableSet1fv( RTvariable v, const float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet1fv );
  return _NVIRTFunctions.RTVariableSet1fv( v, f );
}

inline RTresult RTVariableSet2fv( RTvariable v, const float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet2fv );
  return _NVIRTFunctions.RTVariableSet2fv( v, f );
}

inline RTresult RTVariableSet3fv( RTvariable v, const float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet3fv );
  return _NVIRTFunctions.RTVariableSet3fv( v, f );
}

inline RTresult RTVariableSet4fv( RTvariable v, const float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet4fv );
  return _NVIRTFunctions.RTVariableSet4fv( v, f );
}

inline RTresult RTVariableSet1i( RTvariable v, int i1 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet1i );
  return _NVIRTFunctions.RTVariableSet1i( v, i1 );
}

inline RTresult RTVariableSet2i( RTvariable v, int i1, int i2 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet2i );
  return _NVIRTFunctions.RTVariableSet2i( v, i1, i2 );
}

inline RTresult RTVariableSet3i( RTvariable v, int i1, int i2, int i3 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet3i );
  return _NVIRTFunctions.RTVariableSet3i( v, i1, i2, i3 );
}

inline RTresult RTVariableSet4i( RTvariable v, int i1, int i2, int i3, int i4 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet4i );
  return _NVIRTFunctions.RTVariableSet4i( v, i1, i2, i3, i4 );
}

inline RTresult RTVariableSet1iv( RTvariable v, const int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet1iv );
  return _NVIRTFunctions.RTVariableSet1iv( v, i );
}

inline RTresult RTVariableSet2iv( RTvariable v, const int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet2iv );
  return _NVIRTFunctions.RTVariableSet2iv( v, i );
}

inline RTresult RTVariableSet3iv( RTvariable v, const int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet3iv );
  return _NVIRTFunctions.RTVariableSet3iv( v, i );
}

inline RTresult RTVariableSet4iv( RTvariable v, const int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet4iv );
  return _NVIRTFunctions.RTVariableSet4iv( v, i );
}

inline RTresult RTVariableSet1ui( RTvariable v, unsigned int u1 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet1ui );
  return _NVIRTFunctions.RTVariableSet1ui( v, u1 );
}

inline RTresult RTVariableSet2ui( RTvariable v, unsigned int u1, unsigned int u2 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet2ui );
  return _NVIRTFunctions.RTVariableSet2ui( v, u1, u2 );
}

inline RTresult RTVariableSet3ui( RTvariable v, unsigned int u1, unsigned int u2, unsigned int u3 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet3ui );
  return _NVIRTFunctions.RTVariableSet3ui( v, u1, u2, u3 );
}

inline RTresult RTVariableSet4ui( RTvariable v, unsigned int u1, unsigned int u2, unsigned int u3, unsigned int u4 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet4ui );
  return _NVIRTFunctions.RTVariableSet4ui( v, u1, u2, u3, u4 );
}

inline RTresult RTVariableSet1uiv( RTvariable v, const unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet1uiv );
  return _NVIRTFunctions.RTVariableSet1uiv( v, u );
}

inline RTresult RTVariableSet2uiv( RTvariable v, const unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet2uiv );
  return _NVIRTFunctions.RTVariableSet2uiv( v, u );
}

inline RTresult RTVariableSet3uiv( RTvariable v, const unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet3uiv );
  return _NVIRTFunctions.RTVariableSet3uiv( v, u );
}

inline RTresult RTVariableSet4uiv( RTvariable v, const unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSet4uiv );
  return _NVIRTFunctions.RTVariableSet4uiv( v, u );
}

inline RTresult RTVariableSetMatrix2x2fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix2x2fv );
  return _NVIRTFunctions.RTVariableSetMatrix2x2fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix2x3fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix2x3fv );
  return _NVIRTFunctions.RTVariableSetMatrix2x3fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix2x4fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix2x4fv );
  return _NVIRTFunctions.RTVariableSetMatrix2x4fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix3x2fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix3x2fv );
  return _NVIRTFunctions.RTVariableSetMatrix3x2fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix3x3fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix3x3fv );
  return _NVIRTFunctions.RTVariableSetMatrix3x3fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix3x4fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix3x4fv );
  return _NVIRTFunctions.RTVariableSetMatrix3x4fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix4x2fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix4x2fv );
  return _NVIRTFunctions.RTVariableSetMatrix4x2fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix4x3fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix4x3fv );
  return _NVIRTFunctions.RTVariableSetMatrix4x3fv( v, transpose, m );
}

inline RTresult RTVariableSetMatrix4x4fv( RTvariable v, int transpose, const float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetMatrix4x4fv );
  return _NVIRTFunctions.RTVariableSetMatrix4x4fv( v, transpose, m );
}

inline RTresult RTVariableSetObject( RTvariable v, RTobject object )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetObject );
  return _NVIRTFunctions.RTVariableSetObject( v, object );
}

inline RTresult RTVariableSetUserData( RTvariable v, RTsize size, const void* ptr )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableSetUserData );
  return _NVIRTFunctions.RTVariableSetUserData( v, size, ptr );
}

inline RTresult RTVariableGet1f( RTvariable v, float* f1 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet1f );
  return _NVIRTFunctions.RTVariableGet1f( v, f1 );
}

inline RTresult RTVariableGet2f( RTvariable v, float* f1, float* f2 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet2f );
  return _NVIRTFunctions.RTVariableGet2f( v, f1, f2 );
}

inline RTresult RTVariableGet3f( RTvariable v, float* f1, float* f2, float* f3 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet3f );
  return _NVIRTFunctions.RTVariableGet3f( v, f1, f2, f3 );
}

inline RTresult RTVariableGet4f( RTvariable v, float* f1, float* f2, float* f3, float* f4 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet4f );
  return _NVIRTFunctions.RTVariableGet4f( v, f1, f2, f3, f4 );
}

inline RTresult RTVariableGet1fv( RTvariable v, float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet1fv );
  return _NVIRTFunctions.RTVariableGet1fv( v, f );
}

inline RTresult RTVariableGet2fv( RTvariable v, float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet2fv );
  return _NVIRTFunctions.RTVariableGet2fv( v, f );
}

inline RTresult RTVariableGet3fv( RTvariable v, float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet3fv );
  return _NVIRTFunctions.RTVariableGet3fv( v, f );
}

inline RTresult RTVariableGet4fv( RTvariable v, float* f )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet4fv );
  return _NVIRTFunctions.RTVariableGet4fv( v, f );
}

inline RTresult RTVariableGet1i( RTvariable v, int* i1 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet1i );
  return _NVIRTFunctions.RTVariableGet1i( v, i1 );
}

inline RTresult RTVariableGet2i( RTvariable v, int* i1, int* i2 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet2i );
  return _NVIRTFunctions.RTVariableGet2i( v, i1, i2 );
}

inline RTresult RTVariableGet3i( RTvariable v, int* i1, int* i2, int* i3 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet3i );
  return _NVIRTFunctions.RTVariableGet3i( v, i1, i2, i3 );
}

inline RTresult RTVariableGet4i( RTvariable v, int* i1, int* i2, int* i3, int* i4 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet4i );
  return _NVIRTFunctions.RTVariableGet4i( v, i1, i2, i3, i4 );
}

inline RTresult RTVariableGet1iv( RTvariable v, int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet1iv );
  return _NVIRTFunctions.RTVariableGet1iv( v, i );
}

inline RTresult RTVariableGet2iv( RTvariable v, int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet2iv );
  return _NVIRTFunctions.RTVariableGet2iv( v, i );
}

inline RTresult RTVariableGet3iv( RTvariable v, int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet3iv );
  return _NVIRTFunctions.RTVariableGet3iv( v, i );
}

inline RTresult RTVariableGet4iv( RTvariable v, int* i )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet4iv );
  return _NVIRTFunctions.RTVariableGet4iv( v, i );
}

inline RTresult RTVariableGet1ui( RTvariable v, unsigned int* u1 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet1ui );
  return _NVIRTFunctions.RTVariableGet1ui( v, u1 );
}

inline RTresult RTVariableGet2ui( RTvariable v, unsigned int* u1, unsigned int* u2 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet2ui );
  return _NVIRTFunctions.RTVariableGet2ui( v, u1, u2 );
}

inline RTresult RTVariableGet3ui( RTvariable v, unsigned int* u1, unsigned int* u2, unsigned int* u3 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet3ui );
  return _NVIRTFunctions.RTVariableGet3ui( v, u1, u2, u3 );
}

inline RTresult RTVariableGet4ui( RTvariable v, unsigned int* u1, unsigned int* u2, unsigned int* u3, unsigned int* u4 )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet4ui );
  return _NVIRTFunctions.RTVariableGet4ui( v, u1, u2, u3, u4 );
}

inline RTresult RTVariableGet1uiv( RTvariable v, unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet1uiv );
  return _NVIRTFunctions.RTVariableGet1uiv( v, u );
}

inline RTresult RTVariableGet2uiv( RTvariable v, unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet2uiv );
  return _NVIRTFunctions.RTVariableGet2uiv( v, u );
}

inline RTresult RTVariableGet3uiv( RTvariable v, unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet3uiv );
  return _NVIRTFunctions.RTVariableGet3uiv( v, u );
}

inline RTresult RTVariableGet4uiv( RTvariable v, unsigned int* u )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGet4uiv );
  return _NVIRTFunctions.RTVariableGet4uiv( v, u );
}

inline RTresult RTVariableGetMatrix2x2fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix2x2fv );
  return _NVIRTFunctions.RTVariableGetMatrix2x2fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix2x3fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix2x3fv );
  return _NVIRTFunctions.RTVariableGetMatrix2x3fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix2x4fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix2x4fv );
  return _NVIRTFunctions.RTVariableGetMatrix2x4fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix3x2fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix3x2fv );
  return _NVIRTFunctions.RTVariableGetMatrix3x2fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix3x3fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix3x3fv );
  return _NVIRTFunctions.RTVariableGetMatrix3x3fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix3x4fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix3x4fv );
  return _NVIRTFunctions.RTVariableGetMatrix3x4fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix4x2fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix4x2fv );
  return _NVIRTFunctions.RTVariableGetMatrix4x2fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix4x3fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix4x3fv );
  return _NVIRTFunctions.RTVariableGetMatrix4x3fv( v, transpose, m );
}

inline RTresult RTVariableGetMatrix4x4fv( RTvariable v, int transpose, float* m )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetMatrix4x4fv );
  return _NVIRTFunctions.RTVariableGetMatrix4x4fv( v, transpose, m );
}

inline RTresult RTVariableGetObject( RTvariable v, RTobject* object )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetObject );
  return _NVIRTFunctions.RTVariableGetObject( v, object );
}

inline RTresult RTVariableGetUserData( RTvariable v, RTsize size, void* ptr )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetUserData );
  return _NVIRTFunctions.RTVariableGetUserData( v, size, ptr );
}

inline RTresult RTVariableGetName( RTvariable v, const char** name_return )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetName );
  return _NVIRTFunctions.RTVariableGetName( v, name_return );
}

inline RTresult RTVariableGetAnnotation( RTvariable v, const char** annotation_return )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetAnnotation );
  return _NVIRTFunctions.RTVariableGetAnnotation( v, annotation_return );
}

inline RTresult RTVariableGetType( RTvariable v, RTobjecttype* type_return )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetType );
  return _NVIRTFunctions.RTVariableGetType( v, type_return );
}

inline RTresult RTVariableGetContext( RTvariable v, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetContext );
  return _NVIRTFunctions.RTVariableGetContext( v, context );
}

inline RTresult RTVariableGetSize( RTvariable v, RTsize* size )
{
  NVSG_ASSERT( _NVIRTFunctions.RTVariableGetSize );
  return _NVIRTFunctions.RTVariableGetSize( v, size );
}

inline RTresult RTContextCreate( RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextCreate );
  return _NVIRTFunctions.RTContextCreate( context );
}

inline RTresult RTContextDestroy( RTcontext  context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextDestroy );
  return _NVIRTFunctions.RTContextDestroy( context );
}

inline RTresult RTContextValidate( RTcontext  context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextValidate );
  return _NVIRTFunctions.RTContextValidate( context );
}

inline void RTContextGetErrorString( RTcontext context, RTresult code, const char** return_string )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetErrorString );
  return _NVIRTFunctions.RTContextGetErrorString( context, code, return_string );
}

inline RTresult RTContextSetAttribute( RTcontext context, RTcontextattribute attrib, RTsize size, void* p )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetAttribute );
  return _NVIRTFunctions.RTContextSetAttribute( context, attrib, size, p );
}

inline RTresult RTContextGetAttribute( RTcontext context, RTcontextattribute attrib, RTsize size, void* p )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetAttribute );
  return _NVIRTFunctions.RTContextGetAttribute( context, attrib, size, p );
}

inline RTresult RTContextSetDevices( RTcontext context, unsigned int count, const int* devices )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetDevices );
  return _NVIRTFunctions.RTContextSetDevices( context, count, devices );
}

inline RTresult RTContextGetDevices( RTcontext context, int* devices )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetDevices );
  return _NVIRTFunctions.RTContextGetDevices( context, devices );
}

inline RTresult RTContextGetDeviceCount( RTcontext context, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetDeviceCount );
  return _NVIRTFunctions.RTContextGetDeviceCount( context, count );
}

inline RTresult RTContextSetStackSize( RTcontext context, RTsize  stack_size_bytes )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetStackSize );
  return _NVIRTFunctions.RTContextSetStackSize( context, stack_size_bytes );
}

inline RTresult RTContextGetStackSize( RTcontext context, RTsize* stack_size_bytes )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetStackSize );
  return _NVIRTFunctions.RTContextGetStackSize( context, stack_size_bytes );
}

inline RTresult RTContextSetTimeoutCallback( RTcontext context, RTtimeoutcallback callback, double min_polling_seconds )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetTimeoutCallback );
  return _NVIRTFunctions.RTContextSetTimeoutCallback( context, callback, min_polling_seconds );
}

inline RTresult RTContextSetEntryPointCount( RTcontext context, unsigned int  num_entry_points )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetEntryPointCount );
  return _NVIRTFunctions.RTContextSetEntryPointCount( context, num_entry_points );
}

inline RTresult RTContextGetEntryPointCount( RTcontext context, unsigned int* num_entry_points )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetEntryPointCount );
  return _NVIRTFunctions.RTContextGetEntryPointCount( context, num_entry_points );
}

inline RTresult RTContextSetRayGenerationProgram( RTcontext context, unsigned int entry_point_index, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetRayGenerationProgram );
  return _NVIRTFunctions.RTContextSetRayGenerationProgram( context, entry_point_index, program );
}

inline RTresult RTContextGetRayGenerationProgram( RTcontext context, unsigned int entry_point_index, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetRayGenerationProgram );
  return _NVIRTFunctions.RTContextGetRayGenerationProgram( context, entry_point_index, program );
}

inline RTresult RTContextSetExceptionProgram( RTcontext context, unsigned int entry_point_index, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetExceptionProgram );
  return _NVIRTFunctions.RTContextSetExceptionProgram( context, entry_point_index, program );
}

inline RTresult RTContextGetExceptionProgram( RTcontext context, unsigned int entry_point_index, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetExceptionProgram );
  return _NVIRTFunctions.RTContextGetExceptionProgram( context, entry_point_index, program );
}

inline RTresult RTContextSetExceptionEnabled( RTcontext context, RTexception exception, int  enabled  )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetExceptionEnabled );
  return _NVIRTFunctions.RTContextSetExceptionEnabled( context, exception, enabled );
}

inline RTresult RTContextGetExceptionEnabled( RTcontext context, RTexception exception, int* enabled  )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetExceptionEnabled );
  return _NVIRTFunctions.RTContextGetExceptionEnabled( context, exception, enabled );
}

inline RTresult RTContextSetRayTypeCount( RTcontext context, unsigned int  num_ray_types )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetRayTypeCount );
  return _NVIRTFunctions.RTContextSetRayTypeCount( context, num_ray_types );
}

inline RTresult RTContextGetRayTypeCount( RTcontext context, unsigned int* num_ray_types )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetRayTypeCount );
  return _NVIRTFunctions.RTContextGetRayTypeCount( context, num_ray_types );
}

inline RTresult RTContextSetMissProgram( RTcontext context, unsigned int ray_type_index, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetMissProgram );
  return _NVIRTFunctions.RTContextSetMissProgram( context, ray_type_index, program );
}

inline RTresult RTContextGetMissProgram( RTcontext context, unsigned int ray_type_index, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetMissProgram );
  return _NVIRTFunctions.RTContextGetMissProgram( context, ray_type_index, program );
}

inline RTresult RTContextCompile( RTcontext context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextCompile );
  return _NVIRTFunctions.RTContextCompile( context );
}

inline RTresult RTContextLaunch1D( RTcontext context, unsigned int entry_point_index, RTsize image_width )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextLaunch1D );
  return _NVIRTFunctions.RTContextLaunch1D( context, entry_point_index, image_width );
}

inline RTresult RTContextLaunch2D( RTcontext context, unsigned int entry_point_index, RTsize image_width, RTsize image_height )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextLaunch2D );
  return _NVIRTFunctions.RTContextLaunch2D( context, entry_point_index, image_width, image_height );
}

inline RTresult RTContextLaunch3D( RTcontext context, unsigned int entry_point_index, RTsize image_width, RTsize image_height, RTsize image_depth )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextLaunch3D );
  return _NVIRTFunctions.RTContextLaunch3D( context, entry_point_index, image_width, image_height, image_depth );
}

inline RTresult RTContextGetRunningState( RTcontext context, int* running )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetRunningState );
  return _NVIRTFunctions.RTContextGetRunningState( context, running );
}

inline RTresult RTContextSetPrintEnabled( RTcontext context, int  enabled )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetPrintEnabled );
  return _NVIRTFunctions.RTContextSetPrintEnabled( context, enabled );
}

inline RTresult RTContextGetPrintEnabled( RTcontext context, int* enabled )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetPrintEnabled );
  return _NVIRTFunctions.RTContextGetPrintEnabled( context, enabled );
}

inline RTresult RTContextSetPrintBufferSize( RTcontext context, RTsize  buffer_size_bytes )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetPrintBufferSize );
  return _NVIRTFunctions.RTContextSetPrintBufferSize( context, buffer_size_bytes );
}

inline RTresult RTContextGetPrintBufferSize( RTcontext context, RTsize* buffer_size_bytes )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetPrintBufferSize );
  return _NVIRTFunctions.RTContextGetPrintBufferSize( context, buffer_size_bytes );
}

inline RTresult RTContextSetPrintLaunchIndex( RTcontext context, int  x, int  y, int  z )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextSetPrintLaunchIndex );
  return _NVIRTFunctions.RTContextSetPrintLaunchIndex( context, x, y, z );
}

inline RTresult RTContextGetPrintLaunchIndex( RTcontext context, int* x, int* y, int* z )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetPrintLaunchIndex );
  return _NVIRTFunctions.RTContextGetPrintLaunchIndex( context, x, y, z );
}

inline RTresult RTContextDeclareVariable( RTcontext context, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextDeclareVariable );
  return _NVIRTFunctions.RTContextDeclareVariable( context, name, v );
}

inline RTresult RTContextQueryVariable( RTcontext context, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextQueryVariable );
  return _NVIRTFunctions.RTContextQueryVariable( context, name, v );
}

inline RTresult RTContextRemoveVariable( RTcontext context, RTvariable v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextRemoveVariable );
  return _NVIRTFunctions.RTContextRemoveVariable( context, v );
}

inline RTresult RTContextGetVariableCount( RTcontext context, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetVariableCount );
  return _NVIRTFunctions.RTContextGetVariableCount( context, count );
}

inline RTresult RTContextGetVariable( RTcontext context, unsigned int index, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTContextGetVariable );
  return _NVIRTFunctions.RTContextGetVariable( context, index, v );
}

inline RTresult RTProgramCreateFromPTXString( RTcontext context, const char* ptx, const char* program_name, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramCreateFromPTXString );
  return _NVIRTFunctions.RTProgramCreateFromPTXString( context, ptx, program_name, program );
}

inline RTresult RTProgramCreateFromPTXFile( RTcontext context, const char* filename, const char* program_name, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramCreateFromPTXFile );
  return _NVIRTFunctions.RTProgramCreateFromPTXFile( context, filename, program_name, program );
}

inline RTresult RTProgramDestroy( RTprogram program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramDestroy );
  return _NVIRTFunctions.RTProgramDestroy( program );
}

inline RTresult RTProgramValidate( RTprogram program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramValidate );
  return _NVIRTFunctions.RTProgramValidate( program );
}

inline RTresult RTProgramGetContext( RTprogram program, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramGetContext );
  return _NVIRTFunctions.RTProgramGetContext( program, context );
}

inline RTresult RTProgramDeclareVariable( RTprogram program, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramDeclareVariable );
  return _NVIRTFunctions.RTProgramDeclareVariable( program, name, v );
}

inline RTresult RTProgramQueryVariable( RTprogram program, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramQueryVariable );
  return _NVIRTFunctions.RTProgramQueryVariable( program, name, v );
}

inline RTresult RTProgramRemoveVariable( RTprogram program, RTvariable v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramRemoveVariable );
  return _NVIRTFunctions.RTProgramRemoveVariable( program, v );
}

inline RTresult RTProgramGetVariableCount( RTprogram program, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramGetVariableCount );
  return _NVIRTFunctions.RTProgramGetVariableCount( program, count );
}

inline RTresult RTProgramGetVariable( RTprogram program, unsigned int index, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTProgramGetVariable );
  return _NVIRTFunctions.RTProgramGetVariable( program, index, v );
}

inline RTresult RTGroupCreate( RTcontext context, RTgroup* group )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupCreate );
  return _NVIRTFunctions.RTGroupCreate( context, group );
}

inline RTresult RTGroupDestroy( RTgroup group )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupDestroy );
  return _NVIRTFunctions.RTGroupDestroy( group );
}

inline RTresult RTGroupValidate( RTgroup group )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupValidate );
  return _NVIRTFunctions.RTGroupValidate( group );
}

inline RTresult RTGroupGetContext( RTgroup group, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupGetContext );
  return _NVIRTFunctions.RTGroupGetContext( group, context );
}

inline RTresult RTGroupSetAcceleration( RTgroup group, RTacceleration  acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupSetAcceleration );
  return _NVIRTFunctions.RTGroupSetAcceleration( group, acceleration );
}

inline RTresult RTGroupGetAcceleration( RTgroup group, RTacceleration* acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupGetAcceleration );
  return _NVIRTFunctions.RTGroupGetAcceleration( group, acceleration );
}

inline RTresult RTGroupSetChildCount( RTgroup group, unsigned int  count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupSetChildCount );
  return _NVIRTFunctions.RTGroupSetChildCount( group, count );
}

inline RTresult RTGroupGetChildCount( RTgroup group, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupGetChildCount );
  return _NVIRTFunctions.RTGroupGetChildCount( group, count );
}

inline RTresult RTGroupSetChild( RTgroup group, unsigned int index, RTobject  child )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupSetChild );
  return _NVIRTFunctions.RTGroupSetChild( group, index, child );
}

inline RTresult RTGroupGetChild( RTgroup group, unsigned int index, RTobject* child )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupGetChild );
  return _NVIRTFunctions.RTGroupGetChild( group, index, child );
}

inline RTresult RTGroupGetChildType( RTgroup group, unsigned int index, RTobjecttype* type )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGroupGetChildType );
  return _NVIRTFunctions.RTGroupGetChildType( group, index, type );
}

inline RTresult RTSelectorCreate( RTcontext context, RTselector* selector )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorCreate );
  return _NVIRTFunctions.RTSelectorCreate( context, selector );
}

inline RTresult RTSelectorDestroy( RTselector selector )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorDestroy );
  return _NVIRTFunctions.RTSelectorDestroy( selector );
}

inline RTresult RTSelectorValidate( RTselector selector )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorValidate );
  return _NVIRTFunctions.RTSelectorValidate( selector );
}

inline RTresult RTSelectorGetContext( RTselector selector, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorGetContext );
  return _NVIRTFunctions.RTSelectorGetContext( selector, context );
}

inline RTresult RTSelectorSetVisitProgram( RTselector selector, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorSetVisitProgram );
  return _NVIRTFunctions.RTSelectorSetVisitProgram( selector, program );
}

inline RTresult RTSelectorGetVisitProgram( RTselector selector, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorGetVisitProgram );
  return _NVIRTFunctions.RTSelectorGetVisitProgram( selector, program );
}

inline RTresult RTSelectorSetChildCount( RTselector selector, unsigned int  count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorSetChildCount );
  return _NVIRTFunctions.RTSelectorSetChildCount( selector, count );
}

inline RTresult RTSelectorGetChildCount( RTselector selector, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorGetChildCount );
  return _NVIRTFunctions.RTSelectorGetChildCount( selector, count );
}

inline RTresult RTSelectorSetChild( RTselector selector, unsigned int index, RTobject  child )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorSetChild );
  return _NVIRTFunctions.RTSelectorSetChild( selector, index, child );
}

inline RTresult RTSelectorGetChild( RTselector selector, unsigned int index, RTobject* child )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorGetChild );
  return _NVIRTFunctions.RTSelectorGetChild( selector, index, child );
}

inline RTresult RTSelectorGetChildType( RTselector selector, unsigned int index, RTobjecttype* type )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorGetChildType );
  return _NVIRTFunctions.RTSelectorGetChildType( selector, index, type );
}

inline RTresult RTSelectorDeclareVariable( RTselector selector, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorDeclareVariable );
  return _NVIRTFunctions.RTSelectorDeclareVariable( selector, name, v );
}

inline RTresult RTSelectorQueryVariable( RTselector selector, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorQueryVariable );
  return _NVIRTFunctions.RTSelectorQueryVariable( selector, name, v );
}

inline RTresult RTSelectorRemoveVariable( RTselector selector, RTvariable v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorRemoveVariable );
  return _NVIRTFunctions.RTSelectorRemoveVariable( selector, v );
}

inline RTresult RTSelectorGetVariableCount( RTselector selector, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorGetVariableCount );
  return _NVIRTFunctions.RTSelectorGetVariableCount( selector, count );
}

inline RTresult RTSelectorGetVariable( RTselector selector, unsigned int index, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTSelectorGetVariable );
  return _NVIRTFunctions.RTSelectorGetVariable( selector, index, v );
}

inline RTresult RTTransformCreate( RTcontext context, RTtransform* transform )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformCreate );
  return _NVIRTFunctions.RTTransformCreate( context, transform );
}

inline RTresult RTTransformDestroy( RTtransform transform )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformDestroy );
  return _NVIRTFunctions.RTTransformDestroy( transform );
}

inline RTresult RTTransformValidate( RTtransform transform )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformValidate );
  return _NVIRTFunctions.RTTransformValidate( transform );
}

inline RTresult RTTransformGetContext( RTtransform transform, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformGetContext );
  return _NVIRTFunctions.RTTransformGetContext( transform, context );
}

inline RTresult RTTransformSetMatrix( RTtransform transform, int transpose, const float* matrix, const float* inverse_matrix )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformSetMatrix );
  return _NVIRTFunctions.RTTransformSetMatrix( transform, transpose, matrix, inverse_matrix );
}

inline RTresult RTTransformGetMatrix( RTtransform transform, int transpose, float* matrix, float* inverse_matrix )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformGetMatrix );
  return _NVIRTFunctions.RTTransformGetMatrix( transform, transpose, matrix, inverse_matrix );
}

inline RTresult RTTransformSetChild( RTtransform transform, RTobject  child )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformSetChild );
  return _NVIRTFunctions.RTTransformSetChild( transform, child );
}

inline RTresult RTTransformGetChild( RTtransform transform, RTobject* child )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformGetChild );
  return _NVIRTFunctions.RTTransformGetChild( transform, child );
}

inline RTresult RTTransformGetChildType( RTtransform transform, RTobjecttype* type )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTransformGetChildType );
  return _NVIRTFunctions.RTTransformGetChildType( transform, type );
}

inline RTresult RTGeometryGroupCreate( RTcontext context, RTgeometrygroup* geometrygroup )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupCreate );
  return _NVIRTFunctions.RTGeometryGroupCreate( context, geometrygroup );
}

inline RTresult RTGeometryGroupDestroy( RTgeometrygroup geometrygroup )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupDestroy );
  return _NVIRTFunctions.RTGeometryGroupDestroy( geometrygroup );
}

inline RTresult RTGeometryGroupValidate( RTgeometrygroup geometrygroup )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupValidate );
  return _NVIRTFunctions.RTGeometryGroupValidate( geometrygroup );
}

inline RTresult RTGeometryGroupGetContext( RTgeometrygroup geometrygroup, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupGetContext );
  return _NVIRTFunctions.RTGeometryGroupGetContext( geometrygroup, context );
}

inline RTresult RTGeometryGroupSetAcceleration( RTgeometrygroup geometrygroup, RTacceleration  acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupSetAcceleration );
  return _NVIRTFunctions.RTGeometryGroupSetAcceleration( geometrygroup, acceleration );
}

inline RTresult RTGeometryGroupGetAcceleration( RTgeometrygroup geometrygroup, RTacceleration* acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupGetAcceleration );
  return _NVIRTFunctions.RTGeometryGroupGetAcceleration( geometrygroup, acceleration );
}

inline RTresult RTGeometryGroupSetChildCount( RTgeometrygroup geometrygroup, unsigned int  count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupSetChildCount );
  return _NVIRTFunctions.RTGeometryGroupSetChildCount( geometrygroup, count );
}

inline RTresult RTGeometryGroupGetChildCount( RTgeometrygroup geometrygroup, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupGetChildCount );
  return _NVIRTFunctions.RTGeometryGroupGetChildCount( geometrygroup, count );
}

inline RTresult RTGeometryGroupSetChild( RTgeometrygroup geometrygroup, unsigned int index, RTgeometryinstance  geometryinstance )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupSetChild );
  return _NVIRTFunctions.RTGeometryGroupSetChild( geometrygroup, index, geometryinstance );
}

inline RTresult RTGeometryGroupGetChild( RTgeometrygroup geometrygroup, unsigned int index, RTgeometryinstance* geometryinstance )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGroupGetChild );
  return _NVIRTFunctions.RTGeometryGroupGetChild( geometrygroup, index, geometryinstance );
}

inline RTresult RTAccelerationCreate( RTcontext context, RTacceleration* acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationCreate );
  return _NVIRTFunctions.RTAccelerationCreate( context, acceleration );
}

inline RTresult RTAccelerationDestroy( RTacceleration acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationDestroy );
  return _NVIRTFunctions.RTAccelerationDestroy( acceleration );
}

inline RTresult RTAccelerationValidate( RTacceleration acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationValidate );
  return _NVIRTFunctions.RTAccelerationValidate( acceleration );
}

inline RTresult RTAccelerationGetContext( RTacceleration acceleration, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationGetContext );
  return _NVIRTFunctions.RTAccelerationGetContext( acceleration, context );
}

inline RTresult RTAccelerationSetBuilder( RTacceleration acceleration, const char* builder )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationSetBuilder );
  return _NVIRTFunctions.RTAccelerationSetBuilder( acceleration, builder );
}

inline RTresult RTAccelerationGetBuilder( RTacceleration acceleration, const char** return_string )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationGetBuilder );
  return _NVIRTFunctions.RTAccelerationGetBuilder( acceleration, return_string );
}

inline RTresult RTAccelerationSetTraverser( RTacceleration acceleration, const char* traverser )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationSetTraverser );
  return _NVIRTFunctions.RTAccelerationSetTraverser( acceleration, traverser );
}

inline RTresult RTAccelerationGetTraverser( RTacceleration acceleration, const char** return_string )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationGetTraverser );
  return _NVIRTFunctions.RTAccelerationGetTraverser( acceleration, return_string );
}

inline RTresult RTAccelerationSetProperty( RTacceleration acceleration, const char* name, const char* value )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationSetProperty );
  return _NVIRTFunctions.RTAccelerationSetProperty( acceleration, name, value );
}

inline RTresult RTAccelerationGetProperty( RTacceleration acceleration, const char* name, const char** return_string )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationGetProperty );
  return _NVIRTFunctions.RTAccelerationGetProperty( acceleration, name, return_string );
}

inline RTresult RTAccelerationGetDataSize( RTacceleration acceleration, RTsize* size )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationGetDataSize );
  return _NVIRTFunctions.RTAccelerationGetDataSize( acceleration, size );
}

inline RTresult RTAccelerationGetData( RTacceleration acceleration, void* data )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationGetData );
  return _NVIRTFunctions.RTAccelerationGetData( acceleration, data );
}

inline RTresult RTAccelerationSetData( RTacceleration acceleration, const void* data, RTsize size )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationSetData );
  return _NVIRTFunctions.RTAccelerationSetData( acceleration, data, size );
}

inline RTresult RTAccelerationMarkDirty( RTacceleration acceleration )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationMarkDirty );
  return _NVIRTFunctions.RTAccelerationMarkDirty( acceleration );
}

inline RTresult RTAccelerationIsDirty( RTacceleration acceleration, int* dirty )
{
  NVSG_ASSERT( _NVIRTFunctions.RTAccelerationIsDirty );
  return _NVIRTFunctions.RTAccelerationIsDirty( acceleration, dirty );
}

inline RTresult RTGeometryInstanceCreate( RTcontext context, RTgeometryinstance* geometryinstance )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceCreate );
  return _NVIRTFunctions.RTGeometryInstanceCreate( context, geometryinstance );
}

inline RTresult RTGeometryInstanceDestroy( RTgeometryinstance geometryinstance )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceDestroy );
  return _NVIRTFunctions.RTGeometryInstanceDestroy( geometryinstance );
}

inline RTresult RTGeometryInstanceValidate( RTgeometryinstance geometryinstance )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceValidate );
  return _NVIRTFunctions.RTGeometryInstanceValidate( geometryinstance );
}

inline RTresult RTGeometryInstanceGetContext( RTgeometryinstance geometryinstance, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceGetContext );
  return _NVIRTFunctions.RTGeometryInstanceGetContext( geometryinstance, context );
}

inline RTresult RTGeometryInstanceSetGeometry( RTgeometryinstance geometryinstance, RTgeometry  geometry )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceSetGeometry );
  return _NVIRTFunctions.RTGeometryInstanceSetGeometry( geometryinstance, geometry );
}

inline RTresult RTGeometryInstanceGetGeometry( RTgeometryinstance geometryinstance, RTgeometry* geometry )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceGetGeometry );
  return _NVIRTFunctions.RTGeometryInstanceGetGeometry( geometryinstance, geometry );
}

inline RTresult RTGeometryInstanceSetMaterialCount( RTgeometryinstance geometryinstance, unsigned int  count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceSetMaterialCount );
  return _NVIRTFunctions.RTGeometryInstanceSetMaterialCount( geometryinstance, count );
}

inline RTresult RTGeometryInstanceGetMaterialCount( RTgeometryinstance geometryinstance, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceGetMaterialCount );
  return _NVIRTFunctions.RTGeometryInstanceGetMaterialCount( geometryinstance, count );
}

inline RTresult RTGeometryInstanceSetMaterial( RTgeometryinstance geometryinstance, unsigned int idx, RTmaterial  material )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceSetMaterial );
  return _NVIRTFunctions.RTGeometryInstanceSetMaterial( geometryinstance, idx, material );
}

inline RTresult RTGeometryInstanceGetMaterial( RTgeometryinstance geometryinstance, unsigned int idx, RTmaterial* material )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceGetMaterial );
  return _NVIRTFunctions.RTGeometryInstanceGetMaterial( geometryinstance, idx, material );
}

inline RTresult RTGeometryInstanceDeclareVariable( RTgeometryinstance geometryinstance, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceDeclareVariable );
  return _NVIRTFunctions.RTGeometryInstanceDeclareVariable( geometryinstance, name, v );
}

inline RTresult RTGeometryInstanceQueryVariable( RTgeometryinstance geometryinstance, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceQueryVariable );
  return _NVIRTFunctions.RTGeometryInstanceQueryVariable( geometryinstance, name, v );
}

inline RTresult RTGeometryInstanceRemoveVariable( RTgeometryinstance geometryinstance, RTvariable v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceRemoveVariable );
  return _NVIRTFunctions.RTGeometryInstanceRemoveVariable( geometryinstance, v );
}

inline RTresult RTGeometryInstanceGetVariableCount( RTgeometryinstance geometryinstance, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceGetVariableCount );
  return _NVIRTFunctions.RTGeometryInstanceGetVariableCount( geometryinstance, count );
}

inline RTresult RTGeometryInstanceGetVariable( RTgeometryinstance geometryinstance, unsigned int index, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryInstanceGetVariable );
  return _NVIRTFunctions.RTGeometryInstanceGetVariable( geometryinstance, index, v );
}

inline RTresult RTGeometryCreate( RTcontext context, RTgeometry* geometry )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryCreate );
  return _NVIRTFunctions.RTGeometryCreate( context, geometry );
}

inline RTresult RTGeometryDestroy( RTgeometry geometry )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryDestroy );
  return _NVIRTFunctions.RTGeometryDestroy( geometry );
}

inline RTresult RTGeometryValidate( RTgeometry geometry )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryValidate );
  return _NVIRTFunctions.RTGeometryValidate( geometry );
}

inline RTresult RTGeometryGetContext( RTgeometry geometry, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGetContext );
  return _NVIRTFunctions.RTGeometryGetContext( geometry, context );
}

inline RTresult RTGeometrySetPrimitiveCount( RTgeometry geometry, unsigned int  num_primitives )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometrySetPrimitiveCount );
  return _NVIRTFunctions.RTGeometrySetPrimitiveCount( geometry, num_primitives );
}

inline RTresult RTGeometryGetPrimitiveCount( RTgeometry geometry, unsigned int* num_primitives )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGetPrimitiveCount );
  return _NVIRTFunctions.RTGeometryGetPrimitiveCount( geometry, num_primitives );
}

inline RTresult RTGeometrySetBoundingBoxProgram( RTgeometry geometry, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometrySetBoundingBoxProgram );
  return _NVIRTFunctions.RTGeometrySetBoundingBoxProgram( geometry, program );
}

inline RTresult RTGeometryGetBoundingBoxProgram( RTgeometry geometry, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGetBoundingBoxProgram );
  return _NVIRTFunctions.RTGeometryGetBoundingBoxProgram( geometry, program );
}

inline RTresult RTGeometrySetIntersectionProgram( RTgeometry geometry, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometrySetIntersectionProgram );
  return _NVIRTFunctions.RTGeometrySetIntersectionProgram( geometry, program );
}

inline RTresult RTGeometryGetIntersectionProgram( RTgeometry geometry, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGetIntersectionProgram );
  return _NVIRTFunctions.RTGeometryGetIntersectionProgram( geometry, program );
}

inline RTresult RTGeometryMarkDirty( RTgeometry geometry )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryMarkDirty );
  return _NVIRTFunctions.RTGeometryMarkDirty( geometry );
}

inline RTresult RTGeometryIsDirty( RTgeometry geometry, int* dirty )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryIsDirty );
  return _NVIRTFunctions.RTGeometryIsDirty( geometry, dirty );
}

inline RTresult RTGeometryDeclareVariable( RTgeometry geometry, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryDeclareVariable );
  return _NVIRTFunctions.RTGeometryDeclareVariable( geometry, name, v );
}

inline RTresult RTGeometryQueryVariable( RTgeometry geometry, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryQueryVariable );
  return _NVIRTFunctions.RTGeometryQueryVariable( geometry, name, v );
}

inline RTresult RTGeometryRemoveVariable( RTgeometry geometry, RTvariable v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryRemoveVariable );
  return _NVIRTFunctions.RTGeometryRemoveVariable( geometry, v );
}

inline RTresult RTGeometryGetVariableCount( RTgeometry geometry, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGetVariableCount );
  return _NVIRTFunctions.RTGeometryGetVariableCount( geometry, count );
}

inline RTresult RTGeometryGetVariable( RTgeometry geometry, unsigned int index, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTGeometryGetVariable );
  return _NVIRTFunctions.RTGeometryGetVariable( geometry, index, v );
}

inline RTresult RTMaterialCreate( RTcontext context, RTmaterial* material )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialCreate );
  return _NVIRTFunctions.RTMaterialCreate( context, material );
}

inline RTresult RTMaterialDestroy( RTmaterial material )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialDestroy );
  return _NVIRTFunctions.RTMaterialDestroy( material );
}

inline RTresult RTMaterialValidate( RTmaterial material )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialValidate );
  return _NVIRTFunctions.RTMaterialValidate( material );
}

inline RTresult RTMaterialGetContext( RTmaterial material, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialGetContext );
  return _NVIRTFunctions.RTMaterialGetContext( material, context );
}

inline RTresult RTMaterialSetClosestHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialSetClosestHitProgram );
  return _NVIRTFunctions.RTMaterialSetClosestHitProgram( material, ray_type_index, program );
}

inline RTresult RTMaterialGetClosestHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialGetClosestHitProgram );
  return _NVIRTFunctions.RTMaterialGetClosestHitProgram( material, ray_type_index, program );
}

inline RTresult RTMaterialSetAnyHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram  program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialSetAnyHitProgram );
  return _NVIRTFunctions.RTMaterialSetAnyHitProgram( material, ray_type_index, program );
}

inline RTresult RTMaterialGetAnyHitProgram( RTmaterial material, unsigned int ray_type_index, RTprogram* program )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialGetAnyHitProgram );
  return _NVIRTFunctions.RTMaterialGetAnyHitProgram( material, ray_type_index, program );
}

inline RTresult RTMaterialDeclareVariable( RTmaterial material, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialDeclareVariable );
  return _NVIRTFunctions.RTMaterialDeclareVariable( material, name, v );
}

inline RTresult RTMaterialQueryVariable( RTmaterial material, const char* name, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialQueryVariable );
  return _NVIRTFunctions.RTMaterialQueryVariable( material, name, v );
}

inline RTresult RTMaterialRemoveVariable( RTmaterial material, RTvariable v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialRemoveVariable );
  return _NVIRTFunctions.RTMaterialRemoveVariable( material, v );
}

inline RTresult RTMaterialGetVariableCount( RTmaterial material, unsigned int* count )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialGetVariableCount );
  return _NVIRTFunctions.RTMaterialGetVariableCount( material, count );
}

inline RTresult RTMaterialGetVariable( RTmaterial material, unsigned int index, RTvariable* v )
{
  NVSG_ASSERT( _NVIRTFunctions.RTMaterialGetVariable );
  return _NVIRTFunctions.RTMaterialGetVariable( material, index, v );
}

inline RTresult RTTextureSamplerCreate( RTcontext context, RTtexturesampler* texturesampler )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerCreate );
  return _NVIRTFunctions.RTTextureSamplerCreate( context, texturesampler );
}

inline RTresult RTTextureSamplerDestroy( RTtexturesampler texturesampler )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerDestroy );
  return _NVIRTFunctions.RTTextureSamplerDestroy( texturesampler );
}

inline RTresult RTTextureSamplerValidate( RTtexturesampler texturesampler )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerValidate );
  return _NVIRTFunctions.RTTextureSamplerValidate( texturesampler );
}

inline RTresult RTTextureSamplerGetContext( RTtexturesampler texturesampler, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetContext );
  return _NVIRTFunctions.RTTextureSamplerGetContext( texturesampler, context );
}

inline RTresult RTTextureSamplerSetMipLevelCount( RTtexturesampler texturesampler, unsigned int  num_mip_levels )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetMipLevelCount );
  return _NVIRTFunctions.RTTextureSamplerSetMipLevelCount( texturesampler, num_mip_levels );
}

inline RTresult RTTextureSamplerGetMipLevelCount( RTtexturesampler texturesampler, unsigned int* num_mip_levels )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetMipLevelCount );
  return _NVIRTFunctions.RTTextureSamplerGetMipLevelCount( texturesampler, num_mip_levels );
}

inline RTresult RTTextureSamplerSetArraySize( RTtexturesampler texturesampler, unsigned int  num_textures_in_array )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetArraySize );
  return _NVIRTFunctions.RTTextureSamplerSetArraySize( texturesampler, num_textures_in_array );
}

inline RTresult RTTextureSamplerGetArraySize( RTtexturesampler texturesampler, unsigned int* num_textures_in_array )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetArraySize );
  return _NVIRTFunctions.RTTextureSamplerGetArraySize( texturesampler, num_textures_in_array );
}

inline RTresult RTTextureSamplerSetWrapMode( RTtexturesampler texturesampler, unsigned int dimension, RTwrapmode wrapmode )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetWrapMode );
  return _NVIRTFunctions.RTTextureSamplerSetWrapMode( texturesampler, dimension, wrapmode );
}

inline RTresult RTTextureSamplerGetWrapMode( RTtexturesampler texturesampler, unsigned int dimension, RTwrapmode* wrapmode )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetWrapMode );
  return _NVIRTFunctions.RTTextureSamplerGetWrapMode( texturesampler, dimension, wrapmode );
}

inline RTresult RTTextureSamplerSetFilteringModes( RTtexturesampler texturesampler, RTfiltermode  minification, RTfiltermode  magnification, RTfiltermode  mipmapping )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetFilteringModes );
  return _NVIRTFunctions.RTTextureSamplerSetFilteringModes( texturesampler, minification, magnification, mipmapping );
}

inline RTresult RTTextureSamplerGetFilteringModes( RTtexturesampler texturesampler, RTfiltermode* minification, RTfiltermode* magnification, RTfiltermode* mipmapping )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetFilteringModes );
  return _NVIRTFunctions.RTTextureSamplerGetFilteringModes( texturesampler, minification, magnification, mipmapping );
}

inline RTresult RTTextureSamplerSetMaxAnisotropy( RTtexturesampler texturesampler, float value )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetMaxAnisotropy );
  return _NVIRTFunctions.RTTextureSamplerSetMaxAnisotropy( texturesampler, value );
}

inline RTresult RTTextureSamplerGetMaxAnisotropy( RTtexturesampler texturesampler, float* value )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetMaxAnisotropy );
  return _NVIRTFunctions.RTTextureSamplerGetMaxAnisotropy( texturesampler, value );
}

inline RTresult RTTextureSamplerSetReadMode( RTtexturesampler texturesampler, RTtexturereadmode  readmode )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetReadMode );
  return _NVIRTFunctions.RTTextureSamplerSetReadMode( texturesampler, readmode );
}

inline RTresult RTTextureSamplerGetReadMode( RTtexturesampler texturesampler, RTtexturereadmode* readmode )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetReadMode );
  return _NVIRTFunctions.RTTextureSamplerGetReadMode( texturesampler, readmode );
}

inline RTresult RTTextureSamplerSetIndexingMode( RTtexturesampler texturesampler, RTtextureindexmode  indexmode )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetIndexingMode );
  return _NVIRTFunctions.RTTextureSamplerSetIndexingMode( texturesampler, indexmode );
}

inline RTresult RTTextureSamplerGetIndexingMode( RTtexturesampler texturesampler, RTtextureindexmode* indexmode )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetIndexingMode );
  return _NVIRTFunctions.RTTextureSamplerGetIndexingMode( texturesampler, indexmode );
}

inline RTresult RTTextureSamplerSetBuffer( RTtexturesampler texturesampler, unsigned int texture_array_idx, unsigned int mip_level, RTbuffer  buffer )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerSetBuffer );
  return _NVIRTFunctions.RTTextureSamplerSetBuffer( texturesampler, texture_array_idx, mip_level, buffer );
}

inline RTresult RTTextureSamplerGetBuffer( RTtexturesampler texturesampler, unsigned int texture_array_idx, unsigned int mip_level, RTbuffer* buffer )
{
  NVSG_ASSERT( _NVIRTFunctions.RTTextureSamplerGetBuffer );
  return _NVIRTFunctions.RTTextureSamplerGetBuffer( texturesampler, texture_array_idx, mip_level, buffer );
}

inline RTresult RTBufferCreate( RTcontext context, unsigned int bufferdesc, RTbuffer* buffer )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferCreate );
  return _NVIRTFunctions.RTBufferCreate( context, bufferdesc, buffer );
}

inline RTresult RTBufferDestroy( RTbuffer buffer )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferDestroy );
  return _NVIRTFunctions.RTBufferDestroy( buffer );
}

inline RTresult RTBufferValidate( RTbuffer buffer )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferValidate );
  return _NVIRTFunctions.RTBufferValidate( buffer );
}

inline RTresult RTBufferGetContext( RTbuffer buffer, RTcontext* context )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetContext );
  return _NVIRTFunctions.RTBufferGetContext( buffer, context );
}

inline RTresult RTBufferSetFormat( RTbuffer buffer, RTformat  format )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferSetFormat );
  return _NVIRTFunctions.RTBufferSetFormat( buffer, format );
}

inline RTresult RTBufferGetFormat( RTbuffer buffer, RTformat* format )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetFormat );
  return _NVIRTFunctions.RTBufferGetFormat( buffer, format );
}

inline RTresult RTBufferSetElementSize( RTbuffer buffer, RTsize  size_of_element )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferSetElementSize );
  return _NVIRTFunctions.RTBufferSetElementSize( buffer, size_of_element );
}

inline RTresult RTBufferGetElementSize( RTbuffer buffer, RTsize* size_of_element )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetElementSize );
  return _NVIRTFunctions.RTBufferGetElementSize( buffer, size_of_element );
}

inline RTresult RTBufferSetSize1D( RTbuffer buffer, RTsize  width )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferSetSize1D );
  return _NVIRTFunctions.RTBufferSetSize1D( buffer, width );
}

inline RTresult RTBufferGetSize1D( RTbuffer buffer, RTsize* width )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetSize1D );
  return _NVIRTFunctions.RTBufferGetSize1D( buffer, width );
}

inline RTresult RTBufferSetSize2D( RTbuffer buffer, RTsize  width, RTsize  height )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferSetSize2D );
  return _NVIRTFunctions.RTBufferSetSize2D( buffer, width, height );
}

inline RTresult RTBufferGetSize2D( RTbuffer buffer, RTsize* width, RTsize* height )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetSize2D );
  return _NVIRTFunctions.RTBufferGetSize2D( buffer, width, height );
}

inline RTresult RTBufferSetSize3D( RTbuffer buffer, RTsize  width, RTsize  height, RTsize  depth )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferSetSize3D );
  return _NVIRTFunctions.RTBufferSetSize3D( buffer, width, height, depth );
}

inline RTresult RTBufferGetSize3D( RTbuffer buffer, RTsize* width, RTsize* height, RTsize* depth )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetSize3D );
  return _NVIRTFunctions.RTBufferGetSize3D( buffer, width, height, depth );
}

inline RTresult RTBufferSetSizev( RTbuffer buffer, unsigned int dimensionality, const RTsize* dims )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferSetSizev );
  return _NVIRTFunctions.RTBufferSetSizev( buffer, dimensionality, dims );
}

inline RTresult RTBufferGetSizev( RTbuffer buffer, unsigned int dimensionality,       RTsize* dims )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetSizev );
  return _NVIRTFunctions.RTBufferGetSizev( buffer, dimensionality, dims );
}

inline RTresult RTBufferGetDimensionality( RTbuffer buffer, unsigned int* dimensionality )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferGetDimensionality );
  return _NVIRTFunctions.RTBufferGetDimensionality( buffer, dimensionality );
}

inline RTresult RTBufferMap( RTbuffer buffer, void** user_pointer )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferMap );
  return _NVIRTFunctions.RTBufferMap( buffer, user_pointer );
}

inline RTresult RTBufferUnmap( RTbuffer buffer )
{
  NVSG_ASSERT( _NVIRTFunctions.RTBufferUnmap );
  return _NVIRTFunctions.RTBufferUnmap( buffer );
}


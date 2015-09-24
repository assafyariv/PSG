// Copyright NVIDIA Corporation 2005-2008
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES

#pragma once
/** @file */

#include "Cg/cg.h"
#include "nvutil/SmartPtr.h"
#include "nvutil/Assert.h"

#if !defined(X86_64) || defined(NVSG_DISABLE_CG_MULTIPLEX)

// Types
#define CGMcontext CGcontext
#define CGMeffect CGeffect
#define CGMparameter CGparameter
#define CGMannotation CGannotation
#define CGMtechnique CGtechnique
#define CGMstateassignment CGstateassignment
#define CGMprogram CGprogram
#define CGMpass CGpass
#define CGMstate CGstate
#define CGMprofile CGprofile
#define CGMbuffer CGbuffer

namespace nvsg
{
// Special context initialization functions
inline CGcontext cgfxCreateContext(int)
{
  return cgCreateContext();
}

// Thread local access to GPU affiliated object.
template <typename T>
inline T cgfxGetAffiliate(T &obj)
{
  return obj;
}

inline size_t cgfxGetAffiliateIndex()
{
  return 0;
}

}
// Functions
#define cgsGetLastListing cgGetLastListing
#define cgsIsProgram cgIsProgram
#define cgsCompileProgram cgCompileProgram
#define cgsIsProgramCompiled cgIsProgramCompiled
#define cgsGetProgramString cgGetProgramString
#define cgsGetProgramProfile cgGetProgramProfile
#define cgsGetProgramInput cgGetProgramInput
#define cgsGetProgramOutput cgGetProgramOutput
#define cgsGetArrayType cgGetArrayType
#define cgsGetArraySize cgGetArraySize
#define cgsSetArraySize cgSetArraySize
#define cgsIsParameter cgIsParameter
#define cgsGetParameterName cgGetParameterName
#define cgsGetParameterType cgGetParameterType
#define cgsGetParameterClass cgGetParameterClass
#define cgsGetParameterNamedType cgGetParameterNamedType
#define cgsGetParameterSemantic cgGetParameterSemantic
#define cgsGetParameterDirection cgGetParameterDirection
#define cgsIsParameterReferenced cgIsParameterReferenced
#define cgsIsParameterUsed cgIsParameterUsed
#define cgsGetParameterValues cgGetParameterValues
#define cgsGetParameterValuefc cgGetParameterValuefc
#define cgsGetParameterValuefr cgGetParameterValuefr
#define cgsGetParameterValueic cgGetParameterValueic
#define cgsGetStringParameterValue cgGetStringParameterValue
#define cgsSetParameterValuefc cgSetParameterValuefc
#define cgsSetParameterValueic cgSetParameterValueic
#define cgsGetArrayParameter cgGetArrayParameter
#define cgsDestroyParameter cgDestroyParameter
#define cgsConnectParameter cgConnectParameter
#define cgsDisconnectParameter cgDisconnectParameter
#define cgsGetNamedUserType cgGetNamedUserType
#define cgsSetStringParameterValue cgSetStringParameterValue
#define cgsSetMatrixParameterfc cgSetMatrixParameterfc
#define cgsGetTechniqueName cgGetTechniqueName
#define cgsIsTechnique cgIsTechnique
#define cgsValidateTechnique cgValidateTechnique
#define cgsIsTechniqueValidated cgIsTechniqueValidated
#define cgsIsPass cgIsPass
#define cgsGetPassName cgGetPassName
#define cgsSetPassState cgSetPassState
#define cgsResetPassState cgResetPassState
#define cgsCallStateSetCallback cgCallStateSetCallback
#define cgsCallStateValidateCallback cgCallStateValidateCallback
#define cgsCallStateResetCallback cgCallStateResetCallback
#define cgsGetFloatStateAssignmentValues cgGetFloatStateAssignmentValues
#define cgsGetIntStateAssignmentValues cgGetIntStateAssignmentValues
#define cgsGetBoolStateAssignmentValues cgGetBoolStateAssignmentValues
#define cgsGetStringStateAssignmentValue cgGetStringStateAssignmentValue
#define cgsGetStateType cgGetStateType
#define cgsGetStateName cgGetStateName
#define cgsIsAnnotation cgIsAnnotation
#define cgsGetAnnotationName cgGetAnnotationName
#define cgsGetAnnotationType cgGetAnnotationType
#define cgsGetFloatAnnotationValues cgGetFloatAnnotationValues
#define cgsGetIntAnnotationValues cgGetIntAnnotationValues
#define cgsGetStringAnnotationValue cgGetStringAnnotationValue
#define cgsGetBooleanAnnotationValues cgGetBooleanAnnotationValues
#define cgsGetProgramBufferMaxSize cgGetProgramBufferMaxSize
#define cgsGetProgramBufferMaxIndex cgGetProgramBufferMaxIndex
#define cgsGetBufferSize cgGetBufferSize
#define cgsGetParameterResourceSize cgGetParameterResourceSize
#define cgsGetParameterResourceType cgGetParameterResourceType
#define cgsGetParameterBufferIndex cgGetParameterBufferIndex
#define cgsGetParameterBufferOffset cgGetParameterBufferOffset

#define cgmCreateBuffer cgCreateBuffer
#define cgmDestroyBuffer cgDestroyBuffer
#define cgmSetBufferData cgSetBufferData
#define cgmSetBufferSubData cgSetBufferSubData
#define cgmSetProgramBuffer cgSetProgramBuffer
#define cgmDestroyContext cgDestroyContext
#define cgmIsProgram cgIsProgram
#define cgmSetProgramProfile cgSetProgramProfile
#define cgmGetNextParameter cgGetNextParameter
#define cgmGetFirstLeafParameter cgGetFirstLeafParameter
#define cgmGetNextLeafParameter cgGetNextLeafParameter
#define cgmGetFirstStructParameter cgGetFirstStructParameter
#define cgmGetArrayParameter cgGetArrayParameter
#define cgmSetParameterValuefc cgSetParameterValuefc
#define cgmSetParameterValueic cgSetParameterValueic
#define cgmSetStringParameterValue cgSetStringParameterValue
#define cgmCreateEffect cgCreateEffect
#define cgmCreateEffectFromFile cgCreateEffectFromFile
#define cgmDestroyEffect cgDestroyEffect
#define cgmGetFirstTechnique cgGetFirstTechnique
#define cgmGetNextTechnique cgGetNextTechnique
#define cgmGetFirstPass cgGetFirstPass
#define cgmGetNextPass cgGetNextPass
#define cgmGetFirstStateAssignment cgGetFirstStateAssignment
#define cgmGetNextStateAssignment cgGetNextStateAssignment
#define cgmGetProgramStateAssignmentValue cgGetProgramStateAssignmentValue
#define cgmGetTextureStateAssignmentValue cgGetTextureStateAssignmentValue
#define cgmGetStateAssignmentState cgGetStateAssignmentState
#define cgmGetSamplerStateAssignmentState cgGetSamplerStateAssignmentState
#define cgmCreateSamplerStateAssignment cgCreateSamplerStateAssignment
#define cgmGetFirstSamplerStateAssignment cgGetFirstSamplerStateAssignment
#define cgmGetFirstEffectParameter cgGetFirstEffectParameter
#define cgmGetFirstTechniqueAnnotation cgGetFirstTechniqueAnnotation
#define cgmGetFirstPassAnnotation cgGetFirstPassAnnotation
#define cgmGetFirstParameterAnnotation cgGetFirstParameterAnnotation
#define cgmGetNextAnnotation cgGetNextAnnotation
#define cgmCreateSamplerState cgCreateSamplerState
#define cgmSetFloatStateAssignment cgSetFloatStateAssignment
#define cgmSetIntStateAssignment cgSetIntStateAssignment
#define cgmSetBoolStateAssignment cgSetBoolStateAssignment
#define cgmSetFloatArrayStateAssignment cgSetFloatArrayStateAssignment
#define cgmSetIntArrayStateAssignment cgSetIntArrayStateAssignment
#define cgmSetBoolArrayStateAssignment cgSetBoolArrayStateAssignment
#define cgmCreateParameter cgCreateParameter

#else // #ifdef NVSG_DISABLE_CG_MULTIPLEX

//
// Types
//
typedef nvutil::RCVector<CGcontext> CGMcontextVec;
typedef nvutil::SmartPtr<CGMcontextVec> CGMcontext;
typedef nvutil::RCVector<CGeffect> CGMeffectVec;
typedef nvutil::SmartPtr<CGMeffectVec> CGMeffect;
typedef nvutil::RCVector<CGparameter> CGMparameterVec;
typedef nvutil::SmartPtr<CGMparameterVec> CGMparameter;
typedef nvutil::RCVector<CGannotation> CGMannotationVec;
typedef nvutil::SmartPtr<CGMannotationVec> CGMannotation;
typedef nvutil::RCVector<CGtechnique> CGMtechniqueVec;
typedef nvutil::SmartPtr<CGMtechniqueVec> CGMtechnique;
typedef nvutil::RCVector<CGstateassignment> CGMstateassignmentVec;
typedef nvutil::SmartPtr<CGMstateassignmentVec> CGMstateassignment;
typedef nvutil::RCVector<CGprogram> CGMprogramVec;
typedef nvutil::SmartPtr<CGMprogramVec> CGMprogram;
typedef nvutil::RCVector<CGpass> CGMpassVec;
typedef nvutil::SmartPtr<CGMpassVec> CGMpass;
typedef nvutil::RCVector<CGstate> CGMstateVec;
typedef nvutil::SmartPtr<CGMstateVec> CGMstate;
typedef nvutil::RCVector<CGprofile> CGMprofileVec;
typedef nvutil::SmartPtr<CGMprofileVec> CGMprofile;
typedef nvutil::RCVector<CGbuffer> CGMbufferVec;
typedef nvutil::SmartPtr<CGMbufferVec> CGMbuffer;

// Platform dependent vector size_type to eliminate type conversion warnings
// (C4267). Note: This relies on the fact that conversion from
// vector<T1>::size_type to vector<T2>::size_type can be performed without
// compiler warnings
typedef nvutil::RCVector<int>::size_type vecsize_t;

namespace nvsg
{

//
// Special context creation function
//
CGMcontext cgfxCreateContext(int num_gpus);

//
// Thread local access to GPU affiliated object
//
#if defined(_WIN32)
extern NVSG_API DWORD g_cgfxAffiliateTlsKey;
#elif defined(LINUX)
extern pthread_key_t g_cgfxAffiliateTlsKey;
#else
#error Unsupported Operating System!
#endif

inline void cgfxSetAffiliateIndex(size_t index)
{
#if defined(_WIN32)
  NVSG_ASSERT(g_cgfxAffiliateTlsKey != TLS_OUT_OF_INDEXES);
  TlsSetValue(g_cgfxAffiliateTlsKey, (LPVOID)index);
#elif defined(LINUX)
  pthread_setspecific(g_cgfxAffiliateTlsKey, (void*)index);
#else
#error Unsupported Operating System!
#endif
}

inline size_t cgfxGetAffiliateIndex()
{
#if defined(_WIN32)
  NVSG_ASSERT(g_cgfxAffiliateTlsKey != TLS_OUT_OF_INDEXES);
  LPVOID val = TlsGetValue(g_cgfxAffiliateTlsKey);
  return *((size_t*)&val);
#elif defined(LINUX)
  void *val = pthread_getspecific(g_cgfxAffiliateTlsKey);
  return *((size_t*)&val);
#else
#error Unsupported Operating System!
#endif
}

template <typename T>
inline T cgfxGetAffiliate(nvutil::SmartPtr< nvutil::RCVector<T> > cgmVec)
{
  NVSG_ASSERT(cgmVec);

  size_t aff_idx = cgfxGetAffiliateIndex();
  NVSG_ASSERT(aff_idx >= 0 && aff_idx < cgmVec->size());
  return (*cgmVec)[aff_idx];
}

}
//
// Single-cast functions - accept a multiplexed object, 'select' the affiliated
// Cg object and invoke the appropriate Cg function
//

// Context functions
const char * cgsGetLastListing(CGMcontext ctx);

// Program functions
CGbool cgsIsProgram(CGMprogram program);
void cgsCompileProgram(CGMprogram program);
CGbool cgsIsProgramCompiled(CGMprogram program);
const char * cgsGetProgramString(CGMprogram prog, CGenum pname);
CGprofile cgsGetProgramProfile(CGMprogram prog);
CGenum cgsGetProgramInput(CGMprogram program);
CGenum cgsGetProgramOutput(CGMprogram program);

// Parameter functions
CGtype cgsGetArrayType(CGMparameter param);
int cgsGetArraySize(CGMparameter param, int dimension);
void cgsSetArraySize(CGMparameter param, int size);
CGbool cgsIsParameter(CGMparameter param);
const char * cgsGetParameterName(CGMparameter param);
CGtype cgsGetParameterType(CGMparameter param);
CGparameterclass cgsGetParameterClass(CGMparameter param);
CGtype cgsGetParameterNamedType(CGMparameter param);
const char * cgsGetParameterSemantic(CGMparameter param);
CGenum cgsGetParameterDirection(CGMparameter param);
CGbool cgsIsParameterReferenced(CGMparameter param);
CGbool cgsIsParameterUsed(CGMparameter param, CGMeffect);
const double * cgsGetParameterValues(CGMparameter param, CGenum value_type, int *nvalues);
int cgsGetParameterValuefc(CGMparameter param, int n, float *vals);
int cgsGetParameterValuefr(CGMparameter param, int n, float *vals);
int cgsGetParameterValueic(CGMparameter param, int n, int *vals);
const char * cgsGetStringParameterValue(CGMparameter param);
void cgsSetParameterValuefc(CGMparameter param, int n, const float *vals);
void cgsSetParameterValueic(CGMparameter param, int n, const int *vals);
void cgsSetStringParameterValue(CGMparameter param, const char *str);
void cgsSetMatrixParameterfc(CGMparameter param, const float *matrix);

CGparameter cgsGetArrayParameter( CGMparameter aparam, int index );
void cgsDestroyParameter(CGparameter param);
void cgsConnectParameter(CGparameter from, CGparameter to);
void cgsDisconnectParameter(CGparameter param);

// Type Functions
CGtype cgsGetNamedUserType(CGMeffect effect, const char *name);
// Resource Functions
// Enum Functions
// Profile Functions
// Error Functions
// Misc Functions

// CgFX Functions
const char * cgsGetTechniqueName(CGMtechnique);
CGbool cgsIsTechnique(CGMtechnique);
CGbool cgsValidateTechnique(CGMtechnique);
CGbool cgsIsTechniqueValidated(CGMtechnique);
CGbool cgsIsPass(CGMpass);
const char * cgsGetPassName(CGMpass);
void cgsSetPassState(CGMpass);
void cgsResetPassState(CGMpass);
CGbool cgsCallStateSetCallback(CGMstateassignment);
CGbool cgsCallStateValidateCallback(CGMstateassignment);
CGbool cgsCallStateResetCallback(CGMstateassignment);
const float * cgsGetFloatStateAssignmentValues(CGMstateassignment, int *nVals);
const int * cgsGetIntStateAssignmentValues(CGMstateassignment, int *nVals);
const CGbool * cgsGetBoolStateAssignmentValues(CGMstateassignment, int *nVals);
const char * cgsGetStringStateAssignmentValue(CGMstateassignment);
CGtype cgsGetStateType(CGMstate);
const char * cgsGetStateName(CGMstate);
CGbool cgsIsAnnotation(CGMannotation);
const char * cgsGetAnnotationName(CGMannotation);
CGtype cgsGetAnnotationType(CGMannotation);
const float * cgsGetFloatAnnotationValues(CGMannotation, int *nvalues);
const int * cgsGetIntAnnotationValues(CGMannotation, int *nvalues);
const char * cgsGetStringAnnotationValue(CGMannotation);
const int * cgsGetBooleanAnnotationValues(CGMannotation, int *nvalues);

// Cg 1.5 Additions
// CGobj Functions

// Cg 2.0 Buffer functions
long cgsGetParameterResourceSize(CGMparameter);
CGtype cgsGetParameterResourceType(CGMparameter);
int cgsGetParameterBufferIndex(CGMparameter);
int cgsGetParameterBufferOffset(CGMparameter);
int cgsGetBufferSize(CGMbuffer);
int cgsGetProgramBufferMaxSize(CGMprofile profile);
int cgsGetProgramBufferMaxIndex(CGMprofile profile);

//
// Multi-cast functions - accept a multiplexed object and invoke the
// corresponding Cg function on all objects. Functions with output parameters or
// return values will typically 'select' the affiliated Cg object to query
//

// Context functions
void cgmDestroyContext(CGMcontext ctx);

// Program functions
void cgmSetProgramProfile(CGMprogram prog, CGprofile profile);

// Parameter functions
CGMparameter cgmGetNextParameter(CGMparameter current);
CGMparameter cgmGetFirstLeafParameter(CGMprogram prog, CGenum name_space);
CGMparameter cgmGetNextLeafParameter(CGMparameter current);
CGMparameter cgmGetFirstStructParameter(CGMparameter param);
CGMparameter cgmGetArrayParameter(CGMparameter aparam, int index);
void cgmSetParameterValuefc(CGMparameter param, int n, const float *vals);
void cgmSetParameterValueic(CGMparameter param, int n, const int *vals);
void cgmSetStringParameterValue(CGMparameter param, const char *str);

CGMparameter cgmCreateParameter(CGMcontext context, CGtype type);
void cgmDestroyParameter(CGMparameter param);

// Type Functions
// Resource Functions
// Enum Functions
// Profile Functions
// Error Functions
// Misc Functions

// CgFX Functions
CGMeffect cgmCreateEffect(CGMcontext, const char *code, const char **args);
CGMeffect cgmCreateEffectFromFile(CGMcontext, const char *filename, const char **args);
void cgmDestroyEffect(CGMeffect);
CGMtechnique cgmGetFirstTechnique(CGMeffect);
CGMtechnique cgmGetNextTechnique(CGMtechnique);
CGMpass cgmGetFirstPass(CGMtechnique);
CGMpass cgmGetNextPass(CGMpass);
CGMstateassignment cgmGetFirstStateAssignment(CGMpass);
CGMstateassignment cgmGetNextStateAssignment(CGMstateassignment);
CGMprogram cgmGetProgramStateAssignmentValue(CGMstateassignment);
CGMparameter cgmGetTextureStateAssignmentValue(CGMstateassignment);
CGMstate cgmGetStateAssignmentState(CGMstateassignment);
CGMstate cgmGetSamplerStateAssignmentState(CGMstateassignment);
CGMstateassignment cgmCreateSamplerStateAssignment(CGMparameter, CGMstate);
CGMstateassignment cgmGetFirstSamplerStateAssignment(CGMparameter);
CGMparameter cgmGetFirstEffectParameter(CGMeffect);
CGMannotation cgmGetFirstTechniqueAnnotation(CGMtechnique);
CGMannotation cgmGetFirstPassAnnotation(CGMpass);
CGMannotation cgmGetFirstParameterAnnotation(CGMparameter);
CGMannotation cgmGetNextAnnotation(CGMannotation);

// Cg 1.5 Additions
CGMstate cgmCreateSamplerState(CGMcontext, const char *name, CGtype);
CGbool cgmSetFloatStateAssignment(CGMstateassignment, float);
CGbool cgmSetIntStateAssignment(CGMstateassignment, int);
CGbool cgmSetBoolStateAssignment(CGMstateassignment, CGbool);
CGbool cgmSetFloatArrayStateAssignment(CGMstateassignment, const float *vals);
CGbool cgmSetIntArrayStateAssignment(CGMstateassignment, const int *vals);
CGbool cgmSetBoolArrayStateAssignment(CGMstateassignment, const CGbool *vals);

// CGobj Functions

// Cg 2.0 Buffer functions
CGMbuffer cgmCreateBuffer(CGMcontext, int size, const void *data, CGbufferusage bufferUsage);
void cgmSetBufferData(CGMbuffer, int size, const void *data);
void cgmSetBufferSubData(CGMbuffer, int offset, int size, const void *data);
void cgmSetProgramBuffer(CGMprogram program, int bufferIndex, CGMbuffer buffer);
void cgmDestroyBuffer(CGMbuffer buffer);

#endif // #if !defined(X86_64) || defined(NVSG_DISABLE_CG_MULTIPLEX) .. #else

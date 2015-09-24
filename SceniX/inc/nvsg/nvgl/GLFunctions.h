// Copyright NVIDIA Corporation 2012
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
/** @file */

#include "nvsgcommon.h"
#include "nvgl/nvgl.h"

#include  <set>
#include  <string>

namespace nvgl
{
  
  //! The \c GLFunctions class holds the OpenGL extension function pointers.
  /** Just like a \c GLContext, you will never have to use this class explicitly. A \c GLContext holds a \c GLFunctions member
    * to hold the extension functions of that OpenGL context. For every OpenGL context there might exist a different set of
    * extension functions. A \c GLFunctions holds all function pointers of one context. If you're using the uppercase \c WGL
    * prefix-functions \c WGLCreateContext(), \c WGLMakeCurrent(), and \c WGLDeleteContext(), one \c GLFunctions object is
    * maintained per OpenGL context and switched accordingly. */
  class GLFunctions
  {
    public:
      //! Constructor
#if defined(_WIN32)      
      GLFunctions( void );
#elif defined(LINUX)
      GLFunctions( Display* dpy );
#endif

      //! Destructor
      ~GLFunctions( void );

      //! Test if a given extension is available on the current context.
      /** An OpenGL extension is not only exported, but also available, if all the functions required for that
        * extension are exported.
        * \note Not all exported extensions might be available.
        * \returns  \c true, if the \a extension is available, otherwise \c false  */
      bool  isAvailable( const std::string& extension   //!< extension to test
                       ) const;

      //! Test if a given extension is exported on the current context.
      /** An OpenGL extension is exported, if it is part of the return value of \c glGetString(GL_EXTENSIONS).
        * \note Not all exported extensions might be available.
        * \returns  \c true, if \a extension is exported, otherwise c\ false  */
      bool  isExported( const std::string& extension    //!< extension to test
                      ) const;

      //! Allows manual setting of extension available state.
      /** This function is meant for debugging purposes and can have negative effects on stability
        */

      void  disableAvailability( const std::string& extension );

      //! Returns the version of the OpenGL available on the target system.
      /** \returns The floating point version of the OpenGL with the representation 
        * major.minor is returned as an integer calculated as 10*major+minor.\n\n
        * The OpenGL version 1.4, for example, will be reported as 14. */
      int getVersion() const;

    public:
//  Don't need to document all these wrapper functions !!!
#if ! defined( DOXYGEN_IGNORE )
      //  version 1.2 functions
      //
      //
      //
      //
      PFNGLBLENDCOLORPROC                         glBlendColor;
      PFNGLBLENDEQUATIONPROC                      glBlendEquation;
      PFNGLCOLORTABLEPROC                         glColorTable;
      PFNGLCOLORTABLEPARAMETERFVPROC              glColorTableParameterfv;
      PFNGLCOLORTABLEPARAMETERIVPROC              glColorTableParameteriv;
      PFNGLCOPYCOLORTABLEPROC                     glCopyColorTable;
      PFNGLCOPYTEXSUBIMAGE3DPROC                  glCopyTexSubImage3D;
      PFNGLDRAWRANGEELEMENTSPROC                  glDrawRangeElements;
      PFNGLGETCOLORTABLEPROC                      glGetColorTable;
      PFNGLGETCOLORTABLEPARAMETERFVPROC           glGetColorTableParameterfv;
      PFNGLGETCOLORTABLEPARAMETERIVPROC           glGetColorTableParameteriv;
      PFNGLTEXIMAGE3DPROC                         glTexImage3D;
      PFNGLTEXSUBIMAGE3DPROC                      glTexSubImage3D;

      //  version 1.3 functions
      PFNGLACTIVETEXTUREPROC                      glActiveTexture;
      PFNGLCLIENTACTIVETEXTUREPROC                glClientActiveTexture;
      PFNGLCOMPRESSEDTEXIMAGE3DPROC               glCompressedTexImage3D;
      PFNGLCOMPRESSEDTEXIMAGE2DPROC               glCompressedTexImage2D;
      PFNGLCOMPRESSEDTEXIMAGE1DPROC               glCompressedTexImage1D;
      PFNGLCOMPRESSEDTEXSUBIMAGE3DPROC            glCompressedTexSubImage3D;
      PFNGLCOMPRESSEDTEXSUBIMAGE2DPROC            glCompressedTexSubImage2D;
      PFNGLCOMPRESSEDTEXSUBIMAGE1DPROC            glCompressedTexSubImage1D;
      PFNGLGETCOMPRESSEDTEXIMAGEPROC              glGetCompressedTexImage;
      PFNGLLOADTRANSPOSEMATRIXFPROC               glLoadTransposeMatrixf;
      PFNGLLOADTRANSPOSEMATRIXDPROC               glLoadTransposeMatrixd;
      PFNGLMULTITEXCOORD1DPROC                    glMultiTexCoord1d;
      PFNGLMULTITEXCOORD1DVPROC                   glMultiTexCoord1dv;
      PFNGLMULTITEXCOORD1FPROC                    glMultiTexCoord1f;
      PFNGLMULTITEXCOORD1FVPROC                   glMultiTexCoord1fv;
      PFNGLMULTITEXCOORD1IPROC                    glMultiTexCoord1i;
      PFNGLMULTITEXCOORD1IVPROC                   glMultiTexCoord1iv;
      PFNGLMULTITEXCOORD1SPROC                    glMultiTexCoord1s;
      PFNGLMULTITEXCOORD1SVPROC                   glMultiTexCoord1sv;
      PFNGLMULTITEXCOORD2DPROC                    glMultiTexCoord2d;
      PFNGLMULTITEXCOORD2DVPROC                   glMultiTexCoord2dv;
      PFNGLMULTITEXCOORD2FPROC                    glMultiTexCoord2f;
      PFNGLMULTITEXCOORD2FVPROC                   glMultiTexCoord2fv;
      PFNGLMULTITEXCOORD2IPROC                    glMultiTexCoord2i;
      PFNGLMULTITEXCOORD2IVPROC                   glMultiTexCoord2iv;
      PFNGLMULTITEXCOORD2SPROC                    glMultiTexCoord2s;
      PFNGLMULTITEXCOORD2SVPROC                   glMultiTexCoord2sv;
      PFNGLMULTITEXCOORD3DPROC                    glMultiTexCoord3d;
      PFNGLMULTITEXCOORD3DVPROC                   glMultiTexCoord3dv;
      PFNGLMULTITEXCOORD3FPROC                    glMultiTexCoord3f;
      PFNGLMULTITEXCOORD3FVPROC                   glMultiTexCoord3fv;
      PFNGLMULTITEXCOORD3IPROC                    glMultiTexCoord3i;
      PFNGLMULTITEXCOORD3IVPROC                   glMultiTexCoord3iv;
      PFNGLMULTITEXCOORD3SPROC                    glMultiTexCoord3s;
      PFNGLMULTITEXCOORD3SVPROC                   glMultiTexCoord3sv;
      PFNGLMULTITEXCOORD4DPROC                    glMultiTexCoord4d;
      PFNGLMULTITEXCOORD4DVPROC                   glMultiTexCoord4dv;
      PFNGLMULTITEXCOORD4FPROC                    glMultiTexCoord4f;
      PFNGLMULTITEXCOORD4FVPROC                   glMultiTexCoord4fv;
      PFNGLMULTITEXCOORD4IPROC                    glMultiTexCoord4i;
      PFNGLMULTITEXCOORD4IVPROC                   glMultiTexCoord4iv;
      PFNGLMULTITEXCOORD4SPROC                    glMultiTexCoord4s;
      PFNGLMULTITEXCOORD4SVPROC                   glMultiTexCoord4sv;
      PFNGLMULTTRANSPOSEMATRIXFPROC               glMultTransposeMatrixf;
      PFNGLMULTTRANSPOSEMATRIXDPROC               glMultTransposeMatrixd;
      PFNGLSAMPLECOVERAGEPROC                     glSampleCoverage;

      //  version 1.4 functions
      PFNGLBLENDFUNCSEPARATEPROC                  glBlendFuncSeparate;
      PFNGLFOGCOORDDPROC                          glFogCoordd;
      PFNGLFOGCOORDDVPROC                         glFogCoorddv;
      PFNGLFOGCOORDFPROC                          glFogCoordf;
      PFNGLFOGCOORDFVPROC                         glFogCoordfv;
      PFNGLFOGCOORDPOINTERPROC                    glFogCoordPointer;
      PFNGLMULTIDRAWARRAYSPROC                    glMultiDrawArrays;
      PFNGLMULTIDRAWELEMENTSPROC                  glMultiDrawElements;
      PFNGLPOINTPARAMETERFPROC                    glPointParameterf;
      PFNGLPOINTPARAMETERFVPROC                   glPointParameterfv;
      PFNGLPOINTPARAMETERIPROC                    glPointParameteri;
      PFNGLPOINTPARAMETERIVPROC                   glPointParameteriv;
      PFNGLSECONDARYCOLOR3BPROC                   glSecondaryColor3b;
      PFNGLSECONDARYCOLOR3BVPROC                  glSecondaryColor3bv;
      PFNGLSECONDARYCOLOR3DPROC                   glSecondaryColor3d;
      PFNGLSECONDARYCOLOR3DVPROC                  glSecondaryColor3dv;
      PFNGLSECONDARYCOLOR3FPROC                   glSecondaryColor3f;
      PFNGLSECONDARYCOLOR3FVPROC                  glSecondaryColor3fv;
      PFNGLSECONDARYCOLOR3IPROC                   glSecondaryColor3i;
      PFNGLSECONDARYCOLOR3IVPROC                  glSecondaryColor3iv;
      PFNGLSECONDARYCOLOR3SPROC                   glSecondaryColor3s;
      PFNGLSECONDARYCOLOR3SVPROC                  glSecondaryColor3sv;
      PFNGLSECONDARYCOLOR3UBPROC                  glSecondaryColor3ub;
      PFNGLSECONDARYCOLOR3UBVPROC                 glSecondaryColor3ubv;
      PFNGLSECONDARYCOLOR3UIPROC                  glSecondaryColor3ui;
      PFNGLSECONDARYCOLOR3UIVPROC                 glSecondaryColor3uiv;
      PFNGLSECONDARYCOLOR3USPROC                  glSecondaryColor3us;
      PFNGLSECONDARYCOLOR3USVPROC                 glSecondaryColor3usv;
      PFNGLSECONDARYCOLORPOINTERPROC              glSecondaryColorPointer;
      PFNGLWINDOWPOS2DPROC                        glWindowPos2d;
      PFNGLWINDOWPOS2FPROC                        glWindowPos2f;
      PFNGLWINDOWPOS2IPROC                        glWindowPos2i;
      PFNGLWINDOWPOS2SPROC                        glWindowPos2s;
      PFNGLWINDOWPOS2DVPROC                       glWindowPos2dv;
      PFNGLWINDOWPOS2FVPROC                       glWindowPos2fv;
      PFNGLWINDOWPOS2IVPROC                       glWindowPos2iv;
      PFNGLWINDOWPOS2SVPROC                       glWindowPos2sv;
      PFNGLWINDOWPOS3DPROC                        glWindowPos3d;
      PFNGLWINDOWPOS3FPROC                        glWindowPos3f;
      PFNGLWINDOWPOS3IPROC                        glWindowPos3i;
      PFNGLWINDOWPOS3SPROC                        glWindowPos3s;
      PFNGLWINDOWPOS3DVPROC                       glWindowPos3dv;
      PFNGLWINDOWPOS3FVPROC                       glWindowPos3fv;
      PFNGLWINDOWPOS3IVPROC                       glWindowPos3iv;
      PFNGLWINDOWPOS3SVPROC                       glWindowPos3sv;

      //  version 1.5 functions
      PFNGLBEGINQUERYPROC                         glBeginQuery;
      PFNGLBINDBUFFERPROC                         glBindBuffer;
      PFNGLBUFFERDATAPROC                         glBufferData;
      PFNGLBUFFERSUBDATAPROC                      glBufferSubData;
      PFNGLDELETEBUFFERSPROC                      glDeleteBuffers;
      PFNGLDELETEQUERIESPROC                      glDeleteQueries;
      PFNGLENDQUERYPROC                           glEndQuery;
      PFNGLGENBUFFERSPROC                         glGenBuffers;
      PFNGLGENQUERIESPROC                         glGenQueries;
      PFNGLGETBUFFERPARAMETERIVPROC               glGetBufferParameteriv;
      PFNGLGETBUFFERPOINTERVPROC                  glGetBufferPointerv;
      PFNGLGETBUFFERSUBDATAPROC                   glGetBufferSubData;
      PFNGLGETQUERYOBJECTIVPROC                   glGetQueryObjectiv;
      PFNGLGETQUERYOBJECTUIVPROC                  glGetQueryObjectuiv;
      PFNGLGETQUERYIVPROC                         glGetQueryiv;
      PFNGLISBUFFERPROC                           glIsBuffer;
      PFNGLISQUERYPROC                            glIsQuery;
      PFNGLMAPBUFFERPROC                          glMapBuffer;
      PFNGLUNMAPBUFFERPROC                        glUnmapBuffer;

      //  version 2.0 functions
      PFNGLATTACHSHADERPROC                       glAttachShader;
      PFNGLBINDATTRIBLOCATIONPROC                 glBindAttribLocation;
      PFNGLBLENDEQUATIONSEPARATEPROC              glBlendEquationSeparate;
      PFNGLCOMPILESHADERPROC                      glCompileShader;
      PFNGLCREATEPROGRAMPROC                      glCreateProgram;
      PFNGLCREATESHADERPROC                       glCreateShader;
      PFNGLDELETEPROGRAMPROC                      glDeleteProgram;
      PFNGLDELETESHADERPROC                       glDeleteShader;
      PFNGLDETACHSHADERPROC                       glDetachShader;
      PFNGLDISABLEVERTEXATTRIBARRAYPROC           glDisableVertexAttribArray;
      PFNGLDRAWBUFFERSARBPROC                     glDrawBuffers;
      PFNGLENABLEVERTEXATTRIBARRAYPROC            glEnableVertexAttribArray;
      PFNGLLINKPROGRAMPROC                        glLinkProgram;
      PFNGLGETACTIVEATTRIBPROC                    glGetActiveAttrib;
      PFNGLGETACTIVEUNIFORMPROC                   glGetActiveUniform;
      PFNGLGETATTACHEDSHADERSPROC                 glGetAttachedShaders;
      PFNGLGETATTRIBLOCATIONPROC                  glGetAttribLocation;
      PFNGLGETPROGRAMIVPROC                       glGetProgramiv;
      PFNGLGETPROGRAMINFOLOGPROC                  glGetProgramInfoLog;
      PFNGLGETSHADERIVPROC                        glGetShaderiv;
      PFNGLGETSHADERINFOLOGPROC                   glGetShaderInfoLog;
      PFNGLGETSHADERSOURCEPROC                    glGetShaderSource;
      PFNGLGETUNIFORMFVPROC                       glGetUniformfv;
      PFNGLGETUNIFORMIVPROC                       glGetUniformiv;
      PFNGLGETUNIFORMLOCATIONPROC                 glGetUniformLocation;
      PFNGLGETVERTEXATTRIBDVPROC                  glGetVertexAttribdv;
      PFNGLGETVERTEXATTRIBFVPROC                  glGetVertexAttribfv;
      PFNGLGETVERTEXATTRIBIVPROC                  glGetVertexAttribiv;
      PFNGLGETVERTEXATTRIBPOINTERVPROC            glGetVertexAttribPointerv;
      PFNGLISPROGRAMPROC                          glIsProgram;
      PFNGLISSHADERPROC                           glIsShader;
      PFNGLSHADERSOURCEPROC                       glShaderSource;
      PFNGLSTENCILFUNCSEPARATEPROC                glStencilFuncSeparate;
      PFNGLSTENCILOPSEPARATEPROC                  glStencilOpSeparate;
      PFNGLSTENCILMASKSEPARATEPROC                glStencilMaskSeparate;
      PFNGLUNIFORM1FPROC                          glUniform1f;
      PFNGLUNIFORM1FVPROC                         glUniform1fv;
      PFNGLUNIFORM1IPROC                          glUniform1i;
      PFNGLUNIFORM1IVPROC                         glUniform1iv;
      PFNGLUNIFORM2FPROC                          glUniform2f;
      PFNGLUNIFORM2FVPROC                         glUniform2fv;
      PFNGLUNIFORM2IPROC                          glUniform2i;
      PFNGLUNIFORM2IVPROC                         glUniform2iv;
      PFNGLUNIFORM3FPROC                          glUniform3f;
      PFNGLUNIFORM3FVPROC                         glUniform3fv;
      PFNGLUNIFORM3IPROC                          glUniform3i;
      PFNGLUNIFORM3IVPROC                         glUniform3iv;
      PFNGLUNIFORM4FPROC                          glUniform4f;
      PFNGLUNIFORM4FVPROC                         glUniform4fv;
      PFNGLUNIFORM4IPROC                          glUniform4i;
      PFNGLUNIFORM4IVPROC                         glUniform4iv;
      PFNGLUNIFORMMATRIX2FVPROC                   glUniformMatrix2fv;
      PFNGLUNIFORMMATRIX3FVPROC                   glUniformMatrix3fv;
      PFNGLUNIFORMMATRIX4FVPROC                   glUniformMatrix4fv;
      PFNGLUSEPROGRAMPROC                         glUseProgram;
      PFNGLVALIDATEPROGRAMPROC                    glValidateProgram;
      PFNGLVERTEXATTRIB1DPROC                     glVertexAttrib1d;
      PFNGLVERTEXATTRIB1DVPROC                    glVertexAttrib1dv;
      PFNGLVERTEXATTRIB1FPROC                     glVertexAttrib1f;
      PFNGLVERTEXATTRIB1FVPROC                    glVertexAttrib1fv;
      PFNGLVERTEXATTRIB1SPROC                     glVertexAttrib1s;
      PFNGLVERTEXATTRIB1SVPROC                    glVertexAttrib1sv;
      PFNGLVERTEXATTRIB2DPROC                     glVertexAttrib2d;
      PFNGLVERTEXATTRIB2DVPROC                    glVertexAttrib2dv;
      PFNGLVERTEXATTRIB2FPROC                     glVertexAttrib2f;
      PFNGLVERTEXATTRIB2FVPROC                    glVertexAttrib2fv;
      PFNGLVERTEXATTRIB2SPROC                     glVertexAttrib2s;
      PFNGLVERTEXATTRIB2SVPROC                    glVertexAttrib2sv;
      PFNGLVERTEXATTRIB3DPROC                     glVertexAttrib3d;
      PFNGLVERTEXATTRIB3DVPROC                    glVertexAttrib3dv;
      PFNGLVERTEXATTRIB3FPROC                     glVertexAttrib3f;
      PFNGLVERTEXATTRIB3FVPROC                    glVertexAttrib3fv;
      PFNGLVERTEXATTRIB3SPROC                     glVertexAttrib3s;
      PFNGLVERTEXATTRIB3SVPROC                    glVertexAttrib3sv;
      PFNGLVERTEXATTRIB4NBVPROC                   glVertexAttrib4Nbv;
      PFNGLVERTEXATTRIB4NIVPROC                   glVertexAttrib4Niv;
      PFNGLVERTEXATTRIB4NSVPROC                   glVertexAttrib4Nsv;
      PFNGLVERTEXATTRIB4NUBPROC                   glVertexAttrib4Nub;
      PFNGLVERTEXATTRIB4NUBVPROC                  glVertexAttrib4Nubv;
      PFNGLVERTEXATTRIB4NUIVPROC                  glVertexAttrib4Nuiv;
      PFNGLVERTEXATTRIB4NUSVPROC                  glVertexAttrib4Nusv;
      PFNGLVERTEXATTRIB4BVPROC                    glVertexAttrib4bv;
      PFNGLVERTEXATTRIB4DPROC                     glVertexAttrib4d;
      PFNGLVERTEXATTRIB4DVPROC                    glVertexAttrib4dv;
      PFNGLVERTEXATTRIB4FPROC                     glVertexAttrib4f;
      PFNGLVERTEXATTRIB4FVPROC                    glVertexAttrib4fv;
      PFNGLVERTEXATTRIB4IVPROC                    glVertexAttrib4iv;
      PFNGLVERTEXATTRIB4SPROC                     glVertexAttrib4s;
      PFNGLVERTEXATTRIB4SVPROC                    glVertexAttrib4sv;
      PFNGLVERTEXATTRIB4UBVPROC                   glVertexAttrib4ubv;
      PFNGLVERTEXATTRIB4UIVPROC                   glVertexAttrib4uiv;
      PFNGLVERTEXATTRIB4USVPROC                   glVertexAttrib4usv;
      PFNGLVERTEXATTRIBPOINTERPROC                glVertexAttribPointer;

      //  version 2.1 functions
      PFNGLUNIFORMMATRIX2X3FVPROC                 glUniformMatrix2x3fv;
      PFNGLUNIFORMMATRIX2X4FVPROC                 glUniformMatrix2x4fv;
      PFNGLUNIFORMMATRIX3X2FVPROC                 glUniformMatrix3x2fv;
      PFNGLUNIFORMMATRIX3X4FVPROC                 glUniformMatrix3x4fv;
      PFNGLUNIFORMMATRIX4X2FVPROC                 glUniformMatrix4x2fv;
      PFNGLUNIFORMMATRIX4X3FVPROC                 glUniformMatrix4x3fv;

      //  version 3.0 functions
      PFNGLBEGINTRANSFORMFEEDBACKPROC             glBeginTransformFeedback;
      PFNGLBINDBUFFERBASEPROC                     glBindBufferBase;
      PFNGLBINDBUFFERRANGEPROC                    glBindBufferRange;
      PFNGLENDTRANSFORMFEEDBACKPROC               glEndTransformFeedback;
      PFNGLGETTRANSFORMFEEDBACKVARYINGPROC        glGetTransformFeedbackVarying;
      PFNGLTRANSFORMFEEDBACKVARYINGSPROC          glTransformFeedbackVaryings;

      //  GL_ARB_color_buffer_float functions
      PFNGLCLAMPCOLORARBPROC                      glClampColor;

      //  GL_ARB_fragment_program functions see GL_ARB_vertex_program

      //  GL_ARB_imaging functions
      PFNGLCOLORSUBTABLEPROC                      glColorSubTable;
      PFNGLCOPYCOLORSUBTABLEPROC                  glCopyColorSubTable;
      PFNGLCONVOLUTIONFILTER1DPROC                glConvolutionFilter1D;
      PFNGLCONVOLUTIONFILTER2DPROC                glConvolutionFilter2D;
      PFNGLCONVOLUTIONPARAMETERFPROC              glConvolutionParameterf;
      PFNGLCONVOLUTIONPARAMETERFVPROC             glConvolutionParameterfv;
      PFNGLCONVOLUTIONPARAMETERIPROC              glConvolutionParameteri;
      PFNGLCONVOLUTIONPARAMETERIVPROC             glConvolutionParameteriv;
      PFNGLCOPYCONVOLUTIONFILTER1DPROC            glCopyConvolutionFilter1D;
      PFNGLCOPYCONVOLUTIONFILTER2DPROC            glCopyConvolutionFilter2D;
      PFNGLGETCONVOLUTIONFILTERPROC               glGetConvolutionFilter;
      PFNGLGETCONVOLUTIONPARAMETERFVPROC          glGetConvolutionParameterfv;
      PFNGLGETCONVOLUTIONPARAMETERIVPROC          glGetConvolutionParameteriv;
      PFNGLGETSEPARABLEFILTERPROC                 glGetSeparableFilter;
      PFNGLSEPARABLEFILTER2DPROC                  glSeparableFilter2D;
      PFNGLGETHISTOGRAMPROC                       glGetHistogram;
      PFNGLGETHISTOGRAMPARAMETERFVPROC            glGetHistogramParameterfv;
      PFNGLGETHISTOGRAMPARAMETERIVPROC            glGetHistogramParameteriv;
      PFNGLGETMINMAXPROC                          glGetMinmax;
      PFNGLGETMINMAXPARAMETERFVPROC               glGetMinmaxParameterfv;
      PFNGLGETMINMAXPARAMETERIVPROC               glGetMinmaxParameteriv;
      PFNGLHISTOGRAMPROC                          glHistogram;
      PFNGLMINMAXPROC                             glMinmax;
      PFNGLRESETHISTOGRAMPROC                     glResetHistogram;
      PFNGLRESETMINMAXPROC                        glResetMinmax;

      //  GL_ARB_shader_objects functions
      PFNGLATTACHOBJECTARBPROC                    glAttachObjectARB;
      PFNGLCREATEPROGRAMOBJECTARBPROC             glCreateProgramObjectARB;
      PFNGLCREATESHADEROBJECTARBPROC              glCreateShaderObjectARB;
      PFNGLDELETEOBJECTARBPROC                    glDeleteObjectARB;
      PFNGLDETACHOBJECTARBPROC                    glDetachObjectARB;
      PFNGLGETATTACHEDOBJECTSARBPROC              glGetAttachedObjectsARB;
      PFNGLGETHANDLEARBPROC                       glGetHandleARB;
      PFNGLGETINFOLOGARBPROC                      glGetInfoLogARB;
      PFNGLGETOBJECTPARAMETERFVARBPROC            glGetObjectParameterfvARB;
      PFNGLGETOBJECTPARAMETERIVARBPROC            glGetObjectParameterivARB;
      PFNGLUSEPROGRAMOBJECTARBPROC                glUseProgramObjectARB;

      //  GL_ARB_vertex_program functions
      PFNGLBINDPROGRAMARBPROC                     glBindProgramARB;
      PFNGLDELETEPROGRAMSARBPROC                  glDeleteProgramsARB;
      PFNGLGENPROGRAMSARBPROC                     glGenProgramsARB;
      PFNGLGETPROGRAMENVPARAMETERDVARBPROC        glGetProgramEnvParameterdvARB;
      PFNGLGETPROGRAMENVPARAMETERFVARBPROC        glGetProgramEnvParameterfvARB;
      PFNGLGETPROGRAMLOCALPARAMETERDVARBPROC      glGetProgramLocalParameterdvARB;
      PFNGLGETPROGRAMLOCALPARAMETERFVARBPROC      glGetProgramLocalParameterfvARB;
      PFNGLGETPROGRAMSTRINGARBPROC                glGetProgramStringARB;
      PFNGLPROGRAMENVPARAMETER4DARBPROC           glProgramEnvParameter4dARB;
      PFNGLPROGRAMENVPARAMETER4DVARBPROC          glProgramEnvParameter4dvARB;
      PFNGLPROGRAMENVPARAMETER4FARBPROC           glProgramEnvParameter4fARB;
      PFNGLPROGRAMENVPARAMETER4FVARBPROC          glProgramEnvParameter4fvARB;
      PFNGLPROGRAMLOCALPARAMETER4DARBPROC         glProgramLocalParameter4dARB;
      PFNGLPROGRAMLOCALPARAMETER4DVARBPROC        glProgramLocalParameter4dvARB;
      PFNGLPROGRAMLOCALPARAMETER4FARBPROC         glProgramLocalParameter4fARB;
      PFNGLPROGRAMLOCALPARAMETER4FVARBPROC        glProgramLocalParameter4fvARB;
      PFNGLPROGRAMSTRINGARBPROC                   glProgramStringARB;

      //  GL_ARB_tessellation_shader
      PFNGLPATCHPARAMETERFVPROC                   glPatchParameterfv;
      PFNGLPATCHPARAMETERIPROC                    glPatchParameteri;

      //  GL_ARB_map_buffer_range
      PFNGLMAPBUFFERRANGEPROC                     glMapBufferRange;
      PFNGLFLUSHMAPPEDBUFFERRANGEPROC             glFlushMappedBufferRange;

      //  GL_ARB_copy_buffer
      PFNGLCOPYBUFFERSUBDATAPROC                  glCopyBufferSubData;

      //  GL_ARB_texture_multisample
      PFNGLTEXIMAGE2DMULTISAMPLEPROC              glTexImage2DMultisample;
      PFNGLTEXIMAGE3DMULTISAMPLEPROC              glTexImage3DMultisample;
      PFNGLGETMULTISAMPLEFVPROC                   glGetMultisamplefv;
      PFNGLSAMPLEMASKIPROC                        glSampleMaski;

      // GL_ARB_vertex_array_object
      PFNGLBINDVERTEXARRAYPROC                    glBindVertexArray;
      PFNGLDELETEVERTEXARRAYSPROC                 glDeleteVertexArrays;
      PFNGLGENVERTEXARRAYSPROC                    glGenVertexArrays;
      PFNGLISVERTEXARRAYPROC                      glIsVertexArray;

      PFNGLDEBUGMESSAGECONTROLARBPROC             glDebugMessageControl;
      PFNGLDEBUGMESSAGEINSERTARBPROC              glDebugMessageInsert;
      PFNGLDEBUGMESSAGECALLBACKARBPROC            glDebugMessageCallback;
      PFNGLGETDEBUGMESSAGELOGARBPROC              glGetDebugMessageLog;

      //  GL_EXT_bindable_uniform functions
      PFNGLGETUNIFORMBUFFERSIZEEXTPROC            glGetUniformBufferSize;
      PFNGLGETUNIFORMOFFSETEXTPROC                glGetUniformOffset;
      PFNGLUNIFORMBUFFEREXTPROC                   glUniformBuffer;

      //  GL_EXT_compiled_vertex_array functions
      PFNGLLOCKARRAYSEXTPROC                      glLockArrays;
      PFNGLUNLOCKARRAYSEXTPROC                    glUnlockArrays;

      //  GL_EXT_depth_bounds_test functions
      PFNGLDEPTHBOUNDSEXTPROC                     glDepthBounds;

      //  GL_EXT_draw_buffers2 functions
      PFNGLCOLORMASKINDEXEDEXTPROC                glColorMaskIndexed;
      PFNGLDISABLEINDEXEDEXTPROC                  glDisableIndexed;
      PFNGLENABLEINDEXEDEXTPROC                   glEnableIndexed;
      PFNGLGETBOOLEANINDEXEDVEXTPROC              glGetBooleanIndexedv;
      PFNGLGETINTEGERINDEXEDVEXTPROC              glGetIntegerIndexedv;
      PFNGLISENABLEDINDEXEDEXTPROC                glIsEnabledIndexed;

      //  GL_EXT_draw_instanced functions
      PFNGLDRAWARRAYSINSTANCEDEXTPROC             glDrawArraysInstanced;
      PFNGLDRAWELEMENTSINSTANCEDEXTPROC           glDrawElementsInstanced;

      //  GL_EXT_framebuffer_blit functions
      PFNGLBLITFRAMEBUFFEREXTPROC                 glBlitFramebuffer;

      //  GL_EXT_framebuffer_multisample functions
      PFNGLRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC      glRenderbufferStorageMultisample;

      // GL_EXT_framebuffer_object
      PFNGLISRENDERBUFFEREXTPROC                      glIsRenderbuffer;
      PFNGLBINDRENDERBUFFEREXTPROC                    glBindRenderbuffer;
      PFNGLDELETERENDERBUFFERSEXTPROC                 glDeleteRenderbuffers;
      PFNGLGENRENDERBUFFERSEXTPROC                    glGenRenderbuffers;
      PFNGLRENDERBUFFERSTORAGEEXTPROC                 glRenderbufferStorage;
      PFNGLGETRENDERBUFFERPARAMETERIVEXTPROC          glGetRenderbufferParameteriv;
      PFNGLISFRAMEBUFFEREXTPROC                       glIsFramebuffer;
      PFNGLBINDFRAMEBUFFEREXTPROC                     glBindFramebuffer;
      PFNGLDELETEFRAMEBUFFERSEXTPROC                  glDeleteFramebuffers;
      PFNGLGENFRAMEBUFFERSEXTPROC                     glGenFramebuffers;
      PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC              glCheckFramebufferStatus;
      PFNGLFRAMEBUFFERTEXTURE1DEXTPROC                glFramebufferTexture1D;
      PFNGLFRAMEBUFFERTEXTURE2DEXTPROC                glFramebufferTexture2D;
      PFNGLFRAMEBUFFERTEXTURE3DEXTPROC                glFramebufferTexture3D;
      PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC             glFramebufferRenderbuffer;
      PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC glGetFramebufferAttachmentParameteriv;
      PFNGLGENERATEMIPMAPEXTPROC                      glGenerateMipmap;

      //  GL_EXT_geometry_shader4 functions
      PFNGLPROGRAMPARAMETERIEXTPROC               glProgramParameteri;

      //  GL_EXT_gpu_program_parameters functions
      PFNGLPROGRAMENVPARAMETERS4FVEXTPROC         glProgramEnvParameters4fv;
      PFNGLPROGRAMLOCALPARAMETERS4FVEXTPROC       glProgramLocalParameters4fv;

      //  GL_EXT_gpu_shader4 functions
      PFNGLBINDFRAGDATALOCATIONEXTPROC            glBindFragDataLocation;
      PFNGLGETFRAGDATALOCATIONEXTPROC             glGetFragDataLocation;
      PFNGLGETUNIFORMUIVEXTPROC                   glGetUniformuiv;
      PFNGLUNIFORM1UIEXTPROC                      glUniform1ui;
      PFNGLUNIFORM1UIVEXTPROC                     glUniform1uiv;
      PFNGLUNIFORM2UIEXTPROC                      glUniform2ui;
      PFNGLUNIFORM2UIVEXTPROC                     glUniform2uiv;
      PFNGLUNIFORM3UIEXTPROC                      glUniform3ui;
      PFNGLUNIFORM3UIVEXTPROC                     glUniform3uiv;
      PFNGLUNIFORM4UIEXTPROC                      glUniform4ui;
      PFNGLUNIFORM4UIVEXTPROC                     glUniform4uiv;

      //  GL_EXT_point_parameters functions
      PFNGLPOINTPARAMETERFEXTPROC                 glPointParameterfEXT;
      PFNGLPOINTPARAMETERFVEXTPROC                glPointParameterfvEXT;

      //  GL_EXT_stencil_clear_tag functions
      PFNGLSTENCILCLEARTAGEXTPROC                 glStencilClearTag;

      //  GL_EXT_stencil_two_side functions
      PFNGLACTIVESTENCILFACEEXTPROC               glActiveStencilFaceEXT;

      // GL_EXT_texture_array
      PFNGLFRAMEBUFFERTEXTURELAYEREXTPROC         glFramebufferTextureLayer;

      //  GL_EXT_texture_buffer_object functions
      PFNGLTEXBUFFEREXTPROC                       glTexBuffer;

      //  GL_EXT_texture_integer functions
      PFNGLCLEARCOLORIIEXTPROC                    glClearColorIi;
      PFNGLCLEARCOLORIUIEXTPROC                   glClearColorIui;
      PFNGLGETTEXPARAMETERIIVEXTPROC              glGetTexParameterIiv;
      PFNGLGETTEXPARAMETERIUIVEXTPROC             glGetTexParameterIuiv;
      PFNGLTEXPARAMETERIIVEXTPROC                 glTexParameterIiv;
      PFNGLTEXPARAMETERIUIVEXTPROC                glTexParameterIuiv;

      //  GL_EXT_timer_query
      PFNGLGETQUERYOBJECTI64VEXTPROC              glGetQueryObjecti64v;
      PFNGLGETQUERYOBJECTUI64VEXTPROC             glGetQueryObjectui64v;

      //  GL_EXT_vertex_program4 functions
      PFNGLGETVERTEXATTRIBIIVEXTPROC              glGetVertexAttribIiv;
      PFNGLGETVERTEXATTRIBIUIVEXTPROC             glGetVertexAttribIuiv;
      PFNGLVERTEXATTRIBI1IEXTPROC                 glVertexAttribI1i;
      PFNGLVERTEXATTRIBI1IVEXTPROC                glVertexAttribI1iv;
      PFNGLVERTEXATTRIBI1UIEXTPROC                glVertexAttribI1ui;
      PFNGLVERTEXATTRIBI1UIVEXTPROC               glVertexAttribI1uiv;
      PFNGLVERTEXATTRIBI2IEXTPROC                 glVertexAttribI2i;
      PFNGLVERTEXATTRIBI2IVEXTPROC                glVertexAttribI2iv;
      PFNGLVERTEXATTRIBI2UIEXTPROC                glVertexAttribI2ui;
      PFNGLVERTEXATTRIBI2UIVEXTPROC               glVertexAttribI2uiv;
      PFNGLVERTEXATTRIBI3IEXTPROC                 glVertexAttribI3i;
      PFNGLVERTEXATTRIBI3IVEXTPROC                glVertexAttribI3iv;
      PFNGLVERTEXATTRIBI3UIEXTPROC                glVertexAttribI3ui;
      PFNGLVERTEXATTRIBI3UIVEXTPROC               glVertexAttribI3uiv;
      PFNGLVERTEXATTRIBI4BVEXTPROC                glVertexAttribI4bv;
      PFNGLVERTEXATTRIBI4IEXTPROC                 glVertexAttribI4i;
      PFNGLVERTEXATTRIBI4IVEXTPROC                glVertexAttribI4iv;
      PFNGLVERTEXATTRIBI4SVEXTPROC                glVertexAttribI4sv;
      PFNGLVERTEXATTRIBI4UBVEXTPROC               glVertexAttribI4ubv;
      PFNGLVERTEXATTRIBI4UIEXTPROC                glVertexAttribI4ui;
      PFNGLVERTEXATTRIBI4USVEXTPROC               glVertexAttribI4usv;
      PFNGLVERTEXATTRIBI4UIVEXTPROC               glVertexAttribI4uiv;
      PFNGLVERTEXATTRIBIPOINTEREXTPROC            glVertexAttribIPointer;

      // GL_GREMEDY_string_marker
      PFNGLSTRINGMARKERGREMEDYPROC                glStringMarker;

      // GL_GREMEDY_frame_terminator
      PFNGLFRAMETERMINATORGREMEDYPROC             glFrameTerminator;

      //  GL_NV_depth_buffer_float
      PFNGLDEPTHRANGEDNVPROC                      glDepthRanged;
      PFNGLCLEARDEPTHDNVPROC                      glClearDepthd;
      PFNGLDEPTHBOUNDSDNVPROC                     glDepthBoundsd;

      //  GL_NV_fence functions
      PFNGLDELETEFENCESNVPROC                     glDeleteFences;
      PFNGLFINISHFENCENVPROC                      glFinishFence;
      PFNGLGENFENCESNVPROC                        glGenFences;
      PFNGLGETFENCEIVNVPROC                       glGetFenceiv;
      PFNGLISFENCENVPROC                          glIsFence;
      PFNGLSETFENCENVPROC                         glSetFence;
      PFNGLTESTFENCENVPROC                        glTestFence;

      //  GL_NV_fragment_program functions
      PFNGLGETPROGRAMNAMEDPARAMETERDVNVPROC       glGetProgramNamedParameterdv;
      PFNGLGETPROGRAMNAMEDPARAMETERFVNVPROC       glGetProgramNamedParameterfv;
      PFNGLPROGRAMNAMEDPARAMETER4DNVPROC          glProgramNamedParameter4d;
      PFNGLPROGRAMNAMEDPARAMETER4DVNVPROC         glProgramNamedParameter4dv;
      PFNGLPROGRAMNAMEDPARAMETER4FNVPROC          glProgramNamedParameter4f;
      PFNGLPROGRAMNAMEDPARAMETER4FVNVPROC         glProgramNamedParameter4fv;

      //  GL_NV_framebuffer_multisample_coverage functions
      PFNGLRENDERBUFFERSTORAGEMULTISAMPLECOVERAGENVPROC glRenderbufferStorageMultisampleCoverage;

      //  GL_NV_geometry_program4 functions
      PFNGLPROGRAMVERTEXLIMITNVPROC               glProgramVertexLimit;
      PFNGLFRAMEBUFFERTEXTUREEXTPROC              glFramebufferTexture;
      PFNGLFRAMEBUFFERTEXTUREFACEEXTPROC          glFramebufferTextureFace;

      //  GL_NV_gpu_program4 functions
      PFNGLGETPROGRAMENVPARAMETERIIVNVPROC        glGetProgramEnvParameterIiv;
      PFNGLGETPROGRAMENVPARAMETERIUIVNVPROC       glGetProgramEnvParameterIuiv;
      PFNGLGETPROGRAMLOCALPARAMETERIIVNVPROC      glGetProgramLocalParameterIiv;
      PFNGLGETPROGRAMLOCALPARAMETERIUIVNVPROC     glGetProgramLocalParameterIuiv;
      PFNGLPROGRAMENVPARAMETERSI4IVNVPROC         glProgramEnvParametersI4iv;
      PFNGLPROGRAMENVPARAMETERSI4UIVNVPROC        glProgramEnvParametersI4uiv;
      PFNGLPROGRAMENVPARAMETERI4INVPROC           glProgramEnvParameterI4i;
      PFNGLPROGRAMENVPARAMETERI4IVNVPROC          glProgramEnvParameterI4iv;
      PFNGLPROGRAMENVPARAMETERI4UINVPROC          glProgramEnvParameterI4ui;
      PFNGLPROGRAMENVPARAMETERI4UIVNVPROC         glProgramEnvParameterI4uiv;
      PFNGLPROGRAMLOCALPARAMETERSI4IVNVPROC       glProgramLocalParametersI4iv;
      PFNGLPROGRAMLOCALPARAMETERSI4UIVNVPROC      glProgramLocalParametersI4uiv;
      PFNGLPROGRAMLOCALPARAMETERI4INVPROC         glProgramLocalParameterI4i;
      PFNGLPROGRAMLOCALPARAMETERI4IVNVPROC        glProgramLocalParameterI4iv;
      PFNGLPROGRAMLOCALPARAMETERI4UINVPROC        glProgramLocalParameterI4ui;
      PFNGLPROGRAMLOCALPARAMETERI4UIVNVPROC       glProgramLocalParameterI4uiv;

      //  GL_NV_half_float functions
      PFNGLCOLOR3HNVPROC                          glColor3h;
      PFNGLCOLOR3HVNVPROC                         glColor3hv;
      PFNGLCOLOR4HNVPROC                          glColor4h;
      PFNGLCOLOR4HVNVPROC                         glColor4hv;
      PFNGLFOGCOORDHNVPROC                        glFogCoordh;
      PFNGLFOGCOORDHVNVPROC                       glFogCoordhv;
      PFNGLMULTITEXCOORD1HNVPROC                  glMultiTexCoord1h;
      PFNGLMULTITEXCOORD1HVNVPROC                 glMultiTexCoord1hv;
      PFNGLMULTITEXCOORD2HNVPROC                  glMultiTexCoord2h;
      PFNGLMULTITEXCOORD2HVNVPROC                 glMultiTexCoord2hv;
      PFNGLMULTITEXCOORD3HNVPROC                  glMultiTexCoord3h;
      PFNGLMULTITEXCOORD3HVNVPROC                 glMultiTexCoord3hv;
      PFNGLMULTITEXCOORD4HNVPROC                  glMultiTexCoord4h;
      PFNGLMULTITEXCOORD4HVNVPROC                 glMultiTexCoord4hv;
      PFNGLNORMAL3HNVPROC                         glNormal3h;
      PFNGLNORMAL3HVNVPROC                        glNormal3hv;
      PFNGLSECONDARYCOLOR3HNVPROC                 glSecondaryColor3h;
      PFNGLSECONDARYCOLOR3HVNVPROC                glSecondaryColor3hv;
      PFNGLTEXCOORD1HNVPROC                       glTexCoord1h;
      PFNGLTEXCOORD1HVNVPROC                      glTexCoord1hv;
      PFNGLTEXCOORD2HNVPROC                       glTexCoord2h;
      PFNGLTEXCOORD2HVNVPROC                      glTexCoord2hv;
      PFNGLTEXCOORD3HNVPROC                       glTexCoord3h;
      PFNGLTEXCOORD3HVNVPROC                      glTexCoord3hv;
      PFNGLTEXCOORD4HNVPROC                       glTexCoord4h;
      PFNGLTEXCOORD4HVNVPROC                      glTexCoord4hv;
      PFNGLVERTEX2HNVPROC                         glVertex2h;
      PFNGLVERTEX2HVNVPROC                        glVertex2hv;
      PFNGLVERTEX3HNVPROC                         glVertex3h;
      PFNGLVERTEX3HVNVPROC                        glVertex3hv;
      PFNGLVERTEX4HNVPROC                         glVertex4h;
      PFNGLVERTEX4HVNVPROC                        glVertex4hv;
      PFNGLVERTEXATTRIB1HNVPROC                   glVertexAttrib1h;
      PFNGLVERTEXATTRIB1HVNVPROC                  glVertexAttrib1hv;
      PFNGLVERTEXATTRIB2HNVPROC                   glVertexAttrib2h;
      PFNGLVERTEXATTRIB2HVNVPROC                  glVertexAttrib2hv;
      PFNGLVERTEXATTRIB3HNVPROC                   glVertexAttrib3h;
      PFNGLVERTEXATTRIB3HVNVPROC                  glVertexAttrib3hv;
      PFNGLVERTEXATTRIB4HNVPROC                   glVertexAttrib4h;
      PFNGLVERTEXATTRIB4HVNVPROC                  glVertexAttrib4hv;
      PFNGLVERTEXATTRIBS1HVNVPROC                 glVertexAttribs1hv;
      PFNGLVERTEXATTRIBS2HVNVPROC                 glVertexAttribs2hv;
      PFNGLVERTEXATTRIBS3HVNVPROC                 glVertexAttribs3hv;
      PFNGLVERTEXATTRIBS4HVNVPROC                 glVertexAttribs4hv;

      //  GL_NV_occlusion_query functions
      PFNGLBEGINOCCLUSIONQUERYNVPROC              glBeginOcclusionQueryNV;
      PFNGLDELETEOCCLUSIONQUERIESNVPROC           glDeleteOcclusionQueriesNV;
      PFNGLENDOCCLUSIONQUERYNVPROC                glEndOcclusionQueryNV;
      PFNGLGENOCCLUSIONQUERIESNVPROC              glGenOcclusionQueriesNV;
      PFNGLGETOCCLUSIONQUERYIVNVPROC              glGetOcclusionQueryivNV;
      PFNGLGETOCCLUSIONQUERYUIVNVPROC             glGetOcclusionQueryuivNV;
      PFNGLISOCCLUSIONQUERYNVPROC                 glIsOcclusionQueryNV;

      //  GL_NV_parameter_buffer_object functions
      PFNGLPROGRAMBUFFERPARAMETERSFVNVPROC        glProgramBufferParametersfv;
      PFNGLPROGRAMBUFFERPARAMETERSIIVNVPROC       glProgramBufferParametersIiv;
      PFNGLPROGRAMBUFFERPARAMETERSIUIVNVPROC      glProgramBufferParametersIuiv;

      //  GL_NV_pixel_data_range functions
      PFNGLFLUSHPIXELDATARANGENVPROC              glFlushPixelDataRange;
      PFNGLPIXELDATARANGENVPROC                   glPixelDataRange;

      //  GL_NV_primitive_restart functions
      PFNGLPRIMITIVERESTARTNVPROC                 glPrimitiveRestart;
      PFNGLPRIMITIVERESTARTINDEXNVPROC            glPrimitiveRestartIndex;

      //  GL_NV_register_combiners functions
      PFNGLCOMBINERPARAMETERFVNVPROC              glCombinerParameterfv;
      PFNGLCOMBINERPARAMETERFNVPROC               glCombinerParameterf;
      PFNGLCOMBINERPARAMETERIVNVPROC              glCombinerParameteriv;
      PFNGLCOMBINERPARAMETERINVPROC               glCombinerParameteri;
      PFNGLCOMBINERINPUTNVPROC                    glCombinerInput;
      PFNGLCOMBINEROUTPUTNVPROC                   glCombinerOutput;
      PFNGLFINALCOMBINERINPUTNVPROC               glFinalCombinerInput;
      PFNGLGETCOMBINERINPUTPARAMETERFVNVPROC      glGetCombinerInputParameterfv;
      PFNGLGETCOMBINERINPUTPARAMETERIVNVPROC      glGetCombinerInputParameteriv;
      PFNGLGETCOMBINEROUTPUTPARAMETERFVNVPROC     glGetCombinerOutputParameterfv;
      PFNGLGETCOMBINEROUTPUTPARAMETERIVNVPROC     glGetCombinerOutputParameteriv;
      PFNGLGETFINALCOMBINERINPUTPARAMETERFVNVPROC glGetFinalCombinerInputParameterfv;
      PFNGLGETFINALCOMBINERINPUTPARAMETERIVNVPROC glGetFinalCombinerInputParameteriv;

      //  GL_NV_register_combiners2 functions
      PFNGLCOMBINERSTAGEPARAMETERFVNVPROC         glCombinerStageParameterfv;
      PFNGLGETCOMBINERSTAGEPARAMETERFVNVPROC      glGetCombinerStageParameterfv;

      //  GL_NV_transform_feedback functions
      PFNGLACTIVEVARYINGNVPROC                    glActiveVaryingNV;
      PFNGLBEGINTRANSFORMFEEDBACKNVPROC           glBeginTransformFeedbackNV;
      PFNGLBINDBUFFERBASENVPROC                   glBindBufferBaseNV;
      PFNGLBINDBUFFEROFFSETNVPROC                 glBindBufferOffsetNV;
      PFNGLBINDBUFFERRANGENVPROC                  glBindBufferRangeNV;
      PFNGLENDTRANSFORMFEEDBACKNVPROC             glEndTransformFeedbackNV;
      PFNGLGETACTIVEVARYINGNVPROC                 glGetActiveVaryingNV;
      PFNGLGETTRANSFORMFEEDBACKVARYINGNVPROC      glGetTransformFeedbackVaryingNV;
      PFNGLGETVARYINGLOCATIONNVPROC               glGetVaryingLocationNV;
      PFNGLTRANSFORMFEEDBACKATTRIBSNVPROC         glTransformFeedbackAttribsNV;
      PFNGLTRANSFORMFEEDBACKVARYINGSNVPROC        glTransformFeedbackVaryingsNV;

      //  GL_NV_vertex_array_range functions
      PFNGLFLUSHVERTEXARRAYRANGENVPROC            glFlushVertexArrayRange;
      PFNGLVERTEXARRAYRANGENVPROC                 glVertexArrayRange;

      //  GL_NV_vertex_program functions
      PFNGLAREPROGRAMSRESIDENTNVPROC              glAreProgramsResidentNV;
      PFNGLBINDPROGRAMNVPROC                      glBindProgramNV;
      PFNGLDELETEPROGRAMSNVPROC                   glDeleteProgramsNV;
      PFNGLEXECUTEPROGRAMNVPROC                   glExecuteProgramNV;
      PFNGLGENPROGRAMSNVPROC                      glGenProgramsNV;
      PFNGLGETPROGRAMPARAMETERDVNVPROC            glGetProgramParameterdvNV;
      PFNGLGETPROGRAMPARAMETERFVNVPROC            glGetProgramParameterfvNV;
      PFNGLGETPROGRAMIVNVPROC                     glGetProgramivNV;
      PFNGLGETPROGRAMSTRINGNVPROC                 glGetProgramStringNV;
      PFNGLGETTRACKMATRIXIVNVPROC                 glGetTrackMatrixivNV;
      PFNGLGETVERTEXATTRIBDVNVPROC                glGetVertexAttribdvNV;
      PFNGLGETVERTEXATTRIBFVNVPROC                glGetVertexAttribfvNV;
      PFNGLGETVERTEXATTRIBIVNVPROC                glGetVertexAttribivNV;
      PFNGLGETVERTEXATTRIBPOINTERVNVPROC          glGetVertexAttribPointervNV;
      PFNGLISPROGRAMNVPROC                        glIsProgramNV;
      PFNGLLOADPROGRAMNVPROC                      glLoadProgramNV;
      PFNGLPROGRAMPARAMETER4DNVPROC               glProgramParameter4dNV;
      PFNGLPROGRAMPARAMETER4DVNVPROC              glProgramParameter4dvNV;
      PFNGLPROGRAMPARAMETER4FNVPROC               glProgramParameter4fNV;
      PFNGLPROGRAMPARAMETER4FVNVPROC              glProgramParameter4fvNV;
      PFNGLPROGRAMPARAMETERS4DVNVPROC             glProgramParameters4dvNV;
      PFNGLPROGRAMPARAMETERS4FVNVPROC             glProgramParameters4fvNV;
      PFNGLREQUESTRESIDENTPROGRAMSNVPROC          glRequestResidentProgramsNV;
      PFNGLTRACKMATRIXNVPROC                      glTrackMatrixNV;
      PFNGLVERTEXATTRIBPOINTERNVPROC              glVertexAttribPointerNV;
      PFNGLVERTEXATTRIB1DNVPROC                   glVertexAttrib1dNV;
      PFNGLVERTEXATTRIB1DVNVPROC                  glVertexAttrib1dvNV;
      PFNGLVERTEXATTRIB1FNVPROC                   glVertexAttrib1fNV;
      PFNGLVERTEXATTRIB1FVNVPROC                  glVertexAttrib1fvNV;
      PFNGLVERTEXATTRIB1SNVPROC                   glVertexAttrib1sNV;
      PFNGLVERTEXATTRIB1SVNVPROC                  glVertexAttrib1svNV;
      PFNGLVERTEXATTRIB2DNVPROC                   glVertexAttrib2dNV;
      PFNGLVERTEXATTRIB2DVNVPROC                  glVertexAttrib2dvNV;
      PFNGLVERTEXATTRIB2FNVPROC                   glVertexAttrib2fNV;
      PFNGLVERTEXATTRIB2FVNVPROC                  glVertexAttrib2fvNV;
      PFNGLVERTEXATTRIB2SNVPROC                   glVertexAttrib2sNV;
      PFNGLVERTEXATTRIB2SVNVPROC                  glVertexAttrib2svNV;
      PFNGLVERTEXATTRIB3DNVPROC                   glVertexAttrib3dNV;
      PFNGLVERTEXATTRIB3DVNVPROC                  glVertexAttrib3dvNV;
      PFNGLVERTEXATTRIB3FNVPROC                   glVertexAttrib3fNV;
      PFNGLVERTEXATTRIB3FVNVPROC                  glVertexAttrib3fvNV;
      PFNGLVERTEXATTRIB3SNVPROC                   glVertexAttrib3sNV;
      PFNGLVERTEXATTRIB3SVNVPROC                  glVertexAttrib3svNV;
      PFNGLVERTEXATTRIB4DNVPROC                   glVertexAttrib4dNV;
      PFNGLVERTEXATTRIB4DVNVPROC                  glVertexAttrib4dvNV;
      PFNGLVERTEXATTRIB4FNVPROC                   glVertexAttrib4fNV;
      PFNGLVERTEXATTRIB4FVNVPROC                  glVertexAttrib4fvNV;
      PFNGLVERTEXATTRIB4SNVPROC                   glVertexAttrib4sNV;
      PFNGLVERTEXATTRIB4SVNVPROC                  glVertexAttrib4svNV;
      PFNGLVERTEXATTRIB4UBNVPROC                  glVertexAttrib4ubNV;
      PFNGLVERTEXATTRIB4UBVNVPROC                 glVertexAttrib4ubvNV;
      PFNGLVERTEXATTRIBS1DVNVPROC                 glVertexAttribs1dvNV;
      PFNGLVERTEXATTRIBS1FVNVPROC                 glVertexAttribs1fvNV;
      PFNGLVERTEXATTRIBS1SVNVPROC                 glVertexAttribs1svNV;
      PFNGLVERTEXATTRIBS2DVNVPROC                 glVertexAttribs2dvNV;
      PFNGLVERTEXATTRIBS2FVNVPROC                 glVertexAttribs2fvNV;
      PFNGLVERTEXATTRIBS2SVNVPROC                 glVertexAttribs2svNV;
      PFNGLVERTEXATTRIBS3DVNVPROC                 glVertexAttribs3dvNV;
      PFNGLVERTEXATTRIBS3FVNVPROC                 glVertexAttribs3fvNV;
      PFNGLVERTEXATTRIBS3SVNVPROC                 glVertexAttribs3svNV;
      PFNGLVERTEXATTRIBS4DVNVPROC                 glVertexAttribs4dvNV;
      PFNGLVERTEXATTRIBS4FVNVPROC                 glVertexAttribs4fvNV;
      PFNGLVERTEXATTRIBS4SVNVPROC                 glVertexAttribs4svNV;
      PFNGLVERTEXATTRIBS4UBVNVPROC                glVertexAttribs4ubvNV;

      // GL_NV_conditional_render
      PFNGLBEGINCONDITIONALRENDERNVPROC           glBeginConditionalRenderNV;
      PFNGLENDCONDITIONALRENDERNVPROC             glEndConditionalRenderNV;

      // GL_NV_present_video
      PFNGLPRESENTFRAMEKEYEDNVPROC                glPresentFrameKeyedNV;
      PFNGLPRESENTFRAMEDUALFILLNVPROC             glPresentFrameDualFillNV;
      PFNGLGETVIDEOIVNVPROC                       glGetVideoivNV;
      PFNGLGETVIDEOUIVNVPROC                      glGetVideouivNV;
      PFNGLGETVIDEOI64VNVPROC                     glGetVideoi64vNV;
      PFNGLGETVIDEOUI64VNVPROC                    glGetVideoui64vNV;

      // GL_NV_vertex_buffer_unified_memory
      PFNGLBUFFERADDRESSRANGENVPROC               glBufferAddressRangeNV;
      PFNGLVERTEXFORMATNVPROC                     glVertexFormatNV;
      PFNGLNORMALFORMATNVPROC                     glNormalFormatNV;
      PFNGLCOLORFORMATNVPROC                      glColorFormatNV;
      PFNGLINDEXFORMATNVPROC                      glIndexFormatNV;
      PFNGLTEXCOORDFORMATNVPROC                   glTexCoordFormatNV;
      PFNGLEDGEFLAGFORMATNVPROC                   glEdgeFlagFormatNV;
      PFNGLSECONDARYCOLORFORMATNVPROC             glSecondaryColorFormatNV;
      PFNGLFOGCOORDFORMATNVPROC                   glFogCoordFormatNV;
      PFNGLVERTEXATTRIBFORMATNVPROC               glVertexAttribFormatNV;
      PFNGLVERTEXATTRIBIFORMATNVPROC              glVertexAttribIFormatNV;
      PFNGLGETINTEGERUI64I_VNVPROC                glGetIntegerui64i_vNV;

      // GL_NV_shader_buffer_load
      PFNGLGETBUFFERPARAMETERUI64VNVPROC          glGetBufferParameterui64vNV;
      PFNGLISBUFFERRESIDENTNVPROC                 glIsBufferResidentNV;
      PFNGLMAKEBUFFERNONRESIDENTNVPROC            glMakeBufferNonResidentNV;
      PFNGLMAKEBUFFERRESIDENTNVPROC               glMakeBufferResidentNV;
      PFNGLGETNAMEDBUFFERPARAMETERUI64VNVPROC     glGetNamedBufferParameterui64vNV;
      PFNGLISNAMEDBUFFERRESIDENTNVPROC            glIsNamedBufferResidentNV;
      PFNGLMAKENAMEDBUFFERNONRESIDENTNVPROC       glMakeNamedBufferNonResidentNV;
      PFNGLMAKENAMEDBUFFERRESIDENTNVPROC          glMakeNamedBufferResidentNV;
      PFNGLGETINTEGERUI64VNVPROC                  glGetIntegerui64vNV;
      PFNGLUNIFORMUI64NVPROC                      glUniformui64NV;
      PFNGLUNIFORMUI64VNVPROC                     glUniformui64vNV;
      PFNGLGETUNIFORMUI64VNVPROC                  glGetUniformui64vNV;
      PFNGLPROGRAMUNIFORMUI64NVPROC               glProgramUniformui64NV;
      PFNGLPROGRAMUNIFORMUI64VNVPROC              glProgramUniformui64vNV;

      // GL_NV_video_capture
      PFNGLBEGINVIDEOCAPTURENVPROC                glBeginVideoCaptureNV;
      PFNGLBINDVIDEOCAPTURESTREAMBUFFERNVPROC     glBindVideoCaptureStreamBufferNV;
      PFNGLBINDVIDEOCAPTURESTREAMTEXTURENVPROC    glBindVideoCaptureStreamTextureNV;
      PFNGLENDVIDEOCAPTURENVPROC                  glEndVideoCaptureNV;
      PFNGLGETVIDEOCAPTUREIVNVPROC                glGetVideoCaptureivNV;
      PFNGLGETVIDEOCAPTURESTREAMIVNVPROC          glGetVideoCaptureStreamivNV;
      PFNGLGETVIDEOCAPTURESTREAMFVNVPROC          glGetVideoCaptureStreamfvNV;
      PFNGLGETVIDEOCAPTURESTREAMDVNVPROC          glGetVideoCaptureStreamdvNV;
      PFNGLVIDEOCAPTURENVPROC                     glVideoCaptureNV;
      PFNGLVIDEOCAPTURESTREAMPARAMETERIVNVPROC    glVideoCaptureStreamParameterivNV;
      PFNGLVIDEOCAPTURESTREAMPARAMETERFVNVPROC    glVideoCaptureStreamParameterfvNV;
      PFNGLVIDEOCAPTURESTREAMPARAMETERDVNVPROC    glVideoCaptureStreamParameterdvNV;

      // GL_EXT_direct_state_access
      PFNGLCLIENTATTRIBDEFAULTEXTPROC                         glClientAttribDefaultEXT;
      PFNGLPUSHCLIENTATTRIBDEFAULTEXTPROC                     glPushClientAttribDefaultEXT;
      PFNGLMATRIXLOADFEXTPROC                                 glMatrixLoadfEXT;
      PFNGLMATRIXLOADDEXTPROC                                 glMatrixLoaddEXT;
      PFNGLMATRIXMULTFEXTPROC                                 glMatrixMultfEXT;
      PFNGLMATRIXMULTDEXTPROC                                 glMatrixMultdEXT;
      PFNGLMATRIXLOADIDENTITYEXTPROC                          glMatrixLoadIdentityEXT;
      PFNGLMATRIXROTATEFEXTPROC                               glMatrixRotatefEXT;
      PFNGLMATRIXROTATEDEXTPROC                               glMatrixRotatedEXT;
      PFNGLMATRIXSCALEFEXTPROC                                glMatrixScalefEXT;
      PFNGLMATRIXSCALEDEXTPROC                                glMatrixScaledEXT;
      PFNGLMATRIXTRANSLATEFEXTPROC                            glMatrixTranslatefEXT;
      PFNGLMATRIXTRANSLATEDEXTPROC                            glMatrixTranslatedEXT;
      PFNGLMATRIXFRUSTUMEXTPROC                               glMatrixFrustumEXT;
      PFNGLMATRIXORTHOEXTPROC                                 glMatrixOrthoEXT;
      PFNGLMATRIXPOPEXTPROC                                   glMatrixPopEXT;
      PFNGLMATRIXPUSHEXTPROC                                  glMatrixPushEXT;
      PFNGLMATRIXLOADTRANSPOSEFEXTPROC                        glMatrixLoadTransposefEXT;
      PFNGLMATRIXLOADTRANSPOSEDEXTPROC                        glMatrixLoadTransposedEXT;
      PFNGLMATRIXMULTTRANSPOSEFEXTPROC                        glMatrixMultTransposefEXT;
      PFNGLMATRIXMULTTRANSPOSEDEXTPROC                        glMatrixMultTransposedEXT;
      PFNGLTEXTUREPARAMETERFEXTPROC                           glTextureParameterfEXT;
      PFNGLTEXTUREPARAMETERFVEXTPROC                          glTextureParameterfvEXT;
      PFNGLTEXTUREPARAMETERIEXTPROC                           glTextureParameteriEXT;
      PFNGLTEXTUREPARAMETERIVEXTPROC                          glTextureParameterivEXT;
      PFNGLTEXTUREIMAGE1DEXTPROC                              glTextureImage1DEXT;
      PFNGLTEXTUREIMAGE2DEXTPROC                              glTextureImage2DEXT;
      PFNGLTEXTURESUBIMAGE1DEXTPROC                           glTextureSubImage1DEXT;
      PFNGLTEXTURESUBIMAGE2DEXTPROC                           glTextureSubImage2DEXT;
      PFNGLCOPYTEXTUREIMAGE1DEXTPROC                          glCopyTextureImage1DEXT;
      PFNGLCOPYTEXTUREIMAGE2DEXTPROC                          glCopyTextureImage2DEXT;
      PFNGLCOPYTEXTURESUBIMAGE1DEXTPROC                       glCopyTextureSubImage1DEXT;
      PFNGLCOPYTEXTURESUBIMAGE2DEXTPROC                       glCopyTextureSubImage2DEXT;
      PFNGLGETTEXTUREIMAGEEXTPROC                             glGetTextureImageEXT;
      PFNGLGETTEXTUREPARAMETERFVEXTPROC                       glGetTextureParameterfvEXT;
      PFNGLGETTEXTUREPARAMETERIVEXTPROC                       glGetTextureParameterivEXT;
      PFNGLGETTEXTURELEVELPARAMETERFVEXTPROC                  glGetTextureLevelParameterfvEXT;
      PFNGLGETTEXTURELEVELPARAMETERIVEXTPROC                  glGetTextureLevelParameterivEXT;
      PFNGLTEXTUREIMAGE3DEXTPROC                              glTextureImage3DEXT;
      PFNGLTEXTURESUBIMAGE3DEXTPROC                           glTextureSubImage3DEXT;
      PFNGLCOPYTEXTURESUBIMAGE3DEXTPROC                       glCopyTextureSubImage3DEXT;
      PFNGLMULTITEXPARAMETERFEXTPROC                          glMultiTexParameterfEXT;
      PFNGLMULTITEXPARAMETERFVEXTPROC                         glMultiTexParameterfvEXT;
      PFNGLMULTITEXPARAMETERIEXTPROC                          glMultiTexParameteriEXT;
      PFNGLMULTITEXPARAMETERIVEXTPROC                         glMultiTexParameterivEXT;
      PFNGLMULTITEXIMAGE1DEXTPROC                             glMultiTexImage1DEXT;
      PFNGLMULTITEXIMAGE2DEXTPROC                             glMultiTexImage2DEXT;
      PFNGLMULTITEXSUBIMAGE1DEXTPROC                          glMultiTexSubImage1DEXT;
      PFNGLMULTITEXSUBIMAGE2DEXTPROC                          glMultiTexSubImage2DEXT;
      PFNGLCOPYMULTITEXIMAGE1DEXTPROC                         glCopyMultiTexImage1DEXT;
      PFNGLCOPYMULTITEXIMAGE2DEXTPROC                         glCopyMultiTexImage2DEXT;
      PFNGLCOPYMULTITEXSUBIMAGE1DEXTPROC                      glCopyMultiTexSubImage1DEXT;
      PFNGLCOPYMULTITEXSUBIMAGE2DEXTPROC                      glCopyMultiTexSubImage2DEXT;
      PFNGLGETMULTITEXIMAGEEXTPROC                            glGetMultiTexImageEXT;
      PFNGLGETMULTITEXPARAMETERFVEXTPROC                      glGetMultiTexParameterfvEXT;
      PFNGLGETMULTITEXPARAMETERIVEXTPROC                      glGetMultiTexParameterivEXT;
      PFNGLGETMULTITEXLEVELPARAMETERFVEXTPROC                 glGetMultiTexLevelParameterfvEXT;
      PFNGLGETMULTITEXLEVELPARAMETERIVEXTPROC                 glGetMultiTexLevelParameterivEXT;
      PFNGLMULTITEXIMAGE3DEXTPROC                             glMultiTexImage3DEXT;
      PFNGLMULTITEXSUBIMAGE3DEXTPROC                          glMultiTexSubImage3DEXT;
      PFNGLCOPYMULTITEXSUBIMAGE3DEXTPROC                      glCopyMultiTexSubImage3DEXT;
      PFNGLBINDMULTITEXTUREEXTPROC                            glBindMultiTextureEXT;
      PFNGLENABLECLIENTSTATEINDEXEDEXTPROC                    glEnableClientStateIndexedEXT;
      PFNGLDISABLECLIENTSTATEINDEXEDEXTPROC                   glDisableClientStateIndexedEXT;
      PFNGLMULTITEXCOORDPOINTEREXTPROC                        glMultiTexCoordPointerEXT;
      PFNGLMULTITEXENVFEXTPROC                                glMultiTexEnvfEXT;
      PFNGLMULTITEXENVFVEXTPROC                               glMultiTexEnvfvEXT;
      PFNGLMULTITEXENVIEXTPROC                                glMultiTexEnviEXT;
      PFNGLMULTITEXENVIVEXTPROC                               glMultiTexEnvivEXT;
      PFNGLMULTITEXGENDEXTPROC                                glMultiTexGendEXT;
      PFNGLMULTITEXGENDVEXTPROC                               glMultiTexGendvEXT;
      PFNGLMULTITEXGENFEXTPROC                                glMultiTexGenfEXT;
      PFNGLMULTITEXGENFVEXTPROC                               glMultiTexGenfvEXT;
      PFNGLMULTITEXGENIEXTPROC                                glMultiTexGeniEXT;
      PFNGLMULTITEXGENIVEXTPROC                               glMultiTexGenivEXT;
      PFNGLGETMULTITEXENVFVEXTPROC                            glGetMultiTexEnvfvEXT;
      PFNGLGETMULTITEXENVIVEXTPROC                            glGetMultiTexEnvivEXT;
      PFNGLGETMULTITEXGENDVEXTPROC                            glGetMultiTexGendvEXT;
      PFNGLGETMULTITEXGENFVEXTPROC                            glGetMultiTexGenfvEXT;
      PFNGLGETMULTITEXGENIVEXTPROC                            glGetMultiTexGenivEXT;
      PFNGLGETFLOATINDEXEDVEXTPROC                            glGetFloatIndexedvEXT;
      PFNGLGETDOUBLEINDEXEDVEXTPROC                           glGetDoubleIndexedvEXT;
      PFNGLGETPOINTERINDEXEDVEXTPROC                          glGetPointerIndexedvEXT;
      PFNGLCOMPRESSEDTEXTUREIMAGE3DEXTPROC                    glCompressedTextureImage3DEXT;
      PFNGLCOMPRESSEDTEXTUREIMAGE2DEXTPROC                    glCompressedTextureImage2DEXT;
      PFNGLCOMPRESSEDTEXTUREIMAGE1DEXTPROC                    glCompressedTextureImage1DEXT;
      PFNGLCOMPRESSEDTEXTURESUBIMAGE3DEXTPROC                 glCompressedTextureSubImage3DEXT;
      PFNGLCOMPRESSEDTEXTURESUBIMAGE2DEXTPROC                 glCompressedTextureSubImage2DEXT;
      PFNGLCOMPRESSEDTEXTURESUBIMAGE1DEXTPROC                 glCompressedTextureSubImage1DEXT;
      PFNGLGETCOMPRESSEDTEXTUREIMAGEEXTPROC                   glGetCompressedTextureImageEXT;
      PFNGLCOMPRESSEDMULTITEXIMAGE3DEXTPROC                   glCompressedMultiTexImage3DEXT;
      PFNGLCOMPRESSEDMULTITEXIMAGE2DEXTPROC                   glCompressedMultiTexImage2DEXT;
      PFNGLCOMPRESSEDMULTITEXIMAGE1DEXTPROC                   glCompressedMultiTexImage1DEXT;
      PFNGLCOMPRESSEDMULTITEXSUBIMAGE3DEXTPROC                glCompressedMultiTexSubImage3DEXT;
      PFNGLCOMPRESSEDMULTITEXSUBIMAGE2DEXTPROC                glCompressedMultiTexSubImage2DEXT;
      PFNGLCOMPRESSEDMULTITEXSUBIMAGE1DEXTPROC                glCompressedMultiTexSubImage1DEXT;
      PFNGLGETCOMPRESSEDMULTITEXIMAGEEXTPROC                  glGetCompressedMultiTexImageEXT;
      PFNGLNAMEDPROGRAMSTRINGEXTPROC                          glNamedProgramStringEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETER4DEXTPROC                glNamedProgramLocalParameter4dEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETER4DVEXTPROC               glNamedProgramLocalParameter4dvEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETER4FEXTPROC                glNamedProgramLocalParameter4fEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETER4FVEXTPROC               glNamedProgramLocalParameter4fvEXT;
      PFNGLGETNAMEDPROGRAMLOCALPARAMETERDVEXTPROC             glGetNamedProgramLocalParameterdvEXT;
      PFNGLGETNAMEDPROGRAMLOCALPARAMETERFVEXTPROC             glGetNamedProgramLocalParameterfvEXT;
      PFNGLGETNAMEDPROGRAMIVEXTPROC                           glGetNamedProgramivEXT;
      PFNGLGETNAMEDPROGRAMSTRINGEXTPROC                       glGetNamedProgramStringEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETERS4FVEXTPROC              glNamedProgramLocalParameters4fvEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETERI4IEXTPROC               glNamedProgramLocalParameterI4iEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETERI4IVEXTPROC              glNamedProgramLocalParameterI4ivEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETERSI4IVEXTPROC             glNamedProgramLocalParametersI4ivEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIEXTPROC              glNamedProgramLocalParameterI4uiEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETERI4UIVEXTPROC             glNamedProgramLocalParameterI4uivEXT;
      PFNGLNAMEDPROGRAMLOCALPARAMETERSI4UIVEXTPROC            glNamedProgramLocalParametersI4uivEXT;
      PFNGLGETNAMEDPROGRAMLOCALPARAMETERIIVEXTPROC            glGetNamedProgramLocalParameterIivEXT;
      PFNGLGETNAMEDPROGRAMLOCALPARAMETERIUIVEXTPROC           glGetNamedProgramLocalParameterIuivEXT;
      PFNGLTEXTUREPARAMETERIIVEXTPROC                         glTextureParameterIivEXT;
      PFNGLTEXTUREPARAMETERIUIVEXTPROC                        glTextureParameterIuivEXT;
      PFNGLGETTEXTUREPARAMETERIIVEXTPROC                      glGetTextureParameterIivEXT;
      PFNGLGETTEXTUREPARAMETERIUIVEXTPROC                     glGetTextureParameterIuivEXT;
      PFNGLMULTITEXPARAMETERIIVEXTPROC                        glMultiTexParameterIivEXT;
      PFNGLMULTITEXPARAMETERIUIVEXTPROC                       glMultiTexParameterIuivEXT;
      PFNGLGETMULTITEXPARAMETERIIVEXTPROC                     glGetMultiTexParameterIivEXT;
      PFNGLGETMULTITEXPARAMETERIUIVEXTPROC                    glGetMultiTexParameterIuivEXT;
      PFNGLPROGRAMUNIFORM1FEXTPROC                            glProgramUniform1fEXT;
      PFNGLPROGRAMUNIFORM2FEXTPROC                            glProgramUniform2fEXT;
      PFNGLPROGRAMUNIFORM3FEXTPROC                            glProgramUniform3fEXT;
      PFNGLPROGRAMUNIFORM4FEXTPROC                            glProgramUniform4fEXT;
      PFNGLPROGRAMUNIFORM1IEXTPROC                            glProgramUniform1iEXT;
      PFNGLPROGRAMUNIFORM2IEXTPROC                            glProgramUniform2iEXT;
      PFNGLPROGRAMUNIFORM3IEXTPROC                            glProgramUniform3iEXT;
      PFNGLPROGRAMUNIFORM4IEXTPROC                            glProgramUniform4iEXT;
      PFNGLPROGRAMUNIFORM1FVEXTPROC                           glProgramUniform1fvEXT;
      PFNGLPROGRAMUNIFORM2FVEXTPROC                           glProgramUniform2fvEXT;
      PFNGLPROGRAMUNIFORM3FVEXTPROC                           glProgramUniform3fvEXT;
      PFNGLPROGRAMUNIFORM4FVEXTPROC                           glProgramUniform4fvEXT;
      PFNGLPROGRAMUNIFORM1IVEXTPROC                           glProgramUniform1ivEXT;
      PFNGLPROGRAMUNIFORM2IVEXTPROC                           glProgramUniform2ivEXT;
      PFNGLPROGRAMUNIFORM3IVEXTPROC                           glProgramUniform3ivEXT;
      PFNGLPROGRAMUNIFORM4IVEXTPROC                           glProgramUniform4ivEXT;
      PFNGLPROGRAMUNIFORMMATRIX2FVEXTPROC                     glProgramUniformMatrix2fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX3FVEXTPROC                     glProgramUniformMatrix3fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX4FVEXTPROC                     glProgramUniformMatrix4fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX2X3FVEXTPROC                   glProgramUniformMatrix2x3fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX3X2FVEXTPROC                   glProgramUniformMatrix3x2fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX2X4FVEXTPROC                   glProgramUniformMatrix2x4fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX4X2FVEXTPROC                   glProgramUniformMatrix4x2fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX3X4FVEXTPROC                   glProgramUniformMatrix3x4fvEXT;
      PFNGLPROGRAMUNIFORMMATRIX4X3FVEXTPROC                   glProgramUniformMatrix4x3fvEXT;
      PFNGLPROGRAMUNIFORM1UIEXTPROC                           glProgramUniform1uiEXT;
      PFNGLPROGRAMUNIFORM2UIEXTPROC                           glProgramUniform2uiEXT;
      PFNGLPROGRAMUNIFORM3UIEXTPROC                           glProgramUniform3uiEXT;
      PFNGLPROGRAMUNIFORM4UIEXTPROC                           glProgramUniform4uiEXT;
      PFNGLPROGRAMUNIFORM1UIVEXTPROC                          glProgramUniform1uivEXT;
      PFNGLPROGRAMUNIFORM2UIVEXTPROC                          glProgramUniform2uivEXT;
      PFNGLPROGRAMUNIFORM3UIVEXTPROC                          glProgramUniform3uivEXT;
      PFNGLPROGRAMUNIFORM4UIVEXTPROC                          glProgramUniform4uivEXT;
      PFNGLNAMEDBUFFERDATAEXTPROC                             glNamedBufferDataEXT;
      PFNGLNAMEDBUFFERSUBDATAEXTPROC                          glNamedBufferSubDataEXT;
      PFNGLMAPNAMEDBUFFEREXTPROC                              glMapNamedBufferEXT;
      PFNGLUNMAPNAMEDBUFFEREXTPROC                            glUnmapNamedBufferEXT;
      PFNGLGETNAMEDBUFFERPARAMETERIVEXTPROC                   glGetNamedBufferParameterivEXT;
      PFNGLGETNAMEDBUFFERPOINTERVEXTPROC                      glGetNamedBufferPointervEXT;
      PFNGLGETNAMEDBUFFERSUBDATAEXTPROC                       glGetNamedBufferSubDataEXT;
      PFNGLTEXTUREBUFFEREXTPROC                               glTextureBufferEXT;
      PFNGLMULTITEXBUFFEREXTPROC                              glMultiTexBufferEXT;
      PFNGLNAMEDRENDERBUFFERSTORAGEEXTPROC                    glNamedRenderbufferStorageEXT;
      PFNGLGETNAMEDRENDERBUFFERPARAMETERIVEXTPROC             glGetNamedRenderbufferParameterivEXT;
      PFNGLCHECKNAMEDFRAMEBUFFERSTATUSEXTPROC                 glCheckNamedFramebufferStatusEXT;
      PFNGLNAMEDFRAMEBUFFERTEXTURE1DEXTPROC                   glNamedFramebufferTexture1DEXT;
      PFNGLNAMEDFRAMEBUFFERTEXTURE2DEXTPROC                   glNamedFramebufferTexture2DEXT;
      PFNGLNAMEDFRAMEBUFFERTEXTURE3DEXTPROC                   glNamedFramebufferTexture3DEXT;
      PFNGLNAMEDFRAMEBUFFERRENDERBUFFEREXTPROC                glNamedFramebufferRenderbufferEXT;
      PFNGLGETNAMEDFRAMEBUFFERATTACHMENTPARAMETERIVEXTPROC    glGetNamedFramebufferAttachmentParameterivEXT;
      PFNGLGENERATETEXTUREMIPMAPEXTPROC                       glGenerateTextureMipmapEXT;
      PFNGLGENERATEMULTITEXMIPMAPEXTPROC                      glGenerateMultiTexMipmapEXT;
      PFNGLFRAMEBUFFERDRAWBUFFEREXTPROC                       glFramebufferDrawBufferEXT;
      PFNGLFRAMEBUFFERDRAWBUFFERSEXTPROC                      glFramebufferDrawBuffersEXT;
      PFNGLFRAMEBUFFERREADBUFFEREXTPROC                       glFramebufferReadBufferEXT;
      PFNGLGETFRAMEBUFFERPARAMETERIVEXTPROC                   glGetFramebufferParameterivEXT;
      PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLEEXTPROC         glNamedRenderbufferStorageMultisampleEXT;
      PFNGLNAMEDRENDERBUFFERSTORAGEMULTISAMPLECOVERAGEEXTPROC glNamedRenderbufferStorageMultisampleCoverageEXT;
      PFNGLNAMEDFRAMEBUFFERTEXTUREEXTPROC                     glNamedFramebufferTextureEXT;
      PFNGLNAMEDFRAMEBUFFERTEXTURELAYEREXTPROC                glNamedFramebufferTextureLayerEXT;
      PFNGLNAMEDFRAMEBUFFERTEXTUREFACEEXTPROC                 glNamedFramebufferTextureFaceEXT;
      PFNGLTEXTURERENDERBUFFEREXTPROC                         glTextureRenderbufferEXT;
      PFNGLMULTITEXRENDERBUFFEREXTPROC                        glMultiTexRenderbufferEXT;

#endif  //  DOXYGEN_IGNORE

    private:
      int         m_glVersion;
      std::set<std::string> m_available;
      std::set<std::string> m_exported;
  };

  inline  void  GLFunctions::disableAvailability( const std::string& extension )
  {
     m_available.erase( extension );
  }

  inline  bool  GLFunctions::isAvailable( const std::string& extension ) const
  {
    return( m_available.find( extension ) != m_available.end() );
  }

  inline  bool   GLFunctions::isExported( const std::string& extension ) const
  {
    return( m_exported.find( extension ) != m_exported.end() );
  }
  
  inline int GLFunctions::getVersion() const
  {
    return m_glVersion;
  }



#if defined(_WIN32)

  class WGLFunctions
  {
    public:
      //! Constructor
      WGLFunctions( void );

      //! Destructor
      ~WGLFunctions( void );
      
      /*! Initialization of WGL extension function pointers.
       *  Only returns false if the wglGetExtensionStringEXT pointer is NULL,
       *  which indicates that Init() has been called while an OpenGL context
       *  has been current which doesn't implement any current WGL extension.
       *  Applications only using the granted Win32 WGL-functions would still work. */
      NVSG_API bool Init(void);

      /*! Reset WGLFunctions function pointers and isInitialized,
       *  in case someone actually wants to call Init() more than once. */
      NVSG_API void Reset(void);

      //! Test if a given WGL extension is available on the current context.
      /** An OpenGL extension is not only exported, but also available, if all the functions required for that
        * extension are exported.
        * \note Not all exported extensions might be available.
        * \returns  \c true, if the \a extension is available, otherwise \c false  */
      NVSG_API bool isAvailable( const std::string& extension   //!< extension to test
                               ) const;

      //! Test if a given extension is exported on the current context.
      /** An OpenGL WGL extension is exported, if it is part of the return value of \c wglGetExtensionsStringEXT()
        * \note Not all exported extensions might be available.
        * \returns \c true, if \a extension is exported, otherwise c\ false  */
      NVSG_API bool isExported( const std::string& extension    //!< extension to test
                              ) const;

    public:

//  Don't need to document all these wrapper functions !!!
#if ! defined( DOXYGEN_IGNORE )

      //  WGL_ARB_buffer_region functions
      PFNWGLCREATEBUFFERREGIONARBPROC             wglCreateBufferRegionARB;
      PFNWGLDELETEBUFFERREGIONARBPROC             wglDeleteBufferRegionARB;
      PFNWGLRESTOREBUFFERREGIONARBPROC            wglRestoreBufferRegionARB;
      PFNWGLSAVEBUFFERREGIONARBPROC               wglSaveBufferRegionARB;

      //  WGL_ARB_extensions_string functions
      PFNWGLGETEXTENSIONSSTRINGARBPROC            wglGetExtensionsStringARB;

      //  WGL_ARB_make_current_read functions
      PFNWGLMAKECONTEXTCURRENTARBPROC             wglMakeContextCurrentARB;
      PFNWGLGETCURRENTREADDCARBPROC               wglGetCurrentReadDCARB;

      //  WGL_ARB_pbuffer functions
      PFNWGLCREATEPBUFFERARBPROC                  wglCreatePbufferARB;
      PFNWGLDESTROYPBUFFERARBPROC                 wglDestroyPbufferARB;
      PFNWGLGETPBUFFERDCARBPROC                   wglGetPbufferDCARB;
      PFNWGLQUERYPBUFFERARBPROC                   wglQueryPbufferARB;
      PFNWGLRELEASEPBUFFERDCARBPROC               wglReleasePbufferDCARB;

      //  WGL_ARB_pixel_format functions
      PFNWGLCHOOSEPIXELFORMATARBPROC              wglChoosePixelFormatARB;
      PFNWGLGETPIXELFORMATATTRIBIVARBPROC         wglGetPixelFormatAttribivARB;
      PFNWGLGETPIXELFORMATATTRIBFVARBPROC         wglGetPixelFormatAttribfvARB;

      //  WGL_ARB_render_texture functions
      PFNWGLBINDTEXIMAGEARBPROC                   wglBindTexImageARB;
      PFNWGLRELEASETEXIMAGEARBPROC                wglReleaseTexImageARB;
      PFNWGLSETPBUFFERATTRIBARBPROC               wglSetPbufferAttribARB;

      //  WGL_EXT_extensions_string functions
      PFNWGLGETEXTENSIONSSTRINGEXTPROC            wglGetExtensionsStringEXT;

      //  WGL_EXT_genlock functions
      PFNWGLENABLEGENLOCKEXTPROC                  wglEnableGenlockEXT;
      PFNWGLDISABLEGENLOCKEXTPROC                 wglDisableGenlockEXT;
      PFNWGLGENLOCKSOURCEEXTPROC                  wglGenlockSourceEXT;
      PFNWGLGENLOCKSAMPLERATEEXTPROC              wglGenlockSampleRateEXT;
      PFNWGLGENLOCKSOURCEDELAYEXTPROC             wglGenlockSourceDelayEXT;
      PFNWGLGENLOCKSOURCEEDGEEXTPROC              wglGenlockSourceEdgeEXT;
      PFNWGLGETGENLOCKSOURCEEXTPROC               wglGetGenlockSourceEXT;
      PFNWGLGETGENLOCKSAMPLERATEEXTPROC           wglGetGenlockSampleRateEXT;
      PFNWGLGETGENLOCKSOURCEDELAYEXTPROC          wglGetGenlockSourceDelayEXT;
      PFNWGLGETGENLOCKSOURCEEDGEEXTPROC           wglGetGenlockSourceEdgeEXT;
      PFNWGLISENABLEDGENLOCKEXTPROC               wglIsEnabledGenlockEXT;
      PFNWGLQUERYGENLOCKMAXSOURCEDELAYEXTPROC     wglQueryGenlockMaxSourceDelayEXT;

      //  WGL_EXT_swap_control functions
      PFNWGLSWAPINTERVALEXTPROC                   wglSwapIntervalEXT;
      PFNWGLGETSWAPINTERVALEXTPROC                wglGetSwapIntervalEXT;

      //  WGL_EXT_swap_frame_lock functions
      PFNWGLENABLEFRAMELOCKEXTPROC                wglEnableFrameLockEXT;
      PFNWGLDISABLEFRAMELOCKEXTPROC               wglDisableFrameLockEXT;
      PFNWGLISENABLEDFRAMELOCKEXTPROC             wglIsEnabledFrameLockEXT;
      PFNWGLQUERYFRAMELOCKMASTEREXTPROC           wglQueryFrameLockMasterEXT;

      //  WGL_I3D_genlock functions
      PFNWGLENABLEGENLOCKI3DPROC                  wglEnableGenlockI3D;
      PFNWGLDISABLEGENLOCKI3DPROC                 wglDisableGenlockI3D;
      PFNWGLISENABLEDGENLOCKI3DPROC               wglIsEnabledGenlockI3D;
      PFNWGLGENLOCKSOURCEI3DPROC                  wglGenlockSourceI3D;
      PFNWGLGETGENLOCKSOURCEI3DPROC               wglGetGenlockSourceI3D;
      PFNWGLGENLOCKSOURCEEDGEI3DPROC              wglGenlockSourceEdgeI3D;
      PFNWGLGETGENLOCKSOURCEEDGEI3DPROC           wglGetGenlockSourceEdgeI3D;
      PFNWGLGENLOCKSAMPLERATEI3DPROC              wglGenlockSampleRateI3D;
      PFNWGLGETGENLOCKSAMPLERATEI3DPROC           wglGetGenlockSampleRateI3D;
      PFNWGLGENLOCKSOURCEDELAYI3DPROC             wglGenlockSourceDelayI3D;
      PFNWGLGETGENLOCKSOURCEDELAYI3DPROC          wglGetGenlockSourceDelayI3D;
      PFNWGLQUERYGENLOCKMAXSOURCEDELAYI3DPROC     wglQueryGenlockMaxSourceDelayI3D;

      //  WGL_KTX_buffer_region functions
      PFNWGLNEWBUFFERREGIONPROC                   wglNewBufferRegion;
      PFNWGLDELETEBUFFERREGIONPROC                wglDeleteBufferRegion;
      PFNWGLREADBUFFERREGIONPROC                  wglReadBufferRegion;
      PFNWGLDRAWBUFFERREGIONPROC                  wglDrawBufferRegion;
      PFNWGLBUFFERREGIONENABLEDPROC               wglBufferRegionEnabled;

      //  WGL_NV_allocate_memory functions
      PFNWGLALLOCATEMEMORYNVPROC                  wglAllocateMemoryNV;
      PFNWGLFREEMEMORYNVPROC                      wglFreeMemoryNV;

      //  WGL_NV_swap_group functions
      PFNWGLBINDSWAPBARRIERNVPROC                 wglBindSwapBarrierNV;
      PFNWGLJOINSWAPGROUPNVPROC                   wglJoinSwapGroupNV;
      PFNWGLQUERYSWAPGROUPNVPROC                  wglQuerySwapGroupNV;
      PFNWGLQUERYMAXSWAPGROUPSNVPROC              wglQueryMaxSwapGroupsNV;
      PFNWGLQUERYFRAMECOUNTNVPROC                 wglQueryFrameCountNV;
      PFNWGLRESETFRAMECOUNTNVPROC                 wglResetFrameCountNV;
    
      // WGL_NV_gpu_affinity
      PFNWGLENUMGPUSNVPROC                        wglEnumGpusNV;
      PFNWGLENUMGPUDEVICESNVPROC                  wglEnumGpuDevicesNV;
      PFNWGLCREATEAFFINITYDCNVPROC                wglCreateAffinityDCNV;
      PFNWGLENUMGPUSFROMAFFINITYDCNVPROC          wglEnumGpusFromAffinityDCNV;
      PFNWGLDELETEDCNVPROC                        wglDeleteDCNV;

      // WGL_NV_video_output
      PFNWGLGETVIDEODEVICENVPROC                  wglGetVideoDeviceNV;
      PFNWGLRELEASEVIDEODEVICENVPROC              wglReleaseVideoDeviceNV;
      PFNWGLBINDVIDEOIMAGENVPROC                  wglBindVideoImageNV;
      PFNWGLRELEASEVIDEOIMAGENVPROC               wglReleaseVideoImageNV;
      PFNWGLSENDPBUFFERTOVIDEONVPROC              wglSendPbufferToVideoNV;
      PFNWGLGETVIDEOINFONVPROC                    wglGetVideoInfoNV;

      // WGL_NV_present_video
      PFNWGLENUMERATEVIDEODEVICESNVPROC           wglEnumerateVideoDevicesNV;
      PFNWGLBINDVIDEODEVICENVPROC                 wglBindVideoDeviceNV;
      PFNWGLQUERYCURRENTCONTEXTNVPROC             wglQueryCurrentContextNV;

      // WGL_NV_video_capture
      PFNWGLBINDVIDEOCAPTUREDEVICENVPROC          wglBindVideoCaptureDeviceNV;
      PFNWGLENUMERATEVIDEOCAPTUREDEVICESNVPROC    wglEnumerateVideoCaptureDevicesNV;
      PFNWGLLOCKVIDEOCAPTUREDEVICENVPROC          wglLockVideoCaptureDeviceNV;
      PFNWGLQUERYVIDEOCAPTUREDEVICENVPROC         wglQueryVideoCaptureDeviceNV;
      PFNWGLRELEASEVIDEOCAPTUREDEVICENVPROC       wglReleaseVideoCaptureDeviceNV;


#endif  //  DOXYGEN_IGNORE

    private:
      std::set<std::string> m_available;
      std::set<std::string> m_exported;
      bool                  isInitialized;
  };

  typedef nvutil::Singleton<WGLFunctions> theWGLFunctions;
  // Usage WGLFunctions * pfWGL = theWGLFunctions::instance();

  inline  bool  WGLFunctions::isAvailable( const std::string& extension ) const
  {
    return( m_available.find( extension ) != m_available.end() );
  }

  inline  bool  WGLFunctions::isExported( const std::string& extension ) const
  {
    return( m_exported.find( extension ) != m_exported.end() );
  }

#endif // _WIN32

#if defined(LINUX)

  class GLXFunctions
  {
    public:
      //! Constructor
      GLXFunctions( void );

      //! Destructor
      ~GLXFunctions( void );
      
      /*! Initialization of GLX extension function pointers.
       *  Only returns false if the glXQueryExtensionString pointer is NULL,
       *  which indicates that Init() has been called while an OpenGL context
       *  has been current which doesn't implement any current GLX extension.
       *  Applications only using the granted Win32 GLX-functions would still work. */
      NVSG_API bool Init(Display * display);

      /*! Reset GLXFunctions function pointers and isInitialized,
       *  in case someone actually wants to call Init() more than once. */
      NVSG_API void Reset(void);

      //! Test if a given GLX extension is available on the current context.
      /** An OpenGL extension is not only exported, but also available, if all the functions required for that
        * extension are exported.
        * \note Not all exported extensions might be available.
        * \returns  \c true, if the \a extension is available, otherwise \c false  */
      NVSG_API bool isAvailable( const std::string& extension   //!< extension to test
                               ) const;

      //! Test if a given extension is exported on the current context.
      /** An OpenGL GLX extension is exported, if it is part of the return value of \c glXQueryExtensionsString()
        * \note Not all exported extensions might be available.
        * \returns \c true, if \a extension is exported, otherwise c\ false  */
      NVSG_API bool isExported( const std::string& extension    //!< extension to test
                              ) const;

    public:

//  Don't need to document all these wrapper functions !!!
#if ! defined( DOXYGEN_IGNORE )

  // GLX Version 1.3 - REQUIRED
  // GLX_SGI_make_current_read - rolled into 1.3
  // GLX_EXT_import_context - rolled into 1.3
  // GLX_SGIX_fbconfig - rolled into glx 1.3
  // GLX_SGIX_pbuffer - rolled into glx 1.3
  // no need to export these extensions

  // GLX_ARB_get_proc_address - our code already assumes this is present
  // GLX Version 1.4 - added glXGetProcAddress - not exposed here

  // GLX_SGI_swap_control
  PFNGLXSWAPINTERVALSGIPROC         glXSwapIntervalSGI;

  // GLX_SGI_video_sync
  PFNGLXGETVIDEOSYNCSGIPROC         glXGetVideoSyncSGI;
  PFNGLXWAITVIDEOSYNCSGIPROC        glXWaitVideoSyncSGI;

  // GLX_NV_swap_group
  PFNGLXJOINSWAPGROUPNVPROC         glXJoinSwapGroupNV;
  PFNGLXBINDSWAPBARRIERNVPROC       glXBindSwapBarrierNV;
  PFNGLXQUERYSWAPGROUPNVPROC        glXQuerySwapGroupNV;
  PFNGLXQUERYMAXSWAPGROUPSNVPROC    glXQueryMaxSwapGroupsNV;
  PFNGLXQUERYFRAMECOUNTNVPROC       glXQueryFrameCountNV;
  PFNGLXRESETFRAMECOUNTNVPROC       glXResetFrameCountNV;

  // GLX_NV_video_out
  PFNGLXGETVIDEODEVICENVPROC        glXGetVideoDeviceNV;
  PFNGLXRELEASEVIDEODEVICENVPROC    glXReleaseVideoDeviceNV;
  PFNGLXBINDVIDEOIMAGENVPROC        glXBindVideoImageNV;
  PFNGLXRELEASEVIDEOIMAGENVPROC     glXReleaseVideoImageNV;
  PFNGLXSENDPBUFFERTOVIDEONVPROC    glXSendPbufferToVideoNV;
  PFNGLXGETVIDEOINFONVPROC          glXGetVideoInfoNV;

  // GLX_EXT_texture_from_pixmap
  PFNGLXBINDTEXIMAGEEXTPROC         glXBindTexImageEXT;
  PFNGLXRELEASETEXIMAGEEXTPROC      glXReleaseTexImageEXT;

  // GLX_NV_present_video
  PFNGLXENUMERATEVIDEODEVICESNVPROC glXEnumerateVideoDevicesNV;
  PFNGLXBINDVIDEODEVICENVPROC       glXBindVideoDeviceNV;

  // GLX_NV_video_capture
  PFNGLXBINDVIDEOCAPTUREDEVICENVPROC       glXBindVideoCaptureDeviceNV;
  PFNGLXENUMERATEVIDEOCAPTUREDEVICESNVPROC glXEnumerateVideoCaptureDevicesNV;
  PFNGLXLOCKVIDEOCAPTUREDEVICENVPROC       glXLockVideoCaptureDeviceNV;
  PFNGLXQUERYVIDEOCAPTUREDEVICENVPROC      glXQueryVideoCaptureDeviceNV;
  PFNGLXRELEASEVIDEOCAPTUREDEVICENVPROC    glXReleaseVideoCaptureDeviceNV;

#endif  //  DOXYGEN_IGNORE

    private:
      std::set<std::string> m_available;
      std::set<std::string> m_exported;
      bool                  isInitialized;
  };

  typedef nvutil::Singleton<GLXFunctions> theGLXFunctions;
  // Usage GLXFunctions * pfGLX = theGLXFunctions::instance();

  inline  bool  GLXFunctions::isAvailable( const std::string& extension ) const
  {
    return( m_available.find( extension ) != m_available.end() );
  }

  inline  bool  GLXFunctions::isExported( const std::string& extension ) const
  {
    return( m_exported.find( extension ) != m_exported.end() );
  }

#endif // LINUX


} // namespace nvgl

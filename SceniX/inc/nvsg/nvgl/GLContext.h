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
#include "nvgl/GLStatistics.h"
#include "nvmath/Vecnt.h"
#include "nvutil/RCObject.h"
#include "nvutil/STLExt.h"
#include "nvsg/TextureAttribute.h"
#include "nvsg/TextureHost.h"
#include "nvsg/CgFx.h"
#include <map>
#include <stack>
#include <string>
#include <algorithm>
#include <utility>


// common device context helper data types for Windows and Linux
#if defined(_WIN32)
typedef HDC   DEVICE_CONTEXT; //!< Helper type that maps to HDC on Windows and Drawable on Linux.
typedef HGLRC RENDER_CONTEXT; //!< Helper type that maps to HGLRC on Windows and GLXContext on Linux.
#elif defined(LINUX)
typedef Drawable   DEVICE_CONTEXT;
typedef GLXContext RENDER_CONTEXT;
#endif

namespace nvsg
{
  class DALHost;
}

namespace nvgl
{
class GLFunctions;
class GLObjects;
struct VBOCachingState;
struct GLTextureCache;
struct GLDisplayListCache;
struct GLArrayBufferCache;
struct GLElementArrayBufferCache;
struct GLPixelBufferCache;

#if !defined(DOXYGEN_IGNORE)
typedef void (*PFNUPLOADTEXTUREPROC)(GLint, GLint, GLint, nvsg::TextureHost*, GLTextureCache*);
#endif // DOXYGEN_IGNORE

// common device context helper function for Windows and Linux
#if defined(_WIN32)

class WGLFunctions;

//! Get the current device context.
/** This is a helper macro defined for both Windows and Linux. It maps to either WGLGetCurrentDC or GLXGetCurrentDrawable. */
# define GET_CURRENT_DC()    WGLGetCurrentDC()
//! Get the current render context.
/** This is a helper macro defined for both Windows and Linux. It maps to either WGLGetCurrentContext or GLXGetCurrentContext. */
# define GET_CURRENT_RC()    WGLGetCurrentContext()
//! Make the given device context and render context current.
/** This is a helper macro defined for both Windows and Linux. It maps to either WGLMakeCurrent or GLXMakeCurrent. */
# define MAKE_CURRENT(DC,RC) WGLMakeCurrent(DC,RC)

#elif defined(LINUX)

class GLXFunctions;

# define GET_CURRENT_DC()    GLXGetCurrentDrawable()
# define GET_CURRENT_RC()    GLXGetCurrentContext()
# define MAKE_CURRENT(DC,RC) GLXMakeCurrent(glContext()->getDisplay(),DC,RC)

#else

# error Unsupported Operating System!  

#endif

#if !defined(DOXYGEN_IGNORE)
//# define GL_CACHING
# if defined( GL_CACHING )
#  define GL_SUPPORT_RESET
# endif
#endif // DOXYGEN_IGNORE

//! GLTexImageFmt specifies the format and data type of a texture image.
struct GLTexImageFmt
{
  GLint intFmt;       //!< The OpenGL internal format of a texture.
  GLint usrFmt;       //!< The OpenGL user format of a texture
  GLint type;         //!< The OpenGL type of a texture
  GLint uploadHint;   //!< An upload hint for the texture: 0=glTexImage, 1=PBO
};

//! Internal data structure used to describe formats
struct NVSGTexImageFmt
{
  GLint fixedPtFmt;     //!< The OpenGL internal format for fixed point 
  GLint floatFmt;       //!< The OpenGL internal format for floating point
  GLint integerFmt;     //!< The OpenGL internal format integer textures
  GLint compressedFmt;  //!< The OpenGL internal format for compressed textures
  GLint nonLinearFmt;   //!< The OpenGL internal format SRGB textures
  GLint usrFmt;         //!< The 'typical' OpenGL user format of a texture (integer textures excluded)
  GLint type;           //!< The OpenGL type of a texture
  GLint uploadHint;     //!< An upload hint for the texture: 0=glTexImage, 1=PBO
};

#define NVSG_TIF_INVALID 0    //!< Invalid texture format

#define NVSG_CACHE_FRONT  0   //!< ID for the front caches
#define NVSG_CACHE_BACK   1   //!< ID for the back caches

// provide zero-based mapping of buffer bindings
enum
{
  // vertex buffer targets
  NVSG_ARRAY_BUFFER = 0,
  NVSG_FIRST_VERTEX_BUFFER = NVSG_ARRAY_BUFFER,
  NVSG_ELEMENT_ARRAY_BUFFER,
  NVSG_LAST_VERTEX_BUFFER = NVSG_ELEMENT_ARRAY_BUFFER,
  // pixel buffer targets
  NVSG_PIXEL_PACK_BUFFER,
  NVSG_FIRST_PIXEL_BUFFER = NVSG_PIXEL_PACK_BUFFER,  
  NVSG_PIXEL_UNPACK_BUFFER,
  NVSG_LAST_PIXEL_BUFFER = NVSG_PIXEL_UNPACK_BUFFER,
  // copy buffer targets,
  NVSG_COPY_READ_BUFFER,
  NVSG_FIRST_COPY_BUFFER = NVSG_COPY_READ_BUFFER,
  NVSG_COPY_WRITE_BUFFER,
  NVSG_LAST_COPY_BUFFER = NVSG_COPY_WRITE_BUFFER,
  // texture buffer targets
  NVSG_TEXTURE_BUFFER,

  

  NVSG_NUM_VERTEX_BUFFERS = NVSG_LAST_VERTEX_BUFFER - NVSG_FIRST_VERTEX_BUFFER + 1,
  NVSG_NUM_PIXEL_BUFFERS  = NVSG_LAST_PIXEL_BUFFER - NVSG_FIRST_PIXEL_BUFFER + 1,
  NVSG_NUM_COPY_BUFFERS  = NVSG_LAST_COPY_BUFFER - NVSG_FIRST_COPY_BUFFER + 1,
  NVSG_NUM_TEXTURE_BUFFERS  = 1,
  NVSG_NUM_BUFFERS = NVSG_NUM_VERTEX_BUFFERS + NVSG_NUM_PIXEL_BUFFERS + NVSG_NUM_COPY_BUFFERS + NVSG_NUM_TEXTURE_BUFFERS
};

#define BUFFER_OFFSET(i) ((char*)NULL + (i))  //!< Helper macro with buffer-usage

//! Wrapper for OpenGL functions to provide caching, extension handling, and, in debug mode, error handling per context.
/** A \c GLContext is defined per pair of <tt><HDC,HGLRC></tt> for Windows or <tt><Display*,GLXContext></tt> for Linux.
  * It holds informations to cache the most important OpenGL states. \n
  * You will never have to use this class explicitly. For creation, deletion, and consistent interaction, instead of using
  * the functions with the lowercase prefix \c wgl on Windows, use the corresponding functions with the uppercase prefix
  * \c WGL. That is, instead of using \c wglCreateContext(), use \c WGLCreateContext(). On Linux, instead of using the
  * prefix \c glX, use the prefix \c GLX. \n
  * To use it consistently, all OpenGL calls should be replaced by the corresponding functions with uppercase \c GL, defined
  * in GLContext.h. I.e. instead of using \c glAccum(), you should use \c GLAccum(). For some of the OpenGL state handling
  * functions, there are slightly faster specialized functions with uppercase \c GL. For example, instead of using
  * <tt>glDisable(GL_BLEND)</tt>, you could either use \link GLDisable() <tt>GLDisable(GL_BLEND)</tt>\endlink  or the
  * specialized function \c GLDisableBlend(). Note, that there might be states that are not covered with specialized functions. \n
  * In the debug build, all the functions with uppercase prefix also test for the OpenGL error code being \c GL_NO_ERROR and
  * assert on failure. */
class GLContext
{
  friend NVSG_API void GLAccum( GLenum op, GLfloat value );
  friend NVSG_API void GLActiveStencilFace( GLenum face );
  friend NVSG_API void GLActiveTexture( GLenum texture );
  friend NVSG_API void GLActiveVarying( GLuint program, const GLchar *name );
  friend NVSG_API void GLAddSwapHintRect( GLint x, GLint y, GLsizei width, GLsizei height );
  friend NVSG_API void GLAlphaFunc( GLenum func, GLclampf ref );
  friend NVSG_API GLboolean GLAreProgramsResident( GLsizei n, const GLuint *programs, GLboolean *residences );
  friend NVSG_API GLboolean GLAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences );
  friend NVSG_API void GLArrayElement( GLint i );
  friend NVSG_API void GLAttachObject( GLhandleARB containerObj, GLhandleARB attachedObj );
  friend NVSG_API void GLAttachShader( GLuint program, GLuint shader );
  friend NVSG_API void GLBegin( GLenum mode );  
  friend NVSG_API void GLBeginConditionalRenderNV( GLuint id, GLenum mode );
  friend NVSG_API void GLBeginQuery( GLenum target, GLuint id );
  friend NVSG_API void GLBeginTransformFeedbackNV( GLenum primitiveMode );
  friend NVSG_API void GLBeginTransformFeedback( GLenum primitiveMode );
  friend NVSG_API void GLBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
  friend NVSG_API void GLBindArraySet( GLuint buffer );
  friend NVSG_API void GLBindAttribLocation( GLuint program, GLuint index, const GLchar *name );
  friend NVSG_API void GLBindBuffer(GLenum target, GLuint id);
  friend NVSG_API void GLBindBufferBaseNV( GLenum target, GLuint index, GLuint buffer );
  friend NVSG_API void GLBindBufferBase( GLenum target, GLuint index, GLuint buffer );
  friend NVSG_API void GLBindBufferOffsetNV( GLenum target, GLuint index, GLuint buffer, GLintptr offset );
  friend NVSG_API void GLBindBufferRangeNV( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size );
  friend NVSG_API void GLBindBufferRange( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size );
  friend NVSG_API void GLBindFragDataLocation( GLuint program, GLuint color, const GLchar *name );
  friend NVSG_API void GLBindProgram( GLenum target, GLuint id );
  friend NVSG_API void GLBindTexture( GLenum target, GLuint texture );
  friend NVSG_API void GLBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap );
  friend NVSG_API void GLBlendEquation( GLenum mode );
  friend NVSG_API void GLBlendEquationSeparate( GLenum modeRGB, GLenum modeAlpha );
  friend NVSG_API void GLBlendFunc( GLenum sfactor, GLenum dfactor );
  friend NVSG_API void GLBlendFuncSeparate( GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha );
  friend NVSG_API void GLBlitFramebuffer( GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter );
  friend NVSG_API void GLBufferData( GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage );
  friend NVSG_API void GLBufferSubData( GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data );
  friend NVSG_API void GLCallList( GLuint list );
  friend NVSG_API void GLCallLists( GLsizei n, GLenum type, const GLvoid *lists );
  friend NVSG_API void GLClampColor( GLenum target, GLenum clamp );
  friend NVSG_API void GLClear( GLbitfield mask );
  friend NVSG_API void GLClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
  friend NVSG_API void GLClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha );
  friend NVSG_API void GLClearColorIi( GLint red, GLint green, GLint blue, GLint alpha );
  friend NVSG_API void GLClearColorIui( GLuint red, GLuint green, GLuint blue, GLuint alpha );
  friend NVSG_API void GLClearDepth( GLclampd depth );
  friend NVSG_API void GLClearDepthd( GLdouble depth );
  friend NVSG_API void GLClearIndex( GLfloat c );
  friend NVSG_API void GLClearStencil( GLint s );
  friend NVSG_API void GLClientActiveTexture( GLenum texture );
  friend NVSG_API void GLClipPlane( GLenum plane, const GLdouble *equation );
  friend NVSG_API void GLColor3b( GLbyte red, GLbyte green, GLbyte blue );
  friend NVSG_API void GLColor3bv( const GLbyte *v );
  friend NVSG_API void GLColor3d( GLdouble red, GLdouble green, GLdouble blue );
  friend NVSG_API void GLColor3dv( const GLdouble *v );
  friend NVSG_API void GLColor3f( GLfloat red, GLfloat green, GLfloat blue );
  friend NVSG_API void GLColor3fv( const GLfloat * v );
  friend NVSG_API void GLColor3h( GLhalf red, GLhalf green, GLhalf blue );
  friend NVSG_API void GLColor3hv( const GLhalf * v );
  friend NVSG_API void GLColor3i( GLint red, GLint green, GLint blue );
  friend NVSG_API void GLColor3iv( const GLint * v );
  friend NVSG_API void GLColor3s( GLshort red, GLshort green, GLshort blue );
  friend NVSG_API void GLColor3sv( const GLshort * v );
  friend NVSG_API void GLColor3ub( GLubyte red, GLubyte green, GLubyte blue );
  friend NVSG_API void GLColor3ubv( const GLubyte * v );
  friend NVSG_API void GLColor3ui( GLuint red, GLuint green, GLuint blue );
  friend NVSG_API void GLColor3uiv( const GLuint * v );
  friend NVSG_API void GLColor3us( GLushort red, GLushort green, GLushort blue );
  friend NVSG_API void GLColor3usv( const GLushort * v );
  friend NVSG_API void GLColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha );
  friend NVSG_API void GLColor4bv( const GLbyte * v );
  friend NVSG_API void GLColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha );
  friend NVSG_API void GLColor4dv( const GLdouble * v );
  friend NVSG_API void GLColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha );
  friend NVSG_API void GLColor4fv( const GLfloat * v );
  friend NVSG_API void GLColor4h( GLhalf red, GLhalf green, GLhalf blue, GLhalf alpha );
  friend NVSG_API void GLColor4hv( const GLhalf * v );
  friend NVSG_API void GLColor4i( GLint red, GLint green, GLint blue, GLint alpha );
  friend NVSG_API void GLColor4iv( const GLint * v );
  friend NVSG_API void GLColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha );
  friend NVSG_API void GLColor4sv( const GLshort * v );
  friend NVSG_API void GLColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha );
  friend NVSG_API void GLColor4ubv( const GLubyte * v );
  friend NVSG_API void GLColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha );
  friend NVSG_API void GLColor4uiv( const GLuint * v );
  friend NVSG_API void GLColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha );
  friend NVSG_API void GLColor4usv( const GLushort * v );
  friend NVSG_API void GLColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha );
  friend NVSG_API void GLColorMaskIndexed( GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a );
  friend NVSG_API void GLColorMaterial( GLenum face, GLenum mode );
  friend NVSG_API void GLColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data );
  friend NVSG_API void GLColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table );
  friend NVSG_API void GLColorTableParameterfv( GLenum target, GLenum pname, const GLfloat *params );
  friend NVSG_API void GLColorTableParameteriv( GLenum target, GLenum pname, const GLint *params );
  friend NVSG_API void GLCombinerInput( GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage );
  friend NVSG_API void GLCombinerOutput( GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum );
  friend NVSG_API void GLCombinerParameterf( GLenum pname, GLfloat param );
  friend NVSG_API void GLCombinerParameterfv( GLenum pname, const GLfloat *params );
  friend NVSG_API void GLCombinerParameteri( GLenum pname, GLint param );
  friend NVSG_API void GLCombinerParameteriv( GLenum pname, const GLint *params );
  friend NVSG_API void GLCombinerStageParameterfv( GLenum stage, GLenum pname, const GLfloat *params );
  friend NVSG_API void GLCompileShader( GLuint shader );
  friend NVSG_API void GLCompressedTexImage1D( GLenum target, int level, GLenum internalformat, GLsizei width, int border, GLsizei imageSize, const void *data );
  friend NVSG_API void GLCompressedTexImage2D( GLenum target, int level, GLenum internalformat, GLsizei width, GLsizei height, int border, GLsizei imageSize, const void *data );
  friend NVSG_API void GLCompressedTexImage3D( GLenum target, int level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, int border, GLsizei imageSize, const void *data );
  friend NVSG_API void GLCompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data );
  friend NVSG_API void GLCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data );
  friend NVSG_API void GLCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data );
  friend NVSG_API void GLConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image );
  friend NVSG_API void GLConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image );
  friend NVSG_API void GLConvolutionParameterf( GLenum target, GLenum pname, GLfloat params );
  friend NVSG_API void GLConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params );
  friend NVSG_API void GLConvolutionParameteri( GLenum target, GLenum pname, GLint params );
  friend NVSG_API void GLConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params );
  friend NVSG_API void GLCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width );
  friend NVSG_API void GLCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width );
  friend NVSG_API void GLCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width );
  friend NVSG_API void GLCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height );
  friend NVSG_API void GLCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type );
  friend NVSG_API void GLCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border );
  friend NVSG_API void GLCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border );
  friend NVSG_API void GLCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width );
  friend NVSG_API void GLCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height );
  friend NVSG_API void GLCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height );
  friend NVSG_API GLuint GLCreateProgram();
  friend NVSG_API GLhandleARB GLCreateProgramObject();
  friend NVSG_API GLuint GLCreateShader( GLenum type );
  friend NVSG_API GLhandleARB GLCreateShaderObject( GLenum shaderType );
  friend NVSG_API void GLCullFace( GLenum mode );
  friend NVSG_API void GLCullFaceLock( GLenum mode );
  friend NVSG_API void GLCullFaceUnlock( GLenum mode );
  friend NVSG_API void GLDeleteArraySets( GLsizei n, const GLuint *buffers );
  friend NVSG_API void GLDeleteBuffers( GLsizei n, const GLuint *buffers );
  friend NVSG_API void GLDeleteFences( GLsizei n, const GLuint *fences );
  friend NVSG_API void GLDeleteLists( GLuint list, GLsizei range );
  friend NVSG_API void GLDeleteObject( GLhandleARB obj );
  friend NVSG_API void GLDeleteProgram( GLuint program );
  friend NVSG_API void GLDeletePrograms( GLsizei n, const GLuint *ids );
  friend NVSG_API void GLDeleteQueries( GLsizei n, const GLuint *ids );
  friend NVSG_API void GLDeleteShader( GLuint shader );
  friend NVSG_API void GLDeleteTextures( GLsizei n, const GLuint *textures );
  friend NVSG_API void GLDepthBounds( GLclampd zmin, GLclampd zmax );
  friend NVSG_API void GLDepthBoundsd( GLdouble zmin, GLdouble zmax );
  friend NVSG_API void GLDepthFunc( GLenum func );
  friend NVSG_API void GLDepthMask( GLboolean flag );
  friend NVSG_API void GLDepthRange( GLclampd znear, GLclampd zfar );
  friend NVSG_API void GLDepthRanged( GLdouble zNear, GLdouble zFar );
  friend NVSG_API void GLDetachObject( GLhandleARB containerObj, GLhandleARB attachedObj );
  friend NVSG_API void GLDetachShader( GLuint program, GLuint shader );
  friend NVSG_API void GLDisable( GLenum cap );
  friend NVSG_API void GLDisableAlphaTest( void );
  friend NVSG_API void GLDisableAutoNormal( void );
  friend NVSG_API void GLDisableBlend( void );
  friend NVSG_API void GLDisableClientState( GLenum array );
  friend NVSG_API void GLDisableClipPlane( GLenum plane );
  friend NVSG_API void GLDisableColorArray( void );
  friend NVSG_API void GLDisableColorLogicOp( void );
  friend NVSG_API void GLDisableColorMaterial( void );
  friend NVSG_API void GLDisableCullFace( void );
  friend NVSG_API void GLDisableCullFaceLock( void );
  friend NVSG_API void GLDisableCullFaceUnlock( void );
  friend NVSG_API void GLDisableDepthTest( void );
  friend NVSG_API void GLDisableFogCoordinateArray( void );
  friend NVSG_API void GLDisableNormalArray( void );
  friend NVSG_API void GLDisableDither( void );
  friend NVSG_API void GLDisableFog( void );
  friend NVSG_API void GLDisableIndexed( GLenum target, GLuint index );
  friend NVSG_API void GLDisableLight( GLenum light );
  friend NVSG_API void GLDisableLighting( void );
  friend NVSG_API void GLDisableLightingLock( void );
  friend NVSG_API void GLDisableLightingUnlock( void );
  friend NVSG_API void GLDisableLineSmooth( void );
  friend NVSG_API void GLDisableLineStipple( void );
  friend NVSG_API void GLDisableLogicOp( void );
  friend NVSG_API void GLDisableNormalize( void );
  friend NVSG_API void GLDisableRescaleNormal( void );
  friend NVSG_API void GLDisablePointSmooth( void );
  friend NVSG_API void GLDisablePolygonOffsetFill( void );
  friend NVSG_API void GLDisablePolygonOffsetLine( void );
  friend NVSG_API void GLDisablePolygonOffsetPoint( void );
  friend NVSG_API void GLDisablePolygonSmooth( void );
  friend NVSG_API void GLDisablePolygonStipple( void );
  friend NVSG_API void GLDisablePrimitiveRestart( void );
  friend NVSG_API void GLDisableScissorTest( void );
  friend NVSG_API void GLDisableSecondaryColorArray( void );
  friend NVSG_API void GLDisableStencilTest( void );
  friend NVSG_API void GLDisableTexture( GLenum target );
  friend NVSG_API void GLDisableTextureCoordArray( void );
  friend NVSG_API void GLDisableTextureGen( GLenum target );
  friend NVSG_API void GLDisableVertexArray( void );
  friend NVSG_API void GLDisableVertexAttribArray( GLuint index );
  friend NVSG_API void GLDisableVertexAttribArrayUnified();
  friend NVSG_API void GLDisableElementArrayUnified();
  friend NVSG_API void GLDrawArrays( GLenum mode, GLint first, GLsizei count );
  friend NVSG_API void GLDrawArraysInstanced( GLenum mode, GLint start, GLsizei count, GLsizei primcount );
  friend NVSG_API void GLDrawBuffer( GLenum mode );
  friend NVSG_API void GLDrawBuffers( GLsizei n, const GLenum *bufs );
  friend NVSG_API void GLDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices );
  friend NVSG_API void GLDrawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount );
  friend NVSG_API void GLDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
  friend NVSG_API void GLDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices );
  friend NVSG_API void GLEdgeFlagPointer( GLsizei stride, const GLboolean *pointer );
  friend NVSG_API void GLEdgeFlag( GLboolean flag );
  friend NVSG_API void GLEdgeFlagv( const GLboolean *flag );
  friend NVSG_API void GLEnable( GLenum cap );
  friend NVSG_API void GLEnableAlphaTest( void );
  friend NVSG_API void GLEnableAutoNormal( void );
  friend NVSG_API void GLEnableBlend( void );
  friend NVSG_API void GLEnableClientState( GLenum array );
  friend NVSG_API void GLEnableClipPlane( GLenum plane );
  friend NVSG_API void GLEnableColorArray( void );
  friend NVSG_API void GLEnableColorLogicOp( void );
  friend NVSG_API void GLEnableColorMaterial( void );
  friend NVSG_API void GLEnableCullFace( void );
  friend NVSG_API void GLEnableCullFaceLock( void );
  friend NVSG_API void GLEnableCullFaceUnlock( void );
  friend NVSG_API void GLEnableDepthTest( void );
  friend NVSG_API void GLEnableFogCoordinateArray( void );
  friend NVSG_API void GLEnableDither( void );
  friend NVSG_API void GLEnableFog( void );
  friend NVSG_API void GLEnableIndexed( GLenum target, GLuint index );
  friend NVSG_API void GLEnableLight( GLenum light );
  friend NVSG_API void GLEnableLighting( void );
  friend NVSG_API void GLEnableLineSmooth( void );
  friend NVSG_API void GLEnableLineStipple( void );
  friend NVSG_API void GLEnableLogicOp( void );
  friend NVSG_API void GLEnableNormalArray( void );
  friend NVSG_API void GLEnableNormalize( void );
  friend NVSG_API void GLEnableRescaleNormal( void );
  friend NVSG_API void GLEnablePointSmooth( void );
  friend NVSG_API void GLEnablePolygonOffsetFill( void );
  friend NVSG_API void GLEnablePolygonOffsetLine( void );
  friend NVSG_API void GLEnablePolygonOffsetPoint( void );
  friend NVSG_API void GLEnablePolygonSmooth( void );
  friend NVSG_API void GLEnablePolygonStipple( void );
  friend NVSG_API void GLEnablePrimitiveRestart( void );
  friend NVSG_API void GLEnableScissorTest( void );
  friend NVSG_API void GLEnableStencilTest( void );
  friend NVSG_API void GLEnableSecondaryColorArray( void );
  friend NVSG_API void GLEnableTexture( GLenum target );
  friend NVSG_API void GLEnableTextureCoordArray( void );
  friend NVSG_API void GLEnableTextureGen( GLenum target );
  friend NVSG_API void GLEnableVertexArray( void );
  friend NVSG_API void GLEnableVertexAttribArray( GLuint index );
  friend NVSG_API void GLEnableVertexAttribArrayUnified();
  friend NVSG_API void GLEnableElementArrayUnified();
  friend NVSG_API void GLEnd( void );
  friend NVSG_API void GLEndList( void );
  friend NVSG_API void GLEndConditionalRenderNV( void );
  friend NVSG_API void GLEndQuery( GLenum target );
  friend NVSG_API void GLEndTransformFeedbackNV();
  friend NVSG_API void GLEndTransformFeedback();
  friend NVSG_API void GLEvalCoord1d( GLdouble u );
  friend NVSG_API void GLEvalCoord1f( GLfloat u );
  friend NVSG_API void GLEvalCoord2d( GLdouble u, GLdouble v );
  friend NVSG_API void GLEvalCoord2f( GLfloat u, GLfloat v );
  friend NVSG_API void GLEvalMesh1( GLenum mode, GLint i1, GLint i2 );
  friend NVSG_API void GLEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 );
  friend NVSG_API void GLEvalPoint1( GLint i );
  friend NVSG_API void GLEvalPoint2( GLint i, GLint j );
  friend NVSG_API void GLExecuteProgram( GLenum target, GLuint id, const GLfloat *params );
  friend NVSG_API void GLFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer );
  friend NVSG_API void GLFinalCombinerInput( GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage );
  friend NVSG_API void GLFinish( void );
  friend NVSG_API void GLFinishFence( GLuint fence );
  friend NVSG_API void GLFlush( void );
  friend NVSG_API void GLFlushPixelDataRange( GLenum target );
  friend NVSG_API void GLFlushVertexArrayRange();
  friend NVSG_API void GLFogColor( const GLfloat * color );
  friend NVSG_API void GLFogCoordd( GLdouble fog );
  friend NVSG_API void GLFogCoorddv( const GLdouble *fog );
  friend NVSG_API void GLFogCoordf( GLfloat fog );
  friend NVSG_API void GLFogCoordfv( const GLfloat *fog );
  friend NVSG_API void GLFogCoordh( GLhalf fog );
  friend NVSG_API void GLFogCoordhv( const GLhalf *fog );
  friend NVSG_API void GLFogCoordPointer( GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLFogDensity( GLfloat density );
  friend NVSG_API void GLFogEnd( GLfloat end );
  friend NVSG_API void GLFogf( GLenum pname, GLfloat param );
  friend NVSG_API void GLFogfv( GLenum pname, const GLfloat *params );
  friend NVSG_API void GLFogi( GLenum pname, GLint param );
  friend NVSG_API void GLFogiv( GLenum pname, const GLint *params );
  friend NVSG_API void GLFogMode( GLenum param );
  friend NVSG_API void GLFogStart( GLfloat start );
  friend NVSG_API void GLFrameTerminator( void );
  friend NVSG_API void GLFramebufferTexture( GLenum target, GLenum attachment, GLuint texture, GLint level );
  friend NVSG_API void GLFramebufferTextureFace( GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face );
  friend NVSG_API void GLFrontFace( GLenum mode );
  friend NVSG_API void GLFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
  friend NVSG_API void GLGenArraySets( GLsizei n, GLuint *buffers );
  friend NVSG_API void GLGenBuffers( GLsizei n, GLuint *buffers );
  friend NVSG_API void GLGenFences( GLsizei n, GLuint *fences );
  friend NVSG_API GLuint GLGenLists( GLsizei range );
  friend NVSG_API void GLGenPrograms( GLsizei n, GLuint *ids );
  friend NVSG_API void GLGenQueries( GLsizei n, GLuint *ids );
  friend NVSG_API void GLGenTextures( GLsizei n, GLuint *textures );
  friend NVSG_API void GLGetActiveAttrib( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name );
  friend NVSG_API void GLGetActiveUniform( GLuint program, GLuint index, GLsizei maxLength, GLsizei *length, GLsizei *size, GLenum *type, GLcharARB *name );
  friend NVSG_API void GLGetActiveVaryingNV( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name );
  friend NVSG_API void GLGetAttachedObjects( GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj );
  friend NVSG_API void GLGetAttachedShaders( GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders );
  friend NVSG_API GLint GLGetAttribLocation( GLuint program, const GLchar *name );
  friend NVSG_API void GLGetBooleanv( GLenum pname, GLboolean *params );
  friend NVSG_API void GLGetBooleanIndexedv( GLenum target, GLuint index, GLboolean *data );
  friend NVSG_API void GLGetBufferParameteriv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetBufferPointerv( GLenum target, GLenum pname, GLvoid* *params );
  friend NVSG_API void GLGetBufferSubData( GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data );
  friend NVSG_API void GLGetClipPlane( GLenum plane, GLdouble *equation );
  friend NVSG_API void GLGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table );
  friend NVSG_API void GLGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetCombinerInputParameterfv( GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetCombinerInputParameteriv( GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params );
  friend NVSG_API void GLGetCombinerOutputParameterfv( GLenum stage, GLenum portion, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetCombinerOutputParameteriv( GLenum stage, GLenum portion, GLenum pname, GLint *params );
  friend NVSG_API void GLGetCombinerStageParameterfv( GLenum stage, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetCompressedTexImage( GLenum target, GLint lod, GLvoid *img );
  friend NVSG_API void GLGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image );
  friend NVSG_API void GLGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetDoublev( GLenum pname, GLdouble *params );
  friend NVSG_API void GLGetFenceiv( GLuint fence, GLenum pname, GLint *params );
  friend NVSG_API void GLGetFinalCombinerInputParameterfv( GLenum variable, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetFinalCombinerInputParameteriv( GLenum variable, GLenum pname, GLint *params );
  friend NVSG_API void GLGetFloatv( GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetFogColor( GLfloat * fogColor );
  friend NVSG_API GLint GLGetFragDataLocation( GLuint program, const GLchar *name );
  friend NVSG_API GLhandleARB GLGetHandle( GLenum pname );
  friend NVSG_API void GLGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values );
  friend NVSG_API void GLGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetInfoLog( GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog );
  friend NVSG_API void GLGetIntegerv( GLenum pname, GLint *params );
  friend NVSG_API void GLGetIntegerIndexedv( GLenum target, GLuint index, GLint *data );
  friend NVSG_API void GLGetLightfv( GLenum light, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetLightiv( GLenum light, GLenum pname, GLint *params );
  friend NVSG_API void GLGetMapdv( GLenum target, GLenum query, GLdouble *v );
  friend NVSG_API void GLGetMapfv( GLenum target, GLenum query, GLfloat *v );
  friend NVSG_API void GLGetMapiv( GLenum target, GLenum query, GLint *v );
  friend NVSG_API void GLGetMaterialfv( GLenum face, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetMaterialiv( GLenum face, GLenum pname, GLint *params );
  friend NVSG_API GLint GLGetMaxTexCoords( void );
  friend NVSG_API GLint GLGetMaxTexUnits( void );
  friend NVSG_API GLint GLGetMaxTexImageUnits( void );  
  friend NVSG_API void GLGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values );
  friend NVSG_API void GLGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetObjectParameterfv( GLhandleARB obj, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetObjectParameteriv( GLhandleARB obj, GLenum pname, GLint *params );
  friend NVSG_API void GLGetPixelMapfv( GLenum map, GLfloat *values );
  friend NVSG_API void GLGetPixelMapuiv( GLenum map, GLuint *values );
  friend NVSG_API void GLGetPixelMapusv( GLenum map, GLushort *values );
  friend NVSG_API void GLGetPointerv( GLenum pname, GLvoid* *params );
  friend NVSG_API void GLGetPolygonStipple( GLubyte *mask );
  friend NVSG_API void GLGetProgramEnvParameterdv( GLenum target, GLuint index, GLdouble *params );
  friend NVSG_API void GLGetProgramEnvParameterfv( GLenum target, GLuint index, GLfloat *params );
  friend NVSG_API void GLGetProgramiv( GLuint program, GLenum pname, GLint *params );
  friend NVSG_API void GLGetProgramLocalParameterdv( GLenum target, GLuint index, GLdouble *params );
  friend NVSG_API void GLGetProgramLocalParameterfv( GLenum target, GLuint index, GLfloat *params );
  friend NVSG_API void GLGetProgramInfoLog( GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog );
  friend NVSG_API void GLGetProgramEnvParameterIiv( GLenum target, GLuint index, GLint *params );
  friend NVSG_API void GLGetProgramEnvParameterIuiv( GLenum target, GLuint index, GLuint *params );
  friend NVSG_API void GLGetProgramLocalParameterIiv( GLenum target, GLuint index, GLint *params );
  friend NVSG_API void GLGetProgramLocalParameterIuiv( GLenum target, GLuint index, GLuint *params );
  friend NVSG_API void GLGetProgramNamedParameterdv( GLuint id, GLsizei len, const GLubyte *name, GLdouble *params );
  friend NVSG_API void GLGetProgramNamedParameterfv( GLuint id, GLsizei len, const GLubyte *name, GLfloat *params );
  friend NVSG_API void GLGetProgramParameterdv( GLenum target, GLuint index, GLenum pname, GLdouble *params );
  friend NVSG_API void GLGetProgramParameterfv( GLenum target, GLuint index, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetProgramString( GLuint id, GLenum pname, GLubyte *program );
  friend NVSG_API void GLGetQueryiv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetQueryObjecti64v( GLuint id, GLenum pname, GLint64EXT *params );
  friend NVSG_API void GLGetQueryObjectiv( GLuint id, GLenum pname, GLint *params );
  friend NVSG_API void GLGetQueryObjectui64v( GLuint id, GLenum pname, GLuint64EXT *params );
  friend NVSG_API void GLGetQueryObjectuiv( GLuint id, GLenum pname, GLuint *params );
  friend NVSG_API void GLGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span );
  friend NVSG_API void GLGetShaderiv( GLuint shader, GLenum pname, GLint *params );
  friend NVSG_API void GLGetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog );
  friend NVSG_API void GLGetShaderSource( GLuint shader, GLsizei bufSize, GLsizei *length, GLcharARB *source );
  friend NVSG_API const GLubyte * GLGetString( GLenum name );
  friend NVSG_API void GLGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetTexEnviv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetTexGendv( GLenum coord, GLenum pname, GLdouble *params );
  friend NVSG_API void GLGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetTexGeniv( GLenum coord, GLenum pname, GLint *params );
  friend NVSG_API void GLGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels );
  friend NVSG_API void GLGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params );
  friend NVSG_API void GLGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetTexParameteriv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetTexParameterIiv( GLenum target, GLenum pname, GLint *params );
  friend NVSG_API void GLGetTexParameterIuiv( GLenum target, GLenum pname, GLuint *params );
  friend NVSG_API void GLGetTrackMatrixiv( GLenum target, GLuint address, GLenum pname, GLint *params );
  friend NVSG_API void GLGetTransformFeedbackVaryingNV( GLuint program, GLuint index, GLint *location );
  friend NVSG_API void GLGetTransformFeedbackVarying( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name );
  friend NVSG_API void GLGetUniformfv( GLuint program, GLint location, GLfloat *params );
  friend NVSG_API void GLGetUniformiv( GLuint program, GLint location, GLint *params );
  friend NVSG_API void GLGetUniformuiv( GLuint program, GLint location, GLuint *params );
  friend NVSG_API GLint GLGetUniformBufferSize( GLuint program, GLint location );
  friend NVSG_API GLint GLGetUniformLocation( GLuint program, const GLcharARB *name );
  friend NVSG_API GLintptr GLGetUniformOffset( GLuint program, GLint location );
  friend NVSG_API GLint GLGetVaryingLocationNV( GLuint program, const GLchar *name );
  friend NVSG_API void GLGetVertexAttribdv( GLuint index, GLenum pname, GLdouble *params );
  friend NVSG_API void GLGetVertexAttribfv( GLuint index, GLenum pname, GLfloat *params );
  friend NVSG_API void GLGetVertexAttribiv( GLuint index, GLenum pname, GLint *params );
  friend NVSG_API void GLGetVertexAttribIiv( GLuint index, GLenum pname, GLint *params );
  friend NVSG_API void GLGetVertexAttribIuiv( GLuint index, GLenum pname, GLuint *params );
  friend NVSG_API void GLGetVertexAttribPointerv( GLuint index, GLenum pname, GLvoid* *pointer );
  friend NVSG_API GLint GLGetVersion();
  friend NVSG_API void GLHint( GLenum target, GLenum mode );
  friend NVSG_API void GLHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink );
  friend NVSG_API void GLIndexd( GLdouble c );
  friend NVSG_API void GLIndexf( GLfloat c );
  friend NVSG_API void GLIndexi( GLint c );
  friend NVSG_API void GLIndexMask( GLuint mask );
  friend NVSG_API void GLIndexPointer( GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLIndexs( GLshort c );
  friend NVSG_API void GLIndexub( GLubyte c );
  friend NVSG_API void GLInitNames( void );
  friend NVSG_API void GLInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API GLboolean GLIsArraySet( GLuint buffer );
  friend NVSG_API GLboolean GLIsBuffer( GLuint name );
  friend NVSG_API GLboolean GLIsEnabled( GLenum cap );
  friend NVSG_API GLboolean GLIsEnabledAlphaTest();
  friend NVSG_API GLboolean GLIsEnabledAutoNormal();
  friend NVSG_API GLboolean GLIsEnabledBlend();
  friend NVSG_API GLboolean GLIsEnabledClipPlane( GLenum plane );
  friend NVSG_API GLboolean GLIsEnabledColorLogicOp();
  friend NVSG_API GLboolean GLIsEnabledColorMaterial();
  friend NVSG_API GLboolean GLIsEnabledCullFace();
  friend NVSG_API GLboolean GLIsEnabledDepthTest();
  friend NVSG_API GLboolean GLIsEnabledDither();
  friend NVSG_API GLboolean GLIsEnabledFog();
  friend NVSG_API GLboolean GLIsEnabledIndexed( GLenum target, GLuint index );
  friend NVSG_API GLboolean GLIsEnabledLight( GLenum cap );
  friend NVSG_API GLboolean GLIsEnabledLighting();
  friend NVSG_API GLboolean GLIsEnabledLineSmooth();
  friend NVSG_API GLboolean GLIsEnabledLineStipple();
  friend NVSG_API GLboolean GLIsEnabledLogicOp();
  friend NVSG_API GLboolean GLIsEnabledNormalize();
  friend NVSG_API GLboolean GLIsEnabledRescaleNormal();
  friend NVSG_API GLboolean GLIsEnabledPointSmooth();
  friend NVSG_API GLboolean GLIsEnabledPolygonOffsetFill();
  friend NVSG_API GLboolean GLIsEnabledPolygonOffsetLine();
  friend NVSG_API GLboolean GLIsEnabledPolygonOffsetPoint();
  friend NVSG_API GLboolean GLIsEnabledPolygonSmooth();
  friend NVSG_API GLboolean GLIsEnabledPolygonStipple();
  friend NVSG_API GLboolean GLIsEnabledScissorTest();
  friend NVSG_API GLboolean GLIsEnabledStencilTest();
  friend NVSG_API GLboolean GLIsEnabledTexture( GLenum cap );
  friend NVSG_API GLboolean GLIsEnabledTextureGen( GLenum cap );
  friend NVSG_API GLboolean GLIsExtensionAvailable( const std::string& extension );
  friend NVSG_API GLboolean GLIsExtensionExported( const std::string& extension );
  friend NVSG_API GLboolean GLIsFence( GLuint fence );
  friend NVSG_API GLboolean GLIsList( GLuint list );
  friend NVSG_API GLboolean GLIsProgram( GLuint program );
  friend NVSG_API GLboolean GLIsQuery( GLuint id );
  friend NVSG_API GLboolean GLIsShader( GLuint shader );
  friend NVSG_API GLboolean GLIsTexture( GLuint texture );
  friend NVSG_API void GLDisableExtensionAvailability( const std::string& extension );
  friend NVSG_API void GLLightAmbient( GLenum light, const GLfloat *params );
  friend NVSG_API void GLLightConstantAttenuation( GLenum light, GLfloat param );
  friend NVSG_API void GLLightDiffuse( GLenum light, const GLfloat *params );
  friend NVSG_API void GLLightf( GLenum light, GLenum pname, GLfloat param );
  friend NVSG_API void GLLightfv( GLenum light, GLenum pname, const GLfloat *params );
  friend NVSG_API void GLLighti( GLenum light, GLenum pname, GLint param );
  friend NVSG_API void GLLightiv( GLenum light, GLenum pname, GLint *params );
  friend NVSG_API void GLLightPosition( GLenum light, const GLfloat * params );
  friend NVSG_API void GLLightLinearAttenuation( GLenum light, GLfloat param );
  friend NVSG_API void GLLightQuadraticAttenuation( GLenum light, GLfloat param );
  friend NVSG_API void GLLightSpecular( GLenum light, const GLfloat *params );
  friend NVSG_API void GLLightSpotCutoff( GLenum light, GLfloat param );
  friend NVSG_API void GLLightSpotDirection( GLenum light, const GLfloat *params );
  friend NVSG_API void GLLightSpotExponent( GLenum light, GLfloat param );
  friend NVSG_API void GLLightModelf( GLenum pname, GLfloat param );
  friend NVSG_API void GLLightModelfv( GLenum pname, const GLfloat *params );
  friend NVSG_API void GLLightModeli( GLenum pname, GLint param );
  friend NVSG_API void GLLightModeliv( GLenum pname, const GLint *params );
  friend NVSG_API void GLLightModelAmbient( const GLfloat * params );
  friend NVSG_API void GLLightModelLocalViewer( GLint param );
  friend NVSG_API void GLLightModelTwoSide( GLint param );
  friend NVSG_API void GLLightModelTwoSideLock( GLint param );
  friend NVSG_API void GLLightModelTwoSideUnlock( GLint param );
  friend NVSG_API void GLLineStipple( GLint factor, GLushort pattern );
  friend NVSG_API void GLLineWidth( GLfloat width );
  friend NVSG_API void GLLinkProgram( GLuint program );
  friend NVSG_API void GLListBase( GLuint base );
  friend NVSG_API void GLLoadIdentity( void );
  friend NVSG_API void GLLoadMatrixd( const GLdouble *m );
  friend NVSG_API void GLLoadMatrixf( const GLfloat *m );
  friend NVSG_API void GLLoadTransposedMatrixd( const GLdouble *m );
  friend NVSG_API void GLLoadTransposeMatrixf( const GLfloat *m );
  friend NVSG_API void GLLoadName( GLuint name );
  friend NVSG_API void GLLoadProgram( GLenum target, GLuint id, GLsizei len, const GLubyte *program );
  friend NVSG_API void GLLockArrays( GLint first, GLsizei count );
  friend NVSG_API void GLLogicOp( GLenum opcode );
  friend NVSG_API void GLMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points );
  friend NVSG_API void GLMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points );
  friend NVSG_API void GLMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points );
  friend NVSG_API void GLMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points );
  friend NVSG_API GLvoid* GLMapBuffer( GLenum target, GLenum access );
  friend NVSG_API void GLMapGrid1d( GLint un, GLdouble u1, GLdouble u2 );
  friend NVSG_API void GLMapGrid1f( GLint un, GLfloat u1, GLfloat u2 );
  friend NVSG_API void GLMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 );
  friend NVSG_API void GLMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 );
  friend NVSG_API void GLMaterialAmbient( GLenum face, const GLfloat *params );
  friend NVSG_API void GLMaterialAmbientBack( const GLfloat *params );
  friend NVSG_API void GLMaterialAmbientFront( const GLfloat *params );
  friend NVSG_API void GLMaterialAmbientFrontAndBack( const GLfloat *params );
  friend NVSG_API void GLMaterialDiffuse( GLenum face, const GLfloat *params );
  friend NVSG_API void GLMaterialDiffuseBack( const GLfloat *params );
  friend NVSG_API void GLMaterialDiffuseFront( const GLfloat *params );
  friend NVSG_API void GLMaterialDiffuseFrontAndBack( const GLfloat *params );
  friend NVSG_API void GLMaterialEmission( GLenum face, const GLfloat *params );
  friend NVSG_API void GLMaterialEmissionBack( const GLfloat *params );
  friend NVSG_API void GLMaterialEmissionFront( const GLfloat *params );
  friend NVSG_API void GLMaterialEmissionFrontAndBack( const GLfloat *params );
  friend NVSG_API void GLMaterialf( GLenum face, GLenum pname, GLfloat param );
  friend NVSG_API void GLMaterialfv( GLenum face, GLenum pname, const GLfloat *params );
  friend NVSG_API void GLMateriali( GLenum face, GLenum pname, GLint param );
  friend NVSG_API void GLMaterialiv( GLenum face, GLenum pname, const GLint *params );
  friend NVSG_API void GLMaterialShininess( GLenum face, GLfloat param );
  friend NVSG_API void GLMaterialShininessBack( GLfloat param );
  friend NVSG_API void GLMaterialShininessFront( GLfloat param );
  friend NVSG_API void GLMaterialShininessFrontAndBack( GLfloat param );
  friend NVSG_API void GLMaterialSpecular( GLenum face, const GLfloat *params );
  friend NVSG_API void GLMaterialSpecularBack( const GLfloat *params );
  friend NVSG_API void GLMaterialSpecularFront( const GLfloat *params );
  friend NVSG_API void GLMaterialSpecularFrontAndBack( const GLfloat *params );
  friend NVSG_API void GLMatrixMode( GLenum mode );
  friend NVSG_API void GLMinmax( GLenum target, GLenum internalformat, GLboolean sink );
  friend NVSG_API void GLMultiDrawArrays( GLenum mode, GLint *first, GLsizei *count, GLsizei primcount );
  friend NVSG_API void GLMultiDrawElements( GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount );
  friend NVSG_API void GLMultiTexCoord1d( GLenum target, GLdouble s );
  friend NVSG_API void GLMultiTexCoord1dv( GLenum target, const GLdouble *v );
  friend NVSG_API void GLMultiTexCoord1f( GLenum target, GLfloat s );
  friend NVSG_API void GLMultiTexCoord1fv( GLenum target, const GLfloat *v );
  friend NVSG_API void GLMultiTexCoord1h( GLenum target, GLhalf s );
  friend NVSG_API void GLMultiTexCoord1hv( GLenum target, const GLhalf *v );
  friend NVSG_API void GLMultiTexCoord1i( GLenum target, GLint s );
  friend NVSG_API void GLMultiTexCoord1iv( GLenum target, const GLint *v );
  friend NVSG_API void GLMultiTexCoord1s( GLenum target, GLshort s );
  friend NVSG_API void GLMultiTexCoord1sv( GLenum target, const GLshort *v );
  friend NVSG_API void GLMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t );
  friend NVSG_API void GLMultiTexCoord2dv( GLenum target, const GLdouble *v );
  friend NVSG_API void GLMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t );
  friend NVSG_API void GLMultiTexCoord2fv( GLenum target, const GLfloat *v );
  friend NVSG_API void GLMultiTexCoord2h( GLenum target, GLhalf s, GLhalf t );
  friend NVSG_API void GLMultiTexCoord2hv( GLenum target, const GLhalf *v );
  friend NVSG_API void GLMultiTexCoord2i( GLenum target, GLint s, GLint t );
  friend NVSG_API void GLMultiTexCoord2iv( GLenum target, const GLint *v );
  friend NVSG_API void GLMultiTexCoord2s( GLenum target, GLshort s, GLshort t );
  friend NVSG_API void GLMultiTexCoord2sv( GLenum target, const GLshort *v );
  friend NVSG_API void GLMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r );
  friend NVSG_API void GLMultiTexCoord3dv( GLenum target, const GLdouble *v );
  friend NVSG_API void GLMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r );
  friend NVSG_API void GLMultiTexCoord3fv( GLenum target, const GLfloat *v );
  friend NVSG_API void GLMultiTexCoord3h( GLenum target, GLhalf s, GLhalf t, GLhalf r );
  friend NVSG_API void GLMultiTexCoord3hv( GLenum target, const GLhalf *v );
  friend NVSG_API void GLMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r );
  friend NVSG_API void GLMultiTexCoord3iv( GLenum target, const GLint *v );
  friend NVSG_API void GLMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r );
  friend NVSG_API void GLMultiTexCoord3sv( GLenum target, const GLshort *v );
  friend NVSG_API void GLMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q );
  friend NVSG_API void GLMultiTexCoord4dv( GLenum target, const GLdouble *v );
  friend NVSG_API void GLMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q );
  friend NVSG_API void GLMultiTexCoord4fv( GLenum target, const GLfloat *v );
  friend NVSG_API void GLMultiTexCoord4h( GLenum target, GLhalf s, GLhalf t, GLhalf r, GLhalf q );
  friend NVSG_API void GLMultiTexCoord4hv( GLenum target, const GLhalf *v );
  friend NVSG_API void GLMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q );
  friend NVSG_API void GLMultiTexCoord4iv( GLenum target, const GLint *v );
  friend NVSG_API void GLMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q );
  friend NVSG_API void GLMultiTexCoord4sv( GLenum target, const GLshort *v );
  friend NVSG_API void GLMultMatrixd( const GLdouble *m );
  friend NVSG_API void GLMultMatrixf( const GLfloat *m );
  friend NVSG_API void GLMultTransposeMatrixd( const GLdouble *m );
  friend NVSG_API void GLMultTransposeMatrixf( const GLfloat *m );
  friend NVSG_API void GLNewList( GLuint list, GLenum mode );
  friend NVSG_API void GLNormal3b( GLbyte nx, GLbyte ny, GLbyte nz );
  friend NVSG_API void GLNormal3bv( const GLbyte *v );
  friend NVSG_API void GLNormal3d( GLdouble nx, GLdouble ny, GLdouble nz );
  friend NVSG_API void GLNormal3dv( const GLdouble *v );
  friend NVSG_API void GLNormal3f( GLfloat nx, GLfloat ny, GLfloat nz );
  friend NVSG_API void GLNormal3fv( const GLfloat *v );
  friend NVSG_API void GLNormal3h( GLhalf nx, GLhalf ny, GLhalf nz );
  friend NVSG_API void GLNormal3hv( const GLhalf *v );
  friend NVSG_API void GLNormal3i( GLint nx, GLint ny, GLint nz );
  friend NVSG_API void GLNormal3iv( const GLint *v );
  friend NVSG_API void GLNormal3s( GLshort nx, GLshort ny, GLshort nz );
  friend NVSG_API void GLNormal3sv( const GLshort *v );
  friend NVSG_API void GLNormalPointer( GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar );
  friend NVSG_API void GLPassThrough( GLfloat token );
  friend NVSG_API void GLPixelDataRange( GLenum target, GLsizei size, GLvoid *pointer );
  friend NVSG_API void GLPixelMapfv( GLenum map, GLsizei mapsize, const GLfloat *values );
  friend NVSG_API void GLPixelMapuiv( GLenum map, GLsizei mapsize, const GLuint *values );
  friend NVSG_API void GLPixelMapusv( GLenum map, GLsizei mapsize, const GLushort *values );
  friend NVSG_API void GLPixelStoref( GLenum pname, GLfloat param );
  friend NVSG_API void GLPixelStorei( GLenum pname, GLint param );
  friend NVSG_API void GLPixelTransferf( GLenum pname, GLfloat param );
  friend NVSG_API void GLPixelTransferi( GLenum pname, GLint param );
  friend NVSG_API void GLPixelZoom( GLfloat xfactor, GLfloat yfactor );
  friend NVSG_API void GLPointParameterf( GLenum pname, GLfloat param );
  friend NVSG_API void GLPointParameterfv( GLenum pname, const GLfloat *params );
  friend NVSG_API void GLPointParameteri( GLenum pname, GLint param );
  friend NVSG_API void GLPointParameteriv( GLenum pname, const GLint *params );
  friend NVSG_API void GLPointSize( GLfloat size );
  friend NVSG_API void GLPolygonMode( GLenum face, GLenum mode );
  friend NVSG_API void GLPolygonModeBack( GLenum mode );
  friend NVSG_API void GLPolygonModeFront( GLenum mode );
  friend NVSG_API void GLPolygonModeFrontAndBack( GLenum mode );
  friend NVSG_API void GLPolygonModeLock( GLenum face, GLenum mode );
  friend NVSG_API void GLPolygonModeUnlock( GLenum face, GLenum mode );
  friend NVSG_API void GLPolygonOffset( GLfloat factor, GLfloat units );
  friend NVSG_API void GLPolygonStipple( const GLubyte *mask );
  friend NVSG_API void GLPopAttrib( void );
  friend NVSG_API void GLPopClientAttrib( void );
  friend NVSG_API void GLPopMatrix( void );
  friend NVSG_API void GLPopName( void );
  friend NVSG_API void GLPrimitiveRestart();
  friend NVSG_API void GLPrimitiveRestartIndex( GLuint index );
  friend NVSG_API void GLPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities );
  friend NVSG_API void GLProgramBufferParametersfv( GLenum target, GLuint buffer, GLuint index, GLsizei count, const GLfloat *params );
  friend NVSG_API void GLProgramBufferParametersIiv( GLenum target, GLuint buffer, GLuint index, GLsizei count, const GLint *params );
  friend NVSG_API void GLProgramBufferParametersIuiv( GLenum target, GLuint buffer, GLuint index, GLsizei count, const GLuint *params );
  friend NVSG_API void GLProgramEnvParameter4d(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  friend NVSG_API void GLProgramEnvParameter4dv(GLenum target, GLuint index, const GLdouble *params);
  friend NVSG_API void GLProgramEnvParameter4f(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  friend NVSG_API void GLProgramEnvParameter4fv(GLenum target, GLuint index, const GLfloat *params);
  friend NVSG_API void GLProgramEnvParametersI4iv( GLenum target, GLuint index, GLsizei count, const GLint *params );
  friend NVSG_API void GLProgramEnvParameterI4i( GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w );
  friend NVSG_API void GLProgramEnvParameterI4iv( GLenum target, GLuint index, const GLint *params );
  friend NVSG_API void GLProgramEnvParametersI4uiv( GLenum target, GLuint index, GLsizei count, const GLuint *params );
  friend NVSG_API void GLProgramEnvParameterI4ui( GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w );
  friend NVSG_API void GLProgramEnvParameterI4uiv( GLenum target, GLuint index, const GLuint *params );
  friend NVSG_API void GLProgramEnvParameters4fv( GLenum target, GLuint index, GLsizei count, const GLfloat *params );
  friend NVSG_API void GLProgramLocalParameter4d(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  friend NVSG_API void GLProgramLocalParameter4dv(GLenum target, GLuint index, const GLdouble *params);
  friend NVSG_API void GLProgramLocalParameter4f(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  friend NVSG_API void GLProgramLocalParameter4fv(GLenum target, GLuint index, const GLfloat *params);
  friend NVSG_API void GLProgramLocalParametersI4iv( GLenum target, GLuint index, GLsizei count, const GLint *params );
  friend NVSG_API void GLProgramLocalParametersI4uiv( GLenum target, GLuint index, GLsizei count, const GLuint *params );
  friend NVSG_API void GLProgramLocalParameterI4i( GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w );
  friend NVSG_API void GLProgramLocalParameterI4iv( GLenum target, GLuint index, const GLint *params );
  friend NVSG_API void GLProgramLocalParameterI4ui( GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w );
  friend NVSG_API void GLProgramLocalParameterI4uiv( GLenum target, GLuint index, const GLuint *params );
  friend NVSG_API void GLProgramLocalParameters4fv( GLenum target, GLuint index, GLsizei count, const GLfloat *params );
  friend NVSG_API void GLProgramNamedParameter4d( GLuint id, GLsizei len, const GLubyte *name, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
  friend NVSG_API void GLProgramNamedParameter4dv( GLuint id, GLsizei len, const GLubyte *name, const GLdouble *v );
  friend NVSG_API void GLProgramNamedParameter4f( GLuint id, GLsizei len, const GLubyte *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
  friend NVSG_API void GLProgramNamedParameter4fv( GLuint id, GLsizei len, const GLubyte *name, const GLfloat *v );
  friend NVSG_API void GLProgramParameter4d( GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
  friend NVSG_API void GLProgramParameter4dv( GLenum target, GLuint index, const GLdouble *v );
  friend NVSG_API void GLProgramParameter4f( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
  friend NVSG_API void GLProgramParameter4fv( GLenum target, GLuint index, const GLfloat *v );
  friend NVSG_API void GLProgramParameteri( GLuint program, GLenum pname, GLint value );
  friend NVSG_API void GLProgramParameters4dv( GLenum target, GLuint index, GLsizei count, const GLdouble *v );
  friend NVSG_API void GLProgramParameters4fv( GLenum target, GLuint index, GLsizei count, const GLfloat *v );
  friend NVSG_API void GLProgramString( GLenum target, GLenum format, GLsizei len, const GLvoid *string );
  friend NVSG_API void GLProgramVertexLimit( GLenum target, GLint limit );
  friend NVSG_API void GLPushAttrib( GLbitfield mask );
  friend NVSG_API void GLPushClientAttrib( GLbitfield mask );
  friend NVSG_API void GLPushMatrix( void );
  friend NVSG_API void GLPushName( GLuint name );
  friend NVSG_API void GLRasterPos2d( GLdouble x, GLdouble y );
  friend NVSG_API void GLRasterPos2f( GLfloat x, GLfloat y );
  friend NVSG_API void GLRasterPos2i( GLint x, GLint y );
  friend NVSG_API void GLRasterPos2s( GLshort x, GLshort y );
  friend NVSG_API void GLRasterPos3d( GLdouble x, GLdouble y, GLdouble z );
  friend NVSG_API void GLRasterPos3f( GLfloat x, GLfloat y, GLfloat z );
  friend NVSG_API void GLRasterPos3i( GLint x, GLint y, GLint z );
  friend NVSG_API void GLRasterPos3s( GLshort x, GLshort y, GLshort z );
  friend NVSG_API void GLRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
  friend NVSG_API void GLRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
  friend NVSG_API void GLRasterPos4i( GLint x, GLint y, GLint z, GLint w );
  friend NVSG_API void GLRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w );
  friend NVSG_API void GLRasterPos2dv( const GLdouble *v );
  friend NVSG_API void GLRasterPos2fv( const GLfloat *v );
  friend NVSG_API void GLRasterPos2iv( const GLint *v );
  friend NVSG_API void GLRasterPos2sv( const GLshort *v );
  friend NVSG_API void GLRasterPos3dv( const GLdouble *v );
  friend NVSG_API void GLRasterPos3fv( const GLfloat *v );
  friend NVSG_API void GLRasterPos3iv( const GLint *v );
  friend NVSG_API void GLRasterPos3sv( const GLshort *v );
  friend NVSG_API void GLRasterPos4dv( const GLdouble *v );
  friend NVSG_API void GLRasterPos4fv( const GLfloat *v );
  friend NVSG_API void GLRasterPos4iv( const GLint *v );
  friend NVSG_API void GLRasterPos4sv( const GLshort *v );
  friend NVSG_API void GLReadBuffer( GLenum mode );
  friend NVSG_API void GLReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels );
  friend NVSG_API void GLRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 );
  friend NVSG_API void GLRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
  friend NVSG_API void GLRecti( GLint x1, GLint y1, GLint x2, GLint y2 );
  friend NVSG_API void GLRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 );
  friend NVSG_API void GLRenderMode( GLenum mode );
  friend NVSG_API void GLRequestResidentPrograms( GLsizei n, const GLuint *programs );
  friend NVSG_API void GLResetHistogram( GLenum target );
  friend NVSG_API void GLResetMinmax( GLenum target );
  friend NVSG_API void GLRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z );
  friend NVSG_API void GLRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z );
  friend NVSG_API void GLScaled( GLdouble x, GLdouble y, GLdouble z );
  friend NVSG_API void GLScalef( GLfloat x, GLfloat y, GLfloat z );
  friend NVSG_API void GLSampleCoverage( GLclampf value, GLboolean invert );
  friend NVSG_API void GLScissor( GLint x, GLint y, GLsizei width, GLsizei height );
  friend NVSG_API void GLSecondaryColor3b( GLbyte red, GLbyte green, GLbyte blue );
  friend NVSG_API void GLSecondaryColor3bv( const GLbyte *v );
  friend NVSG_API void GLSecondaryColor3d( GLdouble red, GLdouble green, GLdouble blue );
  friend NVSG_API void GLSecondaryColor3dv( const GLdouble *v );
  friend NVSG_API void GLSecondaryColor3f( GLfloat red, GLfloat green, GLfloat blue );
  friend NVSG_API void GLSecondaryColor3fv( const GLfloat *v );
  friend NVSG_API void GLSecondaryColor3h( GLhalf red, GLhalf green, GLhalf blue );
  friend NVSG_API void GLSecondaryColor3hv( const GLhalf *v );
  friend NVSG_API void GLSecondaryColor3i( GLint red, GLint green, GLint blue );
  friend NVSG_API void GLSecondaryColor3iv( const GLint *v );
  friend NVSG_API void GLSecondaryColor3s( GLshort red, GLshort green, GLshort blue );
  friend NVSG_API void GLSecondaryColor3sv( const GLshort *v );
  friend NVSG_API void GLSecondaryColor3ub( GLubyte red, GLubyte green, GLubyte blue );
  friend NVSG_API void GLSecondaryColor3ubv( const GLubyte *v );
  friend NVSG_API void GLSecondaryColor3ui( GLuint red, GLuint green, GLuint blue );
  friend NVSG_API void GLSecondaryColor3uiv( const GLuint *v );
  friend NVSG_API void GLSecondaryColor3us( GLushort red, GLushort green, GLushort blue );
  friend NVSG_API void GLSecondaryColor3usv( const GLushort *v );
  friend NVSG_API void GLSecondaryColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLSelectBuffer( GLsizei size, GLuint *buffer );
  friend NVSG_API void GLSelectTexture( GLenum target );
  friend NVSG_API void GLSelectTextureCoordSet( GLenum target );
  friend NVSG_API void GLSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column );
  friend NVSG_API void GLSetFence( GLuint fence, GLenum condition );
  friend NVSG_API void GLShadeModel( GLenum mode );
  friend NVSG_API void GLShaderSource( GLuint shader, GLsizei count, const GLcharARB* *string, const GLint *length );
  friend NVSG_API void GLStencilClearTag( GLsizei tagBits, GLuint clearTag );
  friend NVSG_API void GLStencilFunc( GLenum func, GLint ref, GLuint mask );
  friend NVSG_API void GLStencilFuncSeparate( GLenum face, GLenum func, GLint ref, GLuint mask );
  friend NVSG_API void GLStencilOpSeparate( GLenum face, GLenum fail, GLenum zfail, GLenum zpass );
  friend NVSG_API void GLStencilMaskSeparate( GLenum face, GLuint mask );
  friend NVSG_API void GLStencilMask( GLuint mask );
  friend NVSG_API void GLStencilOp( GLenum fail, GLenum zfail, GLenum zpass );
  friend NVSG_API void GLStringMarker( GLsizei len, const GLvoid *string );
  friend NVSG_API GLboolean GLTestFence( GLuint fence );
  friend NVSG_API void GLTexBuffer( GLenum target, GLenum internalformat, GLuint buffer );
  friend NVSG_API void GLTexCoord1d( GLdouble s );
  friend NVSG_API void GLTexCoord1dv( const GLdouble *v );
  friend NVSG_API void GLTexCoord1f( GLfloat s );
  friend NVSG_API void GLTexCoord1fv( const GLfloat *v );
  friend NVSG_API void GLTexCoord1h( GLhalf s );
  friend NVSG_API void GLTexCoord1hv( const GLhalf *v );
  friend NVSG_API void GLTexCoord1i( GLint s );
  friend NVSG_API void GLTexCoord1iv( const GLint *v );
  friend NVSG_API void GLTexCoord1s( GLshort s );
  friend NVSG_API void GLTexCoord1sv( const GLshort *v );
  friend NVSG_API void GLTexCoord2d( GLdouble s, GLdouble t );
  friend NVSG_API void GLTexCoord2dv( const GLdouble *v );
  friend NVSG_API void GLTexCoord2f( GLfloat s, GLfloat t );
  friend NVSG_API void GLTexCoord2fv( const GLfloat *v );
  friend NVSG_API void GLTexCoord2h( GLhalf s, GLhalf t );
  friend NVSG_API void GLTexCoord2hv( const GLhalf *v );
  friend NVSG_API void GLTexCoord2i( GLint s, GLint t );
  friend NVSG_API void GLTexCoord2iv( const GLint *v );
  friend NVSG_API void GLTexCoord2s( GLshort s, GLshort t );
  friend NVSG_API void GLTexCoord2sv( const GLshort *v );
  friend NVSG_API void GLTexCoord3d( GLdouble s, GLdouble t, GLdouble r );
  friend NVSG_API void GLTexCoord3dv( const GLdouble *v );
  friend NVSG_API void GLTexCoord3f( GLfloat s, GLfloat t, GLfloat r );
  friend NVSG_API void GLTexCoord3fv( const GLfloat *v );
  friend NVSG_API void GLTexCoord3h( GLhalf s, GLhalf t, GLhalf r );
  friend NVSG_API void GLTexCoord3hv( const GLhalf *v );
  friend NVSG_API void GLTexCoord3i( GLint s, GLint t, GLint r );
  friend NVSG_API void GLTexCoord3iv( const GLint *v );
  friend NVSG_API void GLTexCoord3s( GLshort s, GLshort t, GLshort r );
  friend NVSG_API void GLTexCoord3sv( const GLshort *v );
  friend NVSG_API void GLTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q );
  friend NVSG_API void GLTexCoord4dv( const GLdouble *v );
  friend NVSG_API void GLTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q );
  friend NVSG_API void GLTexCoord4fv( const GLfloat *v );
  friend NVSG_API void GLTexCoord4h( GLhalf s, GLhalf t, GLhalf r, GLhalf q );
  friend NVSG_API void GLTexCoord4hv( const GLhalf *v );
  friend NVSG_API void GLTexCoord4i( GLint s, GLint t, GLint r, GLint q );
  friend NVSG_API void GLTexCoord4iv( const GLint *v );
  friend NVSG_API void GLTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q );
  friend NVSG_API void GLTexCoord4sv( const GLshort *v );
  friend NVSG_API void GLTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLTexEnvf( GLenum target, GLenum pname, GLfloat param );
  friend NVSG_API void GLTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
  friend NVSG_API void GLTexEnvi( GLenum target, GLenum pname, GLint param );
  friend NVSG_API void GLTexEnviv( GLenum target, GLenum pname, const GLint *params );
  friend NVSG_API void GLTexGend( GLenum coord, GLenum pname, GLdouble param );
  friend NVSG_API void GLTexGenf( GLenum coord, GLenum pname, GLfloat param );
  friend NVSG_API void GLTexGenfv( GLenum coord, GLenum pname, const GLfloat *param );
  friend NVSG_API void GLTexGeni( GLenum coord, GLenum pname, GLint param );
  friend NVSG_API void GLTexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
  friend NVSG_API void GLTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels );
  friend NVSG_API void GLTexImage3D( GLenum target, int level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, int border, GLenum format, GLenum type, const void* pixels);
  friend NVSG_API void GLTexParameterf( GLenum target, GLenum pname, GLfloat param );
  friend NVSG_API void GLTexParameteri( GLenum target, GLenum pname, GLint param );
  friend NVSG_API void GLTexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
  friend NVSG_API void GLTexParameteriv( GLenum target, GLenum pname, const GLint *params );
  friend NVSG_API void GLTexParameterIiv( GLenum target, GLenum pname, const GLint *params );
  friend NVSG_API void GLTexParameterIuiv( GLenum target, GLenum pname, const GLuint *params );
  friend NVSG_API void GLTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels );
  friend NVSG_API void GLTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels );
  friend NVSG_API void GLTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels );
  friend NVSG_API void GLTextureBorderColor( GLenum target, const GLfloat *params );
  friend NVSG_API void GLTextureEnvColor( const GLfloat * params );
  friend NVSG_API void GLTextureEnvMode( GLint param );
  friend NVSG_API void GLTextureMagFilter( GLenum target, GLint param );
  friend NVSG_API void GLTextureMinFilter( GLenum target, GLint param );
  friend NVSG_API void GLTextureWrapR( GLenum target, GLint param );
  friend NVSG_API void GLTextureWrapS( GLenum target, GLint param );
  friend NVSG_API void GLTextureWrapT( GLenum target, GLint param );
  friend NVSG_API void GLTrackMatrix( GLenum target, GLuint address, GLenum matrix, GLenum transform );
  friend NVSG_API void GLTransformFeedbackAttribsNV( GLuint count, const GLint *attribs, GLenum bufferMode );
  friend NVSG_API void GLTransformFeedbackVaryingsNV( GLuint program, GLsizei count, const GLint* varyings, GLenum bufferMode );
  friend NVSG_API void GLTransformFeedbackVaryings( GLuint program, GLsizei count, const GLchar* *varyings, GLenum bufferMode );
  friend NVSG_API void GLTranslated( GLdouble x, GLdouble y, GLdouble z );
  friend NVSG_API void GLTranslatef( GLfloat x, GLfloat y, GLfloat z );
  friend NVSG_API void GLUnlockArrays();
  friend NVSG_API void GLUniform1f( GLint location, GLfloat v0 );
  friend NVSG_API void GLUniform1fv( GLint location, GLsizei count, const GLfloat *value );
  friend NVSG_API void GLUniform1i( GLint location, GLint v0 );
  friend NVSG_API void GLUniform1iv( GLint location, GLsizei count, const GLint *value );
  friend NVSG_API void GLUniform1ui( GLint location, GLuint v0 );
  friend NVSG_API void GLUniform1uiv( GLint location, GLsizei count, const GLuint *value );
  friend NVSG_API void GLUniform2f( GLint location, GLfloat v0, GLfloat v1 );
  friend NVSG_API void GLUniform2fv( GLint location, GLsizei count, const GLfloat *value );
  friend NVSG_API void GLUniform2i( GLint location, GLint v0, GLint v1 );
  friend NVSG_API void GLUniform2iv( GLint location, GLsizei count, const GLint *value );
  friend NVSG_API void GLUniform2ui( GLint location, GLuint v0, GLuint v1 );
  friend NVSG_API void GLUniform2uiv( GLint location, GLsizei count, const GLuint *value );
  friend NVSG_API void GLUniform3f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2 );
  friend NVSG_API void GLUniform3fv( GLint location, GLsizei count, const GLfloat *value );
  friend NVSG_API void GLUniform3i( GLint location, GLint v0, GLint v1, GLint v2 );
  friend NVSG_API void GLUniform3iv( GLint location, GLsizei count, const GLint *value );
  friend NVSG_API void GLUniform3ui( GLint location, GLuint v0, GLuint v1, GLuint v2 );
  friend NVSG_API void GLUniform3uiv( GLint location, GLsizei count, const GLuint *value );
  friend NVSG_API void GLUniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 );
  friend NVSG_API void GLUniform4fv( GLint location, GLsizei count, const GLfloat *value );
  friend NVSG_API void GLUniform4i( GLint location, GLint v0, GLint v1, GLint v2, GLint v3 );
  friend NVSG_API void GLUniform4iv( GLint location, GLsizei count, const GLint *value );
  friend NVSG_API void GLUniform4ui( GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 );
  friend NVSG_API void GLUniform4uiv( GLint location, GLsizei count, const GLuint *value );
  friend NVSG_API void GLUniformBuffer( GLuint program, GLint location, GLuint buffer );
  friend NVSG_API void GLUniformMatrix2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix2x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix2x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix3x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix3x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix4x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API void GLUniformMatrix4x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value );
  friend NVSG_API GLboolean GLUnmapBuffer( GLenum target );
  friend NVSG_API void GLUseProgram( GLuint program );
  friend NVSG_API void GLUseProgramObject( GLhandleARB programObj );
  friend NVSG_API void GLValidateProgram( GLuint program );
  friend NVSG_API void GLVertex2d( GLdouble x, GLdouble y );
  friend NVSG_API void GLVertex2dv( const GLdouble *v );
  friend NVSG_API void GLVertex2f( GLfloat x, GLfloat y );
  friend NVSG_API void GLVertex2fv( const GLfloat *v );
  friend NVSG_API void GLVertex2h( GLhalf x, GLhalf y );
  friend NVSG_API void GLVertex2hv( const GLhalf *v );
  friend NVSG_API void GLVertex2i( GLint x, GLint y );
  friend NVSG_API void GLVertex2iv( const GLint *v );
  friend NVSG_API void GLVertex2s( GLshort x, GLshort y );
  friend NVSG_API void GLVertex2sv( const GLshort *v );
  friend NVSG_API void GLVertex3d( GLdouble x, GLdouble y, GLdouble z );
  friend NVSG_API void GLVertex3dv( const GLdouble *v );
  friend NVSG_API void GLVertex3f( GLfloat x, GLfloat y, GLfloat z );
  friend NVSG_API void GLVertex3fv( const GLfloat *v );
  friend NVSG_API void GLVertex3h( GLhalf x, GLhalf y, GLhalf z );
  friend NVSG_API void GLVertex3hv( const GLhalf *v );
  friend NVSG_API void GLVertex3i( GLint x, GLint y, GLint z );
  friend NVSG_API void GLVertex3iv( const GLint *v );
  friend NVSG_API void GLVertex3s( GLshort x, GLshort y, GLshort z );
  friend NVSG_API void GLVertex3sv( const GLshort *v );
  friend NVSG_API void GLVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w );
  friend NVSG_API void GLVertex4dv( const GLdouble *v );
  friend NVSG_API void GLVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w );
  friend NVSG_API void GLVertex4fv( const GLfloat *v );
  friend NVSG_API void GLVertex4h( GLhalf x, GLhalf y, GLhalf z, GLhalf w );
  friend NVSG_API void GLVertex4hv( const GLhalf *v );
  friend NVSG_API void GLVertex4i( GLint x, GLint y, GLint z, GLint w );
  friend NVSG_API void GLVertex4iv( const GLint *v );
  friend NVSG_API void GLVertex4s( GLshort x, GLshort y, GLshort z, GLshort w );
  friend NVSG_API void GLVertex4sv( const GLshort *v );
  friend NVSG_API void GLVertexArrayRange( GLsizei size, const GLvoid *pointer );
  friend NVSG_API void GLVertexAttrib1d( GLuint index, GLdouble x );
  friend NVSG_API void GLVertexAttrib1dv( GLuint index, const GLdouble *v );
  friend NVSG_API void GLVertexAttrib1f( GLuint index, GLfloat x );
  friend NVSG_API void GLVertexAttrib1fv( GLuint index, const GLfloat *v );
  friend NVSG_API void GLVertexAttrib1h( GLuint index, GLhalf x );
  friend NVSG_API void GLVertexAttrib1hv( GLuint index, const GLhalf *v );
  friend NVSG_API void GLVertexAttrib1s( GLuint index, GLshort x );
  friend NVSG_API void GLVertexAttrib1sv( GLuint index, const GLshort *v );
  friend NVSG_API void GLVertexAttrib2d( GLuint index, GLdouble x, GLdouble y );
  friend NVSG_API void GLVertexAttrib2dv( GLuint index, const GLdouble *v );
  friend NVSG_API void GLVertexAttrib2f( GLuint index, GLfloat x, GLfloat y );
  friend NVSG_API void GLVertexAttrib2fv( GLuint index, const GLfloat *v );
  friend NVSG_API void GLVertexAttrib2h( GLuint index, GLhalf x, GLhalf y );
  friend NVSG_API void GLVertexAttrib2hv( GLuint index, const GLhalf *v );
  friend NVSG_API void GLVertexAttrib2s( GLuint index, GLshort x, GLshort y );
  friend NVSG_API void GLVertexAttrib2sv( GLuint index, const GLshort *v );
  friend NVSG_API void GLVertexAttrib3d( GLuint index, GLdouble x, GLdouble y, GLdouble z );
  friend NVSG_API void GLVertexAttrib3dv( GLuint index, const GLdouble *v );
  friend NVSG_API void GLVertexAttrib3f( GLuint index, GLfloat x, GLfloat y, GLfloat z );
  friend NVSG_API void GLVertexAttrib3fv( GLuint index, const GLfloat *v );
  friend NVSG_API void GLVertexAttrib3h( GLuint index, GLhalf x, GLhalf y, GLhalf z );
  friend NVSG_API void GLVertexAttrib3hv( GLuint index, const GLhalf *v );
  friend NVSG_API void GLVertexAttrib3s( GLuint index, GLshort x, GLshort y, GLshort z );
  friend NVSG_API void GLVertexAttrib3sv( GLuint index, const GLshort *v );
  friend NVSG_API void GLVertexAttrib4bv( GLuint index, const GLbyte *v );
  friend NVSG_API void GLVertexAttrib4d( GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w );
  friend NVSG_API void GLVertexAttrib4dv( GLuint index, const GLdouble *v );
  friend NVSG_API void GLVertexAttrib4f( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w );
  friend NVSG_API void GLVertexAttrib4fv( GLuint index, const GLfloat *v );
  friend NVSG_API void GLVertexAttrib4h( GLuint index, GLhalf x, GLhalf y, GLhalf z, GLhalf w );
  friend NVSG_API void GLVertexAttrib4hv( GLuint index, const GLhalf *v );
  friend NVSG_API void GLVertexAttrib4iv( GLuint index, const GLint *v );
  friend NVSG_API void GLVertexAttrib4s( GLuint index, GLshort x, GLshort y, GLshort z, GLshort w );
  friend NVSG_API void GLVertexAttrib4sv( GLuint index, const GLshort *v );
  friend NVSG_API void GLVertexAttrib4ub( GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w );
  friend NVSG_API void GLVertexAttrib4ubv( GLuint index, const GLubyte *v );
  friend NVSG_API void GLVertexAttrib4uiv( GLuint index, const GLuint *v );
  friend NVSG_API void GLVertexAttrib4usv( GLuint index, const GLushort *v );
  friend NVSG_API void GLVertexAttrib4Nbv( GLuint index, const GLbyte *v );
  friend NVSG_API void GLVertexAttrib4Niv( GLuint index, const GLint *v );
  friend NVSG_API void GLVertexAttrib4Nsv( GLuint index, const GLshort *v );
  friend NVSG_API void GLVertexAttrib4Nub( GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w );
  friend NVSG_API void GLVertexAttrib4Nubv( GLuint index, const GLubyte *v );
  friend NVSG_API void GLVertexAttrib4Nuiv( GLuint index, const GLuint *v );
  friend NVSG_API void GLVertexAttrib4Nusv( GLuint index, const GLushort *v );
  friend NVSG_API void GLVertexAttribI1i( GLuint index, GLint x );
  friend NVSG_API void GLVertexAttribI1iv( GLuint index, const GLint *v );
  friend NVSG_API void GLVertexAttribI1ui( GLuint index, GLuint x );
  friend NVSG_API void GLVertexAttribI1uiv( GLuint index, const GLuint *v );
  friend NVSG_API void GLVertexAttribI2i( GLuint index, GLint x, GLint y );
  friend NVSG_API void GLVertexAttribI2iv( GLuint index, const GLint *v );
  friend NVSG_API void GLVertexAttribI2ui( GLuint index, GLuint x, GLuint y );
  friend NVSG_API void GLVertexAttribI2uiv( GLuint index, const GLuint *v );
  friend NVSG_API void GLVertexAttribI3i( GLuint index, GLint x, GLint y, GLint z );
  friend NVSG_API void GLVertexAttribI3iv( GLuint index, const GLint *v );
  friend NVSG_API void GLVertexAttribI3ui( GLuint index, GLuint x, GLuint y, GLuint z );
  friend NVSG_API void GLVertexAttribI3uiv( GLuint index, const GLuint *v );
  friend NVSG_API void GLVertexAttribI4bv( GLuint index, const GLbyte *v );
  friend NVSG_API void GLVertexAttribI4i( GLuint index, GLint x, GLint y, GLint z, GLint w );
  friend NVSG_API void GLVertexAttribI4iv( GLuint index, const GLint *v );
  friend NVSG_API void GLVertexAttribI4sv( GLuint index, const GLshort *v );
  friend NVSG_API void GLVertexAttribI4ubv( GLuint index, const GLubyte *v );
  friend NVSG_API void GLVertexAttribI4ui( GLuint index, GLuint x, GLuint y, GLuint z, GLuint w );
  friend NVSG_API void GLVertexAttribI4uiv( GLuint index, const GLuint *v );
  friend NVSG_API void GLVertexAttribI4usv( GLuint index, const GLushort *v );
  friend NVSG_API void GLVertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalize, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLVertexAttribPointerNV( GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLVertexAttribs1dvNV( GLuint index, GLsizei count, const GLdouble *v );
  friend NVSG_API void GLVertexAttribs1fvNV( GLuint index, GLsizei count, const GLfloat *v );
  friend NVSG_API void GLVertexAttribs1hv( GLuint index, GLsizei count, const GLhalf *v );
  friend NVSG_API void GLVertexAttribs1svNV( GLuint index, GLsizei count, const GLshort *v );
  friend NVSG_API void GLVertexAttribs2dvNV( GLuint index, GLsizei count, const GLdouble *v );
  friend NVSG_API void GLVertexAttribs2fvNV( GLuint index, GLsizei count, const GLfloat *v );
  friend NVSG_API void GLVertexAttribs2hv( GLuint index, GLsizei count, const GLhalf *v );
  friend NVSG_API void GLVertexAttribs2svNV( GLuint index, GLsizei count, const GLshort *v );
  friend NVSG_API void GLVertexAttribs3dvNV( GLuint index, GLsizei count, const GLdouble *v );
  friend NVSG_API void GLVertexAttribs3fvNV( GLuint index, GLsizei count, const GLfloat *v );
  friend NVSG_API void GLVertexAttribs3hv( GLuint index, GLsizei count, const GLhalf *v );
  friend NVSG_API void GLVertexAttribs3svNV( GLuint index, GLsizei count, const GLshort *v );
  friend NVSG_API void GLVertexAttribs4dvNV( GLuint index, GLsizei count, const GLdouble *v );
  friend NVSG_API void GLVertexAttribs4fvNV( GLuint index, GLsizei count, const GLfloat *v );
  friend NVSG_API void GLVertexAttribs4hv( GLuint index, GLsizei count, const GLhalf *v );
  friend NVSG_API void GLVertexAttribs4svNV( GLuint index, GLsizei count, const GLshort *v );
  friend NVSG_API void GLVertexAttribs4ubvNV( GLuint index, GLsizei count, const GLubyte *v );
  friend NVSG_API void GLVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer );
  friend NVSG_API void GLViewport( GLint x, GLint y, GLsizei width, GLsizei height );
  friend NVSG_API void GLWindowPos2d( GLdouble x, GLdouble y );
  friend NVSG_API void GLWindowPos2dv( const GLdouble *p );
  friend NVSG_API void GLWindowPos2f( GLfloat x, GLfloat y );
  friend NVSG_API void GLWindowPos2fv( const GLfloat *p );
  friend NVSG_API void GLWindowPos2i( GLint x, GLint y );
  friend NVSG_API void GLWindowPos2iv( const GLint *p );
  friend NVSG_API void GLWindowPos2s( GLshort x, GLshort y );
  friend NVSG_API void GLWindowPos2sv( const GLshort *p );
  friend NVSG_API void GLWindowPos3d( GLdouble x, GLdouble y, GLdouble z );
  friend NVSG_API void GLWindowPos3dv( const GLdouble *p );
  friend NVSG_API void GLWindowPos3f( GLfloat x, GLfloat y, GLfloat z );
  friend NVSG_API void GLWindowPos3fv( const GLfloat *p );
  friend NVSG_API void GLWindowPos3i( GLint x, GLint y, GLint z );
  friend NVSG_API void GLWindowPos3iv( const GLint *p );
  friend NVSG_API void GLWindowPos3s( GLshort x, GLshort y, GLshort z );
  friend NVSG_API void GLWindowPos3sv( const GLshort *p );

  // EXT_framebuffer_object API
  friend NVSG_API GLboolean GLIsRenderbuffer(GLuint renderbuffer);
  friend NVSG_API void GLBindRenderbuffer(GLenum target, GLuint renderbuffer);
  friend NVSG_API void GLDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers);
  friend NVSG_API void GLGenRenderbuffers(GLsizei n, GLuint *renderbuffers);
  friend NVSG_API void GLRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height);
  friend NVSG_API void GLRenderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
  friend NVSG_API void GLRenderbufferStorageMultisampleCoverage(GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height);
  friend NVSG_API void GLGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params);
  friend NVSG_API GLboolean GLIsFramebuffer(GLuint framebuffer);
  friend NVSG_API void GLBindFramebuffer(GLenum target, GLuint framebuffer);
  friend NVSG_API void GLDeleteFramebuffers(GLsizei n, const GLuint *framebuffers);
  friend NVSG_API void GLGenFramebuffers(GLsizei n, GLuint *framebuffers);
  friend NVSG_API GLenum GLCheckFramebufferStatus(GLenum target);
  friend NVSG_API void GLFramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
  friend NVSG_API void GLFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
  friend NVSG_API void GLFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
  friend NVSG_API void GLFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer);
  friend NVSG_API void GLFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
  friend NVSG_API void GLGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params);
  friend NVSG_API void GLGenerateMipmap(GLenum target);
  friend NVSG_API void GLGPUSyncGetHandleSize(GLuint *initSize, GLuint *mapSize);
  friend NVSG_API void GLGPUSyncInit(GLvoid* syncData);
  friend NVSG_API void GLGPUSyncEnd(GLvoid* syncData);
  friend NVSG_API void GLGPUSyncMapBuffer(GLvoid* syncData);
  friend NVSG_API void GLGPUSyncUnmapBuffer(GLvoid* syncData);
  friend NVSG_API void GLGPUSyncCopyBuffer(GLuint offset, GLuint size, 
                                                            GLvoid * syncData);
  friend NVSG_API void GLGPUSyncAcquire(GLvoid* syncData);
  friend NVSG_API void GLGPUSyncRelease(GLvoid* syncData);

  // GL_ARB_tessellation_shader
  friend NVSG_API void GLPatchParameterfv(GLenum pname, const GLfloat * values );
  friend NVSG_API void GLPatchParameteri(GLenum pname, GLint value );

  // GL_ARB_map_buffer_range
  friend NVSG_API GLvoid* GLMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access);
  friend NVSG_API void GLFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length);

  // GL_ARB_copy_buffer
  friend NVSG_API void GLCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size);

  // GL_ARB_texture_multisample
  friend NVSG_API void GLTexImage2DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations);
  friend NVSG_API void GLTexImage3DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations);
  friend NVSG_API void GLGetMultisamplefv(GLenum pname, GLuint index, GLfloat *val);
  friend NVSG_API void GLSampleMaski(GLuint index, GLbitfield mask);

  // GL_ARB_vertex_array_object
  friend NVSG_API void GLBindVertexArray( GLuint array );
  friend NVSG_API void GLDeleteVertexArrays( GLsizei n, const GLuint* arrays );
  friend NVSG_API void GLGenVertexArrays( GLsizei n, GLuint *arrays );
  friend NVSG_API void GLIsVertexArray( GLuint array );

  // GL_ARB_debug_output
  friend NVSG_API void GLDebugMessageControlARB (GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled);
  friend NVSG_API void GLDebugMessageInsertARB (GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf);
  friend NVSG_API void GLDebugMessageCallbackARB (GLDEBUGPROCARB callback, const GLvoid *userParam);
  friend NVSG_API GLuint GLGetDebugMessageLogARB (GLuint count, GLsizei bufsize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog);

  // GL_NV_present_video
  friend NVSG_API void GLPresentFrameKeyedNV(GLuint video_slot, GLuint64EXT minPresentTime, GLuint beginPresentTimeId, GLuint presentDurationId,
                                             GLenum type, GLenum target0, GLuint fill0, GLuint key0, GLenum target1, GLuint fill1, GLuint key1);
  friend NVSG_API void GLPresentFrameDualFillNV(GLuint video_slot, GLuint64EXT minPresentTime, GLuint beginPresentTimeId, GLuint presentDurationId, 
                                                GLenum type, GLenum target0, GLuint fill0, GLenum target1, GLuint fill1, GLenum target2, GLuint fill2, GLenum target3, GLuint fill3);
  friend NVSG_API void GLGetVideoivNV(GLuint video_slot, GLenum pname, GLint *params);
  friend NVSG_API void GLGetVideouivNV(GLuint video_slot, GLenum pname, GLuint *params);
  friend NVSG_API void GLGetVideoi64vNV(GLuint video_slot, GLenum pname, GLint64EXT *params);
  friend NVSG_API void GLGetVideoui64vNV(GLuint video_slot, GLenum pname, GLuint64EXT *params);

  // GL_NV_vertex_buffer_unified_memory
  friend NVSG_API void GLBufferAddressRangeNV(GLenum pname, GLuint index, GLuint64EXT address, GLsizeiptr length);
  friend NVSG_API void GLVertexFormatNV(GLint size, GLenum type, GLsizei stride);
  friend NVSG_API void GLNormalFormatNV(GLenum type, GLsizei stride);
  friend NVSG_API void GLColorFormatNV(GLint size, GLenum type, GLsizei stride);
  friend NVSG_API void GLIndexFormatNV(GLenum type, GLsizei stride);
  friend NVSG_API void GLTexCoordFormatNV(GLint size, GLenum type, GLsizei stride);
  friend NVSG_API void GLEdgeFlagFormatNV(GLsizei stride);
  friend NVSG_API void GLSecondaryColorFormatNV(GLint size, GLenum type, GLsizei stride);
  friend NVSG_API void GLFogCoordFormatNV(GLenum type, GLsizei stride);
  friend NVSG_API void GLVertexAttribFormatNV(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride);
  friend NVSG_API void GLVertexAttribIFormatNV(GLuint index, GLint size, GLenum type, GLsizei stride);
  friend NVSG_API void GLGetIntegerui64i_vNV(GLenum target, GLuint index, GLuint64EXT *data);

  // GL_NV_shader_buffer_load
  friend NVSG_API void GLGetBufferParameterui64vNV(GLenum target, GLenum pname, GLuint64EXT *params);
  friend NVSG_API GLboolean GLIsBufferResidentNV(GLenum target);
  friend NVSG_API void GLMakeBufferNonResidentNV(GLenum target);
  friend NVSG_API void GLMakeBufferResidentNV(GLenum target, GLenum access);
  friend NVSG_API void GLGetNamedBufferParameterui64vNV(GLuint buffer, GLenum pname, GLuint64EXT *params);
  friend NVSG_API GLboolean GLIsNamedBufferResidentNV(GLuint buffer);
  friend NVSG_API void GLMakeNamedBufferNonResidentNV(GLuint buffer);
  friend NVSG_API void GLMakeNamedBufferResidentNV(GLuint buffer, GLenum access);
  friend NVSG_API void GLGetIntegerui64vNV(GLenum target, GLuint64EXT *data);
  friend NVSG_API void GLUniformui64NV(GLint location, GLuint64EXT v0);
  friend NVSG_API void GLUniformui64vNV(GLint location, GLsizei count, const GLuint64EXT *value);
  friend NVSG_API void GLGetUniformui64vNV(GLuint program, GLint location, GLuint64EXT *params);
  friend NVSG_API void GLProgramUniformui64NV(GLuint program, GLint location, GLuint64EXT v0);
  friend NVSG_API void GLProgramUniformui64vNV(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value);

  // GL_NV_video_capture
  friend NVSG_API void GLBeginVideoCaptureNV (GLuint video_capture_slot);
  friend NVSG_API void GLBindVideoCaptureStreamBufferNV (GLuint video_capture_slot, GLuint stream, GLenum frame_region, GLintptrARB offset);
  friend NVSG_API void GLBindVideoCaptureStreamTextureNV (GLuint video_capture_slot, GLuint stream, GLenum frame_region, GLenum target, GLuint texture);
  friend NVSG_API void GLEndVideoCaptureNV (GLuint video_capture_slot);
  friend NVSG_API void GLGetVideoCaptureivNV (GLuint video_capture_slot, GLenum pname, GLint *params);
  friend NVSG_API void GLGetVideoCaptureStreamivNV (GLuint video_capture_slot, GLuint stream, GLenum pname, GLint *params);
  friend NVSG_API void GLGetVideoCaptureStreamfvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, GLfloat *params);
  friend NVSG_API void GLGetVideoCaptureStreamdvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, GLdouble *params);
  friend NVSG_API GLenum GLVideoCaptureNV (GLuint video_capture_slot, GLuint *sequence_num, GLuint64EXT *capture_time);
  friend NVSG_API void GLVideoCaptureStreamParameterivNV (GLuint video_capture_slot, GLuint stream, GLenum pname, const GLint *params);
  friend NVSG_API void GLVideoCaptureStreamParameterfvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, const GLfloat *params);
  friend NVSG_API void GLVideoCaptureStreamParameterdvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, const GLdouble *params);
  
  // GL_EXT_direct_state_access
  friend NVSG_API void GLClientAttribDefaultEXT (GLbitfield mask);
  friend NVSG_API void GLPushClientAttribDefaultEXT (GLbitfield mask);
  friend NVSG_API void GLMatrixLoadfEXT (GLenum mode, const GLfloat *m);
  friend NVSG_API void GLMatrixLoaddEXT (GLenum mode, const GLdouble *m);
  friend NVSG_API void GLMatrixMultfEXT (GLenum mode, const GLfloat *m);
  friend NVSG_API void GLMatrixMultdEXT (GLenum mode, const GLdouble *m);
  friend NVSG_API void GLMatrixLoadIdentityEXT (GLenum mode);
  friend NVSG_API void GLMatrixRotatefEXT (GLenum mode, GLfloat angle, GLfloat x, GLfloat y, GLfloat z);
  friend NVSG_API void GLMatrixRotatedEXT (GLenum mode, GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
  friend NVSG_API void GLMatrixScalefEXT (GLenum mode, GLfloat x, GLfloat y, GLfloat z);
  friend NVSG_API void GLMatrixScaledEXT (GLenum mode, GLdouble x, GLdouble y, GLdouble z);
  friend NVSG_API void GLMatrixTranslatefEXT (GLenum mode, GLfloat x, GLfloat y, GLfloat z);
  friend NVSG_API void GLMatrixTranslatedEXT (GLenum mode, GLdouble x, GLdouble y, GLdouble z);
  friend NVSG_API void GLMatrixFrustumEXT (GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
  friend NVSG_API void GLMatrixOrthoEXT (GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar);
  friend NVSG_API void GLMatrixPopEXT (GLenum mode);
  friend NVSG_API void GLMatrixPushEXT (GLenum mode);
  friend NVSG_API void GLMatrixLoadTransposefEXT (GLenum mode, const GLfloat *m);
  friend NVSG_API void GLMatrixLoadTransposedEXT (GLenum mode, const GLdouble *m);
  friend NVSG_API void GLMatrixMultTransposefEXT (GLenum mode, const GLfloat *m);
  friend NVSG_API void GLMatrixMultTransposedEXT (GLenum mode, const GLdouble *m);
  friend NVSG_API void GLTextureParameterfEXT (GLuint texture, GLenum target, GLenum pname, GLfloat param);
  friend NVSG_API void GLTextureParameterfvEXT (GLuint texture, GLenum target, GLenum pname, const GLfloat *params);
  friend NVSG_API void GLTextureParameteriEXT (GLuint texture, GLenum target, GLenum pname, GLint param);
  friend NVSG_API void GLTextureParameterivEXT (GLuint texture, GLenum target, GLenum pname, const GLint *params);
  friend NVSG_API void GLTextureImage1DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLTextureImage2DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLTextureSubImage1DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLTextureSubImage2DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLCopyTextureImage1DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
  friend NVSG_API void GLCopyTextureImage2DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
  friend NVSG_API void GLCopyTextureSubImage1DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
  friend NVSG_API void GLCopyTextureSubImage2DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
  friend NVSG_API void GLGetTextureImageEXT (GLuint texture, GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
  friend NVSG_API void GLGetTextureParameterfvEXT (GLuint texture, GLenum target, GLenum pname, GLfloat *params);
  friend NVSG_API void GLGetTextureParameterivEXT (GLuint texture, GLenum target, GLenum pname, GLint *params);
  friend NVSG_API void GLGetTextureLevelParameterfvEXT (GLuint texture, GLenum target, GLint level, GLenum pname, GLfloat *params);
  friend NVSG_API void GLGetTextureLevelParameterivEXT (GLuint texture, GLenum target, GLint level, GLenum pname, GLint *params);
  friend NVSG_API void GLTextureImage3DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLTextureSubImage3DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLCopyTextureSubImage3DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
  friend NVSG_API void GLMultiTexParameterfEXT (GLenum texunit, GLenum target, GLenum pname, GLfloat param);
  friend NVSG_API void GLMultiTexParameterfvEXT (GLenum texunit, GLenum target, GLenum pname, const GLfloat *params);
  friend NVSG_API void GLMultiTexParameteriEXT (GLenum texunit, GLenum target, GLenum pname, GLint param);
  friend NVSG_API void GLMultiTexParameterivEXT (GLenum texunit, GLenum target, GLenum pname, const GLint *params);
  friend NVSG_API void GLMultiTexImage1DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLMultiTexImage2DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLMultiTexSubImage1DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLMultiTexSubImage2DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLCopyMultiTexImage1DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border);
  friend NVSG_API void GLCopyMultiTexImage2DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border);
  friend NVSG_API void GLCopyMultiTexSubImage1DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width);
  friend NVSG_API void GLCopyMultiTexSubImage2DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height);
  friend NVSG_API void GLGetMultiTexImageEXT (GLenum texunit, GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels);
  friend NVSG_API void GLGetMultiTexParameterfvEXT (GLenum texunit, GLenum target, GLenum pname, GLfloat *params);
  friend NVSG_API void GLGetMultiTexParameterivEXT (GLenum texunit, GLenum target, GLenum pname, GLint *params);
  friend NVSG_API void GLGetMultiTexLevelParameterfvEXT (GLenum texunit, GLenum target, GLint level, GLenum pname, GLfloat *params);
  friend NVSG_API void GLGetMultiTexLevelParameterivEXT (GLenum texunit, GLenum target, GLint level, GLenum pname, GLint *params);
  friend NVSG_API void GLMultiTexImage3DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLMultiTexSubImage3DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels);
  friend NVSG_API void GLCopyMultiTexSubImage3DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height);
  friend NVSG_API void GLBindMultiTextureEXT (GLenum texunit, GLenum target, GLuint texture);
  friend NVSG_API void GLEnableClientStateIndexedEXT (GLenum array, GLuint index);
  friend NVSG_API void GLDisableClientStateIndexedEXT (GLenum array, GLuint index);
  friend NVSG_API void GLMultiTexCoordPointerEXT (GLenum texunit, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer);
  friend NVSG_API void GLMultiTexEnvfEXT (GLenum texunit, GLenum target, GLenum pname, GLfloat param);
  friend NVSG_API void GLMultiTexEnvfvEXT (GLenum texunit, GLenum target, GLenum pname, const GLfloat *params);
  friend NVSG_API void GLMultiTexEnviEXT (GLenum texunit, GLenum target, GLenum pname, GLint param);
  friend NVSG_API void GLMultiTexEnvivEXT (GLenum texunit, GLenum target, GLenum pname, const GLint *params);
  friend NVSG_API void GLMultiTexGendEXT (GLenum texunit, GLenum coord, GLenum pname, GLdouble param);
  friend NVSG_API void GLMultiTexGendvEXT (GLenum texunit, GLenum coord, GLenum pname, const GLdouble *params);
  friend NVSG_API void GLMultiTexGenfEXT (GLenum texunit, GLenum coord, GLenum pname, GLfloat param);
  friend NVSG_API void GLMultiTexGenfvEXT (GLenum texunit, GLenum coord, GLenum pname, const GLfloat *params);
  friend NVSG_API void GLMultiTexGeniEXT (GLenum texunit, GLenum coord, GLenum pname, GLint param);
  friend NVSG_API void GLMultiTexGenivEXT (GLenum texunit, GLenum coord, GLenum pname, const GLint *params);
  friend NVSG_API void GLGetMultiTexEnvfvEXT (GLenum texunit, GLenum target, GLenum pname, GLfloat *params);
  friend NVSG_API void GLGetMultiTexEnvivEXT (GLenum texunit, GLenum target, GLenum pname, GLint *params);
  friend NVSG_API void GLGetMultiTexGendvEXT (GLenum texunit, GLenum coord, GLenum pname, GLdouble *params);
  friend NVSG_API void GLGetMultiTexGenfvEXT (GLenum texunit, GLenum coord, GLenum pname, GLfloat *params);
  friend NVSG_API void GLGetMultiTexGenivEXT (GLenum texunit, GLenum coord, GLenum pname, GLint *params);
  friend NVSG_API void GLGetFloatIndexedvEXT (GLenum target, GLuint index, GLfloat *data);
  friend NVSG_API void GLGetDoubleIndexedvEXT (GLenum target, GLuint index, GLdouble *data);
  friend NVSG_API void GLGetPointerIndexedvEXT (GLenum target, GLuint index, GLvoid* *data);
  friend NVSG_API void GLCompressedTextureImage3DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedTextureImage2DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedTextureImage1DEXT (GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedTextureSubImage3DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedTextureSubImage2DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedTextureSubImage1DEXT (GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLGetCompressedTextureImageEXT (GLuint texture, GLenum target, GLint lod, GLvoid *img);
  friend NVSG_API void GLCompressedMultiTexImage3DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedMultiTexImage2DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedMultiTexImage1DEXT (GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedMultiTexSubImage3DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedMultiTexSubImage2DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLCompressedMultiTexSubImage1DEXT (GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *bits);
  friend NVSG_API void GLGetCompressedMultiTexImageEXT (GLenum texunit, GLenum target, GLint lod, GLvoid *img);
  friend NVSG_API void GLNamedProgramStringEXT (GLuint program, GLenum target, GLenum format, GLsizei len, const GLvoid *string);
  friend NVSG_API void GLNamedProgramLocalParameter4dEXT (GLuint program, GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w);
  friend NVSG_API void GLNamedProgramLocalParameter4dvEXT (GLuint program, GLenum target, GLuint index, const GLdouble *params);
  friend NVSG_API void GLNamedProgramLocalParameter4fEXT (GLuint program, GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
  friend NVSG_API void GLNamedProgramLocalParameter4fvEXT (GLuint program, GLenum target, GLuint index, const GLfloat *params);
  friend NVSG_API void GLGetNamedProgramLocalParameterdvEXT (GLuint program, GLenum target, GLuint index, GLdouble *params);
  friend NVSG_API void GLGetNamedProgramLocalParameterfvEXT (GLuint program, GLenum target, GLuint index, GLfloat *params);
  friend NVSG_API void GLGetNamedProgramivEXT (GLuint program, GLenum target, GLenum pname, GLint *params);
  friend NVSG_API void GLGetNamedProgramStringEXT (GLuint program, GLenum target, GLenum pname, GLvoid *string);
  friend NVSG_API void GLNamedProgramLocalParameters4fvEXT (GLuint program, GLenum target, GLuint index, GLsizei count, const GLfloat *params);
  friend NVSG_API void GLNamedProgramLocalParameterI4iEXT (GLuint program, GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w);
  friend NVSG_API void GLNamedProgramLocalParameterI4ivEXT (GLuint program, GLenum target, GLuint index, const GLint *params);
  friend NVSG_API void GLNamedProgramLocalParametersI4ivEXT (GLuint program, GLenum target, GLuint index, GLsizei count, const GLint *params);
  friend NVSG_API void GLNamedProgramLocalParameterI4uiEXT (GLuint program, GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w);
  friend NVSG_API void GLNamedProgramLocalParameterI4uivEXT (GLuint program, GLenum target, GLuint index, const GLuint *params);
  friend NVSG_API void GLNamedProgramLocalParametersI4uivEXT (GLuint program, GLenum target, GLuint index, GLsizei count, const GLuint *params);
  friend NVSG_API void GLGetNamedProgramLocalParameterIivEXT (GLuint program, GLenum target, GLuint index, GLint *params);
  friend NVSG_API void GLGetNamedProgramLocalParameterIuivEXT (GLuint program, GLenum target, GLuint index, GLuint *params);
  friend NVSG_API void GLTextureParameterIivEXT (GLuint texture, GLenum target, GLenum pname, const GLint *params);
  friend NVSG_API void GLTextureParameterIuivEXT (GLuint texture, GLenum target, GLenum pname, const GLuint *params);
  friend NVSG_API void GLGetTextureParameterIivEXT (GLuint texture, GLenum target, GLenum pname, GLint *params);
  friend NVSG_API void GLGetTextureParameterIuivEXT (GLuint texture, GLenum target, GLenum pname, GLuint *params);
  friend NVSG_API void GLMultiTexParameterIivEXT (GLenum texunit, GLenum target, GLenum pname, const GLint *params);
  friend NVSG_API void GLMultiTexParameterIuivEXT (GLenum texunit, GLenum target, GLenum pname, const GLuint *params);
  friend NVSG_API void GLGetMultiTexParameterIivEXT (GLenum texunit, GLenum target, GLenum pname, GLint *params);
  friend NVSG_API void GLGetMultiTexParameterIuivEXT (GLenum texunit, GLenum target, GLenum pname, GLuint *params);
  friend NVSG_API void GLProgramUniform1fEXT (GLuint program, GLint location, GLfloat v0);
  friend NVSG_API void GLProgramUniform2fEXT (GLuint program, GLint location, GLfloat v0, GLfloat v1);
  friend NVSG_API void GLProgramUniform3fEXT (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2);
  friend NVSG_API void GLProgramUniform4fEXT (GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3);
  friend NVSG_API void GLProgramUniform1iEXT (GLuint program, GLint location, GLint v0);
  friend NVSG_API void GLProgramUniform2iEXT (GLuint program, GLint location, GLint v0, GLint v1);
  friend NVSG_API void GLProgramUniform3iEXT (GLuint program, GLint location, GLint v0, GLint v1, GLint v2);
  friend NVSG_API void GLProgramUniform4iEXT (GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3);
  friend NVSG_API void GLProgramUniform1fvEXT (GLuint program, GLint location, GLsizei count, const GLfloat *value);
  friend NVSG_API void GLProgramUniform2fvEXT (GLuint program, GLint location, GLsizei count, const GLfloat *value);
  friend NVSG_API void GLProgramUniform3fvEXT (GLuint program, GLint location, GLsizei count, const GLfloat *value);
  friend NVSG_API void GLProgramUniform4fvEXT (GLuint program, GLint location, GLsizei count, const GLfloat *value);
  friend NVSG_API void GLProgramUniform1ivEXT (GLuint program, GLint location, GLsizei count, const GLint *value);
  friend NVSG_API void GLProgramUniform2ivEXT (GLuint program, GLint location, GLsizei count, const GLint *value);
  friend NVSG_API void GLProgramUniform3ivEXT (GLuint program, GLint location, GLsizei count, const GLint *value);
  friend NVSG_API void GLProgramUniform4ivEXT (GLuint program, GLint location, GLsizei count, const GLint *value);
  friend NVSG_API void GLProgramUniformMatrix2fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix3fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix4fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix2x3fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix3x2fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix2x4fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix4x2fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix3x4fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniformMatrix4x3fvEXT (GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value);
  friend NVSG_API void GLProgramUniform1uiEXT (GLuint program, GLint location, GLuint v0);
  friend NVSG_API void GLProgramUniform2uiEXT (GLuint program, GLint location, GLuint v0, GLuint v1);
  friend NVSG_API void GLProgramUniform3uiEXT (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2);
  friend NVSG_API void GLProgramUniform4uiEXT (GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3);
  friend NVSG_API void GLProgramUniform1uivEXT (GLuint program, GLint location, GLsizei count, const GLuint *value);
  friend NVSG_API void GLProgramUniform2uivEXT (GLuint program, GLint location, GLsizei count, const GLuint *value);
  friend NVSG_API void GLProgramUniform3uivEXT (GLuint program, GLint location, GLsizei count, const GLuint *value);
  friend NVSG_API void GLProgramUniform4uivEXT (GLuint program, GLint location, GLsizei count, const GLuint *value);
  friend NVSG_API void GLNamedBufferDataEXT (GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage);
  friend NVSG_API void GLNamedBufferSubDataEXT (GLuint buffer, GLintptr offset, GLsizeiptr size, const GLvoid *data);
  friend NVSG_API GLvoid* GLMapNamedBufferEXT (GLuint buffer, GLenum access);
  friend NVSG_API GLboolean GLUnmapNamedBufferEXT (GLuint buffer);
  friend NVSG_API void GLGetNamedBufferParameterivEXT (GLuint buffer, GLenum pname, GLint *params);
  friend NVSG_API void GLGetNamedBufferPointervEXT (GLuint buffer, GLenum pname, GLvoid* *params);
  friend NVSG_API void GLGetNamedBufferSubDataEXT (GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid *data);
  friend NVSG_API void GLTextureBufferEXT (GLuint texture, GLenum target, GLenum internalformat, GLuint buffer);
  friend NVSG_API void GLMultiTexBufferEXT (GLenum texunit, GLenum target, GLenum internalformat, GLuint buffer);
  friend NVSG_API void GLNamedRenderbufferStorageEXT (GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height);
  friend NVSG_API void GLGetNamedRenderbufferParameterivEXT (GLuint renderbuffer, GLenum pname, GLint *params);
  friend NVSG_API GLenum GLCheckNamedFramebufferStatusEXT (GLuint framebuffer, GLenum target);
  friend NVSG_API void GLNamedFramebufferTexture1DEXT (GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
  friend NVSG_API void GLNamedFramebufferTexture2DEXT (GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
  friend NVSG_API void GLNamedFramebufferTexture3DEXT (GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset);
  friend NVSG_API void GLNamedFramebufferRenderbufferEXT (GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer);
  friend NVSG_API void GLGetNamedFramebufferAttachmentParameterivEXT (GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params);
  friend NVSG_API void GLGenerateTextureMipmapEXT (GLuint texture, GLenum target);
  friend NVSG_API void GLGenerateMultiTexMipmapEXT (GLenum texunit, GLenum target);
  friend NVSG_API void GLFramebufferDrawBufferEXT (GLuint framebuffer, GLenum mode);
  friend NVSG_API void GLFramebufferDrawBuffersEXT (GLuint framebuffer, GLsizei n, const GLenum *bufs);
  friend NVSG_API void GLFramebufferReadBufferEXT (GLuint framebuffer, GLenum mode);
  friend NVSG_API void GLGetFramebufferParameterivEXT (GLuint framebuffer, GLenum pname, GLint *params);
  friend NVSG_API void GLNamedRenderbufferStorageMultisampleEXT (GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height);
  friend NVSG_API void GLNamedRenderbufferStorageMultisampleCoverageEXT (GLuint renderbuffer, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height);
  friend NVSG_API void GLNamedFramebufferTextureEXT (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);
  friend NVSG_API void GLNamedFramebufferTextureLayerEXT (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer);
  friend NVSG_API void GLNamedFramebufferTextureFaceEXT (GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLenum face);
  friend NVSG_API void GLTextureRenderbufferEXT (GLuint texture, GLenum target, GLuint renderbuffer);
  friend NVSG_API void GLMultiTexRenderbufferEXT (GLenum texunit, GLenum target, GLuint renderbuffer);

  // helper
  friend NVSG_API void GLUploadBuffers();

#if defined(_WIN32)
  friend NVSG_API BOOL WGLBindTexImage( HPBUFFERARB hPbuffer, int iBuffer );
  friend NVSG_API BOOL WGLChoosePixelFormat( HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats );
  friend NVSG_API BOOL WGLGetPixelFormatAttribiv(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues );
  friend NVSG_API BOOL WGLGetPixelFormatAttribfv(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues );
  friend NVSG_API HGLRC WGLCreateContext( HDC hdc );
  friend NVSG_API HPBUFFERARB WGLCreatePbuffer( HDC hdc, int pixelFormat, int width, int height, const int *attribList );
  friend NVSG_API BOOL WGLDeleteContext( HGLRC hglrc );
  friend NVSG_API BOOL WGLDestroyPbuffer( HPBUFFERARB hPbuffer );
  friend NVSG_API HGLRC WGLGetCurrentContext(void);
  friend NVSG_API HDC WGLGetCurrentDC(void);
  friend NVSG_API BOOL WGLMakeCurrent( HDC hdc, HGLRC hglrc );
  friend NVSG_API HDC WGLGetPbufferDC( HPBUFFERARB hPbuffer );
  friend NVSG_API BOOL WGLQueryPbuffer( HPBUFFERARB hPbuffer, int attribute, int *value );
  friend NVSG_API int WGLReleasePbufferDC( HPBUFFERARB hPbuffer, HDC hdc );
  friend NVSG_API BOOL WGLReleaseTexImage( HPBUFFERARB hPbuffer, int iBuffer );
  friend NVSG_API BOOL WGLShareLists( HGLRC hglrc1, HGLRC hglrc2 );
  friend NVSG_API BOOL WGLNotifyShareLists( HGLRC hglrc1, HGLRC hglrc2 );
  friend NVSG_API BOOL WGLJoinSwapGroupNV(HDC hDC, GLuint group);
  friend NVSG_API BOOL WGLBindSwapBarrierNV(GLuint group, GLuint barrier);
  friend NVSG_API BOOL WGLQuerySwapGroupNV(HDC hDC, GLuint * group, GLuint * barrier);
  friend NVSG_API BOOL WGLQueryMaxSwapGroupsNV(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers);
  friend NVSG_API BOOL WGLQueryFrameCountNV(HDC hDC, GLuint * count);
  friend NVSG_API BOOL WGLResetFrameCountNV(HDC hDC);
  friend NVSG_API BOOL WGLEnumGpus(UINT iIndex, HGPUNV *hGpu);
  friend NVSG_API BOOL WGLEnumGpuDevices(HGPUNV hGpu, UINT iIndex, PGPU_DEVICE pGpuDevice);
  friend NVSG_API HDC  WGLCreateAffinityDC(const HGPUNV *pGpuList);
  friend NVSG_API BOOL WGLEnumGpusFromAffinityDC(HDC hAffinityDC, UINT iIndex, HGPUNV *hGpu);
  friend NVSG_API BOOL WGLDeleteDC(HDC hAffinityDC);
  friend NVSG_API BOOL WGLSwapInterval(int interval);
  friend NVSG_API int WGLGetSwapInterval(void);
  // routines to handle bypassing of WGLCreateContext and WGLDeleteContext  
  friend NVSG_API void WGLAttachContext(HDC, HGLRC);
  friend NVSG_API bool WGLIsAttached(HGLRC);
  friend NVSG_API void WGLDetachContext(HGLRC);
  // WGL_NV_present_video
  friend NVSG_API int WGLEnumerateVideoDevicesNV(HDC hDC, HVIDEOOUTPUTDEVICENV *phDeviceList);
  friend NVSG_API BOOL WGLBindVideoDeviceNV(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int *piAttribList);
  friend NVSG_API BOOL WGLQueryCurrentContextNV(int iAttribute, int *piValue);
  // WGL_NV_video_capture
  friend NVSG_API BOOL WGLBindVideoCaptureDeviceNV(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice);
  friend NVSG_API UINT WGLEnumerateVideoCaptureDevicesNV(HDC hDc, HVIDEOINPUTDEVICENV *phDeviceList);
  friend NVSG_API BOOL WGLLockVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice);
  friend NVSG_API BOOL WGLQueryVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int *piValue);
  friend NVSG_API BOOL WGLReleaseVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice);

#endif
#if defined(LINUX)
  friend NVSG_API GLXContext GLXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);
  friend NVSG_API void GLXDestroyContext(Display *dpy, GLXContext ctx);
  friend NVSG_API GLXContext GLXGetCurrentContext();
  friend NVSG_API GLXDrawable GLXGetCurrentDrawable();
  friend NVSG_API Bool GLXMakeCurrent(Display *dpy, GLXDrawable drawable, GLXContext ctx);
  // routines to handle bypassing of GLXCreateContext and GLXDestroyContext  
  friend NVSG_API void GLXAttachContext(Display*, GLXContext);
  friend NVSG_API bool GLXNotifyShared(GLXContext, GLXContext);
  friend NVSG_API bool GLXIsAttached(GLXContext);
  friend NVSG_API void GLXDetachContext(GLXContext);

  // GLX_SGI_swap_control
  friend NVSG_API int GLXSwapInterval (int);
  // GLX_SGI_video_sync
  friend NVSG_API int GLXGetVideoSync (unsigned int *);
  friend NVSG_API int GLXWaitVideoSync (int, int, unsigned int *);
  // GLX_NV_swap_group
  friend NVSG_API Bool GLXJoinSwapGroup(Display *dpy, GLXDrawable drawable,
                               GLuint group);
  friend NVSG_API Bool GLXBindSwapBarrier(Display *dpy, GLuint group, 
                                                              GLuint barrier);
  friend NVSG_API Bool GLXQuerySwapGroup(Display *dpy, GLXDrawable drawable,
                                GLuint *group, GLuint *barrier);
  friend NVSG_API Bool GLXQueryMaxSwapGroups(Display *dpy, int screen,
                                    GLuint *maxGroups, GLuint *maxBarriers);
  friend NVSG_API Bool GLXQueryFrameCount(Display *dpy, int screen, 
                                                              GLuint *count);
  friend NVSG_API Bool GLXResetFrameCount(Display *dpy, int screen);

  // GLX_NV_video_out
  friend NVSG_API int GLXGetVideoDevice(Display *dpy, int screen, 
                                int numVideoDevices,
                               GLXVideoDeviceNV *pVideoDevice);
  friend NVSG_API int GLXReleaseVideoDevice(Display *dpy, int screen,
                                   GLXVideoDeviceNV VideoDevice);
  friend NVSG_API int GLXBindVideoImage(Display *dpy, 
                               GLXVideoDeviceNV VideoDevice,
                               GLXPbuffer pbuf, int iVideoBuffer);
  friend NVSG_API int GLXReleaseVideoImage(Display *dpy, GLXPbuffer pbuf);
  friend NVSG_API int GLXSendPbufferToVideo(Display *dpy, GLXPbuffer pbuf,
                                   int iBufferType,
                                   unsigned long *pulCounterPbuffer,
                                   GLboolean bBlock);
  friend NVSG_API int GLXGetVideoInfo(Display *dpy, int screen,
                             GLXVideoDeviceNV VideoDevice,
                             unsigned long *pulCounterOutputVideo,
                             unsigned long *pulCounterOutputPbuffer);
  // GLX_EXT_texture_from_pixmap
  friend NVSG_API void GLXBindTexImage (Display *, GLXDrawable, int, 
                                            const int *);
  friend NVSG_API void GLXReleaseTexImage (Display *, GLXDrawable, int);

  // GLX_NV_present_video
  friend NVSG_API unsigned int *GLXEnumerateVideoDevicesNV(Display *dpy, int screen, int *nelements);
  friend NVSG_API int GLXBindVideoDeviceNV(Display *dpy, unsigned int video_slot, unsigned int video_device, const int *attrib_list);
  // GLX_NV_video_capture
  friend NVSG_API int GLXBindVideoCaptureDeviceNV (Display *dpy, unsigned int video_capture_slot, GLXVideoCaptureDeviceNV device);
  friend NVSG_API GLXVideoCaptureDeviceNV * GLXEnumerateVideoCaptureDevicesNV (Display *dpy, int screen, int *nelements);
  friend NVSG_API void GLXLockVideoCaptureDeviceNV (Display *dpy, GLXVideoCaptureDeviceNV device);
  friend NVSG_API int GLXQueryVideoCaptureDeviceNV (Display *dpy, GLXVideoCaptureDeviceNV device, int attribute, int *value);
  friend NVSG_API void GLXReleaseVideoCaptureDeviceNV (Display *dpy, GLXVideoCaptureDeviceNV device);

#endif

  friend NVSG_API void GLResetAlphaFunc();
  friend NVSG_API void GLResetAlphaTest();
  friend NVSG_API void GLResetAutoNormal();
  friend NVSG_API void GLResetBlend();
  friend NVSG_API void GLResetBlendFunc();
  friend NVSG_API void GLResetBlendFuncSeparate();
  friend NVSG_API void GLResetClearColor();
  friend NVSG_API void GLResetClearDepth();
  friend NVSG_API void GLResetClearStencil();
  friend NVSG_API void GLResetClipPlane( GLenum plane );
  friend NVSG_API void GLResetClipPlaneEquation( GLenum plane );
  friend NVSG_API void GLResetColorLogicOp();
  friend NVSG_API void GLResetColorMask();
  friend NVSG_API void GLResetColorMaterial();
  friend NVSG_API void GLResetColorMaterialMode();
  friend NVSG_API void GLResetCullFace();
  friend NVSG_API void GLResetCullFaceMode();
  friend NVSG_API void GLResetDepthFunc();
  friend NVSG_API void GLResetDepthMask();
  friend NVSG_API void GLResetDepthRange();
  friend NVSG_API void GLResetDepthTest();
  friend NVSG_API void GLResetDither();
  friend NVSG_API void GLResetFog();
  friend NVSG_API void GLResetFogColor();
  friend NVSG_API void GLResetFogDensity();
  friend NVSG_API void GLResetFogEnd();
  friend NVSG_API void GLResetFogMode();
  friend NVSG_API void GLResetFogStart();
  friend NVSG_API void GLResetFrontFace();
  friend NVSG_API void GLResetLight( GLenum light );
  friend NVSG_API void GLResetLighting();
  friend NVSG_API void GLResetLightAmbient( GLenum light );
  friend NVSG_API void GLResetLightConstantAttenuation( GLenum light );
  friend NVSG_API void GLResetLightDiffuse( GLenum light );
  friend NVSG_API void GLResetLightLinearAttenuation( GLenum light );
  friend NVSG_API void GLResetLightModelAmbient();
  friend NVSG_API void GLResetLightModelLocalViewer();
  friend NVSG_API void GLResetLightModelTwoSide();
  friend NVSG_API void GLResetLightPosition( GLenum light );
  friend NVSG_API void GLResetLightQuadraticAttenuation( GLenum light );
  friend NVSG_API void GLResetLightSpecular( GLenum light );
  friend NVSG_API void GLResetLightSpotCutoff( GLenum light );
  friend NVSG_API void GLResetLightSpotDirection( GLenum light );
  friend NVSG_API void GLResetLightSpotExponent( GLenum light );
  friend NVSG_API void GLResetLineSmooth();
  friend NVSG_API void GLResetLineStipple();
  friend NVSG_API void GLResetLineStippleDefinition();
  friend NVSG_API void GLResetLineWidth();
  friend NVSG_API void GLResetLogicOp();
  friend NVSG_API void GLResetMaterialAmbientBack();
  friend NVSG_API void GLResetMaterialAmbientFront();
  friend NVSG_API void GLResetMaterialAmbientFrontAndBack();
  friend NVSG_API void GLResetMaterialDiffuseBack();
  friend NVSG_API void GLResetMaterialDiffuseFront();
  friend NVSG_API void GLResetMaterialDiffuseFrontAndBack();
  friend NVSG_API void GLResetMaterialEmissionBack();
  friend NVSG_API void GLResetMaterialEmissionFront();
  friend NVSG_API void GLResetMaterialEmissionFrontAndBack();
  friend NVSG_API void GLResetMaterialShininessBack();
  friend NVSG_API void GLResetMaterialShininessFront();
  friend NVSG_API void GLResetMaterialShininessFrontAndBack();
  friend NVSG_API void GLResetMaterialSpecularBack();
  friend NVSG_API void GLResetMaterialSpecularFront();
  friend NVSG_API void GLResetMaterialSpecularFrontAndBack();
  friend NVSG_API void GLResetNormalize();
  friend NVSG_API void GLResetRescaleNormal();
  friend NVSG_API void GLResetPointSize();
  friend NVSG_API void GLResetPointSmooth();
  friend NVSG_API void GLResetPolygonModeBack();
  friend NVSG_API void GLResetPolygonModeFront();
  friend NVSG_API void GLResetPolygonModeFrontAndBack();
  friend NVSG_API void GLResetPolygonOffset();
  friend NVSG_API void GLResetPolygonOffsetFill();
  friend NVSG_API void GLResetPolygonOffsetLine();
  friend NVSG_API void GLResetPolygonOffsetPoint();
  friend NVSG_API void GLResetPolygonSmooth();
  friend NVSG_API void GLResetPolygonStipple();
  friend NVSG_API void GLResetScissor();
  friend NVSG_API void GLResetScissorTest();
  friend NVSG_API void GLResetShadeModel();
  friend NVSG_API void GLResetStencilFunc();
  friend NVSG_API void GLResetStencilMask();
  friend NVSG_API void GLResetStencilOp();
  friend NVSG_API void GLResetStencilTest();
  friend NVSG_API void GLResetTexture( GLenum target );

  // effect methods
  friend NVSG_API void GLCgGLBindEffect( nvsg::CgFxEffect * effect );
  friend NVSG_API nvsg::CgFxEffect * GLCgGLGetEffect();
  friend NVSG_API void GLCgGLBindPass( nvsg::CgFxPass pass );
  friend NVSG_API nvsg::CgFxPass GLCgGLGetPass();
  friend NVSG_API void GLCgGLBindProgram( nvsg::CgFxProgram program );
  friend NVSG_API nvsg::CgFxProgram GLCgGLGetProgram();
  friend NVSG_API void GLCgGLApplyProgramSamplers();

#if !defined(DOXYGEN_IGNORE)

  //  uncached versions of otherwise cached functions
  friend NVSG_API void _GLDisable( GLenum cap );
  friend NVSG_API void _GLDisableClientState( GLenum array );
  friend NVSG_API void _GLEnable( GLenum cap );
  friend NVSG_API void _GLEnableClientState( GLenum array );
  friend NVSG_API void _GLFogf( GLenum pname, GLfloat param );
  friend NVSG_API void _GLFogfv( GLenum pname, const GLfloat * params );
  friend NVSG_API void _GLFogi( GLenum pname, GLint param );
  friend NVSG_API void _GLFogiv( GLenum pname, const GLint * params );
  friend NVSG_API GLboolean _GLIsEnabled( GLenum cap );
  friend NVSG_API void _GLLightf( GLenum light, GLenum pname, GLfloat param );
  friend NVSG_API void _GLLightfv( GLenum light, GLenum pname, const GLfloat *params );
  friend NVSG_API void _GLLighti( GLenum light, GLenum pname, GLint param );
  friend NVSG_API void _GLLightiv( GLenum light, GLenum pname, GLint *params );
  friend NVSG_API void _GLLightModelf( GLenum pname, GLfloat param );
  friend NVSG_API void _GLLightModelfv( GLenum pname, const GLfloat *params );
  friend NVSG_API void _GLLightModeli( GLenum pname, GLint param );
  friend NVSG_API void _GLLightModeliv( GLenum pname, const GLint *params );
  friend NVSG_API void _GLMaterialf( GLenum face, GLenum pname, GLfloat param );
  friend NVSG_API void _GLMaterialfv( GLenum face, GLenum pname, const GLfloat *params );
  friend NVSG_API void _GLMateriali( GLenum face, GLenum pname, GLint param );
  friend NVSG_API void _GLMaterialiv( GLenum face, GLenum pname, const GLint *params );
  friend NVSG_API void _GLPolygonMode( GLenum face, GLenum mode );
  friend NVSG_API void _GLTexEnvf( GLenum target, GLenum pname, GLfloat param );
  friend NVSG_API void _GLTexEnvfv( GLenum target, GLenum pname, const GLfloat *params );
  friend NVSG_API void _GLTexEnvi( GLenum target, GLenum pname, GLint param );
  friend NVSG_API void _GLTexEnviv( GLenum target, GLenum pname, const GLint *params );
  friend NVSG_API void _GLTexParameterf( GLenum target, GLenum pname, GLfloat param );
  friend NVSG_API void _GLTexParameteri( GLenum target, GLenum pname, GLint param );
  friend NVSG_API void _GLTexParameterfv( GLenum target, GLenum pname, const GLfloat *params );
  friend NVSG_API void _GLTexParameteriv( GLenum target, GLenum pname, const GLint *params );

#if defined( GL_CACHING )
  friend NVSG_API void _GLUpdateActiveTexture( void );
  friend NVSG_API void _GLUpdateClientActiveTexture( void );
#endif

#if !defined(NDEBUG)
  friend NVSG_API void ASSERT_GL_CURRENT_CONTEXT_CONFORMITY();
#endif

#endif

  friend NVSG_API void GLGenDisplayListCache(nvsg::DALHost * host, GLDisplayListCache *& cache);
  friend NVSG_API void GLDeleteDisplayListCaches();
  friend NVSG_API void GLGenArrayBufferCache(nvsg::DALHost * host, GLArrayBufferCache *& cache);
  friend NVSG_API void GLGenElementArrayBufferCache(nvsg::DALHost * host, GLElementArrayBufferCache *& cache);
  friend NVSG_API void GLGenPixelBufferCache(nvsg::DALHost * host, GLPixelBufferCache *& cache);
  friend NVSG_API void GLDeleteBufferCaches();
  friend NVSG_API void GLGenTextureCache(nvsg::DALHost * host, GLTextureCache *& cache);
  friend NVSG_API void GLGenTextureCache(nvsg::DALHost * host, GLTextureCache *& cache, GLuint texture, GLenum target);
  friend NVSG_API void GLDeleteTextureCaches();

  friend NVSG_API bool GLIsValidDisplayListCache(const GLDisplayListCache * cache);
  friend NVSG_API bool GLIsValidArrayBufferCache(const GLArrayBufferCache * cache);
  friend NVSG_API bool GLIsValidElementArrayBufferCache(const GLElementArrayBufferCache * cache);
  friend NVSG_API bool GLIsValidPixelBufferCache(const GLPixelBufferCache * cache);
  friend NVSG_API bool GLIsValidTextureCache(const GLTextureCache * cache);

  /*! \brief Returns a GLTexImageFmt according to the passed in PixelFormat and PixelDataType descriptors. 
   *
   *  \param tfmt The function returns a GLTexImageFmt structure representing the best match of an
   *  OpenGL texture image format according to the input PixelFormat and PixelDataType descriptors.
   *  \param fmt Input PixelFormat descriptor of an nvsg::Image
   *  \param type Input PixelDataType descriptor of an nvsg::Image
   *  \param gpuFmt Input GPU format descriptor of an nvsg::TextureHost
   *
   *  \remarks 
   *  The function translates the passed in SceniX specific PixelFormat and PixelDataType descriptors
   *  into an OpenGL specific texture image format descriptor. Thereby, the function always tries to 
   *  return the 'best match' of a texture image format which is supported by the current hardware 
   *  and driver configuration.
   *
   *  \returns 
   *  True, if a valid format could be translated from the input format, data type and GPU format, false otherwise.
   */
  friend NVSG_API bool GLGetTexImageFmt( GLTexImageFmt & tfmt, 
                nvsg::Image::PixelFormat fmt, nvsg::Image::PixelDataType type, 
                nvsg::TextureHost::TextureGPUFormat gpuFmt = 
                                            nvsg::TextureHost::TGF_DEFAULT );

  friend NVSG_API void GLUploadTexImage1D(GLint intFmt, GLint usrFmt, GLint type, nvsg::TextureHost*, GLTextureCache*);
  friend NVSG_API void GLUploadTexImage2D(GLint intFmt, GLint usrFmt, GLint type, nvsg::TextureHost*, GLTextureCache*);
  friend NVSG_API void GLUploadTexImage3D(GLint intFmt, GLint usrFmt, GLint type, nvsg::TextureHost*, GLTextureCache*);
  friend NVSG_API void GLUploadTexImageCube(GLint intFmt, GLint usrFmt, GLint type, nvsg::TextureHost*, GLTextureCache*);
  friend NVSG_API void GLUploadTexImage1DArray(GLint intFmt, GLint usrFmt, GLint type, nvsg::TextureHost*, GLTextureCache*);
  friend NVSG_API void GLUploadTexImage2DArray(GLint intFmt, GLint usrFmt, GLint type, nvsg::TextureHost*, GLTextureCache*);
  friend NVSG_API void GLUploadTexImageRectangle(GLint intFmt, GLint usrFmt, GLint type, nvsg::TextureHost*, GLTextureCache*);

  friend NVSG_API GLenum GLGetTexWrapMode( nvsg::TextureWrapMode twm );

public:  // public interface
#if defined(LINUX)
  /*! \brief Get the Display of this GLContext (Linux only).
   *  \return The pointer to the Display of this GLContext. */
  inline Display* getDisplay() const { return m_dpy; }  // public access to the Display
#endif 

  // Provide access to otherwise private shared data
  VBOCachingState *getVBOState() const;

private: // private interface

#if defined(_WIN32)
  GLContext(HDC hdc, HGLRC hglrc);  
#elif defined(LINUX)
  GLContext(Display *dpy, GLXContext ctx);  
#endif
  ~GLContext();

#if defined(_WIN32)
  void init(); // init function pointers and caches
#elif defined(LINUX)
  void init(GLXDrawable drw);
#endif
  void exit(); // cleanup when context dies

  void deleteGLObjects(); // deletes all GLObjects
  void deleteSharedGLObjects(); // deletes only shared GLObjects

  // Share sharable GL objects with the GLContext referenced through \a source.
  void share(const GLContext& source);

  // helper

  void addBuffers(GLsizei n, const GLuint * names);
  void removeBuffers(GLsizei n, const GLuint * names);
  ConstNameSetIter firstBuffer() const;
  ConstNameSetIter lastBuffer() const;
  bool isBuffer(GLuint name) const;
  void releaseBuffers();

  void addLists(GLsizei n, GLuint first);
  void removeLists(GLsizei n, GLuint first);
  ConstNameSetIter firstList() const;
  ConstNameSetIter lastList() const;
  bool isList(GLuint name) const;
  void releaseLists();

  void addPrograms(GLsizei n, const GLuint * names);
  void removePrograms(GLsizei n, const GLuint * names);
  ConstNameSetIter firstProgram() const;
  ConstNameSetIter lastProgram() const;
  bool isProgram(GLuint name) const;
  void releasePrograms();

  void addTextures(GLsizei n, const GLuint * names);
  void removeTextures(GLsizei n, const GLuint * names);
  ConstNameSetIter firstTexture() const;
  ConstNameSetIter lastTexture() const;
  bool isTexture(GLuint name) const;
  void releaseTextures();

  void addQueries(GLsizei n, const GLuint * names);
  void removeQueries(GLsizei n, const GLuint * names);
  ConstNameSetIter firstQuery() const;
  ConstNameSetIter lastQuery() const;
  bool isQuery(GLuint name) const;
  void releaseQueries();

  void genTextureCache(nvsg::DALHost * host, GLTextureCache *& cache);
  void genTextureCache(nvsg::DALHost * host, GLTextureCache *& cache, GLuint texture, GLenum target);
  void deleteTextureCaches();
  void genDisplayListCache(nvsg::DALHost * host, GLDisplayListCache *& cache);
  void deleteDisplayListCaches();
  void genElementArrayBufferCache(nvsg::DALHost * host, GLElementArrayBufferCache *& cache);
  void genArrayBufferCache(nvsg::DALHost * host, GLArrayBufferCache *& cache);
  void genPixelBufferCache(nvsg::DALHost * host, GLPixelBufferCache *& cache);
  void deleteBufferCaches();

  bool isValidDisplayListCache(const GLDisplayListCache * cache) const;
  bool isValidArrayBufferCache(const GLArrayBufferCache * cache) const;
  bool isValidElementArrayBufferCache(const GLElementArrayBufferCache * cache) const;
  bool isValidPixelBufferCache(const GLPixelBufferCache * cache) const;
  bool isValidTextureCache(const GLTextureCache * cache) const;

  void genBuffers( GLsizei n, GLuint * buffers );

private:  //  private members
#if defined( GL_CACHING )
  // helper class to make back/front states easier to handle
  template < class T >
  class BFState
  {
  public:
    // constructor using T to fill both back and front state
    explicit BFState( const T& t )
      : backState(t)
      , frontState(t)
    {};
    BFState & operator=( const BFState& rhs )
    {
      if( this != &rhs )
      {
        backState = rhs.backState;
        frontState = rhs.frontState;
      }
      return *this;
    }
    BFState & operator=( const T& rhs )
    {
      backState = rhs;
      frontState = rhs;
      return *this;
    }

    bool operator==( const BFState& rhs ) const { return backState == rhs.backState && frontState == rhs.frontState; }
    bool operator!=( const BFState& rhs ) const { return !(*this == rhs); }
    bool operator==( const T& rhs ) const { return backState == rhs && frontState == rhs; }
    bool operator!=( const T& rhs ) const { return !(*this == rhs); }

    T & operator[]( GLenum face )
    {
      if( face == GL_BACK )
      {
        return backState;
      }
      else if( face == GL_FRONT )
      {
        return frontState;
      }
      else
      {
        NVSG_ASSERT(false);
        return frontState;
      }
    }
    const T & operator[]( GLenum face ) const
    {
      if( face == GL_BACK )
      {
        return backState;
      }
      else if( face == GL_FRONT )
      {
        return frontState;
      }
      else
      {
        NVSG_ASSERT(false);
        return frontState;
      }
    }
  private:
    T backState;
    T frontState;
  };

  // inline helper functions to simplify BFState comparisons and updates 
  // if passed f==GL_FRONT_AND_BACK, the whole BFState is compared/updated,
  // otherwise the corresponding face is compared/updated
  template <typename T>
  inline bool testBFState(GLenum f, const BFState<T>& s, const T& v)
  {
    NVSG_ASSERT( f==GL_FRONT_AND_BACK || f==GL_FRONT || f==GL_BACK );
    if ( f==GL_FRONT_AND_BACK ) { return s==v; }
    return (s[f] == v);
  } 
  template <typename T>
  inline void assignBFState(GLenum f, BFState<T>& s, const T& v)
  {
    NVSG_ASSERT( f==GL_FRONT_AND_BACK || f==GL_FRONT || f==GL_BACK );
    if ( f==GL_FRONT_AND_BACK ) { s = v; return; }
    s[f] = v; 
  }

  class ColorBufferAttributes
  {
    public:
      ColorBufferAttributes( void )
        : m_alphaFunc(GL_ALWAYS)
        , m_alphaRef(0.0f)
        , m_blendSrcRGB(GL_ONE)
        , m_blendSrcAlpha(GL_ONE)
        , m_blendDstRGB(GL_ZERO)
        , m_blendDstAlpha(GL_ZERO)
        , m_clearColor(0.0f,0.0f,0.0f,0.0f)
      {
        for ( size_t i=0 ; i<4 ; i++ )
        {
          m_colorWriteMask[i] = GL_TRUE;
        }
      }

    public:
      GLenum        m_alphaFunc;
      GLclampf      m_alphaRef;
      GLboolean     m_colorWriteMask[4];
      GLenum        m_blendSrcRGB;
      GLenum        m_blendSrcAlpha;
      GLenum        m_blendDstRGB;
      GLenum        m_blendDstAlpha;
      nvmath::Vec4f m_clearColor;
  };

  class ColorBufferEnableAttributes
  {
    public:
      ColorBufferEnableAttributes( void )
        : m_alphaTest(GL_FALSE)
        , m_blend(GL_FALSE)
        , m_colorLogicOp(GL_FALSE)
        , m_dither(GL_TRUE)
        , m_logicOp(GL_FALSE)
      {}

    public:
      GLboolean m_alphaTest;
      GLboolean m_blend;
      GLboolean m_colorLogicOp;
      GLboolean m_dither;
      GLboolean m_logicOp;
  };

  class DepthBufferAttributes
  {
    public:
      DepthBufferAttributes( void )
        : m_depthClearValue(1.0f)
        , m_depthFunc(GL_LESS)
        , m_depthWriting(GL_TRUE)
      {}

    public:
      GLclampd  m_depthClearValue;
      GLenum    m_depthFunc;
      GLboolean m_depthWriting;
  };

  class DepthBufferEnableAttributes
  {
    public:
      DepthBufferEnableAttributes( void )
        : m_depthTesting(GL_FALSE)
      {}

    public:
      GLboolean m_depthTesting;
  };

  class EvalAttributes
  {
    public:
      EvalAttributes( void )
        : m_autoNormal(GL_FALSE)
      {}

    public:
      GLboolean m_autoNormal;
  };

  class FogAttributes
  {
    public:
      FogAttributes()
        : m_color(0.0f,0.0f,0.0f,0.0f)
        , m_density(1.0f)
        , m_end(1.0f)
        , m_mode(GL_EXP)
        , m_start(0.0f)
      {}

    public:
      nvmath::Vec4f m_color;
      GLfloat       m_density;
      GLfloat       m_end;
      GLenum        m_mode;
      GLfloat       m_start;
  };

  class FogEnableAttributes
  {
    public:
      FogEnableAttributes( void )
        : m_fog(GL_FALSE)
      {}

    public:
      GLboolean m_fog;
  };

  class LightParameters
  {
    public:
      LightParameters( void )
        : m_ambient(0.0f,0.0f,0.0f,1.0f)
        , m_attenuation(1.0f,0.0f,0.0f)
        , m_diffuse(0.0f,0.0f,0.0f,1.0f)
        , m_position(0.0f,0.0f,1.0f,0.0f)
        , m_specular(0.0f,0.0f,0.0f,1.0f)
        , m_spotCutoff(180.0f)
        , m_spotDirection(0.0f,0.0f,-1.0f,0.0f)
        , m_spotExponent(0.0f)
      {}

    public:
      nvmath::Vec4f m_ambient;
      nvmath::Vec3f m_attenuation;
      nvmath::Vec4f m_diffuse;
      nvmath::Vec4f m_position;
      nvmath::Vec4f m_specular;
      float         m_spotCutoff;
      nvmath::Vec4f m_spotDirection;
      float         m_spotExponent;
  };

  class LightingAttributes
  {
    public:
      LightingAttributes( void )
        : m_colorMaterial(GL_FALSE)
        , m_colorMaterialFace(GL_FRONT_AND_BACK)
        , m_colorMaterialMode(GL_AMBIENT_AND_DIFFUSE)
        , m_lightModelLocalViewer(GL_FALSE)
        , m_lightModelTwoSide(GL_FALSE)
        , m_lightModelAmbient(0.2f,0.2f,0.2f,1.0f)
        , m_shadeModel(GL_SMOOTH)
      {
        m_materialAmbient[NVSG_CACHE_FRONT]   = m_materialAmbient[NVSG_CACHE_BACK]   = nvmath::Vec4f( 0.2f, 0.2f, 0.2f, 1.0f );
        m_materialDiffuse[NVSG_CACHE_FRONT]   = m_materialDiffuse[NVSG_CACHE_BACK]   = nvmath::Vec4f( 0.8f, 0.8f, 0.8f, 1.0f );
        m_materialSpecular[NVSG_CACHE_FRONT]  = m_materialSpecular[NVSG_CACHE_BACK]  = nvmath::Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
        m_materialEmission[NVSG_CACHE_FRONT]  = m_materialEmission[NVSG_CACHE_BACK]  = nvmath::Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
        m_materialShininess[NVSG_CACHE_FRONT] = m_materialShininess[NVSG_CACHE_BACK] = 0.0f;
      }

    public:
      GLboolean                     m_colorMaterial;
      GLenum                        m_colorMaterialFace;
      GLenum                        m_colorMaterialMode;
      GLboolean                     m_lightModelLocalViewer;
      GLboolean                     m_lightModelTwoSide;
      nvmath::Vec4f                 m_lightModelAmbient;
      std::vector<LightParameters>  m_lightParameters;
      nvmath::Vec4f                 m_materialAmbient[2];
      nvmath::Vec4f                 m_materialDiffuse[2];
      nvmath::Vec4f                 m_materialSpecular[2];
      nvmath::Vec4f                 m_materialEmission[2];
      float                         m_materialShininess[2];
      GLenum                        m_shadeModel;
  };

  class LightingEnableAttributes
  {
    public:
      LightingEnableAttributes( void )
        : m_lighting(GL_FALSE)
      {}

    public:
      std::vector<GLboolean>  m_lights;
      GLboolean               m_lighting;
  };

  class LineAttributes
  {
    public:
      LineAttributes( void )
        : m_width(1.0f)
        , m_stipplePattern(0xFFFF)
        , m_stippleRepeat(1)
      {}

    public:
      float     m_width;
      GLushort  m_stipplePattern;
      GLint     m_stippleRepeat;
  };

  class LineEnableAttributes
  {
    public:
      LineEnableAttributes( void )
        : m_smooth(GL_FALSE)
        , m_stipple(GL_FALSE)
      {}

    public:
      GLboolean m_smooth;
      GLboolean m_stipple;
  };

  class PointAttributes
  {
    public:
      PointAttributes( void )
        : m_size(1.0f)
      {}

    public:
      float m_size;
  };

  class PointEnableAttributes
  {
    public:
      PointEnableAttributes( void )
        : m_smooth(GL_FALSE)
      {}

    public:
      GLboolean m_smooth;
  };

  class PolygonAttributes
  {
    public:
      PolygonAttributes( void )
        : m_cullFaceMode(GL_BACK)
        , m_frontFaceMode(GL_CCW)
        , m_offsetFactor(0.0f)
        , m_offsetBias(0.0f)
      {
        m_rasterMode[NVSG_CACHE_FRONT] = m_rasterMode[NVSG_CACHE_BACK] = GL_FILL;
      }


    public:
      GLenum  m_cullFaceMode;
      GLenum  m_frontFaceMode;
      GLenum  m_rasterMode[2];
      GLfloat m_offsetFactor;
      GLfloat m_offsetBias;
  };

  class PolygonEnableAttributes
  {
    public:
      PolygonEnableAttributes( void )
        : m_cullFace(GL_FALSE)
        , m_offsetFill(GL_FALSE)
        , m_offsetLine(GL_FALSE)
        , m_offsetPoint(GL_FALSE)
        , m_smooth(GL_FALSE)
        , m_stipple(GL_FALSE)
      {}

    public:
      GLboolean m_cullFace;
      GLboolean m_offsetFill;
      GLboolean m_offsetLine;
      GLboolean m_offsetPoint;
      GLboolean m_smooth;
      GLboolean m_stipple;
  };

  class ScissorAttributes
  {
    public:
      GLint   m_x;
      GLint   m_y;
      GLsizei m_width;
      GLsizei m_height;
  };

  class ScissorEnableAttributes
  {
    public:
      ScissorEnableAttributes()
        : m_scissorTest(GL_FALSE)
      {}

    public:
      GLboolean m_scissorTest;
  };

  class StencilBufferAttributes
  {
    public:
      StencilBufferAttributes( void )
        : m_stencilClearValue(0)
        , m_stencilFail(GL_KEEP)
        , m_stencilFunc(GL_ALWAYS)
        , m_stencilPassDepthFail(GL_KEEP)
        , m_stencilPassDepthPass(GL_KEEP)
        , m_stencilRef(0)
        , m_stencilWriteMask(0xFFFFFFFF)
        , m_stencilValueMask(0xFFFFFFFF)
      {}

    public:
      GLint             m_stencilClearValue;
      BFState<GLenum>   m_stencilFail;
      BFState<GLenum>   m_stencilFunc;
      BFState<GLenum>   m_stencilPassDepthFail;
      BFState<GLenum>   m_stencilPassDepthPass;
      BFState<GLint>    m_stencilRef;
      BFState<GLuint>   m_stencilWriteMask;
      BFState<GLuint>   m_stencilValueMask;
  };

  class StencilBufferEnableAttributes
  {
    public:
      StencilBufferEnableAttributes()
        :m_stencilTest(GL_FALSE)
      {}

    public:
      GLboolean m_stencilTest;
  };

  class TextureAttributesPerUnit
  {
    public:
      TextureAttributesPerUnit( void )
        : m_envColor(0.0f,0.0f,0.0f,0.0f)
        , m_envMode(GL_MODULATE)
      {}

    public:
      nvmath::Vec4f                       m_envColor;
      GLenum                              m_envMode;
  };

  class TextureAttributes
  {
    public:
      TextureAttributes( void )
        : m_active(0)
        , m_toActivate(0)
      {}

    public:
      std::vector<TextureAttributesPerUnit> m_unit;
      GLenum                                m_active;
      GLenum                                m_toActivate;
  };

  class TextureEnableParameters
  {
    public:
      TextureEnableParameters( void )
      {
        for ( size_t i=0 ; i<4 ; i++ )
        {
          m_gen[i] = GL_FALSE;
        }
      }

    public:
      std::map<GLenum,GLboolean>  m_texture;  // texture enable per target
      GLboolean                   m_gen[4];
  };

  class TextureEnableAttributes
  {
    public:
      std::vector<TextureEnableParameters> m_unit;
  };

  class TransformAttributes
  {
    public:
      TransformAttributes( void )
        : m_matrixMode(GL_MODELVIEW)
      {}

    public:
      std::vector<nvmath::Vec4d>  m_clipPlaneEquations;
      GLenum                      m_matrixMode;
  };

  class TransformEnableAttributes
  {
    public:
      TransformEnableAttributes( void )
        : m_normalize(GL_FALSE)
        , m_rescaleNormal(GL_FALSE)
      {}

    public:
      std::vector<GLboolean>  m_clipPlanes;
      GLboolean               m_normalize;
      GLboolean               m_rescaleNormal;
  };

  class VertexArrayAttributes
  {
    public:
      VertexArrayAttributes( void )
        : m_vertexArray(GL_FALSE)
        , m_normalArray(GL_FALSE)
        , m_colorArray(GL_FALSE)
        , m_secondaryColorArray(GL_FALSE)
        , m_fogCoordArray(GL_FALSE)
        , m_indexArray(GL_FALSE)
        , m_edgeFlagArray(GL_FALSE)
        , m_elementArrayUnified(GL_FALSE)
        , m_vertexAttribArrayUnified(GL_FALSE)
        , m_primitiveRestart(GL_FALSE)
        , m_primitiveRestartIndex(0)
        , m_clientTextureActive(0)
        , m_clientTextureToActivate(0)
      {}

    public:
      // enable states for vertex attribute arrayz
      GLboolean m_vertexArray;
      GLboolean m_normalArray;
      GLboolean m_colorArray;
      GLboolean m_secondaryColorArray;
      GLboolean m_fogCoordArray;
      GLboolean m_indexArray;
      GLboolean m_edgeFlagArray;
      std::vector<GLboolean> m_textureCoordArray;
      std::vector<GLboolean> m_vertexAttribArray;

      // feature enable states
      GLboolean m_elementArrayUnified;
      GLboolean m_vertexAttribArrayUnified;
      GLboolean m_primitiveRestart;

      GLuint    m_primitiveRestartIndex;
      GLuint    m_clientTextureActive;
      GLuint    m_clientTextureToActivate;
  };

  class ViewportAttributes
  {
    public:
      ViewportAttributes( void )
        : m_depthRangeFar(1.0)
        , m_depthRangeNear(0.0)
      {}

    public:
      GLclampd  m_depthRangeFar;
      GLclampd  m_depthRangeNear;
  };

  ColorBufferAttributes         m_colorBufferAttributes;
  ColorBufferEnableAttributes   m_colorBufferEnableAttributes;
  DepthBufferAttributes         m_depthBufferAttributes;
  DepthBufferEnableAttributes   m_depthBufferEnableAttributes;
  EvalAttributes                m_evalAttributes;
  FogAttributes                 m_fogAttributes;
  FogEnableAttributes           m_fogEnableAttributes;
  LightingAttributes            m_lightingAttributes;
  LightingEnableAttributes      m_lightingEnableAttributes;
  LineAttributes                m_lineAttributes;
  LineEnableAttributes          m_lineEnableAttributes;
  PointAttributes               m_pointAttributes;
  PointEnableAttributes         m_pointEnableAttributes;
  PolygonAttributes             m_polygonAttributes;
  PolygonEnableAttributes       m_polygonEnableAttributes;
  ScissorAttributes             m_scissorAttributes;
  ScissorEnableAttributes       m_scissorEnableAttributes;
  StencilBufferAttributes       m_stencilBufferAttributes;
  StencilBufferEnableAttributes m_stencilBufferEnableAttributes;
  TextureAttributes             m_textureAttributes;
  TextureEnableAttributes       m_textureEnableAttributes;
  TransformAttributes           m_transformAttributes;
  TransformEnableAttributes     m_transformEnableAttributes;
  VertexArrayAttributes         m_vertexArrayAttributes;
  ViewportAttributes            m_viewportAttributes;

  std::stack<GLbitfield>                    m_pushMasks;
  std::stack<GLbitfield>                    m_clientPushMasks;
  std::stack<ColorBufferAttributes>         m_colorBufferAttributesStack;
  std::stack<ColorBufferEnableAttributes>   m_colorBufferEnableAttributesStack;
  std::stack<DepthBufferAttributes>         m_depthBufferAttributesStack;
  std::stack<DepthBufferEnableAttributes>   m_depthBufferEnableAttributesStack;
  std::stack<EvalAttributes>                m_evalAttributesStack;
  std::stack<FogAttributes>                 m_fogAttributesStack;
  std::stack<FogEnableAttributes>           m_fogEnableAttributesStack;
  std::stack<LightingAttributes>            m_lightingAttributesStack;
  std::stack<LightingEnableAttributes>      m_lightingEnableAttributesStack;
  std::stack<LineAttributes>                m_lineAttributesStack;
  std::stack<LineEnableAttributes>          m_lineEnableAttributesStack;
  std::stack<PointAttributes>               m_pointAttributesStack;
  std::stack<PointEnableAttributes>         m_pointEnableAttributesStack;
  std::stack<PolygonAttributes>             m_polygonAttributesStack;
  std::stack<PolygonEnableAttributes>       m_polygonEnableAttributesStack;
  std::stack<ScissorAttributes>             m_scissorAttributesStack;
  std::stack<ScissorEnableAttributes>       m_scissorEnableAttributesStack;
  std::stack<StencilBufferAttributes>       m_stencilBufferAttributesStack;
  std::stack<StencilBufferEnableAttributes> m_stencilBufferEnableAttributesStack;
  std::stack<TextureAttributes>             m_textureAttributesStack;
  std::stack<TextureEnableAttributes>       m_textureEnableAttributesStack;
  std::stack<TransformAttributes>           m_transformAttributesStack;
  std::stack<TransformEnableAttributes>     m_transformEnableAttributesStack;
  std::stack<VertexArrayAttributes>         m_vertexArrayAttributesStack;
  std::stack<ViewportAttributes>            m_viewportAttributesStack;

# if defined(GL_SUPPORT_RESET)
  class PreviousAttributes
  {
    public:
      PreviousAttributes()
        : m_alphaFunc(GL_ALWAYS)
        , m_alphaRef(0.0f)
        , m_alphaTest(GL_FALSE)
        , m_autoNormal(GL_FALSE)
        , m_blend(GL_FALSE)
        , m_blendSrcRGB(GL_ONE)
        , m_blendSrcAlpha(GL_ONE)
        , m_blendDstRGB(GL_ZERO)
        , m_blendDstAlpha(GL_ZERO)
        , m_clearColor(0.0f,0.0f,0.0f,0.0f)
        , m_colorLogicOp(GL_FALSE)
        , m_colorMaterial(GL_FALSE)
        , m_colorMaterialFace(GL_FRONT_AND_BACK)
        , m_colorMaterialMode(GL_AMBIENT_AND_DIFFUSE)
        , m_cullFace(GL_FALSE)
        , m_cullFaceMode(GL_BACK)
        , m_depthClearValue(1.0f)
        , m_depthFunc(GL_LESS)
        , m_depthRangeFar(1.0)
        , m_depthRangeNear(0.0)
        , m_depthTesting(GL_FALSE)
        , m_depthWriting(GL_TRUE)
        , m_dither(GL_TRUE)
        , m_fog(GL_FALSE)
        , m_fogColor(0.0f,0.0f,0.0f,0.0f)
        , m_fogDensity(1.0f)
        , m_fogEnd(1.0f)
        , m_fogMode(GL_EXP)
        , m_fogStart(0.0f)
        , m_frontFaceMode(GL_CCW)
        , m_lighting(GL_FALSE)
        , m_lightModelAmbient(0.2f,0.2f,0.2f,1.0f)
        , m_lightModelLocalViewer(GL_FALSE)
        , m_lightModelTwoSide(GL_FALSE)
        , m_lineSmooth(GL_FALSE)
        , m_lineStipple(GL_FALSE)
        , m_lineStipplePattern(0xFFFF)
        , m_lineStippleRepeat(1)
        , m_lineWidth(1.0f)
        , m_logicOp(GL_FALSE)
        , m_normalize(GL_FALSE)
        , m_rescaleNormal(GL_FALSE)
        , m_pointSize(1.0f)
        , m_pointSmooth(GL_FALSE)
        , m_polygonOffsetFactor(0.0f)
        , m_polygonOffsetBias(0.0f)
        , m_polygonSmooth(GL_FALSE)
        , m_polygonStipple(GL_FALSE)
        , m_offsetFill(GL_FALSE)
        , m_offsetLine(GL_FALSE)
        , m_offsetPoint(GL_FALSE)
        , m_scissorTest(GL_FALSE)
        , m_shadeModel(GL_SMOOTH)
        , m_stencilClearValue(0)
        , m_stencilFail(GL_KEEP)
        , m_stencilFunc(GL_ALWAYS)
        , m_stencilPassDepthFail(GL_KEEP)
        , m_stencilPassDepthPass(GL_KEEP)
        , m_stencilRef(0)
        , m_stencilTest(GL_FALSE)
        , m_stencilValueMask(0xFFFFFFFF)
        , m_stencilWriteMask(0xFFFFFFFF)
      {
        for ( size_t i=0 ; i<4 ; i++ )
        {
          m_colorWriteMask[i] = GL_TRUE;
        }
        m_materialAmbient[NVSG_CACHE_FRONT]   = m_materialAmbient[NVSG_CACHE_BACK]   = nvmath::Vec4f( 0.2f, 0.2f, 0.2f, 1.0f );
        m_materialDiffuse[NVSG_CACHE_FRONT]   = m_materialDiffuse[NVSG_CACHE_BACK]   = nvmath::Vec4f( 0.8f, 0.8f, 0.8f, 1.0f );
        m_materialSpecular[NVSG_CACHE_FRONT]  = m_materialSpecular[NVSG_CACHE_BACK]  = nvmath::Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
        m_materialEmission[NVSG_CACHE_FRONT]  = m_materialEmission[NVSG_CACHE_BACK]  = nvmath::Vec4f( 0.0f, 0.0f, 0.0f, 1.0f );
        m_materialShininess[NVSG_CACHE_FRONT] = m_materialShininess[NVSG_CACHE_BACK] = 0.0f;
        m_rasterMode[NVSG_CACHE_FRONT] = m_rasterMode[NVSG_CACHE_BACK] = GL_FILL;
      }

    public:
      GLenum                                m_alphaFunc;
      GLclampf                              m_alphaRef;
      GLboolean                             m_alphaTest;
      GLboolean                             m_autoNormal;
      GLboolean                             m_blend;
      GLenum                                m_blendSrcRGB;
      GLenum                                m_blendSrcAlpha;
      GLenum                                m_blendDstRGB;
      GLenum                                m_blendDstAlpha;
      nvmath::Vec4f                         m_clearColor;
      std::vector<GLboolean>                m_clipPlanes;
      std::vector<nvmath::Vec4d>            m_clipPlaneEquations;
      GLboolean                             m_colorLogicOp;
      GLboolean                             m_colorMaterial;
      GLenum                                m_colorMaterialFace;
      GLenum                                m_colorMaterialMode;
      GLboolean                             m_colorWriteMask[4];
      GLboolean                             m_cullFace;
      GLenum                                m_cullFaceMode;
      GLclampd                              m_depthClearValue;
      GLenum                                m_depthFunc;
      GLclampd                              m_depthRangeFar;
      GLclampd                              m_depthRangeNear;
      GLboolean                             m_depthTesting;
      GLboolean                             m_depthWriting;
      GLboolean                             m_dither;
      GLboolean                             m_fog;
      nvmath::Vec4f                         m_fogColor;
      GLfloat                               m_fogDensity;
      GLfloat                               m_fogEnd;
      GLenum                                m_fogMode;
      GLfloat                               m_fogStart;
      GLenum                                m_frontFaceMode;
      GLboolean                             m_lighting;
      std::vector<GLboolean>                m_lights;
      nvmath::Vec4f                         m_lightModelAmbient;
      std::vector<LightParameters>          m_lightParameters;
      GLboolean                             m_lightModelLocalViewer;
      GLboolean                             m_lightModelTwoSide;
      GLboolean                             m_lineSmooth;
      GLboolean                             m_lineStipple;
      GLushort                              m_lineStipplePattern;
      GLint                                 m_lineStippleRepeat;
      GLfloat                               m_lineWidth;
      GLboolean                             m_logicOp;
      nvmath::Vec4f                         m_materialAmbient[2];
      nvmath::Vec4f                         m_materialDiffuse[2];
      nvmath::Vec4f                         m_materialSpecular[2];
      nvmath::Vec4f                         m_materialEmission[2];
      float                                 m_materialShininess[2];
      GLboolean                             m_normalize;
      GLboolean                             m_rescaleNormal;
      GLfloat                               m_pointSize;
      GLboolean                             m_pointSmooth;
      GLfloat                               m_polygonOffsetFactor;
      GLfloat                               m_polygonOffsetBias;
      GLboolean                             m_polygonSmooth;
      GLboolean                             m_polygonStipple;
      GLboolean                             m_offsetFill;
      GLboolean                             m_offsetLine;
      GLboolean                             m_offsetPoint;
      GLenum                                m_rasterMode[2];
      GLsizei                               m_scissorHeight;
      GLboolean                             m_scissorTest;
      GLsizei                               m_scissorWidth;
      GLint                                 m_scissorX;
      GLint                                 m_scissorY;
      GLenum                                m_shadeModel;
      GLint                                 m_stencilClearValue;
      BFState<GLenum>                       m_stencilFail;
      BFState<GLenum>                       m_stencilFunc;
      BFState<GLenum>                       m_stencilPassDepthFail;
      BFState<GLenum>                       m_stencilPassDepthPass;
      BFState<GLint>                        m_stencilRef;
      GLboolean                             m_stencilTest;
      BFState<GLuint>                       m_stencilValueMask;
      BFState<GLuint>                       m_stencilWriteMask;
      std::vector<TextureEnableParameters>  m_textureEnables;
  };

  PreviousAttributes  m_previousAttributes;
# endif // GL_SUPPORT_RESET

#endif // GL_CACHING
  GLObjects     * m_glObjects;    // managing shareable and non-shareable objects

  // effect interface data
  struct 
  {
    nvsg::CgFxEffect * effect;  // to access the CgFxEffect interface
    nvsg::CgFxPass     pass;    // currently applied pass
    nvsg::CgFxProgram  program; // currently applied program
  } m_cgBinding;

  class GLLocks
  {
    public:
      GLLocks( void );

    public:
      bool  m_cullFace;
      bool  m_cullFaceMode;
      bool  m_lighting;
      bool  m_lightModelTwoSide;
      bool  m_polygonMode;
  };

  GLLocks         m_locks;

#if defined(_WIN32)
  HDC             m_hdc;        // Handle to the corresponding device context
  HGLRC           m_hglrc;      // Handle of the corresponding GL render context
#elif defined(LINUX)  
  Display         *m_dpy;       // Handle to corresponding X display
  GLXContext       m_ctx;       // Handle to corresponding X render context
  GLXDrawable      m_drawable;  // Handle to corresponding X drawable
#endif

  GLFunctions   * m_glFunctions;  // Pointer to OpenGL functions wrapper object
  GLint           m_maxVertexAttribs; // Maximal number of vertex attributes supported
  GLint           m_maxTexCoords; // Maximal number of supported client texture slots on this HW
  GLint           m_maxTexUnits;  // Maximal number of supported texture slots on this HW
  GLint           m_maxTexImageUnits;  // Maximal number of supported texture image units on this HW
  
  // Counts buffer uploads for the current context. 
  // With NV_vertex_buffer_unified_memory/NV_shader_buffer_load buffers must be made
  // resident per-context, not just per-share-group. At buffer upload, we compare this 
  // counter to the share-group's upload counter to detect buffers not yet made resident 
  // for the current context.
  GLint           m_bufferUploads;  

  // Table of texture image formats mapping given PixelFormat and PixelDataType descriptors to a
  // 'best match' GL format. This table will be initialized at GLContext construction.
  NVSGTexImageFmt m_glTexImageFmts[nvsg::Image::IMG_NUM_FORMATS][nvsg::Image::IMG_NUM_TYPES]; 

  // Table of texture wrap modes mapping from nvsg specific to a 'best match' GL specific.
  GLenum        m_glTexWrapModes[nvsg::TWM_MIRROR_CLAMP_TO_BORDER+1];


#if !defined(NDEBUG)
  bool            m_inBeginEndLoop;  // \c true, when inside a glBegin/glEnd loop, otherwise \c false
#endif
#if defined( GL_STATISTICS )
  GLStatistics  * m_glStats;
#endif

#define GEN_BUFFER_CACHE_SIZE 128
  GLuint      m_buffers[GEN_BUFFER_CACHE_SIZE];
  GLsizei     m_firstBuffer;
  GLsizei     m_bufferCount;
};

} // namespace nvgl

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// inline implementations
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

// need defines now with implementing
#include "nvgl/GLFunctions.h"
#include "nvgl/GLObjects.h"

namespace nvgl 
{

inline VBOCachingState *GLContext::getVBOState() const
{
  return m_glObjects->m_shareables->m_VBOState;
}

inline void GLContext::addBuffers(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  m_glObjects->m_shareables->m_buffers.insert(&names[0], &names[n]);
}

inline void GLContext::removeBuffers(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  nvutil::set_erase(m_glObjects->m_shareables->m_buffers, &names[0], &names[n]);
}

inline void GLContext::addLists(GLsizei n, GLuint first)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && first>0);
  for( GLuint i=first; i<first+n; ++i )
  {
    m_glObjects->m_shareables->m_lists.insert(i);
  }
}

inline void GLContext::removeLists(GLsizei n, GLuint first)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && first>0);
  for( GLuint i=first; i<first+n; ++i )
  {
    m_glObjects->m_shareables->m_lists.erase(i);
  }
}

inline void GLContext::addPrograms(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  m_glObjects->m_shareables->m_programs.insert(&names[0], &names[n]);
}

inline void GLContext::removePrograms(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  nvutil::set_erase(m_glObjects->m_shareables->m_programs, &names[0], &names[n]);
}

inline void GLContext::addTextures(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  m_glObjects->m_shareables->m_textures.insert(&names[0], &names[n]);
}

inline void GLContext::removeTextures(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  nvutil::set_erase(m_glObjects->m_shareables->m_textures, &names[0], &names[n]);
}

inline void GLContext::addQueries(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  m_glObjects->m_queries.insert(&names[0], &names[n]);
}

inline void GLContext::removeQueries(GLsizei n, const GLuint * names)
{
  NVSG_TRACE();
  NVSG_ASSERT(n>0 && names);
  nvutil::set_erase(m_glObjects->m_queries, &names[0], &names[n]);
}

/*! \brief Get the GLContext of the current thread.
 *  \return The GLContext of the current thread.
 *  \remarks When using WGLMakeCurrent (or GLXMakeCurrent on Linux), to make a HGLRC current, the
 *  corresponding GLContext is assigned. As there can be only one HGLRC current per thread, the same
 *  holds for the GLContext. Using this function returns the GLContext of the current thread.
 *  \sa WGLMakeCurrent, GLXMakeCurrent, GLContext */
inline GLContext * glContext()
{
#if defined(_WIN32)
  NVSG_ASSERT(glContextTlsIdx!=TLS_OUT_OF_INDEXES);
  return (GLContext*)TlsGetValue(glContextTlsIdx);
#elif defined(LINUX) 
  return (GLContext*)pthread_getspecific(glContextKey);
#else
#error Unsupported Operating System!
#endif
}

//////////////////////////////////////////////////////////////////////////
#if !defined(DOXYGEN_IGNORE)
# if !defined(NDEBUG)
inline void ASSERT_GL_CURRENT_CONTEXT_CONFORMITY()
{
#  if defined(_WIN32)
  HGLRC hglrc = wglGetCurrentContext();
  NVSG_ASSERT((glContext()!=NULL)==(hglrc!=NULL));
  NVSG_ASSERT(!glContext()||glContext()->m_hglrc==hglrc);
#  elif defined(LINUX)
  GLXContext ctx = glXGetCurrentContext();
  NVSG_ASSERT((glContext()!=NULL)==(ctx!=NULL));
  NVSG_ASSERT(!glContext()||glContext()->m_ctx==ctx);
#  else // unsupported OS
#  endif
}
# else
# define ASSERT_GL_CURRENT_CONTEXT_CONFORMITY() static_cast<void*>(0)
# endif

#endif //DOXYGEN_IGNORE

//! Fast float compare helper
inline bool equalf(GLfloat a, GLfloat b)
{
  return (*((int*)&a) == *((int*)&b));
}
//! Fast float compare helper
inline bool equal3f(GLfloat a1, GLfloat a2, GLfloat a3, const GLfloat *b)
{
  return (  *((int*)&a1) == *((int*)&b[0])
         && *((int*)&a2) == *((int*)&b[1])
         && *((int*)&a3) == *((int*)&b[2]));
}  
//! Fast float compare helper
inline bool equal3fv(const GLfloat *a, const GLfloat *b)
{
  return (  *((int*)&a[0]) == *((int*)&b[0])
         && *((int*)&a[1]) == *((int*)&b[1])
         && *((int*)&a[2]) == *((int*)&b[2]));
}  
//! Fast float compare helper
inline bool equal4f(GLfloat a1, GLfloat a2, GLfloat a3, GLfloat a4, const GLfloat *b)
{
  return (  *((int*)&a1) == *((int*)&b[0])
         && *((int*)&a2) == *((int*)&b[1])
         && *((int*)&a3) == *((int*)&b[2])
         && *((int*)&a4) == *((int*)&b[3]));
}  
//! Fast float compare helper
inline bool equal4fv(const GLfloat *a, const GLfloat *b)
{
  return (  *((int*)&a[0]) == *((int*)&b[0])
         && *((int*)&a[1]) == *((int*)&b[1])
         && *((int*)&a[2]) == *((int*)&b[2])
         && *((int*)&a[3]) == *((int*)&b[3]));
}  

//////////////////////////////////////////////////////////////////////////

//! Replacement of glAccum
/** This function asserts on the result before returning.  */
inline void  GLAccum( GLenum op, GLfloat value )
{
  NVSG_GL_STATS_FCT_COUNT();
  glAccum( op, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glActiveStencilFaceEXT
/** This function asserts on the result before returning.  */
inline  void  GLActiveStencilFace( GLenum face )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glActiveStencilFaceEXT( face );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glActiveTexture
/** This function stores the texture to activate for subsequent texture operations.  */
inline  void  GLActiveTexture( GLenum texture )
{
#if defined( GL_CACHING )
  glContext()->m_textureAttributes.m_toActivate = texture - GL_TEXTURE0;
#else
  NVSG_GL_STATS_ACTIVE_TEXTURE( texture );
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glActiveTexture(texture);
  ASSERT_GL_NO_ERROR;
#endif
}

//! Replacement of glPatchParameterfvNV
/** This function asserts on the result before returning. */
inline  void  GLPatchParameterfv( GLenum pname, const GLfloat * values )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPatchParameterfv( pname, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPatchParameteriNV 
/** This function asserts on the result before returning. */
inline  void  GLPatchParameteri( GLenum pname, GLint value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPatchParameteri( pname, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glActiveVaryingNV
/** This function asserts on the result before returning. */
inline  void  GLActiveVarying( GLuint program, const GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glActiveVaryingNV( program, name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glAlphaFunc
/** This function caches the alpha func and ref and asserts on the result before returning. */
inline  void  GLAlphaFunc( GLenum func, GLclampf ref )
{
#if defined(GL_CACHING)
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_alphaFunc = ctx->m_colorBufferAttributes.m_alphaFunc;
  ctx->m_previousAttributes.m_alphaRef  = ctx->m_colorBufferAttributes.m_alphaRef;
# endif
  if (    ( ctx->m_colorBufferAttributes.m_alphaFunc != func )
      ||  ( ctx->m_colorBufferAttributes.m_alphaRef  != ref ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glAlphaFunc( func, ref );
    ASSERT_GL_NO_ERROR;
#if defined(GL_CACHING)
    ctx->m_colorBufferAttributes.m_alphaFunc = func;
    ctx->m_colorBufferAttributes.m_alphaRef  = ref;
#endif
  }
}

/*! \brief Helper function to reset alpha function and alpha reference value.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this functions sets the alpha
 *  function and alpha reference value to the previous function and value. Otherwise, the alpha
 *  function is set to GL_ALWAYS, and the alpha reference value is set to zero.
 *  \sa GLAlphaFunc */
inline void GLResetAlphaFunc()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLAlphaFunc( ctx->m_previousAttributes.m_alphaFunc, ctx->m_previousAttributes.m_alphaRef );
#else
  GLAlphaFunc( GL_ALWAYS, 0.0f );
#endif
}

//! Replacement of glAreProgramsResidentNV
/** This function asserts on the result before returning.  */
inline  GLboolean GLAreProgramsResident( GLsizei n, const GLuint *programs, GLboolean *residences )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLboolean ok = glContext()->m_glFunctions->glAreProgramsResidentNV( n, programs, residences );
  ASSERT_GL_NO_ERROR;
  return( ok );
}

//! Replacement of glAreTexturesResident
/** This function asserts on the result before returning.  */
inline  GLboolean GLAreTexturesResident( GLsizei n, const GLuint *textures, GLboolean *residences )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLboolean ok = glAreTexturesResident( n, textures, residences );
  ASSERT_GL_NO_ERROR;
  return( ok );
}

//! Replacement of glArrayElement
/** This function asserts on the result before returning.  */
inline  void  GLArrayElement( GLint i )
{
  NVSG_GL_STATS_FCT_COUNT();
  glArrayElement( i );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glAttachObjectARB
/** This function asserts on the result before returning.  */
inline  void  GLAttachObject( GLhandleARB containerObj, GLhandleARB attachedObj )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glAttachObjectARB( containerObj, attachedObj );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glAttachShader
/** This function asserts on the result before returning.  */
inline  void  GLAttachShader( GLuint program, GLuint shader )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glAttachShader( program, shader );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBegin
/** This function asserts on the result before returning.  */
inline  void  GLBegin( GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  glBegin( mode );
#if !defined(NDEBUG)
  glContext()->m_inBeginEndLoop = true;
#endif
}

//! Replacement of glBeginQuery
/** This function asserts on the result before returning. */
inline  void  GLBeginQuery( GLenum target, GLuint id )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBeginQuery( target, id );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBeginTransformFeedbackNV
inline  void  GLBeginTransformFeedbackNV( GLenum primitiveMode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBeginTransformFeedbackNV( primitiveMode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBeginTransformFeedback
inline  void  GLBeginTransformFeedback( GLenum primitiveMode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBeginTransformFeedback( primitiveMode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBeginConditionalRenderNV
inline  void  GLBeginConditionalRenderNV( GLuint id, GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBeginConditionalRenderNV( id, mode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBlendColor
inline  void  GLBlendColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBlendColor( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindAttribLocation
inline  void  GLBindAttribLocation( GLuint program, GLuint index, const GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindAttribLocation( program, index, name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindBuffer
/** This function asserts on the result before returning.  */
inline  void  GLBindBuffer( GLenum target, GLuint id )
{
  GLContext * ctx = glContext();
  NVSG_GL_STATS_BIND_BUFFER( target, id );
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glBindBuffer );
  ctx->m_glFunctions->glBindBuffer( target, id );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindBufferBaseNV
/** This function asserts on the result before returning.  */
inline  void  GLBindBufferBaseNV( GLenum target, GLuint index, GLuint buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindBufferBaseNV( target, index, buffer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindBufferBase
/** This function asserts on the result before returning.  */
inline  void  GLBindBufferBase( GLenum target, GLuint index, GLuint buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindBufferBase( target, index, buffer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindBufferOffsetNV
/** This function asserts on the result before returning.  */
inline  void  GLBindBufferOffsetNV( GLenum target, GLuint index, GLuint buffer, GLintptr offset )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindBufferOffsetNV( target, index, buffer, offset );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindBufferRangeNV
/** This function asserts on the result before returning.  */
inline  void  GLBindBufferRangeNV( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindBufferRangeNV( target, index, buffer, offset, size );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindBufferRange
/** This function asserts on the result before returning.  */
inline  void  GLBindBufferRange( GLenum target, GLuint index, GLuint buffer, GLintptr offset, GLsizeiptr size )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindBufferRange( target, index, buffer, offset, size );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBufferData
/** This function asserts on the result before returning.  */
inline  void  GLBufferData( GLenum target, GLsizeiptrARB size, const GLvoid *data, GLenum usage )
{
  NVSG_GL_STATS_BUFFER_DATA( target, size, data, usage );
  GLContext * ctx = glContext();

  NVSG_ASSERT( ctx->m_glFunctions->glBufferData );
  ctx->m_glFunctions->glBufferData( target, size, data, usage );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBufferSubData
/** This function and asserts on the result before returning.  */
inline  void  GLBufferSubData( GLenum target, GLintptrARB offset, GLsizeiptrARB size, const GLvoid *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext();

  NVSG_ASSERT( ctx->m_glFunctions->glBufferSubData );
  ctx->m_glFunctions->glBufferSubData( target, offset, size, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDeleteBuffers
/** This function updates some caches, and asserts on the result before returning.  */
inline  void  GLDeleteBuffers(GLsizei n, const GLuint *buffers)
{
  NVSG_GL_STATS_DELETE_BUFFERS( n, buffers );
  GLContext * ctx = glContext();
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glDeleteBuffers );

  ctx->m_glFunctions->glDeleteBuffers( n, buffers );
  ctx->removeBuffers(n, buffers);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDeleteFencesNV
inline  void  GLDeleteFences( GLsizei n, const GLuint *fences )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDeleteFences( n, fences );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGenBuffers
/** This function asserts on the result, and updates some caches. */
inline  void  GLGenBuffers(GLsizei n, GLuint *buffers)
{
  NVSG_GL_STATS_GEN_BUFFERS( n, buffers );
  GLContext * ctx = glContext();
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glGenBuffers );
  ctx->genBuffers( n, buffers );
  ctx->addBuffers(n, buffers);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGenFencesNV
inline  void  GLGenFences( GLsizei n, GLuint *fences )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGenFences( n, fences );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMapBuffer
/** This function hides the extension character of glMapBufferARB and asserts 
  * on the result before returning.  */
inline  GLvoid*  GLMapBuffer( GLenum target, GLenum access )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext();

  NVSG_ASSERT( ctx->m_glFunctions->glMapBuffer );
  GLvoid * p = ctx->m_glFunctions->glMapBuffer( target, access );
  ASSERT_GL_NO_ERROR;
  return p;
}

//! Replacement of glUnlockArrays
/** This function asserts on the result before returning.  */
inline  void  GLUnlockArrays()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUnlockArrays();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform1f
/** This function asserts on the result before returning.  */
inline  void  GLUniform1f( GLint location, GLfloat v0 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform1f( location, v0 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform1fv
/** This function asserts on the result before returning.  */
inline  void  GLUniform1fv( GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform1fv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform1i
/** This function asserts on the result before returning.  */
inline  void  GLUniform1i( GLint location, GLint v0 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform1i( location, v0 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform1iv
/** This function asserts on the result before returning.  */
inline  void  GLUniform1iv( GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform1iv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform1uiEXT
/** This function asserts on the result before returning.  */
inline  void  GLUniform1ui( GLint location, GLuint v0 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform1ui( location, v0 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform1uivEXT
/** This function asserts on the result before returning.  */
inline  void  GLUniform1uiv( GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform1uiv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform2f
/** This function asserts on the result before returning.  */
inline  void  GLUniform2f( GLint location, GLfloat v0, GLfloat v1 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform2f( location, v0, v1 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform2fv
/** This function asserts on the result before returning.  */
inline  void  GLUniform2fv( GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform2fv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform2i
/** This function asserts on the result before returning.  */
inline  void  GLUniform2i( GLint location, GLint v0, GLint v1 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform2i( location, v0, v1 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform2iv
/** This function asserts on the result before returning.  */
inline  void  GLUniform2iv( GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform2iv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform2ui
/** This function asserts on the result before returning.  */
inline  void  GLUniform2ui( GLint location, GLuint v0, GLuint v1 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform2ui( location, v0, v1 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform2uiv
/** This function asserts on the result before returning.  */
inline  void  GLUniform2uiv( GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform2uiv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform3f
/** This function asserts on the result before returning.  */
inline  void  GLUniform3f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform3f( location, v0, v1, v2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform3fv
/** This function asserts on the result before returning.  */
inline  void  GLUniform3fv( GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform3fv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform3i
/** This function asserts on the result before returning.  */
inline  void  GLUniform3i( GLint location, GLint v0, GLint v1, GLint v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform3i( location, v0, v1, v2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform3iv
/** This function asserts on the result before returning.  */
inline  void  GLUniform3iv( GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform3iv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform3ui
/** This function asserts on the result before returning.  */
inline  void  GLUniform3ui( GLint location, GLuint v0, GLuint v1, GLuint v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform3ui( location, v0, v1, v2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform3uiv
/** This function asserts on the result before returning.  */
inline  void  GLUniform3uiv( GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform3uiv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform4f
/** This function asserts on the result before returning.  */
inline  void  GLUniform4f( GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform4f( location, v0, v1, v2, v3 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform4fv
/** This function asserts on the result before returning.  */
inline  void  GLUniform4fv( GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform4fv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform4i
/** This function asserts on the result before returning.  */
inline  void  GLUniform4i( GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform4i( location, v0, v1, v2, v3 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform4iv
/** This function asserts on the result before returning.  */
inline  void  GLUniform4iv( GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform4iv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform4ui
/** This function asserts on the result before returning.  */
inline  void  GLUniform4ui( GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform4ui( location, v0, v1, v2, v3 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniform4uiv
/** This function asserts on the result before returning.  */
inline  void  GLUniform4uiv( GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniform4uiv( location, count, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformBufferEXT
/** This function asserts on the result before returning.  */
inline  void  GLUniformBuffer( GLuint program, GLint location, GLuint buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformBuffer( program, location, buffer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix2fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix2fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix2x3fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix2x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix2x3fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix2x4fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix2x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix2x4fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix3fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix3fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix3x2fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix3x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix3x2fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix3x4fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix3x4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix3x4fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix4fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix4fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix4fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix4x2fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix4x2fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix4x2fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUniformMatrix4x3fv
/** This function asserts on the result before returning.  */
inline  void  GLUniformMatrix4x3fv( GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUniformMatrix4x3fv( location, count, transpose, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUnmapBuffer
/** This function asserts on the result before returning.  */
inline  GLboolean  GLUnmapBuffer( GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLboolean b = glContext()->m_glFunctions->glUnmapBuffer( target );
  ASSERT_GL_NO_ERROR;
  return b;
}

//! Replacement of glUseProgram
/** This function asserts on the result before returning.  */
inline  void  GLUseProgram( GLuint program )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUseProgram( program );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glUseProgramObjectARB
/** This function asserts on the result before returning.  */
inline  void  GLUseProgramObject( GLhandleARB programObj )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glUseProgramObjectARB( programObj );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glValidateProgram
/** This function asserts on the result before returning.  */
inline  void  GLValidateProgram( GLuint program )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glValidateProgram( program );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glIsBuffer
/** This function tests if for the current render context, name denotes a valid vertex buffer object. */ 
inline  GLboolean  GLIsBuffer( GLuint name )
{
  NVSG_GL_STATS_FCT_COUNT();
  return glContext()->isBuffer(name);
}

//! Replacement of glBindFragDataLocationEXT
/** This function asserts on the result before returning.  */
inline  void  GLBindFragDataLocation( GLuint program, GLuint color, const GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindFragDataLocation( program, color, name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBindProgramARB
/** This function hides the extension character of glBindProgramARB and asserts 
  * on the result before returning.  */
inline  void  GLBindProgram( GLenum target, GLuint id )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBindProgramARB( target, id );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBitmap
/** This function asserts on the result before returning.  */
inline  void  GLBitmap( GLsizei width, GLsizei height, GLfloat xorig, GLfloat yorig, GLfloat xmove, GLfloat ymove, const GLubyte *bitmap )
{
  NVSG_GL_STATS_FCT_COUNT();
  glBitmap( width, height, xorig, yorig, xmove, ymove, bitmap );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBlendEquation
/** This function asserts on the result before returning.  */
inline  void  GLBlendEquation( GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBlendEquation( mode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBlendEquationSeparate
/** This function asserts on the result before returning.  */
inline  void  GLBlendEquationSeparate( GLenum modeRGB, GLenum modeAlpha )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBlendEquationSeparate( modeRGB, modeAlpha );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glBlendFunc
/** This function caches the blend func settings and asserts on the result before returning.  */
inline  void  GLBlendFunc( GLenum sfactor, GLenum dfactor )
{
#if defined(GL_CACHING)
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_blendSrcRGB   = ctx->m_colorBufferAttributes.m_blendSrcRGB;
  ctx->m_previousAttributes.m_blendSrcAlpha = ctx->m_colorBufferAttributes.m_blendSrcAlpha;
  ctx->m_previousAttributes.m_blendDstRGB   = ctx->m_colorBufferAttributes.m_blendDstRGB;
  ctx->m_previousAttributes.m_blendDstAlpha = ctx->m_colorBufferAttributes.m_blendDstAlpha;
# endif
  if (    ( ctx->m_colorBufferAttributes.m_blendSrcRGB   != sfactor )
      ||  ( ctx->m_colorBufferAttributes.m_blendSrcAlpha != sfactor )
      ||  ( ctx->m_colorBufferAttributes.m_blendDstRGB   != dfactor )
      ||  ( ctx->m_colorBufferAttributes.m_blendDstAlpha != dfactor ) )
#endif
  {
    NVSG_GL_STATS_BLEND_FUNC( sfactor, dfactor );
    glBlendFunc( sfactor, dfactor );
    ASSERT_GL_NO_ERROR;
#if defined(GL_CACHING)
    ctx->m_colorBufferAttributes.m_blendSrcRGB   = sfactor;
    ctx->m_colorBufferAttributes.m_blendSrcAlpha = sfactor;
    ctx->m_colorBufferAttributes.m_blendDstRGB   = dfactor;
    ctx->m_colorBufferAttributes.m_blendDstAlpha = dfactor;
#endif
  }
}

/*! \brief Helper function to reset the blend function.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this functions sets the blend
 *  function to the previous function. Otherwise, the blend function is set to GL_ONE for the source
 *  factor and GL_ZERO for the destination factor.
 *  \sa GLBlendFunc, GLBlendFuncSeparate */
inline void GLResetBlendFunc()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  if (    ( ctx->m_previousAttributes.m_blendSrcAlpha != ctx->m_previousAttributes.m_blendSrcRGB )
      ||  ( ctx->m_previousAttributes.m_blendDstAlpha != ctx->m_previousAttributes.m_blendDstRGB ) )
  {
    GLBlendFuncSeparate( ctx->m_previousAttributes.m_blendSrcRGB
                       , ctx->m_previousAttributes.m_blendDstRGB
                       , ctx->m_previousAttributes.m_blendSrcAlpha
                       , ctx->m_previousAttributes.m_blendDstAlpha );
  }
  else
  {
    GLBlendFunc( ctx->m_previousAttributes.m_blendSrcRGB, ctx->m_previousAttributes.m_blendDstRGB );
  }
#else
  GLBlendFunc( GL_ONE, GL_ZERO );
#endif
}

//! Replacement of glBlendFunc
/** This function caches the blend func settings and asserts on the result before returning.  */
inline  void  GLBlendFuncSeparate( GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha )
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_blend_func_separate"));
#if defined(GL_CACHING)
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_blendSrcRGB   = ctx->m_colorBufferAttributes.m_blendSrcRGB;
  ctx->m_previousAttributes.m_blendSrcAlpha = ctx->m_colorBufferAttributes.m_blendSrcAlpha;
  ctx->m_previousAttributes.m_blendDstRGB   = ctx->m_colorBufferAttributes.m_blendDstRGB;
  ctx->m_previousAttributes.m_blendDstAlpha = ctx->m_colorBufferAttributes.m_blendDstAlpha;
# endif
  if (    ( ctx->m_colorBufferAttributes.m_blendSrcRGB   != sfactorRGB )
      ||  ( ctx->m_colorBufferAttributes.m_blendSrcAlpha != sfactorAlpha )
      ||  ( ctx->m_colorBufferAttributes.m_blendDstRGB   != dfactorRGB )
      ||  ( ctx->m_colorBufferAttributes.m_blendDstAlpha != dfactorAlpha ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    NVSG_ASSERT( glContext()->m_glFunctions );
    glContext()->m_glFunctions->glBlendFuncSeparate(sfactorRGB, dfactorRGB, sfactorAlpha, dfactorAlpha);
    ASSERT_GL_NO_ERROR;
#if defined(GL_CACHING)
    ctx->m_colorBufferAttributes.m_blendSrcRGB   = sfactorRGB;
    ctx->m_colorBufferAttributes.m_blendSrcAlpha = sfactorAlpha;
    ctx->m_colorBufferAttributes.m_blendDstRGB   = dfactorRGB;
    ctx->m_colorBufferAttributes.m_blendDstAlpha = dfactorAlpha;
#endif
  }
}

/*! \brief Helper function to reset the blend function, using the GL_EXT_blend_func_separate extention.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this functions sets the four
 *  factors of the extended blend function the previous values. Otherwise, the blend function
 *  factors are set to GL_ONE, GL_ZERO, GL_ONE, and GL_ZERO, in this order.
 *  \sa GLBlendFuncSeparate */
inline void GLResetBlendFuncSeparate()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLBlendFuncSeparate( ctx->m_previousAttributes.m_blendSrcRGB
                     , ctx->m_previousAttributes.m_blendDstRGB
                     , ctx->m_previousAttributes.m_blendSrcAlpha
                     , ctx->m_previousAttributes.m_blendDstAlpha );
#else
  GLBlendFuncSeparate( GL_ONE, GL_ZERO, GL_ONE, GL_ZERO );
#endif
}

/*! \brief Replacement of glBlitFramebufferEXT.
 *  \param srcX0 source lower left X pos
 *  \param srcY0 source lower left Y pos
 *  \param srcX1 source upper right X pos
 *  \param srcY1 source upper right Y pos
 *  \param dstX0 destination lower left X pos
 *  \param dstY0 destination lower left Y pos
 *  \param dstX1 destination upper right X pos
 *  \param dstY1 destination uppoer right Y pos
 *  \param mask mask containing bits for COLOR, DEPTH, or STENCIL to blit. 
 *  \param filter filter to use if src and dst are not the same size.  Must be
 *         GL_NEAREST or GL_LINEAR.  Must be GL_NEAREST if mask contains
 *         anything but the color buffer.
 *  \note It is assumed that the extension GL_EXT_framebuffer_blit is supported.
 *  \note This function asserts on no GL error. */
inline void GLBlitFramebuffer( GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBlitFramebuffer( srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter );
  ASSERT_GL_NO_ERROR;
}


//! Replacement of glBindTexture
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLBindTexture( GLenum target, GLuint texture )
{
#if defined( GL_CACHING )
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_BIND_TEXTURE( target, texture );
  glBindTexture( target, texture );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCallList
/** This function asserts on the result before returning.  */
inline  void  GLCallList( GLuint list )
{
  NVSG_GL_STATS_CALL_LIST( list );
  glCallList( list );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCallLists
/** This function asserts on the result before returning.  */
inline  void  GLCallLists( GLsizei n, GLenum type, const GLvoid *lists )
{
  NVSG_GL_STATS_FCT_COUNT();
  glCallLists( n, type, lists );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glClampColorARB
/** This function asserts on the result before returning.  */
inline  void  GLClampColor( GLenum target, GLenum clamp )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glClampColor( target, clamp );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glClear
/** This function asserts on the result before returning.  */
inline  void  GLClear( GLbitfield mask )
{
  NVSG_GL_STATS_CLEAR( mask );
  glClear( mask );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glClearAccum
/** This function asserts on the result before returning.  */
inline  void  GLClearAccum( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
  NVSG_GL_STATS_FCT_COUNT();
  glClearAccum( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glClearColor
/** This function caches the clear color settings and asserts on the result before returning.  */
inline  void  GLClearColor( GLclampf red, GLclampf green, GLclampf blue, GLclampf alpha )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_clearColor = ctx->m_colorBufferAttributes.m_clearColor;
# endif
  if ( ! equal4f( red, green, blue, alpha, &ctx->m_colorBufferAttributes.m_clearColor[0] ) )
#endif
  {
    NVSG_GL_STATS_CLEAR_COLOR( red, green, blue, alpha );
    glClearColor(red, green, blue, alpha);
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_colorBufferAttributes.m_clearColor[0] = red;
    ctx->m_colorBufferAttributes.m_clearColor[1] = green;
    ctx->m_colorBufferAttributes.m_clearColor[2] = blue;
    ctx->m_colorBufferAttributes.m_clearColor[3] = alpha;
#endif
  }
}

//! Replacement of glClearColorIiEXT.
/** This function asserts on the result before returning. */
inline  void  GLClearColorIi( GLint red, GLint green, GLint blue, GLint alpha )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glClearColorIi( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glClearColorIuiEXT.
/** This function asserts on the result before returning. */
inline  void  GLClearColorIui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glClearColorIui( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Helper function to reset the clear color.
 *  \remarks if GL_SUPPORT_RESET is defined (per default, it is), this function sets the four values
 *  of the clear color to the previous value. Otherwise, they are all set to 0.0f.
 *  \sa GLClearColor */
inline void GLResetClearColor()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLClearColor( ctx->m_previousAttributes.m_clearColor[0]
              , ctx->m_previousAttributes.m_clearColor[1]
              , ctx->m_previousAttributes.m_clearColor[2]
              , ctx->m_previousAttributes.m_clearColor[3] );
#else
  GLClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
#endif
}

//! Replacement of glClearDepth
/** This function caches the depth clear setting and asserts on the result before returning. */
inline  void  GLClearDepth( GLclampd depth )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_depthClearValue = ctx->m_depthBufferAttributes.m_depthClearValue;
# endif
  if ( ctx->m_depthBufferAttributes.m_depthClearValue != depth )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glClearDepth( depth );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_depthBufferAttributes.m_depthClearValue = depth;
#endif
  }
}

//! Replacement of glClearDepthd
/** This functions asserts on the result before returning. */
inline  void  GLClearDepthd( GLdouble depth )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glClearDepthd( depth );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Helper function to reset the clear depth.
 *  \remarks if ( GL_SUPPORT_RESET is defined (per default, it is), this function sets the clear
 *  depth to the previous value. Otherwise, it is set to 1.0f.
 *  \sa GLClearDepth */
inline void GLResetClearDepth()
{
#if defined(GL_SUPPORT_RESET)
  GLClearDepth( glContext()->m_previousAttributes.m_depthClearValue );
#else
  GLClearDepth( 1.0 );
#endif
}

//! Replacement of glClearIndex
/** This function asserts on the result before returning.  */
inline  void  GLClearIndex( GLfloat c )
{
  NVSG_GL_STATS_FCT_COUNT();
  glClearIndex( c );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glClearStencil
/** This function caches the stencil clear setting and asserts on the result before returning.  */
inline  void  GLClearStencil( GLint s )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_stencilClearValue = ctx->m_stencilBufferAttributes.m_stencilClearValue;
# endif
  if ( ctx->m_stencilBufferAttributes.m_stencilClearValue != s )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glClearStencil( s );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_stencilBufferAttributes.m_stencilClearValue = s;
#endif
  }
}

/*! \brief Helper function to reset the stencil clear value.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the stencil
 *  clear value to the previous value. Otherwise, it is set to zero.
 *  \sa GLClearStencil */
inline void GLResetClearStencil()
{
#if defined(GL_SUPPORT_RESET)
  GLClearStencil( glContext()->m_previousAttributes.m_stencilClearValue );
#else
  GLClearStencil( 0 );
#endif
}

//! Replacement of glClientActiveTexture
/** This functions caches the next active texture unit and asserts on the result before returning.*/
inline  void  GLClientActiveTexture( GLenum texture )
{
#if defined( GL_CACHING )
  glContext()->m_vertexArrayAttributes.m_clientTextureToActivate = texture - GL_TEXTURE0;
#else
  NVSG_GL_STATS_CLIENT_ACTIVE_TEXTURE( texture );
  glContext()->m_glFunctions->glClientActiveTexture( texture );
  ASSERT_GL_NO_ERROR;
#endif
}

//! Replacement of glClipPlane
/** This function asserts on the result before returning.*/
inline  void  GLClipPlane( GLenum plane, const GLdouble *equation )
{
  //  NOTE: clip planes can't be cached !
  NVSG_GL_STATS_FCT_COUNT();
#if defined( GL_SUPPORT_RESET )
  GLContext * ctx = glContext();
  size_t idx = plane - GL_CLIP_PLANE0;
  NVSG_ASSERT(    ( idx < ctx->m_previousAttributes.m_clipPlaneEquations.size() )
              &&  ( idx < ctx->m_transformAttributes.m_clipPlaneEquations.size() ) );
  ctx->m_previousAttributes.m_clipPlaneEquations[idx] = ctx->m_transformAttributes.m_clipPlaneEquations[idx];
  memcpy( &ctx->m_transformAttributes.m_clipPlaneEquations[idx][0], equation, 4*sizeof(GLdouble) );
#endif
  glClipPlane(plane, equation);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Helper function to reset a clip plane equation.
 *  \param plane The id of the clip plane for which the plane equation is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the clip plane
 *  equation of \a plane to the previous equations. Otherwise, it is set to the default plane
 *  (0,0,0,0).
 *  \sa GLClipPlane */
inline void GLResetClipPlaneEquation( GLenum plane )
{
#if defined( GL_SUPPORT_RESET )
  GLClipPlane( plane, &glContext()->m_previousAttributes.m_clipPlaneEquations[plane-GL_CLIP_PLANE0][0] );
#else
  static double zeroPlane[4] = {0.0, 0.0, 0.0, 0.0}; 
  GLClipPlane( plane, zeroPlane );
#endif
}

//! Replacement of glColor3b
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3b( GLbyte red, GLbyte green, GLbyte blue )
{
  NVSG_GL_STATS_COLOR3B( red, green, blue );
  glColor3b( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3bv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3bv( const GLbyte *v )
{
  NVSG_GL_STATS_COLOR3BV( v );
  glColor3bv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3d
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3d( GLdouble red, GLdouble green, GLdouble blue )
{
  NVSG_GL_STATS_COLOR3D( red, green, blue );
  glColor3d( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3dv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3dv( const GLdouble *v )
{
  NVSG_GL_STATS_COLOR3DV( v );
  glColor3dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3f
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
  NVSG_GL_STATS_COLOR3F( red, green, blue );
  glColor3f( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3fv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3fv( const GLfloat * v )
{
  NVSG_GL_STATS_COLOR3FV( v );
  glColor3fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3hNV
/** This function asserts on the result before returning.  */
inline  void  GLColor3h( GLhalf red, GLhalf green, GLhalf blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColor3h( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3hv
/** This function asserts on the result before returning.  */
inline  void  GLColor3hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColor3hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3i
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3i( GLint red, GLint green, GLint blue )
{
  NVSG_GL_STATS_COLOR3I( red, green, blue );
  glColor3i( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3iv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3iv( const GLint * v )
{
  NVSG_GL_STATS_COLOR3IV( v );
  glColor3iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3s
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3s( GLshort red, GLshort green, GLshort blue )
{
  NVSG_GL_STATS_COLOR3S( red, green, blue );
  glColor3s( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3sv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3sv( const GLshort * v )
{
  NVSG_GL_STATS_COLOR3SV( v );
  glColor3sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3ub
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
  NVSG_GL_STATS_COLOR3UB( red, green, blue );
  glColor3ub( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3ubv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3ubv( const GLubyte * v )
{
  NVSG_GL_STATS_COLOR3UBV( v );
  glColor3ubv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3ui
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3ui( GLuint red, GLuint green, GLuint blue )
{
  NVSG_GL_STATS_COLOR3UI( red, green, blue );
  glColor3ui( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3uiv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3uiv( const GLuint * v )
{
  NVSG_GL_STATS_COLOR3UIV( v );
  glColor3uiv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3us
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3us( GLushort red, GLushort green, GLushort blue )
{
  NVSG_GL_STATS_COLOR3US( red, green, blue );
  glColor3us( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor3usv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor3usv( const GLushort * v )
{
  NVSG_GL_STATS_COLOR3USV( v );
  glColor3usv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4b
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4b( GLbyte red, GLbyte green, GLbyte blue, GLbyte alpha )
{
  NVSG_GL_STATS_COLOR4B( red, green, blue, alpha );
  glColor4b( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4bv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4bv( const GLbyte * v )
{
  NVSG_GL_STATS_COLOR4BV( v );
  glColor4bv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4d
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4d( GLdouble red, GLdouble green, GLdouble blue, GLdouble alpha )
{
  NVSG_GL_STATS_COLOR4D( red, green, blue, alpha );
  glColor4d( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4dv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4dv( const GLdouble * v )
{
  NVSG_GL_STATS_COLOR4DV( v );
  glColor4dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4f
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4f( GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha )
{
  NVSG_GL_STATS_COLOR4F( red, green, blue, alpha );
  glColor4f( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4fv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4fv( const GLfloat * v )
{
  NVSG_GL_STATS_COLOR4FV( v );
  glColor4fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4hNV
/** This function asserts on the result before returning.  */
inline  void  GLColor4h( GLhalf red, GLhalf green, GLhalf blue, GLhalf alpha )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColor4h( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4hv
/** This function asserts on the result before returning.  */
inline  void  GLColor4hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColor4hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4i
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4i( GLint red, GLint green, GLint blue, GLint alpha )
{
  NVSG_GL_STATS_COLOR4I( red, green, blue, alpha );
  glColor4i( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4iv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4iv( const GLint * v )
{
  NVSG_GL_STATS_COLOR4IV( v );
  glColor4iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4s
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4s( GLshort red, GLshort green, GLshort blue, GLshort alpha )
{
  NVSG_GL_STATS_COLOR4S( red, green, blue, alpha );
  glColor4s( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4sv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4sv( const GLshort * v )
{
  NVSG_GL_STATS_COLOR4SV( v );
  glColor4sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4ub
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4ub( GLubyte red, GLubyte green, GLubyte blue, GLubyte alpha )
{
  NVSG_GL_STATS_COLOR4UB( red, green, blue, alpha );
  glColor4ub( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4ubv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4ubv( const GLubyte * v )
{
  NVSG_GL_STATS_COLOR4UBV( v );
  glColor4ubv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4ui
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4ui( GLuint red, GLuint green, GLuint blue, GLuint alpha )
{
  NVSG_GL_STATS_COLOR4UI( red, green, blue, alpha );
  glColor4ui( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4uiv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4uiv( const GLuint * v )
{
  NVSG_GL_STATS_COLOR4UIV( v );
  glColor4uiv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4us
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4us( GLushort red, GLushort green, GLushort blue, GLushort alpha )
{
  NVSG_GL_STATS_COLOR4US( red, green, blue, alpha );
  glColor4us( red, green, blue, alpha );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColor4usv
/** This function caches the color values and asserts on the result before returning.  */
inline  void  GLColor4usv( const GLushort * v )
{
  NVSG_GL_STATS_COLOR4USV( v );
  glColor4usv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glColorMask
/** This function asserts on the result before returning.  */
inline  void  GLColorMask( GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  memcpy( &ctx->m_previousAttributes.m_colorWriteMask[0]
        , &ctx->m_colorBufferAttributes.m_colorWriteMask[0], 4*sizeof(GLboolean) );
# endif
  if (    ( ctx->m_colorBufferAttributes.m_colorWriteMask[0] != red )
      ||  ( ctx->m_colorBufferAttributes.m_colorWriteMask[1] != green )
      ||  ( ctx->m_colorBufferAttributes.m_colorWriteMask[2] != blue )
      ||  ( ctx->m_colorBufferAttributes.m_colorWriteMask[3] != alpha ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glColorMask( red, green, blue, alpha );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_colorBufferAttributes.m_colorWriteMask[0] = red;
    ctx->m_colorBufferAttributes.m_colorWriteMask[1] = green;
    ctx->m_colorBufferAttributes.m_colorWriteMask[2] = blue;
    ctx->m_colorBufferAttributes.m_colorWriteMask[3] = alpha;
#endif
  }
}

//! Replacement of glColorMaskIndexedEXT
/** This functions asserts on the result before returning. */
inline  void  GLColorMaskIndexed( GLuint index, GLboolean r, GLboolean g, GLboolean b, GLboolean a )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColorMaskIndexed( index, r, b, g, a );
  ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
  // HACK force caching to some garbage and hope noone ever passes 0xF0 to enable stuff.
  GLContext * ctx = glContext();
  ctx->m_colorBufferAttributes.m_colorWriteMask[0] = 0xF0;
  ctx->m_colorBufferAttributes.m_colorWriteMask[1] = 0xF0;
  ctx->m_colorBufferAttributes.m_colorWriteMask[2] = 0xF0;
  ctx->m_colorBufferAttributes.m_colorWriteMask[3] = 0xF0;
#endif
}

/*! \brief Helper function to reset the color mask.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the color mask
 *  to the previous mask. Otherwise, all four mask components are set to GL_TRUE.
 *  \sa GLColorMask */
inline void GLResetColorMask()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLColorMask( ctx->m_previousAttributes.m_colorWriteMask[0]
             , ctx->m_previousAttributes.m_colorWriteMask[1]
             , ctx->m_previousAttributes.m_colorWriteMask[2]
             , ctx->m_previousAttributes.m_colorWriteMask[3] );
#else
  GLColorMask( GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );
#endif
}

//! Replacement of glColorMaterial
/** This function asserts on the result before returning.  */
inline  void  GLColorMaterial( GLenum face, GLenum mode )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_colorMaterialFace = ctx->m_lightingAttributes.m_colorMaterialFace;
  ctx->m_previousAttributes.m_colorMaterialMode = ctx->m_lightingAttributes.m_colorMaterialMode;
# endif
  if (    ( ctx->m_lightingAttributes.m_colorMaterialFace != face )
      ||  ( ctx->m_lightingAttributes.m_colorMaterialMode != mode ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glColorMaterial( face, mode );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_lightingAttributes.m_colorMaterialFace = face;
    ctx->m_lightingAttributes.m_colorMaterialMode = mode;
#endif
  }
}

/*! \brief Helper function to reset the color material mode.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the color
 *  material mode to the previous mode. Otherwise, it is set to GL_AMBIENT_AND_DIFFUSE for
 *  GL_FRONT_AND_BACK faces.
 *  \sa GLColorMaterial */
inline void GLResetColorMaterialMode()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLColorMaterial( ctx->m_previousAttributes.m_colorMaterialFace
                 , ctx->m_previousAttributes.m_colorMaterialMode );
#else
  GLColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
#endif
}

//! Replacement of glColorPointer
/** This function caches the color pointer settings and asserts on the result before returning.  */
inline  void  GLColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  glColorPointer( size, type, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glColorSubTable
/** This function asserts on the result before returning. */
inline  void  GLColorSubTable( GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColorSubTable( target, start, count, format, type, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glColorTable
/** This function asserts on the result before returning. */
inline  void  GLColorTable( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *table )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColorTable( target, internalformat, width, format, type, table );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glColorTableParameterfv
/** This function asserts on the result before returning. */
inline  void  GLColorTableParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColorTableParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glColorTableParameteriv
/** This function asserts on the result before returning. */
inline  void  GLColorTableParameteriv( GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glColorTableParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCombinerInput
/** This function asserts on the result before returning. */
inline  void  GLCombinerInput( GLenum stage, GLenum portion, GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCombinerInput( stage, portion, variable, input, mapping, componentUsage );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCombinerOutput
/** This function asserts on the result before returning. */
inline  void  GLCombinerOutput( GLenum stage, GLenum portion, GLenum abOutput, GLenum cdOutput, GLenum sumOutput, GLenum scale, GLenum bias, GLboolean abDotProduct, GLboolean cdDotProduct, GLboolean muxSum )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCombinerOutput( stage, portion, abOutput, cdOutput, sumOutput, scale, bias, abDotProduct, cdDotProduct, muxSum );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCombinerParameterf
/** This function asserts on the result before returning. */
inline  void  GLCombinerParameterf( GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCombinerParameterf( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCombinerParameterfv
/** This function asserts on the result before returning. */
inline  void  GLCombinerParameterfv( GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCombinerParameterfv( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCombinerParameteri
/** This function asserts on the result before returning. */
inline  void  GLCombinerParameteri( GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCombinerParameteri( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCombinerParameteriv
/** This function asserts on the result before returning. */
inline  void  GLCombinerParameteriv( GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCombinerParameteriv( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCombinerStageParameterfv
/** This function asserts on the result before returning. */
inline  void  GLCombinerStageParameterfv( GLenum stage, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCombinerStageParameterfv( stage, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCompileShader
/** This function asserts on the result before returning. */
inline  void  GLCompileShader( GLuint shader )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCompileShader( shader );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCompressedTexImage1D
/** This function hides the extension character of glCompressedTexImage1D, updates the active texture,
  * and asserts on the result before returning.  */
inline  void  GLCompressedTexImage1D( GLenum target, int level, GLenum internalformat, GLsizei width, int border, GLsizei imageSize, const void *data )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCompressedTexImage1D( target, level, internalformat, width, border, imageSize, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCompressedTexImage2D
/** This function hides the extension character of glCompressedTexImage2D, updates the active texture,
  * and asserts on the result before returning.  */
inline  void  GLCompressedTexImage2D( GLenum target, int level, GLenum internalformat, GLsizei width, GLsizei height, int border, GLsizei imageSize, const void *data )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCompressedTexImage2D( target, level, internalformat, width, height, border, imageSize, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCompressedTexImage3D
/** This function hides the extension character of glCompressedTexImage3D, updates the active texture,
  * and asserts on the result before returning.  */
inline  void  GLCompressedTexImage3D( GLenum target, int level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, int border, GLsizei imageSize, const void *data )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCompressedTexImage3D( target, level, internalformat, width, height, depth, border, imageSize, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCompressedTexSubImage1D
/** This updates the active texture and asserts on the result before returning.  */
inline  void  GLCompressedTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *data )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCompressedTexSubImage1D( target, level, xoffset, width, format, imageSize, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCompressedTexSubImage2D
/** This updates the active texture and asserts on the result before returning.  */
inline  void  GLCompressedTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *data )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCompressedTexSubImage2D( target, level, xoffset, yoffset, width, height, format, imageSize, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCompressedTexSubImage3D
/** This updates the active texture and asserts on the result before returning.  */
inline  void  GLCompressedTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *data )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCompressedTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glConvolutionFilter1D
/** This function asserts on the result before returning.  */
inline  void  GLConvolutionFilter1D( GLenum target, GLenum internalformat, GLsizei width, GLenum format, GLenum type, const GLvoid *image )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glConvolutionFilter1D( target, internalformat, width, format, type, image );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glConvolutionFilter2D
/** This function asserts on the result before returning.  */
inline  void  GLConvolutionFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *image )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glConvolutionFilter2D( target, internalformat, width, height, format, type, image );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glConvolutionParameterf
/** This function asserts on the result before returning.  */
inline  void  GLConvolutionParameterf( GLenum target, GLenum pname, GLfloat params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glConvolutionParameterf( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glConvolutionParameterfv
/** This function asserts on the result before returning.  */
inline  void  GLConvolutionParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glConvolutionParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glConvolutionParameteri
/** This function asserts on the result before returning.  */
inline  void  GLConvolutionParameteri( GLenum target, GLenum pname, GLint params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glConvolutionParameteri( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glConvolutionParameteriv
/** This function asserts on the result before returning.  */
inline  void  GLConvolutionParameteriv( GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glConvolutionParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyColorSubTable
/** This function asserts on the result before returning.  */
inline  void  GLCopyColorSubTable( GLenum target, GLsizei start, GLint x, GLint y, GLsizei width )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCopyColorSubTable( target, start, x, y, width );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyColorTable
/** This function asserts on the result before returning.  */
inline  void  GLCopyColorTable( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCopyColorTable( target, internalformat, x, y, width );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyConvolutionFilter1D
/** This function asserts on the result before returning.  */
inline  void  GLCopyConvolutionFilter1D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCopyConvolutionFilter1D( target, internalformat, x, y, width );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyConvolutionFilter2D
/** This function asserts on the result before returning.  */
inline  void  GLCopyConvolutionFilter2D( GLenum target, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCopyConvolutionFilter2D( target, internalformat, x, y, width, height );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyPixels
/** This function asserts on the result before returning.  */
inline  void  GLCopyPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum type )
{
  NVSG_GL_STATS_FCT_COUNT();
  glCopyPixels( x, y, width, height, type );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyTexImage1D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLCopyTexImage1D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glCopyTexImage1D( target, level, internalformat, x, y, width, border );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyTexImage2D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLCopyTexImage2D( GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glCopyTexImage2D( target, level, internalformat, x, y, width, height, border );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyTexSubImage1D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLCopyTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glCopyTexSubImage1D( target,level, xoffset, x, y, width );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyTexSubImage2D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLCopyTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glCopyTexSubImage2D( target,level, xoffset, yoffset, x, y, width, height );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCopyTexSubImage2D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLCopyTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glCopyTexSubImage3D( target,level, xoffset, yoffset, zoffset, x, y, width, height );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glCreateProgram
/** This function asserts on the result before returning.  */
inline  GLuint  GLCreateProgram()
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext(); 
  NVSG_ASSERT( ctx->m_glFunctions );
  GLuint prg = ctx->m_glFunctions->glCreateProgram();
  ctx->addPrograms(1, &prg); // add to internal cache
  ASSERT_GL_NO_ERROR;
  return( prg );
}

//! Replacement of glCreateProgramObjectARB
/** This function asserts on the result before returning.  */
inline  GLhandleARB GLCreateProgramObject()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLhandleARB hdl = glContext()->m_glFunctions->glCreateProgramObjectARB();
  ASSERT_GL_NO_ERROR;
  return( hdl );
}

//! Replacement of glCreateShader
/** This function asserts on the result before returning.  */
inline  GLuint  GLCreateShader( GLenum type )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLuint sh = glContext()->m_glFunctions->glCreateShader( type );
  ASSERT_GL_NO_ERROR;
  return( sh );
}

//! Replacement of glCreateShaderObjectARB
/** This function asserts on the result before returning.  */
inline  GLhandleARB GLCreateShaderObject( GLenum shaderType )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLhandleARB hdl = glContext()->m_glFunctions->glCreateShaderObjectARB( shaderType );
  ASSERT_GL_NO_ERROR;
  return( hdl );
}

//! Replacement of glCullFace
/** This function caches the cull face setting and asserts on the result before returning.  */
inline  void  GLCullFace( GLenum mode )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_cullFaceMode = ctx->m_polygonAttributes.m_cullFaceMode;
# endif
  if ( ctx->m_polygonAttributes.m_cullFaceMode != mode )
#endif
  {
    if ( ! ctx->m_locks.m_cullFaceMode )
    {
      NVSG_GL_STATS_FCT_COUNT();
      glCullFace( mode );
      ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
      ctx->m_polygonAttributes.m_cullFaceMode = mode;
#endif
    }
  }
}

/*! \brief Helper function to reset the cull face mode.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the cull face
 *  mode to the previous mode. Otherwise, it is set to GL_BACK.
 *  \sa GLCullFace */
inline void GLResetCullFaceMode()
{
#if defined(GL_SUPPORT_RESET)
  GLCullFace( glContext()->m_previousAttributes.m_cullFaceMode );
#else
  GLCullFace( GL_BACK );
#endif
}

/*! \brief Helper function to set the cull face mode and lock it.
 *  \param mode The cull face mode to set and lock.
 *  \remarks This function sets the cull face mode and locks this state. Any subsequent calls to
 *  GLCullFace will not change that value, until the next call to GLCullFaceUnlock.
 *  \sa GLCullFace, GLCullFaceUnlock */
inline void GLCullFaceLock( GLenum mode )
{
  NVSG_ASSERT( ! glContext()->m_locks.m_cullFaceMode );
  GLCullFace( mode );
  glContext()->m_locks.m_cullFaceMode = true;
}

/*! \brief Helper function to unlock the cull face and set it.
 *  \param mode The cull face mode to set, after unlocking.
 *  \remarks This function unlocks the cull face mode and sets it to \a mode. After a call to
 *  GLCullFaceLock, this function is needed to unlock the cull face mode again.
 *  \sa GLCullFace, GLCullFaceLock */
inline void GLCullFaceUnlock( GLenum mode )
{
  glContext()->m_locks.m_cullFaceMode = false;
  GLCullFace( mode );
}

//! Replacement of glDeleteLists
/** This function updates some caches and asserts on the result before returning.  */
inline  void  GLDeleteLists( GLuint list, GLsizei range )
{
  NVSG_GL_STATS_DELETE_LISTS( list, range );
  glDeleteLists( list, range );
  glContext()->removeLists(range, list);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDeleteObjectARB
/** This function asserts on the result before returning.  */
inline  void  GLDeleteObject( GLhandleARB obj )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDeleteObjectARB( obj );
  ASSERT_GL_NO_ERROR;
}


//! Replacement of glDeleteProgram
/** This function updates some caches and asserts on the result before returning.  */
inline  void  GLDeleteProgram( GLuint program )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext();
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glDeleteProgram );
  ctx->m_glFunctions->glDeleteProgram( program );
  ctx->removePrograms(1, &program);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDeleteProgramsARB
/** This function hides the extension character of glDeleteProgramsARB, updates some caches,
  * and asserts on the result before returning.  */
inline  void  GLDeletePrograms( GLsizei n, const GLuint *ids )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext();
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glDeleteProgramsARB );
  ctx->m_glFunctions->glDeleteProgramsARB( n, ids );
  ctx->removePrograms(n, ids);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDeleteQueries
/** This function updates some caches, and asserts on the result before returning. */
inline  void  GLDeleteQueries( GLsizei n, const GLuint *ids )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext();
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glDeleteQueries );
  ctx->m_glFunctions->glDeleteQueries( n, ids );
  ctx->removeQueries( n, ids );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDeleteShader
/** This function asserts on the result before returning.  */
inline  void  GLDeleteShader( GLuint shader )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDeleteShader( shader );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDeleteTextures
/** This function updates some caches and asserts on the result before returning.  */
inline  void  GLDeleteTextures( GLsizei n, const GLuint *textures )
{
  NVSG_GL_STATS_DELETE_TEXTURES( n, textures );
  GLContext * ctx = glContext();
  glDeleteTextures( n, textures );
  ctx->removeTextures(n, textures);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDepthBounds
/** This function asserts on the result before returning. */
inline  void  GLDepthBounds( GLclampd zmin, GLclampd zmax )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDepthBounds( zmin, zmax );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDepthBoundsd
/** This function asserts on the result before returning. */
inline  void  GLDepthBoundsd( GLdouble zmin, GLdouble zmax )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDepthBoundsd( zmin, zmax );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDepthFunc
/** This function asserts on the result before returning.  */
inline  void  GLDepthFunc( GLenum func )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_depthFunc = ctx->m_depthBufferAttributes.m_depthFunc;
# endif
  if ( ctx->m_depthBufferAttributes.m_depthFunc != func )
#endif
  {
    NVSG_GL_STATS_DEPTH_FUNC( func );
    glDepthFunc( func );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_depthBufferAttributes.m_depthFunc = func;
#endif
  }
}

/*! \brief Helper function to reset the depth func.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the depth func
 *  to the previous value. Otherwise, it is set to GL_LESS.
 *  \sa GLDepthFunc */
inline void GLResetDepthFunc()
{
#if defined(GL_SUPPORT_RESET)
  GLDepthFunc( glContext()->m_previousAttributes.m_depthFunc );
#else
  GLDepthFunc( GL_LESS );
#endif
}

//! Replacement of glDepthMask
/** This function asserts on the result before returning.  */
inline  void  GLDepthMask( GLboolean flag )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_depthWriting = ctx->m_depthBufferAttributes.m_depthWriting;
# endif
  if ( ctx->m_depthBufferAttributes.m_depthWriting != flag )
#endif
  {
    NVSG_GL_STATS_DEPTH_MASK( flag );
    glDepthMask( flag );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_depthBufferAttributes.m_depthWriting = flag;
#endif
  }
}

/*! \brief Helper function to reset the depth mask.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the depth mask
 *  to the previous value. Otherwise, it is set to GL_TRUE.
 *  \sa GLDepthMask */
inline void GLResetDepthMask()
{
#if defined(GL_SUPPORT_RESET)
  GLDepthMask( glContext()->m_previousAttributes.m_depthWriting );
#else
  GLDepthMask( GL_TRUE );
#endif
}

//! Replacement of glDepthRange
/** This function asserts on the result before returning.  */
inline  void  GLDepthRange( GLclampd znear, GLclampd zfar )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_depthRangeNear = ctx->m_viewportAttributes.m_depthRangeNear;
  ctx->m_previousAttributes.m_depthRangeFar  = ctx->m_viewportAttributes.m_depthRangeFar;
# endif
  if (    ( ctx->m_viewportAttributes.m_depthRangeNear != znear )
      ||  ( ctx->m_viewportAttributes.m_depthRangeFar  != zfar ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glDepthRange( znear, zfar );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_viewportAttributes.m_depthRangeNear = znear;
    ctx->m_viewportAttributes.m_depthRangeFar  = zfar;
#endif
  }
}

//! Replacement of glDepthRandedNV
/** This function asserts on the result before returning. */
inline  void  GLDepthRanged( GLdouble zNear, GLdouble zFar )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDepthRanged( zNear, zFar );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Helper function to reset the depth range.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the depth range
 *  to the previous values. Otherwise, it sets \a znear to 0.0 and \a zfar to 1.0.
 *  \sa GLDepthRange */
inline void GLResetDepthRange()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLDepthRange( ctx->m_previousAttributes.m_depthRangeNear
              , ctx->m_previousAttributes.m_depthRangeFar );
#else
  GLDepthRange( 0.0, 1.0 );
#endif
}

//! Replacement of glDetachObjectARB
inline  void  GLDetachObject( GLhandleARB containerObj, GLhandleARB attachedObj )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDetachObjectARB( containerObj, attachedObj );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDetachShader
inline  void  GLDetachShader( GLuint program, GLuint shader )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDetachShader( program, shader );
  ASSERT_GL_NO_ERROR;
}

#if !defined(DOXYGEN_IGNORE)  // no need to document internal helper functions/macros
#if defined( GL_CACHING )
inline void _GLDisableCached( GLenum cap, GLboolean * cache )
{
  if ( *cache )
  {
    NVSG_ASSERT( glIsEnabled( cap ) );
    _GLDisable( cap );
    *cache = GL_FALSE;
  }
}
inline void _GLEnableCached( GLenum cap, GLboolean * cache )
{
  if ( !*cache )
  {
    NVSG_ASSERT( ! glIsEnabled( cap ) );
    _GLEnable( cap );
    *cache = GL_TRUE;
  }
}
inline GLboolean _GLIsEnabledCached( GLenum cap, GLboolean cache )
{
  NVSG_ASSERT( cache == glIsEnabled( cap ) );
  return( cache );
}
# if defined(GL_SUPPORT_RESET)
inline void _GLEnableCached( GLenum cap, GLboolean * cache, GLboolean * prev )
{
  *prev = *cache;
  _GLEnableCached( cap, cache );
}
inline void _GLDisableCached( GLenum cap, GLboolean * cache, GLboolean * prev )
{
  *prev = *cache;
  _GLDisableCached( cap, cache );
}
#  define GL_DISABLE( cap, cache, prev )  _GLDisableCached( cap, cache, prev )
#  define GL_ENABLE( cap, cache, prev )   _GLEnableCached( cap, cache, prev )
#  define GL_IS_ENABLED( cap, cache )      _GLIsEnabledCached( cap, cache )
# else
#  define GL_DISABLE( cap, cache, prev )   _GLDisable( cap )
#  define GL_ENABLE( cap, cache, prev )    _GLEnable( cap )
#  define GL_IS_ENABLED( cap, cache )      _GLIsEnabled( cap )
# endif
#else
# define GL_DISABLE( cap, cache, prev )  _GLDisable( cap )
# define GL_ENABLE( cap, cache, prev )   _GLEnable( cap )
# define GL_IS_ENABLED( cap, cache )     _GLIsEnabled( cap )
#endif

#if defined(GL_SUPPORT_RESET)
# define GL_RESET( cap, cache, prev ) ( prev ? _GLEnableCached( cap, cache ) : _GLDisableCached( cap, cache ) )
#else
# define GL_RESET( cap, cache, prev ) _GLDisable( cap );
#endif
#endif // DOXYGEN_IGNORE


//! Cached version of GLDisable( GL_ALPHA_TEST )
/** This function caches the GL_ALPHA_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_ALPHA_TEST) directly. */
inline  void  GLDisableAlphaTest( void )
{
  GL_DISABLE( GL_ALPHA_TEST, &glContext()->m_colorBufferEnableAttributes.m_alphaTest
            , &glContext()->m_previousAttributes.m_alphaTest );
}

//! Cached version of GLDisable( GL_AUTO_NORMAL )
/** This function caches the GL_AUTO_NORMAL state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_AUTO_NORMAL) directly. */
inline  void  GLDisableAutoNormal( void )
{
  GL_DISABLE( GL_AUTO_NORMAL, &glContext()->m_evalAttributes.m_autoNormal
            , &glContext()->m_previousAttributes.m_autoNormal );
}

//! Cached version of GLDisable( GL_BLEND )
/** This function caches the GL_BLEND state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_BLEND) directly. */
inline  void  GLDisableBlend( void )
{
  GL_DISABLE( GL_BLEND, &glContext()->m_colorBufferEnableAttributes.m_blend
            , &glContext()->m_previousAttributes.m_blend );
}

//! Cached version of GLDisable( GL_CLIP_PLANEi )
/** This function caches the GL_CLIP_PLANEi state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_CLIP_PLANEi) directly. */
inline  void  GLDisableClipPlane( GLenum plane )
{
#if defined( GL_CACHING )
  NVSG_ASSERT( plane-GL_CLIP_PLANE0 < glContext()->m_transformEnableAttributes.m_clipPlanes.size() );
#endif
  GL_DISABLE( plane, &glContext()->m_transformEnableAttributes.m_clipPlanes[plane-GL_CLIP_PLANE0]
            , &glContext()->m_previousAttributes.m_clipPlanes[plane-GL_CLIP_PLANE0] );
}

//! Cached version of GLDisable( GL_COLOR_LOGIC_OP )
/** This function caches the GL_COLOR_LOGIC_OP state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_COLOR_LOGIC_OP) directly. */
inline  void  GLDisableColorLogicOp( void )
{
  GL_DISABLE( GL_COLOR_LOGIC_OP, &glContext()->m_colorBufferEnableAttributes.m_colorLogicOp
            , &glContext()->m_previousAttributes.m_colorLogicOp );
}

//! Cached version of GLDisable( GL_COLOR_MATERIAL )
/** This function caches the GL_COLOR_MATERIAL state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_COLOR_MATERIAL) directly. */
inline  void  GLDisableColorMaterial( void )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_colorMaterial = ctx->m_lightingAttributes.m_colorMaterial;
# endif
  if ( ctx->m_lightingAttributes.m_colorMaterial )
  {
    NVSG_ASSERT( glIsEnabled( GL_COLOR_MATERIAL ) );
#endif
    _GLDisable( GL_COLOR_MATERIAL );
#if defined( GL_CACHING )
    ctx->m_lightingAttributes.m_colorMaterial = GL_FALSE;
    glGetMaterialfv( GL_BACK, GL_AMBIENT, &ctx->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_BACK][0] );
    glGetMaterialfv( GL_FRONT, GL_AMBIENT, &ctx->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0] );
    glGetMaterialfv( GL_BACK, GL_DIFFUSE, &ctx->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0] );
    glGetMaterialfv( GL_FRONT, GL_DIFFUSE, &ctx->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0] );
  }
#endif
}

//! Cached version of GLDisable( GL_CULL_FACE )
/** This function is caches the GL_CULL_FACE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_CULL_FACE) directly. */
inline  void  GLDisableCullFace( void )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_cullFace = ctx->m_polygonEnableAttributes.m_cullFace;
# endif
  if ( ctx->m_polygonEnableAttributes.m_cullFace )
#endif
  {
    if ( ! ctx->m_locks.m_cullFace )
    {
#if defined( GL_CACHING )
      NVSG_ASSERT( glIsEnabled( GL_CULL_FACE ) );
#endif
      _GLDisable( GL_CULL_FACE );
#if defined( GL_CACHING )
      ctx->m_polygonEnableAttributes.m_cullFace = GL_FALSE;
#endif
    }
  }
}

/*! \brief Helper function to disable the cull face and lock it.
 *  \remarks This function disables face culling and locks it. Any subsequent calls to
 *  GLEnableCullFace, GLDisableCullFace, GLEnable(GL_CULL_FACE), or GLDisable(GL_CULL_FACE) will not
 *  change that setting, until the next call to GLDisableCullFaceUnlock or GLEnableCullFaceUnlock.
 *  \sa GLDisableCullFace, GLDisableCullFaceUnlock, GLEnableCullFaceLock */
inline void GLDisableCullFaceLock( void )
{
  NVSG_ASSERT( ! glContext()->m_locks.m_cullFace );
  GLDisableCullFace();
  glContext()->m_locks.m_cullFace = true;
}

/*! \brief Helper function to unlock the cull face and set it.
 *  \remarks This function unlocks face culling and disables it.
 *  \sa GLDisableCullFace, GLDisableCullFaceLock, GLEnableCullFaceUnlock */
inline void GLDisableCullFaceUnlock( void )
{
  NVSG_ASSERT( glContext()->m_locks.m_cullFace );
  glContext()->m_locks.m_cullFace = false;
  GLDisableCullFace();
}

//! Cached version of GLDisable( GL_DEPTH_TEST )
/** This function caches the GL_DEPTH_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_DEPTH_TEST) directly. */
inline  void  GLDisableDepthTest( void )
{
  GL_DISABLE( GL_DEPTH_TEST, &glContext()->m_depthBufferEnableAttributes.m_depthTesting
            , &glContext()->m_previousAttributes.m_depthTesting );
}

//! Cached version of GLDisable( GL_DITHER )
/** This function caches the GL_DITHER state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_DITHER) directly. */
inline  void  GLDisableDither( void )
{
  GL_DISABLE( GL_DITHER, &glContext()->m_colorBufferEnableAttributes.m_dither
            , &glContext()->m_previousAttributes.m_dither );
}

//! Cached version of GLDisable( GL_FOG )
/** This function caches the GL_DITHER state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_FOG) directly. */
inline  void  GLDisableFog( void )
{
  GL_DISABLE( GL_FOG, &glContext()->m_fogEnableAttributes.m_fog
            , &glContext()->m_previousAttributes.m_fog );
}

//! Replacement of glDiableIndexed
/** This functions asserts on the result before returning. */
inline  void  GLDisableIndexed( GLenum target, GLuint index )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDisableIndexed( target, index );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of GLDisable( GL_LIGHTi )
/** This function caches the GL_LIGHTi state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LIGHTi) directly. */
inline  void  GLDisableLight( GLenum light )
{
#if defined( GL_CACHING )
  NVSG_ASSERT( light-GL_LIGHT0 < glContext()->m_lightingEnableAttributes.m_lights.size() );
#endif
  GL_DISABLE( light, &glContext()->m_lightingEnableAttributes.m_lights[light-GL_LIGHT0]
            , &glContext()->m_previousAttributes.m_lights[light-GL_LIGHT0] );
}

//! Cached version of GLDisable( GL_LIGHTING )
/** This function is caches the GL_LIGHTING state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LIGHTING) directly. */
inline  void  GLDisableLighting( void )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_lighting = ctx->m_lightingEnableAttributes.m_lighting;
# endif
  if ( ctx->m_lightingEnableAttributes.m_lighting )
#endif
  {
    if ( ! ctx->m_locks.m_lighting )
    {
#if defined( GL_CACHING )
      NVSG_ASSERT( glIsEnabled( GL_LIGHTING ) );
#endif
      _GLDisable( GL_LIGHTING );
#if defined( GL_CACHING )
      ctx->m_lightingEnableAttributes.m_lighting = GL_FALSE;
#endif
    }
  }
}

/*! \brief Helper function to disable lighting and lock it.
 *  \remarks This function disables lighting and locks it. Any subsequent calls to GLEnableLighting,
 *  GLDisableLighting, GLEnable(GL_LIGHTING), or GLDisable(GL_LIGHTING) will not change that
 *  setting, until the next call to GLDisableLightingUnlock or GLEnableLightingUnlock.
 *  \sa GLDisableLighting, GLDisableLightingUnlock, GLEnableLightingLock */
inline void GLDisableLightingLock( void )
{
  NVSG_ASSERT( ! glContext()->m_locks.m_lighting );
  GLDisableLighting();
  glContext()->m_locks.m_lighting = true;
}

/*! \brief Helper function to unlock the lighting and disable it.
 *  \remarks This function unlocks lighting and disables it.
 *  \sa GLDisableLighting, GLDisableLightingLock, GLEnableLightingUnlock */
inline void GLDisableLightingUnlock( void )
{
  NVSG_ASSERT( glContext()->m_locks.m_lighting );
  glContext()->m_locks.m_lighting = false;
  GLDisableLighting();
}

//! Cached version of GLDisable( GL_LINE_SMOOTH )
/** This function is caches the GL_LINE_SMOOTH state.
* It is faster to call this method instead of calling GL[Disable|Enable](GL_LINE_SMOOTH) directly. */
inline  void  GLDisableLineSmooth( void )
{
  GL_DISABLE( GL_LINE_SMOOTH, &glContext()->m_lineEnableAttributes.m_smooth
            , &glContext()->m_previousAttributes.m_lineSmooth );
}

//! Cached version of GLDisable( GL_LINE_STIPPLE )
/** This function is caches the GL_LINE_STIPPLE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LINE_STIPPLE) directly. */
inline  void  GLDisableLineStipple( void )
{
  GL_DISABLE( GL_LINE_STIPPLE, &glContext()->m_lineEnableAttributes.m_stipple
            , &glContext()->m_previousAttributes.m_lineStipple );
}

//! Cached version of GLDisable( GL_LOGIC_OP )
/** This function caches the GL_LOGIC_OP state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LOGIC_OP) directly. */
inline  void  GLDisableLogicOp( void )
{
  GL_DISABLE( GL_LOGIC_OP, &glContext()->m_colorBufferEnableAttributes.m_logicOp
            , &glContext()->m_previousAttributes.m_logicOp );
}

//! Cached version of GLDisable( GL_NORMALIZE )
/** This function is caches the GL_NORMALIZE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_NORMALIZE) directly. */
inline  void  GLDisableNormalize( void )
{
  GL_DISABLE( GL_NORMALIZE, &glContext()->m_transformEnableAttributes.m_normalize
            , &glContext()->m_previousAttributes.m_normalize );
}

//! Cached version of GLDisable( GL_RESCALE_NORMAL )
/** This function is caches the GL_RESCALE_NORMAL state.
* It is faster to call this method instead of calling GL[Disable|Enable](GL_RESCALE_NORMAL) directly. */
inline  void  GLDisableRescaleNormal( void )
{
  GL_DISABLE( GL_RESCALE_NORMAL, &glContext()->m_transformEnableAttributes.m_rescaleNormal
            , &glContext()->m_previousAttributes.m_rescaleNormal );
}

//! Cached version of GLDisable( GL_POINT_SMOOTH )
/** This function is caches the GL_POINT_SMOOTH state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POINT_SMOOTH) directly. */
inline  void  GLDisablePointSmooth( void )
{
  GL_DISABLE( GL_POINT_SMOOTH, &glContext()->m_pointEnableAttributes.m_smooth
            , &glContext()->m_previousAttributes.m_pointSmooth );
}

//! Cached version of GLDisable( GL_POLYGON_OFFSET_FILL )
/** This function is caches the GL_POLYGON_OFFSET_FILL state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_OFFSET_FILL) directly. */
inline  void  GLDisablePolygonOffsetFill( void )
{
  GL_DISABLE( GL_POLYGON_OFFSET_FILL, &glContext()->m_polygonEnableAttributes.m_offsetFill
            , &glContext()->m_previousAttributes.m_offsetFill );
}

//! Cached version of GLDisable( GL_POLYGON_OFFSET_LINE )
/** This function is caches the GL_POLYGON_OFFSET_LINE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_OFFSET_LINE) directly. */
inline  void  GLDisablePolygonOffsetLine( void )
{
  GL_DISABLE( GL_POLYGON_OFFSET_LINE, &glContext()->m_polygonEnableAttributes.m_offsetLine
            , &glContext()->m_previousAttributes.m_offsetLine );
}

//! Cached version of GLDisable( GL_POLYGON_OFFSET_POINT )
/** This function is caches the GL_POLYGON_OFFSET_POINT state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_OFFSET_POINT) directly. */
inline  void  GLDisablePolygonOffsetPoint( void )
{
  GL_DISABLE( GL_POLYGON_OFFSET_POINT, &glContext()->m_polygonEnableAttributes.m_offsetPoint
            , &glContext()->m_previousAttributes.m_offsetPoint );
}

//! Cached version of GLDisable( GL_POLYGON_SMOOTH )
/** This function caches the GL_POLYGON_SMOOTH state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_SMOOTH) directly. */
inline  void  GLDisablePolygonSmooth( void )
{
  GL_DISABLE( GL_POLYGON_SMOOTH, &glContext()->m_polygonEnableAttributes.m_smooth
            , &glContext()->m_previousAttributes.m_polygonSmooth );
}

//! Cached version of GLDisable( GL_POLYGON_STIPPLE )
/** This function is caches the GL_POLYGON_STIPPLE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_STIPPLE) directly. */
inline  void  GLDisablePolygonStipple( void )
{
  GL_DISABLE( GL_POLYGON_STIPPLE, &glContext()->m_polygonEnableAttributes.m_stipple
            , &glContext()->m_previousAttributes.m_polygonStipple );
}

//! Cached version of GLDisable( GL_SCISSOR_TEST )
/** This function caches the GL_SCISSOR_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_SCISSOR_TEST) directly. */
inline  void  GLDisableScissorTest( void )
{
  GL_DISABLE( GL_SCISSOR_TEST, &glContext()->m_scissorEnableAttributes.m_scissorTest
            , &glContext()->m_previousAttributes.m_scissorTest );
}

//! Cached version of GLDisable( GL_STENCIL_TEST )
/** This function caches the GL_STENCIL_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_STENCIL_TEST) directly. */
inline  void  GLDisableStencilTest( void )
{
  GL_DISABLE( GL_STENCIL_TEST, &glContext()->m_stencilBufferEnableAttributes.m_stencilTest
            , &glContext()->m_previousAttributes.m_stencilTest );
}

//! Cached version of GLDisable( [GL_TEXTURE_1D|GL_TEXTURE_2D|GL_TEXTURE_3D|GL_TEXTURE_CUBE_MAP|GL_TEXTURE_RECTANGLE_NV]
//!                              [GL_TEXTURE_2D_ARRAY_EXT | GL_TEXTURE_1D_ARRAY_EXT] );
/** This function caches the texture target type per texture unit. It is faster to call this method instead of calling
  * GL[Disable|Enable]( [GL_TEXTURE_1D|GL_TEXTURE_2D|GL_TEXTURE_3D|GL_TEXTURE_CUBE_MAP|GL_TEXTURE_RECTANGLE_NV] ) directly. */
inline void GLDisableTexture( GLenum target )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  NVSG_ASSERT(   ( target == GL_TEXTURE_1D ) || ( target == GL_TEXTURE_2D ) || ( target == GL_TEXTURE_3D )
             ||  ( target == GL_TEXTURE_CUBE_MAP ) || ( target == GL_TEXTURE_RECTANGLE_NV ) 
             ||  ( target == GL_TEXTURE_2D_ARRAY_EXT ) || ( target == GL_TEXTURE_1D_ARRAY_EXT ) );
  if ( ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_toActivate].m_texture[target] )
  {
    _GLUpdateActiveTexture();
    // array targets are not supported in fixed function pipeline and therefore
    // should not be enabled/disabled.
    if( target != GL_TEXTURE_2D_ARRAY_EXT && target != GL_TEXTURE_1D_ARRAY_EXT )
    {
      NVSG_ASSERT( glIsEnabled( target ) );
    }
#endif
    if( target != GL_TEXTURE_2D_ARRAY_EXT && target != GL_TEXTURE_1D_ARRAY_EXT )
    {
      _GLDisable( target );
    }
#if defined( GL_CACHING )
    ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_active].m_texture[target] = GL_FALSE;
  }
#endif
}

//! Cached version of GLDisable( [GL_TEXTURE_GEN_S|GL_TEXTURE_GEN_T|GL_TEXTURE_GEN_R|GL_TEXTURE_GEN_Q] )
/** This function caches the texture generation type per texture unit. It is faster to call this method instead of calling
  * GL[Disable|Enable]( [GL_TEXTURE_GEN_S|GL_TEXTURE_GEN_T|GL_TEXTURE_GEN_R|GL_TEXTURE_GEN_Q] ) directly. */
inline void GLDisableTextureGen( GLenum target )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  NVSG_ASSERT(    ( target == GL_TEXTURE_GEN_S )
              ||  ( target == GL_TEXTURE_GEN_T )
              ||  ( target == GL_TEXTURE_GEN_R )
              ||  ( target == GL_TEXTURE_GEN_Q ) );
  if ( ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_toActivate].m_gen[target-GL_TEXTURE_GEN_S] )
  {
    _GLUpdateActiveTexture();
    NVSG_ASSERT( glIsEnabled( target ) );
#endif
    _GLDisable( target );
#if defined( GL_CACHING )
    ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_active].m_gen[target-GL_TEXTURE_GEN_S] = GL_FALSE;
  }
#endif
}

//! Replacement of glDisable
/** This function caches on some cap values and asserts on the result before returning. */
inline  void  GLDisable( GLenum cap )
{
#if defined( GL_CACHING )
  switch( cap )
  {
    case GL_ALPHA_TEST :
      GLDisableAlphaTest();
      break;
    case GL_AUTO_NORMAL :
      GLDisableAutoNormal();
      break;
    case GL_BLEND :
      GLDisableBlend();
      break;
    case GL_CLIP_PLANE0 :
    case GL_CLIP_PLANE1 :
    case GL_CLIP_PLANE2 :
    case GL_CLIP_PLANE3 :
    case GL_CLIP_PLANE4 :
    case GL_CLIP_PLANE5 :
      GLDisableClipPlane( cap );
      break;
    case GL_COLOR_LOGIC_OP :
      GLDisableColorLogicOp();
      break;
    case GL_COLOR_MATERIAL :
      GLDisableColorMaterial();
      break;
    case GL_CULL_FACE :
      GLDisableCullFace();
      break;
    case GL_DEPTH_TEST :
      GLDisableDepthTest();
      break;
    case GL_DITHER :
      GLDisableDither();
      break;
    case GL_FOG :
      GLDisableFog();
      break;
    case GL_LIGHT0 :
    case GL_LIGHT1 :
    case GL_LIGHT2 :
    case GL_LIGHT3 :
    case GL_LIGHT4 :
    case GL_LIGHT5 :
    case GL_LIGHT6 :
    case GL_LIGHT7 :
      GLDisableLight( cap );
      break;
    case GL_LIGHTING :
      GLDisableLighting();
      break;
    case GL_LINE_SMOOTH :
      GLDisableLineSmooth();
      break;
    case GL_LINE_STIPPLE :
      GLDisableLineStipple();
      break;
    case GL_LOGIC_OP :
      GLDisableLogicOp();
      break;
    case GL_NORMALIZE :
      GLDisableNormalize();
      break;
    case GL_RESCALE_NORMAL :
      GLDisableRescaleNormal();
      break;
    case GL_POINT_SMOOTH :
      GLDisablePointSmooth();
      break;
    case GL_POLYGON_OFFSET_FILL :
      GLDisablePolygonOffsetFill();
      break;
    case GL_POLYGON_OFFSET_LINE :
      GLDisablePolygonOffsetLine();
      break;
    case GL_POLYGON_OFFSET_POINT :
      GLDisablePolygonOffsetPoint();
      break;
    case GL_POLYGON_SMOOTH :
      GLDisablePolygonSmooth();
      break;
    case GL_POLYGON_STIPPLE :
      GLDisablePolygonStipple();
      break;
    case GL_SCISSOR_TEST :
      GLDisableScissorTest();
      break;
    case GL_STENCIL_TEST :
      GLDisableStencilTest();
      break;
    case GL_TEXTURE_1D :
    case GL_TEXTURE_2D :
    case GL_TEXTURE_3D :
    case GL_TEXTURE_CUBE_MAP :
    case GL_TEXTURE_RECTANGLE_NV :
    case GL_TEXTURE_2D_ARRAY_EXT:
    case GL_TEXTURE_1D_ARRAY_EXT:
      GLDisableTexture( cap );
      break;
    case GL_TEXTURE_GEN_S :
    case GL_TEXTURE_GEN_T :
    case GL_TEXTURE_GEN_R :
    case GL_TEXTURE_GEN_Q :
      GLDisableTextureGen( cap );
      break;
    default :
      _GLDisable( cap );
      break;
  }
#else
  switch( cap )
  {
    case GL_CULL_FACE :
      GLDisableCullFace();
      break;
    default :
      _GLDisable( cap );
      break;
  }
#endif
}

#if !defined(DOXYGEN_IGNORE)
#if defined( GL_CACHING )
inline void _GLDisableClientStateCached( GLenum cap, GLboolean * cache )
{
  if ( *cache )
  {
    NVSG_ASSERT( glIsEnabled( cap ) );
    _GLDisableClientState( cap );
    *cache = GL_FALSE;
  }
}
#define GL_DISABLE_CLIENT_STATE( cap, cache )   _GLDisableClientStateCached( cap, cache )
#else
#define GL_DISABLE_CLIENT_STATE( cap, cache )   _GLDisableClientState( cap );
#endif
#endif // DOXYGEN_IGNORE

//! Cached version of GLDisableClientState( GL_COLOR_ARRAY )
/** This function caches the GL_COLOR_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_COLOR_ARRAY ) directly. */
inline  void  GLDisableColorArray( void )
{
  GL_DISABLE_CLIENT_STATE( GL_COLOR_ARRAY, &glContext()->m_vertexArrayAttributes.m_colorArray );
}

//! Cached version of GLDisableClientState( GL_FOG_COORDINATE_ARRAY )
/** This function caches the GL_FOG_COORDINATE_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_FOG_COORDINATE_ARRAY ) directly. */
inline  void  GLDisableFogCoordinateArray( void )
{
  GL_DISABLE_CLIENT_STATE( GL_FOG_COORDINATE_ARRAY, &glContext()->m_vertexArrayAttributes.m_fogCoordArray );
}

//! Cached version of GLDisableClientState( GL_NORMAL_ARRAY )
/** This function caches the GL_NORMAL_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_NORMAL_ARRAY ) directly. */
inline  void  GLDisableNormalArray( void )
{
  GL_DISABLE_CLIENT_STATE( GL_NORMAL_ARRAY, &glContext()->m_vertexArrayAttributes.m_normalArray );
}

//! Cached version of GLDisableClientState( GL_SECONDARY_COLOR_ARRAY )
/** This function caches the GL_SECONDARY_COLOR_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_SECONDARY_COLOR_ARRAY ) directly. */
inline  void  GLDisableSecondaryColorArray( void )
{
  GL_DISABLE_CLIENT_STATE( GL_SECONDARY_COLOR_ARRAY, &glContext()->m_vertexArrayAttributes.m_secondaryColorArray );
}

//! Cached version of GLDisableClientState( GL_TEXTURE_COORD_ARRAY )
/** This function caches the GL_TEXTURE_COORD_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_TEXTURE_COORD_ARRAY ) directly. */
inline  void  GLDisableTextureCoordArray( void )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  if ( ctx->m_vertexArrayAttributes.m_textureCoordArray[ctx->m_vertexArrayAttributes.m_clientTextureToActivate] )
  {
    _GLUpdateClientActiveTexture();
    NVSG_ASSERT( glIsEnabled( GL_TEXTURE_COORD_ARRAY ) );
#endif
    _GLDisableClientState( GL_TEXTURE_COORD_ARRAY );
#if defined( GL_CACHING )
    ctx->m_vertexArrayAttributes.m_textureCoordArray[ctx->m_vertexArrayAttributes.m_clientTextureActive] = GL_FALSE;
  }
#endif
}

//! Cached version of GLDisableClientState( GL_VERTEX_ARRAY )
/** This function caches the GL_VERTEX_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_VERTEX_ARRAY ) directly. */
inline  void  GLDisableVertexArray( void )
{
  GL_DISABLE_CLIENT_STATE( GL_VERTEX_ARRAY, &glContext()->m_vertexArrayAttributes.m_vertexArray );
}

//! Cached version of GLDisableClientState( GL_PRIMITIVE_RESTART_NV )
/** This function caches the GL_PRIMITIVE_RESTART_NV client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_PRIMITIVE_RESTART_NV ) directly. */
inline void GLDisablePrimitiveRestart()
{
  GL_DISABLE_CLIENT_STATE( GL_PRIMITIVE_RESTART_NV, &glContext()->m_vertexArrayAttributes.m_primitiveRestart );
}

//! Replacement of glDisableClientState
/** This function caches on some cap values and asserts on the result before returning. */
inline  void  GLDisableClientState( GLenum array )
{
#if defined( GL_CACHING )
  switch( array )
  {
    case GL_COLOR_ARRAY :
      GLDisableColorArray();
      break;
    case GL_FOG_COORDINATE_ARRAY :
      GLDisableFogCoordinateArray();
      break;
    case GL_NORMAL_ARRAY :
      GLDisableNormalArray();
      break;
    case GL_SECONDARY_COLOR_ARRAY :
      GLDisableSecondaryColorArray();
      break;
    case GL_TEXTURE_COORD_ARRAY :
      GLDisableTextureCoordArray();
      break;
    case GL_VERTEX_ARRAY :
      GLDisableVertexArray();
      break;
    case GL_PRIMITIVE_RESTART_NV :
      GLDisablePrimitiveRestart();
      break;
    default :
      _GLDisableClientState( array );
      break;
  }
#else
  _GLDisableClientState( array );
#endif
}

//! Replacement of glDisableVertexAttribArray
inline void GLDisableVertexAttribArray( GLuint index )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLContext * ctx = glContext();
#if defined(GL_CACHING)
  NVSG_ASSERT(index<(GLuint)ctx->m_maxVertexAttribs);
  if ( ctx->m_vertexArrayAttributes.m_vertexAttribArray[index] != GL_FALSE )
#endif
  {
    ctx->m_glFunctions->glDisableVertexAttribArray(index);
#if defined(GL_CACHING)
    ctx->m_vertexArrayAttributes.m_vertexAttribArray[index] = GL_FALSE;
#endif
    ASSERT_GL_NO_ERROR;
  }
}

//! Replacement of glDrawArrays
/** This function asserts on the result before returning.  */
inline  void  GLDrawArrays( GLenum mode, GLint first, GLsizei count )
{
  NVSG_GL_STATS_FCT_COUNT();
  glDrawArrays( mode, first, count );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDrawArraysInstancedEXT
/** This function asserts on the result before returning.  */
inline  void  GLDrawArraysInstanced( GLenum mode, GLint start, GLsizei count, GLsizei primcount )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDrawArraysInstanced( mode, start, count, primcount );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDrawBuffer
/** This function asserts on the result before returning.  */
inline  void  GLDrawBuffer( GLenum mode ) 
{ 
  NVSG_GL_STATS_FCT_COUNT();
  glDrawBuffer( mode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDrawBuffersARB
/** This function asserts on the result before returning.  */
inline  void  GLDrawBuffers( GLsizei n, const GLenum *bufs )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDrawBuffers( n, bufs );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDrawElements
/** This function asserts on the result before returning.  */
inline  void  GLDrawElements( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices )
{
  NVSG_GL_STATS_DRAW_ELEMENTS( mode, count, type, indices );
  glDrawElements( mode, count, type, indices );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDrawElementsInstancedEXT
/** This function asserts on the result before returning.  */
inline  void  GLDrawElementsInstanced( GLenum mode, GLsizei count, GLenum type, const GLvoid *indices, GLsizei primcount )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDrawElementsInstanced( mode, count, type, indices, primcount );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMultiDrawArrays
/** This function asserts on the result before returning.  */
inline  void  GLMultiDrawArrays( GLenum mode, GLint *first, GLsizei *count, GLsizei primcount )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiDrawArrays( mode, first, count, primcount );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMultiDrawElements
/** This function asserts on the result before returning.  */
inline  void  GLMultiDrawElements( GLenum mode, const GLsizei *count, GLenum type, const GLvoid* *indices, GLsizei primcount )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiDrawElements( mode, count, type, indices, primcount );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDrawPixels
/** This function asserts on the result before returning.  */
inline  void  GLDrawPixels( GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  glDrawPixels( width, height, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glDrawRangeElements
/** This function asserts on the result before returning.  */
inline  void  GLDrawRangeElements( GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const GLvoid *indices )
{
  NVSG_GL_STATS_DRAW_RANGE_ELEMENTS( mode, start, end, count, type, indices );
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glDrawRangeElements( mode, start, end, count, type, indices );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEdgeFlagPointer
/** This function asserts on the result before returning.  */
inline  void  GLEdgeFlagPointer( GLsizei stride, const GLboolean *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEdgeFlagPointer( stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEdgeFlag
/** This function asserts on the result before returning.  */
inline  void  GLEdgeFlag( GLboolean flag )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEdgeFlag( flag );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glEdgeFlagv
/** This function asserts on the result before returning.  */
inline  void  GLEdgeFlagv( const GLboolean *flag )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEdgeFlagv( flag );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Cached version of GLEnable( GL_ALPHA_TEST )
/** This function caches the GL_ALPHA_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_ALPHA_TEST) directly. */
inline  void  GLEnableAlphaTest( void )
{
  GL_ENABLE( GL_ALPHA_TEST, &glContext()->m_colorBufferEnableAttributes.m_alphaTest
           , &glContext()->m_previousAttributes.m_alphaTest );
}

/*! \brief Helper function to reset the enable state of the alpha test.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the alpha test
 *  state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableAlphaTest, GLEnableAlphaTest, GLDisable, GLEnable */
inline void GLResetAlphaTest()
{
  GL_RESET( GL_ALPHA_TEST, &glContext()->m_colorBufferEnableAttributes.m_alphaTest
          , glContext()->m_previousAttributes.m_alphaTest );
}

//! Cached version of GLEnable( GL_AUTO_NORMAL )
/** This function caches the GL_AUTO_NORMAL state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_AUTO_NORMAL) directly. */
inline  void  GLEnableAutoNormal( void )
{
  GL_ENABLE( GL_AUTO_NORMAL, &glContext()->m_evalAttributes.m_autoNormal
           , &glContext()->m_previousAttributes.m_autoNormal );
}

/*! \brief Helper function to reset the enable state of the auto normal calculation.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the auto normal
 *  state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableAutoNormal, GLEnableAutoNormal, GLDisable, GLEnable */
inline void GLResetAutoNormal()
{
  GL_RESET( GL_AUTO_NORMAL, &glContext()->m_evalAttributes.m_autoNormal
          , glContext()->m_previousAttributes.m_autoNormal );
}

//! Cached version of GLEnable( GL_BLEND )
/** This function caches the GL_BLEND state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_BLEND) directly. */
inline  void  GLEnableBlend( void )
{
  GL_ENABLE( GL_BLEND, &glContext()->m_colorBufferEnableAttributes.m_blend
           , &glContext()->m_previousAttributes.m_blend );
}

/*! \brief Helper function to reset the enable state of the blending.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the blending
 *  state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableBlend, GLEnableBlend, GLDisable, GLEnable */
inline void GLResetBlend()
{
  GL_RESET( GL_BLEND, &glContext()->m_colorBufferEnableAttributes.m_blend
          , glContext()->m_previousAttributes.m_blend );
}

//! Cached version of GLEnable( GL_CLIP_PLANEi )
/** This function caches the GL_CLIP_PLANEi state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_CLIP_PLANEi) directly. */
inline  void  GLEnableClipPlane( GLenum plane )
{
#if defined( GL_CACHING )
  NVSG_ASSERT( plane-GL_CLIP_PLANE0 < glContext()->m_transformEnableAttributes.m_clipPlanes.size() );
#endif
  GL_ENABLE( plane, &glContext()->m_transformEnableAttributes.m_clipPlanes[plane-GL_CLIP_PLANE0]
           , &glContext()->m_previousAttributes.m_clipPlanes[plane-GL_CLIP_PLANE0] );
}

/*! \brief Helper function to reset the enable state of the clip plane \a plane.
 *  \param plane The id of the plane for which the the enable state is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the clip plane
 *  state for \a plane to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableClipPlane, GLEnableClipPlane, GLDisable, GLEnable */
inline void GLResetClipPlane( GLenum plane )
{
  GL_RESET( plane, &glContext()->m_transformEnableAttributes.m_clipPlanes[plane-GL_CLIP_PLANE0]
          , glContext()->m_previousAttributes.m_clipPlanes[plane-GL_CLIP_PLANE0] );
}

//! Cached version of GLEnable( GL_COLOR_LOGIC_OP )
/** This function caches the GL_COLOR_LOGIC_OP state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_COLOR_LOGIC_OP) directly. */
inline  void  GLEnableColorLogicOp( void )
{
  GL_ENABLE( GL_COLOR_LOGIC_OP, &glContext()->m_colorBufferEnableAttributes.m_colorLogicOp
           , &glContext()->m_previousAttributes.m_colorLogicOp );
}

/*! \brief Helper function to reset the enable state of the color logic operation.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the color logic
 *  operation state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableColorLogicOp, GLEnableColorLogicOp, GLDisable, GLEnable */
inline void GLResetColorLogicOp()
{
  GL_RESET( GL_COLOR_LOGIC_OP, &glContext()->m_colorBufferEnableAttributes.m_colorLogicOp
          , glContext()->m_previousAttributes.m_colorLogicOp );
}

//! Cached version of GLEnable( GL_COLOR_MATERIAL )
/** This function caches the GL_COLOR_MATERIAL state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_COLOR_MATERIAL) directly. */
inline  void  GLEnableColorMaterial( void )
{
  GL_ENABLE( GL_COLOR_MATERIAL, &glContext()->m_lightingAttributes.m_colorMaterial
           , &glContext()->m_previousAttributes.m_colorMaterial );
}

/*! \brief Helper function to reset the enable state of the color material.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the color
 *  material state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableColorMaterial, GLEnableColorMaterial, GLDisable, GLEnable */
inline  void  GLResetColorMaterial()
{
  GL_RESET( GL_COLOR_MATERIAL, &glContext()->m_lightingAttributes.m_colorMaterial
          , glContext()->m_previousAttributes.m_colorMaterial );
}

//! Cached version of GLEnable( GL_CULL_FACE )
/** This function caches the GL_CULL_FACE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_CULL_FACE) directly. */
inline  void  GLEnableCullFace( void )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_cullFace = ctx->m_polygonEnableAttributes.m_cullFace;
# endif
  if ( ! ctx->m_polygonEnableAttributes.m_cullFace )
#endif
  {
    if ( ! ctx->m_locks.m_cullFace )
    {
#if defined( GL_CACHING )
      NVSG_ASSERT( ! glIsEnabled( GL_CULL_FACE ) );
#endif
      _GLEnable( GL_CULL_FACE );
#if defined( GL_CACHING )
      ctx->m_polygonEnableAttributes.m_cullFace = GL_TRUE;
#endif
    }
  }
}

/*! \brief Helper function to reset the enable state of face culling.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the face
 *  culling state to it's previous state. Otherwise, it is enabled. Note, that this enabling face
 *  culling by default is SceniX specific and not like in OpenGL.
 *  \sa GLDisableCullFace, GLEnableCullFace, GLDisable, GLEnable */
inline void GLResetCullFace()
{
#if defined(GL_SUPPORT_RESET)
  glContext()->m_previousAttributes.m_cullFace ? GLEnableCullFace() : GLDisableCullFace();
#else
  GLEnableCullFace(); // SceniX specific! Not OpenGL default!
#endif
}

/*! \brief Helper function to enable face culling and lock it.
 *  \remarks This function enables face culling and locks it. Any subsequent calls to
 *  GLEnableCullFace, GLDisableCullFace, GLEnable(GL_CULL_FACE), or GLDisable(GL_CULL_FACE) will not
 *  change that setting, until the next call to GLEnableCullFaceUnlock or GLDisableCullFaceUnlock.
 *  \sa GLEnableCullFace, GLDisableCullFaceUnlock, GLEnableCullFaceUnlock */
inline void GLEnableCullFaceLock( void )
{
  NVSG_ASSERT( ! glContext()->m_locks.m_cullFace );
  GLEnableCullFace();
  glContext()->m_locks.m_cullFace = true;
}

/*! \brief Helper function to unlock the face culling and enable it.
 *  \remarks This function unlocks face culling and enables it.
 *  \sa GLEnableCullFace, GLEnableCullFaceLock, GLDisableCullFaceUnlock */
inline void GLEnableCullFaceUnlock( void )
{
  NVSG_ASSERT( glContext()->m_locks.m_cullFace );
  glContext()->m_locks.m_cullFace = false;
  GLEnableCullFace();
}

//! Cached version of GLEnable( GL_DEPTH_TEST )
/** This function caches the GL_DEPTH_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_DEPTH_TEST) directly. */
inline  void  GLEnableDepthTest( void )
{
  GL_ENABLE( GL_DEPTH_TEST, &glContext()->m_depthBufferEnableAttributes.m_depthTesting
           , &glContext()->m_previousAttributes.m_depthTesting );
}

/*! \brief Helper function to reset the enable state of depth testing.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the depth test
 *  state to it's previous state. Otherwise, it is enabled. Note, that this enabling depth test by
 *  default is SceniX specific and not like in OpenGL.
 *  \sa GLDisableDepthTest, GLEnableDepthTest, GLDisable, GLEnable */
inline void GLResetDepthTest()
{
#if defined(GL_SUPPORT_RESET)
  glContext()->m_previousAttributes.m_depthTesting ? GLEnableDepthTest() : GLDisableDepthTest();
#else
  GLEnableDepthTest();  // SceniX specific! Not OpenGL default!
#endif
}

//! Cached version of GLEnable( GL_DITHER )
/** This function caches the GL_DITHER state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_DITHER) directly. */
inline  void  GLEnableDither( void )
{
  GL_ENABLE( GL_DITHER, &glContext()->m_colorBufferEnableAttributes.m_dither
           , &glContext()->m_previousAttributes.m_dither );
}

/*! \brief Helper function to reset the enable state of dithering.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the dither
 *  state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableDither, GLEnableDither, GLDisable, GLEnable */
inline void GLResetDither()
{
  GL_RESET( GL_DITHER, &glContext()->m_colorBufferEnableAttributes.m_dither
          , glContext()->m_previousAttributes.m_dither );
}

//! Cached version of GLEnable( GL_FOG )
/** This function caches the GL_DITHER state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_FOG) directly. */
inline  void  GLEnableFog( void )
{
  GL_ENABLE( GL_FOG, &glContext()->m_fogEnableAttributes.m_fog
           , &glContext()->m_previousAttributes.m_fog );
}

/*! \brief Helper function to reset the the enable state of fogging.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the fog state
 *  to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableFog, GLEnableFog, GLDisable, GLEnable */
inline void GLResetFog()
{
  GL_RESET( GL_FOG, &glContext()->m_fogEnableAttributes.m_fog
          , glContext()->m_previousAttributes.m_fog );
}

//! Replacement of glEnabledIndexed
/** This functions asserts on the result before returning. */
inline  void  GLEnableIndexed( GLenum target, GLuint index )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glEnableIndexed( target, index );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of GLEnable( GL_LIGHTi )
/** This function caches the GL_LIGHTi state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LIGHTi) directly. */
inline  void  GLEnableLight( GLenum light )
{
#if defined( GL_CACHING )
  NVSG_ASSERT( light-GL_LIGHT0 < glContext()->m_lightingEnableAttributes.m_lights.size() );
#endif
  GL_ENABLE( light, &glContext()->m_lightingEnableAttributes.m_lights[light-GL_LIGHT0]
           , &glContext()->m_previousAttributes.m_lights[light-GL_LIGHT0] );
}

/*! \brief Helper function to reset the enable state of the light \a light.
 *  \param light The id of the light for which the enable state is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the light state
 *  to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableLight, GLEnableLight, GLDisable, GLEnable */
inline void GLResetLight( GLenum light )
{
  GL_RESET( light, &glContext()->m_lightingEnableAttributes.m_lights[light-GL_LIGHT0]
          , glContext()->m_previousAttributes.m_lights[light-GL_LIGHT0] );
}

//! Cached version of GLEnable( GL_LIGHTING )
/** This function caches the GL_LIGHTING state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LIGHTING) directly. */
inline  void  GLEnableLighting( void )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_lighting = ctx->m_lightingEnableAttributes.m_lighting;
# endif
  if ( ! ctx->m_lightingEnableAttributes.m_lighting )
#endif
  {
    if ( ! ctx->m_locks.m_lighting )
    {
#if defined( GL_CACHING )
      NVSG_ASSERT( ! glIsEnabled( GL_LIGHTING ) );
#endif
      _GLEnable( GL_LIGHTING );
#if defined( GL_CACHING )
      ctx->m_lightingEnableAttributes.m_lighting = GL_TRUE;
#endif
    }
  }
}

/*! \brief Helper function to reset the enable state of lighting.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the lighting
 *  state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableLighting, GLEnableLighting, GLDisable, GLEnable */
inline void GLResetLighting()
{
  GL_RESET( GL_LIGHTING, &glContext()->m_lightingEnableAttributes.m_lighting
          , glContext()->m_previousAttributes.m_lighting );
}

//! Cached version of GLEnable( GL_LINE_SMOOTH )
/** This function caches the GL_LINE_SMOOTH state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LINE_SMOOTH) directly. */
inline  void  GLEnableLineSmooth( void )
{
  GL_ENABLE( GL_LINE_SMOOTH, &glContext()->m_lineEnableAttributes.m_smooth
           , &glContext()->m_previousAttributes.m_lineSmooth );
}

/*! \brief Helper function to reset the enable state of line smoothing.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the line
 *  smoothing state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableLineSmooth, GLEnableLineSmooth, GLDisable, GLEnable */
inline void GLResetLineSmooth()
{
  GL_RESET( GL_LINE_SMOOTH, &glContext()->m_lineEnableAttributes.m_smooth
          , glContext()->m_previousAttributes.m_lineSmooth );
}

//! Cached version of GLEnable( GL_LINE_STIPPLE )
/** This function caches the GL_LINE_STIPPLE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LINE_STIPPLE) directly. */
inline  void  GLEnableLineStipple( void )
{
  GL_ENABLE( GL_LINE_STIPPLE, &glContext()->m_lineEnableAttributes.m_stipple
           , &glContext()->m_previousAttributes.m_lineStipple );
}

/*! \brief Helper function to reset the enable state of line stippling.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the line
 *  stippling state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableLineStipple, GLEnableLineStipple, GLDisable, GLEnable */
inline void GLResetLineStipple()
{
  GL_RESET( GL_LINE_STIPPLE, &glContext()->m_lineEnableAttributes.m_stipple
          , glContext()->m_previousAttributes.m_lineStipple );
}

//! Cached version of GLEnable( GL_LOGIC_OP )
/** This function caches the GL_LOGIC_OP state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_LOGIC_OP) directly. */
inline  void  GLEnableLogicOp( void )
{
  GL_ENABLE( GL_LOGIC_OP, &glContext()->m_colorBufferEnableAttributes.m_logicOp
           , &glContext()->m_previousAttributes.m_logicOp );
}

/*! \brief Helper function to reset the enable state of logic operation.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the logic
 *  operation state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableLogicOp, GLEnableLogicOp, GLDisable, GLEnable */
inline void GLResetLogicOp()
{
  GL_RESET( GL_LOGIC_OP, &glContext()->m_colorBufferEnableAttributes.m_logicOp
          , glContext()->m_previousAttributes.m_logicOp );
}

//! Cached version of GLEnable( GL_NORMALIZE )
/** This function caches the GL_NORMALIZE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_NORMALIZE) directly. */
inline  void  GLEnableNormalize( void )
{
  GL_ENABLE( GL_NORMALIZE, &glContext()->m_transformEnableAttributes.m_normalize
           , &glContext()->m_previousAttributes.m_normalize );
}

//! Cached version of GLEnable( GL_RESCALE_NORMAL )
/** This function caches the GL_RESCALE_NORMAL enable state.
* It is faster to call this method instead of calling GL[Disable|Enable](GL_RESCALE_NORMAL) directly. */
inline  void  GLEnableRescaleNormal( void )
{
  GL_ENABLE( GL_RESCALE_NORMAL, &glContext()->m_transformEnableAttributes.m_rescaleNormal
           , &glContext()->m_previousAttributes.m_rescaleNormal );
}

/*! \brief Helper function to reset the enable state of GL_NORMALIZE.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the normalize
 *  state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableNormalize, GLEnableNormalize, GLDisable, GLEnable */
inline void GLResetNormalize()
{
  GL_RESET( GL_NORMALIZE, &glContext()->m_transformEnableAttributes.m_normalize
          , glContext()->m_previousAttributes.m_normalize );
}

/*! \brief Helper function to reset the enable state of GL_RESCALE_NORMAL.
*  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the GL_RESCALE_NORMAL
*  state to it's previous state. Otherwise, it is disabled.
*  \sa GLDisableRescaleNormal, GLEnableRescaleNormal, GLDisable, GLEnable */
inline void GLResetRescaleNormal()
{
  GL_RESET( GL_RESCALE_NORMAL, &glContext()->m_transformEnableAttributes.m_rescaleNormal
          , glContext()->m_previousAttributes.m_rescaleNormal );
}

//! Cached version of GLEnable( GL_POINT_SMOOTH )
/** This function caches the GL_POINT_SMOOTH state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POINT_SMOOTH) directly. */
inline  void  GLEnablePointSmooth( void )
{
  GL_ENABLE( GL_POINT_SMOOTH, &glContext()->m_pointEnableAttributes.m_smooth
           , &glContext()->m_previousAttributes.m_pointSmooth );
}

/*! \brief Helper function to reset the enable state of point smoothing.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the point
 *  smoothing state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisablePointSmooth, GLEnablePointSmooth, GLDisable, GLEnable */
inline void GLResetPointSmooth()
{
  GL_RESET( GL_POINT_SMOOTH, &glContext()->m_pointEnableAttributes.m_smooth
          , glContext()->m_previousAttributes.m_pointSmooth );
}

//! Cached version of GLEnable( GL_POLYGON_OFFSET_FILL )
/** This function caches the GL_POLYGON_OFFSET_FILL state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_OFFSET_FILL) directly. */
inline  void  GLEnablePolygonOffsetFill( void )
{
  GL_ENABLE( GL_POLYGON_OFFSET_FILL, &glContext()->m_polygonEnableAttributes.m_offsetFill
           , &glContext()->m_previousAttributes.m_offsetFill );
}

/*! \brief Helper function to reset the enable state of filled polygon offset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the polygon
 *  offset fill state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisablePolygonOffsetFill, GLEnablePolygonOffsetFill, GLDisable, GLEnable */
inline void GLResetPolygonOffsetFill()
{
  GL_RESET( GL_POLYGON_OFFSET_FILL, &glContext()->m_polygonEnableAttributes.m_offsetFill
          , glContext()->m_previousAttributes.m_offsetFill );
}

//! Cached version of GLEnable( GL_POLYGON_OFFSET_LINE )
/** This function caches the GL_POLYGON_OFFSET_LINE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_OFFSET_LINE) directly. */
inline  void  GLEnablePolygonOffsetLine( void )
{
  GL_ENABLE( GL_POLYGON_OFFSET_LINE, &glContext()->m_polygonEnableAttributes.m_offsetLine
           , &glContext()->m_previousAttributes.m_offsetLine );
}

/*! \brief Helper function to reset the enable state of lined polygon offset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the polygon
 *  offset line state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisablePolygonOffsetLine, GLEnablePolygonOffsetLine, GLDisable, GLEnable */
inline void GLResetPolygonOffsetLine()
{
  GL_RESET( GL_POLYGON_OFFSET_LINE, &glContext()->m_polygonEnableAttributes.m_offsetLine
          , glContext()->m_previousAttributes.m_offsetLine );
}

//! Cached version of GLEnable( GL_POLYGON_OFFSET_POINT )
/** This function caches the GL_POLYGON_OFFSET_POINT state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_OFFSET_POINT) directly. */
inline  void  GLEnablePolygonOffsetPoint( void )
{
  GL_ENABLE( GL_POLYGON_OFFSET_POINT, &glContext()->m_polygonEnableAttributes.m_offsetPoint
           , &glContext()->m_previousAttributes.m_offsetPoint );
}

/*! \brief Helper function to reset the enable state of pointed polygon offset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the polygon
 *  offset point state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisablePolygonOffsetPoint, GLEnablePolygonOffsetPoint, GLDisable, GLEnable */
inline void GLResetPolygonOffsetPoint()
{
  GL_RESET( GL_POLYGON_OFFSET_POINT, &glContext()->m_polygonEnableAttributes.m_offsetPoint
          , glContext()->m_previousAttributes.m_offsetPoint );
}

//! Cached version of GLEnable( GL_POLYGON_SMOOTH )
/** This function caches the GL_POLYGON_SMOOTH state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_SMOOTH) directly. */
inline  void  GLEnablePolygonSmooth( void )
{
  GL_ENABLE( GL_POLYGON_SMOOTH, &glContext()->m_polygonEnableAttributes.m_smooth
           , &glContext()->m_previousAttributes.m_polygonSmooth );
}

/*! \brief Helper function to reset the enable state of polygon smoothing.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the polygon
 *  smooth state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisablePolygonSmooth, GLEnablePolygonSmooth, GLDisable, GLEnable */
inline  void  GLResetPolygonSmooth()
{
  GL_RESET( GL_POLYGON_SMOOTH, &glContext()->m_polygonEnableAttributes.m_smooth
          , glContext()->m_previousAttributes.m_polygonSmooth );
}

//! Cached version of GLEnable( GL_POLYGON_STIPPLE )
/** This function caches the GL_POLYGON_STIPPLE state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_POLYGON_STIPPLE) directly. */
inline  void  GLEnablePolygonStipple( void )
{
  GL_ENABLE( GL_POLYGON_STIPPLE, &glContext()->m_polygonEnableAttributes.m_stipple
           , &glContext()->m_previousAttributes.m_polygonStipple );
}

/*! \brief Helper function to reset the enable state of polygon stippling.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the polygon
 *  stippling state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisablePolygonStipple, GLEnablePolygonStipple, GLDisable, GLEnable */
inline  void  GLResetPolygonStipple()
{
  GL_RESET( GL_POLYGON_STIPPLE, &glContext()->m_polygonEnableAttributes.m_stipple
          , glContext()->m_previousAttributes.m_polygonStipple );
}

//! Cached version of GLEnable( GL_SCISSOR_TEST )
/** This function caches the GL_SCISSOR_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_SCISSOR_TEST) directly. */
inline  void  GLEnableScissorTest( void )
{
  GL_ENABLE( GL_SCISSOR_TEST, &glContext()->m_scissorEnableAttributes.m_scissorTest
           , &glContext()->m_previousAttributes.m_scissorTest );
}

/*! \brief Helper function to reset the enable state of scissor testing.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the scissor
 *  test state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableScissorTest, GLEnableScissorTest, GLDisable, GLEnable */
inline  void  GLResetScissorTest()
{
  GL_RESET( GL_SCISSOR_TEST, &glContext()->m_scissorEnableAttributes.m_scissorTest
          , glContext()->m_previousAttributes.m_scissorTest );
}

//! Cached version of GLEnable( GL_STENCIL_TEST )
/** This function caches the GL_STENCIL_TEST state.
  * It is faster to call this method instead of calling GL[Disable|Enable](GL_STENCIL_TEST) directly. */
inline  void  GLEnableStencilTest( void )
{
  GL_ENABLE( GL_STENCIL_TEST, &glContext()->m_stencilBufferEnableAttributes.m_stencilTest
           , &glContext()->m_previousAttributes.m_stencilTest );
}

/*! \brief Helper function to reset the enable state of stencil testing.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the stencil
 *  test state to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableStencilTest, GLEnableStencilTest, GLDisable, GLEnable */
inline  void  GLResetStencilTest()
{
  GL_RESET( GL_STENCIL_TEST, &glContext()->m_stencilBufferEnableAttributes.m_stencilTest
          , glContext()->m_previousAttributes.m_stencilTest );
}

//! Cached version of GLEnable( [GL_TEXTURE_1D|GL_TEXTURE_2D|GL_TEXTURE_3D|GL_TEXTURE_CUBE_MAP|GL_TEXTURE_RECTANGLE_NV]
//!                             [GL_TEXTURE_2D_ARRAY | GL_TEXTURE_1D_ARRAY])
/** This function caches the texture target type per texture unit. It is faster to call this method instead of calling 
  * GL[Disable|Enable]( [GL_TEXTURE_1D|GL_TEXTURE_2D|GL_TEXTURE_3D|GL_TEXTURE_CUBE_MAP|GL_TEXTURE_RECTANGLE_NV] ) directly. */
inline void GLEnableTexture( GLenum target )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_textureEnables[ctx->m_textureAttributes.m_toActivate].m_texture[target] = 
        ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_toActivate].m_texture[target];
# endif
  NVSG_ASSERT(   ( target == GL_TEXTURE_1D ) || ( target == GL_TEXTURE_2D ) || ( target == GL_TEXTURE_3D )
             ||  ( target == GL_TEXTURE_CUBE_MAP ) || ( target == GL_TEXTURE_RECTANGLE_NV ) 
             ||  ( target == GL_TEXTURE_2D_ARRAY_EXT ) || ( target == GL_TEXTURE_1D_ARRAY_EXT ) );
  if ( !ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_toActivate].m_texture[target] )
  {
    _GLUpdateActiveTexture();

    // Texture ARRAY targets are not supported by the fixed function pipeline,
    // and therefore can not be 'ENABLED' in the Gl sense of the term, as it will
    // produce an error.
    if( target != GL_TEXTURE_2D_ARRAY_EXT && target != GL_TEXTURE_1D_ARRAY_EXT )
    {
      NVSG_ASSERT( ! glIsEnabled( target ) );
    }
#endif
    if( target != GL_TEXTURE_2D_ARRAY_EXT && target != GL_TEXTURE_1D_ARRAY_EXT )
    {
      _GLEnable( target );
    }
#if defined( GL_CACHING )
    ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_active].m_texture[target] = GL_TRUE;
  }
#endif
}

/*! \brief Helper function to reset the enable state of texture target \a target.
 *  \param target The texture target for which the enable state has to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the texture
 *  target \a target to it's previous state. Otherwise, it is disabled.
 *  \sa GLDisableTexture, GLEnableTexture, GLDisable, GLEnable */
inline void GLResetTexture( GLenum target )
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  if( ctx->m_previousAttributes.m_textureEnables[ctx->m_textureAttributes.m_toActivate].m_texture[target] )
  {
    GLEnableTexture( target );
  }
  else
  {
    GLDisableTexture( target );
  }
#else
  GLDisableTexture( target );
#endif
}

//! Cached version of GLEnable( [GL_TEXTURE_GEN_S|GL_TEXTURE_GEN_T|GL_TEXTURE_GEN_R|GL_TEXTURE_GEN_Q] )
/** This function caches the texture generation type per texture unit. It is faster to call this method instead of calling 
  * GL[Disable|Enable]( [GL_TEXTURE_GEN_S|GL_TEXTURE_GEN_T|GL_TEXTURE_GEN_R|GL_TEXTURE_GEN_Q] ) directly. */
inline void GLEnableTextureGen( GLenum target )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  NVSG_ASSERT(    ( target == GL_TEXTURE_GEN_S )
              ||  ( target == GL_TEXTURE_GEN_T )
              ||  ( target == GL_TEXTURE_GEN_R )
              ||  ( target == GL_TEXTURE_GEN_Q ) );
  if ( ! ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_toActivate].m_gen[target-GL_TEXTURE_GEN_S] )
  {
    _GLUpdateActiveTexture();
    NVSG_ASSERT( ! glIsEnabled( target ) );
#endif
    _GLEnable( target );
#if defined( GL_CACHING )
    ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_active].m_gen[target-GL_TEXTURE_GEN_S] = GL_TRUE;
  }
#endif
}

//! Replacement of glEnable
/** This function caches on some cap values and asserts on the result before returning. */
inline  void  GLEnable( GLenum cap )
{
#if defined( GL_CACHING )
  switch( cap )
  {
    case GL_ALPHA_TEST :
      GLEnableAlphaTest();
      break;
    case GL_AUTO_NORMAL :
      GLEnableAutoNormal();
      break;
    case GL_BLEND :
      GLEnableBlend();
      break;
    case GL_CLIP_PLANE0 :
    case GL_CLIP_PLANE1 :
    case GL_CLIP_PLANE2 :
    case GL_CLIP_PLANE3 :
    case GL_CLIP_PLANE4 :
    case GL_CLIP_PLANE5 :
      GLEnableClipPlane( cap );
      break;
    case GL_COLOR_LOGIC_OP :
      GLEnableColorLogicOp();
      break;
    case GL_COLOR_MATERIAL :
      GLEnableColorMaterial();
      break;
    case GL_CULL_FACE :
      GLEnableCullFace();
      break;
    case GL_DEPTH_TEST :
      GLEnableDepthTest();
      break;
    case GL_DITHER :
      GLEnableDither();
      break;
    case GL_FOG :
      GLEnableFog();
      break;
    case GL_LIGHT0 :
    case GL_LIGHT1 :
    case GL_LIGHT2 :
    case GL_LIGHT3 :
    case GL_LIGHT4 :
    case GL_LIGHT5 :
    case GL_LIGHT6 :
    case GL_LIGHT7 :
      GLEnableLight( cap );
      break;
    case GL_LIGHTING :
      GLEnableLighting();
      break;
    case GL_LINE_SMOOTH :
      GLEnableLineSmooth();
      break;
    case GL_LINE_STIPPLE :
      GLEnableLineStipple();
      break;
    case GL_LOGIC_OP :
      GLEnableLogicOp();
      break;
    case GL_NORMALIZE :
      GLEnableNormalize();
      break;
    case GL_RESCALE_NORMAL :
      GLEnableRescaleNormal();
      break;
    case GL_POINT_SMOOTH :
      GLEnablePointSmooth();
      break;
    case GL_POLYGON_OFFSET_FILL :
      GLEnablePolygonOffsetFill();
      break;
    case GL_POLYGON_OFFSET_LINE :
      GLEnablePolygonOffsetLine();
      break;
    case GL_POLYGON_OFFSET_POINT :
      GLEnablePolygonOffsetPoint();
      break;
    case GL_POLYGON_SMOOTH :
      GLEnablePolygonSmooth();
      break;
    case GL_POLYGON_STIPPLE :
      GLEnablePolygonStipple();
      break;
    case GL_SCISSOR_TEST :
      GLEnableScissorTest();
      break;
    case GL_STENCIL_TEST :
      GLEnableStencilTest();
      break;
    case GL_TEXTURE_1D :
    case GL_TEXTURE_2D :
    case GL_TEXTURE_3D :
    case GL_TEXTURE_CUBE_MAP :
    case GL_TEXTURE_RECTANGLE_NV :
    case GL_TEXTURE_2D_ARRAY_EXT:
    case GL_TEXTURE_1D_ARRAY_EXT:
      GLEnableTexture( cap );
      break;
    case GL_TEXTURE_GEN_S :
    case GL_TEXTURE_GEN_T :
    case GL_TEXTURE_GEN_R :
    case GL_TEXTURE_GEN_Q :
      GLEnableTextureGen( cap );
      break;
    default :
      _GLEnable( cap );
      break;
  }
#else
  switch( cap )
  {
    case GL_CULL_FACE :
      GLEnableCullFace();
      break;
    default :
      _GLEnable( cap );
      break;
  }
#endif
}

#if !defined(DOXYGEN_IGNORE)
#if defined( GL_CACHING )
inline void _GLEnableClientStateCached( GLenum cap, GLboolean * cache )
{
  if ( !*cache )
  {
    NVSG_ASSERT( !glIsEnabled( cap ) );
    _GLEnableClientState( cap );
    *cache = GL_TRUE;
  }
}
#define GL_ENABLE_CLIENT_STATE( cap, cache )  _GLEnableClientStateCached( cap, cache )
#else
#define GL_ENABLE_CLIENT_STATE( cap, cache )  _GLEnableClientState( cap );
#endif
#endif // DOXYGEN_IGNORE

//! Cached version of GLEnableClientState( GL_COLOR_ARRAY )
/** This function caches the GL_COLOR_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_COLOR_ARRAY ) directly. */
inline  void  GLEnableColorArray( void )
{
  GL_ENABLE_CLIENT_STATE( GL_COLOR_ARRAY, &glContext()->m_vertexArrayAttributes.m_colorArray );
}

//! Cached version of GLEnableClientState( GL_FOG_COORDINATE_ARRAY )
/** This function caches the GL_FOG_COORDINATE_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_FOG_COORDINATE_ARRAY ) directly. */
inline  void  GLEnableFogCoordinateArray( void )
{
  GL_ENABLE_CLIENT_STATE( GL_FOG_COORDINATE_ARRAY, &glContext()->m_vertexArrayAttributes.m_fogCoordArray );
}

//! Cached version of GLEnableClientState( GL_NORMAL_ARRAY )
/** This function caches the GL_NORMAL_ARRAY client state.
  * PIt is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_NORMAL_ARRAY ) directly. */
inline  void  GLEnableNormalArray( void )
{
  GL_ENABLE_CLIENT_STATE( GL_NORMAL_ARRAY, &glContext()->m_vertexArrayAttributes.m_normalArray );
}

//! Cached version of GLEnableClientState( GL_SECONDARY_COLOR_ARRAY )
/** This function caches the GL_SECONDARY_COLOR_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_SECONDARY_COLOR_ARRAY ) directly. */
inline  void  GLEnableSecondaryColorArray( void )
{
  GL_ENABLE_CLIENT_STATE( GL_SECONDARY_COLOR_ARRAY, &glContext()->m_vertexArrayAttributes.m_secondaryColorArray );
}

//! Cached version of GLEnableClientState( GL_TEXTURE_COORD_ARRAY )
/** This function caches the GL_TEXTURE_COORD_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_TEXTURE_COORD_ARRAY ) directly. */
inline  void  GLEnableTextureCoordArray( void )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  if ( ! ctx->m_vertexArrayAttributes.m_textureCoordArray[ctx->m_vertexArrayAttributes.m_clientTextureToActivate] )
  {
    _GLUpdateClientActiveTexture();
    NVSG_ASSERT( ! glIsEnabled( GL_TEXTURE_COORD_ARRAY ) );
#endif
    _GLEnableClientState( GL_TEXTURE_COORD_ARRAY );
#if defined( GL_CACHING )
    ctx->m_vertexArrayAttributes.m_textureCoordArray[ctx->m_vertexArrayAttributes.m_clientTextureActive] = GL_TRUE;
  }
#endif
}

//! Cached version of GLEnableClientState( GL_VERTEX_ARRAY )
/** This function caches the GL_VERTEX_ARRAY client state.
  * It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_VERTEX_ARRAY ) directly. */
inline  void  GLEnableVertexArray( void )
{
  GL_ENABLE_CLIENT_STATE( GL_VERTEX_ARRAY, &glContext()->m_vertexArrayAttributes.m_vertexArray );
}

//! Cached version of GLEnableClientState( GL_PRIMITIVE_RESTART_NV )
/** This function caches the GL_PRIMITIVE_RESTART_NV client state.
* It is faster to call this method instead of calling GL[Disable|Enable]ClientState( GL_PRIMITIVE_RESTART_NV ) directly. */
inline void GLEnablePrimitiveRestart()
{
  GL_ENABLE_CLIENT_STATE( GL_PRIMITIVE_RESTART_NV, &glContext()->m_vertexArrayAttributes.m_primitiveRestart );
}

inline void GLEnableVertexAttribArrayUnified()
{
  GL_ENABLE_CLIENT_STATE( GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV, &glContext()->m_vertexArrayAttributes.m_vertexAttribArrayUnified );
}

inline void GLDisableVertexAttribArrayUnified()
{
  GL_DISABLE_CLIENT_STATE( GL_VERTEX_ATTRIB_ARRAY_UNIFIED_NV, &glContext()->m_vertexArrayAttributes.m_vertexAttribArrayUnified );
}

inline void GLEnableElementArrayUnified()
{
  GL_ENABLE_CLIENT_STATE( GL_ELEMENT_ARRAY_UNIFIED_NV, &glContext()->m_vertexArrayAttributes.m_elementArrayUnified );
}

inline void GLDisableElementArrayUnified()
{
  GL_DISABLE_CLIENT_STATE( GL_ELEMENT_ARRAY_UNIFIED_NV, &glContext()->m_vertexArrayAttributes.m_elementArrayUnified );
}

//! Replacement of glEnableClientState
/** This function caches on some cap values and asserts on the result before returning. */
inline  void  GLEnableClientState( GLenum array )
{
#if defined( GL_CACHING )
  switch( array )
  {
    case GL_COLOR_ARRAY :
      GLEnableColorArray();
      break;
    case GL_FOG_COORDINATE_ARRAY :
      GLEnableFogCoordinateArray();
      break;
    case GL_NORMAL_ARRAY :
      GLEnableNormalArray();
      break;
    case GL_SECONDARY_COLOR_ARRAY :
      GLEnableSecondaryColorArray();
      break;
    case GL_TEXTURE_COORD_ARRAY :
      GLEnableTextureCoordArray();
      break;
    case GL_VERTEX_ARRAY :
      GLEnableVertexArray();
      break;
    case GL_PRIMITIVE_RESTART_NV :
      GLEnablePrimitiveRestart();
      break;
    default :
      _GLEnableClientState( array );
      break;
  }
#else
  _GLEnableClientState( array );
#endif
}

//! Replacement of glEnableVertexAttribArray
inline void GLEnableVertexAttribArray( GLuint index )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLContext * ctx = glContext();
#if defined(GL_CACHING)
  NVSG_ASSERT(index<(GLuint)ctx->m_maxVertexAttribs);
  if ( ctx->m_vertexArrayAttributes.m_vertexAttribArray[index] != GL_TRUE )
#endif
  {
    ctx->m_glFunctions->glEnableVertexAttribArray(index);
#if defined(GL_CACHING)
    ctx->m_vertexArrayAttributes.m_vertexAttribArray[index] = GL_TRUE;
#endif
    ASSERT_GL_NO_ERROR;
  }
}

//! Replacement of glEnd
/** This function asserts on the result before returning.  */
inline  void  GLEnd( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEnd();
#if !defined(NDEBUG)
  glContext()->m_inBeginEndLoop = false;
#endif
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEndList
/** This function does not assert on the result before returning, because on glEndList the GL_OUT_OF_MEMORY is to be tested.  */
inline  void  GLEndList( void )
{
  NVSG_GL_STATS_END_LIST();
  glEndList();
  // NOTE: no assertion on GL_NO_ERROR here, because on glEndList the GL_OUT_OF_MEMORY is to be tested!
}

//! Replacement of glEndConditionalRenderNV
inline  void  GLEndConditionalRenderNV()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glEndConditionalRenderNV();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEndQuery
/** This function asserts on the result before returning. */
inline  void  GLEndQuery( GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glEndQuery( target );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEndTransformFeedbackNV
/** This function asserts on the result before returning. */
inline  void  GLEndTransformFeedbackNV()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glEndTransformFeedbackNV();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEndTransformFeedback
/** This function asserts on the result before returning. */
inline  void  GLEndTransformFeedback()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glEndTransformFeedback();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEvalCoord1d
/** This function asserts on the result before returning.  */
inline  void  GLEvalCoord1d( GLdouble u )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalCoord1d( u );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glEvalCoord1f
/** This function asserts on the result before returning.  */
inline  void  GLEvalCoord1f( GLfloat u )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalCoord1f( u );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glEvalCoord2d
/** This function asserts on the result before returning.  */
inline  void  GLEvalCoord2d( GLdouble u, GLdouble v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalCoord2d( u, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glEvalCoord2f
/** This function asserts on the result before returning.  */
inline  void  GLEvalCoord2f( GLfloat u, GLfloat v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalCoord2f( u, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glEvalMesh1
/** This function asserts on the result before returning.  */
inline  void  GLEvalMesh1( GLenum mode, GLint i1, GLint i2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalMesh1( mode, i1, i2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEvalMesh2
/** This function asserts on the result before returning.  */
inline  void  GLEvalMesh2( GLenum mode, GLint i1, GLint i2, GLint j1, GLint j2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalMesh2( mode, i1, i2, j1, j2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glEvalPoint1
/** This function asserts on the result before returning.  */
inline  void  GLEvalPoint1( GLint i )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalPoint1( i );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glEvalPoint2
/** This function asserts on the result before returning.  */
inline  void  GLEvalPoint2( GLint i, GLint j )
{
  NVSG_GL_STATS_FCT_COUNT();
  glEvalPoint2( i, j );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glExecuteProgramNV
/** This function asserts on the result before returning.  */
inline  void  GLExecuteProgram( GLenum target, GLuint id, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glExecuteProgramNV( target, id, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFeedbackBuffer
/** This function asserts on the result before returning.  */
inline  void  GLFeedbackBuffer( GLsizei size, GLenum type, GLfloat *buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  glFeedbackBuffer( size, type, buffer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFinalCombinerInput
/** This function asserts on the result before returning.  */
inline  void  GLFinalCombinerInput( GLenum variable, GLenum input, GLenum mapping, GLenum componentUsage )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFinalCombinerInput( variable, input, mapping, componentUsage );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFinish
/** This function asserts on the result before returning.  */
inline  void  GLFinish( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  glFinish();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFinishFenceNV
inline  void  GLFinishFence( GLuint fence )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFinishFence( fence );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFlush
/** This function asserts on the result before returning.  */
inline  void  GLFlush( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  glFlush();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFlushPixelDataRangeNV
/** This functions asserts on the result before returning. */
inline  void  GLFlushPixelDataRange( GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFlushPixelDataRange( target );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFlushVertexArrayRange
/** This functions asserts on the result before returning. */
inline  void  GLFlushVertexArrayRange()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFlushVertexArrayRange();
  ASSERT_GL_NO_ERROR;
}

#if !defined(DOXYGEN_IGNORE)
#if defined( GL_CACHING )
inline void _GLFogfCached( GLenum pname, GLfloat param, GLfloat * cache )
{
  if ( param != *cache )
  {
    _GLFogf( pname, param );
    *cache = param;
  }
}
inline void _GLFogfvCached( GLenum pname, const GLfloat * params, GLfloat * cache )
{
  if ( !equal4fv( params, cache ) )
  {
    _GLFogfv( pname, params );
    memcpy( cache, params, 4*sizeof(GLfloat) );
  }
}
inline void _GLFogiCached( GLenum pname, GLint param, GLint * cache )
{
  if ( param != *cache )
  {
    _GLFogi( pname, param );
    *cache = param;
  }
}
# if defined(GL_SUPPORT_RESET)
inline void _GLFogfCached( GLenum pname, GLfloat param, GLfloat * cache, GLfloat * prev )
{
  *prev = *cache;
  _GLFogfCached( pname, param, cache );
}
inline void _GLFogfvCached( GLenum pname, const GLfloat * params, GLfloat * cache, GLfloat * prev )
{
  memcpy( prev, cache, 4*sizeof(GLfloat) );
  _GLFogfvCached( pname, params, cache );
}
inline void _GLFogiCached( GLenum pname, GLint param, GLint * cache, GLint * prev )
{
  *prev = *cache;
  _GLFogiCached( pname, param, cache );
}
#  define GL_FOG_F( pname, param, cache, prev )   _GLFogfCached( pname, param, cache )
#  define GL_FOG_FV( pname, params, cache, prev ) _GLFogfvCached( pname, params, cache )
#  define GL_FOG_I( pname, param, cache, prev )   _GLFogiCached( pname, param, cache )
# else
#  define GL_FOG_F( pname, param, cache, prev )   _GLFogfCached( pname, param, cache )
#  define GL_FOG_FV( pname, params, cache, prev ) _GLFogfvCached( pname, params, cache )
#  define GL_FOG_I( pname, param, cache, prev )   _GLFogiCached( pname, param, cache )
# endif
#else
# define GL_FOG_F( pname, param, cache, prev )    _GLFogf( pname, param )
# define GL_FOG_FV( pname, params, cache, prev )  _GLFogfv( pname, params )
# define GL_FOG_I( pname, param, cache, prev )    _GLFogi( pname, param )
#endif
#endif // DOXYGEN_IGNORE

//! Cached version of GLFog[fv|iv]( GL_FOG_COLOR, params )
/** This function caches the GL_FOG_COLOR value.
  * It is faster to call this method instead of GLFog[fv|iv]( GL_FOG_COLOR, params ) directly. */
inline  void  GLFogColor( const GLfloat * color )
{
  GL_FOG_FV( GL_FOG_COLOR, color, &glContext()->m_fogAttributes.m_color[0]
           , &glContext()->m_previousAttributes.m_fogColor[0] );
}

/*! \brief Helper function to reset the fog color
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the fog color
 *  to it's previous state. Otherwise, it is set to (0,0,0,0).
 *  \sa GLFogColor */
inline void GLResetFogColor()
{
#if defined(GL_SUPPORT_RESET)
  GLFogColor( &glContext()->m_previousAttributes.m_fogColor[0] );
#else
  static nvmath::Vec4f fc(0.0f,0.0f,0.0f,0.0f);
  GLFogColor( &fc[0] );
#endif
}

//! Replacement of glFogCoordd
/** This function asserts on the result before returning.  */
inline  void  GLFogCoordd( GLdouble fog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFogCoordd( fog );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glFogCoorddv
/** This function asserts on the result before returning.  */
inline  void  GLFogCoorddv( const GLdouble *fog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFogCoorddv( fog );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glFogCoordf
/** This function asserts on the result before returning.  */
inline  void  GLFogCoordf( GLfloat fog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFogCoordf( fog );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glFogCoordfv
/** This function asserts on the result before returning.  */
inline  void  GLFogCoordfv( const GLfloat *fog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFogCoordfv( fog );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glFogCoordh
/** This function asserts on the result before returning.  */
inline  void  GLFogCoordh( GLhalf fog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFogCoordh( fog );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glFogCoordhv
/** This function asserts on the result before returning.  */
inline  void  GLFogCoordhv( const GLhalf *fog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFogCoordhv( fog );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glFogCoordPointer
/** This function caches the fog coordinate pointer setting and asserts on the result before returning.  */
inline  void  GLFogCoordPointer( GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  glContext()->m_glFunctions->glFogCoordPointer( type, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of GLFog[f|fv|i|iv]( GL_FOG_DENSITY, param[s] )
/** This function caches the GL_FOG_DENSITY value.
  * It is faster to call this method instead of GLFog[f|fv|i|iv]( GL_FOG_MODE, param[s] ) directly. */
inline  void  GLFogDensity( GLfloat density )
{
  GL_FOG_F( GL_FOG_DENSITY, density, &glContext()->m_fogAttributes.m_density
          , &glContext()->m_previousAttributes.m_fogDensity );
}

/*! \brief Helper function to reset the fog density.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the fog density
 *  to it's previous state. Otherwise, it is set to 1.0f.
 *  \sa GLFogDensity */
inline void GLResetFogDensity()
{
#if defined(GL_SUPPORT_RESET)
  GLFogDensity( glContext()->m_previousAttributes.m_fogDensity );
#else
  GLFogDensity( 1.0f );
#endif
}

//! Cached version of GLFog[f|fv|i|iv]( GL_FOG_END, param[s] )
/** This function caches the GL_FOG_END value.
  * It is faster to call this method instead of GLFog[f|fv|i|iv]( GL_FOG_END, param[s] ) directly. */
inline  void  GLFogEnd( GLfloat end )
{
  GL_FOG_F( GL_FOG_END, end, &glContext()->m_fogAttributes.m_end
          , &glContext()->m_previousAttributes.m_fogEnd );
}

/*! \brief Helper function to reset the fog end value.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the fog end
 *  value to it's previous state. Otherwise, it is set to 1.0f.
 *  \sa GLFogDensity */
inline void GLResetFogEnd()
{
#if defined(GL_SUPPORT_RESET)
  GLFogEnd( glContext()->m_previousAttributes.m_fogEnd );
#else
  GLFogEnd( 1.0f );
#endif
}

//! Replacement of glFogf
/** This function asserts on the result before returning.  */
inline  void  GLFogf( GLenum pname, GLfloat param ) 
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_FOG_DENSITY :
      GLFogDensity( param );
      break;
    case GL_FOG_END :
      GLFogEnd( param );
      break;
    case GL_FOG_MODE :
      GLFogMode( (GLenum)param );
      break;
    case GL_FOG_START :
      GLFogStart( param );
      break;
    default :
      _GLFogf( pname, param );
      break;
  }
#else
  _GLFogf( pname, param );
#endif
}

//! Replacement of glFogfv
/** This function asserts on the result before returning.  */
inline  void  GLFogfv( GLenum pname, const GLfloat *params )  
{ 
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_FOG_COLOR :
      GLFogColor( params );
      break;
    case GL_FOG_DENSITY :
      GLFogDensity( *params );
      break;
    case GL_FOG_END :
      GLFogEnd( *params );
      break;
    case GL_FOG_MODE :
      GLFogMode( (GLenum)*params );
      break;
    case GL_FOG_START :
      GLFogStart( *params );
      break;
    default :
      _GLFogfv( pname, params );
      break;
  }
#else
  _GLFogfv( pname, params );
#endif
}

//! Replacement of glFogi
/** This function asserts on the result before returning.  */
inline  void  GLFogi( GLenum pname, GLint param ) 
{ 
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_FOG_DENSITY :
      GLFogDensity( (GLfloat)param );
      break;
    case GL_FOG_END :
      GLFogEnd( (GLfloat)param );
      break;
    case GL_FOG_MODE :
      GLFogMode( param );
      break;
    case GL_FOG_START :
      GLFogStart( (GLfloat)param );
      break;
    default :
      _GLFogi( pname, param );
      break;
  }
#else
  _GLFogi( pname, param );
#endif
}

//! Replacement of glFogiv
/** This function asserts on the result before returning.  */
inline  void  GLFogiv( GLenum pname, const GLint *params ) 
{ 
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_FOG_COLOR :
      {
        GLfloat fParams[4];
        for ( size_t i=0 ; i<4 ; i++ )
        {
          fParams[i] = (GLfloat)params[i];
        }
        GLFogColor( fParams );
      }
      break;
    case GL_FOG_DENSITY :
      GLFogDensity( (GLfloat)*params );
      break;
    case GL_FOG_END :
      GLFogEnd( (GLfloat)*params );
      break;
    case GL_FOG_MODE :
      GLFogMode( *params );
      break;
    case GL_FOG_START :
      GLFogStart( (GLfloat)*params );
      break;
    default :
      _GLFogiv( pname, params );
      break;
  }
#else
  _GLFogiv( pname, params );
#endif
}

//! Cached version of GLFog[f|fv|i|iv]( GL_FOG_MODE, param[s] )
/** This function caches the GL_FOG_MODE value.
  * It is faster to call this method instead of GLFog[f|fv|i|iv]( GL_FOG_MODE, param[s] ) directly. */
inline  void  GLFogMode( GLenum mode )
{
  GL_FOG_I( GL_FOG_MODE, mode, (GLint*)&glContext()->m_fogAttributes.m_mode
          , (GLint*) &glContext()->m_previousAttributes.m_fogMode );
}

/*! \brief Helper function to reset the fog mode.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the fog mode
 *  to it's previous state. Otherwise, it is set to GL_EXP.
 *  \sa GLFogMode */
inline void GLResetFogMode()
{
#if defined(GL_SUPPORT_RESET)
  GLFogMode( glContext()->m_previousAttributes.m_fogMode );
#else
  GLFogMode( GL_EXP );
#endif
}

//! Cached version of GLFog[f|fv|i|iv]( GL_FOG_START, param[s] )
/** This function caches the GL_FOG_START value.
  * It is faster to call this method instead of GLFog[f|fv|i|iv]( GL_FOG_START, param[s] ) directly. */
inline  void  GLFogStart( GLfloat start )
{
  GL_FOG_F( GL_FOG_START, start, &glContext()->m_fogAttributes.m_start
          , glContext()->m_previousAttributes.m_fogStart );
}

/*! \brief Helper function to reset the fog start value.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the fog start
 *  value to it's previous state. Otherwise, it is set to 0.0f.
 *  \sa GLFogStart */
inline void GLResetFogStart()
{
#if defined(GL_SUPPORT_RESET)
  GLFogStart( glContext()->m_previousAttributes.m_fogStart );
#else
  GLFogStart( 0.0f );
#endif
}

//! Replacement of glFramebufferTextureEXT
/** This function asserts on the result before returning. */
inline  void  GLFramebufferTexture( GLenum target, GLenum attachment, GLuint texture, GLint level )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFramebufferTexture( target, attachment, texture, level );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFramebufferTextureFaceEXT
/** This function asserts on the result before returning. */
inline  void  GLFramebufferTextureFace( GLenum target, GLenum attachment, GLuint texture, GLint level, GLenum face )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFramebufferTextureFace( target, attachment, texture, level, face );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glFrontFace
/** This function caches the front face setting and asserts on the result before returning.  */
inline  void  GLFrontFace( GLenum mode )
{
#if defined(GL_CACHING)
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_frontFaceMode = ctx->m_polygonAttributes.m_frontFaceMode;
# endif
  if ( mode != ctx->m_polygonAttributes.m_frontFaceMode )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glFrontFace( mode );
    ASSERT_GL_NO_ERROR;
#if defined(GL_CACHING)
    ctx->m_polygonAttributes.m_frontFaceMode = mode;
#endif
  }
}

/*! \brief Helper function to reset the front face state.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the front face
 *  state to it's previous state. Otherwise, it is set to GL_CCW.
 *  \sa GLFrontFace */
inline void GLResetFrontFace()
{
#if defined(GL_SUPPORT_RESET)
  GLFrontFace( glContext()->m_previousAttributes.m_frontFaceMode );
#else
  GLFrontFace(GL_CCW);
#endif
}

//! Replacement of glFrustum
/** This function asserts on the result before returning.  */
inline  void  GLFrustum( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
  NVSG_GL_STATS_FRUSTUM( left, right, bottom, top, zNear, zFar );
  glFrustum( left, right, bottom, top, zNear, zFar );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGenLists
/** This function asserts on the result and updates some caches. */
inline  GLuint  GLGenLists( GLsizei range )
{
  NVSG_GL_STATS_GEN_LISTS( range );
  GLuint  firstList = glGenLists( range );
  glContext()->addLists(range, firstList);
  ASSERT_GL_NO_ERROR;
  return( firstList );
}

//! Replacement of glGenProgramsARB
/** This function hides the extension character of glGenProgramsARB and asserts on the result, and updates some caches.  */
inline  void  GLGenPrograms( GLsizei n, GLuint *ids )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext();
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glGenProgramsARB );
  ctx->m_glFunctions->glGenProgramsARB( n, ids );
  ctx->addPrograms(n, ids);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGenQueries
/** This function asserts on the result, and updates some caches. */
inline  void  GLGenQueries( GLsizei n, GLuint *ids )
{
  NVSG_GL_STATS_FCT_COUNT();
  GLContext * ctx = glContext();
  NVSG_ASSERT( ctx->m_glFunctions && ctx->m_glFunctions->glGenQueries );
  ctx->m_glFunctions->glGenQueries( n, ids );
  ctx->addQueries( n, ids );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGenTextures
/** This function asserts on the result and updates some caches. */
inline  void  GLGenTextures( GLsizei n, GLuint *textures )
{
  NVSG_GL_STATS_GEN_TEXTURES( n, textures );
  glGenTextures( n, textures );
  glContext()->addTextures(n, textures);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetActiveAttrib
/** This function asserts on the result before returning.  */
inline  void  GLGetActiveAttrib( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetActiveAttrib( program, index, bufSize, length, size, type, name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetActiveUniform
/** This function asserts on the result before returning.  */
inline  void  GLGetActiveUniform( GLuint program, GLuint index, GLsizei maxLength, GLsizei *length, GLsizei *size, GLenum *type, GLcharARB *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetActiveUniform( program, index, maxLength, length, size, type, name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetActiveVaryingNV
/** This function asserts on the result before returning.  */
inline  void  GLGetActiveVaryingNV( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetActiveVaryingNV( program, index, bufSize, length, size, type, name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetAttachedObjectsARB
/** This function asserts on the result before returning.  */
inline  void  GLGetAttachedObjects( GLhandleARB containerObj, GLsizei maxCount, GLsizei *count, GLhandleARB *obj )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetAttachedObjectsARB( containerObj, maxCount, count, obj );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetAttachedShaders
/** This function asserts on the result before returning.  */
inline  void  GLGetAttachedShaders( GLuint program, GLsizei maxCount, GLsizei *count, GLuint *shaders )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetAttachedShaders( program, maxCount, count, shaders );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetAttribLocation
/** This function asserts on the result before returning.  */
inline  GLint GLGetAttribLocation( GLuint program, const GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLint loc = glContext()->m_glFunctions->glGetAttribLocation( program, name );
  ASSERT_GL_NO_ERROR;
  return( loc );
}

//! Replacement of glGetBooleanv
/** This function asserts on the result before returning.  */
inline  void  GLGetBooleanv( GLenum pname, GLboolean *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetBooleanv( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetBooleanIndexedvEXT
/** This function asserts on the result before returning.  */
inline  void  GLGetBooleanIndexedv( GLenum target, GLuint index, GLboolean *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetBooleanIndexedv( target, index, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetColorTable
/** This function asserts on the result before returning. */
inline  void  GLGetColorTable( GLenum target, GLenum format, GLenum type, GLvoid *table )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetColorTable( target, format, type, table );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetColorTableParameterfv
/** This function asserts on the result before returning. */
inline void GLGetColorTableParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetColorTableParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetColorTableParameteriv
/** This function asserts on the result before returning. */
inline void GLGetColorTableParameteriv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetColorTableParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetCombinerInputParameterfv
/** This function asserts on the result before returning. */
inline void GLGetCombinerInputParameterfv( GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetCombinerInputParameterfv( stage, portion, variable, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetCombinerInputParameteriv
/** This function asserts on the result before returning. */
inline void GLGetCombinerInputParameteriv( GLenum stage, GLenum portion, GLenum variable, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetCombinerInputParameteriv( stage, portion, variable, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetCombinerOutputParameterfv
/** This function asserts on the result before returning. */
inline void GLGetCombinerOutputParameterfv( GLenum stage, GLenum portion, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetCombinerOutputParameterfv( stage, portion, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetCombinerOutputParameteriv
/** This function asserts on the result before returning. */
inline void GLGetCombinerOutputParameteriv( GLenum stage, GLenum portion, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetCombinerOutputParameteriv( stage, portion, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetCombinerStageParameterfv
/** This function asserts on the result before returning. */
inline void GLGetCombinerStageParameterfv( GLenum stage, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetCombinerStageParameterfv( stage, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetCompressedTexImage
/** This function asserts on the result before returning. */
inline void GLGetCompressedTexImage( GLenum target, GLint lod, GLvoid *img )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetCompressedTexImage( target, lod, img );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetConvolutionFilter
/** This function asserts on the result before returning. */
inline void GLGetConvolutionFilter( GLenum target, GLenum format, GLenum type, GLvoid *image )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetConvolutionFilter( target, format, type, image );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetConvolutionParameterfv
/** This function asserts on the result before returning. */
inline void GLGetConvolutionParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetConvolutionParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetConvolutionParameteriv
/** This function asserts on the result before returning. */
inline void GLGetConvolutionParameteriv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetConvolutionParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetDoublev
/** This function asserts on the result before returning.  */
inline  void  GLGetDoublev( GLenum pname, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetDoublev( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetFenceivNV
/** This function asserts on the result before returning.  */
inline  void  GLGetFenceiv( GLuint fence, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetFenceiv( fence, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetFinalCombinerInputParamterfv
/** This function asserts on the result before returning.  */
inline  void  GLGetFinalCombinerInputParameterfv( GLenum variable, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetFinalCombinerInputParameterfv( variable, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetFinalCombinerInputParamteriv
/** This function asserts on the result before returning.  */
inline  void  GLGetFinalCombinerInputParameteriv( GLenum variable, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetFinalCombinerInputParameteriv( variable, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetFloatv
/** This function asserts on the result before returning.  */
inline  void  GLGetFloatv( GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetFloatv( pname, params );
  ASSERT_GL_NO_ERROR;
}

inline  void  GLGetFogColor( GLfloat * fogColor )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext() );
#if defined(GL_CACHING)
# if !defined(NDEBUG)
  GLfloat params[4];
  glGetFloatv( GL_FOG_COLOR, params );
  ASSERT_GL_NO_ERROR;
  NVSG_ASSERT( memcmp( params, &glContext()->m_fogAttributes.m_color[0], 4*sizeof(GLfloat) ) == 0 );
# endif
  memcpy( fogColor, &glContext()->m_fogAttributes.m_color[0], 4*sizeof(GLfloat) );
#else
  glGetFloatv( GL_FOG_COLOR, fogColor );
  ASSERT_GL_NO_ERROR;
#endif
}

//! Replacement of glGetFragDataLocationEXT
/** This function asserts on the result before returning.  */
inline  GLint GLGetFragDataLocation( GLuint program, const GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLint loc = glContext()->m_glFunctions->glGetFragDataLocation( program, name );
  ASSERT_GL_NO_ERROR;
  return( loc );
}

//! Replacement of glGetHandleARB
/** This function asserts on the result before returning.  */
inline  GLhandleARB GLGetHandle( GLenum pname )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLhandleARB hdl = glContext()->m_glFunctions->glGetHandleARB( pname );
  ASSERT_GL_NO_ERROR;
  return( hdl );
}

//! Replacement of glGetHistogram
/** This function asserts on the result before returning.  */
inline  void  GLGetHistogram( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values )
{
  NVSG_GL_STATS_GET_INTEGERV( pname, params );
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetHistogram( target, reset, format, type, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetHistogramParameterfv
/** This function asserts on the result before returning.  */
inline  void  GLGetHistogramParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_GET_INTEGERV( pname, params );
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetHistogramParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetHistogramParameteriv
/** This function asserts on the result before returning.  */
inline  void  GLGetHistogramParameteriv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_GET_INTEGERV( pname, params );
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetHistogramParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetInfoLogARB
/** This function asserts on the result before returning.  */
inline  void  GLGetInfoLog( GLhandleARB obj, GLsizei maxLength, GLsizei *length, GLcharARB *infoLog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetInfoLogARB( obj, maxLength, length, infoLog );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetIntegerv
/** This function asserts on the result before returning.  */
inline  void  GLGetIntegerv( GLenum pname, GLint *params )
{
  NVSG_GL_STATS_GET_INTEGERV( pname, params );
  glGetIntegerv( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetIntegerIndexedvEXT
/** This function asserts on the result before returning.  */
inline  void  GLGetIntegerIndexedv( GLenum target, GLuint index, GLint *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetIntegerIndexedv( target, index, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetBufferParameteriv
/** This function asserts on the result before returning.  */
inline  void  GLGetBufferParameteriv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetBufferParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetBufferPointerv
/** This function asserts on the result before returning.  */
inline  void  GLGetBufferPointerv( GLenum target, GLenum pname, GLvoid* *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetBufferPointerv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetBufferSubData
/** This function asserts on the result before returning.  */
inline  void  GLGetBufferSubData( GLenum target, GLintptrARB offset, GLsizeiptrARB size, GLvoid *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetBufferSubData( target, offset, size, data );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetClipPlane
/** This function asserts on the result before returning.  */
inline  void  GLGetClipPlane( GLenum plane, GLdouble *equation )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetClipPlane( plane, equation );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetLightfv
/** This function asserts on the result before returning.  */
inline  void  GLGetLightfv( GLenum light, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetLightfv( light, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetLightiv
/** This function asserts on the result before returning.  */
inline  void  GLGetLightiv( GLenum light, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetLightiv( light, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetMapdv
/** This function asserts on the result before returning.  */
inline  void  GLGetMapdv( GLenum target, GLenum query, GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetMapdv( target, query, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetMapfv
/** This function asserts on the result before returning.  */
inline  void  GLGetMapfv( GLenum target, GLenum query, GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetMapfv( target, query, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetMapiv
/** This function asserts on the result before returning.  */
inline  void  GLGetMapiv( GLenum target, GLenum query, GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetMapiv( target, query, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetMaterialfv
/** This function asserts on the result before returning.  */
inline  void  GLGetMaterialfv( GLenum face, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetMaterialfv( face, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetMaterialiv
/** This function asserts on the result before returning.  */
inline  void  GLGetMaterialiv( GLenum face, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetMaterialiv( face, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Get the maximal number of texture coordinate slots.
inline GLint GLGetMaxTexCoords( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  return( glContext()->m_maxTexCoords );
}

//! Get the maximal number of texture units.
inline GLint GLGetMaxTexUnits( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  return( glContext()->m_maxTexUnits );
}

//! Get the maximal number of texture image units.
inline GLint GLGetMaxTexImageUnits( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  return glContext()->m_maxTexImageUnits;
}

//! Replacement of glGetMinmax
/** This function asserts on the result before returning.  */
inline  void  GLGetMinmax( GLenum target, GLboolean reset, GLenum format, GLenum type, GLvoid *values )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetMinmax( target, reset, format, type, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetMinmaxParameterfv
/** This function asserts on the result before returning.  */
inline  void  GLGetMinmaxParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetMinmaxParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetMinmaxParameteriv
/** This function asserts on the result before returning.  */
inline  void  GLGetMinmaxParameteriv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetMinmaxParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetObjectParameterfvARB
/** This function asserts on the result before returning.  */
inline  void  GLGetObjectParameterfv( GLhandleARB obj, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetObjectParameterfvARB( obj, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetObjectParameterivARB
/** This function asserts on the result before returning.  */
inline  void  GLGetObjectParameteriv( GLhandleARB obj, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetObjectParameterivARB( obj, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetPixelMapfv
/** This function asserts on the result before returning.  */
inline  void  GLGetPixelMapfv( GLenum map, GLfloat *values )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetPixelMapfv( map, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetPixelMapuiv
/** This function asserts on the result before returning.  */
inline  void  GLGetPixelMapuiv( GLenum map, GLuint *values )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetPixelMapuiv( map, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetPixelMapusv
/** This function asserts on the result before returning.  */
inline  void  GLGetPixelMapusv( GLenum map, GLushort *values )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetPixelMapusv( map, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetPointerv
/** This function asserts on the result before returning.  */
inline  void  GLGetPointerv( GLenum pname, GLvoid* *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetPointerv( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetPolygonStipple
/** This function asserts on the result before returning.  */
inline  void  GLGetPolygonStipple( GLubyte *mask )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetPolygonStipple( mask );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramEnvParameterdvARB
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramEnvParameterdv( GLenum target, GLuint index, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramEnvParameterdvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramEnvParameterfvARB
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramEnvParameterfv( GLenum target, GLuint index, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramEnvParameterfvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramiv
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramiv( GLuint program, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramiv( program, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramLocalParameterdvARB
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramLocalParameterdv( GLenum target, GLuint index, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramLocalParameterdvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramLocalParameterfvARB
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramLocalParameterfv( GLenum target, GLuint index, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramLocalParameterfvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}


//! Replacement of glGetProgramInfoLog
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramInfoLog( GLuint program, GLsizei bufSize, GLsizei *length, GLchar *infoLog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramInfoLog( program, bufSize, length, infoLog );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramEnvParameterIivNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramEnvParameterIiv( GLenum target, GLuint index, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramEnvParameterIiv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramEnvParameterIuivNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramEnvParameterIuiv( GLenum target, GLuint index, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramEnvParameterIuiv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramLocalParameterIivNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramLocalParameterIiv( GLenum target, GLuint index, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramLocalParameterIiv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramLocalParameterIuivNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramLocalParameterIuiv( GLenum target, GLuint index, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramLocalParameterIuiv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramNamedParameterdvNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramNamedParameterdv( GLuint id, GLsizei len, const GLubyte *name, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramNamedParameterdv( id, len, name, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramNamedParameterfvNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramNamedParameterfv( GLuint id, GLsizei len, const GLubyte *name, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramNamedParameterfv( id, len, name, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramParameterdvNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramParameterdv( GLenum target, GLuint index, GLenum pname, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramParameterdvNV( target, index, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramParameterfvNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramParameterfv( GLenum target, GLuint index, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramParameterfvNV( target, index, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetProgramStringNV
/** This function asserts on the result before returning.  */
inline  void  GLGetProgramString( GLuint id, GLenum pname, GLubyte *program )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetProgramStringNV( id, pname, program );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetQueryiv
/** This function asserts on the result before returning. */
inline  void  GLGetQueryiv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetQueryiv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetQueryObjecti64vEXT
/** This function asserts on the result before returning. */
inline  void  GLGetQueryObjecti64v( GLuint id, GLenum pname, GLint64EXT *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetQueryObjecti64v( id, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetQueryObjectiv
/** This function asserts on the result before returning. */
inline  void  GLGetQueryObjectiv( GLuint id, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetQueryObjectiv( id, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetQueryObjectui64vEXT
/** This function asserts on the result before returning. */
inline  void  GLGetQueryObjectui64v( GLuint id, GLenum pname, GLuint64EXT *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetQueryObjectui64v( id, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetQueryObjectuiv
/** This function asserts on the result before returning. */
inline  void  GLGetQueryObjectuiv( GLuint id, GLenum pname, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetQueryObjectuiv( id, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetSeparableFilter
/** This asserts on the result before returning. */
inline  void  GLGetSeparableFilter( GLenum target, GLenum format, GLenum type, GLvoid *row, GLvoid *column, GLvoid *span )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetSeparableFilter( target, format, type, row, column, span );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetShaderiv
/** This asserts on the result before returning. */
inline  void  GLGetShaderiv( GLuint shader, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetShaderiv( shader, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetShaderInfoLog
/** This asserts on the result before returning. */
inline  void  GLGetShaderInfoLog( GLuint shader, GLsizei bufSize, GLsizei *length, GLchar *infoLog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetShaderInfoLog( shader, bufSize, length, infoLog );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetShaderSource
/** This asserts on the result before returning. */
inline  void  GLGetShaderSource( GLuint shader, GLsizei bufSize, GLsizei *length, GLcharARB *source )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetShaderSource( shader, bufSize, length, source );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetString
/** This function asserts on the result before returning.  */
inline  const GLubyte * GLGetString( GLenum name )
{
  NVSG_GL_STATS_FCT_COUNT();
  const GLubyte * ret = glGetString( name );
  ASSERT_GL_NO_ERROR;
  return( ret );
}

//! Replacement of glGetTexEnvfv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexEnvfv( GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexEnvfv( target,pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexEnviv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexEnviv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexEnviv( target,pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexGendv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexGendv( GLenum coord, GLenum pname, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexGendv( coord, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexGenfv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexGenfv( GLenum coord, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexGenfv( coord, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexGeniv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexGeniv( GLenum coord, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexGeniv( coord, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexImage
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLGetTexImage( GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexImage( target, level, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexLevelParameterfv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexLevelParameterfv( GLenum target, GLint level, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexLevelParameterfv( target, level, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexLevelParameteriv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexLevelParameteriv( GLenum target, GLint level, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexLevelParameteriv( target, level, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexParameterfv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexParameterfv( GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexParameteriv
/** This function asserts on the result before returning.  */
inline  void  GLGetTexParameteriv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glGetTexParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexParameterIivEXT
/** This function asserts on the result before returning.  */
inline  void  GLGetTexParameterIiv( GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetTexParameterIiv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTexParameterIuivEXT
/** This function asserts on the result before returning.  */
inline  void  GLGetTexParameterIuiv( GLenum target, GLenum pname, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetTexParameterIuiv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTrackMatrixivNV
/** This function asserts on the result before returning.  */
inline  void  GLGetTrackMatrixiv( GLenum target, GLuint address, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetTrackMatrixivNV( target, address, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTransformFeedbackVaryingNV
/** This function asserts on the result before returning.  */
inline  void  GLGetTransformFeedbackVaryingNV( GLuint program, GLuint index, GLint *location )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetTransformFeedbackVaryingNV( program, index, location );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetTransformFeedbackVarying
/** This function asserts on the result before returning.  */
inline  void  GLGetTransformFeedbackVarying( GLuint program, GLuint index, GLsizei bufSize, GLsizei *length, GLsizei *size, GLenum *type, GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetTransformFeedbackVarying( program, index, bufSize, length, size, type, name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetUniformfv
/** This function asserts on the result before returning.  */
inline  void  GLGetUniformfv( GLuint program, GLint location, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetUniformfv( program, location, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetUniformiv
/** This function asserts on the result before returning.  */
inline  void  GLGetUniformiv( GLuint program, GLint location, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetUniformiv( program, location, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetUniformuiv
/** This function asserts on the result before returning.  */
inline  void  GLGetUniformuiv( GLuint program, GLint location, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetUniformuiv( program, location, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetUniformBufferSizeEXT
/** This function asserts on the result before returning.  */
inline  GLint GLGetUniformBufferSize( GLuint program, GLint location )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLint size = glContext()->m_glFunctions->glGetUniformBufferSize( program, location);
  ASSERT_GL_NO_ERROR;
  return( size );
}

//! Replacement of glGetUniformLocation
/** This function asserts on the result before returning.  */
inline  GLint GLGetUniformLocation( GLuint program, const GLcharARB *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLint loc = glContext()->m_glFunctions->glGetUniformLocation( program, name );
  ASSERT_GL_NO_ERROR;
  return( loc );
}

//! Replacement of glGetUniformOffsetEXT
/** This function asserts on the result before returning.  */
inline  GLintptr GLGetUniformOffset( GLuint program, GLint location )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLintptr offset = glContext()->m_glFunctions->glGetUniformOffset( program, location );
  ASSERT_GL_NO_ERROR;
  return( offset );
}

//! Replacement of glGetVaryingLocationNV
/** This function asserts on the result before returning.  */
inline  GLint GLGetVaryingLocationNV( GLuint program, const GLchar *name )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLint loc = glContext()->m_glFunctions->glGetVaryingLocationNV( program, name );
  ASSERT_GL_NO_ERROR;
  return( loc );
}

//! Replacement of glGetVertexAttribdv
/** This function asserts on the result before returning.  */
inline  void  GLGetVertexAttribdv( GLuint index, GLenum pname, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVertexAttribdv( index, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetVertexAttribfv
/** This function asserts on the result before returning.  */
inline  void  GLGetVertexAttribfv( GLuint index, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVertexAttribfv( index, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetVertexAttribiv
/** This function asserts on the result before returning.  */
inline  void  GLGetVertexAttribiv( GLuint index, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVertexAttribiv( index, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetVertexAttribIivEXT
/** This function asserts on the result before returning.  */
inline  void  GLGetVertexAttribIiv( GLuint index, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVertexAttribIiv( index, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetVertexAttribIuivEXT
/** This function asserts on the result before returning.  */
inline  void  GLGetVertexAttribIuiv( GLuint index, GLenum pname, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVertexAttribIuiv( index, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glGetVertexAttribPointer
/** This function asserts on the result before returning.  */
inline  void  GLGetVertexAttribPointerv( GLuint index, GLenum pname, GLvoid* *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVertexAttribPointerv( index, pname, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glHint
/** This function asserts on the result before returning.  */
inline  void  GLHint( GLenum target, GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  glHint( target, mode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glHistogram
/** This function asserts on the result before returning.  */
inline  void  GLHistogram( GLenum target, GLsizei width, GLenum internalformat, GLboolean sink )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glHistogram( target, width, internalformat, sink );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glIndexd
/** This function asserts on the result before returning.  */
inline  void  GLIndexd( GLdouble c )
{
  NVSG_GL_STATS_FCT_COUNT();
  glIndexd( c );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glIndexf
/** This function asserts on the result before returning.  */
inline  void  GLIndexf( GLfloat c )
{
  NVSG_GL_STATS_FCT_COUNT();
  glIndexf( c );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glIndexi
/** This function asserts on the result before returning.  */
inline  void  GLIndexi( GLint c )
{
  NVSG_GL_STATS_FCT_COUNT();
  glIndexi( c );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glIndexMask
/** This function asserts on the result before returning.  */
inline  void  GLIndexMask( GLuint mask )
{
  NVSG_GL_STATS_FCT_COUNT();
  glIndexMask( mask );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glIndexPointer
/** This function asserts on the result before returning.  */
inline  void  GLIndexPointer( GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  glIndexPointer( type, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glIndexs
/** This function asserts on the result before returning.  */
inline  void  GLIndexs( GLshort c )
{
  NVSG_GL_STATS_FCT_COUNT();
  glIndexs( c );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glIndexub
/** This function asserts on the result before returning.  */
inline  void  GLIndexub( GLubyte c )
{
  NVSG_GL_STATS_FCT_COUNT();
  glIndexub( c );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glInitNames
/** This function asserts on the result before returning.  */
inline  void  GLInitNames( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  glInitNames();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glInterleavedArrays
/** This function asserts on the result before returning.  */
inline  void  GLInterleavedArrays( GLenum format, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  glInterleavedArrays( format, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glIsEnabled
/** This function returns the state of the queried capability. */
inline  GLboolean  GLIsEnabled( GLenum cap ) 
{
  GLboolean isEnabled;
#if defined( GL_CACHING )
  switch( cap )
  {
    case GL_ALPHA_TEST :
      isEnabled = GLIsEnabledAlphaTest();
      break;
    case GL_AUTO_NORMAL :
      isEnabled = GLIsEnabledAutoNormal();
      break;
    case GL_BLEND :
      isEnabled = GLIsEnabledBlend();
      break;
    case GL_CLIP_PLANE0 :
    case GL_CLIP_PLANE1 :
    case GL_CLIP_PLANE2 :
    case GL_CLIP_PLANE3 :
    case GL_CLIP_PLANE4 :
    case GL_CLIP_PLANE5 :
      isEnabled = GLIsEnabledClipPlane( cap );
      break;
    case GL_COLOR_LOGIC_OP :
      isEnabled = GLIsEnabledColorLogicOp();
      break;
    case GL_COLOR_MATERIAL :
      isEnabled = GLIsEnabledColorMaterial();
      break;
    case GL_CULL_FACE :
      isEnabled = GLIsEnabledCullFace();
      break;
    case GL_DEPTH_TEST :
      isEnabled = GLIsEnabledDepthTest();
      break;
    case GL_DITHER :
      isEnabled = GLIsEnabledDither();
      break;
    case GL_FOG :
      isEnabled = GLIsEnabledFog();
      break;
    case GL_LIGHT0 :
    case GL_LIGHT1 :
    case GL_LIGHT2 :
    case GL_LIGHT3 :
    case GL_LIGHT4 :
    case GL_LIGHT5 :
    case GL_LIGHT6 :
    case GL_LIGHT7 :
      isEnabled = GLIsEnabledLight( cap );
      break;
    case GL_LIGHTING :
      isEnabled = GLIsEnabledLighting();
      break;
    case GL_LINE_SMOOTH :
      isEnabled = GLIsEnabledLineSmooth();
      break;
    case GL_LINE_STIPPLE :
      isEnabled = GLIsEnabledLineStipple();
      break;
    case GL_LOGIC_OP :
      isEnabled = GLIsEnabledLogicOp();
      break;
    case GL_NORMALIZE :
      isEnabled = GLIsEnabledNormalize();
      break;
    case GL_RESCALE_NORMAL :
      isEnabled = GLIsEnabledRescaleNormal();
      break;
    case GL_POINT_SMOOTH :
      isEnabled = GLIsEnabledPointSmooth();
      break;
    case GL_POLYGON_OFFSET_FILL :
      isEnabled = GLIsEnabledPolygonOffsetFill();
      break;
    case GL_POLYGON_OFFSET_LINE :
      isEnabled = GLIsEnabledPolygonOffsetLine();
      break;
    case GL_POLYGON_OFFSET_POINT :
      isEnabled = GLIsEnabledPolygonOffsetPoint();
      break;
    case GL_POLYGON_SMOOTH :
      isEnabled = GLIsEnabledPolygonSmooth();
      break;
    case GL_POLYGON_STIPPLE :
      isEnabled = GLIsEnabledPolygonStipple();
      break;
    case GL_SCISSOR_TEST :
      isEnabled = GLIsEnabledScissorTest();
      break;
    case GL_STENCIL_TEST :
      isEnabled = GLIsEnabledStencilTest();
      break;
    case GL_TEXTURE_1D :
    case GL_TEXTURE_2D :
    case GL_TEXTURE_3D :
    case GL_TEXTURE_CUBE_MAP :
    case GL_TEXTURE_RECTANGLE_NV :
    case GL_TEXTURE_2D_ARRAY_EXT :
    case GL_TEXTURE_1D_ARRAY_EXT :
      isEnabled = GLIsEnabledTexture( cap );
      break;
    case GL_TEXTURE_GEN_S :
    case GL_TEXTURE_GEN_T :
    case GL_TEXTURE_GEN_R :
    case GL_TEXTURE_GEN_Q :
      isEnabled = GLIsEnabledTextureGen( cap );
      break;
    default :
      isEnabled = _GLIsEnabled( cap );
      break;
  }
#else
  isEnabled = _GLIsEnabled( cap );
#endif
  return( isEnabled );
}

//! Replacement of glIsEnabled( GL_ALPHA_TEST )
/** This function returns the state of the GL_ALPHA_TEST capability. */
inline GLboolean GLIsEnabledAlphaTest()
{
  return( GL_IS_ENABLED( GL_ALPHA_TEST, glContext()->m_colorBufferEnableAttributes.m_alphaTest ) );
}

//! Replacement of glIsEnabled( GL_AUTO_NORMAL )
/** This function returns the state of the GL_AUTO_NORMAL capability. */
inline GLboolean GLIsEnabledAutoNormal()
{
  return( GL_IS_ENABLED( GL_AUTO_NORMAL, glContext()->m_evalAttributes.m_autoNormal ) );
}

//! Replacement of glIsEnabled( GL_BLEND )
/** This function returns the state of the GL_BLEND capability. */
inline GLboolean GLIsEnabledBlend()
{
  return( GL_IS_ENABLED( GL_BLEND, glContext()->m_colorBufferEnableAttributes.m_blend ) );
}

//! Replacement of glIsEnabled( GL_CLIP_PLANEi )
/** This function returns the state of the GL_CLIP_PLANEi capability. */
inline GLboolean GLIsEnabledClipPlane( GLenum plane )
{
#if defined( GL_CACHING )
  NVSG_ASSERT( plane-GL_CLIP_PLANE0 < glContext()->m_transformEnableAttributes.m_clipPlanes.size() );
#endif
  return( GL_IS_ENABLED( plane, glContext()->m_transformEnableAttributes.m_clipPlanes[plane-GL_CLIP_PLANE0] ) );
}

//! Replacement of glIsEnabled( GL_COLOR_LOGIC_OP )
/** This function returns the state of the GL_COLOR_LOGIC_OP capability. */
inline GLboolean GLIsEnabledColorLogicOp()
{
  return( GL_IS_ENABLED( GL_COLOR_LOGIC_OP, glContext()->m_colorBufferEnableAttributes.m_colorLogicOp ) );
}

//! Replacement of glIsEnabled( GL_COLOR_MATERIAL )
/** This function returns the state of the GL_COLOR_MATERIAL capability. */
inline  GLboolean GLIsEnabledColorMaterial()
{
  return( GL_IS_ENABLED( GL_COLOR_MATERIAL, glContext()->m_lightingAttributes.m_colorMaterial ) );
}

//! Replacement of glIsEnabled( GL_CULL_FACE )
/** This function returns the state of the GL_CULL_FACE capability. */
inline GLboolean GLIsEnabledCullFace()
{
  return( GL_IS_ENABLED( GL_CULL_FACE, glContext()->m_polygonEnableAttributes.m_cullFace ) );
}

//! Replacement of glIsEnabled( GL_DEPTH_TEST )
/** This function returns the state of the GL_DEPTH_TEST capability. */
inline GLboolean GLIsEnabledDepthTest()
{
  return( GL_IS_ENABLED( GL_DEPTH_TEST, glContext()->m_depthBufferEnableAttributes.m_depthTesting ) );
}

//! Replacement of glIsEnabled( GL_DITHER )
/** This function returns the state of the GL_DITHER capability. */
inline GLboolean GLIsEnabledDither()
{
  return( GL_IS_ENABLED( GL_DITHER, glContext()->m_colorBufferEnableAttributes.m_dither ) );
}

//! Replacement of glIsEnabled( GL_FOG )
/** This function returns the state of the GL_FOG capability. */
inline GLboolean GLIsEnabledFog()
{
  return( GL_IS_ENABLED( GL_FOG, glContext()->m_fogEnableAttributes.m_fog ) );
}

//! Replacement of glIsEnabledIndexed
/** This functions asserts on the result before returning. */
inline  GLboolean GLIsEnabledIndexed( GLenum target, GLuint index )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLboolean ok = glContext()->m_glFunctions->glIsEnabledIndexed( target, index );
  ASSERT_GL_NO_ERROR;
  return( ok );
}

//! Replacement of glIsEnabled( GL_LIGHTi )
/** This function returns the state of the GL_LIGHTi capability. */
inline GLboolean GLIsEnabledLight( GLenum light )
{
#if defined( GL_CACHING )
  NVSG_ASSERT( light-GL_LIGHT0 < glContext()->m_lightingEnableAttributes.m_lights.size() );
#endif
  return( GL_IS_ENABLED( light, glContext()->m_lightingEnableAttributes.m_lights[light-GL_LIGHT0] ) );
}

//! Replacement of glIsEnabled( GL_LIGHTING )
/** This function returns the state of the GL_LIGHTING capability. */
inline GLboolean GLIsEnabledLighting()
{
  return( GL_IS_ENABLED( GL_LIGHTING, glContext()->m_lightingEnableAttributes.m_lighting ) );
}

//! Replacement of glIsEnabled( GL_LINE_SMOOTH )
/** This function returns the state of the GL_LINE_SMOOTH capability. */
inline GLboolean GLIsEnabledLineSmooth()
{
  return( GL_IS_ENABLED( GL_LINE_SMOOTH, glContext()->m_lineEnableAttributes.m_smooth ) );
}

//! Replacement of glIsEnabled( GL_LINE_STIPPLE )
/** This function returns the state of the GL_LINE_STIPPLE capability. */
inline GLboolean GLIsEnabledLineStipple()
{
  return( GL_IS_ENABLED( GL_LINE_STIPPLE, glContext()->m_lineEnableAttributes.m_stipple ) );
}

//! Replacement of glIsEnabled( GL_LOGIC_OP )
/** This function returns the state of the GL_LOGIC_OP capability. */
inline GLboolean GLIsEnabledLogicOp()
{
  return( GL_IS_ENABLED( GL_LOGIC_OP, glContext()->m_colorBufferEnableAttributes.m_logicOp ) );
}

//! Replacement of glIsEnabled( GL_NORMALIZE )
/** This function returns the state of the GL_NORMALIZE capability. */
inline GLboolean GLIsEnabledNormalize()
{
  return( GL_IS_ENABLED( GL_NORMALIZE, glContext()->m_transformEnableAttributes.m_normalize ) );
}

//! Replacement of glIsEnabled( GL_RESCALE_NORMAL )
/** This function returns the state of the GL_RESCALE_NORMAL capability. */
inline GLboolean GLIsEnabledRescaleNormal()
{
  return( GL_IS_ENABLED( GL_RESCALE_NORMAL, glContext()->m_transformEnableAttributes.m_rescaleNormal ) );
}

//! Replacement of glIsEnabled( GL_POINT_SMOOTH )
/** This function returns the state of the GL_POINT_SMOOTH capability. */
inline GLboolean GLIsEnabledPointSmooth()
{
  return( GL_IS_ENABLED( GL_POINT_SMOOTH, glContext()->m_pointEnableAttributes.m_smooth ) );
}

//! Replacement of glIsEnabled( GL_POLYGON_OFFSET_FILL )
/** This function returns the state of the GL_POLYGON_OFFSET_FILL capability. */
inline GLboolean GLIsEnabledPolygonOffsetFill()
{
  return( GL_IS_ENABLED( GL_POLYGON_OFFSET_FILL, glContext()->m_polygonEnableAttributes.m_offsetFill ) );
}

//! Replacement of glIsEnabled( GL_POLYGON_OFFSET_LINE )
/** This function returns the state of the GL_POLYGON_OFFSET_LINE capability. */
inline GLboolean GLIsEnabledPolygonOffsetLine()
{
  return( GL_IS_ENABLED( GL_POLYGON_OFFSET_LINE, glContext()->m_polygonEnableAttributes.m_offsetLine ) );
}

//! Replacement of glIsEnabled( GL_POLYGON_OFFSET_POINT )
/** This function returns the state of the GL_POLYGON_OFFSET_POINT capability. */
inline GLboolean GLIsEnabledPolygonOffsetPoint()
{
  return( GL_IS_ENABLED( GL_POLYGON_OFFSET_POINT, glContext()->m_polygonEnableAttributes.m_offsetPoint ) );
}

//! Replacement of glIsEnabled( GL_POLYGON_SMOOTH )
/** This function returns the state of the GL_POLYGON_SMOOTH capability. */
inline GLboolean GLIsEnabledPolygonSmooth()
{
  return( GL_IS_ENABLED( GL_POLYGON_SMOOTH, glContext()->m_polygonEnableAttributes.m_smooth ) );
}

//! Replacement of glIsEnabled( GL_POLYGON_STIPPLE )
/** This function returns the state of the GL_POLYGON_STIPPLE capability. */
inline GLboolean GLIsEnabledPolygonStipple()
{
  return( GL_IS_ENABLED( GL_POLYGON_STIPPLE, glContext()->m_polygonEnableAttributes.m_stipple ) );
}

//! Replacement of glIsEnabled( GL_SCISSOR_TEST )
/** This function returns the state of the GL_SCISSOR_TEST capability. */
inline GLboolean GLIsEnabledScissorTest()
{
  return( GL_IS_ENABLED( GL_SCISSOR_TEST, glContext()->m_scissorEnableAttributes.m_scissorTest ) );
}

//! Replacement of glIsEnabled( GL_STENCIL_TEST )
/** This function returns the state of the GL_STENCIL_TEST capability. */
inline GLboolean GLIsEnabledStencilTest()
{
  return( GL_IS_ENABLED( GL_STENCIL_TEST, glContext()->m_stencilBufferEnableAttributes.m_stencilTest ) );
}

//! Replacement of glIsEnabled( [GL_TEXTURE_1D|GL_TEXTURE_2D|GL_TEXTURE_3D|GL_TEXTURE_CUBE_MAP|GL_TEXTURE_RECTANGLE_NV] )
/** This function returns the state of the [GL_TEXTURE_1D|GL_TEXTURE_2D|GL_TEXTURE_3D|GL_TEXTURE_CUBE_MAP|GL_TEXTURE_RECTANGLE_NV] capability. */
inline GLboolean GLIsEnabledTexture( GLenum target )
{
  // check on valid target
  NVSG_ASSERT(    ( target == GL_TEXTURE_1D ) || ( target == GL_TEXTURE_2D ) || ( target == GL_TEXTURE_3D )
              ||  ( target == GL_TEXTURE_CUBE_MAP ) || ( target == GL_TEXTURE_RECTANGLE_NV ) 
              ||  ( target == GL_TEXTURE_2D_ARRAY_EXT ) || ( target == GL_TEXTURE_2D_ARRAY_EXT ) );

#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  GLContext * ctx = glContext();
  return( GL_IS_ENABLED( target, ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_active].m_texture[target] ) );
}

//! Replacement of glIsEnabled( [GL_TEXTURE_GEN_S|GL_TEXTURE_GEN_T|GL_TEXTURE_GEN_R|GL_TEXTURE_GEN_Q] )
/** This function returns the state of the [GL_TEXTURE_GEN_S|GL_TEXTURE_GEN_T|GL_TEXTURE_GEN_R|GL_TEXTURE_GEN_Q] capability. */
inline GLboolean GLIsEnabledTextureGen( GLenum target )
{
  // check on valid target
  NVSG_ASSERT(    ( target == GL_TEXTURE_GEN_S )
              ||  ( target == GL_TEXTURE_GEN_T )
              ||  ( target == GL_TEXTURE_GEN_R )
              ||  ( target == GL_TEXTURE_GEN_Q ) );

#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  GLContext * ctx = glContext();
  return( GL_IS_ENABLED( target, ctx->m_textureEnableAttributes.m_unit[ctx->m_textureAttributes.m_active].m_gen[target-GL_TEXTURE_GEN_S] ) );
}

//! Returns the version of the OpenGL available on the target system.
/** \returns The floating point version of the OpenGL with the representation 
  * major.minor is returned as an integer calculated as 10*major+minor.\n\n
  * The OpenGL version 1.4, for example, will be reported as 14. */
inline GLint GLGetVersion()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT(glContext() && glContext()->m_glFunctions);
  return glContext()->m_glFunctions->getVersion();
}

//! Test if a given extension is available on the current context.
/** \note Not all exported extensions might be available.
  * \returns  \c true, if all functions of \a extension are available, otherwise \c false  */
inline GLboolean GLIsExtensionAvailable( const std::string& extension )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT(glContext() && glContext()->m_glFunctions);
  return glContext()->m_glFunctions->isAvailable( extension );
}

//! Test if a given extension is exported on the current context.
/** \note Not all exported extensions might be available.
  * \returns  \c true, if \a extension is exported, otherwise \c false  */
inline GLboolean GLIsExtensionExported( const std::string& extension )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT(glContext() && glContext()->m_glFunctions);
  return glContext()->m_glFunctions->isExported( extension );
}

//! Allows manual disabling of extension available state.
/** This function is meant for debugging purposes and can have negative effects on stability.
  * It should only be called after the GLContext has been initialized.  */
inline void GLDisableExtensionAvailability( const std::string& extension )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT(glContext() && glContext()->m_glFunctions);
  glContext()->m_glFunctions->disableAvailability( extension );
}


//! Replacement of glIsFenceNV
/** This function asserts on the result before returning. */
inline  GLboolean GLIsFence( GLuint fence )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLboolean ok = glContext()->m_glFunctions->glIsFence( fence );
  ASSERT_GL_NO_ERROR;
  return( ok );
}

//! Replacement of glIsList
/** This function tests if for the current context, \a list denotes a valid display list and asserts on the result before returning. */
inline  GLboolean GLIsList( GLuint list )
{
  NVSG_GL_STATS_FCT_COUNT();
  return glContext()->isList(list);
}

//! Replacement of glIsProgram
/** This function tests if for the current context, \a id denotes a valid program and asserts on the result before returning. */
inline  GLboolean GLIsProgram( GLuint program )
{
  NVSG_GL_STATS_FCT_COUNT();
  return glContext()->isProgram( program );
}

//! Replacement of glIsQuery
/** This function tests if for the current context, \a id denotes a valid query and asserts on the result before returning. */
inline  GLboolean GLIsQuery( GLuint id )
{
  NVSG_GL_STATS_FCT_COUNT();
  return glContext()->isQuery(id);
}

//! Replacement of glIsShader
/** This function asserts on the result before returning. */
inline  GLboolean GLIsShader( GLuint shader )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLboolean ok = glContext()->m_glFunctions->glIsShader( shader );
  ASSERT_GL_NO_ERROR;
  return( ok );
}

//! Replacement of glIsTexture
/** This function tests if for the current context, \a texture denotes a valid texture and asserts on the result before returning. */
inline  GLboolean GLIsTexture( GLuint texture )
{
  NVSG_GL_STATS_IS_TEXTURE( texture );
  return(glContext()->isTexture(texture));
}

#if !defined(DOXYGEN_IGNORE)
#if defined( GL_CACHING )
inline void _GLLightfCached( GLenum light, GLenum pname, GLfloat param, GLfloat * cache )
{
  if ( param != *cache )
  {
#if !defined(NDEBUG)
    GLfloat f;
    glGetLightfv( light, pname, &f );
    NVSG_ASSERT( f == *cache );
#endif
    _GLLightf( light, pname, param );
    *cache = param;
  }
}
inline void _GLLightfvCached( GLenum light, GLenum pname, const GLfloat * params, GLfloat * cache )
{
  if ( !equal4fv( params, cache ) )
  {
#if !defined(NDEBUG)
    GLfloat f[4];
    glGetLightfv( light, pname, f );
    NVSG_ASSERT( equal4fv( f, cache ) );
#endif
    _GLLightfv( light, pname, params );
    memcpy( cache, params, 4*sizeof(GLfloat) );
  }
}
# if defined(GL_SUPPORT_RESET)
inline void _GLLightfCached( GLenum light, GLenum pname, GLfloat param, GLfloat * cache, GLfloat * prev )
{
  *prev = *cache;
  _GLLightfCached( light, pname, param, cache );
}
inline void _GLLightfvCached( GLenum light, GLenum pname, const GLfloat * params, GLfloat * cache, GLfloat * prev )
{
  memcpy( prev, cache, 4*sizeof(GLfloat) );
  _GLLightfvCached( light, pname, params, cache );
}
#  define GL_LIGHT_F( light, pname, param, cache, prev )    _GLLightfCached( light, pname, param, cache, prev )
#  define GL_LIGHT_FV( light, pname, params, cache, prev )  _GLLightfvCached( light, pname, params, cache, prev )
# else
#  define GL_LIGHT_F( light, pname, param, cache, prev )    _GLLightfCached( light, pname, param, cache )
#  define GL_LIGHT_FV( light, pname, params, cache, prev )  _GLLightfvCached( light, pname, params, cache )
# endif
#else
# define GL_LIGHT_F( light, pname, param, cache, prev )     _GLLightf( light, pname, param )
# define GL_LIGHT_FV( light, pname, params, cache, prev )   _GLLightfv( light, pname, params )
#endif
#endif // DOXYGEN_IGNORE

//! Cached version of GLLightfv( light, GL_AMBIENT, param )
/** This function caches the GL_AMBIENT value on the light.
  * It is faster to call this method instead of GLLightfv( light, GL_AMBIENT, param ) directly. */
inline  void  GLLightAmbient( GLenum light, const GLfloat *params )
{
  GL_LIGHT_FV( light, GL_AMBIENT, params
             , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_ambient[0]
             , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_ambient[0] );
}

/*! \brief Helper function to reset the ambient light color.
 *  \param light The id of the light for which the ambient color is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the ambient
 *  light to it's previous state. Otherwise, it is set to (0,0,0,1).
 *  \sa GLLightAmbient */
inline void GLResetLightAmbient( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightAmbient( light, &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_ambient[0] );
#else
  static nvmath::Vec4f al(0.0f,0.0f,0.0f,1.0f);
  GLLightAmbient(light, &al[0]);
#endif
}

//! Cached version of GLLightf( light, GL_CONSTANT_ATTENUATION, param )
/** This function caches the GL_CONSTANT_ATTENUATION value on the light.
  * It is faster to call this method instead of GLLightf( light, GL_CONSTANT_ATTENUATION, param ) directly. */
inline  void  GLLightConstantAttenuation( GLenum light, GLfloat param )
{
  GL_LIGHT_F( light, GL_CONSTANT_ATTENUATION, param
            , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[0]
            , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[0] );
}

/*! \brief Helper function to reset the constant attenuation of the light \a light.
 *  \param light The id of the light for which the constant attenuation is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the constant
 *  attenuation of the light \a light to it's previous state. Otherwise, it is set to 1.0f.
 *  \sa GLLightConstantAttenuation */
inline void GLResetLightConstantAttenuation( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightConstantAttenuation( light, glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[0] );
#else
  GLLightConstantAttenuation( light, 1.0f );
#endif
}

//! Cached version of GLLightfv( light, GL_DIFFUSE, param )
/** This function caches the GL_DIFFUSE value on the light.
  * It is faster to call this method instead of GLLightfv( light, GL_DIFFUSE, param ) directly. */
inline  void  GLLightDiffuse( GLenum light, const GLfloat *params )
{
  GL_LIGHT_FV( light, GL_DIFFUSE, params
             , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_diffuse[0]
             , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_diffuse[0] );
}

/*! \brief Helper function to reset the diffuse color of the light \a light.
 *  \param light The id of the light for which the diffuse color is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the diffuse
 *  lighting to it's previous state. Otherwise, it is set to (1,1,1,1) if \a light is GL_LIGHT0, or
 *  (0,0,0,1) otherwise.
 *  \sa GLLightDiffuse */
inline void GLResetLightDiffuse( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightDiffuse( light, &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_diffuse[0] );
#else
  static nvmath::Vec4f ld[2] = { nvmath::Vec4f(1.0f,1.0f,1.0f,1.0f), nvmath::Vec4f(0.0f,0.0f,0.0f,1.0f) };
  GLLightDiffuse( light, &ld[light!=GL_LIGHT0][0] );
#endif
}

//! Cached version of GLLightf( light, GL_LINEAR_ATTENUATION, param )
/** This function caches the GL_LINEAR_ATTENUATION value on the light.
  * It is faster to call this method instead of GLLightf( light, GL_LINEAR_ATTENUATION, param ) directly. */
inline  void  GLLightLinearAttenuation( GLenum light, GLfloat param )
{
  GL_LIGHT_F( light, GL_LINEAR_ATTENUATION, param
            , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[1]
            , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[1] );
}

/*! \brief Helper function to reset the linear attenuation of the light \a light.
 *  \param light The id of the light for which the linear attenuation is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the linear
 *  attenuation of the light \a light to it's previous state. Otherwise, it is set to 0.0f.
 *  \sa GLLightLinearAttenuation */
inline void GLResetLightLinearAttenuation( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightLinearAttenuation( light, glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[1] );
#else
  GLLightLinearAttenuation( light, 0.0f );
#endif
}

//! Cached version of GLLightfv( light, GL_POSITION, param )
/** The light position is not cached, and it should not be. This function just exists for completeness. */
inline  void  GLLightPosition( GLenum light, const GLfloat *params )
{
#if defined( GL_CACHING )
# if defined( GL_SUPPORT_RESET )
  GLContext * ctx = glContext();
  ctx->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_position = ctx->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_position;
  memcpy( &ctx->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_position[0], params, 4*sizeof(GLfloat) );
# endif
  _GLLightfv( light, GL_POSITION, params );
#else
  GLLightfv( light, GL_POSITION, params );
#endif
}

/*! \brief Helper function to reset the position of the light \a light.
 *  \param light The id of the light for which the position is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the position
 *  of the light \a light to it's previous state. Otherwise, it is set to (0,0,1,0).
 *  \sa GLLightPosition */
inline void GLResetLightPosition( GLenum light )
{
#if defined( GL_SUPPORT_RESET )
  GLLightPosition( light, &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_position[0] );
#else
  static nvmath::Vec4f lp(0.0f,0.0f,1.0f,0.0f);
  GLLightPosition( light, &lp[0] );
#endif
}

//! Cached version of GLLightf( light, GL_QUADRATIC_ATTENUATION, param )
/** This function caches the GL_QUADRATIC_ATTENUATION value on the light.
  * It is faster to call this method instead of GLLightf( light, GL_QUADRATIC_ATTENUATION, param ) directly. */
inline  void  GLLightQuadraticAttenuation( GLenum light, GLfloat param )
{
  GL_LIGHT_F( light, GL_QUADRATIC_ATTENUATION, param
            , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[2]
            , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[2] );
}

/*! \brief Helper function to reset the quadratic attenuation of the light \a light.
 *  \param light The id of the light for which the quadratic attenuation is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the quadratic
 *  attenuation of the light \a light to it's previous state. Otherwise, it is set to 0.0f.
 *  \sa GLLightQuadraticAttenuation */
inline void GLResetLightQuadraticAttenuation( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightQuadraticAttenuation( light, glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_attenuation[2] );
#else
  GLLightQuadraticAttenuation( light, 0.0f );
#endif
}

//! Cached version of GLLightfv( light, GL_SPECULAR, param )
/** This function caches the GL_SPECULAR value on the light.
  * It is faster to call this method instead of GLLightfv( light, GL_SPECULAR, param ) directly. */
inline  void  GLLightSpecular( GLenum light, const GLfloat *params )
{
  GL_LIGHT_FV( light, GL_SPECULAR, params
             , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_specular[0]
             , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_specular[0] );
}

/*! \brief Helper function to reset the specular color of the light \a light.
 *  \param light The id of the light for which the specular color is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the specular
 *  color of the light \a light to it's previous state. Otherwise, it is set to (1,1,1,1) if \a
 *  light is GL_LIGHT0, or (0,0,0,1) otherwise.
 *  \sa GLLightSpecular */
inline void GLResetLightSpecular( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightSpecular( light, &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_specular[0] );
#else
  static nvmath::Vec4f ls[2] = { nvmath::Vec4f(1.0f,1.0f,1.0f,1.0f), nvmath::Vec4f(0.0f,0.0f,0.0f,1.0f) };
  GLLightSpecular( light, &ls[light!=GL_LIGHT0][0] );
#endif
}

//! Cached version of GLLightf( light, GL_SPOT_CUTOFF, param )
/** This function caches the GL_SPOT_CUTOFF value on the light.
  * It is faster to call this method instead of GLLightf( light, GL_SPOT_CUTOFF, param ) directly. */
inline  void  GLLightSpotCutoff( GLenum light, GLfloat param )
{
  GL_LIGHT_F( light, GL_SPOT_CUTOFF, param
            , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_spotCutoff
            , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_spotCutoff );
}

/*! \brief Helper function to reset the spot cutoff of the light \a light.
 *  \param light The id of the light for which the spot cutoff is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the spot cutoff
 *  of the light \a light to it's previous state. Otherwise, it is set to 180.0f.
 *  \sa GLLightSpotCutoff */
inline void GLResetLightSpotCutoff( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightSpotCutoff( light, glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_spotCutoff );
#else
  GLLightSpotCutoff( light, 180.0f );
#endif
}

//! Cached version of GLLightfv( light, GL_SPOT_DIRECTION, param )
/** The light spot direction is not cached, and it should not be. This function just exists for completeness. */
inline  void  GLLightSpotDirection( GLenum light, const GLfloat *params )
{
#if defined( GL_CACHING )
# if defined( GL_SUPPORT_RESET )
  GLContext * ctx = glContext();
  ctx->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_spotDirection = ctx->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_spotDirection;
  memcpy( &ctx->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_spotDirection[0], params, 4*sizeof(GLfloat) );
# endif
  _GLLightfv( light, GL_SPOT_DIRECTION, params );
#else
  GLLightfv( light, GL_SPOT_DIRECTION, params );
#endif
}

/*! \brief Helper function to reset the spot cutoff of the light \a light.
 *  \param light The id of the light for which the spot cutoff is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the spot cutoff
 *  of the light \a light to it's previous state. Otherwise, it is set to 180.0f.
 *  \sa GLLightSpotCutoff */
inline void GLResetLightSpotDirection( GLenum light )
{
#if defined( GL_SUPPORT_RESET )
  GLLightSpotDirection( light
                      , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_spotDirection[0] );
#else
  static nvmath::Vec4f lsd(0.0f,0.0f,-1.0f,1.0f);
  GLLightSpotDirection( light, &lsd[0] );
#endif
}

//! Cached version of GLLightf( light, GL_SPOT_EXPONENT, param )
/** This function caches the GL_SPOT_EXPONENT value on the light.
  * It is faster to call this method instead of GLLightf( light, GL_SPOT_EXPONENT, param ) directly. */
inline  void  GLLightSpotExponent( GLenum light, GLfloat param )
{
  GL_LIGHT_F( light, GL_SPOT_EXPONENT, param
            , &glContext()->m_lightingAttributes.m_lightParameters[light-GL_LIGHT0].m_spotExponent
            , &glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_spotExponent );
}

/*! \brief Helper function to reset the spot exponent of the light \a light.
 *  \param light The id of the light for which the spot exponent is to be reset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the spot
 *  exponent of the light \a light to it's previous state. Otherwise, it is set to 0.0f.
 *  \sa GLLightSpotExponent */
inline void GLResetLightSpotExponent( GLenum light )
{
#if defined(GL_SUPPORT_RESET)
  GLLightSpotExponent( light, glContext()->m_previousAttributes.m_lightParameters[light-GL_LIGHT0].m_spotExponent );
#else
  GLLightSpotExponent( light, 0.0f );
#endif
}

//! Replacement of glLightf
/** This function caches on some pname values and asserts on the result. */
inline  void  GLLightf( GLenum light, GLenum pname, GLfloat param ) 
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_CONSTANT_ATTENUATION :
      GLLightConstantAttenuation( light, param );
      break;
    case GL_LINEAR_ATTENUATION :
      GLLightLinearAttenuation( light, param );
      break;
    case GL_QUADRATIC_ATTENUATION :
      GLLightQuadraticAttenuation( light, param );
      break;
    case GL_SPOT_CUTOFF :
      GLLightSpotCutoff( light, param );
      break;
    case GL_SPOT_EXPONENT :
      GLLightSpotExponent( light, param );
      break;
    default :
      _GLLightf( light, pname, param );
      break;
  }
#else
  _GLLightf( light, pname, param );
#endif
}

//! Replacement of glLightfv
/** This function caches on some pname values and asserts on the result. */
inline  void  GLLightfv( GLenum light, GLenum pname, const GLfloat *params ) 
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_AMBIENT :
      GLLightAmbient( light, params );
      break;
    case GL_CONSTANT_ATTENUATION :
      GLLightConstantAttenuation( light, params[0] );
      break;
    case GL_DIFFUSE :
      GLLightDiffuse( light, params );
      break;
    case GL_LINEAR_ATTENUATION :
      GLLightLinearAttenuation( light, params[0] );
      break;
    case GL_POSITION :
      GLLightPosition( light, params );
      break;
    case GL_QUADRATIC_ATTENUATION :
      GLLightQuadraticAttenuation( light, params[0] );
      break;
    case GL_SPECULAR :
      GLLightSpecular( light, params );
      break;
    case GL_SPOT_CUTOFF :
      GLLightSpotCutoff( light, params[0] );
      break;
    case GL_SPOT_DIRECTION :
      GLLightSpotDirection( light, params );
      break;
    case GL_SPOT_EXPONENT :
      GLLightSpotExponent( light, params[0] );
      break;
    default :
      _GLLightfv( light, pname, params );
      break;
  }
#else
  _GLLightfv( light, pname, params );
#endif
}

//! Replacement of glLighti
/** This function caches on some pname values and asserts on the result. */
inline  void  GLLighti( GLenum light, GLenum pname, GLint param ) 
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_CONSTANT_ATTENUATION :
      GLLightConstantAttenuation( light, (GLfloat)param );
      break;
    case GL_LINEAR_ATTENUATION :
      GLLightLinearAttenuation( light, (GLfloat)param );
      break;
    case GL_QUADRATIC_ATTENUATION :
      GLLightQuadraticAttenuation( light, (GLfloat)param );
      break;
    case GL_SPOT_CUTOFF :
      GLLightSpotCutoff( light, (GLfloat)param );
      break;
    case GL_SPOT_EXPONENT :
      GLLightSpotExponent( light, (GLfloat)param );
      break;
    default :
      _GLLighti( light, pname, param );
      break;
  }
#else
  _GLLighti( light, pname, param );
#endif
}

//! Convert GLInt to GLFloat
inline  void  GLintToGLfloat( const GLint *params, GLfloat *fParams )
{
  for ( size_t i=0 ; i<4 ; i++ )
  {
    fParams[i] = (GLfloat)params[i] / INT_MAX;
  }
}

//! Replacement of glLightiv
/** This function caches on some pname values and asserts on the result. */
inline  void  GLLightiv( GLenum light, GLenum pname, GLint *params ) 
{ 
#if defined( GL_CACHING )
  GLfloat fParams[4];
  switch( pname )
  {
    case GL_AMBIENT :
      GLintToGLfloat( params, fParams );
      GLLightAmbient( light, fParams );
      break;
    case GL_CONSTANT_ATTENUATION :
      GLLightConstantAttenuation( light, (GLfloat)params[0] );
      break;
    case GL_DIFFUSE :
      GLintToGLfloat( params, fParams );
      GLLightDiffuse( light, fParams );
      break;
    case GL_LINEAR_ATTENUATION :
      GLLightLinearAttenuation( light, (GLfloat)params[0] );
      break;
    case GL_POSITION :
      for ( size_t i=0 ; i<4 ; i++ )
      {
        fParams[i] = (GLfloat)params[i];
      }
      GLLightPosition( light, fParams );
      break;
    case GL_QUADRATIC_ATTENUATION :
      GLLightQuadraticAttenuation( light, (GLfloat)params[0] );
      break;
    case GL_SPECULAR :
      GLintToGLfloat( params, fParams );
      GLLightSpecular( light, fParams );
      break;
    case GL_SPOT_CUTOFF :
      GLLightSpotCutoff( light, (GLfloat)params[0] );
      break;
    case GL_SPOT_DIRECTION :
      for ( size_t i=0 ; i<4 ; i++ )
      {
        fParams[i] = (GLfloat)params[i];
      }
      GLLightSpotDirection( light, fParams );
      break;
    case GL_SPOT_EXPONENT :
      GLLightSpotExponent( light, (GLfloat)params[0] );
      break;
    default :
      _GLLightiv( light, pname, params );
      break;
  }
#else
  _GLLightiv( light, pname, params );
#endif
}

//! Replacement of glLightModelfv( GL_LIGHT_MODEL_AMBIENT, params )
/** This function caches the light model settings and asserts on the result before returning.  */
inline  void  GLLightModelAmbient( const GLfloat * params )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_lightModelAmbient = ctx->m_lightingAttributes.m_lightModelAmbient;
# endif
  if ( !equal4fv( &ctx->m_lightingAttributes.m_lightModelAmbient[0], params ) )
#endif
  {
    _GLLightModelfv( GL_LIGHT_MODEL_AMBIENT, params );
#if defined( GL_CACHING )
    memcpy( &ctx->m_lightingAttributes.m_lightModelAmbient[0], params, 4*sizeof(float) );
#endif
  }
}

/*! \brief Helper function to reset the ambient light color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the ambient
 *  light color to it's previous state. Otherwise, it is set to (0.2,0.2,0.2,1.0).
 *  \sa GLLightSpotExponent */
inline void GLResetLightModelAmbient()
{
#if defined(GL_SUPPORT_RESET)
  GLLightModelAmbient( &glContext()->m_previousAttributes.m_lightModelAmbient[0] );
#else
  static nvmath::Vec4f lma(0.2f,0.2f,0.2f,1.0f);
  GLLightModelAmbient( &lma[0] );
#endif
}

//! Replacement of GLLightModelLocalViewer
/** This function caches the light model settings and asserts on the result before returning.  */
inline  void  GLLightModelLocalViewer( GLint param )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_lightModelLocalViewer = ctx->m_lightingAttributes.m_lightModelLocalViewer;
# endif
  if ( ctx->m_lightingAttributes.m_lightModelLocalViewer != !!param )
  {
# if !defined(NDEBUG)
    GLboolean lv;
    glGetBooleanv( GL_LIGHT_MODEL_LOCAL_VIEWER, &lv );
    NVSG_ASSERT( ctx->m_lightingAttributes.m_lightModelLocalViewer == !!lv );
# endif
#endif
    _GLLightModeli( GL_LIGHT_MODEL_LOCAL_VIEWER, param );
#if defined( GL_CACHING )
    ctx->m_lightingAttributes.m_lightModelLocalViewer = !!param;
  }
#endif
}

/*! \brief Helper function to reset the local viewer setting
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the local
 *  viewer to it's previous state. Otherwise, it is set to GL_FALSE.
 *  \sa GLLightModelLocalViewer */
inline void GLResetLightModelLocalViewer()
{
#if defined(GL_SUPPORT_RESET)
  GLLightModelLocalViewer( glContext()->m_previousAttributes.m_lightModelLocalViewer );
#else
  GLLightModelLocalViewer( GL_FALSE );
#endif
}

//! Replacement of GLLightModelTwoSide
/** This function caches the light model settings and asserts on the result before returning.  */
inline  void  GLLightModelTwoSide( GLint param )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_lightModelTwoSide = ctx->m_lightingAttributes.m_lightModelTwoSide;
# endif
  if ( ctx->m_lightingAttributes.m_lightModelTwoSide != !!param )
#endif
  {
    if ( ! ctx->m_locks.m_lightModelTwoSide )
    {
#if !defined( NDEBUG ) && defined( GL_CACHING )
      GLboolean lv;
      glGetBooleanv( GL_LIGHT_MODEL_TWO_SIDE, &lv );
      NVSG_ASSERT( ctx->m_lightingAttributes.m_lightModelTwoSide == !!lv );
#endif
      _GLLightModeli( GL_LIGHT_MODEL_TWO_SIDE, param );
#if defined( GL_CACHING )
      ctx->m_lightingAttributes.m_lightModelTwoSide = !!param;
#endif
    }
  }
}

/*! \brief Helper function to reset the two sided lighting state.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the two sided
 *  lighting to it's previous state. Otherwise, it is set to GL_FALSE.
 *  \sa GLLightModelTwoSide */
inline void GLResetLightModelTwoSide()
{
#if defined(GL_SUPPORT_RESET)
  GLLightModelTwoSide( glContext()->m_previousAttributes.m_lightModelTwoSide );
#else
  GLLightModelTwoSide( GL_FALSE );
#endif
}

/*! \brief Helper function to set two sided lighting and lock it.
 *  \param param The value to set and lock two sided lighting to.
 *  \remarks This function sets two sided lighting to \a param and locks it. Any subsequent calls to
 *  GLLightModelTwoSide or GLLightModel{if}(GL_LIGHT_MODEL_TWO_SIDE,) will not change that setting,
 *  until the next call to GLLightModelTwoSideUnlock.
 *  \sa GLLightModelTwoSide, GLLightModelTwoSideUnlock */
inline void GLLightModelTwoSideLock( GLint param )
{
  NVSG_ASSERT( !glContext()->m_locks.m_lightModelTwoSide );
  GLLightModelTwoSide( param );
  glContext()->m_locks.m_lightModelTwoSide = true;
}

/*! \brief Helper function to unlock the two sided lighting and set it.
 *  \param param The value to set the unlocked two sided lighting to.
 *  \remarks This function unlocks two sided lighting and sets it to \a param.
 *  \sa GLLightModelTwoSide, GLLightModelTwoSideLock */
inline void GLLightModelTwoSideUnlock( GLint param )
{
  NVSG_ASSERT( glContext()->m_locks.m_lightModelTwoSide );
  glContext()->m_locks.m_lightModelTwoSide = false;
  GLLightModelTwoSide( param );
}

//! Replacement of glLightModelf
/** This function caches some of the light model settings and asserts on the result before returning.  */
inline  void  GLLightModelf( GLenum pname, GLfloat param )  
{
  switch( pname )
  {
    case GL_LIGHT_MODEL_LOCAL_VIEWER :
      GLLightModelLocalViewer( (GLint)param );
      break;
    case GL_LIGHT_MODEL_TWO_SIDE :
      GLLightModelTwoSide( (GLint)param );
      break;
    default :
      _GLLightModelf( pname, param );
      break;
  }
}

//! Replacement of glLightModelfv
/** This function caches some the light model settings and asserts on the result before returning.  */
inline  void  GLLightModelfv( GLenum pname, const GLfloat *params ) 
{ 
  switch( pname )
  {
    case GL_LIGHT_MODEL_AMBIENT :
      GLLightModelAmbient( params );
      break;
    case GL_LIGHT_MODEL_LOCAL_VIEWER :
      GLLightModelLocalViewer( (GLint)params[0] );
      break;
    case GL_LIGHT_MODEL_TWO_SIDE :
      GLLightModelTwoSide( (GLint)params[0] );
      break;
    default :
      _GLLightModelfv( pname, params );
      break;
  }
}

//! Replacement of glLightModeli
/** This function caches some of the light model settings and asserts on the result before returning.  */
inline  void  GLLightModeli( GLenum pname, GLint param )  
{
  switch( pname )
  {
    case GL_LIGHT_MODEL_LOCAL_VIEWER :
      GLLightModelLocalViewer( param );
      break;
    case GL_LIGHT_MODEL_TWO_SIDE :
      GLLightModelTwoSide( param );
      break;
    default :
      _GLLightModeli( pname, param );
      break;
  }
}

//! Replacement of glLightModeliv
/** This function caches some of the light model settings and asserts on the result before returning.  */
inline  void  GLLightModeliv( GLenum pname, const GLint *params ) 
{ 
  switch( pname )
  {
    case GL_LIGHT_MODEL_LOCAL_VIEWER :
      GLLightModelLocalViewer( params[0] );
      break;
    case GL_LIGHT_MODEL_TWO_SIDE :
      GLLightModelTwoSide( params[0] );
      break;
    default :
      _GLLightModeliv( pname, params );
      break;
  }
}

//! Replacement of glLineStipple
/** This function caches the line stipple settings and asserts on the result before returning.  */
inline  void  GLLineStipple( GLint factor, GLushort pattern )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_lineStipplePattern = ctx->m_lineAttributes.m_stipplePattern;
  ctx->m_previousAttributes.m_lineStippleRepeat  = ctx->m_lineAttributes.m_stippleRepeat;
# endif
  if ( ( ctx->m_lineAttributes.m_stippleRepeat != factor ) || ( ctx->m_lineAttributes.m_stipplePattern != pattern ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glLineStipple( factor, pattern );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_lineAttributes.m_stippleRepeat = factor;
    ctx->m_lineAttributes.m_stipplePattern = pattern;
#endif
  }
}

/*! \brief Helper function to reset line stipple repeat and pattern.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the line
 *  stipple repeat and pattern to it's previous state. Otherwise, it is set to 1 and 0xFFFF,
 *  respectively.
 *  \sa GLLineStipple */
inline void GLResetLineStippleDefinition()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLLineStipple( ctx->m_previousAttributes.m_lineStippleRepeat
               , ctx->m_previousAttributes.m_lineStipplePattern );
#else
  GLLineStipple( 1, (GLushort)0xFFFF );
#endif
}

//! Replacement of glLineWidth
/** This function caches the line width setting and asserts on the result before returning.  */
inline  void  GLLineWidth( GLfloat width )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_lineWidth = ctx->m_lineAttributes.m_width;
# endif
  if ( ctx->m_lineAttributes.m_width != width )
  {
# if !defined(NDEBUG)
    GLfloat f;
    glGetFloatv( GL_LINE_WIDTH, &f );
    NVSG_ASSERT( ctx->m_lineAttributes.m_width == f );
# endif
#endif
    NVSG_GL_STATS_LINE_WIDTH( width );
    glLineWidth( width );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_lineAttributes.m_width = width;
  }
#endif
}

/*! \brief Helper function to reset the line width.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the line
 *  width to it's previous state. Otherwise, it is set to 1.0f.
 *  \sa GLLineWidth */
inline void GLResetLineWidth()
{
#if defined(GL_SUPPORT_RESET)
  GLLineWidth( glContext()->m_previousAttributes.m_lineWidth );
#else
  GLLineWidth( 1.0f );
#endif
}

//! Replacement of glLinkProgram
/** This function asserts on the result before returning.  */
inline  void  GLLinkProgram( GLuint program )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glLinkProgram( program );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glListBase
/** This function asserts on the result before returning.  */
inline  void  GLListBase( GLuint base )
{
  NVSG_GL_STATS_FCT_COUNT();
  glListBase( base );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLoadItentity
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLLoadIdentity( void )
{
  NVSG_GL_STATS_LOAD_IDENTITY();
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glLoadIdentity();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLoadMatrixd
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLLoadMatrixd( const GLdouble *m )
{
  NVSG_GL_STATS_LOAD_MATRIXD( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glLoadMatrixd( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLoadMatrixf
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLLoadMatrixf( const GLfloat *m )
{
  NVSG_GL_STATS_LOAD_MATRIXF( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glLoadMatrixf( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLoadTransposeMatrixd
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLLoadTransposedMatrixd( const GLdouble *m )
{
  NVSG_GL_STATS_LOAD_MATRIXD( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glLoadTransposeMatrixd( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLoadTransposMatrixf
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLLoadTransposeMatrixf( const GLfloat *m )
{
  NVSG_GL_STATS_LOAD_MATRIXF( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glLoadTransposeMatrixf( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLoadName
/** This function asserts on the result before returning.  */
inline  void  GLLoadName( GLuint name )
{
  NVSG_GL_STATS_FCT_COUNT();
  glLoadName( name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLoadProgramNV
/** This function hides the extension character of glLoadProgramNV and asserts on the result before returning.  */
inline  void  GLLoadProgram( GLenum target, GLuint id, GLsizei len, const GLubyte *program )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glLoadProgramNV( target, id, len, program );
#if !defined(NDEBUG)
  const GLubyte * err = glGetString( GL_PROGRAM_ERROR_STRING_NV );
  NVSG_ASSERT( err[0] == 0 );
  ASSERT_GL_NO_ERROR;
  GLint errorPos;
  glGetIntegerv( GL_PROGRAM_ERROR_POSITION_NV, &errorPos );
  NVSG_ASSERT( errorPos == -1 );
#endif
}

//! Replacement of glLockArrays
/** This function asserts on the result before returning.  */
inline  void  GLLockArrays( GLint first, GLsizei count )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glLockArrays( first, count );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glLogicOp
/** This function asserts on the result before returning.  */
inline  void  GLLogicOp( GLenum opcode )
{
  NVSG_GL_STATS_FCT_COUNT();
  glLogicOp( opcode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMap1d
/** This function asserts on the result before returning.  */
inline  void  GLMap1d( GLenum target, GLdouble u1, GLdouble u2, GLint stride, GLint order, const GLdouble *points )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMap1d( target, u1, u2, stride, order, points );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMap1f
/** This function asserts on the result before returning.  */
inline  void  GLMap1f( GLenum target, GLfloat u1, GLfloat u2, GLint stride, GLint order, const GLfloat *points )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMap1f( target, u1, u2, stride, order, points );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMap2d
/** This function asserts on the result before returning.  */
inline  void  GLMap2d( GLenum target, GLdouble u1, GLdouble u2, GLint ustride, GLint uorder, GLdouble v1, GLdouble v2, GLint vstride, GLint vorder, const GLdouble *points )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMap2d( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMap2f
/** This function asserts on the result before returning.  */
inline  void  GLMap2f( GLenum target, GLfloat u1, GLfloat u2, GLint ustride, GLint uorder, GLfloat v1, GLfloat v2, GLint vstride, GLint vorder, const GLfloat *points )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMap2f( target, u1, u2, ustride, uorder, v1, v2, vstride, vorder, points );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMapGrid1d
/** This function asserts on the result before returning.  */
inline  void  GLMapGrid1d( GLint un, GLdouble u1, GLdouble u2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMapGrid1d( un, u1, u2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMapGrid1f
/** This function asserts on the result before returning.  */
inline  void  GLMapGrid1f( GLint un, GLfloat u1, GLfloat u2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMapGrid1f( un, u1, u2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMapGrid2d
/** This function asserts on the result before returning.  */
inline  void  GLMapGrid2d( GLint un, GLdouble u1, GLdouble u2, GLint vn, GLdouble v1, GLdouble v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMapGrid2d( un, u1, u2, vn, v1, v2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMapGrid2f
/** This function asserts on the result before returning.  */
inline  void  GLMapGrid2f( GLint un, GLfloat u1, GLfloat u2, GLint vn, GLfloat v1, GLfloat v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glMapGrid2f( un, u1, u2, vn, v1, v2 );
  ASSERT_GL_NO_ERROR;
}

#if !defined(DOXYGEN_IGNORE)
#if defined( GL_CACHING )
inline void _GLMaterialfCached( GLenum face, GLenum pname, GLfloat param, GLfloat * cache )
{
  if ( !equalf( param, *cache ) )
  {
#if !defined(NDEBUG)
    GLfloat f;
    glGetMaterialfv( face, pname, &f );
    NVSG_ASSERT( *cache == f );
#endif
    _GLMaterialf( face, pname, param );
    *cache = param;
  }
}
inline void _GLMaterialfCached( GLenum pname, GLfloat param, GLfloat * cacheFront, GLfloat * cacheBack )
{
  if ( !equalf( param, *cacheFront ) || !equalf( param, *cacheBack ) )
  {
#if !defined(NDEBUG)
    GLfloat f0, f1;
    glGetMaterialfv( GL_FRONT, pname, &f0 );
    glGetMaterialfv( GL_BACK, pname, &f1 );
    NVSG_ASSERT( ( *cacheFront == f0 ) && ( *cacheBack == f1 ) );
#endif
    _GLMaterialf( GL_FRONT_AND_BACK, pname, param );
    *cacheFront = param;
    *cacheBack = param;
  }
}
inline void _GLMaterialfvCached( GLenum face, GLenum pname, const GLfloat * params, GLfloat * cache )
{
  if ( !equal4fv( params, cache ) )
  {
#if !defined(NDEBUG)
    GLfloat p[4];
    glGetMaterialfv( face, pname, p );
    NVSG_ASSERT( equal4fv( cache, p ) );
#endif
    _GLMaterialfv( face, pname, params );
    memcpy( cache, params, 4*sizeof(float) );
  }
}
inline void _GLMaterialfvCached( GLenum face, GLenum pname, const GLfloat * params, GLfloat * cache, bool colorMaterial )
{
  if ( colorMaterial )
  {
    _GLMaterialfv( face, pname, params );
  }
  else
  {
    _GLMaterialfvCached( face, pname, params, cache );
  }
}
inline void _GLMaterialfvCached( GLenum pname, const GLfloat * params, GLfloat * cacheFront, GLfloat * cacheBack )
{
  if ( !equal4fv( params, cacheFront ) || !equal4fv( params, cacheBack ) )
  {
#if !defined(NDEBUG)
    GLfloat p0[4], p1[4];
    glGetMaterialfv( GL_FRONT, pname, p0 );
    glGetMaterialfv( GL_BACK, pname, p1 );
    NVSG_ASSERT( equal4fv( cacheFront, p0 ) && equal4fv( cacheBack, p1 ) );
#endif
    _GLMaterialfv( GL_FRONT_AND_BACK, pname, params );
    memcpy( cacheFront, params, 4*sizeof(float) );
    memcpy( cacheBack, params, 4*sizeof(float) );
  }
}
inline void _GLMaterialfvCached( GLenum pname, const GLfloat * params, GLfloat * cacheFront, GLfloat * cacheBack, bool colorMaterial )
{
  if ( colorMaterial )
  {
    _GLMaterialfv( GL_FRONT_AND_BACK, pname, params );
  }
  else
  {
    _GLMaterialfvCached( pname, params, cacheFront, cacheBack );
  }
}
# if defined(GL_SUPPORT_RESET)
inline void _GLMaterialfCachedR( GLenum face, GLenum pname, GLfloat param, GLfloat * cache, GLfloat * prev )
{
  *prev = *cache;
  _GLMaterialfCached( face, pname, param, cache );
}
inline void _GLMaterialfCachedR( GLenum pname, GLfloat param, GLfloat * cacheFront, GLfloat * cacheBack, GLfloat * prevFront, GLfloat * prevBack )
{
  *prevFront = *cacheFront;
  *prevBack = *cacheBack;
  _GLMaterialfCached( pname, param, cacheFront, cacheBack );
}
inline void _GLMaterialfvCachedR( GLenum face, GLenum pname, const GLfloat * params, GLfloat * cache, GLfloat * prev )
{
  memcpy( prev, cache, 4*sizeof(GLfloat) );
  _GLMaterialfvCached( face, pname, params, cache );
}
inline void _GLMaterialfvCachedR( GLenum face, GLenum pname, const GLfloat * params, GLfloat * cache, GLfloat * prev, bool colorMaterial )
{
  memcpy( prev, cache, 4*sizeof(GLfloat) );
  _GLMaterialfvCached( face, pname, params, cache, colorMaterial );
}
inline void _GLMaterialfvCachedR( GLenum pname, const GLfloat * params, GLfloat * cacheFront, GLfloat * cacheBack, GLfloat * prevFront, GLfloat * prevBack )
{
  memcpy( prevFront, cacheFront, 4*sizeof(GLfloat) );
  memcpy( prevBack, cacheBack, 4*sizeof(GLfloat) );
  _GLMaterialfvCached( pname, params, cacheFront, cacheBack );
}
inline void _GLMaterialfvCachedR( GLenum pname, const GLfloat * params, GLfloat * cacheFront, GLfloat * cacheBack, GLfloat * prevFront, GLfloat * prevBack, bool colorMaterial )
{
  memcpy( prevFront, cacheFront, 4*sizeof(GLfloat) );
  memcpy( prevBack, cacheBack, 4*sizeof(GLfloat) );
  _GLMaterialfvCached( pname, params, cacheFront, cacheBack, colorMaterial );
}
#  define GL_MATERIAL_F( face, pname, param, cache, prev )                                    _GLMaterialfCachedR( face, pname, param, cache, prev )
#  define GL_MATERIAL_F2( pname, param, cacheFront, cacheBack, prevFront, prevBack )          _GLMaterialfCachedR( pname, param, cacheFront, cacheBack, prevFront, prevBack )
#  define GL_MATERIAL_FV( face, pname, params, cache, prev )                                  _GLMaterialfvCachedR( face, pname, params, cache, prev )
#  define GL_MATERIAL_FV2( pname, params, cacheFront, cacheBack, prevFront, prevBack )        _GLMaterialfvCachedR( pname, params, cacheFront, cacheBack, prevFront, prevBack )
#  define GL_MATERIAL_FV_CM( face, pname, params, cache, prev, cm )                           _GLMaterialfvCachedR( face, pname, params, cache, prev, cm )
#  define GL_MATERIAL_FV2_CM( pname, params, cacheFront, cacheBack, prevFront, prevBack, cm ) _GLMaterialfvCachedR( pname, params, cacheFront, cacheBack, prevFront, prevBack, cm )
# else
#  define GL_MATERIAL_F( face, pname, param, cache, prev )                                    _GLMaterialfCached( face, pname, param, cache )
#  define GL_MATERIAL_F2( pname, param, cacheFront, cacheBack, prevFront, prevBack )          _GLMaterialfCached( pname, param, cacheFront, cacheBack )
#  define GL_MATERIAL_FV( face, pname, params, cache, prev )                                  _GLMaterialfvCached( face, pname, params, cache )
#  define GL_MATERIAL_FV2( pname, params, cacheFront, cacheBack, prevFront, prevBack )        _GLMaterialfvCached( pname, params, cacheFront, cacheBack )
#  define GL_MATERIAL_FV_CM( face, pname, params, cache, prev, cm )                           _GLMaterialfvCached( face, pname, params, cache, cm )
#  define GL_MATERIAL_FV2_CM( pname, params, cacheFront, cacheBack, prevFront, prevBack, cm ) _GLMaterialfvCached( pname, params, cacheFront, cacheBack, cm )
# endif
#else
# define GL_MATERIAL_F( face, pname, param, cache, prevFront )                                _GLMaterialf( face, pname, param )
# define GL_MATERIAL_F2( pname, param, cacheFront, cacheBack, prevFront, prevBack )           _GLMaterialf( GL_FRONT_AND_BACK, pname, param )
# define GL_MATERIAL_FV( face, pname, params, cache, prev )                                   _GLMaterialfv( face, pname, params )
# define GL_MATERIAL_FV2( pname, params, cacheFront, cacheBack, prevFront, prevBack )         _GLMaterialfv( GL_FRONT_AND_BACK, pname, params )
# define GL_MATERIAL_FV_CM( face, pname, params, cache, prev, cm )                            _GLMaterialfv( face, pname, params )
# define GL_MATERIAL_FV2_CM( pname, params, cacheFront, cacheBack, prevFront, prevBack, cm )  _GLMaterialfv( GL_FRONT_AND_BACK, pname, params )
#endif

#if defined(GL_SUPPORT_RESET)
inline void _GLResetMaterialf( GLenum pname, GLfloat defParam, GLfloat * cacheFront, GLfloat * cacheBack, GLfloat * prevFront, GLfloat * prevBack )
{
  if ( *prevFront == *prevBack )
  {
    _GLMaterialfCachedR( pname, *prevFront, cacheFront, cacheBack, prevFront, prevBack );
  }
  else
  {
    _GLMaterialfCachedR( GL_FRONT, pname, *prevFront, cacheFront, prevFront );
    _GLMaterialfCachedR( GL_BACK, pname, *prevBack, cacheBack, prevBack );
  }
}
inline void _GLResetMaterialfv( GLenum face, GLenum pname, GLfloat * cache, GLfloat * prev )
{
  GLfloat tmp[4];
  memcpy( &tmp[0], prev, 4*sizeof(GLfloat) );
  _GLMaterialfvCachedR( face, pname, tmp, cache, prev );
}
inline void _GLResetMaterialfv( GLenum pname, GLfloat * cacheFront, GLfloat * cacheBack, GLfloat * prevFront, GLfloat * prevBack )
{
  if ( equal4fv( prevFront, prevBack ) )
  {
    GLfloat tmp[4];
    memcpy( &tmp[0], prevFront, 4*sizeof(GLfloat) );
    _GLMaterialfvCachedR( pname, tmp, cacheFront, cacheBack, prevFront, prevBack );
  }
  else
  {
    _GLResetMaterialfv( GL_FRONT, pname, cacheFront, prevFront );
    _GLResetMaterialfv( GL_BACK, pname, cacheBack, prevBack );
  }
}
inline void _GLResetMaterialfv( GLenum face, GLenum pname, const GLfloat * defParams, GLfloat * cache, GLfloat * prev, bool colorMaterial )
{
  if ( colorMaterial )
  {
    _GLMaterialfv( face, pname, defParams );
  }
  else
  {
    _GLResetMaterialfv( face, pname, cache, prev );
  }
}
inline void _GLResetMaterialfv( GLenum pname, const GLfloat * defParams, GLfloat * cacheFront, GLfloat * cacheBack, GLfloat * prevFront, GLfloat * prevBack, bool colorMaterial )
{
  if ( colorMaterial )
  {
    _GLMaterialfv( GL_FRONT_AND_BACK, pname, defParams );
  }
  else
  {
    _GLResetMaterialfv( pname, cacheFront, cacheBack, prevFront, prevBack );
  }
}
# define GL_RESET_MATERIAL_F( face, pname, defParam, cache, prev )                                    _GLMaterialfCachedR( face, pname, *prev, cache, prev )
# define GL_RESET_MATERIAL_F2( pname, defParam, cacheFront, cacheBack, prevFront, prevBack )          _GLResetMaterialf( pname, defParam, cacheFront, cacheBack, prevFront, prevBack )
# define GL_RESET_MATERIAL_FV( face, pname, defParams, cache, prev )                                  _GLResetMaterialfv( face, pname, cache, prev )
# define GL_RESET_MATERIAL_FV2( pname, defParams, cacheFront, cacheBack, prevFront, prevBack )        _GLResetMaterialfv( pname, cacheFront, cacheBack, prevFront, prevBack )
# define GL_RESET_MATERIAL_FV_CM( face, pname, defParams, cache, prev, cm )                           _GLResetMaterialfv( face, pname, defParams, cache, prev, cm )
# define GL_RESET_MATERIAL_FV2_CM( pname, defParams, cacheFront, cacheBack, prevFront, prevBack, cm ) _GLResetMaterialfv( pname, defParams, cacheFront, cacheBack, prevFront, prevBack, cm )
#else
# define GL_RESET_MATERIAL_F( face, pname, defParam, cache, prev )                                    _GLMaterialf( face, pname, defParam )
# define GL_RESET_MATERIAL_F2( pname, defParam, cacheFront, cacheBack, prevFront, prevBack )          _GLMaterialf( GL_FRONT_AND_BACK, pname, defParam )
# define GL_RESET_MATERIAL_FV( face, pname, defParams, cache, prev )                                  _GLMaterialfv( face, pname, defParams )
# define GL_RESET_MATERIAL_FV2( pname, defParams, cacheFront, cacheBack, prevFront, prevBack )        _GLMaterialfv( GL_FRONT_AND_BACK, pname, defParams )
# define GL_RESET_MATERIAL_FV_CM( face, pname, defParams, cache, prev, cm )                           _GLMaterialfv( face, pname, defParams )
# define GL_RESET_MATERIAL_FV2_CM( pname, defParams, cacheFront, cacheBack, prevFront, prevBack, cm ) _GLMaterialfv( GL_FRONT_AND_BACK, pname, defParams )
#endif
#endif // DOXYGEN_IGNORE

//! Cached version of GLMaterialf( GL_BACK, GL_SHININESS, param )
/** This function caches the GL_SHININESS value on the GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_BACK, GL_SHININESS, param ) directly. */
inline  void  GLMaterialShininessBack( GLfloat param )
{
  GL_MATERIAL_F( GL_BACK, GL_SHININESS, param
               , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_BACK]
               , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_BACK] );
}

/*! \brief Helper function to reset the material back face shininess.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back face shininess to it's previous state. Otherwise, it is set to 0.0f.
 *  \sa GLMaterialShininessBack */
inline void GLResetMaterialShininessBack()
{
  GL_RESET_MATERIAL_F( GL_BACK, GL_SHININESS, 0.0f
                     , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_BACK]
                     , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_BACK] );
}

//! Cached version of GLMaterialf( GL_FRONT, GL_SHININESS, param )
/** This function caches the GL_SHININESS value on the GL_FRONT face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT, GL_SHININESS, param ) directly. */
inline  void  GLMaterialShininessFront( GLfloat param )
{
  GL_MATERIAL_F( GL_FRONT, GL_SHININESS, param
               , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_FRONT]
               , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_FRONT] );
}

/*! \brief Helper function to reset the material front face shininess.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  front face shininess to it's previous state. Otherwise, it is set to 0.0f.
 *  \sa GLMaterialShininessFront */
inline void GLResetMaterialShininessFront()
{
  GL_RESET_MATERIAL_F( GL_FRONT, GL_SHININESS, 0.0f
                     , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_FRONT]
                     , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_FRONT] );
}

//! Cached version of GLMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, param )
/** This function caches the GL_SHININESS value on both the GL_FRONT and GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT_AND_BACK, GL_SHININESS, param ) directly. */
inline  void  GLMaterialShininessFrontAndBack( GLfloat param )
{
  GL_MATERIAL_F2( GL_SHININESS, param
                , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_FRONT]
                , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_BACK]
                , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_FRONT]
                , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_BACK] );
}

/*! \brief Helper function to reset the material back and front face shininess.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back and front face shininess to it's previous state. Otherwise, it is set to 0.0f.
 *  \sa GLMaterialShininessFrontAndBack */
inline void GLResetMaterialShininessFrontAndBack()
{
  GL_RESET_MATERIAL_F2( GL_SHININESS, 0.0f
                      , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_FRONT]
                      , &glContext()->m_lightingAttributes.m_materialShininess[NVSG_CACHE_BACK]
                      , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_FRONT]
                      , &glContext()->m_previousAttributes.m_materialShininess[NVSG_CACHE_BACK] );
}

//! Cached version of GLMaterialf( face, GL_SHININESS, param )
/** This function caches the GL_SHININESS value of the material.
  * It is faster to call this method instead of GLMaterialf(face,GL_SHININESS,param) directly. */
inline  void  GLMaterialShininess( GLenum face, GLfloat param )
{
#if defined( GL_CACHING )
  switch( face )
  {
    case GL_BACK :
      GLMaterialShininessBack( param );
      break;
    case GL_FRONT :
      GLMaterialShininessFront( param );
      break;
    case GL_FRONT_AND_BACK :
      GLMaterialShininessFrontAndBack( param );
      break;
    default :
      NVSG_ASSERT( false );
      break;
  }
#else
  GLMaterialf( face, GL_SHININESS, param );
#endif
}

//! Cached version of GLMaterialf( GL_BACK, GL_AMBIENT, param )
/** This function caches the GL_AMBIENT value on the GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_BACK, GL_AMBIENT, param ) directly. */
inline  void  GLMaterialAmbientBack( const GLfloat *params )
{
  GL_MATERIAL_FV_CM( GL_BACK, GL_AMBIENT, params
                   , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                   , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                   , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

/*! \brief Helper function to reset the material back face ambient color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back face ambient color to it's previous state. Otherwise, it is set to (0.2,0.2,0.2,1.0).
 *  \sa GLMaterialAmbientBack */
inline void GLResetMaterialAmbientBack()
{
  static nvmath::Vec4f ma(0.2f,0.2f,0.2f,1.0f);
  GL_RESET_MATERIAL_FV_CM( GL_BACK, GL_AMBIENT, &ma[0]
                         , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                         , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                         , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

//! Cached version of GLMaterialf( GL_FRONT, GL_AMBIENT, param )
/** This function caches the GL_AMBIENT value on the GL_FRONT face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT, GL_AMBIENT, param ) directly. */
inline  void  GLMaterialAmbientFront( const GLfloat *params )
{
  GL_MATERIAL_FV_CM( GL_FRONT, GL_AMBIENT, params
                   , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                   , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                   , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

/*! \brief Helper function to reset the material front face ambient color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  front face ambient color to it's previous state. Otherwise, it is set to (0.2,0.2,0.2,1.0).
 *  \sa GLMaterialAmbientFront */
inline void GLResetMaterialAmbientFront()
{
  static nvmath::Vec4f ma(0.2f,0.2f,0.2f,1.0f);
  GL_RESET_MATERIAL_FV_CM( GL_FRONT, GL_AMBIENT, &ma[0]
                         , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                         , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                         , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

//! Cached version of GLMaterialf( GL_FRONT_AND_BACK, GL_AMBIENT, param )
/** This function caches the GL_AMBIENT value on both the GL_FRONT and GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT_AND_BACK, GL_AMBIENT, param ) directly. */
inline  void  GLMaterialAmbientFrontAndBack( const GLfloat *params )
{
  GL_MATERIAL_FV2_CM( GL_AMBIENT, params
                    , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                    , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                    , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                    , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                    , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

/*! \brief Helper function to reset the material back and front face ambient color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back and front face ambient color to it's previous state. Otherwise, it is set to
 *  (0.2,0.2,0.2,1.0).
 *  \sa GLMaterialAmbientFrontAndBack */
inline void GLResetMaterialAmbientFrontAndBack()
{
  static nvmath::Vec4f ma(0.2f,0.2f,0.2f,1.0f);
  GL_RESET_MATERIAL_FV2_CM( GL_AMBIENT, &ma[0]
                          , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                          , &glContext()->m_lightingAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                          , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_FRONT][0]
                          , &glContext()->m_previousAttributes.m_materialAmbient[NVSG_CACHE_BACK][0]
                          , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

//! Cached version of GLMaterialfv( face, GL_AMBIENT, params )
/** This function caches the GL_AMBIENT value of the material.
  * It is faster to call this method instead of GLMaterialfv(face,GL_AMBIENT,params) directly. */
inline  void  GLMaterialAmbient( GLenum face, const GLfloat *params )
{
#if defined( GL_CACHING )
  switch( face )
  {
    case GL_BACK :
      GLMaterialAmbientBack( params );
      break;
    case GL_FRONT :
      GLMaterialAmbientFront( params );
      break;
    case GL_FRONT_AND_BACK :
      GLMaterialAmbientFrontAndBack( params );
      break;
    default :
      NVSG_ASSERT( false );
      break;
  }
#else
  GLMaterialfv( face, GL_AMBIENT, params );
#endif
}

//! Cached version of GLMaterialf( GL_BACK, GL_DIFFUSE, param )
/** This function caches the GL_DIFFUSE value on the GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_BACK, GL_DIFFUSE, param ) directly. */
inline  void  GLMaterialDiffuseBack( const GLfloat *params )
{
  GL_MATERIAL_FV_CM( GL_BACK, GL_DIFFUSE, params
                   , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                   , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                   , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

/*! \brief Helper function to reset the material back face diffuse color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back face diffuse color to it's previous state. Otherwise, it is set to (0.8,0.8,0.8,1.0).
 *  \sa GLMaterialDiffuseBack */
inline void GLResetMaterialDiffuseBack()
{
  static nvmath::Vec4f md(0.8f,0.8f,0.8f,1.0f);
  GL_RESET_MATERIAL_FV_CM( GL_BACK, GL_DIFFUSE, &md[0]
                          , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                          , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                          , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

//! Cached version of GLMaterialf( GL_FRONT, GL_DIFFUSE, param )
/** This function caches the GL_DIFFUSE value on the GL_FRONT face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT, GL_DIFFUSE, param ) directly. */
inline  void  GLMaterialDiffuseFront( const GLfloat *params )
{
  GL_MATERIAL_FV_CM( GL_FRONT, GL_DIFFUSE, params
                   , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                   , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                   , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

/*! \brief Helper function to reset the material front face diffuse color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  front face diffuse color to it's previous state. Otherwise, it is set to (0.8,0.8,0.8,1.0).
 *  \sa GLMaterialDiffuseFront */
inline void GLResetMaterialDiffuseFront()
{
  static nvmath::Vec4f md(0.8f,0.8f,0.8f,1.0f);
  GL_RESET_MATERIAL_FV_CM( GL_FRONT, GL_DIFFUSE, &md[0]
                         , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                         , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                         , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

//! Cached version of GLMaterialf( GL_FRONT_AND_BACK, GL_DIFFUSE, param )
/** This function caches the GL_DIFFUSE value on both the GL_FRONT and GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT_AND_BACK, GL_DIFFUSE, param ) directly. */
inline  void  GLMaterialDiffuseFrontAndBack( const GLfloat *params )
{
  GL_MATERIAL_FV2_CM( GL_DIFFUSE, params
                    , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                    , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                    , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                    , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                    , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

/*! \brief Helper function to reset the material back and front face diffuse color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back and front face diffuse color to it's previous state. Otherwise, it is set to
 *  (0.8,0.8,0.8,1.0).
 *  \sa GLMaterialDiffuseFrontAndBack */
inline void GLResetMaterialDiffuseFrontAndBack()
{
  static nvmath::Vec4f md(0.8f,0.8f,0.8f,1.0f);
  GL_RESET_MATERIAL_FV2_CM( GL_DIFFUSE, &md[0]
                          , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                          , &glContext()->m_lightingAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                          , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_FRONT][0]
                          , &glContext()->m_previousAttributes.m_materialDiffuse[NVSG_CACHE_BACK][0]
                          , !!glContext()->m_lightingAttributes.m_colorMaterial );
}

//! Cached version of GLMaterialfv( face, GL_DIFFUSE, params )
/** This function caches the GL_DIFFUSE value of the material.
  * It is faster to call this method instead of GLMaterialfv(face,GL_DIFFUSE,params) directly. */
inline  void  GLMaterialDiffuse( GLenum face, const GLfloat *params )
{
#if defined( GL_CACHING )
  switch( face )
  {
    case GL_BACK :
      GLMaterialDiffuseBack( params );
      break;
    case GL_FRONT :
      GLMaterialDiffuseFront( params );
      break;
    case GL_FRONT_AND_BACK :
      GLMaterialDiffuseFrontAndBack( params );
      break;
    default :
      NVSG_ASSERT( false );
      break;
  }
#else
  GLMaterialfv( face, GL_DIFFUSE, params );
#endif
}

//! Cached version of GLMaterialf( GL_BACK, GL_EMISSION, param )
/** This function caches the GL_EMISSION value on the GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_BACK, GL_EMISSION, param ) directly. */
inline  void  GLMaterialEmissionBack( const GLfloat *params )
{
  GL_MATERIAL_FV( GL_BACK, GL_EMISSION, params
                , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_BACK][0]
                , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_BACK][0] );
}

/*! \brief Helper function to reset the material back face emission color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back face emission color to it's previous state. Otherwise, it is set to (0,0,0,1).
 *  \sa GLMaterialEmissionBack */
inline void GLResetMaterialEmissionBack()
{
  static nvmath::Vec4f me(0.0f,0.0f,0.0f,1.0f);
  GL_RESET_MATERIAL_FV( GL_BACK, GL_EMISSION, &me[0]
                      , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_BACK][0]
                      , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_BACK][0] );
}

//! Cached version of GLMaterialf( GL_FRONT, GL_EMISSION, param )
/** This function caches the GL_EMISSION value on the GL_FRONT face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT, GL_EMISSION, param ) directly. */
inline  void  GLMaterialEmissionFront( const GLfloat *params )
{
  GL_MATERIAL_FV( GL_FRONT, GL_EMISSION, params
                , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_FRONT][0]
                , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_FRONT][0] );
}

/*! \brief Helper function to reset the material front face emission color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  front face emission color to it's previous state. Otherwise, it is set to (0,0,0,1).
 *  \sa GLMaterialEmissionFront */
inline void GLResetMaterialEmissionFront()
{
  static nvmath::Vec4f me(0.0f,0.0f,0.0f,1.0f);
  GL_RESET_MATERIAL_FV( GL_FRONT, GL_EMISSION, &me[0]
                      , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_FRONT][0]
                      , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_FRONT][0] );
}

//! Cached version of GLMaterialf( GL_FRONT_AND_BACK, GL_EMISSION, param )
/** This function caches the GL_EMISSION value on both the GL_FRONT and GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT_AND_BACK, GL_EMISSION, param ) directly. */
inline  void  GLMaterialEmissionFrontAndBack( const GLfloat *params )
{
  GL_MATERIAL_FV2( GL_EMISSION, params
                 , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_FRONT][0]
                 , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_BACK][0]
                 , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_FRONT][0]
                 , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_BACK][0] );
}

/*! \brief Helper function to reset the material back and front face emission color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back and front face emission color to it's previous state. Otherwise, it is set to (0,0,0,1).
 *  \sa GLMaterialEmissionFrontAndBack */
inline void GLResetMaterialEmissionFrontAndBack()
{
  static nvmath::Vec4f me(0.0f,0.0f,0.0f,1.0f);
  GL_RESET_MATERIAL_FV2( GL_EMISSION, &me[0]
                       , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_FRONT][0]
                       , &glContext()->m_lightingAttributes.m_materialEmission[NVSG_CACHE_BACK][0]
                       , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_FRONT][0]
                       , &glContext()->m_previousAttributes.m_materialEmission[NVSG_CACHE_BACK][0] );
}

//! Cached version of GLMaterialfv( face, GL_EMISSION, params )
/** This function caches the GL_EMISSION value of the material.
  * It is faster to call this method instead of GLMaterialfv(face,GL_EMISSION,params) directly. */
inline  void  GLMaterialEmission( GLenum face, const GLfloat *params )
{
#if defined( GL_CACHING )
  switch( face )
  {
    case GL_BACK :
      GLMaterialEmissionBack( params );
      break;
    case GL_FRONT :
      GLMaterialEmissionFront( params );
      break;
    case GL_FRONT_AND_BACK :
      GLMaterialEmissionFrontAndBack( params );
      break;
    default :
      NVSG_ASSERT( false );
      break;
  }
#else
  GLMaterialfv( face, GL_EMISSION, params );
#endif
}

//! Cached version of GLMaterialf( GL_BACK, GL_SPECULAR, param )
/** This function caches the GL_SPECULAR value on the GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_BACK, GL_SPECULAR, param ) directly. */
inline  void  GLMaterialSpecularBack( const GLfloat *params )
{
  GL_MATERIAL_FV( GL_BACK, GL_SPECULAR, params
                , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_BACK][0]
                , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_BACK][0] );
}

/*! \brief Helper function to reset the material back face specular color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back face specular color to it's previous state. Otherwise, it is set to (0,0,0,1).
 *  \sa GLMaterialSpecularBack */
inline void GLResetMaterialSpecularBack()
{
  static nvmath::Vec4f ms(0.0f,0.0f,0.0f,1.0f);
  GL_RESET_MATERIAL_FV( GL_BACK, GL_SPECULAR, &ms[0]
                      , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_BACK][0]
                      , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_BACK][0] );
}

//! Cached version of GLMaterialf( GL_FRONT, GL_SPECULAR, param )
/** This function caches the GL_SPECULAR value on the GL_FRONT face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT, GL_SPECULAR, param ) directly. */
inline  void  GLMaterialSpecularFront( const GLfloat *params )
{
  GL_MATERIAL_FV( GL_FRONT, GL_SPECULAR, params
                , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0]
                , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0] );
}

/*! \brief Helper function to reset the material front face specular color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  front face specular color to it's previous state. Otherwise, it is set to (0,0,0,1).
 *  \sa GLMaterialSpecularFront */
inline void GLResetMaterialSpecularFront()
{
  static nvmath::Vec4f ms(0.0f,0.0f,0.0f,1.0f);
  GL_RESET_MATERIAL_FV( GL_FRONT, GL_SPECULAR, &ms[0]
                      , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0]
                      , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0] );
}

//! Cached version of GLMaterialf( GL_FRONT_AND_BACK, GL_SPECULAR, param )
/** This function caches the GL_SPECULAR value on both the GL_FRONT and GL_BACK face of the material.
  * It is faster to call this method instead of GLMaterialf( GL_FRONT_AND_BACK, GL_SPECULAR, param ) directly. */
inline  void  GLMaterialSpecularFrontAndBack( const GLfloat *params )
{
  GL_MATERIAL_FV2( GL_SPECULAR, params
                 , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0]
                 , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_BACK][0]
                 , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0]
                 , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_BACK][0] );
}

/*! \brief Helper function to reset the material back and front face specular color.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the material
 *  back and front face specular color to it's previous state. Otherwise, it is set to (0,0,0,1).
 *  \sa GLMaterialSpecularFrontAndBack */
inline void GLResetMaterialSpecularFrontAndBack()
{
  static nvmath::Vec4f ms(0.0f,0.0f,0.0f,1.0f);
  GL_RESET_MATERIAL_FV2( GL_SPECULAR, &ms[0]
                       , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0]
                       , &glContext()->m_lightingAttributes.m_materialSpecular[NVSG_CACHE_BACK][0]
                       , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_FRONT][0]
                       , &glContext()->m_previousAttributes.m_materialSpecular[NVSG_CACHE_BACK][0] );
}

//! Cached version of GLMaterialfv( face, GL_SPECULAR, params )
/** This function caches the GL_SPECULAR value of the material.
  * It is faster to call this method instead of GLMaterialfv(face,GL_SPECULAR,params) directly. */
inline  void  GLMaterialSpecular( GLenum face, const GLfloat *params )
{
#if defined( GL_CACHING )
  switch( face )
  {
    case GL_BACK :
      GLMaterialSpecularBack( params );
      break;
    case GL_FRONT :
      GLMaterialSpecularFront( params );
      break;
    case GL_FRONT_AND_BACK :
      GLMaterialSpecularFrontAndBack( params );
      break;
    default :
      NVSG_ASSERT( false );
      break;
  }
#else
  GLMaterialfv( face, GL_SPECULAR, params );
#endif
}

//! Replacement of glMaterialf
/** This function caches on some face and pname values and asserts on the result before returning. */
inline  void  GLMaterialf( GLenum face, GLenum pname, GLfloat param )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_SHININESS :
      GLMaterialShininess( face, param );
      break;
    default :
      _GLMaterialf( face, pname, param );
      break;
  }
#else
  _GLMaterialf( face, pname, param );
#endif
}

//! Replacement of glMaterialfv
/** This function caches on some face and pname values and asserts on the result before returning. */
inline  void  GLMaterialfv( GLenum face, GLenum pname, const GLfloat *params )  
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_AMBIENT :
      GLMaterialAmbient( face, params );
      break;
    case GL_DIFFUSE :
      GLMaterialDiffuse( face, params );
      break;
    case GL_EMISSION :
      GLMaterialEmission( face, params );
      break;
    case GL_SPECULAR :
      GLMaterialSpecular( face, params );
      break;
    case GL_SHININESS :
      GLMaterialShininess( face, params[0] );
      break;
    default :
      _GLMaterialfv( face, pname, params );
      break;
  }
#else
  _GLMaterialfv( face, pname, params );
#endif
}

//! Replacement of glMateriali
/** This function caches on some face and pname values and asserts on the result before returning. */
inline  void  GLMateriali( GLenum face, GLenum pname, GLint param ) 
{ 
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_SHININESS :
      GLMaterialShininess( face, (GLfloat)param );
      break;
    default :
      _GLMateriali( face, pname, param );
      break;
  }
#else
  _GLMateriali( face, pname, param );
#endif
}

//! Replacement of glMaterialiv
/** This function caches on some face and pname values and asserts on the result before returning. */
inline  void  GLMaterialiv( GLenum face, GLenum pname, const GLint *params )  
{
#if defined( GL_CACHING )
  GLfloat fParams[4];
  switch( pname )
  {
    case GL_AMBIENT :
      GLintToGLfloat( params, fParams );
      GLMaterialAmbient( face, fParams );
      break;
    case GL_DIFFUSE :
      GLintToGLfloat( params, fParams );
      GLMaterialDiffuse( face, fParams );
      break;
    case GL_EMISSION :
      GLintToGLfloat( params, fParams );
      GLMaterialEmission( face, fParams );
      break;
    case GL_SPECULAR :
      GLintToGLfloat( params, fParams );
      GLMaterialSpecular( face, fParams );
      break;
    case GL_SHININESS :
      GLMaterialShininess( face, (GLfloat)params[0] );
      break;
    default :
      _GLMaterialiv( face, pname, params );
      break;
  }
#else
  _GLMaterialiv( face, pname, params );
#endif
}

//! Replacement of glMatrixMode
/** This function caches the matrix mode setting and asserts on the result before returning.  */
inline  void  GLMatrixMode( GLenum mode )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  if ( ctx->m_transformAttributes.m_matrixMode != mode )
#endif
  {
    NVSG_GL_STATS_MATRIX_MODE( mode );
    glMatrixMode( mode );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_transformAttributes.m_matrixMode = mode;
#endif
  }
}

//! Replacement of glMinmax
/** This function asserts on the result before returning.  */
inline  void  GLMinmax( GLenum target, GLenum internalformat, GLboolean sink )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMinmax( target, internalformat, sink );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMultiTexCoord1d
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1d( GLenum target, GLdouble s )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1d( target, s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1dv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1dv( GLenum target, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1dv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1f
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1f( GLenum target, GLfloat s )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1f( target, s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1fv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1fv( GLenum target, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1fv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1hNV
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1h( GLenum target, GLhalf s )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1h( target, s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1hv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1hv( GLenum target, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1hv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1i
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1i( GLenum target, GLint s )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1i( target, s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1iv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1iv( GLenum target, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1iv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1s
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1s( GLenum target, GLshort s )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1s( target, s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord1sv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord1sv( GLenum target, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord1sv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2d
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2d( GLenum target, GLdouble s, GLdouble t )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2d( target, s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2dv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2dv( GLenum target, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2dv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2f
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2f( GLenum target, GLfloat s, GLfloat t )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2f( target, s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2fv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2fv( GLenum target, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2fv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2hNV
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2h( GLenum target, GLhalf s, GLhalf t )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2h( target, s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2hv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2hv( GLenum target, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2hv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2i
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2i( GLenum target, GLint s, GLint t )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2i( target, s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2iv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2iv( GLenum target, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2iv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2s
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2s( GLenum target, GLshort s, GLshort t )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2s( target, s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord2sv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord2sv( GLenum target, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord2sv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3d
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3d( GLenum target, GLdouble s, GLdouble t, GLdouble r )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3d( target, s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3dv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3dv( GLenum target, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3dv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3f
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3f( GLenum target, GLfloat s, GLfloat t, GLfloat r )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3f( target, s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3fv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3fv( GLenum target, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3fv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3hNV
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3h( GLenum target, GLhalf s, GLhalf t, GLhalf r )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3h( target, s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3hv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3hv( GLenum target, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3hv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3i
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3i( GLenum target, GLint s, GLint t, GLint r )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3i( target, s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3iv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3iv( GLenum target, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3iv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3s
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3s( GLenum target, GLshort s, GLshort t, GLshort r )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3s( target, s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord3sv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord3sv( GLenum target, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord3sv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4d
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4d( GLenum target, GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4d( target, s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4dv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4dv( GLenum target, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4dv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4f
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4f( GLenum target, GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4f( target, s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4fv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4fv( GLenum target, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4fv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4hNV
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4h( GLenum target, GLhalf s, GLhalf t, GLhalf r, GLhalf q )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4h( target, s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4hv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4hv( GLenum target, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4hv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4i
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4i( GLenum target, GLint s, GLint t, GLint r, GLint q )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4i( target, s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4iv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4iv( GLenum target, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4iv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4s
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4s( GLenum target, GLshort s, GLshort t, GLshort r, GLshort q )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4s( target, s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultiTexCoord4sv
/** This function asserts on the result before returning.  */
inline  void  GLMultiTexCoord4sv( GLenum target, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultiTexCoord4sv( target, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glMultMatrixd
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLMultMatrixd( const GLdouble *m )
{
  NVSG_GL_STATS_MULT_MATRIXD( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glMultMatrixd( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMultMatrixf
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLMultMatrixf( const GLfloat *m )
{
  NVSG_GL_STATS_MULT_MATRIXF( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glMultMatrixf( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMultTransposeMatrixd
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLMultTransposeMatrixd( const GLdouble *m )
{
  NVSG_GL_STATS_MULT_MATRIXD( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultTransposeMatrixd( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glMultTransposeMatrixf
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLMultTransposeMatrixf( const GLfloat *m )
{
  NVSG_GL_STATS_MULT_MATRIXF( m );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glMultTransposeMatrixf( m );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glNewList
/** This function asserts on the result before returning.  */
inline  void  GLNewList( GLuint list, GLenum mode )
{
  NVSG_GL_STATS_NEW_LIST( list, mode );
  glNewList( list, mode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glNormal3b
/** This function asserts on the result before returning.  */
inline  void  GLNormal3b( GLbyte nx, GLbyte ny, GLbyte nz )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3b( nx, ny, nz );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3bv
/** This function asserts on the result before returning.  */
inline  void  GLNormal3bv( const GLbyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3bv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3d
/** This function asserts on the result before returning.  */
inline  void  GLNormal3d( GLdouble nx, GLdouble ny, GLdouble nz )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3d( nx, ny, nz );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3dv
/** This function asserts on the result before returning.  */
inline  void  GLNormal3dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3f
/** This function asserts on the result before returning.  */
inline  void  GLNormal3f( GLfloat nx, GLfloat ny, GLfloat nz )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3f( nx, ny, nz );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3fv
/** This function asserts on the result before returning.  */
inline  void  GLNormal3fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3hNV
/** This function asserts on the result before returning.  */
inline  void  GLNormal3h( GLhalf nx, GLhalf ny, GLhalf nz )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glNormal3h( nx, ny, nz );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3hv
/** This function asserts on the result before returning.  */
inline  void  GLNormal3hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glNormal3hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

///! Replacement of glNormal3i
/** This function asserts on the result before returning.  */
inline  void  GLNormal3i( GLint nx, GLint ny, GLint nz )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3i( nx, ny, nz );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3iv
/** This function asserts on the result before returning.  */
inline  void  GLNormal3iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3s
/** This function asserts on the result before returning.  */
inline  void  GLNormal3s( GLshort nx, GLshort ny, GLshort nz )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3s( nx, ny, nz );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormal3sv
/** This function asserts on the result before returning.  */
inline  void  GLNormal3sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glNormal3sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glNormalPointer
/** This function caches the normal pointer and asserts on the result before returning.  */
inline  void  GLNormalPointer( GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_NORMAL_POINTER( type, stride, pointer );
  glNormalPointer( type, stride, pointer );    
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glOrtho
/** This function asserts on the result before returning.  */
inline  void  GLOrtho( GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
  NVSG_GL_STATS_ORTHO( left, right, bottom, top, zNear, zFar );
  glOrtho( left, right, bottom, top, zNear, zFar );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPassThrough
/** This function asserts on the result before returning.  */
inline  void  GLPassThrough( GLfloat token )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPassThrough( token );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelDataRangeNV
/** This function asserts on the result before returning.  */
inline  void  GLPixelDataRange( GLenum target, GLsizei size, GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPixelDataRange( target, size, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelMapfv
/** This function asserts on the result before returning.  */
inline  void  GLPixelMapfv( GLenum map, GLsizei mapsize, const GLfloat *values )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelMapfv( map, mapsize, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelMapuiv
/** This function asserts on the result before returning.  */
inline  void  GLPixelMapuiv( GLenum map, GLsizei mapsize, const GLuint *values )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelMapuiv( map, mapsize, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelMapusv
/** This function asserts on the result before returning.  */
inline  void  GLPixelMapusv( GLenum map, GLsizei mapsize, const GLushort *values )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelMapusv( map, mapsize, values );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelStoref
/** This function asserts on the result before returning.  */
inline  void  GLPixelStoref( GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelStoref( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelStorei
/** This function asserts on the result before returning.  */
inline  void  GLPixelStorei( GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelStorei( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelTransferf
/** This function asserts on the result before returning.  */
inline  void  GLPixelTransferf( GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelTransferf( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelTransferi
/** This function asserts on the result before returning.  */
inline  void  GLPixelTransferi( GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelTransferi( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPixelZoom
/** This function asserts on the result before returning.  */
inline  void  GLPixelZoom( GLfloat xfactor, GLfloat yfactor )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPixelZoom( xfactor, yfactor );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPointParameterf
/** This function asserts on the result before returning.  */
inline  void  GLPointParameterf( GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPointParameterf( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPointParameterfv
/** This function asserts on the result before returning.  */
inline  void  GLPointParameterfv( GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPointParameterfv( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPointParameteri
/** This function asserts on the result before returning.  */
inline  void  GLPointParameteri( GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPointParameteri( pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPointParameteriv
/** This function asserts on the result before returning.  */
inline  void  GLPointParameteriv( GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPointParameteriv( pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPointSize
/** This function caches the point size setting and asserts on the result before returning.  */
inline  void  GLPointSize( GLfloat size )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_pointSize = ctx->m_pointAttributes.m_size;
# endif
  if ( !equalf(ctx->m_pointAttributes.m_size, size) )
  {
# if !defined(NDEBUG)
    GLfloat f;
    glGetFloatv( GL_POINT_SIZE, &f );
    NVSG_ASSERT( equalf(ctx->m_pointAttributes.m_size, f) );
# endif
#endif
    NVSG_GL_STATS_POINT_SIZE( size );
    glPointSize( size );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_pointAttributes.m_size = size;
  }
#endif
}

/*! \brief Helper function to reset the point size.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the point size
 *  to it's previous state. Otherwise, it is set to 1.0f.
 *  \sa GLPointSize */
inline void GLResetPointSize()
{
#if defined(GL_SUPPORT_RESET)
  GLPointSize( glContext()->m_previousAttributes.m_pointSize );
#else
  GLPointSize( 1.0f );
#endif
}

//! Cached version of GLPolygonMode( GL_BACK, mode )
/** This function caches the polygon mode on the GL_BACK face of polygon.
  * It is faster to call this method instead of GLPolygonMode( GL_BACK, mode ) directly. */
inline  void  GLPolygonModeBack( GLenum mode )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_BACK] = ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK];
# endif
  if ( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK] != mode )
#endif
  {
    if ( ! ctx->m_locks.m_polygonMode )
    {
#if defined( GL_CACHING) && !defined( NDEBUG )
      GLint p[2];
      glGetIntegerv( GL_POLYGON_MODE, p );
      NVSG_ASSERT( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK] == p[1] );
#endif
      _GLPolygonMode( GL_BACK, mode );
#if defined( GL_CACHING )
      ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK] = mode;
#endif
    }
  }
}

/*! \brief Helper function to reset back face polygon mode.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the back face
 *  polygon mode to it's previous state. Otherwise, it is set to GL_FILL.
 *  \sa GLPolygonModeBack */
inline void GLResetPolygonModeBack()
{
#if defined(GL_SUPPORT_RESET)
  GLPolygonModeBack( glContext()->m_previousAttributes.m_rasterMode[NVSG_CACHE_BACK] );
#else
  GLPolygonModeBack( GL_FILL );
#endif
}

//! Cached version of GLPolygonMode( GL_FRONT, mode )
/** This function caches the polygon mode on the GL_FRONT face of polygon.
  * It is faster to call this method instead of GLPolygonMode( GL_FRONT, mode ) directly. */
inline  void  GLPolygonModeFront( GLenum mode )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_FRONT] = ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT];
# endif
  if ( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT] != mode )
#endif
  {
    if ( ! ctx->m_locks.m_polygonMode )
    {
#if defined( GL_CACHING ) && !defined( NDEBUG )
      GLint p[2];
      glGetIntegerv( GL_POLYGON_MODE, p );
      NVSG_ASSERT( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT] == p[0] );
#endif
      _GLPolygonMode( GL_FRONT, mode );
#if defined( GL_CACHING )
      ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT] = mode;
#endif
    }
  }
}

/*! \brief Helper function to reset front face polygon mode.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the front face
 *  polygon mode to it's previous state. Otherwise, it is set to GL_FILL.
 *  \sa GLPolygonModeFront */
inline void GLResetPolygonModeFront()
{
#if defined(GL_SUPPORT_RESET)
  GLPolygonModeFront( glContext()->m_previousAttributes.m_rasterMode[NVSG_CACHE_FRONT] );
#else
  GLPolygonModeFront( GL_FILL );
#endif
}

//! Cached version of GLPolygonMode( GL_FRONT_AND_BACK, mode )
/** This function caches the polygon mode on both the GL_FRONT and GL_BACK face of polygon.
  * It is faster to call this method instead of GLPolygonMode( GL_FRONT_AND_BACK, mode ) directly. */
inline  void  GLPolygonModeFrontAndBack( GLenum mode )
{
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_FRONT] = ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT];
  ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_BACK] = ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK];
# endif
  if (    ( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT] != mode )
      ||  ( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK] != mode ) )
#endif
  {
    if ( ! ctx->m_locks.m_polygonMode )
    {
#if defined( GL_CACHING ) && !defined( NDEBUG )
      GLint p[2];
      glGetIntegerv( GL_POLYGON_MODE, p );
      NVSG_ASSERT(    ( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK] == p[NVSG_CACHE_BACK] )
                  &&  ( ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT] == p[NVSG_CACHE_FRONT] ) );
#endif
      _GLPolygonMode( GL_FRONT_AND_BACK, mode );
#if defined( GL_CACHING )
      ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_BACK] = mode;
      ctx->m_polygonAttributes.m_rasterMode[NVSG_CACHE_FRONT] = mode;
#endif
    }
  }
}

/*! \brief Helper function to reset back and front face polygon mode.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the back and
 *  front face polygon mode to it's previous state. Otherwise, it is set to GL_FILL.
 *  \sa GLPolygonModeFrontAndBack, GLPolygonModeFront, GLPolygonMode Back */
inline void GLResetPolygonModeFrontAndBack()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  if ( ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_FRONT] == ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_BACK] )
  {
    GLPolygonModeFrontAndBack( ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_FRONT] );
  }
  else
  {
    GLPolygonModeFront( ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_FRONT] );
    GLPolygonModeBack( ctx->m_previousAttributes.m_rasterMode[NVSG_CACHE_BACK] );
  }
#else
  GLPolygonModeFrontAndBack( GL_FILL );
#endif
}

//! Cached version of GLPolygonMode( face, mode )
/** This function caches the polygon mode and assert on the result. */
inline  void  GLPolygonMode( GLenum face, GLenum mode )
{
#if defined( GL_CACHING )
  switch( face )
  {
    case GL_BACK :
      GLPolygonModeBack( mode );
      break;
    case GL_FRONT :
      GLPolygonModeFront( mode );
      break;
    case GL_FRONT_AND_BACK :
      GLPolygonModeFrontAndBack( mode );
      break;
    default :
      NVSG_ASSERT( false );
      break;
  }
#else
  _GLPolygonMode( face, mode );
#endif
}

/*! \brief Helper function to set the polygon mode and lock it.
 *  \param face The face for which the polygon mode is to set and lock.
 *  \param mode The polygon mode to set and lock.
 *  \remarks This function sets the polygon mode for \a face to \a mode and locks it. Any subsequent
 *  calls to GLPolygonMode, GLPolygonModeBack, or GLPolygonModeFront on the same \a face will not
 *  change that setting, until the next call to GLPolygonModeUnlock.
 *  \sa GLPolygonMode, GLPolygonModeUnlock */
inline void GLPolygonModeLock( GLenum face, GLenum mode )
{
  NVSG_ASSERT( ! glContext()->m_locks.m_polygonMode );
  GLPolygonMode( face, mode );
  glContext()->m_locks.m_polygonMode = true;
}

/*! \brief Helper function to unlock the polygon mode and set it.
 *  \param face The face for which the polygon mode is to be unlocked and set.
 *  \param mode The polygon mode to set.
 *  \remarks This function unlocks the polygon mode for \a face and sets it to \a mode.
 *  \sa GLPolygonMode, GLPolygonModeLock */
inline void GLPolygonModeUnlock( GLenum face, GLenum mode )
{
  glContext()->m_locks.m_polygonMode = false;
  GLPolygonMode( face, mode );
}

//! Replacement of glPolygonOffset
/** This function caches the polygon offset settings and asserts on the result before returning.  */
inline  void  GLPolygonOffset( GLfloat factor, GLfloat units )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_polygonOffsetFactor = ctx->m_polygonAttributes.m_offsetFactor;
  ctx->m_previousAttributes.m_polygonOffsetBias   = ctx->m_polygonAttributes.m_offsetBias;
# endif
  if (  !equalf(ctx->m_polygonAttributes.m_offsetFactor, factor) 
     || !equalf(ctx->m_polygonAttributes.m_offsetBias, units) )
#endif
  {
    NVSG_GL_STATS_POLYGON_OFFSET( factor, units );
    glPolygonOffset( factor, units );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_polygonAttributes.m_offsetFactor  = factor;
    ctx->m_polygonAttributes.m_offsetBias   = units;
#endif
  }
}

/*! \brief Helper function to reset the polygon offset.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the polygon
 *  offset to it's previous state. Otherwise, it is set to (0,0).
 *  \sa GLPolygonOffset */
inline void GLResetPolygonOffset()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLPolygonOffset( ctx->m_previousAttributes.m_polygonOffsetFactor
                 , ctx->m_previousAttributes.m_polygonOffsetBias );
#else
  GLPolygonOffset( 0.0f, 0.0f );
#endif
}

//! Replacement of glPolygonStipple
/** This function asserts on the result before returning.  */
inline  void  GLPolygonStipple( const GLubyte *mask )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPolygonStipple( mask );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPopAttrib
/** This function asserts on the result before returning.
  * \note Currently we don't hold a stack of cached GL states, so GLPushAttrib/GLPopAttrib doesn't affect our cached values.
  * It is therefore recommended to used GLPushAttrib/GLPopAttrib very local and to not use other potentially caching
  * GL* functions (with uppercase GL), inbetween such a pair. */
inline  void  GLPopAttrib( void )
{
  NVSG_GL_STATS_POP_ATTRIB();
  glPopAttrib();
  ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  GLbitfield mask = ctx->m_pushMasks.top();
  if ( mask == GL_ALL_ATTRIB_BITS )
  {
    ctx->m_colorBufferAttributes = ctx->m_colorBufferAttributesStack.top();
    ctx->m_colorBufferAttributesStack.pop();
    ctx->m_colorBufferEnableAttributes = ctx->m_colorBufferEnableAttributesStack.top();
    ctx->m_colorBufferEnableAttributesStack.pop();
    ctx->m_depthBufferAttributes = ctx->m_depthBufferAttributesStack.top();
    ctx->m_depthBufferAttributesStack.pop();
    ctx->m_depthBufferEnableAttributes = ctx->m_depthBufferEnableAttributesStack.top();
    ctx->m_depthBufferEnableAttributesStack.pop();
    ctx->m_evalAttributes = ctx->m_evalAttributesStack.top();
    ctx->m_evalAttributesStack.pop();
    ctx->m_fogAttributes = ctx->m_fogAttributesStack.top();
    ctx->m_fogAttributesStack.pop();
    ctx->m_fogEnableAttributes = ctx->m_fogEnableAttributesStack.top();
    ctx->m_fogEnableAttributesStack.pop();
    ctx->m_lightingAttributes = ctx->m_lightingAttributesStack.top();
    ctx->m_lightingAttributesStack.pop();
    ctx->m_lightingEnableAttributes = ctx->m_lightingEnableAttributesStack.top();
    ctx->m_lightingEnableAttributesStack.pop();
    ctx->m_lineAttributes = ctx->m_lineAttributesStack.top();
    ctx->m_lineAttributesStack.pop();
    ctx->m_lineEnableAttributes = ctx->m_lineEnableAttributesStack.top();
    ctx->m_lineEnableAttributesStack.pop();
    ctx->m_pointAttributes = ctx->m_pointAttributesStack.top();
    ctx->m_pointAttributesStack.pop();
    ctx->m_pointEnableAttributes = ctx->m_pointEnableAttributesStack.top();
    ctx->m_pointEnableAttributesStack.pop();
    ctx->m_polygonAttributes = ctx->m_polygonAttributesStack.top();
    ctx->m_polygonAttributesStack.pop();
    ctx->m_polygonEnableAttributes = ctx->m_polygonEnableAttributesStack.top();
    ctx->m_polygonEnableAttributesStack.pop();
    ctx->m_scissorAttributes = ctx->m_scissorAttributesStack.top();
    ctx->m_scissorAttributesStack.pop();
    ctx->m_scissorEnableAttributes = ctx->m_scissorEnableAttributesStack.top();
    ctx->m_scissorEnableAttributesStack.pop();
    ctx->m_stencilBufferAttributes = ctx->m_stencilBufferAttributesStack.top();
    ctx->m_stencilBufferAttributesStack.pop();
    ctx->m_stencilBufferEnableAttributes = ctx->m_stencilBufferEnableAttributesStack.top();
    ctx->m_stencilBufferEnableAttributesStack.pop();
    ctx->m_textureAttributes = ctx->m_textureAttributesStack.top();
    ctx->m_textureAttributesStack.pop();
    ctx->m_textureEnableAttributes = ctx->m_textureEnableAttributesStack.top();
    ctx->m_textureEnableAttributesStack.pop();
    ctx->m_transformAttributes = ctx->m_transformAttributesStack.top();
    ctx->m_transformAttributesStack.pop();
    ctx->m_transformEnableAttributes = ctx->m_transformEnableAttributesStack.top();
    ctx->m_transformEnableAttributesStack.pop();
    ctx->m_viewportAttributes = ctx->m_viewportAttributesStack.top();
    ctx->m_viewportAttributesStack.pop();
  }
  else
  {
    if ( mask & GL_COLOR_BUFFER_BIT )
    {
      ctx->m_colorBufferAttributes = ctx->m_colorBufferAttributesStack.top();
      ctx->m_colorBufferAttributesStack.pop();
    }
    if ( mask & GL_DEPTH_BUFFER_BIT )
    {
      ctx->m_depthBufferAttributes = ctx->m_depthBufferAttributesStack.top();
      ctx->m_depthBufferAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT ) )
    {
      ctx->m_colorBufferEnableAttributes = ctx->m_colorBufferEnableAttributesStack.top();
      ctx->m_colorBufferEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT ) )
    {
      ctx->m_depthBufferEnableAttributes = ctx->m_depthBufferEnableAttributesStack.top();
      ctx->m_depthBufferEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_FOG_BIT ) )
    {
      ctx->m_fogEnableAttributes = ctx->m_fogEnableAttributesStack.top();
      ctx->m_fogEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_LIGHTING_BIT ) )
    {
      ctx->m_lightingEnableAttributes = ctx->m_lightingEnableAttributesStack.top();
      ctx->m_lightingEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_LINE_BIT ) )
    {
      ctx->m_lineEnableAttributes = ctx->m_lineEnableAttributesStack.top();
      ctx->m_lineEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_POINT_BIT ) )
    {
      ctx->m_pointEnableAttributes = ctx->m_pointEnableAttributesStack.top();
      ctx->m_pointEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_POLYGON_BIT ) )
    {
      ctx->m_polygonEnableAttributes = ctx->m_polygonEnableAttributesStack.top();
      ctx->m_polygonEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_SCISSOR_BIT ) )
    {
      ctx->m_scissorEnableAttributes = ctx->m_scissorEnableAttributesStack.top();
      ctx->m_scissorEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_STENCIL_BUFFER_BIT ) )
    {
      ctx->m_stencilBufferEnableAttributes = ctx->m_stencilBufferEnableAttributesStack.top();
      ctx->m_stencilBufferEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_TEXTURE_BIT ) )
    {
      ctx->m_textureEnableAttributes = ctx->m_textureEnableAttributesStack.top();
      ctx->m_textureEnableAttributesStack.pop();
    }
    if ( mask & ( GL_ENABLE_BIT | GL_TRANSFORM_BIT ) )
    {
      ctx->m_transformEnableAttributes = ctx->m_transformEnableAttributesStack.top();
      ctx->m_transformEnableAttributesStack.pop();
    }
    if ( mask & GL_EVAL_BIT )
    {
      ctx->m_evalAttributes = ctx->m_evalAttributesStack.top();
      ctx->m_evalAttributesStack.pop();
    }
    if ( mask & GL_FOG_BIT )
    {
      ctx->m_fogAttributes = ctx->m_fogAttributesStack.top();
      ctx->m_fogAttributesStack.pop();
    }
    if ( mask & GL_LIGHTING_BIT )
    {
      ctx->m_lightingAttributes = ctx->m_lightingAttributesStack.top();
      ctx->m_lightingAttributesStack.pop();
    }
    if ( mask & GL_LINE_BIT )
    {
      ctx->m_lineAttributes = ctx->m_lineAttributesStack.top();
      ctx->m_lineAttributesStack.pop();
    }
    if ( mask & GL_POINT_BIT )
    {
      ctx->m_pointAttributes = ctx->m_pointAttributesStack.top();
      ctx->m_pointAttributesStack.pop();
    }
    if ( mask & GL_POLYGON_BIT )
    {
      ctx->m_polygonAttributes = ctx->m_polygonAttributesStack.top();
      ctx->m_polygonAttributesStack.pop();
    }
    if ( mask & GL_SCISSOR_BIT )
    {
      ctx->m_scissorAttributes = ctx->m_scissorAttributesStack.top();
      ctx->m_scissorAttributesStack.pop();
    }
    if ( mask & GL_STENCIL_BUFFER_BIT )
    {
      ctx->m_stencilBufferAttributes = ctx->m_stencilBufferAttributesStack.top();
      ctx->m_stencilBufferAttributesStack.pop();
    }
    if ( mask & GL_TEXTURE_BIT )
    {
      ctx->m_textureAttributes = ctx->m_textureAttributesStack.top();
      ctx->m_textureAttributesStack.pop();
    }
    if ( mask & GL_TRANSFORM_BIT )
    {
      ctx->m_transformAttributes = ctx->m_transformAttributesStack.top();
      ctx->m_transformAttributesStack.pop();
    }
    if ( mask & GL_VIEWPORT_BIT )
    {
      ctx->m_viewportAttributes = ctx->m_viewportAttributesStack.top();
      ctx->m_viewportAttributesStack.pop();
    }
  }
  ctx->m_pushMasks.pop();
#endif
}

//! Replacement of glPopClientAttrib
/** This function asserts on the result before returning.
  * \note Currently we don't hold a stack of cached GL states, so GLPushClientAttrib/GLPopClientAttrib doesn't affect our cached
  * values. It is therefore recommended to used GLPushAttrib/GLPopAttrib very local and to not use other potentially caching
  * GL* functions (with uppercase GL), inbetween such a pair. */
inline  void  GLPopClientAttrib( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPopClientAttrib();
  ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  GLbitfield mask = ctx->m_clientPushMasks.top();
  if ( mask & GL_CLIENT_VERTEX_ARRAY_BIT )
  {
    ctx->m_vertexArrayAttributes = ctx->m_vertexArrayAttributesStack.top();
    ctx->m_vertexArrayAttributesStack.pop();
  }

  ctx->m_clientPushMasks.pop();
#endif
}

//! Replacement of glPopMatrix
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLPopMatrix( void )
{
  NVSG_GL_STATS_POP_MATRIX();
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glPopMatrix();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPopName
/** This function asserts on the result before returning.  */
inline  void  GLPopName( void )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPopName();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPrimitiveRestart
/** This function asserts on the result before returning.  */
inline  void  GLPrimitiveRestart()
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPrimitiveRestart();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPrimitiveRestartIndex
/** This function caches the primitive restart index and asserts on the result before returning.  */
inline  void  GLPrimitiveRestartIndex( GLuint index )
{
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLContext * ctx = glContext();
#if defined( GL_CACHING )
  if ( ctx->m_vertexArrayAttributes.m_primitiveRestartIndex != index )
#endif
  {
    NVSG_GL_STATS_PRIMITIVE_RESTART_INDEX( index );
    ctx->m_glFunctions->glPrimitiveRestartIndex( index );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_vertexArrayAttributes.m_primitiveRestartIndex = index;
#endif
  }
}

//! Replacement of glPrioritizeTextures
/** This function asserts on the result before returning.  */
inline  void  GLPrioritizeTextures( GLsizei n, const GLuint *textures, const GLclampf *priorities )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPrioritizeTextures( n, textures, priorities );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramBufferParametersfvNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramBufferParametersfv( GLenum target, GLuint buffer, GLuint index, GLsizei count, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramBufferParametersfv( target, buffer, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramBufferParametersIivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramBufferParametersIiv( GLenum target, GLuint buffer, GLuint index, GLsizei count, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramBufferParametersIiv( target, buffer, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramBufferParametersIuivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramBufferParametersIuiv( GLenum target, GLuint buffer, GLuint index, GLsizei count, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramBufferParametersIuiv( target, buffer, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameter4dARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameter4d(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameter4dARB( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameter4dvARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameter4dv(GLenum target, GLuint index, const GLdouble *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameter4dvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameter4fARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameter4f(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameter4fARB( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameter4fvARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameter4fv(GLenum target, GLuint index, const GLfloat *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameter4fvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}


//! Replacement of glProgramEnvParametersI4ivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParametersI4iv( GLenum target, GLuint index, GLsizei count, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParametersI4iv( target, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParametersI4uivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParametersI4uiv( GLenum target, GLuint index, GLsizei count, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParametersI4uiv( target, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameterI4iNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameterI4i( GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameterI4i( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameterI4ivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameterI4iv( GLenum target, GLuint index, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameterI4iv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameterI4uiNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameterI4ui( GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameterI4ui( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameterI4uivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameterI4uiv( GLenum target, GLuint index, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameterI4uiv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramEnvParameters4fvEXT
/** This function asserts on the result before returning.  */
inline  void  GLProgramEnvParameters4fv( GLenum target, GLuint index, GLsizei count, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramEnvParameters4fv( target, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameter4dARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameter4d(GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameter4dARB( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameter4dvARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameter4dv(GLenum target, GLuint index, const GLdouble *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameter4dvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameter4fARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameter4f(GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameter4fARB( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameter4fvARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameter4fv(GLenum target, GLuint index, const GLfloat *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameter4fvARB( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParametersI4ivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParametersI4iv( GLenum target, GLuint index, GLsizei count, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParametersI4iv( target, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParametersI4uivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParametersI4uiv( GLenum target, GLuint index, GLsizei count, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParametersI4uiv( target, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameterI4iNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameterI4i( GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameterI4i( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameterI4ivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameterI4iv( GLenum target, GLuint index, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameterI4iv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameterI4uiNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameterI4ui( GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameterI4ui( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameterI4uivNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameterI4uiv( GLenum target, GLuint index, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameterI4uiv( target, index, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramLocalParameters4fvEXT
/** This function asserts on the result before returning.  */
inline  void  GLProgramLocalParameters4fv( GLenum target, GLuint index, GLsizei count, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramLocalParameters4fv( target, index, count, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramNamedParameter4dNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramNamedParameter4d( GLuint id, GLsizei len, const GLubyte *name, GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramNamedParameter4d( id, len, name, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramNamedParameter4dvNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramNamedParameter4dv( GLuint id, GLsizei len, const GLubyte *name, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramNamedParameter4dv( id, len, name, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramNamedParameter4fNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramNamedParameter4f( GLuint id, GLsizei len, const GLubyte *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramNamedParameter4f( id, len, name, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramNamedParameter4fvNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramNamedParameter4fv( GLuint id, GLsizei len, const GLubyte *name, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramNamedParameter4fv( id, len, name, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramParameter4dNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramParameter4d( GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramParameter4dNV( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramParameter4dvNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramParameter4dv( GLenum target, GLuint index, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramParameter4dvNV( target, index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramParameter4fNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramParameter4f( GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramParameter4fNV( target, index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramParameter4fvNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramParameter4fv( GLenum target, GLuint index, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramParameter4fvNV( target, index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramParameteriEXT
/** This function asserts on the result before returning.  */
inline  void  GLProgramParameteri( GLuint program, GLenum pname, GLint value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramParameteri( program, pname, value );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramParameters4dvNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramParameters4dv( GLenum target, GLuint index, GLsizei count, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramParameters4dvNV( target, index, count, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramParameters4fvNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramParameters4fv( GLenum target, GLuint index, GLsizei count, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramParameters4fvNV( target, index, count, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramStringARB
/** This function asserts on the result before returning.  */
inline  void  GLProgramString( GLenum target, GLenum format, GLsizei len, const GLvoid *string )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramStringARB( target, format, len, string );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glProgramVertexLimitNV
/** This function asserts on the result before returning.  */
inline  void  GLProgramVertexLimit( GLenum target, GLint limit )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glProgramVertexLimit( target, limit );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPushAttrib
/** This function asserts on the result before returning.
  * \note Currently we don't hold a stack of cached GL states, so GLPushAttrib/GLPopAttrib doesn't affect our cached values.
  * It is therefore recommended to used GLPushAttrib/GLPopAttrib very local and to not use other potentially caching
  * GL* functions (with uppercase GL), inbetween such a pair. */
inline  void  GLPushAttrib( GLbitfield mask )
{
  NVSG_GL_STATS_PUSH_ATTRIB( mask );
  glPushAttrib( mask );
  ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  ctx->m_pushMasks.push( mask );
  if ( mask == GL_ALL_ATTRIB_BITS )
  {
    ctx->m_colorBufferAttributesStack.push( ctx->m_colorBufferAttributes );
    ctx->m_colorBufferEnableAttributesStack.push( ctx->m_colorBufferEnableAttributes );
    ctx->m_depthBufferAttributesStack.push( ctx->m_depthBufferAttributes );
    ctx->m_depthBufferEnableAttributesStack.push( ctx->m_depthBufferEnableAttributes );
    ctx->m_evalAttributesStack.push( ctx->m_evalAttributes );
    ctx->m_fogAttributesStack.push( ctx->m_fogAttributes );
    ctx->m_fogEnableAttributesStack.push( ctx->m_fogEnableAttributes );
    ctx->m_lightingAttributesStack.push( ctx->m_lightingAttributes );
    ctx->m_lightingEnableAttributesStack.push( ctx->m_lightingEnableAttributes );
    ctx->m_lineAttributesStack.push( ctx->m_lineAttributes );
    ctx->m_lineEnableAttributesStack.push( ctx->m_lineEnableAttributes );
    ctx->m_pointAttributesStack.push( ctx->m_pointAttributes );
    ctx->m_pointEnableAttributesStack.push( ctx->m_pointEnableAttributes );
    ctx->m_polygonAttributesStack.push( ctx->m_polygonAttributes );
    ctx->m_polygonEnableAttributesStack.push( ctx->m_polygonEnableAttributes );
    ctx->m_scissorAttributesStack.push( ctx->m_scissorAttributes );
    ctx->m_scissorEnableAttributesStack.push( ctx->m_scissorEnableAttributes );
    ctx->m_stencilBufferAttributesStack.push( ctx->m_stencilBufferAttributes );
    ctx->m_stencilBufferEnableAttributesStack.push( ctx->m_stencilBufferEnableAttributes );
    ctx->m_textureAttributesStack.push( ctx->m_textureAttributes );
    ctx->m_textureEnableAttributesStack.push( ctx->m_textureEnableAttributes );
    ctx->m_transformAttributesStack.push( ctx->m_transformAttributes );
    ctx->m_transformEnableAttributesStack.push( ctx->m_transformEnableAttributes );
    ctx->m_viewportAttributesStack.push( ctx->m_viewportAttributes );
  }
  else
  {
    if ( mask & GL_COLOR_BUFFER_BIT )
    {
      ctx->m_colorBufferAttributesStack.push( ctx->m_colorBufferAttributes );
    }
    if ( mask & GL_DEPTH_BUFFER_BIT )
    {
      ctx->m_depthBufferAttributesStack.push( ctx->m_depthBufferAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT ) )
    {
      ctx->m_colorBufferEnableAttributesStack.push( ctx->m_colorBufferEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_DEPTH_BUFFER_BIT ) )
    {
      ctx->m_depthBufferEnableAttributesStack.push( ctx->m_depthBufferEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_FOG_BIT ) )
    {
      ctx->m_fogEnableAttributesStack.push( ctx->m_fogEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_LIGHTING_BIT ) )
    {
      ctx->m_lightingEnableAttributesStack.push( ctx->m_lightingEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_LINE_BIT ) )
    {
      ctx->m_lineEnableAttributesStack.push( ctx->m_lineEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_POINT_BIT ) )
    {
      ctx->m_pointEnableAttributesStack.push( ctx->m_pointEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_POLYGON_BIT ) )
    {
      ctx->m_polygonEnableAttributesStack.push( ctx->m_polygonEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_SCISSOR_BIT ) )
    {
      ctx->m_scissorEnableAttributesStack.push( ctx->m_scissorEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_STENCIL_BUFFER_BIT ) )
    {
      ctx->m_stencilBufferEnableAttributesStack.push( ctx->m_stencilBufferEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_TEXTURE_BIT ) )
    {
      ctx->m_textureEnableAttributesStack.push( ctx->m_textureEnableAttributes );
    }
    if ( mask & ( GL_ENABLE_BIT | GL_TRANSFORM_BIT ) )
    {
      ctx->m_transformEnableAttributesStack.push( ctx->m_transformEnableAttributes );
    }
    if ( mask & GL_EVAL_BIT )
    {
      ctx->m_evalAttributesStack.push( ctx->m_evalAttributes );
    }
    if ( mask & GL_FOG_BIT )
    {
      ctx->m_fogAttributesStack.push( ctx->m_fogAttributes );
    }
    if ( mask & GL_LIGHTING_BIT )
    {
      ctx->m_lightingAttributesStack.push( ctx->m_lightingAttributes );
    }
    if ( mask & GL_LINE_BIT )
    {
      ctx->m_lineAttributesStack.push( ctx->m_lineAttributes );
    }
    if ( mask & GL_POINT_BIT )
    {
      ctx->m_pointAttributesStack.push( ctx->m_pointAttributes );
    }
    if ( mask & GL_POLYGON_BIT )
    {
      ctx->m_polygonAttributesStack.push( ctx->m_polygonAttributes );
    }
    if ( mask & GL_SCISSOR_BIT )
    {
      ctx->m_scissorAttributesStack.push( ctx->m_scissorAttributes );
    }
    if ( mask & GL_STENCIL_BUFFER_BIT )
    {
      ctx->m_stencilBufferAttributesStack.push( ctx->m_stencilBufferAttributes );
    }
    if ( mask & GL_TEXTURE_BIT )
    {
      ctx->m_textureAttributesStack.push( ctx->m_textureAttributes );
    }
    if ( mask & GL_TRANSFORM_BIT )
    {
      ctx->m_transformAttributesStack.push( ctx->m_transformAttributes );
    }
    if ( mask & GL_VIEWPORT_BIT )
    {
      ctx->m_viewportAttributesStack.push( ctx->m_viewportAttributes );
    }
  }
#endif
}

//! Replacement of glPushClientAttrib
/** This function asserts on the result before returning.
  * \note Currently we don't hold a stack of cached GL states, so GLPushClientAttrib/GLPopClientAttrib doesn't affect our cached
  * values. It is therefore recommended to used GLPushAttrib/GLPopAttrib very local and to not use other potentially caching
  * GL* functions (with uppercase GL), inbetween such a pair. */
inline  void  GLPushClientAttrib( GLbitfield mask )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPushClientAttrib( mask );
  ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
  ctx->m_clientPushMasks.push( mask );
  if ( mask & GL_CLIENT_VERTEX_ARRAY_BIT )
  {
    ctx->m_vertexArrayAttributesStack.push( ctx->m_vertexArrayAttributes );
  }
#endif
}

//! Replacement of glPushMatrix
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLPushMatrix( void )
{
  NVSG_GL_STATS_PUSH_MATRIX();
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glPushMatrix();
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glPushName
/** This function asserts on the result before returning.  */
inline  void  GLPushName( GLuint name )
{
  NVSG_GL_STATS_FCT_COUNT();
  glPushName( name );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2d
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2d( GLdouble x, GLdouble y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2d( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2f
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2f( GLfloat x, GLfloat y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2f( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2i
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2i( GLint x, GLint y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2i( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2s
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2s( GLshort x, GLshort y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2s( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3d
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3d( GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3d( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3f
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3f( GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3f( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3i
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3i( GLint x, GLint y, GLint z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3i( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3s
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3s( GLshort x, GLshort y, GLshort z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3s( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4d
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4d( x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4f
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4f( x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4i
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4i( GLint x, GLint y, GLint z, GLint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4i( x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4s
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4s( x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2dv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2dv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2fv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2fv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2iv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2iv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos2sv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos2sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos2sv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3dv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3dv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3fv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3fv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3iv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3iv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos3sv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos3sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos3sv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4dv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4dv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4fv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4fv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4iv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4iv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRasterPos4sv
/** This function asserts on the result before returning.  */
inline  void  GLRasterPos4sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRasterPos4sv( v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glReadBuffer
/** This function asserts on the result before returning.  */
inline  void  GLReadBuffer( GLenum mode ) 
{ 
  NVSG_GL_STATS_FCT_COUNT();
  glReadBuffer( mode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glReadPixels
/** This function asserts on the result before returning.  */
inline  void  GLReadPixels( GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  glReadPixels( x, y, width, height, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRectd
/** This function asserts on the result before returning.  */
inline  void  GLRectd( GLdouble x1, GLdouble y1, GLdouble x2, GLdouble y2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRectd( x1, y1, x2, y2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRectf
/** This function asserts on the result before returning.  */
inline  void  GLRectf( GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRectf( x1, y1, x2, y2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRecti
/** This function asserts on the result before returning.  */
inline  void  GLRecti( GLint x1, GLint y1, GLint x2, GLint y2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRecti( x1, y1, x2, y2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRects
/** This function asserts on the result before returning.  */
inline  void  GLRects( GLshort x1, GLshort y1, GLshort x2, GLshort y2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRects( x1, y1, x2, y2 );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRenderMode
/** This function asserts on the result before returning.  */
inline  void  GLRenderMode( GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  glRenderMode( mode );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRequestResidentProgramsNV
/** This function asserts on the result before returning.  */
inline  void  GLRequestResidentPrograms( GLsizei n, const GLuint *programs )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glRequestResidentProgramsNV( n, programs );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glResetHistogram
/** This function asserts on the result before returning.  */
inline  void  GLResetHistogram( GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glResetHistogram( target );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glResetMinmax
/** This function asserts on the result before returning.  */
inline  void  GLResetMinmax( GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glResetMinmax( target );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRotated
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLRotated( GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_ROTATED( angle, x, y, z );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glRotated( angle, x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glRotatef
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_ROTATEF( angle, x, y, z );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glRotatef( angle, x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glScaled
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLScaled( GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_SCALED( x, y, z );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glScaled( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glScalef
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLScalef( GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_SCALEF( x, y, z );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glScalef( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glSampleCoverage
/** This function asserts on the result before returning. */
inline  void  GLSampleCoverage( GLclampf value, GLboolean invert )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSampleCoverage( value, invert );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glScissor
/** This function caches the scissor settings and asserts on the result before returning.  */
inline  void  GLScissor( GLint x, GLint y, GLsizei width, GLsizei height )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_scissorX      = ctx->m_scissorAttributes.m_x;
  ctx->m_previousAttributes.m_scissorY      = ctx->m_scissorAttributes.m_y;
  ctx->m_previousAttributes.m_scissorWidth  = ctx->m_scissorAttributes.m_width;
  ctx->m_previousAttributes.m_scissorHeight = ctx->m_scissorAttributes.m_height;
# endif
  if (    ( ctx->m_scissorAttributes.m_x != x )
      ||  ( ctx->m_scissorAttributes.m_y != y )
      ||  ( ctx->m_scissorAttributes.m_width != width )
      ||  ( ctx->m_scissorAttributes.m_height != height ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glScissor( x, y, width, height );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_scissorAttributes.m_x = x;
    ctx->m_scissorAttributes.m_y = y;
    ctx->m_scissorAttributes.m_width = width;
    ctx->m_scissorAttributes.m_height = height;
#endif
  }
}

/*! \brief Helper function to reset the scissor rectangle.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the scissor
 *  rectangle to it's previous state. Otherwise, it does nothing.
 *  \sa GLScissor */
inline void GLResetScissor()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLScissor( ctx->m_previousAttributes.m_scissorX, ctx->m_previousAttributes.m_scissorY
           , ctx->m_previousAttributes.m_scissorWidth, ctx->m_previousAttributes.m_scissorHeight );
#else
#endif
}

//! Replacement of glSecondaryColor3b
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3b( GLbyte red, GLbyte green, GLbyte blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3b( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3bv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3bv( const GLbyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3bv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3d
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3d( GLdouble red, GLdouble green, GLdouble blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3d( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3dv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3f
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3f( GLfloat red, GLfloat green, GLfloat blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3f( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3fv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3h
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3h( GLhalf red, GLhalf green, GLhalf blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3h( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3hv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3i
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3i( GLint red, GLint green, GLint blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3i( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3iv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3s
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3s( GLshort red, GLshort green, GLshort blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3s( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3sv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3ub
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3ub( GLubyte red, GLubyte green, GLubyte blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3ub( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3ubv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3ubv( const GLubyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3ubv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3ui
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3ui( GLuint red, GLuint green, GLuint blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3ui( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3uiv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3uiv( const GLuint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3uiv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3us
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3us( GLushort red, GLushort green, GLushort blue )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3us( red, green, blue );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColor3usv
/** This function asserts on the result before returning.  */
inline  void  GLSecondaryColor3usv( const GLushort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColor3usv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glSecondaryColorPointer
/** This function caches the secondary color pointer setting and asserts on the result before returning.  */
inline  void  GLSecondaryColorPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSecondaryColorPointer( size, type, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glSelectBuffer
/** This function asserts on the result before returning.  */
inline  void  GLSelectBuffer( GLsizei size, GLuint *buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  glSelectBuffer( size, buffer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glSelectTexture
/** This function simply maps to GLActiveTexture. */
inline  void  GLSelectTexture( GLenum target )
{
  GLActiveTexture( target );
}

//! Replacement of GLSelectTextureCoordSet
/** This function simply maps to GLClientActiveTexture. */
inline  void  GLSelectTextureCoordSet( GLenum target )
{
  GLClientActiveTexture( target );
}

//! Replacement of glSeparableFilter2D
/** This function asserts on the result before returning.  */
inline  void  GLSeparableFilter2D( GLenum target, GLenum internalformat, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *row, const GLvoid *column )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSeparableFilter2D( target, internalformat, width, height, format, type, row, column );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glSetFenceNV
inline  void  GLSetFence( GLuint fence, GLenum condition )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glSetFence( fence, condition );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glShadeModel
/** This function caches the shade model setting and asserts on the result before returning.  */
inline  void  GLShadeModel( GLenum mode )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  ctx->m_previousAttributes.m_shadeModel = ctx->m_lightingAttributes.m_shadeModel;
# endif
  if ( ctx->m_lightingAttributes.m_shadeModel != mode )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glShadeModel( mode );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    ctx->m_lightingAttributes.m_shadeModel = mode;
#endif
  }
}

/*! \brief Helper function to reset the shade model.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the shade model
 *  to it's previous state. Otherwise, it is set to GL_SMOOTH.
 *  \sa GLShadeModel */
inline void GLResetShadeModel()
{
#if defined(GL_SUPPORT_RESET)
  GLShadeModel( glContext()->m_previousAttributes.m_shadeModel );
#else
  GLShadeModel( GL_SMOOTH ); 
#endif
}

//! Replacement of glShaderSource
inline  void  GLShaderSource( GLuint shader, GLsizei count, const GLcharARB* *string, const GLint *length )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glShaderSource( shader, count, string, length );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glStencilClearTag
inline  void  GLStencilClearTag( GLsizei tagBits, GLuint clearTag )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glStencilClearTag( tagBits, clearTag );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glStencilFunc
/** This function caches the stencil function setting and asserts on the result before returning.  */
inline  void  GLStencilFunc( GLenum func, GLint ref, GLuint mask )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  // store complete BFState<T>
  ctx->m_previousAttributes.m_stencilFunc      = ctx->m_stencilBufferAttributes.m_stencilFunc;
  ctx->m_previousAttributes.m_stencilRef       = ctx->m_stencilBufferAttributes.m_stencilRef;
  ctx->m_previousAttributes.m_stencilValueMask = ctx->m_stencilBufferAttributes.m_stencilValueMask;
# endif
  // both back and front state in BFState have to be correct for caching to work
  if (    ( ctx->m_stencilBufferAttributes.m_stencilFunc != func )
      ||  ( ctx->m_stencilBufferAttributes.m_stencilRef != ref )
      ||  ( ctx->m_stencilBufferAttributes.m_stencilValueMask != mask ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glStencilFunc( func, ref, mask );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    // update complete BFState<T>
    ctx->m_stencilBufferAttributes.m_stencilFunc = func;
    ctx->m_stencilBufferAttributes.m_stencilRef = ref;
    ctx->m_stencilBufferAttributes.m_stencilValueMask = mask;
#endif
  }
}

//! Replacement of glStencilFuncSeparate
inline  void  GLStencilFuncSeparate( GLenum face, GLenum func, GLint ref, GLuint mask )
{
  NVSG_GL_STATS_FCT_COUNT();
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
#if defined( GL_SUPPORT_RESET )
  // store complete BFState<T>
  ctx->m_previousAttributes.m_stencilFunc      = ctx->m_stencilBufferAttributes.m_stencilFunc;
  ctx->m_previousAttributes.m_stencilRef       = ctx->m_stencilBufferAttributes.m_stencilRef;
  ctx->m_previousAttributes.m_stencilValueMask = ctx->m_stencilBufferAttributes.m_stencilValueMask;
#endif // GL_SUPPORT_RESET
  // compare corresponding BFState
  if ( !( ctx->testBFState( face, ctx->m_stencilBufferAttributes.m_stencilFunc,      func )
       && ctx->testBFState( face, ctx->m_stencilBufferAttributes.m_stencilRef,       ref  ) 
       && ctx->testBFState( face, ctx->m_stencilBufferAttributes.m_stencilValueMask, mask ) ) )
#endif // GL_CACHING
  {
    NVSG_ASSERT( glContext()->m_glFunctions );
    glContext()->m_glFunctions->glStencilFuncSeparate( face, func, ref, mask );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING ) 
    // update corresponding BFState<T>
    ctx->assignBFState( face, ctx->m_stencilBufferAttributes.m_stencilFunc, func );
    ctx->assignBFState( face, ctx->m_stencilBufferAttributes.m_stencilRef, ref );
    ctx->assignBFState( face, ctx->m_stencilBufferAttributes.m_stencilValueMask, mask );
#endif // GL_CACHING
  }
}

//! Replacement of glStencilOpSeparate
inline  void  GLStencilOpSeparate( GLenum face, GLenum fail, GLenum zfail, GLenum zpass )
{
  NVSG_GL_STATS_FCT_COUNT();
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
#if defined( GL_SUPPORT_RESET )
  // store complete BFState<T>
  ctx->m_previousAttributes.m_stencilFail          = ctx->m_stencilBufferAttributes.m_stencilFail;
  ctx->m_previousAttributes.m_stencilPassDepthFail = ctx->m_stencilBufferAttributes.m_stencilPassDepthFail;
  ctx->m_previousAttributes.m_stencilPassDepthPass = ctx->m_stencilBufferAttributes.m_stencilPassDepthPass;
#endif // GL_SUPPORT_RESET
  // compare corresponding BFState
  if ( !( ctx->testBFState( face, ctx->m_stencilBufferAttributes.m_stencilFail, fail ) 
       && ctx->testBFState( face, ctx->m_stencilBufferAttributes.m_stencilPassDepthFail, zfail )
       && ctx->testBFState( face, ctx->m_stencilBufferAttributes.m_stencilPassDepthPass, zpass ) ) )
#endif // GL_CACHING
  {
    NVSG_ASSERT( glContext()->m_glFunctions );
    glContext()->m_glFunctions->glStencilOpSeparate( face, fail, zfail, zpass );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )    
    // update corresponding BFState<T>
    ctx->assignBFState( face, ctx->m_stencilBufferAttributes.m_stencilFail,          fail  );
    ctx->assignBFState( face, ctx->m_stencilBufferAttributes.m_stencilPassDepthFail, zfail );
    ctx->assignBFState( face, ctx->m_stencilBufferAttributes.m_stencilPassDepthPass, zpass );
#endif // GL_CACHING
  }
}

//! Replacement of glStencilMaskSeparate
inline  void  GLStencilMaskSeparate( GLenum face, GLuint mask )
{
  NVSG_GL_STATS_FCT_COUNT();
  
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  // store complete BFState<T>
  ctx->m_previousAttributes.m_stencilWriteMask = ctx->m_stencilBufferAttributes.m_stencilWriteMask;
# endif
  // compare corresponding BFState
  if ( !ctx->testBFState( face, ctx->m_stencilBufferAttributes.m_stencilWriteMask, mask ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glStencilMaskSeparate( face, mask );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING ) 
    // update corresponding BFState<T>
    ctx->assignBFState( face, ctx->m_stencilBufferAttributes.m_stencilWriteMask, mask );
#endif
  }
}

/*! \brief Helper function to reset the stencil function values.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the stencil
 *  function values to it's previous state. Otherwise, it sets the function, reference value,
 *  and mask to GL_ALWAYS, 0, and ~0, respectively.
 *  \sa GLStencilFunc */
inline void GLResetStencilFunc()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLStencilFuncSeparate( GL_BACK, ctx->m_previousAttributes.m_stencilFunc[GL_BACK]
                                , ctx->m_previousAttributes.m_stencilRef[GL_BACK]
                                , ctx->m_previousAttributes.m_stencilValueMask[GL_BACK] );
  GLStencilFuncSeparate( GL_FRONT, ctx->m_previousAttributes.m_stencilFunc[GL_FRONT]
                                , ctx->m_previousAttributes.m_stencilRef[GL_FRONT]
                                , ctx->m_previousAttributes.m_stencilValueMask[GL_FRONT] );
#else
  GLStencilFunc( GL_ALWAYS, 0, ~0 );
#endif
}

//! Replacement of glStencilMask
/** This function caches the stencil mask setting and asserts on the result before returning.  */
inline  void  GLStencilMask( GLuint mask )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  // store complete BFState<T>
  ctx->m_previousAttributes.m_stencilWriteMask = ctx->m_stencilBufferAttributes.m_stencilWriteMask;
# endif
  // both back and front state in BFState have to be correct for caching to work
  if ( ctx->m_stencilBufferAttributes.m_stencilWriteMask != mask )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glStencilMask( mask );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )
    // update complete BFState<T>
    ctx->m_stencilBufferAttributes.m_stencilWriteMask = mask;
#endif
  }
}

/*! \brief Helper function to reset the stencil mask.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the stencil
 *  mask to it's previous state. Otherwise, it is set to ~0.
 *  \sa GLStencilMask */
inline void GLResetStencilMask()
{
#if defined(GL_SUPPORT_RESET)
  GLStencilMaskSeparate( GL_BACK, glContext()->m_previousAttributes.m_stencilWriteMask[GL_BACK] );
  GLStencilMaskSeparate( GL_FRONT, glContext()->m_previousAttributes.m_stencilWriteMask[GL_FRONT] );
#else
  GLStencilMask( ~0 );
#endif
}

//! Replacement of glStencilOp
/** This function caches the stencil operation setting and asserts on the result before returning.  */
inline  void  GLStencilOp( GLenum fail, GLenum zfail, GLenum zpass )
{
#if defined( GL_CACHING )
  GLContext * ctx = glContext();
# if defined(GL_SUPPORT_RESET)
  // store complete BFState<T>
  ctx->m_previousAttributes.m_stencilFail          = ctx->m_stencilBufferAttributes.m_stencilFail;
  ctx->m_previousAttributes.m_stencilPassDepthFail = ctx->m_stencilBufferAttributes.m_stencilPassDepthFail;
  ctx->m_previousAttributes.m_stencilPassDepthPass = ctx->m_stencilBufferAttributes.m_stencilPassDepthPass;
# endif
  // both back and front state in BFState have to be correct for caching to work
  if (    ( ctx->m_stencilBufferAttributes.m_stencilFail != fail )
      ||  ( ctx->m_stencilBufferAttributes.m_stencilPassDepthFail != zfail )
      ||  ( ctx->m_stencilBufferAttributes.m_stencilPassDepthPass != zpass ) )
#endif
  {
    NVSG_GL_STATS_FCT_COUNT();
    glStencilOp( fail, zfail, zpass );
    ASSERT_GL_NO_ERROR;
#if defined( GL_CACHING )    
    // update complete BFState<T>
    ctx->m_stencilBufferAttributes.m_stencilFail = fail;
    ctx->m_stencilBufferAttributes.m_stencilPassDepthFail = zfail;
    ctx->m_stencilBufferAttributes.m_stencilPassDepthPass = zpass;
#endif
  }
}

/*! \brief Helper function to reset the stencil operation values.
 *  \remarks If GL_SUPPORT_RESET is defined (per default, it is), this function sets the stencil
 *  operation values to it's previous state. Otherwise, it sets each value (fail, zfail, zpass) to
 *  GL_KEEP.
 *  \sa GLStencilMask */
inline void GLResetStencilOp()
{
#if defined(GL_SUPPORT_RESET)
  GLContext * ctx = glContext();
  GLStencilOpSeparate( GL_BACK, ctx->m_previousAttributes.m_stencilFail[GL_BACK]
                              , ctx->m_previousAttributes.m_stencilPassDepthFail[GL_BACK]
                              , ctx->m_previousAttributes.m_stencilPassDepthPass[GL_BACK] );
  GLStencilOpSeparate( GL_FRONT, ctx->m_previousAttributes.m_stencilFail[GL_FRONT]
                              , ctx->m_previousAttributes.m_stencilPassDepthFail[GL_FRONT]
                              , ctx->m_previousAttributes.m_stencilPassDepthPass[GL_FRONT] );
#else
  GLStencilOp( GL_KEEP, GL_KEEP, GL_KEEP );
#endif
}

//! Replacement of glStringMarkerGREMEDY
/** This function simply does nothing if the extension is not available  */
inline void GLStringMarker( GLsizei len, const GLvoid * string )
{
  if( glContext()->m_glFunctions && glContext()->m_glFunctions->glStringMarker )
  {
    glContext()->m_glFunctions->glStringMarker( len, string );
    ASSERT_GL_NO_ERROR;
  }
}

//! Replacement of glFrameTerminatorGREMEDY
/** This function simply does nothing if the extension is not available  */
inline void GLFrameTerminator( void )
{
  if( glContext()->m_glFunctions && 
                            glContext()->m_glFunctions->glFrameTerminator )
  {
    glContext()->m_glFunctions->glFrameTerminator();
    ASSERT_GL_NO_ERROR;
  }
}

//! Replacement of glTestFenceNV
/** This function asserts on the result before returning.  */
inline  GLboolean GLTestFence( GLuint fence )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  GLboolean ok = glContext()->m_glFunctions->glTestFence( fence );
  ASSERT_GL_NO_ERROR;
  return( ok );
}

//! Replacement of glTexBufferEXT
/** This function asserts on the result before returning.  */
inline  void  GLTexBuffer( GLenum target, GLenum internalformat, GLuint buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexBuffer( target, internalformat, buffer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexCoord1d
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1d( GLdouble s )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1d( s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1dv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1f
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1f( GLfloat s )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1f( s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1fv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1hNV
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1h( GLhalf s )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord1h( s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1hv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord1hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1i
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1i( GLint s )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1i( s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1iv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1s
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1s( GLshort s )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1s( s );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord1sv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord1sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord1sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2d
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2d( GLdouble s, GLdouble t )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2d( s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2dv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2f
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2f( GLfloat s, GLfloat t )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2f( s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2fv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2hNV
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2h( GLhalf s, GLhalf t )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord2h( s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2hv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord2hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2i
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2i( GLint s, GLint t )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2i( s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2iv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2s
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2s( GLshort s, GLshort t )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2s( s, t );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord2sv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord2sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord2sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3d
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3d( GLdouble s, GLdouble t, GLdouble r )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3d( s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3dv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3f
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3f( GLfloat s, GLfloat t, GLfloat r )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3f( s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3fv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3hNV
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3h( GLhalf s, GLhalf t, GLhalf r )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord3h( s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3hv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord3hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3i
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3i( GLint s, GLint t, GLint r )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3i( s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3iv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3s
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3s( GLshort s, GLshort t, GLshort r )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3s( s, t, r );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord3sv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord3sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord3sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4d
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4d( GLdouble s, GLdouble t, GLdouble r, GLdouble q )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4d( s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4dv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4f
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4f( GLfloat s, GLfloat t, GLfloat r, GLfloat q )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4f( s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4fv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4hNV
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4h( GLhalf s, GLhalf t, GLhalf r, GLhalf q )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord4h( s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4hv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexCoord4hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4i
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4i( GLint s, GLint t, GLint r, GLint q )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4i( s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4iv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4s
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4s( GLshort s, GLshort t, GLshort r, GLshort q )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4s( s, t, r, q );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoord4sv
/** This function asserts on the result before returning.  */
inline  void  GLTexCoord4sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glTexCoord4sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glTexCoordPointer
/** This function caches the tex coord pointer setting and asserts on the result before returning.  */
inline  void  GLTexCoordPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
#if defined( GL_CACHING )
  _GLUpdateClientActiveTexture();
#endif
  NVSG_GL_STATS_TEX_COORD_POINTER( size, type, stride, pointer );
  glTexCoordPointer( size, type, stride, pointer );    
  ASSERT_GL_NO_ERROR;
}

//! Cached version of GLTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, params )
/** This function caches the GL_TEXTURE_ENV_COLOR value of the active texture unit.
  * It is faster to call this method instead of GLTexEnvfv(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, params) directly. */
inline  void  GLTextureEnvColor( const GLfloat * params )
{
#if defined( GL_CACHING )
  if ( !equal4fv( &glContext()->m_textureAttributes.m_unit[glContext()->m_textureAttributes.m_toActivate].m_envColor[0], params ) )
  {
    _GLUpdateActiveTexture();
# if !defined(NDEBUG)
    GLfloat tmp[4];
    glGetTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, tmp );
    NVSG_ASSERT( equal4fv(&glContext()->m_textureAttributes.m_unit[glContext()->m_textureAttributes.m_toActivate].m_envColor[0], tmp ) );
# endif
#endif
    _GLTexEnvfv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, params );
#if defined( GL_CACHING )
    memcpy( &glContext()->m_textureAttributes.m_unit[glContext()->m_textureAttributes.m_toActivate].m_envColor[0], params, 4*sizeof(float) );
  }
#endif
}

//! Cached version of GLTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, param )
/** This function caches the GL_TEXTURE_ENV_MODE value of the active texture unit.
* It is faster to call this method instead of GLTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, param) directly. */
inline  void  GLTextureEnvMode( GLint param )
{
#if defined( GL_CACHING )
  if ( glContext()->m_textureAttributes.m_unit[glContext()->m_textureAttributes.m_toActivate].m_envMode != param )
  {
    _GLUpdateActiveTexture();
# if !defined(NDEBUG)
    GLint tem;
    glGetTexEnviv( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, &tem );
    NVSG_ASSERT( glContext()->m_textureAttributes.m_unit[glContext()->m_textureAttributes.m_toActivate].m_envMode == tem );
# endif
#endif
    _GLTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, param );
#if defined( GL_CACHING )
    glContext()->m_textureAttributes.m_unit[glContext()->m_textureAttributes.m_toActivate].m_envMode = param;
  }
#endif
}

//! Cached version of glTexEnvf( target, pname, param )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_ENV_MODE :
      NVSG_ASSERT ( target == GL_TEXTURE_ENV );
      GLTextureEnvMode( (GLint)param );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexEnvf( target, pname, param );
      break;
  }
#else
  _GLTexEnvf( target, pname, param );
#endif
}

//! Cached version of glTexEnvf( target, pname, param )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexEnvfv( GLenum target, GLenum pname, const GLfloat *params )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_ENV_COLOR :
      NVSG_ASSERT ( target == GL_TEXTURE_ENV );
      GLTextureEnvColor( params );
      break;
    case GL_TEXTURE_ENV_MODE :
      NVSG_ASSERT ( target == GL_TEXTURE_ENV );
      GLTextureEnvMode( (GLint)params[0] );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexEnvfv( target, pname, params );
      break;
  }
#else
  _GLTexEnvfv( target, pname, params );
#endif
}

//! Cached version of glTexEnvi( target, pname, param )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexEnvi( GLenum target, GLenum pname, GLint param )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_ENV_MODE :
      NVSG_ASSERT ( target == GL_TEXTURE_ENV );
      GLTextureEnvMode( param );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexEnvi( target, pname, param );
      break;
  }
#else
  _GLTexEnvi( target, pname, param );
#endif
}

//! Cached version of glTexEnviv( target, pname, param )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexEnviv( GLenum target, GLenum pname, const GLint *params )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_ENV_MODE :
      NVSG_ASSERT ( target == GL_TEXTURE_ENV );
      GLTextureEnvMode( params[0] );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexEnviv( target, pname, params );
      break;
  }
#else
  _GLTexEnviv( target, pname, params );
#endif
}

//! Replacement of glTexGend
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexGend( GLenum coord, GLenum pname, GLdouble param )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glTexGend( coord, pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexGenf
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexGenf( GLenum coord, GLenum pname, GLfloat param )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glTexGenf( coord, pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexGenfv
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexGenfv( GLenum coord, GLenum pname, const GLfloat *param )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glTexGenfv( coord, pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexGeni
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexGeni( GLenum coord, GLenum pname, GLint param )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glTexGeni( coord, pname, param );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexImage1D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexImage1D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glTexImage1D( target, level, internalformat, width, border, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexImage2D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexImage2D( GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border
                          , GLenum format, GLenum type, const GLvoid *pixels )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_TEX_IMAGE2D( target, level, internalformat, width, height, border, format, type, pixels );
  glTexImage2D( target, level, internalformat, width, height, border, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexImage3D
/** This function hides the extension character of glTexImage3D, updates the active texture,
  * and asserts on the result before returning.  */
inline  void  GLTexImage3D( GLenum target, int level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, int border, GLenum format, GLenum type, const void* pixels)
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT(GLGetVersion()>=12 || GLIsExtensionAvailable("GL_EXT_texture3D"));
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexImage3D( target, level, internalformat, width, height, depth, border, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of GLTexParameterfv( target, GL_TEXTURE_BORDER_COLOR, params )
/** This function caches the GL_TEXTURE_BORDER_COLOR values of the texture currently bound to target.
  * It is faster to call this method instead of GLTexParameterfv(target, GL_TEXTURE_BORDER_COLOR, params) directly. */
inline  void  GLTextureBorderColor( GLenum target, const GLfloat *params )
{
#if defined( GL_CACHING )
  _GLUpdateActiveTexture();
#endif
  _GLTexParameterfv( target, GL_TEXTURE_BORDER_COLOR, params );
}

//! Cached version of GLTexParameteri( target, GL_TEXTURE_MAG_FILTER, param )
/** This function caches the GL_TEXTURE_MAG_FILTER values of the texture currently bound to target.
  * It is faster to call this method instead of GLTexParameteri(target, GL_TEXTURE_MAG_FILTER, param) directly. */
inline  void  GLTextureMagFilter( GLenum target, GLint param )
{
#if defined( GL_CACHING )
  _GLUpdateActiveTexture();
#endif
  _GLTexParameteri( target, GL_TEXTURE_MAG_FILTER, param );
}

//! Cached version of GLTexParameteri( target, GL_TEXTURE_MIN_FILTER, param )
/** This function caches the GL_TEXTURE_MIN_FILTER values of the texture currently bound to target.
  * It is faster to call this method instead of GLTexParameteri(target, GL_TEXTURE_MIN_FILTER, param) directly. */
inline  void  GLTextureMinFilter( GLenum target, GLint param )
{
#if defined( GL_CACHING )
  _GLUpdateActiveTexture();
#endif
  _GLTexParameteri( target, GL_TEXTURE_MIN_FILTER, param );
}

//! Cached version of GLTexParameteri( target, GL_TEXTURE_WRAP_R, param )
/** This function caches the GL_TEXTURE_WRAP_R values of the texture currently bound to target.
  * It is faster to call this method instead of GLTexParameteri(target, GL_TEXTURE_WRAP_R, param) directly. */
inline  void  GLTextureWrapR( GLenum target, GLint param )
{
#if defined( GL_CACHING )
  _GLUpdateActiveTexture();
#endif
  _GLTexParameteri( target, GL_TEXTURE_WRAP_R, param );
}

//! Cached version of GLTexParameteri( target, GL_TEXTURE_WRAP_S, param )
/** This function caches the GL_TEXTURE_WRAP_S values of the texture currently bound to target.
  * It is faster to call this method instead of GLTexParameteri(target, GL_TEXTURE_WRAP_S, param) directly. */
inline  void  GLTextureWrapS( GLenum target, GLint param )
{
#if defined( GL_CACHING )
  _GLUpdateActiveTexture();
#endif
  _GLTexParameteri( target, GL_TEXTURE_WRAP_S, param );
}

//! Cached version of GLTexParameteri( target, GL_TEXTURE_WRAP_T, param )
/** This function caches the GL_TEXTURE_WRAP_T values of the texture currently bound to target.
  * It is faster to call this method instead of GLTexParameteri(target, GL_TEXTURE_WRAP_T, param) directly. */
inline  void  GLTextureWrapT( GLenum target, GLint param )
{
#if defined( GL_CACHING )
  _GLUpdateActiveTexture();
#endif
  _GLTexParameteri( target, GL_TEXTURE_WRAP_T, param );
}

//! Cached version of glTrackMatrixNV
/** This function asserts on the result before returning. */
inline  void  GLTrackMatrix( GLenum target, GLuint address, GLenum matrix, GLenum transform )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTrackMatrixNV( target, address, matrix, transform );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of glTransformFeedbackAttribsNV
/** This function asserts on the result before returning. */
inline  void  GLTransformFeedbackAttribsNV( GLuint count, const GLint *attribs, GLenum bufferMode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTransformFeedbackAttribsNV( count, attribs, bufferMode );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of glTransformFeedbackVaryingsNV
/** This function asserts on the result before returning. */
inline  void  GLTransformFeedbackVaryingsNV( GLuint program, GLsizei count, const GLint* varyings, GLenum bufferMode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTransformFeedbackVaryingsNV( program, count, varyings, bufferMode );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of glTransformFeedbackVaryings
/** This function asserts on the result before returning. */
inline  void  GLTransformFeedbackVaryings( GLuint program, GLsizei count, const GLchar* *varyings, GLenum bufferMode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTransformFeedbackVaryings( program, count, varyings, bufferMode );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of glTexParameterf( target, pname, param )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_BORDER_COLOR :
      NVSG_ASSERT( false );
      break;
    case GL_TEXTURE_MAG_FILTER :
      GLTextureMagFilter( target, (GLint)param );
      break;
    case GL_TEXTURE_MIN_FILTER :
      GLTextureMinFilter( target, (GLint)param );
      break;
    case GL_TEXTURE_WRAP_R :
      GLTextureWrapR( target, (GLint)param );
      break;
    case GL_TEXTURE_WRAP_S :
      GLTextureWrapS( target, (GLint)param );
      break;
    case GL_TEXTURE_WRAP_T :
      GLTextureWrapT( target, (GLint)param );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexParameterf( target, pname, param );
      break;
  }
#else
  _GLTexParameterf( target, pname, param );
#endif
}

//! Cached version of glTexParameterfv( target, pname, params )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_BORDER_COLOR :
      GLTextureBorderColor( target, params );
      break;
    case GL_TEXTURE_MAG_FILTER :
      GLTextureMagFilter( target, (GLint)params[0] );
      break;
    case GL_TEXTURE_MIN_FILTER :
      GLTextureMinFilter( target, (GLint)params[0] );
      break;
    case GL_TEXTURE_WRAP_R :
      GLTextureWrapR( target, (GLint)params[0] );
      break;
    case GL_TEXTURE_WRAP_S :
      GLTextureWrapS( target, (GLint)params[0] );
      break;
    case GL_TEXTURE_WRAP_T :
      GLTextureWrapT( target, (GLint)params[0] );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexParameterfv( target, pname, params );
      break;
  }
#else
  _GLTexParameterfv( target, pname, params );
#endif
}

//! Cached version of glTexParameteri( target, pname, param )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexParameteri( GLenum target, GLenum pname, GLint param )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_BORDER_COLOR :
      NVSG_ASSERT( false );
      break;
    case GL_TEXTURE_MAG_FILTER :
      GLTextureMagFilter( target, param );
      break;
    case GL_TEXTURE_MIN_FILTER :
      GLTextureMinFilter( target, param );
      break;
    case GL_TEXTURE_WRAP_R :
      GLTextureWrapR( target, param );
      break;
    case GL_TEXTURE_WRAP_S :
      GLTextureWrapS( target, param );
      break;
    case GL_TEXTURE_WRAP_T :
      GLTextureWrapT( target, param );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexParameteri( target, pname, param );
      break;
  }
#else
  _GLTexParameteri( target, pname, param );
#endif
}

//! Replacement of glTexParameterIivEXT
/** This function asserts on the result before returning. */
inline  void  GLTexParameterIiv( GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexParameterIiv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexParameterIuivEXT
/** This function asserts on the result before returning. */
inline  void  GLTexParameterIuiv( GLenum target, GLenum pname, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glTexParameterIuiv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

//! Cached version of glTexParameteriv( target, pname, params )
/** This function caches some of the texture environment parameters and asserts on the result before returning. */
inline  void  GLTexParameteriv( GLenum target, GLenum pname, const GLint *params )
{
#if defined( GL_CACHING )
  switch( pname )
  {
    case GL_TEXTURE_BORDER_COLOR :
      {
        GLfloat fParams[4];
        GLintToGLfloat( params, fParams );
        GLTextureBorderColor( target, fParams );
      }
      break;
    case GL_TEXTURE_MAG_FILTER :
      GLTextureMagFilter( target, params[0] );
      break;
    case GL_TEXTURE_MIN_FILTER :
      GLTextureMinFilter( target, params[0] );
      break;
    case GL_TEXTURE_WRAP_R :
      GLTextureWrapR( target, params[0] );
      break;
    case GL_TEXTURE_WRAP_S :
      GLTextureWrapS( target, params[0] );
      break;
    case GL_TEXTURE_WRAP_T :
      GLTextureWrapT( target, params[0] );
      break;
    default :
      _GLUpdateActiveTexture();
      _GLTexParameteriv( target, pname, params );
      break;
  }
#else
  _GLTexParameteriv( target, pname, params );
#endif
}

//! Replacement of glTexSubImage1D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexSubImage1D( GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glTexSubImage1D( target, level, xoffset, width, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexSubImage2D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexSubImage2D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  glTexSubImage2D( target, level, xoffset, yoffset, width, height, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTexSubImage3D
/** This function updates the active texture and asserts on the result before returning.  */
inline  void  GLTexSubImage3D( GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels )
{
#if defined(GL_CACHING)
  _GLUpdateActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT(GLGetVersion()>=12 || GLIsExtensionAvailable("GL_EXT_texture3D"));
  NVSG_ASSERT(glContext()->m_glFunctions && glContext()->m_glFunctions->glTexSubImage3D);
  glContext()->m_glFunctions->glTexSubImage3D( target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTranslated
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLTranslated( GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_TRANSLATED( x, y, z );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glTranslated( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glTranslatef
/** This function updates the active texture if the matrix mode is GL_TEXTURE and asserts on the result before returning.  */
inline  void  GLTranslatef( GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_TRANSLATEF( x, y, z );
#if defined( GL_CACHING )
  if ( GL_TEXTURE == glContext()->m_transformAttributes.m_matrixMode )
  { // note: we have a texture matrix stack per unit
    _GLUpdateActiveTexture();
  }
#endif
  glTranslatef( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertex2d
/** This function asserts on the result before returning.  */
inline  void  GLVertex2d( GLdouble x, GLdouble y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2d( x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2dv
/** This function asserts on the result before returning.  */
inline  void  GLVertex2dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2f
/** This function asserts on the result before returning.  */
inline  void  GLVertex2f( GLfloat x, GLfloat y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2f( x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2fv
/** This function asserts on the result before returning.  */
inline  void  GLVertex2fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2hNV
/** This function asserts on the result before returning.  */
inline  void  GLVertex2h( GLhalf x, GLhalf y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertex2h( x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2hv
/** This function asserts on the result before returning.  */
inline  void  GLVertex2hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertex2hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2i
/** This function asserts on the result before returning.  */
inline  void  GLVertex2i( GLint x, GLint y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2i( x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2iv
/** This function asserts on the result before returning.  */
inline  void  GLVertex2iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2s
/** This function asserts on the result before returning.  */
inline  void  GLVertex2s( GLshort x, GLshort y )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2s( x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex2sv
/** This function asserts on the result before returning.  */
inline  void  GLVertex2sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex2sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3d
/** This function asserts on the result before returning.  */
inline  void  GLVertex3d( GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3d( x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3dv
/** This function asserts on the result before returning.  */
inline  void  GLVertex3dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3f
/** This function asserts on the result before returning.  */
inline  void  GLVertex3f( GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3f( x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3fv
/** This function asserts on the result before returning.  */
inline  void  GLVertex3fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3hNV
/** This function asserts on the result before returning.  */
inline  void  GLVertex3h( GLhalf x, GLhalf y, GLhalf z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertex3h( x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3hv
/** This function asserts on the result before returning.  */
inline  void  GLVertex3hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertex3hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3i
/** This function asserts on the result before returning.  */
inline  void  GLVertex3i( GLint x, GLint y, GLint z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3i( x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3iv
/** This function asserts on the result before returning.  */
inline  void  GLVertex3iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3s
/** This function asserts on the result before returning.  */
inline  void  GLVertex3s( GLshort x, GLshort y, GLshort z )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3s( x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex3sv
/** This function asserts on the result before returning.  */
inline  void  GLVertex3sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex3sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4d
/** This function asserts on the result before returning.  */
inline  void  GLVertex4d( GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4d( x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4dv
/** This function asserts on the result before returning.  */
inline  void  GLVertex4dv( const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4dv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4f
/** This function asserts on the result before returning.  */
inline  void  GLVertex4f( GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4f( x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4fv
/** This function asserts on the result before returning.  */
inline  void  GLVertex4fv( const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4fv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4hNV
/** This function asserts on the result before returning.  */
inline  void  GLVertex4h( GLhalf x, GLhalf y, GLhalf z, GLhalf w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertex4h( x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4hv
/** This function asserts on the result before returning.  */
inline  void  GLVertex4hv( const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertex4hv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4i
/** This function asserts on the result before returning.  */
inline  void  GLVertex4i( GLint x, GLint y, GLint z, GLint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4i( x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4iv
/** This function asserts on the result before returning.  */
inline  void  GLVertex4iv( const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4iv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4s
/** This function asserts on the result before returning.  */
inline  void  GLVertex4s( GLshort x, GLshort y, GLshort z, GLshort w )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4s( x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertex4sv
/** This function asserts on the result before returning.  */
inline  void  GLVertex4sv( const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  glVertex4sv( v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexArrayRange
/** This function asserts on the result before returning.  */
inline  void  GLVertexArrayRange( GLsizei size, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexArrayRange( size, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttrib1d
inline  void  GLVertexAttrib1d( GLuint index, GLdouble x )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1d( index, x );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib1dv
inline  void  GLVertexAttrib1dv( GLuint index, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1dv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib1f
inline  void  GLVertexAttrib1f( GLuint index, GLfloat x )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1f( index, x );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib1fv
inline  void  GLVertexAttrib1fv( GLuint index, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1fv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib1h
inline  void  GLVertexAttrib1h( GLuint index, GLhalf x )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1h( index, x );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib1nv
inline  void  GLVertexAttrib1hv( GLuint index, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1hv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib1s
inline  void  GLVertexAttrib1s( GLuint index, GLshort x )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1s( index, x );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib1sv
inline  void  GLVertexAttrib1sv( GLuint index, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib1sv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2d
inline  void  GLVertexAttrib2d( GLuint index, GLdouble x, GLdouble y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2d( index, x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2dv
inline  void  GLVertexAttrib2dv( GLuint index, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2dv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2f
inline  void  GLVertexAttrib2f( GLuint index, GLfloat x, GLfloat y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2f( index, x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2fv
inline  void  GLVertexAttrib2fv( GLuint index, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2fv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2h
inline  void  GLVertexAttrib2h( GLuint index, GLhalf x, GLhalf y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2h( index, x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2hv
inline  void  GLVertexAttrib2hv( GLuint index, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2hv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2s
inline  void  GLVertexAttrib2s( GLuint index, GLshort x, GLshort y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2s( index, x, y );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib2sv
inline  void  GLVertexAttrib2sv( GLuint index, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib2sv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3d
inline  void  GLVertexAttrib3d( GLuint index, GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3d( index, x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3dv
inline  void  GLVertexAttrib3dv( GLuint index, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3dv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3f
inline  void  GLVertexAttrib3f( GLuint index, GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3f( index, x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3fv
inline  void  GLVertexAttrib3fv( GLuint index, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3fv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3h
inline  void  GLVertexAttrib3h( GLuint index, GLhalf x, GLhalf y, GLhalf z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3h( index, x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3hv
inline  void  GLVertexAttrib3hv( GLuint index, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3hv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3s
inline  void  GLVertexAttrib3s( GLuint index, GLshort x, GLshort y, GLshort z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3s( index, x, y, z );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3sv
inline  void  GLVertexAttrib3sv( GLuint index, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib3sv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib3bv
inline  void  GLVertexAttrib4bv( GLuint index, const GLbyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4bv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4d
inline  void  GLVertexAttrib4d( GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4d( index, x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4dv
inline  void  GLVertexAttrib4dv( GLuint index, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4dv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4f
inline  void  GLVertexAttrib4f( GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4f( index, x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4fv
inline  void  GLVertexAttrib4fv( GLuint index, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4fv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4h
inline  void  GLVertexAttrib4h( GLuint index, GLhalf x, GLhalf y, GLhalf z, GLhalf w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4h( index, x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4hv
inline  void  GLVertexAttrib4hv( GLuint index, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4hv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4iv
inline  void  GLVertexAttrib4iv( GLuint index, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4iv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4s
inline  void  GLVertexAttrib4s( GLuint index, GLshort x, GLshort y, GLshort z, GLshort w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4s( index, x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4sv
inline  void  GLVertexAttrib4sv( GLuint index, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4sv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4ub
inline  void  GLVertexAttrib4ub( GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4ubNV( index, x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4ubv
inline  void  GLVertexAttrib4ubv( GLuint index, const GLubyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4ubv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4uiv
inline  void  GLVertexAttrib4uiv( GLuint index, const GLuint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4uiv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4usv
inline  void  GLVertexAttrib4usv( GLuint index, const GLushort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4usv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4Nbv
inline  void  GLVertexAttrib4Nbv( GLuint index, const GLbyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4Nbv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4Niv
inline  void  GLVertexAttrib4Niv( GLuint index, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4Niv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4Nsv
inline  void  GLVertexAttrib4Nsv( GLuint index, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4Nsv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4Nub
inline  void  GLVertexAttrib4Nub( GLuint index, GLubyte x, GLubyte y, GLubyte z, GLubyte w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4Nub( index, x, y, z, w );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4Nubv
inline  void  GLVertexAttrib4Nubv( GLuint index, const GLubyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4Nubv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4Nuiv
inline  void  GLVertexAttrib4Nuiv( GLuint index, const GLuint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4Nuiv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttrib4Nusv
inline  void  GLVertexAttrib4Nusv( GLuint index, const GLushort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttrib4Nusv( index, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribI1iEXT
inline void GLVertexAttribI1i( GLuint index, GLint x )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI1i( index, x );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI2iEXT
inline void GLVertexAttribI2i( GLuint index, GLint x, GLint y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI2i( index, x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI3iEXT
inline void GLVertexAttribI3i( GLuint index, GLint x, GLint y, GLint z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI3i( index, x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4iEXT
inline void GLVertexAttribI4i( GLuint index, GLint x, GLint y, GLint z, GLint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4i( index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI1uiExt
inline void GLVertexAttribI1ui( GLuint index, GLuint x )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI1ui( index, x );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI2uiExt
inline void GLVertexAttribI2ui( GLuint index, GLuint x, GLuint y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI2ui( index, x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI3uiExt
inline void GLVertexAttribI3ui( GLuint index, GLuint x, GLuint y, GLuint z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI3ui( index, x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4uiExt
inline void GLVertexAttribI4ui( GLuint index, GLuint x, GLuint y, GLuint z, GLuint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4ui( index, x, y, z, w );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI1ivEXT
inline void GLVertexAttribI1iv( GLuint index, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI1iv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI2ivEXT
inline void GLVertexAttribI2iv( GLuint index, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI2iv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI3ivEXT
inline void GLVertexAttribI3iv( GLuint index, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI3iv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4ivEXT
inline void GLVertexAttribI4iv( GLuint index, const GLint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4iv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI1iuvEXT
inline void GLVertexAttribI1uiv( GLuint index, const GLuint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI1uiv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI2uivEXT
inline void GLVertexAttribI2uiv( GLuint index, const GLuint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI2uiv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI3uivEXT
inline void GLVertexAttribI3uiv( GLuint index, const GLuint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI3uiv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4uivEXT
inline void GLVertexAttribI4uiv( GLuint index, const GLuint *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4uiv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4bvEXT
inline void GLVertexAttribI4bv( GLuint index, const GLbyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4bv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4ubvEXT
inline void GLVertexAttribI4ubv( GLuint index, const GLubyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4ubv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4svEXT
inline void GLVertexAttribI4sv( GLuint index, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4sv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribI4usvEXT
inline void GLVertexAttribI4usv( GLuint index, const GLushort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribI4usv( index, v );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribIPointerEXT
inline void GLVertexAttribIPointer( GLuint index, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );  
  glContext()->m_glFunctions->glVertexAttribIPointer( index, size, type, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribPointer
inline void GLVertexAttribPointer( GLuint index, GLint size, GLenum type, GLboolean normalize, GLsizei stride, const GLvoid * pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribPointer(index, size, type, normalize, stride, pointer);
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribPointerNV
inline  void  GLVertexAttribPointerNV( GLuint index, GLint fsize, GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribPointerNV( index, fsize, type, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glVertexAttribs1dvNV
inline  void  GLVertexAttribs1dvNV( GLuint index, GLsizei count, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs1dvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs1fvNV
inline  void  GLVertexAttribs1fvNV( GLuint index, GLsizei count, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs1fvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs1hv
inline  void  GLVertexAttribs1hv( GLuint index, GLsizei count, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs1hv( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs1svNV
inline  void  GLVertexAttribs1svNV( GLuint index, GLsizei count, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs1svNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs2dvNV
inline  void  GLVertexAttribs2dvNV( GLuint index, GLsizei count, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs2dvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs2fvNV
inline  void  GLVertexAttribs2fvNV( GLuint index, GLsizei count, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs2fvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs2hv
inline  void  GLVertexAttribs2hv( GLuint index, GLsizei count, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs2hv( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs2svNV
inline  void  GLVertexAttribs2svNV( GLuint index, GLsizei count, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs2svNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs3dvNV
inline  void  GLVertexAttribs3dvNV( GLuint index, GLsizei count, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs3dvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs3fvNV
inline  void  GLVertexAttribs3fvNV( GLuint index, GLsizei count, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs3fvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs3hv
inline  void  GLVertexAttribs3hv( GLuint index, GLsizei count, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs3hv( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs3svNV
inline  void  GLVertexAttribs3svNV( GLuint index, GLsizei count, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs3svNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs4dvNV
inline  void  GLVertexAttribs4dvNV( GLuint index, GLsizei count, const GLdouble *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs4dvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs4fvNV
inline  void  GLVertexAttribs4fvNV( GLuint index, GLsizei count, const GLfloat *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs4fvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs4hv
inline  void  GLVertexAttribs4hv( GLuint index, GLsizei count, const GLhalf *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs4hv( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs4svNV
inline  void  GLVertexAttribs4svNV( GLuint index, GLsizei count, const GLshort *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs4svNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexAttribs4svNV
inline  void  GLVertexAttribs4ubvNV( GLuint index, GLsizei count, const GLubyte *v )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glVertexAttribs4ubvNV( index, count, v );
  ASSERT_GL_NO_ERROR_IN_BEGIN_END;
}

//! Replacement of glVertexPointer
/** This function caches the vertex pointer setting and asserts on the result before returning.  */
inline  void  GLVertexPointer( GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_VERTEX_POINTER( size, type, stride, pointer );
  glVertexPointer( size, type, stride, pointer );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glViewport
/** This function asserts on the result before returning.  */
inline  void  GLViewport( GLint x, GLint y, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_VIEWPORT( x, y, width, height );
  glViewport( x, y, width, height );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2d
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2d( GLdouble x, GLdouble y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2d( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2dv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2dv( const GLdouble *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2dv( p );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2f
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2f( GLfloat x, GLfloat y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2f( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2fv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2fv( const GLfloat *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2fv( p );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2i
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2i( GLint x, GLint y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2i( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2iv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2iv( const GLint *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2iv( p );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2s
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2s( GLshort x, GLshort y )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2s( x, y );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos2sv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos2sv( const GLshort *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos2sv( p );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3d
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3d( GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3d( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3dv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3dv( const GLdouble *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3dv( p );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3f
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3f( GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3f( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3fv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3fv( const GLfloat *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3fv( p );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3i
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3i( GLint x, GLint y, GLint z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3i( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3iv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3iv( const GLint *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3iv( p );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3s
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3s( GLshort x, GLshort y, GLshort z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3s( x, y, z );
  ASSERT_GL_NO_ERROR;
}

//! Replacement of glWindowPos3sv
/** This function asserts on the result before returning.  */
inline  void  GLWindowPos3sv( const GLshort *p )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glWindowPos3sv( p );
  ASSERT_GL_NO_ERROR;
}

// GL_ARB_map_buffer_range

/*! \brief Maps a range within the buffer to client space.
 *  \param target The target buffer to map.
 *  \param offset Memory offset into the buffer in bytes.
 *  \param length Mapped memory range in bytes.
 *  \param access Bitfield that encodes the type of map (read/write).
 *  \return pointer to mapped memory range.
 *  \note It is assumed that the extension GL_ARB_map_buffer_range is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLBindBuffer, GLUnmapBuffer, GLFlushMappedBufferRange */
inline GLvoid* GLMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_map_buffer_range") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  GLvoid* ptr = glContext()->m_glFunctions->glMapBufferRange( target, offset, length, access );
  ASSERT_GL_NO_ERROR;
  return ptr;
}

/*! \brief Marks the range of the mapped buffer as ready to use by GL.
 *  When a buffer was mapped with GLMapBufferRange and the manual flush bit was set, this function
 *  allows to mark the range valid. Once valid, other GL operations depending on the specified
 *  memory range can proceed.
 *  \param target The target buffer to map.
 *  \param offset Memory offset into the buffer in bytes.
 *  \param length Memory size in bytes.
 *  \note It is assumed that the extension GL_ARB_map_buffer_range is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLBindBuffer, GLMapBufferRange */
inline void GLFlushMappedBufferRange(GLenum target, GLintptr offset, GLsizeiptr length)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_map_buffer_range") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glFlushMappedBufferRange( target, offset, length );
  ASSERT_GL_NO_ERROR;
}

// GL_ARB_copy_buffer

/*! \brief Allows direct memory copy between buffer objects.
 *  \param readTarget The target buffer to read from.
 *  \param writeTarget The target buffer to write to.
 *  \param readOffset Memory offset into the read buffer in bytes.
 *  \param writeOffset Memory offset into the write buffer in bytes.
 *  \param size The memory size in bytes.
 *  \note It is assumed that the extension GL_ARB_copy_buffer is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLBindBuffer */
inline void GLCopyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_map_buffer_range") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glCopyBufferSubData( readTarget, writeTarget, readOffset, writeOffset, size );
  ASSERT_GL_NO_ERROR;
}

// GL_ARB_texture_multisample

/*! \brief Establishes data storage for multi-sampled textures.
 *  \param target The texture target
 *  \param samples The number of samples per pixel.
 *  \param internalformat The internal format for the texture must be color-renderable, depth-renderable, or stencil-renderable.
 *  \param width.
 *  \param height.
 *  \param fixedsamplelocations If true the sample locations will be fixed and independent of the internal format.
 *  \note It is assumed that the extension GL_ARB_texture_multisample is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLTexImage3DMultisample, GLGetMultisamplefv, GLSampleMaski */
inline void GLTexImage2DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLboolean fixedsamplelocations)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_texture_multisample") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glTexImage2DMultisample( target, samples, internalformat, width, height, fixedsamplelocations );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Establishes data storage for multi-sampled textures.
 *  \param target The texture target
 *  \param samples The number of samples per pixel.
 *  \param internalformat The internal format for the texture must be color-renderable, depth-renderable, or stencil-renderable.
 *  \param width.
 *  \param height.
 *  \param depth.
 *  \param fixedsamplelocations If true the sample locations will be fixed and independent of the internal format.
 *  \note It is assumed that the extension GL_ARB_texture_multisample is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLTexImage2DMultisample, GLGetMultisamplefv, GLSampleMaski */
inline void GLTexImage3DMultisample(GLenum target, GLsizei samples, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLboolean fixedsamplelocations)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_texture_multisample") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glTexImage3DMultisample( target, samples, internalformat, width, height, depth, fixedsamplelocations );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Allows to retrieve the sample location.
 *  \param pname must be GL_SAMPLE_POSITION
 *  \param index The sample for which the location should be retrieved.
 *  \param val must point to a float[2] which will hold the x and y location of the sample, (0.5,0.5) being a pixel center.
 *  \note It is assumed that the extension GL_ARB_texture_multisample is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLTexImage2DMultisample, GLTexImage3DMultisample, GLSampleMaski */
inline void GLGetMultisamplefv(GLenum pname, GLuint index, GLfloat *val)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_texture_multisample") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glGetMultisamplefv( pname, index, val );
  ASSERT_GL_NO_ERROR;
}

// GL_ARB_vertex_array_object
inline void GLBindVertexArray( GLuint array )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_vertex_array_object") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glBindVertexArray( array );
  ASSERT_GL_NO_ERROR;
}

inline void GLDeleteVertexArrays( GLsizei n, const GLuint* arrays )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_vertex_array_object") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glDeleteVertexArrays( n, arrays );
  ASSERT_GL_NO_ERROR;
}

inline void GLGenVertexArrays( GLsizei n, GLuint *arrays )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_vertex_array_object") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glGenVertexArrays( n, arrays );
  ASSERT_GL_NO_ERROR;
}

inline void GLIsVertexArray( GLuint array )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_vertex_array_object") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glIsVertexArray( array );
  ASSERT_GL_NO_ERROR;
}

// GL_ARB_debug_output
inline void GLDebugMessageControlARB( GLenum source, GLenum type, GLenum severity, GLsizei count, const GLuint *ids, GLboolean enabled )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_debug_output") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glDebugMessageControl( source, type, severity, count, ids, enabled );

  ASSERT_GL_NO_ERROR;
}

inline void GLDebugMessageInsertARB( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *buf )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_debug_output") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glDebugMessageInsert( source, type, id, severity, length, buf );

  ASSERT_GL_NO_ERROR;
}

inline void GLDebugMessageCallbackARB( GLDEBUGPROCARB callback, const GLvoid *userParam )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_debug_output") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glDebugMessageCallback( callback, userParam );

  ASSERT_GL_NO_ERROR;
}

inline GLuint GLGetDebugMessageLogARB( GLuint count, GLsizei bufsize, GLenum *sources, GLenum *types, GLuint *ids, GLenum *severities, GLsizei *lengths, GLchar *messageLog )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_debug_output") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  GLuint result = glContext()->m_glFunctions->glGetDebugMessageLog( count, bufsize, sources, types, ids, severities, lengths, messageLog );

  ASSERT_GL_NO_ERROR;
  return result;
}

/*! \brief When GL_SAMPLE_MASK is enabled this function allows to specify the mask the fragment coverage is ANDed with.
 *  \param maskNumber The mask number M.
 *  \param maskBits Bit B of mask M corresponds to sample 32*M+B.
 *  \note It is assumed that the extension GL_ARB_texture_multisample is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLTexImage2DMultisample, GLTexImage3DMultisample, GLGetMultisamplefv */
inline void GLSampleMaski(GLuint maskNumber, GLbitfield maskBits)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( GLIsExtensionAvailable("GL_ARB_texture_multisample") );
  NVSG_ASSERT( glContext()->m_glFunctions );

  glContext()->m_glFunctions->glSampleMaski( maskNumber, maskBits );
  ASSERT_GL_NO_ERROR;
}

// GL_EXT_framebuffer_object

/*! \brief Replacement of glIsRenderbufferEXT.
 *  \param renderbuffer The id to test.
 *  \return \c TRUE, if \a renderbuffer is the id of a render buffer, otherwise \c FALSE.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLBindRenderbuffer, GLDeleteRenderbuffers, GLGenRenderbuffers, GLRenderbufferStorage,
 *  GLGetRenderbufferParameteriv */
inline GLboolean GLIsRenderbuffer(GLuint renderbuffer)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  GLboolean isRenderBuffer = glContext()->m_glFunctions->glIsRenderbuffer(renderbuffer);
  ASSERT_GL_NO_ERROR;
  return isRenderBuffer;
}

/*! \brief Replacement of glBindRenderbufferEXT.
 *  \param target The id of the target to bind the render buffer to. Must be GL_RENDERBUFFER_EXT.
 *  \param renderbuffer The id of the unused render buffer to bind.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsRenderbuffer, GLDeleteRenderbuffers, GLGenRenderbuffers, GLRenderbufferStorage,
 *  GLGetRenderbufferParameteriv */
inline void GLBindRenderbuffer(GLenum target, GLuint renderbuffer)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glBindRenderbuffer(target, renderbuffer);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glDeleteRenderbuffersEXT.
 *  \param n The number of render buffer ids to delete.
 *  \param renderbuffers A pointer to the render buffer ids to delete.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsRenderbuffer, GLBindRenderbuffer, GLGenRenderbuffers, GLRenderbufferStorage,
 *  GLGetRenderbufferParameteriv */
inline void GLDeleteRenderbuffers(GLsizei n, const GLuint *renderbuffers)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glDeleteRenderbuffers(n, renderbuffers);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glGenRenderbuffersEXT.
 *  \param n The number of render buffer ids to generate.
 *  \param renderbuffers A pointer to the memory to fill with the generated render buffer ids.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsRenderbuffer, GLBindRenderbuffer, GLDeleteRenderbuffers, GLRenderbufferStorage,
 *  GLGetRenderbufferParameteriv */
inline void GLGenRenderbuffers(GLsizei n, GLuint *renderbuffers)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glGenRenderbuffers(n, renderbuffers);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glRenderbufferStorageEXT.
 *  \param target Must be GL_RENDERBUFFER_EXT.
 *  \param internalformat Must be color-renderable, depth-renderable, or stencil-renderable.
 *  \param width The width of the render buffer in pixels.
 *  \param height The height of the render buffer in pixels.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsRenderbuffer, GLBindRenderbuffer, GLDeleteRenderbuffers, GLGenRenderbuffers,
 *  GLGetRenderbufferParameteriv */
inline void GLRenderbufferStorage(GLenum target, GLenum internalformat, GLsizei width, GLsizei height)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glRenderbufferStorage(target, internalformat, width, height);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glRenderbufferStorageMultisampleEXT.
 *  \param target Must be GL_RENDERBUFFER_EXT.
 *  \param samples Is the number of multisample buffers you wish to allocate.
 *  \param internalformat Must be color-renderable, depth-renderable, or stencil-renderable.
 *  \param width The width of the render buffer in pixels.
 *  \param height The height of the render buffer in pixels.
 *  \note It is assumed that the extension GL_EXT_framebuffer_multisample is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsRenderbuffer, GLBindRenderbuffer, GLDeleteRenderbuffers, GLGenRenderbuffers,
 *  GLGetRenderbufferParameteriv, GLRenderbufferStorage, 
 *  GLRenderbufferStorageMultisampleCoverage */
inline void GLRenderbufferStorageMultisample( GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glRenderbufferStorageMultisample( target, samples, internalformat, width, height );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glRenderbufferStorageMultisampleCoverageNV.
 *  \param target Must be GL_RENDERBUFFER_EXT.
 *  \param coverageSamples Is the number of pseudo multisample buffers to allocate.
 *  \param colorSamples Is the number of multisample buffers to allocate.
 *  \param internalformat Must be color-renderable, depth-renderable, or stencil-renderable.
 *  \param width The width of the render buffer in pixels.
 *  \param height The height of the render buffer in pixels.
 *  \note It is assumed that the extension GL_NV_framebuffer_multisample_coverage is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsRenderbuffer, GLBindRenderbuffer, GLDeleteRenderbuffers, GLGenRenderbuffers,
 *  GLGetRenderbufferParameteriv, GLRenderbufferStorage, 
 *  GLRenderbufferStorageMultisampleCoverage */
inline void GLRenderbufferStorageMultisampleCoverage(GLenum target, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glRenderbufferStorageMultisampleCoverage( target, coverageSamples, colorSamples, internalformat, width, height );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glGetRenderbufferParameterivEXT.
 *  \param target Must be GL_RENDERBUFFER_EXT.
 *  \param pname The id of the value to get.
 *  \param params Pointer to the memory to write the queried value to.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsRenderbuffer, GLBindRenderbuffer, GLDeleteRenderbuffers, GLGenRenderbuffers,
 *  GLRenderbufferStorage */
inline void GLGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint *params)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glGetRenderbufferParameteriv(target, pname, params);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glIsFramebufferEXT.
 *  \param framebuffer The id to test.
 *  \return \c TRUE, if \a framebuffer is the id of a frame buffer, otherwise \c FALSE.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers, GLCheckFramebufferStatus,
 *  GLFramebufferTexture1D, GLFramebufferTexture2D, GLFramebufferTexture3D,
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline GLboolean GLIsFramebuffer(GLuint framebuffer)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  GLboolean isFramebuffer = glContext()->m_glFunctions->glIsFramebuffer(framebuffer);
  ASSERT_GL_NO_ERROR;
  return isFramebuffer;
}

/*! \brief Replacement of glBindFramebufferEXT.
 *  \param target The id of the target to bind the frame buffer to. Must be GL_FRAMEBUFFER_EXT.
 *  \param framebuffer The id of the unused frame buffer to bind.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers, GLCheckFramebufferStatus,
 *  GLFramebufferTexture1D, GLFramebufferTexture2D, GLFramebufferTexture3D,
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline void GLBindFramebuffer(GLenum target, GLuint framebuffer)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glBindFramebuffer(target, framebuffer);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glDeleteFramebuffersEXT.
 *  \param n The number of frame buffer ids to delete.
 *  \param framebuffers A pointer to the frame buffer ids to delete.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLGenFramebuffers, GLCheckFramebufferStatus,
 *  GLFramebufferTexture1D, GLFramebufferTexture2D, GLFramebufferTexture3D,
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline void GLDeleteFramebuffers(GLsizei n, const GLuint *framebuffers)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glDeleteFramebuffers(n, framebuffers);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glGenFramebuffersEXT.
 *  \param n The number of frame buffer ids to generate.
 *  \param framebuffers A pointer to the memory to fill with the generated frame buffer ids.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLCheckFramebufferStatus,
 *  GLFramebufferTexture1D, GLFramebufferTexture2D, GLFramebufferTexture3D,
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline void GLGenFramebuffers(GLsizei n, GLuint *framebuffers)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glGenFramebuffers(n, framebuffers);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glCheckFramebufferStatusEXT.
 *  \param target Must be GL_FRAMEBUFFER_EXT.
 *  \return A symbolic value describing the frame buffer status.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers,
 *  GLFramebufferTexture1D, GLFramebufferTexture2D, GLFramebufferTexture3D,
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline GLenum GLCheckFramebufferStatus(GLenum target)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  GLenum status = glContext()->m_glFunctions->glCheckFramebufferStatus(target);
  ASSERT_GL_NO_ERROR;
  return status;
}

/*! \brief Replacement of glFramebufferTexture1DEXT.
 *  \param target Must be GL_FRAMEBUFFER_EXT.
 *  \param attachment One of the attachment points of the frame buffer.
 *  \param textarget The texture target.
 *  \param texture The id of an existing texture with an target of \a textarget.
 *  \param level The mipmap level of the texture image to be attached.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers,
 *  GLCheckFramebufferStatus, GLFramebufferTexture2D, GLFramebufferTexture3D, GLFramebufferTextureLayer
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline void GLFramebufferTexture1D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glFramebufferTexture1D(target, attachment, textarget, texture, level);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glFramebufferTexture2DEXT.
 *  \param target Must be GL_FRAMEBUFFER_EXT.
 *  \param attachment One of the attachment points of the frame buffer.
 *  \param textarget The texture target.
 *  \param texture The id of an existing texture with an target of \a textarget.
 *  \param level The mipmap level of the texture image to be attached.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers,
 *  GLCheckFramebufferStatus, GLFramebufferTexture1D, GLFramebufferTexture3D, GLFramebufferTextureLayer
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline void GLFramebufferTexture2D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glFramebufferTexture2D(target, attachment, textarget, texture, level);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glFramebufferTexture3DEXT.
 *  \param target Must be GL_FRAMEBUFFER_EXT.
 *  \param attachment One of the attachment points of the frame buffer.
 *  \param textarget The texture target.
 *  \param texture The id of an existing texture with an target of \a textarget.
 *  \param level The mipmap level of the texture image to be attached.
 *  \param zoffset Specifies the z-offset of a 2-dimensional image within a 3-dimensional texture.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers,
 *  GLCheckFramebufferStatus, GLFramebufferTexture1D, GLFramebufferTexture2D, GLFramebufferTextureLayer
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline void GLFramebufferTexture3D(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glFramebufferTexture3D(target, attachment, textarget, texture, level, zoffset);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glFramebufferTextureLayerEXT.
 *  \param target Must be GL_FRAMEBUFFER_EXT.
 *  \param attachment One of the attachment points of the frame buffer.
 *  \param texture The id of an existing texture.
 *  \param level The mipmap level of the texture image to be attached.
 *  \param layer Specifies the layer/slice of the texture array - or slice of 3D texture.
 *  \note It is assumed that the extension GL_EXT_texture_array is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers,
 *  GLCheckFramebufferStatus, GLFramebufferTexture1D, GLFramebufferTexture2D,
 *  GLFramebufferRenderbuffer, GLGetFramebufferAttachmentParameteriv */
inline void GLFramebufferTextureLayer(GLenum target, GLenum attachment, GLuint texture, GLint level, GLint layer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glFramebufferTextureLayer( target, attachment, texture, level, layer );
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glFramebufferRenderbufferEXT.
 *  \param target Must be GL_FRAMEBUFFER_EXT.
 *  \param attachment One of the attachment points of the frame buffer.
 *  \param renderbuffertarget Must be GL_RENDERBUFFER_EXT.
 *  \param renderbuffer The id of the render buffer to be attached to the frame buffer.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers,
 *  GLCheckFramebufferStatus, GLFramebufferTexture1D, GLFramebufferTexture2D,
 *  GLFramebufferTexture3D, GLGetFramebufferAttachmentParameteriv */
inline void GLFramebufferRenderbuffer(GLenum target, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glFramebufferRenderbuffer(target, attachment, renderbuffertarget, renderbuffer);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glGetFramebufferAttachmentParameterivEXT.
 *  \param target Must be GL_FRAMEBUFFER_EXT.
 *  \param attachment One of the attachment points of the frame buffer.
 *  \param pname The symbolic values of the attachment parameters to query.
 *  \param params A pointer to the memory where result of the query is written to.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error.
 *  \sa GLIsFramebuffer, GLBindFramebuffer, GLDeleteFramebuffers, GLGenFramebuffers,
 *  GLCheckFramebufferStatus, GLFramebufferTexture1D, GLFramebufferTexture2D,
 *  GLFramebufferTexture3D, GLFramebufferRenderbuffer */
inline void GLGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint *params)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glGetFramebufferAttachmentParameteriv(target, attachment, pname, params);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Replacement of glGenerateMipmapEXT
 *  \param target The symbolic value of the texture target to generate mipmaps for.
 *  \note It is assumed that the extension GL_EXT_framebuffer_object is supported.
 *  \note This function asserts on no GL error. */
inline void GLGenerateMipmap(GLenum target)
{
  NVSG_ASSERT(GLIsExtensionAvailable("GL_EXT_framebuffer_object"));
  glContext()->m_glFunctions->glGenerateMipmap(target);
  ASSERT_GL_NO_ERROR;
}

// GL_NV_present_video
inline void GLPresentFrameKeyedNV(GLuint video_slot, GLuint64EXT minPresentTime, GLuint beginPresentTimeId, GLuint presentDurationId,
                                  GLenum type, GLenum target0, GLuint fill0, GLuint key0, GLenum target1, GLuint fill1, GLuint key1)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPresentFrameKeyedNV(video_slot, minPresentTime, beginPresentTimeId, presentDurationId,
                                                    type, target0, fill0, key0, target1, fill1, key1);
  ASSERT_GL_NO_ERROR;
}

inline void GLPresentFrameDualFillNV(GLuint video_slot, GLuint64EXT minPresentTime, GLuint beginPresentTimeId, GLuint presentDurationId, 
                                     GLenum type, GLenum target0, GLuint fill0, GLenum target1, GLuint fill1, GLenum target2, GLuint fill2, GLenum target3, GLuint fill3)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glPresentFrameDualFillNV(video_slot, minPresentTime, beginPresentTimeId, presentDurationId, 
                                                       type, target0, fill0, target1, fill1, target2, fill2, target3, fill3);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideoivNV(GLuint video_slot, GLenum pname, GLint *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVideoivNV(video_slot, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideouivNV(GLuint video_slot, GLenum pname, GLuint *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVideouivNV(video_slot, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideoi64vNV(GLuint video_slot, GLenum pname, GLint64EXT *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVideoi64vNV(video_slot, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideoui64vNV(GLuint video_slot, GLenum pname, GLuint64EXT *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetVideoui64vNV(video_slot, pname, params);
  ASSERT_GL_NO_ERROR;
}

// GL_NV_vertex_buffer_unified_memory
inline void GLBufferAddressRangeNV(GLenum pname, GLuint index, GLuint64EXT address, GLsizeiptr length)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glBufferAddressRangeNV(pname, index, address, length);
  ASSERT_GL_NO_ERROR;
}


inline void GLVertexFormatNV(GLint size, GLenum type, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glVertexFormatNV);
  glContext()->m_glFunctions->glVertexFormatNV(size, type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLNormalFormatNV(GLenum type, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNormalFormatNV);
  glContext()->m_glFunctions->glNormalFormatNV(type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLColorFormatNV(GLint size, GLenum type, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glColorFormatNV);
  glContext()->m_glFunctions->glColorFormatNV(size, type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLIndexFormatNV(GLenum type, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glIndexFormatNV);
  glContext()->m_glFunctions->glIndexFormatNV(type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLTexCoordFormatNV(GLint size, GLenum type, GLsizei stride)
{
#if defined(GL_CACHING)
  _GLUpdateClientActiveTexture();
#endif
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTexCoordFormatNV);
  glContext()->m_glFunctions->glTexCoordFormatNV(size, type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLEdgeFlagFormatNV(GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glEdgeFlagFormatNV);
  glContext()->m_glFunctions->glEdgeFlagFormatNV(stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLSecondaryColorFormatNV(GLint size, GLenum type, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glSecondaryColorFormatNV);
  glContext()->m_glFunctions->glSecondaryColorFormatNV(size, type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLFogCoordFormatNV(GLenum type, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glFogCoordFormatNV);
  glContext()->m_glFunctions->glFogCoordFormatNV(type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLVertexAttribFormatNV(GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glVertexAttribFormatNV);
  glContext()->m_glFunctions->glVertexAttribFormatNV(index, size, type, normalized, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLVertexAttribIFormatNV(GLuint index, GLint size, GLenum type, GLsizei stride)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glVertexAttribIFormatNV);
  glContext()->m_glFunctions->glVertexAttribIFormatNV(index, size, type, stride);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetIntegerui64i_vNV(GLenum target, GLuint index, GLuint64EXT *data)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions );
  glContext()->m_glFunctions->glGetIntegerui64i_vNV (target, index, data);
  ASSERT_GL_NO_ERROR;
}

// GL_NV_shader_buffer_load
inline void GLGetBufferParameterui64vNV(GLenum target, GLenum pname, GLuint64EXT *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetBufferParameterui64vNV);
  glContext()->m_glFunctions->glGetBufferParameterui64vNV(target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline GLboolean GLIsBufferResidentNV(GLenum target)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glIsBufferResidentNV);
  GLboolean resident = glContext()->m_glFunctions->glIsBufferResidentNV(target);
  ASSERT_GL_NO_ERROR;
  return resident;
}

inline void GLMakeBufferNonResidentNV(GLenum target)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMakeBufferNonResidentNV);
  glContext()->m_glFunctions->glMakeBufferNonResidentNV(target);
  ASSERT_GL_NO_ERROR;
}

inline void GLMakeBufferResidentNV(GLenum target, GLenum access)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMakeBufferResidentNV);
  glContext()->m_glFunctions->glMakeBufferResidentNV(target, access);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedBufferParameterui64vNV(GLuint buffer, GLenum pname, GLuint64EXT *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedBufferParameterui64vNV);
  glContext()->m_glFunctions->glGetNamedBufferParameterui64vNV(buffer, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline GLboolean GLIsNamedBufferResidentNV(GLuint buffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glIsNamedBufferResidentNV);
  GLboolean resident = glContext()->m_glFunctions->glIsNamedBufferResidentNV(buffer);
  ASSERT_GL_NO_ERROR;
  return resident;
}

inline void GLMakeNamedBufferNonResidentNV(GLuint buffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMakeNamedBufferNonResidentNV);
  glContext()->m_glFunctions->glMakeNamedBufferNonResidentNV(buffer);
  ASSERT_GL_NO_ERROR;
}

inline void GLMakeNamedBufferResidentNV(GLuint buffer, GLenum access)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMakeNamedBufferResidentNV);
  glContext()->m_glFunctions->glMakeNamedBufferResidentNV(buffer, access);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetIntegerui64vNV(GLenum target, GLuint64EXT *data)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetIntegerui64vNV);
  glContext()->m_glFunctions->glGetIntegerui64vNV(target, data);
  ASSERT_GL_NO_ERROR;
}

inline void GLUniformui64NV(GLint location, GLuint64EXT v0)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glUniformui64NV);
  glContext()->m_glFunctions->glUniformui64NV(location, v0);
  ASSERT_GL_NO_ERROR;
}

inline void GLUniformui64vNV(GLint location, GLsizei count, const GLuint64EXT *value)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glUniformui64vNV);
  glContext()->m_glFunctions->glUniformui64vNV(location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetUniformui64vNV(GLuint program, GLint location, GLuint64EXT *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetUniformui64vNV);
  glContext()->m_glFunctions->glGetUniformui64vNV(program, location, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformui64NV(GLuint program, GLint location, GLuint64EXT v0)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformui64NV);
  glContext()->m_glFunctions->glProgramUniformui64NV(program, location, v0);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformui64vNV(GLuint program, GLint location, GLsizei count, const GLuint64EXT *value)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformui64vNV);
  glContext()->m_glFunctions->glProgramUniformui64vNV(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

// GL_NV_video_capture
inline void GLBeginVideoCaptureNV (GLuint video_capture_slot)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glBeginVideoCaptureNV);
  glContext()->m_glFunctions->glBeginVideoCaptureNV(video_capture_slot);
  ASSERT_GL_NO_ERROR;
}

inline void GLBindVideoCaptureStreamBufferNV (GLuint video_capture_slot, GLuint stream, GLenum frame_region, GLintptrARB offset)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glBindVideoCaptureStreamBufferNV);
  glContext()->m_glFunctions->glBindVideoCaptureStreamBufferNV(video_capture_slot, stream, frame_region, offset);
  ASSERT_GL_NO_ERROR;
}

inline void GLBindVideoCaptureStreamTextureNV (GLuint video_capture_slot, GLuint stream, GLenum frame_region, GLenum target, GLuint texture)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glBindVideoCaptureStreamTextureNV);
  glContext()->m_glFunctions->glBindVideoCaptureStreamTextureNV(video_capture_slot, stream, frame_region, target, texture);
  ASSERT_GL_NO_ERROR;
}

inline void GLEndVideoCaptureNV (GLuint video_capture_slot)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glEndVideoCaptureNV);
  glContext()->m_glFunctions->glEndVideoCaptureNV(video_capture_slot);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideoCaptureivNV (GLuint video_capture_slot, GLenum pname, GLint *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetVideoCaptureivNV);
  glContext()->m_glFunctions->glGetVideoCaptureivNV(video_capture_slot, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideoCaptureStreamivNV (GLuint video_capture_slot, GLuint stream, GLenum pname, GLint *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetVideoCaptureStreamivNV);
  glContext()->m_glFunctions->glGetVideoCaptureStreamivNV(video_capture_slot, stream, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideoCaptureStreamfvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, GLfloat *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetVideoCaptureStreamfvNV);
  glContext()->m_glFunctions->glGetVideoCaptureStreamfvNV(video_capture_slot, stream, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetVideoCaptureStreamdvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, GLdouble *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetVideoCaptureStreamdvNV);
  glContext()->m_glFunctions->glGetVideoCaptureStreamdvNV(video_capture_slot, stream, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline GLenum GLVideoCaptureNV (GLuint video_capture_slot, GLuint *sequence_num, GLuint64EXT *capture_time)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glVideoCaptureNV);
  GLenum ret = glContext()->m_glFunctions->glVideoCaptureNV(video_capture_slot, sequence_num, capture_time);
  ASSERT_GL_NO_ERROR;
  return ret;
}

inline void GLVideoCaptureStreamParameterivNV (GLuint video_capture_slot, GLuint stream, GLenum pname, const GLint *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glVideoCaptureStreamParameterivNV);
  glContext()->m_glFunctions->glVideoCaptureStreamParameterivNV(video_capture_slot, stream, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLVideoCaptureStreamParameterfvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, const GLfloat *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glVideoCaptureStreamParameterfvNV);
  glContext()->m_glFunctions->glVideoCaptureStreamParameterfvNV(video_capture_slot, stream, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLVideoCaptureStreamParameterdvNV (GLuint video_capture_slot, GLuint stream, GLenum pname, const GLdouble *params)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glVideoCaptureStreamParameterdvNV);
  glContext()->m_glFunctions->glVideoCaptureStreamParameterdvNV(video_capture_slot, stream, pname, params);
  ASSERT_GL_NO_ERROR;
}


// GL_EXT_direct_state_access
inline void GLPushClientAttribDefaultEXT( GLbitfield mask )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glPushClientAttribDefaultEXT);
  glContext()->m_glFunctions->glPushClientAttribDefaultEXT(mask);
  ASSERT_GL_NO_ERROR;
}

inline void GLClientAttribDefaultEXT( GLbitfield mask )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glClientAttribDefaultEXT);
  glContext()->m_glFunctions->glClientAttribDefaultEXT(mask);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixLoadfEXT( GLenum mode, const GLfloat *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixLoadfEXT);
  glContext()->m_glFunctions->glMatrixLoadfEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixLoaddEXT( GLenum mode, const GLdouble *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixLoaddEXT);
  glContext()->m_glFunctions->glMatrixLoaddEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixMultfEXT( GLenum mode, const GLfloat *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixMultfEXT);
  glContext()->m_glFunctions->glMatrixMultfEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixMultdEXT( GLenum mode, const GLdouble *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixMultdEXT);
  glContext()->m_glFunctions->glMatrixMultdEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixLoadIdentityEXT( GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixLoadIdentityEXT);
  glContext()->m_glFunctions->glMatrixLoadIdentityEXT(mode);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixRotatefEXT( GLenum mode, GLfloat angle, GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixRotatefEXT);
  glContext()->m_glFunctions->glMatrixRotatefEXT(mode, angle, x, y, z);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixRotatedEXT( GLenum mode, GLdouble angle, GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixRotatedEXT);
  glContext()->m_glFunctions->glMatrixRotatedEXT(mode, angle, x, y, z);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixScalefEXT( GLenum mode, GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixScalefEXT);
  glContext()->m_glFunctions->glMatrixScalefEXT(mode, x, y, z);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixScaledEXT( GLenum mode, GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixScaledEXT);
  glContext()->m_glFunctions->glMatrixScaledEXT(mode, x, y, z);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixTranslatefEXT( GLenum mode, GLfloat x, GLfloat y, GLfloat z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixTranslatefEXT);
  glContext()->m_glFunctions->glMatrixTranslatefEXT(mode, x, y, z);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixTranslatedEXT( GLenum mode, GLdouble x, GLdouble y, GLdouble z )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixTranslatedEXT);
  glContext()->m_glFunctions->glMatrixTranslatedEXT(mode, x, y, z);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixFrustumEXT( GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixFrustumEXT);
  glContext()->m_glFunctions->glMatrixFrustumEXT(mode, left, right, bottom, top, zNear, zFar);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixOrthoEXT( GLenum mode, GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble zNear, GLdouble zFar )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixOrthoEXT);
  glContext()->m_glFunctions->glMatrixOrthoEXT(mode, left, right, bottom, top, zNear, zFar);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixPopEXT( GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixPopEXT);
  glContext()->m_glFunctions->glMatrixPopEXT(mode);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixPushEXT( GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixPushEXT);
  glContext()->m_glFunctions->glMatrixPushEXT(mode);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixLoadTransposefEXT( GLenum mode, const GLfloat *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixLoadTransposefEXT);
  glContext()->m_glFunctions->glMatrixLoadTransposefEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixLoadTransposedEXT( GLenum mode, const GLdouble *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixLoadTransposedEXT);
  glContext()->m_glFunctions->glMatrixLoadTransposedEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixMultTransposefEXT( GLenum mode, const GLfloat *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixMultTransposefEXT);
  glContext()->m_glFunctions->glMatrixMultTransposefEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLMatrixMultTransposedEXT( GLenum mode, const GLdouble *m )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMatrixMultTransposedEXT);
  glContext()->m_glFunctions->glMatrixMultTransposedEXT(mode, m);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureParameterfEXT( GLuint texture, GLenum target, GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureParameterfEXT);
  glContext()->m_glFunctions->glTextureParameterfEXT(texture, target, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureParameterfvEXT( GLuint texture, GLenum target, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureParameterfvEXT);
  glContext()->m_glFunctions->glTextureParameterfvEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureParameteriEXT( GLuint texture, GLenum target, GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureParameteriEXT);
  glContext()->m_glFunctions->glTextureParameteriEXT(texture, target, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureParameterivEXT( GLuint texture, GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureParameterivEXT);
  glContext()->m_glFunctions->glTextureParameterivEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureImage1DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureImage1DEXT);
  glContext()->m_glFunctions->glTextureImage1DEXT(texture, target, level, internalformat, width, border, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureImage2DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureImage2DEXT);
  glContext()->m_glFunctions->glTextureImage2DEXT(texture, target, level, internalformat, width, height, border, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureSubImage1DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureSubImage1DEXT);
  glContext()->m_glFunctions->glTextureSubImage1DEXT(texture, target, level, xoffset, width, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureSubImage2DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureSubImage2DEXT);
  glContext()->m_glFunctions->glTextureSubImage2DEXT(texture, target, level, xoffset, yoffset, width, height, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyTextureImage1DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyTextureImage1DEXT);
  glContext()->m_glFunctions->glCopyTextureImage1DEXT(texture, target, level, internalformat, x, y, width, border);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyTextureImage2DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyTextureImage2DEXT);
  glContext()->m_glFunctions->glCopyTextureImage2DEXT(texture, target, level, internalformat, x, y, width, height, border);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyTextureSubImage1DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyTextureSubImage1DEXT);
  glContext()->m_glFunctions->glCopyTextureSubImage1DEXT(texture, target, level, xoffset, x, y, width);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyTextureSubImage2DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyTextureSubImage2DEXT);
  glContext()->m_glFunctions->glCopyTextureSubImage2DEXT(texture, target, level, xoffset, yoffset, x, y, width, height);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetTextureImageEXT( GLuint texture, GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetTextureImageEXT);
  glContext()->m_glFunctions->glGetTextureImageEXT(texture, target, level, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetTextureParameterfvEXT( GLuint texture, GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetTextureParameterfvEXT);
  glContext()->m_glFunctions->glGetTextureParameterfvEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetTextureParameterivEXT( GLuint texture, GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetTextureParameterivEXT);
  glContext()->m_glFunctions->glGetTextureParameterivEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetTextureLevelParameterfvEXT( GLuint texture, GLenum target, GLint level, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetTextureLevelParameterfvEXT);
  glContext()->m_glFunctions->glGetTextureLevelParameterfvEXT(texture, target, level, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetTextureLevelParameterivEXT( GLuint texture, GLenum target, GLint level, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetTextureLevelParameterivEXT);
  glContext()->m_glFunctions->glGetTextureLevelParameterivEXT(texture, target, level, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureImage3DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureImage3DEXT);
  glContext()->m_glFunctions->glTextureImage3DEXT(texture, target, level, internalformat, width, height, depth, border, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureSubImage3DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureSubImage3DEXT);
  glContext()->m_glFunctions->glTextureSubImage3DEXT(texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyTextureSubImage3DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyTextureSubImage3DEXT);
  glContext()->m_glFunctions->glCopyTextureSubImage3DEXT(texture, target, level, xoffset, yoffset, zoffset, x, y, width, height);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexParameterfEXT( GLenum texunit, GLenum target, GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexParameterfEXT);
  glContext()->m_glFunctions->glMultiTexParameterfEXT(texunit, target, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexParameterfvEXT( GLenum texunit, GLenum target, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexParameterfvEXT);
  glContext()->m_glFunctions->glMultiTexParameterfvEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexParameteriEXT( GLenum texunit, GLenum target, GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexParameteriEXT);
  glContext()->m_glFunctions->glMultiTexParameteriEXT(texunit, target, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexParameterivEXT( GLenum texunit, GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexParameterivEXT);
  glContext()->m_glFunctions->glMultiTexParameterivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexImage1DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexImage1DEXT);
  glContext()->m_glFunctions->glMultiTexImage1DEXT(texunit, target, level, internalformat, width, border, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexImage2DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexImage2DEXT);
  glContext()->m_glFunctions->glMultiTexImage2DEXT(texunit, target, level, internalformat, width, height, border, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexSubImage1DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexSubImage1DEXT);
  glContext()->m_glFunctions->glMultiTexSubImage1DEXT(texunit, target, level, xoffset, width, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexSubImage2DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexSubImage2DEXT);
  glContext()->m_glFunctions->glMultiTexSubImage2DEXT(texunit, target, level, xoffset, yoffset, width, height, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyMultiTexImage1DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLint border )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyMultiTexImage1DEXT);
  glContext()->m_glFunctions->glCopyMultiTexImage1DEXT(texunit, target, level, internalformat, x, y, width, border);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyMultiTexImage2DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLint x, GLint y, GLsizei width, GLsizei height, GLint border )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyMultiTexImage2DEXT);
  glContext()->m_glFunctions->glCopyMultiTexImage2DEXT(texunit, target, level, internalformat, x, y, width, height, border);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyMultiTexSubImage1DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint x, GLint y, GLsizei width )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyMultiTexSubImage1DEXT);
  glContext()->m_glFunctions->glCopyMultiTexSubImage1DEXT(texunit, target, level, xoffset, x, y, width);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyMultiTexSubImage2DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyMultiTexSubImage2DEXT);
  glContext()->m_glFunctions->glCopyMultiTexSubImage2DEXT(texunit, target, level, xoffset, yoffset, x, y, width, height);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexImageEXT( GLenum texunit, GLenum target, GLint level, GLenum format, GLenum type, GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexImageEXT);
  glContext()->m_glFunctions->glGetMultiTexImageEXT(texunit, target, level, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexParameterfvEXT( GLenum texunit, GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexParameterfvEXT);
  glContext()->m_glFunctions->glGetMultiTexParameterfvEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexParameterivEXT( GLenum texunit, GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexParameterivEXT);
  glContext()->m_glFunctions->glGetMultiTexParameterivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexLevelParameterfvEXT( GLenum texunit, GLenum target, GLint level, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexLevelParameterfvEXT);
  glContext()->m_glFunctions->glGetMultiTexLevelParameterfvEXT(texunit, target, level, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexLevelParameterivEXT( GLenum texunit, GLenum target, GLint level, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexLevelParameterivEXT);
  glContext()->m_glFunctions->glGetMultiTexLevelParameterivEXT(texunit, target, level, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexImage3DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexImage3DEXT);
  glContext()->m_glFunctions->glMultiTexImage3DEXT(texunit, target, level, internalformat, width, height, depth, border, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexSubImage3DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const GLvoid *pixels )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexSubImage3DEXT);
  glContext()->m_glFunctions->glMultiTexSubImage3DEXT(texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels);
  ASSERT_GL_NO_ERROR;
}

inline void GLCopyMultiTexSubImage3DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCopyMultiTexSubImage3DEXT);
  glContext()->m_glFunctions->glCopyMultiTexSubImage3DEXT(texunit, target, level, xoffset, yoffset, zoffset, x, y, width, height);
  ASSERT_GL_NO_ERROR;
}

inline void GLBindMultiTextureEXT( GLenum texunit, GLenum target, GLuint texture )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glBindMultiTextureEXT);
  glContext()->m_glFunctions->glBindMultiTextureEXT(texunit, target, texture);
  ASSERT_GL_NO_ERROR;
}

inline void GLEnableClientStateIndexedEXT( GLenum array, GLuint index )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glEnableClientStateIndexedEXT);
  glContext()->m_glFunctions->glEnableClientStateIndexedEXT(array, index);
  ASSERT_GL_NO_ERROR;
}

inline void GLDisableClientStateIndexedEXT( GLenum array, GLuint index )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glDisableClientStateIndexedEXT);
  glContext()->m_glFunctions->glDisableClientStateIndexedEXT(array, index);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexCoordPointerEXT( GLenum texunit, GLint size, GLenum type, GLsizei stride, const GLvoid *pointer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexCoordPointerEXT);
  glContext()->m_glFunctions->glMultiTexCoordPointerEXT(texunit, size, type, stride, pointer);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexEnvfEXT( GLenum texunit, GLenum target, GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexEnvfEXT);
  glContext()->m_glFunctions->glMultiTexEnvfEXT(texunit, target, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexEnvfvEXT( GLenum texunit, GLenum target, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexEnvfvEXT);
  glContext()->m_glFunctions->glMultiTexEnvfvEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexEnviEXT( GLenum texunit, GLenum target, GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexEnviEXT);
  glContext()->m_glFunctions->glMultiTexEnviEXT(texunit, target, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexEnvivEXT( GLenum texunit, GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexEnvivEXT);
  glContext()->m_glFunctions->glMultiTexEnvivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexGendEXT( GLenum texunit, GLenum coord, GLenum pname, GLdouble param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexGendEXT);
  glContext()->m_glFunctions->glMultiTexGendEXT(texunit, coord, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexGendvEXT( GLenum texunit, GLenum coord, GLenum pname, const GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexGendvEXT);
  glContext()->m_glFunctions->glMultiTexGendvEXT(texunit, coord, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexGenfEXT( GLenum texunit, GLenum coord, GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexGenfEXT);
  glContext()->m_glFunctions->glMultiTexGenfEXT(texunit, coord, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexGenfvEXT( GLenum texunit, GLenum coord, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexGenfvEXT);
  glContext()->m_glFunctions->glMultiTexGenfvEXT(texunit, coord, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexGeniEXT( GLenum texunit, GLenum coord, GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexGeniEXT);
  glContext()->m_glFunctions->glMultiTexGeniEXT(texunit, coord, pname, param);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexGenivEXT( GLenum texunit, GLenum coord, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexGenivEXT);
  glContext()->m_glFunctions->glMultiTexGenivEXT(texunit, coord, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexEnvfvEXT( GLenum texunit, GLenum target, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexEnvfvEXT);
  glContext()->m_glFunctions->glGetMultiTexEnvfvEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexEnvivEXT( GLenum texunit, GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexEnvivEXT);
  glContext()->m_glFunctions->glGetMultiTexEnvivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexGendvEXT( GLenum texunit, GLenum coord, GLenum pname, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexGendvEXT);
  glContext()->m_glFunctions->glGetMultiTexGendvEXT(texunit, coord, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexGenfvEXT( GLenum texunit, GLenum coord, GLenum pname, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexGenfvEXT);
  glContext()->m_glFunctions->glGetMultiTexGenfvEXT(texunit, coord, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexGenivEXT( GLenum texunit, GLenum coord, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexGenivEXT);
  glContext()->m_glFunctions->glGetMultiTexGenivEXT(texunit, coord, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetFloatIndexedvEXT( GLenum target, GLuint index, GLfloat *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetFloatIndexedvEXT);
  glContext()->m_glFunctions->glGetFloatIndexedvEXT(target, index, data);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetDoubleIndexedvEXT( GLenum target, GLuint index, GLdouble *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetDoubleIndexedvEXT);
  glContext()->m_glFunctions->glGetDoubleIndexedvEXT(target, index, data);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetPointerIndexedvEXT( GLenum target, GLuint index, GLvoid* *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetPointerIndexedvEXT);
  glContext()->m_glFunctions->glGetPointerIndexedvEXT(target, index, data);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedTextureImage3DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedTextureImage3DEXT);
  glContext()->m_glFunctions->glCompressedTextureImage3DEXT(texture, target, level, internalformat, width, height, depth, border, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedTextureImage2DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedTextureImage2DEXT);
  glContext()->m_glFunctions->glCompressedTextureImage2DEXT(texture, target, level, internalformat, width, height, border, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedTextureImage1DEXT( GLuint texture, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedTextureImage1DEXT);
  glContext()->m_glFunctions->glCompressedTextureImage1DEXT(texture, target, level, internalformat, width, border, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedTextureSubImage3DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedTextureSubImage3DEXT);
  glContext()->m_glFunctions->glCompressedTextureSubImage3DEXT(texture, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedTextureSubImage2DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedTextureSubImage2DEXT);
  glContext()->m_glFunctions->glCompressedTextureSubImage2DEXT(texture, target, level, xoffset, yoffset, width, height, format, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedTextureSubImage1DEXT( GLuint texture, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedTextureSubImage1DEXT);
  glContext()->m_glFunctions->glCompressedTextureSubImage1DEXT(texture, target, level, xoffset, width, format, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetCompressedTextureImageEXT( GLuint texture, GLenum target, GLint lod, GLvoid *img )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetCompressedTextureImageEXT);
  glContext()->m_glFunctions->glGetCompressedTextureImageEXT(texture, target, lod, img);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedMultiTexImage3DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedMultiTexImage3DEXT);
  glContext()->m_glFunctions->glCompressedMultiTexImage3DEXT(texunit, target, level, internalformat, width, height, depth, border, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedMultiTexImage2DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedMultiTexImage2DEXT);
  glContext()->m_glFunctions->glCompressedMultiTexImage2DEXT(texunit, target, level, internalformat, width, height, border, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedMultiTexImage1DEXT( GLenum texunit, GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedMultiTexImage1DEXT);
  glContext()->m_glFunctions->glCompressedMultiTexImage1DEXT(texunit, target, level, internalformat, width, border, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedMultiTexSubImage3DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedMultiTexSubImage3DEXT);
  glContext()->m_glFunctions->glCompressedMultiTexSubImage3DEXT(texunit, target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedMultiTexSubImage2DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedMultiTexSubImage2DEXT);
  glContext()->m_glFunctions->glCompressedMultiTexSubImage2DEXT(texunit, target, level, xoffset, yoffset, width, height, format, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLCompressedMultiTexSubImage1DEXT( GLenum texunit, GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const GLvoid *bits )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCompressedMultiTexSubImage1DEXT);
  glContext()->m_glFunctions->glCompressedMultiTexSubImage1DEXT(texunit, target, level, xoffset, width, format, imageSize, bits);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetCompressedMultiTexImageEXT( GLenum texunit, GLenum target, GLint lod, GLvoid *img )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetCompressedMultiTexImageEXT);
  glContext()->m_glFunctions->glGetCompressedMultiTexImageEXT(texunit, target, lod, img);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramStringEXT( GLuint program, GLenum target, GLenum format, GLsizei len, const GLvoid *string )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramStringEXT);
  glContext()->m_glFunctions->glNamedProgramStringEXT(program, target, format, len, string);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameter4dEXT( GLuint program, GLenum target, GLuint index, GLdouble x, GLdouble y, GLdouble z, GLdouble w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameter4dEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameter4dEXT(program, target, index, x, y, z, w);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameter4dvEXT( GLuint program, GLenum target, GLuint index, const GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameter4dvEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameter4dvEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameter4fEXT( GLuint program, GLenum target, GLuint index, GLfloat x, GLfloat y, GLfloat z, GLfloat w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameter4fEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameter4fEXT(program, target, index, x, y, z, w);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameter4fvEXT( GLuint program, GLenum target, GLuint index, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameter4fvEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameter4fvEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedProgramLocalParameterdvEXT( GLuint program, GLenum target, GLuint index, GLdouble *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedProgramLocalParameterdvEXT);
  glContext()->m_glFunctions->glGetNamedProgramLocalParameterdvEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedProgramLocalParameterfvEXT( GLuint program, GLenum target, GLuint index, GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedProgramLocalParameterfvEXT);
  glContext()->m_glFunctions->glGetNamedProgramLocalParameterfvEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedProgramivEXT( GLuint program, GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedProgramivEXT);
  glContext()->m_glFunctions->glGetNamedProgramivEXT(program, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedProgramStringEXT( GLuint program, GLenum target, GLenum pname, GLvoid *string )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedProgramStringEXT);
  glContext()->m_glFunctions->glGetNamedProgramStringEXT(program, target, pname, string);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameters4fvEXT( GLuint program, GLenum target, GLuint index, GLsizei count, const GLfloat *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameters4fvEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameters4fvEXT(program, target, index, count, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameterI4iEXT( GLuint program, GLenum target, GLuint index, GLint x, GLint y, GLint z, GLint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameterI4iEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameterI4iEXT(program, target, index, x, y, z, w);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameterI4ivEXT( GLuint program, GLenum target, GLuint index, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameterI4ivEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameterI4ivEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParametersI4ivEXT( GLuint program, GLenum target, GLuint index, GLsizei count, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParametersI4ivEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParametersI4ivEXT(program, target, index, count, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameterI4uiEXT( GLuint program, GLenum target, GLuint index, GLuint x, GLuint y, GLuint z, GLuint w )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameterI4uiEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameterI4uiEXT(program, target, index, x, y, z, w);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParameterI4uivEXT( GLuint program, GLenum target, GLuint index, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParameterI4uivEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParameterI4uivEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedProgramLocalParametersI4uivEXT( GLuint program, GLenum target, GLuint index, GLsizei count, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedProgramLocalParametersI4uivEXT);
  glContext()->m_glFunctions->glNamedProgramLocalParametersI4uivEXT(program, target, index, count, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedProgramLocalParameterIivEXT( GLuint program, GLenum target, GLuint index, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedProgramLocalParameterIivEXT);
  glContext()->m_glFunctions->glGetNamedProgramLocalParameterIivEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedProgramLocalParameterIuivEXT( GLuint program, GLenum target, GLuint index, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedProgramLocalParameterIuivEXT);
  glContext()->m_glFunctions->glGetNamedProgramLocalParameterIuivEXT(program, target, index, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureParameterIivEXT( GLuint texture, GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureParameterIivEXT);
  glContext()->m_glFunctions->glTextureParameterIivEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureParameterIuivEXT( GLuint texture, GLenum target, GLenum pname, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureParameterIuivEXT);
  glContext()->m_glFunctions->glTextureParameterIuivEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetTextureParameterIivEXT( GLuint texture, GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetTextureParameterIivEXT);
  glContext()->m_glFunctions->glGetTextureParameterIivEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetTextureParameterIuivEXT( GLuint texture, GLenum target, GLenum pname, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetTextureParameterIuivEXT);
  glContext()->m_glFunctions->glGetTextureParameterIuivEXT(texture, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexParameterIivEXT( GLenum texunit, GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexParameterIivEXT);
  glContext()->m_glFunctions->glMultiTexParameterIivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexParameterIuivEXT( GLenum texunit, GLenum target, GLenum pname, const GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexParameterIuivEXT);
  glContext()->m_glFunctions->glMultiTexParameterIuivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexParameterIivEXT( GLenum texunit, GLenum target, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexParameterIivEXT);
  glContext()->m_glFunctions->glGetMultiTexParameterIivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetMultiTexParameterIuivEXT( GLenum texunit, GLenum target, GLenum pname, GLuint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetMultiTexParameterIuivEXT);
  glContext()->m_glFunctions->glGetMultiTexParameterIuivEXT(texunit, target, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform1fEXT( GLuint program, GLint location, GLfloat v0 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform1fEXT);
  glContext()->m_glFunctions->glProgramUniform1fEXT(program, location, v0);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform2fEXT( GLuint program, GLint location, GLfloat v0, GLfloat v1 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform2fEXT);
  glContext()->m_glFunctions->glProgramUniform2fEXT(program, location, v0, v1);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform3fEXT( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform3fEXT);
  glContext()->m_glFunctions->glProgramUniform3fEXT(program, location, v0, v1, v2);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform4fEXT( GLuint program, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform4fEXT);
  glContext()->m_glFunctions->glProgramUniform4fEXT(program, location, v0, v1, v2, v3);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform1iEXT( GLuint program, GLint location, GLint v0 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform1iEXT);
  glContext()->m_glFunctions->glProgramUniform1iEXT(program, location, v0);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform2iEXT( GLuint program, GLint location, GLint v0, GLint v1 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform2iEXT);
  glContext()->m_glFunctions->glProgramUniform2iEXT(program, location, v0, v1);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform3iEXT( GLuint program, GLint location, GLint v0, GLint v1, GLint v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform3iEXT);
  glContext()->m_glFunctions->glProgramUniform3iEXT(program, location, v0, v1, v2);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform4iEXT( GLuint program, GLint location, GLint v0, GLint v1, GLint v2, GLint v3 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform4iEXT);
  glContext()->m_glFunctions->glProgramUniform4iEXT(program, location, v0, v1, v2, v3);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform1fvEXT( GLuint program, GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform1fvEXT);
  glContext()->m_glFunctions->glProgramUniform1fvEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform2fvEXT( GLuint program, GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform2fvEXT);
  glContext()->m_glFunctions->glProgramUniform2fvEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform3fvEXT( GLuint program, GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform3fvEXT);
  glContext()->m_glFunctions->glProgramUniform3fvEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform4fvEXT( GLuint program, GLint location, GLsizei count, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform4fvEXT);
  glContext()->m_glFunctions->glProgramUniform4fvEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform1ivEXT( GLuint program, GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform1ivEXT);
  glContext()->m_glFunctions->glProgramUniform1ivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform2ivEXT( GLuint program, GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform2ivEXT);
  glContext()->m_glFunctions->glProgramUniform2ivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform3ivEXT( GLuint program, GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform3ivEXT);
  glContext()->m_glFunctions->glProgramUniform3ivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform4ivEXT( GLuint program, GLint location, GLsizei count, const GLint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform4ivEXT);
  glContext()->m_glFunctions->glProgramUniform4ivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix2fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix2fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix2fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix3fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix3fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix3fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix4fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix4fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix4fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix2x3fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix2x3fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix2x3fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix3x2fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix3x2fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix3x2fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix2x4fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix2x4fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix2x4fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix4x2fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix4x2fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix4x2fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix3x4fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix3x4fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix3x4fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniformMatrix4x3fvEXT( GLuint program, GLint location, GLsizei count, GLboolean transpose, const GLfloat *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniformMatrix4x3fvEXT);
  glContext()->m_glFunctions->glProgramUniformMatrix4x3fvEXT(program, location, count, transpose, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform1uiEXT( GLuint program, GLint location, GLuint v0 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform1uiEXT);
  glContext()->m_glFunctions->glProgramUniform1uiEXT(program, location, v0);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform2uiEXT( GLuint program, GLint location, GLuint v0, GLuint v1 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform2uiEXT);
  glContext()->m_glFunctions->glProgramUniform2uiEXT(program, location, v0, v1);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform3uiEXT( GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform3uiEXT);
  glContext()->m_glFunctions->glProgramUniform3uiEXT(program, location, v0, v1, v2);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform4uiEXT( GLuint program, GLint location, GLuint v0, GLuint v1, GLuint v2, GLuint v3 )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform4uiEXT);
  glContext()->m_glFunctions->glProgramUniform4uiEXT(program, location, v0, v1, v2, v3);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform1uivEXT( GLuint program, GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform1uivEXT);
  glContext()->m_glFunctions->glProgramUniform1uivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform2uivEXT( GLuint program, GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform2uivEXT);
  glContext()->m_glFunctions->glProgramUniform2uivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform3uivEXT( GLuint program, GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform3uivEXT);
  glContext()->m_glFunctions->glProgramUniform3uivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLProgramUniform4uivEXT( GLuint program, GLint location, GLsizei count, const GLuint *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glProgramUniform4uivEXT);
  glContext()->m_glFunctions->glProgramUniform4uivEXT(program, location, count, value);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedBufferDataEXT( GLuint buffer, GLsizeiptr size, const GLvoid *data, GLenum usage )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedBufferDataEXT);
  glContext()->m_glFunctions->glNamedBufferDataEXT(buffer, size, data, usage);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedBufferSubDataEXT( GLuint buffer, GLintptr offset, GLsizeiptr size, const GLvoid *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedBufferSubDataEXT);
  glContext()->m_glFunctions->glNamedBufferSubDataEXT(buffer, offset, size, data);
  ASSERT_GL_NO_ERROR;
}

inline GLvoid* GLMapNamedBufferEXT( GLuint buffer, GLenum access )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMapNamedBufferEXT);
  GLvoid *result = glContext()->m_glFunctions->glMapNamedBufferEXT(buffer, access);
  ASSERT_GL_NO_ERROR;
  return result;
}

inline GLboolean GLUnmapNamedBufferEXT( GLuint buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glUnmapNamedBufferEXT);
  GLboolean result = glContext()->m_glFunctions->glUnmapNamedBufferEXT(buffer);
  ASSERT_GL_NO_ERROR;
  return result;
}

inline void GLGetNamedBufferParameterivEXT( GLuint buffer, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedBufferParameterivEXT);
  glContext()->m_glFunctions->glGetNamedBufferParameterivEXT(buffer, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedBufferPointervEXT( GLuint buffer, GLenum pname, GLvoid* *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedBufferPointervEXT);
  glContext()->m_glFunctions->glGetNamedBufferPointervEXT(buffer, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedBufferSubDataEXT( GLuint buffer, GLintptr offset, GLsizeiptr size, GLvoid *data )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedBufferSubDataEXT);
  glContext()->m_glFunctions->glGetNamedBufferSubDataEXT(buffer, offset, size, data);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureBufferEXT( GLuint texture, GLenum target, GLenum internalformat, GLuint buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureBufferEXT);
  glContext()->m_glFunctions->glTextureBufferEXT(texture, target, internalformat, buffer);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexBufferEXT( GLenum texunit, GLenum target, GLenum internalformat, GLuint buffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexBufferEXT);
  glContext()->m_glFunctions->glMultiTexBufferEXT(texunit, target, internalformat, buffer);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedRenderbufferStorageEXT( GLuint renderbuffer, GLenum internalformat, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedRenderbufferStorageEXT);
  glContext()->m_glFunctions->glNamedRenderbufferStorageEXT(renderbuffer, internalformat, width, height);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedRenderbufferParameterivEXT( GLuint renderbuffer, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedRenderbufferParameterivEXT);
  glContext()->m_glFunctions->glGetNamedRenderbufferParameterivEXT(renderbuffer, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline GLenum GLCheckNamedFramebufferStatusEXT( GLuint framebuffer, GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glCheckNamedFramebufferStatusEXT);
  GLenum result = glContext()->m_glFunctions->glCheckNamedFramebufferStatusEXT(framebuffer, target);
  ASSERT_GL_NO_ERROR;
  return result;
}

inline void GLNamedFramebufferTexture1DEXT( GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedFramebufferTexture1DEXT);
  glContext()->m_glFunctions->glNamedFramebufferTexture1DEXT(framebuffer, attachment, textarget, texture, level);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedFramebufferTexture2DEXT( GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedFramebufferTexture2DEXT);
  glContext()->m_glFunctions->glNamedFramebufferTexture2DEXT(framebuffer, attachment, textarget, texture, level);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedFramebufferTexture3DEXT( GLuint framebuffer, GLenum attachment, GLenum textarget, GLuint texture, GLint level, GLint zoffset )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedFramebufferTexture3DEXT);
  glContext()->m_glFunctions->glNamedFramebufferTexture3DEXT(framebuffer, attachment, textarget, texture, level, zoffset);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedFramebufferRenderbufferEXT( GLuint framebuffer, GLenum attachment, GLenum renderbuffertarget, GLuint renderbuffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedFramebufferRenderbufferEXT);
  glContext()->m_glFunctions->glNamedFramebufferRenderbufferEXT(framebuffer, attachment, renderbuffertarget, renderbuffer);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetNamedFramebufferAttachmentParameterivEXT( GLuint framebuffer, GLenum attachment, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetNamedFramebufferAttachmentParameterivEXT);
  glContext()->m_glFunctions->glGetNamedFramebufferAttachmentParameterivEXT(framebuffer, attachment, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLGenerateTextureMipmapEXT( GLuint texture, GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGenerateTextureMipmapEXT);
  glContext()->m_glFunctions->glGenerateTextureMipmapEXT(texture, target);
  ASSERT_GL_NO_ERROR;
}

inline void GLGenerateMultiTexMipmapEXT( GLenum texunit, GLenum target )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGenerateMultiTexMipmapEXT);
  glContext()->m_glFunctions->glGenerateMultiTexMipmapEXT(texunit, target);
  ASSERT_GL_NO_ERROR;
}

inline void GLFramebufferDrawBufferEXT( GLuint framebuffer, GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glFramebufferDrawBufferEXT);
  glContext()->m_glFunctions->glFramebufferDrawBufferEXT(framebuffer, mode);
  ASSERT_GL_NO_ERROR;
}

inline void GLFramebufferDrawBuffersEXT( GLuint framebuffer, GLsizei n, const GLenum *bufs )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glFramebufferDrawBuffersEXT);
  glContext()->m_glFunctions->glFramebufferDrawBuffersEXT(framebuffer, n, bufs);
  ASSERT_GL_NO_ERROR;
}

inline void GLFramebufferReadBufferEXT( GLuint framebuffer, GLenum mode )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glFramebufferReadBufferEXT);
  glContext()->m_glFunctions->glFramebufferReadBufferEXT(framebuffer, mode);
  ASSERT_GL_NO_ERROR;
}

inline void GLGetFramebufferParameterivEXT( GLuint framebuffer, GLenum pname, GLint *params )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glGetFramebufferParameterivEXT);
  glContext()->m_glFunctions->glGetFramebufferParameterivEXT(framebuffer, pname, params);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedRenderbufferStorageMultisampleEXT( GLuint renderbuffer, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedRenderbufferStorageMultisampleEXT);
  glContext()->m_glFunctions->glNamedRenderbufferStorageMultisampleEXT(renderbuffer, samples, internalformat, width, height);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedRenderbufferStorageMultisampleCoverageEXT( GLuint renderbuffer, GLsizei coverageSamples, GLsizei colorSamples, GLenum internalformat, GLsizei width, GLsizei height )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedRenderbufferStorageMultisampleCoverageEXT);
  glContext()->m_glFunctions->glNamedRenderbufferStorageMultisampleCoverageEXT(renderbuffer, coverageSamples, colorSamples, internalformat, width, height);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedFramebufferTextureEXT( GLuint framebuffer, GLenum attachment, GLuint texture, GLint level )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedFramebufferTextureEXT);
  glContext()->m_glFunctions->glNamedFramebufferTextureEXT(framebuffer, attachment, texture, level);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedFramebufferTextureLayerEXT( GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLint layer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedFramebufferTextureLayerEXT);
  glContext()->m_glFunctions->glNamedFramebufferTextureLayerEXT(framebuffer, attachment, texture, level, layer);
  ASSERT_GL_NO_ERROR;
}

inline void GLNamedFramebufferTextureFaceEXT( GLuint framebuffer, GLenum attachment, GLuint texture, GLint level, GLenum face )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glNamedFramebufferTextureFaceEXT);
  glContext()->m_glFunctions->glNamedFramebufferTextureFaceEXT(framebuffer, attachment, texture, level, face);
  ASSERT_GL_NO_ERROR;
}

inline void GLTextureRenderbufferEXT( GLuint texture, GLenum target, GLuint renderbuffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glTextureRenderbufferEXT);
  glContext()->m_glFunctions->glTextureRenderbufferEXT(texture, target, renderbuffer);
  ASSERT_GL_NO_ERROR;
}

inline void GLMultiTexRenderbufferEXT( GLenum texunit, GLenum target, GLuint renderbuffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( glContext()->m_glFunctions && glContext()->m_glFunctions->glMultiTexRenderbufferEXT);
  glContext()->m_glFunctions->glMultiTexRenderbufferEXT(texunit, target, renderbuffer);
  ASSERT_GL_NO_ERROR;
}

/*! \brief Translate an nvsg::TextureWrapMode into a GL-specific texture wrap mode as a GLenum.
 *  \param twm The TextureWrapMode to translate.
 *  \return The OpenGL-specific GLenum being a 'best match' of the nvsg::TextureWrapMode. */
inline GLenum GLGetTexWrapMode( nvsg::TextureWrapMode twm )
{
  NVSG_ASSERT( twm <= nvsg::TWM_MIRROR_CLAMP_TO_BORDER );
  return( glContext()->m_glTexWrapModes[twm] );
}

#if defined(_WIN32)

/*! \brief Initialize the wgl-extension functions inside the WGLFunctions class.
 *  \remarks This doesn't need to be called more than once from a hardware 
 *  OpenGL context to make the extended wgl-interface functions available
 *  globally to the process (but it can handle being called more than once).
 *  The WGL-wrapper functions below don't depend on any \c GLContext.
 *  This returns false if the \c wglGetExtensionsStringEXT function pointer is not found.
 */
inline bool WGLInitializeExtensions(void)
{
  return theWGLFunctions::instance()->Init();
}

/*! \brief Returns whether the specified wgl-extension is available.
 *  \param extstr Designates the requested wgl-extension.
 *  \return The function returns \c true if the requested wgl-extension is available
 *  and if the corresponding function pointer are initialized properly. Otherwise
 *  the function returns \c false. The function always returns \c false if called prior
 *  to WGLInitializeExtensions.
 *  \remarks WGLIsExtensionAvailable should not be called prior to WGLInitializeExtension.
 *  Doing so would be entirely meaningless and the function would always return false.
 *  It is recommended to test whether a requested wgl-extension is available on the
 *  current configuration prior to exploit the specified wgl-extension. Using a not available 
 *  wgl-extension through the 'WGL' wrapper function provided by SceniX results in 
 *  unexpected behavior.
 *  \sa WGLInitializeExtensions */
inline bool WGLIsExtensionAvailable(const std::string& extstr)
{
  return theWGLFunctions::instance()->isAvailable(extstr);
}

/*! \brief Returns whether the specified wgl-extension is exported.
 *  \param extstr Designates the requested wgl-extension.
 *  \return The function returns \c true if the requested wgl-extension is exported
 *  by the current configuration. Otherwise the function returns \c false. 
 *  The function always returns \c false if called prior to WGLInitializeExtensions.
 *  \remarks WGLIsExtensionExported should not be called prior to WGLInitializeExtension.
 *  Doing so would be entirely meaningless and the function would always return false.
 *  If a wgl-extension defines new functions, WGLIsExtensionAvailable should be called
 *  instead of WGLIsExtensionExported to ensure that all required function pointers
 *  are properly initialized.
 *  \sa WGLInitializeExtensions, WGLIsExtensionAvailable */
inline bool WGLIsExtensionExported(const std::string& extstr)
{
  return theWGLFunctions::instance()->isExported(extstr);
}

//! Replacement of wglBindTexImageARB
/** This function hides the extension character of wglBindTexImageARB and asserts on its result before returning. */
inline BOOL WGLBindTexImage( HPBUFFERARB hPbuffer, int iBuffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglBindTexImageARB );
  return theWGLFunctions::instance()->wglBindTexImageARB( hPbuffer, iBuffer );
}

//! Replacement of wglChoosePixelFormatARB
/** This function hides the extension character of wglChoosePixelFormatARB and asserts on its result before returning. */
inline BOOL WGLChoosePixelFormat( HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglChoosePixelFormatARB );
  return theWGLFunctions::instance()->wglChoosePixelFormatARB( hdc, piAttribIList, pfAttribFList, nMaxFormats, piFormats, nNumFormats );
}

//! Replacement of wglGetPixelFormatAttribivARB
/** This function hides the extension character of wglGetPixelFormatAttribivARB and asserts on its result before returning. */
inline BOOL WGLGetPixelFormatAttribivARB(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, int *piValues )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglGetPixelFormatAttribivARB );
  return theWGLFunctions::instance()->wglGetPixelFormatAttribivARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, piValues );
}

//! Replacement of wglGetPixelFormatAttribfvARB
/** This function hides the extension character of wglGetPixelFormatAttribfvARB and asserts on its result before returning. */
inline BOOL WGLGetPixelFormatAttribfv(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, const int *piAttributes, FLOAT *pfValues )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglGetPixelFormatAttribfvARB );
  return theWGLFunctions::instance()->wglGetPixelFormatAttribfvARB(hdc, iPixelFormat, iLayerPlane, nAttributes, piAttributes, pfValues );
}


//! Replacement of wglCreateContext.
/** If this function is used in conjunction with WGLCreateContext, WGLMakeCurrent, and the functions with
  * uppercase GL prefix, then the states and attributes are cached per context. Moreover, the extension functions of
  * OpenGL are used per context (as they really should!). */
inline  HGLRC WGLCreateContext( HDC hdc )
{
  HGLRC hglrc = wglCreateContext( hdc );
  WGLAttachContext(hdc, hglrc);
  return hglrc;
}

//! Replacement of wglCreatePbufferARB
/** This function hides the extension character of wglCreatePbufferARB. 
  * \note The creation of a PBuffer might fail dependent on available hardware resources. 
  * In this case a NULL handle will be returned. */
inline  HPBUFFERARB WGLCreatePbuffer( HDC hdc, int pixelFormat, int width, int height, const int *attribList )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( wglGetCurrentContext() != NULL );
  NVSG_ASSERT( theWGLFunctions::instance()->wglCreatePbufferARB );
  HPBUFFERARB hPbuffer = theWGLFunctions::instance()->wglCreatePbufferARB( hdc, pixelFormat, width, height, attribList );
  return( hPbuffer );
}

//! Replacement of wglDeleteContext.
/** If this function is used in conjunction with WGLDeleteContext, WGLMakeCurrent, and functions with
  * uppercase 'GL' prefix, the states and attributes are cached per context. 
  * Moreover, OpenGL extension functions are used per context (as they really should!).
  */
inline  BOOL  WGLDeleteContext( HGLRC hglrc )
{
  NVSG_GL_STATS_FCT_COUNT();
  WGLDetachContext(hglrc);
  return wglDeleteContext(hglrc);
}

//! Replacement of wglDestroyPbufferARB
/** This function hides the extension character of wglDestroyPbufferARB and asserts on its result before returning. */
inline  BOOL  WGLDestroyPbuffer( HPBUFFERARB hPbuffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT(theWGLFunctions::instance()->wglDestroyPbufferARB);
  return theWGLFunctions::instance()->wglDestroyPbufferARB( hPbuffer );
}

//! Replacement for wglGetCurrenContext
/** The function obtains a handle to the current OpenGL rendering context of the calling thread. 
  * \returns A handle to the current OpenGL context of the calling thread, or NULL if the calling thread 
  * has no current OpenGL context.
  */
inline  HGLRC  WGLGetCurrentContext(void)
{
  // !! NOTE: no NVSG_GL_STATS_FCT_COUNT macro call here, because it relies on a valid glContext !!
  ASSERT_GL_CURRENT_CONTEXT_CONFORMITY();
  return wglGetCurrentContext();
}

//! Replacement for wglGetCurrentDC
/** The function obtains a handle to the device context that is associated with the current OpenGL rendering context 
  * of the calling thread. 
  * \returns A handle to the device context which is currently associated with the current OpenGL context of the 
  * calling thread by means of WGLMakeCurrent, or NULL if the calling thread has no current OpenGL context.
  */
inline  HDC  WGLGetCurrentDC(void)
{
  NVSG_GL_STATS_FCT_COUNT();
  return wglGetCurrentDC();
}

//! Replacement of wglGetPbufferDCARB
/** This function hides the extension character of wglGetPbufferDCARB and asserts on its result before returning. */
inline  HDC WGLGetPbufferDC( HPBUFFERARB hPbuffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglGetPbufferDCARB );
  return theWGLFunctions::instance()->wglGetPbufferDCARB( hPbuffer );
}

//! Replacement of wglQueryPbufferARB
/** This function hides the extension character of wglQueryPbufferARB and asserts on its result before returning. */
inline  BOOL  WGLQueryPbuffer( HPBUFFERARB hPbuffer, int attribute, int *value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglQueryPbufferARB );
  return theWGLFunctions::instance()->wglQueryPbufferARB( hPbuffer, attribute, value );
}

//! Replacement of wglReleasePbufferDCARB
/** This function hides the extension character of wglReleasePbufferDCARB and asserts on its result before returning. */
inline  int WGLReleasePbufferDC( HPBUFFERARB hPbuffer, HDC hdc )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglReleasePbufferDCARB );
  int i = theWGLFunctions::instance()->wglReleasePbufferDCARB( hPbuffer, hdc );
  NVSG_ASSERT( i == 1 );
  return( i );
}

//! Replacement of wglReleaseTexImageARB
/** This function hides the extension character of wglReleaseTexImageARB and asserts on its result before returning. */
inline  BOOL  WGLReleaseTexImage( HPBUFFERARB hPbuffer, int iBuffer )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglReleaseTexImageARB );
  return theWGLFunctions::instance()->wglReleaseTexImageARB( hPbuffer, iBuffer );
}

//! Replacement of wglJointSwapGroupNV
inline BOOL WGLJoinSwapGroupNV(HDC hDC, GLuint group)
{
  NVSG_GL_STATS_FCT_COUNT();
  return theWGLFunctions::instance()->wglJoinSwapGroupNV(hDC, group);
}

//! Replacement of wglBindSwapBarrierNV
inline BOOL WGLBindSwapBarrierNV(GLuint group, GLuint barrier)
{
  NVSG_GL_STATS_FCT_COUNT();
  return theWGLFunctions::instance()->wglBindSwapBarrierNV(group, barrier);
}

//! Replacement of wglQuerySwapGroupNV
inline BOOL WGLQuerySwapGroupNV(HDC hDC, GLuint * group, GLuint * barrier)
{
  NVSG_GL_STATS_FCT_COUNT();
  return theWGLFunctions::instance()->wglQuerySwapGroupNV(hDC, group, barrier);
}

//! Replacement of wglQueryMaxSwapGroupsNV
inline BOOL WGLQueryMaxSwapGroupsNV(HDC hDC, GLuint * maxGroups, GLuint * maxBarriers)
{
  NVSG_GL_STATS_FCT_COUNT();
  return theWGLFunctions::instance()->wglQueryMaxSwapGroupsNV(hDC, maxGroups, maxBarriers);
}

//! Replacement of wglQueryFrameCountNV
inline BOOL WGLQueryFrameCountNV(HDC hDC, GLuint * count)
{
  NVSG_GL_STATS_FCT_COUNT();
  return theWGLFunctions::instance()->wglQueryFrameCountNV(hDC, count);
}

//! Replacement of wglResetFrameCountNV
inline BOOL WGLResetFrameCountNV(HDC hDC)
{
  NVSG_GL_STATS_FCT_COUNT();
  return theWGLFunctions::instance()->wglResetFrameCountNV(hDC);
}

//! Replacement of wglEnumGpusNV
inline BOOL WGLEnumGpus(UINT iIndex, HGPUNV *hGpu)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglEnumGpusNV );
  return theWGLFunctions::instance()->wglEnumGpusNV(iIndex, hGpu);
}

//! Replacement of wglEnumGpuDevicesNV
inline BOOL WGLEnumGpuDevices(HGPUNV hGpu, UINT iIndex, PGPU_DEVICE pGpuDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglEnumGpuDevicesNV );
  return theWGLFunctions::instance()->wglEnumGpuDevicesNV(hGpu, iIndex, pGpuDevice);
}

//! Replacement for wglCreateAffinityDVNV
inline HDC  WGLCreateAffinityDC(const HGPUNV *pGpuList)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglCreateAffinityDCNV );
  return theWGLFunctions::instance()->wglCreateAffinityDCNV(pGpuList);
}

//! Replacement for wglEnumGpusFromAffinityDCNV
inline BOOL WGLEnumGpusFromAffinityDC(HDC hAffinityDC, UINT iIndex, HGPUNV *hGpu)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglEnumGpusFromAffinityDCNV );
  return theWGLFunctions::instance()->wglEnumGpusFromAffinityDCNV(hAffinityDC, iIndex, hGpu);
}

//! Replacement for wglDeleteDCNV
inline BOOL WGLDeleteDC(HDC hAffinityDC)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglDeleteDCNV );
  return theWGLFunctions::instance()->wglDeleteDCNV(hAffinityDC);
}

//! Replacement for wglSwapIntervalEXT
inline BOOL WGLSwapInterval(int interval)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglSwapIntervalEXT );
  return theWGLFunctions::instance()->wglSwapIntervalEXT(interval);
}

//! Replacement for wglGetSwapIntervalEXT
inline int WGLGetSwapInterval(void)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglGetSwapIntervalEXT );
  return theWGLFunctions::instance()->wglGetSwapIntervalEXT();
}

// WGL_NV_video_out functions:

//! Replacement for wglGetVideoDeviceNV
inline BOOL WGLGetVideoDevice(HDC hDC, INT iVideoStreams, HPVIDEODEVICE* phpVideoDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglGetVideoDeviceNV );
  return theWGLFunctions::instance()->wglGetVideoDeviceNV(hDC, iVideoStreams, phpVideoDevice);
}

//! Replacement for wglReleaseVideoDeviceNV
inline BOOL WGLReleaseVideoDevice(HPVIDEODEVICE hpVideoDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglReleaseVideoDeviceNV );
  return theWGLFunctions::instance()->wglReleaseVideoDeviceNV(hpVideoDevice);
}

//! Replacement for wglBindVideoImageNV
inline BOOL WGLBindVideoImage(HPVIDEODEVICE hpVideoDevice, HPBUFFERARB hPbuffer, INT iVideoBuffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglBindVideoImageNV );
  return theWGLFunctions::instance()->wglBindVideoImageNV(hpVideoDevice, hPbuffer, iVideoBuffer);
}

//! Replacement for wglReleaseVideoImageNV
inline BOOL WGLReleaseVideoImage(HPBUFFERARB hPbuffer, INT iVideoBuffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglReleaseVideoImageNV );
  return theWGLFunctions::instance()->wglReleaseVideoImageNV(hPbuffer, iVideoBuffer);
}

//! Replacement for wglSendPbufferToVideoNV
inline BOOL WGLSendPbufferToVideo(HPBUFFERARB hPbuffer, INT iBufferType, unsigned long * pulCounterPbuffer, BOOL bBlock)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglSendPbufferToVideoNV );
  return theWGLFunctions::instance()->wglSendPbufferToVideoNV(hPbuffer, iBufferType, pulCounterPbuffer, bBlock);
}

//! Replacement for wglGetVideoInfoNV
inline BOOL WGLGetVideoInfo(HPVIDEODEVICE hpVideoDevice, unsigned long * pulCounterOutputVideo, unsigned long * pulCounterOutputPbuffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglGetVideoInfoNV );
  return theWGLFunctions::instance()->wglGetVideoInfoNV(hpVideoDevice, pulCounterOutputVideo, pulCounterOutputPbuffer);
}

// WGL_NV_present_video
//! Replacement for wglEnumerateVideoDevicesNV
inline int WGLEnumerateVideoDevicesNV(HDC hDC, HVIDEOOUTPUTDEVICENV *phDeviceList)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglEnumerateVideoDevicesNV );
  return theWGLFunctions::instance()->wglEnumerateVideoDevicesNV(hDC, phDeviceList);
}

//! Replacement for wglBindVideoDeviceNV
inline BOOL WGLBindVideoDeviceNV(HDC hDC, unsigned int uVideoSlot, HVIDEOOUTPUTDEVICENV hVideoDevice, const int *piAttribList)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglBindVideoDeviceNV );
  return theWGLFunctions::instance()->wglBindVideoDeviceNV(hDC, uVideoSlot, hVideoDevice, piAttribList);
}

//! Replacement for wglQueryCurrentContextNV
inline BOOL WGLQueryCurrentContextNV(int iAttribute, int *piValue)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglQueryCurrentContextNV );
  return theWGLFunctions::instance()->wglQueryCurrentContextNV(iAttribute, piValue);
}

// WGL_NV_video_capture
inline BOOL WGLBindVideoCaptureDeviceNV(UINT uVideoSlot, HVIDEOINPUTDEVICENV hDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglBindVideoCaptureDeviceNV );
  return theWGLFunctions::instance()->wglBindVideoCaptureDeviceNV(uVideoSlot, hDevice);
}

inline UINT WGLEnumerateVideoCaptureDevicesNV(HDC hDc, HVIDEOINPUTDEVICENV *phDeviceList)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglEnumerateVideoCaptureDevicesNV );
  return theWGLFunctions::instance()->wglEnumerateVideoCaptureDevicesNV(hDc, phDeviceList);
}

inline BOOL WGLLockVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglLockVideoCaptureDeviceNV );
  return theWGLFunctions::instance()->wglLockVideoCaptureDeviceNV(hDc, hDevice);
}

inline BOOL WGLQueryVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice, int iAttribute, int *piValue)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglQueryVideoCaptureDeviceNV );
  return theWGLFunctions::instance()->wglQueryVideoCaptureDeviceNV(hDc, hDevice, iAttribute, piValue);
}

inline BOOL WGLReleaseVideoCaptureDeviceNV(HDC hDc, HVIDEOINPUTDEVICENV hDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theWGLFunctions::instance()->wglReleaseVideoCaptureDeviceNV );
  return theWGLFunctions::instance()->wglReleaseVideoCaptureDeviceNV(hDc, hDevice);
}

#endif // _WIN32

#if defined(LINUX)

/*! \brief Initialize the glx-extension functions inside the GLXFunctions class.
 *  \remarks This doesn't need to be called more than once from a hardware 
 *  OpenGL context to make the extended glx-interface functions available
 *  globally to the process (but it can handle being called more than once).
 *  The GLX-wrapper functions below don't depend on any \c GLContext.
 *  This function returns false if dpy is NULL.
 */
inline bool GLXInitializeExtensions( Display * dpy )
{
  if( dpy )
  {
    return theGLXFunctions::instance()->Init( dpy );
  }
  else
  {
    return false;
  }
}

/*! \brief Returns whether the specified glx-extension is available.
 *  \param extstr Designates the requested glx-extension.
 *  \return The function returns \c true if the requested glx-extension is available
 *  and if the corresponding function pointer are initialized properly. Otherwise
 *  the function returns \c false. The function always returns \c false if called prior
 *  to GLXInitializeExtensions.
 *  \remarks GLXIsExtensionAvailable should not be called prior to GLXInitializeExtension.
 *  Doing so would be entirely meaningless and the function would always return false.
 *  It is recommended to test whether a requested glx-extension is available on the
 *  current configuration prior to exploit the specified glx-extension. Using a not available 
 *  glx-extension through the 'GLX' wrapper function provided by SceniX results in 
 *  unexpected behavior.
 *  \sa GLXInitializeExtensions */
inline bool GLXIsExtensionAvailable(const std::string& extstr)
{
  return theGLXFunctions::instance()->isAvailable(extstr);
}

/*! \brief Returns whether the specified glx-extension is exported.
 *  \param extstr Designates the requested glx-extension.
 *  \return The function returns \c true if the requested glx-extension is exported
 *  by the current configuration. Otherwise the function returns \c false. 
 *  The function always returns \c false if called prior to GLXInitializeExtensions.
 *  \remarks GLXIsExtensionExported should not be called prior to GLXInitializeExtension.
 *  Doing so would be entirely meaningless and the function would always return false.
 *  If a glx-extension defines new functions, GLXIsExtensionAvailable should be called
 *  instead of GLXIsExtensionExported to ensure that all required function pointers
 *  are properly initialized.
 *  \sa GLXInitializeExtensions, GLXIsExtensionAvailable */
inline bool GLXIsExtensionExported(const std::string& extstr)
{
  return theGLXFunctions::instance()->isExported(extstr);
}

//! Replacement of glXCreateContext
inline GLXContext GLXCreateContext(Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct)
{
  GLXContext ctx = glXCreateContext(dpy, vis, shareList, direct);
  GLXAttachContext(dpy, ctx);
  if (shareList)
  {
    GLXNotifyShared( shareList, ctx );
  }
  return ctx;
}

//! Replacement of glXDestroyContext
inline void GLXDestroyContext(Display *dpy, GLXContext ctx)
{
  NVSG_GL_STATS_FCT_COUNT();
  GLXDetachContext(ctx);
  return glXDestroyContext(dpy, ctx);
}

//! Replacement of glXGetCurrentContext
inline GLXContext GLXGetCurrentContext()
{
  ASSERT_GL_CURRENT_CONTEXT_CONFORMITY();
  return glXGetCurrentContext();
}

//! Replacement of glXGetCurrentDrawable
inline GLXDrawable GLXGetCurrentDrawable()
{
  NVSG_GL_STATS_FCT_COUNT();
  return glXGetCurrentDrawable();
}

// GLX_SGI_swap_control
inline NVSG_API int GLXSwapInterval (int value )
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXSwapIntervalSGI );
  return theGLXFunctions::instance()->glXSwapIntervalSGI( value );
}

// GLX_SGI_video_sync

inline int GLXGetVideoSync (unsigned int * value)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXGetVideoSyncSGI );
  return theGLXFunctions::instance()->glXGetVideoSyncSGI( value );
}

inline int GLXWaitVideoSync (int divisor, int remainder, unsigned int * cnt)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXWaitVideoSyncSGI );
  return theGLXFunctions::instance()->glXWaitVideoSyncSGI( divisor, remainder, 
                                                                        cnt );
}

// GLX_NV_swap_group
inline  Bool GLXJoinSwapGroup(Display *dpy, GLXDrawable drawable,
                             GLuint group)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXJoinSwapGroupNV );
  return theGLXFunctions::instance()->glXJoinSwapGroupNV(dpy,drawable,group);
}

inline  Bool GLXBindSwapBarrier(Display *dpy, GLuint group, 
                                                            GLuint barrier)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXBindSwapBarrierNV );
  return theGLXFunctions::instance()->glXBindSwapBarrierNV(dpy,group,barrier);
}

inline  Bool GLXQuerySwapGroup(Display *dpy, GLXDrawable drawable,
                              GLuint *group, GLuint *barrier)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXQuerySwapGroupNV );
  return theGLXFunctions::instance()->glXQuerySwapGroupNV(dpy,drawable,group,
                                                                      barrier);
}

inline  Bool GLXQueryMaxSwapGroups(Display *dpy, int screen,
                                  GLuint *maxGroups, GLuint *maxBarriers)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXQueryMaxSwapGroupsNV );
  return theGLXFunctions::instance()->glXQueryMaxSwapGroupsNV(dpy, screen,
                                                  maxGroups, maxBarriers );
}

inline  Bool GLXQueryFrameCount(Display *dpy, int screen, GLuint *count)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXQueryFrameCountNV );
  return theGLXFunctions::instance()->glXQueryFrameCountNV(dpy, screen, count );
}

inline  Bool GLXResetFrameCount(Display *dpy, int screen)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXResetFrameCountNV );
  return theGLXFunctions::instance()->glXResetFrameCountNV(dpy, screen );
}


// GLX_NV_video_out
inline  int GLXGetVideoDevice(Display *dpy, int screen, 
                              int numVideoDevices,
                             GLXVideoDeviceNV *pVideoDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXGetVideoDeviceNV );
  return theGLXFunctions::instance()->glXGetVideoDeviceNV( dpy, screen,
                                              numVideoDevices, pVideoDevice );
}

inline  int GLXReleaseVideoDevice(Display *dpy, int screen,
                                 GLXVideoDeviceNV VideoDevice)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXReleaseVideoDeviceNV );
  return theGLXFunctions::instance()->glXReleaseVideoDeviceNV(dpy,screen,
                                                                VideoDevice);
}

inline  int GLXBindVideoImage(Display *dpy, 
                             GLXVideoDeviceNV VideoDevice,
                             GLXPbuffer pbuf, int iVideoBuffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXBindVideoImageNV );
  return theGLXFunctions::instance()->glXBindVideoImageNV(dpy, VideoDevice,pbuf,
                                                                iVideoBuffer);
}

inline  int GLXReleaseVideoImage(Display *dpy, GLXPbuffer pbuf)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXReleaseVideoImageNV );
  return theGLXFunctions::instance()->glXReleaseVideoImageNV(dpy,pbuf);
}

inline  int GLXSendPbufferToVideo(Display *dpy, GLXPbuffer pbuf,
                                 int iBufferType,
                                 unsigned long *pulCounterPbuffer,
                                 GLboolean bBlock)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXSendPbufferToVideoNV );
  return theGLXFunctions::instance()->glXSendPbufferToVideoNV(dpy,pbuf,
                                    iBufferType, pulCounterPbuffer, bBlock );
}

inline  int GLXGetVideoInfo(Display *dpy, int screen,
                           GLXVideoDeviceNV VideoDevice,
                           unsigned long *pulCounterOutputVideo,
                           unsigned long *pulCounterOutputPbuffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXGetVideoInfoNV );
  return theGLXFunctions::instance()->glXGetVideoInfoNV(dpy,screen,VideoDevice,
                              pulCounterOutputVideo, pulCounterOutputPbuffer );
}

// GLX_EXT_texture_from_pixmap
inline  void GLXBindTexImage (Display *dpy, GLXDrawable d, int buffer, 
                                          const int * attrList)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXBindTexImageEXT );
  return theGLXFunctions::instance()->glXBindTexImageEXT( dpy, d, buffer, 
                                                                    attrList );
}

inline  void GLXReleaseTexImage (Display *dpy, GLXDrawable d, int buffer)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXReleaseTexImageEXT );
  return theGLXFunctions::instance()->glXReleaseTexImageEXT( dpy, d, buffer );
}

// GLX_NV_present_video
inline unsigned int *GLXEnumerateVideoDevicesNV(Display *dpy, int screen, int *nelements)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXEnumerateVideoDevicesNV );
  return theGLXFunctions::instance()->glXEnumerateVideoDevicesNV(dpy, screen, nelements);
}

inline int GLXBindVideoDeviceNV(Display *dpy, unsigned int video_slot, unsigned int video_device, const int *attrib_list)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXBindVideoDeviceNV );
  return theGLXFunctions::instance()->glXBindVideoDeviceNV(dpy, video_slot, video_device, attrib_list);
}

// GLX_NV_video_capture
inline int GLXBindVideoCaptureDeviceNV (Display *dpy, unsigned int video_capture_slot, GLXVideoCaptureDeviceNV device)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXBindVideoCaptureDeviceNV );
  return theGLXFunctions::instance()->glXBindVideoCaptureDeviceNV(dpy, video_capture_slot, device);
}

inline GLXVideoCaptureDeviceNV * GLXEnumerateVideoCaptureDevicesNV (Display *dpy, int screen, int *nelements)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXEnumerateVideoCaptureDevicesNV );
  return theGLXFunctions::instance()->glXEnumerateVideoCaptureDevicesNV(dpy, screen, nelements);
}

inline void GLXLockVideoCaptureDeviceNV (Display *dpy, GLXVideoCaptureDeviceNV device)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXLockVideoCaptureDeviceNV );
  theGLXFunctions::instance()->glXLockVideoCaptureDeviceNV(dpy, device);
}

inline int GLXQueryVideoCaptureDeviceNV (Display *dpy, GLXVideoCaptureDeviceNV device, int attribute, int *value)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXQueryVideoCaptureDeviceNV );
  return theGLXFunctions::instance()->glXQueryVideoCaptureDeviceNV(dpy, device, attribute, value);
}

inline void GLXReleaseVideoCaptureDeviceNV (Display *dpy, GLXVideoCaptureDeviceNV device)
{
  NVSG_GL_STATS_FCT_COUNT();
  NVSG_ASSERT( theGLXFunctions::instance()->glXReleaseVideoCaptureDeviceNV );
  theGLXFunctions::instance()->glXReleaseVideoCaptureDeviceNV(dpy, device);
}

#endif // LINUX

enum 
{
  GL_INVALID_BUFFER_TARGET = -1
  // specialized targets for vertex buffers 
, GL_VERTEX_BUFFER_INTERLEAVED_GENERIC = 0
, GL_FEEDBACK_BUFFER_INTERLEAVED_GENERIC = 1
, GL_VERTEX_BUFFER_FIRST = GL_VERTEX_BUFFER_INTERLEAVED_GENERIC
, GL_VERTEX_BUFFER_LAST = GL_FEEDBACK_BUFFER_INTERLEAVED_GENERIC
, GL_INDEX_BUFFER_UNSIGNED_BYTE
, GL_INDEX_BUFFER_FIRST = GL_INDEX_BUFFER_UNSIGNED_BYTE
, GL_INDEX_BUFFER_UNSIGNED_SHORT
, GL_INDEX_BUFFER_UNSIGNED_INT
, GL_INDEX_BUFFER_LAST = GL_INDEX_BUFFER_UNSIGNED_INT
, GL_NUM_BUFFER_TARGETS
};

//! Generates a vertex buffer of requested size
/** The function creates a buffer object for vertex data and 
  * returns the buffer identifier in the buffer pointed to by \a buffer. */
NVSG_API void GLGenVertexBuffer(GLenum target, GLsizeiptr size, GLuint * buffer);

//! Generates an index buffer of requested size
/** The function creates a buffer object for index data and 
  * returns the buffer identifier in the buffer pointed to by \a buffer. */
NVSG_API void GLGenIndexBuffer(GLenum target, GLsizeiptr size, GLuint * buffer);

//! Deletes a vertex buffer
/** The function deletes the buffer object identified by \a buffer. 
  * The behavior is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenVertexBuffer. */
NVSG_API void GLDeleteVertexBuffer(GLuint buffer);

//! Deletes an index buffer
/** The function deletes the buffer object identified by \a buffer. 
* The behavior is undefined if an invalid identifier is passed through \a buffer,
* that is - an identifier that was not returned by GLGenIndexBuffer. */
NVSG_API void GLDeleteIndexBuffer(GLuint buffer);

//! Binds the buffer name associated with the identifier \a buffer to the GL_ARRAY_ELEMENT target.
/** The behavior is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenVertexBuffer. */
NVSG_API void GLBindVertexBuffer(GLuint buffer);

//! Binds the buffer name associated with the identifier \a buffer to the GL_TRANSFORM_FEEDBACK_BUFFER_NV target.
/** The behavior is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenVertexBuffer. 
  * \note This method actually calls GLBindBufferOffset() */
NVSG_API void GLBindTransformFeedbackBuffer(GLuint index, GLuint buffer, GLuint offset);

//! Binds the buffer name associated with the identifier \a buffer to the GL_ELEMENT_ARRAY_ELEMENT target.
/** The behavior is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenIndexBuffer. */
NVSG_API void GLBindIndexBuffer(GLuint buffer);

//! Uploads all pending buffers 
/** The function uploads the data copied to not yet uploaded vertex or index buffers 
  * to Vertex Buffer Objects (VBO) for fast rendering. 
  *
  * \note This function must be called immediately before rendering if vertex or index 
  * buffers are used for caching strategies. Omitting this call certainly results in
  * undefined runtime behavior! */
NVSG_API void GLUploadBuffers();

//! Maps the vertex buffer identified by \a buffer into process memory for write-only access.
/** The returned pointer can be used to write vertex data to the vertex buffer.
  *
  * Attempts to read back data from the mapped buffer might result in undefined behavior.
  * Writing more bytes to the buffer than it was specified at buffer creation, results in undefined behavior.
  * The behavior also is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenVertexBuffer.
  *
  * After data has been written to the buffer, and before the data will be dereferenced by any GL commands,
  * the mapping must be relinquished by calling GLUnmapVertexData.
  *
  * \returns A pointer to the vertex buffer if the memory could be mapped into process memory.
  * Otherwise a null pointer is returned. */
NVSG_API GLvoid * GLMapVertexBuffer(GLuint buffer);

//! Maps the index buffer identified by \a buffer into process memory for write-only access.
/** The returned pointer can be used to write index data to the vertex buffer.
  *
  * Attempts to read back data from the mapped buffer might result in undefined behavior.
  * Writing more bytes to the buffer than it was specified at buffer creation, results in undefined behavior.
  * The behavior also is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenVertexBuffer.
  *
  * After data has been written to the buffer, and before the data will be dereferenced by any GL commands,
  * the mapping must be relinquished by calling GLUnmapIndexData.
  *
  * \returns A pointer to the index buffer if the memory could be mapped into process memory.
  * Otherwise a null pointer is returned. */
NVSG_API GLvoid * GLMapIndexBuffer(GLuint buffer);

//! Relinquishes a vertex buffer mapping
/** The function relinquishes the mapping for the buffer identified by \a buffer.
  * The behavior is undefined if a mapping for the specified buffer was not initiated
  * before by a call to GLMapVertexBuffer.
  * The behavior also is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenVertexBuffer. 
  * \returns The function returns GL_TRUE unless data values in the data store of the buffer
  * have become corrupted during the period that the buffer was mapped. */
NVSG_API GLboolean GLUnmapVertexBuffer(GLuint buffer);

//! Relinquishes an index buffer mapping
/** The function relinquishes the mapping for the buffer identified by \a buffer.
  * The behavior is undefined if a mapping for the specified buffer was not initiated
  * before by a call to GLMapIndexBuffer.
  * The behavior also is undefined if an invalid identifier is passed through \a buffer,
  * that is - an identifier that was not returned by GLGenIndexBuffer. 
  * \returns The function returns GL_TRUE unless data values in the data store of the buffer
  * have become corrupted during the period that the buffer was mapped. */
NVSG_API GLboolean GLUnmapIndexBuffer(GLuint buffer);

//! Returns the 64-bit video memory address for the specified buffer
/** Vertex and index buffers might be collected in a higher-level buffer object.
  * This function returns the 64-bit video memory address of the higher-level buffer object.
  * \note This funtion requires the GL_NV_vertex_buffer_unified_memory extension.
  * \returns The 64-bit video memory address for the specified buffer. */
NVSG_API GLuint64EXT GLGetBufferAddress(GLuint buffer);

//! Returns the size of the specified buffer
/** Vertex and index buffers might be collected in a higher-level buffer object.
  * This function returns the size of the higher-level buffer object.
  * \note This funtion requires the GL_NV_vertex_buffer_unified_memory extension.
  * \returns The size of the higher-level buffer object. */
NVSG_API GLsizeiptr GLGetBufferSize(GLuint buffer);

//! Returns the start offset for the specified buffer
/** Vertex buffers might be collected in a higher-level buffer object.
  * This function returns the start offset inside the higher-level buffer object.
  * \returns The start offset inside the higher-level vertex buffer object. */
NVSG_API GLsizeiptr GLGetVertexBufferOffset(GLuint buffer);


//! VBO / Cache Filling Methods
/** Build the caches from the attributes sent.
  * \returns A buffer cache. */
NVSG_API void GLBuildVertexBufferCache( nvgl::GLArrayBufferCache * vi, 
                                   GLuint whichBuffer,
                                   const nvsg::VertexAttributeSet * vas );
NVSG_API void GLFillVertexBufferCache( nvgl::GLArrayBufferCache * vi, GLuint whichBuffer
                                      , const nvsg::VertexAttributeSet * vas );

NVSG_API void GLBuildIndexBufferCache( nvgl::GLElementArrayBufferCache* vi, 
      unsigned int numStrips, const nvsg::IndexList * strips,
      bool primitiveRestartSupport );

NVSG_API void GLBuildIndexBufferCache( nvgl::GLElementArrayBufferCache* vi, 
      unsigned int numIndices, const unsigned int * indices );

NVSG_API void GLBuildIndexBufferCache( nvgl::GLElementArrayBufferCache* vi, 
      unsigned int numIndices, const void * indices );


//! Returns the start offset for the specified buffer
/** Index buffers might be collected in a higher-level buffer object.
  * This function returns the start offset inside the higher-level buffer object.
  * \returns The start offset inside the higher-level vertex buffer object. */
NVSG_API GLsizeiptr GLGetIndexBufferOffset(GLuint buffer);

//! Returns whether \a buffer identifies a valid vertex buffer for the current context.
/** \returns GL_TRUE if \a buffer identifies a valid vertex buffer for the current context.
  * GL_FALSE otherwise. */
NVSG_API GLboolean GLIsVertexBuffer(GLuint buffer);

//! Returns whether \a buffer identifies a valid index buffer for the current context.
/** \returns GL_TRUE if \a buffer identifies a valid index buffer for the current context.
  * GL_FALSE otherwise. */
NVSG_API GLboolean GLIsIndexBuffer(GLuint buffer);

#if !defined(DOXYGEN_IGNORE)
inline void _GLDisable( GLenum cap ) 
{
  NVSG_GL_STATS_STATE( cap, false );
  glDisable( cap );
  ASSERT_GL_NO_ERROR;
}

inline void _GLDisableClientState( GLenum array ) 
{
  NVSG_GL_STATS_CLIENT_STATE( array, false );
  glDisableClientState( array );
  ASSERT_GL_NO_ERROR;
}

inline void _GLEnable( GLenum cap ) 
{
  NVSG_GL_STATS_STATE( cap, true );
  glEnable( cap );
  ASSERT_GL_NO_ERROR;
}

inline void _GLEnableClientState( GLenum array ) 
{
  NVSG_GL_STATS_CLIENT_STATE( array, true );
  glEnableClientState( array );
  ASSERT_GL_NO_ERROR;
}

inline  void  _GLFogf( GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_FCT_COUNT();
  glFogf( pname, param );
  ASSERT_GL_NO_ERROR;
}

inline  void  _GLFogfv( GLenum pname, const GLfloat * params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glFogfv( pname, params );
  ASSERT_GL_NO_ERROR;
}

inline  void  _GLFogi( GLenum pname, GLint param )
{
  NVSG_GL_STATS_FCT_COUNT();
  glFogi(pname, param);
  ASSERT_GL_NO_ERROR;
}

inline  void  _GLFogiv( GLenum pname, const GLint * params )
{
  NVSG_GL_STATS_FCT_COUNT();
  glFogiv(pname, params);
  ASSERT_GL_NO_ERROR;
}

inline GLboolean _GLIsEnabled( GLenum cap ) 
{
  NVSG_GL_STATS_FCT_COUNT();
  GLboolean isEnabled = glIsEnabled( cap );
  ASSERT_GL_NO_ERROR;
  return( isEnabled );
}

inline  void  _GLLightf( GLenum light, GLenum pname, GLfloat param ) 
{ 
  NVSG_GL_STATS_LIGHTF( light, pname, param );
  glLightf( light, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLLightfv( GLenum light, GLenum pname, const GLfloat *params ) 
{
  NVSG_GL_STATS_LIGHTFV( light, pname, params );
  glLightfv( light, pname, params );
  ASSERT_GL_NO_ERROR;
}

inline void _GLLighti( GLenum light, GLenum pname, GLint param ) 
{ 
  NVSG_GL_STATS_LIGHTI( light, pname, param );
  glLighti( light, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLLightiv( GLenum light, GLenum pname, GLint *params ) 
{ 
  NVSG_GL_STATS_LIGHTIV( light, pname, params );
  glLightiv( light, pname, params );
  ASSERT_GL_NO_ERROR;
}

inline void _GLLightModelf( GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_LIGHT_MODELF( pname, param );
  glLightModelf( pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLLightModelfv( GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_LIGHT_MODELFV( pname, params );
  glLightModelfv( pname, params );
  ASSERT_GL_NO_ERROR;
}

inline void _GLLightModeli( GLenum pname, GLint param )
{
  NVSG_GL_STATS_LIGHT_MODELI( pname, param );
  glLightModeli( pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLLightModeliv( GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_LIGHT_MODELIV( pname, params );
  glLightModeliv( pname, params );
  ASSERT_GL_NO_ERROR;
}

inline  void  _GLMaterialf( GLenum face, GLenum pname, GLfloat param ) 
{
  NVSG_GL_STATS_MATERIALF( face, pname, param );
  glMaterialf( face, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLMaterialfv( GLenum face, GLenum pname, const GLfloat *params ) 
{
  NVSG_GL_STATS_MATERIALFV( face, pname, params );
  glMaterialfv( face, pname, params );
  ASSERT_GL_NO_ERROR;
}

inline void _GLMateriali( GLenum face, GLenum pname, GLint param ) 
{
  NVSG_GL_STATS_MATERIALI( face, pname, param );
  glMateriali( face, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLMaterialiv( GLenum face, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_MATERIALIV( face, pname, params );
  glMaterialiv( face, pname, params );
  ASSERT_GL_NO_ERROR;
}

inline void _GLPolygonMode( GLenum face, GLenum mode )
{
  NVSG_GL_STATS_POLYGON_MODE( face, mode );
  glPolygonMode( face, mode );
  ASSERT_GL_NO_ERROR;
}

inline  void  _GLTexEnvf( GLenum target, GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_TEX_ENVF( target, pname, param );
  glTexEnvf( target, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLTexEnvfv( GLenum target, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_TEX_ENVFV( target, pname, params );
  glTexEnvfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

inline void _GLTexEnvi( GLenum target, GLenum pname, GLint param )
{
  NVSG_GL_STATS_TEX_ENVI( target, pname, param );
  glTexEnvi( target, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLTexEnviv( GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_TEX_ENVIV( target, pname, params );
  glTexEnviv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

inline  void  _GLTexParameterf( GLenum target, GLenum pname, GLfloat param )
{
  NVSG_GL_STATS_TEX_PARAMETERF( target, pname, param );
  glTexParameterf( target, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLTexParameterfv( GLenum target, GLenum pname, const GLfloat *params )
{
  NVSG_GL_STATS_TEX_PARAMETERFV( target, pname, params );
  glTexParameterfv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

inline void _GLTexParameteri( GLenum target, GLenum pname, GLint param )
{
  NVSG_GL_STATS_TEX_PARAMETERI( target, pname, param );
  glTexParameteri( target, pname, param );
  ASSERT_GL_NO_ERROR;
}

inline void _GLTexParameteriv( GLenum target, GLenum pname, const GLint *params )
{
  NVSG_GL_STATS_TEX_PARAMETERIV( target, pname, params );
  glTexParameteriv( target, pname, params );
  ASSERT_GL_NO_ERROR;
}

#if defined( GL_CACHING )

inline  void  _GLUpdateActiveTexture( void )
{
  GLContext * ctx = glContext();
  if ( ctx->m_textureAttributes.m_toActivate != ctx->m_textureAttributes.m_active )
  {
#if !defined(NDEBUG)
    GLint at;
    glGetIntegerv( GL_ACTIVE_TEXTURE, &at );
    NVSG_ASSERT( at == GL_TEXTURE0 + ctx->m_textureAttributes.m_active );
#endif
    ctx->m_textureAttributes.m_active = ctx->m_textureAttributes.m_toActivate;
    NVSG_GL_STATS_ACTIVE_TEXTURE( GL_TEXTURE0 + ctx->m_textureAttributes.m_active );
    ctx->m_glFunctions->glActiveTexture( GL_TEXTURE0 + ctx->m_textureAttributes.m_active );
  }
}

inline  void  _GLUpdateClientActiveTexture( void )
{
  GLContext * ctx = glContext();
  if ( ctx->m_vertexArrayAttributes.m_clientTextureToActivate != ctx->m_vertexArrayAttributes.m_clientTextureActive )
  {
#if !defined(NDEBUG)
    GLint cat;
    glGetIntegerv( GL_CLIENT_ACTIVE_TEXTURE, &cat );
    NVSG_ASSERT( cat == GL_TEXTURE0 + ctx->m_vertexArrayAttributes.m_clientTextureActive );
#endif
    ctx->m_vertexArrayAttributes.m_clientTextureActive = ctx->m_vertexArrayAttributes.m_clientTextureToActivate;
    NVSG_GL_STATS_CLIENT_ACTIVE_TEXTURE( GL_TEXTURE0 + ctx->m_vertexArrayAttributes.m_clientTextureActive );
    ctx->m_glFunctions->glClientActiveTexture( GL_TEXTURE0 + ctx->m_vertexArrayAttributes.m_clientTextureActive );
  }
}

#endif

#endif // DOXYGEN_IGNORE

} // namespace nvgl
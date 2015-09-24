// Copyright NVIDIA Corporation 2002-2007
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

#include <nvgl/nvgl.h>

#include <nvgl/GLObjectRenderer.h>

namespace nvsg
{
  class VertexAttributeSet;
}

namespace nvgl
{

  struct GLArrayBufferCache;

  template <GLuint attrIndex>
  struct GLEvalVertexAttrib
  {
    enum { 
      cshift = attrIndex,
      gshift = attrIndex+16,
      cmask = 1 << cshift,
      gmask = 1 << gshift,
      mask = cmask | gmask
    };

    static void disable(); 
    static void enableGeneric(GLint size, GLenum type, GLboolean normalize, GLsizei stride, const GLvoid * ptr);
    static void enableConventional(GLint size, GLenum type, GLsizei stride, const GLvoid * ptr);
    
    static NVSG_API void eval(const nvsg::VertexAttributeSet * vset);
    static NVSG_API void eval(const nvsg::VertexAttributeSet * vset, const GLArrayBufferCache * cache);
  };

  template <GLuint attrIndex>
  struct GLEvalVertexAttribUnifiedMemory
  {
    enum { 
      cshift = attrIndex,
      gshift = attrIndex+16,
      cmask = 1 << cshift,
      gmask = 1 << gshift,
      mask = cmask | gmask
    };

    static void disable(); 
    static void enableGeneric(GLint size, GLenum type, GLboolean normalize, GLsizei stride, GLuint64EXT address, GLsizeiptr length);
    static void enableConventional(GLint size, GLenum type, GLsizei stride, GLuint64EXT address, GLsizeiptr length);

    static NVSG_API void eval(const nvsg::VertexAttributeSet * vset, const GLArrayBufferCache * cache);
  };


  // version of GLEvalVertexAttrib for new RenderList processing framework using RenderObjects
  template <GLuint attrIndex>
  struct GLEvalVertexAttrib2
  {
    enum { 
      cshift = attrIndex,
      gshift = attrIndex+16,
      cmask = 1 << cshift,
      gmask = 1 << gshift,
      mask = cmask | gmask
    };

    static void disable(); 
    static void enableGeneric(GLint size, GLenum type, GLboolean normalize, GLsizei stride, const GLvoid * ptr);
    static void enableConventional(GLint size, GLenum type, GLsizei stride, const GLvoid * ptr);

    static NVSG_API void eval(const nvsg::VertexAttributeSet * vset);
    static NVSG_API void eval(const nvsg::VertexAttributeSet * vset, const DIResourceSharedPtr& resource );
    static NVSG_API void eval( const DIResourceSharedPtr& resource );
  };

  // version of GLEvalVertexAttrib for new RenderList processing framework using RenderObjects
  template <GLuint attrIndex>
  struct GLEvalVertexAttrib3
  {
    enum { 
      cshift = attrIndex,
      gshift = attrIndex+16,
      cmask = 1 << cshift,
      gmask = 1 << gshift,
      mask = cmask | gmask
    };

    static NVSG_API void enableGeneric();
    static NVSG_API void enableConventional();
    static NVSG_API void disableGeneric(); 
    static NVSG_API void disableConventional(); 
    static NVSG_API void updateGeneric(GLint size, GLenum type, GLboolean normalize, GLsizei stride, const GLvoid * ptr);
    static NVSG_API void updateGeneric( const DIResourceWeakPtr &diResource );

    // TODO GLsizeiptr?
    static NVSG_API void updateConventional(GLint size, GLenum type, GLboolean normalize, GLsizei stride, const GLvoid * ptr);
    static NVSG_API void updateConventional(GLint size, GLenum type, GLsizei stride, const GLvoid * ptr);
    static NVSG_API void updateConventional( const DIResourceWeakPtr &diResource );

    static NVSG_API void updateUnifiedFormatConventional(GLint size, GLenum type, GLsizei stride);
    static NVSG_API void updateFormatUnifiedConventional( const VertexDeclarationSharedPtr& renderObject ); // update only format
    static NVSG_API void updateFormatUnifiedGeneric( const VertexDeclarationSharedPtr& vertexDeclaration ); // update only format

    static NVSG_API void updateUnifiedGeneric( const VertexDeclarationSharedPtr& vertexDeclaration );
    static NVSG_API void updateUnifiedConventional( const VertexDeclarationSharedPtr& vertexDeclaration );
  };

}

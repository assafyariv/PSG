// Copyright NVIDIA Corporation 2011
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

#include <nvsg/CoreTypes.h>
#include <nvgl/RenderContextGL.h>
#include <nvutil/HandledObject.h>

namespace nvgl
{
  class DisplayListGL : public nvutil::HandledObject
  {
  public:
    /** \brief Create a new shared displaylist
        \return DisplayListGLSharedPtr to a new displaylist
    **/
    NVSG_API static DisplayListGLSharedPtr create();

    NVSG_API void beginCompile( );
    NVSG_API void beginCompileAndExecute( );
    NVSG_API void endCompile( );

    NVSG_API void execute() const;

    NVSG_API GLuint getGLId() const;

  protected:
    friend struct nvutil::Holder<DisplayListGL>;

    /** \brief Construct a new DisplayList.
        \remarks Use \sa compile and \sa compileAndExecute to feed the display list with data **/
    NVSG_API DisplayListGL();
    /** \brief Copy construction not allowed, but required for handles. **/
    NVSG_API DisplayListGL( const DisplayListGL & );
    NVSG_API virtual ~DisplayListGL();

  protected:
    GLuint                m_glId;
    ShareGroupGLSharedPtr m_shareGroupGL; // ShareGroup to use for deletion
  };

  inline DisplayListGL::DisplayListGL()
  {
    m_glId = GLGenLists( 1 );
    m_shareGroupGL = RenderContextGL::getCurrentRenderContextGL()->getShareGroupGL();
  }

  inline DisplayListGL::DisplayListGL( const DisplayListGL &)
  {
    NVSG_ASSERT( 0 && "copy constructor may not be called" );
  }

  inline DisplayListGLSharedPtr DisplayListGL::create()
  {
    return DisplayListGLHandle::create();
  }

  inline void DisplayListGL::beginCompile( )
  {
    GLNewList( m_glId, GL_COMPILE );
  }

  inline void DisplayListGL::beginCompileAndExecute( )
  {
    GLNewList( m_glId, GL_COMPILE_AND_EXECUTE );
  }

  inline void DisplayListGL::endCompile( )
  {
    GLEndList( );
  }

  inline void DisplayListGL::execute() const
  {
    GLCallList( m_glId );
  }

  inline GLuint DisplayListGL::getGLId() const
  {
    return m_glId;
  }

} // namespace nvgl

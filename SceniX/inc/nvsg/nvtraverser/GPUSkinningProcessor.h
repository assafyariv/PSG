// Copyright NVIDIA Corporation 2002-2005
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
/** \file */

#include "nvsgcommon.h"
#include "nvsg/CoreTypes.h"
#include "nvtraverser/AppTraverser.h"
#include "nvmath/Matnnt.h"
#include <map>
#include <vector>
#include <string>
#include "Cg/cg.h"
#include "Cg/cgGL.h"

namespace nvsg
{
  class Group;
  class DALHost;
}

namespace nvgl
{
  struct GLArrayBufferCache;
}

namespace nvtraverser {

//! GPUSkinningProcessor Class
/** A processor that performs skinning on the CPU.
  * \note Needs a valid ViewState. Call setViewState prior to apply().*/
class GPUSkinningProcessor : public AppTraverser::SkinningProcessor
{
public:
  //! Default constructor.
  NVSG_API GPUSkinningProcessor();

  //! apply to scene
  NVSG_API virtual void process( bool process );
  NVSG_API virtual AppTraverser::SkinningProcessorType getType();

  //! is this processor supported on the current architecture?
  static NVSG_API bool isSupported( void );

protected:
  //!Default destructor.
  NVSG_API virtual ~GPUSkinningProcessor();

private:
  static void cgErrorCallback( void );

  void pushErrorCallback();
  void popErrorCallback();
  CGerrorCallbackFunc m_oldErrCallback;

  struct GPUInfo
  {
    CGprogram vertexProgram;
    CGprogram geometryProgram;

    GLuint dataBufferTexture;
    GLuint skeletonTexture;

    int attribs[ 16 * 3 ];
    int numAttribs;
  };

  void initCg();
  void initTransformFeedback();
  GLuint createSkeletonTexture( int numMatrices );
  unsigned char * getWritableBuffer( unsigned int size, bool pbo );
  void releaseBuffer( bool pbo );
  unsigned char * bufferOffset( unsigned char *, bool pbo );
  void updateSkinTexture( GLuint texture
                        , std::vector<std::pair<nvmath::Mat44f,nvmath::Mat44f> > & jointMatrices );
  void processSkin( const nvsg::SkinSharedPtr & skin, GLuint texture );
  GPUInfo * createGPUInfo();
  void processSkinnedPrimitives();
  void processPrimitives( const nvsg::VertexAttributeSet *vas, 
                          nvgl::BufferGLSharedPtr bindPoseBuffer );
  static bool isArrayBufferCacheValid( const nvgl::GLArrayBufferCache * info );
  nvgl::GLArrayBufferCache * getArrayBufferCache( const nvsg::DALHost * host );
  nvgl::BufferGLSharedPtr reallocateBufferGL( const nvsg::VertexAttributeSetSharedPtr& vassp );
  void drawVAS( const nvsg::VertexAttributeSet * vas, const nvsg::SkinSharedPtr & skin, 
                nvgl::BufferGLSharedPtr bindPoseBuffer );
  void beginFrame( int numValues );
  void endFrame();
  void renderFeedback( const nvsg::VertexAttributeSet * vas, 
                       const nvsg::SkinSharedPtr & skin, 
                       nvgl::BufferGLSharedPtr bindPoseBuffer, nvgl::BufferGLSharedPtr feedbackBuffer, GPUInfo * gpuInfo );
  void processSkinnedEntity( const nvsg::VertexAttributeSetSharedPtr & vassp, const nvsg::SkinSharedPtr & skin, GPUInfo * info );
  GLint createDataBufferTexture( const nvsg::SkinSharedPtr & sh, unsigned int & maxCount );
  bool buildShaders( const nvsg::VertexAttributeSet * vas, const nvsg::Skin * skin, GPUInfo * info, unsigned int maxCount );

  enum
  {
    SUPPORT_BINDLESS_GRAPHICS = BIT0
  };

  unsigned int m_supportFlags;

  std::map<nvsg::DrawableSharedPtr,GPUInfo*>  m_drawableToInfo;

  CGcontext m_cgContext; 
  static CGcontext s_cgContext; 
  CGprofile m_cgVprofile, m_cgGprofile, m_cgFprofile;

  GLuint        m_writableBuffer[2];
  unsigned int  m_writableBufferPBOSize[2];
  unsigned int  m_whichPBO;
  unsigned char * m_writableBufferB;
  unsigned int  m_writableBufferSize;
  GLuint        m_maxTextureDimension;
  bool          m_initialized;
};

} // namespace nvtraverser

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

#include <list>
#include "nvsgcommon.h" // common used stuff 
#include "nvtraverser/GLTraverser.h" // base class definition

namespace nvtraverser
{  
  //! UploadTraverser using OpenGL
  /** \remarks This traverser will upload (and optionally display) data (such as texture maps)
   *  so that they do not have to be uploaded at runtime.
    * \note This traverser requires a valid OpenGL context to be current
    *  when constructing the object.  Behavior is undefined if there is no current context.
    */
  class GLUploadTraverser : public GLTraverser
  {
    public:
      //! Default constructor
      /** \param display Displays the textures/geometry as it is loaded.
        *  
        * \remarks There must be a current OpenGL context available during construction.
        */
      NVSG_API GLUploadTraverser( bool display );

    protected:
      //! Destructor
      NVSG_API virtual ~GLUploadTraverser(void);

      // the three methods we override
      NVSG_API virtual void handleLOD( const nvsg::LOD * lod );
      NVSG_API virtual void handleSwitch( const nvsg::Switch * swtch );

      //! doRender override to perform GL-rendering
      NVSG_API virtual void doRender( const nvsg::NodeSharedPtr & root );

    private:

      bool  m_display;
  };
}

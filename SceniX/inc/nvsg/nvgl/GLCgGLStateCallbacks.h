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

#include "nvsg/nvsgapi.h"
#include "Cg/cg.h"
#include "nvsg/CgFx.h"

namespace nvgl
{
#if !defined(DOXYGEN_IGNORE)

//! Overrides GL state callbacks
/** Adds state callbacks to the Cg runtime to allow SceniX to track GL state set
 *  by the CgFX runtime. */
NVSG_API void GLCgGLRegisterStateCallbacks(CGcontext context);

//! Binds an effect to the current context.
/** Binds the given effect as current effect in the context.
* \note This method is simply a way to associate an effect with the current
*       context, it does not actually call into the GL or Cg runtime. */
NVSG_API void GLCgGLBindEffect( nvsg::CgFxEffect * effect );

//! Retrieves the currently bound effect.
/** Gets the effect bound to the current context.
* \returns The CgFxEffect  */
NVSG_API nvsg::CgFxEffect * GLCgGLGetEffect();

//! Binds a pass to the current context.
/** Binds the given pass as current pass in the context.
* \note This method is simply a way to associate a pass with the current
*       context, it does not actually call into the GL or Cg runtime. */
NVSG_API void GLCgGLBindPass( nvsg::CgFxPass pass );

//! Retrieves the currently bound pass.
/** Gets the pass bound to the current context.
* \returns The CgFxPass  */
NVSG_API nvsg::CgFxPass GLCgGLGetPass();

//! Binds a program to the current context.
/** Binds the given program as current program in the context.
* \note This method is simply a way to associate a program with the current
*       context, it does not actually call into the GL or Cg runtime. */
NVSG_API void GLCgGLBindProgram( nvsg::CgFxProgram program );

//! Retrieves the currently bound program.
/** Gets the program bound to the current context.
* \returns The CgFxProgram  */
NVSG_API nvsg::CgFxProgram GLCgGLGetProgram();

//! Apply all samplers in the currently bound program.
/** Binds all referenced samplers in the current program to their respective
 *  texture units.
  * \note This method calls into the GL runtime in order to bind textures
  *       to their proper texture units. */
NVSG_API void GLCgGLApplyProgramSamplers();

#endif
}

// Copyright NVIDIA Corporation 2005-2011
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


#include <set>
#include "nvsg/StateAttribute.h"
#include "nvsg/Texture.h"
#include "nvsg/TextureHost.h"
#include "nvsg/CgmFx.h"
#include "nvutil/HashGenerator.h"


namespace nvsg
{

// forward declaration of internal types
class CgFxEffectResource;
struct CGIAnnotation;
struct CGIParameter;
struct CGITechnique;
struct CGIPass;
struct CGIProgram;
struct CGIBuffer;
struct CGIErrorInfo;

/*! \brief Typedef for an annotation handle. */
typedef CGIAnnotation * CgFxAnnotation;
/*! \brief Typedef for a parameter handle. */
typedef CGIParameter  * CgFxParameter;
/*! \brief Typedef for a technique handle. */
typedef CGITechnique  * CgFxTechnique;
/*! \brief Typedef for a pass handle. */
typedef CGIPass       * CgFxPass;
/*! \brief Typedef for a program handle. */
typedef CGIProgram    * CgFxProgram;
/*! \brief Typedef for a buffer handle. */
typedef CGIBuffer     * CgFxBuffer;

typedef std::pair<CgFxParameter, CGMparameter> LeafParam;
typedef std::set<LeafParam>::iterator LeafIter;

inline bool operator<(const LeafParam& lhs, const LeafParam& rhs) 
{
  return lhs.second < rhs.second;
}

/*! \brief CgFxContext wraps multiplexed CGcontexts and maintains its state machine configuration. 
 * \par Namespace: nvsg
 * \remarks
 * To apply a Cg effect to a certain geometry it requires the associated CGcontext to be properly
 * configured using a state machine (OpenGL, D3D) to perform state management as the effect's 
 * technique passes get applied. A CgFxContext wraps a CGMcontext and maintains its actual state 
 * machine configuration.
 * \n\n
 * SceniX core maintains a global CgFxContext singleton object to share between all effects
 * that are applied to geometry by means of a CgFx state attribute. The user can access 
 * this CgFxContext through nvsg::cgfxContext. Normally, the user only needs to access the
 * shared CgFxContext to either query its current state machine configuration, or to re-configure
 * the CgFxContext's state machine if required. For this two actions the CgFxContext provides the 
 * two APIs CgFxContext::getStateMachine, and CgFxContext::setStateMachine.
 * \n\n
 * There is no need for the SceniX user to take care of CgFxContext creation or deletion. This 
 * entirely is handled by the SceniX core framework. Effects that are going to be applied to a
 * certain geometry by means of a CgFx state attribute, automatically will share the global
 * CgFxContext singleton object which is available after nvsg::nvsgInitialize returns. 
 */
class CgFxContext
{
  friend class CgFxEffect;
  friend NVSG_API std::string cgfxGetLastError(CGerror&);

public:

  /*! \brief CgFxStateMachineEnum enumeration type
   */
  enum CgFxStateMachineEnum
  {
    CGFX_STATE_MACHINE_INVALID = 0, /*!< Indicates an invalid state machine. */
    CGFX_STATE_MACHINE_GL           /*!< Indicates an OpenGL state machine. */
  };

  /*! \brief Constructs a CgFxContext.
   * \remarks
   * This creates the singleton CgFxContext object which is in an unconfigured
   * state. The object must later be initialized by calling CgFxContext::initialize().
   * \n\n
   * The SceniX core uses a global CgFxContext singleton object, which can be accessed through 
   * nvsg::cgfxContext. This CgFxContext by default will be shared between all effects in a 
   * Scene. 
   */
  NVSG_API explicit CgFxContext();

  /*! \brief Destructs a CgFxContext.
   * \remarks
   * The wrapped CGMcontext will be destroyed. 
   */
  NVSG_API ~CgFxContext();

  /*! \brief Exclusively locks the CgFxContext
   * \remarks
   * If different renderer access the same CgFxContext from different threads, these renderer
   * need to ensure the CgFxContext's configuration remains unchanged while applying effects
   * that belong to that CgFxContext. Otherwise, the runtime behaviour would be undefined.
   * \n\n
   * Use unlock to release the CgFxContext after the effects have been applied.
   * \n\n
   * Note that this function is a NOP, if the SceniX runtime is not configured to run in a 
   * multithreaded environment. You can configure the SceniX runtime to run in a multithreaded
   * environment by passing NVSG_MULTITHREADED to the nvsg::nvsgInitialize entry point.
   */
  NVSG_API void lock() { m_lock.lockExclusive(); } 

  /*! \brief Releases an exclusive lock.
   * \remarks
   * This function is a NOP, if the SceniX runtime is not configured to run in a multithreaded 
   * environment. You can configure the SceniX runtime to run in a multithreaded environment 
   * by passing NVSG_MULTITHREADED to the nvsg::nvsgInitialize entry point.
   */
  NVSG_API void unlock() { m_lock.unlockExclusive(); }

  /*! \brief Implicit converts a CgFxContext to a CGMcontext where needed.
   * \remarks
   * This operator enables you to always pass a CgFxContext where a CGMcontext is expected.
   * The wrapped CGMcontext then will be passed implicitly.
   */
  NVSG_API operator CGMcontext() const { return m_context; }

  /*! \brief Returns the state machine for which the CgFxContext currently is configured.
   * \return
   * The function returns a CgFxStateMachineEnum indicating the state machine for which
   * this CgFxContext currently is configured. 
   * \remarks
   * A CgFxContext can be configured for only one state machine at a time. A renderer that is
   * going to apply an effect that belongs to this CgFxContext, needs to ensure that a
   * suitable state machine is configured. If an unsuitable state machine is configured, 
   * the renderer needs to re-configure the CgFxContext for the right state machine prior
   * to apply the effect. Use setStateMachine to re-configure the state machine.
   */
  NVSG_API CgFxStateMachineEnum getStateMachine() const { return m_stateMachine; }

  /*! \brief Re-configures the CgFxContext for the specified state machine.
   * \param stateMachine 
   * Specifies the state machine for which the CgFxContext should be re-configured.
   * \param num_gpus
   * Specifies the number of GPUs the CgFxContext should be re-configured for.
   * \return
   * The function returns \c true if the CgFxContext successfully was re-configured
   * for the specified state machine. Otherwise, the function returns \c false.
   * The CgFxContext remains configured for the previous state machine in case of failure.
   * \remarks
   * A CgFxContext can be configured for only one state machine at a time. A renderer that is
   * going to apply an effect that belongs to this CgFxContext, needs to ensure that a
   * suitable state machine is configured, and, if necessary, needs to re-configure the 
   * CgFxContext for the right state machine prior to apply the effect.
   * \n\n
   * Use getStateMachine to check what state machine currently is used by the CgFxContext.
   * \n\n
   * Note that re-configuring the CgFxContext for a different state machine requires a
   * re-validation of the effect's current technique. Use CgFxEffect::validateCurrentTechnique
   * to perform current technique validation.
   */
  NVSG_API bool initialize(CgFxStateMachineEnum stateMachine, int num_gpus); 

  /*! \brief Uninitializes the CgFxContext and releases all resources.
   */
  NVSG_API void uninitialize(); 
  
  enum BufferAccess
  {
    ACCESS_READ_ONLY  = CG_MAP_READ,
    ACCESS_READ_WRITE = CG_MAP_READ_WRITE,
    ACCESS_WRITE_ONLY = CG_MAP_WRITE
  };

  /*! \brief Creates a CgFxBuffer object
    * \par Namespace: nvsg
    * \param initialSize The intial size of the buffer.  Must be smaller than
    * the value returned by getBufferMaxSize() or an error is returned.
    * \param bam BufferAccess mode to use.
    * \remarks  
    * This class provides an interface to Cg's buffer objects.  Buffer objects
    * provide a method to send a large amount of data to a shader.  The shader
    * can then interpret this data in any way it chooses.  Buffer objects can
    * be accessed from a shader using the following method:
    *
    * float4 value : BUFFER[index][offset (in bytes)];
    *
    * Variables with BUFFER semantics are not considered "tweakable."
    *
    * On G80 hardware, the memory layout for buffers are organized as 4096 
    * vectors of 4-component floats.  This leads to 128 bit memory alignment for
    * all data types, as shown below:  
    *
    * A buffer declared as "float4" would expect to find each 4 component float
    * in memory like this:
    *
    * float4 myBuffer[1024] : BUFFER[0];
    *
    *    Bytes: 0-3   4-7   8-11  12-15
    *           (X)   (Y)   (Z)   (W)
    * Vector 0 [float float float float]
    * Vector 1 [float float float float]
    * [...]
    *
    * A buffer declared simply as "float" would expect to find
    * each floating point value in memory like this:
    *
    * float myBuffer[1024] : BUFFER[0];
    *
    *    Bytes: 0-3   4-7   8-11  12-15
    *           (X)   (Y)   (Z)   (W)
    * Vector 0 [float xxxxx xxxxx xxxxx]
    * Vector 1 [float xxxxx xxxxx xxxxx]
    * [...]           x = don't care
    *
    * It is essential that the application understand the data packing
    * requirements of the hardware in order to properly pack the data.
    *
    * The CgFxBuffer class is always considered in conjunction with a CgFxEffect
    * and applied to a particular CgFxProgram within that effect.
    */
  NVSG_API CgFxBuffer createBuffer( size_t initialSize, BufferAccess bam );

  /*! \brief Destroys a CgFxBuffer object
    * \param buf The buffer to destroy.
    * \par Namespace: nvsg
    * \remarks
    * The bufer object must be unbound before it is destroyed.
    */
  NVSG_API void destroyBuffer( CgFxBuffer buf );

  /*! \brief Gets the current size of a CgFxBuffer object
    * \param buf The buffer to get the size of.
    * \par Namespace: nvsg
    */
  NVSG_API size_t getBufferSize( CgFxBuffer buf );

  /*! \brief Sets data in a buffer object, beginning at offset 0 within the
   *         buffer.
    * \par Namespace: nvsg
    * \param buf The buffer to set data in.
    * \param data Pointer to the data.
    * \param size The length of the data.
    * \remarks 
    * This method will fail if the buffer object is currently mapped.
    */
  NVSG_API void setBufferData( CgFxBuffer buf, const void * data, size_t size );

  /*! \brief Sets data in a buffer object, beginning at the offset specified.
    * \par Namespace: nvsg
    * \param buf The buffer to set data in.
    * \param offset An offset into the buffer to begin writing.
    * \param data Pointer to the data.
    * \param size The length of the data.
    * \remarks 
    * This method will fail if the buffer object is currently mapped.
    */
  NVSG_API void setBufferSubData( CgFxBuffer buf, size_t offset, const void * data, size_t size );

  /*! \brief Gets the maximum allowable index for CgFxBuffer objects.
    * \par Namespace: nvsg
    * \remarks
    * This method will typically return '9' meaning 10 buffers are available
    * to each program stage in an effect.  This number will always be less
    * than the hardware's maximum, since SceniX reserves several buffers for 
    * future global parameter updates.
    */
  NVSG_API unsigned int getBufferMaxIndex();

  /*! \brief Gets the maximum allowable size for CgFxBuffer objects, in bytes.
    * \par Namespace: nvsg
    * \remarks
    * While GL buffer objects can be very large, CgFxBuffers are limited in
    * size.  This size is hardware dependent, and on G80 is currently 4096 
    * vectors of 4 floats, or 16384 bytes.
    */
  NVSG_API unsigned int getBufferMaxSize();

private:
  CgFxStateMachineEnum m_stateMachine; // indicates currently configured state machine
  int m_nGPUs; // the number of CGcontexts in CGMcontext (cached for speedy access)
  CGMcontext m_context; // shareable CGMcontext
  nvutil::SWMRSync m_lock; // for exclusive locking purposes
  CGIErrorInfo * m_errorInfo; // info to populate by our internal error callback
};

/*! \brief Returns the CgFxContext singleton object.
 * \return 
 * The function returns a pointer to the CgFxContext singleton object used in the SceniX core. 
 * \remarks
 * The SceniX core uses a CgFxContext singleton object that will be shared between all effects 
 * in a Scene. 
 */
 NVSG_API CgFxContext * cgfxContext();

/*! \brief Returns last error that occurred during a Cg-runtime operation.
 * \param error Retrieves the CGerror code.
 * \return The last error listing from the CGcontext that was current while the error occurred.
 * \remarks
 * The function retrieves the last most error that occurred during a Cg-runtime operation 
 * and returns the corresponding listing as string. The last error retrieved will be popped
 * off the internal error stack. Immediately calling this function again will retrieve the 
 * second last error and so on. If no errors are left anymore on the internal stack, 
 * CG_NO_ERROR will be retrieved.
 */
 NVSG_API std::string cgfxGetLastError(CGerror& error);

/*! \brief Abstracts a render effect to be applied to a specific geometry.
  * \par Namespace: nvsg
  * \remarks  
  * This class provides interfaces that lets you
  *
  * - Create a render effect from either a file or a source string,
  * - Query dependent data from the underlying effect resource,
  * - Exchange dependent data with the underlying effect resource, and
  * - Apply the render effect to a specific geometry.
  * 
  * The CgFxEffect class is always considered in conjunction with a CgFx state attribute.
  * A CgFxEffect object cannot be instantiated directly, but will be instantiated through
  * its associated CgFx attribute.  You need to call CgFx::getEffect to get the embedded 
  * CgFxEffect object of a certain CgFx attribute.
  * The CgFx::getEffect function returns a CgFxEffectWeakPtr. If you need read or write
  * access to the CgFxEffectWeakPtr, you have to use the CgFxEffectReadLock or
  * CgFxEffectWriteLock types to gain access to the objects members.  The scope of the
  * locks determines the duration of the read/write access.  See the documentation on the
  * "handles-based" interface for more information. 
  */
class CgFxEffect : public OwnedObject<CgFx>
{
  friend class CgFx;

    public:
      NVSG_API static CgFxEffectSharedPtr create();

public:

  /*! \brief Enumeration type to identify specific transforms.
   * \remarks
   * Uniform transform parameters always have a defined meaning. 
   * Knowing this meaning is essential, for example, when a renderer needs to update transform data 
   * for a certain transform parameter. 
   * \n\n
   * CgFxEffect::getTransformType lets you determine what kind of transform a certain transform 
   * parameter represents.
   */
  enum TransformType
  {
    /*! \brief Indicates an unknown transform. */
    TRAFO_UNKNOWN = -1,

    /*! \brief Indicates a transform from object space to world space. */
    TRAFO_WORLD = 0,
    /*! \brief Indicates a transform from object space to world space. */
    TRAFO_MODEL                             = TRAFO_WORLD,

    /*! \brief Indicates a transform from world space to eye space. */
    TRAFO_VIEW,

    /*! \brief Indicates a transform from eye space to clip space. */
    TRAFO_PROJECTION,

    /*! \brief Indicates a transposed transform from object space to world space. */
    TRAFO_WORLDT,
    /*! \brief Indicates a transposed transform from object space to world space. */
    TRAFO_MODELT                            = TRAFO_WORLDT,
    /*! \brief Indicates a transposed transform from object space to world space. */
    TRAFO_WORLDTRANSPOSE                    = TRAFO_WORLDT,
    /*! \brief Indicates a transposed transform from object space to world space. */
    TRAFO_MODELTRANSPOSE                    = TRAFO_WORLDT,

    /*! \brief Indicates a transposed transform from world space to eye space. */
    TRAFO_VIEWT,
    /*! \brief Indicates a transposed transform from world space to eye space. */
    TRAFO_VIEWTRANSPOSE                     = TRAFO_VIEWT,

    /*! \brief Indicates a transposed transform from eye space to clip space. */
    TRAFO_PROJECTIONT,
    /*! \brief Indicates a transposed transform from eye space to clip space. */
    TRAFO_PROJECTIONTRANSPOSE               = TRAFO_PROJECTIONT,

    /*! \brief Indicates a transform from world space to object space. */
    TRAFO_WORLDI,
    /*! \brief Indicates a transform from world space to object space. */
    TRAFO_MODELI                            = TRAFO_WORLDI,
    /*! \brief Indicates a transform from world space to object space. */
    TRAFO_WORLDINVERSE                      = TRAFO_WORLDI,
    /*! \brief Indicates a transform from world space to object space. */
    TRAFO_MODELINVERSE                      = TRAFO_WORLDI,

    /*! \brief Indicates a transform from eye space to world space. */
    TRAFO_VIEWI,
    /*! \brief Indicates a transform from eye space to world space. */
    TRAFO_VIEWINVERSE                        = TRAFO_VIEWI,

    /*! \brief Indicates a transform from clip space to eye space. */
    TRAFO_PROJECTIONI,
    /*! \brief Indicates a transform from clip space to eye space. */
    TRAFO_PROJECTIONINVERSE                  = TRAFO_PROJECTIONI,

    /*! \brief Indicates a transposed transform from world space to object space. */
    TRAFO_WORLDIT,
    /*! \brief Indicates a transposed transform from world space to object space. */
    TRAFO_MODELIT                            = TRAFO_WORLDIT,
    /*! \brief Indicates a transposed transform from world space to object space. */
    TRAFO_WORLDINVERSETRANSPOSE              = TRAFO_WORLDIT,
    /*! \brief Indicates a transposed transform from world space to object space. */
    TRAFO_MODELINVERSETRANSPOSE              = TRAFO_WORLDIT,

    /*! \brief Indicates a transposed transform from eye space to world space. */
    TRAFO_VIEWIT,
    /*! \brief Indicates a transposed transform from eye space to world space. */
    TRAFO_VIEWINVERSETRANSPOSE               = TRAFO_VIEWIT,

    /*! \brief Indicates a transposed transform from clip space to eye space. */
    TRAFO_PROJECTIONIT,
    /*! \brief Indicates a transposed transform from clip space to eye space. */
    TRAFO_PROJECTIONINVERSETRANSPOSE         = TRAFO_PROJECTIONIT,

    /*! \brief Indicates a transform from object space to eye space. */
    TRAFO_WORLDVIEW,              
    /*! \brief Indicates a transform from object space to eye space. */
    TRAFO_MODELVIEW                           = TRAFO_WORLDVIEW,
    /*! \brief Indicates a transform from world space to clip space. */
    TRAFO_VIEWPROJECTION,              

    /*! \brief Indicates a transform from object space to clip space. */
    TRAFO_WORLDVIEWPROJECTION,
    /*! \brief Indicates a transform from object space to clip space. */
    TRAFO_MODELVIEWPROJECTION                 = TRAFO_WORLDVIEWPROJECTION,

    /*! \brief Indicates a transposed transform from object space to eye space. */
    TRAFO_WORLDVIEWT,
    /*! \brief Indicates a transposed transform from object space to eye space. */
    TRAFO_MODELVIEWT                          = TRAFO_WORLDVIEWT,
    /*! \brief Indicates a transposed transform from object space to eye space. */
    TRAFO_WORLDVIEWTRANSPOSE                  = TRAFO_WORLDVIEWT,
    /*! \brief Indicates a transposed transform from object space to eye space. */
    TRAFO_MODELVIEWTRANSPOSE                  = TRAFO_WORLDVIEWT,
    /*! \brief Indicates a transposed transform from world space to clip space. */
    TRAFO_VIEWPROJECTIONT,              
    /*! \brief Indicates a transposed transform from world space to clip space. */
    TRAFO_VIEWPROJECTIONTRANSPOSE             = TRAFO_VIEWPROJECTIONT,              

    /*! \brief Indicates a transposed transform from object space to clip space. */
    TRAFO_WORLDVIEWPROJECTIONT,
    /*! \brief Indicates a transposed transform from object space to clip space. */
    TRAFO_MODELVIEWPROJECTIONT                = TRAFO_WORLDVIEWPROJECTIONT,
    /*! \brief Indicates a transposed transform from object space to clip space. */
    TRAFO_WORLDVIEWPROJECTIONTRANSPOSE        = TRAFO_WORLDVIEWPROJECTIONT,
    /*! \brief Indicates a transposed transform from object space to clip space. */
    TRAFO_MODELVIEWPROJECTIONTRANSPOSE        = TRAFO_WORLDVIEWPROJECTIONT,

    /*! \brief Indicates a transform from eye space to object space. */
    TRAFO_WORLDVIEWI,
    /*! \brief Indicates a transform from eye space to object space. */
    TRAFO_MODELVIEWI                          = TRAFO_WORLDVIEWI,
    /*! \brief Indicates a transform from eye space to object space. */
    TRAFO_WORLDVIEWINVERSE                    = TRAFO_WORLDVIEWI,
    /*! \brief Indicates a transform from eye space to object space. */
    TRAFO_MODELVIEWINVERSE                    = TRAFO_WORLDVIEWI,
    /*! \brief Indicates a transform from clip space to world space. */
    TRAFO_VIEWPROJECTIONI,              
    /*! \brief Indicates a transform from clip space to world space. */
    TRAFO_VIEWPROJECTIONINVERSE               = TRAFO_VIEWPROJECTIONI,              

    /*! \brief Indicates a transform from clip space to object space. */
    TRAFO_WORLDVIEWPROJECTIONI,
    /*! \brief Indicates a transform from clip space to object space. */
    TRAFO_MODELVIEWPROJECTIONI                = TRAFO_WORLDVIEWPROJECTIONI,
    /*! \brief Indicates a transform from clip space to object space. */
    TRAFO_WORLDVIEWPROJECTIONINVERSE          = TRAFO_WORLDVIEWPROJECTIONI,
    /*! \brief Indicates a transform from clip space to object space. */
    TRAFO_MODELVIEWPROJECTIONINVERSE          = TRAFO_WORLDVIEWPROJECTIONI,

    /*! \brief Indicates a transposed transform from eye space to object space. */
    TRAFO_WORLDVIEWIT,
    /*! \brief Indicates a transposed transform from eye space to object space. */
    TRAFO_MODELVIEWIT                         = TRAFO_WORLDVIEWIT,
    /*! \brief Indicates a transposed transform from eye space to object space. */
    TRAFO_WORLDVIEWINVERSETRANSPOSE           = TRAFO_WORLDVIEWIT,
    /*! \brief Indicates a transposed transform from eye space to object space. */
    TRAFO_MODELVIEWINVERSETRANSPOSE           = TRAFO_WORLDVIEWIT,
    /*! \brief Indicates a transposed transform from clip space to world space. */
    TRAFO_VIEWPROJECTIONIT,              
    /*! \brief Indicates a transposed transform from clip space to world space. */
    TRAFO_VIEWPROJECTIONINVERSETRANSPOSE      = TRAFO_VIEWPROJECTIONIT,              

    /*! \brief Indicates a transposed transform from clip space to object space. */
    TRAFO_WORLDVIEWPROJECTIONIT,
    /*! \brief Indicates a transposed transform from clip space to object space. */
    TRAFO_MODELVIEWPROJECTIONIT               = TRAFO_WORLDVIEWPROJECTIONIT,
    /*! \brief Indicates a transposed transform from clip space to object space. */
    TRAFO_WORLDVIEWPROJECTIONINVERSETRANSPOSE = TRAFO_WORLDVIEWPROJECTIONIT,
    /*! \brief Indicates a transposed transform from clip space to object space. */
    TRAFO_MODELVIEWPROJECTIONINVERSETRANSPOSE = TRAFO_WORLDVIEWPROJECTIONIT,

    NUM_TRAFOS
  };

  /*! \brief Enumeration type to identify input or output of shaders.
   * \remarks
   * CgFxEffect::getProgramInputType and CgFxEffect::getProgramOutputType let
   * you determine the input or output types of the programs.
   */
  enum ProgramIOType
  {
    /*! \brief input/output is a vertex */
    PROGRAM_IO_VERTEX       = CG_VERTEX,
    /*! \brief input/output is a fragment */
    PROGRAM_IO_FRAGMENT     = CG_FRAGMENT,
    /*! \brief input/output is a point */
    PROGRAM_IO_POINT        = CG_POINT,
    /*! \brief input/output is a line */
    PROGRAM_IO_LINE         = CG_LINE,
    /*! \brief input/output is a line with adjacency */
    PROGRAM_IO_LINE_ADJ     = CG_LINE_ADJ,
    /*! \brief input/output is a triangle */
    PROGRAM_IO_TRIANGLE     = CG_TRIANGLE,
    /*! \brief input/output is a triangle with adjacency*/
    PROGRAM_IO_TRIANGLE_ADJ = CG_TRIANGLE_ADJ,
  };

  enum
  {
    /*! \brief Maximum number of simultaneously supported MetaSL light parameters as
        \ref lightpositionparameters, \ref lightdirectionparameters, and \ref lighttypeparameters */
    MSL_MAX_LIGHTS = 8, 
    /*! \brief Accepted as \c type argument by CgFxEffect::setMslLightTypeValue to indicate
     * a positional light */
    MSL_LIGHT_POSITION = 0,
    /*! \brief Accepted as \c type argument by CgFxEffect::setMslLightTypeValue to indicate
     * a spot light */
    MSL_LIGHT_SPOT = 1,
    /*! \brief Accepted as \c type argument by CgFxEffect::setMslLightTypeValue to indicate
     * a directional light */
    MSL_LIGHT_INFINITE = 2
  };

public:
  /*! \brief Creates a render effect from an input file.
    * \param file
    * Specifies the path and filename of the effect's source file.
    * \param searchPaths
    * Specifies additional include directories to the Cg compiler.
    * The drive+dir string of the input file will be added as first additional
    * include directory for the Cg compiler if the searchPaths vector doesn't
    * already contain it. This will resolve the usual case of #include
    * directives for headers located relative to the input file directory.
    * \param err
    * Reference to a string that receives an error string in the event an
    * error occurred during creation.
    * \return
    * The function returns \c true if the effect creation was successful.
    * If the effect creation fails, the function returns \c false.
    * \remarks
    * The function tries to create an effect from the CgFX source file specified by \a file.
    * Known file extensions for files to hold valid CgFX code are .cg and .cgfx.
    * For the effect to be created successfully, the CgFX source code must comply with Cg 1.4
    * specification. More detailed information about Cg/CgFX can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * If an error occurred during creation, the function returns an error string in \a err which 
    * provides more information about the root cause of the error.
    * \par Example:
    * \code
    * // create a CgFX State Attribute Handle through the reference counted interface.
    * CgFxSharedPtr hCgfx(CgFx::create());
    * {
    *   // gain writable access to the CgFx handle
    *   CgFxWriteLock cg( hCgfx );
    *   // gain writable access to the CgFxEffect
    *   CgFxEffectWriteLock effect( cg->getEffect() );
    *   // 
    *   // Create effect from the file
    *   //
    *   vector<string> searchPaths; // Empty, add additional search paths here.
    *   std::string err;
    *   if(! effect->createFromFile( "c:\morrisonIsCool.cgfx", searchPaths, err ) )
    *   {
    *     nvutil::IO::errorMessage("effect creation failed", err, true);
    *     return false;
    *   }
    *   // effect creation succeeded - modify as needed
    *   [...]
    * 
    *  }
    *  // write access to the Cg shader and CgFxEffect ended inside above scope
    *
    *  // ...
    * \endcode
    * \sa CgFx, CgFx::getEffect, CgFxEffect::createFromLump 
    */
  NVSG_API bool createFromFile( const std::string& file, const std::vector<std::string>& searchPaths, std::string& err );

  /*! \brief Creates a render effect from a code string.
    * \param lump
    * Specifies the code string from which to create the effect.
    * \param searchPaths
    * Reference to a vector of strings which are used as additional include directories in the Cg compiler.
    * This allows to use the same shader code inside createFromFile() and createFromLump() with #include directives.
    * If the searchPaths vector is empty the shader source string should be self contained.
    * Avoid #include directives then, they would only be resolved to the application's current working directory by the Cg compiler
    * \param err
    * Reference to a string that receives an error string in case an error occurred during creation.
    * \return
    * The function returns \c true if the effect creation was successful.
    * If the effect creation fails, the function returns \c false.
    * \remarks
    * In contrast to CgFxEffect::createFromFile, this function attempts to create a render effect
    * from a string of CgFX source code. For the effect to be created successfully, The CgFX source 
    * code must comply with Cg 1.4 specification. 
    * More detailed information about Cg/CgFX can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * If an error occurred during creation, the function returns an error string in \a err which 
    * provides more information about the root cause of the error.
    * \par Example:
    * This example shows how to create a render effect from a CgFX source string.
    * \code
    * #include "nvsg/CgFx.h"
    * #include "nvutil/IOLayer.h"
    * #include "nvutil/SWMRSync.h" 
    * #include <string> // STL string
    *
    * using namespace nvsg;
    * using namespace nvutil;
    * using namespace nvutil::IO;
    *
    * std::string visualizeNormals(
    *   "float4x4 mvp : ModelViewProjection;                \n"
    *   "                                                   \n"
    *   "void vertex( float3 position : POSITION            \n"
    *   "           , float3 normal   : NORMAL              \n"
    *   "           , out float4 pos : POSITION             \n"
    *   "           , out float3 nor : TEXCOORD0 )          \n"
    *   "{                                                  \n"
    *   "  pos = mul(mvp, float4(position, 1.0f));          \n"
    *   "  nor = normal; // visualize object space normals  \n"
    *   "}                                                  \n"
    *   "                                                   \n"
    *   "float4 fragment ( float4 position : POSITION       \n"
    *   "                , float3 normal   : TEXCOORD0      \n"
    *   "                ) : COLOR                          \n"
    *   "{                                                  \n"
    *   "  float3 n = normalize(normal) * 0.5f + 0.5f;      \n"
    *   "  return float4(n, 1.0f);                          \n"
    *   "}                                                  \n"
    *   "                                                   \n"
    *   "technique t0                                       \n"
    *   "{                                                  \n"
    *   "  pass p0                                          \n"
    *   "  {                                                \n"
    *   "    VertexProgram   = compile latest vertex();     \n"
    *   "    FragmentProgram = compile latest fragment();   \n"
    *   "  }                                                \n"
    *   "}                                                  \n"
    * );
    *
    * // create a CgFX State Attribute Handle through the reference counted interface.
    * CgFxSharedPtr hCgfx(CgFx::create());
    * {
    *   CgFxWriteLock cgfx( hCgfx );
    *   CgFxEffectWriteLock effect( cgfx->getEffect() );
    *
    *   vector<string> searchPaths; // No searchPaths needed, shader code is self contained.
    *   std::string err;
    *   if ( !effect->createFromLump(visualizeNormals, searchPaths, err) )
    *   { 
    *       errorMessage("effect creation failed", err, true);
    *       return false;
    *   }
    *   // effect creation succeeded - go ahead
    *
    * } // exit of scope release write access to cgfx and effect
    * // ...
    * \endcode
    * \sa CgFx, CgFxEffect::createFromFile
    */
  NVSG_API bool createFromLump( const std::string& lump, const std::vector<std::string>& searchPaths, std::string& err );

  /*! \brief Returns whether the effect is valid.
    * \return 
    * The function returns \c true if the effect is valid.
    * If the effect is invalid, the function returns \c false.  
    * \remarks 
    * An effect should be valid if it was previously created successfully.
    * A render effect can be either created from a file using CgFxEffect::createFromFile,
    * or alternatively from a string of source code using CgFxEffect::createFromLump.
    * \n\n
    * This function primarily serves runtime checks to prevent an invalid effect from being  
    * applied to a geometry. Applying an invalid effect to a 
    * geometry results in undefined runtime behavior.
    * \sa CgFxEffect::createFromFile, CgFxEffect::createFromLump
    */
  NVSG_API bool isValid() const;

  /*! \brief Returns the CGMcontext associated with the effect.
    * \return 
    * The function returns a handle to the CGMcontext that is associated with the effect.
    * \remarks
    * Creating an effect requires a valid CGMcontext to be created in beforehand. 
    * The creation of a valid CGMcontext is wrapped by CgFxEffect::createFromFile and
    * CgFxEffect::createFromLump . Therefore, the user does not need to take care of the
    * CGMcontext creation by himself. However, one can think of situations where the
    * application needs to call the Cg-runtime directly, in particular when it comes to
    * device (e.g. OpenGL) dependent calls that require a valid CGMcontext handle as a 
    * parameter.
    */
  NVSG_API CGMcontext getContext() const;

  /*! \brief Labels the current transform state.
   * \param firstLabel First user defined label to identify a transform state.
   * \param secondLabel Second user defined label to identify a transform state.
   * \return
   * If the specified label does not match the previously set label, the function returns \c true
   * to indicate an invalid transform state. Otherwise the function returns \c false, indicating
   * that no update of the transform state is required.
   * \remarks
   * This function enables clients to label the current transform state for caching purposes.
   * An altered transform state needs to be synch'ed to the render effect immediately prior
   * to apply the effect. This function labels a client-side transform state and reports
   * back whether the effect's transform state requires an update. The label passed in from 
   * the client is treated as 'user-defined', and is only taken for comparison purposes. 
   */
  NVSG_API bool updateTransformState(size_t firstLabel, size_t secondLabel);

  /*! \brief Labels the current lighting state.
   * \param firstLabel First user defined label to identify a lighting state.
   * \param secondLabel Second user defined label to identify a lighting state.
   * \return
   * If the specified label does not match the previously set label, the function returns \c true
   * to indicate an invalid lighting state. Otherwise the function returns \c false, indicating
   * that no update of the lighting state is required.
   * \remarks
   * This function enables clients to label the current lighting state for caching purposes.
   * An altered lighting state needs to be synch'ed to the render effect immediately prior
   * to apply the effect. This function labels a client-side lighting state and reports
   * back whether the effect's lighting state requires an update. The label passed in from 
   * the client is treated as 'user-defined', and is only taken for comparison purposes. 
   */
  NVSG_API bool updateLightingState(size_t firstLabel, size_t secondLabel);

  /*! \brief Labels the current material state.
   * \param firstLabel First user defined label to identify a material state.
   * \param secondLabel Second user defined label to identify a material state.
   * \return
   * If the specified label does not match the previously set label, the function returns \c true
   * to indicate an invalid material state. Otherwise the function returns \c false, indicating
   * that no update of the material state is required.
   * \remarks
   * This function enables clients to label the current material state for caching purposes.
   * An altered material state needs to be synch'ed to the render effect immediately prior
   * to apply the effect. This function labels a client-side material state and reports
   * back whether the effect's material state requires an update. The label passed in from 
   * the client is treated as 'user-defined', and is only taken for comparison purposes. 
   */
  NVSG_API bool updateMaterialState(size_t firstLabel, size_t secondLabel);
  
  /*! \brief Labels the current scene state.
   * \param firstLabel First user defined label to identify a scene state.
   * \param secondLabel Second user defined label to identify a scene state.
   * \return
   * If the specified label does not match the previously set label, the function returns \c true
   * to indicate an invalid scene state. Otherwise the function returns \c false, indicating
   * that no update of the scene state is required.
   * \remarks
   * This function enables clients to label the current scene state for caching purposes.
   * An altered scene state needs to be synch'ed to the render effect immediately prior
   * to apply the effect. This function labels a client-side scene state and reports
   * back whether the effect's scene state requires an update. The label passed in from 
   * the client is treated as 'user-defined', and is only taken for comparison purposes. 
   */
  NVSG_API bool updateSceneState(size_t firstLabel, size_t secondLabel);

  /*! \brief Returns whether the effect was created from a file.
    * \return
    * The function returns \c true if the effect was create from a CgFX source file.
    * Otherwise, the function returns \c false.
    * \remarks
    * A render effect can be created either from a CgFX source file using CgFxEffect::createFromFile, 
    * or alternatively from a string of CgFX source code using CgFxEffect::createFromLump.
    * \n\n
    * Information on whether a certain effect was created from a file or a string could be useful,
    * for example, for implementing a scene saver which probably needs to track this information.
    * \sa CgFxEffect::createFromFile
    */
  NVSG_API bool isFromFile() const;

  /*! \brief Returns whether the effect was created from a lump of CgFX source code.
    * \return
    * The function returns \c true if the effect was create from a lump of CgFX source code.
    * Otherwise the function returns \c false.
    * \remarks
    * A render effect can be either created from a CgFX source file using CgFxEffect::createFromFile, 
    * or alternatively from a string of CgFX source code using CgFxEffect::createFromLump.
    * \n\n
    * Information on whether a certain effect was created from a file or a lump could be useful,
    * for example, for implementing a scene saver which probably needs to track this information.
    * \sa CgFxEffect::createFromLump
    */
  NVSG_API bool isFromLump() const;

  /*! \brief Returns the file name of the CgFX source file from which the effect was created.
    * \param 
    * file Reference to a string that receives the file name of the source file. 
    * \return
    * The function returns \c true if a source file name could be received successfully.
    * Otherwise, the function returns \c false and \a file will receive an empty string.
    * \remarks
    * If the function succeeds, the parameter \a file holds the path and file name of the
    * particular CgFX source file from which the effect was previously created.
    * \n\n
    * The function will fail if either the creation from file failed in beforehand, or if
    * the effect was not created from a file but from a source string. 
    * You can use CgFxEffect::isFromFile to test whether the effect was created from a file. 
    * \par Example:
    * \code
    * void printEffectFileName(const CgFx * cgfx)
    * {
    *     // assumes cgfx is read locked
    *     CgFxEffectReadLock effect( cgfx->getEffect() );
    *     if ( effect->isFromFile() )
    *     {
    *         string filename;
    *         NVSG_VERIFY(effect->getFileName(filename));
    *         printf("The effect was created from %s\n", filename.c_str());
    *     }
    *     else
    *     {
    *         printf("The effect was not created from a file.\n");
    *     }
    * }
    * \endcode
    * \sa CgFxEffect::isFromFile, CgFxEffect::createFromFile
    */
  NVSG_API bool getFileName(std::string& file) const;

  /*! \brief Returns CgFX source string from which the effect was created.
    * \param 
    * code Reference to a string that receives the string of CgFX source code. 
    * \return
    * The function returns \c true if the source code could be received successfully.
    * Otherwise, the function returns \c false and \a code will receive an empty string.
    * \remarks
    * If the function succeeds, the parameter \a code holds the source code 
    * from which the effect was previously created through CgFxEffect::createFromLump.
    * \n\n
    * The function will fail if either the creation from a source string failed beforehand, 
    * or if the effect was not created from a source string but from a file. 
    * You can use CgFxEffect::isFromLump to test whether the effect was created from a source string. 
    * \par Example:
    * \code
    * void printEffectSources(const CgFx * cgfx)
    * {
    *     // assumes cgfx is read locked
    *     CgFxEffectReadLock effect(cgfx->getEffect());
    *     if ( effect->isFromLump() )
    *     {
    *         string code;
    *         NVSG_VERIFY(effect->getSourceCode(code));
    *         printf("%s\n", code.c_str());
    *     }
    *     else
    *     {
    *         printf("The effect was not created from a source string.\n");
    *     }
    * }
    * \endcode
    * \sa CgFxEfect::isFromLump, CgFxEffect::createFromLump
    */
  NVSG_API bool getSourceCode(std::string& code) const;

  // API concerning annotation queries

  /*! \brief Returns the number of annotations available for a specified parameter.
    * \param
    * param Handle to the effect parameter from which to query the number of available annotations.
    * \return
    * The function returns the number of annotations available for the parameter specified by
    * \a param.
    * \remarks
    * The function is particularly useful if you need to iterate over all available annotations
    * for a certain effect parameter.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use getTweakable, getParameterByName, or getParameterBySemantic to get a valid
    * handle to a specific effect parameter.
    * \par Example:
    * \code
    * void printParameterAnnotationNames(const CgFxEffect * effect, CgFxParameter param)
    * {
    *     unsigned int numAnnos = effect->getNumberOfParameterAnnotations(param);
    *     for ( unsigned int i=0; i<numAnnos; ++i )
    *     {
    *         CgFxAnnotation anno = effect->getParameterAnnotation(param, i);
    *         printf("Annotation %d: %s\n", i, effect->getAnnotationName(anno).c_str());
    *     }
    * }
    *
    * \endcode
    * \sa CgFxEffect::getParameterAnnotation, CgFxEffect::getTweakable, CgFxEffect::getParameterByName,
    * CgFxEffect::getParameterBySemantic
    */
  NVSG_API unsigned int getNumberOfParameterAnnotations(CgFxParameter param) const;

  /*! \brief Returns the parameter annotation at the specified zero-based index.
    * \param
    * param Handle to the effect parameter from which to get the annotation.
    * \param
    * index Zero-based index of the annotation.
    * \return
    * The function returns a handle to the queried parameter annotation.
    * \remarks
    * For example, to query an annotation's name, its value type, or its value, you need
    * a handle to that annotation. This function is particularly useful if you don't
    * know the exact names of a certain parameter's annotations. If you want to get a  
    * particular annotation that you know by name, it would be more effective to get the
    * annotation by calling CgFxEffect::getParameterAnnotationByName instead.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \n\n
    * You should use the function always in conjunction with 
    * CgFxEffect::getNumberOfParameterAnnotations  to secure always passing a valid index 
    * to the function. Passing an invalid index -  that is, an index greater or equal to what 
    * CgFxEffect::getNumberOfParameterAnnotation returns for the specified parameter - would 
    * result in undefined runtime behavior.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * CgFxParameter param;
    * [...]
    * // Assume a valid read-locked effect and a valid parameter here
    * unsigned int numAnnos = effect->getNumberOfParameterAnnotations(param);
    *
    * for ( unsigned int i=0; i<numAnnos; ++i )
    * {
    *     CgFxAnnotation anno = effect->getParameterAnnotation(param, i);
    *     // proceed with annotation
    *     [...]
    * }
    * \endcode
    * \sa CgFxEffec::getParameterAnnotationByName, CgFxEffect::getTweakable, CgFxEffect::getParameterByName,
    * CgFxEffect::getParameterBySemantic
    */
  NVSG_API CgFxAnnotation getParameterAnnotation(CgFxParameter param, unsigned int index) const;

  /*! \brief Returns an annotation of the specified parameter by name.
    * \param
    * param Handle to the effect parameter for which to get the named annotation.
    * \param
    * name A string holding the name of the annotation to query for.
    * \return
    * The function returns a valid annotation handle if a matching annotation was found. 
    * Otherwise, a NULL handle is returned to indicate that an annotation with the specified name 
    * could not be found.
    * \remarks
    * To query a parameter annotation for a certain attributes such as its name, its value type, or
    * its value, you need a valid handle to that annotation. Use this function if you know
    * the annotation by name, or if you expect a certain kind of parameter to have a conventional
    * annotation. For example a texture parameter often has the \b File annotation to hold the filename
    * of the default texture file to load the texture image from. Or you might conventionally add
    * a \b Description annotation to your effect parameters, which holds a general parameter description
    * that you might offer to the user by means of a graphical user interface (GUI).
    * \n\n
    * If you are not interested in a certain 'named' annotation but you want to access all available 
    * annotations for a certain parameter, you should use getParameterAnnotation instead, which accesses
    * the available parameter annotations by a zero-based index.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use getTweakable, getParameterByName, or getParameterBySemantic to get a valid
    * handle to a certain effect parameter.
    * \par Example:
    * \code
    * void printParameterDescription(const CgFxEffect * effect, CgFxParameter param)
    * {
    *     CgFxAnnotation anno = effect->getParameterAnnotationByName(param, "Description");
    *     if ( anno )
    *     {
    *         // description should be a string
    *         NVSG_ASSERT(effect->isStringAnnotation(anno));
    *         std::string desc;
    *         effect->getStringAnnotationValue(anno, desc);
    *         printf("Description: %s\n", desc.c_str());
    *     }
    * }
    * \endcode
    * \sa CgFxEffect::getParameterAnnotation, CgFxEffect::getTweakable, CgFxEffect::getParameterByName,
    * CgFxEffect::getParameterBySemantic
    */
  NVSG_API CgFxAnnotation getParameterAnnotationByName(CgFxParameter param, const std::string& name) const;

  /*! \brief Returns the number of annotations available for a specified technique.
    * \param
    * tech Handle to the technique from which to query the number of available annotations.
    * \return
    * The function returns the number of annotations available for the technique specified by \a tech.
    * \remarks
    * The function is particularly useful if you need to iterate over all available annotations
    * for a certain technique.
    * \n\n
    * Calling this function for an invalid technique handle results in undefined runtime behavior.
    * You can use getTechnique or getTechniqueByName to get a valid handle to a certain technique.
    * \par Example:
    * \code
    * void printTechniqueAnnotationNames(const CgFxEffect * effect, CgFxTechnique tech)
    * {
    *     unsigned int numAnnos = effect->getNumberOfTechniqueAnnotations(tech);
    *     for ( unsigned int i=0; i<numAnnos; ++i )
    *     {
    *         CgFxAnnotation anno = effect->getTechniqueAnnotation(tech, i);
    *         printf("Annotation %d: %s\n", i, effect->getAnnotationName(anno).c_str());
    *     }
    * }
    *
    * \endcode
    * \sa CgFxEffect::getTechniqueAnnotation, CgFxEffect::getTechnique, CgFxEffect::getTechniqueByName
    */
  NVSG_API unsigned int getNumberOfTechniqueAnnotations(CgFxTechnique tech) const;

  /*! \brief Returns the technique annotation at the specified zero-based index.
    * \param
    * tech Handle to the technique from which to get the annotation.
    * \param
    * index Zero-based index of the annotation.
    * \return
    * The function returns a handle to the queried technique annotation.
    * \remarks
    * For example, to query an annotation's name, its value type, or its value, you need
    * a handle to that annotation. This function is particularly useful if you don't
    * know the exact names of a certain technique's annotations. If you want to get 
    * a particular annotation that you know by name, it would be more effective to get the
    * annotation by calling getTechniqueAnnotationByName instead.
    * \n\n
    * Calling this function for an invalid technique handle results in undefined runtime behavior.
    * You can use getTechnique or getTechniqueByName to get a valid handle to a certain technique.
    * \n\n
    * You should use the function always in conjunction with getNumberOfTechniqueAnnotations
    * to secure always passing a valid index to the function. Passing an invalid index - 
    * that is, an index greater or equal to what getNumberOfTechniqueAnnotation returns for
    * the specified technique - would result in undefined runtime behavior.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * CgFxTechnique tech;
    * [...]
    * // assume a valid read-locked effect and a valid technique here
    * unsigned int numAnnos = effect->getNumberOfTechniqueAnnotations(tech);
    *
    * for ( unsigned int i=0; i<numAnnos; ++i )
    * {
    *     CgFxAnnotation anno = effect->getTechniqueAnnotation(tech, i);
    *     // proceed with annotation
    *     [...]
    * }
    * \endcode
    * \sa CgFxEffec::getTechniqueAnnotationByName, CgFxEffect::getTechnique, CgFxEffect::getTechniqueByName
    */
  NVSG_API CgFxAnnotation getTechniqueAnnotation(CgFxTechnique tech, unsigned int index) const;

  /*! \brief Returns an annotation of the specified technique by name.
    * \param
    * tech Handle to the technique for which to get the named annotation.
    * \param
    * name A string holding the name of the annotation to query for.
    * \return
    * The function returns a valid annotation handle if a matching annotation was found. 
    * Otherwise a NULL handle is returned to indicate that an annotation with the specified name 
    * could not be found.
    * \remarks
    * To query a technique annotation for a certain attribute such as its name, its value type, or
    * its value, you need a valid handle to that annotation. Use this function if you know
    * the annotation by name, or if you expect a technique to have a conventional annotation. 
    * For example, you might conventionally add a \b Description annotation to your techniques, 
    * which holds a general technique description that you might offer to the user by means of a 
    * graphical user interface (GUI).
    * \n\n
    * If you are not interested in a certain 'named' annotation but you want to access all available 
    * annotations for a certain technique, you should use getTechniqueAnnotation instead, which accesses
    * the available technique annotations by a zero-based index.
    * \n\n
    * Calling this function for an invalid technique handle results in undefined runtime behavior.
    * You can use getTechnique or getTechniqueByName to get a valid handle to a certain technique.
    * \par Example:
    * \code
    * void printTechniqueDescription(const CgFxEffect * effect, CgFxTechnique tech)
    * {
    *     CgFxAnnotation anno = effect->getTechniqueAnnotationByName(tech, "Description");
    *     if ( anno )
    *     {
    *         // description should be a string
    *         NVSG_ASSERT(effect->isStringAnnotation(anno));
    *         std::string desc;
    *         effect->getStringAnnotationValue(anno, desc);
    *         printf("Description: %s\n", desc.c_str());
    *     }
    * }
    * \endcode
    * \sa CgFxEffect::getTechniqueAnnotation, CgFxEffect::getTechnique, CgFxEffect::getTechniqueByName
    */
  NVSG_API CgFxAnnotation getTechniqueAnnotationByName(CgFxTechnique tech, const std::string& name) const;

  /*! \brief Returns the number of annotations available for a specified render pass.
    * \param
    * pass Handle to the pass from which to query the number of available annotations.
    * \return
    * The function returns the number of annotations available for the render pass specified by \a pass.
    * \remarks
    * The function is particularly useful if you need to iterate over all annotations available
    * for a certain render pass.
    * \n\n
    * Calling this function for an invalid pass handle results in undefined runtime behavior.
    * You can use getPass or getPassByName to get a valid handle to a certain render pass.
    * \par Example:
    * \code
    * void printPassAnnotationNames(const CgFxEffect * effect, CgFxPass pass)
    * {
    *     unsigned int numAnnos = effect->getNumberOfPassAnnotations(pass);
    *     for ( unsigned int i=0; i<numAnnos; ++i )
    *     {
    *         CgFxAnnotation anno = effect->getPassAnnotation(pass, i);
    *         printf("Annotation %d: %s\n", i, effect->getAnnotationName(anno).c_str());
    *     }
    * }
    * \endcode
    * \sa CgFxEffect::getPassAnnotation, CgFxEffect::getPass, CgFxEffect::getPassByName
    */
  NVSG_API unsigned int getNumberOfPassAnnotations(CgFxPass pass) const;

  /*! \brief Returns the pass annotation at the specified zero-based index.
    * \param
    * pass Handle to the pass from which to get the annotation.
    * \param
    * index Zero-based index of the annotation.
    * \return
    * The function returns a handle to the queried pass annotation.
    * \remarks
    * For example, to query an annotation's name, its value type, or its value, you need
    * a handle to that annotation. This function in particular is useful if you don't
    * know the exact names of a certain pass's annotations. If you want to get 
    * a particular annotation that you know by name, it would be more effective to get the
    * annotation by calling getTechniqueAnnotationByName instead.
    * \n\n
    * Calling this function for an invalid pass handle results in undefined runtime behavior.
    * You can use getPass or getPassByName to get a valid handle to a certain render pass.
    * \n\n
    * You should use the function always in conjunction with getNumberOfPassAnnotations
    * to avoid passing an invalid index to the function. Passing an invalid index - , 
    * that is, an index greater or equal to what getNumberOfPassAnnotation returns for
    * the specified pass - would result in undefined runtime behaviour.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * CgFxPass pass;
    * [...]
    * // assume a valid read-locked effect and a valid pass here
    * unsigned int numAnnos = effect->getNumberOfPassAnnotations(pass);
    *
    * for ( unsigned int i=0; i<numAnnos; ++i )
    * {
    *     CgFxAnnotation anno = effect->getPassAnnotation(pass, i);
    *     // proceed with annotation
    *     [...]
    * }
    * \endcode
    * \sa CgFxEffec::getPassAnnotationByName, CgFxEffect::getPass, CgFxEffect::getPassByName
    */
  NVSG_API CgFxAnnotation getPassAnnotation(CgFxPass pass, unsigned int index) const;

  /*! \brief Returns an annotation of the specified pass by name.
    * \param
    * pass Handle to the render pass for which to get the named annotation.
    * \param
    * name A string holding the name of the annotation to query for.
    * \return
    * The function returns a valid annotation handle if a matching annotation was found. 
    * Otherwise a NULL handle is returned to indicate that an annotation with the specified name 
    * could not be found.
    * \remarks
    * To query a pass annotation for a certain attributes such as its name, its value type, or
    * its value, you need a valid handle to that annotation. Use this function if you know
    * the annotation by name, or if you expect a render pass to have a conventional annotation. 
    * For example, you might conventionally add a \b Description annotation to your render passes, 
    * which holds a general pass description that you might offer to the user by means of a 
    * graphical user interface (GUI).
    * \n\n
    * If you are not interested in a certain 'named' annotation but you want to access all 
    * annotations available for a certain pass, you should use getPassAnnotation instead, which 
    * accesses the available pass annotations by a zero-based index.
    * \n\n
    * Calling this function for an invalid pass handle results in undefined runtime behavior.
    * You can use getPass or getPassByName to get a valid handle to a certain render pass.
    * \par Example:
    * \code
    * void printPassDescription(const CgFxEffect * effect, CgFxPass pass)
    * {
    *     CgFxAnnotation anno = effect->getPassAnnotationByName(pass, "Description");
    *     if ( anno )
    *     {
    *         // description should be a string
    *         NVSG_ASSERT(effect->isStringAnnotation(anno));
    *         std::string desc;
    *         effect->getStringAnnotationValue(anno, desc);
    *         printf("Description: %s\n", desc.c_str());
    *     }
    * }
    * \endcode
    * \sa CgFxEffect::getPassAnnotation, CgFxEffect::getPass, CgFxEffect::getPassByName
    */
  NVSG_API CgFxAnnotation getPassAnnotationByName(CgFxPass pass, const std::string& name) const;
  
  /*! \brief Returns the name of the specified annotation.
    * \param
    * anno Handle to the annotation for which to get the name.
    * \return
    * The function returns a reference to a string holding the annotation's name as specified
    * in the CgFx effect sources.
    * \n\n
    * Annotations can be optionally added to effect parameters, to techniques, or to render passes.
    * Annotations are thought to provide additional information that might be useful for certain 
    * applications. For an effect technique, for example, the effect writer could add an optional
    * \b Description annotation that describes exactly what the technique does.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For a CgFxParameter, for example, you can use getParameterAnnotation to get a valid annotation
    * handle.
    * \sa CgFxEffect::getParameterAnnotation, CgFxEffect::GetTechniqueAnnotation, 
    * CgFxEffect::getPassAnnotation
    */
  NVSG_API const std::string& getAnnotationName(CgFxAnnotation anno) const;

  /*! \brief Returns the type string of the specified annotation's value.
    * \param
    * anno Handle to the annotation for which to get the type string.
    * \return
    * The function returns a reference to a string holding annotation value's type string.
    * For a string-type annotation, for example, this would be "string".
    * \remarks
    * Each annotation has a value of a certain type. Use this function if you need this
    * value as a string.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For a CgFxParameter, for example, you can use getParameterAnnotation to get a valid annotation
    * handle.
    * \sa CgFxEffect::getParameterAnnotation, CgFxEffect::GetTechniqueAnnotation, 
    * CgFxEffect::getPassAnnotation
    */ 
  NVSG_API const std::string& getAnnotationTypeString(CgFxAnnotation anno) const;
  
  /*! \brief Returns whether the specified annotation is a float-type annotation.
    * \param
    * anno Handle to the annotation to query.
    * \return
    * The function returns \c true if the annotations value is of type float.
    * Otherwise, the function returns \c false.
    * \remarks
    * To query an annotation value, you must know the value type and then
    * call the corresponding 'getValue' function to receive the value.
    * For a float-type annotation value, for example, you must call 
    * CgFxEffect::getFloatAnnotationValue, whereas for a string-type annotation value
    * you must call CgFxEffect::getStringAnnotationValue to get the value.
    * \n\n
    * This function tells you if the annotation value is a float-type value.
    * It does not tell you how many components - that is, how many single float values -
    * the value has. To determine this, you additioally need to call 
    * CgFxEffect::getNumberOfAnnotationValueComponents.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid 
    * annotation handle.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * CgFxAnnotation anno;
    * // ...
    * // assume a valid read-locked effect and a valid annotation handle here
    * if ( effect->isFloatAnnotation(anno) )
    * {
    *     if ( 1==effect->getNumberOfAnnotationValueComponents(anno) )
    *     {
    *         // its a single float value
    *         float value;
    *         effect->getFloatAnnotationValue(anno, &value);
    *         printf("Value: %f\n", value);
    *     }
    *     else
    *     {
    *       // more components ...
    *     }
    * }
    * // other type
    * else
    * // ...
    * \endcode
    * \sa CgFxEffect::getFloatAnnotationValue, CgFxEffect::getNumberOfAnnotationValueComponents
    */
  NVSG_API bool isFloatAnnotation(CgFxAnnotation anno) const;

  /*! \brief Returns whether the specified annotation is a integer-type annotation.
    * \param
    * anno Handle to the annotation to query.
    * \return
    * The function returns \c true if the annotations value is of type int.
    * Otherwise, the function returns \c false.
    * \remarks
    * To query an annotation value, you must know the value type and then
    * call the corresponding 'getValue' function to receive the value.
    * For a integer-type annotation value, for example, you must call 
    * CgFxEffect::getIntAnnotationValue, whereas for a string-type annotation value
    * you must call CgFxEffect::getStringAnnotationValue to get the value.
    * \n\n
    * This function tells you if the annotation value is a integer-type value.
    * It does not tell you how many components - that is, how many single integer values -
    * the value has. To determine this, you additionally need to call 
    * CgFxEffect::getNumberOfAnnotationValueComponents.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid 
    * annotation handle.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * CgFxAnnotation anno;
    * // ...
    * // assume a valid read-locked effect and a valid annotation handle here
    * if ( effect->isIntAnnotation(anno) )
    * {
    *     if ( 4==effect->getNumberOfAnnotationValueComponents(anno) )
    *     {
    *         // its a 4-component integer value
    *         int value[4]; // array to receive the 4-component integer value
    *         effect->getIntAnnotationValue(anno, value);
    *         printf("Value: %d, %d, %d, %d\n", value[0], value[1], value[2], value[3]);
    *     }
    *     else
    *     {
    *       // ...
    *     }
    * }
    * // other type
    * else
    * // ...
    * \endcode
    * \sa CgFxEffect::getIntAnnotationValue, CgFxEffect::getNumberOfAnnotationValueComponents
    */
  NVSG_API bool isIntAnnotation(CgFxAnnotation anno) const;

  /*! \brief Returns whether the specified annotation is a boolean-type annotation.
    * \param
    * anno Handle to the annotation to query.
    * \return
    * The function returns \c true if the annotations value is of type 32-bit bool.
    * Otherwise, the function returns \c false.
    * \remarks
    * To query an annotation value, you must know the value type and then
    * call the corresponding 'getValue' function to receive the value.
    * For a boolean-type annotation value, for example, you must call 
    * CgFxEffect::getBoolAnnotationValue, whereas for a string-type annotation value
    * you must call CgFxEffect::getStringAnnotationValue to get the value.
    * \n\n
    * This function tells you if the annotation value is a boolean-type value.
    * It does not tell you how many components - that is, how many single integer values - 
    * the value has. To determine this, you additionally need to call 
    * CgFxEffect::getNumberOfAnnotationValueComponents.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid 
    * annotation handle.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * CgFxAnnotation anno;
    * // ...
    * // assume a valid read-locked effect and a valid annotation handle here
    * if ( effect->isBoolAnnotation(anno) )
    * {
    *     if ( 1==effect->getNumberOfAnnotationValueComponents(anno) )
    *     {
    *         // its a 1-component 32-bit boolean value
    *         int value; // value to receive the 1-component 32-bit boolean value
    *         effect->getBoolAnnotationValue(anno, &value);
    *         printf("Value: %s\n", value ? "TRUE" : "FALSE");
    *     }
    *     else
    *     {
    *       // ...
    *     }
    * }
    * // other type
    * else
    * // ...
    * \endcode
    * \sa CgFxEffect::getBoolAnnotationValue, CgFxEffect::getNumberOfAnnotationValueComponents
    */
  NVSG_API bool isBoolAnnotation(CgFxAnnotation anno) const;

  /*! \brief Returns whether the specified annotation is a string-type annotation.
    * \param
    * anno Handle to the annotation to query.
    * \return
    * The function returns \c true if the annotations value is of type string.
    * Otherwise, the function returns \c false.
    * \remarks
    * To query an annotation value, you must know the value type and then
    * call the corresponding 'getValue' function to receive the value.
    * For a string-type annotation value, for example, you must call 
    * CgFxEffect::getStringAnnotationValue, whereas for a float-type annotation value
    * you must call CgFxEffect::getFloatAnnotationValue to get the value.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter for example, you can use getParameterAnnotation to get a valid 
    * annotation handle.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * CgFxAnnotation anno;
    * // ...
    * // assume a valid read-locked effect and a valid annotation handle here
    * if ( effect->isStringAnnotation(anno) )
    * {
    *     // its a string value
    *     std::string value; // value to receive the string
    *     effect->getStringAnnotationValue(anno, value);
    *     printf("Value: %s\n", value.c_str());
    * }
    * // other type
    * else
    * // ...
    * \endcode
    * \sa CgFxEffect::getBoolAnnotationValue, CgFxEffect::getNumberOfAnnotationValueComponents
    */
  NVSG_API bool isStringAnnotation(CgFxAnnotation anno) const;

  /*! \brief Returns the size in bytes of the annotation value.
    * \param
    * anno Handle to the annotation to query.
    * \return
    * The function returns the size in bytes of the annotation value.
    * \remarks
    * To receive an annotation value the user must provide a value buffer large enough
    * to hold the value. This function can be used to determine how large the receiving
    * buffer needs to be in bytes. 
    * \n\n
    * Alternatively, you can use CgFxEffect::getNumberOfAnnotationValueComponents together 
    * with the value's type information (e.g. CgFxEffect::isFloatAnnotation) to determine 
    * the size of the receiving value buffer.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter for example, you can use getParameterAnnotation to get a valid annotation
    * handle.
    * \sa CgFxEffect::getNumberOfAnnotationValueComponents 
    */
  NVSG_API unsigned int getAnnotationValueSize(CgFxAnnotation anno) const;
  
  /*! \brief Returns the number of components the annotation value has.
    * \param
    * anno Handle to the annotation to query.
    * \return
    * The function returns the number of value components.
    * An annotation of type float4, for example, has 4 value components.
    * \remarks
    * To receive an annotation value the user must provide a value buffer large enough
    * to hold the value. This function together with the value type information ( e.g. 
    * CgFxEffect::isFloatAnnotation" can be used to determine how large the receiving
    * buffer needs to be.
    * \n\n
    * Note that this function is meaningful only for numeric value types like float, integer,
    * or boolean types. For string annotations the value will be received in a STL string object
    * (see CgFxEffect::getStringAnnotationValue) instead of a plain value buffer.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid 
    * annotation
    * handle.
    * \sa CgFxEffect::getAnnotationValueSize
    */
  NVSG_API unsigned int getNumberOfAnnotationValueComponents(CgFxAnnotation anno) const;

  /*! \brief Returns the value of a float-type annotation.
    * \param
    * anno Handle to the float-type annotation to get the value from.
    * \param
    * value Pointer to a float buffer that will receive the annotation value.
    * \return
    * The function returns \c true if the value successfully could have copied to the \c value buffer.
    * Otherwise, the function returns false.
    * \remarks
    * The function is dedicated to be used with float-type annotations only. 
    * The function returns \c false if it is called for other than float-type annotations.
    * Use CgFxEffect::isFloatAnnotation to determine whether the annotation is a float-type
    * annotation.
    * \n\n
    * The caller must provide a buffer large enough to hold the annotation value.
    * Use either CgFxEffect::getNumberOfAnnotationValueComponents or CgFxEffect::getAnnotationValueSize 
    * to determine the required size for the buffer to receive the value.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid annotation
    * handle.
    * \n\n
    * The behavior also is undefined if either the pointer to the \c value buffer is invalid, or the
    * buffer behind is not large enough to hold the annotation value.
    * \sa CgFxEffect::isFloatAnnotation, CgFxEffect::getNumberOfAnnotationValueComponents, 
    * CgFxEffect::getAnnotationValueSize
    */
  NVSG_API bool getFloatAnnotationValue(CgFxAnnotation anno, float * value) const;

  /*! \brief Returns the value of a integer-type annotation.
    * \param
    * anno Handle to the integer-type annotation to get the value from.
    * \param
    * value Pointer to an integer buffer that will receive the annotation value.
    * \return
    * The function returns \c true if the value successfully could have copied to the \c value buffer.
    * Otherwise, the function returns false.
    * \remarks
    * The function is dedicated to be used with integer-type annotations only. 
    * The function returns \c false if it is called for other than integer-type annotations.
    * Use CgFxEffect::isIntAnnotation to determine whether the annotation is an integer-type
    * annotation.
    * \n\n
    * The caller must provide a buffer large enough to hold the annotation value.
    * Use either CgFxEffect::getNumberOfAnnotationValueComponents or CgFxEffect::getAnnotationValueSize 
    * to determine the required size for the buffer to receive the value.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid annotation
    * handle.
    * \n\n
    * The behavior also is undefined if either the pointer to the \c value buffer is invalid, or the
    * buffer behind is not large enough to hold the annotation value.
    * \sa CgFxEffect::isIntAnnotation, CgFxEffect::getNumberOfAnnotationValueComponents, 
    * CgFxEffect::getAnnotationValueSize
    */
  NVSG_API bool getIntAnnotationValue(CgFxAnnotation anno, int * value) const;

  /*! \brief Returns the value of a boolean-type annotation.
    * \param
    * anno Handle to the boolean-type annotation to get the value from.
    * \param
    * value Pointer to an integer buffer that will receive the annotation value.
    * \return
    * The function returns \c true if the value successfully could have copied to the \c value buffer.
    * Otherwise, the function returns false.
    * \remarks
    * The function is dedicated to be used with boolean-type annotations only. 
    * The function returns \c false if it is called for other than boolean-type annotations.
    * Use CgFxEffect::isBoolAnnotation to determine whether the annotation is a boolean-type
    * annotation.
    * \n\n
    * The caller must provide a buffer large enough to hold the annotation value.
    * Use either CgFxEffect::getNumberOfAnnotationValueComponents or CgFxEffect::getAnnotationValueSize 
    * to determine the required size for the buffer to receive the value. Keep in mind that a boolean
    * in this context is a 32-bit integer value!
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid annotation
    * handle.
    * \n\n
    * The behavior also is undefined if either the pointer to the \c value buffer is invalid, or the
    * buffer behind is not large enough to hold the annotation value.
    * \sa CgFxEffect::isBoolAnnotation, CgFxEffect::getNumberOfAnnotationValueComponents, 
    * CgFxEffect::getAnnotationValueSize
    */
  NVSG_API bool getBoolAnnotationValue(CgFxAnnotation anno, int * value) const;

  /*! \brief Returns the value of a string-type annotation.
    * \param
    * anno Handle to the string-type annotation to get the value from.
    * \param
    * value Reference to a STL string that receives the value
    * \return
    * The function returns \c true if the string value could have assigned to \c value.
    * Otherwise, the function returns false.
    * \remarks
    * The function is dedicated to be used with string-type annotations only. 
    * The function returns \c false if it is called for other than string-type annotations.
    * Use CgFxEffect::isStringAnnotation to determine whether the annotation is a string-type
    * annotation.
    * \n\n
    * Calling this function for an invalid annotation handle results in undefined runtime behavior.
    * For an effect parameter, for example, you can use getParameterAnnotation to get a valid annotation
    * handle.
    * \sa CgFxEffect::isStringAnnotation
    */
  NVSG_API bool getStringAnnotationValue(CgFxAnnotation anno, std::string& value) const;

  /*! \brief Returns the name of the specified effect parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns a reference to a const STL string that holds the parameter name.
    * \remarks
    * The parameter name will be received as specified in the CgFX sources. 
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getTweakable, CgFxEffect::getParameterByName, CgFxEffect::getParameterBySemantic
    */
  NVSG_API const std::string& getParameterName(CgFxParameter param) const;    

  /*! \brief Returns the semantic of the specified effect parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns a reference to a const STL string that holds the semantic.
    * \remarks
    * The parameter semantic will be received as specified in the CgFX sources. 
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getTweakable, CgFxEffect::getParameterByName, CgFxEffect::getParameterBySemantic
    */
  NVSG_API const std::string& getParameterSemantic(CgFxParameter param) const;

  /*! \brief Returns the type of the specified parameter as string.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns a reference to a const STL string that holds the type string.
    * For a float4-type parameter, for example, this would be "float4".
    * \remarks
    * Each parameter is of a certain type. Use this function if you need this type as a string.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getTweakable, CgFxEffect::getParameterByName, CgFxEffect::getParameterBySemantic
    */ 
  NVSG_API const std::string& getParameterTypeString(CgFxParameter param) const;
  
  /*! \brief Returns an effect parameter by name.
    * \param
    * name Specifies the name of the desired effect parameter.
    * \return
    * The function returns a valid parameter handle if a parameter with the specified name
    * was found in the effect. Otherwise, NULL is returned to indicate that an effect parameter 
    * with the specified name could not be found. 
    * \remarks
    * The function case-sensitive looks up the effect parameter by its name.
    * Note that for SceniX only tweakables and sampler parameters are considered effect parameters!
    * Parameters that automatically get connected to objects in the scene graph, like NVSGLight,
    * NVSGMaterial, NVSGScene, MetaSL specific light parameters as \verbatim '__light<N>_position', 
    * '__light<N>_direction', and '__light<N>_type',\endverbatim and transform parameters, can not be queried
    * using this function.
    * \n\n
    * The parameters of an effect can be retrieved directly by name using this function. 
    * The names of the parameters in an effect can be discovered by iterating through the 
    * effect parameters (see e.g. CgFxEffect::getNumberOfTweakables and 
    * CgFxEffect::getTweakable), and calling CgFxEffect::getParameterName for each parameter 
    * in turn. 
    * \par Example:
    * The parameter name does not have to be a complete name for a leaf node parameter. 
    * For example, if you have a CgFX effect with the following parameters:
    * \code
    * struct FooStruct
    * {
    *     float4 A;
    *     float4 B;
    * };
    *
    * struct BarStruct
    * {
    *     FooStruct Foo[2];
    * };
    *
    * BarStruct Bar[3];
    * \endcode
    * The following leaf-node parameters will be generated:
    * \code
    * Bar[0].Foo[0].A
    * Bar[0].Foo[0].B
    * Bar[0].Foo[1].A
    * Bar[0].Foo[1].B
    * Bar[1].Foo[0].A
    * Bar[1].Foo[0].B
    * Bar[1].Foo[1].A
    * Bar[1].Foo[1].B
    * Bar[2].Foo[0].A
    * Bar[2].Foo[0].B
    * Bar[2].Foo[1].A
    * Bar[2].Foo[1].B
    * \endcode
    * A handle to any of the non-leaf arrays or structs can be directly obtained by using the 
    * appropriate name. The following are a few examples of valid names that may be used with 
    * CgFxEffect::getParameterByName given the above example CgFX effect:
    * \code
    * "Bar"
    * "Bar[1]"
    * "Bar[1].Foo"
    * "Bar[1].Foo[0]"
    * "Bar[1].Foo[0].B"
    * ...
    * \endcode
    * \sa CgFxEffect::getNumberOfTweakables, CgFxEffect::getTweakable 
    */
  NVSG_API CgFxParameter getParameterByName(const std::string& name) const;

  /*! \brief Returns an effect parameter by semantic.
    * \param
    * semantic Specifies the semantic of the desired effect parameter.
    * \return
    * The function returns a valid parameter handle if a matching effect parameter was found. 
    * Otherwise, NULL is returned to indicate that an effect parameter with the specified 
    * semantic could not be found.
    * \remarks
    * The function case-sensitive looks up the effect parameter by it's optional semantic.
    * Note that for SceniX only tweakables and sampler parameters are considered effect parameters!
    * Parameters that automatically get connected to objects in the scene graph, like NVSGLight,
    * NVSGMaterial, NVSGScene, MetaSL specific light parameters as \verbatim '__light<N>_position', 
    * '__light<N>_direction', and '__light<N>_type', \endverbatim and transform parameters, can not be queried
    * using this function.
    * \sa CgFxEffect::getParameterByName
    */
  NVSG_API CgFxParameter getParameterBySemantic(const std::string& semantic) const;

  /*! \brief Returns the array size of the specified array parameter.
    * \param
    * param Handle to the array parameter to query.
    * \return
    * The function returns the number of array elements - that is,the size of the array.
    * \remarks
    * This function in conjunction with CgFxEffect::getArrayParameterElement can be used to
    * iterate through the elements of an array parameter.
    * \n\n
    * Calling this function for other than array parameters results in undefined behavior.
    * Use CgFxEffect::isArrayParameter to determine whether the parameter is an array parameter.
    * \sa CgFxEffect::getArrayParameterElement
    */
  NVSG_API unsigned int getNumberOfArrayParameterElements(CgFxParameter param) const;

  /*! \brief Returns the array element at a specified zero-based index.
    * \param
    * param Handle to the array parameter to retrieve the specified element from.
    * \param
    * index Zero-based index of the array element to retrieve.
    * \return
    * The function returns a handle to the \c index-th element of the specified array parameter. 
    * \remarks
    * Use this function to get the single elements of the specified array parameter.
    * You should use this function always in conjunction with 
    * CgFxEffect::getNumberOfArrayParameterElements to avoid passing an invalid element index
    * to the function. Passing an invalid index - that is, an index greater or equal to what
    * CgFxEffect::GetNumberOfArrayParameterElements returns - results in undefined runtime behavior.
    * \n\n
    * Calling this function for other than array parameters also results in undefined behavior.
    * Use CgFxEffect::isArrayParameter to determine whether the parameter is an array parameter.
    * \sa CgFxEffect::getArrayParameterElement, CgFxEffect::GetNumberOfArrayParameterElements
    */
  NVSG_API CgFxParameter getArrayParameterElement(CgFxParameter param, unsigned int index) const;

  /*! \brief Returns the number of members for the specifies struct parameter.
    * \param
    * param Handle to the struct parameter to query.
    * \return
    * The function returns the number of struct members
    * \remarks
    * This function in conjunction with CgFxEffect::getStructParameterMember supports
    * iterating through the members of a struct parameter. For direct access to a named struct
    * parameter it is recommend to use CgFxEffect::getStructParameterMemberByName instead.
    * \n\n
    * Calling this function for others than struct parameters results in undefined behavior.
    * Use CgFxEffect::isStructParameter to determine whether the parameter is a struct parameter.
    * \sa CgFxEffect::getStructParameterMember, CgFxEffect::getStructParameterMemberByName
    */
  NVSG_API unsigned int getNumberOfStructParameterMembers(CgFxParameter param) const;
  
  /*! \brief Returns the struct member at a specified zero-based index.
    * \param
    * param Handle to the struct parameter from which to retrieve the member.
    * \param
    * index Zero-based index of the struct member.
    * \return
    * The function returns a handle to the \c index-th struct member.
    * \remarks
    * Use this function together with CgFxEffect::getNumberOfStructParameterMembers to iterate
    * through a struct parameter's members. Note that the function does not guarantee to preserve
    * the order of struct members as specified in the struct definition. For direct access to a 
    * named struct parameter it is recommended to use CgFxEffect::getStructParameterMemberByName 
    * instead.
    * \n\n
    * Calling this function for others than struct parameters results in undefined behavior.
    * Use CgFxEffect::isStructParameter to determine whether the parameter is a struct parameter.
    * \n\n
    * Passing an invalid index to the function also results in undefined behavior. Use
    * CgFxEffect::getNumberOfStructParameterMembers to determine a valid range for the index to
    * pass.
    * \sa CgFxEffect::getNumberOfStructParameterMembers, CgFxEffect::getStructParameterMemberByName
    */
  NVSG_API CgFxParameter getStructParameterMember(CgFxParameter param, unsigned int index) const;

  /*! \brief Returns a struct member by name.
    * \param
    * param Handle to the struct parameter to retrieve the member from.
    * \param
    * member Name of the struct member to retrieve.
    * \return
    * The function returns a valid parameter handle to the to the struct member if the look-up was
    * successful. Otherwise, the function returns NULL to indicate that a member with the specified 
    * name does not exist for the struct parameter.
    * \remarks
    * The function case-sensitive looks up the struct member by name. Note that names of struct 
    * members need to be fully qualified to successfully look up the member!
    * \n\n
    * To retrieve the names of a struct parameter's members you can iterate through the members of a 
    * struct parameter using CgFxEffect::getNumberOfStructParameterMembers and 
    * CgFxEffect::getStructParameterMember, and then call CgFxEffect::getParameterName for each in 
    * turn.
    * \n\n
    * Calling this function for other than struct parameters results in undefined behavior.
    * Use CgFxEffect::isStructParameter to determine whether the parameter is a struct parameter.
    * \par Example:
    * If a struct type is defined as follows:
    * \code
    * struct FooStruct
    * {
    *     float4 a;
    *     float3 b;
    * };
    * \endcode
    * And you have defined a parameter of type \c FooStruct as follows:
    * \code
    * FooStruct foo;
    * \endcode
    *
    * The right way to look up the member 'b' would be to fully qualify its name:
    *
    * \code
    * CgFxParameter fooB = effect->getStructParameterMemberByName(param, "foo.b");
    * \endcode
    *
    * Equivalent for an array of FooStructs:
    *
    * \code
    * FooStruct bar[5];
    * \endcode
    *
    * The right way to look up the member 'b' of the first array element would be again to fully 
    * qualify its name:
    *
    * \code
    * CgFxParameter barB = effect->getStructParameterMemberByName(param, "bar[0].b");
    * \endcode
    *
    * \sa CgFxEffect::getNumberOfStructParameterMembers, CgFxEffect::getStructParameterMember
    */
  NVSG_API CgFxParameter getStructParameterMemberByName(CgFxParameter param, const std::string& member) const;

  /*! \brief Returns the size in bytes of the parameter value
    * \param
    * param Handle to the parameter to retrieve the value size from.
    * \return
    * The function returns the size in bytes of the parameter value.
    * \remarks
    * To get parameter values, the caller needs to provide a buffer large enough to receive 
    * the value. Use this function to determine the amount of bytes the parameter value consumes.
    * Alternatively you can use CgFxEffect::getNumberOfParameterValueComponents together with the
    * parameter's type information (e.g. CgFxEffect::isFloatParameter, or CgFxEffect::isIntParameter)
    * to determine what size the receiving buffer needs to be.
    * \n\n
    * Note that only leaf parameters have a value. For example, a struct parameter has no concrete
    * value (hence, CgFxEffect::getParameterValueSize would return 0) but the struct members, if
    * they are leaf parameters, have a value.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getNumberOfParameterValueComponents
    */
  NVSG_API unsigned int getParameterValueSize(CgFxParameter param) const;

  //! Returns the number of components the parameter value has.
  /** A parameter of type float4, for example, has 4 value components. */
  /*! \brief Returns the number of components the parameter value has.
    * \param
    * param Handle to the parameter to query
    * \return
    * The function returns the number of components the parameter value has.
    * A parameter of type float4, for example, has 4 value components. 
    * \remarks
    * This function makes sense for numerical parameters only, as they can have multiple components
    * (e.g. vectors or matrices). 
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getParameterValueSize
    */
  NVSG_API unsigned int getNumberOfParameterValueComponents(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a scalar-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a scalar-type parameter, that is -
    * a single component, numerical type. Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isScalarParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 2-component vector-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 2-component vector-type parameter, 
    * that is - a 2-component, numerical type. Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isVector2Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 3-component vector-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 3-component vector-type parameter, 
    * that is - a 3-component, numerical type. Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isVector3Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 4-component vector-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 4-component vector-type parameter, 
    * that is - a 4-component, numerical type. Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isVector4Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 2x2 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 2x2 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix22Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 2x3 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 2x3 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix23Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 2x4 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 2x4 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix24Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 3x2 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 3x2 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix32Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 3x3 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 3x3 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix33Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 3x4 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 3x4 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix34Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 4x2 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 4x2 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix42Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 4x3 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 4x3 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix43Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a 4x4 matrix-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a 4x4 matrix-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isMatrix44Parameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a float-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a float-type parameter (e.g.
    * float, float2, float3, float4x4, ...). Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isFloatParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a integer-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a integer-type parameter (e.g.
    * int, int2, int4x4, ...). Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isIntParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a boolean-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a integer-type parameter (e.g.
    * bool, bool2, bool4x4, ...). Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isBoolParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a string-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a string-type parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isStringParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a sampler-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a sampler-type parameter, that is -
    * either sampler1D, sampler2D, sampler3D, samplerRECT, samplerCUBE or samplerBUF. 
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture. 
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSamplerParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a sampler1D-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a sampler1D-type parameter. 
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture.
    * The 1D suffix for the sampler1D type indicates that the texture is a conventional 
    * one-dimensional texture.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSampler1DParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a sampler2D-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a sampler2D-type parameter. 
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture.
    * The 2D suffix for the sampler2D type indicates that the texture is a conventional 
    * two-dimensional texture.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSampler2DParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a sampler3D-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a sampler3D-type parameter. 
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture.
    * The 3D suffix for the sampler2D type indicates that the texture is a conventional 
    * three-dimensional texture.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSampler3DParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a samplerRECT-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a samplerRECT-type parameter. 
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture.
    * The RECT suffix for the samplerRECT type indicates that the texture is a  
    * rectangular (non-power-of-two) texture.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSamplerRectParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a samplerCUBE-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a samplerCUBE-type parameter. 
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture.
    * The CUBE suffix for the samplerCUBE type indicates that the texture is a cube map.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSamplerCubeParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a samplerBUF-type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a samplerBUF-type parameter. 
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture.
    * The BUF suffix for the samplerCUBE type indicates that the texture is a buffer.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSamplerBufParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a sampler array type parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a sampler array.
    * Otherwise, the function returns \c false.
    * \remarks
    * A sampler in Cg refers to an external object that Cg can sample, such as a texture.
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isSamplerArrayParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is part of a CgFxBuffer
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a buffered parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    */
  NVSG_API bool isBufferedParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a light parameter.
    * \param
    * param Parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a \ref lightparameters "light parameter".
    * Otherwise, the function returns \c false.
    * \remarks
    * \ref lightparameters fall into the category of \ref autoconnectedeffectparameters.
    */
  NVSG_API bool isLightParameter(CgFxParameter param) const;

  /*! \brief Returns wheteher the specified parameter represents a light position.
   * \param param Parameter to query.
   * \return
   * The function returns \c true if the specified parameter represents a light position.
   * \remarks
   * \ref lightpositionparameters fall into the category of \ref autoconnectedeffectparameters.
   */
  NVSG_API bool isLightPositionParameter(CgFxParameter param) const;

  /*! \brief Returns wheteher the specified parameter represents a light direction.
   * \param param Parameter to query.
   * \return
   * The function returns \c true if the specified parameter represents a light direction.
   * \remarks
   * \ref lightdirectionparameters fall into the category of \ref autoconnectedeffectparameters.
   */
  NVSG_API bool isLightDirectionParameter(CgFxParameter param) const;

  /*! \brief Returns wheteher the specified parameter represents a light type.
   * \param param Parameter to query.
   * \return
   * The function returns \c true if the specified parameter represents a light type.
   * \remarks
   * \ref lighttypeparameters fall into the category of \ref autoconnectedeffectparameters.
   */
  NVSG_API bool isLightTypeParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter specifies an array of light parameters.
    * \param param Parameter to query.
    * \return
    * The function returns \c true if the specified parameter specifies an array of 
    * \ref lightparameters. Otherwise, the function returns \c false.
    * \remarks
    * \ref lightparameters fall into the category of \ref autoconnectedeffectparameters.
    */
  NVSG_API bool isLightArrayParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter specifies a dynamic array of light parameters.
    * \param param Parameter to query.
    * \return
    * The function returns \c true if the specified parameter specifies a dynamic array of 
    * \ref lightparameters. Otherwise, the function returns \c false.
    * \remarks
    * \ref lightparameters fall into the category of \ref autoconnectedeffectparameters.
    */
  NVSG_API bool isDynamicLightArrayParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter represents a \ref materialparameter.
    * \param
    * param Parameter to query.
    * \return
    * The function returns \c true if \a param is a material parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * A \ref materialparameter falls into the category of \ref autoconnectedeffectparameters.
    */
  NVSG_API bool isMaterialParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a \ref sceneparameter.
    * \param
    * param Parameter to query.
    * \return
    * The function returns \c true if \a param represents a scene parameter.
    * Otherwise, the function returns \c false.
    * \remarks
    * A \ref sceneparameter falls into the category of \ref autoconnectedeffectparameters.
    */
  NVSG_API bool isSceneParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a \ref transformparameters "transform parameter".
    * \param
    * param Parameter to query.
    * \returns
    * The function returns \c true if the specified parameter is a transform parameter.
    * Otherwise the function returns \c false.
    * \remarks
    * A \ref transformparameters "transform parameter" falls into the category of 
    * \ref autoconnectedeffectparameters. 
    */
  NVSG_API bool isTransformParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is an array parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is an array parameter.
    * Otherwise, the function returns \c false.
    * \sa CgFxEffect::getNumberOfArrayParameterElements, CgFxEffect::getArrayParameterElement
    */
  NVSG_API bool isArrayParameter(CgFxParameter param) const;

  /*! \brief Returns whether the specified parameter is a struct parameter.
    * \param
    * param Handle to the parameter to query.
    * \return
    * The function returns \c true if the specified parameter is a struct parameter.
    * Otherwise the function returns \c false.
    * \sa CgFxEffect::getNumberOfStructParameterMembers, CgFxEffect::getStructParameterMember,
    * CgFxEffect::getStructParameterMemberByName
    */
  NVSG_API bool isStructParameter(CgFxParameter param) const;

  /*! \brief Returns the number of transform parameters available.
   * \return
   * The function returns the number transform parameters available for this effect.
   * \remarks
   * See the documentation on CgFxEffect::isTransformParameter for a more detailed description about
   * transform parameters.
   * \n\n
   * Use this function together with CgFxEffect::getTransform to iterate through all available
   * transform parameters of this effect.
   */
  NVSG_API unsigned int getNumberOfTransforms() const;

  /*! \brief Returns the transform parameter at the specified zero-based index.
    * \param
    * index Zero-based index of the transform parameter to retrieve.
    * \return
    * The function returns the \c index-th transform parameter.
    * \remarks
    * Use this function together with CgFxEffect::getNumberOfTransforms to iterate
    * through all \ref transformparameters that are used by the effect.
    * \par Usage example:
    * \code
    * void printTransformSemantics(const CgFxEffect * effect)
    * {
    *   unsigned int numTrafos = effect->getNumberOfTransforms();
    *   for ( unsigned int i=0; i<numTrafos; ++i )
    *   {
    *       CgFxParameter trafo = effect->getTransform(i);
    *       printf( "%s %s : %s\n"
    *             , effect->getParameterTypeString(trafo).c_str()
    *             , effect->getParameterName(trafo).c_str()
    *             , effect->getParameterSemantic(trafo).c_str() );
    *   }
    * }
    * \endcode
    * \sa
    */
  NVSG_API CgFxParameter getTransform(unsigned int index) const;

  /*! \brief Returns the transform type of the specified \ref transformparameters "transform parameter".
    * \param
    * param Transform parameter to query.
    * \return
    * The function returns a CgFxEffect::TransformType enum that indicates what kind of transform
    * the parameter specifies.
    * \remarks
    * The transform type of a transform parameter is tightly related to its semantic.
    * For example, a renderer needs this information to always upload the correct
    * state matrix to the underlying effect resource.
    * \n\n
    * The behaviour is undefined if this function is called for a non-transform parameter.    
    */
  NVSG_API TransformType getTransformType(CgFxParameter param) const;

  /*! \brief Uploads a 4x4 transform matrix to the underlying effect resource.
    * \param
    * param Handle to the transform parameter.
    * \param
    * value Pointer to the 16 32-bit float values to upload.
    * \return
    * The function return \c true if the matrix upload was successful.
    * Otherwise the function returns \c false.
    * \remarks
    * The function copies 16 32-bit float values from the buffer pointed to by \a value to the 
    * underlying effect resource.
    * \n\n
    * Calling this function for other than transform parameters results in undefined behavior.
    * Use CgFxEffect::isTransformParameter to test whether a certain parameter is a transform
    * parameter.
    * \n\n
    * The runtime behavior also is undefined if \c value specifies an invalid pointer.
    */
  NVSG_API bool setTransformValue(CgFxParameter param, const float * value);

  /*! \brief Returns the number of alterable effect parameters (so called 'tweakables')
    * \return
    * The function returns the number of tweakables available for this effect.
    * \remarks
    * Tweakables are all alterable effect parameters that will not be automatically updated by
    * the SceniX framework but need to be manually updated as wanted. For example, light or material 
    * parameters (see CgFxEffect::isLightParameter and CgFxEffect::isMaterialParameter) will 
    * be automatically updated by the SceniX framework every frame, and hence, are not recognized as 
    * tweakables. Same is true for transform parameters (see CgFxEffect::isTransformParameter).
    * \n\n
    * An application can offer a graphical user interface that allows for manipulating the 
    * tweakable values manually.
    */
  NVSG_API unsigned int getNumberOfTweakables() const;

  /*! \brief Returns the tweakable at the specified zero-based index.
    * \param
    * index Zero-based index of the tweakable to retrieve.
    * \return
    * The function returns a valid handle to the \c index-th tweakable.
    * \remarks
    * This function is particularly useful when iterating through all tweakables available for this
    * effect.
    * \n\n
    * Passing an invalid index results in undefined behavior. Use CgFxEffect::getNumberOfTweakables
    * to determine a valid range for the indices to pass.
    */
  NVSG_API CgFxParameter getTweakable(unsigned int index) const;

  /*! \brief Returns a tweakable effect parameter by semantic.
    * \param
    * semantic Semantic string used to look up the tweakable.
    * \return
    * The function returns a valid handle to the tweakable with the specified semantic, if found.
    * Otherwise, the function returns NULL, to indicate that a tweakable with that semantic does
    * not exist.
    * \remarks
    * A tweakable can be directly accessed by its semantic.
    * This function case-sensitive looks up the tweakable by it's semantic string.
    */
  NVSG_API CgFxParameter getTweakableBySemantic(const std::string& semantic) const;

  /*! \brief Returns a tweakable effect parameter by name.
    * \param
    * name Name string used to look up the tweakable.
    * \return
    * The function returns a valid handle to the tweakable with the specified name, if found.
    * Otherwise, the function returns NULL, to indicate that a tweakable with that name does
    * not exist.
    * \remarks
    * A tweakable can be directly accessed by its name.
    * This function case-sensitive looks up the tweakable by it's name string.
    */
  NVSG_API CgFxParameter getTweakableByName(const std::string& name) const;

  /*! \brief Returns the number of NVSGLight-typed parameters defined for the effect
    * \return
    * The function returns the number of NVSGLight parameters available.
    * \remarks
    * Use this function together with CgFxEffect::getLight to iterate through all 
    * available effect lights. 
    * \n\n
    * Parameters are considered \ref lightparameters if they are declared as NVSGLight
    * parameters in the effect source.
    */
  NVSG_API unsigned int getNumberOfLights() const;

  /*! \brief Returns the light parameter at the indicated index.
   * \return
   * The function returns the light parameter at the indicated zero-based index.
   * \remarks
   * Use this function together with CgFxEffect::getNumberOfLights to iterate through
   * all light parameters declared in the effect. \ref lightparameters will be
   * auto-connected to tree lights, and they will be updated automatically by SceniX's
   * render framework. If you derive your own renderer, and you plan to also update (connect)
   * light parameters as your renderer gets applied, you should exclusively use the 
   * dedicated setLight member functions for this task.
   */
  NVSG_API CgFxParameter getLight(unsigned int index) const;

  /*! \brief Returns a light parameter by semantic.
    * \param
    * semantic Semantic string used to look up the light parameter.
    * \return
    * The function returns a valid handle to the \ref lightparameters "light parameter" with the 
    * specified semantic, if found. Otherwise, the function returns NULL, to indicate that a light 
    * parameter with that semantic does not exist.
    */
  NVSG_API CgFxParameter getLightBySemantic(const std::string& semantic) const;
  
  /*! \brief Returns a light parameter by name.
   * \param
   * name Name string used to look up the light parameter.
   * \return
   * The function returns a valid handle to the \ref lightparameters "light parameter" with the 
   * specified name, if found. Otherwise, the function returns NULL, to indicate that a light 
   * parameter with that name does not exist.
   */
  NVSG_API CgFxParameter getLightByName(const std::string& name) const;

  /*! \brief Returns whether this effect contains a dynamically sized light array
   * \return \c true, if the effect contains a dynamically sized light array, \c false otherwise.
   * \remarks An effect can either contain a dynamically sized light array or a number of normal lights and light arrays.
   */
  NVSG_API bool hasDynamicNumberOfLights() const;

  /*! \brief Set the desired number of lights of a dynamically sized light array
   * \param num The number of lights the effect should have
   * \remarks An effect can either contain a dynamically sized light array or a number of normal lights and light arrays.
   * If the \c num is equal to the current number of lights, the function has no effect.
   */
  NVSG_API void setDynamicNumberOfLights(unsigned int num);

  /*! \brief Get the current number of lights
   * \return The current number of lights, if the effect contains a dynamic array of lights.
   */
  NVSG_API unsigned int getDynamicNumberOfLights() const;

  /*! \brief Returns the effects \ref materialparameter if available.
    * \return
    * The function returns a valid handle to the material parameter, if available.
    * Otherwise, the function returns NULL, to indicate that a material parameter is not available
    * for this effect.
    * \remarks
    * A \ref materialparameter falls into the category of \ref autoconnectedeffectparameters
    * \sa nvsg::Material, CgFxEffect::isMaterialParameter
    */
  NVSG_API CgFxParameter getMaterial() const;

  /*! \brief Returns the effect's \ref sceneparameter
    * \return
    * The function returns a valid handle to the NVSGScene parameter, if available.
    * Otherwise, the function returns NULL, to indicate that a scene parameter is not available
    * for this effect.  Note that this is NOT the nvsg::Scene, but the 
    * NVSGScene data structure that may be defined in the effect!
    * \sa nvsg::Scene, CgFxEffect::isSceneParameter
    */
  NVSG_API CgFxParameter getScene() const;

  /*! \brief Returns a numeric-type parameter value as 32-bit float values.
    * \param
    * param Handle to the parameter to retrieve the value from.
    * \param 
    * value Pointer to the buffer that receives the float values.
    * \return
    * The function returns \c true if the value could be retrieved successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * This function lets you get a numerical parameter's value as float value regardless of what
    * the real type of the numerical parameter is. This could save a lot of type-testing on numerical 
    * parameters where applicable. 
    * \n\n
    * The function returns the value in the buffer pointed to by value. The number of floats that 
    * will be copied to \a value corresponds to the number of value components for the specified 
    * numeric-type parameter. The caller must provide a buffer large enough to hold 
    * all the value components. Use CgFxEffect::getNumberOfParameterValueComponents to determine 
    * the required size for the output buffer. Note that the behavior is undefined if the buffer 
    * provided is not large enough to hold all the value components!
    * \n\n
    * The function has no effect if called for a non-numeric-type parameter!
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getNumberOfParameterValueComponents
    */
  NVSG_API bool getParameterValueAsFloat(CgFxParameter param, float * value) const;

  /*! \brief Sets the value of a numeric-type parameter as 32-bit float value.
    * \param
    * param Handle to the parameter for which to set the value.
    * \param
    * value Pointer to the buffer from which to read the new values.
    * \return
    * The function returns \c true if the parameter value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * The function lets you set the value for an arbitrary numerical parameter as 32-bit floating
    * point numbers. This probably could save you a lot of type-checking where applicable.
    * \n\n
    * The function uploads the 32-bit float values from the buffer pointed to by \a value to the 
    * underlying effect resource.
    * \n\n
    * The number of floats that will be copied from \a value corresponds to the number of value 
    * components for the specified numeric-type parameter.
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine how many components the
    * specified parameter has.
    * \n\n
    * The function has no effect if called for a non-numeric-type parameter!    
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getNumberOfParameterValueComponents
    */
  NVSG_API bool setParameterValueAsFloat(CgFxParameter param, const float * value);

  /*! \brief Returns the value of a float-type parameter.
    * \param
    * param Handle to the parameter from which to retrieve the value.
    * \param 
    * value Pointer to a buffer that receives the value.
    * \return
    * The function returns \c true if the value could be retrieved successfully.
    * Otherwise, the function returns false.
    * \remarks
    * Use this function to retrieve the value of a float-type parameter. 
    * The function returns \c false if called for parameters other than float-type parameters to
    * indicate that no values have been copied to the output buffer.
    * Use CgFxEffect::isFloatParameter to test whether a certain parameter is a float-type parameter.
    * \n\n
    * The number of floats that will be copied to \a value corresponds to the number of value 
    * components for the specified float-type parameter. The caller must provide a 
    * buffer large enough to hold all the value components. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the required size for the 
    * output buffer. Note that the behavior is undefined if the buffer provided is not large 
    * enough to hold all the value components!
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::setFloatParameterValue
    */
  NVSG_API bool getFloatParameterValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the value of a float-type parameter.
    * \param
    * param Handle to the parameter for which to set the value.
    * \param
    * value Pointer to a buffer from which to read the new value.
    * \return
    * The function returns \c true if the parameter value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to update the value of a float-type parameter. 
    * The function returns \c false if called for parameters other than float-type parameters to
    * indicate that no values have been updated.
    * Use CgFxEffect::isFloatParameter to test whether a certain parameter is a float-type parameter.
    * \n\n
    * The number of floats that will be copied from \a value corresponds to the number of value 
    * components for the specified float-type parameter. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the number of value 
    * components for the specified parameter.
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \note
    * This function stores the parameter value in an internal flush list, and it's flushed with the
    * next call to applyPass.
    * \sa CgFxEffect::getFloatParameterValue, CgFxEffect::flushFloatParameterValue
    */
  NVSG_API bool setFloatParameterValue(CgFxParameter param, const float* value);

  /*! \brief Flush the value of a float-type parameter.
    * \param
    * param Handle to the parameter for which to set the value.
    * \param
    * value Pointer to a buffer from which to read the new value.
    * \return
    * The function returns \c true if the parameter value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to immediatly update the value of a float-type parameter. 
    * The function returns \c false if called for parameters other than float-type parameters to
    * indicate that no values have been updated.
    * Use CgFxEffect::isFloatParameter to test whether a certain parameter is a float-type parameter.
    * \n\n
    * The number of floats that will be copied from \a value corresponds to the number of value 
    * components for the specified float-type parameter. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the number of value 
    * components for the specified parameter.
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \note
    * This function directly flushes the parameter value. Its value is not stored locally for later
    * flushes during calls to applyPass. If that same CgFxParameter is also set via setFloatParameterValue
    * and thus locally stored, that locally stored value is flushed with the next applyPass.
    * \sa CgFxEffect::getFloatParameterValue, CgFxEffect::setFloatParameterValue
    */
  NVSG_API bool flushFloatParameterValue(CgFxParameter param, const float* value);

  /*! \brief Returns the value of a integer-type parameter.
    * \param
    * param Handle to the parameter from which to retrieve the value.
    * \param 
    * value Pointer to a buffer that receives the value.
    * \return
    * The function returns \c true if the value could be retrieved successfully.
    * Otherwise, the function returns false.
    * \remarks
    * Use this function to retrieve the value of an integer-type parameter. 
    * The function returns \c false if called for parameters other than integer-type parameters to
    * indicate that no values have been copied to the output buffer.
    * Use CgFxEffect::isIntParameter to test whether a certain parameter is a integer-type parameter.
    * \n\n
    * The number of integers that will be copied to \a value corresponds to the number of value 
    * components for the specified integer-type parameter. The caller must provide a 
    * buffer large enough to hold all the value components. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the required size for the 
    * output buffer. Note that the behavior is undefined if the buffer provided is not large 
    * enough to hold all the value components!
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::setIntParameterValue
    */
  NVSG_API bool getIntParameterValue(CgFxParameter param, int * value) const;

  /*! \brief Sets the value of a integer-type parameter.
    * \param
    * param Handle to the parameter for which to set the value.
    * \param
    * value Pointer to a buffer from which to read the new value.
    * \return
    * The function returns \c true if the parameter value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to update the value of an integer-type parameter. 
    * The function returns \c false if called for parameters other than integer-type parameters to
    * indicate that no values have been updated.
    * Use CgFxEffect::isIntParameter to test whether a certain parameter is a integer-type parameter.
    * \n\n
    * The number of integers that will be copied from \a value corresponds to the number of value 
    * components for the specified integer-type parameter. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the number of value 
    * components for the specified parameter.
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \note
    * This function stores the parameter value in an internal flush list, and it's flushed with the
    * next call to applyPass.
    * \sa CgFxEffect::getIntParameterValue, CgFxEffect::flushIntParameterValue
    */
  NVSG_API bool setIntParameterValue(CgFxParameter param, const int* value);

  /*! \brief Flush the value of an integer-type parameter.
    * \param
    * param Handle to the parameter for which to set the value.
    * \param
    * value Pointer to a buffer from which to read the new value.
    * \return
    * The function returns \c true if the parameter value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to immediatly update the value of an integer-type parameter. 
    * The function returns \c false if called for parameters other than integer-type parameters to
    * indicate that no values have been updated.
    * Use CgFxEffect::isIntParameter to test whether a certain parameter is an integer-type parameter.
    * \n\n
    * The number of integers that will be copied from \a value corresponds to the number of value 
    * components for the specified integer-type parameter. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the number of value 
    * components for the specified parameter.
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \note
    * This function directly flushes the parameter value. Its value is not stored locally for later
    * flushes during calls to applyPass. If that same CgFxParameter is also set via setIntParameterValue
    * and thus locally stored, that locally stored value is flushed with the next applyPass.
    * \sa CgFxEffect::getIntParameterValue, CgFxEffect::setIntParameterValue
    */
  NVSG_API bool flushIntParameterValue(CgFxParameter param, const int* value);

  /*! \brief Returns the value of a 32-bit boolean-type parameter.
    * \param
    * param Handle to the parameter from which to retrieve the value.
    * \param 
    * value Pointer to a buffer that receives the value.
    * \return
    * The function returns \c true if the value could be retrieved successfully.
    * Otherwise, the function returns false.
    * \remarks
    * Use this function to retrieve the value of a boolean-type parameter. 
    * The function returns \c false if called for parameters other than boolean-type parameters to
    * indicate that no values have been copied to the output buffer.
    * Use CgFxEffect::isBoolParameter to test whether a certain parameter is a boolean-type 
    * parameter.
    * \n\n
    * The number of 32-bit booleans that will be copied to \a value corresponds to the number of 
    * value components for the specified boolean-type parameter. The caller is responsible for  
    * providing a buffer large enough to hold all the value components. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the required size for the 
    * output buffer. Note that the behavior is undefined if the buffer provided is not large 
    * enough to hold all the value components!
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::setBoolParameterValue
    */
  NVSG_API bool getBoolParameterValue(CgFxParameter param, int * value) const;

  /*! \brief Sets the value of a 32-bit boolean-type parameter.
    * \param
    * param Handle to the parameter for which to set the value.
    * \param
    * value Pointer to a buffer from which to read the new value.
    * \return
    * The function returns \c true if the parameter value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to update the value of a boolean-type parameter. 
    * The function returns \c false if called for parameters other than boolean-type parameters to
    * indicate that no values have been updated.
    * Use CgFxEffect::isBoolParameter to test whether a certain parameter is a boolean-type 
    * parameter.
    * \n\n
    * The number of 32-bit booleans that will be copied from \a value corresponds to the number of 
    * value components for the specified boolean-type parameter. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the number of value 
    * components for the specified parameter.
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \note
    * This function stores the parameter value in an internal flush list, and it's flushed with the
    * next call to applyPass.
    * \sa CgFxEffect::getBoolParameterValue, CgFxEffect::flushBoolParameterValue
    */
  NVSG_API bool setBoolParameterValue(CgFxParameter param, const int* value);

  /*! \brief Flush the value of a 32-bit boolean-type parameter.
    * \param
    * param Handle to the parameter for which to set the value.
    * \param
    * value Pointer to a buffer from which to read the new value.
    * \return
    * The function returns \c true if the parameter value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to update the value of a boolean-type parameter. 
    * The function returns \c false if called for parameters other than boolean-type parameters to
    * indicate that no values have been updated.
    * Use CgFxEffect::isBoolParameter to test whether a certain parameter is a boolean-type 
    * parameter.
    * \n\n
    * The number of 32-bit booleans that will be copied from \a value corresponds to the number of 
    * value components for the specified boolean-type parameter. 
    * Use CgFxEffect::getNumberOfParameterValueComponents to determine the number of value 
    * components for the specified parameter.
    * \n\n   
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \note
    * This function directly flushes the parameter value. Its value is not stored locally for later
    * flushes during calls to applyPass. If that same CgFxParameter is also set via setBoolParameterValue
    * and thus locally stored, that locally stored value is flushed with the next applyPass.
    * \sa CgFxEffect::getBoolParameterValue, CgFxEffect::setBoolParameterValue
    */
  NVSG_API bool flushBoolParameterValue(CgFxParameter param, const int* value);

  /*! \brief Returns the string value of a string-type parameter.
    * \param
    * param Handle to the string-type parameter to query.
    * \param
    * value Reference to a STL string that receives the string value. 
    * \return
    * The function returns \c true if the string could have been retrieved successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Use this function to retrieve the string from a string-type parameter.
    * The function returns \c false if called for other parameters than string-type parameters to
    * indicate that no string could have been retrieved.
    * Use CgFxEffect::isStringParameter to test whether a certain parameter is a string-type
    * parameter.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::setStringParameterValue
    */
  NVSG_API bool getStringParameterValue(CgFxParameter param, std::string& value) const;

  /*! \brief Sets the string value for a string-type parameter.
    * \param
    * param Handle to the string-type parameter for which to set the string.
    * \param
    * value Reference to a STL string that holds the string to set.
    * \return
    * Use this function to update the string value of a string-type parameter.
    * The function returns \c false if called for parameters other than string-type parameters to
    * indicate that the parameter's string value has not been updated.
    * Use CgFxEffect::isStringParameter to test whether a certain parameter is a string-type 
    * parameter.
    * \n\n
    * Calling this function for an invalid parameter handle results in undefined runtime behavior.
    * You can use CgFxEffect::getTweakable, CgFxEffect::getParameterByName, or 
    * CgFxEffect::getParameterBySemantic to get a valid handle to a certain effect parameter.
    * \sa CgFxEffect::getStringParameterValue
    */
  NVSG_API bool setStringParameterValue(CgFxParameter param, const std::string& value);

  /*! \brief Resets the specified Scene to default.
    * \param
    * param Handle to the Scene to reset
    * \remarks
    * Calling this function for other than Scene parameters results in undefined behavior.
    * Use CgFxEffect::isSceneParameter to test whether a certain parameter is a Scene
    * parameter.
    */
  NVSG_API void resetSceneValues(CgFxParameter param);

  /*! \brief Returns the ambient RGBA intensity of the entire scene.
    * \param
    * param Handle to the Scene parameter.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the 
    * ambient RGBA intensity.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Typically, this value will track the Scene::Ambient color automatically.
    * Calling this function for other than Scene parameters results in undefined behavior.
    * Use CgFxEffect::isSceneParameter to test whether a certain parameter specifies a Scene
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating point values.
    * \sa CgFxEffect::setSceneAmbientValue
    * \sa Scene::setAmbientColor Scene::getAmbientColor
    */
  NVSG_API bool getSceneAmbientValue(CgFxParameter param, float * value) const;   // float4

  /*! \brief Sets the ambient RGBA intensity of the entire scene.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer to read the input values from.
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Typically, this value will track the Scene::Ambient color automatically.
    * Calling this function for other than Scene parameters results in undefined behavior.
    * Use CgFxEffect::isSceneParameter to test whether a certain parameter specifies a Scene
    * parameter.
    * \sa CgFxEffect::getSceneAmbientValue
    * \sa Scene::setAmbientColor Scene::getAmbientColor
    */
  NVSG_API bool setSceneAmbientValue(CgFxParameter param, const float * value);   // float4

  /*! \brief Returns the scene frame number.
    * \param
    * param Handle to the Scene parameter.
    * \param 
    * value Pointer to the buffer that receives the unsigned int
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Typically, this value will track the number of times the scene has been
    * rendered automatically.
    * Calling this function for other than Scene parameters results in undefined behavior.
    * Use CgFxEffect::isSceneParameter to test whether a certain parameter specifies a Scene
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold a single 32-bit integer
    * See notes in Scene::setFrameNumber for range issues with this value.
    * \sa CgFxEffect::setSceneFrameNumberValue
    * \sa Scene::setFrameNumber Scene::getFrameNumber
    */
  NVSG_API bool getSceneFrameNumberValue(CgFxParameter param, int * value) const; 

  /*! \brief Sets the current frame number.
    * \param
    * param Handle to the scene parameter.
    * \param 
    * value Pointer to the buffer to read the input values from.
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Typically, this value will track the Scene::FrameNumber automatically.
    * Calling this function for other than Scene parameters results in undefined behavior.
    * Use CgFxEffect::isSceneParameter to test whether a certain parameter specifies a Scene
    * parameter.
    * See notes in Scene::setFrameNumber for range issues with this value.
    * \sa CgFxEffect::getSceneFrameNumberValue
    * \sa Scene::setFrameNumber Scene::getFrameNumber
    */
  NVSG_API bool setSceneFrameNumberValue(CgFxParameter param, const int * value);  


  /*! \brief Resets the specified material to default.
    * \param
    * param Handle to the material to reset
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter is a material
    * parameter.
    */
  NVSG_API void resetMaterialValues(CgFxParameter param);

  /*! \brief Returns the RGBA emitted light intensity of the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the RGBA
    * emitted light intensity.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating point values.
    * \sa CgFxEffect::setMaterialEmissionValue
    */
  NVSG_API bool getMaterialEmissionValue(CgFxParameter param, float * value) const;  // float4

  /*! \brief Sets the RGBA emitted light intensity for the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer to read the input values from.
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \sa CgFxEffect::getMaterialEmissionValue
    */
  NVSG_API bool setMaterialEmissionValue(CgFxParameter param, const float * value);  // float4

  /*! \brief Returns the ambient RGBA reflectance of the specified material.
    * \param
    * param Handle to the material parameter.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the 
    * ambient RGBA reflectance values.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating point values.
    * \sa CgFxEffect::setMaterialAmbientValue
    */
  NVSG_API bool getMaterialAmbientValue(CgFxParameter param, float * value) const;   // float4

  /*! \brief Sets the ambient RGBA reflectance for the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer to read the input values from.
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \sa CgFxEffect::getMaterialAmbientValue
    */
  NVSG_API bool setMaterialAmbientValue(CgFxParameter param, const float * value);   // float4
  
  /*! \brief Returns the diffuse RGBA reflectance of the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the 
    * diffuse RGBA reflectance values.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating point values.
    * \sa CgFxEffect::setMaterialDiffuseValue
    */
  NVSG_API bool getMaterialDiffuseValue(CgFxParameter param, float * value) const;   // float4

  /*! \brief Sets the diffuse RGBA reflectance for the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer to read the input values from.
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \sa CgFxEffect::getMaterialDiffuseValue
    */
  NVSG_API bool setMaterialDiffuseValue(CgFxParameter param, const float * value);   // float4

  /*! \brief Returns the specular RGBA reflectance of the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the 
    * specular RGBA reflectance values.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating point values.
    * \sa CgFxEffect::setMaterialSpecularValue
    */
  NVSG_API bool getMaterialSpecularValue(CgFxParameter param, float * value) const;  // float4

  /*! \brief Sets the specular RGBA reflectance for the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to the buffer to read the input values from.
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \sa CgFxEffect::getMaterialSpecularValue
    */
  NVSG_API bool setMaterialSpecularValue(CgFxParameter param, const float * value);  // float4
  
  /*! \brief Returns the RGBA specular exponent of the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to a single float value that represents the specular exponent.
    * \return
    * The function returns \c true if the requested value could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \n\n
    * The behaviour also is undefined if \a value specifies an invalid pointer.
    * \sa CgFxEffect::setMaterialShininessValue
    */
  NVSG_API bool getMaterialShininessValue(CgFxParameter param, float * value) const; // float 

  /*! \brief Sets the RGBA specular exponent for the specified material.
    * \param
    * param Handle to the material.
    * \param 
    * value Pointer to a single float to read the input value from.
    * \return
    * The function returns \c true if the value could be updated successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than material parameters results in undefined behavior.
    * Use CgFxEffect::isMaterialParameter to test whether a certain parameter specifies a material
    * parameter.
    * \sa CgFxEffect::getMaterialShininessValue
    */
  NVSG_API bool setMaterialShininessValue(CgFxParameter param, const float * value); // float 

  /*! \brief Resets the values for the specified light to default.
    * \param
    * param Handle to the light parameter to reset
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter is a material
    * parameter.
    */
  NVSG_API void resetLightValues(CgFxParameter param);

  /*! \brief Returns the ambient RGBA intensity of the specified light.
    * \param
    * param Handle to the light.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the 
    * ambient RGBA intensity values.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating point values.
    * \sa CgFxEffect::setLightAmbientValue
    */
  NVSG_API bool getLightAmbientValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the ambient RGBA intensity for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to the buffer to read the input values from. 
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightAmbientValue
    */
  NVSG_API bool setLightAmbientValue(CgFxParameter param, const float * value);

  /*! \brief Returns the diffuse RGBA intensity of the specified light.
    * \param
    * param Handle to the light.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the 
    * diffuse RGBA intensity values.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating point values.
    * \sa CgFxEffect::setLightDiffuseValue
    */
  NVSG_API bool getLightDiffuseValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the diffuse RGBA intensity for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to the buffer to read the input values from. 
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightDiffuseValue
    */
  NVSG_API bool setLightDiffuseValue(CgFxParameter param, const float * value);

  /*! \brief Returns the specular RGBA intensity of the specified light.
    * \param
    * param Handle to the light.
    * \param 
    * value Pointer to the buffer that receives four floating-point values that represent the 
    * specular RGBA intensity values.
    * \return
    * The function returns \c true if the requested values could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating-point values.
    * \sa CgFxEffect::setLightSpecularValue
    */
  NVSG_API bool getLightSpecularValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the specular RGBA intensity for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to the buffer to read the input values from. 
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightSpecularValue
    */
  NVSG_API bool setLightSpecularValue(CgFxParameter param, const float * value);
  
  /*! \brief Returns the position of the specified light.
    * \param
    * param Handle to the light
    * \param
    * value Pointer to the buffer that receives four floating-point values that represent the
    * light position in homogeneous world coordinates.
    * \return
    * The function returns \c true if the light position could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating-point values.
    * \sa CgFxEffect::setLightPositionValue
    */
  NVSG_API bool getLightPositionValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the position for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to a buffer of four floating-point values that represents the new light 
    * position in homogeneous world coordinates. 
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightPositionValue
    */
  NVSG_API bool setLightPositionValue(CgFxParameter param, const float * value);

  /*! \brief Returns the direction of the specified light.
    * \param
    * param Handle to the light
    * \param
    * value Pointer to the buffer that receives four floating-point values that represent the
    * light direction in homogeneous world coordinates.
    * \return
    * The function returns \c true if the light direction could be retrieved successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if the output buffer pointed to by \a value is not large 
    * enough to hold four 32-bit floating-point values.
    * \sa CgFxEffect::setLightDirectionValue
    */
  NVSG_API bool getLightDirectionValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the direction for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to a buffer of four floating-point values that represents the new light 
    * direction in homogeneous world coordinates. 
    * \return
    * The function returns \c true if the values could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightDirectionValue
    */
  NVSG_API bool setLightDirectionValue(CgFxParameter param, const float * value);
  
  /*! \brief Returns the intensity distribution of the specified light.
    * \param
    * param Handle to the light
    * \param
    * value Pointer to the buffer that receives a single floating-point value that represents the
    * intensity distribution of the light.
    * \return
    * The function returns \c true if the intensity distribution could be retrieved 
    * successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if \a value specifies an invalid pointer.
    * \sa CgFxEffect::setLightSpotExponentValue
    */
  NVSG_API bool getLightSpotExponentValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the intensity distribution for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to a single float value that represents the new intensity distribution. 
    * \return
    * The function returns \c true if the value could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightSpotExponentValue
    */
  NVSG_API bool setLightSpotExponentValue(CgFxParameter param, const float * value);
  
  /*! \brief Returns the maximum spread angle of the specified light.
    * \param
    * param Handle to the light
    * \param
    * value Pointer to the buffer that receives a single floating-point value that represents the
    * maximum spread angle of the light.
    * \return
    * The function returns \c true if the maximum spread angle could be retrieved 
    * successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if \a value specifies an invalid pointer.
    * \sa CgFxEffect::setLightPosition
    */
  NVSG_API bool getLightSpotCutoffValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the maximum spread angle for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to a single float value that represents the new maximum spread angle. 
    * \return
    * The function returns \c true if the value could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightSpotCutoffValue
    */
  NVSG_API bool setLightSpotCutoffValue(CgFxParameter param, const float * value);

  /*! \brief Returns the constant attenuation factor of the specified light.
    * \param
    * param Handle to the light
    * \param
    * value Pointer to the buffer that receives a single floating-point value that represents the
    * constant attenuation factor of the light.
    * \return
    * The function returns \c true if the constant attenuation factor could be retrieved 
    * successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if \a value specifies an invalid pointer.
    * \sa CgFxEffect::setLightPosition
    */
  NVSG_API bool getLightConstantAttenuationValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the constant attenuation factor for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to a single float value that represents the constant attenuation factor. 
    * \return
    * The function returns \c true if the value could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightConstantAttenuationValue
    */
  NVSG_API bool setLightConstantAttenuationValue(CgFxParameter param, const float * value);

  /*! \brief Returns the linear attenuation factor of the specified light.
    * \param
    * param Handle to the light
    * \param
    * value Pointer to the buffer that receives a single floating-point value that represents the
    * linear attenuation factor of the light.
    * \return
    * The function returns \c true if the linear attenuation factor could be retrieved 
    * successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if \a value specifies an invalid pointer.
    * \sa CgFxEffect::setLightPosition
    */
  NVSG_API bool getLightLinearAttenuationValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the linear attenuation factor for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to a single float value that represents the linear attenuation factor. 
    * \return
    * The function returns \c true if the value could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightLinearAttenuationValue
    */
  NVSG_API bool setLightLinearAttenuationValue(CgFxParameter param, const float * value);

  /*! \brief Returns the quadratic attenuation factor of the specified light.
    * \param
    * param Handle to the light
    * \param
    * value Pointer to the buffer that receives a single floating-point value that represents the
    * quadratic attenuation factor of the light.
    * \return
    * The function returns \c true if the quadratic attenuation factor could be retrieved 
    * successfully.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \n\n
    * The behaviour also is undefined if \a value specifies an invalid pointer.
    * \sa CgFxEffect::setLightPosition
    */
  NVSG_API bool getLightQuadraticAttenuationValue(CgFxParameter param, float * value) const;

  /*! \brief Sets the quadratic attenuation factor for the specified light.
    * \param
    * param Handle to the light.
    * \param
    * value Pointer to a single float value that represents the quadratic attenuation factor. 
    * \return
    * The function returns \c true if the value could be updated successfully.
    * Otherwise the function returns \c false.
    * \remarks
    * Calling this function for other than light parameters results in undefined behavior.
    * Use CgFxEffect::isLightParameter to test whether a certain parameter specifies a light
    * parameter.
    * \sa CgFxEffect::getLightQuadraticAttenuationValue
    */
  NVSG_API bool setLightQuadraticAttenuationValue(CgFxParameter param, const float * value);

  /*! \brief Returns the position parameter for the i-th light.
   * \return 
   * The function returns a valid parameter handle to the position parameter of the
   * i-th light. If no such parameter was declared in the effect, the function returns
   * NULL.
   * \remarks
   * \ref lightpositionparameters fall into the category of \ref autoconnectedeffectparameters
   */ 
  NVSG_API CgFxParameter getMslLightPosition(unsigned int i) const;

  /*! \brief Returns the direction parameter for the i-th light.
   * \return 
   * The function returns a valid parameter handle to the direction parameter of the
   * i-th light. If no such parameter was declared in the effect, the function returns
   * NULL.
   * \remarks
   * \ref lightdirectionparameters fall into the category of \ref autoconnectedeffectparameters
   */ 
  NVSG_API CgFxParameter getMslLightDirection(unsigned int i) const;

  /*! \brief Returns the type parameter for the i-th light.
   * \return 
   * The function returns a valid parameter handle to the type parameter of the
   * i-th light. If no such parameter was declared in the effect, the function returns
   * NULL.
   * \remarks
   * \ref lighttypeparameters fall into the category of \ref autoconnectedeffectparameters
   */ 
  NVSG_API CgFxParameter getMslLightType(unsigned int i) const;

  /*! \brief Retrieves the value of the indicated light position parameter.
   * \param param Parameter representing the light position.
   * \param value Start address of a buffer of floats to retrieve the value. 
   * The buffer must be large enough to hold 3 floats.
   * \returns The function returns \c true if the value was successfully retrieved.
   * Otherwise the function returns \c false.
   * \sa \ref lightpositionparameters
   */
  NVSG_API bool getMslLightPositionValue(CgFxParameter param, float * value) const;

  /*! \brief Retrieves the value of the indicated light direction parameter.
   * \param param Parameter representing the light direction.
   * \param value Start address of a buffer of floats to retrieve the value. 
   * The buffer must be large enough to hold 3 floats.
   * \returns The function returns \c true if the value was successfully retrieved.
   * Otherwise the function returns \c false.
   * \sa \ref lightdirectionparameters
   */
  NVSG_API bool getMslLightDirectionValue(CgFxParameter param, float * value) const;

  /*! \brief Retrieves the value of the indicated light type parameter.
   * \param param Parameter representing the light type.
   * \param value Start address of an integer buffer to retrieve the value. 
   * \returns The function returns \c true if the value was successfully retrieved.
   * Otherwise the function returns \c false.
   * \sa \ref lighttypeparameters
   */
  NVSG_API bool getMslLightTypeValue(CgFxParameter param, int * value) const;

  /*! \brief Sets the value for the indicated light position parameter
   * \param param Light position parameter for which to set the value.
   * \param value Source buffer from which the value will be read.
   * \return The function returns \c true.
   * \sa \ref lightpositionparameters
   */
  NVSG_API bool setMslLightPositionValue(CgFxParameter param, const float * value);

  /*! \brief Sets the value for the indicated light direction parameter
   * \param param Light direction parameter for which to set the value.
   * \param value Source buffer from which the value will be read.
   * \return The function returns \c true.
   * \sa \ref lightdirectionparameters
   */
  NVSG_API bool setMslLightDirectionValue(CgFxParameter param, const float * value);

  /*! \brief Sets the value for the indicated light type parameter
   * \param param Light type parameter for which to set the value.
   * \param value Source buffer from which the value will be read.
   * \return The function returns \c true.
   * \sa \ref lighttypeparameters
   */
  NVSG_API bool setMslLightTypeValue(CgFxParameter param, int value);

  /*! \brief Returns the number of sampler available.
    * \return
    * The function returns the number of samplers available for this effect.
    * \remarks
    * This function in conjunction with CgFxEffect::getSampler is useful for iterating through
    * the effect samplers.
    */
  NVSG_API unsigned int getNumberOfSamplers() const;

  /*! \brief Returns the sampler at the specified zero-based index.
    * \param
    * index Zero-based index of the sampler.
    * \return
    * The function returns a handle to the requested sampler.
    * \remarks
    * Passing an invalid index, that is - an index greater or equal to what 
    * CgFxEffect::getNumberOfSamplers returns results in undefined behavior.
    */
  NVSG_API CgFxParameter getSampler(unsigned int index) const;

  /*! \brief Returns a sampler parameter by name.
    * \param
    * name Name of the parameter to get.
    * \return
    * The function returns a handle to the sampler if found by name.
    * Otherwise, the function returns NULL to indicate that a sampler with the specified name does 
    * not exist.
    * \remarks
    * A sampler can be directly accessed by name using this function.
    * The function case-sensitive looks up the sampler parameter by name.
    */
  NVSG_API CgFxParameter getSamplerByName(const std::string& name) const;

  /*! \brief Returns a sampler parameter by the name of its associated texture parameter.
    * \param
    * name Name of the associated texture parameter.
    * \return
    * The function returns a handle to the requested sampler if the lookup was successful.
    * Otherwise, the function returns NULL to indicate that no such sampler is available.
    * \remarks
    * If in a CgFx source a texture parameter and a sampler parameter are defined as follows:
    * \code
    * texture environment
    * <
    *     string FILE = "textures/myenv.dds"
    * >;
    *
    * samplerCUBE environmentSampler = sampler_state
    * {
    *     Texture = <environment>;
    *     MinFilter = LinearMipMapLinear;
    *     MagFilter = Linear;
    *     WrapS  = ClampToEdge;
    *     WrapT  = ClampToEdge;
    * };
    * \endcode
    * We call the sampler 'environmentSampler' to be associated with the texture 'environment'.
    * \sa CgFxEffect::getSamplerTexture
    */
  NVSG_API CgFxParameter getSamplerByTextureParameterName(const std::string& name) const;

  /*! \brief Returns the CGparameter handle that corresponds to the specified sampler parameter.
    * \param
    * param Handle to the sampler.
    * \return
    * The function returns the CGparameter handle that corresponds to the specified sampler 
    * parameter.
    * \remarks
    * A CGparameter handle sometimes is required for platform dependent calls that need to be
    * executed, for example, from a certain renderer to pass platform dependent texture resources
    * to the underlying effect or to manage platform dependent texture states. These platform
    * dependent calls are not wrapped by SceniX, and hence, require the original Cg types instead
    * of the SceniX wrapped types. If SceniX is initialized for multiple GPUs, the GPU specific
    * CGparameter is returned. 
    */
  NVSG_API CGparameter getSamplerTextureParameter(CgFxParameter param) const;

  /*! \brief Returns the number of samplers used in a particular program.
    * \return
    * The function returns the number of samplers in the given program.
    * \remarks
    * This function in conjunction with CgFxEffect::getProgramSampler is useful for iterating through
    * the samplers in a given program.
    */
  NVSG_API unsigned int getNumberOfProgramSamplers(CgFxProgram prog) const;

  //! Returns the sampler at the specified zero-based index in the given program.
  /** This function is useful when iterating through the programs samplers.
    *
    * The behavior is undefined if passing an invalid index! Use getNumberOfProgramSamplers
    * to determine the number of program samplers available.
    *
    * \returns A handle to \c index-th effect sampler. */
  /*! \brief Returns the sampler at the specified zero-based index.
    * \param
    * prog Program (bound to this effect) to query.
    * \param
    * index Zero-based index of the sampler.
    * \return
    * The function returns a handle to the requested sampler.
    * \remarks
    * Passing an invalid index, that is - an index greater or equal to what 
    * CgFxEffect::getNumberOfSamplers returns results in undefined behavior.
    */
  NVSG_API CgFxParameter getProgramSampler(CgFxProgram prog, unsigned int index) const;

  //! Returns the input type for the given program.
  /** This function is typically only useful for geometry programs, since
    * the input and output to Vertex and Fragment programs are Vertices and
    * Fragments.  Hence, the return value for Vertex and Fragment
    * programs will be PROGRAM_IO_VERTEX and PROGRAM_IO_FRAGMENT respectively.
    *
    * The behavior is undefined if passed an invalid program.
    *
    * \returns A ProgramIOType associated with this program.
    * \sa ProgramIOType, CgFxEffect::getProgramOutputType
    */
  NVSG_API ProgramIOType getProgramInputType(CgFxProgram prog) const;

  //! Returns the output type for the given program.
  /** This function is typically only useful for geometry programs, since
    * the input and output to Vertex and Fragment programs are Vertices and
    * Fragments.  Hence, the return value for Vertex and Fragment
    * programs will be PROGRAM_IO_VERTEX and PROGRAM_IO_FRAGMENT respectively.
    *
    * The behavior is undefined if passed an invalid program.
    *
    * \returns A ProgramIOType associated with this program.
    * \sa ProgramIOType, CgFxEffect::getProgramInputType
    */
  NVSG_API ProgramIOType getProgramOutputType(CgFxProgram prog) const;


  /*! \brief Returns whether the sampler state for the specified sampler requires explicit 
   *  mipmap specification on the client-site.
   *  \param sampler Specifies the sampler for which to query information.
   *  \return \c true if mipmap specification on the client-site is required, \c false otherwise.
   *  \remarks The function evaluates if the current MinFilter and GenerateMipmap sampler state 
   *  assignments require explicit mipmap specification for the assigned texture on the client-site. 
   *  This is true if the MinFilter state is set to LinearMipMapNearest, NearestMipMapNearest,
   *  NearestMipMapLinear, or LinearMipMapLinear, and the GenerateMipmap state is either not
   *  specified, or set to \c false. If GenerateMipmap is set to \c true, mipmap auto-generation
   *  will be enabled and only mipmap level 0 needs to be uploaded from the client-site.
   *  \note Calling this function for others than sampler parameters results in undefined
   *  behavior. Use CgFxEffect::isSamplerParameter to test whether a certain parameter is a
   *  sampler parameter.
   *  \sa isSamplerParameter 
   */
  NVSG_API bool isMipmapSamplerState(CgFxParameter sampler) const;

  /*! \brief Returns whether the indicated Texture is compliant to the specified sampler.
    * \param sampler Handle to the sampler.
    * \param image Specifies the Texture to test for compliance.
    * \return 
    * The function returns \c true if the indicated Texture is compliant to the specified 
    * sampler, and hence, could be assigned to the sampler. Otherwise the function returns \c false.
    * \remarks
    * A Texture that is not compliant to a given sampler cannot be assigned to that sampler.
    * For example, a cube map Texture cannot be assigned to a Sampler2D. Doing so, would
    * result in display corruption during rendering.
    * \n\n
    * Calling this function for others than sampler parameters results in undefined behavior. 
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter is a sampler parameter.
    * \sa setSamplerTexture
    */
  NVSG_API bool isTextureSamplerCompliant(CgFxParameter sampler, const TextureSharedPtr & image) const;

  /*! \brief Assigns a new Texture to the specified sampler parameter.
    * \param
    * param Handle to the sampler.
    * \param
    * image Specifies the new Texture.
    * \return
    * The function returns \c true if the re-specification of the sampler's texture image was successful.
    * Otherwise, the function returns \c false.
    * \remarks
    * The function can be used to re-specify a sampler's texture image.\n 
    * If the type of the passed through Texture doesn't match the sampler's type,
    * or if the type is not convertible to a type matching the sampler's type,
    * the current assignment remains unchanged, and the function returns \c false, to
    * indicate an error. Use isTextureSamplerCompliant to obtain if a given Texture
    * is compliant to a given sampler.\n
    * If the type of the new Texture matches the sampler's type, the new Texture
    * will be assigned. If the new Texture is convertible to a type matching the 
    * sampler's type, the new Texture will be converted prior to assignment.\n
    * A previously assigned Texture will be unreferenced prior to assign the new Texture.
    * If the previously assigned Texture is not referenced by any other objects, it will be
    * destroyed as it gets unreferenced.
    * \n\n
    * Calling this function for others than sampler parameters results in undefined behavior. 
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter is a sampler parameter.
    * \sa getSamplerTexture, isTextureSamplerCompliant, Texture 
    */
  NVSG_API bool setSamplerTexture(CgFxParameter param, const TextureSharedPtr & image);

  /*! \brief Returns the file name that is associated with the sampler's texture image.
    * \param
    * param Handle to the sampler.
    * \param
    * file Reference to a STL string that receives the requested file name.
    * \return
    * The function returns \c true if a file is associated with the sampler's texture image and the
    * file name could have been successfully assigned to \c file. Otherwise the function returns 
    * \c false.
    * \remarks
    * If a valid filename is associated with the sampler's texture image, the function returns 
    * the filename in the string object referenced by \a file.
    * \sa cgFxEffect::getSamplerTexture
    */
  NVSG_API bool getSamplerTextureFileName(CgFxParameter param, std::string& file) const;

    /*! \brief Returns the texture assigned to the specified sampler parameter.
    * \param
    * param Handle to the sampler.
    * \return
    * The function returns the Texture that is associated with the specified sampler. 
    * \n\n
    * The function returns NULL if there is no Texture associated with the sampler.
    * \remarks
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    */
  NVSG_API const TextureSharedPtr& getSamplerTexture(CgFxParameter param) const;

  /*! \brief Returns whether a TextureHost is assigned to the sampler.
    * \param
    * param Handle to the sampler.
    * \return
    * the function returns \c true if a TextureHost is assigned to the sampler, otherwise \c false.
    * \remarks
    * The return value is a cached result from setSamplerTexture operations.
    */
  NVSG_API bool hasSamplerTextureHost(CgFxParameter param) const;

  
  /*! \brief Requests a particular texture GPU format that be used for this sampler.
    * \param
    * param Handle to the sampler.
    * \param
    * format The format from TextureGPUFormat enum.
    * \return
    * The function returns false if there is no texture associated with this
    * parameter.
    * \sa getSamplerTextureGPUFormat
    */
  NVSG_API bool setSamplerTextureGPUFormat(CgFxParameter param, 
                                        TextureHost::TextureGPUFormat format);

  /*! \brief Returns the texture GPU format that was requested of this sampler.
    * \param
    * param Handle to the sampler.
    * \param
    * format The returned format.
    * \return
    * The function returns the GPU format that was requested for this sampler.  
    * Note that the return value does not indicate the ACTUAL GPU format used,
    * only the one that was requested.
    * \n\n
    * The function returns false if there is no texture associated with this
    * parameter.
    * \sa setSamplerTextureGPUFormat
    */
  NVSG_API bool getSamplerTextureGPUFormat(CgFxParameter param, 
                                    TextureHost::TextureGPUFormat & format) const;

  /*! \brief Retrieves the TextureTarget for the specified sampler parameter
   * \param
   * param Handle to the Sampler to query.
   * \return
   * The function returns a TextureTarget specifier that indicates the texture target to 
   * which the assigned Texture should be bound by the renderer. The texture target
   * for a certain sampler is determined by it's unambiguous type. This is read-only
   * information and cannot be changed through the CgFxEffect API. 
   * \sa setSamplerTexture */
  NVSG_API TextureTarget getSamplerTextureTarget(CgFxParameter param) const;


  /*! \brief Returns the number of components the specified named sampler state value has.
    * \param
    * param Handle to the sampler.
    * \param
    * name Specifies the name of the sampler state to query.
    * \return
    * The function returns the number of value components for the sampler state identified by \a 
    * name. If for the specified sampler the named state could not be found, the function returns 0. 
    * \remarks
    * This function is particularly useful when calculating the size of a certain sampler state's 
    * value. Together with the type information for the sampler state 
    * (e.g. CgFxEffect::isFloatSamplerState) this is an easy task:
    * Calculating the size that a certain 
    * sampler state's value consumes is essential when querying the value 
    * (e.g. CgFxEffect::getFloatSamplerStateValue).
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    */
  NVSG_API unsigned int getNumberOfSamplerStateValueComponents(CgFxParameter param, const std::string& name) const;
  
  /*! \brief Returns whether the specified sampler state is a float-type sampler state.
    * \param
    * param Handle to the sampler.
    * \param
    * name Specifies the name of the sampler state to query.
    * \return
    * The function returns \c true if the specified sampler state is a float-type sampler state.
    * If the sampler state is of another type, or if the named sampler state is not defined for
    * the sampler, the function returns \c false.
    * \remarks
    * To query the current value of a certain sampler state, one need to know what type the sampler 
    * state is because corresponding query functions are type dependent (e.g. 
    * CgFxEffect::getFloatSamplerStateValue or CgFxEffect::getIntSamplerStateValue).
    * \n\n
    * Calling this function for others than sampler parameter results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    */
  NVSG_API bool isFloatSamplerState(CgFxParameter param, const std::string& name) const;
  
  /*! \brief Returns whether the specified sampler state is a integer-type sampler state.
    * \param
    * param Handle to the sampler.
    * \param
    * name Specifies the name of the sampler state to query.
    * \return
    * The function returns \c true if the specified sampler state is a integer-type sampler state.
    * If the sampler state is of another type, or if the named sampler state is not defined for
    * the sampler, the function returns \c false.
    * \remarks
    * To query the current value of a certain sampler state, you need to know what type the sampler 
    * state is because corresponding query functions are type dependent (e.g. 
    * CgFxEffect::getIntSamplerStateValue or CgFxEffect::getFloatSamplerStateValue).
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    */
  NVSG_API bool isIntSamplerState(CgFxParameter param, const std::string& name) const;
  
  /*! \brief Returns whether the specified sampler state is a boolean-type sampler state.
    * \param
    * param Handle to the sampler.
    * \param
    * name Specifies the name of the sampler state to query.
    * \return
    * The function returns \c true if the specified sampler state is a boolean-type sampler state.
    * If the sampler state is of another type, or if the named sampler state is not defined for
    * the sampler, the function returns \c false.
    * \remarks
    * To query the current value of a certain sampler state, you need to know what type the sampler 
    * state is because corresponding query functions are type dependent (e.g. 
    * CgFxEffect::getBoolSamplerStateValue or CgFxEffect::getIntSamplerStateValue).
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    */
  NVSG_API bool isBoolSamplerState(CgFxParameter param, const std::string& name) const;
 
  /*! \brief Returns the value of the specified float-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to query.
    * \param
    * value Pointer to the buffer that receives the value. 
    * \return
    * The function returns \c true if the value could be retrieved successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to get hold of the the value of the float-type sampler state specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfSamplerStateValueComponents to determine the number of
    * components a certain sampler state value has. Together with the sampler state's type 
    * information (e.g. CgFxEffect::isFloatSamplerState) it is easy to calculate the right size
    * for the receiving buffer.
    * \n\n
    * The function has no effect if called for other than float-type sampler states!
    * Use CgFxEffect::isFloatSamplerState to determine whether a certain sampler state is a 
    * float-type sampler state.
    * \n\n
    * The value for a certain sampler state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    * \sa CgFxEffect::getIntSamplerStateValue, CgFxEffect::getBoolSamplerStateValue
    */
  NVSG_API bool getFloatSamplerStateValue(CgFxParameter param, const std::string& name, float* value) const;

  /*! \brief Sets the value of the specified float-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to set.
    * \param
    * value Pointer to the buffer that contains the data to be set. 
    * \return
    * The function returns \c true if the sampler contained the state assignment specified 
    * with name and the value(s) could be set successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to override an existing float sampler state assignment specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfSamplerStateValueComponents to determine the number of
    * components a certain sampler state value has. Together with the sampler state's type 
    * information (e.g. CgFxEffect::isFloatSamplerState) it is easy to calculate the right size
    * for the source buffer.
    * \n\n
    * The function has no effect if called for other than float-type sampler states!
    * Use CgFxEffect::isFloatSamplerState to determine whether a certain sampler state is a 
    * float-type sampler state.
    * \n\n
    * The value for a certain sampler state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    * Note that modifications to the sampler state assignments on CgFX shaders loaded from a file 
    * will not be saved in the modified state as the CgFX file's contents are not touched by current savers.
    * \sa CgFxEffect::setIntSamplerStateValue, CgFxEffect::setBoolSamplerStateValue, CgFxEffect::createFloatSamplerStateValue
    */
  NVSG_API bool setFloatSamplerStateValue(CgFxParameter param, const std::string& name, const float* value) const;
  
  /*! \brief Creates and sets the value of the specified float-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to set.
    * \param
    * components Number of components of the float-type sampler state, must be in the range [1,4], 
    * but currently there are only two kinds of float-type sampler states: float and float4 (BorderColor).
    * \param
    * value Pointer to the buffer that contains the data to be set. 
    * \return
    * The function returns \c true if the sampler state assignment specified 
    * with name could be created and the value(s) could be set successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to create and set sampler state assignments which have not been specified inside a CgFX file.
    * This function required for cases where a CgFX file doesn't contain a sampler state assignment 
    * which should be changed during runtime from the default to some other value.
    * Currently there are only float and float4 sampler state assignments 
    * The rest of the functionality is identical to \sa CgFxEffect::setFloatSamplerStateValue.
    * Note that modifications to the sampler state assignments on CgFX shaders loaded from a file 
    * will not be saved in the modified state as the CgFX file's contents are not touched by current savers.
    * \sa CgFxEffect::createIntSamplerStateValue, CgFxEffect::createBoolSamplerStateValue
    */
  NVSG_API bool createFloatSamplerStateValue(CgFxParameter param, const std::string& name, int components, const float* value) const;
 
  /*! \brief Returns the value of the specified integer-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to query.
    * \param
    * value Pointer to the buffer that receives the value. 
    * \return
    * The function returns \c true if the value could be retrieved successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to get the value of the integer-type sampler state specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfSamplerStateValueComponents to determine the number of
    * components a certain sampler state value has. Together with the sampler state's type 
    * information (e.g. CgFxEffect::isIntSamplerState) it is easy to calculate the right size
    * for the receiving buffer.
    * \n\n
    * The function has no effect if called for other than integer-type sampler states!
    * Use CgFxEffect::isIntSamplerState to determine whether a certain sampler state is a 
    * integer-type sampler state.
    * \n\n
    * The value for a certain sampler state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    * \sa CgFxEffect::getFloatSamplerStateValue, CgFxEffect::getBoolSamplerStateValue
    */
  NVSG_API bool getIntSamplerStateValue(CgFxParameter param, const std::string& name, int* value) const;

  /*! \brief Sets the value of the specified int-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to set.
    * \param
    * value Pointer to the buffer that contains the data to be set.
    * Currently there are only single integer sampler states supported.
    * \return
    * The function returns \c true if the sampler contained the state assignment specified 
    * with name and the value(s) could be set successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to override an existing int sampler state assignment specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfSamplerStateValueComponents to determine the number of
    * components a certain sampler state value has. Together with the sampler state's type 
    * information (e.g. CgFxEffect::isIntSamplerState) it is easy to calculate the right size
    * for the source buffer.
    * \n\n
    * The function has no effect if called for other than int-type sampler states!
    * Use CgFxEffect::isIntSamplerState to determine whether a certain sampler state is a 
    * int-type sampler state.
    * \n\n
    * The value for a certain sampler state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    * Note that modifications to the sampler state assignments on CgFX shaders loaded from a file 
    * will not be saved in the modified state as the CgFX file's contents are not touched by current savers.
    * \sa CgFxEffect::setFloatSamplerStateValue, CgFxEffect::setBoolSamplerStateValue, CgFxEffect::createintSamplerStateValue
    */
  NVSG_API bool setIntSamplerStateValue(CgFxParameter param, const std::string& name, const int* value) const;

  /*! \brief Creates and sets the value of the specified int-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to set.
    * \param
    * components Number of components of the int-type sampler state, must be 1 today.
    * because currently there exist only single int-type sampler states.
    * \param
    * value Pointer to the buffer that contains the source data to be set. 
    * \return
    * The function returns \c true if the sampler state assignment specified 
    * with name could be created and the value(s) could be set successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to create and set sampler state assignments which have not been specified inside a CgFX file.
    * This function is required for cases where a CgFX file doesn't contain a sampler state assignment 
    * which should be changed during runtime from the default to some other value.
    * The rest of the functionality is identical to \sa CgFxEffect::setIntSamplerStateValue
    * Note that modifications to the sampler state assignments on CgFX shaders loaded from a file 
    * will not be saved in the modified state as the CgFX file's contents are not touched by current savers.
    * \sa CgFxEffect::createIntSamplerStateValue, CgFxEffect::createBoolSamplerStateValue
    */
  NVSG_API bool createIntSamplerStateValue(CgFxParameter param, const std::string& name, int components, const int* value) const;


  /*! \brief Returns the value of the specified 32-bit boolean-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to query.
    * \param
    * value Pointer to the buffer that receives the value. 
    * \return
    * The function returns \c true if the value could be retrieved successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to get the value of the 32-bit boolean-type sampler state specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfSamplerStateValueComponents to determine the number of
    * components a certain sampler state value has. Together with the sampler state's type 
    * information (e.g. CgFxEffect::isBoolSamplerState) it is easy to calculate the right size
    * for the receiving buffer.
    * \n\n
    * The function has no effect if called for other than 32-bit boolean-type sampler states!
    * Use CgFxEffect::isBoolSamplerState to determine whether a certain sampler state is a 
    * 32-bit boolean-type sampler state.
    * \n\n
    * The value for a certain sampler state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    * \sa CgFxEffect::getIntSamplerStateValue, CgFxEffect::getFloatSamplerStateValue
    */
  NVSG_API bool getBoolSamplerStateValue(CgFxParameter param, const std::string& name, int* value) const;
  
    /*! \brief Sets the value of the specified bool-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to set.
    * The only boolean sampler state is GenerateMipMap.
    * \param
    * value Pointer to the buffer that contains the data to be set. 
    * Note that the values are provided as integers (0 == false), not booleans.
    * \return
    * The function returns \c true if the sampler contained the state assignment specified 
    * with name and the value(s) could be set successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to override an existing bool sampler state assignment specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfSamplerStateValueComponents to determine the number of
    * components a certain sampler state value has. Together with the sampler state's type 
    * information (e.g. CgFxEffect::isBoolSamplerState) it is easy to calculate the right size
    * for the source buffer.
    * \n\n
    * The function has no effect if called for other than bool-type sampler states!
    * Use CgFxEffect::isBoolSamplerState to determine whether a certain sampler state is a 
    * bool-type sampler state.
    * \n\n
    * The value for a certain sampler state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Calling this function for other than sampler parameters results in undefined behavior.
    * Use CgFxEffect::isSamplerParameter to test whether a certain parameter specifies a sampler
    * parameter.
    * Note that modifications to the sampler state assignments on CgFX shaders loaded from a file 
    * will not be saved in the modified state as the CgFX file's contents are not touched by current savers.
    * \sa CgFxEffect::setFloatSamplerStateValue, CgFxEffect::setIntSamplerStateValue, CgFxEffect::createBoolSamplerStateValue
    */
  NVSG_API bool setBoolSamplerStateValue(CgFxParameter param, const std::string& name, const int* value) const;

  /*! \brief Creates and sets the value of the specified bool-type sampler state.
    * \param
    * param Handle to the sampler
    * \param
    * name Specifies the name of the sampler state to set.
    * \param
    * components Number of components of the bool-type sampler state, must be 1 today,
    * because currently there exists only single bool-type sampler state: GenerateMipMap.
    * \param
    * value Pointer to the buffer that contains the source data to be set.
    * Note that the values are provided as integers (0 == false), not booleans.
    * \return
    * The function returns \c true if the sampler state assignment specified 
    * with name could be created and the value(s) could be set successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to create and set sampler state assignments which have not been specified inside a CgFX file.
    * This function is required for cases where a CgFX file doesn't contain a sampler state assignment 
    * which should be changed during runtime from the default to some other value.
    * The rest of the functionality is identical to \sa CgFxEffect::setBoolSamplerStateValue
    * Note that modifications to the sampler state assignments on CgFX shaders loaded from a file 
    * will not be saved in the modified state as the CgFX file's contents are not touched by current savers.
    * \sa CgFxEffect::createFloatSamplerStateValue, CgFxEffect::createIntSamplerStateValue
    */
  NVSG_API bool createBoolSamplerStateValue(CgFxParameter param, const std::string& name, int components, const int* value) const;

  /*! \brief Returns the number of techniques defined for this effect.
    * \return
    * The function returns the number of techniques defined for this effect.
    * \remarks
    * Use this function together with CgFxEffect::getTechnique to iterate through all techniques
    * defined for this effect.
    * \par Example:
    * \code
    * const CgFxEffect * effect;
    * // ...
    * // assume a valid read-locked effect here
    * unsigned int numTechs = effect->getNumberOfTechniques();
    * for ( unsigned int i=0; i<numTechs; ++i )
    * {
    *     CgFxTechnique tech = effect->getTechnique(i);
    *     // proceed with the technique received ...
    * }
    * \endcode
    * \sa CgFxEffect::setCurrentTechnique, CgFxEffect::validateCurrentTechnique
    */
  NVSG_API unsigned int getNumberOfTechniques() const;

  /*! \brief Returns the technique at the specified zero-based index.
    * \param
    * index Zero-based index of the technique to query.
    * \return
    *  The function returns the \a index-th technique defined in the effect.
    * \remarks
    * This function together with CgFxEffect::getNumberOfTechniques can be used to iterate through 
    * all techniques defined in the effect.
    * \n\n
    * Calling this function with an invalid index, that is - an index greater or equal to what
    * CgFxEffect::getNumberOfTechniques returns, results in undefined behavior.
    * \sa CgFxEffect::getTechniqueByName
    */
  NVSG_API CgFxTechnique getTechnique(unsigned int index) const;

  /*! \brief Returns a technique by name.
    * \param
    * name Specifies the name of the technique to retrieve.
    * \return
    * If a technique with the specified name could be found in the effect, the function returns
    * a valid handle to that technique. Otherwise, the function returns NULL.
    * \remarks
    * A technique defined in an effect can be accessed directly by name. The names of the techniques
    * defined in an effect can be discovered by iterating through all techniques using
    * CgFxEffect::getNumberOfTechniques and CgFxEffect::getTechnique, and in turn calling
    * CgFxEffect::getTechniqueName for each.
    * \sa CgFxEffect::getTechnique
    */
  NVSG_API CgFxTechnique getTechniqueByName(const std::string& name) const;

  /*! \brief Returns the zero-based index of the specified technique.
   * \param tech Handle to the technique to query.
   * \return
   * The function returns the zero-based index of the specified technique if found.
   * The function returns ~0 if the technique could not be found.
   * \sa getTechnique */
  NVSG_API unsigned int getTechniqueIndex(CgFxTechnique tech) const;

  /*! \brief Returns the name of the specified technique.
    * \param
    * tech Handle to the technique to query.
    * \return
    * The function returns a reference to a const STL string that holds the name of the technique.
    * \remarks
    * Once a the name of a technique is determined, the technique can be directly accessed by its
    * name using CgFxEffect::getTechniqueByName.
    * \n\n
    * Calling this function for an invalid technique results in undefined behavior. Use either
    * CgFxEffect::getTechnique or CgFxEffect::getTechniqueByName to get hold of valid techniques
    * defined in the effect.
    */
  NVSG_API const std::string& getTechniqueName(CgFxTechnique tech) const;

  /*! \brief Returns the current technique.
    * \return
    * The function returns a handle to the technique currently used for rendering.
    * \remarks
    * The function returns the current technique used for rendering. Initially this is the first 
    * technique found in the effect. Use CgFxEffect::setCurrentTechnique to alter the current 
    * technique as wanted. Note that before using the current technique for rendering, it must
    * be validated using CgFxEffect::validateCurrentTechniqe.
    */
  NVSG_API CgFxTechnique getCurrentTechnique() const;

  /*! \brief Sets the current technique to be used with rendering.
    * \param
    * tech Handle to the technique to make current.
    * \remarks
    * Before using the current technique for rendering, it must be validated using 
    * CgFxEffect::validateCurrentTechniqe.
    * \n\n
    * Calling this function for an invalid technique results in undefined behavior. Use either
    * CgFxEffect::getTechnique or CgFxEffect::getTechniqueByName to get hold of valid techniques
    * defined in the effect.
    * \sa CgFxEffect::getCurrentTechnique
    */
  NVSG_API void setCurrentTechnique(CgFxTechnique tech);

  /*! \brief Validates the current technique.
    * \return
    * The function returns \c true if the validation succeeded.
    * Otherwise the function returns \c false.
    * \remarks
    * Before using the current technique, it is important to validate it.
    * A technique validation can fail if, for instance, for a fragment program an unsupported 
    * profile is used, or if the technique includes an unsupported state assignment. 
    * \sa CgFxEffect::setCurrentTechnique
    */
  NVSG_API bool validateCurrentTechnique();

  /*! \brief Returns whether the specified technique contains tessellation a program.
    * \param
    * tech Handle to the technique to query.
    * \return
    * The function returns \c true if the specified technique contains a tessellation program, otherwise \c false.
    * \remarks
    * Calling this function for an invalid technique results in undefined behavior. Use either
    * CgFxEffect::getTechnique or CgFxEffect::getTechniqueByName to get hold of valid techniques
    * defined in the effect.
    */
  NVSG_API bool containsTessellationProgram( CgFxTechnique tech ) const;

  /*! \brief Returns the number of render passes defined for the specified technique.
    * \param
    * tech Handle to the technique to query.
    * \return
    * The function returns the number of passes defined for the specified technique.
    * \remarks
    * A pass in CgFX defines, for example, what vertex and fragment programs should be applied, and
    * what specific render states need to be assigned to achieve the desired render effect.
    * \n\n
    * Use this function together with CgFxEffect::getPass to iterate through all passes defined in
    * the specified technique.
    * \n\n
    * Calling this function for an invalid technique results in undefined behavior. Use either
    * CgFxEffect::getTechnique or CgFxEffect::getTechniqueByName to get hold of valid techniques
    * defined in the effect.
    */
  NVSG_API unsigned int getNumberOfPasses(CgFxTechnique tech) const;

  /*! \brief Returns the pass at the specified zero-based index defined in the specified technique.
    * \param
    * tech Handle to the technique to query.
    * \param
    * index Zero-based index of the pass to retrieve.
    * \return
    * The function returns a handle to the \a index-th pass defined in the specified technique.
    * \remarks
    * A pass in CgFX defines, for example, what vertex and fragment programs should be applied, and
    * what specific render states need to be assigned to achieve the desired render effect.
    * \n\n
    * Calling this function for an invalid technique results in undefined behavior. Use either
    * CgFxEffect::getTechnique or CgFxEffect::getTechniqueByName to get hold of valid techniques
    * defined in the effect.
    * \n\n 
    * Passing an invalid index - that is, an index greater or equal to what 
    * CgFxEffect::getNumberOfPasses returns - also results in undefined behavior.
    */
  NVSG_API CgFxPass getPass(CgFxTechnique tech, unsigned int index) const;

  /*! \brief Returns a pass of a specified technique by name.
    * \param
    * tech Handle to the technique to query.
    * \param
    * name Name of the pass to retrieve.
    * \return
    * If a pass with the specified name could be found in the specified technique, the function 
    * returns a valid handle to that pass. Otherwise, the function returns NULL.
    * \remarks
    * A pass can be accessed directly by name. The names of all passes defined in a technique can be 
    * discovered by iterating through all passes using CgFxEffect::getNumberOfPasses and 
    * CgFxEffect::getPass, and in turn calling CgFxEffect::getPassName for each.
    * \n\n
    * A pass in CgFX defines, for example, what vertex and fragment programs should be applied, and
    * what specific render states need to be assigned to achieve the desired render effect.
    * \n\n
    * Calling this function for an invalid technique results in undefined behavior. Use either
    * CgFxEffect::getTechnique or CgFxEffect::getTechniqueByName to get valid techniques
    * defined in the effect.
    */
  NVSG_API CgFxPass getPassByName(CgFxTechnique tech, const std::string& name) const;

  /*! \brief Returns the name of the specified render pass.
    * \param
    * pass Handle to the pass to query.
    * \return
    * The function returns a reference to a STL string that holds the name of the pass.
    * \remarks
    * Once a the name of a pass is determined, the pass can be directly accessed by its
    * name using CgFxEffect::getPassByName.
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    */
  NVSG_API const std::string& getPassName(CgFxPass pass) const;

  /*! \brief Tests whether the specified pass forces transparency 
    * \param
    * pass Handle to the pass to query
    * \return
    * The function returns \c true if the pass forces the respective geometry to appear transparent.
    * Otherwise, the function returns \c false.
    * \remarks
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get valid passes
    * defined for a certain technique.
    * \sa CgFX::determineTransparency
    */
  NVSG_API bool isTransparentPass(CgFxPass pass) const;

  /*! \brief Applies the specified render pass.
    * \param
    * index Zero-based index of the pass of the current technique
    * \remarks
    * The function applies all state assignments of the \a index-th pass of the current technique.
    * The function must be called immediately before sending the geometry for that pass down the
    * pipeline. After the geometry was sent you need to un-apply the pass by calling 
    * CgFxEffect::resetPass. This call sequence (apply-draw-reset) must be repeated for all passes 
    * available in the current technique. Passes must be applied in ascending order starting at the 
    * first pass (index 0).
    * \n\n
    * Note that before applying the passes of the current technique, the current technique must
    * be validated using CgFxEffect::validateCurrentTechnique.
    * \n\n
    * Passing an invalid index - that is, an index greater or equal to what 
    * CgFxEffect::getNumberOfPasses for the current technique returns - results in undefined 
    * behavior.
    * \par Example:
    * \code
    * const CgFx * cgfx;
    * // ...
    * { // limit effect writable scope
    *   
    *   // assume a valid write-locked CgFx attribute here
    *   CgFxEffectWriteLock effect( cgfx->getEffect() );
    *   if ( effect->validateCurrentTechnique() )
    *   {
    *     // prepare the effect for rendering, that is 
    *     // - setup all required textures (platform dependent), and
    *     // - update uniform transforms
    *     // ...
    * 
    *     // apply all the passes
    *     CgFxTechnique tech = effect->getCurrentTechnique();
    *     unsigned int numPasses = effect->getNumberOfPasses(tech);
    *     unsigned int currPass = 0;
    *     do
    *     {
    *         effect->applyPass(currPass);
    *         DrawGeometry(); // send geometry down the pipeline
    *         effect->resetPass(currPass);
    *     } while (++currPass < numPasses)
    *   }
    * } // end writable scope
    * \endcode
    * \sa CgFxEffect::setCurrentTechnique
    */
  NVSG_API void applyPass(unsigned int index);

  /*! \brief Resets the specified render pass.
    * \param
    * index Zero-based index of the pass of the current technique
    * \remarks
    * The function resets all state assignments specified in the \a index-th pass of the current 
    * technique. This function should be called after the pass was applied and geometry was sent 
    * down the pipeline.
    * \n\n
    * Passing an invalid index - that is, an index greater or equal to what 
    * CgFxEffect::getNumberOfPasses for the current technique returns - results in undefined 
    * behavior.
    * \sa CgFxEffect::applyPass
    */
  NVSG_API void resetPass(unsigned int index);

  /*! \brief Returns the number of components the specified pass state value has.
    * \param
    * pass Handle to the pass.
    * \param
    * name Specifies the name of the pass state to query.
    * \return
    * The function returns the number of value components for the pass state identified by \a 
    * name. If for the specified pass the named state could not be found, the function returns 0. 
    * \remarks
    * This function is particularly useful when calculating the size of a certain pass state's 
    * value. Together with the type information for the pass state 
    * (e.g. CgFxEffect::isFloatPassState) this is an easy task:
    * Calculating the size that a certain pass state's value consumes is essential when querying the 
    * value (e.g. CgFxEffect::getFloatPassStateValue).
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    */
  NVSG_API unsigned int getNumberOfPassStateValueComponents(CgFxPass pass, const std::string& name) const;
  
  /*! \brief Returns whether the specified pass state is a float-type pass state.
    * \param
    * pass Handle to the pass.
    * \param
    * name Specifies the name of the pass state to query.
    * \return
    * The function returns \c true if the specified pass state is a float-type pass state.
    * If the pass state is of another type, or if the named pass state is not defined for
    * the pass, the function returns \c false.
    * \remarks
    * To query the current value of a certain pass state, one need to know what type the pass 
    * state is, because the corresponding query functions are type dependent (e.g. 
    * CgFxEffect::getFloatPassStateValue or CgFxEffect::getIntPassStateValue).
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    */
  NVSG_API bool isFloatPassState(CgFxPass pass, const std::string& name) const;
  
  /*! \brief Returns whether the specified pass state is a integer-type pass state.
    * \param
    * pass Handle to the pass.
    * \param
    * name Specifies the name of the pass state to query.
    * \return
    * The function returns \c true if the specified pass state is a integer-type pass state.
    * If the pass state is of another type, or if the named pass state is not defined for
    * the pass, the function returns \c false.
    * \remarks
    * To query the current value of a certain pass state, you need to know what type the pass 
    * state is, because the corresponding query functions are type dependent (e.g. 
    * CgFxEffect::getIntPassStateValue or CgFxEffect::getFloatPassStateValue).
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    */
  NVSG_API bool isIntPassState(CgFxPass pass, const std::string& name) const;
  
  /*! \brief Returns whether the specified pass state is a 32-bit boolean-type pass state.
    * \param
    * pass Handle to the pass.
    * \param
    * name Specifies the name of the pass state to query.
    * \return
    * The function returns \c true if the specified pass state is a 32-bit boolean-type pass state.
    * If the pass state is of another type, or if the named pass state is not defined for
    * the pass, the function returns \c false.
    * \remarks
    * To query the current value of a certain pass state, you need to know what type the pass 
    * state is, because the corresponding query functions are type dependent (e.g. 
    * CgFxEffect::getBoolPassStateValue or CgFxEffect::getFloatPassStateValue).
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    */
  NVSG_API bool isBoolPassState(CgFxPass pass, const std::string& name) const;
 
  /*! \brief Returns the value of the specified float-type pass state.
    * \param
    * pass Handle to the pass
    * \param
    * name Specifies the name of the pass state to query.
    * \param
    * value Pointer to the buffer that receives the value. 
    * \return
    * The function returns \c true if the value could be retrieved successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to get the value of the float-type pass state specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfPassStateValueComponents to determine the number of
    * components a certain pass state value has. Together with the pass state's type 
    * information (e.g. CgFxEffect::isFloatPassState) it is easy to calculate the right size
    * for the receiving buffer.
    * \n\n
    * The function has no effect if called for others than float-type pass states!
    * Use CgFxEffect::isFloatPassState to determine whether a certain pass state is a 
    * float-type pass state.
    * \n\n
    * The value for a certain pass state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Note that pass states are read-only. Hence, there is no corresponding function to modify
    * a certain pass state's value.
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    * \sa CgFxEffect::getIntPassStateValue, CgFxEffect::getBoolPassStateValue
    */
  NVSG_API bool getFloatPassStateValue(CgFxPass pass, const std::string& name, float* value) const;
 
  /*! \brief Returns the value of the specified integer-type pass state.
    * \param
    * pass Handle to the pass
    * \param
    * name Specifies the name of the pass state to query.
    * \param
    * value Pointer to the buffer that receives the value. 
    * \return
    * The function returns \c true if the value could be retrieved successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to get the value of the integer-type pass state specified by 
    * \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfPassStateValueComponents to determine the number of
    * components a certain pass state value has. Together with the pass state's type 
    * information (e.g. CgFxEffect::isIntPassState) it is easy to calculate the right size
    * for the receiving buffer.
    * \n\n
    * The function has no effect if called for other than integer-type pass states!
    * Use CgFxEffect::isIntPassState to determine whether a certain pass state is a 
    * integer-type pass state.
    * \n\n
    * The value for a certain pass state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Note that pass states are read-only. Hence, there is no corresponding function to modify
    * a certain pass state's value.
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    * \sa CgFxEffect::getFloatPassStateValue, CgFxEffect::getBoolPassStateValue
    */
  NVSG_API bool getIntPassStateValue(CgFxPass pass, const std::string& name, int* value) const;

  /*! \brief Returns the value of the specified 32-bit boolean-type pass state.
    * \param
    * pass Handle to the pass
    * \param
    * name Specifies the name of the pass state to query.
    * \param
    * value Pointer to the buffer that receives the value. 
    * \return
    * The function returns \c true if the value could be retrieved successfully. 
    * Otherwise, the function returns \c false.
    * \remarks
    * Use this function to get the value of the 32-bit boolean-type pass state specified 
    * by \a name. The user must provide a buffer large enough to hold the value. The
    * behavior is undefined if the buffer pointed to by \a value does not fulfill this requirement.
    * You can call CgFxEffect::getNumberOfPassStateValueComponents to determine the number of
    * components a certain pass state value has. Together with the pass state's type 
    * information (e.g. CgFxEffect::isBoolPassState) it is easy to calculate the right size
    * for the receiving buffer.
    * \n\n
    * The function has no effect if called for other than 32-bit boolean-type pass states!
    * Use CgFxEffect::isBoolPassState to determine whether a certain pass state is a 
    * 32-bit boolean-type pass state.
    * \n\n
    * The value for a certain pass state corresponds to the actual Cg 1.4 specification.
    * More details regarding the Cg 1.4 specification can be found at
    * <a href="http://developer.nvidia.com/page/cg_main.html">
    * http://developer.nvidia.com/page/cg_main.html</a><p>
    * \n\n
    * Note that pass states are read-only. Hence, there is no corresponding function to modify
    * a certain pass state's value.
    * \n\n
    * Calling this function for an invalid pass results in undefined behavior. Use either
    * CgFxEffect::getPass or CgFxEffect::getPassByName to get hold of valid passes
    * defined for a certain technique.
    * \sa CgFxEffect::getFloatPassStateValue, CgFxEffect::getIntPassStateValue
    */
  NVSG_API bool getBoolPassStateValue(CgFxPass pass, const std::string& name, int* value) const;

  /*! \brief Returns the vertex program defined for the specified pass
    * \param
    * pass Handle to the pass to query.
    * \return
    * If a vertex program is defined for the specified pass, the function returns a handle to that
    * vertex program. Otherwise, the function returns NULL.
    */
  NVSG_API CgFxProgram getVertexProgram(CgFxPass pass) const;

  /*! \brief Returns the tessellation control program defined for the specified pass
    * \param
    * pass Handle to the pass to query.
    * \return
    * If a tessellation control program is defined for the specified pass, the function returns a handle to that
    * tessellation control program. Otherwise, the function returns NULL.
    */
  NVSG_API CgFxProgram getTessellationControlProgram(CgFxPass pass) const;

  /*! \brief Returns the tessellation evaluation program defined for the specified pass
    * \param
    * pass Handle to the pass to query.
    * \return
    * If a tessellation evaluation program is defined for the specified pass, the function returns a handle to that
    * tessellation evaluation program. Otherwise, the function returns NULL.
    */
  NVSG_API CgFxProgram getTessellationEvaluationProgram(CgFxPass pass) const;

  /*! \brief Returns the geometry program defined for the specified pass
    * \param
    * pass Handle to the pass to query.
    * \return
    * If a geometry program is defined for the specified pass, the function returns a handle to that
    * geometry program. Otherwise, the function returns NULL.
    */
  NVSG_API CgFxProgram getGeometryProgram(CgFxPass pass) const;

  /*! \brief Returns the fragment program defined for the specified pass
    * \param
    * pass Handle to the pass to query.
    * \return
    * If a fragment program is defined for the specified pass, the function returns a handle to that
    * fragment program. Otherwise, the function returns NULL.
    */
  NVSG_API CgFxProgram getFragmentProgram(CgFxPass pass) const;

  /*! \brief Returns whether the specified program is compiled.
    * \param
    * prog Handle to the program.
    * \return
    * The function returns \c true if the specified program is compiled.
    * Otherwise the function returns \c false.
    * \remarks
    * Passing this function an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    * \sa CgFxEffect::compileProgram
    */
  NVSG_API bool isProgramCompiled(CgFxProgram prog) const;

  /*! \brief Compiles the specified program
    * \param 
    * prog Handle to the program to compile.
    * \return
    * The function returns \c true if the compilation succeeded. 
    * Otherwise, the function returns \c false.
    * \remarks
    * The function compiles the specified program for its target profile. 
    * Use CgFxEffect::getProgramProfile to determine the current profile used for the program
    * compilation. CgFxEffect::setProgramProfile lets you change that profile as needed.
    * \n\n
    * A program must be compiled before its parameters can be inspected. 
    * \n\n
    * Passing this function an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    */
  NVSG_API bool compileProgram(CgFxProgram prog);

  /*! \brief Sets the profile to be used to compile the specified program.
    * \param
    * prog Handle to the program.
    * \param
    * profile The new CGprofile to use for program compilation.
    * \return
    * The function returns \c true if the profile is recognized as a known profile, and hence,
    * could have been set successfully. Otherwise the function returns \c false.
    * \remarks
    * Dependent on the graphics platform, a profile may not be supported. For OpenGL, for example,
    * this would be the case if a certain OpenGL extension, the profile requires, is not available.
    * \n\n
    * If the specified profile could not be set for some reasons, the former profile remains the
    * current one.
    * \n\n
    * Note that the program must be recompiled if the program's profile changes in order to take 
    * effect.
    * \n\n
    * Calling this function passing it an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    * \sa CgFxEffect::compileProgram
    */
  NVSG_API bool setProgramProfile(CgFxProgram prog, CGprofile profile);

  /*! \brief Returns the actual profile used to compile the specified program.
    * \param
    * prog Handle to the program.
    * \return
    * The function returns the CGprofile used to compile the specified program. 
    * \remarks
    * Passing this function an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    * \sa CgFxEffect::setProgramProfile
    */
  NVSG_API CGprofile getProgramProfile(CgFxProgram prog) const;

  /*! \brief Returns the number of parameters passed to the specified program.
    * \param
    * prog Handle to the program.
    * \return
    * The function returns the number of parameters passed to the specified program.
    * \remarks
    * Use this function together with CgFxEffect::getProgramParameter to iterate through all
    * parameters passed to the specified program.
    * \n\n
    * Passing this function an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    */
  NVSG_API unsigned int getNumberOfProgramParameters(CgFxProgram prog) const;

  /*! \brief Returns the program parameter at the specified zero-based index.
    * \param
    * prog Handle to the program.
    * \param 
    * index Zero-based index of the parameter to query
    * \return
    * The function returns a handle to the requested program parameter.
    * \remarks
    * Use this function together with CgFxEffect::getNumberOfProgramParameter to iterate through 
    * all parameters passed to the specified program.
    * \n\n
    * Passing this an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    */
  NVSG_API CgFxParameter getProgramParameter(CgFxProgram prog, unsigned int index) const;

  /*! \brief Returns whether the specified program parameter is an input parameter.
    * \param
    * prog Handle to the program
    * \param
    * param Handle to the program parameter.
    * \return
    * The function returns \c true if the specified parameter is either an input parameter or
    * an input/output parameter. Otherwise, the function returns \c false. 
    * \remarks
    * Passing this function an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    * \n\n
    * The behavior also is undefined if an invalid parameter handle is passed. Use
    * CgFxEffect::getProgramParameter together with CgFxEffect::getNumberOfProgramParameter to 
    * iterate through all parameters of a program.
    * \sa CgFxEffect::isOutputProgramParameter
    */
  NVSG_API bool isInputProgramParameter(CgFxProgram prog, CgFxParameter param) const;

  /*! \brief Returns whether the specified program parameter is an output parameter.
    * \param
    * prog Handle to the program
    * \param
    * param Handle to the program parameter.
    * \return
    * The function returns \c true if the specified parameter is either an output parameter or
    * an input/output parameter. Otherwise, the function returns \c false. 
    * \remarks
    * Passing this function an invalid program handle results in undefined behavior.
    * Use either CgFxEffect::getVertexProgram or CgFxEffect::getFragmentProgram to get a valid 
    * handle to either the vertex program or the fragment program defined for a certain pass.
    * \n\n
    * The behavior also is undefined if an invalid parameter handle is passed. Use
    * CgFxEffect::getProgramParameter together with CgFxEffect::getNumberOfProgramParameter to 
    * iterate through all parameters of a program.
    * \sa CgFxEffect::isInputProgramParameter
    */
  NVSG_API bool isOutputProgramParameter(CgFxProgram prog, CgFxParameter param) const;

    /*! \brief Gets the buffer at the given index from the given program.
    * \param
    * prog Handle to the program
    * \param
    * index The buffer index to use.  There are a small, fixed, number of
    * buffers available.  Buffer indices range from 0 to the value returned from
    * CgFxContext::getBufferMaxIndex() (typically "9").  SceniX reserves several 
    * buffers for future use.
    * \return
    * The function returns a pointer to the CgFxBuffer at the given index.
    * If no buffer exists at that index, NULL is returned.  The function returns
    * NULL if a buffer index greater than the maximum
    * \remarks
    * Passing this function an invalid program handle results in undefined behavior.
    * Use CgFxEffect::get[Vertex|Geometry|Fragment]Program to get a valid 
    * handle to either the vertex, geometry or fragment program defined for a certain pass.
    * \n
    * \sa CgFxEffect::setProgramBuffer CgFxBuffer
    */
  NVSG_API CgFxBuffer getProgramBuffer( CgFxProgram prog, size_t index ) const;

  /*! \brief Sets the buffer at the given index on the given program.
    * \param
    * prog Handle to the program
    * \param
    * b Pointer to the CgFxBuffer.  If this parameter is NULL, it clears the
    * buffer binding at this index.
    * \param
    * index The buffer index.  See the notes in \sa CgFxEffect::setProgramBuffer
    * \return
    * The function returns \c true if the buffer was able to be set, \c false
    * otherwise.  The function returns \c false if an invalid index is 
    * specified.
    * \remarks
    * Passing this function an invalid buffer or program handle results in undefined behavior.
    * Buffers are not managed by the program.  This means that if a program is 
    * deleted, any associated buffer objects will NOT be deleted.  It is the
    * application's responsibility to manage buffers across multiple 
    * programs/effects.  If an effect is cloned, the buffer object pointers 
    * will simply be copied.
    * Use CgFxEffect::get[Vertex|Geometry|Fragment]Program to get a valid 
    * handle to either the vertex, geometry or fragment program defined for a certain pass.
    * \n
    * \sa CgFxEffect::getProgramBuffer CgFxBuffer
    */
  NVSG_API bool setProgramBuffer( CgFxProgram prog, CgFxBuffer b, size_t index );

  /*! \brief Retrieves the parameter buffer index of a parameter.
    * \param
    * param Handle to a parameter
    * \return
    * The function returns the index of a CgFxBuffer object that this parameter 
    * was assigned to, or -1 if this parameter is not associated with a buffer.
    * \n
    * \sa CgFxEffect::getParameterBufferOffset 
    * \sa CgFxEffect::getParameterResourceSize 
    * \sa CgFxEffect::getParameterResourceType 
    * \sa CgFxBuffer
    */
  NVSG_API int getParameterBufferIndex(CgFxParameter param) const;

  /*! \brief Retrieves the parameter buffer offset of a parameter.
    * \param
    * param Handle to a parameter
    * \return
    * The function returns the offset within the CgFxBuffer object that this 
    * parameter was assigned to, or -1 if this parameter is not associated 
    * with a buffer.
    * \n
    * \sa CgFxEffect::getParameterBufferIndex 
    * \sa CgFxEffect::getParameterResourceSize 
    * \sa CgFxEffect::getParameterResourceType 
    * \sa CgFxBuffer
    */
  NVSG_API int getParameterBufferOffset(CgFxParameter param) const;

  /*! \brief Retrieves the size, in bytes, of a parameter associated with a
    * buffer.
    * \param
    * param Handle to a parameter
    * \return
    * The function returns the size, in bytes, of the resource corresponding
    * to the given parameter, if the parameter belongs to a CgFxBuffer object.
    * \remarks
    * The size of sampler parameters is zero because they have no actual data
    * storage.
    * The size of an array parameter is the size of an array element parameter
    * times the length of the array.
    * The size of a structure parameter is the sum of the size of all the 
    * member elements, plus zero or more padding bytes which are implementation
    * dependent.
    * \n
    * \sa CgFxEffect::getParameterBufferIndex 
    * \sa CgFxEffect::getParameterBufferOffset 
    * \sa CgFxEffect::getParameterResourceType 
    * \sa CgFxBuffer
    */
  NVSG_API int getParameterResourceSize(CgFxParameter param) const;

  /*! \brief Retrieves the type of a parameter associated with a buffer.
    * \param
    * param Handle to a parameter
    * \return
    * The function returns the CGtype of the given parameters, if the 
    * parameter belongs to a CgFxBuffer object.
    * \n
    * \sa CgFxEffect::getParameterBufferIndex 
    * \sa CgFxEffect::getParameterBufferOffset 
    * \sa CgFxEffect::getParameterResourceSize 
    * \sa CgFxBuffer
    */
  NVSG_API CGtype getParameterResourceType(CgFxParameter param) const;

  REFLECTION_INFO_API( NVSG_API, CgFxEffect );
protected:
  friend struct nvutil::Holder<CgFxEffect>;

  /*! \brief Default-constructs a CgFxEffect. 
   */
  CgFxEffect(); 

  /*! \brief Constructs a CgFxEffect as a copy from another CgFxEffect. 
   * \param rhs Source CgFxEffect. 
   */
  CgFxEffect(const CgFxEffect& rhs);

  /*! \brief Destructs a CgFxEffect. 
   */
  ~CgFxEffect(); 

  /*! \brief Feed the data of this object into the provied HashGenerator.
   *  \param hg The HashGenerator to update with the data of this object.
   *  \sa getHashKey */
  NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

private:
  CgFxEffect& operator=(const CgFxEffect& rhs);

  bool initialize(const std::string &source, std::string &err); // parse effect and fill internal structs
  void uninitialize(); // cleanup internal structs

  template <typename T> 
  bool getParameterValue(CgFxParameter param, T * value, bool (*testType)(CGtype)) const;
  template <typename T> 
  bool setParameterValue(CgFxParameter param, const T * value, bool (*testType)(CGtype));

  // sync cached values with effect
  void flush();

  // walks all techniques for their passes to unload their vertex and fragment programs
  void unloadPrograms();

  // tests on equivalency with another effect
  bool isEquivalent(const CgFxEffect * rhs) const;

  void registerTransformParameter(CgFxParameter param);
  void registerLightParameter(CgFxParameter param);
  void registerLightPositionParameter(CgFxParameter param, unsigned int i);
  void registerLightDirectionParameter(CgFxParameter param, unsigned int i);
  void registerLightTypeParameter(CgFxParameter param, unsigned int i);
  void registerMaterialParameter(CgFxParameter param);
  void registerSamplerParameter(CgFxParameter param);
  void registerBufferedParameter(CgFxParameter param);
  void registerSceneParameter(CgFxParameter  param);
  void registerTweakableParameter(CgFxParameter  param);
  

private:
  CGMeffect m_cgmEffect;
  std::vector<nvutil::SmartPtr<CgFxEffectResource> > m_effects; // handles to the multiplexed effect resources

  enum SourceType
  {
    SOURCE_UNKNOWN = 0
  , SOURCE_FILE = 1
  , SOURCE_LUMP
  };

  unsigned int m_from; // either SOURCE_FILE or SOURCE_LUMP
  std::string * m_source; // either filename or code

  struct SharedParams : public nvutil::RCObject
  {
    SharedParams();
    ~SharedParams();

    CgFxParameter m_scene; // NVSGScene
    
    // NVSG transforms known by semantic
    std::vector<CgFxParameter> m_transforms;
   
    std::vector<CgFxParameter> m_lights; // NVSGLights
    CgFxParameter m_dynamicLightArray;   // dynamic array of NVSGLights in CgFxEffect
    CgFxParameter m_material; // NVSGMaterial
    
    // MetaSL light params
    std::vector<CgFxParameter> m_lightPositions;
    std::vector<CgFxParameter> m_lightDirections;
    std::vector<CgFxParameter> m_lightTypes;

    
    struct StateLabel {
      StateLabel() : m_first(~0), m_second(~0) {}
      size_t m_first;
      size_t m_second;
    };
    
    // One StateLabel per multiplexed CGeffect
    std::vector<StateLabel> m_sceneState;
    std::vector<StateLabel> m_transformState;
    std::vector<StateLabel> m_lightingState;
    std::vector<StateLabel> m_materialState;

    nvutil::SWMRSync m_lock; // sync access to shared parameters
  };

  nvutil::SmartPtr<SharedParams> m_sharedParams; 
  
  std::set<CgFxParameter> m_effectParams; // tweakables & samplers only
  std::set<LeafParam> m_leafParams; // tweakable leaf parameters 
  
  // shortcuts
  std::vector<CgFxParameter> m_tweaks; // ordinary parameters
  std::vector<CgFxParameter> m_samplers; // samplers
  std::vector<CgFxParameter> m_buffered; // buffered parameters

  std::vector<CgFxTechnique> m_techs; // techniques
  CgFxTechnique m_currTech; // current technique
};


/*! \brief A StateAttribute to apply render effects that are compliant to the Cg 1.4 standard.
 * \par Namespace: nvsg
 */
class CgFx : public StateAttribute
{
public:
  NVSG_API static CgFxSharedPtr create();

public:
  /*! \brief Assigns contents from a source CgFx object.
    * \param
    * rhs Source CgFx object
    * \return
    * The assignment operator returns a reference to this CgFx object.
    * \remarks
    * The assignment operator releases old contents before re-assigning the new contents.
    */
  NVSG_API CgFx& operator=(const CgFx& rhs);

  /*! \brief Returns whether this object shares its embedded data with other CgFx objects.
    * \return
    * The function returns \c true if the object shares its embedded data with another CgFx object.
    * Otherwise, the function returns \c false.
    * \remarks
    * This function is useful only if you need to deal with shared data.
    * \n\n
    * A certain CgFx object shares its data with another CgFx object if it was either instantiated
    * as a copy of the other object (CgFxHandle::clone), or if it was assigned to by the other object 
    * using the \link CgFx::operator= assignment operator \endlink.
    * \n\n
    * If you need to write code that, for instance, saves a CgFx object to a file, you
    * can use this function if you want your code to handle shared data. In this context,
    * CgFx::getDataID would be also very helpful to identify the shared data. 
    */
  NVSG_API virtual bool isDataShared() const;

  /*! \brief Returns the unique data identifier for this object.
    * \return
    * The function returns a 64-bit value which uniquely identifies the embedded data.
    * \remarks
    * This function helps to identify shared CgFx objects. Two CgFx objects that share their data
    * do have the same data ID. Use CgFx::isDataShared to determine whether a certain CgFx object
    * shares its data with other CgFx objects.
    * \sa CgFxHandle::clone, CgFx::operator=()
    */
  NVSG_API virtual DataID getDataID() const;

  /*! \brief Returns the embedded effect.
    * \return
    * The function returns a pointer to the embedded const CgFxEffect object.
    * \remarks
    * The function lets you access the embedded effect, for instance, to initialize it or to 
    * exchange data with it.
    * \n\n
    * If you need to write-access the embedded effect, for example, to exchange data with it,
    * you first need to make it writable using the type CgFxEfffectWriteLock.  You should
    * limit the scope of the write lock as shown in the example below.
    * \par Example:
    * \code
    *   const CgFx * cgfx;
    *   [...]
    *   // assume a valid read-locked CgFx object here
    *   
    *   { // limit write access
    *     CgFxEffectWriteLock effect( cgfx->getEffect() );
    *     // write access the embedded effect
    *     [...]
    *   } // end of scope releases the writable effect
    *   
    * \endcode
    */
  NVSG_API const CgFxEffectSharedPtr & getEffect() const;

  /*! \brief Creates a CgFx Effect from a file, and performs all appropriate 
    *        initialization, such as loading associated textures specified in the CgFx
    *        into the appropriate samplers.
    * \return
    * The function returns a CgFxSharedPtr, which might be invalid, if there was an error.
    * If it is returned invalid, check the text in "errorReturn" for the reason.
    * failOnTextureLoad signifies whether creation of the CgFx should fail if one of
    * the texture files specified in the CgFx file is not found.  If failOnTextureLoad is
    * true, and a texture is not found, createFromFile will return an invalid CgFxSharedPtr,
    * and errorReturn will contain text about which texture was not found.  If failOnTextureLoad
    * is false on the way into the method, then its value on the way out will indicate whether any
    * textures were not found.  In this case, errorReturn will also contain text indicating which
    * textures were not found.  In other words, errorReturn contains valid text if the method returns
    * an invalid SmartCgFXhandle or failOnTexLoad is true;
    * 
    * \remarks
    * The function lets you load a complete cgfx, including textures.  
    * The CgFxEffect::createFromFile method will not load textures automatically.
    * \par Example:
    * \code
    *   vector<string> searchPaths; // configure this to search paths
    *   string err;                 // will receive error in event of null return.
    *   bool failOnTexLoad = false; // don't fail if a texture was not found.
    *   CgFxSharedPtr cgfx = CgFx::createFromFile( "c:/hermannIsCool.cgfx", searchPaths, err, failOnTexLoad );
    *   [...]
    *   if( !cgfx )
    *   { 
    *     printf("I guess hermann was not cool - error: %s\n", err.c_str() );
    *     return;
    *   }
    *
    *   //
    *   // return value indicates whether textures were not found
    *   //
    *   if( failOnTexLoad )
    *   {
    *     printf("One or more textures were not found: %s\n", err.c_str() );
    *   }
    *   
    * \endcode
        * \sa CgFx, CgFxEffect::createFromFile
    */
  static NVSG_API CgFxSharedPtr createFromFile( const std::string & file,
                                                  const std::vector< std::string > & searchPaths, 
                                                  std::string & errorReturn,
                                                  bool & failOnTextureLoadInOut );

  /*! \brief Creates a CgFx Effect from a text string, and performs all appropriate 
    *        initialization, such as loading associated textures specified in the CgFx
    *        into the appropriate samplers.
    * \return
    * The function returns a CgFxSharedPtr, which might be invalid, if there was an error.
    * If it is returned invalid, check the text in "errorReturn" for the reason.
    * failOnTextureLoad signifies whether creation of the CgFx should fail if one of
    * the texture files specified in the CgFx file is not found.  If failOnTextureLoad is
    * true, and a texture is not found, createFromFile will return an invalid CgFxSharedPtr,
    * and errorReturn will contain text about which texture was not found.  If failOnTextureLoad
    * is false on the way into the method, then its value on the way out will indicate whether any
    * textures were not found.  In this case, errorReturn will also contain text indicating which
    * textures were not found.  In other words, errorReturn contains valid text if the method returns
    * an invalid SmartCgFXhandle or failOnTexLoad is true;
    * \remarks
    * The function lets you load a complete cgfx, including textures.  
    * The CgFxEffect::createFromLump method will not load textures automatically.
    * \par Example:
    * \code
    *   vector<string> searchPaths; // configure this to search paths
    *   string err;                 // will receive error in event of null return.
    *   bool failOnTexLoad = false; // don't fail if a texture was not found.
    *   CgFxSharedPtr cgfx = CgFx::createFromFile( "c:/hermannIsCool.cgfx", searchPaths, err, failOnTexLoad );
    *   [...]
    *   if( !cgfx )
    *   { 
    *     printf("I guess hermann was not cool - error: %s\n", err.c_str() );
    *     return;
    *   }
    *
    *   //
    *   // return value indicates whether textures were not found
    *   //
    *   if( failOnTexLoad )
    *   {
    *     printf("One or more textures were not found: %s\n", err.c_str() );
    *   }
    *   
    * \endcode
    */
  static NVSG_API CgFxSharedPtr createFromLump( const std::string & lump,
                                                  const std::vector< std::string > & searchPaths, 
                                                  std::string & errorReturn,
                                                  bool & failOnTextureLoadInOut );

  /*! \brief Unloads vertex and fragment programs of the embedded CgFxEffect.
    * \remarks
    * This function unloads all currently loaded vertex and fragment programs of the embedded 
    * effect. This will be necessary after the render context for which the programs were loaded
    * has been destroyed and the geometry with this CgFx attribute applied is going to be rendered 
    * to a new created render context.
    */
  NVSG_API void unloadPrograms();

  /*! \brief Overrides Object::isEquivalent.  
    * \param
    * object Pointer to the const Object to test for equivalence with this CgFx object.
    * \param
    * ignoreNames Object names will be ignored while testing if this is \c true.
    * \param
    * deepCompare The function performs a deep-compare instead of a shallow compare if this is \c true.
    * \return
    * The function returns \c true if the Object pointed to by \a object is detected to be equivalent
    * to this CgFx object.
    * \remarks
    * The test will consider the optional control parameters ignoreNames and deepCompare. 
    * If you omit these two, the function ignores the object names and performs a shallow
    * compare only.
    * \sa Object::isEquivalent
    */
  NVSG_API virtual bool isEquivalent(const Object *object, bool ignoreNames, bool deepCompare) const;

  REFLECTION_INFO_API( NVSG_API, CgFx );
protected:
  friend struct nvutil::Holder<CgFx>;

  /*! \brief Default-constructs a CgFx object.
    */
  NVSG_API CgFx();

  /*! \brief Constructs a CgFx object as a copy from another CgFx object.
    */
  NVSG_API CgFx(const CgFx& rhs);

  /*! \brief Destructs a CgFx object.
   */
  NVSG_API virtual ~CgFx(); 

  /*! \brief Determine the transparency status of this CgFx.
   *  \return \c true, if this CgFx is transparent, otherwise \c false.
   *  \remarks A CgFx is considered to be transparent, if the underlying CgFxEffect is valid, and at
   *  least one pass of the current technique is transparent. */
  NVSG_API virtual bool determineTransparencyContainment() const;

  NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

  /*! \brief Feed the data of this object into the provied HashGenerator.
   *  \param hg The HashGenerator to update with the data of this object.
   *  \sa getHashKey */
  NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

private:
  // Iterate over all samplers and assign TextureHosts
  static void loadSamplerImages(CgFxEffect *effect, const std::vector< std::string > & searchPaths,
                                int &nFailures, std::string &errorReturn, bool bFailFast);

private:
  CgFxEffectSharedPtr m_effect; // embedded effect
};

} // namespace nvsg

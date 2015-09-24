// Copyright NVIDIA Corporation 2002-2008
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

#include "nvrt/RTBase.h"
#include "nvrt/RTVariablesManager.h"
#include "nvsg/CoreTypes.h"
#include "nvutil/HashGeneratorMD5.h"
#include "nvutil/SmartPtr.h"
#include "nvgl/RenderContextGL.h"
#include "nvmath/Vecnt.h"

namespace nvtraverser
{
  class RTTraverser;
}

namespace nvrt
{

  /*! \brief Central class to create and control ray tracing objects.
   *  \par Namespace nvrt
   *  \remarks An RTContext provides the basic operations on the ray tracing framework. It is used
   *  to\n
   *  - compile a final computation kernel from the ray-tracing programs and the scene hierarchy
   *  - start the ray tracing process;
   *  - manage resources like stack size, number of devices, and programs;
   *  - manage the creation of new ray-tracing objects.
   *  \sa RTBase, RTVariablesManager
   */
  class RTContext : public RTBase, public RTVariablesManager
  {
    friend class RTMaterial;
    friend class RTTextureSampler;

    public:
      NVRT_API static RTContextSharedPtr create();

    public:
      /*! \brief Enumeration to identify a program type.
       *  \par Namespace: nvrt
       *  \remarks ProgramType specifies the three program types defined for ray tracing: miss, ray
       *  generation, and exception. */
      typedef enum
      {
        PT_MISS,                //!< Identifies the miss program.
        PT_RAY_GENERATION,      //!< Identifies the ray generation program.
        PT_EXCEPTION            //!< Identifies the exception program.
      } ProgramType;

    public :
      /*! \brief Default-constructor of RTContext */
      NVRT_API RTContext();

      /*! \brief Destructor of RTContext */
      NVRT_API virtual ~RTContext();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_CONTEXT. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Create a final computation kernel
       *  \remarks This function creates a final computation kernel from the given programs and
       *  scene hierarchy. This will be executed upon subsequent invocations of \c launch.\n
       *  Calling \c compile is not strictly necessary since any changes to the scene or the
       *  programs will cause an internal compilation upon the next \c launch call. \c compile allows
       *  the application to control when the compilation work occurs.\n
       *  Conversely, if no changes to the scene specification or programs have occurred since the
       *  last compilation, \c compile and \c launch will not perform a recompilation.
       *  \sa isTracing, launch */
      NVRT_API bool compile();

      /*! \brief Check if this RTContext is currently tracing.
       *  \return \c true, if a \c launch command is currently active, otherwise \c false.
       *  \sa launch, compile */
      NVRT_API bool isTracing() const;

      /*! \brief Execute the computation kernel of this RTContext.
       *  \param entryPointIndex The initial entry point index into the kernel.
       *  \param width The width of the computation grid.
       *  \param height The height of the computation grid. Defaults to zero for a one-dimensional grid.
       *  \param depth The depth of the computation grid. Defaults to zero for a one- or two-dimensional grid.
       *  \remarks This function initiates the execution of the computation kernel of this RTContext.
       *  If the context has not yet been compiled, or if the context has been modified since the
       *  last compile, the kernel will be recompiled internally. Acceleration structures which are
       *  marked dirty will be updated and their dirty flags will be cleared.\n
       *  Similarly, validation will occur if necessary. The ray generation program specified by
       *  \a entryPointIndex, will be invoked once for every element of the computation grid
       *  specified by \a width, \a height, and \a depth.
       *  \sa compile, isTracing */
      NVRT_API void launch( unsigned int entryPointIndex, unsigned int width, unsigned int height = 0, unsigned int depth = 0 );

      /*! \brief Set the stack size of this RTContext.
       *  \param stackSize The new stack size for this RTContext in bytes.
       *  \remarks The OptiX internal stack size will be 16-byte aligned and potentially increased to support paging.
       *  These set/getStackSize wrappers will handle the user provided value transparently though.
       *  \sa getStackSize */
      NVRT_API void setStackSize( RTsize stackSize );

      /*! \brief Get the stack size of this RTContext.
       *  \return The user defined stack size of this RTContext in bytes.
       *  \remarks The OptiX internal stack size is 16-byte aligned and potentially increased to support paging.
       *  This wrapper function will not return the internal size, but the user defined size to make the set/getStackSize
       *  functions consistent.
       *  \sa setStackSize */
      NVRT_API RTsize getStackSize() const;
      
      /*! \brief setPrintEnabled() is used to control whether text printing in OptiX programs
       *  through rtPrintf is currently enabled for this context.
       *  \param onOff Enables or disables the printing
       *  \remarks rtPrintf output is printed to the standard output.
       *  \sa setPrintLaunchIndex, setPrintBufferSize, getPrintEnabled
       */
      NVRT_API void setPrintEnabled( bool onOff );

      /*! \brief Queries whether printing from inside OptiX programs is currently enabled for this context.
       *  \sa setPrintEnabled 
       */
      NVRT_API bool getPrintEnabled() const;

      /*! \brief Sets the size of the OptiX print buffer.
       *  \param size The print buffer size in bytes. The default size is 65536 bytes.
       *  \sa setPrintEnabled, setPrintLaunchIndex
       */
      NVRT_API void setPrintBufferSize( RTsize size );

      /*! \brief Gets size of the OptiX print buffer.
       *  \sa setPrintEnabled, setPrintLaunchIndex
       */
      NVRT_API RTsize getPrintBufferSize() const;

      /*! \brief Sets the active launch index to limit rtPrintf text output to.
       *  \param index The launch index in the x, y, z dimensions to which to limit the output of rtPrintf invocations.
       *  \remarks If an index component is set to -1, output is generated for all launch indices in the that dimension.
       *  The initial value of index is (-1, -1, -1), which generates output for all indices.
       *  \sa setPrintEnabled, setPrintBufferSize, getPrintLaunchIndex
       */
      NVRT_API void setPrintLaunchIndex( nvmath::Vec3i index );

      /*! \brief Gets the active launch index to limit rtPrintf text output to.
       *  \sa setPrintLaunchIndex
       */
      NVRT_API const nvmath::Vec3i & getPrintLaunchIndex() const;

      /*! \brief setAttribute() sets p as the value of the per context attribute specified by attrib.
       *  \param attrib Attribute to set.
       *  \param size Size of the attribute being set.
       *  \param p Pointer to where the value of the attribute will be copied from. This must point to at least size bytes of memory.
       */
      NVRT_API bool setAttribute( RTcontextattribute attrib, RTsize size, void* p );

      /*! \brief getAttribute() returns in p the value of the per context attribute specified by attrib
       *  \param attrib Attribute to query.
       *  \param size Size of the attribute being queried. Parameter p must have at least this much memory backing it.
       *  \param p Return pointer where the value of the attribute will be copied into. This must point to at least size bytes of memory.
       */
      NVRT_API bool getAttribute( RTcontextattribute attrib, RTsize size, void* p ) const;

      /*! \brief setTimeoutCallback sets an application-side callback function and a time interval in seconds. 
       *  \param callback The function to be called.
       *  \param minPollingSeconds The timeout interval in seconds after which the function is called.
       *  \remarks Long-running RTCcontext calls such as compile() and launch() call the callback function about every
       *  minPollingSeconds seconds. If the callback function returns true, the API call tries to abort,
       *  leaving the context in a clean but unfinished state. Output buffers are left in an unpredictable state.
       *  In case an OptiX API call is terminated by a callback function, it returns RT TIMEOUT CALLBACK.
       *  RTtimeoutcallback is defined as int (*RTtimeoutcallback)(void).
       *  To unregister a callback function, callback needs to be set to NULL and minPollingSeconds to 0.
       *  Fails if the context is not valid, if minPollingSeconds is negative, if callback is NULL but minPollingSeconds is not 0,
       *  or if callback is not NULL but minPollingSeconds is 0.
       */
      NVRT_API bool setTimeoutCallback( RTtimeoutcallback callback, double minPollingSeconds );

      /*! \brief 
       *  \param exception The exception which is to be enabled or disabled.
       *  It may take one of the following values:
       *  RT_EXCEPTION_INDEX_OUT_OF_BOUNDS, RT_EXCEPTION_STACK_OVERFLOW, RT_EXCEPTION_BUFFER_INDEX_OUT_OF_BOUNDS,
       *  RT_EXCEPTION_INVALID_RAY, RT_EXCEPTION_INTERNAL_ERROR, RT_EXCEPTION_USER, RT_EXCEPTION_ALL
       *  \param enabled true to enable the exception, false to disable the exception.
       *  \remarks setExceptionEnabled is used to enable or disable specific exceptions.
       *  If an exception is enabled, the exception condition is checked for at runtime,
       *  and the exception program is invoked if the condition is met. 
       *  The exception program can query the type of the caught exception by calling rtGetExceptionCode (RT_EXCEPTION_ALL excluded).
       */
      NVRT_API bool setExceptionEnabled( RTexception exception, bool enabled );

      /*! \brief Query whether a specified exception is enabled.
       *  \param exception The exception which is to be enabled or disabled.
       *  It may take one of the following values:
       *  RT_EXCEPTION_INDEX_OUT_OF_BOUNDS, RT_EXCEPTION_STACK_OVERFLOW, RT_EXCEPTION_BUFFER_INDEX_OUT_OF_BOUNDS,
       *  RT_EXCEPTION_INVALID_RAY, RT_EXCEPTION_INTERNAL_ERROR, RT_EXCEPTION_USER, RT_EXCEPTION_ALL
       *  \return \c true if the exception is enabled, otherwise \c false.
       *  If exception is RT EXCEPTION ALL, true is returned only if all possible exceptions are enabled.
       */
      NVRT_API bool getExceptionEnabled( RTexception exception ) const;

      /*! \brief Query the number of ray types associated with this context.
       *  \return The number of ray types in the context.
       */
      NVRT_API unsigned int getRayTypeCount() const;

      /*! \brief Query the number of entry points for this context.
       *  \return The number of entry points in this context.
       */
      NVRT_API unsigned int getEntryPointCount() const;

      /*! \brief Specify the hardware devices to be used by the kernel.
       *  \param devices A vector of indices specifying the hardware devices.
       *  \remarks TODO: some more words on what those integers are, and what a device is!
       *  \sa getDevices */
      NVRT_API void setDevices( const std::vector<int> & devices );

      /*! \brief Get the devices used by the kernel.
       *  \return A reference to a vector of indices specifying the hardware devices.
       *  \remarks TODO: some more words on what those integers are, and what a device is!
       *  \sa setDevices */
      NVRT_API const std::vector<int> & getDevices() const;

      /*! \brief Set a program
       *  \param type The program type. Accepted values are PT_MISS, PT_RAY_GENERATION, and PT_EXCEPTION.
       *  \param id For programs of type PT_MISS, this is the ray type index. For programs of type
       *  PT_RAY_GENERATION or PT_EXCEPTION, this is the entry point index.
       *  \param program A pointer to the program to set.
       *  \remarks Any program previously set for the ProgramType \a type with ray type index or
       *  entry point index \a id is removed.
       *  \sa getNumberOfPrograms, getProgramId, getProgram, getProgramById */
      NVRT_API void setProgram( ProgramType type, unsigned int id, const RTProgramWeakPtr & program );

      /*! \brief Get the number of programs of a specified type.
       *  \param type The program type. Accepted values are PT_MISS, PT_RAY_GENERATION, and PT_EXCEPTION.
       *  \return The number of programs of the specified type in this RTContext.
       *  \sa setProgram, getProgramId, getProgram, getProgramById */
      NVRT_API unsigned int getNumberOfPrograms( ProgramType type ) const;

      /*! \brief Get the program id of the program specified by type and index.
       *  \param type The program type. Accepted values are PT_MISS, PT_RAY_GENERATION, and PT_EXCEPTION.
       *  \param index The index of the program.
       *  \return The id of the program, which is the ray type index if \a type is PT_MISS, and the
       *  entry point index if \a type is PT_RAY_GENERATION or PT_EXCEPTION.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of programs
       *  of the specified type in this RTContext. 
       *  \sa setProgram, getNumberOfPrograms, getProgram, getProgramById */
      NVRT_API unsigned int getProgramId( ProgramType type, unsigned int index ) const;

      /*! \brief Get the program specified by type and index.
       *  \param type The program type. Accepted values are PT_MISS, PT_RAY_GENERATION, and PT_EXCEPTION.
       *  \param index The index of the program.
       *  \return A pointer to the specified program.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of programs
       *  of the specified type in this RTContext. 
       *  \sa setProgram, getNumberOfPrograms, getProgramId, getProgramById */
      NVRT_API RTProgramWeakPtr getProgram( ProgramType type, unsigned int index ) const;

      /*! \brief Get the program specified by type and id.
       *  \param type The program type. Accepted values are PT_MISS, PT_RAY_GENERATION, and PT_EXCEPTION.
       *  \param id For programs of type PT_MISS, this is the ray type index. For programs of type
       *  PT_RAY_GENERATION or PT_EXCEPTION, this is the entry point index.
       *  \return A pointer to the specified program, or NULL if there is none.
       *  \sa setProgram, getNumberOfPrograms, getProgramId, getProgram */
      NVRT_API RTProgramWeakPtr getProgramById( ProgramType type, unsigned int id ) const;

      /*! \brief Create a new program from a file containing PTX code.
       *  \param fileName The name of the file containing the PTX source for the program to create.
       *  \param programName The name of the program to create.
       *  \return A pointer to the newly created program.
       *  \sa getProgramFromPTXString, getProgramFromCUDAFile, getProgramFromCUDAString */
      NVRT_API RTProgramSharedPtr getProgramFromPTXFile( const std::string & fileName, const std::string & programName );

      /*! \brief Create a new program from a string containing PTX code.
       *  \param ptxString The string containing the PTX source for the program to create.
       *  \param programName The name of the program to create.
       *  \return A pointer to the newly created program.
       *  \sa getProgramFromPTXFile, getProgramFromCUDAFile, getProgramFromCUDAString */
      NVRT_API RTProgramSharedPtr getProgramFromPTXString( const std::string & ptxString, const std::string & programName ); 

      /*! \brief Create a new program from a file containing CUDA code.
       *  \param fileName The name of the file containing the CUDA source for the program to create.
       *  \param programName The name of the program to create.
       *  \return A pointer to the newly created program.
       *  \sa getProgramFromPTXFile, getProgramFromPTXString, getProgramFromCUDAString */
      NVRT_API RTProgramSharedPtr getProgramFromCUDAFile( const std::string & fileName, const std::string & programName );

      /*! \brief Create a new program from a string containing CUDA code.
       *  \param cudaString The string containing the CUDA source for the program to create.
       *  \param programName The name of the program to create.
       *  \return A pointer to the newly created program.
       *  \sa getProgramFromPTXFile, getProgramFromPTXString, getProgramFromCUDAFile */
      NVRT_API RTProgramSharedPtr getProgramFromCUDAString( const std::string & cudaString, const std::string & programName );

      /*! \brief Create a new RTAcceleration.
       *  \return A pointer to the newly created RTAcceleration. */
      NVRT_API RTAccelerationSharedPtr newAcceleration();

      /*! \brief Create a new RTBuffer.
       *  \return A pointer to the newly created RTBuffer. */
      NVRT_API RTBufferSharedPtr newBuffer( RTbuffertype type );

      /*! \brief Create a new RTBufferGL.
       *  \return A pointer to the newly created RTBufferGL. */
      NVRT_API RTBufferGLSharedPtr newBufferGL( RTbuffertype type );

      /*! \brief Create a new RTBufferGLTexture
       *  \return A pointer to the newly created RTBufferGLTexture. */
      NVRT_API RTBufferGLTextureSharedPtr newBufferGLTexture( RTbuffertype type, unsigned int internalFormat, unsigned int format, unsigned int dataType );

      /*! \brief Create a new RTGeometry.
       *  \return A pointer to the newly created RTGeometry. */
      NVRT_API RTGeometrySharedPtr newGeometry();

      /*! \brief Create a new RTGeometryGroup.
       *  \return A pointer to the newly created RTGeometryGroup. */
      NVRT_API RTGeometryGroupSharedPtr newGeometryGroup();

      /*! \brief Create a new RTGeometryInstance.
       *  \return A pointer to the newly created RTGeometryInstance. */
      NVRT_API RTGeometryInstanceSharedPtr newGeometryInstance();

      /*! \brief Create a new RTGroup.
       *  \return A pointer to the newly created RTGroup. */
      NVRT_API RTGroupSharedPtr newGroup();

      /*! \brief Create a new RTMaterial.
       *  \return A pointer to the newly created RTMaterial. */
      NVRT_API RTMaterialSharedPtr newMaterial();

      /*! \brief Create a new RTSelector.
       *  \return A pointer to the newly created RTSelector. */
      NVRT_API RTSelectorSharedPtr newSelector();

      /*! \brief Create a new RTTextureSampler.
       *  \return A pointer to the newly created RTTextureSampler. */
      NVRT_API RTTextureSamplerSharedPtr newTextureSampler();

      /*! \brief Create a new RTTextureSampler from a TextureAttributeItem.
       *  \param preserveTexture If \c true will cause the textures image data to be preserved after upload, if \c false,
       *         the TextureHost's F_PRESERVE_IMAGE_DATA_AFTER_UPLOAD flag will be honored.
       *  \return A pointer to the newly created RTTextureSampler. */
      NVRT_API RTTextureSamplerSharedPtr newTextureSampler( const nvsg::TextureAttributeItem * item, bool preserveTexture = false );

      /*! \brief Create a new RTTextureSampler from a RTTransform.
       *  \return A pointer to the newly created RTTransform. */
      NVRT_API RTTransformSharedPtr newTransform();

      /*! \brief Set the caching directory.
       *  \param cacheDir The caching directory to be used for compiled PTX strings.
       *  \remarks Compiling a CUDA string to a PTX string is a relatively expensive operation. The
       *  compiled strings are cached in generated files in the \a cacheDir directory to reduce
       *  recompilation efforts of already compiled CUDA strings.
       *  \note The default for the cache director is './caches/'.
       *  \sa getCacheDir */
      NVRT_API void setCacheDir( const std::string & cacheDir );

      /*! \brief Get the caching directory.
       *  \return A reference to the constant string specifying the caching directory.
       *  \sa setCacheDir */
      NVRT_API const std::string & getCacheDir() const;

      /*! \brief Set the include directory.
       *  \param includeDir The include directory to be used in compiling CUDA strings to PTX.
       *  \sa getIncludeDir */
      NVRT_API void setIncludeDir( const std::string & includeDir );

      /*! \brief Get the include directory.
       *  \return A reference to the constant string specifying the include directory.
       *  \sa setIncludeDir */
      NVRT_API const std::string & getIncludeDir() const;

      /*! \brief Setup OpenGL context which can be used for resource cleanup
          \param renderContextGL A RenderContextGL shared with the RenderContextGL which was active during construction of this object
       **/
      NVRT_API void setCleanupRenderContextGL( const nvgl::SmartRenderContextGL &renderContextGL );

      /*! \brief Retrieve OpenGL context which is being used for cleanup
          \returns RenderContextGL shared with the RenderContextGL which was active during construction of this object
       **/
      NVRT_API nvgl::SmartRenderContextGL getCleanupRenderContextGL() const;

      /*! \brief Retrieve RTTraverser which should be used by this context.
       */
      NVRT_API nvutil::SmartPtr<nvtraverser::RTTraverser> getRTTraverser() const;

    protected:
      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTContext wraps the functionality of an nvirt::RTcontext. This function is
       *  used internally for accessing that resource. */
      NVRT_API RTcontext getResource() const;

      /*! \brief Add a ray type index to use.
       *  \param rt The ray type index to add.
       *  \remarks If \a rt is larger than the largest ray type index added before, the ray type
       *  count is adjusted accordingly.
       *  \sa addEntryPoint */
      NVRT_API void addRayType( unsigned int rt );

      /*! \brief Add an entry point index to use.
       *  \param ep The entry point index to add.
       *  \remarks If \a ep is larger than the largest entry point index added before, the entry
       *  point count is adjusted accordingly.
       *  \sa addRayType */
      NVRT_API void addEntryPoint( unsigned int ep );

      /*! \brief Declare a variable.
       *  \param name The name of the variable to declare.
       *  \return The newly declared variable.
       *  \remarks This function is called by the framework to declare a variable in this RTContext.
       *  \sa RTVariablesManager */
      NVRT_API virtual RTvariable doDeclareVariable( const std::string & name );

      /*! \brief Remove a variable.
       *  \param v The variable to remove.
       *  \remarks This function is called by the framework to remove a variable in this RTContext.
       *  \sa RTVariablesManager */
      NVRT_API virtual void doRemoveVariable( RTvariable v );

      /*! \brief Query a variable.
       *  \param name The name of the variable to query.
       *  \return The queried variable.
       *  \remarks This function is called by the framework to query in this RTContext.
       *  \sa RTVariablesManager */
      NVRT_API virtual RTvariable doQueryVariable( const std::string & name ) const;

      /*! \brief Get the number of variables in this RTContext.
       *  \return The number of variables in this RTContext.
       *  \remarks This function is called by the framework to get the number of variables in this
       *  RTContext.
       *  \sa RTVariablesManager */
      NVRT_API virtual unsigned int doGetVariableCount() const;

      /*! \brief Get a specified variable from this RTContext.
       *  \param index The index of the variable to get.
       *  \return The specified variable.
       *  \remarks This function is called by the framework to get a specific variable in this
       *  RTContext.
       *  \sa RTVariablesManager */
      NVRT_API virtual RTvariable doGetVariable( unsigned int index ) const;

    private:
      typedef std::pair<unsigned int, RTProgramSharedPtr> RTProgramSpec;
      typedef std::map<std::string,RTProgramSharedPtr >   NameRTProgramMap;
      typedef std::map<std::string,NameRTProgramMap>      RTProgramMap;

    private:
      NVRT_API RTProgramWeakPtr findRTProgram( const std::string & firstKey, const std::string & programName );
      NVRT_API void loadFileToString( const std::string & fileName, std::string & fileString );
      NVRT_API RTProgramSharedPtr newProgramFromCUDAString( const std::string & cudaString, const std::string & programName );
#if !defined(NDEBUG)
      NVRT_API bool checkProgram( ProgramType type, unsigned int index, const RTProgramWeakPtr & program ) const;
#endif

    private:
      std::string                 m_cacheDir;
      RTcontext                   m_context;
      std::vector<int>            m_devices;
      nvutil::HashGeneratorMD5    m_hashGenerator;
      std::string                 m_includeDir;
      unsigned int                m_maxEntryPoint;
      unsigned int                m_maxRayType;
      std::vector<RTProgramSpec>  m_programs[PT_EXCEPTION+1];   // one vector per program type !
      RTProgramMap                m_rtPrograms;                 // shares all kind of sources of RTPrograms !!
      RTsize                      m_stackSize;
      bool                        m_printEnabled;
      RTsize                      m_printBufferSize;
      nvmath::Vec3i               m_printLaunchIndex;
      nvgl::SmartRenderContextGL  m_renderContextGL;
      nvutil::SmartPtr<nvtraverser::RTTraverser> m_rtTraverser;  // the only one which should be used for a context
  };
}

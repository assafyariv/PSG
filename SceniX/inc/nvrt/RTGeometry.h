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

#include "optix.h"
#include "nvrt/RTBase.h"
#include "nvrt/RTVariablesManager.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Wrapper class for an nvirt RTgeometry object.
   *  \par Namespace: nvrt
   *  \remarks An RTGeometry represents a piece of geometry, together with the corresponding
   *  programs defined on that geometry (bounding box program and intersection program), and the
   *  associated variable values.
   *  \sa RTBase, RTVariablesManager */
  class RTGeometry : public RTBase, public RTVariablesManager
  {
    friend class RTContext;
    friend class RTGeometryInstance;

    public:
      /*! \brief Enumeration to identify a program type.
       *  \par Namespace: nvrt
       *  \remarks ProgramType specifies the two program types defined for ray tracing on geometry:
       *  bounding box and intersection. */
      typedef enum
      {
        PT_BOUNDING_BOX,        //!< Identifies the bounding box program
        PT_INTERSECTION,        //!< Identifies the intersection program
      } ProgramType;

    public:
      NVRT_API static RTGeometrySharedPtr create();

    public:
      /*! \brief Default-constructor of RTGeometry */
      NVRT_API RTGeometry();

      /*! \brief Destructor of RTGeometry */
      NVRT_API virtual ~RTGeometry();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_GEOMETRY. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief markDirty() sets the dirty flag for this geometry. 
       *  By default the dirty flag is set for a new geometry node.
       *  After a call to RTContext::launch() the flag is automatically cleared.
       *  When the dirty flag is set, the geometry data is uploaded automatically
       *  to the device during an RTContext::launch() call.
       */
      NVRT_API void markDirty();

      /*! \brief Query the geometry's dirty flag.

       *  \return \c true if the geometry is dirty, otherwise \c false
       */
      NVRT_API bool isDirty() const;

      /*! \brief Set the number of primitives.
       *  \param pc The number of primitives.
       *  \remark The geometry has to know how many primitives are to be handled. The actual
       *  primitives are specified by some variable of the geometry.
       *  \sa getPrimitiveCount */
      NVRT_API void setPrimitiveCount( unsigned int pc );

      /*! \brief Get the number of primitives.
       *  \return The number of primitives.
       *  \remark The geometry knows how many primitives are to be handled. The actual primitives
       *  are specified by some variable of the geometry.
       *  \sa setPrimitiveCount */
      NVRT_API unsigned int getPrimitiveCount() const;

      /*! \brief Set the program for a specified program type.
       *  \param type Specifies the type of program to set.
       *  \param program A pointer to the program to set.
       *  \remarks For a geometry, two types of programs are supported: bounding box, and
       *  intersection.
       *  \sa getProgram */
      NVRT_API void setProgram( ProgramType type, const RTProgramWeakPtr & program );

      /*! \brief Get the program of a specified program type.
       *  \param type Specifies the type of program to get.
       *  \return A pointer to the specified program, or NULL, if there is no such program.
       *  \remarks For a geometry, two types of programs are supported: bounding box, and
       *  intersection.
       *  \sa setProgram */
      NVRT_API RTProgramWeakPtr getProgram( ProgramType type ) const;

    protected :
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the geometry with.
       *  \remarks This function is called from RTContext::newGeometry to initialize the new
       *  geometry.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTGeometry wraps the functionality of an nvirt::RTgeometry. This function is
       *  used internally for accessing that resource. */
      NVRT_API RTgeometry getResource() const;

      /*! \brief Declare a variable.
       *  \param name The name of the variable to declare.
       *  \return The newly declared variable.
       *  \remarks This function is called by the framework to declare a variable in this RTGeometry.
       *  \sa RTVariablesManager */
      NVRT_API virtual RTvariable doDeclareVariable( const std::string & name );

      /*! \brief Remove a variable.
       *  \param v The variable to remove.
       *  \remarks This function is called by the framework to remove a variable in this RTGeometry.
       *  \sa RTVariablesManager */
      NVRT_API virtual void doRemoveVariable( RTvariable v );

      /*! \brief Query a variable.
       *  \param name The name of the variable to query.
       *  \return The queried variable.
       *  \remarks This function is called by the framework to query in this RTGeometry.
       *  \sa RTVariablesManager */
      NVRT_API virtual RTvariable doQueryVariable( const std::string & name ) const;

      /*! \brief Get the number of variables in this RTGeometry.
       *  \return The number of variables in this RTGeometry.
       *  \remarks This function is called by the framework to get the number of variables in this
       *  RTGeometry.
       *  \sa RTVariablesManager */
      NVRT_API virtual unsigned int doGetVariableCount() const;

      /*! \brief Get a specified variable from this RTGeometry.
       *  \param index The index of the variable to get.
       *  \return The specified variable.
       *  \remarks This function is called by the framework to get a specific variable in this
       *  RTGeometry.
       *  \sa RTVariablesManager */
      NVRT_API virtual RTvariable doGetVariable( unsigned int index ) const;

    private:
#if !defined(NDEBUG)
      NVRT_API bool checkProgram( RTGeometry::ProgramType type, const RTProgramSharedPtr & program ) const;
#endif

    private:
      RTgeometry            m_geometry;
      unsigned int          m_primitiveCount;
      RTProgramSharedPtr  m_programs[PT_INTERSECTION+1];    // one vector per program type !
  };
}


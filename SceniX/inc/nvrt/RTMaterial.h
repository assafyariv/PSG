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
#include "nvrt/RTProgram.h"
#include "nvrt/RTVariablesManager.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Wrapper class for an nvirt RTmaterial object.
   *  \par Namespace: nvrt
   *  \remarks An RTMaterial represents the a material node managing two programs (closest_hit and
   *  any_hit) as well as the associated variables.
   *  \sa RTBase, RTVariablesManager, RTContext */
  class RTMaterial : public RTBase, public RTVariablesManager
  {
    friend class RTContext;
    friend class RTGeometryInstance;

    public:
      /*! \brief Enumeration to identify a program type.
       *  \par Namespace: nvrt
       *  \remarks ProgramType specifies the two program types defined for ray tracing on material:
       *  closest hit and any hit. */
      enum ProgramType
      {
        PT_CLOSEST_HIT,         //!< Identifies the closest hit program
        PT_ANY_HIT              //!< Identifies the any hit program
      };

    public:
      NVRT_API static RTMaterialSharedPtr create();

    public:
      /*! \brief Default-constructor of RTMaterial */
      NVRT_API RTMaterial();

      /*! \brief Destructor of RTMaterial */
      NVRT_API ~RTMaterial();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_MATERIAL. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Set a program
       *  \param type The program type. Accepted values are PT_CLOSEST_HIT and PT_ANY_HIT.
       *  \param id The ray type index of the program.
       *  \param program A pointer to the program to set.
       *  \remarks Any program previously set for the ProgramType \a type with ray type index \a id
       *  is removed.
       *  \sa getNumberOfPrograms, getProgramId, getProgram, getProgramById, clearPrograms */
      NVRT_API void setProgram( ProgramType type, unsigned int id, const RTProgramWeakPtr & program );

      /*! \brief Set a program
       *  \param type The program type. Accepted values are PT_CLOSEST_HIT and PT_ANY_HIT.
       *  \param id The ray type index of the program.
       *  \param program A pointer to the program to set.
       *  \remarks Any program previously set for the ProgramType \a type with ray type index \a id
       *  is removed.
       *  \sa getNumberOfPrograms, getProgramId, getProgram, getProgramById, clearPrograms */
      NVRT_API void setProgram( ProgramType type, unsigned int id, const RTProgramSharedPtr & program );

      /*! \brief Clears all programs.
       *  \remarks Any programs previously set on this material are removed, and probably deleted.
       *  \sa getNumberOfPrograms, getProgramId, getProgram, getProgramById */
      NVRT_API void clearPrograms();

      /*! \brief Get the number of programs of a specified type.
       *  \param type The program type. Accepted values are PT_CLOSEST_HIT and PT_ANY_HIT.
       *  \return The number of programs of the specified type in this RTMaterial.
       *  \sa setProgram, getProgramId, getProgram, getProgramById */
      NVRT_API unsigned int getNumberOfPrograms( ProgramType type ) const;

      /*! \brief Get the program id of the program specified by type and index.
       *  \param type The program type. Accepted values are PT_CLOSEST_HIT and PT_ANY_HIT.
       *  \param index The index of the program.
       *  \return The ray type index of the specified program.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of programs
       *  of the specified type in this RTContext. 
       *  \sa setProgram, getNumberOfPrograms, getProgram, getProgramById */
      NVRT_API unsigned int getProgramId( ProgramType type, unsigned int index ) const;

      /*! \brief Get the program specified by type and index.
       *  \param type The program type. Accepted values are PT_CLOSEST_HIT and PT_ANY_HIT.
       *  \param index The index of the program.
       *  \return A pointer to the specified program.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of programs
       *  of the specified type in this RTContext. 
       *  \sa setProgram, getNumberOfPrograms, getProgramId, getProgramById */
      NVRT_API RTProgramWeakPtr getProgram( ProgramType type, unsigned int index ) const;

      /*! \brief Get the program specified by type and id.
       *  \param type The program type. Accepted values are PT_CLOSEST_HIT and PT_ANY_HIT.
       *  \param id The ray type index of the program to get.
       *  \return A pointer to the specified program, or NULL if there is none.
       *  \sa setProgram, getNumberOfPrograms, getProgramId, getProgram */
      NVRT_API RTProgramWeakPtr getProgramById( ProgramType type, unsigned int id ) const;

    protected:
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the material with.
       *  \remarks This function is called from RTContext::newMaterial to initialize the new material.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTContext * context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTMaterial wraps the functionality of an nvirt::RTmaterial.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTmaterial getResource() const;

      /*! \brief Declare a variable.
       *  \param name The name of the variable to declare.
       *  \return The newly declared variable.
       *  \remarks This function is called by the framework to declare a variable in this RTMaterial.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doDeclareVariable( const std::string & name );

      /*! \brief Remove a variable.
       *  \param v The variable to remove.
       *  \remarks This function is called by the framework to remove a variable in this RTMaterial.
       *  \sa RTVariablesManager */
      NVRT_API void doRemoveVariable( RTvariable v );

      /*! \brief Query a variable.
       *  \param name The name of the variable to query.
       *  \return The queried variable.
       *  \remarks This function is called by the framework to query in this RTMaterial.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doQueryVariable( const std::string & name ) const;

      /*! \brief Get the number of variables in this RTMaterial.
       *  \return The number of variables in this RTMaterial.
       *  \remarks This function is called by the framework to get the number of variables in this
       *  RTMaterial.
       *  \sa RTVariablesManager */
      NVRT_API unsigned int doGetVariableCount() const;

      /*! \brief Get a specified variable from this RTMaterial.
       *  \param index The index of the variable to get.
       *  \return The specified variable.
       *  \remarks This function is called by the framework to get a specific variable in this
       *  RTMaterial.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doGetVariable( unsigned int index ) const;

    private:
      typedef std::pair<unsigned int, RTProgramSharedPtr>  RTProgramSpec;

    private:
#if !defined(NDEBUG)
      NVRT_API bool checkProgram( ProgramType type, unsigned int index, const RTProgramWeakPtr & program ) const;
#endif

    private:
      RTContext                   * m_context;
      RTmaterial                    m_material;
      std::vector<RTProgramSpec>    m_programs[PT_ANY_HIT+1];   // one vector per program type !
  };
}


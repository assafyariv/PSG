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
  /*! \brief Wrapper class for an NVIRT RTprogram object.
   *  \par Namespace: nvrt
   *  \remarks An RTProgram represents a program and manages the associated variables.
   *  \sa RTBase, RTVariablesManager, RTContext */
  class RTProgram : public RTBase, public RTVariablesManager
  {
    friend class RTContext;
    friend class RTGeometry;
    friend class RTMaterial;
    friend class RTSelector;

    public:
      NVRT_API static RTProgramSharedPtr create();

    public:
      /*! \brief Default-constructor of RTProgram */
      NVRT_API RTProgram();

      /*! \brief Destructor of RTProgram */
      NVRT_API virtual ~RTProgram();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_PROGRAM. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

    protected:
      /*! \brief Create an RTProgram from a file.
       *  \param context The context to create the new program in.
       *  \param fileName The name of the file containing the program.
       *  \param programName The name of the program to create.
       *  \remarks This function is called from RTContext::getProgramFromPTXFile to create and
       *  initialize the new program.
       *  \sa createFromString, RTContext */
      NVRT_API static RTProgramSharedPtr createFromFile( RTcontext context, const std::string & fileName, const std::string & programName );

      /*! \brief Create an RTProgram from a string.
       *  \param context The context to create the new program in.
       *  \param ptxString The string containing the program.
       *  \param programName The name of the program to create.
       *  \remarks This function is called from RTContext::getProgramFromPTXString to create and
       *  initialize the new program.
       *  \sa createFromFile, RTContext */
      NVRT_API static RTProgramSharedPtr createFromString( RTcontext context, const std::string & ptxString, const std::string & programName );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTProgram wraps the functionality of an nvirt::RTprogram.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTprogram getResource() const;

      /*! \brief Declare a variable.
       *  \param name The name of the variable to declare.
       *  \return The newly declared variable.
       *  \remarks This function is called by the framework to declare a variable in this RTProgram.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doDeclareVariable( const std::string & name );

      /*! \brief Remove a variable.
       *  \param v The variable to remove.
       *  \remarks This function is called by the framework to remove a variable in this RTProgram.
       *  \sa RTVariablesManager */
      NVRT_API void doRemoveVariable( RTvariable v );

      /*! \brief Query a variable.
       *  \param name The name of the variable to query.
       *  \return The queried variable.
       *  \remarks This function is called by the framework to query in this RTProgram.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doQueryVariable( const std::string & name ) const;

      /*! \brief Get the number of variables in this RTProgram.
       *  \return The number of variables in this RTProgram.
       *  \remarks This function is called by the framework to get the number of variables in this
       *  RTProgram.
       *  \sa RTVariablesManager */
      NVRT_API unsigned int doGetVariableCount() const;

      /*! \brief Get a specified variable from this RTProgram.
       *  \param index The index of the variable to get.
       *  \return The specified variable.
       *  \remarks This function is called by the framework to get a specific variable in this
       *  RTProgram.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doGetVariable( unsigned int index ) const;

    private:
      RTprogram m_program;
  };
}


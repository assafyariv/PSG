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
#include "nvsgrtapi.h"

namespace nvrt
{
  class RTVariable;

  /*! \brief Base class to manage a set of variables.
   *  \par Namespace: nvrt
   *  \remarks An RTVariablesManager provides the functionality to manage the handling of variables.
   *  RTContext, RTGeometry, RTGeometryInstance, RTMaterial, RTProgram, and RTSelector derive from
   *  RTChildrenManager to have a unique interface on that task. Those classes need to provide the pure
   *  virtual functions that finally perform the required operations.
   *  \sa RTContext, RTGeometry, RTGeometryInstance, RTMaterial, RTProgram, RTSelector */
  class RTVariablesManager
  {
    public:
      /*! \brief Templated function to create a new variable with a specified name.
       *  \param name The name of the new variable.
       *  \returns A pointer to the new variable.
       *  \remarks To create a variable of a specific type, you have to provide that type explicitly as
       *  the template parameter, like the following:\n
       *    ctx->newVariable<RTVariableBuffer>( "outputBuffer" );
       *  \sa deleteVariable, getVariable, getNumberOfVariable, clearVariables */
      template<typename Var> Var * newVariable( const std::string & name );

      /*! \brief Delete a variable.
       *  \param v The variable to delete.
       *  \remarks Variables in the array of variables with an index larger than the index of the variable
       *  to delete, are shifted one down in that array.
       *  \sa newVariable, getVariable, getNumberOfVariable, clearVariables */
      NVRT_API void deleteVariable( RTVariable * v );

      /*! \brief Get a variable by name.
       *  \param name The name of the variable to get.
       *  \remarks If there is no variable with that name, a NULL pointer is returned.
       *  \sa newVariable, deleteVariable, getNumberOfVariables, clearVariables */
      NVRT_API RTVariable * getVariable( const std::string & name ) const;

      /*! \brief Get the number of variables.
       *  \return The number of variables.
       *  \sa newVariable, deleteVariable, getVariable, clearVariables */
      NVRT_API unsigned int getNumberOfVariables() const;

      /*! \brief Get a variable by index.
       *  \param index The index of the variable to get.
       *  \note the behaviour is undefined if \a index is larger or equal to the number of variables.
       *  \sa newVariable, deleteVariable, getNumberOfVariables, clearVariables */
      NVRT_API RTVariable * getVariable( unsigned int index ) const;

      /*! \brief Clear the array of variables
       *  \remarks All variables are deleted.
       *  \sa newVariable, deleteVariable, getVariable, getNumberOfVariables */
      NVRT_API void clearVariables();

    protected:
      /*! \brief Default-constructor of RTVariablesManager */
      NVRT_API RTVariablesManager();

      /*! \brief Destructor of RTVariablesManager */
      NVRT_API virtual ~RTVariablesManager();

      /*! \brief Interface to declare a variable of a specified name.
       *  \param name The name of the variable to declare.
       *  \remarks This function is called from \s newVariable to declare a new variable.
       *  \sa newVariable */
      NVRT_API virtual RTvariable doDeclareVariable( const std::string & name ) = 0;

      /*! \brief Interface to remove a variable.
       *  \param v The variable to remove.
       *  \remarks This function is called by \s deleteVariable to remove that variable.
       *  \sa deleteVariable */
      NVRT_API virtual void doRemoveVariable( RTvariable v ) = 0;

      /*! \brief Interface to query a variable by name.
       *  \param name The name of the variable to query.
       *  \returns The variable named \a name, or NULL, is there is none. */
      NVRT_API virtual RTvariable doQueryVariable( const std::string & name ) const = 0;

      /*! \brief Interface to get the number of variables.
       *  \returns The number of variables. */
      NVRT_API virtual unsigned int doGetVariableCount() const = 0;

      /*! \brief Interface to get a variable by index.
       *  \param index The index of the variable to get.
       *  \returns The variable at the specified index. */
      NVRT_API virtual RTvariable doGetVariable( unsigned int index ) const = 0;

    private:
      NVRT_API bool checkNameIncluded( const std::string & name );

    private:
      std::vector<RTVariable*>  m_variables;
  };

  template<typename Var>
  inline Var * RTVariablesManager::newVariable( const std::string & name )
  {
    NVSG_ASSERT( !checkNameIncluded( name ) );
    RTvariable v = doDeclareVariable( name );
    Var * var = new Var( v );
    NVSG_ASSERT( var );
    m_variables.push_back( var );
    return( var );
  }
}


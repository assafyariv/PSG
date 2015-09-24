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
#include "nvrt/RTChildrenManager.h"
#include "nvrt/RTNode.h"
#include "nvrt/RTVariablesManager.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Wrapper class for an nvirt RTselector object.
   *  \par Namespace: nvrt
   *  \remarks An RTselector represents the a selector node managing some RTNodes as children and a
   *  visit program that selects zero or more of those children for traversal, as well as the
   *  associated variables.
   *  \sa RTNode, RTVariablesManager, RTAccelerationManager, RTChildrenManager, RTNode */
  class RTSelector : public RTNode, public RTVariablesManager, public RTChildrenManager<RTNode>
  {
    friend class RTContext;
    friend class RTVariableType<RTSelector>;

    public:
      NVRT_API static RTSelectorSharedPtr create();

    public:
      /*! \brief Default-constructor of RTSelector */
      NVRT_API RTSelector();

      /*! \brief Destructor of RTSelector */
      NVRT_API virtual ~RTSelector();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_SELECTOR. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Set the visit program for this RTSelector.
       *  \param program A pointer to the visit program to set.
       *  \remarks The visit program is used to select zero or more of the children of this
       *  RTSelector for traversal.
       *  \sa getVisitProgram, RTProgram */
      NVRT_API void setVisitProgram( const RTProgramWeakPtr & program );

      /*! \brief Set the visit program for this RTSelector.
       *  \param program A pointer to the visit program to set.
       *  \remarks The visit program is used to select zero or more of the children of this
       *  RTSelector for traversal.
       *  \sa getVisitProgram, RTProgram */
      NVRT_API void setVisitProgram( const RTProgramSharedPtr & program );

      /*! \brief Get the visit program of this RTSelector.
       *  \return A pointer to the visit program of this RTSelector.
       *  \remarks The visit program is used to select zero or more of the children of this
       *  RTSelector for traversal.
       *  \sa setVisitProgram, RTProgram */
      NVRT_API RTProgramWeakPtr getVisitProgram() const;

    protected :
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the selector with.
       *  \remarks This function is called from RTContext::newSelector to initialize the new selector.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTSelector wraps the functionality of an nvirt::RTselector.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTobject getResource() const;

      /*! \brief Declare a variable.
       *  \param name The name of the variable to declare.
       *  \return The newly declared variable.
       *  \remarks This function is called by the framework to declare a variable in this RTSelector.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doDeclareVariable( const std::string & name );

      /*! \brief Remove a variable.
       *  \param v The variable to remove.
       *  \remarks This function is called by the framework to remove a variable in this RTSelector.
       *  \sa RTVariablesManager */
      NVRT_API void doRemoveVariable( RTvariable v );

      /*! \brief Query a variable.
       *  \param name The name of the variable to query.
       *  \return The queried variable.
       *  \remarks This function is called by the framework to query in this RTSelector.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doQueryVariable( const std::string & name ) const;

      /*! \brief Get the number of variables in this RTSelector.
       *  \return The number of variables in this RTSelector.
       *  \remarks This function is called by the framework to get the number of variables in this
       *  RTSelector.
       *  \sa RTVariablesManager */
      NVRT_API unsigned int doGetVariableCount() const;

      /*! \brief Get a specified variable from this RTSelector.
       *  \param index The index of the variable to get.
       *  \return The specified variable.
       *  \remarks This function is called by the framework to get a specific variable in this
       *  RTSelector.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doGetVariable( unsigned int index ) const;

      /*! \brief Get this object as an RTNode.
       *  \return A this object as an RTNode.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTChildrenManager.
       *  \sa RTChildrenManager */
      NVRT_API virtual RTNodeWeakPtr doGetNode() const;

      /*! \brief Get the specified child.
       *  \param index The index of the child to get.
       *  \return The specified child as an RTobject.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTChildrenManager.
       *  \sa doGetNumberOfChildren, doSetNumberOfChildren, doSetChildResource, RTChildrenManager */
      NVRT_API virtual RTobject doGetChildResource( unsigned int index ) const;

      /*! \brief Get the number of children of this RTSelector.
       *  \return The number of children of this RTSelector.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTChildrenManager.
       *  \sa doGetChildResource, doSetChildResource, doSetNumberOfChildren, RTChildrenManager */
      NVRT_API virtual unsigned int doGetNumberOfChildren() const;

      /*! \brief Set a child at the specified position.
       *  \param index The index where to set the child.
       *  \param child The child to set.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTChildrenManager.
       *  \sa doGetChildResource, doGetNumberOfChildren, doSetNumberOfChildren, RTChildrenManager */
      NVRT_API virtual void doSetChildResource( unsigned int index, RTobject child );

      /*! \brief Set the number of children.
       *  \param count The number of children to set.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTChildrenManager.
       *  \sa doGetChildResource, doGetNumberOfChildren, doSetChildResource, RTChildrenManager */
      NVRT_API virtual void doSetNumberOfChildren( unsigned int numberOfChildren );

    private:
#if !defined(NDEBUG)
      NVRT_API bool checkVisitProgram( const RTProgramSharedPtr & visitProgram ) const;
#endif

    private:
      std::vector<RTNodeWeakPtr>  m_children;
      RTselector                  m_selector;
      RTProgramSharedPtr          m_visitProgram;
  };
}


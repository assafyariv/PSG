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
#include "nvrt/RTAccelerationManager.h"
#include "nvrt/RTChildrenManager.h"
#include "nvrt/RTNode.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Wrapper class for an nvirt RTgroup object.
   *  \par Namespace: nvrt
   *  \remarks An RTGroup represents the a grouping node holding some RTNodes as children, and an
   *  acceleration structure.
   *  \sa RTNode, RTAccelerationManager, RTChildrenManager, RTNode */
  class RTGroup : public RTNode, public RTAccelerationManager, public RTChildrenManager<RTNode>
  {
    friend class RTContext;
    friend class RTVariableType<RTGroup>;

    public:
      NVRT_API static RTGroupSharedPtr create();

    public:
      /*! \brief Default-constructor of RTGroup */
      NVRT_API RTGroup();

      /*! \brief Destructor of RTGroup */
      NVRT_API virtual ~RTGroup();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_GROUP. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

    protected:
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the group with.
       *  \remarks This function is called from RTContext::newGroup to initialize the new group.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTGroup wraps the functionality of an nvirt::RTgroup.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTobject getResource() const;

      /*! \brief Get this object as an RTNode.
       *  \return A this object as an RTNode.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTAccelerationManager and RTChildrenManager.
       *  \sa RTAccelerationManager, RTChildrenManager */
      NVRT_API virtual RTNodeWeakPtr doGetNode() const;

      /*! \brief Get the acceleration structure associated with this group.
       *  \return The acceleration structure associated with this group.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTAccelerationManager.
       *  \sa doSetAccelerationResource, RTAcceleration, RTAccelerationManager */
      NVRT_API virtual RTacceleration doGetAccelerationResource() const;

      /*! \brief Set the acceleration structure for this group.
       *  \param accel The acceleration structure to set.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTAccelerationManager.
       *  \sa doGetAccelerationResource, RTAcceleration, RTAccelerationManager */
      NVRT_API virtual void doSetAccelerationResource( RTacceleration accel );

      /*! \brief Get the specified child.
       *  \param index The index of the child to get.
       *  \return The specified child as an RTobject.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTChildrenManager.
       *  \sa doGetNumberOfChildren, doSetNumberOfChildren, doSetChildResource, RTChildrenManager */
      NVRT_API virtual RTobject doGetChildResource( unsigned int index ) const;

      /*! \brief Get the number of children of this RTGroup.
       *  \return The number of children of this RTGroup.
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
      RTgroup m_group;
  };
}


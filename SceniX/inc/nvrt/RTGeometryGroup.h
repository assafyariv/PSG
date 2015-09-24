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
  /*! \brief Wrapper class for an nvirt RTgeometrygroup object.
   *  \par Namespace: nvrt
   *  \remarks An RTGeometryGroup represents a group of geometries, together with the corresponding
   *  acceleration structure.
   *  \sa RTNode */
  class RTGeometryGroup : public RTNode, public RTAccelerationManager, public RTChildrenManager<RTGeometryInstance>
  {
    friend class RTContext;
    friend class RTVariableType<RTGeometryGroup>;

    public:
      NVRT_API static RTGeometryGroupSharedPtr create();

    public:
      /*! \brief Default-constructor of RTGeometryGroup */
      NVRT_API RTGeometryGroup();

      /*! \brief Destructor of RTGeometryGroup */
      NVRT_API virtual ~RTGeometryGroup();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_GEOMETRY_GROUP. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

    protected:
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the geometry with.
       *  \remarks This function is called from RTContext::newGeometryGroup to initialize the new
       *  geometry group.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTGeometryGroup wraps the functionality of an nvirt::RTgeometrygroup. This
       *  function is used internally for accessing that resource. */
      NVRT_API virtual RTobject getResource() const;

      /*! \brief Get this object as an RTNode.
       *  \return A This object as an RTNode.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTAccelerationManager and RTChildrenManager.
       *  \sa RTAccelerationManager, RTChildrenManager */
      NVRT_API virtual RTNodeWeakPtr doGetNode() const;

      /*! \brief Get the acceleration structure associated with this geometry group.
       *  \return The acceleration structure associated with this geometry group.
       *  \remarks This function is called by the framework. It is needed to resolve a pure virtual
       *  function in RTAccelerationManager.
       *  \sa doSetAccelerationResource, RTAcceleration, RTAccelerationManager */
      NVRT_API virtual RTacceleration doGetAccelerationResource() const;

      /*! \brief Set the acceleration structure for this geometry group.
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
       *  \note Even though a RTgeometrygroup can just have RTgeometryinstances as children, the
       *  return type has to be RTobject to fit to the general interface of RTChildrenManager.
       *  \sa doGetNumberOfChildren, doSetNumberOfChildren, doSetChildResource, RTChildrenManager */
      NVRT_API virtual RTobject doGetChildResource( unsigned int index ) const;

      /*! \brief Get the number of children of this RTGeometryGroup.
       *  \return The number of children of this RTGeometryGroup.
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
      NVRT_API virtual void doSetNumberOfChildren( unsigned int count );

    private:
      RTgeometrygroup m_geometryGroup;
  };
}

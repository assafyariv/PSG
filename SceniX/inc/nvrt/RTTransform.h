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
#include "nvmath/Matnnt.h"
#include "nvrt/RTNode.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Wrapper class for an nvirt RTtransform object.
   *  \par Namespace: nvrt
   *  \remarks An RTTransform manages a transformation matrix an a single child.
   *  \sa RTNode */
  class RTTransform : public RTNode
  {
    friend class RTContext;
    friend class RTVariableType<RTTransform>;

    public:
      NVRT_API static RTTransformSharedPtr create();

    public:
      /*! \brief Default-constructor of RTTransform */
      NVRT_API RTTransform();

      /*! \brief Destructor of RTTransform */
      NVRT_API virtual ~RTTransform();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_TRANSFORM. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Get the matrix of this RTTransform.
       *  \return A constant reference to the matrix of this RTTransform.
       *  \sa getInverse, setMatrix */
      NVRT_API const nvmath::Mat44f & getMatrix() const;

      /*! \brief Get the inverse matrix of this RTTransform.
       *  \return A constant reference to the inverse matrix of this RTTransform.
       *  \sa getMatrix, setMatrix */
      NVRT_API const nvmath::Mat44f & getInverse() const;

      /*! \brief Set the transformation matrix and its inverse of this RTTransform.
       *  \param matrix A constant reference to the matrix to use.
       *  \param inverse A constant reference to the inverse matrix to use.
       *  \sa getMatrix, getInverse */
      NVRT_API void setMatrix( const nvmath::Mat44f & matrix, const nvmath::Mat44f & inverse );

      /*! \brief Set the child of this RTTransform.
       *  \param child A pointer to the child to set.
       *  \remarks An RTTransform can hold just one child
       *  \sa getChild */
      NVRT_API void setChild( const RTNodeWeakPtr & child );

      /*! \brief Set the child of this RTTransform.
       *  \param child A pointer to the child to set.
       *  \remarks An RTTransform can hold just one child
       *  \sa getChild */
      NVRT_API void setChild( const RTNodeSharedPtr & child );

      /*! \brief Get the child of this RTTransform.
       *  \return A pointer to the child.
       *  \remarks An RTTransform can hold just one child
       *  \sa setChild */
      NVRT_API RTNodeWeakPtr getChild() const;

    protected:
      /*! \brief Initialize this object.
       *  \param context The RTcontext to create the group with.
       *  \remarks This function is called from RTContext::newTransform to initialize the new
       *  transform.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTTransform wraps the functionality of an nvirt::RTtransform.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTobject getResource() const;

    private:
      RTNodeSharedPtr m_child;
      nvmath::Mat44f    m_inverse;
      nvmath::Mat44f    m_matrix;
      RTtransform       m_transform;
  };
}


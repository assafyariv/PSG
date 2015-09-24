// Copyright NVIDIA Corporation 2002-2011
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

#include "nvsgcommon.h"
#include "nvsg/OwnedObject.h"

namespace nvsg
{  
  /*! \brief Abstract base class of all attribute objects.
   *  \par Namespace: nvsg
   *  \remarks Defines the interface that every state attribute has to resolve. A StateAttribute
   *  can be the child of one or more StatePass objects.
   *  \sa BlendAttribute, CgFx, Material, PrimitiveAttribute, StatePass, TextureAttribute */
  class StateAttribute : public OwnedObject<StatePass>
  {
    public:
      /*! \brief Destructs a StateAttribute. */
      NVSG_API virtual ~StateAttribute( void );

      REFLECTION_INFO_API( NVSG_API, StateAttribute );
    protected:
      /*! \brief Protected default constructor to prevent explicit creation.
       *  \remarks This default constructor is accessible only by derived classes. */
      NVSG_API StateAttribute( void );

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \param rhs A reference to the constant StateAttribute to copy from
       *  \remarks This copy constructor is accessible only by derived classes. The copy
       *  constructor of OwnedObject is called, but the parents of \a rhs are not copied. */
      NVSG_API StateAttribute(const StateAttribute& rhs);

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant StateAttribute to copy from.
       *  \return A reference to the assigned StateAttribute.
       *  \remarks This assignment operator is accessible only by derived classes. It just calls
       *  the assignment operator of OwnedObject. The parents of \a rhs are not copied. */
      NVSG_API StateAttribute & operator=(const StateAttribute & rhs);
  };

} //  namespace nvsg


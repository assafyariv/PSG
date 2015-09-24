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
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Pure virtual base class for RT classes that can be child of other RT classes.
   *  \sa RTBase, RTGeometryGroup, RTGroup, RTSelector, RTTransform */
  class RTNode : public RTBase
  {
    friend class RTContext;

    public:
      /*! \brief Default constructor of RTNode. */
      NVRT_API RTNode();

      /*! \brief Destructor of RTNode. */
      NVRT_API virtual ~RTNode();

      /*! \brief Interface for all RTNode derived classes to get the underlying nvirt resource.
       *  \return A general RTobject that specifies the underlying nvirt resource for the concrete class.
       */
      NVRT_API virtual RTobject getResource() const = 0;
  };
}

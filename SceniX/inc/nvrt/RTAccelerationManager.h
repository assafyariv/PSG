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
  /*! \brief Base class to manage RTAcceleration objects.
   *  \par Namespace: nvrt
   *  \remarks An RTAccelerationManager provides the functionality to manage the handling of an
   *  RTAcceleration object. RTGeometryGroup and RTGroup derive from RTAccelerationManager to have a
   *  unique interface on that task. Those classes need to provide the pure virtual functions that finally
   *  perform the required operations.
   *  \sa RTGeometryGroup, RTGroup */
  class RTAccelerationManager
  {
    public:
      /*! \brief Set the RTAcceleration.
       *  \param accel The RTAcceleration to set.
       *  \remarks If \a accel is different from the currently set RTAcceleration, that
       *  RTAcceleration is replaced by \a accel.
       *  \sa getAcceleration, markAccelerationDirty */
      NVRT_API void setAcceleration( const RTAccelerationWeakPtr & accel );

      /*! \brief Get the current RTAcceleration.
       *  \return A pointer to the current RTAcceleration.
       *  \remarks If there's no RTAcceleration set, this function returns NULL.
       *  \sa setAcceleration, markAccelerationDirty */
      NVRT_API RTAccelerationWeakPtr getAcceleration() const;

      /*! \brief Mark the current RTAcceleration as dirty.
       *  \remarks If an RTAcceleration is marked as dirty, it is recreated on the next ray-tracing
       *  traversal.
       *  \sa setAcceleration, getAcceleration */
      NVRT_API void markAccelerationDirty() const;

    protected:
      /*! \brief Default-constructor of RTAccelerationManager */
      NVRT_API RTAccelerationManager();

      /*! \brief Destructor of RTAccelerationManager */
      NVRT_API virtual ~RTAccelerationManager();

      /*! \brief Interface to get the RTNode of this object.
       *  \returns The RTNode of this object.
       *  \remarks RTAccelerationManager is designed to be inherited by classes that also derive from
       *  RTNode. This pure virtual function should return that RTNode. */
      NVRT_API virtual RTNodeWeakPtr doGetNode() const = 0;

      /*! \brief Interface to get the RTacceleration resource of this object.
       *  \return The RTacceleration resource managed by this class.
       *  \remarks This class manages an RTAcceleration, which in turn wraps an RTacceleration
       *  resource. This function is used to get that resource.
       *  \sa doSetAccelerationResource */
      NVRT_API virtual RTacceleration doGetAccelerationResource() const = 0;

      /*! \brief Interface to set the RTacceleration resource of this object.
       *  \param accel the RTacceleration resource to set.
       *  \remarks This class manages an RTAcceleration, which in turn wraps an RTacceleration
       *  resource. This function is used to set that resource.
       *  \sa doGetAccelerationResource */
      NVRT_API virtual void doSetAccelerationResource( RTacceleration accel ) = 0;

#if !defined(NDEBUG)
    private :
      bool checkAcceleration() const;
#endif

    private:
      RTAccelerationSharedPtr m_accel;
  };
}


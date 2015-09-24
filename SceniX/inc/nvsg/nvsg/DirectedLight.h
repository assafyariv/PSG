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
/** @file */

#include "nvsg/LightSource.h"

namespace nvsg
{
  /*! \brief LightSource representing a directed light.
   *  \par Namespace: nvsg
   *  \remarks A DirectedLight is a LightSource with a direction (or orientation), but without a
   *  position. Its light is emitted along parallel rays from an infinite distance away.
   *  \sa LightSource, PointLight, SpotLight */
  class DirectedLight : public LightSource
  {
    public:
      NVSG_API static DirectedLightSharedPtr create();

    public:
      NVSG_API virtual void syncEffectLight(CgFxEffect * fx, CgFxParameter fxLight, const nvmath::Mat44f& modelToWorld) const;
      NVSG_API virtual void syncMetaSLLightParameters(CgFxEffect * fx, unsigned int light, const nvmath::Mat44f& world, const nvmath::Mat44f& view) const;

      /*! \brief Get the direction of the light rays.
       *  \return The direction of the light rays.
       *  \remarks The default direction is the negative z-axis (0.0,0.0,-1.0).
       *  \sa getOrientation, setDirection */
      NVSG_API nvmath::Vec3f getDirection() const;

      /*! \brief Set the direction of the light rays.
       *  \param direction  A reference to the constant direction of the light rays.
       *  \remarks The default direction is the negative z-axis (0.0,0.0,-1.0).\n
       *  Setting the direction increases the Incarnation. 
       *  \note The direction is expected to be normalized.
       *  \sa getDirection, setOrientation */
      NVSG_API void setDirection( const nvmath::Vec3f & direction );

      /*! \brief Get the orientation of the light rays.
       *  \return The reference to the constant orientation of the light rays.
       *  \remarks The orientation is the rotation from the default light direction, the negative
       *  z-axis (0.0,0.0,-1.0).
       *  \sa getDirection, setOrientation */
      NVSG_API const nvmath::Quatf & getOrientation() const;
      
      /*! \brief Set the orientation of the light rays.
       *  \param orientation  A reference to the constant orientation of the light rays.
       *  \remarks The orientation is the rotation from the default light direction, the negative
       *  z-axis (0.0,0.0,-1.0).\n
       *  Setting the orientation increases the Incarnation. 
       *  \sa getOrientation, setDirection */
      NVSG_API void setOrientation( const nvmath::Quatf & orientation );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant DirectedLight to copy from
       *  \return A reference to the assigned DirectedLight
       *  \remarks The assignment operator just calls the assignment operator of LightSource.
       *  \sa LightSource */
      NVSG_API DirectedLight & operator=( const DirectedLight & rhs );

    protected:
      friend struct nvutil::Holder<DirectedLight>;

      /*! \brief Default-constructs a DirectedLight. 
       */
      NVSG_API DirectedLight();

      /*! \brief Constructs a PointLight as a copy of another DirectedLight.
      */
      NVSG_API DirectedLight( const DirectedLight &rhs );

      /*! \brief Destructs a DirectedLight
      */
      NVSG_API virtual ~DirectedLight();

    public:
      // reflected properties

      REFLECTION_INFO_API( NVSG_API, DirectedLight );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( Direction );
          NVSG_API DECLARE_STATIC_PROPERTY( Orientation );
      END_DECLARE_STATIC_PROPERTIES
  };

  inline nvmath::Vec3f DirectedLight::getDirection() const
  {
    NVSG_TRACE();
    // exploit return value optimization by returning a constructor call
    return nvmath::Vec3f(nvmath::Vec3f (0.f, 0.f, -1.f) * getOrientation());
  }

} //  namespace nvsg

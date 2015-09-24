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

#include "nvsgcommon.h" // commonly used stuff
#include "nvsg/OwnedObject.h"
#include "nvsg/Animation.h"
#include "nvsg/CgFx.h"
#include "nvmath/Matnnt.h"
#include "nvmath/Trafo.h"
#include "nvutil/HashGenerator.h"
#if !defined(NDEBUG)
#include "nvutil/Conversion.h"
#endif

namespace nvsg
{
  /*! \brief Abstract base class for all light sources.
   *  \par Namespace: nvsg
   *  \remarks A LightSource contains data and functionality common to all light sources
   *  (DirectedLight, SpotLight, ...).\n
   *  \note A LightSource has no method to turn light sources on or off. This functionality is
   *  located in the Group holding the LightSource objects.
   *  \note LightSource objects only influence objects beneath the Group the LightSource is part
   *  of.
   *  \sa DirectedLight, PointLight, SpotLight, Switch */
  class LightSource : public OwnedObject<Group>
  {
    friend class Group;

    public:
      /*! \brief Destructs a LightSource.
       *  If the LightSource holds an Animation, its reference count is decremented. */
      NVSG_API virtual ~LightSource();

      NVSG_API virtual void syncEffectLight(CgFxEffect * fx, CgFxParameter fxLight, const nvmath::Mat44f& modelToWorld) const {}
      NVSG_API virtual void syncMetaSLLightParameters(CgFxEffect * fx, unsigned int light, const nvmath::Mat44f& world, const nvmath::Mat44f& view) const {}

      /*! \brief Get the ambient color of this LightSource.
       *  \return A reference to the constant ambient color.
       *  \remarks The ambient color of a LightSource modulates the material ambient term.
       *  It is not dependent on any other lighting factors except attenuation and spotlight cutoff.
       *  The default is black (0.0,0.0,0.0).
       *  \sa getDiffuseColor, getSpecularColor, setAmbientColor */
      NVSG_API const nvmath::Vec3f& getAmbientColor() const;

      /*! \brief Set the ambient color of this LightSource.
       *  \param color A reference to the constant ambient color to set.
       *  \remarks The ambient color of a LightSource modulates the material ambient term. 
       *  It is not dependent on any other lighting factors except for attenuation and spotlight cutoff.
       *  The default is black (0.0,0.0,0.0).
       *  \sa getAmbientColor, setDiffuseColor, setSpecularColor */
      NVSG_API void setAmbientColor( const nvmath::Vec3f &color );

      /*! \brief Get the diffuse color of this LightSource.
       *  \return A reference to the constant diffuse color.
       *  \remarks The diffuse color of a light is used to calculate lighting contributions that
       *  might depend on vertex normals, light direction, light position, range, or attenuation,
       *  depending on the type of light source. The default is white (1.0,1.0,1.0).
       *  \sa getAmbientColor, getSpecularColor, setDiffuseColor */
      NVSG_API const nvmath::Vec3f& getDiffuseColor() const;

      /*! \brief Set the diffuse color of this LightSource.
       *  \param color A reference to the constant diffuse color to set.
       *  \remarks The diffuse color of a light is used to calculate lighting contributions that
       *  might depend on vertex normals, light direction, light position, range, or attenuation,
       *  depending on the type of light source. The default is white (1.0,1.0,1.0).
       *  \sa getDiffuseColor, setAmbientColor, setSpecularColor */
      NVSG_API void setDiffuseColor( const nvmath::Vec3f &color );

      /*! \brief Get the specular color of this LightSource.
       *  \return A reference to the constant specular color.
       *  \remarks The specular color of a light is used to calculate any lighting contributions
       *  from vertex normals, vertex position, light direction, light position, range, or
       *  attenuation, depending on the type of light source. The default is white (1.0,1.0,1.0).
       *  \sa getAmbientColor, getDiffuseColor, setSpecularColor, */
      NVSG_API const nvmath::Vec3f& getSpecularColor() const;

      /*! \brief Set the specular color of this LightSource.
       *  \param color A reference to the constant specular color to set.
       *  \remarks The specular color of a light is used to calculate any lighting contributions
       *  from vertex normals, vertex position, light direction, light position, range, or
       *  attenuation, depending on the type of light source. The default is white (1.0,1.0,1.0).
       *  \sa getSpecularColor, setAmbientColor, setDiffuseColor */
      NVSG_API void setSpecularColor( const nvmath::Vec3f &color );

      /*! \brief Get the intensity of this LightSource.
       *  \return The intensity.
       *  \remarks The intensity of a LightSource is a multiplier for all three color types of this
       *  LightSource.
       *  \sa getAmbientColor, getDiffuseColor, getSpecularColor, setIntensity */
      NVSG_API float getIntensity() const;

      /*! \brief Set the intensity of this LightSource.
       *  \param intensity The intensity to set.
       *  \remarks The intensity of a LightSource is a multiplier for all three color types of this
       *  LightSource.
       *  \sa getIntensity, setAmbientColor, setDiffuseColor, setSpecularColor */
      NVSG_API void setIntensity( float intensity );

      /*! \brief Query if this LightSource is enabled.
       *  \return \c true, if this LightSource is enabled, otherwise \c false.
       *  \sa setEnabled */
      NVSG_API bool isEnabled() const;

      /*! \brief Query if this LightSource casts shadows.
       *  \return \c true, if this LightSource casts shadows, otherwise \c false.
       *  \remarks A LightSource might be excluded from the shadow casters by this flag. The
       *  default value is \c true.
       *  \sa setShadowCasting */
      NVSG_API bool isShadowCasting() const;

      /*! \brief Set the enabled state of this LightSource.
       *  \param enable \c true, to switch the LightSource on, \c false to switch it off.
       *  \sa isEnabled */
      NVSG_API void setEnabled( bool enable );

      /*! \brief Set the shadow casting state of this LightSource.
       *  \param cast \c true, to switch shadow casting on, \c false to switch it off.
       *  \remarks A LightSource might be excluded from the shadow casters by this flag. The
       *  default value is \c true.
       *  \sa isShadowCasting */
      NVSG_API void setShadowCasting( bool cast );

      /*! \brief Get the Animation of this LightSource.
       *  \return The Trafo Animation of this LightSource.
       *  \remarks The position and orientation of a LightSource can be animated by an Animation of
       *  Trafo.
       *  \sa Animation, nvmath::Trafo, setAnimation */
      NVSG_API const TrafoAnimationSharedPtr & getAnimation() const;

      /*! \brief Set the Animation of this LightSource.
       *  \param animation The Animation to use.
       *  \remarks The position and orientation of a LightSource can be animated by an Animation of
       *  Trafo.
       *  \sa Animation, nvmath::Trafo, getAnimation */
      NVSG_API void setAnimation( const TrafoAnimationSharedPtr & animation );

      /*! \brief Get the current transformation.
       *  \return The transformation that determines position and orientation of this LightSource.
       *  \sa getInverse */
      NVSG_API nvmath::Mat44f getTransformationMatrix() const;

      /*! \brief Get the inverse of the transformation.
       *  \return The inverse of the transformation that determines position and orientation of
       *  this LightSource.
       *  \sa getTransformationMatrix */
      NVSG_API nvmath::Mat44f getInverse() const;

      /*! \brief Test for equivalence with another LightSource.
       *  \param p A pointer to the LightSource to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the LightSource \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Node, their Animation objects are tested
       *  for equivalence. If \a deepCompare is true, and there is an Animation object in both \c
       *  this and \a p, a full equivalence test on them is performed, otherwise they are
       *  considered to be equivalent if they are the same pointers. All the other LightSource
       *  data are tested for equality.
       *  \note The behavior is undefined if \a p is not a LightSource nor derived from one.
       *  \sa DirectedLight, Node, PointLight, SpotLight */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, LightSource );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( AmbientColor );
          NVSG_API DECLARE_STATIC_PROPERTY( DiffuseColor );
          NVSG_API DECLARE_STATIC_PROPERTY( SpecularColor );
          NVSG_API DECLARE_STATIC_PROPERTY( Intensity );
          NVSG_API DECLARE_STATIC_PROPERTY( Enabled );
          NVSG_API DECLARE_STATIC_PROPERTY( ShadowCasting );
      END_DECLARE_STATIC_PROPERTIES 

    protected:
      /*! \brief Protected default constructor to prevent explicit creation.
       *  \remarks The default values of the newly created LightSource are as follows:\n
       *    - ambient color is black (0.0,0.0,0.0)\n
       *    - animation is none (NULL)\n
       *    - diffuse color is white (1.0,1.0,1.0)\n
       *    - intensity is one (1.0)\n
       *    - shadow casting is on\n
       *    - specular color is white (1.0,1.0,1.0)\n
       *    - transform is the identity
       *  \note A LightSource will not be instantiated directly, but by instantiating a
       *  derived object like DirectedLight or PointLight, for example.*/
      NVSG_API LightSource();

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \param rhs A reference to the constant LightSource to copy from.
       *  \remarks The newly created LightSource is copied from \a rhs.
       *  \note A LightSource will not be instantiated directly, but by instantiating a
       *  derived object like Quads or Triangles for example.*/
      NVSG_API LightSource( const LightSource& rhs );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant LightSource to copy from.
       *  \return A reference to the assigned LightSource.
       *  \remarks The assignment operator calls the assignment operator of Node. The reference
       *  count of any currently attached Animation is decremented, all data specific to a
       *  LightSource is copied, and any Animation attached to \a rhs is cloned. */
      NVSG_API LightSource & operator=(const LightSource & rhs);

      /*! \brief Determines the space in which the indicated MetaSL light parameter is expected.
       * \param fx The CgFxEffect in which the light parameter is defined
       * \param param The MetaSL light parameter
       * \param world Specifies the object-to-world transformation at the time of calling
       * \param view Specifies the world-to-view transformation at the time of calling
       * \param transform Determined 4x4 matrix that can be used to transform object space coordinates to target space.
       * \remarks This utility function evaluates the given MetaSL light parameter, and determines
       * the 4x4 matrix to transform object space coordinates to the light parameter's target space. */ 
      NVSG_API void determineMetaSLLightParameterSpace( CgFxEffect * fx, 
                                                        CgFxParameter param, 
                                                        const nvmath::Mat44f& world, 
                                                        const nvmath::Mat44f& view, 
                                                        nvmath::Mat44f& transform ) const;

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    protected:
      nvmath::Trafo m_trafo; //!< Trafo used by every kind of light source

    private:
      nvmath::Vec3f           m_ambientColor;
      TrafoAnimationSharedPtr m_animation; // animation working on a Trafo
      nvmath::Vec3f           m_diffuseColor;
      float                   m_intensity;
      bool                    m_shadowCasting;
      bool                    m_enabled;
      nvmath::Vec3f           m_specularColor;
  };

  inline const nvmath::Vec3f& LightSource::getAmbientColor() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_ambientColor );
  }

  inline const TrafoAnimationSharedPtr & LightSource::getAnimation() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_animation );
  }

  inline const nvmath::Vec3f& LightSource::getDiffuseColor() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_diffuseColor );
  }

  inline float LightSource::getIntensity() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_intensity );
  }

  inline nvmath::Mat44f LightSource::getInverse() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( (m_animation ? TrafoAnimationReadLock(m_animation)->getCurrentValue() : m_trafo).getInverse() );
  }

  inline const nvmath::Vec3f& LightSource::getSpecularColor() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_specularColor );
  }

  inline nvmath::Mat44f LightSource::getTransformationMatrix() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( (m_animation ? TrafoAnimationReadLock(m_animation)->getCurrentValue() : m_trafo).getMatrix() );
  }

  inline bool LightSource::isEnabled() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_enabled );
  }

  inline bool LightSource::isShadowCasting() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_shadowCasting );
  }

  inline void LightSource::setAmbientColor( const nvmath::Vec3f &c )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_ambientColor != c )
    {
      m_ambientColor = c;
      notifyChange( this );
      notify( PID_AmbientColor );
    }
  }

  inline void LightSource::setDiffuseColor( const nvmath::Vec3f &c )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_diffuseColor != c )
    {
      m_diffuseColor = c;
      notifyChange( this );
      notify( PID_DiffuseColor );
    }
  }

  inline void LightSource::setEnabled( bool enable )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_enabled != enable )
    {
      m_enabled = enable;
      notifyChange( this );
    }
  }

  inline void LightSource::setIntensity( float i )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_intensity != i )
    {
      m_intensity = i;
      notifyChange( this );
      notify( PID_Enabled );
    }
  }

  inline void LightSource::setSpecularColor( const nvmath::Vec3f &c )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_specularColor != c )
    {
      m_specularColor = c;
      notifyChange( this );
      notify( PID_SpecularColor );
    }
  }

  inline void LightSource::setShadowCasting( bool cast )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_shadowCasting != cast )
    {
      m_shadowCasting = cast;
      notifyChange( this );
      notify( PID_ShadowCasting );
    }
  }

} //  namespace nvsg

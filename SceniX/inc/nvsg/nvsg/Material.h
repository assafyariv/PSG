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
#include "nvmath/Vecnt.h"
#include "nvsg/StateAttribute.h" // base class definition
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  class Material;

  /*! \brief ID to identify the front and back face properties of a Material.
   *  \par Namespace: nvsg
   *  \remarks All the properties of a Material can be independently set for the front and the    
   *  back faces.
   *  \sa Material */
  typedef enum
  {
    MF_FRONT = 0x01,                        //!< used for front material
    MF_BACK  = 0x02,                        //!< used for back material
    MF_FRONT_AND_BACK = MF_FRONT | MF_BACK  //!< used for both front and back material
  } MaterialFace;

//  ignore private member class on documentation
# if ! defined( DOXYGEN_IGNORE )
  class MaterialData : public nvutil::RCObject
  {
    friend class Material;

    public:
      //! Copy constructor
      /** Constructs a MaterialData object from another MaterialData object.
        * Public, because reference counting requires the copy constructor to be accessible. */
      NVSG_API MaterialData(const MaterialData& rhs);

    protected:
      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      // instantiation on stack is not permitted
      MaterialData();
      ~MaterialData();
      // assignment also not permitted
      MaterialData& operator=(const MaterialData& rhs);

      nvmath::Vec3f m_backAmbientColor;
      nvmath::Vec3f m_backDiffuseColor;
      nvmath::Vec3f m_backEmissiveColor;
      float         m_backIOR;
      float         m_backOpacity;
      nvmath::Vec3f m_backOpacityColor;
      float         m_backReflectivity;
      nvmath::Vec3f m_backReflectivityColor;
      nvmath::Vec3f m_backSpecularColor;
      float         m_backSpecularExponent;

      nvmath::Vec3f m_frontAmbientColor;
      nvmath::Vec3f m_frontDiffuseColor;
      nvmath::Vec3f m_frontEmissiveColor;
      float         m_frontIOR;
      float         m_frontOpacity;
      nvmath::Vec3f m_frontOpacityColor;
      float         m_frontReflectivity;
      nvmath::Vec3f m_frontReflectivityColor;
      nvmath::Vec3f m_frontSpecularColor;
      float         m_frontSpecularExponent;
  };
#endif    //  DOXYGEN_IGNORE

  /*! \brief StateAttribute that describes material properties.
   *  \par Namespace: nvsg
   *  \remarks A Material describes the color properties of a geometry. The properties for front
   *  and back faces can be described independently. The following are the default values 
   *  that Material uses for both front and back faces:\n
   *    - ambient color is dark grey    (0.2,0.2,0.2)\n
   *    - diffuse color is light grey   (0.8,0.8,0.8)\n
   *    - emissive color is black       (0.0,0.0,0.0)\n
   *    - index of refraction is        1.0\n
   *    - opacity is opaque             1.0\n
   *    - opacity color is opaque       (1.0,1.0,1.0)\n
   *    - reflectivity is none          0.0\n
   *    - reflective color  is none     (0.0,0.0,0.0)\n
   *    - specular color is black       (0.0,0.0,0.0)\n
   *    - specular exponent is zero     0.0\n
   *  \sa StateAttribute */
  class Material : public StateAttribute
  {
    public:
      NVSG_API static MaterialSharedPtr create();

    public:
      /*! \brief Query whether the data of this Material is shared with other Material objects.
       *  \return \c true, if the data of this Material is shared with other Material objects,
       *  otherwise \c false.
       *  \remarks A Material object shares its data with another Material object if it was
       *  either instantiated as a copy of the other, using Material::clone, or if it was assigned
       *  to by the other object using the assignment operator. In that case, the two objects have
       *  the same DataID.
       *  \sa clone, getDataID, operator=() */
      NVSG_API virtual bool isDataShared() const;

      /*! \brief Get the unique data identifier of this object.
       *  \return A 64-bit value that uniquely identifies the embedded data.
       *  \remarks A Material object shares its data with another Material object if it was
       *  either instantiated as a copy of the other, using Material::clone, or if it was assigned
       *  to by the other object using the assignment operator. In that case, the two objects have
       *  the same DataID.
       *  \sa clone, isDataShared, operator=() */
      NVSG_API virtual DataID getDataID() const;

      /*! \brief Retrieves the ambient color.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  ambient color. The default is MF_FRONT.
       *  \return The function returns the ambient color value of this Material.
       *  \remarks Ambient reflection, like ambient light, is non-directional. Ambient reflection
       *  has a lesser impact on the apparent color of a rendered object, but it does affect the
       *  overall color and is most noticeable when little or no diffuse light reflects off the
       *  material. A material's ambient reflection is affected by the ambient light set for a
       *  scene. The initial value is (0.2,0.2,0.2).
       *  \note The behavior is undefined, if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa getDiffuseColor, getEmissiveColor, getSpecularColor, setAmbientColor */
      NVSG_API const nvmath::Vec3f& getAmbientColor( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the ambient color
       *  \param ac Specifies the ambient color to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks Ambient reflection, like ambient light, is non-directional. Ambient reflection
       *  has a lesser impact on the apparent color of a rendered object, but it does affect the
       *  overall color and is most noticeable when little or no diffuse light reflects off the
       *  material. A material's ambient reflection is affected by the ambient light set for a
       *  scene.
       *  \sa getAmbientColor, setDiffuseColor, setEmissiveColor, setSpecularColor */
      NVSG_API void setAmbientColor( const nvmath::Vec3f &ac, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the diffuse color.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  diffuse color. The default is MF_FRONT.
       *  \return The function returns the diffuse color value of this Material.
       *  \remarks The diffuse and ambient colors of a material describe how a material reflects
       *  the ambient and diffuse light in a scene. Because most scenes contain much more diffuse
       *  light than ambient light, diffuse reflection plays the largest role in determining
       *  color. Additionally, because diffuse light is directional, the angle of incidence for
       *  diffuse light affects the overall intensity of the reflection. Diffuse reflection is
       *  greatest when the light strikes a vertex parallel to the vertex normal. As the angle
       *  increases, the effect of diffuse reflection diminishes. The amount of light reflected is
       *  the cosine of the angle between the incoming light and the vertex normal. 
       *  The initial value is (0.8,0.8,0.8).
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa getAmbientColor, getEmissiveColor, getSpecularColor, setDiffuseColor */
      NVSG_API const nvmath::Vec3f& getDiffuseColor( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Set the diffuse color
       *  \param dc Specifies the diffuse color to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks The diffuse and ambient colors of a material describe how a material reflects
       *  the ambient and diffuse light in a scene. Because most scenes contain much more diffuse
       *  light than ambient light, diffuse reflection plays the largest role in determining
       *  color. Additionally, because diffuse light is directional, the angle of incidence for
       *  diffuse light affects the overall intensity of the reflection. Diffuse reflection is
       *  greatest when the light strikes a vertex parallel to the vertex normal. As the angle
       *  increases, the effect of diffuse reflection diminishes. The amount of light reflected is
       *  the cosine of the angle between the incoming light and the vertex normal. 
       *  \sa getDiffuseColor, setAmbientColor, setEmissiveColor, setSpecularColor */
      NVSG_API void setDiffuseColor( const nvmath::Vec3f &dc, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the emissive color.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  emissive color. The default is MF_FRONT.
       *  \return The function returns the emissive color value of this Material.
       *  \remarks Materials can be used to make a rendered object appear to be self-luminous. The
       *  emissive color of a material is used to describe the color of the emitted light.
       *  Emission affects an object's color and can, for example, make a dark material brighter
       *  and take on part of the emitted color. You can use a material's emissive color to add
       *  the illusion that an object is emitting light, without incurring the computational
       *  overhead of adding a light to the scene. Remember, materials with emissive color don't
       *  emit light that can be reflected by other objects in a scene. To achieve this reflected
       *  light, you need to place an additional light within the scene. The default is black
       *  (0.0,0.0,0.0).
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa getAmbientColor, getDiffuseColor, getSpecularColor, setEmissiveColor */
      NVSG_API const nvmath::Vec3f& getEmissiveColor( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the emissive color
       *  \param ec Specifies the emissive color to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks Materials can be used to make a rendered object appear to be self-luminous. The
       *  emissive color of a material is used to describe the color of the emitted light.
       *  Emission affects an object's color, and can, for example, make a dark material brighter
       *  and take on part of the emitted color. You can use a material's emissive color to add
       *  the illusion that an object is emitting light, without incurring the computational
       *  overhead of adding a light to the scene. Remember, materials with emissive color don't
       *  emit light that can be reflected by other objects in a scene. To achieve this reflected
       *  light, you need to place an additional light within the scene.
       *  \sa getEmissiveColor, setAmbientColor, setDiffuseColor, setSpecularColor */
      NVSG_API void setEmissiveColor( const nvmath::Vec3f &ec , MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the opacity.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  opacity. The default is MF_FRONT.
       *  \return The opacity.
       *  \remarks The opacity of a material controls how much light is reflected and how much
       *  light goes through. An opacity of 1.0 means the material is completely opaque - all
       *  light is reflected and no objects behind shine through. An opacity of 0.0 means the
       *  material is completely transparent - no light is reflected and objects behind are seen
       *  undisturbed. The default is opaque 1.0.
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa setOpacity */
      NVSG_API float getOpacity( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the opacity.
       *  \param o The opacity to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks The opacity of a material controls how much light is reflected and how much
       *  light goes through. An opacity of 1.0 means the material is completely opaque - all
       *  light is reflected and no objects behind shine through. An opacity of 0.0 means the
       *  material is completely transparent - no light is reflected and objects behind are seen
       *  undisturbed. The default opacity is 1.0 (completely opaque).
       *  \note This function also sets the opacity color to \a o on all three channels.
       *  \sa getOpacity */
      NVSG_API void setOpacity( float o, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the specular color.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  specular color. The default is MF_FRONT.
       *  \return The function returns the specular color value of this Material.
       *  \remarks Specular reflection creates highlights on objects, making them appear shiny.
       *  The material contains two members that describe the specular highlight color as well as
       *  the material's overall shininess. You establish the color of the specular highlights by
       *  setting the specular color to the desired RGBA color. 
       *  The values you set in the specular exponent control how sharp the
       *  specular effects are. The initial value is (0.0,0.0,0.0).
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa getAmbientColor, getDiffuseColor, getEmissiveColor, getSpecularExponent, setSpecularColor */
      NVSG_API const nvmath::Vec3f& getSpecularColor( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Set the specular color
       *  \param sc Specifies the specular color to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks Specular reflection creates highlights on objects, making them appear shiny.
       *  The material contains two members that describe the specular highlight color as well as
       *  the material's overall shininess. You establish the color of the specular highlights by
       *  setting the specular color to the desired RGBA color. The values you set in the specular 
       *  exponent control how sharp the specular effects are. 
       *  \sa getSpecularColor, setAmbientColor, setDiffuseColor, setEmissiveColor, setSpecularExponent */
      NVSG_API void setSpecularColor( const nvmath::Vec3f &sc, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the specular exponent.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  opacity. The default is MF_FRONT.
       *  \return The specular exponent.
       *  \remarks Specular reflection creates highlights on objects, making them appear shiny.
       *  The material contains two members that describe the specular highlight color as well as
       *  the material's overall shininess. You establish the color of the specular highlights by
       *  setting the specular color to the desired RGBA color. The value you set as the specular 
       *  exponent controls how sharp the specular effects are. The initial value is 0.0.
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa getSpecularColor, setSpecularExponent */
      NVSG_API float getSpecularExponent( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the specular exponent.
       *  \param se The specular exponent to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks Specular reflection creates highlights on objects, making them appear shiny.
       *  The material contains two members that describe the specular highlight color as well as
       *  the material's overall shininess. You establish the color of the specular highlights by
       *  setting the specular color to the desired RGBA color. The value you set as the specular 
       *  exponent controls how sharp the specular effects are. 
       *  \sa getSpecularExponent, setSpecularColor */
      NVSG_API void setSpecularExponent( float se, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the reflectivity.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  reflectivity. The default is MF_FRONT.
       *  \return The reflectivity.
       *  \remarks The reflectivity of a material declares the amount of perfect mirror reflection
       *  to be added to the reflected light as a value between 0.0 and 1.0. The default for
       *  reflectivity is 0.0.
       *  \note OpenGL does not have a notion of reflectivity, and therefore this value will have no
       *  effect on the OpenGL fixed-function pipeline.
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa setReflectivity */
      NVSG_API float getReflectivity( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the reflectivity.
       *  \param r The reflectivity to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks The reflectivity of a material declares the amount of perfect mirror reflection
       *  to be added to the reflected light as a value between 0.0 and 1.0. The default for
       *  reflectivity is 0.0.
       *  \note OpenGL does not have a notion of reflectivity, and therefore this value will have no
       *  effect on the OpenGL fixed-function pipeline.
       *  \note This function also sets the reflectivity color to \a r on all three channels.
       *  \sa getReflectivity */
      NVSG_API void setReflectivity( float r, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the IndexOfRefraction.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  IndexOfRefraction. The default is MF_FRONT.
       *  \return The IndexOfRefraction.
       *  \remarks The IndexOfRefraction of a material declares the IOR for perfectly refracted
       *  light as a single scalar index. The default for IndexOfRefraction is 1.0.
       *  \note OpenGL does not have a notion of IndexOfRefraction, and therefore this value will
       *  have no effect on the OpenGL fixed-function pipeline.
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \sa setIndexOfRefraction */
      NVSG_API float getIndexOfRefraction( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the IndexOfRefraction.
       *  \param ior The IndexOfRefraction to set.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks The IndexOfRefraction of a material declares the IOR for perfectly refracted
       *  light as a single scalar index. The default for IndexOfRefraction is 1.0.
       *  \note OpenGL does not have a notion of IndexOfRefraction, and therefore this value will
       *  have no effect on the OpenGL fixed-function pipeline.
       *  \sa getIndexOfRefraction */
      NVSG_API void setIndexOfRefraction( float ior, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the Reflectivity color.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  reflectivity color. The default is MF_FRONT.
       *  \return A reference to the constant reflectivity color.
       *  \remarks The reflectivity color declares an reflectivity per color channel. If, for
       *  example, the reflectivity color is (1.0,0.5,0.0), the material would be completely
       *  reflective on the red channel, half reflective on the green channel, and not reflective at
       *  all on the blue channel.
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \note OpenGL does not have a notion of reflectivity color, and therefore this value will have
       *  no effect on the OpenGL fixed-function pipeline.
       *  \sa getAmbientColor, getDiffuseColor, getEmissiveColor, getSpecularColor,
       *  setReflectivitiyColor, getReflectivity */
      NVSG_API const nvmath::Vec3f& getReflectivityColor( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the reflectivity color
       *  \param sc A reference to the constant color to set as reflectivity color.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks The reflectivity color declares an reflectivity per color channel. If, for
       *  example, the reflectivity color is (1.0,0.5,0.0), the material would be completely
       *  reflective on the red channel, half reflective on the green channel, and not reflective at
       *  all on the blue channel.
       *  \note OpenGL does not have a notion of reflectivity color, and therefore this value will have
       *  no effect on the OpenGL fixed-function pipeline.
       *  \note Setting the reflectivity color also sets the single valued reflectivity to the
       *  intensity of that color.
       *  \sa getReflectivityColor, setAmbientColor, setDiffuseColor, setEmissiveColor, setReflectivity */
      NVSG_API void setReflectivityColor( const nvmath::Vec3f &sc, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Retrieves the opacity color.
       *  \param mf Optional identifier to select front face (MF_FRONT) or back face (MF_BACK)
       *  opacity color. The default is MF_FRONT.
       *  \return A reference to the constant opacity color.
       *  \remarks The opacity color declares an opacity per color channel. If, for example, the
       *  opacity color is (1.0,0.5,0.0), the material would be completely opaque on the red channel,
       *  half transparent on the green channel, and completely transparent on the blue channel.
       *  \note The behavior is undefined if MF_FRONT_AND_BACK is used as the MaterialFace \a mf.
       *  \note OpenGL does not have a notion of TransparentColor, and therefore this value will
       *  have no effect on the OpenGL fixed-function pipeline.
       *  \sa getAmbientColor, getDiffuseColor, getEmissiveColor, getSpecularColor, setReflectivityColor,
       *  getOpacity */
      NVSG_API const nvmath::Vec3f& getOpacityColor( MaterialFace mf = MF_FRONT ) const;

      /*! \brief Sets the opacity color
       *  \param oc A reference to the constant color to set as the opacity color.
       *  \param mf Optional identifier to select front face (MF_FRONT), back face (FM_BACK) or
       *  both (MF_FRONT_AND_BACK) to set. The default is MF_FRONT_AND_BACK.
       *  \remarks The opacity color declares an opacity per color channel. If, for example, the
       *  opacity color is (1.0,0.5,0.0), the material would be completely opaque on the red channel,
       *  half transparent on the green channel, and completely transparent on the blue channel.
       *  \note OpenGL does not have a notion of opacity color, and therefore this value will
       *  have no effect on the OpenGL fixed-function pipeline.
       *  \note Setting the opacity color also sets the single valued opacity to the intensity of
       *  that color.
       *  \sa getTransparentColor, setAmbientColor, setDiffuseColor, setEmissiveColor,
       *  setTransparentExponent, setOpacity */
      NVSG_API void setOpacityColor( const nvmath::Vec3f &oc, MaterialFace mf = MF_FRONT_AND_BACK );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Material to copy from.
       *  \return A reference to the assigned Material.
       *  \remarks The assignment operator calls the assignment operator of StateAttribute and
       *  copies all the material properties. */
      NVSG_API Material & operator=( const Material & rhs );

      /*! \brief Test for equivalence with an other Material. 
       *  \param p A reference to the constant Material to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the Material \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as StateAttribute, they are equivalent
       *  if all their material properties are equal.
       *  \sa StateAttribute */
      NVSG_API virtual bool isEquivalent( const Object * p , bool ignoreNames, bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, Material );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( AmbientColorFront );
          NVSG_API DECLARE_STATIC_PROPERTY( DiffuseColorFront );
          NVSG_API DECLARE_STATIC_PROPERTY( EmissiveColorFront );
          NVSG_API DECLARE_STATIC_PROPERTY( SpecularColorFront );
          NVSG_API DECLARE_STATIC_PROPERTY( ReflectivityColorFront );
          NVSG_API DECLARE_STATIC_PROPERTY( OpacityColorFront );

          NVSG_API DECLARE_STATIC_PROPERTY( OpacityFront );
          NVSG_API DECLARE_STATIC_PROPERTY( ReflectivityFront );
          NVSG_API DECLARE_STATIC_PROPERTY( SpecularExponentFront );
          NVSG_API DECLARE_STATIC_PROPERTY( IndexOfRefractionFront );

          NVSG_API DECLARE_STATIC_PROPERTY( AmbientColorBack );
          NVSG_API DECLARE_STATIC_PROPERTY( DiffuseColorBack );
          NVSG_API DECLARE_STATIC_PROPERTY( EmissiveColorBack );
          NVSG_API DECLARE_STATIC_PROPERTY( SpecularColorBack );
          NVSG_API DECLARE_STATIC_PROPERTY( ReflectivityColorBack );
          NVSG_API DECLARE_STATIC_PROPERTY( OpacityColorBack );

          NVSG_API DECLARE_STATIC_PROPERTY( OpacityBack );
          NVSG_API DECLARE_STATIC_PROPERTY( ReflectivityBack );
          NVSG_API DECLARE_STATIC_PROPERTY( SpecularExponentBack );
          NVSG_API DECLARE_STATIC_PROPERTY( IndexOfRefractionBack );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      friend struct nvutil::Holder<Material>;

      /*! \brief Default-constructs a Material. */
      NVSG_API Material();

      /*! \brief Constructs a Material as a copy from another Material. */
      NVSG_API Material( const Material &rhs );

      /*! \brief Destructs a Material. */
      NVSG_API virtual ~Material();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      // Property helper functions frontface
      const nvmath::Vec3f &getAmbientColorFront() const      { return getAmbientColor( MF_FRONT ); }
      const nvmath::Vec3f &getDiffuseColorFront() const      { return getDiffuseColor( MF_FRONT ); }
      const nvmath::Vec3f &getEmissiveColorFront() const     { return getEmissiveColor( MF_FRONT ); }
      const nvmath::Vec3f &getSpecularColorFront() const     { return getSpecularColor( MF_FRONT ); }
      const nvmath::Vec3f &getReflectivityColorFront() const { return getReflectivityColor( MF_FRONT ); }
      const nvmath::Vec3f &getOpacityColorFront() const      { return getOpacityColor( MF_FRONT ); }

      void setAmbientColorFront( const nvmath::Vec3f &color )      { setAmbientColor( color, MF_FRONT ); }
      void setDiffuseColorFront( const nvmath::Vec3f &color )      { setDiffuseColor( color, MF_FRONT ); }
      void setEmissiveColorFront( const nvmath::Vec3f &color )     { setEmissiveColor( color, MF_FRONT ); }
      void setSpecularColorFront( const nvmath::Vec3f &color )     { setSpecularColor( color, MF_FRONT ); }
      void setReflectivityColorFront( const nvmath::Vec3f &color ) { setReflectivityColor( color, MF_FRONT ); }
      void setOpacityColorFront( const nvmath::Vec3f &color )      { setOpacityColor( color, MF_FRONT ); }

      float getOpacityFront() const          { return getOpacity( MF_FRONT ); }
      float getReflectivityFront() const     { return getReflectivity( MF_FRONT ); }
      float getSpecularExponentFront() const { return getSpecularExponent( MF_FRONT ); }
      float getIndexOfRefractionFront() const{ return getIndexOfRefraction( MF_FRONT ); }

      void setOpacityFront( float value )           { setOpacity( value, MF_FRONT ); }
      void setReflectivityFront( float value )      { setReflectivity( value, MF_FRONT ); }
      void setSpecularExponentFront( float value )  { setSpecularExponent( value, MF_FRONT ); }
      void setIndexOfRefractionFront( float value ) { setIndexOfRefraction( value, MF_FRONT ); }

      // Property helper functions backface
      const nvmath::Vec3f &getAmbientColorBack() const      {  return getAmbientColor( MF_BACK ); }
      const nvmath::Vec3f &getDiffuseColorBack() const      { return getDiffuseColor( MF_BACK ); }
      const nvmath::Vec3f &getEmissiveColorBack() const     { return getEmissiveColor( MF_BACK ); }
      const nvmath::Vec3f &getSpecularColorBack() const     { return getSpecularColor( MF_BACK ); }
      const nvmath::Vec3f &getReflectivityColorBack() const { return getReflectivityColor( MF_BACK ); }
      const nvmath::Vec3f &getOpacityColorBack() const      { return getOpacityColor( MF_BACK ); }

      void setAmbientColorBack( const nvmath::Vec3f &color )      { setAmbientColor( color, MF_BACK ); }
      void setDiffuseColorBack( const nvmath::Vec3f &color )      { setDiffuseColor( color, MF_BACK ); }
      void setEmissiveColorBack( const nvmath::Vec3f &color )     { setEmissiveColor( color, MF_BACK ); }
      void setSpecularColorBack( const nvmath::Vec3f &color )     { setSpecularColor( color, MF_BACK ); }
      void setReflectivityColorBack( const nvmath::Vec3f &color ) { setReflectivityColor( color, MF_BACK ); }
      void setOpacityColorBack( const nvmath::Vec3f &color )      { setOpacityColor( color, MF_BACK ); }

      float getOpacityBack() const           { return getOpacity( MF_BACK ); }
      float getReflectivityBack() const      { return getReflectivity( MF_BACK ); }
      float getSpecularExponentBack() const  { return getSpecularExponent( MF_BACK ); }
      float getIndexOfRefractionBack() const { return getIndexOfRefraction( MF_BACK ); }

      void setOpacityBack( float value )           { setOpacity( value, MF_BACK ); }
      void setReflectivityBack( float value )      { setReflectivity( value, MF_BACK ); }
      void setSpecularExponentBack( float value )  { setSpecularExponent( value, MF_BACK ); }
      void setIndexOfRefractionBack( float value ) { setIndexOfRefraction( value, MF_BACK ); }

      // helper to determine whether a change should cause an invalidation
      void evalChange( int flags = 0 );

  private:
      nvutil::SmartPtr<MaterialData>  m_material; // shareable material data
  };

  inline const nvmath::Vec3f& Material::getAmbientColor( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontAmbientColor : m_material->m_backAmbientColor );
  }

  inline const nvmath::Vec3f& Material::getDiffuseColor( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontDiffuseColor : m_material->m_backDiffuseColor );
  }

  inline const nvmath::Vec3f& Material::getEmissiveColor( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontEmissiveColor : m_material->m_backEmissiveColor );
  }

  inline float Material::getOpacity( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontOpacity : m_material->m_backOpacity );
  }

  inline const nvmath::Vec3f& Material::getSpecularColor( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontSpecularColor : m_material->m_backSpecularColor );
  }

  inline const nvmath::Vec3f& Material::getOpacityColor( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontOpacityColor : m_material->m_backOpacityColor );
  }

  inline const nvmath::Vec3f& Material::getReflectivityColor( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontReflectivityColor : m_material->m_backReflectivityColor );
  }

  inline float Material::getSpecularExponent( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontSpecularExponent : m_material->m_backSpecularExponent );
  }

  inline float Material::getIndexOfRefraction( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontIOR : m_material->m_backIOR );
  }

  inline float Material::getReflectivity( MaterialFace mf ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( mf != MF_FRONT_AND_BACK );
    return( ( mf & MF_FRONT ) ? m_material->m_frontReflectivity : m_material->m_backReflectivity );
  }

} //  namespace nvsg


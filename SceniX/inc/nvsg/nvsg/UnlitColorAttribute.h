// Copyright NVIDIA Corporation 2007-2011
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
#include "nvsg/StateAttribute.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief StateAttribute specifying the color in case there is no lighting.
   *  \remarks The color of an UnlitColorAttribute is used as the color of an object when lighting
   *  is off. This can happen, when that object is drawn in wireframe mode. If there is no
   *  UnlitColorAttribute associated with a Drawable, the diffuse color of the associated Material
   *  is used. If there is no associated Material either, a default color is used. */
  class UnlitColorAttribute : public StateAttribute
  {
    public:
      NVSG_API static UnlitColorAttributeSharedPtr create();

    public:
      /*! \brief Get the color.
       *  \return The color of this UnlitColorAttribute.
       *  \sa setColor */
      NVSG_API const nvmath::Vec4f & getColor() const;

      /*! \brief Set the color.
       *  \param color The color for this UnlitColorAttribute.
       *  \sa getColor */
      NVSG_API void setColor( const nvmath::Vec4f & color );

      /*! \brief Test for equivalence with another UnlitColorAttribute.
       *  \param p            Pointer to the constant UnlitColorAttribute to test for equivalence with.
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c true
       *  \param deepCompare  Optional parameter to perform a deep comparsion; default is \c false
       *  \return \c true if the UnlitColorAttribute \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as StateAttribute, they are considered to be
       *  equivalent if they have the same color.
       *  \note The behaviour is undefined if \a p is not a UnlitColorAttribute nor derived from one.
       *  \sa StateAttribute, getColor */
      NVSG_API virtual bool isEquivalent( const Object *p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, UnlitColorAttribute );
    protected:
      friend struct nvutil::Holder<UnlitColorAttribute>;

      /*! \brief Default-constructs an UnlitColorAttribute.
       *  \remarks The default-constructed UnlitColorAttribute initially is an opaque light grey
       *  (0.9,0.9,0.9,1.0). */
      NVSG_API UnlitColorAttribute();

      /*! \brief Constructs a UnlitColorAttribute as a copy from another UnlitColorAttribute. */
      NVSG_API UnlitColorAttribute( const UnlitColorAttribute &rhs );

      /*! \brief Destructs a UnlitColorAttribute. */
      NVSG_API virtual ~UnlitColorAttribute();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      nvmath::Vec4f m_color;
  };

  inline const nvmath::Vec4f & UnlitColorAttribute::getColor() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_color );
  }

  inline void UnlitColorAttribute::setColor( const nvmath::Vec4f & color )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_color != color )
    {
      m_color = color;
      notifyChange( this, NVSG_MATERIAL_INCARNATION );
    }
  }

}

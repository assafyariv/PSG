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
#include "nvsg/StateAttribute.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief StateAttribute indicating whether lighting should be disabled.
   *  \par Namespace: nvsg
   *  \remarks SceniX's default mode is to render all geometry with lighting 
   *           enabled.  This state attribute provides a means to force lighting
   *           off for a set of geometry.  Since the default mode is
   *           to render with lighting enabled, it is NOT NECESSARY to add this
   *           state attribute to all StateSets with geometry that should be
   *           affected by lighting.  Therefore, adding this state attribute
   *           and setting the value to "enabled" is essentially a NO-OP.
   *  \sa StateAttribute,  */
  class LightingAttribute : public StateAttribute
  {
    public:
      NVSG_API static LightingAttributeSharedPtr create();

    public:
      /*! \brief Set the enabled state.
       *  \param state the enabled or disabled state of the attribute.
       *  \sa getEnabled */
      NVSG_API void setEnabled( bool state );

      /*! \brief Get the enabled state.
       *  \return bool indicating the current state
       *  \sa setEnabled */
      NVSG_API bool getEnabled() const;

      /*! \brief Test for equivalence with another LightingAttribute.
       *  \param p            Pointer to the constant LightingAttribute to test for equivalence with.
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c true
       *  \param deepCompare  Optional parameter to perform a deep comparsion; default is \c false
       *  \return \c true if the LightingAttribute \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as StateAttribute, they are considered to be
       *  equivalent if they have the same state value.
       *  \note The behaviour is undefined if \a p is not a LightingAttribute nor derived from one.
       *  \sa StateAttribute, getAlphaFunction */
      NVSG_API virtual bool isEquivalent( const Object *p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      /*! \brief Assignment operator
      *  \param rhs A reference to the constant LightingAttribute to copy from
      *  \return A reference to the assigned LightingAttribute
      *  \remarks The assignment operator calls the assignment operator of StateAttribute and
      *  copies the LightingAttribute specific data then.
      *  \sa PrimitiveAttribute */
      NVSG_API LightingAttribute & operator=(const LightingAttribute & rhs);

      REFLECTION_INFO_API( NVSG_API, LightingAttribute );
    protected:
      friend struct nvutil::Holder<LightingAttribute>;

      /*! \brief Default-constructs a LightingAttribute. 
       *  \remarks A default-constructed LightingAttribute is initialized with 
       *   its state set to DISABLED */
      NVSG_API LightingAttribute();

      /*! \brief Constructs a LightingAttribute as a copy from another LightingAttribute.
       *  \param rhs Source LightingAttribute. */ 
      NVSG_API LightingAttribute( const LightingAttribute &rhs );

      /*! \brief Destructs a LightingAttribute. */
      NVSG_API virtual ~LightingAttribute();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      bool  m_enabled;
  };

  inline bool LightingAttribute::getEnabled() const
  {
    return( m_enabled );
  }

} // namespace nvsg

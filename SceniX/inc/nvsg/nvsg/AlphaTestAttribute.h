// Copyright NVIDIA Corporation 2002-2005
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

#include "nvsg/StateAttribute.h"
#include "nvutil/HashGenerator.h"


namespace nvsg
{
  /*! \brief Enumeration to identify the AlphaFunc conditional.
   *  \par Namespace: nvsg
   *  \remarks AlphaTest can be used in cases where the fragment is to be drawn
   *   or culled based on the alpha value.  The AlphaFunction along with
   *   the threshold value define the equation used to determine whether the
   *   fragment is drawn.
   *   By default the comparison function is set to AF_ALWAYS, the threshold 
   *   value is 0.0f and the "enabled during MultiSampleAlpha" flag is set to true.
   *  \sa AlphaTestAttribute */
  typedef enum
  {
    AF_NEVER = 0,  //!< Never draw the fragment                      
    AF_LESS,       //!< Draw the fragment if fragment.a <  threshold 
    AF_EQUAL,      //!< Draw the fragment if fragment.a == threshold 
    AF_LEQUAL,     //!< Draw the fragment if fragment.a <= threshold    
    AF_GREATER,    //!< Draw the fragment if fragment.a >  threshold 
    AF_NOTEQUAL,   //!< Draw the fragment if fragment.a != threshold 
    AF_GEQUAL,     //!< Draw the fragment if fragment.a >= threshold 
    AF_ALWAYS      //!< Always draw the fragment                     
  } AlphaFunction;

  /*! \brief StateAttribute that defines the Alpha test.
   *  \par Namespace: nvsg
   *  \remarks An AlphaTestAttribute defines an equation that can be used to
   *   cull or draw fragments based on the alpha value of that fragment.  
   *   AlphaTestAttribute reduces the alpha test to a simple pass fail based
   *   on the supplied AlphaFunction and threshold value.  If the fragment
   *   passes the test, it is drawn - otherwise it is culled.
   *  \sa BlendAttribute, LineAttribute, PointAttribute, StateAttribute  */
  class AlphaTestAttribute : public StateAttribute
  {
    public:
      NVSG_API static AlphaTestAttributeSharedPtr create();

    public:
      /*! \brief Set the comparison function.
       *  \param func The conditional for the comparison equation.
       *  \remarks This function controls whether alpha values will cause
       *   certain fragments to be culled or drawn.  If the equation:
       *   (fragment.a AlphaFunction threshold) == true, the fragment is
       *   drawn.
       *  \sa AlphaFunction */
      NVSG_API void setAlphaFunction( AlphaFunction func );

      /*! \brief Get the compare function.
       *  \return nvsg::AlphaFunction indicating the compare function.
       *  \sa setAlphaFunction */
      NVSG_API AlphaFunction getAlphaFunction() const;

      /*! \brief Set the threshold value.
       *  \param threshold The threshold value used as described in 
       *   setAlphaFunction.
       *  \sa getThreshold */
      NVSG_API void setThreshold( float threshold );

      /*! \brief Get the threshold value.
       *  \return float indicating the current threshold value
       *  \sa setThreshold */
      NVSG_API float getThreshold() const;

      /*! \brief Get the "enabled during MultiSampleAlpha" flag.
       *  \return bool indicating whether this StateAttribute should be left enabled
       *               or set to disabled when using MultiSampleAlpha Coverage for transparency.
       *  \sa setAffectsMultiSampleAlpha */
      NVSG_API bool getEnabledDuringMultiSampleAlpha() const;

      /*! \brief Set the "enabled during MultiSampleAlpha" flag.
       *  \param state boolean indicating whether this StateAttribute should be left enabled
       *               or set to disabled when using MultiSampleAlpha Coverage for transparency.
       *               Disabling AlphaFunc during MultiSample can provide improved image quality
       *               (with reduced performance) in certain circumstances - specifically where 
       *               AlphaFunc is being used as a performance transparency mode. 
       *  \sa getEnabledDuringMultiSampleAlpha */
      NVSG_API void setEnabledDuringMultiSampleAlpha( bool state );

      /*! \brief Test for equivalence with another AlphaTestAttribute.
       *  \param p            Pointer to the constant AlphaTestAttribute to test for equivalence with.
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c true
       *  \param deepCompare  Optional parameter to perform a deep comparsion; default is \c false
       *  \return \c true if the AlphaTestAttribute \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as StateAttribute, they are considered to be
       *  equivalent if they have the same nvsg::AlphaFunction and the same threshold values.
       *  \note The behaviour is undefined if \a p is not a AlphaTestAttribute nor derived from one.
       *  \sa StateAttribute, getAlphaFunction */
      NVSG_API virtual bool isEquivalent( const Object *p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      /*! \brief Assignment operator
      *  \param rhs A reference to the constant AlphaTestAttribute to copy from
      *  \return A reference to the assigned AlphaTestAttribute
      *  \remarks The assignment operator calls the assignment operator of StateAttribute and
      *  copies the AlphaTestAttribute specific data then.
      *  \sa PrimitiveAttribute */
      NVSG_API AlphaTestAttribute & operator=(const AlphaTestAttribute & rhs);

      REFLECTION_INFO_API( NVSG_API, AlphaTestAttribute );
    protected:
      friend struct nvutil::Holder<AlphaTestAttribute>;

      /*! \brief Default-constructs a AlphaTestAttribute. 
       *  \remarks A default-constructed AlphaTestAttribute is initialized with 
       *   AlphaFunction to AF_ALWAYS and threshold to 0.0 - allowing all
       *   fragments to be drawn. */
      NVSG_API AlphaTestAttribute();

      /*! \brief Constructs a AlphaTestAttribute as a copy from another AlphaTestAttribute.
       *  \param rhs Source AlphaTestAttribute. */ 
      NVSG_API AlphaTestAttribute( const AlphaTestAttribute &rhs );

      /*! \brief Destructs a AlphaTestAttribute. */
      NVSG_API virtual ~AlphaTestAttribute();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      AlphaFunction  m_alphaFunction;
      float          m_threshold;
      bool           m_enabledDuringMultiSampleAlpha;
  };

  inline AlphaFunction AlphaTestAttribute::getAlphaFunction() const
  {
    return( m_alphaFunction );
  }

  inline float AlphaTestAttribute::getThreshold() const
  {
    return( m_threshold );
  }

  inline bool AlphaTestAttribute::getEnabledDuringMultiSampleAlpha() const
  {
    return( m_enabledDuringMultiSampleAlpha );
  }

} // namespace nvsg

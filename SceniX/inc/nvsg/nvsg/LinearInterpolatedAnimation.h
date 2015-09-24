// Copyright NVIDIA Corporation 2002-2n005
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

#include "nvsgcommon.h"
#include "nvsg/KeyFramedAnimation.h"
#include "nvsg/VertexAttribute.h"
#include "nvmath/Quatt.h"

namespace nvsg
{

  /*! Helper class which translates AnimationCode<Type> to ObjectCode **/
    template <typename T> 
    struct LinearInterpolatedAnimationDescriptionObjectCode; // Generates compile time error for unsupported types in Animation

    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<nvmath::Trafo>    { enum { ObjectCode = OC_LINEARINTERPOLATEDTRAFOANIMATIONDESCRIPTION  }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<VertexAttribute>  { enum { ObjectCode = OC_LINEARINTERPOLATEDVERTEXATTRIBUTEANIMATIONDESCRIPTION  }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<float>            { enum { ObjectCode = OC_LINEARINTERPOLATEDFLOATANIMATIONDESCRIPTION  }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<nvmath::Vec2f>    { enum { ObjectCode = OC_LINEARINTERPOLATEDVEC2FANIMATIONDESCRIPTION  }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<nvmath::Vec3f>    { enum { ObjectCode = OC_LINEARINTERPOLATEDVEC3FANIMATIONDESCRIPTION  }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<nvmath::Vec4f>    { enum { ObjectCode = OC_LINEARINTERPOLATEDVEC4FANIMATIONDESCRIPTION  }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<nvmath::Mat33f>   { enum { ObjectCode = OC_LINEARINTERPOLATEDMAT33FANIMATIONDESCRIPTION }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<nvmath::Mat44f>   { enum { ObjectCode = OC_LINEARINTERPOLATEDMAT44FANIMATIONDESCRIPTION }; };
    template <> struct LinearInterpolatedAnimationDescriptionObjectCode<nvmath::Quatf>    { enum { ObjectCode = OC_LINEARINTERPOLATEDQUATFANIMATIONDESCRIPTION  }; };
  /*! \brief An KeyFramedAnimationDescription class that performs linear interpolation.
   *  \par Namespace: nvsg
   *  \remarks The LinearInterpolatedAnimationDescription of type T essentially is an KeyFramedAnimationDescription
   *  with the constant random access operator being overloaded.
   *  \note Currently the only supported types for LinearInterpolatedAnimationDescription are nvmath::Trafo
   *  and VertexAttribute
   *  \sa KeyFramedAnimationDescription, nvmath::Trafo, VertexAttribute */
  template<typename T> 
  class LinearInterpolatedAnimationDescription : public KeyFramedAnimationDescription<T>
  {
    public:
      static typename nvutil::ObjectTraits<LinearInterpolatedAnimationDescription<T> >::SharedPtr create();

    public:
      /*! \brief Default-constructs a LinearInterpolatedAnimationDescription<T>. */
      LinearInterpolatedAnimationDescription();

      /*! \brief Copy-constructs a LinearInterpolatedAnimationDescription<T>.
       *  \param rhs Source LinearInterpolatedAnimationDescription<T>. */
      LinearInterpolatedAnimationDescription( const LinearInterpolatedAnimationDescription &rhs );

      /*! \brief Destructs a LinearInterpolatedAnimationDescription<T>. */
      virtual ~LinearInterpolatedAnimationDescription();

      /*! \brief Get the potentially interpolated value at the specified step.
       *  \param step Index of the Animation to get the value at.
       *  \return The value at the specified step.
       *  \remarks If \a step is defined by a key frame of this LinearInterpolatedAnimationDescription, its
       *  value is returned. Otherwise, the linear interpolation between the two neighboring
       *  key frames is calculated and returned. */
      virtual T getStepValue( unsigned int step ) const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant LinearInterpolatedAnimationDescription of type \c T to copy
       *  from.
       *  \return A reference to the assigned LinearInterpolatedAnimationDescription of type \c T.
       *  \remarks The assignment operator just calls the assignment operator of the
       *  KeyFramedAnimationDescription of type \c T. */
      LinearInterpolatedAnimationDescription<T> & operator=(const LinearInterpolatedAnimationDescription<T> & rhs);
  };

  template<typename T>
  inline typename nvutil::ObjectTraits<LinearInterpolatedAnimationDescription<T> >::SharedPtr LinearInterpolatedAnimationDescription<T>::create()
  {
    return( nvutil::ObjectTraits<LinearInterpolatedAnimationDescription<T> >::Handle::create() );
  }

  template<typename T>
  inline LinearInterpolatedAnimationDescription<T>::LinearInterpolatedAnimationDescription()
  {
    NVSG_TRACE();
    this->m_objectCode = LinearInterpolatedAnimationDescriptionObjectCode<T>::ObjectCode;
  }

  template<typename T> 
  inline LinearInterpolatedAnimationDescription<T>::LinearInterpolatedAnimationDescription( const LinearInterpolatedAnimationDescription<T> &rhs )
    : KeyFramedAnimationDescription<T>(rhs)
  {
    NVSG_TRACE();
    this->m_objectCode = LinearInterpolatedAnimationDescriptionObjectCode<T>::ObjectCode;
  }

  template<typename T> 
  inline LinearInterpolatedAnimationDescription<T>::~LinearInterpolatedAnimationDescription()
  {
    NVSG_TRACE();
  }

  template<typename T> 
  inline T LinearInterpolatedAnimationDescription<T>::getStepValue( unsigned int step ) const
  {
    NVSG_TRACE();

    unsigned int key = this->getKey( step );
    if ( this->getKeyStep( key ) == step ) //  step is on a key => use this key step
    {
      return( this->getKeyValue( key ) );
    }
    else    //  step is between key-1 and key
    {
      NVSG_ASSERT( key != 0 );
      T stepValue;
      nvmath::lerp( (float)( step - this->getKeyStep(key-1) ) / ( this->getKeyStep(key) - this->getKeyStep(key-1) )
                  , this->getKeyValue( key-1 ), this->getKeyValue( key ), stepValue );
      return( stepValue );
    }
  }

  template<typename T>
  inline LinearInterpolatedAnimationDescription<T> & LinearInterpolatedAnimationDescription<T>::operator=(const LinearInterpolatedAnimationDescription<T> & rhs)
  {
    NVSG_TRACE();
    return( KeyFramedAnimationDescription<T>::operator=(rhs) );    // invalidates incarnation
  }
}

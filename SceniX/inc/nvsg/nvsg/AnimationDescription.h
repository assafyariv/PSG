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
/** @file */

#include "nvsg/OwnedObject.h"
#include "nvsg/Animation.h"

namespace nvsg
{
  /*! \brief Base class to describe the data of and access to an animation
   *  \remarks A concrete AnimationDescription provides the per-step access to the animation data.
   *  \sa AnimationState, Animation, FramedAnimationDescription, KeyFramedAnimationDescription */
  template<typename T>
  class AnimationDescription : public OwnedObject<Animation<T> >
  {
    public:
      virtual ~AnimationDescription();

      /*! \brief Test for equivalence with an other AnimationDescription.
       *  \param p A pointer to an Object to test for equivalence with.
       *  \param ignoreNames  \c true, if names are to be ignored in the equivalence test, otherwise \c false.
       *  \param deepCompare  \c true, if a deep compare should be done, otherwise \c false.
       *  \return \c true, if \a this and \a p are equivalent, otherwise \c false.
       *  \remarks An Object \a p is considered to be equivalent to the AnimationDescription \a this,
       *  if it is also an AnimationDescription, and it is equivalent to \c this as an Object. */
      virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Interface to get the number of steps in this AnimationDescription.
       *  \return The number of steps in this AnimationDescription.
       *  \remarks This is the number of steps to run once from the first to the last defined frame.
       *  \sa getNumberOfFrames */
      virtual unsigned int getNumberOfSteps() const = 0;

      /*! \brief Interface to get the value of the AnimationDescription at the specified step.
       *  \param step The step to get the value of the AnimationDescription.
       *  \return The value of the AnimationDescription at the specified step.
       *  \sa getNumberOfSteps */
      virtual T getStepValue( unsigned int step ) const = 0;

      /*! \brief Interface to replace the value of the AnimationDescription at the specified step.
       *  \param step The step to be replaced in the AnimationDescription
       *  \param value The value to be set in the AnimationDescription at the specified step.
       *  \sa getNumberOfSteps, getStepValue, removeStep */
      virtual void replaceStep( unsigned int step, const T & value ) = 0;

      /*! \brief Interface to remove one step of the AnimationDescription.
       *  \param step The step to remove from the AnimationDescription.
       *  \sa getNumberOfSteps, getStepValue, replaceStep */
      virtual void removeStep( unsigned int step ) = 0;

      void invalidate();

    protected:
      /*! \brief Protected constructor to prevent explicit creation.
       *  \remarks Because AnimationDescription is an abstract class, its constructor is never used
       *  explicitly, but rather in the constructor of derived classes.
       *  \sa FramedAnimationDescription, KeyFramedAnimationDescription */
      AnimationDescription( void );

      /*! \brief Protected assignment operator 
       *  \param rhs Reference of the constant AnimationDescription to copy from
       *  \return A reference to the assigned AnimationDescription.
       *  \remarks The assignment operator just calls the assignment operator of Object.
       *  \sa Object */
      AnimationDescription & operator=( const AnimationDescription & rhs );
  };

  template<typename T>
  inline AnimationDescription<T>::AnimationDescription()
  {
    NVSG_TRACE();
  }

  template<typename T>
  inline AnimationDescription<T>::~AnimationDescription()
  {
    NVSG_TRACE();
  }

  template<typename T>
  inline AnimationDescription<T> & AnimationDescription<T>::operator=( const AnimationDescription<T> & rhs )
  {
    NVSG_TRACE();
    if (&rhs != this)
    {
      Object::operator=(rhs);   // invalidates incarnation
    }
    return *this;
  }

  template<typename T>
  inline bool AnimationDescription<T>::isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const
  {
    NVSG_TRACE();
    return(     ( dynamic_cast<const AnimationDescription<T>*>(p) != NULL )
            &&  Object::isEquivalent( p, ignoreNames, deepCompare ) );
  }

  template<typename T>
  inline void AnimationDescription<T>::invalidate()
  {
    NVSG_TRACE();
    notifyChange( this );
  }
} //  namespace nvsg

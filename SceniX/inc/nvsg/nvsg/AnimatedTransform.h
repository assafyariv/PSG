// Copyright NVIDIA Corporation 2002-2007
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

#include "nvsg/Animation.h"
#include "nvsg/Transform.h" // base class definition
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Class representing an animated Transform.
   *  \par Namespace: nvsg
   *  \remarks An AnimatedTransform is a Transform that also holds an Animation of nvmath::Trafo
   *  objects. The nvmath::Trafo to use is determined from the Animation of each frame.
   *  \sa Animation, nvmath::Trafo, Transform */
  class AnimatedTransform : public Transform
  {
    public:
      NVSG_API static AnimatedTransformSharedPtr create();

    public:
      /*! \brief Get the current transformation represented by this AnimatedTransform.
       *  \return The nvmath::Trafo, representing the current transformation.
       *  \sa nvmath::Trafo, setTrafo */
      NVSG_API virtual const nvmath::Trafo& getTrafo( void ) const;

      /*! \brief Set the current transformation for this AnimatedTransform.
       *  \param trafo A reference to the constant nvmath::Trafo to use.
       *  \remarks The current transformation is replaced by \a trafo.
       *  \sa nvmath::Trafo, getTrafo */
      NVSG_API virtual void setTrafo( const nvmath::Trafo & trafo );

      /*! \brief Get a pointer to the constant Animation.
       *  \return The Animation of this AnimatedTransform.
       *  \sa Animation, nvmath::Trafo */
      NVSG_API const TrafoAnimationSharedPtr & getAnimation() const;

      /*! \brief Set the Animation. 
       *  \param animation The Animation to set.
       *  \remarks If these a current Animation and the new Animation hold a different number of frames,
       *  the number of frames is invalidated. The current frame is reset to zero.
       *  \note The behavior is undefined if \a animation is not valid.
       *  \sa Animation, nvmath::Trafo, invalidateAnimationContainment */
      NVSG_API void setAnimation( const TrafoAnimationSharedPtr & animation );

      /*! \brief Assignment operator 
       *  \param rhs  Reference to a constant AnimatedTransform to copy from
       *  \return A reference to the assigned AnimatedTransform
       *  \remarks The assignment operator first calls the assignment operator of Transform. Then
       *  the reference count of the current Animation is decremented, the Animation of \a rhs is
       *  cloned, and it's reference count is incremented.
       *  \sa Animation, Transform */
      NVSG_API AnimatedTransform & operator=( const AnimatedTransform & rhs );
      
      /*! \brief Test for equivalence with another AnimatedTransform. 
       *  \param p            Pointer to the constant AnimatedTransform to test for equivalence
       *  with
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c
       *  true
       *  \param deepCompare  Optional parameter to perform a deep comparsion; default is \c false
       *  \return \c true if the AnimatedTransform \a p is equivalent to \c this, otherwise \c
       *  false.
       *  \remarks If \a p and \c this are equivalent as Transform objects, their Animation
       *  objects are tested for equivalence. If \a deepCompare is false, they are considered to
       *  be equivalent if they are the same pointers. Otherwise, a full equivalence test is
       *  performed.
       *  \note The behavior is undefined if \a p is not an AnimatedTransform nor derived from
       *  one.
       *  \sa Animation, Object, Transform */
      NVSG_API virtual bool isEquivalent( const Object *p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      REFLECTION_INFO_API( NVSG_API, AnimatedTransform );
    protected:
      friend struct nvutil::Holder<AnimatedTransform>;

      /*! \brief Default-constructs a AnimatedTransform. 
       */
      NVSG_API AnimatedTransform();

      /*! \brief Constructs a AnimatedTransform as a copy of another AnimatedTransform. 
       */
      NVSG_API AnimatedTransform(const AnimatedTransform& rhs);

      /*! Destructs a AnimatedTransform
       */
      NVSG_API virtual ~AnimatedTransform();

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;
      
      /*! \brief Returns whether an animation is attached
       *  \return \c true if an animation is attached. \c false otherwise.
       *  \remarks This function overrides the base class implementation to 
       *  report back correct status of animation containment. It is called
       *  from the framework on query through the Object::containsAnimation 
       *  API if the current animation containment status is detected not valid.
       *  \sa Object::containsAnimation */
      NVSG_API virtual bool determineAnimationContainment() const;

      /*! \brief Calculates the AnimatedTransform's axis-aligned bounding box
       * \return 
       * The function returns the AnimatedTransform's axis-aligned bounding box in world space.
       * \remarks
       * This function overrides Transform::calculateBoundingBox in order to also flag
       * the bounding box of the attached Animation object valid. This is required for 
       * the reverse process of invalidating bounding volumes to work as expected.
       * The bounding volume of an AnimatedTransform becomes invalid as the attached 
       * Animation advances. In this case the Animation calls invalidateBoundingVolumes
       * to notify owning Objects about the status change. The change in bounding 
       * volumes will be communicated up the hierarchy only if the bounding volume
       * status of the Animation is flagged valid.
       * \sa Object::invalidateBoundingVolumes */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;
      
      /*! \brief Calculates the AnimatedTransform's bounding sphere
      * \return 
      * The function returns the AnimatedTransform's bounding sphere in world space.
      * \remarks
      * This function overrides Transform::calculateBoundingBox in order to also flag
      * the bounding box of the attached Animation object valid. This is required for 
      * the reverse process of invalidating bounding volumes to work as expected.
      * The bounding volume of an AnimatedTransform becomes invalid as the attached 
      * Animation advances. In this case the Animation calls invalidateBoundingVolumes
      * to notify owning Objects about the status change. The change in bounding 
      * volumes will be communicated up the hierarchy only if the bounding volume
      * status of the Animation is flagged valid.
      * \sa Object::invalidateBoundingVolumes */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:

      void invalidate();

    private:
      TrafoAnimationSharedPtr m_animation;
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline const nvmath::Trafo& AnimatedTransform::getTrafo() const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_animation );
    return( TrafoAnimationReadLock(m_animation)->getCurrentValue() );
  }

  inline const TrafoAnimationSharedPtr & AnimatedTransform::getAnimation() const
  {
    NVSG_TRACE();
    return( m_animation );
  }

  inline bool AnimatedTransform::determineAnimationContainment() const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_animation );
    return( true );
  }

} //  namespace nvsg

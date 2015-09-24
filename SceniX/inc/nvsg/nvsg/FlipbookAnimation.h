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

#include "nvsgcommon.h" // commonly used stuff
#include "nvsg/Animation.h"
#include "nvsg/Group.h" // base class definition
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Class representing a flipbook animation.
   *  \par Namespace: nvsg
   *  \remarks A FlipbookAnimation is a kind of Group that holds an Animation of indices. For a
   *  given animation step the corresponding index specifies which child of the FlipbookAnimation is
   *  to use.
   *  \sa Animation, Group */
  class FlipbookAnimation : public Group
  {
    public:
      NVSG_API static FlipbookAnimationSharedPtr create();

    public:
      /*! \brief Get the Animation.
       *  \return The Animation.
       *  \sa setAnimation */
      NVSG_API const IndexAnimationSharedPtr & getAnimation() const;

      /*! \brief Set the Animation. 
       *  \param animation The Animation to set
       *  \remarks If any current Animation and the new Animation hold a different number of frames,
       *  the number of frames is invalidated. The current frame is reset to zero.
       *  \note The behavior is undefined if \a animation is not valid.
       *  \sa Animation, invalidateAnimationContainment */
      NVSG_API void setAnimation( const IndexAnimationSharedPtr & animation );

      /*! \brief Query if this objects contains an animation.
      *  \return \c true.
      *  \remarks As a FlipbookAnimation always holds an Animation, this always is true. */
      NVSG_API virtual bool determineAnimationContainment() const;

      /*! \brief Assignment operator 
       *  \param rhs  Reference to a constant FlipbookAnimation to copy from
       *  \return A reference to the assigned FlipbookAnimation
       *  \remarks The assignment operator first calls the assignment operator of Group. Then
       *  the reference count of the current Animation is decremented, the Animation of \a rhs is
       *  cloned, and it's reference count is incremented.
       *  \sa Animation, Group */
      NVSG_API FlipbookAnimation & operator=( const FlipbookAnimation & rhs );

      /*! \brief Test for equivalence with another FlipbookAnimation. 
       *  \param p Pointer to the constant FlipbookAnimation to test for equivalence with
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false
       *  \return \c true if the FlipbookAnimation \a p is equivalent to \c this, otherwise \c
       *  false.
       *  \remarks If \a p and \c this are equivalent as Group objects, their Animation
       *  objects are tested for equivalence. If \a deepCompare is false, they are considered to
       *  be equivalent if they are the same pointers. Otherwise, a full equivalence test is
       *  performed.
       *  \note The behavior is undefined if \a p is not a FlipbookAnimation nor derived from one.
       *  \sa Animation, Object */
      NVSG_API virtual bool isEquivalent( const Object *p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      /*! \brief Get the child for the current animation frame.
       *  \return The Node to be used for the current animation frame. */
      NVSG_API const NodeSharedPtr & getCurrentChild() const;

      REFLECTION_INFO_API( NVSG_API, FlipbookAnimation );
    protected:
      friend struct nvutil::Holder<FlipbookAnimation>;

      /*! \brief Default-constructs a FlipbookAnimation. */
      NVSG_API FlipbookAnimation();

      /*! \brief Constructs a FlipbookAnimation as a copy of another FlipbookAnimation. */
      NVSG_API FlipbookAnimation(const FlipbookAnimation& rhs);

      /*! \brief Destructs a FlipbookAnimation */
      NVSG_API virtual ~FlipbookAnimation();

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      IndexAnimationSharedPtr m_animation;
  };

  // - - - - - - - - - - - - - - - - - - - - - - - - - 
  // inlines
  // - - - - - - - - - - - - - - - - - - - - - - - - - 

  inline const IndexAnimationSharedPtr & FlipbookAnimation::getAnimation() const
  {
    NVSG_TRACE();
    return( m_animation );
  }
} //  namespace nvsg

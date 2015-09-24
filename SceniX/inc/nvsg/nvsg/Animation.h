// Copyright NVIDIA Corporation 2002-2010
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

#include "nvmath/Trafo.h"
#include "nvsg/OwnedObject.h"
#include "nvsg/AnimationDescription.h"
#include "nvsg/VertexAttribute.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Base class of all Animation classes
   *  \remarks An AnimationState holds the status data of an Animation. Those data are independent
   *  of the type of the animation data. */
  class AnimationState : public OwnedObject<Object>
  {
    friend class Object;

    public:
      NVSG_API virtual ~AnimationState();

      /*! \brief Start the Animation.
       *  \remarks While the Animation is running, calling advance() advances the Animation.
       *  \note Starting the Animation invalidates the incarnation.
       *  \sa stop, advance, isRunning, setSpeed, getSpeed */
      NVSG_API void start();

      /*! \brief Stop the Animation.
       *  \remarks While the Animation is stopped, calling advance() does nothing.
       *  \note Stopping the Animation invalidates the incarnation.
       *  \sa start, advance, isRunning */
      NVSG_API void stop();

      /*! \brief Query if this Animation is running.
       *  \return \c true, if the Animation is running, otherwise \c false.
       *  \sa start, stop */
      NVSG_API bool isRunning() const;

      /*! \brief Advance the Animation.
       *  \param tick The current tick.
       *  \param delta The number of steps to go.
       *  \return \c true, if the frame did change due to the running animation, otherwise \c false.
       *  \remarks If the Animation is running and its current tick is different from \a tick, the
       *  Animation is advanced by the number of steps specified by setSpeed() times \a delta,
       *  and \a tick is stored as the current tick. Otherwise nothing is done.
       *  \note Advancing the Animation invalidates the incarnation.
       *  \sa start, stop, isRunning, setSpeed, getSpeed */
      NVSG_API bool advance( unsigned int tick, int delta = 1 );

      /*! \brief Set the animation speed.
       *  \param stepsOnAdvance The number of steps in the Animation to advance on a new tick.
       *  \remarks Whenever advance() is called with a different tick while the Animation is
       *  running, the Animation is advanced by \a stepsOnAdvance steps.
       *  \note Changing the speed invalidates the incarnation.
       *  \sa getSpeed, advance, start, stop */
      NVSG_API void setSpeed( unsigned int stepsOnAdvance );

      /*! \brief Get the animation speed.
       *  \return The number of steps in the Animation to advance on a new tick.
       *  \remarks Whenever advance() is called with a different tick while the Animation is
       *  running, the Animation is advanced by the number of steps returned.
       *  \sa setSpeed */
      NVSG_API unsigned int getSpeed() const;

      /*! \brief Get the number of frames in this Animation.
       *  \return The number of frames in this Animation.
       *  \remarks The number of frames is determined by the number of steps, the number of loops,
       *  and the looping mode (either looping or swinging). If the Animation is infinite (due to
       *  the loop count set to zero) or larger than UINT_MAX, UINT_MAX is returned.
       *  \sa getNumberOfSteps */
      virtual unsigned int getNumberOfFrames() const = 0;

      /*! \brief Get the number of steps in this Animation.
       *  \return The number of steps in this Animation.
       *  \remarks The number of steps of an animation is the pure length of it, without considering
       *  any loops or the looping mode. It is always finite.
       *  \sa getNumberOfFrames */
      virtual unsigned int getNumberOfSteps() const = 0;

      /*! \brief Get the current frame index of this Animation.
       *  \return The current frame index of this Animation.
       *  \remarks The current frame is the currently used frame. The frame index is mapped to an
       *  animation step, using the number of steps, number of loops, and the looping/swinging mode.
       *  \sa getCurrentStep, getNumberOfFrames */
      NVSG_API unsigned int getCurrentFrame() const;

      /*! \brief Set the current frame index of this Animation.
       *  \return \c true, if the frame did change, otherwise \c false.
       *  \sa advance, getCurrentFrame */
      NVSG_API bool setCurrentFrame( unsigned int frame );

      /*! \brief The the current step index of this Animation.
       *  \return The current step index of this Animation.
       *  \remarks The current step is the currently used step. The step index is the index into the
       *  Animation that defines the currently used value.
       *  \sa getCurrentValue */
      NVSG_API unsigned int getCurrentStep() const;

      /*! \brief Set the number of loops to run.
       *  \param count The number of loops to run.
       *  \remarks An animation can either run "infinitely" (the loop count is 0), or a finite
       *  times. One run is from the first to the last frame in loop-mode (or from the last to the
       *  first if running backwards), or from the first to the last and back to the first in
       *  swing-mode (or from the last to the first and back to the last if running backwards).
       *  \note Changing the loop count invalidates the incarnation.
       *  \sa getLoopCount, setForwardAnimation, setSwingingAnimation */
      NVSG_API void setLoopCount( unsigned int count );

      /*! \brief Get the number of loops to run.
       *  \return The number of loops to run.
       *  \remarks An animation can either run "infinitely" (the loop count is 0), or a finite
       *  times. One run is from the first to the last frame in loop-mode (or from the last to the
       *  first if running backwards), or from the first to the last and back to the first in
       *  swing-mode (or from the last to the first and back to the last if running backwards).
       *  \sa setLoopCount, isForwardAnimation, isSwingingAnimation */
      NVSG_API int getLoopCount() const;

      /*! \brief Set the running direction of this Animation.
       *  \param forward \c true to set the direction to forward, \c false to set it to backwards.
       *  \remarks An Animation can either run forward or backward.
       *  \note Changing the direction of the Animation invalidates the incarnation.
       *  \sa isForwardAnimation, setSwingingAnimation, setLoopCount */
      NVSG_API void setForwardAnimation( bool forward = true );

      /*! \brief Query if this Animation is running in forward direction.
       *  \return \c true, if this Animation is running in forward direction, otherwise \c false.
       *  \remarks An Animation can either run forward or backward.
       *  \sa setForwardAnimation, isSwingingAnimation, getLoopCount */
      NVSG_API bool isForwardAnimation() const;

      /*! \brief Set the loop behavior of this Animation.
       *  \param swing \c true to set the Animation into swing-mode, \c false to set it to loop-mode.
       *  \remarks An Animation can either run in swing-mode or in loop-mode. In loop-mode, each
       *  iteration of the Animation is done in the same running direction. In swing-mode, each
       *  iteration of the Animation is once to and once fro.
       *  \note Changing the loop behavior of the Animation invalidates the incarnation.
       *  \sa isSwingingAnimation, isForwardAnimation, getLoopCount */
      NVSG_API void setSwingingAnimation( bool swing = true );

      /*! \brief Query if this Animation is running in swing-mode.
       *  \return \c true, if this Animation is running in swing-mode, otherwise \c false.
       *  \remarks An Animation can either run in swing-mode or in loop-mode. In loop-mode, each
       *  iteration of the Animation is done in the same running direction. In swing-mode, each
       *  iteration of the Animation is once to and once fro.
       *  \sa setSwingingAnimation, isForwardAnimation, getLoopCount */
      NVSG_API bool isSwingingAnimation() const;

      /*! \brief Test for equivalence with an other AnimationState.
       *  \param p A pointer to an Object to test for equivalence with.
       *  \param ignoreNames  \c true, if names are to be ignored in the equivalence test, otherwise \c false.
       *  \param deepCompare  \c true, if a deep compare should be done, otherwise \c false.
       *  \return \c true, if \a this and \a p are equivalent, otherwise \c false.
       *  \remarks An Object \a p is considered to be equivalent to the Animation \a this, if it is
       *  also an Animation of type \c T, and it is equivalent to \c this as an Object. */
      NVSG_API virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Signals a change in the AnimationState
       * \remarks 
       * Calling this function signals a change in the AnimationState. The function calls
       * the onInvalidate virtual function, which derived AnimationStates should override 
       * to invalidate their internal status. */
      NVSG_API void invalidate();

    protected:
      /*! \brief Protected constructor to prevent explicit creation.
       *  \remarks Because Animation is an abstract class, its constructor is never used
       *  explicitly, but rather in the constructor of derived classes.
       *  \sa FramedAnimationDescription, InterpolatedAnimation */
      NVSG_API AnimationState( void );

      /*! \brief Protected assignment operator 
       *  \param rhs  Reference of the constant Animation to copy from
       *  \return A reference to the assigned Animation.
       *  \remarks The assignment operator just calls the assignment operator of Object.
       *  \sa Object */
      NVSG_API AnimationState & operator=( const AnimationState & rhs );

      /*! \brief Sets the specified step as the current one.
       *  \param step The step index to set as the current one.
       *  \remarks This function invalidates the incarnation. */
      NVSG_API virtual void setCurrentStep( unsigned int step );

      /*! \brief Translates a frame index into a step index.
       *  \param frame The frame index to translate.
       *  \return The step index corresponding to the frame index \a frame.
       *  \remarks The index-operator operator[i] simply accesses the i'th value of an Animation. To
       *  access the value corresponding to a frame counter, you have to translate that frame
       *  counter to the step counter. This function does that calculation.
       *  \sa getNumberOfSteps */
      NVSG_API virtual unsigned int frameToStep( unsigned int frame ) const = 0;

      /*! \brief Called from invalidate
       *  \remarks
       *  This function gets called from the invalidate interface.
       *  For derived AnimationStates it is recommended to override this function
       *  in order to invalidate their internal animation status. 
       * \sa invalidate */
      NVSG_API virtual void onInvalidate() {};

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      //  defining values
      bool          m_forward;      // true: animate from step 0 to n-1; false; animate from step n-1 to 0
      bool          m_swinging;     // true: swing (or ping-pong); false: loop
      unsigned int  m_loopCount;    // 0: forever, other: number of sings/loops
      unsigned int  m_speed;        // steps per advance

      //  "temporary" values
      bool          m_running;      // true: animation is started; false: animation stopped
      unsigned int  m_currentFrame;
      unsigned int  m_currentTick;
      unsigned int  m_currentStep;
  };

  inline AnimationState::AnimationState()
    : m_loopCount(0)    // infinite
    , m_speed(1)        // single step
    , m_forward(true)   // forward
    , m_swinging(false) // looping
    , m_running(false)  // not running
    , m_currentStep(0)  // start of animation
    , m_currentFrame(0) // start of animation
    , m_currentTick(~0) // not defined
  {
    NVSG_TRACE();
  }

  inline AnimationState::~AnimationState()
  {
    NVSG_TRACE();
  }

  inline void AnimationState::invalidate()
  {
    NVSG_TRACE();
    // kick-off polymorphic invalidation
    onInvalidate();
    notifyChange( this );
  }

  inline void AnimationState::start()
  {
    NVSG_TRACE();

    if ( ! m_running )
    {
      m_running = true;
      invalidate();
    }
  }

  inline void AnimationState::stop()
  {
    NVSG_TRACE();

    if ( m_running )
    {
      m_running = false;
      invalidate();
    }
  }

  inline bool AnimationState::isRunning() const
  {
    NVSG_TRACE();

    return( m_running );
  }

  inline bool AnimationState::advance( unsigned int tick, int delta )
  {
    NVSG_TRACE();

    bool ok = ( m_running && ( m_currentTick != tick ) );
    if ( ok )
    {
      m_currentTick = tick;
      setCurrentFrame( m_currentFrame + delta * m_speed );
    }
    return( ok );
  }

  inline void AnimationState::setSpeed( unsigned int stepsOnAdvance )
  {
    NVSG_TRACE();
    NVSG_ASSERT( 0 < stepsOnAdvance );

    if ( m_speed != stepsOnAdvance )
    {
      m_speed = stepsOnAdvance;
      invalidate();
    }
  }

  inline unsigned int AnimationState::getSpeed() const
  {
    NVSG_TRACE();

    return( m_speed );
  }

  inline unsigned int AnimationState::getCurrentFrame() const
  {
    NVSG_TRACE();

    return( m_currentFrame );
  }

  inline bool AnimationState::setCurrentFrame( unsigned int frame )
  {
    NVSG_TRACE();

    bool ok = ( m_currentFrame != frame );
    if ( ok )
    {
      m_currentFrame = frame;
      unsigned int step = frameToStep( m_currentFrame );
      if ( step != m_currentStep )
      {
        setCurrentStep( step );
      }
    }
    return( ok );
  }

  inline void AnimationState::setCurrentStep( unsigned int step )
  {
    NVSG_TRACE();

    if ( m_currentStep != step )
    {
      m_currentStep = step;
      invalidate();
    }
  }

  inline unsigned int AnimationState::getCurrentStep() const
  {
    NVSG_TRACE();

    return( m_currentStep );
  }

  inline void AnimationState::setLoopCount( unsigned int count )
  {
    NVSG_TRACE();

    if ( m_loopCount != count )
    {
      m_loopCount = count;
      invalidate();
    }
  }

  inline int AnimationState::getLoopCount() const
  {
    NVSG_TRACE();

    return( m_loopCount );
  }

  inline void AnimationState::setForwardAnimation( bool forward )
  {
    NVSG_TRACE();

    if ( m_forward != forward )
    {
      m_forward = forward;
      invalidate();
    }
  }

  inline bool AnimationState::isForwardAnimation() const
  {
    NVSG_TRACE();

    return( m_forward );
  }

  inline void AnimationState::setSwingingAnimation( bool swinging )
  {
    NVSG_TRACE();

    if ( m_swinging != swinging )
    {
      m_swinging = swinging;
      invalidate();
    }
  }

  inline bool AnimationState::isSwingingAnimation() const
  {
    NVSG_TRACE();

    return( m_swinging );
  }

  inline AnimationState & AnimationState::operator=( const AnimationState & rhs )
  {
    NVSG_TRACE();

    if (&rhs != this)
    {
      Object::operator=(rhs);     // invalidates incarnation
      m_loopCount     = rhs.m_loopCount;
      m_speed         = rhs.m_speed;
      m_forward       = rhs.m_forward;
      m_swinging      = rhs.m_swinging;
      m_running       = rhs.m_running;
      m_currentFrame  = rhs.m_currentFrame;
      m_currentStep   = rhs.m_currentStep;
      m_currentTick   = rhs.m_currentTick;
    }
    return *this;
  }

  inline bool AnimationState::isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const
  {
    NVSG_TRACE();

    bool equi =   ( dynamic_cast<const AnimationState*>(p) != NULL )
              &&  Object::isEquivalent( p, ignoreNames, deepCompare );
    if ( equi )
    {
      const AnimationState * ab = static_cast<const AnimationState*>(p);
      equi =    ( m_loopCount == ab->m_loopCount  )
            &&  ( m_speed     == ab->m_speed      )
            &&  ( m_forward   == ab->m_forward    )
            &&  ( m_swinging  == ab->m_swinging   )
            &&  ( m_running   == ab->m_running    );
    }
    return( equi );
  }


  /*! Helper class for Animation **/
    template <typename T> 
    struct AnimationObjectCode; // Generates compile time error for unsupported types in Animation

    template <> struct AnimationObjectCode<unsigned int >    { enum { ObjectCode = OC_INDEXANIMATION  }; };
    template <> struct AnimationObjectCode<nvmath::Trafo>    { enum { ObjectCode = OC_TRAFOANIMATION  }; };
    template <> struct AnimationObjectCode<VertexAttribute>  { enum { ObjectCode = OC_VERTEXATTRIBUTEANIMATION  }; };
    template <> struct AnimationObjectCode<float>            { enum { ObjectCode = OC_FLOATANIMATION  }; };
    template <> struct AnimationObjectCode<nvmath::Vec2f>    { enum { ObjectCode = OC_VEC2FANIMATION  }; };
    template <> struct AnimationObjectCode<nvmath::Vec3f>    { enum { ObjectCode = OC_VEC3FANIMATION  }; };
    template <> struct AnimationObjectCode<nvmath::Vec4f>    { enum { ObjectCode = OC_VEC4FANIMATION  }; };
    template <> struct AnimationObjectCode<nvmath::Mat33f>   { enum { ObjectCode = OC_MAT33FANIMATION }; };
    template <> struct AnimationObjectCode<nvmath::Mat44f>   { enum { ObjectCode = OC_MAT44FANIMATION }; };
    template <> struct AnimationObjectCode<nvmath::Quatf>    { enum { ObjectCode = OC_QUATFANIMATION  }; };

  /*! \brief Template class for animations
   *  \remarks An Animation holds the status data of an Animation (via its base class
   *  AnimationState), and an AnimationDescription of the appropriate type, that holds the animation
   *  data. Together, you can use one set of animation data (AnimationDescription) from multiple
   *  Animations (of the same type, of course), to provide animations of different speed or
   *  directions, using the same animation data.
   *  \sa AnimationState, AnimationDescription */
  template<typename T> class Animation : public AnimationState
  {
    public:
      static typename nvutil::ObjectTraits<Animation<T> >::SharedPtr create();

    public:
      /*! \brief Default-constructs a FramedAnimationDescription. */
      Animation();

      /*! \brief Constructs an Animation as a copy of another Animation. */
      Animation( const Animation<T> &rhs );

      /*! \brief Destructs a FramedAnimationDescription */
      virtual ~Animation();

      /*! \brief Get the current value of this Animation.
       *  \return The current value of this Animation.
       *  \remarks The current value is used in the current animation step.
       *  \sa getCurrentStep */
      const T & getCurrentValue() const;

      /*! \brief Get the number of frames in this Animation.
       *  \return The number of frames in this Animation.
       *  \remarks The number of frames is determined by the number of steps, the number of loops,
       *  and the looping mode (either looping or swinging). If the Animation is infinite (due to
       *  the loop count set to zero) or larger than UINT_MAX, UINT_MAX is returned.
       *  \sa getNumberOfSteps */
      unsigned int getNumberOfFrames() const;

      /*! \brief Get the number of steps in this Animation.
       *  \return The number of steps in this Animation.
       *  \remarks The number of steps of an animation is the pure length of it, without considering
       *  any loops or the looping mode. It is always finite.
       *  \sa getNumberOfFrames */
      unsigned int getNumberOfSteps() const;

      /*! \brief Get the value of this Animation at the specified frame.
       *  \param frame The frame index of the Animation to get the value at.
       *  \return The value of this Animation at the specified step.
       *  \sa getStepValue */
      T getFrameValue( unsigned int frame ) const;

      /*! \brief Get the AnimationDescription of this Animation.
       *  \return The AnimationDescription of this Animation.
       *  \remarks An Animation holds an AnimationDescription that holds the data for this Animation.
       *  \sa setDescription */
      const typename nvutil::ObjectTraits<AnimationDescription<T> >::SharedPtr & getDescription() const;

      /*! \brief Set the AnimationDescription of this Animation.
       *  \param description The AnimationDescription to set.
       *  \remarks An Animation holds an AnimationDescription that holds the data for this Animation.
       *  \sa getDescription */
      void setDescription( const typename nvutil::ObjectTraits<AnimationDescription<T> >::SharedPtr & description );

      /*! \brief Test for equivalence with an other Animation.
       *  \param p A pointer to an Object to test for equivalence with.
       *  \param ignoreNames  \c true, if names are to be ignored in the equivalence test, otherwise \c false.
       *  \param deepCompare  \c true, if a deep compare should be done, otherwise \c false.
       *  \return \c true, if \a this and \a p are equivalent, otherwise \c false.
       *  \remarks An Object \a p  is considered to be equivalent to the Animation \a this, if it is
       *  also an Animation of type \c T, and it is equivalent to \c this as an Object. */
      virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      REFLECTION_INFO_TEMPLATE_API( NVSG_API, Animation<T> )
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( CurrentValue );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      /*! \brief Protected assignment operator 
       *  \param rhs  Reference of the constant Animation to copy from
       *  \return A reference to the assigned Animation.
       *  \remarks The assignment operator just calls the assignment operator of Object.
       *  \sa Object */
      Animation<T> & operator=( const Animation<T> & rhs );

      /*! \brief Tracks Animation's status changes.
       *  \remarks This function overrides AnimationState::onInvalidate to invalidate 
       *  internal animation status. */
      virtual void onInvalidate(); 

      /*! \brief Translates a frame index into a step index.
       *  \param frame The frame index to translate.
       *  \return The step index corresponding to the frame index \a frame.
       *  \remarks The index-operator operator[i] simply accesses the i'th value of an Animation. To
       *  access the value corresponding to a frame counter, you have to translate that frame
       *  counter to the step counter. This function does that calculation.
       *  \sa getNumberOfSteps */
      virtual unsigned int frameToStep( unsigned int frame ) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      mutable T                                                           m_currentValue;
      mutable bool                                                        m_currentValueValid;
      typename nvutil::ObjectTraits<AnimationDescription<T> >::SharedPtr  m_description;
  };

  template <typename T> DEFINE_STATIC_PROPERTY( Animation<T>, CurrentValue );

  template<typename T>
  inline typename nvutil::ObjectTraits<Animation<T> >::SharedPtr Animation<T>::create()
  {
    return( nvutil::ObjectTraits<Animation<T> >::Handle::create() );
  }

  template<typename T> 
  inline Animation<T>::Animation( void )
  : m_description(NULL)
  , m_currentValueValid( false )
  {
    NVSG_TRACE();
    this->m_objectCode = AnimationObjectCode<T>::ObjectCode;
  }

  template<typename T> 
  inline Animation<T>::~Animation( void )
  {
    NVSG_TRACE();
    removeAsOwnerFrom( this, m_description );
  }

  template<typename T> 
  inline Animation<T>::Animation( const Animation<T> &rhs )
  : AnimationState(rhs)
  , m_description(rhs.m_description)
  , m_currentValue(rhs.m_currentValue)
  , m_currentValueValid(rhs.m_currentValueValid)
  {
    NVSG_TRACE();
    this->m_objectCode = AnimationObjectCode<T>::ObjectCode;
    addAsOwnerTo( this, m_description );
  }

  template<typename T>
  inline const T & Animation<T>::getCurrentValue() const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_description );

    nvutil::AutoLock lock(m_mutableLock); // secure exclusive write access to mutable members!
    if ( !m_currentValueValid )
    {
      nvutil::ReadableObject<AnimationDescription<T> > description(m_description);
      m_currentValue = description->getStepValue( getCurrentStep() );
      m_currentValueValid = true;
    }
    return( m_currentValue );
  }

  template<typename T>
  inline unsigned int Animation<T>::getNumberOfFrames() const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_description );

    unsigned int stepCount = getNumberOfSteps();
    return( getLoopCount() == 0
            ? UINT_MAX
            : ( isSwingingAnimation() ? getLoopCount() * 2 * ( stepCount - 1 ) : getLoopCount() * stepCount ) );
  }

  template<typename T>
  inline unsigned int Animation<T>::getNumberOfSteps() const
  {
    return( nvutil::ReadableObject<AnimationDescription<T> >(m_description)->getNumberOfSteps() );
  }

  template<typename T>
  inline T Animation<T>::getFrameValue( unsigned int frame ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_description );

    if ( frame == getCurrentFrame() )
    {
      return( getCurrentValue() );
    }
    else
    {
      nvutil::ReadableObject<AnimationDescription<T> > description(m_description);
      return( description->getStepValue( frameToStep(frame) ) );
    }
  }

  template<typename T>
  const typename nvutil::ObjectTraits<AnimationDescription<T> >::SharedPtr & Animation<T>::getDescription() const
  {
    NVSG_TRACE();

    return( m_description );
  }

  template<typename T>
  inline void Animation<T>::setDescription( const typename nvutil::ObjectTraits<AnimationDescription<T> >::SharedPtr & description )
  {
    NVSG_TRACE();
    if ( m_description != description )
    {
      removeAsOwnerFrom( this, m_description );
      m_description = description;
      addAsOwnerTo( this, m_description );

      invalidate();
    }
  }

  template<typename T> 
  inline Animation<T> & Animation<T>::operator=(const Animation<T> & rhs)
  {
    NVSG_TRACE();
    AnimationState::operator=(rhs);
    return *this;
  }

  template<typename T>
  inline bool Animation<T>::isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_description );

    bool equi =     ( dynamic_cast<const Animation<T>*>(p) != NULL )
                &&  AnimationState::isEquivalent( p, ignoreNames, deepCompare );
    if ( equi )
    {
      const Animation<T> * rhs = static_cast<const Animation<T>*>(p);
      NVSG_ASSERT( rhs->getDescription() );
      equi = ( m_description == rhs->getDescription() );
      if ( !equi && deepCompare )
      {
        nvutil::ReadableObject<AnimationDescription<T> > description( m_description );
        nvutil::ReadableObject<AnimationDescription<T> > rhsDesc( rhs->getDescription() );
        equi = description->isEquivalent( rhsDesc, ignoreNames, deepCompare );
      }
    }
    return( equi );
  }

  template<typename T>
  inline void Animation<T>::onInvalidate()
  {
    NVSG_TRACE();

    // status changed when we get here
    // -> force re-evaluation with the next query
    //
    m_currentValueValid = false;
    
    // bounding volumes of owning objects are most likely affected
    // if the animation status changes
    //
    notifyChange( this, NVSG_BOUNDING_VOLUMES );
  }

  template<typename T>
  inline unsigned int Animation<T>::frameToStep( unsigned int frame ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_description );

    unsigned int stepCount = nvutil::ReadableObject<AnimationDescription<T> >(m_description)->getNumberOfSteps();
    if ( frame < getNumberOfFrames() )
    {
      //  step is in animation range
      if ( isSwingingAnimation() )
      {
        //  ping-pong animation -> map step into interval 0 ... 2*(stepCount-1)
        frame %= 2 * ( stepCount - 1 );
        if ( stepCount <= frame )
        {
          //  on reverse part of animation -> map step into interval stepCount-2 ... 0
          frame = 2 * ( stepCount - 1 ) - frame;
        }
      }
      else
      {
        //  loop animation -> map step into interval 0 ... stepCount-1
        frame %= stepCount;
      }
      if ( ! isForwardAnimation() )
      {
        //  animation's running backwards -> invert step on stepCount-1
        frame = stepCount - 1 - frame;
      }
    }
    else
    {
      //  step is out of animation range -> use step 0 or stepCount-1
      frame = ( isForwardAnimation() == isSwingingAnimation() ) ? 0 : ( stepCount - 1 );
    }
    return( frame );
  }

  template<typename T>
  inline void Animation<T>::feedHashGenerator( nvutil::HashGenerator & hg ) const
  {
    AnimationState::feedHashGenerator( hg );
    if ( m_description )
    {
      HashKey hk = nvutil::ReadableObject<AnimationDescription<T> >(m_description)->getHashKey();
      hg.update( reinterpret_cast<const unsigned char *>(&hk), sizeof(hk) );
    }
  }
    
  /*! \brief Start or stop all animations in a scene, including camera animations.
   *  \param scene The scene to switch all animations on or off.
   *  \param enable Indicates whether the animations are to be started (true) or stopped (false).
   *  \remarks Searches for all classes based on nvsg::AnimationState, and calls their start or stop function.
   *  \sa stopAllAnimations */
  NVSG_API void setAllAnimations( const SceneSharedPtr & scene, bool enable );

  /*! \brief Start or stop all animations in a subtree.
   *  \param root The node of the subtree to switch all animations on or off.
   *  \param enable Indicates whether the animations are to be started (true) or stopped (false).
   *  \remarks Searches for all classes based on nvsg::AnimationState, and calls their start or stop function.
   *  \sa stopAllAnimations */
  NVSG_API void setAllAnimations( const NodeSharedPtr & root, bool enable );

} //  namespace nvsg

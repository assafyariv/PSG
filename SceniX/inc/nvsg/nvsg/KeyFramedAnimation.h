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

#include "nvsgcommon.h"
#include "nvsg/AnimationDescription.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Abstract base class for key-framed Animations of type \c T.
   *  \par Namespace: nvsg
   *  \remarks A KeyFramedAnimationDescription holds the information for a key-frame based animation. For
   *  each key, it holds the frame corresponding to that key, and a value of type \c T that is to
   *  be interpolated between the frames of successive keys. It provides the interface needed for an
   *  interpolated animation, but does not perform any interpolations. Interpolations must be
   *  performed by a derived class such as LinearInterpolatedAnimationDescription.
   *  \sa Animation, LinearInterpolatedAnimationDescription */
  template<typename T> class KeyFramedAnimationDescription : public AnimationDescription<T>
  {
    public:
      static typename nvutil::ObjectTraits<KeyFramedAnimationDescription<T> >::SharedPtr create();

    public:
      /*! \brief Destructs a KeyFramedAnimationDescription. */
      virtual ~KeyFramedAnimationDescription( void );

      /*! \brief Get the number of steps in this AnimationDescription.
       *  \return The number of steps in this AnimationDescription.
       *  \remarks This is the number of steps to run once from the first to the last defined frame.
       *  \sa getNumberOfFrames */
      virtual unsigned int getNumberOfSteps() const;

      /*! \brief Add a single key frame to the end of the Animation.
       *  \param step The frame index of the key frame.
       *  \param value The value of the key frame.
       *  \note The behavior is undefined if this KeysFramedAnimation already holds at least one key
       *  frame and \a step is less or equal to the step of the current last key frame.
       *  \sa addKeys, reserveKeys */
      void addKey( unsigned int step, const T & value );

      /*! \brief Add a number of key frames to the end of the Animation.
       *  \param numberOfKeys The number of key frames to add as the last key frames.
       *  \param steps A pointer to the frame indices of the key frames.
       *  \param values A pointer to the values of the key frames.
       *  \note The behavior is undefined if the steps in \a steps are not strictly increasing, or
       *  this KeyFramedAnimationDescription already holds at least one key frame and the first step in \a
       *  steps is less or equal to the step of the current last key frame.
       *  \sa addKey, reserveKeys */
      void addKeys( unsigned int numberOfKeys, const unsigned int * steps, const T * values );

      /*! \brief Insert a key frame defining a step with a value.
       *  \param key Index of the key to insert.
       *  \param step Step of the new key frame.
       *  \param value Value of the new key frame.
       *  \remarks If the key frame at index \a key already specifies animation step \a step, the
       *  value of that key frame is replaced by \a value. Otherwise a new key frame at index \a key
       *  with the given \a step and \a value is inserted.
       *  \note The behavior is undefined if \a step is not greater than the step value of the key
       *  frame before \a key, or if \a step is not smaller than the step value of the key frame
       *  after \a key. */
      void insertKey( unsigned int key, unsigned int step, const T & value );

      /*! \brief Get the step index corresponding to the key \a key.
       *  \param key The key index from which to get the corresponding step.
       *  \return The step index corresponding to the key \a key
       *  \note The behavior is undefined if \a key is greater than or equal to the number of
       *  keys.
       *  \sa getNumberOfKeys, getKeyValue */
      unsigned int getKeyStep( unsigned int key ) const;

      /*! \brief Get a pointer to the key frame indices.
       *  \return A constant pointer to the key frame indices.
       *  \sa getKeyStep, getKeyValue, getKeyValues */
      const unsigned int * getKeySteps() const;

      /*! \brief Get the value of type \c T corresponding to the key \a key.
       *  \param key The key index to get the corresponding value of type \c T of.
       *  \return The value of type \c T corresponding to the key \c key.
       *  \note The behavior is undefined if \a key is greater than or equal to the number of
       *  keys.
       *  \sa getKeyStep, getNumberOfKeys */
      const T & getKeyValue( unsigned int key ) const;

      /*! \brief Get a pointer to the animation key frame values.
       *  \return A constant pointer to the animation key frame values.
       *  \sa getKeyValue, getKeySteps */
      const T * getKeyValues() const;

      /*! \brief Get the number of keys in this KeyFramedAnimationDescription.
       *  \return The number of keys
       *  \remarks A key frame is defined by the step corresponding to a key, and a value of type
       *  \c T.
       *  \sa getKeyStep, getKeyValue */
      unsigned int getNumberOfKeys( void ) const;

      /*! \brief Get the value of a KeyFramedAnimationDescription at the specified step.
       *  \param step Index of the step to get the value at.
       *  \return The value at the specified step.
       *  \remarks For a KeyFramedAnimationDescription, the value at the specified step equals the value at the
       *  key frame just before \a step.
       *  \note The behavior is undefined if \a step is larger than or equal to the current number
       *  of steps.
       *  \sa getNumberOfStep, getStepValues */
      virtual T getStepValue( unsigned int step ) const;

      /*! \brief Replace step and value of a specified key frame.
       *  \param key The key frame to replace step and value.
       *  \param step The step index to use for key frame \a key.
       *  \param value The value to use for key frame \a key.
       *  \note The behavior is undefined if \key is larger or equal to the current number of keys, of if
       *  \a step is less or equal to the step of the key frame \a key - 1, if that exists, or if \a step
       *  is greater or equal to the step of the key frame \a key + 1, if that exists.
       *  \sa replaceKeys */
      void replaceKey( unsigned int key, unsigned int step, const T & value );

      /*! \brief Replace steps and values of a number of consecutive key frames.
       *  \param key The first key frame to change step and value of.
       *  \param numberOfKeys The number of key frames to change.
       *  \param steps A constant pointer to the step indices to use.
       *  \param values A constant pointer to the values to use.
       *  \note The behavior is undefined if \a key + \a numberOfKeys is larger than the current number of
       *  keys, of if the steps in \a steps are not strictly increasing, or the first step in \a steps is
       *  less or equal to the step of key frame \a key - 1, if that exists, or the last step in \a steps
       *  is greater or equal to the step of key frame \a key + \a numberOfKeys + 1, if that exists.
       *  \sa replaceKey */
      void replaceKeys( unsigned int key, unsigned int numberOfKeys, const unsigned int * steps, const T * values );

      /*! \brief Remove a single key frame from the Animation.
       *  \param key The key frame to remove from the Animation.
       *  \note The behavior is undefined if \a key is larger than or equal to the current number of keys.
       *  \sa removeKeys, getNumberOfKeys */
      void removeKey( unsigned int key );

      /*! \brief Remove a number of consecutive keys frames.
       *  \param key The first key frame to remove.
       *  \param numberOfKeys The number of key frames to remove.
       *  \note The behavior is undefined if \a key + \a numberOfKeys is larger than the current number of
       *  steps.
       *  \sa removeKey, getNumberOfKeys */
      void removeKeys( unsigned int key, unsigned int numberOfKeys );

      /*! \brief Reserve space for a specified number of key frames.
       *  \param numberOfKeys Reserve space for that number of key frames.
       *  \sa addKey, addKeys */
      void reserveKeys( unsigned int numberOfKeys );

      /*! \brief Replace the value at the specified animation step.
       *  \param step The step index of the animation to replace.
       *  \param value The value to use for the specified step.
       *  \remarks The animation step \a step is replaced by inserting a key frame at the specified
       *  animation step.
       *  \sa insertKey */
      virtual void replaceStep( unsigned int step, const T & value );

      /*! \brief Remove the current frame.
       *  \param step The step index to remove.
       *  \remarks The animation step \a step is removed by inserting two key frames to the left and
       *  right of the animation step \a step and decrementing the step index of each key frame
       *  after the animation step to remove by one. If there is a key frame defined at the
       *  animation step \a step, it is removed.
       *  \sa insertKey, removeKey */
      virtual void removeStep( unsigned int step );

      /*! \brief Test for equivalence with another KeyFramedAnimationDescription of type \c T.
       *  \param p A reference to the constant KeyFramedAnimationDescription of type \c T to test for
       *  equivalence with
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false
       *  \return \c true if the KeyFramedAnimationDescription \a p of type \c T is equivalent to \c this,
       *  otherwise \c false.
       *  \remarks Two KeyFramedAnimationDescription objects of type \c T are equivalent if they are
       *  equivalent as Animation of type \c T, and all of their keys and values are pair-wise equal.
       *  \note The behavior is undefined if \a p is not an KeyFramedAnimationDescription of type \c T nor
       *  derived from one.
       *  \sa Animation */
      virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

    protected:
      /*! \brief Protected constructor to prevent explicit creation.
       *  \remarks The current frame is set to be invalid, and there are no keys or values.
       *  \note Because KeyFramedAnimationDescription is an abstract class, its constructor is never
       *  used explicitly, but in the constructor of derived classes. */
      KeyFramedAnimationDescription( void );

      /*! \brief Protected copy constructor to prevent explicit creation.
       *  \remarks Keys and values, as well as the current frame are copied from \a rhs.
       *  \note Because KeyFramedAnimationDescription is an abstract class, its constructor is never
       *  used explicitly, but in the constructor of derived classes. */
      KeyFramedAnimationDescription( const KeyFramedAnimationDescription &rhs );

      /*! \brief Get the key above or equal to frame \a frame.
       *  \param frame The frame index for which to get the corresponding (or above) key index
       *  \return The key index corresponding to the frame \a frame, or the key index above.
       *  \remarks 
       *  \note The behavior is undefined if \a frame is greater than or equal to the number of
       *  frames.
       *  \sa  */
      unsigned int getKey( unsigned int frame ) const;

      /*! \brief Protected assignment operator 
       *  \param rhs  Reference of the constant KeyFramedAnimationDescription of type \c T to copy from
       *  \return A reference to the assigned KeyFramedAnimationDescription of type \c T
       *  \remarks The assignment operator calls the assignment operator of Animation of type \c T
       *  and copies the keys and values, as well as the current frame information.
       *  \sa Animation */
      KeyFramedAnimationDescription<T> & operator=(const KeyFramedAnimationDescription<T> & rhs);

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      std::vector<unsigned int> m_keys;
      std::vector<T>            m_values;
  };

  template<typename T>
  inline typename nvutil::ObjectTraits<KeyFramedAnimationDescription<T> >::SharedPtr KeyFramedAnimationDescription<T>::create()
  {
    return( nvutil::ObjectTraits<KeyFramedAnimationDescription<T> >::Handle::create() );
  }

  template<typename T> 
  inline KeyFramedAnimationDescription<T>::KeyFramedAnimationDescription( void )
  : AnimationDescription<T>()
  {
    NVSG_TRACE();
  }

  template<typename T> 
  inline KeyFramedAnimationDescription<T>::KeyFramedAnimationDescription( const KeyFramedAnimationDescription &rhs )
  : AnimationDescription<T>(rhs)
  , m_keys( rhs.m_keys )
  , m_values( rhs.m_values )
  {
    NVSG_TRACE();
  }

  template<typename T> 
  inline KeyFramedAnimationDescription<T>::~KeyFramedAnimationDescription(void)
  {
    NVSG_TRACE();
  }

  template<typename T>
  unsigned int KeyFramedAnimationDescription<T>::getNumberOfSteps() const
  {
    NVSG_TRACE();
    return( m_keys.empty() ? 0 : m_keys.back() + 1 );
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::addKey( unsigned int step, const T & value )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( m_keys.empty() || ( m_keys.back() < step ) );
    m_keys.push_back( step );
    m_values.push_back( value );
    this->invalidate();
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::addKeys( unsigned int numberOfKeys, const unsigned int * steps, const T * values )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( numberOfKeys && steps && values );
    NVSG_ASSERT( m_keys.empty() || ( m_keys.back() < steps[0] ) );
#if !defined(NDEBUG)
    for ( unsigned int i=1 ; i<numberOfKeys ; i++ )
    {
      NVSG_ASSERT( steps[i-1] < steps[i] );
    }
#endif
    m_keys.insert( m_keys.end(), steps, steps + numberOfKeys );
    m_values.insert( m_values.end(), values, values + numberOfKeys );
    this->invalidate();
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::insertKey( unsigned int key, unsigned int step, const T & value )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( getKeyStep( key ) == step )
    {
      replaceKey( key, step, value );   // invalidates incarnation
    }
    else
    {
      NVSG_ASSERT( ( key == 0 ) || ( m_keys[key-1] < step ) );
      NVSG_ASSERT( ( key == m_keys.size() ) || ( step < m_keys[key] ) );
      m_keys.insert( m_keys.begin() + key, step );
      m_values.insert( m_values.begin() + key, value );
      this->invalidate();
    }
  }

  template<typename T>
  inline unsigned int KeyFramedAnimationDescription<T>::getKeyStep( unsigned int key ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( key < m_keys.size() );
    return( m_keys[key] );
  }

  template<typename T>
  inline const unsigned int * KeyFramedAnimationDescription<T>::getKeySteps() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_keys.empty() ? NULL : &m_keys[0] );
  }

  template<typename T>
  inline const T & KeyFramedAnimationDescription<T>::getKeyValue( unsigned int key ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( key < m_values.size() );
    return( m_values[key] );
  }

  template<typename T>
  inline const T * KeyFramedAnimationDescription<T>::getKeyValues() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_values.empty() ? NULL : &m_values[0] );
  }

  template<typename T> 
  inline unsigned int KeyFramedAnimationDescription<T>::getNumberOfKeys( void ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_keys.size()) );
  }

  template<typename T>
  inline T KeyFramedAnimationDescription<T>::getStepValue( unsigned int step ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( step < this->getNumberOfSteps() );
    return( getKeyValue( getKey( step ) ) );
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::replaceKey( unsigned int key, unsigned int step, const T & value )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( key < m_keys.size() );
    NVSG_ASSERT( ( key == 0 ) || ( m_keys[key-1] < step ) );
    NVSG_ASSERT( ( key == m_keys.size()-1 ) || ( step < m_keys[key+1] ) );
    m_keys[key] = step;
    m_values[key] = value;
    this->invalidate();
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::replaceKeys( unsigned int key, unsigned int numberOfKeys
                                                , const unsigned int * steps, const T * values )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( numberOfKeys && steps && values );
    NVSG_ASSERT( key + numberOfKeys <= m_keys.size() );
    NVSG_ASSERT( ( key == 0 ) || ( m_keys[key-1] < steps[0] ) );
#if !defined(NDEBUG)
    for ( unsigned int i=1 ; i<numberOfKeys ; i++ )
    {
      NVSG_ASSERT( steps[i-1] < steps[i] );
    }
#endif
    NVSG_ASSERT(    ( key + numberOfKeys == m_keys.size() )
                ||  ( steps[numberOfKeys-1] < m_keys[key+numberOfKeys] ) );
    for ( unsigned int i=0, j=key ; i<numberOfKeys ; i++, j++ )
    {
      m_keys[j] = steps[i];
      m_values[j] = values[i];
    }
    this->invalidate();
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::removeKey( unsigned int key )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( key < m_keys.size() );
    m_keys.erase( m_keys.begin() + key );
    m_values.erase( m_values.begin() + key );
    this->invalidate();
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::removeKeys( unsigned int key, unsigned int numberOfKeys )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( key + numberOfKeys <= m_keys.size() );
    m_keys.erase( m_keys.begin() + key, m_keys.begin() + key + numberOfKeys );
    m_values.erase( m_values.begin() + key, m_values.begin() + key + numberOfKeys );
    this->invalidate();
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::reserveKeys( unsigned int numberOfKeys )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    m_keys.reserve( numberOfKeys );
    m_values.reserve( numberOfKeys );
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::replaceStep( unsigned int step, const T & value )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( step < this->getNumberOfSteps() );
    insertKey( getKey( step ), step, value );   // invalidates incarnation
  }

  template<typename T>
  inline void KeyFramedAnimationDescription<T>::removeStep( unsigned int step )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( step < this->getNumberOfSteps() );
    unsigned int key = getKey(step);

    // if the key just before 'step' is more than one step away -> introduce a new key at 'step'-1
    if ( key && ( m_keys[key-1] < step-1 ) )
    {
      NVSG_ASSERT( step );
      insertKey( key, step-1, getStepValue(step-1) );
      key++;  // now 'key' again is the key index at or to the right of 'step'
    }

    // if the key just after 'step' is more than one step away -> introduce a new key at 'step'+1
    if ( step < this->getNumberOfSteps()-1 )
    {
      unsigned int nextStepKey = getKey(step+1);
      if ( step+1 < m_keys[nextStepKey] )
      {
        insertKey( nextStepKey, step+1, getStepValue(step+1) );
      }
    }

    // if 'step' is at a key position -> remove it
    NVSG_ASSERT( getKey(step) == key );
    if ( getKeyStep(key) == step )
    {
      removeKey( key );
    }

    // now reduce each key by one step -> 'step' is removed and the animation is one step shorter
    // incarnation is already invalidated above!
    for ( unsigned int i=key ; i<m_keys.size() ; i++ )
    {
      m_keys[i]--;
    }
  }

  template<typename T> 
  inline unsigned int KeyFramedAnimationDescription<T>::getKey( unsigned int frame ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( frame < this->getNumberOfSteps() );
    std::vector<unsigned int>::const_iterator it = std::lower_bound( m_keys.begin(), m_keys.end(), frame );
    NVSG_ASSERT( it != m_keys.end() );
    return( checked_cast<unsigned int>(it - m_keys.begin()) );
  }

  template<typename T> 
  KeyFramedAnimationDescription<T> & KeyFramedAnimationDescription<T>::operator=(const KeyFramedAnimationDescription<T> & rhs)
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if (&rhs != this)
    {
      Animation<T>::operator=(rhs);   // invalidates incarnation
      m_keys    = rhs.m_keys;
      m_values  = rhs.m_values;
    }
    return *this;
  }

  template<typename T> 
  inline bool KeyFramedAnimationDescription<T>::isEquivalent( const Object * p, bool ignoreNames, bool deepCompare) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    bool equi =     ( dynamic_cast<const KeyFramedAnimationDescription<T>*>(p) != NULL )
                &&  AnimationDescription<T>::isEquivalent( p, ignoreNames, deepCompare );
    if ( equi )
    {
      NVSG_ASSERT( dynamic_cast<const KeyFramedAnimationDescription<T>*>(p) );
      const KeyFramedAnimationDescription<T> * iat = static_cast<const KeyFramedAnimationDescription<T>*>(p);
      equi = ( m_keys == iat->m_keys ) && ( m_values == iat->m_values );
    }
    return( equi );
  }

  template<typename T>
    inline void KeyFramedAnimationDescription<T>::feedHashGenerator( nvutil::HashGenerator & hg ) const
  {
    AnimationDescription<T>::feedHashGenerator( hg );
    hg.update( reinterpret_cast<const unsigned char *>(&m_keys[0]), checked_cast<unsigned int>(m_keys.size() * sizeof(unsigned int)) );
    hg.update( reinterpret_cast<const unsigned char *>(&m_values[0]), checked_cast<unsigned int>(m_values.size() * sizeof(T)) );
  }
}

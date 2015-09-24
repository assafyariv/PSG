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
#include "nvmath/Trafo.h"
#include "nvsg/AnimationDescription.h"
#include "nvsg/VertexAttribute.h"
#include "nvutil/HashGenerator.h"

// Important note: extra qualifier required for name lookup!
// from the ISO c++ spec (14.6.2, section 3):
// "In the definition of a class template or in the definition of a member of such a 
// template that appears outside of the template definition, if a base class of this
// template depends on a template-parameter, the base class scope is not examined 
// during name lookup until the class template is instantiated."

namespace nvsg
{
  /*! \brief Template class of an Animation that holds a complete data set for each frame.
   *  \par Namespace: nvsg
   *  \remarks A FramedAnimationDescription holds a data set of type \c T, where \c T is the template
   *  parameter, for each frame.
   *  \note The FramedAnimationDescription of type \c T performs no reference counting on the frame values of
   *  type \c T.
   *  \sa Animation, InterpolatedAnimation */
  template<typename T> class FramedAnimationDescription : public AnimationDescription<T>
  {
    public:
      static typename nvutil::ObjectTraits<FramedAnimationDescription<T> >::SharedPtr create();

    public:
      /*! \brief Default-constructs a FramedAnimationDescription. */
      FramedAnimationDescription();

      /*! \brief Constructs a FramedAnimationDescription as a copy of another FramedAnimationDescription. */
      FramedAnimationDescription( const FramedAnimationDescription<T> &rhs );

      /*! Destructs a FramedAnimationDescription */
      virtual ~FramedAnimationDescription();

      /*! \brief Get the number of steps in this FramedAnimationDescription.
       *  \return The number of steps in this FramedAnimationDescription.
       *  \remarks This is the number of steps to run once from the first to the last defined frame.
       *  \sa getNumberOfFrames */
      virtual unsigned int getNumberOfSteps() const;

      /*! \brief Add a single step to the end of the Animation.
       *  \param value The value to add as the last step.
       *  \sa addSteps, reserveSteps */
      void addStep( const T & value );

      /*! \brief Add a number of steps to the end of the Animation.
       *  \param numberOfSteps The number of steps to add as the last steps.
       *  \param values A pointer to the values to add as the last steps.
       *  \sa addStep, reserveSteps */
      void addSteps( unsigned int numberOfSteps, const T * values );

      /*! \brief Get the value of a FramedAnimationDescription at the specified step.
       *  \param step Index of the step to get the value at.
       *  \return The value at the specified step.
       *  \note The behavior is undefined if \a step is larger than or equal to the current number
       *  of steps.
       *  \sa getNumberOfSteps, getStepValues */
      virtual T getStepValue( unsigned int step ) const;

      /*! \brief Get a pointer to the animation values.
       *  \return A constant pointer to the animation values.
       *  \sa getStepValue */
      const T * getStepValues() const;

      /*! \brief Replace the value of a specified step.
       *  \param step The step to change the value at.
       *  \param value The value to change to.
       *  \note The behavior is undefined if \a step is larger than or equal to the current number
       *  of steps.
       *  \sa replaceSteps */
      virtual void replaceStep( unsigned int step, const T & value );

      /*! \brief Replace the values of a number of consecutive steps.
       *  \param step The first step to change the value of.
       *  \param numberOfSteps The number of steps to change.
       *  \param values A constant pointer to the values to use.
       *  \note The behavior is undefined if \a step + \a numberOfSteps is larger than or equal to
       *  the current number of steps.
       *  \sa replaceStep */
      void replaceSteps( unsigned int step, unsigned int numberOfSteps, const T * values );

      /*! \brief Remove a single step from the Animation.
       *  \param step The step to remove from the Animation.
       *  \note The behavior is undefined if \a step is larger than or equal to the current number
       *  of steps.
       *  \sa removeSteps, getNumberOfSteps */
      virtual void removeStep( unsigned int step );

      /*! \brief Remove a number of consecutive steps.
       *  \param step The first step to remove.
       *  \param numberOfSteps The number of steps to remove.
       *  \note The behavior is undefined if \a step + \a numberOfSteps is larger than or equal to
       *  the current number of steps.
       *  \sa removeStep, getNumberOfSteps */
      void removeSteps( unsigned int step, unsigned int numberOfSteps );

      /*! \brief Reserve space for a specified number of steps.
       *  \param numberOfSteps Reserve space for that number of steps.
       *  \sa addStep, addSteps */
      void reserveSteps( unsigned int numberOfSteps );

      /*! \brief Test for equivalence with an other FramedAnimationDescription of type \c T.
       *  \param p            A reference to the constant FramedAnimationDescription of type \c T to test for
       *  equivalence with.
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare  Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the FramedAnimationDescription of type \c T \a p is equivalent to \c this,
       *  otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Animation of type \c T, they are
       *  equivalent if they have the same number of frames, and each frame has the same value of
       *  type \c T.
       *  \note The behavior is undefined if \a p is not a FramedAnimationDescription of type \c T nor derived
       *  from one.
       *  \sa Animation */
      virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant FramedAnimationDescription of type \c T to copy from.
       *  \return A reference to the assigned FramedAnimationDescription of type \c T.
       *  \remarks The assignment operator calls the assignment operator of Animation of type \c T
       *  and then copies the vector of values of type \c T .
       *  \sa Animation */
      FramedAnimationDescription<T> & operator=( const FramedAnimationDescription<T> & rhs );
    protected:
      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      std::vector<T> m_values;
  };

  template<typename T>
  inline typename nvutil::ObjectTraits<FramedAnimationDescription<T> >::SharedPtr FramedAnimationDescription<T>::create()
  {
    return( nvutil::ObjectTraits<FramedAnimationDescription<T> >::Handle::create() );
  }

  template<typename T>
    inline FramedAnimationDescription<T>::FramedAnimationDescription( void )
  {
    NVSG_TRACE();
    NVSG_ASSERT( false );
  }

  template<typename T>
    inline FramedAnimationDescription<T>::~FramedAnimationDescription( void )
  {
    NVSG_TRACE();
  }

  template<>
    inline FramedAnimationDescription<unsigned int>::FramedAnimationDescription( void )
  { // explicit constructor
    NVSG_TRACE();
    //  See Important Note above !!
    this->m_objectCode = OC_FRAMEDINDEXANIMATIONDESCRIPTION;
  }

  template<>
    inline FramedAnimationDescription<nvmath::Trafo>::FramedAnimationDescription( void )
  { // explicit constructor
    NVSG_TRACE();
    //  See Important Note above !!
    this->m_objectCode = OC_FRAMEDTRAFOANIMATIONDESCRIPTION;
  }

  template<>
    inline FramedAnimationDescription<VertexAttribute>::FramedAnimationDescription( void )
  { // explicit constructor
    NVSG_TRACE();
    //  See Important Note above !!
    this->m_objectCode = OC_FRAMEDVERTEXATTRIBUTEANIMATIONDESCRIPTION;
  }

  template<typename T>
    inline FramedAnimationDescription<T>::FramedAnimationDescription( const FramedAnimationDescription<T> &rhs )
    : AnimationDescription<T>(rhs)
    , m_values(rhs.m_values)
  {
    NVSG_TRACE();
    //  See Important Note above !!
    this->m_objectCode=rhs.m_objectCode;
  }

  template<>
    inline FramedAnimationDescription<unsigned int>::FramedAnimationDescription( const FramedAnimationDescription<unsigned int> &rhs )
    : AnimationDescription<unsigned int>(rhs)
    , m_values(rhs.m_values)
  {
    NVSG_TRACE();
    //  See Important Note above !!
    this->m_objectCode = OC_FRAMEDINDEXANIMATIONDESCRIPTION;
  }

  template<>
    inline FramedAnimationDescription<nvmath::Trafo>::FramedAnimationDescription( const FramedAnimationDescription<nvmath::Trafo> &rhs )
    : AnimationDescription<nvmath::Trafo>(rhs)
    , m_values(rhs.m_values)
  {
    NVSG_TRACE();
    //  See Important Note above !!
    this->m_objectCode = OC_FRAMEDTRAFOANIMATIONDESCRIPTION;
  }

  template<>
    inline FramedAnimationDescription<VertexAttribute>::FramedAnimationDescription( const FramedAnimationDescription<VertexAttribute> &rhs )
    : AnimationDescription<VertexAttribute>(rhs)
    , m_values(rhs.m_values)
  {
    NVSG_TRACE();
    //  See Important Note above !!
    this->m_objectCode = OC_FRAMEDVERTEXATTRIBUTEANIMATIONDESCRIPTION;
  }

  template<typename T>
  inline unsigned int FramedAnimationDescription<T>::getNumberOfSteps() const
  {
    NVSG_TRACE();
    NVSG_ASSERT( m_values.size() <= UINT_MAX );
    return( checked_cast<unsigned int>(m_values.size()) );
  }

  template<typename T>
  inline void FramedAnimationDescription<T>::addStep( const T & value )
  {
    NVSG_TRACE();
    m_values.push_back( value );
    NVSG_ASSERT( m_values.size() <= UINT_MAX );
    this->invalidate();
  }

  template<typename T>
  inline void FramedAnimationDescription<T>::addSteps( unsigned int numberOfSteps, const T * values )
  {
    NVSG_TRACE();
    m_values.insert( m_values.end(), values, values + numberOfSteps );
    NVSG_ASSERT( m_values.size() <= UINT_MAX );
    this->invalidate();
  }

  template<typename T>
  inline T FramedAnimationDescription<T>::getStepValue( unsigned int step ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT( step < m_values.size() );
    return( m_values[step] );
  }

  template<typename T>
  inline const T * FramedAnimationDescription<T>::getStepValues() const
  {
    NVSG_TRACE();
    return( m_values.empty() ? NULL : &m_values[0] );
  }

  template<typename T>
  inline void FramedAnimationDescription<T>::replaceStep( unsigned int step, const T & value )
  {
    NVSG_TRACE();
    NVSG_ASSERT( step < m_values.size() );
    m_values[step] = value;
    this->invalidate();
  }

  template<typename T>
  inline void FramedAnimationDescription<T>::replaceSteps( unsigned int step, unsigned int numberOfSteps, const T * values )
  {
    NVSG_TRACE();
    NVSG_ASSERT( step + numberOfSteps < m_values.size() );
    for ( unsigned int i=0, j=step ; i<numberOfSteps ; i++, j++ )
    {
      m_values[j] = values[i];
    }
   this->invalidate();
  }

  template<typename T>
  inline void FramedAnimationDescription<T>::removeStep( unsigned int step )
  {
    NVSG_TRACE();
    NVSG_ASSERT( step < m_values.size() );
    m_values.erase( m_values.begin() + step );
    this->invalidate();
  }

  template<typename T>
  inline void FramedAnimationDescription<T>::removeSteps( unsigned int step, unsigned int numberOfSteps )
  {
    NVSG_TRACE();
    NVSG_ASSERT( step + numberOfSteps < m_values.size() );
    m_values.erase( m_values.begin() + step, m_values.begin() + step + numberOfSteps );
    this->invalidate();
  }

  template<typename T>
  inline void FramedAnimationDescription<T>::reserveSteps( unsigned int numberOfSteps )
  {
    NVSG_TRACE();
    m_values.reserve( numberOfSteps );
  }

  template<typename T>
  inline bool FramedAnimationDescription<T>::isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const
  {
    NVSG_TRACE();
    bool equi =     ( dynamic_cast<const FramedAnimationDescription<T>*>(p) != NULL )
                &&  AnimationDescription<T>::isEquivalent( p, ignoreNames, deepCompare );
    if ( equi )
    {
      NVSG_ASSERT( dynamic_cast<const FramedAnimationDescription<T> *>(p) );
      const FramedAnimationDescription<T> * fat = static_cast<const FramedAnimationDescription<T> *>(p);
      equi = ( m_values == fat->m_values );
    }
    return( equi );
  }

  template<typename T>
    inline FramedAnimationDescription<T> & FramedAnimationDescription<T>::operator=(const FramedAnimationDescription<T> & rhs)
  {
    NVSG_TRACE();
    if (&rhs != this)
    {
      AnimationDescription<T>::operator=(rhs);  // invalidates incarnation
      m_values = rhs.m_values;
    }
    return *this;
  }

  template<typename T>
    inline void FramedAnimationDescription<T>::feedHashGenerator( nvutil::HashGenerator & hg ) const
  {
    AnimationDescription<T>::feedHashGenerator( hg );
    hg.update( reinterpret_cast<const unsigned char *>(&m_values[0]), checked_cast<unsigned int>(m_values.size() * sizeof(T)) );
  }
}

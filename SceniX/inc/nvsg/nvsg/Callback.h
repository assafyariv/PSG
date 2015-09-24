// Copyright NVIDIA Corporation 2002-2008
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

#include "nvsg/CoreTypes.h"
#include "nvutil/RCObject.h"

namespace nvsg
{
  /*! \brief Callback class to link objects with an observer.
   *  \remarks This is a pure virtual class to link an object with an observing object. */
  class Callback : public nvutil::RCObject
  {
    public:
      /*! \brief Constructor of a Callback
       *  \param observer Object to be the observer for this callback.
       *  \remarks The observer can be used in the function operator() that is used as the callback.
          Note that the observer cannot be identical to the object that the callback is attached to,
          nor to one of this object's parents. Otherwise there will be circular references breaking object
          cleanup and the observer will be read locked when entering the operator() function.
       *  \sa Object
       */
      NVSG_API explicit Callback( const ObjectSharedPtr & observer );

      /*! \brief Get the observer of this callback.
       *  \return The Object that is the observer for this callback.
       *  \remarks The observer can be used in the function operator() that is used as the callback.
       *  \sa Object
       */
      NVSG_API const ObjectSharedPtr & getObserver() const;

      /*! \brief Interface for the function call operator used as the callback.
       *  \param subject The pointer to the read-locked object that is to be observed by observer of this callback.
       *  \param data A pointer to const for any callback specific data.
       */
      NVSG_API virtual void operator()( const Object * subject, const void * data = NULL ) const = 0;

      /*! \brief Interface to get the ID of the callback class.
       *  \return The ID of this class.
       *  \remarks Each concrete callback class has to overwrite this function and return it's ID to identify this
       *  callback class. */
      NVSG_API virtual unsigned int getClassID() const = 0;

      /*! \brief Interface to get the name of the callback class.
       *  \return The name of this class.
       *  \remarks Each concrete callback class has to overwrite this function and return it's name to identify this
       *  callback class. */
      NVSG_API virtual const char * getClassName() const = 0;

    protected:
      /*! \brief Destructor of this Callback.
       *  \remarks The destructor is protected to prevent direct deletion. A Callback is reference counted, and to
       *  delete it it's reference count has to be decreased to zero.
       */
      NVSG_API virtual ~Callback();

    private:
      ObjectSharedPtr m_observer;
  };

  inline Callback::Callback( const ObjectSharedPtr & observer )
    : RCObject()
    , m_observer(observer)
  {
  }

  inline Callback::~Callback()
  {
  }

  inline const ObjectSharedPtr & Callback::getObserver() const
  {
    return( m_observer );
  }
}

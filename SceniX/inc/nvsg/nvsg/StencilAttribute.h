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

/*! \brief Enumeration to control the stencil function
 *  \par Namespace: nvsg
 *  \remarks A fragment is drawn depending on the outcome of a stencil function,
 *  which in turn evaluates a \a threshold, a \a mask and the stencil \a value of the fragment.
 *  \sa StencilAttribute::setStencilFunction */
typedef enum
{
  SF_NEVER,             //!< The fragment is never drawn
  SF_ALWAYS,            //!< The fragment is always drawn
  SF_LESS,              //!< The fragment is drawn if (\a treshold & \a mask) <  (\a value & \a mask)
  SF_LESS_OR_EQUAL,     //!< The fragment is drawn if (\a treshold & \a mask) <= (\a value & \a mask)
  SF_EQUAL,             //!< The fragment is drawn if (\a treshold & \a mask) == (\a value & \a mask)
  SF_GREATER_OR_EQUAL,  //!< The fragment is drawn if (\a treshold & \a mask) >= (\a value & \a mask)
  SF_GREATER,           //!< The fragment is drawn if (\a treshold & \a mask) >  (\a value & \a mask)
  SF_NOT_EQUAL          //!< The fragment is drawn if (\a treshold & \a mask) != (\a value & \a mask)
} StencilFunction;

/*! \brief Enumeration to control the stencil operation
 *  \par Namespace: nvsg
 *  \remarks The operation is applied depending on the state of the fragment, see setStencilOperation.
 *  \sa StencilAttribute::setStencilOperation */
typedef enum
{
  SO_KEEP,           //!< Keep the current value
  SO_ZERO,           //!< Set the stencil buffer to 0
  SO_REPLACE,        //!< Set the stencil buffer to \a threshold, see setThreshold
  SO_INCREMENT,      //!< Increments the current stencil buffer value. Clamps to the maximum representable unsigned value.
  SO_DECREMENT,      //!< Decrements the current stencil buffer value. Clamps to 0.
  SO_INVERT,         //!< Bitwise inverts the current stencil buffer value.
  SO_INCREMENT_WRAP, //!< Increments the current stencil buffer value. Wraps stencil buffer value to zero when incrementing the maximum representable unsigned value.
  SO_DECREMENT_WRAP, //!< Decrements the current stencil buffer value. Wraps stencil buffer value to the maximum representable unsigned value when decrementing a stencil buffer value of zero.
} StencilOperation;

/*! \brief Controls the set of stencil state that are affected by the setStencil* functions
 *  \par Namespace: nvsg
 *  \remarks The stencil functionality can be applied to both back faces and front faces. The setStencil* functions all have a parameter
 *  to control which faces are affected.
 *  \sa StencilAttribute, StencilAttribute::setStencilFunction, StencilAttribute::setStencilOperation */
typedef enum
{
  SS_FRONT, //!< Set stencil state for front faces
  SS_BACK,  //!< Set stencil state for back faces
} StencilSide;

/*! \brief Lets you specify a stencil test to apply to a number of Drawable objects.
 * \remarks
 * The StencilAttribute provides an API that lets you specify a particular stencil test 
 * and the corresponding operation on the pixel value in the stencil buffer that should 
 * be applied depending on the stencil test result. */
class StencilAttribute : public StateAttribute
{
    public:
      NVSG_API static StencilAttributeSharedPtr create();

public:
  /*! \brief Defines the stencil comparison test function.
   * \param func
   * Specifies the comparison function. The function can be SF_NEVER, SF_ALWAYS, SF_LESS,
   * SF_LESS_OR_EQUAL, SF_EQUAL, SF_GREATER_OR_EQUAL, SF_GREATER, or SF_NOT_EQUAL.
   * \param refVal
   * Reference value to compare with the pixel value in the stencil buffer.
   * \param valMask
   * Specifies which bits of the reference value and the pixel value in the stencil buffer
   * should be considered for comparison.
   * \remarks
   * The stencil test takes place only if there is a stencil buffer. If no stencil buffer
   * was configured, the stencil test always passes. The test function compares the reference 
   * value with the value stored at a pixel in the stencil buffer. The comparison applies only
   * to those bits where the corresponding bits of the value mask are 1. Depending on the
   * test result, the value in the stencil buffer is modified according to the stencil
   * operations that can be specified through setStencilOperation. The stencil test only 
   * applies if the test explicitly was enabled through setStencilTestingEnabled.
   * \sa setStencilOperation, setStencilTestingEnabled */
  NVSG_API void setStencilFunction(StencilFunction func, int refVal, unsigned int valMask, StencilSide side = SS_FRONT );

  /*! \brief Retrieves the actual test function.
   * \returns 
   * The function returns the comparison function identifier last specified through setStencilFunction.
   * \sa setStencilFunction, getStencilReferenceValue, getStencilValueMask */
  NVSG_API StencilFunction getStencilFunction( StencilSide side = SS_FRONT ) const;

  /*! \brief Retrieves the actual reference value.
   * \returns 
   * The function returns the reference value last specified through setStencilFunction.
   * \sa setStencilFunction, getStencilFunction, getStencilValueMask */
  NVSG_API int getStencilRefenceValue( StencilSide side = SS_FRONT ) const;

  /*! \brief Retrieves the actual value mask.
   * \returns 
   * The function returns the value mask last specified through setStencilFunction.
   * \sa setStencilFunction, getStencilFunction, getStencilReferenceValue */
  NVSG_API unsigned int getStencilValueMask( StencilSide side = SS_FRONT ) const;

  /*! \brief Specifies the stencil operation.
   * \param fail 
   * The operation to apply if the fragment fails the stencil test.
   * \param zfail
   * The operation to apply if the fragment passes the stencil test, and fails the depth test.
   * \param zpass
   * The operation to apply if the fragment passes the stencil test, and passes the depth test.
   * \remarks
   * This function specifies how data in the stencil buffer is modified when a fragment passes
   * or fails the stencil test specified through setStencilFunction. Modifications only apply
   * to those bit planes for which the corresponding bits in the stencil mask are set to 1.
   * Use setStencilWriteMask to define the actual stencil write mask. The three operations, 
   * \a fail, \a zfail, and \a zpass, can be SO_KEEP, SO_ZERO, SO_REPLACE, SO_INCREMENT, SO_INCREMENT_WRAP, 
   * SO_DECREMENT, SO_DECREMENT_WRAP, or SO_INVERT. They correspond to keeping the current value, 
   * replacing it with zero, replacing it with the reference value specified through setStencilFunction, 
   * incrementing it, decrementing it, and bitwise-inverting it. The result of increment and decrement is
   * clamped from 0 to the maximum stencil buffer value unless the wrap version is used.
   * \sa setStencilFunction, setStencilWriteMask */
  NVSG_API void setStencilOperation(StencilOperation fail, StencilOperation zfail, StencilOperation zpass, StencilSide side = SS_FRONT);

  /*! \brief Retrieves the operation to apply if the fragment fails the stencil test.
   * \return
   * The function returns the fail operation last specified through setStencilOperation.
   * \sa getStencilZFailOperation, getStencilZPassOperation, setStencilOperation. */
  NVSG_API StencilOperation getStencilFailOperation( StencilSide side = SS_FRONT ) const;

  /*! \brief Retrieves the operation to apply if the fragment passes the stencil test,
   * and fails the depth test.
   * \return
   * The function returns the zfail operation last specified through setStencilOperation.
   * \sa getStencilFailOperation, getStencilZPassOperation, setStencilOperation. */
  NVSG_API StencilOperation getStencilZFailOperation( StencilSide side = SS_FRONT ) const;

  /*! \brief Retrieves the operation to apply if the fragment passes the stencil test,
   * and passes the depth test.
   * \return
   * The function returns the zpass operation last specified through setStencilOperation.
   * \sa getStencilFailOperation, getStencilZFailOperation, setStencilOperation. */
  NVSG_API StencilOperation getStencilZPassOperation( StencilSide side = SS_FRONT ) const;

    /*! \brief Enables or disables whether separate stencil version should be used. 
   * \param twoSided 
   * Indicates if testing should be enabled or disabled.
   * Pass \c true for using separate stencil states for front and back faces, and \c false to only use the front values for all.
   * \sa isStencilSeparate */
  NVSG_API void setStencilSeparateEnabled(bool separate);

  /*! \brief Retrieve the current state for the two-sided stencil usage.
   * \return 
   * The function returns \c true if the separate variant is used. Otherwise it returns \c false.
   * \sa setStencilSeparate */
  NVSG_API bool isStencilSeparateEnabled() const;

  /*! \brief Enables or disables the stencil test. 
   * \param testing 
   * Indicates if testing should be enabled or disabled.
   * Pass \c true for enabling the stencil test, and \c false to disable the test again.
   * \sa isStencilTestingEnabled, setStencilWriteMask */
  NVSG_API void setStencilTestingEnabled(bool testing);

  /*! \brief Retrieve the current enable state for the stencil test.
   * \return 
   * The function returns \c true if the stencil test is enabled. Otherwise it returns \c false.
   * \sa setStencilTestingEnabled */
  NVSG_API bool isStencilTestingEnabled() const;

  /*! \brief Sets the mask used to control writing into the stencil buffer.
   * \param mask
   * Specifies the write mask to control writing into the stencil buffer.
   * \remarks
   * Bits in the stencil buffer will only be written to if their corresponding bits in the
   * write mask are set to 1. Bits in the stencil buffer will not be written if their corresponding
   * bits in the write mask are set to 0.
   * \sa setStencilTestingEnabled, getStencilWriteMask */
  NVSG_API void setStencilWriteMask(unsigned int mask, StencilSide side = SS_FRONT);

  /*! \brief Retrieves the current mask that controls writing into the stencil buffer.
   * \return 
   * The function returns the write mask last specified through setStencilWriteMask.
   * \sa setStencilWriteMask */
  NVSG_API unsigned int getStencilWriteMask( StencilSide side = SS_FRONT ) const;

  /*! \brief Tests if two StencilAttributes are equivalent.
   * \param object
   * The Object to compare with this StencilAttribute.
   * \param ignoreNames
   * If \c true, Object names are not considered for testing.
   * \param deepCompare
   * This parameter does not apply to StencilAttribute comparison, and will be ignored.
   * \return
   * The function returns \c true if this StencilAttribute can be considered equivalent to 
   * the Object passed through \a object. Otherwise, the function returns \c false.
   * \remarks
   * This function overrides Object's base implementation to provide comparison between
   * two StencilAttributes. */
  NVSG_API bool isEquivalent(const Object * object , bool ignoreNames, bool deepCompare) const;

  REFLECTION_INFO_API( NVSG_API, StencilAttribute );
protected:
  friend struct nvutil::Holder<StencilAttribute>;

  /*! \brief Default-constructs a StencilAttribute. 
   * \remarks 
   * This constructor will be called on instantiation through StencilAttribute::create().
   * Stencil testing is disabled by default, and the stencil write mask is initialized to all 1s.
   * The compare function is initialized to SF_ALWAYS, the reference value is set to 0, and the
   * value mask is set to all 1s by default. All operations, fail, zfail, and zpass, will be initialized
   * to SO_KEEP. */
  NVSG_API StencilAttribute();

  /*! \brief Copy-constructs a StencilAttribute from source StencilAttribute. 
   * \param rhs 
   * StencilAttribute object that is taken as source copy.
   * \remarks
   * This copy-constructor will be called when StencilAttributeHandle::clone gets invoked.
   * It completely copies the state of the right-hand-sided object. */
  NVSG_API StencilAttribute(const StencilAttribute& rhs);

  /*! \brief Feed the data of this object into the provied HashGenerator.
   *  \param hg The HashGenerator to update with the data of this object.
   *  \sa getHashKey */
  NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

private:
  bool m_testing;   // do the test or not?
  bool m_separate;  // use two-sided variant or only front values?

  struct StencilFace
  {
    StencilFace();

    unsigned int m_writeMask; // indicates which bit planes to write to

    // operations to perform depending on test results
    //
    StencilOperation m_fail;  // operation performed if fragment fails the stencil test
    StencilOperation m_zfail; // operation performed if fragment passes the test, and fails the depth test
    StencilOperation m_zpass; // operation performed if fragment passes the test, and passes the depth test

    // stencil function parameters
    //
    StencilFunction m_func; // stencil function
    int m_refVal; // reference value
    unsigned int m_valMask; // apply reference value comparison only to those bits set in this mask
  };

  StencilFace  m_faces[2]; // front and back state
 
};

} // namespace

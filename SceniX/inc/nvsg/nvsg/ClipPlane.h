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
#include "nvsg/OwnedObject.h"
#include "nvmath/Planent.h"
#include "nvmath/Vecnt.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{

/*! \brief Defines a user clipping plane 
 * \remarks 
 * A ClipPlane will be defined through a normal vector, that is - a vector perpendicular to the plane,
 * and the distance, or offset, to the origin along the ClipPlane's normal. The ClipPlane is specified 
 * in object space. All points x in object space that satisfy normal * x + offset >=0 lie in the half-space 
 * defined by the ClipPlane. All points outside this half-space will be clipped away during rendering. 
 * \n\n
 * A ClipPlane in SceniX can be added to a Group using the Group::addClipPlane API. If a ClipPlane is enabled,
 * it will affect all children of the Group it was added to. Also, a ClipPlane will be transformed by the 
 * exact same hierarchy of Transforms that apply to the Group it was added to.
 * \par Namespace: nvsg
 */ 
class ClipPlane : public OwnedObject<Group>
{
public:
  NVSG_API static ClipPlaneSharedPtr create();

public:
  /*! \brief Defines the orientation of the ClipPlane
   * \param normal 
   * Defines the normal vector of the ClipPlane which defines the orientation of the plane in object space.
   * \remarks
   * This function lets you specify the orientation of the ClipPlane in object space.
   * The ClipPlane's normal vector together with the ClipPlane's offset completely defines
   * the ClipPlane. All points x in object space that satisfy normal * x + offset >= 0 lie in the
   * half-space defined by the ClipPlane. All points outside this half-space are clipped away. 
   * \sa setOffset, getNormal */
  NVSG_API void setNormal(const nvmath::Vec3f& normal);

  /*! \brief Retrieves the ClipPlane's normal vector.
   * \return 
   * The function returns the normal vector of the ClipPlane as last specified through setNormal.
   * \sa setNormal */
  NVSG_API const nvmath::Vec3f& getNormal() const;

  /*! \brief Defines the ClipPlane's offset
   * \param offset 
   * Specifies the offset of the ClipPlane in object space.
   * \remarks
   * This function lets you specify the distance of the ClipPlane to the object space origin 
   * along the ClipPlane's normal vector.
   * The ClipPlane's normal vector together with the ClipPlane's offset completely defines
   * the ClipPlane. All points x in object space that satisfy normal * x + offset >= 0 lie in the
   * half-space defined by the ClipPlane. All points outside this half-space are clipped away. 
   * \sa setNormal, getOffset */
  NVSG_API void setOffset(float offset);

  /*! \brief Retrieves the ClipPlane's offset.
   * \return 
   * The function returns the ClipPlane's offset as last specified through setOffset.
   * \sa setOffset */
  NVSG_API float getOffset() const;

  /*! \brief Specifies the enable state of the ClipPlane
   * \param onOff
   * Indicates the new enable state. If you pass \c false, the ClipPlane will be further on ignored
   * during rendering until you re-enable it. Passing \c true enables the ClipPlane for rendering. 
   * \sa isEnabled */
  NVSG_API void setEnabled(bool onOff);

  /*! \brief Retrieves the ClipPlane's current enable state
   * \return
   * \c true if the ClipPlane is enabled for rendering, \c false otherwise.
   * \remarks
   * If a ClipPlane is disabled, that is - if this function returns \c false, it will be
   * ignored during rendering.
   * \sa setEnabled */
  NVSG_API bool isEnabled() const;

  /*! \brief Returns the distance of the specified point to this ClipPlane
   * \param point The point, in object space, for which to calculate the distance to this ClipPlane.
   * \return 
   * The function returns the distance of the given point to the ClipPlane along the ClipPlane's normal. 
   * \remarks 
   * The function expects the point to evaluate to be in object space.
   * A negative return value indicates that the point lies outside the half-space defined by this ClipPlane.
   * If the ClipPlane is enabled, all points outside this half-space will be clipped away during rendering. 
   * \sa isInside */
  NVSG_API float getDistance(const nvmath::Vec3f& point) const;
  
  /*! \brief Evaluates if the indicated point lies inside the half-space defined by the ClipPlane
   * \param point
   * Indicates the point in object space to evaluate.
   * \return
   * The function returns \c true if the indicated point lies inside the half-space defined by the 
   * ClipPlane.
   * \remarks
   * Use this function to test if a given point in object space lies inside the half-space defined
   * by the ClipPlane. If the function returns \c false, the indicated point lies outside
   * this half-space and would be clipped away during rendering if the ClipPlane is enabled. 
   * Alternative to this function, you can also use getDistance and evaluate its return value. 
   * If getDistance returns a negative value, the point lies outside the half-space defined by this
   * ClipPlane. Otherwise, it lies inside the half-space. 
   * \sa getDistance */
  NVSG_API bool isInside(const nvmath::Vec3f& point) const;

  /*! \brief Tests whether the indicated object is equivalent to this ClipPlane.
   * \param object 
   * The object to test against this ClipPlane.
   * \param ignoreNames
   * Specifies if Object names should be considered for the test or not. If \c true, object names will
   * be ignored while testing.
   * \param deepCompare 
   * This parameter does not apply for testing ClipPlanes and can be ignored.
   * \remarks
   * This function overrides Object::isEquivalent. */
  NVSG_API virtual bool isEquivalent(const Object * object , bool ignoreNames, bool deepCompare) const;

  REFLECTION_INFO_API( NVSG_API, ClipPlane );
protected:
  friend struct nvutil::Holder<ClipPlane>;

  /*! \brief Default-constructs a ClipPlane
   * \remarks
   * This constructor will be called on instantiation through ClipPlane::create().
   * All plane coefficients are set to 0 after default construction, and the ClipPlane is
   * enabled. */
  NVSG_API ClipPlane();

  /*! \brief Copy-constructs a ClipPlane from a source ClipPlane. 
   * \param rhs
   * ClipPlane object that is taken as source.
   * \remarks
   * This copy-constructor will be called when ClipPlaneHandle::clone is invoked.
   * It completely copies the state of the right-hand-sided object. */
  NVSG_API ClipPlane(const ClipPlane& rhs);

  /*! \brief Feed the data of this object into the provied HashGenerator.
   *  \param hg The HashGenerator to update with the data of this object.
   *  \sa getHashKey */
  NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

private:
  nvmath::Plane3f m_plane; // internal representation 
  bool m_enabled;
};

inline const nvmath::Vec3f& ClipPlane::getNormal() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  return m_plane.getNormal();
}

inline float ClipPlane::getOffset() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  return m_plane.getOffset();
}

inline bool ClipPlane::isEnabled() const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  return m_enabled;
}

inline float ClipPlane::getDistance(const nvmath::Vec3f& point) const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  return m_plane(point);
}

inline bool ClipPlane::isInside(const nvmath::Vec3f& point) const
{
  NVSG_TRACE();
  NVSG_ASSERT_OBJECT_READ_LOCKED(this);
  return m_plane(point) >= 0.0f;
}

} // namespace nvsg

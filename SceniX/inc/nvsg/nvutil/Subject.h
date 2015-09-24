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

#include <nvsgcommon.h>
#include <nvutil/Handle.h>
#include <nvutil/STLAllocator.h>
#include <map>

namespace nvutil
{
  class HandledObject;

  CORE_TYPES_BASE( HandledObject );
}

namespace nvsg
{
  class Object;
  class Group;
  class Node;
  class ClipPlane;
  class LightSource;

  CORE_TYPES( Object, nvutil::HandledObject );
  CORE_TYPES( Node, Object );
  CORE_TYPES( ClipPlane, Object );
  CORE_TYPES( LightSource, Object );
};

namespace nvutil
{
  class Property;
  typedef Property * PropertyId;

  /** \brief A Subject is observable by some Oberver objects. */
  //  A Subject is _not_ derived from Observer, as a Subject need not be handled, but an Observer has to.
  class Subject
  {
    public:
      Subject();
      ~Subject();

      struct Payload : public nvutil::RCObject
      {
        Payload()
          : m_observerId( ~0 )
        {
        }

      private:
        friend class nvutil::Subject;
        size_t m_observerId;
      };

      typedef nvutil::SmartPtr<Payload> SmartPayload;

    public:
      /** \brief Attach an observer to this subject.
        * \param oh The Observer to attach.
        * \param payload A refcounted pointer to payload which will be passed on notifications.
        * \note The reference count of the attached observer is not increased, but it needs a valid one to be attached.
        * \sa detach */
      NVSG_API void attach( const ObserverWeakPtr & oh, const SmartPayload& payload ) const;

      /** \brief Test if an observer is already attached to this subject
        * \param oh The Observer to test
        * \return true if the given \a oh already has been attached to this Subject and false otherwise.
        */
      NVSG_API bool isAttached( const ObserverWeakPtr & oh, const SmartPayload& payload  ) const;

      /** \brief Detach an observer from this subject
        * \param oh The Observer to detach.
        * \param payload A refcounted pointer to payload which will be passed on notifications.
        * \note The reference count of the detached observer is not decreased, but it needs a valid one to be detached.
        * \sa attach */
      NVSG_API void detach( const ObserverWeakPtr & oh, const SmartPayload &payload ) const;

      /** \brief Notify this subject, that a state has changed.
        * \param state A bitfield indicating which state has changed.
        * \param originator Pointer to the object that this event came from.
        * \note Every Observer of this Subject is write-locked, and its update(state) function is called. */
      NVSG_API virtual void notify( const Subject * originator, unsigned int state ) const;

      /** \brief Notify this subject, that the specified property has changed.
        * \param propertyId The ID of the property that changed.
        * \note Every Observer of this Subject is write-locked, and its update(propertyId) function is called. */
      NVSG_API virtual void notify( PropertyId propertyId ) const;

      /** \brief Notify this subject, that the specified child had been added to the given group.
          \param group The group the child was added to.
          \param child The child which was added to the group.
          \param index The index the child was added at.
      **/
      NVSG_API void notifyPostAddChild( const nvsg::Group *group, const nvsg::NodeSharedPtr &child, unsigned int index );

      /** \brief Notify this subject, that the specified child will be removed from the given group.
          \param group The group the child will be removed from.
          \param child The child to be removed from the group.
          \param index The index the child will be removed from.
      **/
      NVSG_API void notifyPreRemoveChild( const nvsg::Group *group, const nvsg::NodeSharedPtr &child, unsigned int index );

      /** \brief Notify this subject, that the content of the specified group had been exchanged.
          \param group The group whose content had been exchanged.
        */
      NVSG_API void notifyPostGroupExchanged( const nvsg::Group *group );

      /** \brief Notify this subject, that the specified lightsource had been added to the given group
          \param group The group the lightsource was added to.
          \param lightSource The lightsource which was added to the group.
      **/
      NVSG_API void notifyPostAddLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr& lightSource );

      /** \brief Notify this subject, that the specified lightsource will be removed from the given group.
          \param group The group the lightsource will be removed from.
          \param lightSource The lightsource to be removed from the group.
      **/
      NVSG_API void notifyPreRemoveLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr &lightSource );

      /** \brief Notify this subject, that the specified clipPlane had been added to the given group
          \param group The group the clipplane was added to.
          \param clipPlane The clipplane which was added to the group.
      **/
      NVSG_API void notifyPostAddClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane );

      /** \brief Notify this subject, that the specified clipPlane will be removed from the given group.
          \param group The group the clipplane will be removed from.
          \param clipPlane The clipplane to be removed from the group.
      **/
      NVSG_API void notifyPreRemoveClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane );

      Subject& operator=( const Subject &rhs );

    private:
      typedef nvutil::STLVector<std::pair<ObserverWeakPtr,SmartPayload> > ObserversContainer;

      struct Attachments : public IAllocator
      {
        unsigned int        dirtyCount;
        ObserversContainer  observers;
      };

    private:
      void updateObservers() const; // is being called by notify functions only

    protected:
      SWMRSync              m_mutableLock;

    private:
      mutable Attachments * m_attachments;
  };

  inline Subject& Subject::operator=( const Subject &rhs )
  {
    NVSG_TRACE();
    return( *this );
  }
} // namespace nvutil

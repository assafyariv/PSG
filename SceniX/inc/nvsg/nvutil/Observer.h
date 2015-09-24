// Copyright NVIDIA Corporation 2010
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

#include "nvsgcommon.h"
#include "nvsg/CoreTypes.h"
#include <nvutil/HandledObject.h>
#include <nvutil/Subject.h>
#include <set>

namespace nvutil
{
  class Subject;

  /** \brief Class used to observe other subjects.
    * \sa Subject */
  class Observer : public HandledObject
  {
    public:
      NVSG_API Observer();
      NVSG_API virtual ~Observer();

      /** \brief Marks the indicated state as not dirty
        * \param state The state, as a bitfield, that has to be cleared
        * \return \c false if the indicated state was not marked as dirty, \c true otherwise. */
      NVSG_API bool clear(unsigned int state);

      /** \brief Called from observed subject right before destruction.
        * \param subject The subject which is going to be destructed.
        * \param payload Payload object passed to the Subject::attach method. */
      NVSG_API void destroyed( const Subject *subject, const nvutil::Subject::SmartPayload &payload );

      /** \brief Called from observed subject upon a property change.
        * \param subject The subject that has changed.
        * \param state The state, as a bitfield, that changed.
        * \param payload Payload object passed to the Subject::attach method. */
      NVSG_API void update( const Subject *subject, unsigned int state, const nvutil::Subject::SmartPayload &payload );

      /** \brief Called from observed subject upon a property change
        * \param subject The subject that has changed.
        * \param propertyId Id of the property which has changed.
        * \param payload Payload object passed to the Subject::attach method. */
      NVSG_API void update( const Subject *subject, PropertyId propertyId, const Subject::SmartPayload &payload );

      /** \brief Called from observed group after a child has been added.
          \param group The group the child was added to.
          \param child The child which was added to the group.
          \param index The index the child was added at.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API void postAddChild( const nvsg::Group *group, const nvsg::NodeSharedPtr& child, unsigned int index, const nvutil::Subject::SmartPayload &payload );

      /** \brief Called from observed group before a child will be removed from a Group.
          \param group The group the child will be removed from.
          \param child The child to be removed from the group.
          \param index The index the child will be removed from.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API void preRemoveChild( const nvsg::Group *group, const nvsg::NodeSharedPtr& child, unsigned int index, const nvutil::Subject::SmartPayload &payload );

      /** \brief Called from the observed group after the content of the specified group had been exchanged.
          \param group The group whose content had been exchanged.
          \param payload Payload object passed to the Subject::attach method.
        */
      NVSG_API void postGroupExchanged( const nvsg::Group *group, const Subject::SmartPayload& payload );

      /** \brief Called from the observed group after a lightsource has been added.
          \param group The group the lightsource was added to.
          \param lightSource The lightsource which was added to the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API void postAddLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr& lightSource, const nvutil::Subject::SmartPayload& payload );

      /** \brief Called from observed group before a lightsource will be removed from a Group.
          \param group The group the lightsource will be removed from.
          \param lightSource The lightsource to be removed from the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API void preRemoveLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr &lightSource, const nvutil::Subject::SmartPayload& payload );

      /** \brief Called from the observed group after a clipplane has been added.
          \param group The group the clipplane was added to.
          \param clipPlane The clipplane which was added to the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API void postAddClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane, const nvutil::Subject::SmartPayload& payload );

      /** \brief Called from observed group before a clipplane will be removed from a Group.
          \param group The group the clipplane will be removed from.
          \param clipPlane The clipplane to be removed from the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API void preRemoveClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane, const Subject::SmartPayload& payload );
    protected:
      /** \brief Called by the framework right before destruction of the Subject \a subject.
        * \param subject Pointer to the subject that is going to be destroyed.
        * \param payload Payload object passed to the Subject::attach method.
        */
      NVSG_API virtual void onDestroyed( const Subject *subject, const nvutil::Subject::SmartPayload &payload );

      /** \brief Called by the framework when a state has changed.
        * \param subject Pointer to the subject that changed.
        * \param currentState Bitfield representing the current state.
        * \param changeState Bitfield representing the changing state.
        * \param payload Payload object passed to the Subject::attach method.
        * \return Provide the new state which should be stored by the observer. Usually it should be currentState | changeState.
        **/
      NVSG_API virtual unsigned int onUpdate( const Subject *subject, unsigned int currentState, unsigned int changeState, const nvutil::Subject::SmartPayload &payload ) const;

      /** \brief Called by the framework when a property changed.
        * \param subject Pointer to the subject that changed.
        * \param propertyId Id of the property which has changed.
        * \param payload Payload object passed to the Subject::attach method.
        */
      NVSG_API virtual void onUpdate( const Subject *subject, PropertyId propertyId , const nvutil::Subject::SmartPayload &payload) const;

      /** \brief Called by the framework after a child has been added to a Group.
          \param group The group the child was added to.
          \param child The child which was added to the group.
          \param index The index the child was added at.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API virtual void onPostAddChild( const nvsg::Group *group, const nvsg::NodeSharedPtr &child, unsigned int index, const nvutil::Subject::SmartPayload &payload );

      /** \brief Called by the framework before a child will be removed from a Group.
          \param group The group the child will be removed from.
          \param child The child to be removed from the group.
          \param index The index the child will be removed from.
          \param payload Payload object passed to the Subject::attach method
      **/
      NVSG_API virtual void onPreRemoveChild( const nvsg::Group *group, const nvsg::NodeSharedPtr &child, unsigned int index, const nvutil::Subject::SmartPayload &payload );

      /** \brief Called by the framework after the content of the specified group had been exchanged.
          \param group The group whose content had been exchanged.
          \param payload Payload object passed to the Subject::attach method.
        */
      NVSG_API virtual void onPostGroupExchanged( const nvsg::Group *group, const nvutil::Subject::SmartPayload& payload );

      /** \brief Called by the framework after a lightsource has been added.
          \param group The group the lightsource was added to.
          \param lightSource The lightsource which was added to the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API virtual void onPostAddLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr& lightSource, const nvutil::Subject::SmartPayload& payload );

      /** \brief Called by the framework before a lightsource will be removed from a Group.
          \param group The group the lightsource will be removed from.
          \param lightSource The lightsource to be removed from the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API virtual void onPreRemoveLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr &lightSource, const nvutil::Subject::SmartPayload& payload );

      /** \brief Called by the framework after a clipplane has been added.
          \param group The group the clipplane was added to.
          \param clipPlane The clipplane which was added to the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API virtual void onPostAddClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane, const nvutil::Subject::SmartPayload& payload);

      /** \brief Called by the framework before a clipplane will be removed from a Group.
          \param group The group the clipplane will be removed from.
          \param clipPlane The clipplane to be removed from the group.
          \param payload Payload object passed to the Subject::attach method.
      **/
      NVSG_API virtual void onPreRemoveClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane, const nvutil::Subject::SmartPayload& payload );
    private:
      unsigned int  m_state;  // reflects whether observed states are dirty or not
                              // a set state bit indicates a dirty state
  };

} // namespace nvutil

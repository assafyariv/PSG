#include "pch.h"
#include <nvutil/Observer.h>

namespace nvutil {
	Observer::Observer () {}
	Observer::~Observer () {}
	void Observer::onDestroyed( const Subject *subject, const nvutil::Subject::SmartPayload &payload ) {};

      /** \brief Called by the framework when a state has changed.
        * \param subject Pointer to the subject that changed.
        * \param currentState Bitfield representing the current state.
        * \param changeState Bitfield representing the changing state.
        * \param payload Payload object passed to the Subject::attach method.
        * \return Provide the new state which should be stored by the observer. Usually it should be currentState | changeState.
        **/
	unsigned int Observer::onUpdate( const Subject *subject, unsigned int currentState, unsigned int changeState, const nvutil::Subject::SmartPayload &payload ) const{ return 0;};
	void Observer::onUpdate( const Subject *subject, PropertyId propertyId , const nvutil::Subject::SmartPayload &payload) const {};
  void  Observer::onPostAddChild( const nvsg::Group *group, const nvsg::NodeSharedPtr &child, unsigned int index, const nvutil::Subject::SmartPayload &payload )
  {
  }

  void  Observer::onPreRemoveChild( const nvsg::Group *group, const nvsg::NodeSharedPtr &child, unsigned int index, const nvutil::Subject::SmartPayload &payload )
  {
  }


 
  void  Observer::onPostGroupExchanged( const nvsg::Group *group, const nvutil::Subject::SmartPayload& payload )
  {
  }


  void  Observer::onPostAddLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr& lightSource, const nvutil::Subject::SmartPayload& payload )
  {
  }


  void  Observer::onPreRemoveLightSource( const nvsg::Group *group, const nvsg::LightSourceSharedPtr &lightSource, const nvutil::Subject::SmartPayload& payload )
  {
  }

  void  Observer::onPostAddClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane, const nvutil::Subject::SmartPayload& payload)
  {
  }
 
  void  Observer::onPreRemoveClipPlane( const nvsg::Group *group, const nvsg::ClipPlaneSharedPtr& clipPlane, const nvutil::Subject::SmartPayload& payload )
  {
  }
 
   
}
#include "pch.h"
#include <nvsg/ViewState.h>

namespace nvsg {
	ViewState::ViewState()
	{
	}
	ViewState::~ViewState ()
	{
	}
	void ViewState::initReflectionInfo () {}

	ViewState::ViewState(const ViewState & vs) {}
	ViewStateSharedPtr ViewState::create ()
	{
		return ViewStateSharedPtr (ViewStateHandle::create()); 
	}
	nvutil::PropertyId ViewState::PID_LODRangeScale;
	nvutil::PropertyId ViewState::PID_TraversalMask;
	nvutil::PropertyId ViewState::PID_StereoEyeDistance;
	nvutil::PropertyId ViewState::PID_StereoAutomaticEyeDistanceFactor;
	nvutil::PropertyId ViewState::PID_TargetDistance;
	nvutil::PropertyId ViewState::PID_AutoClipPlanes;


}
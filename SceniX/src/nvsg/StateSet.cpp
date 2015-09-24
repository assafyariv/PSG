#include "pch.h"
#include <nvsg/StateSet.h>

namespace nvsg {
		void StateSet::syncIncarnation ( class nvutil::Incarnation const & (__thiscall nvsg::Object::*)(void)const ) const
	{
	}
	bool StateSet::isEquivalent (const Object *x,bool ise,bool isb) const { return false;}
	void StateSet::feedHashGenerator ( nvutil::HashGenerator  & ha ) const {}   
	bool  StateSet::determineShaderContainment() const {return false;}
	bool  StateSet::determineTransparencyContainment() const {return false;}
	void StateSet::initReflectionInfo () {}
	void StateSet::notifyChange (const nvutil::Subject *sub, unsigned int ui) const {}
	StateSet::StateSet ()
	{
		m_objectCode = nvsg::OC_STATESET;
	}
	StateSet::~StateSet ()
	{
	}
	StateSet::StateSet (const StateSet &x)
	{
	}
	StateSetSharedPtr StateSet::create ()
	{
		return StateSetSharedPtr (StateSetHandle::create());
	}	unsigned int  StateSet::determineHintsContainment(unsigned int which) const {return 0;}

}
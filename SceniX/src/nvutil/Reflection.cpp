#include "pch.h"
#include <nvutil/Reflection.h>
namespace nvutil 
{
	
	bool Reflection::hasProperty(const nvutil::PropertyId propertyId) const
	{
		return false;
	}

	bool Reflection::hasProperty(const std::string &name) const 
	{
		return false;
	}
	Reflection::~Reflection ()
	{
	}
	Reflection::Reflection (const Reflection &)
	{
	}
	

	
	ReflectionInfo *createReflectionInfo (void (class nvutil::ReflectionInfo &) )
	{
		return 0;
	}
}
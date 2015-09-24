#include "pch.h"

#include <nvutil/subject.h>

namespace nvutil 
{
	Subject::Subject () {}
	Subject::~Subject () {}
    void Subject::notify (Property *)const
	{
	}
	void Subject::notify (Subject const * , unsigned int)const
	{
	}

}
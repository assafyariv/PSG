#include "pch.h"
#include <nvsg/StatePass.h>

namespace nvsg {
	void StatePass::initReflectionInfo() {};
    StatePass::AttributeIterator StatePass::removeAttribute( const AttributeIterator & sci ) 
	{
		return this->endAttributes ();
	}
	StatePass::AttributeConstIterator StatePass::findAttribute( unsigned int code ) const {return this->endAttributes ();};
	StatePass::AttributeIterator StatePass::findAttribute( unsigned int code ) {return this->endAttributes ();};


}

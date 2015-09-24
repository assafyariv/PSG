#include "pch.h"
#include <nvsg/statevariant.h>

namespace nvsg {
	StateVariant::PassIterator StateVariant::addPass( const StatePassSharedPtr & pass ){ return beginPasses();};
	StateVariant::PassIterator StateVariant::insertPass( const PassIterator & spi, const StatePassSharedPtr & pass ){ return beginPasses();};
	bool StateVariant::removePass( const StatePassSharedPtr & pass ) { return true; };
	StateVariant::PassIterator StateVariant::removePass( const PassIterator & spi ) { return beginPasses();};
	void StateVariant::clearAttributes( const PassIterator & spi ) {};
	void StateVariant::initReflectionInfo() {};
	bool StateVariant::removeAttribute( const PassIterator & spi, const StateAttributeSharedPtr & attribute ) {return true;};
	unsigned int StateVariant::getNumberOfAttributes( const PassConstIterator & spci ) const { return 0;}
 StatePass::AttributeIterator StateVariant::addAttribute( const PassIterator & spi, const StateAttributeSharedPtr & attribute ) {return this->beginAttributes();};
 void StateVariant::clearPasses () {};
 bool StateVariant::replacePass( const StatePassSharedPtr & newPass, const StatePassSharedPtr & oldPass ) {return true;}
 bool StateVariant::replacePass( const StatePassSharedPtr & newPass, PassIterator & oldSPI ){return true;}
}
#include "pch.h"
#include <nvutil\HashGeneratorMurMur.h>

namespace nvutil 
{
	HashGeneratorMurMur::HashGeneratorMurMur( unsigned int x) {};
	void HashGeneratorMurMur::update( const unsigned char * input, unsigned int byteCount ) {};
	  void HashGeneratorMurMur::finalize( void * hash ) {}
	 std::string HashGeneratorMurMur::finalize() {return std::string("Hash");}
}

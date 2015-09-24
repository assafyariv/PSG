#include "pch.h"
#include <nvsg/Buffer.h>

namespace nvsg {
	
	void Buffer::setData( size_t dst_offset, size_t length, const void* src_data ) 
	{
	}
    void Buffer::setData( size_t dst_offset, size_t length, const BufferSharedPtr &src_buffer , size_t src_offset )
	{
	}

    void Buffer::getData( size_t src_offset, size_t length, void* dst_data ) const
	{
	}
    void Buffer::getData( size_t src_offset, size_t length, const BufferSharedPtr &dst_buffer , size_t dst_offset ) const
	{
	}

   

	Buffer::~Buffer () {}	

}
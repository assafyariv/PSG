#include "pch.h"
#include "D3DDal.h"

namespace nvsg {
D3DDalHost::D3DDalHost () {}
	D3DDalHost::~D3DDalHost () {}
	D3DDalHostSharedPtr D3DDalHost::create ()
	{
		return D3DDalHostSharedPtr (D3DDalHostHandle::create ());
	}
}
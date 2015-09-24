// SceniXWin32.cpp : Defines the exported functions for the DLL application.
//

#include "pch.h"
#include "SceniXWin32.h"


// This is an example of an exported variable
SCENIXWIN32_API int nSceniXWin32=0;

// This is an example of an exported function.
SCENIXWIN32_API int fnSceniXWin32(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see SceniXWin32.h for the class definition
CSceniXWin32::CSceniXWin32()
{
	return;
}

#pragma once
#include <windows.h>
#include <WinDef.h>
class  CElevator 
{
public :
	static int IsAdmin (  BOOL &fIsRunAsAdmin) ;
	static int ElevateMe (HWND hwnd) ;
};
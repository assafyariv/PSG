#pragma once

// AvocadoActiveXCtrl.h : main header file for AvocadoActiveXCtrl.DLL

#if !defined( __AFXCTL_H__ )
#error "include 'afxctl.h' before including this file"
#endif

#include "resource.h"       // main symbols


// CAvocadoActiveXCtrlApp : See AvocadoActiveXCtrl.cpp for implementation.

class CAvocadoActiveXCtrlApp : public COleControlModule
{
public:
	BOOL InitInstance();
	int ExitInstance();
};

extern const GUID CDECL _tlid;
extern const WORD _wVerMajor;
extern const WORD _wVerMinor;


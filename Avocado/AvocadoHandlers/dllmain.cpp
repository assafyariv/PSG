// dllmain.cpp : Implementation of DllMain.

#include "stdafx.h"
#include "resource.h"
#include "AvocadoHandlers_i.h"
#include "dllmain.h"
#include "xdlldata.h"

CAvocadoHandlersModule _AtlModule;

class CAvocadoHandlersApp : public CWinApp
{
public:

// Overrides
	virtual BOOL InitInstance();
	virtual int ExitInstance();

	DECLARE_MESSAGE_MAP()
};

BEGIN_MESSAGE_MAP(CAvocadoHandlersApp, CWinApp)
END_MESSAGE_MAP()

CAvocadoHandlersApp theApp;

BOOL CAvocadoHandlersApp::InitInstance()
{
	if (!PrxDllMain(m_hInstance, DLL_PROCESS_ATTACH, NULL))
		return FALSE;
	return CWinApp::InitInstance();
}

int CAvocadoHandlersApp::ExitInstance()
{
	return CWinApp::ExitInstance();
}

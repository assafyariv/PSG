// AvocadoActiveXCtrl.cpp : Implementation of CAvocadoActiveXCtrlApp and DLL registration.

#include "stdafx.h"
#include "AvocadoActiveXCtrl.h"
#include "../AvocadoEngine/AvocadoAppInterface.h"
#include "Cathelp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CAvocadoActiveXCtrlApp theApp;

const GUID CDECL _tlid = { 0x1D49C79F, 0x2D78, 0x417F, { 0x91, 0x23, 0x8D, 0xF5, 0xC1, 0x8E, 0xA, 0x7F } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;



// CLSID_SafeItem - Necessary for safe ActiveX control
// The value is taken from IMPLEMENT_OLECREATE_EX in MFCSafeActiveXCtrl.cpp.
// It is actually the CLSID of the ActiveX control.

const CATID CLSID_SafeItem =
{ 0x1ebae592, 0x7515, 0x43c2,{ 0xa6, 0xf1, 0xcd, 0xee, 0xdf, 0x3f, 0xd8, 0x2b}};

// CAvocadoActiveXCtrlApp::InitInstance - DLL initialization

BOOL CAvocadoActiveXCtrlApp::InitInstance()
{
	BOOL bInit = COleControlModule::InitInstance();

	if (bInit)
	{
		// TODO: Add your own module initialization code here.
		avocado::AvocadoInit ();
		
	}

	return bInit;
}



// CAvocadoActiveXCtrlApp::ExitInstance - DLL termination

int CAvocadoActiveXCtrlApp::ExitInstance()
{
	// TODO: Add your own module termination code here.
	avocado::AvocadoTerminate();
	return COleControlModule::ExitInstance();
}



// DllRegisterServer - Adds entries to the system registry

STDAPI DllRegisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(TRUE))
		return ResultFromScode(SELFREG_E_CLASS);
		// Mark the control as safe for initializing. (Added)

	HRESULT hr = CreateComponentCategory(CATID_SafeForInitializing, 
		L"Controls safely initializable from persistent data!");
	if (FAILED(hr))
		return hr;

	 hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	// Mark the control as safe for scripting. (Added)

	hr = CreateComponentCategory(CATID_SafeForScripting,
		L"Controls safely  scriptable!");
	if (FAILED(hr))
		return hr;
	
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;
	hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_PersistsToFile);
	if (FAILED(hr))
		return hr;
		hr = RegisterCLSIDInCategory(CLSID_SafeItem, CATID_DesignTimeUIActivatableControl);
	if (FAILED(hr))
		return hr;
	return NOERROR;
}



// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	AFX_MANAGE_STATE(_afxModuleAddrThis);

	if (!AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor))
		return ResultFromScode(SELFREG_E_TYPELIB);

	if (!COleObjectFactoryEx::UpdateRegistryAll(FALSE))
		return ResultFromScode(SELFREG_E_CLASS);
		// Remove entries from the registry.

	// safe stuff ..
	HRESULT hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForInitializing);
	if (FAILED(hr))
		return hr;

	hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_SafeForScripting);
	if (FAILED(hr))
		return hr;
		hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_PersistsToFile);
	if (FAILED(hr))
		return hr;
		hr = UnRegisterCLSIDInCategory(CLSID_SafeItem, CATID_DesignTimeUIActivatableControl);
	if (FAILED(hr))
		return hr;
	
	return NOERROR;
}

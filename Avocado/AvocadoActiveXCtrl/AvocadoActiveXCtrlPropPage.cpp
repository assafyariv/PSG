// AvocadoActiveXCtrlPropPage.cpp : Implementation of the CAvocadoActiveXCtrlPropPage property page class.

#include "stdafx.h"
#include "AvocadoActiveXCtrl.h"
#include "AvocadoActiveXCtrlPropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CAvocadoActiveXCtrlPropPage, COlePropertyPage)



// Message map

BEGIN_MESSAGE_MAP(CAvocadoActiveXCtrlPropPage, COlePropertyPage)
END_MESSAGE_MAP()



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAvocadoActiveXCtrlPropPage, "AVOCADOACTIVEX.AvocadoActiveXPropPage.1",
	0x44a9ac89, 0xec0c, 0x4879, 0xa2, 0xb8, 0xdf, 0x51, 0x67, 0xd1, 0x11, 0x8f)



// CAvocadoActiveXCtrlPropPage::CAvocadoActiveXCtrlPropPageFactory::UpdateRegistry -
// Adds or removes system registry entries for CAvocadoActiveXCtrlPropPage

BOOL CAvocadoActiveXCtrlPropPage::CAvocadoActiveXCtrlPropPageFactory::UpdateRegistry(BOOL bRegister)
{
	if (bRegister)
		return AfxOleRegisterPropertyPageClass(AfxGetInstanceHandle(),
			m_clsid, IDS_AVOCADOACTIVEXCTRL_PPG);
	else
		return AfxOleUnregisterClass(m_clsid, NULL);
}



// CAvocadoActiveXCtrlPropPage::CAvocadoActiveXCtrlPropPage - Constructor

CAvocadoActiveXCtrlPropPage::CAvocadoActiveXCtrlPropPage() :
	COlePropertyPage(IDD, IDS_AVOCADOACTIVEXCTRL_PPG_CAPTION)
{
}



// CAvocadoActiveXCtrlPropPage::DoDataExchange - Moves data between page and properties

void CAvocadoActiveXCtrlPropPage::DoDataExchange(CDataExchange* pDX)
{
	DDP_PostProcessing(pDX);
}



// CAvocadoActiveXCtrlPropPage message handlers

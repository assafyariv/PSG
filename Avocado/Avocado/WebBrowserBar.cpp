/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
// This MFC Samples source code demonstrates using MFC Microsoft Office Fluent User Interface 
// (the "Fluent UI") and is provided only as referential material to supplement the 
// Microsoft Foundation Classes Reference and related electronic documentation 
// included with the MFC C++ library software.  
// License terms to copy, use or distribute the Fluent UI are available separately.  
// To learn more about our Fluent UI licensing program, please visit 
// http://msdn.microsoft.com/officeui.
//
// Copyright (C) Microsoft Corporation
// All rights reserved.

#include "stdafx.h"
#include "WebBrowserBar.h"
#include "Avocado.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const int nBorderSize = 10;

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserBar

CWebBrowserBar::CWebBrowserBar()
{
	m_nMyCalendarsY = 0;
}

CWebBrowserBar::~CWebBrowserBar()
{
}
BEGIN_EVENTSINK_MAP(CWebBrowserBar, CWnd)
	ON_EVENT(CWebBrowserBar, ID_VIEW_HTMLVIEW, 259 /* DocumentComplete */,
	OnDocumentComplete, VTS_DISPATCH VTS_PVARIANT)
END_EVENTSINK_MAP()

BEGIN_MESSAGE_MAP(CWebBrowserBar, CWnd)
	ON_WM_CREATE()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_SETTINGCHANGE()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserBar message handlers

int CWebBrowserBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy(0, 0, 0, 0);
	m_wndCalendar.Create(_T("CWebBrowser2"),
		NULL,WS_CHILD | WS_VISIBLE, rectDummy, this, 1303);

	CBitmap bmp;
	bmp.LoadBitmap(IDB_PAGES_SMALL_HC);

	m_Images.Create(16, 16, ILC_COLOR24 | ILC_MASK, 0, 0);
	m_Images.Add(&bmp, RGB(255, 0, 255));

	return 0;
}

BOOL CWebBrowserBar::OnEraseBkgnd(CDC* /*pDC*/)
{
	return TRUE;
}

void CWebBrowserBar::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	int nMyCalendarsHeight = 2;

	if (m_wndCalendar.GetSafeHwnd() != NULL)
	{
		m_wndCalendar.SetWindowPos(NULL, nBorderSize, nBorderSize, cx - 2 * nBorderSize, cy - 2 * nBorderSize - nMyCalendarsHeight - 10, SWP_NOZORDER | SWP_NOACTIVATE);
	}

	m_nMyCalendarsY = cy - nMyCalendarsHeight;

}

BOOL CWebBrowserBar::Create(const RECT& rect, CWnd* pParentWnd, UINT nID)
{
	BOOL ret = CWnd::Create(NULL, _T("BrowserWnd"), WS_CHILD | WS_VISIBLE | WF_OLECTLCONTAINER , rect, pParentWnd, nID);
	Init();
	return ret;
}
void CWebBrowserBar::Init () {
	///m_wndCalendar.
	m_wndCalendar.put_ToolBar(TRUE);
	m_wndCalendar.put_AddressBar(TRUE);

	/*VARIANT vtBandGUID;
	VARIANT vtShow;
	VARIANT vtNotUsed;
	vtBandGUID.vt = VT_BSTR;
	vtBandGUID.bstrVal = SysAllocString( L"{01E04581-4EEE-11d0-BFE9-00AA005B4383}");
	//vtBandGUID.vt = VT_INT;
	//vtBandGUID.intVal = 0x0009;//SysAllocString( _T("{01E04581-4EEE-11d0-BFE9-00AA005B4383}") );

	vtShow.vt = VT_BOOL;
	vtShow.boolVal = VARIANT_TRUE;
	vtNotUsed.vt = VT_INT;
	vtNotUsed.intVal = 1;
	m_wndCalendar.ShowBrowserBar (&vtBandGUID,&vtShow,&vtNotUsed);*/

	//	VARIANT vtZoom;
	//vtZoom.vt= VT_INT;
	//	vtZoom.intVal = 1;
	m_wndCalendar.put_Resizable (VARIANT_TRUE);
	m_wndCalendar.put_Silent (VARIANT_TRUE);
	//	m_wndCalendar.ExecWB (OLECMDID_OPTICAL_ZOOM,OLECMDEXECOPT_DONTPROMPTUSER,&vtZoom,0);


	//m_wndCalendar.Navigate(_T("MININT-q53n57s/BServer/Products/Plist"), NULL, NULL, NULL, NULL);
	m_wndCalendar.Navigate(_T("http://localhost/BServer/Products/Plist"), NULL, NULL, NULL, NULL);
	//
}
void CWebBrowserBar::OnPaint()
{

	CPaintDC dc(this); // device context for painting

	CRect rectClient;
	GetClientRect(rectClient);

	dc.FillRect(rectClient, &afxGlobalData.brWindow);

	if (rectClient.bottom - m_nMyCalendarsY > 0)
	{
		CRect rectMyCalendarsCaption = rectClient;
		rectMyCalendarsCaption.top = m_nMyCalendarsY;
		rectMyCalendarsCaption.bottom = rectMyCalendarsCaption.top + afxGlobalData.GetTextHeight(TRUE) * 3 / 2;

		COLORREF clrText = CMFCVisualManager::GetInstance()->OnDrawPaneCaption(&dc, NULL, FALSE, rectMyCalendarsCaption, CRect(0, 0, 0, 0));

		CPen* pOldPen = dc.SelectObject(&afxGlobalData.penBarShadow);

		dc.MoveTo(rectMyCalendarsCaption.left - 1, rectMyCalendarsCaption.top);
		dc.LineTo(rectMyCalendarsCaption.right, rectMyCalendarsCaption.top);

		dc.SelectStockObject(BLACK_PEN);

		dc.MoveTo(rectMyCalendarsCaption.left - 1, rectMyCalendarsCaption.bottom);
		dc.LineTo(rectMyCalendarsCaption.right, rectMyCalendarsCaption.bottom);

		dc.SelectObject(pOldPen);

		CRect rectText = rectMyCalendarsCaption;
		rectText.DeflateRect(10, 0);

		dc.SetBkMode(TRANSPARENT);
		dc.SetTextColor(clrText);

		CFont* pOldFont = dc.SelectObject(&afxGlobalData.fontRegular);

		BOOL bNameValid;
		CString str;

		bNameValid = str.LoadString(IDS_MYCALENDARS);
		ASSERT(bNameValid);
		dc.DrawText(str, rectText, DT_VCENTER | DT_LEFT | DT_SINGLELINE);

		CRect rectCalendar = rectClient;
		rectCalendar.top = rectMyCalendarsCaption.bottom + 5;
		rectCalendar.bottom = rectCalendar.top + afxGlobalData.GetTextHeight(TRUE) * 3 / 2 - 5;

		dc.FillSolidRect(rectCalendar, RGB(255, 255, 213));

		rectCalendar.DeflateRect(20, 0);
		m_Images.Draw(&dc, 3, rectCalendar.TopLeft(), 0);

		rectCalendar.left += 20;

		bNameValid = str.LoadString(IDS_CALENDAR);
		ASSERT(bNameValid);

		dc.SetTextColor(afxGlobalData.clrHotLinkNormalText);
		dc.DrawText(str, rectCalendar, DT_VCENTER | DT_LEFT | DT_SINGLELINE);

		dc.SelectObject(pOldFont);
	}
	//CWnd::OnPaint ();
}

void CWebBrowserBar::OnDocumentComplete(LPDISPATCH lpDisp,
	VARIANT FAR* URL)
{
	/*  IUnknown*  pUnk;
	LPDISPATCH lpWBDisp;
	HRESULT    hr;

	pUnk = m_wndCalendar.GetControlUnknown();
	ASSERT(pUnk);

	hr = pUnk->QueryInterface(IID_IDispatch, (void**)&lpWBDisp);
	ASSERT(SUCCEEDED(hr));

	if (lpDisp == lpWBDisp )
	{*/

	//	VARIANT vtZoom;
	//	vtZoom.vt=    VT_INT;
	//	vtZoom.intVal = 100;
	//	m_wndCalendar.put_Resizable (VARIANT_TRUE);
	//	m_wndCalendar.put_Silent (VARIANT_TRUE);
	//	m_wndCalendar.ExecWB (OLECMDID_OPTICAL_ZOOM,OLECMDEXECOPT_DONTPROMPTUSER,&vtZoom,NULL);

	// Top-level Window object, so document has been loaded
	// TRACE("Web document is finished downloading\n");
	//}

	//  lpWBDisp->Release();

	VARIANT vtZoom;
	vtZoom.vt=    VT_INT;
	vtZoom.intVal = 10;
	m_wndCalendar.put_Resizable (VARIANT_TRUE);
	m_wndCalendar.put_Silent (VARIANT_TRUE);

	m_wndCalendar.ExecWB (OLECMDID_OPTICAL_ZOOM,OLECMDEXECOPT_DONTPROMPTUSER,&vtZoom,NULL);

}





BOOL CWebBrowserBar::OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar)
{
	// TODO: Add your specialized code here and/or call the base class
	/*  if (dispid == DISPID_AMBIENT_DLCONTROL) {
	pvar->vt = VT_I4; 
	pvar->lVal = DLCTL_NO_SCRIPTS | DLCTL_NO_JAVA | DLCTL_NO_RUNACTIVEXCTLS | DLCTL_NO_DLACTIVEXCTLS;
	return true;
	}*/
	//  return HtmlView::OnAmbientProperty(pSite, dispid, pvar);
	//DISPID_AMBIENT_USERMODE
	return CWnd::OnAmbientProperty(pSite, dispid, pvar);
}

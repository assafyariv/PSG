// AvocadoActiveXCtrlCtrl.cpp : Implementation of the CAvocadoActiveXCtrlCtrl ActiveX Control class.

#include "stdafx.h"
#include "../Avocado/AvocadoDoc.h"
#include "../Avocado/AvocadoView.h"
#include "AvocadoActiveXCtrl.h"
#include "AvocadoActiveXCtrlCtrl.h"
#include "AvocadoActiveXCtrlPropPage.h"
#include "afxdialogex.h"
#include "../AvocadoEngine/AvocadoAppInterface.h"


using namespace avocado;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


IMPLEMENT_DYNCREATE(CAvocadoActiveXCtrlCtrl, COleControl)



// Message map

BEGIN_MESSAGE_MAP(CAvocadoActiveXCtrlCtrl, COleControl)
	ON_OLEVERB(AFX_IDS_VERB_PROPERTIES, OnProperties)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_MBUTTONDOWN()
	ON_WM_MBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_SETFOCUS()
END_MESSAGE_MAP()



// Dispatch map

BEGIN_DISPATCH_MAP(CAvocadoActiveXCtrlCtrl, COleControl)
END_DISPATCH_MAP()



// Event map

BEGIN_EVENT_MAP(CAvocadoActiveXCtrlCtrl, COleControl)
END_EVENT_MAP()



// Property pages

// TODO: Add more property pages as needed.  Remember to increase the count!
BEGIN_PROPPAGEIDS(CAvocadoActiveXCtrlCtrl, 1)
	PROPPAGEID(CAvocadoActiveXCtrlPropPage::guid)
END_PROPPAGEIDS(CAvocadoActiveXCtrlCtrl)



// Initialize class factory and guid

IMPLEMENT_OLECREATE_EX(CAvocadoActiveXCtrlCtrl, "AVOCADOACTIVEXCT.AvocadoActiveXCtCtrl.1",
	0x3702b77e, 0x8fd4, 0x452e, 0xae, 0x15, 0x82, 0xbe, 0xef, 0x40, 0xcb, 0xd2)



// Type library ID and version

IMPLEMENT_OLETYPELIB(CAvocadoActiveXCtrlCtrl, _tlid, _wVerMajor, _wVerMinor)



// Interface IDs

const IID IID_DAvocadoActiveXCtrl = { 0xB220DA40, 0xF9D, 0x4B4F, { 0x98, 0x78, 0xAA, 0x20, 0x4F, 0xC8, 0x55, 0xBE } };
const IID IID_DAvocadoActiveXCtrlEvents = { 0x6976EC40, 0x2A03, 0x4DA5, { 0xA9, 0x8B, 0xF5, 0xFC, 0xE7, 0x86, 0x91, 0xF7 } };


// Control type information

static const DWORD _dwAvocadoActiveXCtrlOleMisc =
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST |
	OLEMISC_INSIDEOUT |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_RECOMPOSEONRESIZE;

IMPLEMENT_OLECTLTYPE(CAvocadoActiveXCtrlCtrl, IDS_AVOCADOACTIVEXCTRL, _dwAvocadoActiveXCtrlOleMisc)



// CAvocadoActiveXCtrlCtrl::CAvocadoActiveXCtrlCtrlFactory::UpdateRegistry -
// Adds or removes system registry entries for CAvocadoActiveXCtrlCtrl

BOOL CAvocadoActiveXCtrlCtrl::CAvocadoActiveXCtrlCtrlFactory::UpdateRegistry(BOOL bRegister)
{
	// TODO: Verify that your control follows apartment-model threading rules.
	// Refer to MFC TechNote 64 for more information.
	// If your control does not conform to the apartment-model rules, then
	// you must modify the code below, changing the 6th parameter from
	// afxRegApartmentThreading to 0.

	if (bRegister)
		return AfxOleRegisterControlClass(
			AfxGetInstanceHandle(),
			m_clsid,
			m_lpszProgID,
			IDS_AVOCADOACTIVEXCTRL,
			IDB_AVOCADOACTIVEXCTRL,
			afxRegApartmentThreading,
			_dwAvocadoActiveXCtrlOleMisc,
			_tlid,
			_wVerMajor,
			_wVerMinor);
	else
		return AfxOleUnregisterClass(m_clsid, m_lpszProgID);
}



// Licensing strings

static const TCHAR BASED_CODE _szLicFileName[] = _T("MFCSafeActiveX.lic");

static const WCHAR BASED_CODE _szLicString[] =
	L"Copyright (c) 2009 ";


// CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::VerifyUserLicense -
// Checks for existence of a user license
BOOL CAvocadoActiveXCtrlCtrl::CAvocadoActiveXCtrlCtrlFactory::VerifyUserLicense()
{
	return AfxVerifyLicFile(AfxGetInstanceHandle(), _szLicFileName,
		_szLicString);
}


// CMFCSafeActiveXCtrl::CMFCSafeActiveXCtrlFactory::GetLicenseKey -
// Returns a runtime licensing key
BOOL CAvocadoActiveXCtrlCtrl::CAvocadoActiveXCtrlCtrlFactory::GetLicenseKey(DWORD dwReserved,
	BSTR FAR* pbstrKey)
{
	if (pbstrKey == NULL)
		return FALSE;

	*pbstrKey = SysAllocString(_szLicString);
	return (*pbstrKey != NULL);
}

// CAvocadoActiveXCtrlCtrl::CAvocadoActiveXCtrlCtrl - Constructor
static int v = -1;
CAvocadoActiveXCtrlCtrl::CAvocadoActiveXCtrlCtrl()
{
	InitializeIIDs(&IID_DAvocadoActiveXCtrl, &IID_DAvocadoActiveXCtrlEvents);

	{
		
	//	InitAvocado();
	

			/*    AvocadoMessageInterface *m_msgInter = new AvocadoMessageInterface ;
				m_msgInter->m_params = new AvocadoParams;
				m_msgInter->m_params->m_viewId = v;
				m_msgInter->m_params->m_px = 100;
				m_msgInter->m_params->m_py = 100;
				m_msgInter->m_message = "OnSize";

				OnSendAvocadoGeneralMessage(m_msgInter);*/
	}
	// TODO: Initialize your control's instance data here.
}
void
CAvocadoActiveXCtrlCtrl::InitAvocado( void *hwnd ,int cx, int cy)
{
	//HDC hdc = GetDC()->m_hDC;
		int d= OnCreateDoc();
	SetActiveDoc (d);
	//v= OnCreateDCView((void*)(&hdc), 0);
	//OnSizeView(v,cx-5,cy-5);
	if (hwnd!=NULL)
	{
		//CDC* cd = m_childWnd->GetDC();
		//HDC h = cd->m_hDC;//GetSafeHdc();
		//v= OnCreateDCView(&h, 0);
	  v= OnCreateView(hwnd, 0);
	  OnSizeView(v,cx,cy);
	
	  //OnSerializeDoc (d,std::string ("c:\\Education\\myhouse.avc"));
	  /*  CPaintDC dc(m_childWnd); 
	  OnSendAvocadoGeneralStringMessage("OnPaint",v,"");
	 

	  SwapBuffers(dc.GetSafeHdc());*/
	}
	 
/*	*/
}

// CAvocadoActiveXCtrlCtrl::~CAvocadoActiveXCtrlCtrl - Destructor

CAvocadoActiveXCtrlCtrl::~CAvocadoActiveXCtrlCtrl()
{
	// TODO: Cleanup your control's instance data here.
}



// CAvocadoActiveXCtrlCtrl::OnDraw - Drawing function

void CAvocadoActiveXCtrlCtrl::OnDraw(
			CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid)
{
	if (!pdc)
		return;
	//m_AVWnd->Invalidate();
	//if (v < 0)
	//	return;
// CPaintDC dc(m_childWnd); 
	//  OnSendAvocadoGeneralStringMessage("OnPaint",v,"");
	 

	//  SwapBuffers(dc.GetSafeHdc());
//SwapBuffers(pdc->m_hDC);
/*	AvocadoMessageInterface *m_msgInter = new AvocadoMessageInterface ;
				m_msgInter->m_params = new AvocadoParams;
				m_msgInter->m_params->m_viewId = v;
				//m_msgInter->m_params.m_y = 0;
				m_msgInter->m_message = "OnPaint";

				OnSendAvocadoGeneralMessage(m_msgInter);*/
	// TODO: Replace the following code with your own drawing code.
	//pdc->FillRect(rcBounds, CBrush::FromHandle((HBRUSH)GetStockObject(WHITE_BRUSH)));
	//pdc->Ellipse(rcBounds);
}



// CAvocadoActiveXCtrlCtrl::DoPropExchange - Persistence support

void CAvocadoActiveXCtrlCtrl::DoPropExchange(CPropExchange* pPX)
{
	ExchangeVersion(pPX, MAKELONG(_wVerMinor, _wVerMajor));
	COleControl::DoPropExchange(pPX);

	// TODO: Call PX_ functions for each persistent custom property.
}



// CAvocadoActiveXCtrlCtrl::OnResetState - Reset control to default state

void CAvocadoActiveXCtrlCtrl::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange

	// TODO: Reset any other control state here.
}



// CAvocadoActiveXCtrlCtrl message handlers

class AvoCWnd : public CWnd 
{
	void OnSize(UINT nType, int cx, int cy) 
	{
		if (v >= 0)
		OnSizeView(0,cx,cy);
	}
	void OnMButtonUp(UINT nFlags, CPoint point)
	{
		avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_MOUSE_MUP, 0, point.x,point.y, 0);
	}
	void OnMButtonDown(UINT nFlags, CPoint point)
	{
		avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_MOUSE_MDOWN, 0, point.x,point.y, 0);
	}
	void OnMouseMove(UINT nFlags, CPoint point)
	{
		avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_MOUSE_MOVE,0, point.x, point.y, 0);
	}

};
int CAvocadoActiveXCtrlCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
//   lpCreateStruct->style |= (/*WS_CLIPCHILDREN |*/ WS_CLIPSIBLINGS | CS_OWNDC);
    lpCreateStruct->style |= (WS_VISIBLE | CS_OWNDC);
	if (COleControl::OnCreate(lpCreateStruct) == -1)
		return -1;

	//m_AVDoc =  (CAvocadoDoc*)RUNTIME_CLASS(CAvocadoDoc );//();
	//int d= OnCreateDoc();
	//SetActiveDoc (d);
	//m_AVWnd =  (CAvocadoView*)RUNTIME_CLASS(CAvocadoView );//();
	//m_AVDoc->AddView (m_AVWnd);
	//this->createchil
	//CAvocadoView *a = (CAvocadoView*) m_AVWnd;
	//m_AVWnd->Create (_T("CAvocadoView"),"Hi",(WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS  | CS_OWNDC ),CRect (0,0,lpCreateStruct->cx,lpCreateStruct->cy),this,123456);
	//m_AVWnd->SetWindowPos (this,0,0,lpCreateStruct->cx,lpCreateStruct->cy,0/*nType*/);
		
	m_childWnd = new AvoCWnd ();
	m_childWnd->Create (0/*_T("STATIC")*/,L"Hi",(WS_VISIBLE | WS_CHILD | WS_CLIPCHILDREN | WS_CLIPSIBLINGS  |  CS_OWNDC ),CRect (0,0,lpCreateStruct->cx,lpCreateStruct->cy),this,123456);
	//m_childWnd->SetActiveWindow();
	//m_childWnd->SetFocus();
	//this->GetWindow()->CreateEx();
	// TODO:  Add your specialized creation code here
	
	HWND h = m_childWnd->GetSafeHwnd();
	//InitAvocado(&h,lpCreateStruct->cx,lpCreateStruct->cy);

	return 0;
}

/*void CAvocadoActiveXCtrlCtrl::window
{
	m_childWnd->SendMessage (message,w,l);
}
*/
void CAvocadoActiveXCtrlCtrl::OnSize(UINT nType, int cx, int cy)
{
	m_childWnd->SetWindowPos (this,0,0,cx,cy,0);
	COleControl::OnSize(nType, cx, cy);
	
	//OnPaint();
	//LPARAM parap = (long(cx) & 0x0000ffff) | ((long(cy) << 16) & 0xffff0000);

	//m_AVWnd->PostMessageA (WM_SIZE,0,parap);
	//m_childWnd->PostMessageA (WM_SIZE,0,parap);
	if (v >= 0)
	OnSizeView(0,cx,cy);
	OnPaint ();
	// TODO: Add your message handler code here
}


void CAvocadoActiveXCtrlCtrl::OnDestroy()
{
	//m_AVWnd->DestroyWindow();
	m_childWnd->DestroyWindow();
	delete m_childWnd;
	
	COleControl::OnDestroy();
	
	// TODO: Add your message handler code here
}


void CAvocadoActiveXCtrlCtrl::OnPaint()
{
	//m_AVWnd->Invalidate();
//	m_AVWnd->InvokePaint();
//	LPARAM parap = 0;
	//m_AVWnd->PostMessageA (WM_PAINT,0,parap);
//	CPaintDC dc (m_childWnd);


	CPaintDC dc(m_childWnd); // device context for painting

	//CDC* cd = m_childWnd->GetDC();
	
	// TODO: Add your message handler code here
	// Do not call COleControl::OnPaint() for painting messages
	 OnSendAvocadoGeneralStringMessage("OnPaint",v,"");
	 
	//SwapBuffers(cd->GetSafeHdc());
	  SwapBuffers(dc.GetSafeHdc());

	//  COleControl::OnPaint(GetDC());
	 
}


void CAvocadoActiveXCtrlCtrl::OnMButtonDown(UINT nFlags, CPoint point)
{
	//COleControl::OnMButtonDown(nFlags, point);
	//m_childWnd->SetFocus();
	// TODO: Add your message handler code here and/or call default
avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_MOUSE_MDOWN, 0, point.x,point.y, 0);
	//m_AVWnd->OnMButtonDown(nFlags,point);

	//
	
}


void CAvocadoActiveXCtrlCtrl::OnMButtonUp(UINT nFlags, CPoint point)
{
	//COleControl::OnMButtonUp(nFlags, point);
	//m_childWnd->SetFocus();
	// TODO: Add your message handler code here and/or call default
	avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_MOUSE_MUP, 0, point.x,point.y, 0);
	//COleControl::OnMButtonUp(nFlags, point);
	//m_AVWnd->OnMButtonUp(nFlags,point);
	OnPaint();

	
}


void CAvocadoActiveXCtrlCtrl::OnMouseMove(UINT nFlags, CPoint point)
{

	//COleControl::OnMouseMove(nFlags, point);
	// TODO: Add your message handler code here and/or call default
	//m_childWnd->SetFocus ();
	avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_MOUSE_MOVE,0, point.x, point.y, 0);
	//m_AVWnd->OnMouseMove(nFlags,point);

//	

//	OnPaint();
}


void CAvocadoActiveXCtrlCtrl::OnSetFocus(CWnd* pOldWnd)
{
	COleControl::OnSetFocus(pOldWnd);
//	m_childWnd->SetFocus();
	// TODO: Add your message handler code here
}


BOOL CAvocadoActiveXCtrlCtrl::OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class

	return COleControl::OnWndMsg(message, wParam, lParam, pResult);
}


void CAvocadoActiveXCtrlCtrl::OnClick(USHORT iButton)
{
	// TODO: Add your specialized code here and/or call the base class
	//m_childWnd->SetFocus();
	//this->setfo
	//COleControl::OnClick(iButton);
	//avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_MOUSE_MDOWN, 0, point.x,point.y, 0);
}


BOOL CAvocadoActiveXCtrlCtrl::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Add your specialized code here and/or call the base class
	
	cs.lpszClass = _T("STATIC");


	return COleControl::PreCreateWindow(cs);
}

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

// AvocadoView.cpp : implementation of the CAvocadoView class
//
#ifndef _AVOCADO_ACTIVEX
#include "stdafx.h"
#endif
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#ifndef _AVOCADO_ACTIVEX
#include "Avocado.h"
#else
#include <stdafx.h>
#include "MFCSafeActiveX.h"

#endif
#endif

#include "AvocadoDoc.h"
#include "AvocadoView.h"
#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifndef SHARED_HANDLERS
void CAvocadoViewInterface::InvokePaintView()
{
	m_view->InvokePaint();
}
void CAvocadoViewInterface::SelectionChanged(std::vector<int> selectedElements)
{
	m_view->SetCurrentSelection(selectedElements);
}
void CAvocadoViewInterface::ViewParamChanged (const char *paramName, const char *value)
{
	string stpn (paramName);
	string val  (value);
	m_view->SetViewParameter (string (paramName),string (value));
}
void CAvocadoViewInterface::ErrorCallback (const char *errDesc)
{
	m_view->RaiseError(string (errDesc));
}
void CAvocadoViewInterface::SuccessCallback ()
{
	
}
void
	CAvocadoView::RaiseError (std::string err)
{
	((CAvocadoDoc*)(GetDocument()))->RaiseError (err);
}
int
CAvocadoView::GetViewParameterAsInt (string paramName)
{
	int ivalue = 0;
	for (size_t i=0;i<m_viewParameters.size();i++)
	{
		if (m_viewParameters[i].first == paramName)
		{
			string val = m_viewParameters[i].second ;
			std::istringstream xs(val);
			xs >> ivalue;
			return ivalue;
		}
	} 
	return -1;
}
bool
CAvocadoView::SetViewParameter (string paramName, string value)
{
	for (size_t i=0;i<m_viewParameters.size();i++)
	{
		if (m_viewParameters[i].first == paramName)
		{
			m_viewParameters[i].second = value;
			return true;
		}
	} 
	{
		m_viewParameters.push_back (pair<string,string> (paramName,value));	
		return true;
	}
	return false;
}
#endif
// CAvocadoView

IMPLEMENT_DYNCREATE(CAvocadoView, CView)

BEGIN_MESSAGE_MAP(CAvocadoView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CAvocadoView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()

	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
#ifndef SHARED_HANDLERS
	ON_WM_PAINT()
  ON_WM_MOUSEMOVE()
  ON_WM_MOUSEHWHEEL()
  ON_WM_MOUSEWHEEL()
  ON_WM_LBUTTONDOWN()
  ON_WM_LBUTTONUP()
  ON_WM_RBUTTONUP()
  ON_WM_TIMER()
  ON_WM_TIMECHANGE()
    ON_WM_MBUTTONDOWN()
  ON_WM_MBUTTONUP()
#ifndef _AVOCADO_ACTIVEX
  ON_COMMAND(IDC_CUT, &CAvocadoView::OnHide)
  ON_COMMAND(IDC_COPY, &CAvocadoView::OnCopy)
  ON_COMMAND(IDC_PASTE, &CAvocadoView::OnPaste)
//  ON_COMMAND(ID_CANCEL_EDIT_SRVR,&CAvocadoView::OnCancelEditSrvr);

  ON_COMMAND(IDC_PLAY, &CAvocadoView::OnPlay)
  ON_COMMAND(IDC_STOP, &CAvocadoView::OnStop)
  ON_COMMAND(IDC_PAUSE, &CAvocadoView::OnPause)
  ON_COMMAND(IDC_TIMERTOG, &CAvocadoView::OnTimertog)
  ON_COMMAND(ID_LAYOUT_GALERY, &CAvocadoView::OnLayoutGalery)
  ON_UPDATE_COMMAND_UI(ID_LAYOUT_GALERY, &CAvocadoView::OnUpdateLayoutGalery)
#endif
#endif

  ON_COMMAND(ID_EDIT_COPY, &CAvocadoView::OnEditCopy)
//  ON_UPDATE_COMMAND_UI(ID_GRAPHICS_RAYTRACING, &CAvocadoView::OnUpdateGraphicsRaytracing)
END_MESSAGE_MAP()

// CAvocadoView construction/destruction

CAvocadoView::CAvocadoView()
{
	// TODO: add construction code here
#ifndef SHARED_HANDLERS
	m_viewInterface = new CAvocadoViewInterface(this);
#endif
	m_showProgressText = false;
	m_splashBmpInit = false;
	m_viewSizeX = 0;
	m_viewSizeY = 0;
	mouse_pt_init = false;
	m_animating = false;
	m_frameCount = 0;
	m_rayTracing = false;
	m_displayLights = false;
	m_displayCameras = false;
	m_displaySpinCenter = false;
	m_modelScale = false;
	m_modelMove = false;
	m_modelRotate = false;
	m_renderStyle = RENDER_STYLE_SHADED;
	m_orthographic = false;
	m_displayAnnotations = false;
	m_preSelectionHighlights = true;
	m_currentSelection = -1;
	m_displayFilters = ~0;
	m_dragFilters = 0;
	m_mouseX = 0;
	m_mouseY = 0;
	m_id = -1;
}

CAvocadoView::~CAvocadoView()
{
#ifndef SHARED_HANDLERS
	delete m_viewInterface;
#endif
}

BOOL CAvocadoView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  the CREATESTRUCT cs
	if (!CView::PreCreateWindow(cs))
		return FALSE;
#ifndef SHARED_HANDLERS
	cs.style |= WS_CLIPCHILDREN | WS_CLIPSIBLINGS; // SceniX, required for OpenGL.
	cs.dwExStyle |= WS_EX_CLIENTEDGE;// | WS_EX_LAYERED;

	cs.style &= ~WS_BORDER;
	cs.style &= ~(WS_HSCROLL|WS_VSCROLL);
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), NULL);
#endif
	return TRUE;
}

// CAvocadoView drawing
static int ppvX = 0;
static int ppvY = 0;
CRect ppvtarget;
void CAvocadoView::OnDraw(CDC* pDC)
{
	CAvocadoDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;
	
	
	if (pDC->IsPrinting ())
	{
// print preview here... 
#ifndef SHARED_HANDLERS
		  CString tempFileName = theApp.GetAppSessionTempFolder () + CString ("\\printpreview.bmp");
		  CWindowDC dcWindow(theApp.GetMainWnd ());
		 CDC dc;
		 dc.CreateCompatibleDC(&dcWindow);
 
		 CBitmap bmp;
		  bmp.m_hObject=(HBITMAP)::LoadImage(NULL,tempFileName.GetString(),
					  IMAGE_BITMAP,3000,3000,LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_VGACOLOR | LR_LOADTRANSPARENT);
		 CBitmap* pOldBitmap = dc.SelectObject(&bmp);

		pDC->StretchBlt(0, 0, 3000,3000, &dc, 0, 0, 3000, 3000, SRCCOPY);
		dc.SelectObject(pOldBitmap);
#endif
	} 
}


// CAvocadoView printing


void CAvocadoView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
#ifndef _AVOCADO_ACTIVEX
	AFXPrintPreview(this);
#endif
#endif
}

BOOL CAvocadoView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	//m_pInfo=pInfo;
	// 1. Determine the DIB size for either printing or print preview.
	pInfo->SetMaxPage (1);
	ppvtarget = pInfo->m_rectDraw ;
	return DoPreparePrinting(pInfo);
}

void CAvocadoView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rcClient;
	SIZE m_szPage;
	GetClientRect(&rcClient);  
	float fClientRatio = float(rcClient.Height())/rcClient.Width();  
	// Get page size
	m_szPage.cx  = pDC->GetDeviceCaps(HORZRES);
	m_szPage.cy = pDC->GetDeviceCaps(VERTRES);  
	CSize szDIB; 
	if (pInfo->m_bPreview) {
		// Use screen resolution for preview.  
#ifndef SHARED_HANDLERS
		szDIB.cx = rcClient.Width();  szDIB.cy = rcClient.Height(); 
			char ssizx[30];
		char ssizy[30];
		itoa (szDIB.cx,ssizx,10);
		itoa (szDIB.cx,ssizy,10);
		ppvX = szDIB.cx;
		ppvY = szDIB.cy;
	  CString tempFileName = theApp.GetAppSessionTempFolder () + CString ("\\printpreview.bmp");
		   char dst[MAX_PATH];
		   ::wcstombs (dst,tempFileName.GetString (),MAX_PATH);
		  // string fileParam = "string filename="+string (dst) + string (",int x=") + string(ssizx) +string(",int y=") + string (ssizy)+ string(";");
		   string fileParam = "string filename="+string (dst) + string (",int x=300,int y=300,bool forceOptix=1;") ;//+ string (ssizy)+ string(";");
		  
		   GetDocument()->AvocadoInvokeView ("SaveViewToBitmapFile",m_id,fileParam.c_str());
#endif
} else  {  // Use higher resolution for printing.  
	// Adjust size according screen's ratio. 
	if (m_szPage.cy > fClientRatio*m_szPage.cx) 
		{
		 // View area is wider than Printer area  
			 szDIB.cx = m_szPage.cx;   
			szDIB.cy = long(fClientRatio*m_szPage.cx);
		}  
		else  
		{ 
			// View area is narrower than Printer area 
			szDIB.cx = long(float(m_szPage.cy)/fClientRatio);   
			szDIB.cy = m_szPage.cy; 
		}
	// Reduce the Resolution if the Bitmap size is too big. 
	// Ajdust the maximum value, which is depends on printer's memory.  
	// I use 20 MB.  
	while (szDIB.cx*szDIB.cy > 20e6)   
		 {   szDIB.cx = szDIB.cx/2;   szDIB.cy = szDIB.cy/2;  }

		}

	// TODO: add extra initialization before printing
}

void CAvocadoView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}


void CAvocadoView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
#ifndef _AVOCADO_ACTIVEX
	if (GetSelection () > -1)
	{
		// here we need to filter by selection types.
		// refactor all of this crap below into the app class..
		int resId = (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP ? 
					IDR_POPUP_VIEW_SELECTED_ELEM :
					IDR_POPUP_VIEW_SELECTED_ELEM1 );
		theApp.GetContextMenuManager()->ShowPopupMenu(resId/*IDR_POPUP_VIEW_ELEMENTSELECTION*/, point.x, point.y, theApp.GetMainWnd(), TRUE);
	}
	else
	{
		int resId = (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP ? 
					IDR_POPUP_VIEW_DEFAULT :
					IDR_POPUP_VIEW_DEFAULT1 );
		theApp.GetContextMenuManager()->ShowPopupMenu(resId/*IDR_POPUP_VIEW_NOSELECTION*/, point.x, point.y, theApp.GetMainWnd(), TRUE);
	}
#endif
#endif
}

// CAvocadoView diagnostics

#ifdef _DEBUG
void CAvocadoView::AssertValid() const
{
	CView::AssertValid();
}
void CAvocadoView::OnDestroy() 
{
	 GetDocument()->AvocadoInvokeView ("OnDestroy",m_id,"");
	CWnd::OnDestroy();
}
void CAvocadoView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAvocadoDoc* CAvocadoView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAvocadoDoc)));
	return (CAvocadoDoc*)m_pDocument;
}
#endif //_DEBUG

//#define _USE_OPTIX
// CAvocadoView message handlers
int CAvocadoView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
#ifndef SHARED_HANDLERS
	// Create a SimpleScene object, containing a SceniX scene.
	// Create a render context with a standard format (RGBA8, DEPTH24) on our window.
	//HWND hWnd = GetSafeHwnd();
	HWND hWnd = GetSafeHwnd();
	m_id = avocado::OnCreateView((void *)&hWnd,m_viewInterface);	
	CView::SetTimer(1,1,0);

//	if (GetParentFrame () && GetParentFrame ()->GetActiveDocument ())
	
//		CAvocadoDoc* doc = (CAvocadoDoc*) GetParentFrame ()->GetActiveDocument ();
		//m_id = avocado::OnCreateView((void *)&hWnd,m_viewInterface);
//		m_id =doc->AvocadoCreateView((void *)&hWnd,m_viewInterface);
		
		CView::SetTimer(1,1,0);
	
#endif
	return CWnd::OnCreate(lpCreateStruct);
}

// Avoid that GDI is clearing the window background. Would flicker on resize.
BOOL CAvocadoView::OnEraseBkgnd(CDC* pDC) 
{ 
#ifndef SHARED_HANDLERS
	if (!m_pDocument)
		return TRUE;

	if ((GetDocument() && GetDocument()->GetElementsCount()<= 0) || m_id <  0)
			{
				CDC mdc;
				CWindowDC dcWindow(0);

				mdc.CreateCompatibleDC(&dcWindow);
				RECT rect;
			
				GetClientRect(&rect);  

				if (!m_splashBmpInit)
				{
					//m_splashBitmap

					m_splashBitmap.LoadMappedBitmap (IDB_VIEW_SPLASH_LARGE);
					m_splashBmpInit= true;
				}

				mdc.SelectObject(m_splashBitmap);

				BITMAP bm;	
				m_splashBitmap.GetBitmap( &bm );	

				pDC->StretchBlt(0, 0, rect.right,rect.bottom, &mdc, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);

				std::string stat;
				int prog = 0;
				((CAvocadoDoc*)GetDocument())->GetDocumentStatus (stat,prog);
				CBrush brushi;
				brushi.CreateSolidBrush (RGB(0,25,150));
				CBrush brushFull;
				brushFull.CreateSolidBrush (RGB (80,80,80));
				
				RECT dd;
				RECT fullrect;

				if (prog>100)
					prog = 100;
				float fprog = float(prog) / 100.0f;
				
				dd.bottom = (rect.bottom - rect.top) / 2 + 140;
				dd.left = (rect.right-rect.left) / 2 - int (220.0f*fprog);
				dd.right  = (rect.right - rect.left) / 2 + int(220.0f*fprog);
				dd.top = (rect.bottom - rect.top) / 2 + 180;
				fullrect = dd;
				fullrect.left =  (rect.right-rect.left) / 2 - int (220.0f);
				fullrect.right =  (rect.right-rect.left) / 2 + int (220.0f);

				if (prog > 0 && prog < 100)
				{
				//pDC->Draw3dRect (dd.left,dd.top,dd.right,dd.bottom,0x555555,0x888888);
					// draw full rect
					pDC->Draw3dRect (&fullrect,0x555555,0x888888);
					pDC->FillRect (&fullrect,&brushFull);
					//draw progress
					pDC->Draw3dRect (&dd,0x555555,0x888888);
					pDC->FillRect (&dd,&brushi);
				}
				{
					if (m_showProgressText)
					{
						stringstream sstrm(stat);
						sstrm << "progress: " << prog;
						wchar_t progtext [MAX_PATH];
						mbstowcs(progtext,sstrm.str().c_str(),MAX_PATH);
						pDC->DrawText (progtext,CRect(rect.left,rect.top,rect.right,rect.bottom),0);
					}
				}
			} 
#endif
  return TRUE;
}

void CAvocadoView::OnSize(UINT nType, int cx, int cy)
{
	
	if (m_pDocument && m_id > -1)//GetParentFrame () && GetParentFrame ()->GetActiveDocument ())
	{
		//CAvocadoDoc* doc = (CAvocadoDoc*) GetParentFrame ()->GetActiveDocument ();
		GetDocument()->AvocadoResizeView (m_id,cx,cy);
		m_viewSizeX = -1;
		m_viewSizeY = -1;
		return;
	} 
	m_viewSizeX = cx;
	m_viewSizeY = cy;

}
#ifndef _DEBUG
void CAvocadoView::OnDestroy ()
{
//#ifndef SHARED_HANDLERS
	 GetDocument()->AvocadoInvokeView ("OnDestroy",m_id,"");
//#endif
	CView::OnDestroy();
}
#endif

#ifndef SHARED_HANDLERS
void CAvocadoView::SetCurrentSelection(std::vector<int> eids) {
	m_currentSelection = eids[0];
	m_currentSelectionList = eids;
#ifndef SHARED_HANDLERS
	NMHDR pNMHDR; 
	pNMHDR.code = AVC_REFRESH_VIEW_SELECTION;
	pNMHDR.hwndFrom = this->GetSafeHwnd();

	GetParentFrame()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
	//theApp.RefreshViewSelection ();
#endif
}
void CAvocadoView::InvokePaint ()
{
	InvokePaintLow();
	//not used...
	//theApp.SyncRepaint (m_id);
}
void CAvocadoView::InvokePaintLow()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	if (m_animating && m_frameCount < 1)
		return;
	m_frameCount = 0;
	if (m_pDocument && m_id > -1 && GetDocument()->GetElementsCount () > 0)
	{
		if (GetDocument()->GetIsSaving () == false)
		{
			if (m_viewSizeX != -1 || m_viewSizeY!=-1)
				OnSize(0,m_viewSizeX,m_viewSizeY);
			 GetDocument()->AvocadoInvokeView ("OnPaint",m_id,"");
			//avocado::OnSendAvocadoMouseStringMessage(avocado::AVC_ONPAINT,m_id,0,0,0);
			// Show the rendered image.
			SwapBuffers( dc.GetSafeHdc() );
		}
	}
	else
	{
	  OnEraseBkgnd (&dc);
	}
}

void CAvocadoView::OnPaint() 
{
	InvokePaint();
	// Do not call CWnd::OnPaint() for painting messages
}

void CAvocadoView::OnMouseMove(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default

	m_mouseX = point.x;
	m_mouseY = point.y;

	
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_MOVE,m_id, point.x, point.y, nFlags);

	CView::OnMouseMove(nFlags, point);
}


void CAvocadoView::OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// This feature requires Windows Vista or greater.
	// The symbol _WIN32_WINNT must be >= 0x0600.

	// TODO: Add your message handler code here and/or call default
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_WHEEL, m_id, pt.x, pt.y, zDelta);
	CView::OnMouseHWheel(nFlags, zDelta, pt);
}


BOOL CAvocadoView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: Add your message handler code here and/or call default
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_WHEEL, m_id, pt.x, pt.y, zDelta);
	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

void CAvocadoView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_LDOWN,m_id,point.x,point.y, nFlags);
	CView::OnLButtonDown(nFlags, point);
	CView::SetFocus();
}

void CAvocadoView::OnRButtonUp(UINT  nFlags , CPoint point)
{
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_RUP,m_id,point.x,point.y, nFlags);
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAvocadoView::OnLButtonUp(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_LUP,m_id,point.x,point.y, 0);
	CView::OnLButtonUp(nFlags, point);
}

void CAvocadoView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	m_frameCount++;
	if (this->m_pDocument != NULL)
		GetDocument()->AvocadoInvokeUI (avocado::AVC_TIMER_TICK, m_id, 0,0, 0);
	
	if (m_frameCount == 1000)
		m_frameCount = 0;

	CView::OnTimer(nIDEvent);
}

void CAvocadoView::OnTimeChange()
{
	CView::OnTimeChange();
	// TODO: Add your message handler code here
}

void CAvocadoView::OnHide()
{
	// TODO: Add your command handler code here
	 GetDocument()->AvocadoInvokeView ("OnTogHighlights",m_id,"");
}


void CAvocadoView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class
	/*if (bActivate && (pActivateView != pDeactiveView))
	{
		avocado::SetActiveView(((CAvocadoView *)pActivateView)->GetID());
	}*/

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void CAvocadoView::OnCopy()
{
	
	 GetDocument()->AvocadoInvokeView ("OnTogCellShading",m_id,"");
}

void CAvocadoView::OnPaste()
{
	 GetDocument()->AvocadoInvokeView ("OnTogHLR",m_id,"");
}

void CAvocadoView::OnMButtonDown(UINT nFlags, CPoint point)
{
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_MDOWN, m_id, point.x,point.y, nFlags);
	CView::OnMButtonDown(nFlags, point);
}

void CAvocadoView::OnMButtonUp(UINT nFlags, CPoint point)
{
	GetDocument()->AvocadoInvokeUI (avocado::AVC_MOUSE_MUP, m_id, point.x,point.y, nFlags);
	CView::OnMButtonUp(nFlags, point);
}

void CAvocadoView::OnInitialUpdate()
{
	CView::OnInitialUpdate();
	
	if (m_pDocument == NULL && this->GetParentFrame ()->GetActiveDocument () )
	{
		GetParentFrame ()->GetActiveDocument ()->AddView (this);
		
	}
	if (m_id == -1)
	{
//		OnEraseBkgnd (GetDC());
//			HWND hWnd = GetSafeHwnd();
//		m_id = avocado::OnCreateView((void *)&hWnd,m_viewInterface);	
//		CView::SetTimer(1,1,0);
		
	}
}



void CAvocadoView::OnPlay()
{
	 GetDocument()->AvocadoInvokeView ("OnPlayAnimation",m_id,"");
	m_animating = true;
}


void CAvocadoView::OnStop()
{
	 GetDocument()->AvocadoInvokeView ("OnStopAnimation",m_id,"");
	m_animating = false;
}


void CAvocadoView::OnPause()
{
	 GetDocument()->AvocadoInvokeView ("OnPauseAnimation",m_id,"");
	m_animating = !m_animating;
}


void CAvocadoView::OnTimertog()
{

}

void CAvocadoView::OnLayoutGalery()
{
	
}

int CAvocadoView::GetSelection ()
{
	return m_currentSelection;
}

int CAvocadoView::GetSelection (int i)
{
	if (i>=0 && i<GetSelectionCount())
	return m_currentSelectionList[i];
	else return -1;
}

int CAvocadoView::GetSelectionCount ()
{
	return int( m_currentSelectionList.size() );
}

void CAvocadoView::OnUpdateLayoutGalery(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(true);
}

bool CAvocadoView::HasSelection ()
{
    bool hasSelection = ( (m_currentSelection != -1) ? true : false );
	return hasSelection;
}
#endif // SHARED_HANDLERS

void CAvocadoView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo)
{
	CView::OnPrepareDC(pDC, pInfo);

	pDC->SetMapMode(MM_ANISOTROPIC);
}

void CAvocadoView::OnEditCopy()
{

}



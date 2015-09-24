
// IpFrame.cpp : implementation of the CInPlaceFrame class
//

#include "stdafx.h"
#include "Avocado.h"
#include "IpFrame.h"
#include "AvocadoDoc.h"
#include "AvocadoView.h"
#include <sstream>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CInPlaceFrame
static UINT BASED_CODE buttons[] =
{
	// same order as in the bitmap 'toolbar.bmp'
	ID_LAYOUT_NEXTPAGE,
	ID_LAYOUT_BACKPAGE,
	ID_FILE_IMPORT2,
	ID_MODEL_ROTATE,
	ID_MODEL_MOVE,
	ID_GRAPHICS_RAYTRACING,
};

IMPLEMENT_DYNCREATE(CInPlaceFrame, COleDocIPFrameWndEx)

BEGIN_MESSAGE_MAP(CInPlaceFrame, COleDocIPFrameWndEx)
	ON_WM_CREATE()
//	ON_UPDATE_COMMAND_UI(ID_GRAPHICS_RAYTRACING, &CInPlaceFrame::OnUpdateGraphicsRaytracing)
//	ON_COMMAND(ID_GRAPHICS_RAYTRACING, &CInPlaceFrame::OnGraphicsRaytracing)
ON_COMMAND(ID_LAYOUT_BACKPAGE, &CInPlaceFrame::OnLayoutBackpage)
ON_COMMAND(ID_LAYOUT_NEXTPAGE, &CInPlaceFrame::OnLayoutNextpage)
ON_COMMAND(ID_MODEL_MOVE, &CInPlaceFrame::OnModelMove)
ON_UPDATE_COMMAND_UI(ID_MODEL_MOVE, &CInPlaceFrame::OnUpdateModelMove)
ON_COMMAND(ID_MODEL_ROTATE, &CInPlaceFrame::OnModelRotate)
ON_UPDATE_COMMAND_UI(ID_MODEL_ROTATE, &CInPlaceFrame::OnUpdateModelRotate)
ON_COMMAND(ID_MODEL_SCALE, &CInPlaceFrame::OnModelScale)
ON_UPDATE_COMMAND_UI(ID_MODEL_SCALE, &CInPlaceFrame::OnUpdateModelScale)
ON_COMMAND(ID_GRAPHICS_RAYTRACING, &CInPlaceFrame::OnGraphicsRaytracing)
END_MESSAGE_MAP()


// CInPlaceFrame construction/destruction

CInPlaceFrame::CInPlaceFrame()
{
}

CInPlaceFrame::~CInPlaceFrame()
{
}
BOOL CInPlaceFrame::OnCreateControlBars(CFrameWnd* pWndFrame,
	CFrameWnd* pWndDoc)
{
#if 0
	m_wndToolBar.SetOwner(this);
	// create toolbar on client's frame window
	if (!m_wndToolBar.CreateEx(pWndFrame, TBSTYLE_FLAT,WS_CHILD | WS_VISIBLE | CBRS_TOP
           | CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
          !m_wndToolBar.LoadToolBar(IDR_TOOLBAR_INPLACE))
       {
          TRACE0("Failed to create toolbar\n");
           return FALSE;
    }
 


	// set owner to this window, so messages are delivered to correct app
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);
	pWndFrame->EnableDocking(CBRS_ALIGN_ANY);
	pWndFrame->DockControlBar(&m_wndToolBar);
#endif
#if 0
//#if 0
	m_wndRibbonBar.Create(this);
	{
		// Authoring ribbon..
		m_wndRibbonBar.LoadFromResource(IDR_RIBBON1);
			// set owner to this window, so messages are delivered to correct app
		//	m_wndRibbonBar.EnableDocking(CBRS_ALIGN_ANY);
			//m_wndRibbonBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_SIZE_DYNAMIC);
			//pWndFrame->EnableDocking(CBRS_ALIGN_ANY);
			//pWndFrame->AddControlBar (&m_wndRibbonBar);
	//	m_wndRibbonBar.Attach (pWndFrame->GetSafeHwnd ());
			//m_wndRibbonBar.SetOwner(this);
	}
#endif
	return (COleDocIPFrameWndEx::OnCreateControlBars(pWndFrame,pWndDoc));
}
int CInPlaceFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (COleDocIPFrameWndEx::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// CResizeBar implements in-place resizing.
	if (!m_wndResizeBar.Create(this))
	{
		TRACE0("Failed to create resize bar\n");
		return -1;      // fail to create
	}

	// By default, it is a good idea to register a drop-target that does
	//  nothing with your frame window.  This prevents drops from
	//  "falling through" to a container that supports drag-drop.
	m_dropTarget.Register(this);

	return 0;
}

BOOL CInPlaceFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying the CREATESTRUCT cs

	return COleDocIPFrameWndEx::PreCreateWindow(cs);
}


// CInPlaceFrame diagnostics

#ifdef _DEBUG
void CInPlaceFrame::AssertValid() const
{
	COleDocIPFrameWndEx::AssertValid();
}

void CInPlaceFrame::Dump(CDumpContext& dc) const
{
	COleDocIPFrameWndEx::Dump(dc);
}
#endif //_DEBUG


// CInPlaceFrame commands



void CInPlaceFrame::ActivateFrame(int nCmdShow)
{
	// TODO: Add your specialized code here and/or call the base class
	
	COleDocIPFrameWndEx::ActivateFrame(nCmdShow);
}

//
//int CInPlaceFrame::GetActiveViewIntParam (const char* param)
//{
//	// All view related update commands should be moved to view..
//	int paramValue = 0;
//	if (GetActiveView ())
//	{
//		 paramValue = (((CAvocadoView*)GetActiveAvocadoView())->GetViewParameterAsInt (param));
//	}
//	return paramValue;
//}
//
//int CInPlaceFrame::GetActiveViewID ()
//{
//	// All view related update commands should be moved to view..
//	int vid = -1;
//	if (GetActiveView ())
//	{
//		 vid = (((CAvocadoView*)GetActiveAvocadoView())->GetID());// (param));
//	}
//	return vid;
//}
//CView* CInPlaceFrame::GetActiveAvocadoView ()
//{
//	if (GetActiveView())
//		return ((CView*)GetActiveView ());
//	return false;
//}

//void CInPlaceFrame::OnUpdateGraphicsRaytracing(CCmdUI *pCmdUI)
//{
//	bool isOptixSupported = true;//(GetActiveViewIntParam("supportOptix") == 1);
//	if (isOptixSupported)
//	{
//	bool isUsingOptix = 0;//(GetActiveViewIntParam("UseOptix") == 1);
//	pCmdUI->m_nIndex = isUsingOptix;
//	pCmdUI->SetCheck(isUsingOptix);
//	}
//	pCmdUI->Enable(isOptixSupported);
//}


//void CInPlaceFrame::OnGraphicsRaytracing()
//{
//	if (GetActiveViewIntParam("UseOptix") == 1)
//		avocado::OnSendAvocadoGeneralStringMessage("SetViewParam",GetActiveViewID (),string("string UseOptix=0;"));
//	else
//		avocado::OnSendAvocadoGeneralStringMessage("SetViewParam",GetActiveViewID (),string("string UseOptix=1;"));
//
//}


int CInPlaceFrame::GetActiveViewIntParam (const char* param)
{
	// All view related update commands should be moved to view..
	int paramValue = 0;
	if (GetActiveView ())
	{
		 paramValue = (GetActiveAvocadoView()->GetViewParameterAsInt (param));
	}
	return paramValue;
}
CAvocadoDoc* CInPlaceFrame::GetActiveAvocadoDoc()
{
	if (GetActiveDocument())
		return ((CAvocadoDoc*)GetActiveDocument ());
	else
	{
		
	}
	return false;
}

void CInPlaceFrame::OnGraphicsRaytracing()
{
	if (GetActiveViewIntParam("UseOptix") == 1)
		GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string UseOptix=0;");
	else
		GetActiveAvocadoDoc()->AvocadoInvokeView("SetViewParam",GetActiveViewID (),"string UseOptix=1;");
}

int CInPlaceFrame::GetActiveViewID ()
{
	if (GetActiveAvocadoView())
		return 	GetActiveAvocadoView()->GetID();
	return -1;
}
CAvocadoView* CInPlaceFrame::GetActiveAvocadoView ()
{
	if (GetActiveView())
		return ((CAvocadoView*)GetActiveView ());
	return false;
}

void CInPlaceFrame::OnLayoutBackpage()
{
	int viewID = GetActiveAvocadoView()->GetID ();
	if (viewID != -1)
	{
		//OnHtmlEditorSave();
		string pr;
		std::stringstream pStr;
		pStr <<  "int vid=" << viewID
			<< ";";
		avocado::OnSendAvocadoDocGeneralStringMessage("BackPage",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}
}

void CInPlaceFrame::ToggleViewFilter (const char* param , unsigned int filt)
{
	unsigned int df = GetActiveAvocadoView()->GetViewParameterAsInt (param);

	df =  df ^ filt;
	std::stringstream idStr;
	idStr << df;
	GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetViewParam",GetActiveViewID (),string (string("string ")+param+string ("=")+idStr.str()+string(";")).c_str());
	
	//GetActiveAvocadoView()->SetViewParameter (param,idStr );
}
void CInPlaceFrame::OnUpdateModelRotate(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ActiveViewHasSelection ());
	pCmdUI->SetCheck(GetActiveViewFilter("DraggerFilters",MODEL_ROTATE));
}
void CInPlaceFrame::OnUpdateModelScale(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ActiveViewHasSelection ());
	pCmdUI->SetCheck(GetActiveViewFilter("DraggerFilters",MODEL_SCALE));
}

bool CInPlaceFrame::GetActiveViewFilter (const char* param,unsigned int filt)
{
	if (GetActiveView ())
	{
	unsigned int df = GetActiveAvocadoView()->GetViewParameterAsInt(param);
	if ((df & filt) == filt)
		return true;
	}
	return false;
}


void CInPlaceFrame::OnModelScale()
{
//	bool newVal = (!IsActiveViewOrthographic());
	ToggleViewFilter("DraggerFilters",MODEL_SCALE);
	
}


void CInPlaceFrame::OnLayoutNextpage()
{
	int viewID = GetActiveAvocadoView()->GetID ();
	if (viewID != -1)
	{
		//OnHtmlEditorSave();
		string pr;
		std::stringstream pStr;
		pStr <<  "int vid=" << viewID
			<< ";";
		GetActiveAvocadoDoc()->AvocadoInvokeDoc ("NextPage",GetActiveAvocadoDoc ()->GetAvoID (),pStr.str ().c_str());
	}
}
void CInPlaceFrame::OnModelMove()
{
	ToggleViewFilter("DraggerFilters",MODEL_MOVE);
	
}

bool CInPlaceFrame::ActiveViewHasSelection ()
{
	if (GetActiveAvocadoView())
	{
		return GetActiveAvocadoView()->HasSelection ();
	}
	return false;
}

void CInPlaceFrame::OnUpdateModelMove(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(ActiveViewHasSelection ());
	pCmdUI->SetCheck(GetActiveViewFilter("DraggerFilters",MODEL_MOVE));
	
}


void CInPlaceFrame::OnModelRotate()
{
	ToggleViewFilter("DraggerFilters",MODEL_ROTATE);
}

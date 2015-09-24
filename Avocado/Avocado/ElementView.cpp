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
#include "MainFrm.h"
#include "ElementView.h"
#include "Resource.h"
#include "Avocado.h"

class CElementViewMenuButton : public CMFCToolBarMenuButton
{
	friend class CElementView;

	DECLARE_SERIAL(CElementViewMenuButton)

public:
	CElementViewMenuButton(HMENU hMenu = NULL) : CMFCToolBarMenuButton((UINT)-1, hMenu, -1)
	{
	}

	virtual void OnDraw(CDC* pDC, const CRect& rect, CMFCToolBarImages* pImages, BOOL bHorz = TRUE,
		BOOL bCustomizeMode = FALSE, BOOL bHighlight = FALSE, BOOL bDrawBorder = TRUE, BOOL bGrayDisabledButtons = TRUE)
	{
		pImages = CMFCToolBar::GetImages();

		CAfxDrawState ds;
		pImages->PrepareDrawImage(ds);

		CMFCToolBarMenuButton::OnDraw(pDC, rect, pImages, bHorz, bCustomizeMode, bHighlight, bDrawBorder, bGrayDisabledButtons);

		pImages->EndDrawImage(ds);
	}
};

IMPLEMENT_SERIAL(CElementViewMenuButton, CMFCToolBarMenuButton, 1)

	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

	CElementView::CElementView()
{
	m_nCurrSort = ID_SORTING_GROUPBYTYPE;
	m_selected = -1;
	
}

CElementView::~CElementView()
{
}

BEGIN_MESSAGE_MAP(CElementView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_CLASS_ADD_MEMBER_FUNCTION, OnClassAddMemberFunction)
	ON_COMMAND(ID_CLASS_ADD_MEMBER_VARIABLE, OnClassAddMemberVariable)
	ON_COMMAND(ID_CLASS_DEFINITION, OnClassDefinition)
	ON_COMMAND(ID_CLASS_PROPERTIES, OnClassProperties)
	ON_COMMAND(ID_NEW_FOLDER, OnNewFolder)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_COMMAND_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnSort)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SORTING_GROUPBYTYPE, ID_SORTING_SORTBYACCESS, OnUpdateSort)
	//	ON_UPDATE_COMMAND_UI(ID_APPREANCE_HIDE, &CElementView::OnUpdateAppreanceHide)
	//	ON_UPDATE_COMMAND_UI(ID_APPREANCE_UNHIDEALL, &CElementView::OnUpdateAppreanceUnhideall)
	//ON_COMMAND(ID_APPREANCE_HIDE, &CElementView::OnAppreanceHide)
	ON_UPDATE_COMMAND_UI(ID_APPREANCE_HIDE, &CElementView::OnUpdateAppreanceHide)
	//ON_COMMAND(ID_APPREANCE_UNHIDEALL, &CElementView::OnAppreanceUnhideall)
	ON_UPDATE_COMMAND_UI(ID_APPREANCE_UNHIDEALL, &CElementView::OnUpdateAppreanceUnhideall)
	ON_UPDATE_COMMAND_UI(ID_APPERANCE_UNHIDE, &CElementView::OnUpdateApperanceUnhide)
	ON_UPDATE_COMMAND_UI(ID_SPLIT_ELEMENT, &CElementView::OnUpdateSplitElement)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CElementView message handlers

int CElementView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create views:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | TVS_HASLINES /*| TVS_LINESATROOT */| TVS_HASBUTTONS | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | TVS_SHOWSELALWAYS | TVS_TRACKSELECT | TVS_EDITLABELS;

	if (!m_wndClassView.Create(dwViewStyle, rectDummy, this, 2))
	{
		TRACE0("Failed to create Class View\n");
		return -1;      // fail to create
	}

	// Load images:
	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_SORT);
	m_wndToolBar.LoadToolBar(IDR_SORT, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);
	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	for (int ii=m_wndToolBar.GetCount() - 1;ii > 0;ii--)
	{
		if (theApp.GetAppMode() == CAvocadoApp::VIEWER_APP)
		{
			m_wndToolBar.RemoveButton (ii);
		}
		//CMFCToolBarButton* pButton =  DYNAMIC_DOWNCAST(CMFCToolBarButton, m_wndToolBar.GetButton(ii));
		//if (pButton != NULL)
		{ 
			//pButton->setpaSetMessageWnd(theApp.GetMainWnd());
		}
	}
	CMenu menuSort;
	menuSort.LoadMenu(IDR_POPUP_SORT);

	m_wndToolBar.ReplaceButton(ID_SORT_MENU, CElementViewMenuButton(menuSort.GetSubMenu(0)->GetSafeHmenu()));

	CElementViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CElementViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->m_bText = FALSE;
		pButton->m_bImage = TRUE;
		pButton->SetImage(GetCmdMgr()->GetCmdImage(m_nCurrSort));
		pButton->SetMessageWnd(this);
	}

	// Fill in some static tree view data (dummy code, nothing magic here)
	FillClassView();

	return 0;
}

void CElementView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}
void CElementView::ClearList ()
{
	m_wndClassView.DeleteAllItems ();
	HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Document"), 0, 0);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	m_list.clear();
}
bool CElementView::AddElement (std::string name, int id , bool visible,int parentID)
{
	int iconIdx = 0;
	HTREEITEM hRoot = NULL;
	if (parentID < 0)
	{
		hRoot= m_wndClassView.GetRootItem();
	}
	else
	{
		for (size_t k=0;k<m_list.size();k++)
		{
			pair <int,HTREEITEM> par;
			par = m_list[k];
			if (par.first == parentID)
			{
				hRoot = par.second;
				iconIdx = 2;
				break;
			}
		}
		if (hRoot == NULL)
			// not good.. 
			//hRoot= m_wndClassView.GetRootItem();
			return false;
	}

	TCHAR wname[MAX_PATH];
#ifdef UNICODE
	std::mbstowcs (wname,name.c_str(),MAX_PATH);
#else
	::strcpy (wname,name.c_str ());
#endif
	HTREEITEM hClass = (visible ? m_wndClassView.InsertItem(wname, 1+iconIdx, 1+iconIdx, hRoot) :
		m_wndClassView.InsertItem(wname, 2+iconIdx, 2+iconIdx, hRoot));

	/*m_wndClassView.InsertItem(_T("Material"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Type"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Position"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("Metadata"), 3, 3, hClass);*/
	m_wndClassView.Expand(hRoot, TVE_EXPAND);

	m_list.push_back (pair<int,HTREEITEM> (id,hClass));
	return true;
}
void CElementView::HighlightElement (int sel)
{
	m_selected = sel;
	for (size_t i=0; i<m_list.size();i++)
	{
		HTREEITEM hClass = m_list[i].second;
		if (m_list[i].first == sel )//&& hClass != m_wndClassView.GetSelectedItem())
		{
			//	m_wndClassView.SelectItem(0);
				m_wndClassView.SetItemState(hClass, TVIS_SELECTED, TVIS_SELECTED);
			//m_wndClassView.SelectItem(hClass);
			//m_wndClassView.Select(hClass,TVGN_CARET);
			//m_selected =  m_list[i].first;
		}
		else
		{
			//m_wndClassView.Select(hClass,0);
			m_wndClassView.SetItemState(hClass, 0, TVIS_SELECTED);
		}
	}
}

void CElementView::HighlightElements (std::vector<int> sel)
{
	if (sel.size()>0)
	{
		m_selected = sel[0];
	}
	for (size_t i=0; i<m_list.size();i++)
	{
		HTREEITEM hClass = m_list[i].second;
		bool currentIsSelected = false;
		for (int selIdx = 0;selIdx < int(sel.size());selIdx++)
		{
		if (m_list[i].first == sel[selIdx] )//&& hClass != m_wndClassView.GetSelectedItem())
			currentIsSelected = true;
		}
		if (currentIsSelected)
			{
				//	m_wndClassView.SelectItem(0);
					m_wndClassView.SetItemState(hClass, TVIS_SELECTED, TVIS_SELECTED);
				//m_wndClassView.SelectItem(hClass);
				//m_wndClassView.Select(hClass,TVGN_CARET);
				//m_selected =  m_list[i].first;
			}
			else
			{
				//m_wndClassView.Select(hClass,0);
				m_wndClassView.SetItemState(hClass, 0, TVIS_SELECTED);
			}
	}
	
}
int CElementView::GetSelectedID ()
{
	int res = -1;
	HTREEITEM sel = m_wndClassView.GetSelectedItem ();
	if (sel)
	{
		for (size_t i=0; i<m_list.size();i++)
		{
			HTREEITEM hClass = m_list[i].second;
			if (hClass == sel)
			{
				//	m_wndClassView.SetItemState(hClass, TVIS_SELECTED, TVIS_SELECTED);
				res = m_list[i].first;
				m_selected =  m_list[i].first;
				break;
			}
		}
	}

	return res;
}
void CElementView::FillClassView()
{
	HTREEITEM hRoot = m_wndClassView.InsertItem(_T("Document"), 0, 0);
	m_wndClassView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);
	/*	
	HTREEITEM hClass = m_wndClassView.InsertItem(_T("CFakeAboutDlg"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAboutDlg()"), 3, 3, hClass);

	m_wndClassView.Expand(hRoot, TVE_EXPAND);

	hClass = m_wndClassView.InsertItem(_T("CFakeApp"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeApp()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("InitInstance()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnAppAbout()"), 3, 3, hClass);

	hClass = m_wndClassView.InsertItem(_T("CFakeAppDoc"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAppDoc()"), 4, 4, hClass);
	m_wndClassView.InsertItem(_T("~CFakeAppDoc()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("OnNewDocument()"), 3, 3, hClass);

	hClass = m_wndClassView.InsertItem(_T("CFakeAppView"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAppView()"), 4, 4, hClass);
	m_wndClassView.InsertItem(_T("~CFakeAppView()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("GetDocument()"), 3, 3, hClass);
	m_wndClassView.Expand(hClass, TVE_EXPAND);

	hClass = m_wndClassView.InsertItem(_T("CFakeAppFrame"), 1, 1, hRoot);
	m_wndClassView.InsertItem(_T("CFakeAppFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("~CFakeAppFrame()"), 3, 3, hClass);
	m_wndClassView.InsertItem(_T("m_wndMenuBar"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndToolBar"), 6, 6, hClass);
	m_wndClassView.InsertItem(_T("m_wndStatusBar"), 6, 6, hClass);

	hClass = m_wndClassView.InsertItem(_T("Globals"), 2, 2, hRoot);
	m_wndClassView.InsertItem(_T("theFakeApp"), 5, 5, hClass);
	m_wndClassView.Expand(hClass, TVE_EXPAND);
	*/
}

void CElementView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*)&m_wndClassView;
	ASSERT_VALID(pWndTree);

	if (pWnd != pWndTree)
	{
		CDockablePane::OnContextMenu(pWnd, point);

		return;
	}

	if (point != CPoint(-1, -1))
	{
		// Select clicked item:
		CPoint ptTree = point;
		pWndTree->ScreenToClient(&ptTree);

		UINT flags = 0;
		HTREEITEM hTreeItem = pWndTree->HitTest(ptTree, &flags);
		if (hTreeItem != NULL)
		{
			//pWndTree->SelectItem(hTreeItem);
			pWndTree->Select(hTreeItem,TVGN_CARET);
		}

	}
	pWndTree->SetFocus();
	int resId = (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP ? 
IDR_POPUP_SORT :
	IDR_POPUP_SORT1 );
	theApp.GetContextMenuManager()->ShowPopupMenu(resId/*IDR_POPUP_VIEW_NOSELECTION*/, point.x, point.y, theApp.GetMainWnd(), TRUE);
	//

	//UpdateDialogControls(this, FALSE);
	/*CMenu menu;
	menu.LoadMenu(IDR_POPUP_SORT);

	CMenu* pSumMenu = menu.GetSubMenu(0);

	if (AfxGetMainWnd()->IsKindOf(RUNTIME_CLASS(CMDIFrameWndEx)))
	{
	CMFCPopupMenu* pPopupMenu = new CMFCPopupMenu;

	if (!pPopupMenu->Create(this, point.x, point.y, (HMENU)pSumMenu->m_hMenu, FALSE, TRUE))
	return;

	((CMDIFrameWndEx*)AfxGetMainWnd())->OnShowPopupMenu(pPopupMenu);
	UpdateDialogControls(this, FALSE);
	}*/
}

void CElementView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	m_wndClassView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1, rectClient.Width() - 2, rectClient.Height() - cyTlb - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

BOOL CElementView::PreTranslateMessage(MSG* pMsg)
{
	return CDockablePane::PreTranslateMessage(pMsg);
}

void CElementView::OnSort(UINT id)
{
	if (m_nCurrSort == id)
	{
		return;
	}

	m_nCurrSort = id;

	CElementViewMenuButton* pButton =  DYNAMIC_DOWNCAST(CElementViewMenuButton, m_wndToolBar.GetButton(0));

	if (pButton != NULL)
	{
		pButton->SetImage(GetCmdMgr()->GetCmdImage(id));
		m_wndToolBar.Invalidate();
		m_wndToolBar.UpdateWindow();
	}
}

void CElementView::OnUpdateSort(CCmdUI* pCmdUI)
{
	pCmdUI->SetCheck(pCmdUI->m_nID == m_nCurrSort);
}

void CElementView::OnClassAddMemberFunction()
{
	AfxMessageBox(_T("Add member function..."));
}

void CElementView::OnClassAddMemberVariable()
{
	// TODO: Add your command handler code here
}

void CElementView::OnClassDefinition()
{
	// TODO: Add your command handler code here
}

void CElementView::OnClassProperties()
{
	// TODO: Add your command handler code here
}

void CElementView::OnNewFolder()
{
	AfxMessageBox(_T("New Folder..."));
}
CString CElementView::GetLastString ()
{
	return m_wndClassView.m_lastString;
}
void CElementView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndClassView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CElementView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndClassView.SetFocus();
}

void CElementView::OnChangeVisualStyle()
{
	m_ClassViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_CLASS_VIEW_24 : IDB_CLASS_VIEW;

	CBitmap bmp;
	if (!bmp.LoadBitmap(uiBmpId))
	{
		TRACE(_T("Can't load bitmap: %x\n"), uiBmpId);
		ASSERT(FALSE);
		return;
	}

	BITMAP bmpObj;
	bmp.GetBitmap(&bmpObj);

	UINT nFlags = ILC_MASK;

	nFlags |= (theApp.m_bHiColorIcons) ? ILC_COLOR24 : ILC_COLOR4;

	m_ClassViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_ClassViewImages.Add(&bmp, RGB(255, 0, 0));

	m_wndClassView.SetImageList(&m_ClassViewImages, TVSIL_NORMAL);

	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_SORT_24 : IDR_SORT, 0, 0, TRUE /* Locked */);
}



//BOOL CElementView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	NMHDR *pNMHDR  = (NMHDR*)lParam;
//	if (pNMHDR->code == TVN_SELCHANGED)
//	{
//		//m_wndClassView.getob
//		//return true;
//	}
//
//	return CDockablePane::OnNotify(wParam, lParam, pResult);
//}


//BOOL CElementView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
//{
//	// TODO: Add your specialized code here and/or call the base class
//	NMHDR *pNMHDR  = (NMHDR*)lParam;
//	if (pNMHDR->code == TVN_SELCHANGED)
//	return CDockablePane::OnNotify(wParam, lParam, pResult);
//}


//void CElementView::OnUpdateAppreanceHide(CCmdUI *pCmdUI)
//{
//	// TODO: Add your command update UI handler code here
//}


//void CElementView::OnUpdateAppreanceUnhideall(CCmdUI *pCmdUI)
//{
//	// TODO: Add your command update UI handler code here
//}


//void CElementView::OnAppreanceHide()
//{
//	//theApp.GetMainWnd ()->SendMessage(WM_COMMAND,;
//	// TODO: Add your command handler code here
//}


void CElementView::OnUpdateAppreanceHide(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable ((m_selected > -1));
}


//void CElementView::OnAppreanceUnhideall()
//{
//	// TODO: Add your command handler code here
//}


void CElementView::OnUpdateAppreanceUnhideall(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (true);
	// TODO: Add your command update UI handler code here
}


void CElementView::OnUpdateApperanceUnhide(CCmdUI *pCmdUI)
{
	pCmdUI->Enable ((m_selected > -1));
	// TODO: Add your command update UI handler code here
}


void CElementView::OnUpdateSplitElement(CCmdUI *pCmdUI)
{
	pCmdUI->Enable ((m_selected > -1));
	// TODO: Add your command update UI handler code here
}

bool CElementView::GetLastShift ()
{
	if (m_wndClassView)
	return m_wndClassView.m_lastShift;
	return false;
}

void CElementView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	/*if ((nFlags & MK_SHIFT ) == MK_SHIFT)
		m_lastShiftState = true;
	else 
		m_lastShiftState = false;*/
	CDockablePane::OnLButtonDown(nFlags, point);
}

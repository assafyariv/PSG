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
#include "mainfrm.h"
#include "ShellView.h"
#include "Resource.h"
#include "Avocado.h"
#include "AvocadoPathUtils.h"
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileView

CShellView::CShellView()
{
}

CShellView::~CShellView()
{
}

BEGIN_MESSAGE_MAP(CShellView, CDockablePane)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_PROPERTIES, OnProperties)
	ON_COMMAND(ID_OPEN, OnFileOpen)
	ON_COMMAND(ID_OPEN_WITH, OnFileOpenWith)
	ON_COMMAND(ID_DUMMY_COMPILE, OnDummyCompile)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CLEAR, OnEditClear)
	ON_WM_PAINT()
	ON_WM_SETFOCUS()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers
class MyFilter : public IFolderFilter
{
private:
	ULONG mRef;
public:
	MyFilter()
		: mRef(0)
	{
	}
	HRESULT __stdcall QueryInterface(/*const*/ REFIID riid, void** ppv)
	{
		if( !ppv )
			return E_POINTER;
		*ppv = NULL;
		if( riid == IID_IUnknown )
			*ppv = (IUnknown*) this;
		else if( riid == IID_IFolderFilter )
			*ppv = (IFolderFilter*) this;
		if( *ppv == NULL )
			return E_NOINTERFACE;
		AddRef();
		return S_OK;
	}
	ULONG __stdcall AddRef()
	{
		return ++mRef;
	}
	ULONG __stdcall Release()
	{
		if( --mRef>0 )
			return mRef;
		delete this;
		return 0;
	}
	HRESULT __stdcall GetEnumFlags(IShellFolder *psf,
		PCIDLIST_ABSOLUTE pidlFolder, HWND *phwnd, DWORD *pgrfFlags)
	{
		*pgrfFlags = SHCONTF_FOLDERS;
		return S_OK;
	}
	HRESULT __stdcall ShouldShow(IShellFolder *psf,
		PCIDLIST_ABSOLUTE pidlFolder, PCUITEMID_CHILD pidlItem)
	{
		//ULONG attrs = 0;

		//psf->GetAttributesOf(1, &pidlFolder, &attrs);
		//if( (attrs & SFGAO_FILESYSTEM) && (attrs &
		//	SFGAO_REMOVABLE) )
		IShellItem *psi;

		HRESULT hr = SHCreateItemWithParent(NULL, psf, pidlItem, IID_PPV_ARGS(&psi));
		if (SUCCEEDED(hr))
		{
			// Determine here whether the item should be shown. This determination
			// is application-dependent.
			LPWSTR displName;
			psi->GetDisplayName (SIGDN_NORMALDISPLAY,&displName);
			CString cname (displName);
			if (cname.Find (CString (L".avc")) > 0)
			{
				return S_OK;
			}
			psi->Release();
		}
		return S_FALSE;
		//return hr;

		//return S_OK;
		//return S_FALSE;
	}
};
int CShellView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();
#if 0
	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE | LVS_ICON;

	if (!m_wndShellView.Create(dwViewStyle, rectDummy, this, 4))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}


	// Load view images:
	m_FileViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	//m_wndShellView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);

	//m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE, IDR_EXPLORER);
	//m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	//m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	//m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	//m_wndToolBar.SetRouteCommandsViaFrame(FALSE);
	//m_wndTool
	// Fill in some static tree view data (dummy code, nothing magic here)
	//FillShellView();
	m_wndShellView.SetView (LV_VIEW_MAX);
	//m_wndShellView.SetExtendedStyle ();
	m_wndShellView.DisplayFolder (L"C:\\Education\\Scenix72\\media\\scenes\\simple\\");
	AdjustLayout();

#endif
	HRESULT hr = SHCoCreateInstance(NULL, &CLSID_ExplorerBrowser, NULL, 
		IID_PPV_ARGS(&m_spExplorerBrowser));

	if (SUCCEEDED(hr))
	{
		// if (theApp.ShowFrames())
		//    m_spExplorerBrowser->SetOptions(EBO_SHOWFRAMES);

		FOLDERSETTINGS fs;
		fs.fFlags = FWF_AUTOARRANGE | FWF_NOWEBVIEW | FWF_HIDEFILENAMES | FWF_EXTENDEDTILES | FWF_NOSUBFOLDERS | FWF_NOFILTERS | FWF_NOCOLUMNHEADER;

		fs.ViewMode = FVM_THUMBNAIL;
		hr = m_spExplorerBrowser->Initialize(m_hWnd, CRect(0, 0, 0, 0), &fs);

		IFolderFilterSite *pSite = NULL;
		m_spExplorerBrowser->QueryInterface(IID_IFolderFilterSite, (void**)&pSite);
		MyFilter *filter = new MyFilter;
		if( FAILED(pSite->SetFilter(filter)) )
			delete filter;
		pSite->Release();

		CString mycats = CAvocadoPathUtils::GetMyDocumentsPath (_T("My Catalogs"));
		//m_spExplorerBrowser.QueryInterface ();
#ifndef UNICODE
		// convert URLs to unicode..
	    wchar_t urlBaseChar [MAX_PATH];
		::mbstowcs (urlBaseChar,mycats.GetString (),MAX_PATH);
		NavigateTo (urlBaseChar);
#else
		NavigateTo (mycats.GetString());
#endif
		//extra large shit..
		// get applied view mode
		//FOLDERSETTINGS fs;

		IFolderView2* pfv2;
		hr = m_spExplorerBrowser->GetCurrentView(IID_PPV_ARGS(&pfv2));
		if (SUCCEEDED(hr))
		{
			//pfv2->SetTileViewProperties();
			pfv2->SetViewModeAndIconSize (FVM_THUMBNAIL,150);
		}
		//pFSite->Release ();
		//FOLDERLOGICALVIEWMODE plvm;

		//m_spExplorerBrowser.QueryInterface (GetCurrentInfo(&fs);
		// if mode viw differ
		//if (fs.ViewMode!=this->FolderSettings.ViewMode)
		//pFSite->GetViewMode(&plvm);
		//if (plvm != 0)
		{

			//		 FOLDERSETTINGS ifs;
			//       ifs.fFlags =  FWF_NOWEBVIEW | FWF_HIDEFILENAMES | /*FWF_EXTENDEDTILES |*/ FWF_NOSUBFOLDERS | FWF_NOFILTERS | FWF_NOCOLUMNHEADER;

			//        ifs.ViewMode = FVM_TILE;
			//		m_spExplorerBrowser->SetFolderSettings (&ifs);

		}

	}

	return SUCCEEDED(hr) ? 0 : -1;




	return 0;
}
bool CShellView::NavigateTo(LPCWSTR szPath)
{
	HRESULT hr = S_OK;
	LPITEMIDLIST pidlBrowse;

	if (FAILED(hr = SHParseDisplayName(szPath, NULL, &pidlBrowse, 0, NULL)))
	{
		ATLTRACE("SHParseDisplayName Failed! hr = %d\n", hr);
		return false;
	}

	if (FAILED(hr = m_spExplorerBrowser->BrowseToIDList(pidlBrowse, 0)))
		ATLTRACE("BrowseToIDList Failed! hr = %d\n", hr);

	ILFree(pidlBrowse);

	return SUCCEEDED(hr);
}
void CShellView::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	//	AdjustLayout();
	m_spExplorerBrowser->SetRect(NULL, CRect(0, 0, cx, cy));

}
/*
void CFileView::FillFileView()
{
HTREEITEM hRoot = m_wndFileView.InsertItem(_T("FakeApp files"), 0, 0);
m_wndFileView.SetItemState(hRoot, TVIS_BOLD, TVIS_BOLD);

HTREEITEM hSrc = m_wndFileView.InsertItem(_T("FakeApp Source Files"), 0, 0, hRoot);

m_wndFileView.InsertItem(_T("FakeApp.cpp"), 1, 1, hSrc);
m_wndFileView.InsertItem(_T("FakeApp.rc"), 1, 1, hSrc);
m_wndFileView.InsertItem(_T("FakeAppDoc.cpp"), 1, 1, hSrc);
m_wndFileView.InsertItem(_T("FakeAppView.cpp"), 1, 1, hSrc);
m_wndFileView.InsertItem(_T("MainFrm.cpp"), 1, 1, hSrc);
m_wndFileView.InsertItem(_T("StdAfx.cpp"), 1, 1, hSrc);

HTREEITEM hInc = m_wndFileView.InsertItem(_T("FakeApp Header Files"), 0, 0, hRoot);

m_wndFileView.InsertItem(_T("FakeApp.h"), 2, 2, hInc);
m_wndFileView.InsertItem(_T("FakeAppDoc.h"), 2, 2, hInc);
m_wndFileView.InsertItem(_T("FakeAppView.h"), 2, 2, hInc);
m_wndFileView.InsertItem(_T("Resource.h"), 2, 2, hInc);
m_wndFileView.InsertItem(_T("MainFrm.h"), 2, 2, hInc);
m_wndFileView.InsertItem(_T("StdAfx.h"), 2, 2, hInc);

HTREEITEM hRes = m_wndFileView.InsertItem(_T("FakeApp Resource Files"), 0, 0, hRoot);

m_wndFileView.InsertItem(_T("FakeApp.ico"), 2, 2, hRes);
m_wndFileView.InsertItem(_T("FakeApp.rc2"), 2, 2, hRes);
m_wndFileView.InsertItem(_T("FakeAppDoc.ico"), 2, 2, hRes);
m_wndFileView.InsertItem(_T("FakeToolbar.bmp"), 2, 2, hRes);

m_wndFileView.Expand(hRoot, TVE_EXPAND);
m_wndFileView.Expand(hSrc, TVE_EXPAND);
m_wndFileView.Expand(hInc, TVE_EXPAND);
}
*/
void CShellView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	//CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndShellView;
	//ASSERT_VALID(pWndTree);
	/*
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
	pWndTree->SelectItem(hTreeItem);
	}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
	*/
}

void CShellView::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	//int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	//m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndShellView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + /*cyTlb*/ + 1, rectClient.Width() - 2, rectClient.Height() - /*cyTlb*/ - 2, SWP_NOACTIVATE | SWP_NOZORDER);
	//	m_spExplorerBrowser.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + /*cyTlb*/ + 1, rectClient.Width() - 2, rectClient.Height() - /*cyTlb*/ - 2, SWP_NOACTIVATE | SWP_NOZORDER);
}

void CShellView::OnProperties()
{
	AfxMessageBox(_T("Properties...."));

}

void CShellView::OnFileOpen()
{
	// TODO: Add your command handler code here
}

void CShellView::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void CShellView::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void CShellView::OnEditCut()
{
	// TODO: Add your command handler code here
}

void CShellView::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CShellView::OnEditClear()
{
	// TODO: Add your command handler code here
}

void CShellView::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	//CRect rectTree;
	//m_wndShellView.GetWindowRect(rectTree);
	//ScreenToClient(rectTree);

	//rectTree.InflateRect(1, 1);
	//dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CShellView::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	//.SetFocus();
}

void CShellView::OnChangeVisualStyle()
{
#if 0
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	m_FileViewImages.DeleteImageList();

	UINT uiBmpId = theApp.m_bHiColorIcons ? IDB_FILE_VIEW_24 : IDB_FILE_VIEW;

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

	m_FileViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_FileViewImages.Add(&bmp, RGB(255, 0, 255));

	m_wndFileView.SetImageList(&m_FileViewImages, TVSIL_NORMAL);
#endif
}




void CShellView::OnDestroy()
{
	CDockablePane::OnDestroy();
	m_spExplorerBrowser->Destroy();

	// TODO: Add your message handler code here
}

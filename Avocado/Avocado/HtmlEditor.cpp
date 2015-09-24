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
#include "HtmlEditor.h"
#include <afxhtml.h>
#include "Resource.h"
#include "Avocado.h"
#include "afxwin.h"
#include "afxeditbrowsectrl.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif



class CAddJSButtonDlg : public CDialogEx
{
public:
	CAddJSButtonDlg();

	CString GetButtonName ();
	CString GetJSCode ();
	int     GetButtonType ();
	CString GetImageFileName ();
	// Dialog Data
	enum { IDD = IDD_ADD_JS_BUTTON };

protected:
	int m_buttonType;
	CString m_buttonString;
	CString m_JSCode;
	CString m_imageFileName;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual  BOOL DestroyWindow();
	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimertog();
	CEdit m_jsCodeText;
	CEdit m_buttonText;
	CComboBox m_jsButtonType;
	virtual BOOL OnInitDialog();
	CMFCEditBrowseCtrl m_imageFile;
};
BOOL CAddJSButtonDlg::DestroyWindow() 
{ 
     m_jsCodeText.GetWindowTextW (m_JSCode);
	 m_buttonText.GetWindowTextW (m_buttonString);
	 m_buttonType = m_jsButtonType.GetCurSel ();
	 m_imageFile.GetWindowTextW (m_imageFileName);
	return CDialog::DestroyWindow(); 
} 

CString CAddJSButtonDlg::GetImageFileName()
{
	return m_imageFileName;
}

CString CAddJSButtonDlg::GetButtonName()
{
	return m_buttonString;
}
CString CAddJSButtonDlg::GetJSCode ()
{
	return m_JSCode;
}
int CAddJSButtonDlg::GetButtonType ()
{
	return m_buttonType;
}
CAddJSButtonDlg::CAddJSButtonDlg() : CDialogEx(CAddJSButtonDlg::IDD)
{
}

void CAddJSButtonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_jsCodeText);
	DDX_Control(pDX, IDC_EDIT2, m_buttonText);
	DDX_Control(pDX, IDC_COMBO1, m_jsButtonType);
	DDX_Control(pDX, IDC_MFCEDITBROWSE1, m_imageFile);
}

BOOL CAddJSButtonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	m_jsButtonType.AddString (L"HyperLink");
	m_jsButtonType.AddString (L"Button");
	m_jsButtonType.AddString (L"Image");
	m_jsButtonType.AddString (L"OnLoad");
	m_jsButtonType.SetCurSel (0);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
BEGIN_MESSAGE_MAP(CAddJSButtonDlg, CDialogEx)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CMyHtmlEditCtrl
BEGIN_MESSAGE_MAP(CMyHtmlEditCtrl, CHtmlEditCtrl)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

void
	CMyHtmlEditCtrl::OnKillFocus (CWnd* pNewWnd)
{
	this->GetParent()->SendMessage (WM_KILLFOCUS,0,0);
}

BOOL CMyHtmlEditCtrl::Create(LPCTSTR lpszWindowName, DWORD /*dwStyle*/, const RECT& rect, CWnd* pParentWnd,
						   int nID, CCreateContext *pContext) 
{
	BOOL bRet = FALSE;
	// create the control window
	// Overriding CHtmlEditCtrl create method , only for the purpose of skipping AfxEnableControlContainer.
	// because it was already called in the application initinstance.
	//AfxEnableControlContainer();
	if (CreateControl(CLSID_WebBrowser, lpszWindowName,
				WS_VISIBLE | WS_CHILD, rect, pParentWnd, nID))
	{
		// in order to put the webbrowser in design mode, you must
		// first load a document into it. "about:blank" seems to
		// be the safest thing to load for a good starting point.
		CComQIPtr<IWebBrowser2> pBrowserApp = GetControlUnknown();
		ASSERT(pBrowserApp);
		if (pBrowserApp)
		{
			CComVariant vEmpty;
			LPCTSTR szDoc = GetStartDocument();
			if (szDoc)
			{
				CComBSTR bstrStart(szDoc);
				if (S_OK == pBrowserApp->Navigate(bstrStart, 
													&vEmpty,
													&vEmpty,
													&vEmpty,
													&vEmpty))
				{
					bRet = TRUE;
				}
			}
			else
				bRet = TRUE;

		}
	}

	if (!bRet)
		DestroyWindow();
	return bRet;
}

/////////////////////////////////////////////////////////////////////////////
// CHtmlEditor

CHtmlEditor::CHtmlEditor()
{
	//m_bHideInAutoHideMode= FALSE;
	//	is_here_allocated=false;
}

CHtmlEditor::~CHtmlEditor()
{
	//	if (is_here_allocated)
	{
		//free (here);
	}
}

BEGIN_MESSAGE_MAP(CHtmlEditor, CDockablePane)
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
	ON_WM_KILLFOCUS()
	ON_COMMAND(ID_HE_BACK, &CHtmlEditor::OnHeBack)
	ON_COMMAND(ID_HE_NEXT, &CHtmlEditor::OnHeNext)
	ON_COMMAND(ID_HTML_BACKCOLOR, &CHtmlEditor::OnHtmlBackcolor)
	ON_COMMAND(ID_EXPLORER_TEXTCOLOR, &CHtmlEditor::OnExplorerTextcolor)
	ON_UPDATE_COMMAND_UI(ID_EXPLORER_TEXTCOLOR, &CHtmlEditor::OnUpdateExplorerTextcolor)
	ON_UPDATE_COMMAND_UI(ID_HTML_BACKCOLOR, &CHtmlEditor::OnUpdateHtmlBackcolor)
	ON_COMMAND(ID_EXPLORER_BOLD, &CHtmlEditor::OnExplorerBold)
	ON_COMMAND(ID_EXPLORER_HEADING, &CHtmlEditor::OnExplorerHeading)
	ON_UPDATE_COMMAND_UI(ID_EXPLORER_BOLD, &CHtmlEditor::OnUpdateExplorerBold)
	ON_UPDATE_COMMAND_UI(ID_EXPLORER_HEADING, &CHtmlEditor::OnUpdateExplorerHeading)
	ON_COMMAND(ID_EXPLORER_TAKESNAPSHOT, &CHtmlEditor::OnExplorerTakesnapshot)
	ON_UPDATE_COMMAND_UI(ID_EXPLORER_TAKESNAPSHOT, &CHtmlEditor::OnUpdateExplorerTakesnapshot)
	ON_COMMAND(ID_EXPLORER_ADDJSCOMMAND, &CHtmlEditor::OnExplorerAddjscommand)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWorkspaceBar message handlers

int CHtmlEditor::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDockablePane::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rectDummy;
	rectDummy.SetRectEmpty();

	// Create view:
	const DWORD dwViewStyle = WS_CHILD | WS_VISIBLE ;//| TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS;

	if (!m_wndWebView.Create(NULL,dwViewStyle, rectDummy, this, -1))
	{
		TRACE0("Failed to create file view\n");
		return -1;      // fail to create
	}
	//m_wndWebView.SetLiveResize (false);
	// Load view images:
	m_wndWebView.SetScrollRange (SB_BOTH,0,0);
	m_WebViewImages.Create(IDB_FILE_VIEW, 16, 0, RGB(255, 0, 255));
	//	m_wndWebView.SetImageList(&m_WebViewImages, TVSIL_NORMAL);

	m_wndToolBar.Create(this, AFX_DEFAULT_TOOLBAR_STYLE | CBRS_TOOLTIPS , IDR_EXPLORER);
	m_wndToolBar.LoadToolBar(IDR_EXPLORER, 0, 0, TRUE /* Is locked */);

	OnChangeVisualStyle();

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	m_wndToolBar.SetPaneStyle(m_wndToolBar.GetPaneStyle() & ~(CBRS_GRIPPER | CBRS_SIZE_DYNAMIC | CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));

	m_wndToolBar.SetOwner(this);

	// All commands will be routed via this control , not via the parent frame:
	m_wndToolBar.SetRouteCommandsViaFrame(FALSE);

	for (int ii=m_wndToolBar.GetCount() - 1;ii > 2;ii--)
	{
		if (theApp.GetAppMode() == CAvocadoApp::VIEWER_APP)
		{
			m_wndToolBar.RemoveButton (ii);
		}
	}
	if (theApp.GetAppMode() == CAvocadoApp::VIEWER_APP)	
		m_wndToolBar.RemoveButton (0);
	// Fill in some static tree view data (dummy code, nothing magic here)
	FillWebView();
	AdjustLayout();

	return 0;
}
std::string CHtmlEditor::GetCurrentHtml  ()
{
	char cde[10000];
	CString szHtml;



	CComPtr<IHTMLDocument2> document;
	BSTR ready;
	m_wndWebView.GetDocument(&document);
	document->get_readyState(&ready);
	if( StrCmpW (ready, L"complete") == 0)
	{
		m_wndWebView.GetDocumentHTML (szHtml);
#ifdef UNICODE
		::wcstombs (cde,szHtml.GetString (),10000);
#else
		::strcpy (cde,szHtml.GetString());
#endif
		//document.Release ();
		SysFreeString(ready);

		return std::string (cde);
	}
	SysFreeString(ready);
	return string ("");



}


//static  
//static  
BSTR CHtmlEditor::char2bstr(const char* st)
{
	BSTR here;

	if (!st)
		return SysAllocString(L"");

	//if (is_here_allocated)
	{
		//	 free ( here );
	}
	// is_here_allocated=true;
	int size=MultiByteToWideChar(CP_ACP,0,st,-1,0,0);

	here=(BSTR)malloc(size*sizeof(WCHAR));

	//AntiLeak(here);

	MultiByteToWideChar(CP_ACP,0,st,-1,here,size);

	BSTR ret = SysAllocString(here);

	free (here);

	return ret;
}

void CHtmlEditor::TryOut(IHTMLDocument2* document,BSTR what)
{
	HRESULT hresult = S_OK;
	VARIANT *param;
	SAFEARRAY *sfArray;
	BSTR bstr = what;

	// Creates a new one-dimensional array
	sfArray = SafeArrayCreateVector(VT_VARIANT, 0, 1);

	if (sfArray == NULL || document == NULL) {
		goto cleanup;
	}

	hresult = SafeArrayAccessData(sfArray,(LPVOID*) & param);
	param->vt = VT_BSTR;
	param->bstrVal = bstr;
	hresult = SafeArrayUnaccessData(sfArray);
	hresult = document->write(sfArray);

cleanup:
	SysFreeString(bstr);
	if (sfArray != NULL) {
		SafeArrayDestroy(sfArray);
	}
}

void CHtmlEditor::SetCurrentHtml (std::string html)
{
	// Declare the variables needed 
#ifdef _DEBUG 
	CMemoryState oldMemState, newMemState, diffMemState; 
	oldMemState.Checkpoint(); 
#endif 


	BSTR szHtml ;
	wchar_t *ss= new wchar_t[10000];
	::mbstowcs (ss,html.c_str (),10000);
	//::mbstowcs (ss,testHtml.c_str (),10000);
	szHtml = ss;

	//m_wndWebView.NewDocument ();
	//m_wndWebView.
	CComPtr<IHTMLDocument2> document;
	BSTR ready;

	m_wndWebView.GetDocument(&document);
	document->get_readyState(&ready);
	int waitCount = 0;
	while ( CString (ready) != CString (L"complete"))
	{
		for (int i=0; i < 1000; i++);
		document->get_readyState(&ready);
		waitCount ++;
		if (waitCount > 3)
		{
			delete [] ss;
			return;
		}
	}
	if( CString (ready) == CString (L"complete"))
	{ 
		//void SetDocumentHTML(IHTMLDocument2* pDoc,const char* code) 
		{

			// TryOut(document,char2bstr(""));
			if (html != std::string (""))
			{
				//	BSTR bsthtml ;//= char2bstr(html.c_str());
				TryOut(document,char2bstr(html.c_str()));
				document->close ();
				TryOut(document,char2bstr(html.c_str()));
				document->close ();
				if (theApp.GetAppMode() != CAvocadoApp::AUTHORING_APP)
				{
					// hide scroll bars..
					CComPtr<IHTMLElement> ibody;
					document->get_body(&ibody);
					VARIANT varNo;
					varNo.vt=    VT_BSTR;
					varNo.bstrVal = L"no";
					
					
					BSTR bsstr = new wchar_t [7];
					::wsprintfW (bsstr,L"scroll");
					bsstr[6] = 0;
					ibody->setAttribute (bsstr,varNo);
					
					delete [] bsstr;
					//ibody->release();
				}
			}
			// document->close();
		}
		m_wndWebView.SetDesignMode ((theApp.GetAppMode()==CAvocadoApp::AUTHORING_APP));
		//m_wndWebView.SetDesignMode (TRUE);
		//m_wndWebView.SetDocumentHTML  (ss);//szHtml);
		//m_wndWebView.UpdateWindow ();
	}
	delete [] ss;
#ifdef _DEBUG 
	newMemState.Checkpoint(); 
	if( diffMemState.Difference( oldMemState, newMemState ) ) 
	{ 
		TRACE( "Memory leaked!\n" ); 
		diffMemState.DumpStatistics(); 
	} 
#endif 
}
void CHtmlEditor::OnSize(UINT nType, int cx, int cy)
{
	CDockablePane::OnSize(nType, cx, cy);
	AdjustLayout();
}

void CHtmlEditor::FillWebView()
{

}

void CHtmlEditor::OnContextMenu(CWnd* pWnd, CPoint point)
{
	CTreeCtrl* pWndTree = (CTreeCtrl*) &m_wndWebView;
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
			pWndTree->SelectItem(hTreeItem);
		}
	}

	pWndTree->SetFocus();
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EXPLORER, point.x, point.y, this, TRUE);
}

void CHtmlEditor::AdjustLayout()
{
	if (GetSafeHwnd() == NULL)
	{
		return;
	}

	CRect rectClient;
	GetClientRect(rectClient);

	int cyTlb = m_wndToolBar.CalcFixedLayout(FALSE, TRUE).cy;

	m_wndToolBar.SetWindowPos(NULL, rectClient.left, rectClient.top, rectClient.Width(), cyTlb, SWP_NOACTIVATE | SWP_NOZORDER);
	int htmlCtrlHeight = rectClient.Height() - cyTlb - 2;
	int htmlCtrlWidth = rectClient.Width() - 2;
	if (htmlCtrlHeight < 1)
		htmlCtrlHeight = 1;
	
	m_wndWebView.SetWindowPos(NULL, rectClient.left + 1, rectClient.top + cyTlb + 1,htmlCtrlWidth,htmlCtrlHeight , SWP_NOACTIVATE | SWP_NOZORDER);
	//m_wndWebView.ShowScrollBar (SB_BOTH   ,FALSE);
}

void CHtmlEditor::OnProperties()
{
	// Saving current html.

	//AfxMessageBox(_T("Properties...."));
	NMHDR pNMHDR; 
	pNMHDR.code = ID_HTML_EDITOR_SAVE;
	pNMHDR.hwndFrom = this->GetSafeHwnd ();

	GetParentFrame()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
}

void CHtmlEditor::OnFileOpen()
{
	// TODO: Add your command handler code here
}

void CHtmlEditor::OnFileOpenWith()
{
	// TODO: Add your command handler code here
}

void CHtmlEditor::OnDummyCompile()
{
	// TODO: Add your command handler code here
}

void CHtmlEditor::OnEditCut()
{
	// TODO: Add your command handler code here
}

void CHtmlEditor::OnEditCopy()
{
	// TODO: Add your command handler code here
}

void CHtmlEditor::OnEditClear()
{
	// TODO: Add your command handler code here
}

void CHtmlEditor::OnPaint()
{
	CPaintDC dc(this); // device context for painting

	CRect rectTree;
	m_wndWebView.GetWindowRect(rectTree);
	ScreenToClient(rectTree);

	rectTree.InflateRect(1, 1);
	dc.Draw3dRect(rectTree, ::GetSysColor(COLOR_3DSHADOW), ::GetSysColor(COLOR_3DSHADOW));
}

void CHtmlEditor::OnSetFocus(CWnd* pOldWnd)
{
	CDockablePane::OnSetFocus(pOldWnd);

	m_wndWebView.SetFocus();

}

void CHtmlEditor::OnChangeVisualStyle()
{
	m_wndToolBar.CleanUpLockedImages();
	m_wndToolBar.LoadBitmap(theApp.m_bHiColorIcons ? IDB_EXPLORER_24 : IDR_EXPLORER, 0, 0, TRUE /* Locked */);

	m_WebViewImages.DeleteImageList();

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

	m_WebViewImages.Create(16, bmpObj.bmHeight, nFlags, 0, 0);
	m_WebViewImages.Add(&bmp, RGB(255, 0, 255));

	//	m_wndWebView.SetImageList(&m_WebViewImages, TVSIL_NORMAL);
}



void CHtmlEditor::EmbbedImage (CString path,int w,int h)
{
	m_wndWebView.Image (path);
#if 0
	CComPtr<IHTMLDocument2> document;
	BSTR ready;

	m_wndWebView.GetDocument(&document);
	document->get_readyState(&ready);
	/*while ( CString (ready) != CString (L"complete"))
	{
	for (int i=0; i < 1000; i++);
	document->get_readyState(&ready);
	}*/
	if( CString (ready) == CString (L"complete"))
	{ 
		CComPtr<IHTMLElement> ibody;
		document->get_body(&ibody);
		CString htmls = CString ("<img src=")+path+CString (">");
		ibody->insertAdjacentHTML (L"",htmls.GetBuffer ());
	}
#endif
}
void CHtmlEditor::OnKillFocus(CWnd* pNewWnd)
{
	CDockablePane::OnKillFocus(pNewWnd);
#if 0
	if (pNewWnd == (void*)m_wndWebView)
		return;
	//AfxMessageBox(_T("Properties...."));
	NMHDR pNMHDR; 
	pNMHDR.code = ID_HTML_EDITOR_SAVE;
	pNMHDR.hwndFrom = this->GetSafeHwnd ();

	this->GetParentFrame()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
#endif
	// TODO: Add your message handler code here
}


void CHtmlEditor::OnHeBack()
{

	NMHDR pNMHDR; 
	pNMHDR.code = ID_HTML_EDITOR_BACK;
	pNMHDR.hwndFrom = this->GetSafeHwnd ();

	this->GetParentFrame()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
}


void CHtmlEditor::OnHeNext()
{
	// TODO: Add your command handler code here
	NMHDR pNMHDR; 
	pNMHDR.code = ID_HTML_EDITOR_NEXT;
	pNMHDR.hwndFrom = this->GetSafeHwnd ();

	this->GetParentFrame()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
}


void CHtmlEditor::OnHtmlBackcolor()
{
	// TODO: Add your command handler code here
	CColorDialog dlg; 
	if (dlg.DoModal() == IDOK) 
	{
		COLORREF sel_col;
		sel_col=dlg.GetColor(); 
		CComPtr<IHTMLDocument2> document;
		BSTR ready;

		m_wndWebView.GetDocument(&document);
		document->get_readyState(&ready);
		/*while ( CString (ready) != CString (L"complete"))
		{
		for (int i=0; i < 1000; i++);
		document->get_readyState(&ready);
		}*/
		if( CString (ready) == CString (L"complete"))
		{
			VARIANT v;
			v.intVal = sel_col;
			v.vt = VT_INT;
			document->put_bgColor (v);
			CComPtr<IHTMLElement> ibody;
			document->get_body(&ibody);
		}
	}

}

bool CHtmlEditor::IsDesignMode ()
{
	return (theApp.GetAppMode () == CAvocadoApp::AUTHORING_APP);
}

void CHtmlEditor::OnExplorerTextcolor()
{
	CColorDialog dlg; 
	if (dlg.DoModal() == IDOK) 
	{
		COLORREF sel_col;
		sel_col=dlg.GetColor(); 	
		m_wndWebView.SetForeColor (sel_col);
	}

	// TODO: Add your command handler code here
}


void CHtmlEditor::OnUpdateExplorerTextcolor(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (IsDesignMode ());
}


void CHtmlEditor::OnUpdateHtmlBackcolor(CCmdUI *pCmdUI)
{
	pCmdUI->Enable (IsDesignMode ());
	// TODO: Add your command update UI handler code here
}


void CHtmlEditor::OnExplorerBold()
{
	// TODO: Add your command handler code here	
	m_wndWebView.Bold ();		
}


void CHtmlEditor::OnExplorerHeading()
{
	// TODO: Add your command handler code here
	m_wndWebView.Font ();
}


void CHtmlEditor::OnUpdateExplorerBold(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (IsDesignMode ());
}


void CHtmlEditor::OnUpdateExplorerHeading(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (IsDesignMode ());
}


void CHtmlEditor::OnExplorerTakesnapshot()
{
	AfxMessageBox(_T("Snapshot taken successfully !"));
	NMHDR pNMHDR; 
	pNMHDR.code = ID_PROPERTIES;
	pNMHDR.hwndFrom = this->GetSafeHwnd ();

	GetParentFrame()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
}


void CHtmlEditor::OnUpdateExplorerTakesnapshot(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	pCmdUI->Enable (IsDesignMode ());
}
BOOL CHtmlEditor::WaitForDocument ()
{
		CComPtr<IHTMLDocument2> document;
		BSTR ready;
		m_wndWebView.GetDocument(&document);
		document->get_readyState(&ready);
		int waitCount = 0;
		while ( CString (ready) != CString (L"complete"))
		{
			for (int i=0; i < 1000; i++);
			document->get_readyState(&ready);
			waitCount ++;
			if (waitCount > 3)
			{
				return FALSE;
			}
		}
		if( CString (ready) == CString (L"complete"))
			return TRUE;
		return FALSE;
}

void CHtmlEditor::ParseJSCode (CString &jscode)
{

}

void CHtmlEditor::AddJsButton()
{
	CAddJSButtonDlg dls;
	if (dls.DoModal () == IDOK)
	{
		CString usr_jsCode = dls.GetJSCode ();
		ParseJSCode (usr_jsCode);
		CString usr_JSButtonName = dls.GetButtonName();
		int usr_buttonType = dls.GetButtonType ();

		CComPtr<IHTMLElement> spElement; 
		CComPtr<IHTMLScriptElement> spScript; 
	
		CComPtr<IHTMLDocument2> document;
//		BSTR ready;
		m_wndWebView.GetDocument(&document);
		CString functionName = usr_JSButtonName;
		functionName.Replace (L" ",L"_");
		CString ScriptText = L"function btnOnClick_";
		ScriptText += functionName;
		ScriptText += "() {";
		ScriptText+= usr_jsCode;
		ScriptText+= L"};";
		if (usr_buttonType ==3)
		{
			CString funccallName = CString (L"btnOnClick_") + functionName+ CString (L";") ;//+ CString (L"();");

			ScriptText += CString (L"window.onload=")+funccallName;
		}
		//window.alert('Hi');}"; 
		HRESULT hr;
		hr = document-> createElement (CComBSTR (L"SCRIPT"), &spElement); 
		hr = spElement-> QueryInterface (IID_IHTMLScriptElement, (void **) &spScript); 
		hr = spScript-> put_text (_bstr_t (ScriptText)); 
		hr = spScript-> put_type (_bstr_t (L"text/javascript")); 

		IHTMLElementCollection* pColl; 
		IHTMLElementCollection* pCollHead; 

		hr = document-> get_all (&pColl); 

		CComVariant vt; 
		vt.vt = VT_BSTR; 
		vt.bstrVal = CComBSTR (L"head"); 

		CComPtr <IDispatch> pDispatch; 
		hr = pColl-> tags (vt,&pDispatch); 
		hr = pDispatch-> QueryInterface (IID_IHTMLElementCollection, (void **) &pCollHead); 

		CComQIPtr <IHTMLElement> pElement; 
		CComQIPtr <IDispatch> pDispImgElement; 

		CComVariant v = 0; 
		pCollHead-> item (v, v, &pDispImgElement); 
		pElement = pDispImgElement; 

		IHTMLDOMNode* pNodea; 
		IHTMLDOMNode* pNodeb; 
		IHTMLDOMNode* pNodec; 
		IHTMLDOMNode* pNodeimg;

		if (SUCCEEDED (pElement-> QueryInterface (IID_IHTMLDOMNode, (void **) &pNodea))) 
		{ 
			if (SUCCEEDED (spElement-> QueryInterface (IID_IHTMLDOMNode, (void **) &pNodeb))) 
			{ 
				hr = pNodea-> appendChild (pNodeb, &pNodec); 
				hr = pNodea-> Release (); 
				hr = pNodeb-> Release (); 
				hr = pNodec-> Release (); 
               
			}
		}
		CComPtr<IHTMLElement> ibody;
		document->get_body(&ibody);
		if (SUCCEEDED (ibody-> QueryInterface (IID_IHTMLDOMNode, (void **) &pNodea))) 
		{ 
			if (usr_buttonType < 0) 
				usr_buttonType = 1;

			CComPtr<IHTMLElement> buttonObj;
			CComPtr<IHTMLElement> imageObj;
			if (usr_buttonType == 3)
			{
				CComVariant vt2; 
				vt2.vt = VT_BSTR; 
				CString funccallName = CString (L"btnOnClick_") + functionName + CString (L"();");
				//vt2.bstrVal = CComBSTR (L"btnOnClick_();"); 
				vt2.bstrVal = CComBSTR (funccallName.GetString ()); 

				//ibody->setAttribute (_bstr_t (L"onload"),vt2);
			}
			else
			if (usr_buttonType == 1)
			{			
				document->createElement (_bstr_t("a"),&buttonObj);
			//	buttonObj->put_innerHTML (_bstr_t(L"href=www.google.com onclick=btn_OnClick();"));
				buttonObj->put_innerText (_bstr_t (usr_JSButtonName.GetString ()));
				CComVariant vt2; 
				vt2.vt = VT_BSTR; 
				CString funccallName = CString (L"btnOnClick_") + functionName + CString (L"();");
				//vt2.bstrVal = CComBSTR (L"btnOnClick_();"); 
				vt2.bstrVal = CComBSTR (funccallName.GetString ()); 

				buttonObj->setAttribute (_bstr_t (L"onclick"),vt2);

				if (WaitForDocument ())
				{
					CComVariant vt3; 
					vt3.vt = VT_BSTR; 
					vt3.bstrVal = ::SysAllocString (L"javascript:void(0)");//CComBSTR (L"#"); 
			
					buttonObj->setAttribute (_bstr_t (L"href"),vt3);
				}
			} 
			else if (usr_buttonType == 0)
			{
				document->createElement (_bstr_t("button"),&buttonObj);
			//	buttonObj->put_innerHTML (_bstr_t(L"href=www.google.com onclick=btn_OnClick();"));
				buttonObj->put_innerText (_bstr_t (usr_JSButtonName.GetString ()));
				CComVariant vt2; 
				vt2.vt = VT_BSTR; 
				CString funccallName = CString (L"btnOnClick_") + functionName + CString (L"();");
				//vt2.bstrVal = CComBSTR (L"btnOnClick_();"); 
				vt2.bstrVal = CComBSTR (funccallName.GetString ()); 

				buttonObj->setAttribute (_bstr_t (L"onclick"),vt2);
#if 0
				if (WaitForDocument ())
				{
					CComVariant vt3; 
					vt3.vt = VT_BSTR; 
					vt3.bstrVal = ::SysAllocString (L"#");//CComBSTR (L"#"); 
			
					buttonObj->setAttribute (_bstr_t (L"href"),vt3);
				}
#endif
			}
			else if (usr_buttonType == 2)
			{
				document->createElement (_bstr_t("a"),&buttonObj);
			//	buttonObj->put_innerHTML (_bstr_t(L"href=www.google.com onclick=btn_OnClick();"));
				//buttonObj->put_innerText (_bstr_t (usr_JSButtonName.GetString ()));
				CComVariant vt2; 
				vt2.vt = VT_BSTR; 
				CString funccallName = CString (L"btnOnClick_") + functionName + CString (L"();");
				//vt2.bstrVal = CComBSTR (L"btnOnClick_();"); 
				vt2.bstrVal = CComBSTR (funccallName.GetString ()); 

				buttonObj->setAttribute (_bstr_t (L"onclick"),vt2);

				if (WaitForDocument ())
				{
					CComVariant vt3; 
					vt3.vt = VT_BSTR; 
					vt3.bstrVal = ::SysAllocString (L"javascript:void(0)");//CComBSTR (L"#"); 
			
					buttonObj->setAttribute (_bstr_t (L"href"),vt3);
				}

				document->createElement (_bstr_t("img"),&imageObj);
			//	buttonObj->put_innerHTML (_bstr_t(L"href=www.google.com onclick=btn_OnClick();"));
				//buttonObj->put_innerText (_bstr_t (usr_JSButtonName.GetString ()));
				if (WaitForDocument ())
				{
					CComVariant vt3; 
					vt3.vt = VT_BSTR; 
					vt3.bstrVal = ::SysAllocString (dls.GetImageFileName ().GetString ());//CComBSTR (L"#"); 
			
					imageObj->setAttribute (_bstr_t (L"src"),vt3);
				}
							if (SUCCEEDED (imageObj-> QueryInterface (IID_IHTMLDOMNode, (void **) &pNodeimg))) 
							{
								if (SUCCEEDED (buttonObj-> QueryInterface (IID_IHTMLDOMNode, (void **) &pNodeb))) 
							{ 
								pNodeb->appendChild (pNodeimg,&pNodec);
								hr = pNodeimg-> Release (); 
								hr = pNodeb-> Release (); 
								hr = pNodec-> Release (); 
							}
							}
			}

			if (buttonObj && SUCCEEDED (buttonObj-> QueryInterface (IID_IHTMLDOMNode, (void **) &pNodeb))) 
			{ 
				pNodea->appendChild (pNodeb,&pNodec);
		
				hr = pNodea-> Release (); 
				hr = pNodeb-> Release (); 
				hr = pNodec-> Release (); 
			}
		}
	}
}

void CHtmlEditor::OnExplorerAddjscommand()
{
	return AddJsButton();
	// TODO: Add your command handler code here
	
#if 0
	CComPtr<IHTMLDocument2> document;
	BSTR ready;
	m_wndWebView.GetDocument(&document);

	IHTMLElementCollection *elcol ;
	CComPtr<IHTMLElement> ibody;
	document->get_body(&ibody);
	
	//IHTMLElement head = (IHTMLElement)((IHTMLElementCollection)
	//	document->get_all (&elcol);
	//	elcol->tags("head",
	//	get_hea`.all.tags("head")).item(null, 0);
	CComPtr<IHTMLElement> scriptObject;
	document->createElement (L"script",&scriptObject);
	
	if (scriptObject)
	{
		CComQIPtr<IHTMLScriptElement> scriptObj (scriptObject);
		//CComPtr<IHTMLScriptElement>
		CComBSTR bstrStart(L"text/javascript");
		scriptObj->put_type (bstrStart);
		CComBSTR bstrStart2(L"function btn1_OnClick () { alert('Hi');}");
		scriptObj->put_text (bstrStart2);
		CComPtr<IHTMLElement> buttonObj;
		document->createElement (L"a",&buttonObj);
		
		buttonObj->put_innerHTML (L"href=www.google.com onclick=btn_OnClick();");
		buttonObj->put_innerText (L"hello");
		ibody->insertAdjacentHTML (L"afterBegin",L"HELLO");

		//VARIANT VV ;
		//VV.bstrVal = L"Hello";
		//VV.vt = B_STR;
	//	buttonObj->put_onclick (VV);
		//buttonObj->put_outerText (L"AAAAA");
		//buttonObj->put_onclick (L"btn1_OnClick()");
		//ibody->pu
	}
	//((CComPtr<IHTMLScriptElement>)scriptObject)
	//IHTMLScriptElement scriptObject = (IHTMLScriptElement)doc.createElement("script");
	//scriptObject.type = @"text/javascript";
	//scriptObject.text = @"function btn1_OnClick(str){
	//	alert('you clicked' + str);
	//}";
	//((HTMLHeadElementClass)head).appendChild((IHTMLDOMNode)scriptObject);
#endif
}



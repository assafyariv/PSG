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

#pragma once

#include "ViewTree.h"

class CHtmlEditorToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CMyHtmlEditCtrl : public CHtmlEditCtrl 
{
public:
//	CMyHtmlEditCtrl () : CHtmlEditCtrl() {}
	//CMyHtmlEditCtrl (const CMyHtmlEditCtrl &p) : CHtmlEditCtrl((CHtmlEditCtrl)p) {}
	~CMyHtmlEditCtrl () {}
virtual BOOL Create(LPCTSTR lpszWindowName, DWORD dwStyle,
		const RECT& rect, CWnd* pParentWnd, int nID, CCreateContext *pContext=NULL);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};

class CHtmlEditor : public CDockablePane
{
// Construction
public:
	CHtmlEditor();
	bool IsDesignMode ();
	void AddJsButton();
	BOOL WaitForDocument ();
	BSTR char2bstr(const char* st);
	void TryOut(IHTMLDocument2* document,BSTR what);
	void AdjustLayout();
	void OnChangeVisualStyle();
	void SetCurrentHtml (std::string html);
	void EmbbedImage (CString path,int w, int h);
	std::string GetCurrentHtml ();
	void ParseJSCode (CString &jscode);
	//BSTR here;
	//bool is_here_allocated; 
// Attributes
protected:
	
	CMyHtmlEditCtrl m_wndWebView;
	CImageList m_WebViewImages;
	CHtmlEditorToolBar m_wndToolBar;

protected:
	void FillWebView();

// Implementation
public:
	virtual ~CHtmlEditor();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnProperties();
	afx_msg void OnFileOpen();
	afx_msg void OnFileOpenWith();
	afx_msg void OnDummyCompile();
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditClear();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnHeBack();
	afx_msg void OnHeNext();
	afx_msg void OnHtmlBackcolor();
	afx_msg void OnExplorerTextcolor();
	afx_msg void OnUpdateExplorerTextcolor(CCmdUI *pCmdUI);
	afx_msg void OnUpdateHtmlBackcolor(CCmdUI *pCmdUI);
	afx_msg void OnExplorerBold();
	afx_msg void OnExplorerHeading();
	afx_msg void OnUpdateExplorerBold(CCmdUI *pCmdUI);
	afx_msg void OnUpdateExplorerHeading(CCmdUI *pCmdUI);
	afx_msg void OnExplorerTakesnapshot();
	afx_msg void OnUpdateExplorerTakesnapshot(CCmdUI *pCmdUI);
	afx_msg void OnExplorerAddjscommand();
};


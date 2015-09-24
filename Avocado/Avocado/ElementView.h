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
#include <string>
#include <vector>

using namespace std;
class CClassToolBar : public CMFCToolBar
{
	virtual void OnUpdateCmdUI(CFrameWnd* /*pTarget*/, BOOL bDisableIfNoHndler)
	{
		CMFCToolBar::OnUpdateCmdUI((CFrameWnd*) GetOwner(), bDisableIfNoHndler);
	}

	virtual BOOL AllowShowOnList() const { return FALSE; }
};

class CElementView : public CDockablePane
{
public:
	CElementView();
	virtual ~CElementView();
		void HighlightElements (std::vector<int> sel);
	void HighlightElement (int sel);
	bool AddElement (std::string name, int id , bool visible,int parentID);
	void ClearList ();
	void AdjustLayout();
	void OnChangeVisualStyle();
	CString GetLastString ();
	int GetSelectedID ();
protected:
	CClassToolBar m_wndToolBar;
	CViewTree m_wndClassView;
	CImageList m_ClassViewImages;
	UINT m_nCurrSort;
	int  m_selected;
	void FillClassView();
	vector<pair<int,HTREEITEM>> m_list;
	
// Overrides
public:
	bool GetLastShift ();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnClassAddMemberFunction();
	afx_msg void OnClassAddMemberVariable();
	afx_msg void OnClassDefinition();
	afx_msg void OnClassProperties();
	afx_msg void OnNewFolder();
	afx_msg void OnPaint();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg LRESULT OnChangeActiveTab(WPARAM, LPARAM);
	afx_msg void OnSort(UINT id);
	afx_msg void OnUpdateSort(CCmdUI* pCmdUI);

	DECLARE_MESSAGE_MAP()
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
//	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
public:
//	afx_msg void OnUpdateAppreanceHide(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateAppreanceUnhideall(CCmdUI *pCmdUI);
//	afx_msg void OnAppreanceHide();
	afx_msg void OnUpdateAppreanceHide(CCmdUI *pCmdUI);
//	afx_msg void OnAppreanceUnhideall();
	afx_msg void OnUpdateAppreanceUnhideall(CCmdUI *pCmdUI);
	afx_msg void OnUpdateApperanceUnhide(CCmdUI *pCmdUI);
	afx_msg void OnUpdateSplitElement(CCmdUI *pCmdUI);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


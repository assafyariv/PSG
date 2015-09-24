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
#include "ViewTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewTree

CViewTree::CViewTree()
{
	m_lastShift  = false;
}

CViewTree::~CViewTree()
{
}

BEGIN_MESSAGE_MAP(CViewTree, CTreeCtrl)
//	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CViewTree::OnTvnEndlabeledit)
	ON_NOTIFY_REFLECT(TVN_ENDLABELEDIT, &CViewTree::OnTvnEndlabeledit)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CViewTree message handlers

BOOL CViewTree::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	BOOL bRes = CTreeCtrl::OnNotify(wParam, lParam, pResult);

	NMHDR* pNMHDR = (NMHDR*)lParam;
	ASSERT(pNMHDR != NULL);

	if (pNMHDR && pNMHDR->code == TTN_SHOW && GetToolTips() != NULL)
	{
		GetToolTips()->SetWindowPos(&wndTop, -1, -1, -1, -1, SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOSIZE);
	}

	return bRes;
}


//void CViewTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	TV_DISPINFO *pTVDispInfo = (TV_DISPINFO*)pNMHDR; 
//
//	GetParent()->SendMessage (WM_NOTIFY, 0,(LPARAM)pTVDispInfo);
//
//	*pResult = 0;
//}


void CViewTree::OnTvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTVDISPINFO pTVDispInfo = reinterpret_cast<LPNMTVDISPINFO>(pNMHDR);
	// TODO: Add your control notification handler code here
	m_lastString = pTVDispInfo->item.pszText;
	//GetParent ()->SendMessage (WM_NOTIFY,0,TVN_ENDLABELEDIT);

	NMHDR pNMHDR2; 
	pNMHDR2.code = TVN_ENDLABELEDIT;
	pNMHDR2.hwndFrom = this->GetSafeHwnd ();

	GetParentFrame()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR2);
	*pResult = 0;
}


void CViewTree::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: Add your message handler code here and/or call default
	m_lastShift =((nFlags & MK_SHIFT) == MK_SHIFT);
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

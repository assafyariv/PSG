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

/////////////////////////////////////////////////////////////////////////////
// CWebBrowserBar window
#include "CWebBrowser2.h"

class CWebBrowserBar : public CWnd
{
// Construction
public:
	CWebBrowserBar();
	void Init ();
// Attributes
protected:
	CWebBrowser2 m_wndCalendar;
	int m_nMyCalendarsY;
	CImageList m_Images;

// Overrides
public:
	virtual BOOL Create(const RECT& rect, CWnd* pParentWnd, UINT nID = (UINT)-1);

// Implementation
public:
	virtual ~CWebBrowserBar();

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDocumentComplete(LPDISPATCH lpDisp,
                                VARIANT FAR* URL);

	DECLARE_MESSAGE_MAP()
	DECLARE_EVENTSINK_MAP()

public:
	virtual BOOL OnAmbientProperty(COleControlSite* pSite, DISPID dispid, VARIANT* pvar);
};

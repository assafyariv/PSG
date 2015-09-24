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

// AvocadoView.h : interface of the CAvocadoView class
//

#pragma once
#include "../AvocadoEngine/AvocadoAppInterface.h"
#define RENDER_STYLE_TOON 2
#define RENDER_STYLE_HLR 3
#define RENDER_STYLE_SHADED 1
#define RENDER_STYLE_FLAT 4

#define DISPLAY_CAMERAS 0x0001
#define DISPLAY_LIGHTS  0x0002
#define DISPLAY_BACKGROUND 0x0004
#define DISPLAY_SPIN_CENTER 0x0008
#define DISPLAY_ANNOTATIONS 0x0010

#define MODEL_MOVE 0x0001
#define MODEL_ROTATE 0x0002
#define MODEL_SCALE 0x0004

#define TRACK_MODE_CAD  0x0001
#define TRACK_MODE_USER 0x0002
#define TRACK_MODE_FLY  0x0003


class CAvocadoView;
class CAvocadoViewInterface: public avocado::AvocadoViewInterface
{
public:
	CAvocadoViewInterface(CAvocadoView *view){ m_view = view; }
	~CAvocadoViewInterface(){}
	virtual void ViewParamChanged (const char *paramName,const char *value);
	virtual void InvokePaintView();
	virtual void SelectionChanged ( std::vector<int> selectedElement ) ;
	virtual void  ErrorCallback (const char *errDesc);
	virtual void  SuccessCallback ();

private:
	CAvocadoView *m_view;
};

class CAvocadoView : public CView
{
protected: // create from serialization only
	CAvocadoView();
	DECLARE_DYNCREATE(CAvocadoView)

// Attributes
public:
	CAvocadoDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:

	virtual ~CAvocadoView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
#ifndef SHARED_HANDLERS
	void RaiseError (std::string err);
	void InvokePaint();
	void InvokePaintLow();
	int GetID(){ return m_id; }
	int GetViewParameterAsInt (string paramName);
	bool SetViewParameter (string name,string value);
	bool SetOrthographic (bool ortho);
		bool IsOrthographic ();
	int GetRenderStyle () {return m_renderStyle;}
	void  SetRenderStyle (int style,bool updateEngine = true);
	void SetCurrentSelection (vector<int> eids);
	bool HasSelection ();
	int GetSelection ();
	int GetSelectionCount ();
	int GetSelection (int i);
	const int &GetMouseX () const {return m_mouseX;}
	const int &GetMouseY  () const {return m_mouseY;}
#endif
private:
	CBitmap									m_splashBitmap;
	bool									m_splashBmpInit;
	int	m_mouseX;
	int m_mouseY;
	int										m_id;
	CAvocadoViewInterface					*m_viewInterface;
	CPoint									mouse_pt;
	bool									mouse_pt_init;
	bool									m_animating;
	int										m_frameCount;
	bool									m_rayTracing;
	int									    m_renderStyle;
	bool									m_orthographic;
	vector <pair<string,string>>            m_viewParameters;
	bool									m_preSelectionHighlights;
	bool		m_modelMove;
	bool		m_modelRotate;
	bool		m_modelScale;
	unsigned int m_displayFilters;
	unsigned int m_dragFilters;
	bool		m_showProgressText;
	int			m_currentSelection;
	std::vector<int>			m_currentSelectionList;
	bool		m_displayCameras;
	bool		m_displayAnnotations;
	bool		m_displayLights;
	bool		m_displaySpinCenter;
	bool		m_displayBackground;
	int m_viewSizeX;
	int m_viewSizeY;
// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
#ifndef SHARED_HANDLERS
	afx_msg void OnPaint();
#endif
	afx_msg int  OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	DECLARE_MESSAGE_MAP()
public:
#ifndef SHARED_HANDLERS
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnMouseHWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnTimeChange();
	afx_msg void OnHide();
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	afx_msg void OnCopy();
	afx_msg void OnPaste();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	virtual void OnInitialUpdate();
	afx_msg void OnPlay();
	afx_msg void OnStop();
	afx_msg void OnPause();
	afx_msg void OnTimertog();
	afx_msg void OnLayoutGalery();
	afx_msg void OnUpdateLayoutGalery(CCmdUI *pCmdUI);
	//afx_msg void OnCancelEditSrvr ();
#endif
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	afx_msg void OnEditCopy();
//	afx_msg void OnUpdateGraphicsRaytracing(CCmdUI *pCmdUI);
};

#ifndef _DEBUG  // debug version in AvocadoView.cpp
inline CAvocadoDoc* CAvocadoView::GetDocument() const
   { return reinterpret_cast<CAvocadoDoc*>(m_pDocument); }
#endif


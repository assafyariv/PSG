
// IpFrame.h : interface of the CInPlaceFrame class
//

#pragma once
class CAvocadoView;
class CAvocadoDoc;
class CInPlaceFrame : public COleDocIPFrameWndEx
{
	DECLARE_DYNCREATE(CInPlaceFrame)
public:
	CInPlaceFrame();

// Attributes
public:

// Operations
public:

// Overrides
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);

// Implementation
public:
	virtual ~CInPlaceFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CToolBar    m_wndToolBar;
	CAvocadoView* GetActiveAvocadoView ();
	CAvocadoDoc* GetActiveAvocadoDoc ();
	COleDropTarget	m_dropTarget;
	COleResizeBar   m_wndResizeBar;
	CMFCRibbonBar   m_wndRibbonBar;
	void ToggleViewFilter (const char* param, unsigned int filt);
	bool GetActiveViewFilter (const char* param,unsigned int filt);
		bool ActiveViewHasSelection ();
		int	GetActiveViewID();
		int GetActiveViewIntParam (const char *param);
	//int GetActiveViewIntParam (const char* param);
	//CView* GetActiveAvocadoView ();
	//int GetActiveViewID();
// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	virtual void ActivateFrame(int nCmdShow = -1);
//	afx_msg void OnUpdateGraphicsRaytracing(CCmdUI *pCmdUI);
//	afx_msg void OnGraphicsRaytracing();
	afx_msg void OnLayoutBackpage();
	afx_msg void OnLayoutNextpage();
	afx_msg void OnModelMove();
	afx_msg void OnUpdateModelMove(CCmdUI *pCmdUI);
	afx_msg void OnModelRotate();
	afx_msg void OnUpdateModelRotate(CCmdUI *pCmdUI);
	afx_msg void OnModelScale();
	afx_msg void OnUpdateModelScale(CCmdUI *pCmdUI);
	afx_msg void OnGraphicsRaytracing();
};



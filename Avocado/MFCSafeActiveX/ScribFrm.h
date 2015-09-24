// ScribFrm.h : interface of the CScribbleBar and
//    CScribbleFrame classes

class CScribbleBar : public CToolBar
{
    LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

public:
    BOOL Create(CWnd* pParentWnd);
};

class CScribbleFrame : public CFrameWnd
{
    DECLARE_DYNCREATE(CScribbleFrame)

public:
	
	//~CScribbleFrame();
    //{{AFX_VIRTUAL(CScribbleFrame)
    //}}AFX_VIRTUAL

protected:
	CScribbleFrame();
   // CScribbleBar m_wndToolBar;
	void RefreshDocLoadStatus ();
	int		s_last_prog;
    //{{AFX_MSG(CScribbleFrame)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnChangeWidth();
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
};


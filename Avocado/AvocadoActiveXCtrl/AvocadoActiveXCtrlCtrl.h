#pragma once
#include "../Avocado/AvocadoDoc.h"
#include "../Avocado/AvocadoView.h"
// AvocadoActiveXCtrlCtrl.h : Declaration of the CAvocadoActiveXCtrlCtrl ActiveX Control class.


// CAvocadoActiveXCtrlCtrl : See AvocadoActiveXCtrlCtrl.cpp for implementation.

class CAvocadoActiveXCtrlCtrl : public COleControl
{
	DECLARE_DYNCREATE(CAvocadoActiveXCtrlCtrl)

// Constructor
public:
	CAvocadoActiveXCtrlCtrl();
	void InitAvocado (void *hwnd, int cx, int cy);
	CWnd    *m_childWnd;
	CAvocadoDoc  *m_AVDoc;
	CAvocadoView *m_AVWnd;
// Overrides
public:
	virtual void OnDraw(CDC* pdc, const CRect& rcBounds, const CRect& rcInvalid);
	virtual void DoPropExchange(CPropExchange* pPX);
	virtual void OnResetState();

// Implementation
protected:
	~CAvocadoActiveXCtrlCtrl();

	BEGIN_OLEFACTORY(CAvocadoActiveXCtrlCtrl)        // Class factory and guid
		virtual BOOL VerifyUserLicense();
		virtual BOOL GetLicenseKey(DWORD, BSTR FAR*);
	END_OLEFACTORY(CAvocadoActiveXCtrlCtrl)

//	DECLARE_OLECREATE_EX(CAvocadoActiveXCtrlCtrl)    // Class factory and guid
	DECLARE_OLETYPELIB(CAvocadoActiveXCtrlCtrl)      // GetTypeInfo
	DECLARE_PROPPAGEIDS(CAvocadoActiveXCtrlCtrl)     // Property page IDs
	DECLARE_OLECTLTYPE(CAvocadoActiveXCtrlCtrl)		// Type name and misc status

// Message maps
	DECLARE_MESSAGE_MAP()

// Dispatch maps
	DECLARE_DISPATCH_MAP()

// Event maps
	DECLARE_EVENT_MAP()

// Dispatch and event IDs
public:
	enum {
	};
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnDestroy();
	afx_msg void OnPaint();
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL OnWndMsg(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual void OnClick(USHORT iButton);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
};


// ScribFrm.cpp : implementation of the CScribbleFrame class

#include "stdafx.h"
#include "resource.h"
#include "ScribFrm.h"
#include "AvocadoDoc.h"
#include "AvocadoView.h"
#include <string>
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CScribbleFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CScribbleFrame, CFrameWnd)
    //{{AFX_MSG_MAP(CScribbleFrame)
    ON_WM_CREATE()
    //ON_COMMAND(IDB_CHANGE_WIDTH, OnChangeWidth)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()
CScribbleFrame::CScribbleFrame()
{
	s_last_prog = 0;
}
//CScribbleFrame::~CScribbleFrame()
//{
//}

int CScribbleFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
        return -1;
    
   // if (!m_wndToolBar.Create(this))
    {
     //   TRACE0("Failed to create toolbar\n");
       // return -1;
    }

    return 0;
}

void CScribbleFrame::OnChangeWidth() 
{
    // When the Change Width button is clicked, we display
    // a popup menu from which the user can select a new
    // pixel width.
#if 0
    CMenu menu;
    VERIFY(menu.LoadMenu(IDR_WIDTH_MENU));
    CMenu* pPopup = menu.GetSubMenu(0);
    ASSERT(pPopup != NULL);

    CPoint point;
    GetCursorPos(&point);
    pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
        point.x, point.y, this);
#endif
}

////////////////////////////////////////////////////////////////////////

BOOL CScribbleBar::Create(CWnd* pParentWnd)
{
    //if (!CToolBar::Create(pParentWnd) || !LoadToolBar(IDR_MAINFRAME))
      //  return FALSE;

    // Remove this if you don't want tool tips

    SetBarStyle(GetBarStyle() |    CBRS_TOOLTIPS |
        CBRS_FLYBY | CBRS_SIZE_FIXED);

    // By setting the button style of each the marker color buttons
    // to TBBS_CHECKGROUP, we ensure that only one may be depressed
    // at a time.

    SetButtonStyle(0, TBBS_CHECKGROUP);
    SetButtonStyle(1, TBBS_CHECKGROUP);
    SetButtonStyle(2, TBBS_CHECKGROUP);
    SetButtonStyle(3, TBBS_CHECKGROUP);
    //GetToolBarCtrl().CheckButton(IDB_BLACK_MARKER);

    return TRUE;
}

// Since we're inside an ActiveX control, we don't own the message
// loop. Thus, the CWnd::PreTranslateMessage function is not called.
// As a result, we must manually call the FilterToolTipMessage()
// function to get tooltips. If you don't care about tooltips then
// you can remove this function from your toolbar.

LRESULT CScribbleBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    
    MSG msg;
    msg.hwnd = m_hWnd;
    msg.message = message;
    msg.wParam = wParam;
    msg.lParam = lParam;

    GetCursorPos(&msg.pt);
    FilterToolTipMessage(&msg);

    return CToolBar::WindowProc(message,wParam,lParam);
}
void CScribbleFrame::RefreshDocLoadStatus ()
{
	if (GetActiveDocument())
	{
					std::string strMsg = "";
					int prog = -1;
					((CAvocadoDoc*)GetActiveDocument())->GetDocumentStatus(strMsg,prog);

					//if (abs(s_last_prog - prog) > 5)
					{
						s_last_prog = prog;
						if (GetActiveView())
						{
							// the view does not get the ondraw/onpaing msgs..
							// what the f... ?!
							GetActiveView()->Invalidate ();
							//GetActiveAvocadoView()->UpdateWindow ();
							((CAvocadoView*)GetActiveView())->RedrawWindow ();
						}
					//	GetActiveAvocadoView()->InvokePaint ();
					}
				}
}


BOOL CScribbleFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	// TODO: Add your specialized code here and/or call the base class
	NMHDR *pNMHDR  = (NMHDR*)lParam;
	if ( pNMHDR->code == AVC_REFRESH_DOC_LOAD_STATUS)
	{
		RefreshDocLoadStatus();
	} else if (pNMHDR->code == AVC_REFRESH_VIEW_STATES)
	{
		//RefreshViewStates ();
	} else if (pNMHDR->code == AVC_REFRESH_MATERIAL_STATES)
	{
		//RefreshMaterialStates ();
	} else if (pNMHDR->code == AVC_REFRESH_ACTIVE_DOCUMENT)
	{
		//RefreshActiveDocument();
	} else
	if (pNMHDR->code == AVC_REFRESH_VIEW_SELECTION)
	{
		//RefreshViewSelection();
	}
	else if (pNMHDR->code == TVN_ENDLABELEDIT)
	{
	//	OnRenameElement  ();
		return true;
	}
	if (pNMHDR->code == TVN_SELCHANGED)
	{
		//OnClassViewSelection(pNMHDR,pResult);
		return true;
	}
#if 0
	if (pNMHDR->code == ID_PROPERTIES)
	{
		//OnTakeSnapshot ();
	} 
	else if (pNMHDR->code == ID_HTML_EDITOR_SAVE)
	{
		//OnHtmlEditorSave ();
	} 
	else if (pNMHDR->code == ID_HTML_EDITOR_BACK)
	{
		//OnLayoutBackpage();
	}
	else if (pNMHDR->code == ID_HTML_EDITOR_NEXT)
	{
	//	OnLayoutNextpage();
	}
#endif
	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}

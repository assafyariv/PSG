
// SrvrItem.cpp : implementation of the CSDISampleSrvrItem class
//

#include "stdafx.h"
#include "Avocado.h"
#include "MainFrm.h"
#include "AvocadoDoc.h"
#include "SrvrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CSDISampleSrvrItem implementation

IMPLEMENT_DYNAMIC(CAvocadoSrvrItem, CDocObjectServerItem)

	CAvocadoSrvrItem::CAvocadoSrvrItem(CAvocadoDoc* pContainerDoc)
	: COleServerItem(pContainerDoc, TRUE)
{
	GetDataSource()->DelayRenderFileData(CF_TEXT);
	// TODO: add one-time construction code here
	//  (eg, adding additional clipboard formats to the item's data source)
}

CAvocadoSrvrItem::~CAvocadoSrvrItem()
{
	// TODO: add cleanup code here
}

void CAvocadoSrvrItem::Serialize(CArchive& ar)
{
	// CSDISampleSrvrItem::Serialize will be called by the framework if
	//  the item is copied to the clipboard.  This can happen automatically
	//  through the OLE callback OnGetClipboardData.  A good default for
	//  the embedded item is simply to delegate to the document's Serialize
	//  function.  If you support links, then you will want to serialize
	//  just a portion of the document.

	if (!IsLinkedItem())
	{
		CAvocadoDoc* pDoc = GetDocument();
		ASSERT_VALID(pDoc);
		if (pDoc)
			pDoc->Serialize(ar);
	}
}

BOOL CAvocadoSrvrItem::OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize)
{
	// Most applications, like this one, only handle drawing the content
	//  aspect of the item.  If you wish to support other aspects, such
	//  as DVASPECT_THUMBNAIL (by overriding OnDrawEx), then this
	//  implementation of OnGetExtent should be modified to handle the
	//  additional aspect(s).

	if (dwDrawAspect != DVASPECT_CONTENT)
		return COleServerItem::OnGetExtent(dwDrawAspect, rSize);

	// CSDISampleSrvrItem::OnGetExtent is called to get the extent in
	//  HIMETRIC units of the entire item.  The default implementation
	//  here simply returns a hard-coded number of units.

	// TODO: replace this arbitrary size

	rSize = CSize(3000, 3000);   // 3000 x 3000 HIMETRIC units

	return TRUE;
}

BOOL CAvocadoSrvrItem::OnDraw(CDC* pDC, CSize& rSize)
{
	if (!pDC)
		return FALSE;
	if (GetDocument()->IsInPlaceActive ())
		return TRUE;
	// Remove this if you use rSize
	UNREFERENCED_PARAMETER(rSize);

	// TODO: set mapping mode and extent
	//  (The extent is usually the same as the size returned from OnGetExtent)
	pDC->SetMapMode(MM_ANISOTROPIC);
	pDC->SetWindowOrg(0,0);
	pDC->SetWindowExt(3000, 3000);

	//CMetaFileDC* pmeta = (CMetaFileDC*)pDC;
	//#if 0
	CAvocadoDoc* pDoc = GetDocument();

	CString tempFileName = theApp.GetAppSessionTempFolder () + CString ("\\printpreview.bmp");
	char dst[MAX_PATH];
	::wcstombs (dst,tempFileName.GetString (),MAX_PATH);
	string fileParam = "string filename="+string (dst) + string (",int x=300,int y=300,bool forceOptix=1;") ;//+ string (ssizy)+ string(";");

	CMainFrame* mv ;

	//	if (this->IsLinkedItem())
	{
		if (theApp.GetMainWnd ())
		{
			mv  = (CMainFrame*)theApp.GetMainWnd ();

			theApp.GetMainWnd ()->ShowWindow (SW_SHOW);
			//theApp.GetMainWnd ()->UpdateWindow();
		}

	}
	//else

	int vid = 0;//mv->GetActiveViewID ();
	if (vid > -1)
	{

		 GetDocument()->AvocadoInvokeView ("SaveViewToBitmapFile",vid,fileParam.c_str());

	}

	CWindowDC dcWindow(0);//theApp.GetMainWnd ()->GetActiveWindow ());

	//dcWindow.SetMapMode (
	CDC dc;
	dc.CreateCompatibleDC(&dcWindow);

	CBitmap bmp;
	// bmp.CreateCompatibleBitmap(&dcWindow, 3000, 3000);

	bmp.m_hObject=(HBITMAP)::LoadImage(NULL,tempFileName.GetString(),
		IMAGE_BITMAP,300,300,LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_VGACOLOR /*| LR_LOADTRANSPARENT*/);

	CBitmap* pOldBitmap = dc.SelectObject(&bmp);

	//dc.FillSolidRect(0, 0, 3000, 3000, 0xFF);

	/* RECT pRect ;
	pRect.bottom = 0;
	pRect.left = 0;
	pRect.right = 300;
	pRect.top = 300;
	pDoc->OnDrawThumbnail (dc,&pRect);*/

	pDC->StretchBlt(0, 0, 3000, 3000, &dc, 0, 0, 300, 300, SRCCOPY);


	dc.SelectObject(pOldBitmap);


	//if (pDoc)
	//pDoc->OnDrawThumbnail (*pDC,&pRect);
	// TODO: add drawing code here.  Optionally, fill in the HIMETRIC extent.
	//  All drawing takes place in the metafile device context (pDC).
	if (theApp.GetMainWnd ())
	{
		//			if (!IsLinkedItem())
		{
			//				GetDocument()->ActivateInPlace ();
		}
		theApp.GetMainWnd ()->ShowWindow (SW_HIDE);
		theApp.GetMainWnd ()->UpdateWindow();
	}
	//#endif
	return TRUE;
}

BOOL CAvocadoSrvrItem::OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile)
{
	ENSURE(lpFormatEtc != NULL);
	if (lpFormatEtc->cfFormat != CF_TEXT)
		return COleServerItem::OnRenderFileData(lpFormatEtc, pFile);

	BOOL bResult = FALSE;
	if (GetDocument() != NULL)
	{
		TRY
		{
			// save as text
			CArchive ar(pFile, CArchive::store);
			GetDocument()->Serialize (ar);
			ar << (BYTE)'\0';   // terminate with NUL character
			bResult = TRUE;
		}
		END_TRY
	}
	return bResult;
}
// CSDISampleSrvrItem diagnostics

#ifdef _DEBUG
void CAvocadoSrvrItem::AssertValid() const
{
	COleServerItem::AssertValid();
}

void CAvocadoSrvrItem::Dump(CDumpContext& dc) const
{
	COleServerItem::Dump(dc);
}
#endif



void CAvocadoSrvrItem::OnDoVerb(LONG iVerb)
{
	// TODO: Add your specialized code here and/or call the base class
	if (iVerb == 9)
	{
		NMHDR pNMHDR; 
		pNMHDR.code = ID_HTML_EDITOR_BACK;
		pNMHDR.hwndFrom = theApp.GetMainWnd ()->GetActiveWindow ()->GetSafeHwnd ();

		theApp.GetMainWnd()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
	}
	COleServerItem::OnDoVerb(iVerb);
}

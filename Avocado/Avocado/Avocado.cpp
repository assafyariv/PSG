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

// Avocado.cpp : Defines the class behaviors for the application.
//
#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "CElevator.h"
#include "Avocado.h"
#include "MainFrm.h"
#include "IpFrame.h"
#include "AvocadoDoc.h"
#include "AvocadoView.h"
#include "../AvocadoEngine/AvocadoAppInterface.h"
#include "CWebBrowser2.h"
#include "AvocadoPathUtils.h"
#include "AvocadoOptions.h"
#include "custsite.h"
#include <sstream>
#include <ctime>
#include "afxwin.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

class CSplashWnd : public CDialog
{
public:
	CSplashWnd();
	void Show ( );
	void Kill ();
	bool m_visible;
	// Dialog Data
	enum { IDD = IDD_SPLASHWND };
};

CSplashWnd::CSplashWnd() : CDialog(CSplashWnd::IDD)
{
	m_visible = false;
}
void CSplashWnd::Show ( )
{
	if (Create(IDD_SPLASHWND,GetDesktopWindow()))
	{
		m_visible = true;
		ShowWindow (SW_SHOWNORMAL);

	}
}
void CSplashWnd::Kill ( )
{
	if (m_visible)
	{
		DestroyWindow ();
	}
}



class CProductKeyDlg : public CDialogEx
{
public:
	CProductKeyDlg();

	// Dialog Data
	enum { IDD = IDD_PRODUCTKEY };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual  BOOL DestroyWindow();
	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	BOOL m_isValid;
	int m_key;
	unsigned long m_avocadodays;
	CString langLocale;
	afx_msg void OnTimertog();
};
BOOL CProductKeyDlg::DestroyWindow() 
{ 
	TCHAR inputStr[30];
	int ikey = 0;
	GetDlgItemText(IDC_EDIT1, inputStr,30); 
#if UNICODE
	wstringstream ss (inputStr);
#else
	stringstream ss (inputStr);
#endif
	ss >> ikey;

	/* critical section*/
	m_isValid = (ikey == AVOCADO_KEY_FULL) || (ikey == AVOCADO_KEY_PERIOD);
	m_key = ikey;
	time_t t = time(0);   // get time now 
    struct tm * now = localtime( & t ); 
	m_avocadodays = now->tm_yday + 365*now->tm_year;

	/* end critcal*/
	return CDialog::DestroyWindow(); 
} 

CProductKeyDlg::CProductKeyDlg() : CDialogEx(CProductKeyDlg::IDD)
{
	m_isValid = FALSE;
	m_avocadodays = 0;
	langLocale = CString (L"English");
}

void CProductKeyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CProductKeyDlg, CDialogEx)
END_MESSAGE_MAP()


class CSimpleInputDlg : public CDialogEx
{
public:
	CSimpleInputDlg(CString title, CString prompt);
	CString GetResult ();
	// Dialog Data
	enum { IDD = IDD_SIMPLE_PROMPT};

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual  BOOL DestroyWindow();
	// Implementation
	CString m_title;
	CString m_prompt;
	CString m_result;
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimertog();
	CEdit m_inputPrompt;
	CStatic m_promptText;
	virtual BOOL OnInitDialog();
};
CString CSimpleInputDlg::GetResult ()
{
	return m_result;
}
BOOL CSimpleInputDlg::DestroyWindow() 
{ 
	m_inputPrompt.GetWindowTextW(m_result);
	return CDialog::DestroyWindow(); 
} 

CSimpleInputDlg::CSimpleInputDlg(CString title, CString prompt) : CDialogEx(CSimpleInputDlg::IDD)
{
	m_title = title;
	m_prompt = prompt;
}

void CSimpleInputDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_inputPrompt);
	DDX_Control(pDX, IDC_PROMPT_TEXT, m_promptText);
}

BOOL CSimpleInputDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	this->SetWindowTextW (m_title.GetString ());
	m_promptText.SetWindowTextW (m_prompt.GetString ());

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BEGIN_MESSAGE_MAP(CSimpleInputDlg, CDialogEx)
END_MESSAGE_MAP()


class CMaterialStateSelectDlg : public CDialogEx
{
public:
	CMaterialStateSelectDlg();

	// Dialog Data
	enum { IDD = IDD_MATERIALSTATE_SELECT };
	int GetResult ();
	void SetStates (std::vector<std::string>);
private:
	std::vector<std::string> m_stateList ;
	int m_selection;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual  BOOL DestroyWindow();
	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimertog();
	CComboBox m_statesCombo;
	virtual BOOL OnInitDialog();
};

BOOL CMaterialStateSelectDlg::DestroyWindow() 
{ 
	m_selection = m_statesCombo.GetCurSel ();
	return CDialog::DestroyWindow(); 
} 

int CMaterialStateSelectDlg::GetResult ()
{
	return m_selection;
}

void CMaterialStateSelectDlg::SetStates (std::vector<std::string> msv)
{
    m_stateList = msv;
}

CMaterialStateSelectDlg::CMaterialStateSelectDlg() : CDialogEx(CMaterialStateSelectDlg::IDD)
{
}

BOOL CMaterialStateSelectDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	 m_statesCombo.Clear ();//RemoveAllItems ();
		for (size_t i=0; i < m_stateList.size();i++)
		{
			wchar_t vuf[MAX_PATH];
			::mbstowcs (vuf,m_stateList[i].c_str(),MAX_PATH);
			m_statesCombo.AddString  (vuf);
		}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CMaterialStateSelectDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_statesCombo);
}

BEGIN_MESSAGE_MAP(CMaterialStateSelectDlg, CDialogEx)
END_MESSAGE_MAP()



// CAvocadoApp

BEGIN_MESSAGE_MAP(CAvocadoApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CAvocadoApp::OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, &CWinAppEx::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CWinAppEx::OnFileOpen)
	ON_COMMAND(ID_FILE_IMPORT2, &CAvocadoApp::OnFileImport)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	//	ON_COMMAND(ID_FILE_SAVE_AS, &CAvocadoApp::OnFileSaveAs)
	ON_COMMAND(ID_FILE_SAVE_PAGE, &CAvocadoApp::OnFileSavePage)
	ON_COMMAND(ID_FILE_PUBLISH, &CAvocadoApp::OnFilePublish)
	ON_COMMAND(ID_PUBLISH_TOLOCALARCHIVE, &CAvocadoApp::OnPublishTolocalarchive)
	ON_COMMAND(ID_PUBLISH_TOSERVER, &CAvocadoApp::OnPublishToserver)
END_MESSAGE_MAP()

BOOL CAvocadoApp::DoPromptImportFileName(std::vector<std::pair<CString,CString>> &filters,
	CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate, int extIndx)
{
	CFileDialog dlgFile(bOpenFileDialog, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0);

	CString title;
	ENSURE(title.LoadString(nIDSTitle));

	dlgFile.m_ofn.Flags |= lFlags;

	CString strFilter;
	CString strDefault;
	if (extIndx == -1)
	{
		for (size_t i=0;i<filters.size();i++)
		{
			CString ImportFilter = filters[i].first;
			CString FilterExt = filters[i].second;

			strFilter += ImportFilter;
			strFilter += (TCHAR)'\0';   // next string please
			strFilter += FilterExt;
			strFilter += (TCHAR)'\0';   // last string
			dlgFile.m_ofn.nMaxCustFilter++;
		}
	}
	else
	{
		CString ImportFilter = filters[extIndx].first;
		CString FilterExt = filters[extIndx].second;

		strFilter += ImportFilter;
		strFilter += (TCHAR)'\0';   // next string please
		strFilter += FilterExt;
		strFilter += (TCHAR)'\0';   // last string
		dlgFile.m_ofn.nMaxCustFilter++;
	}
	// append the "*.*" all files filter
	CString allFilter;
	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);

	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
	return nResult == IDOK;
}
void CAvocadoApp::SendAvocadoDocMessage ( const TCHAR* msgName, const TCHAR* msgParams)
{
	CDocument* cdoc = ((CFrameWndEx*)GetMainWnd())->GetActiveDocument ();
	CAvocadoDoc *cad = (CAvocadoDoc*)cdoc;
	cad->AvocadoInvokeDocW (msgName,cad->GetAvoID (),msgParams);
}
void CAvocadoApp::PublishDocumentFile (LPCTSTR filename)
{
	if (m_pMainWnd)
	{
		CString newName;
		vector<pair<CString,CString>> avfilt;
		avfilt.push_back (pair<CString,CString>(L".avc",L".avc"));
		if (!DoPromptImportFileName(avfilt,newName, AFX_IDS_SAVEFILECOPY,
			OFN_HIDEREADONLY , FALSE, NULL))
			return; // open cancelled

		CDocument* cdoc = ((CFrameWndEx*)GetMainWnd())->GetActiveDocument ();
		CAvocadoDoc *cad = (CAvocadoDoc*)cdoc;
		cad->SetPublishing (true);
		cad->DoSave (newName,true);
		cad->SetPublishing (false);
	}	
}


int CAvocadoApp::RaiseMaterialStateDlg (vector<string> kk)
{
	int res = -1;
	CMaterialStateSelectDlg msdlg;
	msdlg.SetStates (kk);
	if (msdlg.DoModal() == IDOK)
	{
		res = msdlg.GetResult ();
	}
	return res;
}

void
	CAvocadoApp::OnFileImport ()
{
	
	CString newName;
	if (!DoPromptImportFileName(m_importFilters,newName, AFX_IDS_OPENFILE,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL))
		return; // open cancelled

	AfxGetApp()->OpenDocumentFile(newName);
}
// CAvocadoApp construction
CDocument* CAvocadoApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	CDocTemplate* cdt = m_pDocManager->GetBestTemplate(lpszFileName); 
	CString filterExt;
	if (cdt)
		cdt->GetDocString (filterExt,CDocTemplate::filterExt);
	TCHAR *ext;
	ext = ATL::AtlFindExtension (lpszFileName);
	{
		if (filterExt == ext)
			return CWinApp::OpenDocumentFile(lpszFileName);
		else
		{
			if (GetAppMode () == VIEWER_APP)
				::AfxMessageBox (_T("File format not supported in viewer.\nPlease contact customer support for more information"));
			else
				ImportFile (lpszFileName);
		}
	}
	return NULL;
}
void CAvocadoApp::ImportFile (LPCTSTR lpszFileName)
{
	((CMainFrame*)m_pMainWnd)->ImportFile ( lpszFileName );
}
CAvocadoApp::CAvocadoApp()
{
	m_appMode = AUTHORING_APP;
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("Avocado.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CAvocadoApp object

CAvocadoApp theApp;


CString CAvocadoApp::GetAppSessionNextTempFileName (CString extension)
{
	int id = GetInt (_T("NextTempFileID"),0);
	wstringstream wsc;
	wsc << id;
	wstring wss = wsc.str();
	CString idStr (wss.c_str());

	CString tempFileName = GetAppSessionTempFolder() + "avocadoTempFile"+idStr+extension;
	//(L"C:\\Temp\\Images\\newimage22.jpg");
	id++;
	WriteInt (_T("NextTempFileID"),id);

	return tempFileName;
}


// CAvocadoApp initialization
void CAvocadoApp::RefreshActiveDocument ()
{
	// ASSAF : TODO : MOVE THIS TO MAINFRAME AS NOTIFICATION
	if (m_pMainWnd)
	{
		NMHDR pNMHDR; 
		pNMHDR.code = AVC_REFRESH_ACTIVE_DOCUMENT;
		pNMHDR.hwndFrom = (m_pMainWnd)->GetSafeHwnd();


		m_pMainWnd->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
	}
	//((CMainFrame*)(this->GetMainWnd()))->RefreshActiveDocument();
}

CString CAvocadoApp::GetAppStorageFolder ()
{
	CString path = CAvocadoPathUtils::GetAppDataPath (CString("AvocadoApp"));
	path += CString (_T("\\Storage\\"));
	DWORD dwAttrib = GetFileAttributes(path); 
	if (!(dwAttrib != INVALID_FILE_ATTRIBUTES &&  
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
	{
		::CreateDirectory(path,0);
	}
	return path;
}
CString CAvocadoApp::GetAppStorageGalleryFolder ()
{
	CString path = GetAppStorageFolder() + CString ("Galleries\\");
	DWORD dwAttrib = GetFileAttributes(path); 
	if (!(dwAttrib != INVALID_FILE_ATTRIBUTES &&  
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
	{
		::CreateDirectory(path,0);
	}
	return path;
}
	
BOOL CAvocadoApp::LoadPictureFile(LPCTSTR    szFile,
                     LPPICTURE* pgpPicture)
{
    // open file
    HANDLE hFile = CreateFile(szFile,
                              GENERIC_READ,
                              0,
                              NULL,
                              OPEN_EXISTING,
                              0,
                              NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        ::AfxMessageBox (_T("Could not read file"));
        return FALSE;
    }

    // get file size
    DWORD dwFileSize = GetFileSize(hFile, NULL);

    if (dwFileSize == (DWORD)-1)
    {
        CloseHandle(hFile);
        AfxMessageBox (_T("File seems to be empty"));
        return FALSE;
    }

    LPVOID pvData = NULL;

    // alloc memory based on file size
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, dwFileSize);

    if (hGlobal == NULL)
    {
        CloseHandle(hFile);
        AfxMessageBox (_T("Could not allocate memory for image"));
        return FALSE;
    }

    pvData = GlobalLock(hGlobal);

    if (pvData == NULL)
    {
        GlobalUnlock(hGlobal);
        CloseHandle(hFile);
        AfxMessageBox (_T("Could not lock memory for image"));
        return FALSE;
    }

    DWORD dwBytesRead = 0;

    // read file and store in global memory
    BOOL bRead = ReadFile(hFile,
                          pvData,
                          dwFileSize,
                          &dwBytesRead,
                          NULL);

    GlobalUnlock(hGlobal);
    CloseHandle(hFile);

    if (!bRead)
    {
        AfxMessageBox (_T("Could not read file"));
        return FALSE;
    }

    LPSTREAM pstm = NULL;

    // create IStream* from global memory
    HRESULT hr = CreateStreamOnHGlobal(hGlobal,
                                       TRUE,
                                       &pstm);

    if (!(SUCCEEDED(hr)))
    {
        AfxMessageBox (_T("CreateStreamOnHGlobal() failed"));

        if (pstm != NULL)
            pstm->Release();
            
        return FALSE;
    }

    else if (pstm == NULL)
    {
        AfxMessageBox (_T("CreateStreamOnHGlobal() failed"));
        return FALSE;
    }

	// Create IPicture from image file
	if (*pgpPicture)
		(*pgpPicture)->Release();

    hr = ::OleLoadPicture(pstm,
                          dwFileSize,
                          FALSE,
                          IID_IPicture,
                          (LPVOID *)&(*pgpPicture));

    if (!(SUCCEEDED(hr)))
    {
    	pstm->Release();
        AfxMessageBox(_T("Could not load image (hr failure)"));
        return FALSE;
    }

    else if (*pgpPicture == NULL)
    {
    	pstm->Release();
        AfxMessageBox(_T("Could not load image (pgpPicture failure)"));
        return FALSE;
    }

    pstm->Release();

    return TRUE;  //Made it ...!
}

void CAvocadoApp::ResizePicture (LPCTSTR infile,LPCTSTR outfile,int w,int h)
{
	int iNewWidth = w;
	int iNewHeight = h;

	CImage image;
	image.Load(infile);
	if (this->GetMainWnd())
	{
		
		CDC *screenDC = ((CMainFrame*)GetMainWnd())->GetActiveView()->GetDC ();
		CDC pMDC ;
		pMDC.CreateCompatibleDC(screenDC);

		CBitmap *pb = new CBitmap;
		pb->CreateCompatibleBitmap(screenDC, iNewWidth, iNewHeight);

		CBitmap *pob = pMDC.SelectObject(pb);
		image.StretchBlt(pMDC.m_hDC,0, 0, iNewWidth, iNewHeight, 0, 0, image.GetWidth(), image.GetHeight(), SRCCOPY);
		pMDC.SelectObject(pob);
	
		CImage new_image;
		new_image.Attach((HBITMAP)(*pb));
		new_image.Save(outfile);
		new_image.Detach();
		//ReleaseDC(screenDC);
	}
}
	
BOOL CAvocadoApp::SavePictureFile(LPCTSTR    szFile,
                     LPPICTURE* pgpPicture)
{
  
    // get file size
	LONG dwFileSize = 0;//(*pgpPicture)->get_Height() * 
   /* if (dwFileSize == (DWORD)-1)
    {
        CloseHandle(hFile);
        AfxMessageBox (L"File seems to be empty");
        return FALSE;
    }*/

    LPVOID pvData = NULL;

    // alloc memory based on file size
    HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, 500*1000);

    if (hGlobal == NULL)
	{
        AfxMessageBox (_T("Could not allocate memory for image"));
        return FALSE;
    }
	 pvData = GlobalLock(hGlobal);

    if (pvData == NULL)
    {
        GlobalUnlock(hGlobal);
        //CloseHandle(hFile);
        AfxMessageBox (_T("Could not lock memory for image"));
        return FALSE;
    }
 LPSTREAM pstm = NULL;

    // create IStream* from global memory
    HRESULT hr = CreateStreamOnHGlobal(hGlobal,
                                       TRUE,
                                       &pstm);

    if (!(SUCCEEDED(hr)))
    {
        AfxMessageBox (_T("CreateStreamOnHGlobal() failed"));

        if (pstm != NULL)
            pstm->Release();
            
        return FALSE;
    }

    else if (pstm == NULL)
    {
        AfxMessageBox (_T("CreateStreamOnHGlobal() failed"));
        return FALSE;
    }

	(*pgpPicture)->SaveAsFile (pstm,false,&dwFileSize);
	
	//write stream to file...

	  // open file
    HANDLE hFile = CreateFile(szFile,
                              GENERIC_WRITE,
                              0,
                              NULL,
                              CREATE_ALWAYS,
                              0,
                              NULL);

    if (hFile == INVALID_HANDLE_VALUE)
    {
        ::AfxMessageBox (_T("Could not write file"));
        return FALSE;
    }



 /*   pvData = GlobalLock(hGlobal);

    if (pvData == NULL)
    {
        GlobalUnlock(hGlobal);
        CloseHandle(hFile);
        AfxMessageBox (L"Could not lock memory for image");
        return FALSE;
    }
	*/
    DWORD dwBytesRead = 0;

    // read file and store in global memory
    BOOL bRead = WriteFile(hFile,
                          pvData,
                          dwFileSize,
                          &dwBytesRead,
                          NULL);

    GlobalUnlock(hGlobal);
    CloseHandle(hFile);

    if (!bRead)
    {
        AfxMessageBox (_T("Could not write file"));
        return FALSE;
    }

  
    pstm->Release();

    return TRUE;  //Made it ...!
}

void CAvocadoApp::ClearAppSessionTempFolder ()
{
	if (!m_appDataFolder.IsEmpty())
	{
		if (::GetFileAttributes (m_appDataFolder) != 0xFFFFFF)
		{
				WIN32_FIND_DATA fd;
				HANDLE  hFind = FindFirstFile(m_appDataFolder+ CString ("\\*.*"), &fd); 
	 //little bit of old fashion C.. for fun..
				if (INVALID_HANDLE_VALUE == hFind)  
				   return ; 
 
				do 
				{ 
						CString filenamecomplete = m_appDataFolder +CString (L"\\")+ CString (fd.cFileName);
						::DeleteFile (filenamecomplete);
						TRACE1 ("Deleting temp session file: %s \n",filenamecomplete.GetString());
				} 
				while (FindNextFile(hFind, &fd) != 0); 
			
		}
	}
}
CString CAvocadoApp::GetAppSessionTempFolder ()
{
	if (m_appDataFolder.IsEmpty())
	{
		int nextSessionKey = this->GetInt (_T("NextSessionKey"),0) ;
		nextSessionKey ++;
		this->WriteInt (_T("NextSessionKey"),nextSessionKey);

		CString path = CAvocadoPathUtils::GetAppDataPath (CString("AvocadoApp"));
		std::wostringstream istr;
		istr << nextSessionKey;

		// Assuming that folder exists , created in path utils.

		/*	DWORD dwAttrib = GetFileAttributes(path); 
		if (!(dwAttrib != INVALID_FILE_ATTRIBUTES &&  
		(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
		::CreateDirectory(path,0);
		}*/

		// Create temp sub folder with uniqe sesison key.
		path +=  CString("\\") + CString (istr.str().c_str());
		DWORD dwAttrib = GetFileAttributes(path); 
		if (!(dwAttrib != INVALID_FILE_ATTRIBUTES &&  
			(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		{
			::CreateDirectory(path,0);
		}

		m_appDataFolder = path;
	}
	return m_appDataFolder;

}
#if 0
// CAvocadoApp initialization
void CAvocadoApp::RefreshViewSelection ()
{
	// ASSAF : TODO : MOVE THIS TO MAINFRAME AS NOTIFICATION
	if (GetMainWnd())
		((CMainFrame*)(this->GetMainWnd()))->RefreshViewSelection();
}
#endif
void CAvocadoApp::InitImportFilters ()
{
	m_exportFilters.push_back ( pair <CString,CString> (_T("Wavefront"),_T("*.obj")));
	m_exportFilters.push_back ( pair <CString,CString> (_T("NViDiA Format"),_T("*.nbf")));
	m_exportFilters.push_back ( pair <CString,CString> (_T("Jpeg"),_T("*.jpg")));
	m_exportFilters.push_back ( pair <CString,CString> (_T("Bitmap"),_T("*.bmp")));
	m_exportFilters.push_back ( pair <CString,CString> (_T("TIFF"),_T("*.tif")));
	m_exportFilters.push_back ( pair <CString,CString> (_T("PNG"),_T("*.png")));
	m_exportFilters.push_back ( pair <CString,CString> (_T("HTML"),_T("*.html")));

	m_importFilters.push_back ( pair <CString,CString> (_T("Collada"),_T("*.dae")));
	m_importFilters.push_back ( pair <CString,CString> (_T("3D Studio"),_T("*.3ds")));
	m_importFilters.push_back ( pair <CString,CString> (_T("VRML"),_T("*.wrl")));
	m_importFilters.push_back ( pair <CString,CString> (_T("WaveFront"),_T("*.obj")));
	m_importFilters.push_back ( pair <CString,CString> (_T("NViDia Binary"),_T("*.nbf")));

	m_imageFilters.push_back ( pair <CString,CString> (_T("Bitmap"),_T("*.bmp")));
	m_imageFilters.push_back ( pair <CString,CString> (_T("JPEG"),_T("*.jpg")));
	m_imageFilters.push_back ( pair <CString,CString> (_T("TIFF"),_T("*.tif")));
	m_imageFilters.push_back ( pair <CString,CString> (_T("PNG"),_T("*.png")));

}

class CAvocadoCommandLineInfo : public CCommandLineInfo
{
public:
	CAvocadoCommandLineInfo () : CCommandLineInfo () { m_appMode = 0; m_commit = FALSE;}
	~CAvocadoCommandLineInfo () {};

	virtual void ParseParam (LPCTSTR pszParam, BOOL flag , BOOL v) 
	{
		if(CString(pszParam) == CString (L"viewer"))
		{
			m_appMode = 1;
			return;
		} else
		if(CString(pszParam) == CString (L"Commit"))
		{
			m_commit = TRUE;
			return;
		}
		CCommandLineInfo::ParseParam (pszParam,flag,v);
	}
	unsigned int m_appMode;
	BOOL m_commit;
};

CString CAvocadoApp::RaiseImageOpenDialog()
{
	CString filename = CString  (L"");
	if (!DoPromptImportFileName(m_imageFilters,filename, AFX_IDS_OPENFILE,
		OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, TRUE, NULL))
	{
		return CString ("");
	}
	return filename;
}

// {DA96493C-33A3-4D50-91A5-41DFF6287FBF}
static const CLSID clsid =
{ 0xDA96493C, 0x33A3, 0x4D50, { 0x91, 0xA5, 0x41, 0xDF, 0xF6, 0x28, 0x7F, 0xBF } };

const GUID CDECL _tlid = { 0x207B9E5E, 0x4DF4, 0x405B, { 0xA5, 0x21, 0x30, 0x61, 0x4A, 0x80, 0x4E, 0x8E } };
const WORD _wVerMajor = 1;
const WORD _wVerMinor = 0;
/////////////////////////////////////////////////////////////////////////////
//

CAvocadoDocTemplate::CAvocadoDocTemplate()
	: CSingleDocTemplate(IDR_MAINFRAME,
	RUNTIME_CLASS(CAvocadoDoc),
	RUNTIME_CLASS(CMainFrame), // standard MDI child frame
	RUNTIME_CLASS(CAvocadoView))
{
}

CDocTemplate::Confidence CAvocadoDocTemplate::MatchDocType(LPCTSTR lpszPathName,
	CDocument*& rpDocMatch)
{
	CDocTemplate::Confidence ret;

	ret = CSingleDocTemplate::MatchDocType(lpszPathName, rpDocMatch);

	if (ret == CDocTemplate::yesAttemptForeign)
		ret = CDocTemplate::noAttempt;

	return ret;
}
// Identical to COleTemplateServer::OnCreateObject() except for
// the call to OpenDocumentFile() with the last parameter set to
// TRUE instead of FALSE.
CCmdTarget* CMyTemplateServer::OnCreateObject()
{
	ASSERT_VALID(this);
	ASSERT_VALID(m_pDocTemplate);

	// Save application user control status
	BOOL bUserCtrl = AfxOleGetUserCtrl();

	// Create visible doc/view/frame set
	CDocument* pDoc;
	pDoc = m_pDocTemplate->OpenDocumentFile(NULL,FALSE);// FALSE);

	// Restore application's user control status
	AfxOleSetUserCtrl(bUserCtrl);

	if (pDoc != NULL)
	{
		ASSERT_VALID(pDoc);
		ASSERT_KINDOF(CDocument, pDoc);

		// All new documents created by OLE start out modified
		pDoc->SetModifiedFlag();
	}
	return pDoc;
}

int CAvocadoApp::GetAvocadoRegValue (LPCTSTR keyname,bool try_user)
{
	{
		CRegKey regKey;
		LONG result = regKey.Open( HKEY_CLASSES_ROOT, TEXT("SOFTWARE\\BexxonPrivate"), KEY_READ );

		if  (try_user)
		{
			if (result !=ERROR_SUCCESS)
				// UAC , lets try the user..
				result = regKey.Open( HKEY_CURRENT_USER, TEXT("SOFTWARE\\BexxonPrivate"), KEY_READ );
		}
		if( result == ERROR_SUCCESS )
		{ 
			LONG valResult;
			DWORD value;
			valResult = regKey.QueryDWORDValue (keyname,value);
			regKey.Close();
			if (valResult == ERROR_SUCCESS)
				return int(value);
		} 
		return int(0);
	}
}
void CAvocadoApp::SetAvocadoRegValue (LPCTSTR keyname,DWORD keyval,bool try_user)
{
	{
		CRegKey regKey;
		LONG result = regKey.Open( HKEY_CLASSES_ROOT, TEXT("SOFTWARE\\BexxonPrivate"), KEY_WRITE );
		if (result != ERROR_SUCCESS)
		{
			result = regKey.Create (HKEY_CLASSES_ROOT, TEXT("SOFTWARE\\BexxonPrivate"));
			if (result ==  ERROR_SUCCESS)
				 result = regKey.Open( HKEY_CLASSES_ROOT, TEXT("SOFTWARE\\BexxonPrivate"), KEY_WRITE );
		}
		if (try_user)
		{
			//uac - try current user..
			if (result != ERROR_SUCCESS)
			{
				result = regKey.Open( HKEY_CURRENT_USER, TEXT("SOFTWARE\\BexxonPrivate"), KEY_WRITE );
				if (result != ERROR_SUCCESS)
				{
					result = regKey.Create (HKEY_CURRENT_USER, TEXT("SOFTWARE\\BexxonPrivate"));
					if (result ==  ERROR_SUCCESS)
						 result = regKey.Open( HKEY_CURRENT_USER, TEXT("SOFTWARE\\BexxonPrivate"), KEY_WRITE );
				}
			}
		}
		if( result == ERROR_SUCCESS )
		{ 
			regKey.SetDWORDValue (keyname,keyval);		
			regKey.Close();
		} 
		return ;
	}
}
/* This critical thingy should be very protected */
static BOOL handleAppRegistration (int &licKey,int perKey,  long &days)
{
	if (licKey == int (AVOCADO_KEY_PERIOD))
	{
		 time_t t = time(0);   // get time now 
		 struct tm * now = localtime( & t ); 
		 long avocadodays = now->tm_yday + 365*now->tm_year;
		 //compare to saved data.
		 if (avocadodays - (long)perKey < 30)
		 {
			 days = -1;
			 return TRUE;
		 }
		// check dates..
		AfxMessageBox (_T("Trial period has ended.\n please visit our website for more information."));
		AfxPostQuitMessage (0);
		return FALSE;
	}
	else if (licKey != int ( AVOCADO_KEY_FULL))
	{
		BOOL isAdmin = FALSE;
		if (CElevator::IsAdmin (isAdmin) == ERROR_SUCCESS)
		{
			if (isAdmin == FALSE)
			{
			// Restart as admin.
				CElevator::ElevateMe (AfxGetMainWnd()->GetSafeHwnd ());
				AfxPostQuitMessage (0);
				return FALSE;
			}
		}
		// if we reach this part , user is admin privelaged , so ask for registration.
		CProductKeyDlg dialog;
		if (dialog.DoModal() == IDOK) { 

			if (dialog.m_isValid)
			{
				licKey = dialog.m_key;
				days = dialog.m_avocadodays;
				// register this guy..
				return TRUE;
			}
			else
			{
				AfxMessageBox (_T("Wrong product key.\n"));
				AfxPostQuitMessage (0);
				return FALSE;
			}

		} 
		else
			//if (licKey != 0) // remove this line in real life of course :)
		{
			AfxMessageBox (_T("You must be registered in order to use avocado.\n"));

			AfxPostQuitMessage (0);
			return FALSE;
		}
		
	}
	return TRUE;
}
BOOL CAvocadoApp::InitInstance()
{
#ifdef _DEBUG
	/*_CrtSetBreakAlloc(779338);
	_CrtSetBreakAlloc(268609);
	_CrtSetBreakAlloc(268586);
	_CrtSetBreakAlloc(740025);
	_CrtSetBreakAlloc(212913);*/
	//	__debugbreak();
	afxMemDF = allocMemDF | delayFreeMemDF | checkAlwaysMemDF;
#endif
#ifndef _AVOCADO_WIN8
	afxAmbientActCtx = FALSE; // this baby will disable SxS activation context used in windows7. I add this line 
	// to see if this interffere with Optix in some way. god knows why.
	// see http://blogs.msdn.com/b/junfeng/archive/2006/03/19/sxs-activation-context-activate-and-deactivate.aspx
#endif
	//Check langauge and fit resource dlls.
	m_langID = (LangMode)GetAvocadoRegValue (_T("LanguageCode"));
	if (m_langID == 0)
	{
		// development .. dont load res dlls.
	}

	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
#ifdef _AVOCADO_DISAVLE_DHTML_CMD_MAPPINF
	AfxEnableControlContainer();
#else
	CCustomOccManager *pMgr = new CCustomOccManager;

	// Create an IDispatch class for extending the Dynamic HTML Object Model 
	m_pDispOM = new CImpIDispatch;

	// Set our control containment up but using our control container 
	// management class instead of MFC's default
	AfxEnableControlContainer(pMgr);
#endif
	EnableTaskbarInteraction(FALSE);
	CSplashWnd splash;
	splash.Show();
	bool s_mt = false;
#ifdef _AVOCADO_MT
	s_mt = true;
#endif
	InitImportFilters ();
	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();
	 
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	//SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	SetRegistryKey(_T("Bexxon Software"));
	SetRegistryBase (_T("Avocado Editor"));
	LoadStdProfileSettings(6);  // Load standard INI file options (including MRU)


	InitContextMenuManager();
	InitShellManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Initialize our magic stick.
	CString tempFolder  = GetAppSessionTempFolder ();//CAvocadoPathUtils::GetAppDataPath (CString("AvocadoApp"));
	char appFolderChar[MAX_PATH] ;
	::wcstombs (appFolderChar,tempFolder.GetString(),MAX_PATH);
	avocado::AvocadoInit(s_mt,string(appFolderChar));
	// end avocado initialization. Should really fail it this fails.


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CAvocadoDocTemplate();//CSingleDocTemplate(
	//	IDR_MAINFRAME,
	//	RUNTIME_CLASS(CAvocadoDoc),
	//	RUNTIME_CLASS(CMainFrame),       // main SDI frame window
	//	RUNTIME_CLASS(CAvocadoView));
	if (!pDocTemplate)
		return FALSE;
	pDocTemplate->SetServerInfo(
		IDR_MENU3, IDR_MENU2,
		RUNTIME_CLASS(CInPlaceFrame),
		RUNTIME_CLASS (CAvocadoView));
	/*pDocTemplate->SetServerInfo(
	IDR_MAINFRAME, IDR_MAINFRAME,
	RUNTIME_CLASS(CMainFrame));*/
	AddDocTemplate(pDocTemplate);

	// Parse command line for standard shell commands, DDE, file open
	CAvocadoCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	// Get requested app mode.
	m_appMode = (AppMode)cmdInfo.m_appMode;

	// Check licencse. Very critical code section.
	// Should be protected and encrypted.
	if (m_appMode == AUTHORING_APP)
	{
		//****///
		// This entire thing should move to a seperate and highly secured source file..
		 int licKey = GetAvocadoRegValue (_T("AvocadoLicence"));
		const int periodKey = GetAvocadoRegValue (_T("AvocadoLicencePeriod"));
		long days = -1;
		if (!handleAppRegistration (licKey,periodKey,days))
			return TRUE;
		else
		{
			SetAvocadoRegValue (_T("AvocadoLicence"),licKey);
			if (days > 0)
			{
				//register the current date..
				SetAvocadoRegValue (_T("AvocadoLicencePeriod"),days);
			}
		}
		if (cmdInfo.m_commit)
		{
			// called from installer..
			AfxPostQuitMessage (0);
			return TRUE;
		}
		// dont continue loading if registration failed.
	}

	// Enable DDE Execute open
	EnableShellOpen();
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);


	int initialShowState = SW_SHOWMAXIMIZED;
	// App was launched with /Embedding or /Automation switch.
	// Run app as automation server.
	COleTemplateServer::RegisterAll(); 
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register all OLE server factories as running.  This enables the
		//  OLE libraries to create objects from other applications

		//m_appMode = CAvocadoApp::VIEWER_APP;
		initialShowState = SW_SHOWNORMAL;
		// Don't show the main window
		return TRUE;

	} 
	// App was launched with /Unregserver or /Unregister switch.  Unregister
	// typelibrary.  Other unregistration occurs in ProcessShellCommand().
	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::AppUnregister)
	{
		UnregisterShellFileTypes();
		m_server.UpdateRegistry(OAT_DOC_OBJECT_SERVER, NULL, NULL, FALSE);
		AfxOleUnregisterTypeLib(_tlid, _wVerMajor, _wVerMinor);
	}
	// App was launched standalone or with other switches (e.g. /Register
	// or /Regserver).  Update registry entries, including typelibrary.
	else
	{

		m_server.UpdateRegistry(OAT_DOC_OBJECT_SERVER);
		RegisterShellFileTypes(TRUE);
		COleObjectFactory::UpdateRegistryAll();
		AfxOleRegisterTypeLib(AfxGetInstanceHandle(), _tlid);
	}

	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	CView* pActiveView = ((CFrameWnd*) m_pMainWnd)->GetActiveView();
	CDocument* pCurrentDoc = ((CFrameWnd*)GetMainWnd())->GetActiveDocument();


	// we initialize the doc in newdocument/opendocument , cview::initialupdate

	POSITION pos = pDocTemplate->GetFirstDocPosition();
	if (pos != NULL)
	{
		pCurrentDoc = pDocTemplate->GetNextDoc(pos);
	}

	pCurrentDoc->AddView (pActiveView);

	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen)
	{
		RefreshActiveDocument();
	}
	//avocado::SetActiveView(((CAvocadoView *)pActiveView)->GetID());
	//reinterpret_cast<CAvocadoDoc *>(pCurrentDoc)->SetCurrentView(pActiveView);
	if (GetAppMode () == VIEWER_APP)
	{
		SetRegistryBase (_T("Avocado Viewer"));
	}

	// The one and only window has been initialized, so show and update it

	if (GetAppMode () == VIEWER_APP)
		initialShowState = SW_SHOWNORMAL;

	m_pMainWnd->ShowWindow(m_nCmdShow);//initialShowState);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();
	splash.Kill();
	return TRUE;
}

int CAvocadoApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);
	avocado::AvocadoTerminate();
	int c = CWinAppEx::ExitInstance();
	return c;
}

// CAvocadoApp message handlers

// END GALLERY MANAGER CODE SECTION


// CPublishDlg dialog used for publishing
class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimertog();
	afx_msg void OnUpdateSaveDoc(CCmdUI *pCmdUI);
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
	//ON_COMMAND(IDC_TIMERTOG, &CAboutDlg::OnTimertog)
	ON_UPDATE_COMMAND_UI(IDC_SAVE_DOC, &CAboutDlg::OnUpdateSaveDoc)
END_MESSAGE_MAP()

// App command to run the dialog
void CAvocadoApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// App command to run the dialog
void CAvocadoApp::InitOptionsDialog()
{
	COptionsDlg optDlg;
	optDlg.DoModal();
}

// CPublishDlg dialog used for publishing

class CPublishDlg : public CDialogEx
{
public:
	CPublishDlg();

	// Dialog Data
	enum { IDD = IDD_PUBLISH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimertog();
	afx_msg void OnUpdateSaveDoc(CCmdUI *pCmdUI);
};

CPublishDlg::CPublishDlg() : CDialogEx(CPublishDlg::IDD)
{
}

void CPublishDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CPublishDlg, CDialogEx)

END_MESSAGE_MAP()

// CAvocadoApp customization load/save methods

void CAvocadoApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
	bNameValid = strName.LoadString(IDS_EXPLORER);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EXPLORER);
	if (GetAppMode () == AUTHORING_APP)
	{
		bNameValid = strName.LoadString(IDS_POPUP_VIEW_DEFAULT);
		ASSERT(bNameValid);
		GetContextMenuManager()->AddMenu(strName, IDR_POPUP_VIEW_DEFAULT);

		bNameValid = strName.LoadString(IDS_POPUP_VIEW_SELECTED_ELEM);
		ASSERT(bNameValid);
		GetContextMenuManager()->AddMenu(strName, IDR_POPUP_VIEW_SELECTED_ELEM);

		bNameValid = strName.LoadString(IDS_POPUP_SORT);
		ASSERT(bNameValid);
		GetContextMenuManager()->AddMenu(strName, IDR_POPUP_SORT);

	} else
	{
		bNameValid = strName.LoadString(IDS_POPUP_VIEW_DEFAULT);
		ASSERT(bNameValid);
		GetContextMenuManager()->AddMenu(strName, IDR_POPUP_VIEW_DEFAULT1);

		bNameValid = strName.LoadString(IDS_POPUP_VIEW_SELECTED_ELEM);
		ASSERT(bNameValid);
		GetContextMenuManager()->AddMenu(strName, IDR_POPUP_VIEW_SELECTED_ELEM1);

		bNameValid = strName.LoadString(IDS_POPUP_SORT);
		ASSERT(bNameValid);
		GetContextMenuManager()->AddMenu(strName, IDR_POPUP_SORT1);
	}
}

void CAvocadoApp::LoadCustomState()
{
}

void CAvocadoApp::SaveCustomState()
{
}

// CAvocadoApp message handlers

void CAboutDlg::OnUpdateSaveDoc(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}


BOOL CAvocadoApp::InitApplication()
{
	// TODO: Add your specialized code here and/or call the base class
	return CWinAppEx::InitApplication();
}


//void CAvocadoApp::PreSaveState()
//{
//	// TODO: Add your specialized code here and/or call the base class
//
//	return CWinAppEx::PreSaveState();
//}


//void CAvocadoApp::OnFileSaveAs()
//{
//	CFileDialog dlgFile(bOpenFileDialog, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, NULL, NULL, 0);
//
//	CString title;
//	ENSURE(title.LoadString(nIDSTitle));
//
//	dlgFile.m_ofn.Flags |= lFlags;
//
//	CString strFilter;
//	CString strDefault;
//	
//	for (size_t i=0;i<m_importFilters.size();i++)
//	{
//		CString ImportFilter = m_importFilters[i].first;
//		CString FilterExt = m_importFilters[i].second;
//
//		strFilter += ImportFilter;
//		strFilter += (TCHAR)'\0';   // next string please
//		strFilter += FilterExt;
//		strFilter += (TCHAR)'\0';   // last string
//		dlgFile.m_ofn.nMaxCustFilter++;
//	}
//	// append the "*.*" all files filter
//	CString allFilter;
//	VERIFY(allFilter.LoadString(AFX_IDS_ALLFILTER));
//	strFilter += allFilter;
//	strFilter += (TCHAR)'\0';   // next string please
//	strFilter += _T("*.*");
//	strFilter += (TCHAR)'\0';   // last string
//	dlgFile.m_ofn.nMaxCustFilter++;
//
//	dlgFile.m_ofn.lpstrFilter = strFilter;
//	dlgFile.m_ofn.lpstrTitle = title;
//	dlgFile.m_ofn.lpstrFile = fileName.GetBuffer(_MAX_PATH);
//
//	INT_PTR nResult = dlgFile.DoModal();
//	fileName.ReleaseBuffer();
//	return nResult == IDOK;
//	// TODO: Add your command handler code here
//}


void CAvocadoApp::OnFileSavePage()
{
	CString newName;
	if (!DoPromptImportFileName(m_exportFilters,newName, AFX_IDS_SAVEFILECOPY,
		OFN_HIDEREADONLY , FALSE, NULL))
		return; // open cancelled
	if (m_pMainWnd)
	{
		if (newName == CString (""))
		{
			AfxMessageBox (_T("File name not valid"));
			return;
		}

		((CMainFrame*)(m_pMainWnd))->ExportDocument (newName);
	}
	//AfxGetApp()->sOpenDocumentFile(newName);
	// TODO: Add your command handler code here
}


BOOL CAvocadoApp::SaveState(LPCTSTR lpszSectionName , CFrameImpl* pFrameImpl)
{
	// TODO: Add your specialized code here and/or call the base class
	if ((m_appMode == AUTHORING_APP))
		return CWinAppEx::SaveState(lpszSectionName, pFrameImpl);
	else
	{
		CString newSect (lpszSectionName);
		newSect += L"Viewer";
		return CWinAppEx::SaveState(newSect.GetBuffer(), pFrameImpl);
	}
	return true;
}


BOOL CAvocadoApp::LoadState(LPCTSTR lpszSectionName , CFrameImpl* pFrameImpl)
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL ret = true;
	if ((m_appMode == AUTHORING_APP))
	{
		ret = CWinAppEx::LoadState(lpszSectionName, pFrameImpl);
		if (m_pMainWnd)
			((CMainFrame*)(m_pMainWnd))->ShowCaptionBar ();
	} else
	{
		CString newSect (lpszSectionName);
		newSect += L"Viewer";
		return CWinAppEx::LoadState(newSect.GetBuffer(), pFrameImpl);
	}

	return ret;
}

//void CAvocadoApp::SyncRepaint (int vid)
//{
/*
std::vector<int>::iterator vit  = m_pendingViews.begin();
while (vit != m_pendingViews.end())
{
if ((*vit) == vid)
{
return;
}
}
m_pendingViews.push_back (vid);
*/
///}
BOOL CAvocadoApp::OnIdle(LONG lCount)
{
#if 0
	// Get back to this later
	// TODO: Add your specialized code here and/or call the base class
	if (m_pendingViews.size ())
	{
		std::vector<int>::iterator vit  = m_pendingViews.begin();
		while (vit != m_pendingViews.end())
		{
			//((CMainFrame*)GetMainWnd())->GetActiveAvocadoView()->InvokePaintLow();
		}
		m_pendingViews.clear ();
	}
#endif
	return CWinAppEx::OnIdle(lCount);
}


void CAvocadoApp::OnFilePublish()
{
	CPublishDlg publishDlg;
	publishDlg.DoModal();
	PublishDocumentFile (_T(""));
}


void CAvocadoApp::OnPublishTolocalarchive()
{
	CPublishDlg publishDlg;
	publishDlg.DoModal();
	PublishDocumentFile (_T(""));
}


void CAvocadoApp::OnPublishToserver()
{
	CPublishDlg publishDlg;
	publishDlg.DoModal();
	PublishDocumentFile (_T(""));
}

CString CAvocadoApp::DoPromptInputDlg (CString title,CString prompt)
{
	CSimpleInputDlg dlg (title,prompt);
	if (dlg.DoModal ()== IDOK)
	{
		return dlg.GetResult ();
	}
	return CString("");
}







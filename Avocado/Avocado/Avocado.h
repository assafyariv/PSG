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

// Avocado.h : main header file for the Avocado application
//
#pragma once

#ifndef __AFXWIN_H__
#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "idispimp.h"
#include <vector>
#include "AvocadoGallery.h"
// CAvocadoApp:
// See Avocado.cpp for the implementation of this class
//
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// This will tell the compiler to use secure versions of strcpy and perhaps also wcstombs,mbstowcs
//#if defined(_CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES)
//#undef _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES
//#endif
//#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES  1 

// CMyTemplateServer used to override OnCreateObject.
class CMyTemplateServer : public COleTemplateServer
{
	// Constructors / Destructors
public:
	CMyTemplateServer(){}
	virtual ~CMyTemplateServer(){}

	// Overrides
	virtual CCmdTarget* OnCreateObject();
};


class CAvocadoDocTemplate : public CSingleDocTemplate
{
public:
	CAvocadoDocTemplate();

	CDocTemplate::Confidence MatchDocType(LPCTSTR lpszPathName,
		CDocument*& rpDocMatch);
};

#define AVOCADO_KEY_PERIOD   8978222899
#define AVOCADO_KEY_FULL	 1474323727


class CAvocadoApp : public CWinAppEx
{
public:
	CAvocadoApp();

	// Overrides
public:
	enum AppMode 
	{ 
		AUTHORING_APP , 
		VIEWER_APP 
	};
	enum LangMode 
	{ 
		DEVEL , 
		ENGLISH,
		RUSSIAN 
	};

	const AppMode &GetAppMode () const { return m_appMode; }
	void SendAvocadoDocMessage (const TCHAR* msgName, const TCHAR* msgParams);
	// Picture utils.
	void ResizePicture (LPCTSTR infile,LPCTSTR outfile,int w,int h);
	BOOL LoadPictureFile(LPCTSTR    szFile, LPPICTURE* pgpPicture);
	BOOL SavePictureFile(LPCTSTR    szFile, LPPICTURE* pgpPicture);
	
	// Temporary folders
	CString		m_appDataFolder;
	void	    ClearAppSessionTempFolder();
	CString		GetAppSessionTempFolder ();
	CString		GetAppStorageFolder ();
	CString		GetAppStorageGalleryFolder ();
	CString     GetAppSessionNextTempFileName (CString extension);

	// Gallery Manager
	CGallerySuperManager	m_galleryManager;
	
	virtual BOOL InitInstance();
	virtual int CAvocadoApp::ExitInstance();
	
	void RefreshActiveDocument ();

	// Avocado Options.
	bool		GetAvocadoOptionBool    (std::string optionName);
	std::string	GetAvocadoOptionString  (std::string optionName);
	int			GetAvocadoOptionInt     (std::string optionName);
	float		GetAvocadoOptionFloat   (std::string optionName);

	// void SyncRepaint (int vid);


	// Import and export dialogs.
	std::vector<std::pair<CString,CString>> m_imageFilters;
	std::vector<std::pair<CString,CString>> m_importFilters;
	std::vector<std::pair<CString,CString>> m_exportFilters;
	void InitImportFilters();
	CString CAvocadoApp::RaiseImageOpenDialog();
	BOOL DoPromptImportFileName(std::vector<std::pair<CString,CString>> &filters,
		CString& fileName, UINT nIDSTitle, DWORD lFlags, BOOL bOpenFileDialog, CDocTemplate* pTemplate, int extInd = -1);
	
	// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();
	virtual void ImportFile (LPCTSTR lpszFileName);
	virtual void InitOptionsDialog ();
	afx_msg void OnAppAbout();
	afx_msg void OnFileImport();
	DECLARE_MESSAGE_MAP()
	virtual BOOL InitApplication();

	afx_msg void OnFileSavePage();
	virtual BOOL SaveState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);
	virtual BOOL LoadState(LPCTSTR lpszSectionName = NULL, CFrameImpl* pFrameImpl = NULL);
protected:
	// Registry values.
	int		GetAvocadoRegValue (LPCTSTR keyname,bool try_user=false);
	void	SetAvocadoRegValue (LPCTSTR keyname,DWORD keyval,bool try_user=false);

	AppMode				m_appMode;
	std::vector <int>   m_pendingViews;
	CMyTemplateServer   m_server;
	LangMode			m_langID;
	
public:
	CImpIDispatch       *m_pDispOM;
	virtual void PublishDocumentFile (LPCTSTR filename);
	virtual int  RaiseMaterialStateDlg (std::vector<std::string> kk);
	virtual CString DoPromptInputDlg (CString title,CString prompt);
	virtual BOOL OnIdle(LONG lCount);
	afx_msg void OnFilePublish();

	afx_msg void OnPublishTolocalarchive();
	afx_msg void OnPublishToserver();
};

extern CAvocadoApp theApp;

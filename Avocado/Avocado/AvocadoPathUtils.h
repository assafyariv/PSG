/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once

class CAvocadoPathUtils 
{
public:
	
	static CString GetAppDataPath (CString app) 
	{	
		 ITEMIDLIST* pidl;
		 
		 TCHAR AppDir[MAX_PATH];
		 HRESULT hRes = SHGetSpecialFolderLocation( NULL, CSIDL_APPDATA|CSIDL_FLAG_CREATE , &pidl );
		 if (hRes==NOERROR) 
		 { 
		  SHGetPathFromIDList( pidl, AppDir );
		 }
		  // strongly relyig ob appdir to be NULL terminated. FIXME
		 CString appPath = CString (AppDir)+CString (L"\\")+app;
		// Create main appliaction folder.
		DWORD dwAttrib = GetFileAttributes(appPath); 
		if (!(dwAttrib != INVALID_FILE_ATTRIBUTES &&  
			 (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		  {
			  ::CreateDirectory(appPath,0);
		  }

		 return appPath;
	}
	static CString GetMyDocumentsPath (CString app) 
	{	
		 ITEMIDLIST* pidl;
		 TCHAR AppDir[MAX_PATH];
		 HRESULT hRes = SHGetSpecialFolderLocation( NULL, CSIDL_PERSONAL|CSIDL_FLAG_CREATE , &pidl );
		 if (hRes==NOERROR) 
		 { 
		  SHGetPathFromIDList( pidl, AppDir );
		 }
		 // strongly relyig ob appdir to be NULL terminated. FIXME
		 CString appPath = CString (AppDir)+CString (L"\\")+app;
		 //Create main appliaction folder.
		DWORD dwAttrib = GetFileAttributes(appPath); 
		if (!(dwAttrib != INVALID_FILE_ATTRIBUTES &&  
			 (dwAttrib & FILE_ATTRIBUTE_DIRECTORY)))
		  {
			  ::CreateDirectory(appPath,0);
		  }

		 return appPath;
	}
};
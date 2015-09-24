/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
// ThumbnailHandler.h : Declaration of the thumbnail handler

#pragma once
#include "resource.h"       // main symbols

#include <atlhandler.h>
#include <atlhandlerimpl.h>
#include "..\Avocado\AvocadoDoc.h"
#include "..\Avocado\AvocadoPathUtils.h"
#include "AvocadoHandlers_i.h"

using namespace ATL;

// CThumbnailHandler

class ATL_NO_VTABLE CThumbnailHandler :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CThumbnailHandler, &CLSID_Thumbnail>,
	public IDispatchImpl<IInitializeWithFile, &__uuidof(IInitializeWithFile), &LIBID_PropSysObjects, /* wMajor = */ 1, /* wMinor = */ 0>,
	public IDispatchImpl<IInitializeWithItem, &__uuidof(IInitializeWithItem), &LIBID_ShellObjects, /* wMajor = */ 1, /* wMinor = */ 0>,
	public CThumbnailProviderImpl
{
private:
	IShellItem *tbg_pShellItem;
public:
	CThumbnailHandler()
	{
	}
	 
DECLARE_REGISTRY_RESOURCEID(IDR_THUMBNAIL_HANDLER)
DECLARE_NOT_AGGREGATABLE(CThumbnailHandler)

BEGIN_COM_MAP(CThumbnailHandler)
	COM_INTERFACE_ENTRY(IInitializeWithItem)
	COM_INTERFACE_ENTRY(IInitializeWithFile)
	COM_INTERFACE_ENTRY(IInitializeWithStream)
	COM_INTERFACE_ENTRY(IThumbnailProvider)
END_COM_MAP()

	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
		CThumbnailProviderImpl::FinalRelease();
	}
// IInitializeWithFile
	STDMETHOD(Initialize)(LPCWSTR pszFilePath, DWORD grfMode)
	{
		if (!pszFilePath)
		{
			return E_POINTER;
		}

		if (tbg_pShellItem)
		{
			return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
		}

		IShellItem *pShellItem = 0;

		HRESULT hr = SHCreateItemFromParsingName(pszFilePath, NULL, IID_PPV_ARGS(&pShellItem));

		if (S_OK == hr && !pShellItem)
		{
			hr = E_FAIL;
		}

		if (S_OK == hr)
		{
			tbg_pShellItem = pShellItem;
		}

		return hr;
	}

	// IInitializeWithItem
	STDMETHOD(Initialize)(IShellItem *psi, DWORD grfMode)
	{
		if (!psi)
		{
			return E_POINTER;
		}

		if (tbg_pShellItem)
		{
			return HRESULT_FROM_WIN32(ERROR_ALREADY_INITIALIZED);
		}

		tbg_pShellItem = psi;
		tbg_pShellItem->AddRef();

		return S_OK;
	}

protected:

	virtual HRESULT GetBitmap(UINT cx, HBITMAP *phbmp, WTS_ALPHATYPE *pdwAlpha)
	{
		if (m_pDocument == NULL)
		{
			return E_NOTIMPL;
		}
	
		if (0)//tbg_pShellItem != NULL)
		{
			// Untested code here
			LPWSTR *filename = 0;
			tbg_pShellItem->GetDisplayName (SIGDN_FILESYSPATH,filename);
			CDocument* m_pCurrentDoc = DYNAMIC_DOWNCAST(CAvocadoDoc, (CObject*) m_pDocument->GetContainer());
		    m_pCurrentDoc->SetPathName (*filename,FALSE);
			CoTaskMemFree (*filename);
		} else
		{
			//copy p_stream to a temp file.. :(
			CDocument* m_pCurrentDoc = DYNAMIC_DOWNCAST(CAvocadoDoc, (CObject*) m_pDocument->GetContainer());
			CString tempPath = CAvocadoPathUtils::GetAppDataPath (CString(L"AvocadoApp"));
			tempPath += CString ("\\avpp_temppp.ass");
		    m_pCurrentDoc->SetPathName (/*L"C:\\Temp\\avpp_temppp.ass"*/
			tempPath,FALSE);
			STATSTG StatInfo;

			CComPtr<IStream> outStream;
			DWORD grfMode = STGM_WRITE | STGM_SHARE_EXCLUSIVE | STGM_CREATE; 
			HRESULT hr = ::SHCreateStreamOnFile(tempPath/*_T("C:\\Temp\\avpp_temppp.ass")*/, grfMode, &outStream);
		    m_pStream->Stat(&StatInfo, STATFLAG_NONAME );
			
			ULONG xbyts;
			unsigned long sizexx = StatInfo.cbSize.QuadPart;
			
			char *buf = new char [sizexx];
			m_pStream->Read (buf,sizexx,&xbyts);
			outStream->Write (buf,sizexx,&xbyts);
			delete [] buf;
			// why like this mr. softy ?? they have a bug in this function. or in documentation.
			//m_pStream->CopyTo (outStream,StatInfo.cbSize,&rrb,&wwb);
						
			outStream->Commit(STGC_DEFAULT);
			m_pStream->Release ();
			m_pStream = NULL;
			outStream.Release ();
			outStream = NULL;

		}
		// Implement OnDrawThumbnail in IDocument derived class

		static int const nDocDimensions = 256;
		if (!m_pDocument->GetThumbnail(nDocDimensions, phbmp, pdwAlpha))
		{
			return E_FAIL;
		}

		return S_OK;
	}

	DECLARE_DOCUMENT(CAvocadoDoc)
};

OBJECT_ENTRY_AUTO(__uuidof(Thumbnail), CThumbnailHandler)

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

// AvocadoDoc.cpp : implementation of the CAvocadoDoc class
//
#ifndef _AVOCADO_ACTIVEX
#include "stdafx.h"
#endif
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#ifndef _AVOCADO_ACTIVEX
#include "Avocado.h"
#else
#include <stdafx.h>
#include "MFCSafeActiveX.h"
#endif
#endif
//#ifdef SHARED_HANDLERS
#include "AvocadoPathUtils.h"
#include "../AvocadoHandlers/AvocadoThumbnailExtractor.h"
//#include "../../3rdParty/jpeg-8d/Jpegfile.h"
//#endif
#include "AvocadoDoc.h"
#include "SrvrItem.h"

#include <propkey.h>
#include "AvocadoView.h"
#include "../AvocadoEngine/AvocadoAppInterface.h"
#include <sstream>
#include <afxdatarecovery.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


CLIPFORMAT NEAR CAvocadoDoc::m_cfPrivate = NULL;
// CAvocadoDoc

IMPLEMENT_DYNCREATE(CAvocadoDoc, COleServerDocEx)

	BEGIN_MESSAGE_MAP(CAvocadoDoc, COleServerDocEx)

		ON_UPDATE_COMMAND_UI(ID_FILE_UPDATE, &CAvocadoDoc::OnUpdateFileUpdate)
	END_MESSAGE_MAP()
	/* begin com old stuff*/


	BEGIN_DISPATCH_MAP(CAvocadoDoc, COleServerDocEx)
		DISP_PROPERTY_EX(CAvocadoDoc, "DocumentID", GetAvoID, SetAvoID, VT_I4)
	//	DISP_FUNCTION(CAvocadoDoc, "SaveModified", SaveModified, VT_BOOL, VTS_NONE)
		DISP_FUNCTION(CAvocadoDoc, "AvocadoInvokeDoc", AvocadoInvokeDocW, VT_BOOL, VTS_BSTR VTS_I4 VTS_BSTR VTS_BSTR)
		DISP_FUNCTION(CAvocadoDoc, "AvocadoInvokeView", AvocadoInvokeViewW, VT_BOOL, VTS_BSTR VTS_I4 VTS_BSTR VTS_BOOL)
		DISP_FUNCTION(CAvocadoDoc, "AvocadoInvokeUI", AvocadoInvokeUI, VT_BOOL, VTS_PI1 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
		DISP_FUNCTION(CAvocadoDoc, "AvocadoResizeView", AvocadoResizeView, VT_BOOL, VTS_I4 VTS_I4 VTS_I4)
		DISP_FUNCTION(CAvocadoDoc, "AvocadoCreateView", AvocadoCreateView, VT_I4,  VTS_I4 VTS_I4)
	END_DISPATCH_MAP()

	// Note: we add support for IID_ISDISample to support typesafe binding
	//  from VBA.  This IID must match the GUID that is attached to the 
	//  dispinterface in the .IDL file.

	// {F1BE1303-1859-46CA-A6C9-8A1E6E813223}
	static const IID IID_IAvocado =
	{ 0xF1BE1303, 0x1859, 0x46CA, { 0xA6, 0xC9, 0x8A, 0x1E, 0x6E, 0x81, 0x32, 0x23 } };

	BEGIN_INTERFACE_MAP(CAvocadoDoc, COleServerDocEx)
		INTERFACE_PART(CAvocadoDoc, IID_IAvocado, Dispatch)
	END_INTERFACE_MAP()

	// CSDISampleDoc server implementation
	
	// Not sure about the code below. taken from the calculator sample.
#if 0
#ifndef IMPLEMENT_OLECREATE_SINGLE
// MFC will provide this macro in the future.  For now, we define it.
#define IMPLEMENT_OLECREATE_SINGLE(class_name, external_name, \
    l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    AFX_DATADEF COleObjectFactory class_name::factory(class_name::guid, \
        RUNTIME_CLASS(class_name), TRUE, _T(external_name)); \
    const AFX_DATADEF GUID class_name::guid = \
        { l, w1, w2, { b1, b2, b3, b4, b5, b6, b7, b8 } };
#endif

// {62C4DD10-F45E-11cd-8C3D-00AA004BB3B7}
IMPLEMENT_OLECREATE_SINGLE(CAvocadoDoc, "avocado.document", 
   	0xDA96493C, 0x33A3, 0x4D50,  0x91, 0xA5, 0x41, 0xDF, 0xF6, 0x28, 0x7F, 0xBF );
#endif

	COleServerItem* CAvocadoDoc::OnGetEmbeddedItem()
	{
#ifndef SHARED_HANDLERS
#ifndef _AVOCADO_ACTIVEX
		// OnGetEmbeddedItem is called by the framework to get the COleServerItem
		//  that is associated with the document.  It is only called when necessary.

		//	static CAvocadoSrvrItem* m_pItem = NULL;
		if (m_pItem == NULL)
		{
			m_pItem = new CAvocadoSrvrItem(this);
		}
		ASSERT_VALID(m_pItem);
		return m_pItem;
#else
		return NULL;
#endif
#else
		return NULL;
#endif

	}


	// CSDISampleDoc Active Document server implementation

	CDocObjectServer *CAvocadoDoc::GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite)
	{
		return NULL;// new CDocObjectServer(this, pDocSite);
	}
	/* end com ole stuff*/
	using namespace avocado;
	// CAvocadoDoc construction/destruction
	void CAvocadoDocInterface::ErrorCallback (const char *errDesc) 
	{
		((CAvocadoDoc*)m_doc)->RaiseError (string(errDesc));
	}
	void CAvocadoDocInterface::DocuemntStatusCallback (DocumentStatus stat, std::string name, int progress ) 
	{
		string statusString ="";
		switch (stat)
		{
		case LOAD_DOCUMENT_STARTED :
			statusString = "Started loading avocado document";
			break;
		case LOAD_ELEMENT_STARTED :
			statusString = "Started reading avocado element";
			break;
		case LOAD_ELEMENT_COMPLETE :
			statusString = "Element " + std::string (name) + " loaded successfully";
			break;
		case LOAD_DOCUMENT_COMPLETE :
			statusString = "Document loaded successfully";
			break;
		case SAVE_DOCUMENT_STARTED :
			statusString = "Started loading avocado document";
			break;
		case SAVE_ELEMENT_STARTED :
			statusString = "Started saving avocado element - " + std::string (name) ;
			break;
		case SAVE_ELEMENT_COMPLETE :
			statusString = "Element "+ std::string (name) + " saved successfully";
			break;
		case SAVE_DOCUMENT_COMPLETE :
			statusString = "Finished saving avocado document";
			break;
		case DOWNLOAD_STARTED :
			statusString = "Started downloading " + std::string (name);
			break;

		case DOWNLOAD_PROGRESS :
			statusString = "Downloading " + std::string (name) ;
			break;

		case DOWNLOAD_COMPLETE :
			statusString = "Finished downloading "+ std::string (name);
			break;

		default:
			break;
		};

		((CAvocadoDoc*)m_doc)->SetDocumentStatus (statusString,progress);
	}
	void CAvocadoDocInterface::SuccessCallback () 
	{
		
	}
	void CAvocadoDocInterface::ElementsChanged (AvocadoElementsList elementsList,AvocadoFilesList files)
	{
		((CAvocadoDoc*)m_doc)->SetFiles (files);
		((CAvocadoDoc*)m_doc)->SetElements (elementsList);

	}
	void CAvocadoDocInterface::ViewStateChanged (AvocadoViewStateList viewStateList, int current )
	{
		((CAvocadoDoc*)m_doc)->SetViewStates (viewStateList,current);
	}
	void CAvocadoDocInterface::MaterialStateChanged (AvocadoMaterialStateList materialStateList )
	{
		std::vector<std::string> msv;
		for (size_t i=0;i<materialStateList.size();i++)
			msv.push_back (materialStateList[i].name);
		((CAvocadoDoc*)m_doc)->SetMaterialStates (msv);
	}
	void CAvocadoDoc::RaiseError (std::string err)
	{
		CString errS ((string ("Avocado Engine Error:\n")+err).c_str());
		::AfxMessageBox (errS);
	}
	void CAvocadoDoc::SetMaterialStates (std::vector<std::string> msList)
	{
		m_materialStates = msList;
#ifndef SHARED_HANDLERS

		//if (currentVSChanged && m_viewList.GetCount () > 0)
		if (m_viewList.GetCount () > 0)
		{
			NMHDR pNMHDR; 
			pNMHDR.code = AVC_REFRESH_MATERIAL_STATES;
			pNMHDR.hwndFrom = ((CView*)m_viewList.GetHead())->GetSafeHwnd();
			//pNMHDR.hwndFrom = (theApp.GetMainWnd ())->GetSafeHwnd();
			theApp.GetMainWnd ()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
		}
#endif

	}
	std::vector<std::string> CAvocadoDoc::GetMaterialStates ()
	{
		return m_materialStates;
	}
	void CAvocadoDoc::SetViewStates (AvocadoViewStateList viewStateList, int current)
	{
		
		m_viewStatesHtml.clear();;
		m_viewStatesImages.clear();
		for (size_t i=0;i<viewStateList.size();i++)
		{
			m_viewStatesHtml.push_back(viewStateList[i].htmlString);
			m_viewStatesImages.push_back(viewStateList[i].image);
		}
		bool currentVSChanged= false;
		if (m_currentViewState != current || current == 0)
		{
			currentVSChanged= true;
			m_currentViewState = current;
		}
#ifndef SHARED_HANDLERS

		if (currentVSChanged && m_viewList.GetCount () > 0)
		{
			NMHDR pNMHDR; 
			pNMHDR.code = AVC_REFRESH_VIEW_STATES;
			pNMHDR.hwndFrom = ((CView*)m_viewList.GetHead())->GetSafeHwnd();
			//pNMHDR.hwndFrom = (theApp.GetMainWnd ())->GetSafeHwnd();
			theApp.GetMainWnd ()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
		}
#endif
	}

	std::string CAvocadoDoc::GetViewStateImage (int vsi)
	{
		if (vsi >= 0 && vsi < int(m_viewStatesImages.size()))
		{
			return m_viewStatesImages[vsi];
		}
		return std::string ("");
	}
	int CAvocadoDoc::GetCurrentViewState ()
	{
		return m_currentViewState;
	}

	bool CAvocadoDoc::AvocadoInvokeDocW (const TCHAR* msg, int docId, const TCHAR* paramStr,const TCHAR* targetModule)
	{
#ifndef SHARED_HANDLERS
#ifdef UNICODE
		static char dest1[MAX_PATH];
		static char dest2[MAX_PATH];
		static char dest3[MAX_PATH];
		dest1[0] = '\0';
		dest2[0] = '\0';
		dest3[0] = '\0';
			
		::wcstombs (dest1,msg,MAX_PATH);
		::wcstombs (dest2,paramStr,MAX_PATH);
		::wcstombs (dest3,targetModule,MAX_PATH);

		return avocado::OnSendAvocadoDocGeneralStringMessage (dest1,docId,dest2,dest3);
#else
	   return AvocadoInvokeDoc (msg,docId,paramStr,targetModule);
#endif
#endif
		return true;
	}

	UINT ThreadProc( LPVOID lpParameter )
	{
		CAvocadoDoc* doc = (CAvocadoDoc*)lpParameter;
		doc->OnSerializeDocResume ();
		doc->OnSerializeDocComplete();
		return 0L;
	}
    void CAvocadoDoc::OnSerializeDocComplete()
 	{
		// UpdateAllViews(0);
#ifndef SHARED_HANDLERS
			if (m_viewList.GetCount () > 0)
			{
				NMHDR pNMHDR; 
				pNMHDR.code = AVC_REFRESH_VIEW_STATES;
				pNMHDR.hwndFrom = ((CView*)m_viewList.GetHead())->GetSafeHwnd();
				//pNMHDR.hwndFrom = (theApp.GetMainWnd ())->GetSafeHwnd();
				theApp.GetMainWnd ()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
			}
#endif
		TRACE0("OnSerializeDocComplete multi thread loading complete\n");
	}
	void CAvocadoDoc::OnSerializeDocResume()
	{
#ifndef SHARED_HANDLERS
		TRACE0("OnSerializeDocResume started loading in seperate thread\n");
		 avocado::OnSerializeDoc(ser_thread_i1,ser_thread_p1,ser_thread_b1,ser_thread_b2);
#endif
		OnSerializeDocComplete();
	}
	bool CAvocadoDoc::AvocadoSerializeDoc(int viewId, std::string & paramStr ,bool isImport,bool isStoring)
	{
#ifndef SHARED_HANDLERS
			ser_thread_b1 = isImport;
			ser_thread_b2 = isStoring;
			ser_thread_i1 = viewId;
			ser_thread_p1 = paramStr;
			if (0)//!isImport && !isStoring)
			{
				TRACE0("AvocaoSerializeDoc started in multi thread mode\n");
				CWinThread* pThread = ::AfxBeginThread( ThreadProc, this, 0, 0, 
										CREATE_SUSPENDED, NULL);
				pThread->m_bAutoDelete = TRUE; 
				pThread->ResumeThread();
			} 
			else
			{
				TRACE0("AvocaoSerializeDoc started in single thread mode\n");
				 avocado::OnSerializeDoc(viewId,paramStr,isImport,isStoring);
			}
		 return TRUE;
#endif
		return TRUE;
	}
	bool CAvocadoDoc::AvocadoSerializeDocW(int viewId, const TCHAR* paramStr ,bool isImport,bool isStoring)
	{
#ifndef SHARED_HANDLERS
#ifdef UNICODE
		static char dest1[MAX_PATH];
		dest1[0] = '\0';
		::wcstombs (dest1,paramStr,MAX_PATH);
		
		avocado::OnSerializeDoc(viewId,std::string (dest1),isImport,isStoring);
		return TRUE;
#else
		return AvocadoSerializeDoc(viewId,paramStr,isImport,isStoring);
#endif
#endif
		return true;
	}
	bool CAvocadoDoc::AvocadoInvokeViewW (const TCHAR* msg, int viewId, const TCHAR* paramStr,bool toAllModules)
	{
#ifndef SHARED_HANDLERS
#ifdef UNICODE
		static char dest1[MAX_PATH];
		static char dest2[MAX_PATH];
		dest1[0] = '\0';
			dest2[0] = '\0';
		::wcstombs (dest1,msg,MAX_PATH);
		::wcstombs (dest2,paramStr,MAX_PATH);
		
		return avocado::OnSendAvocadoGeneralStringMessage (dest1,viewId,dest2,toAllModules);
#else
		return AvocadoInvokeView(msg,viewId,paramStr,toAllModules);
#endif
#endif
		return true;
	}
	bool CAvocadoDoc::AvocadoInvokeDoc (const char* msg, int docId, const char* paramStr,const char* targetModule)
	{
#ifndef SHARED_HANDLERS
#ifdef _DEBUG
		wchar_t wmsg[MAX_PATH];
		wchar_t wparams[MAX_PATH];
		::mbstowcs (wmsg,msg,MAX_PATH);
		::mbstowcs (wparams,paramStr,MAX_PATH);

		TRACE3("AvocadoInvokeDoc msg=%s docid=%d paramStr=%s \n",wmsg,docId,wparams);
#endif
		return avocado::OnSendAvocadoDocGeneralStringMessage (msg,docId,paramStr,targetModule);
#endif
		return true;
	}
	bool CAvocadoDoc::AvocadoInvokeView (const char* msg, int viewId, const char* paramStr,bool toAllModules)
	{
#ifndef SHARED_HANDLERS
#ifdef _DEBUG
		wchar_t wmsg[MAX_PATH];
		wchar_t wparams[MAX_PATH];
		::mbstowcs (wmsg,msg,MAX_PATH);
		::mbstowcs (wparams,paramStr,MAX_PATH);
		TRACE3("AvocadoInvokeView msg=%s vid=%d paramStr=%s \n",wmsg,viewId,wparams);
#endif
		return avocado::OnSendAvocadoGeneralStringMessage (msg,viewId,paramStr,toAllModules);
#endif
		return true;
	}
	bool CAvocadoDoc::AvocadoInvokeUI (UINT msg, int viewId, int x, int y, int zDelta)
	{
#ifndef SHARED_HANDLERS
		if (viewId > -1)
		return avocado::OnSendAvocadoMouseStringMessage ((AvcMouseActType)msg,viewId,x,y,zDelta);
#endif
		return true;
	}
		bool CAvocadoDoc:: AvocadoResizeView(int viewId, int px, int py)
	{
#ifndef SHARED_HANDLERS
		TRACE3("AvocadoResizeView vid=%d sizeX=%d sizeY=%d\n",viewId,px,py);
		 avocado::OnSizeView (viewId,px,py);
		 return true;
#endif
		return true;
	}
		int CAvocadoDoc:: AvocadoCreateView(void *phWnd, void *viewInterface)
	{
#ifndef SHARED_HANDLERS
		TRACE0("AvocadoCreateView\n");
		return avocado::OnCreateView(phWnd,(AvocadoViewInterface *)viewInterface);
#endif
		return true;
	}
	bool CAvocadoDoc::SetViewStateImage (int vsi,string & imageString )
	{
		string params = string("int vid=0,") + string("string htmlText=") + imageString + string (";");

		AvocadoInvokeDoc("SaveViewStateImage",m_id,params.c_str());
		return true;
	}
	bool CAvocadoDoc::SetViewStateHtml (int vsi,string & htmlString )
	{
		if (htmlString == string (""))
			return true;
		string params = string("int vid=0,") + string("string htmlText=") + htmlString + string (";");

		AvocadoInvokeDoc("SaveViewStateHtml",m_id,params.c_str());
		return true;
	}
	std::string CAvocadoDoc::GetViewStateHtml (int vsi )
	{
		if (vsi >= 0 && vsi < int(m_viewStatesHtml.size()))
		{
			return m_viewStatesHtml[vsi];
		}
		return std::string ("");
	}
	void CAvocadoDoc::GetElementName (int id , std::string &tname )
	{
		size_t elemIdx;
		bool found = false;
		if (m_cachedElemID < m_elementsList.size() &&
				id == m_elementsList[m_cachedElemID].id)
		{
				elemIdx = m_cachedElemID;
				found = true;
		} else
		for (size_t i=0;i<m_elementsList.size ();i++)
			if (m_elementsList[i].id == id)
			{
				elemIdx = i;
				m_cachedElemID = elemIdx;
				found = true;
				break;
			}
			if (!found)
				return;

			tname = m_elementsList[elemIdx].name;
	};
	void CAvocadoDoc::GetElementVisibility (int id , bool &visible )
	{
		size_t elemIdx;
		bool found = false;
			if (m_cachedElemID < m_elementsList.size() &&
				id == m_elementsList[m_cachedElemID].id)
			{
				elemIdx = m_cachedElemID;
				found = true;
			} else

		for (size_t i=0;i<m_elementsList.size ();i++)
			if (m_elementsList[i].id == id)
			{
				elemIdx = i;
				m_cachedElemID = elemIdx;
				found = true;
				break;
			}
			if (!found)
				return;

			visible = m_elementsList[elemIdx].isVisible;
	}
	AvocadoMaterialInterface * CAvocadoDoc::GetElementMaterialData (int id)
	{
		{
			size_t elemIdx;
			bool found = false;
			
			if (m_cachedElemID < m_elementsList.size() &&
				id == m_elementsList[m_cachedElemID].id)
			{
				elemIdx = m_cachedElemID;
				found = true;
			} else
			for (size_t i=0;i<m_elementsList.size ();i++)
				if (m_elementsList[i].id == id)
				{
					elemIdx = i;
					m_cachedElemID = elemIdx;
					found = true;
					break;
				}
				if (!found)
					return 0;
				if (m_elementsList[elemIdx].materialID > -1)
				 return &(m_elementsList[elemIdx].materialData);

		}
		return 0 ;
	}
	unsigned int  CAvocadoDoc::GetElementContext (int id )
	{
				//for (size_t i=0; i<m_elementsList.size ();i++)
		{
			size_t elemIdx;
			bool found = false;
			if (m_cachedElemID < m_elementsList.size() &&
				id == m_elementsList[m_cachedElemID].id)
			{
				elemIdx = m_cachedElemID;
				found = true;
			} else

			for (size_t i=0;i<m_elementsList.size ();i++)
				if (m_elementsList[i].id == id)
				{
					elemIdx = i;
					m_cachedElemID = elemIdx;
					found = true;
					break;
				}
				if (!found)
					return 0;
#ifndef SHARED_HANDLERS
				if (m_elementsList[elemIdx].elementType == string ("AnnotationsModule")) 
				{
					if (m_elementsList[elemIdx].elementSubType == string ("SpotLight")) 
						return ID_CONTEXT_ANNOTATION_SPOT;
					else
					if (m_elementsList[elemIdx].elementSubType == string ("Callout")) 
						return ID_CONTEXT_LINE_STYLE;
					else
					if (m_elementsList[elemIdx].elementSubType == string ("PathLine")) 
						return ID_CONTEXT_LINE_STYLE;
					else					
					return ID_CONTEXT_ANNOTATIONS;
				}
				else
				{
					if (m_elementsList[elemIdx].materialID > -1)
					 return ID_CONTEXT2;
				}
#endif

		}
		return 0 ;
	}
	void CAvocadoDoc::GetElementColor (int id , std::vector<int> &col )
	{
		//for (size_t i=0; i<m_elementsList.size ();i++)
		{
			size_t elemIdx;
			bool found = false;
			if (m_cachedElemID < m_elementsList.size() &&
				id == m_elementsList[m_cachedElemID].id)
			{
				elemIdx = m_cachedElemID;
				found = true;
			} else

			for (size_t i=0;i<m_elementsList.size ();i++)
				if (m_elementsList[i].id == id)
				{
					elemIdx = i;
					m_cachedElemID = elemIdx;
					found = true;
					break;
				}
				if (!found)
					return;

				col.push_back (m_elementsList[elemIdx].color[0]);
				col.push_back (m_elementsList[elemIdx].color[1]);
				col.push_back (m_elementsList[elemIdx].color[2]);


		}
		return ;
	}

	std::string CAvocadoDoc::GetElementMetaData (int id , std::string metaVarName )
	{
		//for (size_t i=0; i<m_elementsList.size ();i++)
		{
			size_t elemIdx;
			bool found = false;
			if (m_cachedElemID < m_elementsList.size() &&
				id == m_elementsList[m_cachedElemID].id)
			{
				elemIdx = m_cachedElemID;
				found = true;
			} else

			for (size_t i=0;i<m_elementsList.size ();i++)
				if (m_elementsList[i].id == id)
				{
					elemIdx = i;
					m_cachedElemID = elemIdx;
					found = true;
					break;
				}
				if (!found)
					return "";

				AvocadoElementInterface::metaDataIterator it = m_elementsList[elemIdx].metaData.begin ();
				while (it != m_elementsList[elemIdx].metaData.end())
				{
					if (it->first == metaVarName )
						return it->second;
					it++;
				}
		}
		return std::string ("");
	}
	
	std::string CAvocadoDoc::GetElementAnnotationData (int id , std::string metaVarName )
	{
		//for (size_t i=0; i<m_elementsList.size ();i++)
		{
			size_t elemIdx;
			bool found = false;
			if (m_cachedElemID < m_elementsList.size() &&
				id == m_elementsList[m_cachedElemID].id)
			{
				elemIdx = m_cachedElemID;
				found = true;
			} else

			for (size_t i=0;i<m_elementsList.size ();i++)
				if (m_elementsList[i].id == id)
				{
					elemIdx = i;
					m_cachedElemID = elemIdx;
					found = true;
					break;
				}
				if (!found)
					return "";

				AvocadoElementInterface::metaDataIterator it = m_elementsList[elemIdx].annotationData.begin ();
				while (it != m_elementsList[elemIdx].annotationData.end())
				{
					if (it->first == metaVarName )
						return it->second;
					it++;
				}
		}
		return std::string ("");
	}
#ifndef SHARED_HANDLERS
	void CAvocadoDoc::SetElementMetaData (int id , std::string metaVarName ,std::string data)
	{
		//for (size_t i=0; i<m_elementsList.size ();i++)
		{

			size_t elemIdx;
			bool foundelid = false;
			for (size_t i=0;i<m_elementsList.size ();i++)
				if (m_elementsList[i].id == id)
				{
					elemIdx = i;
					foundelid = true;
					break;
				}
				if (!foundelid)
					return;
				bool found = false;
				AvocadoElementInterface::metaDataIterator it = m_elementsList[elemIdx].metaData.begin ();
				while (it != m_elementsList[elemIdx].metaData.end())
				{
					if (it->first == metaVarName )
					{
						it->second = data;
						found = true;
						break;
					}
					it++;
				}
				if (!found)
				{
					pair <string , string> newmeta (metaVarName,data);
					m_elementsList[elemIdx].metaData.push_back (newmeta);
				}
				std::stringstream pStr;
				pStr <<   "string metavar=" << metaVarName << ",string metadata=" << data 
					<<  ",int eid=" << id
					<< ";";
				AvocadoInvokeDoc("SetElementMetaData",m_id,pStr.str().c_str());
		}


	}
#endif
	int CAvocadoDoc::GetElementsCount ()
	{

		return (int) m_elementsList.size();
	}
	void CAvocadoDoc::GetElements (AvocadoElementsList &list)
	{
		list =  m_elementsList;
	}
	void CAvocadoDoc::GetFiles (AvocadoFilesList &list)
	{
		list =  m_filesList;
	}
	void CAvocadoDoc::SetFiles (AvocadoFilesList list)
	{
		m_filesList  = list;
	}
	void CAvocadoDoc::SetElements (AvocadoElementsList list)
	{
	
		bool sendUpdateMessage = true;
			m_hasHiddenElements = false;
		if (list.size () == m_elementsList.size())
		{
			// element count hasnt changed..
			int eq = 0;
			for (size_t kk=0;kk<list.size();kk++)
				if (list[kk].name == m_elementsList[kk].name)
				{
					eq ++;
					m_elementsList[kk] = list[kk];
					if (m_elementsList[kk].isVisible == false)
					{
						m_hasHiddenElements = true;
					}
				}
			if (eq == list.size())
				// elements have the same name..
				sendUpdateMessage = false;
			else
				m_elementsList  = list;	
			// why should we update the UI ? .. no good reason so far.
		}
		
		else
		{
			m_elementsList  = list;	
			for (size_t i=0;i < list.size();i++)
			{
				if (m_elementsList[i].isVisible == false)
				{
					m_hasHiddenElements = true;
					break;
				}
			}
		}

		this->SetModifiedFlag (true);
#ifndef SHARED_HANDLERS
		if (sendUpdateMessage && m_viewList.GetCount () > 0)
		{
			NMHDR pNMHDR; 
			pNMHDR.code = AVC_REFRESH_ACTIVE_DOCUMENT;
			pNMHDR.hwndFrom = ((CView*)m_viewList.GetHead())->GetSafeHwnd();
			//pNMHDR.hwndFrom = (theApp.GetMainWnd ())->GetSafeHwnd();
			theApp.GetMainWnd ()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
		}
#endif
	}
	void CAvocadoDoc::OnDocumentEvent(DocumentEvent deEvent)
	{
		COleServerDocEx::OnDocumentEvent( deEvent);

		//	if ((deEvent == onAfterOpenDocument))
		//	{
		//		this->SetModifiedFlag (false);
		//		#ifndef SHARED_HANDLERS
		//		theApp.RefreshActiveDocument ();
		//		#endif
		//	}
	}


	void CAvocadoDocInterface::DocParamChanged (const char *paramName, const char *value)
	{
		string stpn (paramName);
		string val  (value);
		((CAvocadoDoc*)m_doc)->SetDocParameter (string (paramName),string (value));
	}

	float
		CAvocadoDoc::GetDocParameterAsFloat (string paramName)
	{
		float ivalue = 0.0f;
		for (size_t i=0;i<m_docParameters.size();i++)
		{
			if (m_docParameters[i].first == paramName)
			{
				string val = m_docParameters[i].second ;
				std::istringstream xs(val);
				xs >> ivalue;
				return ivalue;
			}
		} 
		return -1.0f;
	}
	int
		CAvocadoDoc::GetDocParameterAsInt (string paramName)
	{
		int ivalue = 0;
		for (size_t i=0;i<m_docParameters.size();i++)
		{
			if (m_docParameters[i].first == paramName)
			{
				string val = m_docParameters[i].second ;
				std::istringstream xs(val);
				xs >> ivalue;
				return ivalue;
			}
		} 
		return -1;
	}
	bool
		CAvocadoDoc::SetDocParameter (string paramName, string value)
	{
		for (size_t i=0;i<m_docParameters.size();i++)
		{
			if (m_docParameters[i].first == paramName)
			{
				m_docParameters[i].second = value;
				return true;
			}
		} 
		{
			m_docParameters.push_back (pair<string,string> (paramName,value));	
			return true;
		}
		return false;
	}
		string
		CAvocadoDoc::GetDocParameterAsString (string paramName)
	{
		for (size_t i=0;i<m_docParameters.size();i++)
		{
			if (m_docParameters[i].first == paramName)
			{
				string val = m_docParameters[i].second ;
				
				return val;
			}
		} 
		return "";
	}

	void CAvocadoDoc::GetDocumentStatus (std::string &stat,int &progress)
	{
		stat = m_lastStatus;
		progress = 	m_lastProgress;
	}
	void CAvocadoDoc::SetDocumentStatus (std::string stat,int progress)
	{
		m_lastStatus = stat;
		m_lastProgress = progress;
		if (m_viewList.GetCount() == 0)
			return;
	
		// Notify the application/ mainframe about load status change.
#ifndef SHARED_HANDLERS
       if (theApp.GetMainWnd () == NULL)
			return;
		NMHDR pNMHDR; 
		pNMHDR.code = AVC_REFRESH_DOC_LOAD_STATUS;
		pNMHDR.hwndFrom = ((CView*)m_viewList.GetHead())->GetSafeHwnd();
		//pNMHDR.hwndFrom = (theApp.GetMainWnd ())->GetSafeHwnd();
		theApp.GetMainWnd ()->SendMessage (WM_NOTIFY,0,(LPARAM)&pNMHDR);
#endif
	}
	CAvocadoDoc::CAvocadoDoc()
	{
		// TODO: manage multi-document lists... Currently single document support, 
		// so we set it to be the current
#ifndef SHARED_HANDLERS
		m_docInterface = new CAvocadoDocInterface(this);
		m_id = avocado::OnCreateDoc(m_docInterface);
		m_cachedElemID = 0;
		avocado::SetActiveDoc(m_id);
#endif
		m_lastProgress = -1;
		m_lastStatus = "";
		m_hasHiddenElements = false;
		m_thumbX = 0;
		m_isAutoSaving = false;
		m_isSaving = false;
		m_isPublishing = false;
		m_currentViewState = 0;
		// com ole stuff
		//	EnableCompoundFile();

		// TODO: add one-time construction code here
		m_pItem = NULL;
		EnableAutomation();

		AfxOleLockApp();

		if (m_cfPrivate == NULL)
		{
			m_cfPrivate = (CLIPFORMAT)
				::RegisterClipboardFormat(_T("Avocado"));
		}
		//	m_elementsCount = 0;
		// TODO: add one-time construction code here
	}
	long CAvocadoDoc::GetAvoID()

	{ 
		return m_id ;
	}

	CAvocadoDoc::~CAvocadoDoc()
	{
#ifndef SHARED_HANDLERS
		if (m_docInterface)
			delete m_docInterface;
#endif
		AfxOleUnlockApp();
	}

	BOOL CAvocadoDoc::OnNewDocument()
	{

		if (!COleServerDocEx::OnNewDocument())
			return FALSE;
		

#ifndef SHARED_HANDLERS
		theApp.ClearAppSessionTempFolder();
		//avocado::OnSendAvocadoDocGeneralStringMessage("NewDocument",m_id,std::string (""));

		// Document reinitialization code here
		// (SDI documents will reuse this document)


		// Needed for OLE Active document server
		if (theApp.GetMainWnd ())
		{
			CView* activeView = ((CFrameWnd*)theApp.GetMainWnd ())->GetActiveView ();
			if (activeView && m_viewList.GetCount() == 0 )
			{
				AddView(activeView);
			}
		}
#endif
		UpdateAllViews(NULL);
		SetModifiedFlag (false);
		return TRUE;
	}

	bool CAvocadoDoc::ImportFile (LPCTSTR lpszFileName)
	{
#ifndef SHARED_HANDLERS
		CT2CA pszConvertedAnsiString (lpszFileName);

		// construct a std::string using the LPCSTR input
		std::string strStd (pszConvertedAnsiString);

		// Insert to file list..

		//  add the element..
		// TODO: keep track of doc id
	
		AvocadoSerializeDoc(m_id,strStd,true);
#endif
		return true;
	}
	// CAvocadoDoc serialization

	void CAvocadoDoc::Serialize(CArchive& ar)
	{
		// construct a std::string using the LPCSTR input
		//CT2CA pszConvertedAnsiString (ar.GetFile()->GetFilePath());
		//std::string strStd (pszConvertedAnsiString);

#ifndef SHARED_HANDLERS
#ifdef _DEBUG 
		CMemoryState oldMemState, newMemState, diffMemState; 
		oldMemState.Checkpoint(); 
#endif 

		if (ar.IsStoring())
		{
			string strBuf;
			AvocadoSerializeDoc(m_id,strBuf,false,true);
			CA2CT ctStrBuf (strBuf.c_str());
			ar.WriteString(ctStrBuf);

		}
		else
		{
			string docString;
			CString rString;
			//TRY
			{
				bool eof = false;
				while (!eof)
				{
					TRY
					{
						eof =  ar.ReadString (rString) ? false:true;
					}
					CATCH (CArchiveException, pArchiveException)
					{
						// not needed , archive expections are already handled
						if (pArchiveException->m_cause == 3)
						{
							eof =  true;
						}
						else
						{
							THROW(pArchiveException);
						}
						// catch archive exception
					}
					END_CATCH

						if (rString == "")
							break;
					CT2CA caString (rString);
					docString += string(caString) + string("\n");
				}
			}
			//CATCH (CFileException, pFileException)
			//{
			// catch file exception
			//}
			//AND_CATCH (CArchiveException, pArchiveException)

			AvocadoSerializeDoc(m_id,docString,false,false);
		

			//avocado::OnSerializeDoc(m_id,strStd);
		}

#ifdef _DEBUG 
		newMemState.Checkpoint(); 
		if( diffMemState.Difference( oldMemState, newMemState ) ) 
		{ 
			TRACE( "Memory leaked!\n" ); 
			diffMemState.DumpStatistics(); 
		} 
#endif

#endif
	}

	bool CAvocadoDoc::GetBitmapThumbnail (LPCTSTR lpszPathName)
	{
		//#ifdef SHARED_HANDLERS
		m_thumbX =  new CAvocadoThumbExtract(lpszPathName,
			/*CString (L"C:\\Temp")*/CAvocadoPathUtils::GetAppDataPath(CString("AvocadoApp")));

		m_thumbX->Extract ();
		//#endif
		return true;
	}

#ifdef SHARED_HANDLERS
	// Support for thumbnails
	void CAvocadoDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
	{
		CRect rect;
		rect = lprcBounds;

		GetBitmapThumbnail (this->GetPathName());
		if (m_thumbX && m_thumbX->m_valid)
		{
			CDC *mdc=new CDC;
			mdc->CreateCompatibleDC(&dc);

			//CBitmap is devived from CGdiObject
			//It has a handle:m_hObject, which can obtained with LoadImage
			CBitmap bitmap;
			bitmap.m_hObject=(HBITMAP)::LoadImage(NULL,m_thumbX->m_thumbnailBitmapFile,
				IMAGE_BITMAP,rect.right,rect.bottom,LR_LOADFROMFILE);
			mdc->SelectObject(bitmap);

			//BitBlt()
			dc.BitBlt(0,0,rect.right,rect.bottom,mdc,0,0,SRCCOPY);
		}
		else
		{
			// Modify this code to draw the document's data
			dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

			CString strText = this->GetPathName();//_T("TODO: implement thumbnail drawing here");
			LOGFONT lf;

			CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
			pDefaultGUIFont->GetLogFont(&lf);
			lf.lfHeight = 36;

			CFont fontDraw;
			fontDraw.CreateFontIndirect(&lf);

			CFont* pOldFont = dc.SelectObject(&fontDraw);
			dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
			dc.SelectObject(pOldFont);
		}
	}

	// Support for Search Handlers
	void CAvocadoDoc::InitializeSearchContent()
	{
		CString strSearchContent;
		// Set search contents from document's data. 
		// The content parts should be separated by ";"

		// For example:  strSearchContent = _T("point;rectangle;circle;ole object;");
		SetSearchContent(strSearchContent);
	}

	void CAvocadoDoc::SetSearchContent(const CString& value)
	{
		if (value.IsEmpty())
		{
			RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
		}
		else
		{
			CMFCFilterChunkValueImpl *pChunk = NULL;
			ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
			if (pChunk != NULL)
			{
				pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
				SetChunkValue(pChunk);
			}
		}
	}

#endif // SHARED_HANDLERS

	// CAvocadoDoc diagnostics

#ifdef _DEBUG
	void CAvocadoDoc::AssertValid() const
	{
		COleServerDocEx::AssertValid();
	}

	void CAvocadoDoc::Dump(CDumpContext& dc) const
	{
		COleServerDocEx::Dump(dc);
	}
#endif //_DEBUG


	// CAvocadoDoc commands

	void CAvocadoDoc::OnChangedViewList()
	{
		

		COleServerDocEx::OnChangedViewList();
	}

	void CAvocadoDoc::OnLoadDoc()
	{
		
	}


	


	void CAvocadoDoc::OnUpdateSaveDoc(CCmdUI *pCmdUI)
	{
		pCmdUI->Enable(true);
	}



#define _CREATE_THUMBNAIL
#define _ZIP_DOC
#define _EXPORT_VIEWSTATE_HTML
#define _EXPORT_TOP_OBJ
#define _EXPORT_VIEWSTATE_IMAGES
#define _EXPORT_DOC_PARAMS
#define _EXPORT_SAVE_EMBED_CONTENT
	BOOL CAvocadoDoc::OnOpenDocument(LPCTSTR lpszPathName)
	{
#ifndef SHARED_HANDLERS
		// Needed for OLE Active document server
		bool isstorage = false;
		if (theApp.GetMainWnd ())
		{
			CView* activeView = ((CFrameWnd*)theApp.GetMainWnd ())->GetActiveView ();
			if (activeView && m_viewList.GetCount() == 0 )
			{
				AddView(activeView);
			}
		}


		if (lpszPathName == NULL)
			//open from sub-storage;
		{
			return (COleServerDocEx::OnOpenDocument(lpszPathName));
			CString storageFolder = theApp.GetAppStorageFolder();
			storageFolder += CString (L"storage.avc");
			isstorage = true;

			// save to clip board ??
			lpszPathName = new wchar_t[MAX_PATH];
			wcscpy ((wchar_t*)lpszPathName,storageFolder.GetString());
		}
#endif
#ifndef SHARED_HANDLERS
		CString newLocation = CString (lpszPathName);
#ifdef _ZIP_DOC
		//LPCTSTR tempFileName ;
		LPWSTR tempFileName = newLocation.GetBuffer ();
		::PathStripPath (tempFileName);
		//CFile tempFile (lpszPathName,0);
#ifndef SHARED_HANDLERS
		newLocation = theApp.GetAppSessionTempFolder () + CString (L"\\") + tempFileName;
		
#else
		CString appdatapath = CAvocadoPathUtils::GetAppDataPath (CString("AvocadoThumb"));
		newLocation = appdatapath+CString("\\temp\\") + tempFileName;
#endif
		char inpath[200];
		char outpath[200];
		::wcstombs (inpath, lpszPathName,200);
		::wcstombs (outpath, newLocation ,200);
		vector<string> blankAttach;
		vector<string> blankTextures;
		vector<string> blankModels;
		avocado::DoCompressDoc (m_id,std::string (inpath),std::string (outpath),blankAttach,blankModels,blankTextures,true);
#endif
		if (!CDocument::OnOpenDocument(newLocation))
			return FALSE;
		if (isstorage)
		{
			delete [] lpszPathName;
			lpszPathName = NULL;
		}
		// TODO:  Add your specialized creation code here
#endif
#ifndef SHARED_HANDLERS
		//theApp.RefreshActiveDocument();
#endif
		return TRUE;
	}

	void CAvocadoDoc::OnIdle()
	{
		// check if recovery handling is enabled...
		CWinApp *pApp = AfxGetApp();
#ifndef SHARED_HANDLERS
#ifndef _AVOCADO_ACTIVEX
		if (pApp != NULL && theApp.GetAppMode () != CAvocadoApp::VIEWER_APP )
		{
			CDataRecoveryHandler *pHandler = pApp->GetDataRecoveryHandler();
			if ((pHandler != NULL) && (pHandler->GetSaveDocumentInfoOnIdle()))
			{
				// ...and if so, save the document info.  Note that the info is saved
				// even if the document is not modified (for reopen on restart), but
				// the content is auto-saved only if the document is modified.
				m_isAutoSaving = true;
				pHandler->AutosaveDocumentInfo(this);
			}
		}
#endif
#endif
	}
	static string ParseHtmlImages (string inHtml,CString localPath,vector<string> &htmlfiles)
	{
		string outHtml;
		outHtml = inHtml;

		string hs,hs2,ipathname,opathname,hs3,final;
		size_t fo = inHtml.find("<IMG");
		if (fo <= 0 || fo == string::npos)
			return "";
		//html until first img tag
		hs = inHtml.substr (0,fo);
		// html from image tag to end..
		hs2 = inHtml.substr (fo,inHtml.size()-fo);
		size_t fc = hs2.find_first_of (">");

		// pathname inside img tag
		ipathname = hs2.substr (0,fc+1);
		// here ipathname contains <IMG src=...>.
		// lets replace it..
		size_t lq = ipathname.find ("src=\"");
		size_t rq = ipathname.rfind ("\"");

		string filename = ipathname.substr (lq+4,rq-lq-1);
		bool setsizeim = true;
		if (filename.find (string ("file:///"))!=string::npos)
		{
			setsizeim = false;
				filename = filename.substr (9,filename.size()-8-3);
		}
		else
			filename = filename.substr (1,filename.size()-3);
		for (int spi = 0;spi<int(filename.size ());spi++)
		{
			if (filename[spi] == '/')
			{
				filename[spi] = '\\';
			}
		}
		if (filename[filename.size()-1] == '\"')
			filename = filename.substr (0,filename.size()-1);
		{
			// now we have a filename , let copy , zip it and strip path..
			string stripfile = filename.substr (filename.rfind  ("\\")+1,filename.size()-filename.rfind  ("\\")-1);
			

			wchar_t winfilename[MAX_PATH];
			::mbstowcs (winfilename,filename.c_str(),MAX_PATH);
			CFile infile (winfilename,CFile::modeRead);


			char localPathStr[MAX_PATH];
			::wcstombs (localPathStr,localPath.GetString(),MAX_PATH);

			string outfilename = string (localPathStr) + string ("\\") +stripfile;
			wchar_t woutfilename [MAX_PATH];
			::mbstowcs (woutfilename,outfilename.c_str(),MAX_PATH);
			CFile outfile (CString (woutfilename),CFile::modeWrite | CFile::modeCreate);

			bool eof= false;
			while (!eof)
			{
				char buf[4096];
				UINT rb = infile.Read (buf,4096);
				outfile.Write(buf,4096);
				if (rb < 4096)
					eof = true;
			}
			infile.Close ();
			outfile.Close ();
		
			opathname = "<img src="+stripfile+" height=\"96\" width=\"128\" >";
			bool foundFile = false;
			for (int fi = 0;fi < int(htmlfiles.size());fi++)
				if  (htmlfiles[fi] == outfilename)
				{
					foundFile = true;
					break;
				}
				if (!foundFile)
			htmlfiles.push_back (outfilename);
		}
		// opathname = ipathname;
		// rest of html with out image tag
		hs3 = hs2.substr (fc+1,hs2.size()-fc-1);

		final = hs + opathname + hs3;

		return final;
	}
	void CAvocadoDoc::SaveDocParamsToFile (CString localPath,vector<string> &htmlfiles)
	{
		CString fullpath = localPath + CString(L"\\docparams.xml");
		CFile outfile (fullpath,CFile::modeWrite | CFile::modeCreate);

		string outbuf;
		for (size_t k=0;k<this->m_docParameters.size ();k++)
		{
			pair<string,string> pa = m_docParameters[k];
			outbuf += pa.first + "=" + pa.second + "\n";
		}
		outfile.Write (outbuf.c_str(),UINT(outbuf.size()));
		outfile.Close();

		char fullpathStr [MAX_PATH];
		::wcstombs (fullpathStr,fullpath.GetString (),MAX_PATH);
		htmlfiles.push_back (string (fullpathStr));

	}
	BOOL CAvocadoDoc::OnSaveDocument(LPCTSTR lpszPathName)
	{

		bool isstorage = false;
		
#ifndef SHARED_HANDLERS
		if (lpszPathName== NULL)
		{
			isstorage = true;
			// save to clip board ??
			
			return COleServerDocEx::OnSaveDocument (lpszPathName);
			//return TRUE;
			CString storageFolder = theApp.GetAppStorageFolder();
			storageFolder += CString (L"storage.avc");
			isstorage = true;

			// save to clip board ??
			lpszPathName = new wchar_t[MAX_PATH];
			wcscpy ((wchar_t*)lpszPathName,storageFolder.GetString());
		}
#endif
#ifndef SHARED_HANDLERS
		m_isSaving = true;
		CString newLocation ;

		newLocation.SetString ( lpszPathName );

#ifdef _ZIP_DOC
		//LPCTSTR tempFileName ;
		wchar_t tempFileName [MAX_PATH];
		::wcscpy (tempFileName,lpszPathName);
		//= newLocation.GetBuffer ();
		::PathStripPath (tempFileName);
		newLocation = theApp.GetAppSessionTempFolder () + CString (L"\\") + CString (tempFileName);
		//CFile tempFile (lpszPathName,0);

#endif
		wchar_t newLocStr[MAX_PATH];
		::wcscpy (newLocStr,newLocation.GetString ());
		BOOL res = CDocument::OnSaveDocument(newLocStr);
#ifdef _CREATE_THUMBNAIL
		CString thumbLocation = theApp.GetAppSessionTempFolder () + CString (L"\\avothumb.jpg");
		char buf[2048];
		::wcstombs (buf,thumbLocation,2048);
		AvocadoInvokeDoc ("SaveDocToFile",m_id,std::string (buf).c_str());
		
#endif
		vector<string> htmlFiles;
		vector<string> textureFiles;
		vector<string> modelFiles;
#ifdef _EXPORT_SAVE_EMBED_CONTENT
		// We dont want to embed hundreds of MB for autosave.
		if (!m_isAutoSaving)
		{
			for (size_t ifile=0;ifile< m_filesList.size();ifile++)
			{
				if (m_filesList[ifile].m_type == AvocadoFileLinkInterface::IMAGE)
				{
					wchar_t wmodelfilename [MAX_PATH];
					::mbstowcs (wmodelfilename,m_filesList[ifile].m_fileName.c_str(),MAX_PATH);
					if(0xffffffff == GetFileAttributes(wmodelfilename))
					{
						//File not found in its original folder.
						// Lets search the session folder.
						::PathStripPath (wmodelfilename);
						CString wss = theApp.GetAppSessionTempFolder () + CString ("\\textures\\") + CString ( wmodelfilename );
						if(0xffffffff == GetFileAttributes(wss.GetString ()))
						{
							//File not found ib temp sessioon folder , lets check the application folder
							wchar_t szPath[MAX_PATH];
							if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
							{
								std::wstring mfp (szPath);
								mfp = mfp.substr (0,mfp.rfind (L"\\"));
								CString wappss = CString (mfp.c_str ()) + CString ("\\media\\textures\\") + CString ( wmodelfilename );
								if(0xffffffff == GetFileAttributes(wappss.GetString ()))
								{
									// Not found anywhere.. :(
								} else
								{
									char wsst [MAX_PATH];
									::wcstombs (wsst,wappss.GetString (),MAX_PATH);
									textureFiles.push_back (string (wsst));	
								}
							}
						}
						else
						{
							char wsst [MAX_PATH];
							::wcstombs (wsst,wss.GetString (),MAX_PATH);
							textureFiles.push_back (string (wsst));
						}
					} 
					else
					{
				
						textureFiles.push_back (m_filesList[ifile].m_fileName);
					}
				} 
				else if (m_filesList[ifile].m_type == AvocadoFileLinkInterface::MODEL)
				{
				
					wchar_t wmodelfilename [MAX_PATH];
					::mbstowcs (wmodelfilename,m_filesList[ifile].m_fileName.c_str(),MAX_PATH);
					if(0xffffffff == GetFileAttributes(wmodelfilename))
					{
						::PathStripPath (wmodelfilename);
						CString wss = theApp.GetAppSessionTempFolder () + CString ("\\models\\") + CString ( wmodelfilename );
						if(0xffffffff == GetFileAttributes(wss.GetString ()))
						{
							//File not found
						} else
						{
							char wsst [MAX_PATH];
							::wcstombs (wsst,wss.GetString (),MAX_PATH);
							
							modelFiles.push_back (string (wsst));
						}
					}
					else 
					{
						modelFiles.push_back (m_filesList[ifile].m_fileName);
					}
				}
			}
		}
#endif
#ifdef _EXPORT_VIEWSTATE_HTML
#ifndef _AVOCADO_ACTIVEX
		if ( m_isPublishing && theApp.GetAppMode () != CAvocadoApp::VIEWER_APP &&	m_isAutoSaving == false)
		{
			for (size_t vsi=0; vsi < m_viewStatesHtml.size ();vsi++)
			{
				CString htmlname;
				char cc [30];
				itoa (int(vsi),cc,10);
				htmlname = theApp.GetAppSessionTempFolder () + CString (L"\\view") + CString (cc) + CString (".htm");
				CFile file (htmlname,CFile::modeWrite | CFile::modeCreate);
				bool stopParse = false;
				string inhtml = m_viewStatesHtml[vsi];
				string outhtml = "";
				while (!stopParse)
				{
					string paresedHtml = ParseHtmlImages (inhtml,theApp.GetAppSessionTempFolder(),htmlFiles);
					if (paresedHtml == "")
					{
						stopParse = true;
						outhtml = inhtml;//paresedHtml;
						break;
					}
					inhtml = paresedHtml;
				}

				file.Write (outhtml.c_str(),int(outhtml.size ()));
				file.Close ();
				char cfilename [MAX_PATH];
				::wcstombs (cfilename,htmlname.GetString  (),MAX_PATH);

				htmlFiles.push_back (cfilename);
			}
		}
#endif
#endif
#ifdef _EXPORT_DOC_PARAMS
		SaveDocParamsToFile (theApp.GetAppSessionTempFolder (),htmlFiles);
#endif
#ifdef _EXPORT_TOP_OBJ
#ifndef _AVOCADO_ACTIVEX
		if ( m_isPublishing && theApp.GetAppMode () != CAvocadoApp::VIEWER_APP && m_isAutoSaving == false)
		{
			CString objname ;
			objname = theApp.GetAppSessionTempFolder () + CString (L"\\modeltop.obj");
			char cfilenameobj [MAX_PATH];
			::wcstombs (cfilenameobj,objname.GetString  (),MAX_PATH);

			CString obj_mtl_name ;
			obj_mtl_name = theApp.GetAppSessionTempFolder () + CString (L"\\modeltop.obj.mtl");
			char cfilenamemtl[MAX_PATH];
			::wcstombs (cfilenamemtl,obj_mtl_name.GetString  (),MAX_PATH);

			htmlFiles.push_back (cfilenameobj);
			htmlFiles.push_back (cfilenamemtl);
			AvocadoInvokeDoc ("ExportCurrentPage",m_id,cfilenameobj);
		}
#endif
#endif
#ifdef  _EXPORT_VIEWSTATE_IMAGES
		for (size_t vsImageIdx = 0; vsImageIdx< GetViewStateCount (); vsImageIdx++)
		{
			std::string vsImageFileName = GetViewStateImage (int(vsImageIdx));
			CString tempF =theApp.GetAppSessionTempFolder () + CString ("\\");
			char tempC [MAX_PATH];
			::wcstombs (tempC,tempF.GetString (),MAX_PATH);

			htmlFiles.push_back (string (tempC)+vsImageFileName);
		}
#endif
		m_isAutoSaving = false;
		m_isSaving = false;
#ifdef _ZIP_DOC
		char inpath[2048];
		char outpath[2048];
		::wcstombs (inpath, newLocation,2048);
		::wcstombs (outpath, lpszPathName ,2048);
		// clear html files duplicates images..


		avocado::DoCompressDoc (m_id,std::string (inpath),std::string (outpath),htmlFiles,modelFiles,textureFiles,false);
#endif

		if (isstorage)
		{
			delete [] lpszPathName;
			lpszPathName = NULL;
		}

		return res;
#else
		return true;
#endif

	}


	void CAvocadoDoc::DeleteContents()
	{
		COleServerDocEx::DeleteContents();
		// TODO:: MOVE THIS TO ENGINE..!!
		AvocadoInvokeDoc("NewDocument",m_id,"");
		AvocadoInvokeDoc("SetDocParam",m_id, "string globalExposure=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string backimage=crosscube.hdr;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string backColor=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string globalLightAmbient=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string globalLightHeadLight=0.4;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string GlobalAmbientColor=0.0;");

		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectEnabled=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectContrast=0.6;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectBrightness=0.2;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectHue=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectSaturation=0.6;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectNoise=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectSoften=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectToneMap=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectGamma=1.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectFXAA=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectAOIntensity=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectSoftenEnabled=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectAOEnabled=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectNoiseEnabled=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectBrightnessEnabled=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectSaturationEnabled=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectTonemapEnabled=0.0;");
		AvocadoInvokeDoc("SetDocParam",m_id, "string PostEffectGammaEnabled=0.0;");
		


		/*GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), "string globalLightHeadLight=0.5;");
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), "string globalLightAmbient=0.0");
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), "string backimage=crosscube.hdr");
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), "string roomType=0.0");
		GetActiveAvocadoDoc()->AvocadoInvokeDoc("SetDocParam",GetActiveAvocadoDoc ()->GetAvoID (), "string bgcolor=0.0");*/
	}


	BOOL CAvocadoDoc::OnUpdateDocument()
	{
		return COleServerDocEx::OnUpdateDocument();
	}


	void CAvocadoDoc::OnUpdateFileUpdate(CCmdUI *pCmdUI)
	{
		//if (GetActiveDocument() &&  ((COleServerDoc *) GetActiveDocument())->IsEmbedded () )
		pCmdUI->Enable (IsEmbedded ());
	}

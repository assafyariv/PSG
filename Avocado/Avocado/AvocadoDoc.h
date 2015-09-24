/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
// will pay :)
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

// AvocadoDoc.h : interface of the CAvocadoDoc class
//

#pragma once
#include <string>
#include "../AvocadoEngine/AvocadoAppInterface.h"
//#ifdef SHARED_HANDLERS
class CAvocadoThumbExtract;

//#endif
class CAvocadoDocInterface : public avocado::AvocadoDocInterface 
{
public:
	
	CAvocadoDocInterface (void* doc) 
	: avocado::AvocadoDocInterface ()
	{
		m_doc = doc;
	}

//	~CAvocadoDocInterface () : ~AvocadoDocInterface () {}
	virtual void ElementsChanged (avocado::AvocadoElementsList elementsList ,avocado::AvocadoFilesList filesList);
	virtual void ViewStateChanged (avocado::AvocadoViewStateList viewStateList, int current );
	virtual void MaterialStateChanged (avocado::AvocadoMaterialStateList materialStateList);
	virtual void DocParamChanged (const char *paramName, const char *value);
	virtual void ErrorCallback (const char *errDesc) ;
	virtual void SuccessCallback () ;
	virtual void DocuemntStatusCallback (DocumentStatus stat, std::string name, int progress ) ;
	void* m_doc;
};


class CAvocadoSrvrItem;
class CAvocadoDoc :   public COleServerDocEx
{
protected: // create from serialization only
	CAvocadoDoc();
	DECLARE_DYNCREATE(CAvocadoDoc)
	DECLARE_OLECREATE(CAvocadoDoc)
// Attributes
public:
	CAvocadoSrvrItem* GetEmbeddedItem()
		{ return reinterpret_cast<CAvocadoSrvrItem*>(COleServerDocEx::GetEmbeddedItem()); }

// Operations
public:
// Overrides
public:
    int  AvocadoCreateView(void *phWnd, void *viewInterface);
	bool AvocadoResizeView(int viewId, int px, int py);
    bool AvocadoInvokeUI   (UINT msg, int viewId, int x, int y, int zDelta);

	bool AvocadoInvokeDoc  (const char *msg, int docId, const char * paramStr,const char* targetModule = "");
	bool AvocadoInvokeView (const char *msg, int viewId, const char * paramStr,bool toAllModules= true) ;
	bool AvocadoSerializeDoc(int viewId, std::string &paramStr ,bool isImport = false,bool isStoring = false);

	bool AvocadoInvokeDocW  (const TCHAR* msg, int docId, const TCHAR* paramStr,const TCHAR* targetModule = _T(""));
	bool AvocadoInvokeViewW (const TCHAR*  msg, int viewId, const TCHAR* paramStr,bool toAllModules= true) ;
	bool AvocadoSerializeDocW(int viewId, const TCHAR* paramStr ,bool isImport = false,bool isStoring = false);

	void RaiseError (std::string err);
		int GetDocParameterAsInt (string paramName);
	bool SetDocParameter (string name,string value);

	virtual void OnDocumentEvent(DocumentEvent deEvent);
	virtual BOOL OnNewDocument();
	virtual bool ImportFile (LPCTSTR lpszFileName);
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS
	void OnSerializeDocResume();
	void OnSerializeDocComplete();

	

private:

	
	std::vector<std::string>  m_materialStates;
	int m_lastProgress;
	std::string  m_lastStatus;
	bool m_isPublishing ;
	int m_id;
	size_t m_cachedElemID;
	avocado::AvocadoElementsList m_elementsList;
	avocado::AvocadoFilesList	 m_filesList;
	vector <pair<string,string>>            m_docParameters;
	void SaveDocParamsToFile (CString localPath,std::vector<std::string> &htmlfiles);
// Implementation
public:
	void SetMaterialStates (std::vector<std::string> msList);
	std::vector<std::string> GetMaterialStates ();
	bool GetIsSaving () { return m_isSaving;}
	void SetDocumentStatus (std::string stat,int progress);
	void GetDocumentStatus (std::string &stat,int &progress);
	
	void SetPublishing (bool pub) { m_isPublishing = pub ; };
	virtual ~CAvocadoDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	virtual CDocObjectServer* GetDocObjectServer(LPOLEDOCUMENTSITE pDocSite);
		virtual void OnIdle();
	bool    m_isAutoSaving;
	bool    m_isSaving;
	CAvocadoDocInterface *m_docInterface;
	bool GetBitmapThumbnail (LPCTSTR lpszPathName);
//#ifdef SHARED_HANDLERS
	CAvocadoThumbExtract *m_thumbX;
//#endif
// Generated message map functions
protected:
	//afx_msg void OnSaveDoc();
	afx_msg void OnUpdateSaveDoc(CCmdUI *pCmdUI);
	//afx_msg void OnUpdateAddSectionPlane(CCmdUI *pCmdUI);
	//afx_msg void OnAddSectionPlane();
	afx_msg void OnLoadDoc();
	DECLARE_MESSAGE_MAP()
	static CLIPFORMAT NEAR m_cfPrivate;
	CAvocadoSrvrItem* m_pItem ;
#ifdef SHARED_HANDLERS
	// Helper function that sets search content for a Search Handler
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
public:
	afx_msg long GetAvoID (); 
	afx_msg void SetAvoID (long newValue) {m_id = newValue;}//BUG};
	bool m_hasHiddenElements;
	void SetViewStates (avocado::AvocadoViewStateList viewStateList, int current);
	int m_currentViewState;
	std::vector<std::string> m_viewStatesImages;
	std::vector<std::string> m_viewStatesHtml;
	void SetElements (avocado::AvocadoElementsList cnt) ;
	void SetFiles (avocado::AvocadoFilesList cnt) ;
	int	 GetElementsCount ();
	void GetElementName (int id, std::string &tname);
	void GetElementVisibility (int id , bool &visible );
	void GetElementColor (int id , std::vector<int> &col );
	unsigned int GetElementContext (int id );
    avocado::AvocadoMaterialInterface *GetElementMaterialData (int id );
	std::string GetElementMetaData (int id, std::string varName);
	void SetElementMetaData (int id , std::string metaVarName ,std::string data);
	std::string GetElementAnnotationData (int id, std::string varName);
	void GetElements (avocado::AvocadoElementsList &list);
	void GetFiles (avocado::AvocadoFilesList &list);
	virtual void OnChangedViewList();
	bool CAvocadoDoc::SetViewStateImage (int vsi,std::string & imageString );
	int GetCurrentViewState ();
	std::string GetViewStateImage (int vsi);
	std::string GetViewStateHtml (int vsi );
	bool SetViewStateHtml (int vsi,std::string & htmlString );
	size_t GetViewStateCount () { return m_viewStatesHtml.size();}
	float GetDocParameterAsFloat (std::string paramName);
	string GetDocParameterAsString (std::string paramName);
	//afx_msg void OnSerializeElements();
	//afx_msg void OnSerializeDocElements();
	//afx_msg void OnSaveDoc();
	//afx_msg void OnSaveAvoDoc();
	//afx_msg void OnUpdateString61204(CCmdUI *pCmdUI);
//	afx_msg void OnUpdateSelectPart(CCmdUI *pCmdUI);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);


	// Generated OLE dispatch map functions

	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()

	virtual void DeleteContents();
	virtual BOOL OnUpdateDocument();
	afx_msg void OnUpdateFileUpdate(CCmdUI *pCmdUI);

	// thread///
	bool ser_thread_b1;
	bool ser_thread_b2;
	int ser_thread_i1;
	std::string ser_thread_p1;
};

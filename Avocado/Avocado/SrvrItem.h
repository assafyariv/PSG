
// SrvrItem.h : interface of the CSDISampleSrvrItem class
//

#pragma once

class CAvocadoSrvrItem : public COleServerItem//CDocObjectServerItem
{
	DECLARE_DYNAMIC(CAvocadoSrvrItem)

// Constructors
public:
	CAvocadoSrvrItem(CAvocadoDoc* pContainerDoc);

// Attributes
	CAvocadoDoc* GetDocument() const
		{ return reinterpret_cast<CAvocadoDoc*>(COleServerItem::GetDocument()); }

// Overrides
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	virtual BOOL OnRenderFileData(LPFORMATETC lpFormatEtc, CFile* pFile);
// Implementation
public:
	~CAvocadoSrvrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
public:
	virtual void OnDoVerb(LONG iVerb);
};


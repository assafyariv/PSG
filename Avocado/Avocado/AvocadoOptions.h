#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "../AvocadoEngine/AvocadoAppInterface.h"

class COptionsDlg : public CDialogEx
{
public:
	COptionsDlg();
	
	// Dialog Data
	enum { IDD = IDD_OPTIONS_DIALOG };

protected:
	virtual void LoadPage (int pageNum);
	virtual void ClearCurrentPage ();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void SetOptionOverride (avocado::AvocadoOption op);
	virtual bool GetOptionOverride (avocado::AvocadoOption &op);
	virtual void  SaveOptions ();
	// Implementatiopn
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimertog();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnUpdateSaveDoc(CCmdUI *pCmdUI);
	virtual BOOL OnInitDialog();
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	CListBox m_pageList;
	avocado::AvocadoOptionStructure m_opStruct;
	int m_currentPage;
	std::vector<avocado::AvocadoOption>   m_overrides;
	std::vector<std::pair<void *,avocado::AvocadoOption>> m_currentPageControls;
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
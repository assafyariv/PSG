#pragma once

// AvocadoActiveXCtrlPropPage.h : Declaration of the CAvocadoActiveXCtrlPropPage property page class.


// CAvocadoActiveXCtrlPropPage : See AvocadoActiveXCtrlPropPage.cpp for implementation.

class CAvocadoActiveXCtrlPropPage : public COlePropertyPage
{
	DECLARE_DYNCREATE(CAvocadoActiveXCtrlPropPage)
	DECLARE_OLECREATE_EX(CAvocadoActiveXCtrlPropPage)

// Constructor
public:
	CAvocadoActiveXCtrlPropPage();

// Dialog Data
	enum { IDD = IDD_PROPPAGE_AVOCADOACTIVEXCTRL };

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Message maps
protected:
	DECLARE_MESSAGE_MAP()
};


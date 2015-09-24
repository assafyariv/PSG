#include "stdafx.h"
#include "AvocadoOptions.h"
#include "Avocado.h"
#include "../AvocadoEngine/AvocadoAppOptionsInterface.h"
#include <sstream>

COptionsDlg::COptionsDlg() : CDialogEx(COptionsDlg::IDD)
{
}

void COptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_pageList);
}

BEGIN_MESSAGE_MAP(COptionsDlg, CDialogEx)
	//ON_COMMAND(IDC_TIMERTOG, &CAboutDlg::OnTimertog)
//	ON_UPDATE_COMMAND_UI(IDC_SAVE_DOC, &CAboutDlg::OnUpdateSaveDoc)
	ON_LBN_SELCHANGE(IDC_LIST1, &COptionsDlg::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDOK, &COptionsDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &COptionsDlg::OnBnClickedCancel)
END_MESSAGE_MAP()

bool COptionsDlg::GetOptionOverride (avocado::AvocadoOption &op)
{
	for (size_t i=0;i<m_overrides.size();i++)
	{
		if (m_overrides[i].Name == op.Name )
		{
			op = m_overrides[i] ;
			return true;
		}
	}
	return false;
}
void COptionsDlg::SetOptionOverride (avocado::AvocadoOption op)
{
	for (size_t i=0;i<m_overrides.size();i++)
	{
		if (m_overrides[i].Name == op.Name )
		{
			m_overrides[i] = op;
			return;
		}
	}
	m_overrides.push_back (op);
}
void COptionsDlg::ClearCurrentPage ()
{
	CWnd * parentGroupBox = GetDlgItem (IDC_OPTIONGROUP);
	for (size_t i=0;i<m_currentPageControls.size ();i++)
	{
		CWnd *item = parentGroupBox->GetDlgItem  ( ((CWnd*)(m_currentPageControls[i].first))->GetDlgCtrlID ());
		if (item)
		{				
				if (m_currentPageControls[i].second.UIType == avocado::AvocadoOption::CHECKBOX)
				{
					CButton *obj = (CButton*)(m_currentPageControls[i].first);
					
				    bool ischek = (obj->GetCheck () > 0);

					m_currentPageControls[i].second.valueBool = ischek;

					item->DestroyWindow ();
					
					delete obj;
				}
				else if (m_currentPageControls[i].second.UIType == avocado::AvocadoOption::EDITBOX)
				{
					 int labelID = ((CWnd*)(m_currentPageControls[i].first))->GetDlgCtrlID () - 1100 + 2200;

					CWnd *labelitem = parentGroupBox->GetDlgItem  (labelID);
					
					if (labelitem)
					{
						labelitem->DestroyWindow ();
						CStatic *css = (CStatic*) labelitem;
						delete css;
					}
					CEdit *obj = (CEdit*)(m_currentPageControls[i].first);

					wchar_t buf[MAX_PATH];
					obj->GetWindowTextW (buf, MAX_PATH);
					char cbuf [MAX_PATH];
					::wcstombs (cbuf,buf,MAX_PATH);
					
					if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::STRING)
						m_currentPageControls[i].second.valueString = string (cbuf);
					else if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::FLOAT)
					{
						stringstream ss ( cbuf);
						ss >> m_currentPageControls[i].second.valueFloat;
					}
					else if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::BOOL)
					{
						string ss ( cbuf);
						bool bv = false;
						if (ss == "Yes")
							bv = true; else bv = false;
						m_currentPageControls[i].second.valueBool = bv;
					}
					else if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::INT)
					{
						stringstream ss ( cbuf);
						ss >> m_currentPageControls[i].second.valueInt;
					}

					item->DestroyWindow ();
					
					delete obj;
				}
				else if (m_currentPageControls[i].second.UIType == avocado::AvocadoOption::COMBOLIST)
				{
					 int labelID = ((CWnd*)(m_currentPageControls[i].first))->GetDlgCtrlID () - 1100 + 2200;

					CWnd *labelitem = parentGroupBox->GetDlgItem  (labelID);
					
					if (labelitem)
					{
						labelitem->DestroyWindow ();
						CStatic *css = (CStatic*) labelitem;
						delete css;
					}
					CComboBox *obj = (CComboBox*)(m_currentPageControls[i].first);
					/*
					wchar_t buf[MAX_PATH];
					obj->GetWindowTextW (buf, MAX_PATH);
					char cbuf [MAX_PATH];
					::wcstombs (cbuf,buf,MAX_PATH);
					*/
					int curSel = obj->GetCurSel ();

					if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::STRING)
					{
						m_currentPageControls[i].second.valueString = m_currentPageControls[i].second.possibleValuesString[curSel];

					}
					else if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::FLOAT)
					{
						m_currentPageControls[i].second.valueFloat = m_currentPageControls[i].second.possibleValuesFloat[curSel];

					}
					else if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::BOOL)
					{
							m_currentPageControls[i].second.valueBool = m_currentPageControls[i].second.possibleValuesBool[curSel];
					}
					else if (m_currentPageControls[i].second.Type == avocado::AvocadoOption::INT)
					{
						m_currentPageControls[i].second.valueInt = m_currentPageControls[i].second.possibleValuesInt[curSel];
					}

					item->DestroyWindow ();
					
					delete obj;
				}

		
				else if (m_currentPageControls[i].second.UIType == avocado::AvocadoOption::SPINBOX)
				{
					 int labelID = ((CWnd*)(m_currentPageControls[i].first))->GetDlgCtrlID () - 1100 + 2200;

					CWnd *labelitem = parentGroupBox->GetDlgItem  (labelID);
					
					if (labelitem)
					{
						labelitem->DestroyWindow ();
						CStatic *css = (CStatic*) labelitem;
						delete css;
					}
					CSpinButtonCtrl *obj = (CSpinButtonCtrl*)(m_currentPageControls[i].first);
					CStatic *csbuddy = (CStatic*)obj->GetBuddy ();
					if (csbuddy)
					{
						csbuddy->DestroyWindow ( );
						delete csbuddy;
					}
/*
					wchar_t buf[MAX_PATH];
					obj->GetWindowTextW (buf, MAX_PATH);
					char cbuf [MAX_PATH];
					::wcstombs (cbuf,buf,MAX_PATH);
*/

					m_currentPageControls[i].second.valueInt = obj->GetPos ();

					item->DestroyWindow ();
					
					delete obj;
				}

				SetOptionOverride (m_currentPageControls[i].second);
		}
	}
	this->Invalidate ();
	m_currentPageControls.clear ();
}

void COptionsDlg::LoadPage (int pageNum )
{
	if (pageNum < 0 || pageNum >int(m_opStruct.pages.size()))
		return;
	CWnd * parentGroupBox = GetDlgItem (IDC_OPTIONGROUP);
	wchar_t pageName [MAX_PATH];
	::mbstowcs (pageName,m_opStruct.pages[pageNum].name.c_str(),MAX_PATH);

	parentGroupBox->SetWindowTextW (pageName);
	for (size_t opi =0;opi<m_opStruct.pages[pageNum].options.size ();opi ++)
	{
		avocado::AvocadoOption co = m_opStruct.pages[pageNum].options[opi];
		if (co.UIType == avocado::AvocadoOption::HIDDEN)
			continue;

		if (co.UIType == avocado::AvocadoOption::CHECKBOX )
		{
			 CButton *chkb =  new CButton;
			 wchar_t blabel [MAX_PATH];
			 ::mbstowcs (blabel,co.Label.c_str(),MAX_PATH);
			 int id = 1100;
			 id += int (opi);
			 
			 chkb->Create(blabel,WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX,CRect(10, 15 + int ( opi )*32, 140, 45 + int (opi)*32), parentGroupBox, id);

			 m_currentPageControls.push_back (std::pair<void*,avocado::AvocadoOption> ((void*)(chkb),co));
			 bool ischeck;
			 avocado::AvocadoOption opo;
			 opo.Name = co.Name;
			 if (GetOptionOverride (opo))
			 {
				 ischeck = opo.valueBool;
			 }
			 else
			 {
				 if (!avocado::GetEngineOptionBool (co.Name,&ischeck))
				 {
					 ischeck = false;
				 }
			 }
			 chkb->SetCheck (ischeck);
		} 
		else if (co.UIType == avocado::AvocadoOption::EDITBOX )
		{
			 CEdit *chkb =  new CEdit;
			 wchar_t blabel [MAX_PATH];
			 ::mbstowcs (blabel,co.Label.c_str(),MAX_PATH);
			 int id = 1100;
			 id += int (opi);
			 
			 chkb->Create(WS_CHILD | WS_VISIBLE ,CRect(250, 15 + int(opi)*32, 340, 45 + int(opi)*32), parentGroupBox, id);
			 ::CStatic *labb = new CStatic ();
			 labb->Create (blabel,WS_CHILD | WS_VISIBLE | BS_TEXT,CRect (10,int(opi)*32 + 15,240, int(opi)*32 + 45),parentGroupBox, 2200 + int(opi));
			 labb->SetWindowTextW (blabel);

			 m_currentPageControls.push_back (std::pair<void*,avocado::AvocadoOption> ((void*)(chkb),co));

			 string ischeck;
			 avocado::AvocadoOption opo;
			 opo.Name = co.Name;

			 if (GetOptionOverride (opo))
			 {   
				 if (opo.Type == avocado::AvocadoOption::STRING)
					ischeck = opo.valueString;
				 else
				{
						 stringstream ss;
						 if (opo.Type == avocado::AvocadoOption::INT)
						 {
							ss << opo.valueInt;
						 } else if (opo.Type == avocado::AvocadoOption::BOOL)
						 {
							 if (opo.valueBool)
								ss << "Yes"; 
							 else
								 ss << "No";
						 } 
						 else if (opo.Type == avocado::AvocadoOption::FLOAT)
						 {
							ss << opo.valueFloat;
						 }
						 ischeck = ss.str ();
				 }
			 }
			 else
			 {

				if (co.Type == avocado::AvocadoOption::STRING)
				{
				 if (!avocado::GetEngineOptionString (co.Name,&ischeck))
				 { 
					 ischeck = "";
				 }
				} 
				else
					{
						ischeck = "";
						stringstream ss;
						if (co.Type == avocado::AvocadoOption::FLOAT)
						{
							float valf;
							if (avocado::GetEngineOptionFloat (co.Name,&valf))
							{ 
							  ss << valf;
							} 
						}
						else if (co.Type == avocado::AvocadoOption::BOOL)
						{
							bool valf;
							if (avocado::GetEngineOptionBool (co.Name,&valf))
							{ 
								if (valf)
							  ss << "Yes"; else ss << "No";
							} 

						}
						else if (co.Type == avocado::AvocadoOption::INT)
						{
							int valf;
							if (avocado::GetEngineOptionInt (co.Name,&valf))
							{ 
							  ss << valf;
							} 

						}
						ischeck = ss.str ();
				}
			 }
			 wchar_t bval [MAX_PATH];
			 ::mbstowcs (bval,ischeck.c_str (),MAX_PATH);
			 
			 chkb->SetWindowTextW (bval);
		}

		else if (co.UIType == avocado::AvocadoOption::SPINBOX )
		{
			
			 CSpinButtonCtrl  *chkb =  new CSpinButtonCtrl;
			 wchar_t blabel [MAX_PATH];
			 ::mbstowcs (blabel,co.Label.c_str(),MAX_PATH);
			 int id = 1100;
			 id += int (opi);
			 
			 chkb->Create(WS_CHILD | WS_VISIBLE | UDS_SETBUDDYINT ,CRect(150, 15 + int(opi)*32, 200, 45 + int(opi)*32), parentGroupBox, id);
			 ::CStatic *labb = new CStatic ();
			 labb->Create (blabel,WS_CHILD | WS_VISIBLE | BS_TEXT,CRect (10,int(opi)*32 + 15,140, int(opi)*32 + 45),parentGroupBox, 2200 + int(opi));
			 labb->SetWindowTextW (blabel);

			 ::CStatic *buddy = new CStatic ();
			 buddy->Create (blabel,WS_CHILD | WS_VISIBLE | BS_TEXT,CRect (200,int(opi)*32 + 15,240, int(opi)*32 + 45),parentGroupBox, 2300 + int(opi));
			 buddy->SetWindowTextW (L"0");

			 m_currentPageControls.push_back (std::pair<void*,avocado::AvocadoOption> ((void*)(chkb),co));
			 int ischeck;
			 avocado::AvocadoOption opo;
			 opo.Name = co.Name;
			 if (GetOptionOverride (opo))
			 {
				 ischeck = opo.valueInt;
			 }
			 else
			 {
				 if (!avocado::GetEngineOptionInt (co.Name,&ischeck))
				 {
					 ischeck = 0;
				 }
			 }
			 chkb->SetPos(ischeck);
			 chkb->SetRange (co.scrollMin,co.scrollMax);
			 chkb->SetBuddy (buddy);
		}
		else if (co.UIType == avocado::AvocadoOption::COMBOLIST )
		{
			
			 CComboBox *chkb =  new CComboBox;
			 wchar_t blabel [MAX_PATH];
			 ::mbstowcs (blabel,co.Label.c_str(),MAX_PATH);
			 int id = 1100;
			 id += int (opi);
			 
			 chkb->Create(WS_CHILD | WS_VISIBLE | CBS_DROPDOWNLIST,CRect(150, 15 + int(opi)*32, 240, 45 + int(opi)*32), parentGroupBox, id);
			 ::CStatic *labb = new CStatic ();
			 labb->Create (blabel,WS_CHILD | WS_VISIBLE | BS_TEXT,CRect (10,int(opi)*32 + 15,140, int(opi)*32 + 45),parentGroupBox, 2200 + int(opi));
			 labb->SetWindowTextW (blabel);

			 m_currentPageControls.push_back (std::pair<void*,avocado::AvocadoOption> ((void*)(chkb),co));

			 for (size_t ii = 0; ii < co.listValueNames.size(); ii++)
			 {
				 wchar_t sbfr[MAX_PATH];
				 ::mbstowcs (sbfr,co.listValueNames[ii].c_str(),MAX_PATH);
				 chkb->InsertString (int(ii),sbfr);
			 }
			
			// string ischeck;
			 int curSel = 0;
			 avocado::AvocadoOption opo;
			 opo.Name = co.Name;
			 if (GetOptionOverride (opo))
			 {
				 if (co.Type == avocado::AvocadoOption::STRING)
				 {
					 for (size_t kg = 0; kg < co.possibleValuesString.size ();kg++)
					 {
						 if (co.possibleValuesString[kg] == opo.valueString)
						 {
							 curSel = int(kg);
						 }
					 }
				 }
				 else if (co.Type == avocado::AvocadoOption::INT)
				 {
					 for (size_t kg = 0; kg < co.possibleValuesInt.size ();kg++)
					 {
						 if (co.possibleValuesInt[kg] == opo.valueInt)
						 {
							 curSel =  int(kg);
						 }
					 }
				 } else if (co.Type == avocado::AvocadoOption::FLOAT)
				 {
					 for (size_t kg = 0; kg < co.possibleValuesString.size ();kg++)
					 {
						 if (co.possibleValuesFloat[kg] == opo.valueFloat)
						 {
							 curSel =  int(kg);
						 }
					 }
				 }
				 else if (co.Type == avocado::AvocadoOption::BOOL)
				 {
					 for (size_t kg = 0; kg < co.possibleValuesBool.size ();kg++)
					 {
						 if (co.possibleValuesBool[kg] == opo.valueBool)
						 {
							 curSel =  int(kg);
						 }
					 }
				 }

			 }
			 else
			 {
				 if (co.Type == avocado::AvocadoOption::BOOL)
				 {
					 bool ischeck;
					 avocado::GetEngineOptionBool (co.Name,&ischeck);
					 for (size_t kg = 0; kg < co.possibleValuesBool.size ();kg++)
					 {
						 if (co.possibleValuesBool[kg] == ischeck)
						 {
							 curSel =  int(kg);
						 }
					 }
				 }
				 else if (co.Type == avocado::AvocadoOption::FLOAT)
				 {
					 float ischeck;
					 avocado::GetEngineOptionFloat (co.Name,&ischeck);
					 for (size_t kg = 0; kg < co.possibleValuesFloat.size ();kg++)
					 {
						 if (co.possibleValuesFloat[kg] == ischeck)
						 {
							 curSel =  int(kg);
						 }
					 }
				 }
				 else if (co.Type == avocado::AvocadoOption::STRING)
				 {
					 string ischeck;
					 avocado::GetEngineOptionString (co.Name,&ischeck);
					 for (size_t kg = 0; kg < co.possibleValuesString.size ();kg++)
					 {
						 if (co.possibleValuesString[kg] == ischeck)
						 {
							 curSel =  int(kg);
						 }
					 }
				 }
				 else if (co.Type == avocado::AvocadoOption::INT)
				 {
					 int ischeck;
					 avocado::GetEngineOptionInt (co.Name,&ischeck);
					 for (size_t kg = 0; kg < co.possibleValuesInt.size ();kg++)
					 {
						 if (co.possibleValuesInt[kg] == ischeck)
						 {
							 curSel =  int(kg);
						 }
					 }
				 }
			 }
			 chkb->SetCurSel (curSel);
		}

	}
	m_currentPage = pageNum;
}
void COptionsDlg::OnLbnSelchangeList1 ()
{
	int sel = m_pageList.GetCurSel ();
	if (sel != m_currentPage)
	{
		ClearCurrentPage ();
		LoadPage (sel);
	}
}

BOOL COptionsDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	avocado::GetEngineOptionStructure (m_opStruct);

	for (size_t pageI = 0; pageI < m_opStruct.pages.size ();pageI++)
	{

		wchar_t dst[MAX_PATH];
		::mbstowcs (dst, m_opStruct.pages[pageI].name.c_str(),MAX_PATH );
		m_pageList.InsertString (int (pageI),dst);
	}
	if (m_opStruct.pages.size ())
		LoadPage (0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT COptionsDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
#if 0
	// TODO: Add your specialized code here and/or call the base class
	   if ((message == WM_COMMAND)) 
	   {
        if (wParam>=1100) //ids for our buttons
        {
            int id = (int)wParam - 1100;
			for (size_t xx = 0; xx < m_currentPageControls.size ();xx++)
			{
				if (m_currentPageControls[xx].second.UIType == avocado::AvocadoOption::CHECKBOX)
				{
					//m_currentPageControls[xx].
					CButton *chkb = (CButton*)(m_currentPageControls[xx].first);
					if (chkb->GetDlgCtrlID () == wParam)
					{
						m_currentPageControls[xx].second.valueBool = chkb->GetCheck ();
						break;
					}
				}
			}
		}
	   }
#endif
	return CDialogEx::WindowProc(message, wParam, lParam);
 
}
void COptionsDlg::SaveOptions ()
{
	for  (size_t i=0;i<m_overrides.size ();i++)
	{
		if (m_overrides[i].Type == avocado::AvocadoOption::BOOL)
		{
			avocado::SetEngineOptionBool (m_overrides[i].Name,m_overrides[i].valueBool);
		} 
		else if (m_overrides[i].Type == avocado::AvocadoOption::FLOAT)
		{
			avocado::SetEngineOptionFloat (m_overrides[i].Name,m_overrides[i].valueFloat);
		}
		else if (m_overrides[i].Type == avocado::AvocadoOption::STRING)
		{
			avocado::SetEngineOptionString (m_overrides[i].Name,m_overrides[i].valueString);
		}
		else if (m_overrides[i].Type == avocado::AvocadoOption::INT)
		{
			avocado::SetEngineOptionInt (m_overrides[i].Name,m_overrides[i].valueInt);
		}
	}
	if (m_overrides.size () > 0)
		avocado::SaveOptionsFile ();
}
void COptionsDlg::OnBnClickedOk()
{
	ClearCurrentPage  ();

	SaveOptions ();
	
	m_overrides.clear ();

	CDialogEx::OnOK();
}


void COptionsDlg::OnBnClickedCancel()
{
	ClearCurrentPage  ();
	
	m_overrides.clear ();

	CDialogEx::OnCancel();
}

bool CAvocadoApp::GetAvocadoOptionBool (std::string optionName)
{
	bool res = false;
	avocado::GetEngineOptionBool (optionName,&res);
	return res;
}
string CAvocadoApp::GetAvocadoOptionString (std::string optionName)
{
	string res = "";
	avocado::GetEngineOptionString (optionName,&res);
	return res;
}
int CAvocadoApp::GetAvocadoOptionInt (std::string optionName)
{
	int res = 0;
	avocado::GetEngineOptionInt (optionName,&res);
	return res;
}
float CAvocadoApp::GetAvocadoOptionFloat (std::string optionName)
{
	float res = 0.0f;
	avocado::GetEngineOptionFloat (optionName,&res);
	return res;
}
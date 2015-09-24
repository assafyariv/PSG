#include "stdafx.h"
#include "afxctl.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "Avocado.h"
#include <sstream>
#include "../AvocadoHandlers/AvocadoThumbnailExtractor.h"

// START GALLERY MANAGER CODE SECTION
using namespace std;

static std::wstring GetProccessDirectory ()
{ 
	// windows specific stuff inside the engine is never a good thing :(
	wchar_t szPath[MAX_PATH];
	if (GetModuleFileName(NULL, szPath, ARRAYSIZE(szPath)))
	{
		std::wstring mfp (szPath);
		mfp = mfp.substr (0,mfp.rfind (L"\\"));
		mfp += wstring (L"\\media\\");
		return mfp; 
	}
	return wstring(L".\\media\\");
}

class CGalleryManagerDlg2 : public CDialogEx
{
public:
	CGalleryManagerDlg2();

	// Dialog Data
	enum { IDD = IDD_GALLERYMANAGER_REMOVEPRESET };
	//string m_savedName;
	CString m_title;
	void SetData (vector <GalleryRec> & data);

	vector <GalleryRec>  m_allRecs;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	// Implementation
	
	void SelectFirstItem();
	void UpdateOKButton(BOOL enable);
	void UpdateRenameButton(BOOL enable);
	void InitList();
	string  GetEditText();
	void SetEditText (string text);
	void SetControlPicture (CString oname);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedRemovePreset();
	afx_msg void OnLbnSelchangeList1();
	afx_msg void OnBnClickedButton1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedPresetImage();
	afx_msg void OnEnChangeEdit1();
};
void CGalleryManagerDlg2::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGalleryManagerDlg2, CDialogEx)
	ON_BN_CLICKED(IDC_REMOVE_PRESET, &CGalleryManagerDlg2::OnBnClickedRemovePreset)
	ON_LBN_SELCHANGE(IDC_LIST1, &CGalleryManagerDlg2::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON1, &CGalleryManagerDlg2::OnBnClickedButton1)
	ON_BN_CLICKED(IDC_PRESET_IMAGE, &CGalleryManagerDlg2::OnBnClickedPresetImage)
	ON_EN_CHANGE(IDC_EDIT1, &CGalleryManagerDlg2::OnEnChangeEdit1)
END_MESSAGE_MAP()

void CGalleryManagerDlg2::InitList ()
{
	CListBox *e = (CListBox*)GetDlgItem (IDC_LIST1);
	if (e)
	{
		e->ResetContent();
		// fill in reverse..
		for (size_t i=0; i< m_allRecs.size(); i++)
		{
			e->AddString (CString(m_allRecs[i].name.c_str()));
		}
	}
}
void CGalleryManagerDlg2::SetData (vector <GalleryRec> & data)
{

	InitList();

};

CGalleryManagerDlg2::CGalleryManagerDlg2() : CDialogEx(CGalleryManagerDlg2::IDD)
{
}
class CGalleryManagerDlg : public CDialogEx
{
public:
	CGalleryManagerDlg();

	// Dialog Data
	enum { IDD = IDD_GALLERYMANAGER };
	string m_savedName;
	CString m_title;
	string m_savedImage;
	void SetEditText (string text);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support


	// Implementation
	void SetControlPicture (CString oname);
	void CGalleryManagerDlg::LoadPicture( CString filename);
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnTimertog();
	afx_msg void OnUpdateSaveDoc(CCmdUI *pCmdUI);
	afx_msg void OnBnClickedOk();
	afx_msg void OnEnChangeEdit1();
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedButton3();
};

void CGalleryManagerDlg::SetEditText (string text)
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit *e = (CEdit*)GetDlgItem (IDC_EDIT1);

	m_savedName = text;

	if (e )
	{
		wchar_t pname[MAX_PATH];
		::mbstowcs (pname,text.c_str(),MAX_PATH);

		// some validation wont hurt..
		e->SetWindowTextW (pname);
	}
}
void CGalleryManagerDlg::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit *e = (CEdit*)GetDlgItem (IDC_EDIT1);

	m_savedName = "";
	if (e != NULL)
	{
		if (e->GetWindowTextLengthW ()>0) 
		{
			wchar_t pname[MAX_PATH];
			char cname [MAX_PATH];
			e->GetWindowTextW (pname,MAX_PATH);
			// some validation wont hurt..

			::wcstombs (cname,pname,MAX_PATH);
			m_savedName = string (cname);
		}
	}
}

CString CGalleryManager::GetDefaultTexturePath ()
{
	CString file =  CString(GetProccessDirectory().c_str()) + "textures\\" + m_galleryDefaultIcon ;
	return file;
}
string CGalleryManager::GetDefaultValue (int i)
{
	if (i>=0 && i <6)
	{
		stringstream sst;
		sst << i;

		switch (m_cmdID)
		{
		case ID_AVOACDOGALLERY_SKIN:
			return string ("myskintexture") + sst.str () + string (".dds");
			break;
		case ID_AVOACDOGALLERY_HAIR:
			return string ("myhairtexture") + sst.str () + string (".dds");
			break;
		case ID_LAYOUT_BACKGROUNIMAGEGALLRY:
			return string ("crosscube") + sst.str () + string (".hdr");
			break;
		case ID_AVOACDOGALLERY_BUMP:
			return string ("mybump") + sst.str () + string (".dds");
			break;
		case ID_AVOACDOGALLERY_TEXTURE:
			return string ("mytexture") + sst.str () + string (".dds");
			break;
		case ID_AVOACDOGALLERY_WOODBUMP:
			return string ("mywoodtexture") + sst.str () + string (".dds");
			break;
		case ID_AVOCADOVIEW_MATERIAL:
			//special case for materials and lights..
			break;
		case ID_AVOACDOGALLERY_LIGHT1:
		case ID_AVOACDOGALLERY_LIGHT2:
		case ID_AVOACDOGALLERY_LIGHT3:
			//special case for materials and lights..
			break;
		default:
			break;
		};
	}
	return "";
}
bool CGalleryManager::RestoreToDefault ()
{
	// delete the gallery xml file here..
	if (::AfxMessageBox (L"This action will delete all of your saved presets and restore the factory defaults.\n Are you sure ?",MB_YESNO)== IDYES)
	{
		::DeleteFile (this->m_fileName.GetString());
		// restore to default pallete.
		m_gal->SetPalette (m_defaultPallete,48);
		return true;
	}
	return false;
}

string CGalleryManager::GetPresetValue (UINT cmd)
{
	int index = m_gal->GetSelectedItem ();
	if (index >= 0)
	{
		PopulateFromFile();
		if (index >= 0 && index <int(allRecs.size()) )
			return allRecs[index].data;
		else
		{
			if (allRecs.empty())
			{
				return (GetDefaultValue (index));
			} else
				return ""; // error
		}
	}
	return "";
}
bool CGalleryManager::EditPresets()
{

	CGalleryManagerDlg2 dlg;

	//string res = "New Preset";
	// get preset new name..
	PopulateFromFile();
	if (allRecs.size()>0)
	{
		dlg.m_allRecs = allRecs;
		dlg.m_title = m_title;

		if (dlg.DoModal () == IDOK)
		{
			allRecs.clear();
			allRecs = dlg.m_allRecs;
			// save preset file..
			SaveToFile();
			// update the related galleries.
			InitGallery ();
			return true;
		}
	}
	else
	{
		AfxMessageBox (L"There are no user presets available. \n Please add a preset before editing.");
	}
	return false;
}
bool CGalleryManager::AddPresetDlg (GalleryRec rec,bool raiseImageDlg)
{
	string res = "New Preset";
	// get preset new name..
	CGalleryManagerDlg dlg;
	//dlg.SetWindowTextW (m_title.GetString ());

	if (raiseImageDlg)
	{
		char inputData[MAX_PATH];
		CString fopendialogResult = theApp.RaiseImageOpenDialog ();
		if (fopendialogResult== CString (""))
			return false;
		if (GetFileAttributes(fopendialogResult.GetString()) == 0xFFFFFF)
			return false;
		::wcstombs (inputData,(fopendialogResult).GetString(),MAX_PATH);
		rec.data = string (inputData);
		rec.image = string (inputData);
	}
	dlg.m_title = m_title;
	dlg.m_savedName = res;
	if (rec.image == "")
	{
		char sfile [MAX_PATH];
		::wcstombs(sfile,GetDefaultTexturePath().GetString (),MAX_PATH);
		dlg.m_savedImage = string (sfile);
	}
	else
		dlg.m_savedImage = rec.image;

	if (dlg.DoModal () == IDOK)
	{
		rec.name = dlg.m_savedName;
		rec.image = dlg.m_savedImage;
		//load preser file...
		PopulateFromFile();
		// add the new preset data and name.
		allRecs.push_back ( rec );
		// save preset file..
		SaveToFile();
		// update the related galleries.
		InitGallery ();
		return true;
	}
	return false;
}

void CGalleryManager::InitGallery(bool redraw)
{
	CMFCToolBarImages pal;
	//
	if (m_gal)
	{
		pal.SetImageSize  (CSize(48,48));
		for (size_t i=0;i< allRecs.size();i++)
		{

			CString tfile ;//= CString(GetProccessDirectory().c_str());
			if (allRecs[i].image == "")
				tfile= GetDefaultTexturePath ();// CString  (L"galleries\\gallery_lights.bmp");
			else
			{

				wchar_t filename[MAX_PATH];
				::mbstowcs (filename,allRecs[i].image.c_str (),MAX_PATH);
				tfile = CString (theApp.GetAppStorageGalleryFolder()) +  CString (filename);

			}
			HBITMAP hbmp = (HBITMAP)::LoadImage(NULL,tfile.GetString(),
				IMAGE_BITMAP,48,48,LR_LOADFROMFILE);	

			//if (i >= allRecs.size())
			//	pal.DeleteImage (i);
			pal.AddImage (hbmp);	
		}

		//	m_gal->SetAlwaysLargeImage (true);
		m_gal->SetIconsInRow (4);
		if (pal.GetCount()>0)
			m_gal->SetPalette (pal);
		//m_gal->SetRect (CRect (0,0,48,48);
		//m_gal->RedrawIcons ();

		// do not force redraw on register..
		//if (redraw)
		//	m_gal->GetParentPanel

		for (size_t i=0;i<allRecs.size();i++)
		{

			m_gal->SetItemToolTip (int(i),CString(allRecs[i].name.c_str()));
		}
	}

	//HICON ih = m_gal->GetIcon (0);
	//int cnt = m_gal->getpa
}
void CGalleryManager::PopulateFromFile ()
{
	allRecs.clear ();
	if ((GetFileAttributes(m_fileName.GetString ())!=0xFFFFFFFF))
	{
		CFile file;//CFile::modeWrite | CFile::modeCreate
		file.Open (m_fileName.GetString (),CFile::modeRead);
		string fileContent = "";
		
		std::vector<string> lines;
		while (1)
		{
			char lpBuf [4096];
			UINT sizeRead = file.Read (lpBuf,4096);
			if (sizeRead <4096)
				lpBuf[sizeRead] = '\0';
			fileContent+=string(lpBuf);
			if (sizeRead < 4096)
				break;
		}
     	file.Close();
			while (fileContent.size() > 0)
				{
					size_t eolPos = fileContent.find ("\n") ;
					if (eolPos <= 0 || eolPos > fileContent.size())
						break;
					string line = fileContent.substr (0,eolPos );
					fileContent = fileContent.substr  (eolPos+1,fileContent.size()-eolPos-1);
					lines.push_back (line);
				}
		unsigned int lc = 0;

		int currentRecPos = 0;
		while (lc < lines.size())
		{
			if (lines[lc] == "PresetStart")
			{
				GalleryRec newRec;//currentRec = new GalleryRec ();
				allRecs.push_back (newRec);
				currentRecPos = int(allRecs.size ()) - 1;
			}
			else if  (lines[lc] == "PresetEnd")
			{

			} 
			else if (lines[lc] == "PresetName")
			{
				lc++;
				allRecs[currentRecPos].name= lines[lc];
			}
			else if (lines[lc] == "PresetImage")
			{
				lc++;
				allRecs[currentRecPos].image= lines[lc];
			}
			else if (lines[lc] == "PresetData")
			{
				lc++;
				allRecs[currentRecPos].data= lines[lc];
			}
			lc++;
		}
		
	}
}

void CGalleryManager::SaveToFile ()
{
	CFile file(m_fileName,CFile::modeWrite | CFile::modeCreate);
	//char lpBuf [4096];
	string lines = "";
	size_t cnt = 0;
	while (cnt<allRecs.size ())
	{
		if (allRecs[cnt].data != string ("") && allRecs[cnt].data.size () > 0)
		{
			lines+= string("PresetStart\n");
			lines+= string("PresetData\n");
			lines+= string(allRecs[cnt].data + string ("\n"));
			lines+= string("PresetName\n");
			lines+= string(allRecs[cnt].name + string ("\n"));
			if (allRecs[cnt].image != string ("") && allRecs[cnt].image.size () > 1)
			{
				lines+= string("PresetImage\n");
				lines+= string(allRecs[cnt].image + string ("\n"));
			}
			lines+= string("PresetEnd\n");
		}
		cnt++;

	}
	file.Write (lines.c_str(),UINT(lines.size()));
	file.Close ();
}
CGalleryManagerDlg::CGalleryManagerDlg() : CDialogEx(CGalleryManagerDlg::IDD)
{
}
CGalleryManager::CGalleryManager(CString settingFileName,CMFCRibbonGallery* RibbonGallery,UINT cmd,CString title,CString def_icon,int defpal)
{
	m_cmdID = cmd;
	m_fileName = theApp.GetAppStorageFolder ();
	//	m_fileName += "\\";
	m_title = title;
	m_gal = RibbonGallery;
	m_fileName += settingFileName;
	m_galleryDefaultIcon = def_icon;
	m_defaultPallete = defpal;
	PopulateFromFile ();

}
void CGalleryManagerDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CDialogEx::OnOK();
}
void CGalleryManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGalleryManagerDlg, CDialogEx)
	//ON_COMMAND(IDC_TIMERTOG, &CAboutDlg::OnTimertog)
	//ON_UPDATE_COMMAND_UI(IDC_SAVE_DOC, &CAboutDlg::OnUpdateSaveDoc)
	ON_BN_CLICKED(IDOK, &CGalleryManagerDlg::OnBnClickedOk)
	ON_EN_CHANGE(IDC_EDIT1, &CGalleryManagerDlg::OnEnChangeEdit1)
	ON_BN_CLICKED(IDC_BUTTON3, &CGalleryManagerDlg::OnBnClickedButton3)
END_MESSAGE_MAP()


void  CGallerySuperManager::GetPresetValue (UINT cmd,std::string &data)
{
	for (size_t i=0;i<m_galleries.size ();i++)
	{
		if (m_galleries[i].m_cmdID == cmd)
		{
			data = m_galleries[i].GetPresetValue (cmd);
			break;
		}
	}
}
void CGallerySuperManager::RegisterGallery (CString title, UINT cmd, CMFCRibbonGallery* f, CString filename,CString def_icon,int def_pal) 
{
	CGalleryManager gm (filename,f,cmd,title,def_icon,def_pal);
	m_galleries.push_back(gm);
	gm.InitGallery (false);
}
bool CGallerySuperManager::AddNewPreset (UINT cmd,std::string data,string image,bool raiseImageDlg)
{
	for (size_t i=0;i<m_galleries.size ();i++)
	{
		if (m_galleries[i].m_cmdID == cmd)
		{
			GalleryRec datas;
			datas.data = data;
			datas.image = image;
			return m_galleries[i].AddPresetDlg (datas,raiseImageDlg);
		}
	}
	return false;
}
bool  CGallerySuperManager::EditPresets (UINT cmd)
{
	for (size_t i=0;i<m_galleries.size ();i++)
	{
		if (m_galleries[i].m_cmdID == cmd)
		{
			return m_galleries[i].EditPresets();
		}
	}
	return false;
}
bool  CGallerySuperManager::RestoreToDefault (UINT cmd)
{
	for (size_t i=0;i<m_galleries.size ();i++)
	{
		if (m_galleries[i].m_cmdID == cmd)
		{
			return (m_galleries[i].RestoreToDefault());
		}
	}
	return false;
}


void CGalleryManagerDlg2::OnBnClickedRemovePreset()
{
	CListBox *e = (CListBox*)GetDlgItem (IDC_LIST1);
	if (e)
	{
		int selid = e->GetCurSel ();
		if (selid < int( m_allRecs.size() ) && selid >= 0)
		{
			vector<GalleryRec>::iterator it = m_allRecs.begin();
			while (selid > 0)
			{it++;selid --;}
			m_allRecs.erase(it);
			
			UpdateOKButton(TRUE);
		}
		InitList();
		if (m_allRecs.size()>0)
				SelectFirstItem();
		else
			{
				// list is clear , disable all buttons..
				// oh windows8 , wont you come, wont you come...

				SetEditText ("");
				CStatic *m = (CStatic*)GetDlgItem(IDC_PICTURE1);
				m->SetBitmap (NULL);
				UpdateRenameButton(FALSE);
				CButton *b = (CButton*)GetDlgItem(IDC_PRESET_IMAGE);
				CButton *b2 = (CButton*)GetDlgItem(IDC_REMOVE_PRESET);
				b->EnableWindow (FALSE);
				b2->EnableWindow (FALSE);
			}
	}
}


void CGalleryManagerDlg2::OnLbnSelchangeList1()
{
	CListBox *e = (CListBox*)GetDlgItem (IDC_LIST1);
	if (e)
	{
		for (int iid = 0;iid < e->GetCount ();iid++)
		{
			if (e->GetSel(iid))
			{
				SetEditText(m_allRecs[iid].name);
				if (m_allRecs[iid].image != "")
				{
					wchar_t selectedImage[MAX_PATH];
					::mbstowcs (selectedImage,m_allRecs[iid].image.c_str(),MAX_PATH);
					SetControlPicture (theApp.GetAppStorageGalleryFolder() + CString (selectedImage));
					UpdateRenameButton(FALSE);
					break;
				}
			}
		}
	}
}

void CGalleryManagerDlg2::SetControlPicture (CString oname)
{
	if (GetFileAttributesW( oname.GetString() ) != 0xFFFFFFFF)
	{
		HBITMAP hbmp = (HBITMAP)::LoadImage(NULL,oname.GetBuffer (),
			IMAGE_BITMAP,96,96,LR_LOADFROMFILE);
		if (hbmp)
		{
			CBitmap c;
			CStatic *m = (CStatic*)GetDlgItem(IDC_PICTURE1);	
			m->SetBitmap (hbmp);

			RedrawWindow ();
		}
	}
}
void CGalleryManagerDlg2::SetEditText (string text)
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	CEdit *e = (CEdit*)GetDlgItem (IDC_EDIT1);

	if (e )
	{
		wchar_t pname[MAX_PATH];
		::mbstowcs (pname,text.c_str(),MAX_PATH);

		// some validation wont hurt..
		e->SetWindowTextW (pname);
	}
}

string
	CGalleryManagerDlg2::GetEditText ()
{
	CEdit *e = (CEdit*)GetDlgItem (IDC_EDIT1);

	string savedName = "";
	if (e != NULL)
	{
		if ( e->GetWindowTextLengthW ()>0)
		{
			wchar_t pname[MAX_PATH];
			char cname [MAX_PATH];
			e->GetWindowTextW (pname,MAX_PATH);
			// some validation wont hurt..

			::wcstombs (cname,pname,MAX_PATH);
			savedName = string (cname);
		}
	}
	return savedName;
}
void CGalleryManagerDlg2::OnBnClickedButton1()
{
	CListBox *e = (CListBox*)GetDlgItem (IDC_LIST1);
	if (e)
	{
		int selid = -1;
		for (int i=0;i<e->GetCount();i++)
			if (e->GetSel(i))
			{
				selid = i;
				break;
			}
			if (selid> -1)
			{
				if (selid < int(m_allRecs.size()) && selid >= 0)
				{
					vector<GalleryRec>::iterator it = m_allRecs.begin();
					while (selid > 0)
					{it++;selid --;}
					it->name = GetEditText();
					UpdateOKButton(TRUE);
					UpdateRenameButton(FALSE);
				}
			}
			InitList();
	}
}

void CGalleryManagerDlg2::SelectFirstItem ()
{
	CListBox *e = (CListBox*)GetDlgItem (IDC_LIST1);
	if (e)
	{
		e->SetCurSel (0);
		OnLbnSelchangeList1();
				/*SetEditText(m_allRecs[0].name);
				if (m_allRecs[0].image != "")
				{
					wchar_t selectedImage[MAX_PATH];
					::mbstowcs (selectedImage,m_allRecs[0].image.c_str(),MAX_PATH);
					SetControlPicture (theApp.GetAppStorageGalleryFolder() + CString (selectedImage));
					
				}*/
	}
}
void CGalleryManagerDlg2::UpdateOKButton(BOOL enable)
{
	CButton *e  = (CButton*)GetDlgItem(IDOK);
	if (e)
	{
		e->EnableWindow(enable);
	}
}
void CGalleryManagerDlg2::UpdateRenameButton(BOOL enable)
{
	CButton *e  = (CButton*)GetDlgItem(IDC_BUTTON1);
	if (e)
	{
		e->EnableWindow(enable);
	}
}

BOOL CGalleryManagerDlg2::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	SetWindowTextW (m_title.GetString ());
	
	SetData (m_allRecs);
	// TODO:  Add extra initialization here
	if (m_allRecs.size()>0)
		SelectFirstItem();
	UpdateOKButton(FALSE);
	UpdateRenameButton (FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CGalleryManagerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  Add extra initialization here
	SetEditText (m_savedName);
	if (m_savedImage.size() > 0)
	{
		wchar_t wimage[MAX_PATH];
		::mbstowcs (wimage,m_savedImage.c_str(),MAX_PATH);

		LoadPicture (wimage);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}


void CGalleryManagerDlg::SetControlPicture (CString oname)
{
	if (GetFileAttributesW( oname.GetString() ) != 0xFFFFFFFF)
	{
		HBITMAP hbmp = (HBITMAP)::LoadImage(NULL,oname.GetBuffer (),
			IMAGE_BITMAP,96,96,LR_LOADFROMFILE);
		if (hbmp)
		{
			CBitmap c;
			CStatic *m = (CStatic*)GetDlgItem(IDC_PICTURE1);	
			m->SetBitmap (hbmp);

			RedrawWindow ();
		}
	}
}
void CGalleryManagerDlg::OnBnClickedButton3()
{
	CString filename = theApp.RaiseImageOpenDialog ();
	if (filename.GetLength() >0)
	{
		LoadPicture(filename);
	}
}

static void DoPictureProc (CString filename, string &outSavedName,CString &outDisplayName)
{
	CString oname;
	CString oname48,oname96;
	CString org_name = filename;
	LPWSTR filext =PathFindExtensionW (filename.GetString());
	if (CString (filext) == CString (".jpg") || CString (filext) == CString (".jpeg") || CString (filext) == CString (".bmp"))
	{
		LPWSTR tfile = filename.GetBuffer();
		PathStripPath(tfile);
		::PathRemoveExtension (tfile);
		char cfile[MAX_PATH];
		::wcstombs (cfile,tfile,MAX_PATH);
		outSavedName = string (cfile) + string ("_48.bmp");

		oname48  =CString(theApp.GetAppStorageGalleryFolder())  +CString (tfile)+ CString (L"_48.bmp");
		oname96  =CString(theApp.GetAppStorageGalleryFolder()) + CString (tfile)+ CString (L"_96.bmp");
		outDisplayName = oname96;
		theApp.ResizePicture (org_name.GetString(),oname48.GetString(),48,48);
		theApp.ResizePicture (org_name.GetString(),oname96.GetString(),96,96);

	} 
	else
	{
		outSavedName = "";
		::AfxMessageBox (L"Image format not valid");
	}

}

void CGalleryManagerDlg::LoadPicture( CString filename)
{
	//LPPICTURE *pict = 0;
	//(*pict)->SaveAsFile ();
	//theApp.LoadPictureFile (filename.GetBuffer (),pict);
	CString displayName;
	DoPictureProc (filename,m_savedImage,displayName);
	SetControlPicture (displayName);

}




void CGalleryManagerDlg2::OnBnClickedPresetImage()
{
	CString filename = theApp.RaiseImageOpenDialog ();
	if (filename.GetLength() >0)
	{
		CListBox *e = (CListBox*)GetDlgItem (IDC_LIST1);
		if (e)
		{
			for (int iid = 0;iid < e->GetCount ();iid++)
			{
				if (e->GetSel(iid))

				{
					CString displayName;
					DoPictureProc (filename,m_allRecs[iid].image,displayName);
					SetControlPicture (displayName);
					UpdateOKButton(TRUE);
				}
			}
		}
	}
}


void CGalleryManagerDlg2::OnEnChangeEdit1()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialogEx::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	 UpdateRenameButton(TRUE);
	// TODO:  Add your control notification handler code here
}

class GalleryRec 
{
	public:

	std::string name;
	std::string data;
	std::string image;
};

class CGalleryManager
{
public:
	CGalleryManager(CString filename,CMFCRibbonGallery* RibbonGallery,UINT cmd,CString title,CString defIcon,int defpal);

	void PopulateFromFile ();
	void SaveToFile ();
	bool AddPresetDlg (GalleryRec rec,bool raiseImageDlg = false);
	void InitGallery (bool redraw = true);
	bool EditPresets ();
	std::string GetPresetValue (UINT cmd);
	CString m_fileName;
	GalleryRec *m_rec;
	std::vector<GalleryRec > allRecs;
	CMFCRibbonGallery *m_gal;
	UINT m_cmdID;
	CString m_title;
	CString m_galleryDefaultIcon;
	bool  RestoreToDefault();
private:
	int m_defaultPallete;
	CString GetDefaultTexturePath ();
	std::string GetDefaultValue (int i);
	
};
class CGallerySuperManager {
public:
	CGallerySuperManager () {}
	~CGallerySuperManager () {}
	void RegisterGallery ( CString title,UINT cmd, CMFCRibbonGallery* f, CString filename,CString def_icon,int defpal) ;
	bool AddNewPreset (UINT cmd,std::string data,std::string image,bool raiseImageDlg = false);
	void GetPresetValue (UINT cmd,std::string &data);
	bool EditPresets (UINT cmd);
	bool RestoreToDefault (UINT cmd);
private:
	std::vector < CGalleryManager > m_galleries;
};
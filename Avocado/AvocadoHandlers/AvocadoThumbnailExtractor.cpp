/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#include "AvocadoThumbnailExtractor.h"
#include "../AvocadoEngine/unzip.h"
#include "../../3rdParty/jpeg-8d/Jpegfile.h"

CAvocadoThumbExtract::
	CAvocadoThumbExtract (CString filename, CString workingDir)
{
	m_filename = filename;
	m_workingDir = workingDir;
	m_width = 0;
	m_height = 0;
	buf = 0;
}
CAvocadoThumbExtract::~CAvocadoThumbExtract()
{
	if (buf)
		delete [] buf;
}

void
	CAvocadoThumbExtract::Extract ()
{
	// Extract jpeg out of the file into working dir.
	HZIP hz;
	CString woutp = m_workingDir;
	woutp += CString (L"\\avothumb.jpg");
	CString woutbmp = m_workingDir;
	woutbmp += CString (L"\\avothumb.bmp");

	m_valid = false;

	hz = OpenZip(m_filename,0);
	ZIPENTRY ze; 
	GetZipItem(hz,-1,&ze); 
	int numitems=ze.index;
	for (int zi=0; zi<numitems; zi++)
	{ 
		GetZipItem(hz,zi,&ze);
		if (CString (ze.name) == CString ("avothumb.jpg"))
		{
			UnzipItem(hz,zi,/*ze.name*/ woutp);
			m_valid = true;
			break;
		}
	}
	CloseZip(hz);
	if (m_valid)
	{
		// Load jpeg into cbitmap object.
		char cfn[200];
		::wcstombs (cfn,woutp,200);

		char cofn[200];
		::wcstombs (cofn,woutbmp,200);

		//buf = new char [m_width * m_height];
		char *dataBuf = JpegFile::JpegFileToRGB (
			std::string (/*"c:\\Temp\\avothumb.jpg"*/cfn),
			&m_width,
			&m_height,
			std::string(cofn/*"c:\\Temp\\avoOutThu.bmp"*/),
			true);
		delete [] dataBuf;

		m_thumbnailBitmapFile = woutbmp;
	}
	//new_height = (unsigned int) (float(m_height) * float(new_width)/float(m_width));
	//m_bitmap.CreateBitmap (new_width,new_height,1,24,(void *)(buf));
	//buf = cbuf;

	//	BYTE buffer[16*16*3];
	//	::memset(buffer,192,sizeof(buffer));
	//	HBITMAP hBitmap=CreateBitmap(16,16,1,24,buffer);//with all the parameters filled.
	//	m_bitmap.Attach(hBitmap);

	//jp.JpegFileToRGB (std::string ("c:\\Temp\\avothumb.jpg"/*cfn*/), &m_width,&m_height);
	//m_bitmap.m_hObject=(HBITMAP)::CreateBitmap (m_width,m_height,1,24,(buf));
	//m_bitmap.m_hObject=(HBITMAP)::LoadImage(NULL,L"c:\\Temp\\av.bmp",IMAGE_BITMAP,newwid,m_height,LR_LOADFROMFILE);

	// Delete jpeg from disk.
	//m_bitmap.CreateCompatibleBitmap ();
	//m_bitmap.CreateBitmap (m_width,m_height,1,24,(void *)(buf));
	//	m_bitmap.m_hObject=(HBITMAP)::CreateBitmap (m_width,m_height,1,24,(void *)(buf));

}

void
	CAvocadoThumbExtract::Convert (CString ifilename,CString ofilename)
{
	// Extract jpeg out of the file into working dir.
	CString woutp = ifilename;
	CString woutbmp =ofilename;

	//if (m_valid)
	{
		// Load jpeg into cbitmap object.
		char cfn[200];
		::wcstombs (cfn,woutp,200);

		char cofn[200];
		::wcstombs (cofn,woutbmp,200);

		//buf = new char [m_width * m_height];
		char *dataBuf = JpegFile::JpegFileToRGB (
			std::string (/*"c:\\Temp\\avothumb.jpg"*/cfn),
			&m_width,
			&m_height,
			std::string(cofn/*"c:\\Temp\\avoOutThu.bmp"*/),
			true);
		delete [] dataBuf;

		m_thumbnailBitmapFile = woutbmp;
	}
	//new_height = (unsigned int) (float(m_height) * float(new_width)/float(m_width));
	//m_bitmap.CreateBitmap (new_width,new_height,1,24,(void *)(buf));
	//buf = cbuf;

	//	BYTE buffer[16*16*3];
	//	::memset(buffer,192,sizeof(buffer));
	//	HBITMAP hBitmap=CreateBitmap(16,16,1,24,buffer);//with all the parameters filled.
	//	m_bitmap.Attach(hBitmap);

	//jp.JpegFileToRGB (std::string ("c:\\Temp\\avothumb.jpg"/*cfn*/), &m_width,&m_height);
	//m_bitmap.m_hObject=(HBITMAP)::CreateBitmap (m_width,m_height,1,24,(buf));
	//m_bitmap.m_hObject=(HBITMAP)::LoadImage(NULL,L"c:\\Temp\\av.bmp",IMAGE_BITMAP,newwid,m_height,LR_LOADFROMFILE);

	// Delete jpeg from disk.
	//m_bitmap.CreateCompatibleBitmap ();
	//m_bitmap.CreateBitmap (m_width,m_height,1,24,(void *)(buf));
	//	m_bitmap.m_hObject=(HBITMAP)::CreateBitmap (m_width,m_height,1,24,(void *)(buf));

}

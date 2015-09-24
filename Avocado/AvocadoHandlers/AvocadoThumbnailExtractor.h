/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#ifndef _AVOCADO_ACTIVEX
#include "stdafx.h"
#else
#include <stdafx.h>
#endif
#include <string>

class CAvocadoThumbExtract
{
public:
	CAvocadoThumbExtract (CString filename, CString workingDir);
	~CAvocadoThumbExtract();
	void Extract ();
	void Convert (CString ifilename,CString ofilename);
	//CBitmap *GetBitmap ();
	CBitmap	m_bitmap;
	CString m_thumbnailBitmapFile;
	bool m_valid;
protected:
	char *buf;
	unsigned int m_height;
	unsigned int m_width;
	CString m_filename;
	CString m_workingDir;

};
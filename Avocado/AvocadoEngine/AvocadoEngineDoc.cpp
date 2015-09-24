/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#include "AvocadoEngineDoc.h"
#include "AvocadoScenixAdapter.h"
#include <nvgl/SceneRendererGL2.h>
#include <nvgl/RenderTargetGLFBO.h>
// need to avoid nvsg includes here , move this to paging module
#include <nvsg/FrustumCamera.h>
#include <nvsg/LightSource.h>
#include <nvsg/DirectedLight.h>
#include <nvtraverser\SearchTraverser.h>
#include <nvutil/SmartPtr.h>
#include <nvmath/Trafo.h>
#include <nvsg/Transform.h>
#include <nvsg/AnimatedTransform.h>
#include "AvocadoInternalModules.h"
#include <nvsg/KeyFramedAnimation.h>
#include <nvsg/LinearInterpolatedAnimation.h>
#include <nvsg/Animation.h>
#include <nvsg/AnimationDescription.h>
#include "AvocadoMaterials.h"
#include "SceneFunctions.h"
#include "unzip.h"
#include "zip.h"

#include <fstream>
namespace avocado
{
	// AvocadoEngineDoc

	void ClearAvocadoShaders (); // forward declare instead of including meshgenerator.. 
	// should be moved to itts own module..

	AvocadoEngineDoc::AvocadoEngineDoc ()
	{
		NVSG_TRACE();
		m_nvsgDocData = new CNVSGDocData ();
		AvocadoViewStateData defaultVS;
		defaultVS.viewID = 0;
		defaultVS.cameraMatrix = nvmath::Mat44f(true);
		m_viewStates.push_back (defaultVS);	
	}
	void AvocadoEngineDoc::ResetDocIntr ()
	{
		m_docInterface = 0;
	}
	AvocadoEngineDoc::~AvocadoEngineDoc ()
	{
		// BADSTUFF - WE MUST CLEAR all the references that we
		// keep to textures / shaders / materials . 
		// In the future we will introduce a seperate material module that will do this.
		//avocado::ClearAvocadoShaders();
		if (m_docInterface)
			(GetDocInterface())->m_isAvailable = false;
		ClearDocElements ();
		ClearDocModules (); 
		delete m_nvsgDocData; 
	}

	CNVSGDocData *AvocadoEngineDoc::GetCNVSGDocData ()
	{
		return m_nvsgDocData;
	}

	bool AvocadoEngineDoc::HandleAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta, bool &needRepaint)
	{

		std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
		while (it != m_viewList.end ())
		{
			AvocadoEngineView *view = *it;
			view->HandleAvocadoMouseStringMessage(msg,view->GetID(),x,y,zDelta,needRepaint);
			it++;
		}
		return true;
	}

	bool AvocadoEngineDoc::HandleAvocadoViewGeneralStringMessage (const std::string &msg, int viewId,const std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());
		std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
		while (it != m_viewList.end ())
		{
			AvocadoEngineView *view = *it;
			view->HandleAvocadoViewGeneralStringMessage(msg,view->GetID(),paramStr,needRepaint);
			it++;
		}
		return true;
	}
	bool AvocadoEngineDoc::HandleSpecificModuleMessage (std::string msg, int docId, std::string paramStr, bool &needRepaint,string targetModule)
	{
		bool ret = false;
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());
		for (size_t i=0;i<m_modules.size();i++)
		{
			// TODO : Send the message to all modules , no break. each module will decide wheather or not to implement its virtual handlers.
			/* later on , set priorites for each module */
			if (m_modules[i]->m_name == targetModule)
			{
				if (m_modules[i]->HandleAvocadoDocGeneralStringMessage(msg,docId,paramStr,needRepaint))
					return true; // Message was handled succsusfully 
			}
		}
		return ret;
	}

	bool AvocadoEngineDoc::InsertFile (std::string filename,AvocadoFileLinkInterface::FileType type,bool embed)
	{
		NVSG_TRACE();
		AvocadoFileLinkInterface fli;

		fli.m_fileName = filename;
		fli.m_type = type;
		fli.m_embed= embed;

		m_files.push_back (fli);
		return true;
	}
	bool AvocadoEngineDoc::RemoveFile (std::string filename)
	{
		return true;
	}
	void AvocadoEngineDoc::SetSessionFolder (std::string folder)
	{
		m_sessionFolder = folder;

		if (GetCNVSGDocData ())
		{
			GetCNVSGDocData ()->SetSessionFolder (folder);
		}
	}

	bool AvocadoEngineDoc::CompressFile (std::string &inpath,std::string &outpath,
		std::vector<std::string> attachments,
		std::vector<std::string> embeddedModels,
		std::vector<std::string> embeddedTextures, 
		bool unzip)
	{
		// jumping back and forth from unicode really sucks.. needs to be fixed asap.
		NVSG_TRACE();
		std::string tpath (inpath);
		
		tpath = tpath.substr (0,tpath.rfind("\\")) + "\\avothumb.jpg";
#ifdef UNICODE
		wchar_t winp [MAX_PATH];
		wchar_t woutp [MAX_PATH];
		wchar_t wtpath [MAX_PATH];
		size_t bc = 0;
		::mbstowcs_s<MAX_PATH> (&bc,winp,inpath.c_str (),MAX_PATH);
		::mbstowcs_s<MAX_PATH> (&bc,woutp,outpath.c_str (),MAX_PATH);
		::mbstowcs_s<MAX_PATH> (&bc,wtpath,tpath.c_str (),MAX_PATH);
#else
		TCHAR winp [MAX_PATH];
		TCHAR woutp [MAX_PATH];
		TCHAR wtpath [MAX_PATH];

		::strcpy (winp,inpath.c_str ());
		::strcpy (woutp,outpath.c_str ());
		::strcpy (wtpath,tpath.c_str ());
#endif
		HZIP hz;
		if (!unzip)
		{
			hz = CreateZip(woutp,0);
#ifdef UNICODE
			ZipAdd(hz,L"Main.avc", winp);
			ZipAdd(hz,L"avothumb.jpg", wtpath);
#else
			ZipAdd(hz,"Main.avc", winp);
			ZipAdd(hz,"avothumb.jpg", wtpath);
#endif
			for (size_t ati =  0; ati < attachments.size(); ati++)
			{
#ifdef UNICODE
				wchar_t att_path [MAX_PATH];
				bc = 0;
				::mbstowcs_s<MAX_PATH> (&bc,att_path,attachments[ati].c_str (),MAX_PATH);

				string nameStr = attachments[ati].c_str ();
				nameStr = nameStr.substr (nameStr.rfind ("\\")+1,nameStr.size() - nameStr.rfind ("\\") - 1);

				wchar_t att_name [MAX_PATH];
				::mbstowcs_s<MAX_PATH> (&bc,att_name,nameStr.c_str(),MAX_PATH);
				ZipAdd(hz,att_name, att_path);
#else
				TCHAR att_path [MAX_PATH];
				::strcpy (att_path,attachments[ati].c_str ());

				string nameStr = attachments[ati].c_str ();
				nameStr = nameStr.substr (nameStr.rfind ("\\")+1,nameStr.size() - nameStr.rfind ("\\") - 1);
				TCHAR att_name [MAX_PATH];

				::strcpy (att_name,nameStr.c_str());
				ZipAdd(hz,att_name, att_path);
#endif

			}
			// embed models
			for (size_t ati =  0; ati < embeddedModels.size(); ati++)
			{
#ifdef UNICODE
			     bc = 0;
				wchar_t att_path [MAX_PATH];
				::mbstowcs_s<MAX_PATH> (&bc,att_path,embeddedModels[ati].c_str (),MAX_PATH);

				string nameStr = embeddedModels[ati].c_str ();
				nameStr = nameStr.substr (nameStr.rfind ("\\")+1,nameStr.size() - nameStr.rfind ("\\") - 1);
				nameStr = "models\\" + nameStr;
				wchar_t att_name [MAX_PATH];
				::mbstowcs_s<MAX_PATH> (&bc,att_name,nameStr.c_str(),MAX_PATH);
				ZipAdd(hz,att_name, att_path);
#else
				TCHAR att_path [MAX_PATH];
				::strcpy (att_path,embeddedModels[ati].c_str ());

				string nameStr = embeddedModels[ati].c_str ();
				nameStr = nameStr.substr (nameStr.rfind ("\\")+1,nameStr.size() - nameStr.rfind ("\\") - 1);
				nameStr = "models\\" + nameStr;
				TCHAR att_name [MAX_PATH];

				::strcpy (att_name,nameStr.c_str());
				ZipAdd(hz,att_name, att_path);
#endif

			}
			
			// embed textures
			for (size_t ati =  0; ati < embeddedTextures.size(); ati++)
			{
#ifdef UNICODE
				wchar_t att_path [MAX_PATH];
			     bc = 0;
				::mbstowcs_s<MAX_PATH> (&bc,att_path,embeddedTextures[ati].c_str (),MAX_PATH);

				string nameStr = embeddedTextures[ati].c_str ();
				nameStr = nameStr.substr (nameStr.rfind ("\\")+1,nameStr.size() - nameStr.rfind ("\\") - 1);
				nameStr = "textures\\" + nameStr;
				wchar_t att_name [MAX_PATH];
				::mbstowcs_s<MAX_PATH> (&bc,att_name,nameStr.c_str(),MAX_PATH);
				ZipAdd(hz,att_name, att_path);
#else
				TCHAR att_path [MAX_PATH];
				::strcpy (att_path,embeddedTextures[ati].c_str ());

				string nameStr = embeddedTextures[ati].c_str ();
				nameStr = nameStr.substr (nameStr.rfind ("\\")+1,nameStr.size() - nameStr.rfind ("\\") - 1);
				nameStr = "textures\\" + nameStr;
				TCHAR att_name [MAX_PATH];

				::strcpy (att_name,nameStr.c_str());
				ZipAdd(hz,att_name, att_path);
#endif

			}

			CloseZip(hz);
		} 
		else
		{
			hz = OpenZip(winp,0);
			ZIPENTRY ze; 
			GetZipItem(hz,-1,&ze); 
			int numitems=ze.index;
			for (int zi=0; zi<numitems; zi++)
			{ 
				GetZipItem(hz,zi,&ze);
				char ppbuf [MAX_PATH];
#ifdef UNICODE
				bc = 0;
				::wcstombs_s<MAX_PATH> (&bc,ppbuf,ze.name,MAX_PATH);
#else
				::strcpy (ppbuf,ze.name);
#endif
				if (string(ppbuf) == std::string ("Main.avc"))
					UnzipItem(hz,zi,/*ze.name*/ woutp);
				else if (string(ppbuf).find(std::string ("textures/")) != string::npos)
				{
#ifdef UNICODE
					string stripPath = outpath.substr (0, outpath.find_last_of ('\\')+1); 
#else
					string stripPath = string (woutp).substr (0,string (woutp).find_last_of ('\\')+1); 
#endif
					// should we create a new folder called textures ?
					stripPath +=  string(ppbuf);
#ifdef UNICODE
					wchar_t bitmapPath [MAX_PATH];
					bc = 0;
					::mbstowcs_s<MAX_PATH> (&bc,bitmapPath,stripPath.c_str(),MAX_PATH);
					UnzipItem(hz,zi,/*ze.name*/bitmapPath );
#else
					UnzipItem(hz,zi,/*ze.name*/ stripPath.c_str ());
#endif
				}
				else if (string(ppbuf).find(std::string ("models/")) != string::npos)
				{
#ifdef UNICODE
					string stripPath = outpath.substr (0, outpath.find_last_of ('\\')+1); 
#else
					string stripPath = string (woutp).substr (0,string (woutp).find_last_of ('\\')+1); 
#endif
					stripPath +=  string(ppbuf);
#ifdef UNICODE
					wchar_t bitmapPath [MAX_PATH];
					bc = 0;
					::mbstowcs_s<MAX_PATH> (&bc,bitmapPath,stripPath.c_str(),MAX_PATH);
					UnzipItem(hz,zi,/*ze.name*/bitmapPath );
#else
					UnzipItem(hz,zi,/*ze.name*/ stripPath.c_str ());
#endif
				}
				else if (string(ppbuf).find(std::string (".bmp")) != string::npos)
				{
#ifdef UNICODE
					string stripPath = outpath.substr (0, outpath.find_last_of ('\\')+1); 
#else
					string stripPath = string (woutp).substr (0,string (woutp).find_last_of ('\\')+1); 
#endif
					stripPath += string(ppbuf);
#ifdef UNICODE
					wchar_t bitmapPath [MAX_PATH];
					bc = 0;
					::mbstowcs_s<MAX_PATH> (&bc,bitmapPath,stripPath.c_str(),MAX_PATH);
					UnzipItem(hz,zi,/*ze.name*/bitmapPath );
#else
					UnzipItem(hz,zi,/*ze.name*/ stripPath.c_str ());
#endif
				}
			}
			CloseZip(hz);
		}
		return true;
	}
	 static std::string & replaceSubString(std::string & subj, std::string old, std::string neu)
    {
		while (true)
		{
			size_t uiui = subj.find(old);
			if (uiui != std::string::npos)
			{
			   subj.erase(uiui, old.size());
			   subj.insert(uiui, neu);
			} 
			else
			{
				break;
			}
		}
        return subj;
    }
	bool AvocadoEngineDoc::SerializeDocument (std::string &str,bool isStoring)
	{
		NVSG_TRACE();
		if (isStoring)
		{
			GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::SAVE_DOCUMENT_STARTED,"",0);
			NVSG_TRACE_OUT(string ("AvocadoEngine Storing "+ string(" - ") +str+ string("\n")).c_str());
		}
		else
		{
			
			GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::LOAD_DOCUMENT_STARTED,"",0);
			NVSG_TRACE_OUT(string ("AvocadoEngine Storing "+ string(" - ") +str+ string("\n")).c_str());
		}
		if (isStoring)
		{
			string DocString ("<AvocadoDocV1>\n");
			ParamListSharedPtr dppl = ParamList::createNew ();

			dppl->PushInt ("AvocadoDocParamsCount",int(m_nvsgDocData->m_docParams->GetParamCount()));
			for (size_t ipara = 0; ipara < m_nvsgDocData->m_docParams->GetParamCount();ipara++)
			{
				StringParam *stparam = (StringParam*)m_nvsgDocData->m_docParams->GetParam(ipara);
				string parname,parval;
				stparam->GetName  (parname);
				stparam->GetValue  (parval);
				char cc[30];
				itoa (int (ipara),cc,10);
				dppl->PushString (string ("AvocadoDocParamName") + string (cc),parname);
				dppl->PushString (string ("AvocadoDocParamValue") + string (cc),parval);
			}
			// write material states
			if (m_materialStates.size ())
			{
				dppl->PushInt (string ("MaterialStateCount"),int(m_materialStates.size ()));
				for (size_t matStateInd = 0; matStateInd < m_materialStates.size ();matStateInd++)
				{
					char ddms[30];
					itoa(int(matStateInd),ddms,10);
					dppl->PushString (string ("MaterialStateName")+string (ddms),m_materialStates[matStateInd].m_name);
					dppl->PushInt (string ("MaterialStateMatCount")+string (ddms),int(m_materialStates[matStateInd].m_ss.size()));
					for (size_t matStateElem = 0;matStateElem<m_materialStates[matStateInd].m_ss.size();matStateElem++)
					{
						char ccms[30];
						itoa(int(matStateElem),ccms,10);
						dppl->PushInt (string ("MaterialStateMatElemID")+string (ddms)+string("-")+string(ccms),m_materialStates[matStateInd].m_ss[matStateElem].first);
						string msdatapar =m_materialStates[matStateInd].m_ss[matStateElem].second;
						for (size_t mkss = 0;mkss<msdatapar.size();mkss++)
						{
							if (msdatapar[mkss] == '=')
								msdatapar[mkss] = '?';
							if (msdatapar[mkss] == ' ')
								msdatapar[mkss] = '+';
							if (msdatapar[mkss] == ',')
								msdatapar[mkss] = '&';
						}
						dppl->PushString (string ("MaterialStateMatElemData")+string (ddms)+string("-")+string(ccms),msdatapar);
					}
				}

			}
			// end write material states

			//nvmath::Mat44f mat = TransformReadLock (m_elementRoot)->getMatrix ();
			//const float * matPtr = 
			float *matPtr = new float [16];
			m_viewList[0]->GetCNVSGViewData ()->GetCameraLocation(&matPtr);//mat.getPtr();

			for (int i=0;i<16;i++)
			{
				char cc[30];
				itoa(i,cc,10);

				dppl->PushFloat ("ViewLocation"+string(cc),matPtr[i]);		
			}

			if (m_viewStates.size() == 1)
			{
				nvmath::Mat44f cmm (matPtr[0],matPtr[1],matPtr[2],matPtr[3],
					matPtr[4],matPtr[5],matPtr[6],matPtr[7],
					matPtr[8],matPtr[9],matPtr[10],matPtr[11],
					matPtr[12],matPtr[13],matPtr[14],matPtr[15]);
				m_viewStates[0].cameraMatrix = cmm;
			}

			delete [] matPtr;

			int currentViewState = -1;

			for (size_t vsi = 0; vsi < m_viewStates.size ();vsi++)
			{
				if (m_viewStates[vsi].viewID == 0)
					currentViewState = int(vsi);
				char dd[30];
				itoa(int(vsi),dd,10);
/*				for (int i=0;i<16;i++)
				{
					char cc[30];
					itoa(i,cc,10);
					dppl->PushFloat ("ViewStateLocation"+string(cc) + string ("-")+string (dd),(m_viewStates[vsi].cameraMatrix.getPtr ())[i]);		
				}
	*/			
				dppl->PushFloat16 ("ViewStateLocation"+ string ("-")+string (dd),m_viewStates[vsi].cameraMatrix.getPtr ());

				if (m_viewStates[vsi].html_text!=string (""))
				{
					//break html into lines..
					stringstream htmlstream (m_viewStates[vsi].html_text);
					char buf[4096];
					int line= 0;
					while (!htmlstream.eof ()){

						htmlstream.getline (buf,4096);
						if (buf[0] == 0)
							break;
						string htmlLine (buf);
						if (htmlLine[htmlLine.size()-1] == '\n' || htmlLine[htmlLine.size()-1] == char(13))
							htmlLine = htmlLine.substr (0,htmlLine.size()-1);

						replaceSubString(htmlLine,"href=\"#\"","href=\"javascript:void(0)\"");

						stringstream linoo;
						linoo << line;
						dppl->PushString ("ViewStateHtml"+string(dd)+string("-")+linoo.str(),htmlLine);
						line++;
					}
					dppl->PushInt ("ViewStateHtmlLineCount"+string(dd),line);
				}
				if (m_viewStates[vsi].bg_image_file!=string(""))
					dppl->PushString ("ViewStateImage"+string(dd),m_viewStates[vsi].bg_image_file);
				dppl->PushInt ("ViewStateElementCount"+string (dd),int(m_viewStates[vsi].elementLocation.size()));
				for (size_t ks = 0;ks < m_viewStates[vsi].elementLocation.size();ks++)
				{
					char ianid[30];
					itoa(int(ks),ianid,10);
					dppl->PushInt ("ViewStateElementID"+string(dd) + string ("-")+string (ianid), m_viewStates[vsi].elementLocation[ks].first);
				}
				for (size_t ks = 0;ks < m_viewStates[vsi].elementLocation.size();ks++)
				{
					
					char ianid[30];
					itoa(int(ks),ianid,10);
					dppl->PushFloat16 ("ViewStateElementLocation" + string ("-")+string (dd)+string ("-") + string (ianid),
						 m_viewStates[vsi].elementLocation[ks].second.getMatrix().getPtr ());
				}
				if ( m_viewStates[vsi].elementVisibility.size() ==  m_viewStates[vsi].elementLocation.size())
				{
					for (size_t ks = 0;ks < m_viewStates[vsi].elementVisibility.size();ks++)
					{
						char ianid[30];
						itoa(int(ks),ianid,10);
						dppl->PushBool ("ViewStateElementVisibility" + string ("-")+string (dd)+string ("-") + string (ianid),
							 m_viewStates[vsi].elementVisibility[ks].second);
					}
				}
			}
			dppl->PushInt ("CurrentViewState",currentViewState);
			dppl->PushInt ("ViewStateCount",int(m_viewStates.size ()));

			dppl->PushInt ("LastIDCount",this->GetCNVSGDocData()->getIDGenerator()->m_nextID);

			DocString += dppl->SerializeList () + "\n";
			for (size_t i=0;i < m_docElems.size ();i++)
			{
				GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::SAVE_ELEMENT_STARTED,m_docElems[i]->GetName(),int (100.0f*float(i)/float(m_docElems.size ())));
				DocString += "<Element>\n";
				DocString += m_docElems[i]->serialize () + string ("\n");
				DocString += "</Element>\n";
				GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::SAVE_ELEMENT_COMPLETE,m_docElems[i]->GetName(),int (100.0f*float(i)/float(m_docElems.size ())));
			}

			DocString += "</AvocadoDocV1>\n";
			str = DocString.c_str();
			GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::SAVE_DOCUMENT_COMPLETE,"",0);
		}
		else
		{
			//string DocParamString = string ("");
			string DocString  = str;
			int totalElementsCount = 1; // init to 1 to avoid division by zero.
			istringstream ty (DocString);
			string DocLine;
			float mat[16];
			bool hasLocation = false;
			ParamListSharedPtr newParamList;
			//	if (FileStream.is_open())
			{
				bool elStarted = false;
				bool docValid = false;
				string accum ("");
				while ( ty.good() )
				{
					getline (ty,DocLine);
					if (DocLine == "<AvocadoDocV1>")
					{
						string DocPropLine;
						getline (ty,DocPropLine);
						ParamListSharedPtr dppl = ParamList::createFromString (DocPropLine);

						int lastKey;
						IntParam *lid = (IntParam*)(dppl->GetParam ("LastIDCount"));
						lid->GetValue(lastKey);

						
						/* Get doc pararms */
						if (dppl->GetParam("AvocadoDocParamsCount"))
						{
						    newParamList = ParamList::createNew ();
							IntParam *docparCount = (IntParam*)dppl->GetParam("AvocadoDocParamsCount");
							int idoccount;
							docparCount->GetValue (idoccount);

							for (int parK = 0;parK < idoccount;parK++)
							{
								char cc[30];
								itoa (parK,cc,10);
								if (dppl->GetParam("AvocadoDocParamName"+ string (cc)))
								{

									StringParam *docparName = (StringParam*)dppl->GetParam("AvocadoDocParamName"+string (cc));
									StringParam *docparValue = (StringParam*)dppl->GetParam("AvocadoDocParamValue"+string (cc));
									string parname,parval;
									docparName->GetValue (parname);
									docparValue->GetValue (parval);

									newParamList->PushString (parname,parval);
								}
							}
						//	m_nvsgDocData->m_docParams = newParamList;//ParamList::createFromString (parstr);
							//DocParamString = ;
						}
						/* end get do params*/

						for (int j=0;j<16;j++)
						{
							char cc[30];
							itoa(j,cc,10);
							mat[j] = 1.0f;
							if (dppl->GetParam ("ViewLocation" + string (cc)))
							{
								FloatParam *irm_vn = (FloatParam*)dppl->GetParam ("ViewLocation" + string (cc));
								if (irm_vn)
								{
									irm_vn->GetValue (mat[j]);
									hasLocation = true;
								}
							}	
						}
						// hack - set the view location here...
						if (hasLocation)
						{
							for (size_t viewDD = 0;viewDD<this->m_viewList.size();viewDD++)
							{
								(*GetViewById (int(viewDD)))->GetCNVSGViewData ()->SetCameraLocation (mat);
							}
						}
						
						// read material states;
						m_materialStates.clear();
						if (dppl->GetParam ("MaterialStateCount"))
						{
							IntParam *imaterialStateCount = (IntParam*)(dppl->GetParam ("MaterialStateCount"));
							int ms_count;
							imaterialStateCount->GetValue(ms_count);
							for (int imscx = 0; imscx <ms_count;imscx++)
							{
								
							    char ddms[30];
								itoa(imscx,ddms,10);
								string ms_name = "";
								int ms_value = 0;
								if (dppl->GetParam (string ("MaterialStateName") + string (ddms)))
								{
									StringParam *smaterialStateName = (StringParam*)dppl->GetParam (string ("MaterialStateName")+ string (ddms));
									smaterialStateName->GetValue (ms_name);
								}
								if (dppl->GetParam (string ("MaterialStateMatCount") + string (ddms)))
								{
									IntParam *smaterialStateMatCount = (IntParam*)dppl->GetParam (string ("MaterialStateMatCount")+ string (ddms));
									smaterialStateMatCount->GetValue (ms_value);
								}
								AvocadoMaterialStateData msd;
								msd.m_name = ms_name;

								for (int msmc = 0;msmc < ms_value ; msmc++)
								{
									int ms_elemid = 0;
									string ms_elemmatdata = "";
									char ccms[30];
									itoa(msmc,ccms,10);
									if (dppl->GetParam (string ("MaterialStateMatElemID") + string (ddms)+ string("-") + string (ccms)))
									{
										IntParam *smaterialStateMatElemID = (IntParam*)dppl->GetParam (string ("MaterialStateMatElemID")+ string (ddms) + string("-") + string (ccms));
										smaterialStateMatElemID->GetValue (ms_elemid);
									}	
									if (dppl->GetParam (string ("MaterialStateMatElemData") + string (ddms)+ string("-") + string (ccms)))
									{
										StringParam *smaterialStateMatElemID = (StringParam*)dppl->GetParam (string ("MaterialStateMatElemData")+ string (ddms) + string("-") + string (ccms));
										smaterialStateMatElemID->GetValue (ms_elemmatdata);
										for (size_t mkss = 0;mkss<ms_elemmatdata.size();mkss++)
										{
											if (ms_elemmatdata[mkss] == '?')
												ms_elemmatdata[mkss] = '=';
											if (ms_elemmatdata[mkss] == '+')
												ms_elemmatdata[mkss] = ' ';
											if (ms_elemmatdata[mkss] == '&')
												ms_elemmatdata[mkss] = ',';
										}

									}	
									msd.m_ss.push_back(std::pair<int,string>(ms_elemid,ms_elemmatdata));
								
								}
								m_materialStates.push_back(msd);
							}
						}
						// end read material states

						// read view states..
						m_viewStates.clear ();
						int current_view_state = -1;
						if (dppl->GetParam ("ViewStateCount"))
						{
							float matVS[16];
							IntParam *iviewStateCount = (IntParam*)(dppl->GetParam ("ViewStateCount"));
							int vs_count;
							iviewStateCount->GetValue(vs_count);
							if (dppl->GetParam ("CurrentViewState"))
							{
								IntParam *icurrent_viewState = (IntParam*)(dppl->GetParam ("CurrentViewState"));
								icurrent_viewState->GetValue(current_view_state);
							}
							for (int vsK = 0; vsK < vs_count; vsK++)
							{
								AvocadoViewStateData new_vs;
								//new_vs.cameraMatrix = mat;
								new_vs.viewID = -1;

								char dd[30];
								itoa(vsK,dd,10);
								bool useIDMat = false;
								if (!dppl->GetFloat16ValueByName ("ViewStateLocation" +  string ("-")+ string (dd),matVS))
								{
									//
									useIDMat = true;
									//for (int kiiid = 0; kiiid < 16; kiiid ++)
										//matVS[kiiid] = iddddmat.getPtr ()[kiiid];
								} 


								nvmath::Mat44f mmat (true);
								if (!useIDMat)
								{
									for (int xj=0;xj<4;xj++)
										for (int xi=0;xi<4;xi++)
											mmat[xj][xi] = matVS[xi+4*xj];
								}
								new_vs.cameraMatrix = mmat; 
								
								if (dppl->GetParam (string("ViewStateHtmlLineCount")+string(dd)))
								{
									IntParam *ivs_html = (IntParam*)dppl->GetParam (string("ViewStateHtmlLineCount")+string(dd));
									if (ivs_html)
									{
										int lineCount = 0;
										ivs_html->GetValue (lineCount);
										for (size_t lipos=0;lipos< (size_t)lineCount;lipos++)
										{
											char cc[30];
											itoa(int(lipos),cc,10);
											if (dppl->GetParam (string("ViewStateHtml")+string(dd)+string("-")+string(cc)))
											{
												StringParam *ivs_html_line = (StringParam*)dppl->GetParam (string("ViewStateHtml")+string(dd) + string ("-") + string (cc));
												if (ivs_html_line)
												{
													string linestring ;
													ivs_html_line->GetValue(linestring);
													new_vs.html_text += linestring ;//+ string("\n");
												}
											}

										}
									}

								}
								if (dppl->GetParam (string("ViewStateImage")+string(dd)))
								{
									StringParam *ivs_html = (StringParam*)dppl->GetParam (string("ViewStateImage")+string(dd));
									if (ivs_html)
									{
										ivs_html->GetValue (new_vs.bg_image_file);
									}

								}
								if (dppl->GetParam (string("ViewStateElementCount")+string(dd)))
								{
									IntParam *ivs_elemcnt = (IntParam*)dppl->GetParam (string("ViewStateElementCount")+string(dd));
									int elemtCount=0;
									if (ivs_elemcnt)
									{
										ivs_elemcnt->GetValue (elemtCount);
										if (totalElementsCount < elemtCount)
											totalElementsCount = elemtCount;
									}
									for (int ks = 0;ks < elemtCount ;ks ++)
									{
										bool apply_matrix = true;
										int vselemId = 0;
										float matVSElem[16];
											char cc[30];
											itoa(int(ks),cc,10);
											//if (dppl->GetParam (string("ViewStateElementID")+string(dd)+string("-")+string(cc)))
											{
												bool res = dppl->GetIntValueByName (string("ViewStateElementID")+string(dd)+string("-")+string(cc),vselemId);
												if (!res)
													apply_matrix = false;
												//IntParam *vselemIdprm = (IntParam*)dppl->GetParam (string("ViewStateElementID")+string(dd)+string("-")+string(cc));
												//vselemIdprm->GetValue (vselemId);

											}
											if (!dppl->GetFloat16ValueByName ("ViewStateElementLocation" + string ("-")+ string (dd) + string ("-")+ string (cc),matVSElem))
											{
												apply_matrix = false;
											}
											if (apply_matrix)
											{
												nvmath::Mat44f matElementLoca
													(matVSElem[0],matVSElem[1],matVSElem[2],matVSElem[3],matVSElem[4],matVSElem[5],matVSElem[6],matVSElem[7],matVSElem[8],
													matVSElem[9],matVSElem[10],matVSElem[11],matVSElem[12],matVSElem[13],matVSElem[14],matVSElem[15]);
												nvmath::Trafo matElementLocaTraf;
												matElementLocaTraf.setMatrix (matElementLoca);
												matElementLocaTraf.setCenter (nvmath::Vec3f (0.0f,0.0f,0.0f));//
												new_vs.elementLocation.push_back (pair<int,nvmath::Trafo>(vselemId,matElementLocaTraf));
											}
											if (apply_matrix)
											{
												bool vselemVis = true;
												if (dppl->GetBoolValueByName ("ViewStateElementVisibility" + string ("-")+ string (dd) + string ("-")+ string (cc),vselemVis))
												{
														new_vs.elementVisibility.push_back (pair<int,bool>(vselemId,vselemVis));
												}
											}
									}
								}
								m_viewStates.push_back(new_vs);
							}
							//if (current_view_state >= 0)
							m_viewStates[0].viewID=0;//current_view_state].viewID = 0;
						}
						//end read view states;
						GetCNVSGDocData()->getIDGenerator()->m_nextID = (unsigned int)lastKey;
						ClearDocElements ();
						docValid = true;
					}
					else if (DocLine == "</AvocadoDocV1>")
					{
						GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::LOAD_DOCUMENT_COMPLETE,"",int (100.0f));
						if (hasLocation)
							m_viewList[0]->GetCNVSGViewData()->SetCameraLocation (mat);

						m_nvsgDocData->m_docParams->Clear ();
//#if 0
						for (size_t pari = 0;pari < newParamList->GetParamCount(); /*m_nvsgDocData->m_docParams->GetParamCount()*/pari++)
						{
							//Param *p = m_nvsgDocData->m_docParams->GetParam (pari);
							Param *p =  newParamList->GetParam (pari);
							string paramName;
							string paramVal;
							p->GetName(paramName);
							p->GetValue (paramVal);
							if (paramName == "backimage" )
							{
								ParamListSharedPtr newParam = ParamList::createNew ();
								newParam->PushString (paramName,paramVal);
								NotifyDocParamChanged (newParam->SerializeList());
								bool needRepaint = false;
								HandleAvocadoDocGeneralStringMessage ("SetDocParam",m_id,newParam->SerializeList (),needRepaint);
							}
						}
//#endif
						for (size_t pari = 0;pari < newParamList->GetParamCount(); /*m_nvsgDocData->m_docParams->GetParamCount()*/pari++)
						{
							//Param *p = m_nvsgDocData->m_docParams->GetParam (pari);
							Param *p =  newParamList->GetParam (pari);
							string paramName;
							string paramVal;
							p->GetName(paramName);
							p->GetValue (paramVal);
							if (paramName == "lightpreset" || paramName == "roomType" ) 
							{
								if (paramName == "backimage")
 								continue;
								ParamListSharedPtr newParam = ParamList::createNew ();
								newParam->PushString (paramName,paramVal);
								NotifyDocParamChanged (newParam->SerializeList());
								bool needRepaint = false;
								HandleAvocadoDocGeneralStringMessage ("SetDocParam",m_id,newParam->SerializeList (),needRepaint);
							}
						}
					
						for (size_t pari = 0;pari <  newParamList->GetParamCount();/*m_nvsgDocData->m_docParams->GetParamCount();*/pari++)
						{
							//Param *p = m_nvsgDocData->m_docParams->GetParam (pari);
							Param *p =  newParamList->GetParam (pari);
							string paramName;
							string paramVal;
							p->GetName(paramName);
							p->GetValue (paramVal);
							if (paramName != "lightpreset" && paramName != "roomType")
							{
								if (paramName == "backimage")
									continue;

							ParamListSharedPtr newParam = ParamList::createNew ();
							newParam->PushString (paramName,paramVal);
							NotifyDocParamChanged (newParam->SerializeList());
							bool needRepaint = false;
							HandleAvocadoDocGeneralStringMessage ("SetDocParam",m_id,newParam->SerializeList (),needRepaint);
							}
						}
						
						NotifyViewStateChanged(0);
						NotifyMaterialStateChanged();

						break;
					} 
					else if (DocLine == "<Element>")
					{
						if (elStarted)
							docValid = false;
						if (!docValid)
							break;
						elStarted = true;
						const float progressFactor = (totalElementsCount == 1 ? 1.0f : 100.0f);
						GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::LOAD_ELEMENT_STARTED,"",int (progressFactor*float(float(m_docElems.size())/float(totalElementsCount))));
					}
					else if (DocLine == "</Element>")
					{
						string namefortracing = "";
						if (m_docElems.size() > 0) 
								namefortracing = m_docElems[m_docElems.size()-1]->GetName();
						const float progressFactor = (totalElementsCount == 1 ? 1.0f : 100.0f);
						GetDocInterface()->DocuemntStatusCallback (AvocadoDocInterface::LOAD_ELEMENT_COMPLETE,namefortracing,int(progressFactor*float(float(m_docElems.size())/float(totalElementsCount))));
						if (!elStarted)
							docValid = false;
						if (!docValid)
							break;
						// Do parse accum element
						bool needRepaint = false;

						{
							// check if element is a reference or a real file.
							bool isRef = false;
							ParamListSharedPtr ppl = ParamList::createFromString (accum);
							BoolParam *bp = (BoolParam*)ppl->GetParam ("isRef");
							bp->GetValue (isRef);
							string ownerModuleForElement = "";
							if (ppl->GetParam ("OwnerModule"))
							{
								StringParam *ownerModuleParam = (StringParam*)ppl->GetParam ("OwnerModule");
								ownerModuleParam->GetValue (ownerModuleForElement);
							}
							if (ownerModuleForElement != "") 
							{
								for (size_t i=0;i<m_modules.size();i++)
								{
									if (m_modules[i]->m_name == ownerModuleForElement)
									{
										if (ownerModuleForElement == "ImportModule")
										{
											if (isRef)
											{
												ppl->PushBool ("UpdateDocUI",false);
												m_modules[i]->HandleAvocadoDocGeneralStringMessage("AddDocInstancedElement",m_id,ppl->SerializeList (),needRepaint);
											}
											else
											{
												HandleAvocadoDocGeneralStringMessage("AddDocFileElement",m_id,accum,needRepaint);
											}
										}
										else if (ownerModuleForElement == "AnnotationsModule")
										{
											HandleAvocadoDocGeneralStringMessage("AddAnnotationElement",m_id,accum,needRepaint);
										}
									}
								}
							}
							else
							{
								// default . ownder module not found.. should actually raise an error here. but this code is still alive
								// for bw compatability reasons.
								if (isRef)
								{
									ppl->PushBool ("UpdateDocUI",false);
									HandleAvocadoDocGeneralStringMessage("AddDocInstancedElement",m_id,ppl->SerializeList (),needRepaint);
								}
								else
								{
									HandleAvocadoDocGeneralStringMessage("AddDocFileElement",m_id,accum,needRepaint);
								}
							}
						}
						//HandleAvocadoDocGeneralStringMessage("AddDocFileElement",m_id,accum,needRepaint);
						elStarted = false;
						accum = "";
					}
					else
					{
						if (elStarted && docValid)
							accum += DocLine;
					}
				}
			}
		}

		return true;
	}
	void AvocadoEngineDoc::ClearViewStates ()
	{
		m_viewStates.clear();
		AvocadoViewStateData defaultVS;
		defaultVS.viewID = 0;
		defaultVS.cameraMatrix = nvmath::Mat44f(true);
		m_viewStates.push_back (defaultVS);
		NotifyViewStateChanged(0);
	}
	void AvocadoEngineDoc::ClearMaterialStates ()
	{
		m_materialStates.clear();
		NotifyMaterialStateChanged();
	}
	void AvocadoEngineDoc::PopulateViewStateWithElementLocation (int idx)
	{
		std::vector<std::pair<int,nvmath::Trafo>> elementLocation;
		std::vector<std::pair<int,bool>> elementVisibility;
		std::vector<AvocadoEngineDocElement *>::iterator it = m_docElems.begin ();
		while (it != m_docElems.end ())
		{
			nvmath::Trafo traf ;

			traf.setMatrix (nvsg::TransformWriteLock ((*it)->m_elementRoot)->getMatrix());
			
			traf.setCenter(nvsg::TransformWriteLock ((*it)->m_elementRoot)->getTrafo().getCenter ());// - nvsg::TransformWriteLock ((*it)->m_elementRoot)->getTranslation());
			elementLocation.push_back (pair<int,nvmath::Trafo>((*it)->GetID(),traf));
			elementVisibility.push_back (pair<int,bool>((*it)->GetID(),(*it)->m_intr.isVisible));
			it++;
		}
		m_viewStates[idx].elementLocation = elementLocation;
		m_viewStates[idx].elementVisibility = elementVisibility;
	}
	void AvocadoEngineDoc::RestoreElementLocationFromViewState (int idx)
	{
		int vismask = 1;//~0;
		int nonvismask = 0;
//		for (int vid = 0; vid < 8 ; vid++)
	//	 vismask = vismask & (~ int(pow(2.0,2*vid+4)));

		size_t k = 0;
		while (k < m_viewStates[idx].elementLocation.size ())
		{
			bool isVisible = true;

			//if (
			int eid = m_viewStates[idx].elementLocation[k].first;

			if ((m_viewStates[idx].elementVisibility.size() > k)
				&& (m_viewStates[idx].elementVisibility[k].first == eid))
					isVisible = m_viewStates[idx].elementVisibility[k].second;


			nvmath::Trafo trafmat = m_viewStates[idx].elementLocation[k].second;
			nvmath::Mat44f mat = trafmat.getMatrix();
			if (mat == nvmath::Mat44f (true))
			{
				k++;
				continue;
			}
			nvmath::Vec3f targetCenter = trafmat.getCenter ();
			AvocadoEngineDocElement * it = GetDocElementPtrById(eid);
			if (it == 0)
			{
				// element might be deleted..groupped or anything else..
				k++;
				continue;
			}

			// this code below is problematic... sort out hide/unhide stuff
#if 0
			int  trs = TransformWriteLock((it)->m_elementRoot)->getTraversalMask();
			if (isVisible)
				trs |= 1;
			else
				trs = 0;
			TransformWriteLock((it)->m_elementRoot)->setTraversalMask(trs);
			it->m_intr.isVisible = isVisible;
#endif		
			if (trafmat==TransformWriteLock((it)->m_elementRoot)->getTrafo())
			{
				k++;
				continue;
			}
			nvsg::Group::ChildrenIterator chPtr  = TransformWriteLock((it)->m_elementRoot)->beginChildren();
		//	nvmath::Vec3f bbtra = TransformWriteLock(nvutil::sharedPtr_cast <Transform>(*chPtr))->getTranslation ();
			nvsg::AnimatedTransformSharedPtr anit = nvsg::AnimatedTransform::create();
			nvsg::LinearInterpolatedTrafoAnimationDescriptionSharedPtr aniDesc = nvsg::LinearInterpolatedTrafoAnimationDescription::create ();

		
	//		nvsg::AnimatedTransformSharedPtr anit = nvsg::AnimatedTransform::create();
			nvmath::Trafo idtraf;
			nvmath::Vec3f centt =  TransformWriteLock((it)->m_elementRoot)->getBoundingBox().getCenter();//
			nvmath::Vec3f centt22 =  TransformWriteLock((it)->m_elementRoot)->getTrafo().getCenter();//
			nvmath::Vec3f centtChild = GroupWriteLock (chPtr->get ())->getBoundingBox().getCenter ();
			//centt -=  TransformWriteLock((*it)->m_elementRoot)->getTranslation();
			idtraf.setMatrix (nvmath::Mat44f (true));
			idtraf.setCenter (nvmath::Vec3f (0.0f,0.0f,0.0f));
			AnimatedTransformWriteLock(anit)->setTrafo( idtraf);//TransformWriteLock ((*it)->m_elementRoot)->getTrafo ()  );
			nvmath::Mat44f mat1 (true);
			nvmath::Mat44f mat2 (true);
		
			mat1 = TransformWriteLock((it)->m_elementRoot)->getMatrix ();
			mat1.invert();
			mat2 = mat * mat1;

			{
				// do combined movement..
				nvmath::Trafo ty1;
				nvmath::Trafo ty2;
				nvmath::Trafo ty3;

				ty1.setIdentity ();
				ty2.setIdentity ();
				ty3.setIdentity ();
				ty1.setMatrix (mat1);
				ty2.setMatrix (mat);
				ty3.setOrientation (ty2.getOrientation () * ty1.getOrientation ());

				nvmath::Mat33f rotma (ty3.getOrientation ());

				ty3.setTranslation (-TransformWriteLock((it)->m_elementRoot)->getTranslation () + trafmat.getTranslation ());
				mat2 = ty3.getMatrix ();
			}
			TrafoAnimationSharedPtr newAnim = TrafoAnimation::create();
			TrafoAnimationWriteLock(newAnim)->setName ("AvocadoViewStateAnimation");
		//	nvsg::KeyFramedTrafoAnimationDescriptionSharedPtr aniDesc
			//TrafoAnimationWriteLock(newAnim)->
			//nvsg::KeyFramedTrafoAnimationDescription::create ();
			{
			nvsg::LinearInterpolatedTrafoAnimationDescriptionWriteLock aniWL (aniDesc);
			
			//aniWL->insertKey (0,1, TransformWriteLock ((*it)->m_elementRoot)->getTrafo () );
			nvmath::Trafo trafomat,tr1,tr;
			trafomat.setMatrix (mat2);
		
			//	trafomat.setCenter (nvmath::Vec3f (0.0f,0.0f,0.0f));
			tr.setMatrix (mat2);
			tr1.setMatrix (mat1);
			//centt *= tr;
			nvmath::Trafo centtraf;
			centtraf.setIdentity ();
			centtraf.setTranslation (centt);
		//	tr.setMatrix (  tr.getMatrix () * centtraf.getMatrix());
			//centt = tr.getTranslation () * tr.getOrientation ();//- bbtra;

		//	centt = n vmath::Vec3f(0.0f,0.0f,0.0f);
			trafomat.setCenter (centt);//-centtChild);//- centt);//centt + trafomat.getTranslation ()*trafomat.getOrientation ());

			//trafomat.setMatrix (trafomat.getInverse());
			//aniWL->addKey (0,tr1);
			aniWL->addKey (0,idtraf);
			aniWL->addKey (60,trafomat);
		
			//aniWL->addKey (200,tr);
			
			}
			
			//aniWL->insertKey (1,1, trafomat );
			//aniWL->
			TrafoAnimationWriteLock (newAnim)->setSpeed (1);
			//TrafoAnimationWriteLock (newAnim)->advance (
			TrafoAnimationWriteLock (newAnim)->setLoopCount (1);
			//TrafoAnimationWriteLock (newAnim)->s;

			TrafoAnimationWriteLock (newAnim)->setDescription (aniDesc);
			AnimatedTransformWriteLock(anit)->setAnimation(  newAnim  );
			//TrafoAnimationWriteLock (newAnim)->setSpeed (
			//TransformWriteLock ((*it)->m_elementRoot)->setMatrix (nvmath::Mat44f(true));
			nvsg::Group::OwnerIterator gPtr  = TransformWriteLock((it)->m_elementRoot)->ownersBegin ();
			nvsg::GroupSharedPtr root = *gPtr;
			AnimatedTransformWriteLock(anit)->addChild ((it)->m_elementRoot);
			//GroupWriteLock (root)->replaceChild (anit,(*it)->m_elementRoot);
			GroupWriteLock (root)->removeChild ((it)->m_elementRoot);
			GroupWriteLock (root)->addChild (anit);
			
			TrafoAnimationWriteLock (newAnim)->start ();
			//(*it)->m_elementRoot = nvutil::sharedPtr_cast<Group>(anit);
			if ((it)->m_attachments.size ())
			{
				for (size_t ka = 0;ka < (it)->m_attachments.size();ka++)
				{
					AvocadoEngineDocElement * itq = GetDocElementPtrById( (it)->m_attachments[ka]);
					if (itq == 0)
						continue;
					NodeWriteLock (itq->m_elementRoot)->setTraversalMask (0);
				}
			}
			m_animationWaitList.push_back (anit);	
//#endif
		k++;

		}
	avocado::OnSendAvocadoGeneralStringMessage ("OnLoadAnimation",0,"");
	}
	bool AvocadoEngineDocElement::ApplyStateSet (nvsg::StateSetSharedPtr newStateSet, bool cache,bool overwrite,bool convertFFP)
	{
		if (cache)
				m_cachedStateSet = newStateSet;
				
				NodeSharedPtr root = m_elementRoot;
				nvutil::SmartPtr<nvtraverser::SearchTraverser> st( new nvtraverser::SearchTraverser );
				NVSG_ASSERT(st);
				st->setClassName("class nvsg::GeoNode");
				st->setBaseClassSearch(true);
				st->apply( root );

				const std::vector<ObjectWeakPtr> & searchResults = st->getResults();
				std::vector<ObjectWeakPtr>::const_iterator its;
				for(its=searchResults.begin(); its!=searchResults.end(); its++)
				{
					GeoNodeSharedPtr node(dynamic_cast<GeoNodeWeakPtr>(*its));
					GeoNodeWriteLock (node)->replaceAllStateSets (newStateSet);			
				}
				return true;
	}
	bool AvocadoEngineDoc::HandleAvocadoDocGeneralStringMessage (const std::string &msg, int docId,const  std::string &paramStr, bool &needRepaint)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string (msg + string("( ") +paramStr+ string(")\n")).c_str());

		if (msg == "SaveDocToFile")
		{
			/*std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
			while (it != m_viewList.end ())
			{
			AvocadoEngineView *view = *it;
			view->HandleAvocadoViewGeneralStringMessage (msg,view->GetID(),paramStr,needRepaint );
			it++;
			}*/
			avocado::OnSendAvocadoGeneralStringMessage("SaveViewToFile",this->m_viewList[0]->GetID(),paramStr);
		} 
		else if (msg == "SaveViewStateHtml")
		{
			//if (m_viewStates.size() == 1)
			//	return true;

			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			int vid;
			string htmlText;
			pl->PopString (htmlText);
			pl->PopInt (vid);
			size_t i =0;
			//for (size_t i=0;i<m_viewStates.size();i++)
			while (i<m_viewStates.size())
			{
				if (m_viewStates[i].viewID == vid)
				{
					m_viewStates[i].html_text = htmlText;
					//m_viewStates[i].viewID = -1;
					break;
				}
				i++;
			}

			if (i >= m_viewStates.size ())
				i = 0;

			m_viewStates[i].viewID = vid;
			NotifyViewStateChanged(int(i));
		}
		else if (msg == "SaveViewStateImage")
		{
			//if (m_viewStates.size() == 1)
			//	return true;

			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			int vid;
			string htmlText;
			pl->PopString (htmlText);
			pl->PopInt (vid);
			size_t i =0;
		    for ( i=0;i<m_viewStates.size();i++)
			{
				if (m_viewStates[i].viewID == vid)
				{
					break;
				}
			}
			m_viewStates[i].bg_image_file= htmlText;
		
		
		}
		// We need a new "paging" module to do these things..
		else if (msg == "NewPage")
		{
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			int vid;
			bool fit;
			pl->PopBool (fit);
			pl->PopInt (vid);
			std::vector<AvocadoEngineView *>::iterator it = GetViewById (vid);

			ViewStateSharedPtr vs =(*it)->GetCNVSGViewData ()->GetViewState();
			FrustumCameraSharedPtr cam( nvutil::sharedPtr_cast<FrustumCamera>(ViewStateReadLock(vs)->getCamera()) );

			nvmath::Mat44f mat = FrustumCameraWriteLock (cam)->getWorldToViewMatrix ();

			for (size_t i=0;i<m_viewStates.size();i++)
			{
				if (m_viewStates[i].viewID == vid)
				{
					m_viewStates[i].cameraMatrix = mat;
					m_viewStates[i].viewID = -1;
					PopulateViewStateWithElementLocation (int(i));
					break;
				}
			}
			AvocadoViewStateData vsd;
			vsd.viewID = vid;
			m_viewStates.push_back (vsd);
			NotifyViewStateChanged(int (m_viewStates.size())-1);
			if (fit)
				avocado::OnSendAvocadoGeneralStringMessage("FitToPage",vid,"");

		}
		else if (msg == "GoToPage")
		{
			if (m_viewStates.size() == 1)
				return true;

			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			int vid;
			int targetViewStateIdx;
			pl->PopInt (targetViewStateIdx);
			pl->PopInt (vid);
			std::vector<AvocadoEngineView *>::iterator it = GetViewById (vid);

			ViewStateSharedPtr vs =(*it)->GetCNVSGViewData ()->GetViewState();

			FrustumCameraSharedPtr cam( nvutil::sharedPtr_cast<FrustumCamera>(ViewStateReadLock(vs)->getCamera()) );

			nvmath::Mat44f mat = FrustumCameraWriteLock (cam)->getWorldToViewMatrix ();
			size_t i =0;

			while (i<m_viewStates.size())
			{
				if (m_viewStates[i].viewID == vid)
				{
					m_viewStates[i].cameraMatrix = mat;
					m_viewStates[i].viewID = -1;
					PopulateViewStateWithElementLocation(int(i));
					break;
				}
				i++;
			}
			i ++ ;
			if (i >= m_viewStates.size ())
				i = 0;

			m_viewStates[targetViewStateIdx].viewID = vid;
			NotifyViewStateChanged(int(targetViewStateIdx));
			RestoreElementLocationFromViewState (int(targetViewStateIdx));
			ParamListSharedPtr matPL = ParamList::createNew ();
			for (int ki=0;ki<16;ki++)
				matPL->PushFloat ("matf",m_viewStates[targetViewStateIdx].cameraMatrix.getPtr()[ki]);
			avocado::OnSendAvocadoGeneralStringMessage("MoveToCamera",vid,matPL->SerializeList());
		}
		else if (msg == "NextPage")
		{
			if (m_viewStates.size() == 1)
				return true;

			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			int vid;
			pl->PopInt (vid);
			std::vector<AvocadoEngineView *>::iterator it = GetViewById (vid);

			ViewStateSharedPtr vs =(*it)->GetCNVSGViewData ()->GetViewState();

			FrustumCameraSharedPtr cam( nvutil::sharedPtr_cast<FrustumCamera>(ViewStateReadLock(vs)->getCamera()) );

			nvmath::Mat44f mat = FrustumCameraWriteLock (cam)->getWorldToViewMatrix ();
			size_t i =0;

			while (i<m_viewStates.size())
			{
				if (m_viewStates[i].viewID == vid)
				{
					m_viewStates[i].cameraMatrix = mat;
					m_viewStates[i].viewID = -1;
					PopulateViewStateWithElementLocation(int(i));
					break;
				}
				i++;
			}
			i ++ ;
			if (i >= m_viewStates.size ())
				i = 0;

			m_viewStates[i].viewID = vid;
			NotifyViewStateChanged(int(i));
			RestoreElementLocationFromViewState (int(i));
			ParamListSharedPtr matPL = ParamList::createNew ();
			for (int ki=0;ki<16;ki++)
				matPL->PushFloat ("matf",m_viewStates[i].cameraMatrix.getPtr()[ki]);
			avocado::OnSendAvocadoGeneralStringMessage("MoveToCamera",vid,matPL->SerializeList());

		}
		else if (msg == "BackPage")
		{
			if (m_viewStates.size() == 1)
				return true;
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			int vid;
			pl->PopInt (vid);
			std::vector<AvocadoEngineView *>::iterator it = GetViewById (vid);

			ViewStateSharedPtr vs =(*it)->GetCNVSGViewData ()->GetViewState();
			FrustumCameraSharedPtr cam( nvutil::sharedPtr_cast<FrustumCamera>(ViewStateReadLock(vs)->getCamera()) );

			nvmath::Mat44f mat = FrustumCameraWriteLock (cam)->getWorldToViewMatrix ();
			int i =0;
			while (i< ( (int)m_viewStates.size()))
			{
				if (m_viewStates[i].viewID == vid)
				{
					m_viewStates[i].cameraMatrix = mat;
					m_viewStates[i].viewID = -1;
					PopulateViewStateWithElementLocation(i);
					break;
				}
				i++;
			}
			i--;
			if (i < 0)
				i = int (m_viewStates.size())-1;

			AvocadoViewStateData vsd;
			m_viewStates[i].viewID = vid;

			NotifyViewStateChanged(i);

			RestoreElementLocationFromViewState (i);
			ParamListSharedPtr matPL = ParamList::createNew ();
			for (int ki=0;ki<16;ki++)
				matPL->PushFloat ("matf",m_viewStates[i].cameraMatrix.getPtr()[ki]);
			avocado::OnSendAvocadoGeneralStringMessage("MoveToCamera",vid,matPL->SerializeList());
		}
		else if (msg == "NotifyCamAnimationEnd")
		{

			for (size_t k=0;k<m_animationWaitList.size();k++)
			{
				AnimatedTransformSharedPtr trans = m_animationWaitList[k];
				TrafoAnimationSharedPtr animTraf = AnimatedTransformReadLock (trans)->getAnimation ();
				
				Group::ChildrenIterator oldRootIT = AnimatedTransformWriteLock (trans)->beginChildren ();
				TransformSharedPtr oldRoot = nvutil::sharedPtr_cast<Transform>(*oldRootIT);
				nvsg::Group::OwnerIterator gPtr  = TransformWriteLock(trans)->ownersBegin ();
				nvsg::GroupSharedPtr root = *gPtr;
				nvmath::Mat44f mat1 = AnimatedTransformWriteLock( trans )->getTrafo().getMatrix ();
				nvmath::Mat44f mat2 = TransformWriteLock( oldRoot )->getMatrix ();
				TransformWriteLock( oldRoot )->setMatrix (mat2*mat1);
				//TransformWriteLock( oldRoot )->setCenter (AnimatedTransformWriteLock( trans )->getBoundingBox().getCenter());

				GroupWriteLock (root)->addChild (oldRoot);
				GroupWriteLock (root)->removeChild (trans);
					//GroupWriteLock (trans)->removeChild (oldRootIT);
				const void* usd = TransformReadLock (oldRoot)->getUserData ();

				if (usd != 0)
				{
						AvocadoEngineDocElement * itq = (AvocadoEngineDocElement *)(usd);//GetDocElementPtrById( (it)->m_attachments[ka]);
						if (itq == 0)
							continue;

						for (size_t ka = 0;ka < (itq)->m_attachments.size();ka++)
						{ 
								AvocadoEngineDocElement * itant = GetDocElementPtrById( (itq)->m_attachments[ka]);
								if (itant == 0)
									continue;
								NodeWriteLock (itant->m_elementRoot)->setTraversalMask (~0);
									itant->attachmentMoved (itq->GetID(),itq->m_elementRoot,GetCNVSGDocData()->GetScene());
						}				
				}
			}
			m_animationWaitList.clear();

			return true;
		}
		if (msg == "RenameElement")
		{
			ParamListSharedPtr i_ppl = ParamList::createFromString (paramStr);
			int eid;
			string newName;
			IntParam *prm = (IntParam*)i_ppl->GetParam ("eid");
			prm->GetValue (eid);
			StringParam *newnameprm = (StringParam*)i_ppl->GetParam ("newname");
			newnameprm->GetValue (newName);
			if (newName != "")
			{
				for (size_t i=0;i<this->m_docElems.size();i++)
					if (m_docElems[i]->GetID () == eid)
					{
						m_docElems[i]->SetName (newName);
						NotifyElementsChanged ();
						break;
					}
			}	
			return true;
		}
		else if (msg == "NotifyAttachmentMove")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid,eid1;
			ParamListWriteLock(pl)->PopInt (eid);
			ParamListWriteLock(pl)->PopInt (eid1);

			AvocadoEngineDocElement *docElem = GetDocElementPtrById(eid);
			AvocadoEngineDocElement *docElemAnno = GetDocElementPtrById(eid1);
			docElemAnno->attachmentMoved (eid,docElem->m_elementRoot,GetCNVSGDocData()->GetScene());
			return true;
		}
		
		else if (msg == "RegisterAttachment")
		{
				ParamListSharedPtr pl = ParamList::createFromString(paramStr);
				int eidSource,eidTarget;
				pl->PopInt (eidTarget);
				pl->PopInt (eidSource);
				AvocadoEngineDocElement *it = GetDocElementPtrById(eidTarget);
				bool invalid = false;
				for (size_t k=0;k<it->m_attachments.size();k++)
					if (it->m_attachments[k]==eidSource)
						invalid = true;

				if (!invalid)
				{
					it->m_attachments.push_back (eidSource);
					// First call on registration..
					AvocadoEngineDocElement *it2 = GetDocElementPtrById(eidSource);
					it2->attachmentMoved (it->GetID(),it->m_elementRoot,GetCNVSGDocData ()->GetScene());
				}
				return true;
		}
		else if (msg == "ExportCurrentPage")
		{
			nvsg::SceneSharedPtr currentScene = this->GetCNVSGDocData()->GetScene();
			// We should really get view ID as parameter along with the filename.. same for above message. 
			nvutil::saveScene (paramStr,currentScene,m_viewList[0]->GetCNVSGViewData()->GetViewState ());
		}
		else  if (msg == "AddDocElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			//int eid;
			//pl->PopInt(eid);
			void *ptr;
			AvocadoEngineDocElement *elem;
			ParamListWriteLock(pl)->PopPtr(&ptr);
			elem = (AvocadoEngineDocElement *)ptr;
			OnAddDocElement(&elem);
			return true;
		}
		else if (msg == "NewDocument")
		{
			FreeGlobalMaterialCache ();
			GetCNVSGDocData()->ResetRoomData();
			GetCNVSGDocData()->getIDGenerator()->m_nextID = (unsigned int)0;
			m_files.clear ();
			ClearDocElements ();
			ClearViewStates ();
			ClearMaterialStates();
			return true;
		} 
		else if (msg == "DeleteDocElement")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			ParamListWriteLock(pl)->PopInt(eid);
			OnDeleteDocElement(eid);
			return true;
		} 
		else if (msg== "SetElementMetaData")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int eid;
			bool found = false;
			string data,varname;
			ParamListWriteLock(pl)->PopInt(eid);
			ParamListWriteLock(pl)->PopString(data);
			ParamListWriteLock(pl)->PopString(varname);
			AvocadoEngineDocElement *it = GetDocElementPtrById(eid);
			AvocadoElementInterface::metaDataIterator itt = (it)->m_intr.metaData.begin  ();

			while (itt !=  (it)->m_intr.metaData.end())
			{
				if (itt->first == varname)
				{
					itt->second = data;
					found = true;
					break;
				}
				itt++;
			}
			if (!found)
			{
				pair<string,string> newMeta (varname,data);
				(it)->m_intr.metaData.push_back (newMeta);
			}
			//SetElementMetaData (eid,varname,data);
		} 
		else if (msg == "DeleteMaterialState")
		{
		}
		else if (msg == "AddMaterialState")
		{
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			string ms_name;
			pl->PopString (ms_name);
			std::vector<int> eids;
			while (pl->GetParamCount())
			{
				int x;
				pl->PopInt(x);
				eids.push_back(x);
			}
			AddMaterialState (ms_name,eids);
		}
		else if (msg == "AddToMaterialState")
		{
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			int ms_index;
			pl->PopInt (ms_index);
			std::vector<int> eids;
			while (pl->GetParamCount())
			{
				int x;
				pl->PopInt(x);
				eids.push_back(x);
			}
			AddToMaterialState (ms_index,eids);
		}
		else if (msg == "SwitchToMaterialState")
		{
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			string ms_name;
			if (pl->GetStringValueByName ("msName",ms_name))
			{
				if (SwitchToMaterialState(ms_name))
					needRepaint = true;
			} 
			else
			{
				int ms_idx;
				pl->PopInt (ms_idx);
				if (SwitchToMaterialState(ms_idx))
					needRepaint = true;
			}
		}
		else if (msg  ==  "SetDocParam")
		{
			ParamListSharedPtr pl = ParamList::createFromString (paramStr);
			if (pl->GetParam ("backColor"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("backColor");
				string val;
				ip->GetValue (val);
				stringstream ii (val);
				int ival;
				ii >> ival;
				int r = GetRValue(ival);
				int g = GetGValue(ival);
				int b = GetBValue(ival);
				GetCNVSGDocData()->SetSceneBackColor (r,g,b);
			} else if (pl->GetParam ("backimage"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("backimage");
				string val;
				ip->GetValue (val);
				std::vector<AvocadoEngineDocElement *>::iterator it = this->m_docElems.begin();
				string oldbackimage = "";
				GetCNVSGDocData()->m_docParams->GetStringValueByName ( "backimage",oldbackimage);
			//	if (oldbackimage == val)
			//		return true;
				while (it != m_docElems.end())
				{
					if ((*it)->m_intr.materialData.environmentMap != val)
					{
						if ((*it)->m_intr.materialData.reflectivity > 0.0 && (*it)->m_intr.materialID > 0)
						{
						
						(*it)->m_intr.materialData.environmentMap = val;
						MaterialBase2 mb2 (&((*it)->m_intr.materialData),m_sessionFolder);
						StateSetSharedPtr newStateSet = mb2.CreateShaderLogic ( (*it)->m_shaderFileName);
						//(it)->m_cachedStateSet = newStateSet;
						(*it)->ApplyStateSet (newStateSet,true,false);
						}
					}
					it++;
				}
				GetCNVSGDocData()->SetSceneBackImage (val);
				needRepaint = true;
			}
			else if (pl->GetParam ("globalLightAmbient"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("globalLightAmbient");
				string val;
				ip->GetValue (val);
				float floatval;
				stringstream fval (val);
				fval >> floatval;
				/*
				std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
				while (it != m_viewList.end ())
				{
					AvocadoEngineView *view = *it;
					view->GetCNVSGViewData()->SetGlobalAmbient (floatval);
					it++;
				}*/
				if (GetCNVSGDocData()->SetGlobalAmbientIntensity (floatval) )
					needRepaint = true;
			}
			else if (pl->GetParam ("GlobalAmbientColor"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("GlobalAmbientColor");
				string val;
				ip->GetValue (val);
				float floatval;
				stringstream fval (val);
				fval >> floatval;
				/*
				std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
				while (it != m_viewList.end ())
				{
					AvocadoEngineView *view = *it;
					view->GetCNVSGViewData()->SetGlobalAmbient (floatval);
					it++;
				}
				*/
				if (GetCNVSGDocData()->SetGlobalAmbientColor (floatval) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLightHeadlight"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("globalLightHeadlight");
				string val;
				ip->GetValue (val);
				float floatval;
				stringstream fval (val);
				fval >> floatval;
				std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
				while (it != m_viewList.end ())
				{
					AvocadoEngineView *view = *it;
					view->GetCNVSGViewData()->SetGlobalHeadLight (floatval);
					it++;
				}
				needRepaint = true;
			}
			else if (pl->GetParam ("roomType"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("roomType");
				string val;
				ip->GetValue (val);
				if (GetCNVSGDocData()->SetRoomType (val) )
				needRepaint = true;
			}
			else if (pl->GetParam ("roomScaleX"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("roomScaleX");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetRoomScale (0,fval) )
				needRepaint = true;
			}
			else if (pl->GetParam ("roomScaleY"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("roomScaleY");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetRoomScale (1,fval) )
				needRepaint = true;
			}
			else if (pl->GetParam ("roomScaleZ"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("roomScaleZ");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetRoomScale (2,fval) )
				needRepaint = true;
			}
			else if (pl->GetParam ("globalExposure"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalExposure");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetGlobalExposure(fval) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight1X"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight1X");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,1,0) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight1Y"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight1Y");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,1,1) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight1Z"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight1Z");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,1,2) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight1Intensity"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight1Intensity");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightIntensity(fval,1) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight1Enabled"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight1Enabled");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightEnabled(bool(fval > 0.0f),1) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight2X"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight2X");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,2,0) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight2Y"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight2Y");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,2,1) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight2Z"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight2Z");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,2,2) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight2Intensity"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight2Intensity");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightIntensity(fval,2) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight2Enabled"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight2Enabled");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightEnabled( bool(fval > 0.0f),2) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight3X"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight3X");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,3,0) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight3Y"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight3Y");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,3,1) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight3Z"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight3Z");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDirection(fval,3,2) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight3Intensity"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight3Intensity");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightIntensity(fval,3) )
					needRepaint = true;
			}
			else if (pl->GetParam ("globalLight3Enabled"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("globalLight3Enabled");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightEnabled( bool(fval > 0.0f),3) )
					needRepaint = true;
			}
			else if (pl->GetParam ("light1AmbientColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light1AmbientColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightAmbientColor (fval,1))
					needRepaint = true;
			}
			else if (pl->GetParam ("light1DiffuseColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light1DiffuseColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDiffuseColor (fval,1))
					needRepaint = true;
			}
			else if (pl->GetParam ("light1SpecularColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light1SpecularColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightSpecularColor (fval,1))
					needRepaint = true;
			}
			else if (pl->GetParam ("light2AmbientColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light2AmbientColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightAmbientColor (fval,2))
					needRepaint = true;
			}
			else if (pl->GetParam ("light2DiffuseColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light2DiffuseColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDiffuseColor (fval,2))
					needRepaint = true;
			}
			else if (pl->GetParam ("light2SpecularColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light2SpecularColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightSpecularColor (fval,2))
					needRepaint = true;
			}
			else if (pl->GetParam ("light3AmbientColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light3AmbientColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightAmbientColor (fval,3))
					needRepaint = true;
			}
			else if (pl->GetParam ("light3DiffuseColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light3DiffuseColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightDiffuseColor (fval,3))
					needRepaint = true;
			}
			else if (pl->GetParam ("light3SpecularColor"))
			{
				
				StringParam* ip = (StringParam*)pl->GetParam ("light3SpecularColor");
				string val;
				ip->GetValue (val);
				float fval;
				istringstream strstrm (val);
				strstrm >> fval;
				if (GetCNVSGDocData()->SetLightSpecularColor (fval,3))
					needRepaint = true;
			}

			else if (pl->GetParam ("lightpreset"))
			{
				StringParam* ip = (StringParam*)pl->GetParam ("lightpreset");
				string val;
				ip->GetValue (val);
				int preset;
				stringstream fval (val);
				fval >> preset;
				vector <nvsg::LightSourceSharedPtr> livec;
				GetCNVSGDocData()->SetLightsPreset (preset,livec);

				{
					//update doc params with new lights values from preset;
					int ixl = 0;
					stringstream lim[20];
					lim[ixl] << string ("string globalLight1X=") << nvsg::DirectedLightWriteLock (livec[0])->getDirection ()[0] <<string (";");
					UpdateDocParam (lim[ixl].str ());
					ixl++;

					lim[ixl] << string ("string globalLight1Y=") << nvsg::DirectedLightWriteLock (livec[0])->getDirection ()[1] <<string (";");
					UpdateDocParam (lim[ixl].str ());
					ixl++;

					lim[ixl]  << string ("string globalLight1Z=") << nvsg::DirectedLightWriteLock (livec[0])->getDirection ()[2] <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					lim[ixl]  << string ("string globalLight1Intensity=") << nvsg::DirectedLightWriteLock (livec[0])->getIntensity() <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					lim[ixl]  << string ("string globalLight1Enabled=") << float(nvsg::DirectedLightWriteLock (livec[0])->isEnabled()) <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					lim[ixl]  << string ("string globalLight2X=") << nvsg::DirectedLightWriteLock (livec[1])->getDirection ()[0] <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;

					lim[ixl]  << string ("string globalLight2Y=") << nvsg::DirectedLightWriteLock (livec[1])->getDirection ()[1] <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl ++;

					lim[ixl]  << string ("string globalLight2Z=") << nvsg::DirectedLightWriteLock (livec[1])->getDirection ()[2] <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					lim[ixl]  << string ("string globalLight2Intensity=") << nvsg::DirectedLightWriteLock (livec[1])->getIntensity () <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					lim[ixl]  << string ("string globalLight2Enabled=") << float (nvsg::DirectedLightWriteLock (livec[1])->isEnabled ()) <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;

					lim[ixl]  << string ("string globalLight3X=") << nvsg::DirectedLightWriteLock (livec[2])->getDirection ()[0] <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;

					lim[ixl]  << string ("string globalLight3Y=") << nvsg::DirectedLightWriteLock (livec[2])->getDirection ()[1] <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl ++;

					lim[ixl]  << string ("string globalLight3Z=") << nvsg::DirectedLightWriteLock (livec[2])->getDirection ()[2] <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					lim[ixl]  << string ("string globalLight3Intensity=") << nvsg::DirectedLightWriteLock (livec[2])->getIntensity () <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					lim[ixl]  << string ("string globalLight3Enabled=") << float (nvsg::DirectedLightWriteLock (livec[2])->isEnabled ()) <<string (";");
						UpdateDocParam (lim[ixl].str ());
					ixl++;
					// now the coolors..
//					nvmath::Vec3f (nvsg::DirectedLightWriteLock (livec[2])->getDiffuseColor () ;
	//				lim[ixl]  << string ("string globalLight1Diffuse=") << float (<<string (";");
		//				UpdateDocParam (lim[ixl].str ());
			//		ixl++;
					
				}
				needRepaint = true;

			}
			UpdateDocParam (paramStr);
		} 
		else if (msg == "InsertFileLink")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int type;
			bool embd;
			string filename;
			bool found = false;
			string data,varname;
			ParamListWriteLock(pl)->PopInt(type);
			ParamListWriteLock(pl)->PopString(filename);
			ParamListWriteLock(pl)->PopBool(embd);
			InsertFile (filename,(avocado::AvocadoFileLinkInterface::FileType)type,embd);
			return true;
		} 
		else if (msg == "UpdateDocumentStatus")
		{
			ParamListSharedPtr pl = ParamList::createFromString(paramStr);
			int type,prog;
			string filename;
			bool found = false;
			string data,varname;
			ParamListWriteLock(pl)->PopString(filename);
			ParamListWriteLock(pl)->PopInt(prog);
			ParamListWriteLock(pl)->PopInt(type);
			
			
			this->GetDocInterface ()->DocuemntStatusCallback ((AvocadoDocInterface::DocumentStatus)type,filename,prog);
		}
		for (size_t i=0;i<m_modules.size();i++)
		{
			// TODO : Send the message to all modules , no break. each module will decide wheather or not to implement its virtual handlers.
			/* later on , set priorites for each module */
			if (m_modules[i]->HandleAvocadoDocGeneralStringMessage(msg,docId,paramStr,needRepaint))
				return true; // Message was handled succsusfully 
		}
		{
			/* Document did not handle the message , pass it on to views. */
			std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
			while (it != m_viewList.end ())
			{
				AvocadoEngineView *view = *it;
				view->HandleAvocadoViewGeneralStringMessage (msg,view->GetID(),paramStr,needRepaint );
				it++;
			}
			return true;
		}
		return true;

	}
	bool AvocadoEngineDoc::UpdateDocParam (std::string paramStr)
	{
		NVSG_TRACE();
		NVSG_TRACE_OUT(string ("AvocadoEngine Doc Updating Param" + string("( ") +paramStr+ string(")\n")).c_str());

		string srcname;
		ParamListSharedPtr pl = ParamList::createFromString(paramStr);
		if (pl->IsEmpty())
			return false;
		Param* p = pl->GetParam(0);
		ParamListSharedPtr viewParams = GetCNVSGDocData()->m_docParams;
		p->GetName(srcname);
		Param *pdst = viewParams->GetParam (srcname);
		if (pdst)
		{
			string vdst,vsrc;
			((StringParam*)pdst)->GetValue(vdst);
			((StringParam*)p)->GetValue (vsrc);
			if (vdst!= vsrc)
			{
				((StringParam*)pdst)->SetValue (vsrc);
				NotifyDocParamChanged (paramStr);
				return true;
			}
		}
		else
		{
			string pval;
			((StringParam*)p)->GetName (srcname);
			((StringParam*)p)->GetValue (pval);
			viewParams->PushString (srcname,pval);
			NotifyDocParamChanged (paramStr);
			return true;
		}
		return false;
	}

	void	AvocadoEngineDoc::AddMaterialState (std::string name,std::vector<int> eids)
	{
		AvocadoMaterialStateData      msd;
		msd.m_name = name;
		for (size_t xi =0; xi<eids.size ();xi++)
		{
			AvocadoEngineDocElement *el = GetDocElementPtrById (eids[xi]);
			std::string materialData = el->m_intr.materialData.ToString ();
			
			msd.m_ss.push_back (std::pair<int,string>(eids[xi],materialData));
			
		}
		m_materialStates.push_back (msd);
		NotifyMaterialStateChanged ();
	}
	void	AvocadoEngineDoc::AddToMaterialState (int ms_index,std::vector<int> eids)
	{
		
		if (ms_index < 0 || ms_index > int(m_materialStates.size()))
			return;

		for (size_t xi =0; xi<eids.size ();xi++)
		{
			bool found = false;
			AvocadoEngineDocElement *el = GetDocElementPtrById (eids[xi]);
			std::string materialData = el->m_intr.materialData.ToString ();
			for (size_t lk = 0;lk < m_materialStates[ms_index].m_ss.size ();lk++)
			{
				if (m_materialStates[ms_index].m_ss[lk].first == eids[xi])
				{
					found = true;
					m_materialStates[ms_index].m_ss[lk].second = materialData;
					break;
				}
			}
			if (!found)
			     m_materialStates[ms_index].m_ss.push_back (std::pair<int,string>(eids[xi],materialData));
		}
		
		NotifyMaterialStateChanged ();
	}
	void    AvocadoEngineDoc::NotifyMaterialStateChanged ()
	{
		std::vector<AvocadoMaterialStateInterface> s;
		for (size_t xi=0;xi<m_materialStates.size();xi++)
		{
			AvocadoMaterialStateInterface mis;
			mis.name = m_materialStates[xi].m_name;
			s.push_back(mis);
		}
		m_docInterface->MaterialStateChanged (s);
	}
	bool	AvocadoEngineDoc::SwitchToMaterialState (int i)
	{
		bool needRepaint = false;
		if (i>=0 && i < int (m_materialStates.size()))
		{
			AvocadoMaterialStateData msd = m_materialStates[i];
			for (size_t xi=0;xi<msd.m_ss.size ();xi++)
			{
				// start applying materials for elements;
				int eid = msd.m_ss[xi].first;
				string edata = msd.m_ss[xi].second;
				ParamListSharedPtr pls = ParamList::createFromString (edata);
				pls->PushInt ("eid",eid);

				this->HandleAvocadoDocGeneralStringMessage ("ChangeElementMaterialPropAllColor",GetID(),pls->SerializeList(),needRepaint);
			}
		}
		return needRepaint;
	}
	bool	AvocadoEngineDoc::SwitchToMaterialState (std::string ms_name)
	{
		bool needRepaint = false;
		for (size_t i=0; i < int (m_materialStates.size()); i++)
		{
			if (m_materialStates[i].m_name == ms_name)
			{
				needRepaint = SwitchToMaterialState (int(i));
				break;
			}
		}
		return needRepaint;
	}

    void	AvocadoEngineDoc::DeleteMaterialState (int i)
	{
		int x = i;
		if (i>0 && i < int (m_materialStates.size()))
		{
			std::vector<AvocadoMaterialStateData>::iterator it = m_materialStates.begin();
			while (x<int(m_materialStates.size ()) && it != m_materialStates.end())
			{
				x++;it++;
			}
			if (it != m_materialStates.end())
				m_materialStates.erase (it);
			NotifyMaterialStateChanged();
		}
	}

	bool AvocadoEngineDoc::NotifyViewStateChanged (int i)
	{
		NVSG_TRACE();

		// apply to active view only.. needs thinking..
		vector<AvocadoViewStateInterface> vsi;

		for (size_t gg= 0; gg < m_viewStates.size();gg++)
		{
			AvocadoViewStateInterface nvs;
			nvs.htmlString = m_viewStates[gg].html_text;
			nvs.image = m_viewStates[gg].bg_image_file;
			vsi.push_back (nvs);
		}
		m_docInterface->ViewStateChanged 
			(vsi,i);
		return true;
	}
	bool AvocadoEngineDoc::NotifyDocParamChanged (string paramStr)
	{
		string srcname,srcval;
		ParamListSharedPtr pl = ParamList::createFromString(paramStr);
		Param* p = pl->GetParam(0);
		p->GetName(srcname);
		p->GetValue(srcval);
		GetDocInterface()->DocParamChanged (srcname.c_str(),srcval.c_str());
		return true;
	}
	void AvocadoEngineDoc::SetDocInterface (AvocadoDocInterface *docInterface) 
	{
		m_docInterface=docInterface;
		//NotifyViewStateChanged(0);
	}
	int AvocadoEngineDoc::OnCreateView (void *phWnd, AvocadoViewInterface *viewInterface, bool from_hwnd)
	{
		NVSG_TRACE();

		nvgl::SmartRenderContextGL m_SharedRCGL = nullptr;
		if (!m_viewList.empty())
		{
			// get the render context from the first view in the list, use it to share the render contexts between views
			std::vector<AvocadoEngineView *>::iterator itSharedRC =  m_viewList.begin ();
			AvocadoEngineView *viewSharedRC = *itSharedRC;
			m_SharedRCGL = viewSharedRC->GetCNVSGViewData()->GetRenderContextGL();
		}
		int id = AddView ();
		std::vector<AvocadoEngineView *>::iterator it = GetViewById (id);
		AvocadoEngineView *view = *it;
		bool supportOptix,supportAA;
		view->SetViewInterface (viewInterface);
		view->GetCNVSGViewData()->SetupRenderContextGL (phWnd,supportOptix,supportAA,from_hwnd,m_SharedRCGL);
		if (!supportOptix)
		{
			(*it)->UpdateViewParam ("string supportOptix=0;");
		} else
		{
			(*it)->UpdateViewParam ("string supportOptix=1;");

		}
		if (!supportAA)
		{
			(*it)->UpdateViewParam ("string supportAA=0;");
		}	else
		{
			(*it)->UpdateViewParam ("string supportAA=1;");
			(*it)->UpdateViewParam ("string AntiAliasing=1;");
		}
		view->GetCNVSGViewData()->SetupSceneRendererGL2 ();
		view->GetCNVSGViewData()->SetViewState (GetCNVSGDocData()->GetScene());
		// TODO : Initialization of default values for view parameters should be 
		// done by reading from external txt / xml file. Desirable if we have a view-params 
		// resource file on a per-module basis. MUST DO SOMETHING SMART HERE.
		// For now , we just set the default values here.
		view->UpdateViewParam ("string RenderStyle=1;");
		view->UpdateViewParam ("string OrthographicView=0;");
		view->UpdateViewParam ("string DraggerFilters=0;");
		view->UpdateViewParam ("string DisplayFilters=0;");
		view->UpdateViewParam ("string ManipulationMode=0;");
		view->UpdateViewParam ("string AntiAliasing=0;");
		view->UpdateViewParam ("string StereoView=0;");
		view->UpdateViewParam ("string PreSelectionEnabled=0;");
		view->UpdateViewParam ("string UseOptix=0;");


		return id;
	}

	
	AvocadoEngineDocElement* AvocadoEngineDoc::GetDocElementPtrById(int elemId)
	{
		AvocadoEngineDocElement * ret = 0;
		// first check cache
		DocElementHasIterator hit = m_elementHash.find (elemId);
		if (hit != m_elementHash.end ())
		{
			// great
			return hit->second;
		}
		else
		{
			std::vector<AvocadoEngineDocElement *>::iterator it = GetDocElementById (elemId);
			if (it != m_docElems.end())
			{
				ret = *it;
			//	m_cachedElementID = elemId;
				//m_cachedElement = ret;
				m_elementHash.insert (std::pair<int,AvocadoEngineDocElement *>(elemId,ret));
			}
		}
		return ret;
	}
	std::vector<AvocadoEngineDocElement *>::iterator AvocadoEngineDoc::GetDocElementById(int elemId)
	{
		std::vector<AvocadoEngineDocElement *>::iterator it = m_docElems.begin ();
		while (it != m_docElems.end ())
		{
			AvocadoEngineDocElement *elem = *it;
			int id = elem->GetID();
			if (id == elemId)
			{
				return it;
			}
			it++;
		}
		return m_docElems.end ();
	}

	void AvocadoEngineDoc::OnAddDocElement(AvocadoEngineDocElement **elem)
	{
		//AvocadoEngineDocElement *docElem = new AvocadoEngineDocElement(*elem);
		m_docElems.push_back(*elem);
	}

	void AvocadoEngineDoc::OnDeleteDocElement(int elemId)
	{
		std::vector<AvocadoEngineDocElement *>::iterator it = GetDocElementById(elemId);
		//delete *it;
		DocElementHasIterator hit= m_elementHash.find ((*it)->GetID());
		if (hit !=m_elementHash.end())
			m_elementHash.erase (hit);

		m_docElems.erase(it);
	}


	void
		AvocadoEngineDoc::ClearDocElements ()
	{
		NVSG_TRACE();
		m_elementHash.clear();
		vector<AvocadoEngineDocElement*>::iterator elit = m_docElems.begin();
		size_t orgSize = m_docElems.size();
		//for (size_t i=0;i<m_docElems.size();i++)
		// unhighlight all before delete...
		ParamListSharedPtr pl2 = ParamList::createNew();
		ParamListWriteLock(pl2)->PushBool ("prepick",false);
		ParamListWriteLock(pl2)->PushInt ("vid",0);
		if (m_docInterface && this->m_docInterface->m_isAvailable)
			// this indicates that we have a document and not leaving application;
			// there might be a better way to do this.. handle with care :)
			if (m_viewList.size() > 0)
				OnSendAvocadoDocGeneralStringMessage("OnPickNothing", m_id, ParamListWriteLock(pl2)->SerializeList());
		while (m_docElems.size())
		{
			//delete m_docElems[i];
			int elid = m_docElems[0]->GetID ();
			//int elid =(*elit)->GetID();

			// importaatnt to increment iterator before delete.
			//	elit++;
			ParamListSharedPtr pl = ParamList::createNew ();
			pl->PushInt ("eid",elid);
			pl->PushBool ("unhighlight",false);//(GetDocInterface())->m_isAvailable);
			
			string parentModule = m_docElems[0]->m_ownerModule;
			for (size_t mi=0;mi<m_modules.size();mi++)
				if (parentModule == m_modules[mi]->m_name)
				{
					bool needRepaint = false;
					m_modules[mi]->HandleAvocadoDocGeneralStringMessage ("DeleteDocCommonElement",m_id, pl->SerializeList (),needRepaint);
					//OnSendAvocadoDocGeneralStringMessage("DeleteDocCommonElement", );
				}

		}
		if (orgSize >0 &&m_docInterface&& (GetDocInterface())->m_isAvailable)
			NotifyElementsChanged ();
		m_docElems.clear();
		
	}

	bool AvocadoEngineDoc::OnSizeView (int id, int px, int py)
	{
		NVSG_TRACE();
		std::vector<AvocadoEngineView *>::iterator it = GetViewById (id);
		AvocadoEngineView *view = *it;
		view->GetCNVSGViewData ()->SetRenderContextSize (px,py);
		return true;
	}

	void AvocadoEngineDoc::InvokePaintView (int id)
	{
		NVSG_TRACE();
		std::vector<AvocadoEngineView *>::iterator it = GetViewById (id);
		AvocadoEngineView *view = *it;
		if (view->GetViewInterface () != 0 )
			view->GetViewInterface ()->InvokePaintView ();
	}
	void AvocadoEngineDoc::NotifySelectionChanged (int viewId,std::vector<int> eids)
	{
		NVSG_TRACE();
	
		std::vector<AvocadoEngineView *>::iterator it = GetViewById (viewId);
		AvocadoEngineView *view = *it;
		if (view->GetViewInterface () != 0 )
			view->GetViewInterface ()->SelectionChanged (eids);
	}
	static bool IsFileUnique (vector<AvocadoFileLinkInterface> &f,AvocadoFileLinkInterface &i)
	{
		for (size_t xi=0;xi<f.size();xi++)
		{
			if (f[xi].m_fileName == i.m_fileName)
			{
				return false;
			}
		}
		return true;
	}
	void AvocadoEngineDoc::NotifyElementsChanged ()
	{
		NVSG_TRACE();
		if (m_docInterface->m_isAvailable)
		{
			vector<string> environmentFirst; 
			vector<AvocadoFileLinkInterface> f;
			vector <AvocadoElementInterface> v;
			for (size_t i=0;i<m_docElems.size();i++)
			{
				AvocadoElementInterface t;
				// Fix the crap below !! we only need one m_intr
				t.id = m_docElems[i]->GetID();
				t.name = m_docElems[i]->GetName();

				t.color[0] = m_docElems[i]->m_intr.color[0];
				t.color[1] = m_docElems[i]->m_intr.color[1];
				t.color[2] = m_docElems[i]->m_intr.color[2];
				t.materialID = m_docElems[i]->m_intr.materialID;
				t.isVisible = m_docElems[i]->m_intr.isVisible;//m_isVisible;
				t.metaData = m_docElems[i]->m_intr.metaData;
				t.materialData = m_docElems[i]->m_intr.materialData;
				t.annotationData = m_docElems[i]->m_intr.annotationData;
				if (m_docElems[i]->m_parentGroupID > -1)
				{
					t.parentID = m_docElems[i]->m_parentGroupID ;
				}
				else
				{
					if (m_docElems[i]->m_isRef)
						t.parentID = m_docElems[i]->m_refEID;
					else
						t.parentID = -1;
				}
				t.elementType = m_docElems[i]->m_ownerModule;
				t.elementSubType = m_docElems[i]->m_refGeoNode;
				v.push_back(t);

				// add any referenced/dependency file.. like texture. bumps,environemtn.
				string bumptextureFile = m_docElems[i]->m_intr.materialData.bumpTexture;
				if ( bumptextureFile!= "")
				//if (m_docElems[i]->m_intr.materialData.bumpTexture != "")
				{
					AvocadoFileLinkInterface tf;
					tf.m_fileName = bumptextureFile;
					tf.m_type = AvocadoFileLinkInterface::IMAGE;
					if (IsFileUnique (f,tf))
						f.push_back (tf);	
				}
				string skintextureFile = m_docElems[i]->m_intr.materialData.skinTex;
				if ( skintextureFile!= ""  && m_docElems[i]->m_intr.materialData.isSkin)
				//if (m_docElems[i]->m_intr.materialData.bumpTexture != "")
				{
					AvocadoFileLinkInterface tf;
					tf.m_fileName = skintextureFile;
					tf.m_type = AvocadoFileLinkInterface::IMAGE;
					if (IsFileUnique (f,tf))
						f.push_back (tf);	
				}
				string hairtextureFile = m_docElems[i]->m_intr.materialData.hairTex;
				if ( hairtextureFile!= "" && m_docElems[i]->m_intr.materialData.isHair)
				//if (m_docElems[i]->m_intr.materialData.bumpTexture != "")
				{
					AvocadoFileLinkInterface tff;
					tff.m_fileName = hairtextureFile;
					tff.m_type = AvocadoFileLinkInterface::IMAGE;
					if (IsFileUnique (f,tff))
						f.push_back (tff);	
				}
				string textureFile = m_docElems[i]->m_intr.materialData.textureFilename;
				if ( textureFile!= "")
				{
					AvocadoFileLinkInterface tf;
					tf.m_fileName = textureFile;
					tf.m_type = AvocadoFileLinkInterface::IMAGE;
					if (IsFileUnique (f,tf))
						f.push_back (tf);	
				}
				string environmentTextureFile = m_docElems[i]->m_intr.materialData.environmentMap;
				if ( environmentTextureFile!= "")
				{
					bool skip = false;
					for (size_t ie=0;ie<environmentFirst.size();ie++)
						if (environmentFirst[ie] == environmentTextureFile)
							skip = true;
					if (!skip)
					{
						AvocadoFileLinkInterface tf;
						tf.m_fileName = environmentTextureFile;
						tf.m_type = AvocadoFileLinkInterface::IMAGE;
						environmentFirst.push_back (environmentTextureFile);
						f.push_back (tf);	
					}
				}
			}
			//vector <AvocadoFileLinkInterface> f;
			// These are model files.. 3ds, nbf, etc'..
			for (size_t i=0;i<m_files.size();i++)
			{
				AvocadoFileLinkInterface t;
				t.m_fileName = m_files[i].m_fileName;
				t.m_type = AvocadoFileLinkInterface::MODEL;
				if (IsFileUnique (f,t))
					f.push_back (t);	
			}
			// These are referenced files, i.e textures, bumps maps, environement
			m_docInterface->ElementsChanged (v,f);
		}
	}

	void AvocadoEngineDoc::InvokePaintAll ()
	{
		NVSG_TRACE();
		std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
		while (it != m_viewList.end ())
		{
			AvocadoEngineView *view = *it;
			if (view->GetViewInterface () != 0 )
				view->GetViewInterface ()->InvokePaintView ();
			it++;
		}
	}

	bool AvocadoEngineDoc::OnPaintView (int id)
	{
		NVSG_TRACE();
		bool ret = false;
		std::vector<AvocadoEngineView *>::iterator it = GetViewById (id);
		AvocadoEngineView *view = *it;
		OnSendAvocadoGeneralStringMessage("Render", id, "");
		return ret;
	}

	std::vector<AvocadoEngineView *>::iterator AvocadoEngineDoc::GetViewById (int id)
	{
		std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
		while (it != m_viewList.end ())
		{
			if ((*it)->GetID () == id)
			{
				return it;
			}
			it++;
		}
		return m_viewList.end ();
	}

	int AvocadoEngineDoc::AddView ()
	{
		NVSG_TRACE();
		int id = 0;
		AvocadoEngineView *view = new AvocadoEngineView ();
		if (!m_viewList.empty())
		{
			std::vector<AvocadoEngineView *>::iterator it = m_viewList.begin ();
			while (it != m_viewList.end ())
			{
				if ((*it)->GetID () == id)
				{
					id++;
					it = m_viewList.begin ();
					continue;
				}
				it++;
			}
		}
		std::stringstream idStr;
		idStr << id;
		std::string name = "view_" + idStr.str();
		view->SetID (id);
		view->SetName (name);
		m_viewList.push_back (view);
		return id;
	}

	bool AvocadoEngineDoc::DeleteView (int id)
	{
		std::vector<AvocadoEngineView *>::iterator it = GetViewById (id);

		delete *it;
		m_viewList.erase (it);
		return true;
	}

	void
		AvocadoEngineDoc::AddDocModule (AvocadoDocModule *module)
	{

		NVSG_TRACE();
		m_modules.push_back (module);
		module->registerModule (m_nvsgDocData->GetScene(),m_nvsgDocData->getIDGenerator(),GetID(),m_sessionFolder);
	}

	void
		AvocadoEngineDoc::ClearDocModules ()
	{
		NVSG_TRACE();
		for (size_t i=0;i<m_modules.size();i++)
		{	
			m_modules[i]->OnUnload ();
			//delete m_modules[i];
			if (m_modules[i]->m_name == "ImportModule")
			{
				AvocadoImport*	 am = (AvocadoImport*)m_modules[i];
				delete am;
			} 
			else if (m_modules[i]->m_name == "SelectionModule")
			{
				AvocadoSelection*	 am = (AvocadoSelection*)m_modules[i];
				delete am;
			}
			else if (m_modules[i]->m_name == "AnnotationsModule")
			{
				AvocadoAnnotations*	 am = (AvocadoAnnotations*)m_modules[i];
				delete am;
			}
			m_modules[i] = NULL;
		}
		m_modules.clear ();
	}

}
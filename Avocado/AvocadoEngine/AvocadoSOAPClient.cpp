/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
// AvocadoSOAPClient.cpp : Defines the entry point for the console application.
//


#import "msxml6.dll" raw_interfaces_only
//using namespace MSXML2;
#import "C:\Program Files (x86)\Common Files\MSSoap\Binaries\mssoap30.dll" \
            exclude("IStream", "IErrorInfo", "ISequentialStream", "_LARGE_INTEGER", \
                    "_ULARGE_INTEGER", "tagSTATSTG", "_FILETIME")
using namespace MSSOAPLib30;

#include "AvocadoMessageHandler.h"
#include <stdio.h>
#include <string>
#include <sstream>
#include "AvocadoSOAPClient.h"
#include "AvocadoParams.h"
#ifdef _WIN32
#include <WinInet.h>
#endif
#include <nvutil/DbgNew.h>

using namespace avocado;
	AvocadoSoapClient::AvocadoSoapClient () 
	{ 
		m_connected = false; 
		m_serviceURL = string("http://192.168.1.11:1336/WebService1.asmx?wsdl") ; 
		m_user = "guest";
		CoInitialize(NULL);
	}

	AvocadoSoapClient::~AvocadoSoapClient ()
	{
		if (m_connected)
		{
			// then what ?

		}
		CoUninitialize();
	}

string AvocadoSoapClient::CallAvocadoRemoteFunction(char * function , ParamList *pl )
{
   ISoapSerializerPtr Serializer;
   ISoapReaderPtr Reader;
   ISoapConnectorPtr Connector;

   // Connect to the service.
   Connector.CreateInstance(__uuidof(HttpConnector30));
   Connector->Property["EndPointURL"] = m_serviceURL.c_str();
   Connector->Connect();

   // Begin the message.
   //Connector->Property["SoapAction"] = "uri:AddNumbers";
   Connector->Property["SoapAction"] = string(string ("http://tempuri.org/")+string(function)).c_str();
   Connector->BeginMessage();

   // Create the SoapSerializer object.
   Serializer.CreateInstance(__uuidof(SoapSerializer30));
   // Connect the serializer object to the input stream of the connector object.
   Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));
   
   // Build the SOAP Message.s
   Serializer->StartEnvelope("soap","NONE","");
   Serializer->StartBody("");
   Serializer->StartElement(function,"http://tempuri.org/","NONE","");

  // ParamList *pl = ParamList::createFromString (string(params));
   unsigned int ind = 0;
   bool foundAttachments = false;
   while (ind <pl->GetParamCount())
   {
	   Param * p = pl->GetParam(ind);
	   
	   string val,name;
	   if (p->GetValue (val) && p->GetName (name))
	   {
		   if (p->GetType()!="file")
		   {
			   Serializer->StartElement(name.c_str(),"","","");
			   Serializer->WriteString(val.c_str());
			   Serializer->EndElement();
		   } else foundAttachments = true;
	   }
	   ind++;
   }
   Serializer->EndElement();
   Serializer->EndBody();
   Serializer->EndEnvelope();
   // Add file attachements if any 
   if (foundAttachments)
   {
	   ind = 0;
	   while (ind <pl->GetParamCount())
	   {
		   Param * p = pl->GetParam(ind);
	   
		   string val,name;
		   if (p->GetValue (val) && p->GetName (name))
		   {
			   if (p->GetType()=="file")
			   {
				   IFileAttachmentPtr FileAttachement;
				   FileAttachement.CreateInstance(__uuidof(FileAttachment30));
				   FileAttachement->FileName=val.c_str();
				   Serializer->AddAttachment (FileAttachement);
			   } 
		   }
		   ind++;
	   }

   }
   // Send the message to the XML Web service.
   Connector->EndMessage();      
  
   // Read the response.
   Reader.CreateInstance(__uuidof(SoapReader30));

   // Connect the reader to the output stream of the connector object.
   Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");
   if (Reader->Attachments->Count)
   {
	   // We have files coming in..
	   // Lets handle this thing wisely..

   }
   // Display the result.
   string ret ("");
   BSTR btext = NULL;///::SysAllocString( L"Some text with alot of words" );
   Reader->RpcResult->get_text(&btext);
   char *mbtext = new char [::wcslen (btext)+1];
   ::wcstombs (mbtext,(wchar_t*)btext,::wcslen (btext)+1);
   ret = string (mbtext);
   delete [] mbtext;
   return ret;
}

bool AvocadoSoapClient::StartSession( char * serviceUrl , char * user, char *password)
{
	m_serviceURL = serviceUrl;
	m_user = user;
	ParamListSharedPtr pl= ParamList::createNew ();
	ParamListWriteLock(pl)->PushString ("user",user);
	ParamListWriteLock(pl)->PushString ("password",password);
	m_sessionKey = CallAvocadoRemoteFunction ("StartSession", pl.get());
	if (m_sessionKey != "")
	{
		m_connected = true;
	}
	return true;
}

bool AvocadoSoapClient::EndSession ()
{
	if (m_connected)
	{
		ParamListSharedPtr pl = ParamList::createNew ();
		pl->PushString ("s_key",m_sessionKey);
		m_sessionKey = CallAvocadoRemoteFunction ("EndSession", pl.get());
		m_connected = false;
		return true;
	}
	return false;
}

string AvocadoSoapClient::SendAvocadoSharedMessage( char * messageName, char *params)
{
	if (m_connected)
	{
		ParamListSharedPtr pl = ParamList::createFromString(string (params));
		pl->PushString ("messageName",messageName);
		pl->PushString ("s_key",m_sessionKey);
		pl->PushString ("user",m_user);
		return CallAvocadoRemoteFunction ("handleMessage", pl.get());
	}
	return string ("ERR");
}

string AvocadoSoapClient::CallAvocadoFunction (char *message)
{
	if (m_connected)
	{
		ParamListSharedPtr pl = ParamList::createFromString(string (message));
		string functionName;
		pl->PopString (functionName);
		pl->PushString ("s_key",m_sessionKey);
		pl->PushString ("user",m_user);
		return CallAvocadoRemoteFunction ((char*)functionName.c_str(), /*pl.get()*/pl.get());
	}
	return string ("ERR");
}
bool AvocadoSoapClient::DownloadFileWin32 (const char* szURL, const char * szOutPutFile)
{
#ifdef _WIN32
	 //szContent[0]=0; 
#ifdef _UNICODE
	 HINTERNET hInternet=InternetOpen(L"Microsoft Internet Explorer", INTERNET_OPEN_TYPE_PRECONFIG,   
		 NULL, NULL, 0);  
#else
	HINTERNET hInternet=InternetOpen("Microsoft Internet Explorer", INTERNET_OPEN_TYPE_PRECONFIG,   
		 NULL, NULL, 0);  
#endif
	 if (!hInternet)  
		 return false;
#ifdef _UNICODE
	 size_t bc = 0;
	 wchar_t wsURL [MAX_PATH];
	 ::mbstowcs_s<MAX_PATH> (&bc,wsURL,szURL,MAX_PATH);
	 wchar_t wsOutputFile [MAX_PATH];
	 ::mbstowcs_s<MAX_PATH> (&bc,wsOutputFile,szOutPutFile,MAX_PATH);
#endif
#ifdef _UNICODE
	  HINTERNET hInternetURL=InternetOpenUrl(hInternet, wsURL, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_RELOAD|INTERNET_FLAG_HYPERLINK, 0);  
#else
	  HINTERNET hInternetURL=InternetOpenUrl(hInternet, szURL, NULL, 0, INTERNET_FLAG_NO_CACHE_WRITE|INTERNET_FLAG_RELOAD|INTERNET_FLAG_HYPERLINK, 0);  
#endif
	  if (!hInternetURL)  
	  {  
		  InternetCloseHandle(hInternet);  
		  return false;  
	  }  
	  char buff[1024];  
	  DWORD dwReadit=0;  
	  BOOL bReadFile=FALSE;  
      DWORD l=1024;  
	  long dataLength;
	  if (HttpQueryInfoA(hInternetURL, HTTP_QUERY_STATUS_CODE, buff, &l, 0) && atoi(buff)!=HTTP_STATUS_OK)  
		  return false; 
	   l = 1024;
	   if (HttpQueryInfoA(hInternetURL, HTTP_QUERY_CONTENT_LENGTH, buff, &l, 0))
	   {
			 dataLength =  atol(buff);
	   }
	  
	 // MSG msg;      
	  int pos=0;  
	  HANDLE hFile; 
      DWORD wmWritten; 
      //char strData[] = "Test data written to explain the win32 file sample";
#ifdef _UNICODE
      hFile = CreateFile(wsOutputFile,GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 
#else
	  hFile = CreateFile(szOutPutFile,GENERIC_READ|GENERIC_WRITE, FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL); 
#endif
	  {
		  stringstream params;
		  params << "int type=" << int (avocado::AvocadoDocInterface::DOWNLOAD_STARTED) << ",int prog=0,string filename="<<szOutPutFile<<";";
		  avocado::OnSendAvocadoDocGeneralStringMessage ("UpdateDocumentStatus",0,params.str ());
	  }
	  long byteCount = 0;
	  while ((bReadFile=InternetReadFile(hInternetURL, buff, 1024, &dwReadit)) && dwReadit)  
	  {
		  WriteFile(hFile,buff,(DWORD)(dwReadit),&wmWritten,NULL); 
		  byteCount +=dwReadit;
		  //memcpy(szContent+pos, buff, min((int)dwReadit, nMaxSize-pos-1));  
		  //pos+=min((int)dwReadit, nMaxSize-pos-1);  
		  //szContent[pos]=0;         
		  	  {
				stringstream params;
				int prog = 0;
				if (dataLength > 0)
					prog = int (double(double(byteCount) / double (dataLength))*100.0);
				params << "int type=" << int (avocado::AvocadoDocInterface::DOWNLOAD_PROGRESS) << ",int prog="<<prog <<",string filename="<<szOutPutFile<<";";
				avocado::OnSendAvocadoDocGeneralStringMessage ("UpdateDocumentStatus",0,params.str ());
			  }

	  }         
	  CloseHandle(hFile); 
	  InternetCloseHandle(hInternetURL);  
	  InternetCloseHandle(hInternet);  
	  {
		stringstream params;
		params << "int type=" << int (avocado::AvocadoDocInterface::DOWNLOAD_COMPLETE) << ",int prog=100,string filename="<<szOutPutFile<<";";
		avocado::OnSendAvocadoDocGeneralStringMessage ("UpdateDocumentStatus",0,params.str ());
	  }

	  return  (bReadFile==TRUE);
#endif
}
string AvocadoSoapClient::DownloadFile(const char * fileURL , char *outFile )
{
	DownloadFileWin32 (fileURL,"C:\\temp\\trialdown.down");
	return "";
   ISoapSerializerPtr Serializer;
   ISoapReaderPtr Reader;
   ISoapConnectorPtr Connector;
   std::string fileSURL = fileURL;
   std::string httpURL = fileSURL.substr (7,fileSURL.size()-7);
   std::string httpAction = httpURL.substr (httpURL.find_first_of ("/") + 1, httpURL.size()-httpURL.find_first_of ("/") - 1);
   
   httpURL = httpURL.substr  (0,httpURL.find_first_of ("/") + 1);
   httpURL = "http://" + httpURL;

   // Connect to the service.
   Connector.CreateInstance(__uuidof(HttpConnector30));
   Connector->Property["EndPointURL"] = fileSURL.c_str();//httpURL.c_str ();//fileSURL.c_str();//m_serviceURL.c_str();
   
   Connector->Connect();

   // Begin the message.
   //Connector->Property["SoapAction"] = "uri:AddNumbers";
   Connector->Property["SoapAction"] =  "";//fileSURL.c_str();//httpAction.c_str();//string(string ("http://tempuri.org/")+string(function)).c_str();
   Connector->BeginMessage();
   
   // Create the SoapSerializer object.
   Serializer.CreateInstance(__uuidof(SoapSerializer30));
   // Connect the serializer object to the input stream of the connector object.
   Serializer->Init(_variant_t((IUnknown*)Connector->InputStream));
   
   // Build the SOAP Message.s
   Serializer->StartEnvelope("soap","NONE","");
   Serializer->StartBody("");
   Serializer->StartElement(httpAction.c_str (),"http://tempuri.org/","NONE","");

  // ParamList *pl = ParamList::createFromString (string(params));
   Serializer->EndElement();
   Serializer->EndBody();
   Serializer->EndEnvelope();
   // Add file attachements if any 
   // Send the message to the XML Web service.
   Connector->EndMessage();      
  
   // Read the response.
   Reader.CreateInstance(__uuidof(SoapReader30));

   // Connect the reader to the output stream of the connector object.
   Reader->Load(_variant_t((IUnknown*)Connector->OutputStream), "");
   if (Reader->Attachments->Count)
   {
	   // We have files coming in..
	   // Lets handle this thing wisely..
	  
   }
   // Display the result.
   string ret ("");
   BSTR btext = NULL;///::SysAllocString( L"Some text with alot of words" );
   Reader->RpcResult->get_text(&btext);
   char *mbtext = new char [::wcslen (btext)+1];
   ::wcstombs (mbtext,(wchar_t*)btext,::wcslen (btext)+1);
   ret = string (mbtext);
   delete [] mbtext;
   return ret;
}

#if 0
int main()
{
   string res;
   AvocadoSoapClient as;
   char url[200],user[20],password[20],message[200]; 
   printf ("Connect to Avocado Web Service URL: \n");
   gets ( url );
   printf ("User name: \n");
   gets ( user );
   printf ("Password: \n");
   gets ( password );

   as.StartSession (url,user,password);
   while (0)
   {
	   printf ("Enter Avocado Message: \n");
	   gets ( message );
	   if (string(message)==string("quit"))
			break;
	   res = as.CallAvocadoFunction (message);
	   printf ("Reponse: %s\n",res.c_str());
   }
   as.EndSession ();
   
   return 0;
}

#endif



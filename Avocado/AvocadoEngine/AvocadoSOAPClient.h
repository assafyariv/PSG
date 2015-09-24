#pragma once
#include <string>

using namespace std;
namespace avocado{
class ParamList;
}

class AvocadoSoapClient
{
public:
	AvocadoSoapClient ();

	~AvocadoSoapClient ();
	bool StartSession( char * serviceUrl , char * user, char *password);
	bool EndSession ();
	string SendAvocadoSharedMessage( char * messageName, char *params);
	string CallAvocadoFunction (char *message);
	string DownloadFile(const char * fileURL , char *outFile );
	bool   DownloadFileWin32 (const char* szURL, const char * szOutPutFile);
private:
	string CallAvocadoRemoteFunction(char * function , avocado::ParamList *pl );

	string m_serviceURL;
	string m_user;
	string m_sessionKey;
	bool m_connected;
};


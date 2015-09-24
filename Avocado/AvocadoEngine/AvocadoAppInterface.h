/****************************************/
/* Copyright (C) 2010-2012 Assaf Yariv  */
/*--------------------------------------*/
/****************************************/

#pragma once
#ifndef AVOCADO_API
#define AVOCADO_API
#include "AvocadoAppOptionsInterface.h"

#include <string>
#include <vector>
using namespace std;

namespace avocado
{
#ifndef AVDLL
#ifdef _AVENG
#define AVDLL __declspec(dllexport) 
#else
#define AVDLL __declspec(dllimport) 
#endif
#endif
	#define MANIPILATION_MODE_CAD 1
#define MANIPILATION_MODE_USER 2
#define MANIPILATION_MODE_FLY 3

	enum AvcMouseActType {
		AVC_MOUSE_LDOWN = 0,
		AVC_MOUSE_LUP,
		AVC_MOUSE_MDOWN,
		AVC_MOUSE_MUP,
		AVC_MOUSE_RUP,
		AVC_MOUSE_RDOWN,
		AVC_MOUSE_MOVE,
		AVC_MOUSE_WHEEL,
		AVC_TIMER_TICK
		//AVC_ONPAINT
	};

	#define AVC_MK_LBUTTON	 0x0001
	#define AVC_MK_RBUTTON	 0x0002
	#define AVC_MK_SHIFT	 0x0004
	#define AVC_MK_CONTROL	 0x0008
	#define AVC_MK_MBUTTON	 0x0010
	
	

	class AvocadoFileLinkInterface  
	{
	public:
		std::string m_fileName;
		int dmy [3]; // day-month-year
		int hms [3];  // hour-minute-second
		bool m_embed;
		enum FileType 
		{
			MODEL,
			IMAGE,
			VIDEO,
			AUDIO
		};
		FileType m_type;
	};
	typedef std::vector<AvocadoFileLinkInterface> AvocadoFilesList ;

	class AvocadoViewStateInterface 
	{
	public:
		std::string htmlString;
		std::string image;
	};
	typedef std::vector<AvocadoViewStateInterface> AvocadoViewStateList ;

	class AvocadoMaterialStateInterface 
	{
	public:
		std::string name;
		std::string data;
	};
	typedef std::vector<AvocadoMaterialStateInterface> AvocadoMaterialStateList ;


#define AVOCADO_MATERIAL_CUSTOM 999
class AvocadoMaterialInterface
{
public:
	AvocadoMaterialInterface () 
{
		ambient  = 0.0f;
		baseColor[0] = 0.0f;
		baseColor[1] = 0.0f;
		baseColor[2] = 0.0f;
		bumpEnabled = true;
		bumpIsNormalMap = false;
		bumpScale = 1.0f;
		bumpTexture = "";
		diffuse = 0.0f;
		environmentMap = "";
		exposure = 0.0f;
		flakes = 0.0f;
		frenselExp = 0.0f;
		hairDens = 0.0f;
		hairLengh = 0.0f;
		hairTex = "";
		ior = 0.0f;
		isHair = false;
		isSkin = false;
		isWood = false;
		polished = 1.0f;
		reflectionMax = 0.0f;
		reflectionMin = 0.0f;
		reflectivity = 0.0f;
		ringScale = 0.01f;
		shininess = 0.0f;
		roughness = 0.0f;
		skinOil = 0.0;
		skinShin = 0.0;
		skinTex = "";
		specular = 0.0f;
		texAngle = 0.0f;
		texBlend = 0.0f;
		texEnabled = true;
		texOffsetU = 0.0f;
		texOffsetV= 0.0f;
		texRepeat = true;
		texScaleU = 1.0f;
		texScaleV = 1.0f;
		textureFilename = "";
		wooble = 0.0f;
		woodColor[0] = 0.0f;
		woodColor[1] = 0.0f;
		woodColor[2] = 0.0f;
		woodColor[3] = 0.0f;
		woodOffset[0] = 0.0f;
		woodOffset[1] = 0.0f;
		woodOffset[2] = 0.0f;
		woodScale = 1.0f;
		inherit_from_parent = true;
}
	~AvocadoMaterialInterface ()
{
}

	AVDLL std::string __stdcall ToString (); // Implentation inside AvocadoMaterials.cpp
	void FromString (std::string instr); // Implentation inside AvocadoMaterials.cpp
	// 25-Properties that will completly define the looks of the element.
	string name;
	// standard  phong
	float ambient;
	float diffuse;
	float baseColor[3];
	float specular;
	float shininess;
	// transparency control
	float opacity;
	// beckman.
	float roughness;
	// car paint.
	float polished;
	// HDR and frensel reflection.
	string environmentMap;
	float frenselExp;
	float reflectionMin;
	float reflectionMax;
	float exposure;
	// Optix.
	float reflectivity;
	float ior;
	// bump.
	float bumpHeight;
	float bumpScale;
	bool  bumpIsNormalMap;
	string bumpTexture;
	bool bumpEnabled;
	// Wood properties , zero scale is no-wood.
	bool isWood;
	float woodScale;
	float wooble;
	float ringScale;
	float woodOffset[3];
	float woodColor[4];
	// texturing.
	string textureFilename;
	float texAngle;
	float texScaleU;
	float texScaleV;
	float texOffsetU;
	float texOffsetV;
	float texBlend;
	bool  texRepeat;
	bool  texEnabled;
	// Skin and Hair.
	bool isSkin;
	bool isHair;
	float skinShin;
	float skinOil;
	float hairLengh;
	float hairDens;
	string hairTex;
	string skinTex;
	// amazin.
	float flakes;
    float noise;
	// group override.
	bool inherit_from_parent;
};
class AvocadoElementInterface 
{
public:
	int	id;
	std::string name;
	bool isVisible;
	std::string filename;
	typedef std::vector <std::pair < std::string, std::string > >  MetaDataList;
	MetaDataList metaData;
	MetaDataList annotationData;
	typedef std::vector <std::pair < std::string, std::string > >::iterator metaDataIterator;
	int materialID;
	int color[3];
	int parentID;
	std::string elementType;
	std::string elementSubType;
	AvocadoMaterialInterface materialData;
};
typedef std::vector<AvocadoElementInterface> AvocadoElementsList ;

class AvocadoViewInterface
{
public:
	virtual void InvokePaintView() = 0;
	virtual void SelectionChanged ( std::vector<int> selectedElements ) = 0;
	virtual void ViewParamChanged (const char *paramName, const char *value) = 0;
	virtual void ErrorCallback (const char *errDesc) {};
	virtual void SuccessCallback () {};
};

class AvocadoDocInterface
{
public:
	AvocadoDocInterface () {m_isAvailable=true;}
	~AvocadoDocInterface () {}
	
	enum DocumentStatus
	{
		LOAD_DOCUMENT_STARTED,
		LOAD_ELEMENT_STARTED,
		LOAD_ELEMENT_COMPLETE,
		LOAD_DOCUMENT_COMPLETE,
		SAVE_DOCUMENT_STARTED,
		SAVE_ELEMENT_STARTED,
		SAVE_ELEMENT_COMPLETE,
		SAVE_DOCUMENT_COMPLETE,
		DOWNLOAD_STARTED,
		DOWNLOAD_PROGRESS,
		DOWNLOAD_COMPLETE
	};

	virtual void ViewStateChanged (vector <AvocadoViewStateInterface>, int current )=0;
	virtual void MaterialStateChanged (vector<AvocadoMaterialStateInterface>) = 0;
	virtual void ElementsChanged ( vector <AvocadoElementInterface> ,vector <AvocadoFileLinkInterface>) = 0;
	virtual void DocParamChanged (const char *paramName, const char *value) = 0;
	virtual void ErrorCallback (const char* errDesc) {}
	virtual void SuccessCallback () {}
	virtual void DocuemntStatusCallback (DocumentStatus stat, std::string name, int progress ) {}
	bool m_isAvailable;
};

// Avocado Initizalizations and termination.
AVDLL	void __stdcall AvocadoInit(bool multiThreded=false,std::string sessionFolder = "");
AVDLL	void __stdcall AvocadoTerminate();

// Avocado Message handling. These are the most improtant API function.
AVDLL	bool __stdcall OnSendAvocadoMouseStringMessage (AvcMouseActType msg, int viewId, int x, int y, int zDelta);
AVDLL	bool __stdcall OnSendAvocadoGeneralStringMessage (const string &msg, int viewId,const string &paramStr,bool toAllModules = true);
AVDLL	bool __stdcall OnSendAvocadoDocGeneralStringMessage (const string &msg, int docId, const string &paramStr,string targetModule = "");

// Document and view creation and sizing.
AVDLL	void __stdcall SetActiveDoc(int docId);
AVDLL	int  __stdcall OnCreateDoc(AvocadoDocInterface *docInterface = NULL);
AVDLL	int  __stdcall OnCreateView(void *phWnd, AvocadoViewInterface *viewInterface);
AVDLL	int  __stdcall OnCreateDCView(void *phWnd, AvocadoViewInterface *viewInterface);
AVDLL	void __stdcall OnSizeView(int viewId, int px, int py);

// Document serialization.
AVDLL	void __stdcall DoCompressDoc (int docId, std::string &inpath,std::string &outpath,
	std::vector<std::string> attachments, 
	std::vector<std::string> embeddedModels,
	std::vector<std::string> embeddedTextures, 
	bool unzip);
AVDLL	void __stdcall OnSerializeDoc(int docId, string &path, bool isImport=false, bool isStoring=false);

// App Interface callbacks.
AVDLL	void __stdcall InvokePaintView(int viewId);
AVDLL   void __stdcall NotifySelectionChanged (int viewId,std::vector<int>  eid);
AVDLL	void __stdcall InvokePaintAll();
AVDLL	void __stdcall NotifyElementsChanged();
AVDLL	void __stdcall RaiseAvocadoViewErrorMessage(int docID,std::string err);
AVDLL	void __stdcall RaiseAvocadoDocErrorMessage(int viewID,std::string err);
// end app Interface callbacks.

// Global timer for animation and painting synchronizaion.Internal.
AVDLL	double __stdcall GetEngineTimer();

}

#endif
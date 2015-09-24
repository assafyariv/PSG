#pragma once
#include <vector>
#include <string>

namespace avocado
{
class AvocadoOption 
{
public:
	enum UIType
	{
		CHECKBOX,
		EDITBOX,
		SPINBOX,
		COMBOLIST,
		HIDDEN
	};
	enum InternalType
	{
		INT,
		FLOAT,
		STRING,
		BOOL
	};

	std::string			Name;
	std::string			Label;
	std::string			Description;
	InternalType		Type;
	UIType				UIType;
	int					valueInt;
	float				valueFloat;
	std::string			valueString;
	bool				valueBool;

	std::vector<std::string>    listValueNames;
	std::vector <float>			possibleValuesFloat;
	std::vector <int>			possibleValuesInt;
	std::vector <bool>			possibleValuesBool;
	std::vector <std::string>	possibleValuesString;

	int						scrollMin;
	int						scrollMax;
};

class AvocadoOptionPage
{
public:
	std::string name;
	std::vector < AvocadoOption > options;
};

class AvocadoOptionStructure
{
public:
	std::string name;
	std::vector < AvocadoOptionPage > pages;
};

#ifndef AVDLL
#ifdef _AVENG
#define AVDLL __declspec(dllexport) 
#else
#define AVDLL __declspec(dllimport) 
#endif
#endif

// Options getters.
AVDLL	bool  __stdcall GetEngineOptionBool(std::string name, bool *oval);
AVDLL	bool   __stdcall GetEngineOptionInt(std::string name, int *oval);
AVDLL	bool __stdcall GetEngineOptionFloat(std::string name, float *oval);
AVDLL	bool  __stdcall GetEngineOptionString(std::string name, std::string *oval);
// Options setters.
AVDLL	void __stdcall SetEngineOptionBool(std::string name,bool val);
AVDLL	void __stdcall SetEngineOptionInt(std::string name,int val);
AVDLL	void __stdcall SetEngineOptionFloat(std::string name,float val);
AVDLL	void __stdcall SetEngineOptionString(std::string name,std::string val);
// Options save to file.
AVDLL	bool __stdcall SaveOptionsFile();
// Option structure.
AVDLL   bool __stdcall GetEngineOptionStructure (AvocadoOptionStructure &op);
}
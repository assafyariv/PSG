/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "string"
#include "vector"

#include <nvutil/Handle.h>
#include <nvutil/HandledObject.h>
#include <nvutil/SmartPtr.h>
#include <nvutil/DbgNew.h>

using namespace std;
namespace avocado {
	class ParamList;
	//CORE_TYPES_BASE(ParamList);//, nvutil::HandledObject);
	typedef nvutil::SmartPtr< ParamList > ParamListSharedPtr;
	typedef ParamListSharedPtr ParamListWriteLock;
	
class Param {
public:
	Param (string name);
	~Param (); 
	virtual bool GetValue (int &val);
	virtual bool GetValue (bool &val);
	virtual bool GetValue (string &val);
	virtual bool GetValue (void** val);
	bool GetName  (string &name);
	virtual string GetType () = 0;
	string m_name; 
};

class IntParam : public Param 
{
public:
	~IntParam () {};
	IntParam (string name, int x);
	IntParam (string name, string x);
	virtual string GetType ();
	virtual bool GetValue (int &val);
	virtual bool GetValue (string &val);
	virtual void SetValue (const int val);
private:
	int value;
};
class FloatParam : public Param 
{
public:
	~FloatParam () {};
	FloatParam (string name, float x);
	FloatParam (string name, string x);
	virtual string GetType ();
	virtual bool GetValue (float &val);
	virtual bool GetValue (string &val);
	virtual void SetValue (const float val);
private:
	float value;
};
class Float16Param : public Param 
{
public:
	~Float16Param () {};
	Float16Param (string name, const float *x);
	Float16Param (string name, string x);
	virtual string GetType ();
	virtual bool GetValue (float *val);
	virtual bool GetValue (string &val);
private:
	float value[16];
};

class BoolParam : public Param 
{
public:
	~BoolParam () {};
	BoolParam (string name , bool x);
	BoolParam (string name , string x);
	virtual string GetType ();
	virtual bool GetValue (bool &val); 
	virtual bool GetValue (string &val); 
	virtual void SetValue (const bool val);
private:
	bool value;
};

class StringParam : public Param 
{
public:
	~StringParam() {}
	StringParam (string name, string x);
	virtual string GetType ();
	bool SetValue (string val);
	virtual bool GetValue (string &val);
private:
	string value;
};

class PointerParam : public Param 
{
public:
	~PointerParam() {}
	PointerParam (string name , void * x);
	PointerParam (string name , string x);
	virtual string GetType ();
	virtual bool GetValue (void **val);
private:
	void* value;
};

/* Is this cool or what ? we have a general parameter list , can represent anything and get created from a string */
class ParamList : public nvutil::RCObject
{
	/* 
		Examples of param list syntax : 
		params = "int x=1,int y=3,string s=hello,bool b5=0,bool b2=0;"

		so now , in thoery , the user can use a shell command line to send messages into Avocado.
		Not only that , in the near future , we will send messages from outside applications (== different procces , network connection).
	*/
public:
	static ParamListSharedPtr createNew ();
	static ParamListSharedPtr createFromString (const string &params);
	static ParamListSharedPtr createFromFile (string filename);
	
	const string SerializeList ();
	const bool SaveToFile (string filename);
	void Clear(); 

	/* Push Commands */
	void PushInt (string name, int i);
	void PushBool (string name,bool b);
	void PushString (string name,string str);
	void PushPtr (string name,void * p);
	void PushFloat (string name, float f);
	void PushFloat16 (string name, const float *f);

	/* Pop Commands */
	void PopInt (int &i);
	void PopBool (bool &b);
	void PopString (string &str);
	void PopPtr (void ** p);
	void PopFloat (float &f);
	void PopFloat16 (float *f);

	bool IsEmpty ();
	size_t GetParamCount ();
	Param* GetParam (size_t i);
	Param* GetParam (const string &name);
	vector <Param*> m_list;
	~ParamList();
		ParamList ();
	bool GetFloatValueByName (const string &paramname,float &target);
	bool GetBoolValueByName (const string &paramname,bool &target);
	bool GetStringValueByName (const string &paramname, std::string &target);
	bool GetIntValueByName (const string &paramname,int &target);
	bool GetFloat16ValueByName (const string &paramname,float *target);
	
private:
	const string SerializeListLow (bool endOfLine);

};

}
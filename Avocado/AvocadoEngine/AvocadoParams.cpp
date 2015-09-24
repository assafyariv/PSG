/* --------------------------------*/
/* Copyright 2010-2013 Assaf Yariv */
/* --------------------------------*/
#pragma once
#include "AvocadoParams.h"
#include <iostream> 
#include <fstream>

#include <nvutil/DbgNew.h>
namespace avocado {

	//----------------------------------------------
	// Param

	Param::Param (string name)
	{
		m_name = name;
	}

	Param::~Param()
	{
	}

	bool Param::GetValue (int &val) 
	{ 
		return false;
	}

	bool Param::GetValue (bool &val) 
	{ 
		return false;
	}

	bool Param::GetValue (string &val) 
	{ 
		return false;
	}

	bool Param::GetValue (void** val) 
	{ 
		return false;
	}

	bool Param::GetName  (string &name) 
	{ 
		name = m_name; 
		return true;
	}
	//----------------------------------------------
	// IntParam

	IntParam::IntParam(string name, int x) : Param(name)
	{
		value = x; 
	}

	IntParam::IntParam(string name, string x) : Param(name)
	{
		std::istringstream xs(x);
		xs >> value;
	}

	string IntParam::GetType () 
	{
		return "Integer";
	}
	void IntParam::SetValue (int val) 
	{ 
		value = val; 
		return;
	}
	

	bool IntParam::GetValue (int &val) 
	{ 
		val = value; 
		return true; 
	}

	bool IntParam::GetValue (string &val) 
	{ 
		std::stringstream vs;
		vs << value;
		val = vs.str();
		return true; 
	}

	//----------------------------------------------
	// FloatParam

	FloatParam::FloatParam(string name, float x) : Param(name)
	{
		value = x; 
	}

	FloatParam::FloatParam(string name, string x) : Param(name)
	{
		std::istringstream xs(x);
		xs >> value;
	}

	string FloatParam::GetType () 
	{
		return "Float";
	}

	bool FloatParam::GetValue (float &val) 
	{ 
		val = value; 
		return true; 
	}
	void FloatParam::SetValue (float val) 
	{ 
		value = val; 
		return;
	}
	
	bool FloatParam::GetValue (string &val) 
	{ 
		std::stringstream vs;
		vs << value;
		val = vs.str();
		return true; 
	}
	
	//----------------------------------------------
	// Float16Param

	Float16Param::Float16Param(string name, const float *x) : Param(name)
	{
		if (x)
		{
			for (int i=0;i<16;i++)
				value[i] = x[i]; 
		}
	}

	Float16Param::Float16Param(string name, string x) : Param(name)
	{
		//std::istringstream xs(x);
		size_t k=0;
		size_t cell = 0;
		while (k<x.size())
		{
			std::string temp  = "";
			while (x[k] != 'F')
			{
				temp+= x[k];
				k++;
			}
			std::istringstream xs(temp);
			xs >> value[cell] ;
			cell++;
			k++;
		}
	}

	string Float16Param::GetType () 
	{
		return "Float16";
	}

	bool Float16Param::GetValue (float *val) 
	{ 

		for (int i=0;i<16;i++)
				val[i] = value[i]; 

		return true; 
	}

	bool Float16Param::GetValue (string &val) 
	{ 
		std::stringstream xs;
		for (int i=0;i<16;i++)
		{
			xs << value[i] << "F";
		}
		val = xs.str();
		return true; 
	}

	//----------------------------------------------
	// BoolParam

	BoolParam::BoolParam(string name, bool x) : Param(name)
	{
		value = x; 
	}
	void BoolParam::SetValue (bool val) 
	{ 
		value = val; 
		return;
	}
	BoolParam::BoolParam(string name, string x) : Param(name)
	{
		if (x == "1") 
			value = true; 
		else 
			value = false;
	}

	string BoolParam::GetType () 
	{
		return "Boolean";
	}

	bool BoolParam::GetValue (bool &val) 
	{ 
		val = value; 
		return true; 
	}

	bool BoolParam::GetValue (string &val) 
	{ 
		if (value) 
			val = string ("1"); 
		else 
			val = string ("0");
		return true; 
	}

	//----------------------------------------------
	// StringParam

	StringParam::StringParam(string name, string x) : Param (name)
	{
		value = x;
	}

	string StringParam::GetType () 
	{
		return "String";
	}

	bool StringParam::GetValue (string &val)
	{
		val = value; 
		return true;
	}

	bool StringParam::SetValue (string val)
	{
		value = val; 
		return true;
	}
	//----------------------------------------------
	// PointerParam

	PointerParam::PointerParam(string name , void * x) : Param (name)
	{
		value = x;
	}

	PointerParam::PointerParam(string name , string x) : Param (name) 
	{
		std::istringstream xs(x);
#ifdef _AVENG64
		int xi;
#else
		long xi;
#endif
		xs >> xi;
		value = (void*)xi;
	}

	string PointerParam::GetType () 
	{
		return "Pointer";
	}

	bool PointerParam::GetValue (void **val) 
	{ 
		(*val) = value; 
		return true; 
	}

	//----------------------------------------------
	// ParamList

	ParamList::ParamList()
	{
		//m_list.clear();
	}

	ParamList::~ParamList()
	{
		for (size_t i=0;i<m_list.size();i++)
		{
			Param* p = m_list[i];
			if (p->GetType() == "Integer")
			{
				IntParam* pp = (IntParam*)p;
				delete pp;
			} 
			else if (p->GetType() == "Boolean")
			{
				BoolParam* pp = (BoolParam*)p;
				delete pp;
			}
			else if (p->GetType() == "String")
			{
				StringParam* pp = (StringParam*)p;
				delete pp;
			}
			else if (p->GetType() == "Pointer")
			{
				PointerParam* pp = (PointerParam*)p;
				delete pp;
			} 
			else if (p->GetType() == "Float")
			{
				FloatParam* pp = (FloatParam*)p;
				delete pp;
			} 
			else if (p->GetType() == "Float16")
			{
				Float16Param* pp = (Float16Param*)p;
				delete pp;
			} 
			else
			{
				int x=1;
				x++;
				//Assert !!
			}

		}
		m_list.clear();
	}

	ParamListSharedPtr ParamList::createNew ()
	{

		ParamListSharedPtr newParams =  ParamListSharedPtr(new ParamList);
		return newParams;
	}

	/* createFromString is one of the most important functions in Avocado */
	/* it is called trilions times in each run, any performance improvement is welcome */
	/* any error here might be critical */
	ParamListSharedPtr ParamList::createFromString (const string &params)
	{
		ParamList *plh = new ParamList;
		ParamListSharedPtr res = ParamListSharedPtr(plh);


		unsigned int pos = 0;
		int last = -1;
		int prevStringIdx = -1;
		while (pos < params.size())
		{
			if (params[pos] == ',' || params[pos] == ';')
			{
				string param =  params.substr (last+1,pos-last-1);
				size_t eq = param.find ("=");
				size_t sq = param.find (" ");

				//if (sq > last + 1)
				string type,name,value ;
				{
				if (sq == string::npos || eq == string::npos)
				{
					type = "none";
				}
				else
				{
					type =  param.substr (0,sq);
					name = param.substr(sq+1,eq-sq-1);
					value = param.substr (eq+1,param.size()-eq-1);
				}
				if (type == "int")
				{
					prevStringIdx = -1;
					(res)->m_list.push_back (new IntParam (name,value));
				} else
				if (type == "float")
				{
					prevStringIdx = -1;
					(res)->m_list.push_back (new FloatParam (name,value));
				}
				else if (type == "float16")
				{
					prevStringIdx = -1;
					(res)->m_list.push_back (new Float16Param (name,value));
				}
				else if (type == "bool")
				{
					prevStringIdx = -1;
					ParamListWriteLock(res)->m_list.push_back  (new BoolParam (name,value));
				}
				else if (type == "string")
				{
					prevStringIdx = (int)ParamListWriteLock(res)->m_list.size();
					ParamListWriteLock(res)->m_list.push_back (new StringParam (name,value));
				}
				else if (type == "ptr")
				{
					prevStringIdx = -1;
					ParamListWriteLock(res)->m_list.push_back (new PointerParam (name,value));
				}
				else
				{
				//	if (prevStringIdx != -1)
					{
						StringParam *p = (StringParam*)(ParamListWriteLock(res)->m_list[prevStringIdx]);
						string val;
						p->GetValue (val);
					
						val += params[last];
						val += param;
						// not sure about this one here..but I am almost sure that its needed.
						//if (pos < params.size() - 1)
					
								
						p->SetValue (val);
					}
				//	else
				//	{
						// this is a big problem !!! we dont want to get here, never.
				//	}
				}

				}
						last = pos;
			}
			pos++;
		}
		string param =  params.substr (last+1,pos-last-1);
		if (param.size() > 1)
		{
			param.clear ();
		}
		return res;
	}	
	const string ParamList::SerializeList ()
	{
		return SerializeListLow(false);
	}
	ParamListSharedPtr ParamList::createFromFile (string filename)
	{
		ifstream infile (filename);
		if (!infile)
			return 0;
		char linestr [255];
		string res = "";
		while (!infile.eof ())
		{
			infile.getline (linestr,255);
			res += string (linestr);
		}
		ParamListSharedPtr resP = createFromString  (res);
		return resP;
	}
	const bool ParamList::SaveToFile (string filename)
	{
		string res = SerializeListLow (true);
		ofstream out(filename); 
		if(!out) { 
			//cout << "Cannot open file.\n"; 
			return false;
		   } 
 
		out << res; 
 
		out.close();
		return true;
	}
	
	const string ParamList::SerializeListLow (bool endOfLine)
	{
		string res ("");
		for (size_t i=0;i<m_list.size();i++)
		{
			Param* p  = m_list[i];
			string name;
			p->GetName (name);
			if (p->GetType()=="Integer")
			{
				int val ;
				((IntParam*)p)->GetValue(val);
				std::stringstream vs;
				vs << val;
				res.append ("int ");
				res.append (name);
				res.append ("=");
				res.append (vs.str());
				res.append (",");
			}
			if (p->GetType()=="Float")
			{
				float val ;
				((FloatParam*)p)->GetValue(val);
				std::stringstream vs;
				vs << val;
				res.append ("float ");
				res.append (name);
				res.append ("=");
				res.append (vs.str());
				res.append (",");
			}
			if (p->GetType()=="Float16")
			{
				//1float val[16] ;
				std::string val;
				((Float16Param*)p)->GetValue(val);
				std::stringstream vs;
				vs << val;
				res.append ("float16 ");
				res.append (name);
				res.append ("=");
				res.append (vs.str());
				res.append (",");
			}
			else if (p->GetType()=="Boolean")
			{
				bool val ;
				((BoolParam*)p)->GetValue(val);
				std::stringstream vs;
				vs << val;
				res.append ("bool ");
				res.append (name);
				res.append ("=");
				res.append (vs.str());
				res.append (",");
			}
			else if (p->GetType()=="String")
			{
				string val;
				((StringParam*)p)->GetValue(val);		
				res.append ("string ");
				res.append (name);
				res.append ("=");
				res.append (val);
				res.append (",");
			}
			else if (p->GetType()== "Pointer")
			{
				void* val ;
				((PointerParam*)p)->GetValue(&val);
				int vp = (int)val;
				std::stringstream vs;
				vs << vp;
				res.append ("ptr ");
				res.append (name);
				res.append ("=");
				res.append (vs.str());
				res.append (",");
			}
			if (endOfLine)
				res.append ("\n");
		}

		return res;
	}

	void ParamList::Clear()
	{ 
		m_list.clear();
	}

	/* Push Commands */
	void ParamList::PushInt (string name, int i)
	{
		m_list.push_back(new IntParam (name,i));
	}
	void ParamList::PushFloat (string name, float f)
	{
		m_list.push_back(new FloatParam (name,f));
	}
	void ParamList::PushFloat16 (string name, const float *f)
	{
		m_list.push_back(new Float16Param (name,f));
	}
	void ParamList::PushBool (string name,bool b)
	{
		m_list.push_back(new BoolParam (name,b)); 
	}
	void ParamList::PushString (string name,string str)
	{
		m_list.push_back(new StringParam (name,str)); 
	}

	void ParamList::PushPtr (string name,void * p)
	{
		m_list.push_back (new PointerParam (name,p));
	}	

	/* Pop Commands */
	void ParamList::PopInt (int &i) 
	{
		((IntParam*)(m_list.back()))->GetValue (i); 
		IntParam* ip = (IntParam*)m_list[m_list.size()-1];
		delete ip;
		m_list.pop_back(); 
	}
	void ParamList::PopFloat (float &f) 
	{
		((FloatParam*)(m_list.back()))->GetValue (f); 
		FloatParam* ip = (FloatParam*)m_list[m_list.size()-1];
		delete ip;
		m_list.pop_back(); 
	}
	void ParamList::PopFloat16 (float *f) 
	{
		((Float16Param*)(m_list.back()))->GetValue (f); 
		Float16Param* ip = (Float16Param*)m_list[m_list.size()-1];
		delete ip;
		m_list.pop_back(); 
	}
	void ParamList::PopBool (bool &b)
	{ 
		((BoolParam*)(m_list.back()))->GetValue (b);
		BoolParam* bp = (BoolParam*)m_list[m_list.size()-1];
		delete bp;
		m_list.pop_back(); 
	}

	void ParamList::PopString (string &str)
	{
		((StringParam*)(m_list.back()))->GetValue (str); 
		StringParam* sp = (StringParam*)m_list[m_list.size()-1];
		delete sp;
		m_list.pop_back(); 
	}

	void ParamList::PopPtr (void ** p) 
	{
		((PointerParam*)(m_list.back()))->GetValue (p); 
		PointerParam* pp = (PointerParam*)m_list[m_list.size()-1];
		delete pp;
		m_list.pop_back(); 
	} 

	bool ParamList::IsEmpty () 
	{
		return m_list.empty();
	}

	size_t ParamList::GetParamCount () 
	{
		return m_list.size();
	}

	Param* ParamList::GetParam (size_t i) 
	{
		return m_list[i];
	}
	Param* ParamList::GetParam (const string &name) 
	{
		size_t i=0;
		while (i<m_list.size())
		{
			string cn;
			m_list[i]->GetName (cn);
			if (cn == name)
				return m_list[i];
			i++;
		}
		return 0;
	}
	bool  ParamList::GetFloatValueByName (const string &paramname,float &target)
    {
		    Param* trp = GetParam (paramname);
		    if (trp)
			{
				FloatParam *irmat = (FloatParam*)trp;//GetParam ( paramname);
				irmat->GetValue (target);
				return true;
			}
			return false;
	 }
	bool  ParamList::GetFloat16ValueByName (const string &paramname,float *target)
    {
		    Param* trp = GetParam (paramname);
		    if (trp)
			{
				Float16Param *irmat = (Float16Param*)trp;//GetParam ( paramname);
				irmat->GetValue (target);
				return true;
			}
			return false;
	 }
	bool  ParamList::GetBoolValueByName (const string &paramname,bool &target)
	{
		    Param* trp = GetParam (paramname);
		    if (trp)
			{
				BoolParam *irmat = (BoolParam*)trp;//GetParam ( paramname);
				irmat->GetValue (target);
				return true;
			}
			return false;
	}
	bool  ParamList::GetStringValueByName (const string &paramname, std::string &target)
	{
		   Param* trp = GetParam (paramname);
		    if (trp)
			{
				StringParam *irmat = (StringParam*)trp;//GetParam ( paramname);
				irmat->GetValue (target);
				return true;
			}
			return false;
	}
	bool  ParamList::GetIntValueByName (const string &paramname,int &target)
	{
			Param* trp = GetParam (paramname);
		    if (trp)
			{
				IntParam *irmat = (IntParam*)trp;//GetParam ( paramname);
				irmat->GetValue (target);
				return true;
			}
			return false;
	}
  
}
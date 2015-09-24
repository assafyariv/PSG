/*
 * idispimp.CPP
 * IDispatch for Extending Dynamic HTML Object Model
 *
 * Copyright (c)1995-2000 Microsoft Corporation, All Rights Reserved
 */

#include "stdafx.h"
#include "idispimp.h"
#include "Avocado.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Hardcoded information for extending the Object Model 
// Typically this would be supplied through a TypeInfo
// In this case the name "xxyyzz" maps to DISPID_Extend 
const	WCHAR pszExtend[15]=L"SendAvocadoMsg";
#define DISPID_Extend 12345



/*
 * CImpIDispatch::CImpIDispatch
 * CImpIDispatch::~CImpIDispatch
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CImpIDispatch::CImpIDispatch( void )
{
    m_cRef = 0;
}

CImpIDispatch::~CImpIDispatch( void )
{
	ASSERT( m_cRef == 0 );
}


/*
 * CImpIDispatch::QueryInterface
 * CImpIDispatch::AddRef
 * CImpIDispatch::Release
 *
 * Purpose:
 *  IUnknown members for CImpIDispatch object.
 */

STDMETHODIMP CImpIDispatch::QueryInterface( REFIID riid, void **ppv )
{
    *ppv = NULL;


    if ( IID_IDispatch == riid )
	{
        *ppv = this;
	}
	
	if ( NULL != *ppv )
    {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
    }

	return E_NOINTERFACE;
}


STDMETHODIMP_(ULONG) CImpIDispatch::AddRef(void)
{
    return ++m_cRef;
}

STDMETHODIMP_(ULONG) CImpIDispatch::Release(void)
{
    return --m_cRef;
}


//IDispatch
STDMETHODIMP CImpIDispatch::GetTypeInfoCount(UINT* /*pctinfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetTypeInfo(/* [in] */ UINT /*iTInfo*/,
            /* [in] */ LCID /*lcid*/,
            /* [out] */ ITypeInfo** /*ppTInfo*/)
{
	return E_NOTIMPL;
}

STDMETHODIMP CImpIDispatch::GetIDsOfNames(
            /* [in] */ REFIID riid,
            /* [size_is][in] */ OLECHAR** rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID* rgDispId)
{
	HRESULT hr;
	UINT	i;

	// Assume some degree of success
	hr = NOERROR;

	// Hardcoded mapping for this sample
	// A more usual procedure would be to use a TypeInfo
	for ( i=0; i < cNames; i++)
	{
		if (  2 == CompareString( lcid, NORM_IGNOREWIDTH, (wchar_t*)pszExtend, 3, (wchar_t*)rgszNames[i], 3 ) )
		{
			rgDispId[i] = DISPID_Extend;
		}
		else
		{
			// One or more are unknown so set the return code accordingly
			hr = ResultFromScode(DISP_E_UNKNOWNNAME);
			rgDispId[i] = DISPID_UNKNOWN;
		}
	}
	return hr;
}

#ifdef UNICODE
   static std::wstring & replaceSubStr(std::wstring & subj, std::wstring old, std::wstring neu)
    {
		while (true)
		{
			size_t uiui = subj.find(old);
			if (uiui != std::wstring::npos)
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
   	std::wstring decodeJavaScriptParams (std::wstring params)
	{
		std::wstring result = params;

		// replace AvInt with int
		result = replaceSubStr (result,L"AvInt",L"int");
		// replace AvString with string
		result = replaceSubStr (result,L"AvString",L"string");
		// replace AvBool with bool
		result = replaceSubStr (result,L"AvBool",L"bool");
		// replace Av
		result = replaceSubStr (result,L"AvFloat",L"float");
		//result = replaceSubStr (result,L"@",L",");
		
		return result;
	}
#else
   static std::string & replaceSubStr(std::string & subj, std::string old, std::string neu)
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
	std::string  decodeJavaScriptParams (std::string params)
	{
		std::string result = params;

		// replace AvInt with int
		result = replaceSubStr (result,"AvInt","int");
		result = replaceSubStr (result,"AvString","string");
		result = replaceSubStr (result,"AvBool","bool");
		result = replaceSubStr (result,"AvFloat","float");
		return result;
		// replace AvString with string
		// replace AvBool with bool
		// replace Av
	}
#endif
STDMETHODIMP CImpIDispatch::Invoke(
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID /*riid*/,
            /* [in] */ LCID /*lcid*/,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS* pDispParams,
            /* [out] */ VARIANT* pVarResult,
            /* [out] */ EXCEPINFO* /*pExcepInfo*/,
            /* [out] */ UINT* puArgErr)
{

	// For this sample we only support a Property Get on DISPID_Extend
	// returning a BSTR with "Wibble" as the value
	if ( dispIdMember == DISPID_Extend )
	{
		if ( wFlags & DISPATCH_PROPERTYGET )
		{/*
			if ( pVarResult != NULL )
			{
				WCHAR buff[10]=L"Wibble";
				BSTR bstrRet = SysAllocString( buff );
				VariantInit(pVarResult);
				V_VT(pVarResult)=VT_BSTR;
				V_BSTR(pVarResult) = bstrRet;
			}*/
		} 
		else
		if (DISPATCH_METHOD & wFlags)
		{
			if (pDispParams != NULL)
			{
				unsigned int paramCount = pDispParams->cArgs;
				BSTR msgName = pDispParams->rgvarg[1].bstrVal;
				BSTR msgParams = pDispParams->rgvarg[0].bstrVal;
				
				
				theApp.SendAvocadoDocMessage (msgName,decodeJavaScriptParams (msgParams).c_str ());
			}
		}
	}

	return S_OK;
}

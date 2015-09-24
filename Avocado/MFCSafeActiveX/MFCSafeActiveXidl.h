

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat May 18 11:34:06 2013
 */
/* Compiler settings for MFCSafeActiveX.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 7.00.0555 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__


#ifndef __MFCSafeActiveXidl_h__
#define __MFCSafeActiveXidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DMFCSafeActiveX_FWD_DEFINED__
#define ___DMFCSafeActiveX_FWD_DEFINED__
typedef interface _DMFCSafeActiveX _DMFCSafeActiveX;
#endif 	/* ___DMFCSafeActiveX_FWD_DEFINED__ */


#ifndef ___DMFCSafeActiveXEvents_FWD_DEFINED__
#define ___DMFCSafeActiveXEvents_FWD_DEFINED__
typedef interface _DMFCSafeActiveXEvents _DMFCSafeActiveXEvents;
#endif 	/* ___DMFCSafeActiveXEvents_FWD_DEFINED__ */


#ifndef __MFCSafeActiveX_FWD_DEFINED__
#define __MFCSafeActiveX_FWD_DEFINED__

#ifdef __cplusplus
typedef class MFCSafeActiveX MFCSafeActiveX;
#else
typedef struct MFCSafeActiveX MFCSafeActiveX;
#endif /* __cplusplus */

#endif 	/* __MFCSafeActiveX_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __MFCSafeActiveXLib_LIBRARY_DEFINED__
#define __MFCSafeActiveXLib_LIBRARY_DEFINED__

/* library MFCSafeActiveXLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_MFCSafeActiveXLib;

#ifndef ___DMFCSafeActiveX_DISPINTERFACE_DEFINED__
#define ___DMFCSafeActiveX_DISPINTERFACE_DEFINED__

/* dispinterface _DMFCSafeActiveX */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DMFCSafeActiveX;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6267760D-4EDC-430A-A94F-1181971ABA02")
    _DMFCSafeActiveX : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMFCSafeActiveXVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DMFCSafeActiveX * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DMFCSafeActiveX * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DMFCSafeActiveX * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DMFCSafeActiveX * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DMFCSafeActiveX * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DMFCSafeActiveX * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DMFCSafeActiveX * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DMFCSafeActiveXVtbl;

    interface _DMFCSafeActiveX
    {
        CONST_VTBL struct _DMFCSafeActiveXVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMFCSafeActiveX_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DMFCSafeActiveX_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DMFCSafeActiveX_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DMFCSafeActiveX_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DMFCSafeActiveX_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DMFCSafeActiveX_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DMFCSafeActiveX_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMFCSafeActiveX_DISPINTERFACE_DEFINED__ */


#ifndef ___DMFCSafeActiveXEvents_DISPINTERFACE_DEFINED__
#define ___DMFCSafeActiveXEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DMFCSafeActiveXEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DMFCSafeActiveXEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("050C9E59-ADA3-440A-92B4-59AE97009569")
    _DMFCSafeActiveXEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DMFCSafeActiveXEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DMFCSafeActiveXEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DMFCSafeActiveXEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DMFCSafeActiveXEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DMFCSafeActiveXEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DMFCSafeActiveXEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DMFCSafeActiveXEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DMFCSafeActiveXEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DMFCSafeActiveXEventsVtbl;

    interface _DMFCSafeActiveXEvents
    {
        CONST_VTBL struct _DMFCSafeActiveXEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DMFCSafeActiveXEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DMFCSafeActiveXEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DMFCSafeActiveXEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DMFCSafeActiveXEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DMFCSafeActiveXEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DMFCSafeActiveXEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DMFCSafeActiveXEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DMFCSafeActiveXEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_MFCSafeActiveX;

#ifdef __cplusplus

class DECLSPEC_UUID("1EBAE592-7515-43C2-A6F1-CDEEDF3FD82B")
MFCSafeActiveX;
#endif
#endif /* __MFCSafeActiveXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



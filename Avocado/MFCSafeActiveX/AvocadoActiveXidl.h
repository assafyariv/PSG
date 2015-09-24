

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sun Jun 30 12:42:28 2013
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


#ifndef __AvocadoActiveXidl_h__
#define __AvocadoActiveXidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DAvocadoActiveX_FWD_DEFINED__
#define ___DAvocadoActiveX_FWD_DEFINED__
typedef interface _DAvocadoActiveX _DAvocadoActiveX;
#endif 	/* ___DAvocadoActiveX_FWD_DEFINED__ */


#ifndef ___DAvocadoActiveXEvents_FWD_DEFINED__
#define ___DAvocadoActiveXEvents_FWD_DEFINED__
typedef interface _DAvocadoActiveXEvents _DAvocadoActiveXEvents;
#endif 	/* ___DAvocadoActiveXEvents_FWD_DEFINED__ */


#ifndef __AvocadoActiveX_FWD_DEFINED__
#define __AvocadoActiveX_FWD_DEFINED__

#ifdef __cplusplus
typedef class AvocadoActiveX AvocadoActiveX;
#else
typedef struct AvocadoActiveX AvocadoActiveX;
#endif /* __cplusplus */

#endif 	/* __AvocadoActiveX_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __AvocadoActiveXLib_LIBRARY_DEFINED__
#define __AvocadoActiveXLib_LIBRARY_DEFINED__

/* library AvocadoActiveXLib */
/* [control][helpstring][helpfile][version][uuid] */ 


EXTERN_C const IID LIBID_AvocadoActiveXLib;

#ifndef ___DAvocadoActiveX_DISPINTERFACE_DEFINED__
#define ___DAvocadoActiveX_DISPINTERFACE_DEFINED__

/* dispinterface _DAvocadoActiveX */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DAvocadoActiveX;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6267760D-4EDC-430A-A94F-1181971ABA02")
    _DAvocadoActiveX : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAvocadoActiveXVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAvocadoActiveX * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAvocadoActiveX * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAvocadoActiveX * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAvocadoActiveX * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAvocadoActiveX * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAvocadoActiveX * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAvocadoActiveX * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAvocadoActiveXVtbl;

    interface _DAvocadoActiveX
    {
        CONST_VTBL struct _DAvocadoActiveXVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAvocadoActiveX_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAvocadoActiveX_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAvocadoActiveX_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAvocadoActiveX_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAvocadoActiveX_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAvocadoActiveX_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAvocadoActiveX_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAvocadoActiveX_DISPINTERFACE_DEFINED__ */


#ifndef ___DAvocadoActiveXEvents_DISPINTERFACE_DEFINED__
#define ___DAvocadoActiveXEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAvocadoActiveXEvents */
/* [helpstring][uuid] */ 


EXTERN_C const IID DIID__DAvocadoActiveXEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("050C9E59-ADA3-440A-92B4-59AE97009569")
    _DAvocadoActiveXEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAvocadoActiveXEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAvocadoActiveXEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAvocadoActiveXEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAvocadoActiveXEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAvocadoActiveXEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAvocadoActiveXEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAvocadoActiveXEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAvocadoActiveXEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAvocadoActiveXEventsVtbl;

    interface _DAvocadoActiveXEvents
    {
        CONST_VTBL struct _DAvocadoActiveXEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAvocadoActiveXEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAvocadoActiveXEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAvocadoActiveXEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAvocadoActiveXEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAvocadoActiveXEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAvocadoActiveXEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAvocadoActiveXEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAvocadoActiveXEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AvocadoActiveX;

#ifdef __cplusplus

class DECLSPEC_UUID("1EBAE592-7515-43C2-A6F1-CDEEDF3FD82B")
AvocadoActiveX;
#endif
#endif /* __AvocadoActiveXLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



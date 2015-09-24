

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Tue Jul 03 20:26:30 2012
 */
/* Compiler settings for AvocadoActiveXCtrl.idl:
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


#ifndef __AvocadoActiveXCtrlidl_h__
#define __AvocadoActiveXCtrlidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef ___DAvocadoActiveXCtrl_FWD_DEFINED__
#define ___DAvocadoActiveXCtrl_FWD_DEFINED__
typedef interface _DAvocadoActiveXCtrl _DAvocadoActiveXCtrl;
#endif 	/* ___DAvocadoActiveXCtrl_FWD_DEFINED__ */


#ifndef ___DAvocadoActiveXCtrlEvents_FWD_DEFINED__
#define ___DAvocadoActiveXCtrlEvents_FWD_DEFINED__
typedef interface _DAvocadoActiveXCtrlEvents _DAvocadoActiveXCtrlEvents;
#endif 	/* ___DAvocadoActiveXCtrlEvents_FWD_DEFINED__ */


#ifndef __AvocadoActiveXCtrl_FWD_DEFINED__
#define __AvocadoActiveXCtrl_FWD_DEFINED__

#ifdef __cplusplus
typedef class AvocadoActiveXCtrl AvocadoActiveXCtrl;
#else
typedef struct AvocadoActiveXCtrl AvocadoActiveXCtrl;
#endif /* __cplusplus */

#endif 	/* __AvocadoActiveXCtrl_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __AvocadoActiveXCtrlLib_LIBRARY_DEFINED__
#define __AvocadoActiveXCtrlLib_LIBRARY_DEFINED__

/* library AvocadoActiveXCtrlLib */
/* [control][version][uuid] */ 


EXTERN_C const IID LIBID_AvocadoActiveXCtrlLib;

#ifndef ___DAvocadoActiveXCtrl_DISPINTERFACE_DEFINED__
#define ___DAvocadoActiveXCtrl_DISPINTERFACE_DEFINED__

/* dispinterface _DAvocadoActiveXCtrl */
/* [uuid] */ 


EXTERN_C const IID DIID__DAvocadoActiveXCtrl;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("B220DA40-0F9D-4B4F-9878-AA204FC855BE")
    _DAvocadoActiveXCtrl : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAvocadoActiveXCtrlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAvocadoActiveXCtrl * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAvocadoActiveXCtrl * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAvocadoActiveXCtrl * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAvocadoActiveXCtrl * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAvocadoActiveXCtrl * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAvocadoActiveXCtrl * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAvocadoActiveXCtrl * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAvocadoActiveXCtrlVtbl;

    interface _DAvocadoActiveXCtrl
    {
        CONST_VTBL struct _DAvocadoActiveXCtrlVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAvocadoActiveXCtrl_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAvocadoActiveXCtrl_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAvocadoActiveXCtrl_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAvocadoActiveXCtrl_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAvocadoActiveXCtrl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAvocadoActiveXCtrl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAvocadoActiveXCtrl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAvocadoActiveXCtrl_DISPINTERFACE_DEFINED__ */


#ifndef ___DAvocadoActiveXCtrlEvents_DISPINTERFACE_DEFINED__
#define ___DAvocadoActiveXCtrlEvents_DISPINTERFACE_DEFINED__

/* dispinterface _DAvocadoActiveXCtrlEvents */
/* [uuid] */ 


EXTERN_C const IID DIID__DAvocadoActiveXCtrlEvents;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("6976EC40-2A03-4DA5-A98B-F5FCE78691F7")
    _DAvocadoActiveXCtrlEvents : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct _DAvocadoActiveXCtrlEventsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            _DAvocadoActiveXCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            _DAvocadoActiveXCtrlEvents * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            _DAvocadoActiveXCtrlEvents * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            _DAvocadoActiveXCtrlEvents * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            _DAvocadoActiveXCtrlEvents * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            _DAvocadoActiveXCtrlEvents * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            _DAvocadoActiveXCtrlEvents * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } _DAvocadoActiveXCtrlEventsVtbl;

    interface _DAvocadoActiveXCtrlEvents
    {
        CONST_VTBL struct _DAvocadoActiveXCtrlEventsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define _DAvocadoActiveXCtrlEvents_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define _DAvocadoActiveXCtrlEvents_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define _DAvocadoActiveXCtrlEvents_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define _DAvocadoActiveXCtrlEvents_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define _DAvocadoActiveXCtrlEvents_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define _DAvocadoActiveXCtrlEvents_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define _DAvocadoActiveXCtrlEvents_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* ___DAvocadoActiveXCtrlEvents_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_AvocadoActiveXCtrl;

#ifdef __cplusplus

class DECLSPEC_UUID("3702B77E-8FD4-452E-AE15-82BEEF40CBD2")
AvocadoActiveXCtrl;
#endif
#endif /* __AvocadoActiveXCtrlLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



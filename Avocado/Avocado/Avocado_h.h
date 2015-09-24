

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat Dec 14 12:12:55 2013
 */
/* Compiler settings for Avocado.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
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


#ifndef __Avocado_h_h__
#define __Avocado_h_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IAvocado_FWD_DEFINED__
#define __IAvocado_FWD_DEFINED__
typedef interface IAvocado IAvocado;
#endif 	/* __IAvocado_FWD_DEFINED__ */


#ifndef __CAvocadoDoc_FWD_DEFINED__
#define __CAvocadoDoc_FWD_DEFINED__

#ifdef __cplusplus
typedef class CAvocadoDoc CAvocadoDoc;
#else
typedef struct CAvocadoDoc CAvocadoDoc;
#endif /* __cplusplus */

#endif 	/* __CAvocadoDoc_FWD_DEFINED__ */


#ifdef __cplusplus
extern "C"{
#endif 



#ifndef __IAvocado_LIBRARY_DEFINED__
#define __IAvocado_LIBRARY_DEFINED__

/* library IAvocado */
/* [version][uuid] */ 


EXTERN_C const IID LIBID_IAvocado;

#ifndef __IAvocado_DISPINTERFACE_DEFINED__
#define __IAvocado_DISPINTERFACE_DEFINED__

/* dispinterface IAvocado */
/* [uuid] */ 


EXTERN_C const IID DIID_IAvocado;

#if defined(__cplusplus) && !defined(CINTERFACE)

    MIDL_INTERFACE("F1BE1303-1859-46CA-A6C9-8A1E6E813223")
    IAvocado : public IDispatch
    {
    };
    
#else 	/* C style interface */

    typedef struct IAvocadoVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAvocado * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            __RPC__deref_out  void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAvocado * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAvocado * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            IAvocado * This,
            /* [out] */ UINT *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            IAvocado * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            IAvocado * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            IAvocado * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
        
        END_INTERFACE
    } IAvocadoVtbl;

    interface IAvocado
    {
        CONST_VTBL struct IAvocadoVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAvocado_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define IAvocado_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define IAvocado_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define IAvocado_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define IAvocado_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define IAvocado_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define IAvocado_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */


#endif 	/* __IAvocado_DISPINTERFACE_DEFINED__ */


EXTERN_C const CLSID CLSID_CAvocadoDoc;

#ifdef __cplusplus

class DECLSPEC_UUID("DA96493C-33A3-4D50-91A5-41DFF6287FBF")
CAvocadoDoc;
#endif
#endif /* __IAvocado_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif



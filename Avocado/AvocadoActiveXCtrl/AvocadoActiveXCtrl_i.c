

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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


#ifdef __cplusplus
extern "C"{
#endif 


#include <rpc.h>
#include <rpcndr.h>

#ifdef _MIDL_USE_GUIDDEF_

#ifndef INITGUID
#define INITGUID
#include <guiddef.h>
#undef INITGUID
#else
#include <guiddef.h>
#endif

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        DEFINE_GUID(name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8)

#else // !_MIDL_USE_GUIDDEF_

#ifndef __IID_DEFINED__
#define __IID_DEFINED__

typedef struct _IID
{
    unsigned long x;
    unsigned short s1;
    unsigned short s2;
    unsigned char  c[8];
} IID;

#endif // __IID_DEFINED__

#ifndef CLSID_DEFINED
#define CLSID_DEFINED
typedef IID CLSID;
#endif // CLSID_DEFINED

#define MIDL_DEFINE_GUID(type,name,l,w1,w2,b1,b2,b3,b4,b5,b6,b7,b8) \
        const type name = {l,w1,w2,{b1,b2,b3,b4,b5,b6,b7,b8}}

#endif !_MIDL_USE_GUIDDEF_

MIDL_DEFINE_GUID(IID, LIBID_AvocadoActiveXCtrlLib,0x1D49C79F,0x2D78,0x417F,0x91,0x23,0x8D,0xF5,0xC1,0x8E,0x0A,0x7F);


MIDL_DEFINE_GUID(IID, DIID__DAvocadoActiveXCtrl,0xB220DA40,0x0F9D,0x4B4F,0x98,0x78,0xAA,0x20,0x4F,0xC8,0x55,0xBE);


MIDL_DEFINE_GUID(IID, DIID__DAvocadoActiveXCtrlEvents,0x6976EC40,0x2A03,0x4DA5,0xA9,0x8B,0xF5,0xFC,0xE7,0x86,0x91,0xF7);


MIDL_DEFINE_GUID(CLSID, CLSID_AvocadoActiveXCtrl,0x3702B77E,0x8FD4,0x452E,0xAE,0x15,0x82,0xBE,0xEF,0x40,0xCB,0xD2);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif




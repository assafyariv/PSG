

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


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

MIDL_DEFINE_GUID(IID, LIBID_AvocadoActiveXLib,0x098DB52D,0x2AAE,0x499B,0xB9,0x59,0xA4,0x30,0xBA,0x0F,0xF3,0x57);


MIDL_DEFINE_GUID(IID, DIID__DAvocadoActiveX,0x6267760D,0x4EDC,0x430A,0xA9,0x4F,0x11,0x81,0x97,0x1A,0xBA,0x02);


MIDL_DEFINE_GUID(IID, DIID__DAvocadoActiveXEvents,0x050C9E59,0xADA3,0x440A,0x92,0xB4,0x59,0xAE,0x97,0x00,0x95,0x69);


MIDL_DEFINE_GUID(CLSID, CLSID_AvocadoActiveX,0x1EBAE592,0x7515,0x43C2,0xA6,0xF1,0xCD,0xEE,0xDF,0x3F,0xD8,0x2B);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif




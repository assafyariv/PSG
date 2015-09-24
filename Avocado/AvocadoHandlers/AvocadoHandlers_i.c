

/* this ALWAYS GENERATED file contains the IIDs and CLSIDs */

/* link this file in with the server and any clients */


 /* File created by MIDL compiler version 7.00.0555 */
/* at Sat May 18 21:23:50 2013
 */
/* Compiler settings for AvocadoHandlers.idl:
    Oicf, W1, Zp8, env=Win64 (32b run), target_arch=AMD64 7.00.0555 
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

MIDL_DEFINE_GUID(IID, IID_IPreview,0x9F036E58,0x474A,0x47FD,0x96,0x00,0x94,0x49,0x52,0xAA,0x67,0x99);


MIDL_DEFINE_GUID(IID, IID_IThumbnail,0x0CC320A4,0xE4DF,0x4A09,0x96,0xF4,0x94,0x37,0x05,0x86,0x89,0x3D);


MIDL_DEFINE_GUID(IID, IID_ISearch,0xB23E0090,0x60FC,0x4759,0xA9,0x06,0x90,0x8C,0x86,0xF1,0x94,0x4F);


MIDL_DEFINE_GUID(IID, LIBID_AvocadoHandlersLib,0x0518B3F7,0xC4B5,0x4F2C,0xA4,0x20,0x01,0x52,0xFF,0xE3,0xD3,0x76);


MIDL_DEFINE_GUID(CLSID, CLSID_Preview,0x9CA7FB05,0xE893,0x4127,0x9B,0xCB,0xA5,0x33,0x4D,0xE7,0xC7,0xEF);


MIDL_DEFINE_GUID(CLSID, CLSID_Thumbnail,0x5FDC1221,0x46DC,0x4F78,0x82,0x28,0xE0,0x11,0x01,0xA8,0x86,0xCE);


MIDL_DEFINE_GUID(CLSID, CLSID_Search,0xD6EBC255,0xEA82,0x45CB,0x9E,0x32,0x61,0x4C,0x0A,0xAD,0x58,0x14);

#undef MIDL_DEFINE_GUID

#ifdef __cplusplus
}
#endif




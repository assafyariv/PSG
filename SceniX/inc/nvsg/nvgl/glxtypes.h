// Copyright NVIDIA Corporation 2002-2005
// TO THE MAXIMUM EXTENT PERMITTED BY APPLICABLE LAW, THIS SOFTWARE IS PROVIDED
// *AS IS* AND NVIDIA AND ITS SUPPLIERS DISCLAIM ALL WARRANTIES, EITHER EXPRESS
// OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY
// AND FITNESS FOR A PARTICULAR PURPOSE.  IN NO EVENT SHALL NVIDIA OR ITS SUPPLIERS
// BE LIABLE FOR ANY SPECIAL, INCIDENTAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
// WHATSOEVER (INCLUDING, WITHOUT LIMITATION, DAMAGES FOR LOSS OF BUSINESS PROFITS,
// BUSINESS INTERRUPTION, LOSS OF BUSINESS INFORMATION, OR ANY OTHER PECUNIARY LOSS)
// ARISING OUT OF THE USE OF OR INABILITY TO USE THIS SOFTWARE, EVEN IF NVIDIA HAS
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES 

#pragma once
  
  /**  This file holds the typedefs for GLX specific functions,
    *  which may not be type-defined in the Linux GLX header files.
    *  Even a typedef redefine is valid, and will generate a compiler
    *  error if it does not match the previous definition.
    */

// these types are not defined in linux gl headers
typedef const char * (GLAPIENTRYP PFNGLXQUERYEXTENSIONSSTRINGPROC) (Display *dpy, int screen );

typedef GLXContext   (GLAPIENTRYP PFNGLXCREATECONTEXT) (Display *dpy, XVisualInfo *vis, GLXContext shareList, Bool direct);
typedef void         (GLAPIENTRYP PFNGLXDESTROYCONTEXT) (Display *dpy, GLXContext ctx);
typedef GLXContext   (GLAPIENTRYP PFNGLXGETCURRENTCONTEXT) (void);
typedef Bool         (GLAPIENTRYP PFNGLXMAKECONTEXTCURRENTPROC) (Display *display, GLXDrawable draw, GLXDrawable read, GLXContext ctx);
typedef GLXDrawable  (GLAPIENTRYP PFNGLXGETCURRENTREADDRAWABLEPROC) (void);

typedef GLXPbuffer   (GLAPIENTRYP PFNGLXCREATEGLXPBUFFERSGIXPROC) (Display *dpy, GLXFBConfig config,
                                                                   unsigned int width,
                                                                   unsigned int height,
                                                                   const int *attrib_list);
typedef void         (GLAPIENTRYP PFNGLXDESTROYGLXPBUFFERSGIXPROC) (Display *dpy, GLXPbuffer pbuf);       
typedef void         (GLAPIENTRYP PFNGLXQUERYGLXPBUFFERSGIXPROC) (Display *dpy, GLXPbuffer pbuf,
                                                                  int attribute, unsigned int *value);      

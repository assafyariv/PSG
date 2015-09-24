// Copyright NVIDIA Corporation 2002-2011
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
/** \file */

#if !defined( DOXYGEN_IGNORE )    //  no need to document the version header
//---------------------------------------------------------------------------
// Version Number
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
// - This is the ONLY place to edit the version number.
// - There must be no unnecessary leading zeros in the numbers.
//   (e.g.: don't use '02' - use '2' instead)
//- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
#define NVSG_VER_MAIN        7    // marketing main version
#define NVSG_VER_AUX         3    // marketing auxiliary version

#define NVSG_VER_MAJOR       9
#define NVSG_VER_MINOR       3

#if !defined(NVSG_VER_BUILD)
#define NVSG_VER_BUILD       4
#endif

//#define NVSG_ALPHA
//#define NVSG_BETA

// MaKe STRing helper macro
#define MKSTR(s)    MKSTR_i(s)
#define MKSTR_i(s)  MKSTR_ii(s)
#define MKSTR_ii(s) #s

// conditionally add a leading zero to the single parts of the version string
#define NVSG_VER_MAIN_STR           MKSTR(NVSG_VER_MAIN)
#define NVSG_VER_AUX_STR            MKSTR(NVSG_VER_AUX)
#define NVSG_VER_MAJOR_STR          MKSTR(NVSG_VER_MAJOR)
#define NVSG_VER_MINOR_STR          MKSTR(NVSG_VER_MINOR)
#define NVSG_VER_BUILD_STR          MKSTR(NVSG_VER_BUILD)

#define SDK_VENDOR "NVIDIA"
#define SDK_NAME   "SceniX"

// internal version string
#define _VER_STR   NVSG_VER_MAIN_STR "."\
                   NVSG_VER_AUX_STR "."\
                   NVSG_VER_MAJOR_STR "."\
                   NVSG_VER_MINOR_STR "."\
                   NVSG_VER_BUILD_STR

// no need to update these
//
#ifdef NVSG_ALPHA
  #if !defined(NDEBUG)
    #define VERSION_STR   _VER_STR " alpha" " (DEBUG)"
  #else  //DEBUG
    #define VERSION_STR   _VER_STR " alpha" 
  #endif //DEBUG
#else
  #ifdef NVSG_BETA
    #if !defined(NDEBUG)
      #define VERSION_STR   _VER_STR " beta (DEBUG)"
    #else  //DEBUG
      #define VERSION_STR   _VER_STR " beta"
    #endif //DEBUG
  #else  //BETA
    #if !defined(NDEBUG)
      #define VERSION_STR   _VER_STR " (DEBUG)"
    #else  //DEBUG
      #define VERSION_STR   _VER_STR
    #endif //DEBUG
  #endif //BETA
#endif

#define NVSG_COPYRIGHT	 SDK_VENDOR" "SDK_NAME" Version "VERSION_STR"\n� Copyright 2011 NVIDIA Corporation\n";

#endif  //  DOXYGEN_IGNORE
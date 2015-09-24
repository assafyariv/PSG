// Copyright NVIDIA Corporation 2002-2007
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

#include "nvsgcommon.h"

namespace nvsg 
{

// NVSG type specifier
enum
{
  NVSG_UNSUPPORTED_TYPE = ~0,
  NVSG_BYTE = 0,
  NVSG_UNSIGNED_BYTE,
  NVSG_SHORT,
  NVSG_UNSIGNED_SHORT,
  NVSG_INT,
  NVSG_UNSIGNED_INT,
  NVSG_FLOAT,
  NVSG_DOUBLE
};

/*! \brief Converts a given type into a symbolic constant representing a SceniX type */
template <typename T> struct Type2EnumType        { enum { type = NVSG_UNSUPPORTED_TYPE }; };
template <> struct Type2EnumType<char>            { enum { type = NVSG_BYTE }; };
template <> struct Type2EnumType<unsigned char>   { enum { type = NVSG_UNSIGNED_BYTE }; };
template <> struct Type2EnumType<short>           { enum { type = NVSG_SHORT }; };
template <> struct Type2EnumType<unsigned short>  { enum { type = NVSG_UNSIGNED_SHORT }; };
template <> struct Type2EnumType<int>             { enum { type = NVSG_INT }; };
template <> struct Type2EnumType<unsigned int>    { enum { type = NVSG_UNSIGNED_INT }; };
template <> struct Type2EnumType<float>           { enum { type = NVSG_FLOAT }; };
template <> struct Type2EnumType<double>          { enum { type = NVSG_DOUBLE }; };

/*! \brief Yields the size in bytes of the respective type 
 * \param type Indicates a symbolic constant representing a SceniX type.
 *  NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT, NVSG_UNSIGNED_SHORT,
 *  NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted values.
 * \return The function yields the size in bytes of the type indicated by \a type. */
NVSG_API unsigned int sizeOfType(unsigned int type);
NVSG_API bool isIntegerType(unsigned int type);
NVSG_API bool isFloatingPointType(unsigned int type);

}

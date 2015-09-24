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
/** @file */

#include "nvsgcommon.h"
#include "nvutil/BitMask.h"

// NBF is independent of packing. However, this forces
// the compile-time asserts below to fire on inconsistencies
#pragma pack(push, 1)

// convenient aliases for used built-in types
typedef char                    byte_t;   //!< Specifies an 8-bit signed type.
typedef unsigned char           ubyte_t;  //!< Specifies an 8-bit unsigned type.
typedef short                   short_t;  //!< Specifies a 16-bit signed integer type
typedef unsigned short          ushort_t; //!< Specifies a 16-bit unsigned integer type
typedef int                     int_t;    //!< Specifies a 32-bit signed integer type.
typedef unsigned int            uint_t;   //!< Specifies a 32-bit unsigned integer type.
#ifdef LINUX
# if __WORDSIZE == 64           // defined indirectly through stdint.h
// avoid a conflict with GNU stdint.h on Linux64
// note: long is a 64-bit type on Linux64, while it is 32bit on Win64
// Linux64
typedef unsigned long           uint64_t; //!< Specifies a 64-bit unsigned integer type.
# else
// Linux32
typedef unsigned long long      uint64_t; //!< Specifies a 64-bit unsigned integer type.
# endif
#else
// Win32 and Win64
typedef unsigned long long      uint64_t; //!< Specifies a 64-bit unsigned integer type.
#endif
typedef float                   float2_t[2]; //!< Specifies a 2-component float vector.
typedef float                   float3_t[3]; //!< Specifies a 3-component float vector.
typedef float                   float4_t[4]; //!< Specifies a 4-component float vector.
typedef float                   float44_t[4][4];  //!< Specifies a 4x4-component float matrix.

#define PADDING(n) PADDING_i(n,__LINE__)        //!< Convenient macro to add padding bits, part one of three
#define PADDING_i(n,l) PADDING_ii(n,l)          //!< Convenient macro to add padding bits, part two of three
#define PADDING_ii(n,l) ubyte_t padding##l[n]   //!< Convenient macro to add padding bits, part three of three

// NBF version
const ubyte_t NBF_VER_MAJOR  =  0x4D; //!< NBF major version number
const ubyte_t NBF_VER_MINOR  =  0x03; //!< NBF version compatibility level
const ubyte_t NBF_VER_BUGFIX =  0x00; //!< NBF version bugfix level
  
// constants specifying a certain byte order
const ubyte_t NBF_LITTLE_ENDIAN = 0x00; //!< Specifies little endian byte order
const ubyte_t NBF_BIG_ENDIAN    = 0x01; //!< Specifies big endian byte order

// convenient helper types

//! The byteArray_t structure specifies how an array of bytes is stored in a .NBF file.
struct byteArray_t
{
  uint_t  numBytes;               //!< Specifies the number of bytes in the array.
  uint_t  bytes;                  //!< Specifies the file offset to the byte data.
};

//! The str_t structure specifies how a string is stored in a .NBF file.
struct str_t 
{ 
  uint_t      numChars;           //!< Specifies the number of characters in the actual string,
                                  //!< not including the terminating null character.
  uint_t      chars;              //!< Specifies the file offset to the string characters.
};
NVSG_CTASSERT_MODULO_BYTESIZE(str_t,4);   //!< Compile-time assert on size of structure

//! The sstr_t structure specifies how a small string is stored in a .NBF file.
/** \note A small string is limited to 65535 characters, including the terminating 0 */
struct sstr_t
{
  ushort_t    numChars;           //!< Specifies the number of characters in the actual small string,
                                  //!< not including the terminating null character.
  uint_t      chars;              //!< Specifies the file offset to the string characters.
};

//! The vertexAttrib_t structure specifies how vertex attributes are stored in a .NBF file
/** Objects of type vertexAttrib_t alway are considered in conjunction with NBFVertexAttributeSet objects. */
struct vertexAttrib_t
{
  uint_t      size;               //!< Specifies the number of coordinates per vertex.
  uint_t      type;               //!< Symbolic constant indicating the data type of each coordinate.
  uint_t      numVData;           //!< Specifies the number of vertex data stored at offset \a vdata.
  uint_t      vdata;              //!< Specifies the file offset to the raw vertex data
};
NVSG_CTASSERT_MODULO_BYTESIZE(vertexAttrib_t,4);    //!< Compile-time assert on size of structure

//! The indexSet_t structure specifies how primitive indices are stored in a .NBF file
/** Objects of type indexSet_t are always considered in conjunction with NBFPrimitive objects. */
struct indexSet_t
{
  uint_t      dataType;               //!< Data type
  uint_t      primitiveRestartIndex;  //!< Primitive Restart Index
  uint_t      numberOfIndices;        //!< Number of indices in buffer
  uint_t      idata;                  //!< the index data
};
NVSG_CTASSERT_MODULO_BYTESIZE(indexSet_t,4);    //!< Compile-time assert on size of structure


//! The texCoordSet_t structure specifies how a texture coordinate set is stored in a .NBF file.
/** Texture coordinate sets, in this context, need to be considered in conjunction with NBFVertexAttributeSet objects. */
struct texCoordSet_t 
{
  uint_t      numTexCoords;       //!< Specifies the number of texture coordinates contained in the actual set.
  uint_t      coordDim;           //!< Specifies the dimension, in terms of float, of the contained texture coordinates.
                                  //!< Texture coordinates can be either one, two, three, or four dimensional.
  uint_t      texCoords;          //!< Specifies the file offset to the contained texture coordinates. 
};
NVSG_CTASSERT_MODULO_BYTESIZE(texCoordSet_t,4);   //!< Compile-time assert on size of structure

//! The geometrySet_t structure specifies how a geometry set is stored in a .NBF file.
/** Geometry sets, in this context, need to be considered in conjunction with NBFGeoNode objects. */
struct geometrySet_t
{
  uint_t      stateSet;           //!< Specifies the file offset to the corresponding NBFStateSet object.
  uint_t      numDrawables;       //!< Specifies the number of this geometry's NBFDrawable objects.
  uint_t      drawables;          //!< Specifies the file offset to the offsets to the NBFDrawable objects. 
};
NVSG_CTASSERT_MODULO_BYTESIZE(geometrySet_t,4);   //!< Compile-time assert on size of structure

//! The indexList_t structure specifies how an index set is stored in a .NBF file.
/** Index sets, in this context, need to be considered in conjunction with NBFStrippedPrimitiveSet objects. */
struct indexList_t
{
  uint_t      numIndices;         //!< Specifies the number of indices in the actual index set.
  uint_t      indices;            //!< Specifies the file offset to the indices. As specified by the NBF format,
                                  //!< a index is a 32-bit unsigned integer value.
};
NVSG_CTASSERT_MODULO_BYTESIZE(indexList_t,4);    //!< Compile-time assert on size of structure

//! The meshSet_t structure specifies how an mesh set is stored in a .NBF file.
/** Mesh sets, in this context, need to be considered in conjunction with NBFMeshedPrimitiveSet objects. */
struct meshSet_t
{
  uint_t      width;              //!< Specifies the width of the mesh
  uint_t      height;             //!< Specifies the height of the mesh
  uint_t      indices;            //!< Specifies the file offset to the indices. As specified by the NBF format,
  //!< a index is a 32-bit unsigned integer value.
};
NVSG_CTASSERT_MODULO_BYTESIZE(meshSet_t,4);   //!< Compile-time assert on size of structure

//! The texBinding_t structure specifies how a texture binding is stored in a .NBF file.
/** Texture bindings, in this context, need to be considered in conjunction with 
  * NBFTextureAttribute and NBFTextureAttributeItem objects. */
struct texBinding_t
{
  uint_t      texUnit;            //!< Specifies the texture unit where the actual NBFTextureAttributeItem object is bound to.
  uint_t      texAttribItem;      //!< Specifies the file offset to the corresponding NBFTextureAttributeItem object.
};
NVSG_CTASSERT_MODULO_BYTESIZE(texBinding_t,4);    //!< Compile-time assert on size of structure

//! The texImage_t structure specifies how a texture image is stored in a .NBF file.
/** Texture images are considered in conjunction with NBFTextureAttributeItem objects. */
struct texImage_t
{
  uint_t      flags;              //!< Creation flags.
  str_t       file;               //!< Specifies the filename of the image file in case the image is from a file.
  // the following are only relevant in case the image is not from a file but from a image data lump.
  uint_t      width;              //!< Specifies the width of the texture in pixels.
  uint_t      height;             //!< Specifies the height of the texture in pixels.
  uint_t      depth;              //!< Specifies the depth of the texture in pixels.
  uint_t      target;             //!< texture target.
  PADDING(8);                     //!< Padding bits to ensure offset of scene is on a 4-byte boundary, regardless of packing.
  uint_t      pixelFormat;        //!< Specifies the format of the pixel data. 
  uint_t      dataType;           //!< Specifies the type of the pixel data.
  uint_t      pixels;             //!< Specifies the file offset to the raw pixel data.
};
NVSG_CTASSERT_MODULO_BYTESIZE(texImage_t,4);    //!< Compile-time assert on size of structure

//! The trafo_t structure specifies how a transformation is stored in a .NBF file.
struct trafo_t
{
  float4_t    orientation;        //!< Specifies the orientational part of the transformation.
  float3_t    scaling;            //!< Specifies the scaling part of the transformation.
  float3_t    translation;        //!< Specifies the translational part of the transformation.
  float3_t    center;             //!< Specifies the center of rotation of the transformation.
  float4_t    scaleOrientation;   //!< Specifies the scale orientational part of the transformation.
};
NVSG_CTASSERT_MODULO_BYTESIZE(trafo_t,4);   //!< Compile-time assert on size of structure

//! The tweak_t structure specifies how a tweakable parameter is stored in a .NBF file.
/** Tweakables need to be considered in conjunction with NBFCgFx objects. */
struct tweak_t
{
  str_t       name;               //!< Specifies the tweakable's name as it appears in the effect source.

  // type flags
  enum {
    TYPE_STRUCT = BIT0
  , TYPE_ARRAY  = BIT1
  , TYPE_FLOAT  = BIT2
  , TYPE_INT    = BIT3
  , TYPE_BOOL   = BIT4
  , TYPE_STRING = BIT5
  };
  uint_t      type;               //!< Specifies the type of the tweakable (array, float,...)
  uint_t      numValues;          //!< Specifies the number of values stored. For a float4-typed parameter, for example, this is 4.
  union
  {
    uint_t    floats;             //!< Specifies the file offset to a non-string value. Note that this offset is not a valid offset for string types! Non-string values internally always are stored in terms of floats.
    str_t     str;                //!< Specifies the string value in case of a string parameter. 
  };                              //!< Specifies the value of the tweakable, either as floats or as a string.
 
  // the following is relevant only for arrays and structs (nesting tweaks) 
  uint_t   numTweaks;         //!< In case of an array parameter this specifies the array size. In case of a struct parameter this specifies the number of members.
  uint_t   tweaks;            //!< Specifies the file offset to either the array elements or struct members.
};
NVSG_CTASSERT_MODULO_BYTESIZE(tweak_t,4);   //!< Compile-time assert on size of structure

//! The sampler_t structure specifies how a sampler parameter is stored in a .NBF file. 
/** Samplers need to be considered in conjunction with NBFCgFx objects. */
struct sampler_t
{
  str_t       name;               //!< Specifies the sampler's name as it appears in the effect source.
  uint_t      texImg;             //!< Specifies the file offset to the corresponding texture image.
};

//! The texture_t structure specifies how an RTFx texture parameter is stored in a .NBF file. 
/** Textures need to be considered in conjunction with NBFRTFx objects. */
struct texture_t
{
  str_t       file;               //!< Specifies the texture's file name
  str_t       bufferName;         //!< Specifies the texture's buffer name
  str_t       format;             //!< Specifies the texture's format string
  // MMM - do we need creation flags here?
  uint_t      texImg;             //!< Specifies the file offset to the corresponding texture image.
};
NVSG_CTASSERT_MODULO_BYTESIZE(texture_t,4);   //!< Compile-time assert on size of structure

//! The domain_t structure specifies how an RTFx domain parameter is stored in a .NBF file. 
/** Domains need to be considered in conjunction with NBFRTFx objects. */
struct domain_t
{
  str_t       name;        //!< Specifies the domains's name
  str_t       entryPoint;  //!< Specifies the domains's entry point
  uint_t      raytype;     //!< Specifies the ray type for this value
  PADDING(4);              //!< ensure modulo 4
};
NVSG_CTASSERT_MODULO_BYTESIZE(domain_t,4);   //!< Compile-time assert on size of structure

//! The parameter_t structure specifies how a parameter is stored in a .NBF file.
/** Tweakables need to be considered in conjunction with NBFRTFx objects. */
struct parameter_t
{
  str_t       name;               //!< Specifies the tweakable's name as it appears in the effect source.

  // type flags
  enum {
    PTYPE_BUFFER  = BIT0
  , PTYPE_TEXTURE = BIT1
  , PTYPE_FLOAT   = BIT2
  , PTYPE_INT     = BIT3
  , PTYPE_UINT    = BIT4
  };
  uint_t type;              //!< Specifies the type of the tweakable
  uint_t numComponents;     //!< Specifies the number of values stored. For a float4-typed parameter, for example, this is 4.
  uint_t param;            //!< Specifies the file offset to either the elements.
};
NVSG_CTASSERT_MODULO_BYTESIZE(parameter_t,4);   //!< Compile-time assert on size of structure

//! The bone_t structure specifies how a bone is stored in a .NBF file. 
/** Bones need to be considered in conjunction with the skin_t structure. */
struct bone_t
{
  float       weight;             //!< Specifies a weight factor to be applied to the initial vertices and normals of the
                                  //!< actual skin.
  uint_t      animation;          //!< Specifies the file offset to the animation to be applied to the initial vertices 
                                  //!< and normals of the actual skin.
};
NVSG_CTASSERT_MODULO_BYTESIZE(bone_t,4);    //!< Compile-time assert on size of structure

//! The skin_t structure specifies how a skin is stored in a .NBF file.
/** Skins need to be considered in conjunction with NBFSkinnedTriangles objects. */
struct skin_t
{
  uint_t      numBones;           //!< Specifies the number of bones describing the actual skin.
  uint_t      bones;              //!< Specifies the file offset to the bones describing the actual skin.
                                  //!< The way bones are stored in a .NBF file is specified by the bone_t structure. 
  float3_t    vertex;             //!< Specifies the initial vertex for the actual skin.
  float3_t    normal;             //!< Specifies the initial normal for the actual skin.
};
NVSG_CTASSERT_MODULO_BYTESIZE(skin_t,4);    //!< Compile-time assert on size of structure

//! The keyVariant_t structure specifies how a pair of a VariantKey and a StateVariant is stored in a .NBF file.
struct keyVariant_t
{
  uint_t      key;                //!< Specifies the key of this pair
  uint_t      variant;            //!< Specifies the offset to an NBFStateVariant in the .NBF file.
};
NVSG_CTASSERT_MODULO_BYTESIZE(keyVariant_t,4);    //!< Compile-time assert on size of structure

//! The plane_t structure specifies how a clipping plane is stored in an .NBF file.
struct plane_t
{
  uint_t      active;             //!< Specifies if this plane is active
  float3_t    normal;             //!< Specifies the normal of the plane
  float       offset;             //!< Specifies the offset of the plane
};
NVSG_CTASSERT_MODULO_BYTESIZE(plane_t,4);   //!< Compile-time assert on size of structure

//! The switchMask_t structure specifies how a SwitchMask is stored in a .NBF file.
struct switchMask_t
{
  uint_t        maskKey;          //!< Specifies the key to identify this mask
  uint_t        numChildren;      //!< Specifies the number of active children stored with this mask
  uint_t        children;         //!< Specifies the file offset to the zero-based indexes referencing the active children
};
NVSG_CTASSERT_MODULO_BYTESIZE(switchMask_t,4);    //!< Compile-time assert on size of structure

//! Unique NBF Object Codes
/** Each concrete NBFObject type is assigned to a unique NBF object code. 
  * This code is a 32-bit unsigned integer value, stored at offset 0, of each concrete NBFObject.
  * The purpose of the unique 'per-object' code is to provide a Load-Time Type Information (LTTI) 
  * to resolve concrete NBFObjects while loading NBF files. */   
enum
{
  // scene object 
  NBF_UNKNOWN                     = 0x00000000  //!< Unknown, and hence, invalid object code.
  // animation objects (0x100 - 0x1FF)
, NBF_TRAFO_ANIMATION           = 0x00000100  //!< Identifies an NBFTrafoAnimation object.
, NBF_VNVECTOR_ANIMATION                      //!< obsolete
, NBF_VERTEX_ATTRIBUTE_ANIMATION              //!< Identifies an NBFVertexAttributeAnimation object.
, NBF_INDEX_ANIMATION                         //!< Identifies an NBFIndexAnimation object.
  // framed animation descriptions
, NBF_FRAMED_ANIMATION          = 0x00000120
, NBF_FRAMED_TRAFO_ANIMATION_DESCRIPTION      //!< Identifies an NBFFramedTrafoAnimationDescription object.
, NBF_FRAMED_VNVECTOR_ANIMATION               //!< Obsolete
, NBF_FRAMED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION //!< Identifies an NBFFramedVertexAttributeAnimationDescription object.
, NBF_FRAMED_INDEX_ANIMATION_DESCRIPTION      //!< Identifies an NBFFramedIndexAnimationDescription
  // interpolated animation descriptions
, NBF_INTERPOLATED_ANIMATION    = 0x00000140
, NBF_LINEAR_INTERPOLATED_TRAFO_ANIMATION_DESCRIPTION //!< Identifies an NBFKeyFramedTrafoAnimationDescription object.
, NBF_LINEAR_INTERPOLATED_VNVECTOR_ANIMATION  //!< Obsolete
, NBF_LINEAR_INTERPOLATED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION
  // camera objects (0x200 - 0x2FF)
, NBF_CAMERA                    = 0x00000200
, NBF_MONO_CAMERA
, NBF_JITTER_CAMERA
, NBF_SIMPLE_CAMERA
, NBF_PARALLEL_CAMERA                         //!< Identifies an NBFParallelCamera object.
, NBF_PERSPECTIVE_CAMERA                      //!< Identifies an NBFPerspectiveCamera object.
, NBF_STEREO_CAMERA  
, NBF_MATRIX_CAMERA                           //!< Identifies an NBFMatrixCamera object.
  // drawable objects (0x300 - 0x3FF) 
, NBF_DRAWABLE                  = 0x00000300
, NBF_VERTEX_ATTRIBUTE_SET                    //!< Identifies an NBFVertexAttributeSet object.
, NBF_TRIANGLES                               //!< Identifies an NBFIndependentPrimitiveSet object.
, NBF_ANIMATED_TRIANGLES                      //!< Obsolete
, NBF_SKINNED_TRIANGLES                       //!< Identifies an NBFSkinnedTriangles object.
, NBF_TRISTRIPS                               //!< Identifies an NBFStrippedPrimitiveSet object.
, NBF_QUADS                                   //!< Identifies an NBFIndependentPrimitiveSet object (support since v0.3).
, NBF_ANIMATED_QUADS                          //!< Obsolete
, NBF_QUADSTRIPS                              //!< Identifies an NBFStrippedPrimitiveSet object (support since v0.4)
, NBF_LINES                                   //!< Identifies an NBFIndependentPrimitiveSet object (support since v0.5)
, NBF_LINESTRIPS                              //!< Identifies an NBFStrippedPrimitiveSet object (support since v0.5)
, NBF_TRIFANS                                 //!< Identifies an NBFTriFans object (support since v0.6)
, NBF_POINTS                                  //!< Identifies an NBFPoints object (support since v0.6)
, NBF_QUADMESHES                              //!< Identifies an NBFQuadMeshes object
, NBF_ANIMATED_VERTEX_ATTRIBUTE_SET           //!< Identifies an NBFAnimatedVertexAttributeSet
, NBF_SKIN                                    //!< Identifies an NBFSkin
, NBF_PATCHES                                 //!< Identifies an NBFPatches
, NBF_QUAD_PATCHES                            //!< Identifies an NBFQuadPatches
, NBF_QUAD_PATCHES_4X4                        //!< Identifies an NBFQuadPatches4x4
, NBF_RECT_PATCHES                            //!< Identifies an NBFRectPatches
, NBF_TRI_PATCHES                             //!< Identifies an NBFTriPatches
, NBF_TRI_PATCHES_4                           //!< Identifies an NBFTriPatches4
, NBF_PRIMITIVE                               //!< Identifies an NBFPrimitive
  // node objects (0x400 - 0x4FF)
, NBF_NODE                      = 0x00000400
, NBF_GEO_NODE                                //!< Identifies a NBFGeoNode object.
, NBF_GROUP                                   //!< Identifies a NBFGroup object.
, NBF_LOD                                     //!< Identifies a NBFLOD object.
, NBF_SWITCH                                  //!< Identifies a NBFSwitch object.
, NBF_TRANSFORM                               //!< Identifies a NBFTransform object.
, NBF_ANIMATED_TRANSFORM                      //!< Identifies a NBFAnimatedTransform object.
, NBF_LIGHT_SOURCE
, NBF_DIRECTED_LIGHT                          //!< Identifies an NBFDirectedLight object.
, NBF_POINT_LIGHT                             //!< Identifies an NBFPointLight object.
, NBF_SPOT_LIGHT                              //!< Identifies an NBFSpotLight object.
, NBF_BILLBOARD                               //!< Identifies an NBFBillboard object.
, NBF_VOLUME_NODE                             //!< Identifies an NBFVolumeNode object.
, NBF_FLIPBOOK_ANIMATION                      //!< Identifies an NBFFlipbookAnimation object.
  // state set objects (0x500 - 0x5FF)
, NBF_STATE_SET                 = 0x00000500  //!< Identifies an NBFStateSet object.
, NBF_STATE_VARIANT                           //!< Identifies an NBFStateVariant object.
, NBF_STATE_PASS                              //!< Identifies an NBFStatePass object.
  // state attribute objects (0x600 - 0x6FF)
, NBF_STATE_ATTRIBUTE           = 0x00000600
, NBF_CGFX                                    //!< Identifies an NBFCgFx object.
, NBF_MATERIAL                                //!< Identifies an NBFMaterial object.
, NBF_FACE_ATTRIBUTE                          //!< Identifies an NBFFaceAttribute object.
, NBF_TEXTURE_ATTRIBUTE                       //!< Identifies an NBFTextureAttribute object.
, NBF_TEXTURE_ATTRIBUTE_ITEM                  //!< Identifies an NBFTextureAttributeItem object.
, NBF_LINE_ATTRIBUTE                          //!< Identifies an NBFLineAttribute object.
, NBF_POINT_ATTRIBUTE                         //!< Identifies an NBFPointAttribute object.
, NBF_BLEND_ATTRIBUTE                         //!< Identifies an NBFBlendAttribute object.
, NBF_DEPTH_ATTRIBUTE                         //!< Identifies an NBFDepthAttribute object.
, NBF_ALPHA_TEST_ATTRIBUTE                    //!< Identifies an NBFAlphaTestAttribute object.
, NBF_LIGHTING_ATTRIBUTE                      //!< Identifies an NBFLightingAttribute object.
, NBF_UNLIT_COLOR_ATTRIBUTE                   //!< Identifies an NBFUnlitColorAttribute object.
, NBF_STENCIL_ATTRIBUTE                       //!< Identifies an NBFStencilAttribute.
, NBF_RTFX                                    //!< Identifies an NBFRTFx object.
, NBF_RTBUFFER_ATTRIBUTE                      //!< Identifies an NBFRTBufferAttribute object.
, NBF_RTFX_SCENE_ATTRIBUTE                    //!< Identifies an NBFRTFxSceneAttribute object.
, NBF_RTFX_PROGRAM                            //!< Identifies an NBFRTFxProgram object.
  // custom objects (>=0x700)
, NBF_CUSTOM_OBJECT             = 0x00000700  //!< Custom objects must not have codes lower than this.
};

//! The NBFHeader structure represents the NBF header format.
/** The NBFHeader structure is the primary location where NBF specifics are stored.\n
  * For a valid NBF file, the NBFHeader structure is stored at file offset 0. Note that,
  * except for the NBFHeader object, a file offset of 0 indicates an invalid file offset!\n
  * This structure mainly serves as validation and compatibility checks for verification 
  * purposes. It also maintains the file offset to the contained NBFScene object, which  
  * represents a scene in the context of computer graphics. */ 
struct NBFHeader
{
  // signature
  byte_t      signature[4];       //!< A 4-byte signature identifying the file as a valid NBF file. The bytes are "#NBF".
  // NBF version
  ubyte_t     nbfMajorVersion;    //!< Specifies the major part of the NBF version used to save the file.
  ubyte_t     nbfMinorVersion;    //!< Specifies the minor part (compatibility level) of the NBF version used to save the file.
  ubyte_t     nbfBugfixLevel;     //!< Specifies the bugfix level of the NBF version used to save the file. This is optional
                                  //!< information, as a bugfix level does not influence compatibility issues, and hence 
                                  //!< must not be taken into account for compatibility checks.
  // SceniX version
  ubyte_t     nvsgMajorVersion;   //!< Specifies the major part of the SceniX version the content of this file is compatible to.
  ubyte_t     nvsgMinorVersion;   //!< Specifies the minor part of the SceniX version the content of this file is compatible to.
  ubyte_t     nvsgBugfixLevel;    //!< Specifies the bugfix level of the SceniX version. This is optional information, as a 
                                  //!< bugfix level does not influence compatibility issues, and hence must not be taken 
                                  //!< into account for compatibility checks.
  // Reserved bytes 
  ubyte_t     reserved[16];       //!< Reserved bytes for future extensions.
  // Date
  ubyte_t     dayLastModified;    //!< Specifies the day (1-31) of last modification.
  ubyte_t     monthLastModified;  //!< Specifies the month (1-12) of last modification.
  ubyte_t     yearLastModified[2]; //!< Specifies the year of last modification.
  // Time stamp
  ubyte_t     secondLastModified; //!< Specifies the second (0-59) of last modification. 
  ubyte_t     minuteLastModified; //!< Specifies the minute (0-59) of last modification.
  ubyte_t     hourLastModified;   //!< Specifies the hour (0-23) of last modification.
  // endianess
  ubyte_t     byteOrder;          //!< Specifies the byte order used to save the contained data.
                                  //!< A value of 0 implies little-endian byte order, a value of 1 implies big-endian byte order.
                                  //!< It is more convenient to use the symbolic constants NBF_LITTLE_ENDIAN and NBF_BIG_ENDIAN here.
  PADDING(2);                     //!< Padding bits to ensure offset of scene is on a 4-byte boundary, regardless of packing.
  // scene object
  uint_t      scene;              //!< Specifies the file offset to the contained NBFScene object.
  // optional view state
  uint_t      viewState;          //!< Specifies the file offset to an optional NBFViewState object. 
                                  //!< An offset of 0 indicates that no NBFViewState object is available in this file. 
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFHeader,4);   //!< Compile-time assert on size of structure

//! The NBFScene structure represents a scene in the context of computer graphics.
/** A valid NBF file always contains one - and only one - NBFScene object. 
  * The file offset to this NBFScene object is specified within the NBFHeader structure. */
struct NBFScene
{
  float3_t    ambientColor;             //!< Specifies the global ambient color to be used for rendering.
  float4_t    backColor;                //!< Specifies the scene's RGBA background color used with rendering.
  uint_t      backImg;                  //!< Specifies the file offset to the back image object
  uint_t      numCameras;               //!< Specifies the number of the scene's NBFCamera objects.
  uint_t      cameras;                  //!< Specifies the file offset to the offsets of the scene's NBFCamera objects.
  uint_t      numCameraAnimations;      //!< Specifies the number of the scene's NBFTrafoAnimation objects (camera animations). 
  uint_t      cameraAnimations;         //!< Specifies the offset to the offsets of the scene's NBFTrafoAnimation objects (camera animations).
  uint_t      numberOfAnimationFrames;  //!< For animated scenes, this specifies the number of animation frames.
  uint_t      root;                     //!< Specifies the file offset to the scene's root node, which always is of a NBFNode derived type.
  uint_t      numObjectLinks;           //!< Specifies the number of objects links in the scene
  uint_t      objectLinks;              //!< Specifies the file offset to the scenes's object links
  uint_t      numAttributes;            //!< Specifies the number of attributes in the scene
  uint_t      attributes;               //!< Specifies the file offset to the scenes's attributes
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFScene,4);    //!< Compile-time assert on size of structure

//! The NBFViewState represents an optional view state used to render the scene.
/** The file offset to an optional NBFViewState object is specified within the NBFHeader structure. */
struct NBFViewState
{
  uint_t      objectCode;           //!< Specifies the object code of the actual object. The object code is unique per object type! 
  uint_t      camera;               //!< Specifies the offset to the NBFCamera object to be used for viewing.
  ubyte_t     isStereo;             //!< Indicates whether the view is in stereo mode.
  ubyte_t     isStereoAutomatic;    //!< Indicates whether eye distance is automatically adjusted in stereo mode.
  ubyte_t     isAutoClipPlanes;     //!< Indicates if automatic generation of clipping planes is enabled.
  PADDING(1);                       //!< Padding bits to ensure offset of next member is on a 4-byte boundary, regardless of packing.
  PADDING(20); //!< discontinuing support for jitter
  float       stereoAutomaticFactor;//!< Specifies the automatic eye distance adjustment factor in stereo mode.
  float       stereoEyeDistance;    //!< Specifies the stereo eye distance used if the view is in stereo mode.
  float       targetDistance;       //!< Specifies the target distance to the projection plane.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFViewState,4);    //!< Compile-time assert on size of structure

//! The NBFObject structure represents general object data. 
struct NBFObject
{
  uint_t      objectCode;         //!< Specifies the object code of the actual object. The object code is unique per object type! 
  ubyte_t     isShared;           //!< Indicates whether the data of the actual object is shared among different objects.
                                  //!< A value of 1 indicates that this object's data is shared, whereas a value of 0 indicates
                                  //!< that this object's data is not shared.

  PADDING(3);                     //!< Padding bits to keep compatibility to earlier versions.

  uint64_t    objectDataID;       //!< A unique 64-bit value to identify shared object data while loading.
  uint_t      sourceObject;       //!< Specifies the file offset to the source object in case of data sharing. 
                                  //!< A file offset of 0 always indicates that no source object is available for the actual object.
  uint_t       objectName;        //!< Specifies the offset to an optional name. A 0-offset implies no name.
                                  //!< The name is stored as a str_t object.
  uint_t       objectAnno;        //!< Specifies the offset to an optional annotation that can be specified for an object.
                                  //!< A 0-offset implies no annotation. An annotation is stored as a str_t object.
  uint_t       hints;             //!< Hints vars for node, user, object
};
// NOTE: Because of the uint64_t member objectDataID above, which is 8-byte aligned, 
// we need to ensure the size of NBFObject is fixed - that is, independent of whatever
// the compilers actual packing value might be! We achieve this by making the size of 
// NBFObject a multiple of 8 bytes (see compile time assert below).  
NVSG_CTASSERT_MODULO_BYTESIZE(NBFObject,8);   //!< Compile-time assert on size of structure

//! The NBFIndexAnimation represents an animation that will be applied to uint_t objects.
/** A NBFIndexAnimation serves as base class only and needs to be considered in conjunction with
  * NBFFramedAnimationDescription.\n
  * Valid object code for a NBFIndexAnimation is NBF_FRAMED_INDEX_ANIMATION_DESCRIPTION. Further concrete object
  * codes valid for NBFIndexAnimation objects are subject to future extensions of the NBF format. */
struct NBFIndexAnimation
{
  uint_t      indices;             //!< Specifies the file offset to the uint_t objects the animation will be applied to.
                                   //!< The actual number of uint_t objects is specified by the corresponding animation
                                   //!< type, which is NBFFramedAnimationDescription.
  PADDING(4);        //!< Padding bits to ensure the size of NBFIndexAnimation is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFIndexAnimation,8);   //!< Compile-time assert on size of structure

//! The NBFTrafoAnimation represents an animation that will be applied to trafo_t objects.
/** A NBFTrafoAnimation serves as base class only and needs to be considered in conjunction
  * with either NBFFramedAnimationDescription or NBFKeyFramedAnimationDescription.\n
  * Valid object codes for a NBFTrafoAnimation are NBF_FRAMED_TRAFO_ANIMATION_DESCRIPTION, 
  * and NBF_LINEAR_INTERPOLATED_TRAFO_ANIMATION_DESCRIPTION. Further concrete object codes valid for
  * NBFTrafoAnimation objects are subject to future extensions of the NBF format. */
struct NBFTrafoAnimation
{
  uint_t      trafos;             //!< Specifies the file offset to the trafo_t objects the animation will be applied to.
                                  //!< The actual number of trafo_t objects is specified by the corresponding animation
                                  //!< type, which is either NBFFramedAnimationDescription or NBFKeyFramedAnimationDescription.
  PADDING(4);        //!< Padding bits to ensure the size of NBFTrafoAnimation is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFTrafoAnimation,8);   //!< Compile-time assert on size of structure

//! The NBFVertexAttributeAnimation represents an animation that will be applied to vertexAttrib_t objects.
/** A NBFVertexAttributeAnimation serves as base class only and needs to be considered in conjunction
  * with either NBFFramedAnimationDescription or NBFKeyFramedAnimationDescription.\n
  * Valid object codes for a NBFVertexAttributeAnimation are NBF_FRAMED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION, 
  * and NBF_LINEAR_INTERPOLATED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION. Further concrete object codes valid for
  * NBFVertexAttributeAnimation objects are subject to future extensions of the NBF format. */
struct NBFVertexAttributeAnimation
{
  uint_t      attribs;            //!< Specifies the file offset to the vertexAttrib_t objects the animation will be applied to.
                                  //!< The actual number of vertexAttrib_t objects is specified by the corresponding animation
                                  //!< type, which is either NBFFramedAnimationDescription or NBFKeyFramedAnimationDescription.
  PADDING(4);        //!< Padding bits to ensure the size of NBFVertexAttributeAnimation is a multiple of 8, regardless of packing.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFVertexAttributeAnimation,8);   //!< Compile-time assert on size of structure

//! The NBFAnimationState structure represents the common anchor of all animation structures.
/** A NBFAnimationState serves as a base class only and needs to be considered in conjunction with either
  * NBFFramedAnimationDescription or NBFKeyFramedAnimationDescription. */
struct NBFAnimationState : public NBFObject
{
  uint_t  loopCount;              //!< Specifies the number of times to run through the Animation.
  uint_t  speed;                  //!< Specifies the number if steps per advance.
  bool    forward;                //!< Specifies to run the Animation forward or backward.
  bool    swinging;               //!< Specifies to run the Animation looping or swinging (ping-pong).
  PADDING(6);        //!< Padding bits to ensure the size of NBFAnimationState is a multiple of 8, regardless of packing.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFAnimationState,8);   //!< Compile-time assert on size of structure

/*! \brief The NBFAnimation structure represents an animation object. */
struct NBFAnimation : public NBFAnimationState
{
  uint_t  description;            //!< Specifies the file offset to the animation description
  PADDING(4);        //!< Padding bits to ensure the size of NBFAnimation is a multiple of 8, regardless of packing.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFAnimation,8);   // Compile-time assert on size of structure

/*! \brief The NBFAnimationDescription structure represents an animation description object. */
struct NBFAnimationDescription : public NBFObject
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFAnimationDescription,8);   //!< Compile-time assert on size of structure

//! The NBFFramedAnimationDescription structure represents a framed animation.
/** A NBFFramedAnimationDescription serves as base class only and needs to be considered in conjunction
  * with either NBFIndexAnimation, NBFTrafoAnimation or NBFVertexAttributeAnimation.\n
  * Concrete object codes valid for a NBFFramedAnimationDescription are NBF_FRAMED_INDEX_ANIMATION_DESCRIPTION,
  * NBF_FRAMED_TRAFO_ANIMATION_DESCRIPTION, and NBF_FRAMED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION. Further concrete object
  * codes valid for NBFFramedAnimationDescription objects are subject to future extensions of the NBF format. */
struct NBFFramedAnimationDescription
{
  uint_t      numSteps;           //!< Specifies the number of animation steps.\n 
                                  //!< In conjunction with a NBFIndexAnimation, this corresponds to the number of
                                  //!< uint_t objects stored with a NBFTrafoAnimation.\n
                                  //!< In conjunction with a NBFTrafoAnimation, this corresponds to the number of
                                  //!< trafo_t objects stored with a NBFTrafoAnimation.\n
                                  //!< In conjunction with a NBFVertexAttributeAnimation, this corresponds to the number of
                                  //!< VertexAttribute sets stored with a NBFVertexAttributeAnimation.
  PADDING(4);         //!< Padding bits to ensure the size of NBFFramedIndexAnimationDescription is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFFramedAnimationDescription,4);    //!< Compile-time assert on size of structure

//! The NBFFramedIndexAnimationDescription structure represents a framed animation that will be applied to uint_t objects.
/** A NBFFramedIndexAnimationDescription is a concrete animation type. It publicly inherits from NBFIndexAnimation and
  * NBFFramedAnimationDescription. The object code for a NBFFramedIndexAnimationDescription is NBF_FRAMED_INDEX_ANIMATION_DESCRIPTION. */ 
struct NBFFramedIndexAnimationDescription : public NBFAnimationDescription
                                          , public NBFFramedAnimationDescription
                                          , public NBFIndexAnimation
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFFramedIndexAnimationDescription,8);   //!< Compile-time assert on size of structure

//! The NBFFramedTrafoAnimationDescription structure represents a framed animation that will be applied to trafo_t objects.
/** A NBFFramedTrafoAnimationDescription is a concrete animation type. It publicly inherits from NBFTrafoAnimation and
  * NBFFramedAnimationDescription. The object code for a NBFFramedTrafoAnimationDescription is NBF_FRAMED_TRAFO_ANIMATION_DESCRIPTION. */ 
struct NBFFramedTrafoAnimationDescription : public NBFAnimationDescription
                                          , public NBFFramedAnimationDescription
                                          , public NBFTrafoAnimation
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFFramedTrafoAnimationDescription,8);   //!< Compile-time assert on size of structure

//! The NBFFramedVertexAttributeAnimationDescription structure represents a framed animation that will be applied to vertexAttrib_t objects.
/** A NBFFramedVertexAttributeAnimationDescription is a concrete animation type. It publicly inherits from NBFVertexAttributeAnimation and
  * NBFFramedAnimationDescription. The object code for a NBFFramedVertexAttributeAnimationDescription is NBF_FRAMED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION. */ 
struct NBFFramedVertexAttributeAnimationDescription : public NBFAnimationDescription
                                                    , public NBFFramedAnimationDescription
                                                    , public NBFVertexAttributeAnimation
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFFramedVertexAttributeAnimationDescription,8);   //!< Compile-time assert on size of structure

//! The NBFKeyFramedAnimationDescription structure represents a interpolated animation.
/** A NBFKeyFramedAnimationDescription serves as base class only and needs to be considered in conjunction
  * with either NBFTrafoAnimation or NBFVertexAttributeAnimation.\n
  * Concrete object codes valid for a NBFKeyFramedAnimationDescription are
  * NBF_LINEAR_INTERPOLATED_TRAFO_ANIMATION_DESCRIPTION and
  * NBF_LINEAR_INTERPOLATED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION. Further concrete object codes valid for
  * NBFKeyFramedAnimationDescription objects are subject to future extensions of the NBF format. */
struct NBFKeyFramedAnimationDescription
{
  uint_t      numKeys;            //!< Specifies the number of key frames
  uint_t      keys;               //!< Specifies the file offset to the key frames. As specified in the NBF format,
                                  //!< a key is a 32-bit unsigned integer value.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFKeyFramedAnimationDescription,4);    //!< Compile-time assert on size of structure

//! The NBFKeyFramedTrafoAnimationDescription structure represents a interpolated animation that will be applied to trafo_t objects.
/** A NBFKeyFramedTrafoAnimationDescription is a concrete animation type. It publicly inherits from
  * NBFTrafoAnimation and NBFKeyFramedAnimationDescription.\n 
  * The object code valid for a NBFKeyFramedTrafoAnimationDescription is
  * NBF_LINEAR_INTERPOLATED_TRAFO_ANIMATION_DESCRIPTION. Further object codes valid for
  * NBFKeyFramedTrafoAnimationDescription objects are subject to future extensions of the NBF format. */
struct NBFKeyFramedTrafoAnimationDescription : public NBFAnimationDescription
                                             , public NBFKeyFramedAnimationDescription
                                             , public NBFTrafoAnimation
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFKeyFramedTrafoAnimationDescription,8);   //!< Compile-time assert on size of structure

/*! The NBFLinearInterpolatedTrafoAnimationDescription structure represents an animation description object for linear interpolated transformations.
 *  \remarks The object code for a NBFLinearInterpolatedTrafoAnimationDescription is NBF_LINEAR_INTERPOLATED_TRAFO_ANIMATION_DESCRIPTION. */
struct NBFLinearInterpolatedTrafoAnimationDescription : public NBFKeyFramedTrafoAnimationDescription
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFLinearInterpolatedTrafoAnimationDescription,8);   //!< Compile-time assert on size of structure

//! The NBFKeyFramedVertexAttributeAnimationDescription structure represents a interpolated animation that will be applied to vertexAttrib_t objects.
/** A NBFKeyFramedVertexAttributeAnimationDescription is a concrete animation type. It publicly inherits from NBFVertexAttributeAnimation and
  * NBFKeyFramedAnimationDescription.\n 
  * The object code valid for a NBFKeyFramedVertexAttributeAnimationDescription is NBF_LINEAR_INTERPOLATED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION. Further object codes
  * valid for NBFKeyFramedVertexAttributeAnimationDescription objects are subject to future extensions of the NBF format. */
struct NBFKeyFramedVertexAttributeAnimationDescription : public NBFAnimationDescription
                                                       , public NBFKeyFramedAnimationDescription
                                                       , public NBFVertexAttributeAnimation
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFKeyFramedVertexAttributeAnimationDescription,8);   //!< Compile-time assert on size of structure

/*! The NBFLinearInterpolatedVertexAttributeAnimationDescription structure represents an animation description object for linear interpolated vertex attributes.
 *  \remarks The object code for a NBFLinearInterpolatedVertexAttributeAnimationDescription is NBF_LINEAR_INTERPOLATED_VERTEX_ATTRIBUTE_ANIMATION_DESCRIPTION. */
struct NBFLinearInterpolatedVertexAttributeAnimationDescription : public NBFKeyFramedVertexAttributeAnimationDescription
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFLinearInterpolatedVertexAttributeAnimationDescription,8);   //!< Compile-time assert on size of structure

//! The NBFCamera represents a camera.
/** A NBFCamera serves as base class only.\n
  * Concrete object codes valid for a NBFCamera are NBF_PARALLEL_CAMERA
  * and NBF_PERSPECTIVE_CAMERA. Further object codes valid for a NBFCamera
  * are subject to future extensions for the NBF format. */
struct NBFCamera : public NBFObject
{
  uint_t      numHeadLights;    //!< Specifies the number of headlights attached.
  uint_t      headLights;       //!< Specifies the file offset to the offsets to the attached headlight objects.
                                //!< Headlights are of type NBFLightSource. 
  float3_t    upVector;         //!< Specifies the camera's normalized up vector.
  float3_t    position;         //!< Specifies the actual position of camera in world space.
  float3_t    direction;        //!< Specifies the normalized direction for the camera to look along.
  float       focusDist;        //!< Specifies the distance to the projection plane.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFCamera,8);   //!< Compile-time assert on size of structure

/*! \brief The NBFFrustumCamera structure is the base of the NBFParalleleCamera and the NBFPerspectiveCamera. */
struct NBFFrustumCamera : public NBFCamera
{
  float       farDist;          //!< Specifies the distance from the actual camera position to the far clipping plane.
  PADDING(4);      //!< Padding bits to ensure offset of windowSize is on a 4-byte boundary, regardless of packing
  float       nearDist;         //!< Specifies the distance from the actual camera position to the near clipping plane.
  float2_t    windowOffset;     //!< Specifies the world-relative offset from the viewing reference point to the center 
                                //!< of the viewing window.
  float2_t    windowSize;       //!< Specifies the world-relative size of the viewing window. Whereas the x-component of 
                                //!< of the vector specifies the width, and the y-component of the vector specifies the height.
  PADDING(4);        //!< Padding bits to ensure the size of NBFCamera is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFFrustumCamera,8);   //!< Compile-time assert on size of structure

//! The NBFParallelCamera represents a parallel camera.
/** A NBFParallelCamera is a concrete camera type. 
  * The object code for a NBFParallelCamera is NBF_PARALLEL_CAMERA. */
struct NBFParallelCamera : public NBFFrustumCamera
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFParallelCamera,8);   //!< Compile-time assert on size of structure

//! The NBFPerspectiveCamera represents a perspective camera.
/** A NBFPerspectiveCamera is a concrete camera type. 
  * The object code for a NBFPerspectiveCamera is NBF_PERSPECTIVE_CAMERA. */
struct NBFPerspectiveCamera : public NBFFrustumCamera
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFPerspectiveCamera,8);    //!< Compile-time assert on size of structure

/*! \brief The NBFMatrixCamera structure represents a general matrix camera.
 *  \remarks The object code for a NBFMatrixCamera is NBF_MATRIX_CAMERA. */
struct NBFMatrixCamera : public NBFCamera
{
  float44_t   projection;         //!< Specifies the projection matrix
  float44_t   inverseProjection;  //!< Specifies the inverse projection matrix
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFMatrixCamera,8);    //!< Compile-time assert on size of structure

//! The NBFDrawable represents any kind of a drawable object.
/** A NBFDrawable serves a base class only.\n
  * Concrete object codes valid for a NBFDrawable are NBF_TRIANGLES, NBF_ANIMATED_TRIANGLES, 
  * NBF_SKINNED_TRIANGLES, NBF_TRIFANS, and NBF_TRISTRIPS. Further object codes valid for a NBFDrawable
  * are subject to future extensions of the NBF format. */
struct NBFDrawable : public NBFObject
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFDrawable,8);   //!< Compile-time assert on size of structure

//! The NBFVertexAttributeSet structure represents a set of vertex attributes.
/** A NBFVertexAttributeSet maintains a full set of geometry
  * data (vertex attributes) that is used by all topology classes. */
struct NBFVertexAttributeSet : public NBFObject
{
  uint_t      enableFlags;            //!< Specifies which of the 16 vertex attributes is enabled.
  uint_t      normalizeEnableFlags;   //!< Specifies for which of the 16 vertex attributes normalization is enabled.
  vertexAttrib_t  vattribs[16];       //!< Specifies the 16 vertex attributes.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFVertexAttributeSet,8);   //!< Compile-time assert on size of structure

/*! \brief The NBFAnimatedVertexAttributeSet structure represents a set of animated vertex attributes.
 *  \remarks A NBFAnimatedVertexAttributeSet derives from NBFVertexAttributeSet, and holds up to 16
 *  animations, one for each of the up to 16 vertex attributes in a NBFVertexAttributeSet, and a
 *  field of flags to specify if a vertex attribute has to be normalized, for example after
 *  interpolating. */
struct NBFAnimatedVertexAttributeSet : public NBFVertexAttributeSet
{
  uint_t      animations[16];     //!< Specifies the file offset to the animation objects, one
                                  //!< offset for each vertex attribute.
  uint_t      normalizeFlags;     //!< Flags that determine if a vertex attribute has to be
                                  //!< normalized after interpolation
  PADDING(4);                     //!< Padding bits to ensure the size of NBFAnimatedVertexAttributeSet is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFAnimatedVertexAttributeSet,8);   //!< Compile-time assert on size of structure

/*! \brief The NBFSkin structure represents a set of skin information for a vertex attribute.
 *  \remarks An NBFSkin derives from NBFObject, and holds the bindShapeMatrix, the number
 *  of influences (numCounts), the number of indices/weights per influence (counts), the indices
 *  into the array of joints and the corresponding weights, the skinning types for each vertex
 *  attribute, the number of joints, and the offsets to the inverseBindMatrices and the joints. */
struct NBFSkin : public NBFObject
{
  float44_t   bindShapeMatrix;    //!< Specifies the bindShapeMatrix
  uint_t      numCounts;          //!< Specifies the number of influences (counts).
  uint_t      counts;             //!< Specifies the file offset to the indices/weights per vertex.
  uint_t      indices;            //!< Specifies the file offset to the indices.
  uint_t      weights;            //!< Specifies the file offset to the weights.
  ubyte_t     skinningType[16];   //!< Specifies the skinning type for each vertex attribute.
  uint_t      numJoints;          //!< Specifies the number of joints.
  uint_t      inverseBindMatrices;  //!< Specifies the file offset to the inverseBindMatrices.
  uint_t      joints;             //!< Specifies the file offset to the joints.
  uint_t      bindPose;           //!< Specifies the file offset to the bind pose vertex attribute set.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFSkin,8);

//! The NBFPrimitiveSet structure represents a geometry with an NBFVertexAttributeSet.
/** A NBFPrimitiveSet holds the offset to an NBFVertexAttributeSet. */
struct NBFPrimitiveSet : public NBFDrawable
{
  uint_t      vertexAttributeSet; //!< Specifies the file offset to the vertex attribute set.
  uint_t      skin;               //!< Specifies the file offset to the skin (from version 0x3e on!)
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFPrimitiveSet,8);   //!< Compile-time assert on size of structure

//! The NBFPrimitive structure represents a geometry with an NBFVertexAttributeSet, and possibly an index set
/** A NBFPrimitive holds the offset to an NBFVertexAttributeSet, and possibly an index set */
struct NBFPrimitive : public NBFDrawable
{
  uint_t      primitiveType;      //!< Specified the primitive type (unit because it may be a user enum)
  uint_t      elementOffset;      //!< Specifies the element offset
  uint_t      elementCount;       //!< Specifies the element count
  uint_t      instanceCount;      //!< Specified the instance count
  uint_t      verticesPerPatch;   //!< Specified the num verts per patch
  uint_t      renderFlags;        //!< Specified the rendering flags
  uint_t      vertexAttributeSet; //!< Specifies the file offset to the vertex attribute set.
  indexSet_t  indexSet;           //!< Specifies the index set, if any
  uint_t      skin;               //!< Specifies the file offset to the skin
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFPrimitive,8);   // Compile-time assert on size of structure

//! The NBFIndependentPrimitiveSet structure represents a set of independent primitives.
/** A NBFIndependentPrimitiveSet is an abstract topology class derived from NBFPrimitiveSet.
  * It is used with NBF_LINES, NBF_POINTS, NBF_QUADS, and NBF_TRIANGLES. */
struct NBFIndependentPrimitiveSet : public NBFPrimitiveSet
{
  uint_t      numIndices;         //!< Specifies the number of contained indices
  uint_t      indices;            //!< Specifies the file offset to the Independent objects.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFIndependentPrimitiveSet,8);    //!< Compile-time assert on size of structure

//! The NBFMeshedPrimitiveSet structure represents a mesh set.
/** A NBFMeshedPrimitiveSet is an abstract topology class derived from NBFPrimitiveSet. 
* Valid object codes for a NBFMeshedPrimitiveSet are NBF_QUADMESHES. */
struct NBFMeshedPrimitiveSet : public NBFPrimitiveSet
{
  uint_t      numMeshes;          //!< Specifies the number of meshes
  uint_t      meshes;             //!< Specifies the file offset to the meshes. 
  //!< Strips are stored as indexList_t objects.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFMeshedPrimitiveSet,8);   //!< Compile-time assert on size of structure

//! The NBFStrippedPrimitiveSet structure represents a strip set.
/** A NBFStrippedPrimitiveSet is an abstract topology class derived from NBFPrimitiveSet. 
  * Valid object codes for a NBFStrippedPrimitiveSet are NBF_TRIFANS, NBF_TRISTRIPS, and NBF_QUADSTRIPS. */
struct NBFStrippedPrimitiveSet : public NBFPrimitiveSet
{
  uint_t      numStrips;          //!< Specifies the number of strips
  uint_t      strips;             //!< Specifies the file offset to the strips. 
                                  //!< Strips are stored as indexList_t objects.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFStrippedPrimitiveSet,8);   //!< Compile-time assert on size of structure

//! The NBFSkinnedTriangles structure represents skin animated triangles.
/** A NBFSkinnedTriangles is a concrete topology class derived from NBFIndependentPrimitiveSet. 
* The object code for a NBFSkinnedTriangles is NBF_SKINNED_TRIANGLES. */
struct NBFSkinnedTriangles : public NBFIndependentPrimitiveSet
{
  uint_t      numSkins;           //!< Specifies the number of contained skins
  uint_t      skins;              //!< Specifies the file offset to the corresponding skin_t objects.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFSkinnedTriangles,8);   //!< Compile-time assert on size of structure

//! The NBFNode structure represents a general node.
/** A NBFNode serves as base class only. Concrete object codes valid for a NBFNode are
  * NBF_GEO_NODE, NBF_LOD, NBF_SWITCH, NBF_TRANSFORM, NBF_ANIMATED_TRANSFORM,
  * NBF_DIRECTED_LIGHT, NBF_POINT_LIGHT, NBF_SPOT_LIGHT, and NBF_VOLUME_NODE. Further concrete
  * object codes valid for a NBFNode are subject to future extensions of the NBF format. */
struct NBFNode : public NBFObject
{
  str_t       annotation;         //!< Specifies an optional annotation string. Unused since v61.2!
  PADDING(6);                     //!< Padding bits to ensure the size of NBFStateAttribute is a multiple of 4, regardless of packing.
  PADDING(2);                     //!< Two more padding for backwards compat
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFNode,8);   //!< Compile-time assert on size of structure

//! The NBFGeoNode structure represents a geometry node.
/** The object code for a NBFGeoNode is NBF_GEO_NODE. */
struct NBFGeoNode : public NBFNode
{
  uint_t      numStateSets;       //!< Specifies the number of contained StateSets.
  uint_t      geometrySets;       //!< Specifies the file offset to the geometry sets. Geometry sets are stored as geometrySet_t objects.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFGeoNode,8);    //!< Compile-time assert on size of structure

//! The NBFGroup structure represents a group node.
/** A NBFGroup serves as base class only. Concrete object codes valid for a NBFGroup are
  * NBF_LOD, NBF_SWITCH, NBF_TRANSFORM, and NBF_ANIMATED_TRANSFORM. Further concrete object
  * codes valid for a NBFGroup are subject to future extensions of the NBF format. */
struct NBFGroup : public NBFNode
{
  uint_t      numChildren;        //!< Specifies the number of maintained children.
  uint_t      children;           //!< Specifies the file offset to the offsets to the maintained children.
                                  //!< NBFGroup's children always are of NBFNode-derived types.
  uint_t      numClipPlanes;      //!< Specifies the number of clipping planes.
  uint_t      clipPlanes;         //!< Specifies the file offset to the clipping planes
  uint_t      numLightSource;     //!< Specifies the number of light sources.
  uint_t      lightSources;       //!< Specifies the fie offset to the offsets to the light sources.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFGroup,8);    //!< Compile-time assert on size of structure

//! The NBFLOD structure represents a 'Level Of Detail' group node.
/** The object code for a NBFLOD is NBF_LOD. */
struct NBFLOD : public NBFGroup
{
  float3_t    center;             //!< Specifies the center point used for distance calculations.
  uint_t      numRanges;          //!< Specifies the number of contained ranges.
  uint_t      ranges;             //!< Specifies the file offset to the ranges. 
                                  //!< Ranges are stored as 32-bit floating point numbers.
  PADDING(4);        //!< Padding bits to ensure the size of NBFLOD is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFLOD,8);    //!< Compile-time assert on size of structure

//! The NBFSwitch structure represents a switch group node.
/** The object code for a NBFSwitch is NBF_SWITCH. */
struct NBFSwitch : public NBFGroup
{
  uint_t      activeMaskKey;      //!< Specifies the key of the active mask
  uint_t      numMasks;           //!< Specifies the number of masks stored at offset masks
  uint_t      masks;              //!< Specifies the file offset to the masks stored as switchMask_t objects
  PADDING(4);        //!< Padding bits to ensure the size of NBFSwitch is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFSwitch,8);   //!< Compile-time assert on size of structure

//! The NBFBillboard structure represents a billboard group node.
/** The object code for a NBFBillboard is NBF_BILLBOARD. */
struct NBFBillboard: public NBFGroup
{
  float3_t  rotationAxis;         //!< Specifies the axis to rotate the Billboard around
  ubyte_t   alignment;            //!< Specifies the alignment (axis, viewer, or screen aligned)
  PADDING(3);        //!< Padding bits to ensure the size of NBFBillboard is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFBillboard,8);    //!< Compile-time assert on size of structure

//! The NBFTransform structure represents a transform group node.
/** The object code for a NBFTransform is NBF_TRANSFORM. */
struct NBFTransform : public NBFGroup
{
  trafo_t             trafo;      //!< Specifies the transformation of the NBFTransform.
  PADDING(4);        //!< Padding bits to ensure the size of NBFTransform is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFTransform,8);    //!< Compile-time assert on size of structure

//! The NBFFlipbookAnimation structure represents an animated group node.
/** The object code for a NBFFlipbookAnimation is NBF_FLIPBOOK_ANIMATION. */
struct NBFFlipbookAnimation : public NBFGroup
{
  uint_t      animation;          //!< Specifies the file offset to the NBFIndexAnimation object
                                  //!< to be applied to the transform group node.
  PADDING(4);        //!< Padding bits to ensure the size of NBFFlipbookAnimation is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFFlipbookAnimation,8);    //!< Compile-time assert on size of structure

//! The NBFAnimatedTransform structure represents an animated transform group node.
/** The object code for a NBFAnimatedTransform is NBF_ANIMATED_TRANSFORM. */
struct NBFAnimatedTransform : public NBFTransform
{
  uint_t      animation;          //!< Specifies the file offset to the NBFTrafoAnimation object
                                  //!< to be applied to the transform group node.
  PADDING(4);        //!< Padding bits to ensure the size of NBFAnimatedTransform is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFAnimatedTransform,8);    //!< Compile-time assert on size of structure

//! The NBFLightSource structure represents a light source node.
/** A NBFLightSource serves as base class only. Concrete object codes valid for 
  * a NBFLightSource are NBF_DIRECTED_LIGHT, NBF_POINT_LIGHT, and NBF_SPOT_LIGHT. */
struct NBFLightSource : public NBFObject
{
  float       intensity;          //!< Specifies the light intensity.
  float3_t    ambientColor;       //!< Specifies the ambient color term of the light.
  float3_t    diffuseColor;       //!< Specifies the diffuse color term of the light.
  float3_t    specularColor;      //!< Specifies the specular color term of the light.
  ubyte_t     castShadow;         //!< flag that determines if this light source creates shadows.
  ubyte_t     enabled;            //!< flag to indicate enabled state.
  PADDING(2);        //!< Padding bits to ensure the offset of the next struct member is on a 4-byte boundary.
  uint_t      animation;          //!< Specifies the file offset to an optional NBFTrafoAnimation object
                                  //!< to be applied to the light transform (orientation and translation).
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFLightSource,8);    //!< Compile-time assert on size of structure

// The NBFDirectedLight structure represents a directed light source.
/** The object code for a NBFDirectedLight is NBF_DIRECTED_LIGHT. */
struct NBFDirectedLight : public NBFLightSource
{
  float3_t direction; //!< Specifies the direction of the light source.
  PADDING(4);         //!< Padding bits to ensure the offset of the next struct member is on a 4-byte boundary.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFDirectedLight,8);    //!< Compile-time assert on size of structure

// The NBFPointLight structure represents a point light source.
/** The object code for a NBFPointLight is NBF_POINT_LIGHT. */
struct NBFPointLight : public NBFLightSource
{
  float3_t position;    //!< Specifies the position of the light source.
  float3_t attenuation; //!< Specifies the attenuation factors of the point light.
                        //!< The x-component of the vector specifies the constant term of the attenuation,
                        //!< the y-component of the vector specifies the linear term of the attenuation, and
                        //!< the z-component of the vector specifies the quadratic term of the attenuation.
};                        
NVSG_CTASSERT_MODULO_BYTESIZE(NBFPointLight,8);   //!< Compile-time assert on size of structure

// The NBFSpotLight structure represents a spot light.
/** The object code for a NBFSpotLight is NBF_SPOT_LIGHT. */
struct NBFSpotLight : public NBFLightSource
{
  float3_t position;        //!< Specifies the position of the light source.
  float3_t direction;       //!< Specifies the direction of the light source.
  float3_t attenuation;     //!< Specifies the attenuation factors of the spot light.
                            //!< The x-component of the vector specifies the constant term of the attenuation,
                            //!< the y-component of the vector specifies the linear term of the attenuation, and
                            //!< the z-component of the vector specifies the quadratic term of the attenuation.
  float    cutoffAngle;     //!< Specifies the angle between the axis of the cone, the light is emitted to, and
                            //!< a ray along the edge of the cone.
  float    falloffExponent; //!< Controls the intensity distribution inside the cone, the light is mitted to.
  PADDING(4);               //!< Padding bits to ensure the size of NBFSpotLight is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFSpotLight,8);    //!< Compile-time assert on size of structure

//! The NBFStateSet structure represents a set of pairs of VariantKeys and StateVariants.
/** The object code for a NBFStateSet is NBF_STATE_SET. */
struct NBFStateSet : public NBFObject
{
  uint_t activeKey;               //!< Specifies the currently active VariantKey
  uint_t numStateVariants;        //!< Specifies the number of contained pairs of VariantKey and StateVariant.
  uint_t keyStateVariantPairs;    //!< Specifies the file offset to the offsets to the keyVariant_t objects
  PADDING(4);               //!< Padding bits to ensure the size of NBFStateSet is a multiple of 8, regardless of packing.    
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFStateSet,8);   //!< Compile-time assert on size of structure

//! The NBFStateVariant structure represents a set of StatePasses
/** The object code for a NBFStateVariant is NBF_STATE_VARIANT. */
struct NBFStateVariant : public NBFObject
{
  uint_t      numStatePasses;     //!< Specifies the number of contained state passes.
  uint_t      statePasses;        //!< Specifies the file offset to the offsets to the NBFStatePass objects
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFStateVariant,8);   //!< Compile-time assert on size of structure

//! The NBFStatePass structure represents a set of heterogeneous NBFStateAttributes.
/** The object code for a NBFStatePass is NBF_STATE_PASS. */
struct NBFStatePass : public NBFObject
{
  uint_t      numStateAttribs;    //!< Specifies the number of contained state attributes.
  uint_t      stateAttribs;       //!< Specifies the file offset to the offsets to the NBFStateAttribute objects
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFStatePass,8);    //!< Compile-time assert on size of structure

//! The NBFStateAttribute structure represents a state attribute.
/** A NBFStateAttribute serves a base class only. Concrete object codes valid for a NBFStateAttribute
  * are NBF_BLEND_ATTRIBUTE, NBF_CGFX, NBF_RTFX, NBF_MATERIAL, NBF_FACE_ATTRIBUTE, and NBF_TEXTURE_ATTRIBUTE. */
struct NBFStateAttribute : public NBFObject
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFStateAttribute,8);   //!< Compile-time assert on size of structure

//! The NBFAlphaTestAttribute structure represents an alpha test state attribute in the NBF format.
/** The object code for a NBFAlphaTestAttribute is NBF_ALPHA_TEST_ATTRIBUTE. */
struct NBFAlphaTestAttribute : public NBFStateAttribute
{
  ubyte_t     alphaFunction; //!< Specifies the alpha function
  ubyte_t     enabledMSAlpha; //!< Specifies whether enabled during msalpha 
  PADDING(2); //!< Padding bits to ensure offset of threshold member is on a 4-byte boundary, regardless of packing
  float       threshold; //!< Specifies the threshold value
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFAlphaTestAttribute,8);   //!< Compile-time assert on size of structure

//! The NBFBlendAttribute structure represents a blend function.
/** The object code for a NBFBlendAttribute is NBF_BLEND_ATTRIBUTE. */
struct NBFBlendAttribute : public NBFStateAttribute
{
  ubyte_t     sourceFunction;       //!< Specifies the source blending function
  ubyte_t     destinationFunction;  //!< Specifies the destination blending function
  PADDING(6);                       //!< Padding bits to ensure size of NBFBlendAttribute is a multiple of 8
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFBlendAttribute,8);   //!< Compile-time assert on size of structure

//! The NBFCgFx structure represents an effect attribute.
/** The object code for a NBFCgFx is NBF_CGFX. */
struct NBFCgFx : public NBFStateAttribute
{
  PADDING(3);
  ubyte_t     fromFile;           //!< Specifies whether the effect's source is a file. If this is false, the source member below is interpreted as a code string.
  str_t       source;             //!< Holds either the filename of an effect file or a source code string. 
  uint_t      numTweaks;          //!< Specifies the number of tweakable effect parameters stored.
  uint_t      tweaks;             //!< Specifies the file offset to the tweakable effect paramters. Tweakables in the NBF format are stored as tweak_t objects.
  uint_t      numSamplers;        //!< Specifies the number of sampler parameters stored. 
  uint_t      samplers;           //!< Specifies the file offset t the sampler parameters. Samplers parameters a stored as sampler_t objects.
  str_t       currTech;           //!< Specifies the name of the current technique used with rendering.
  PADDING(4); //!< Padding bits to ensure the size of NBFCgFx is a multiple of 8, regardless of packing.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFCgFx,8);   //!< Compile-time assert on size of structure

//! An NBFRTFxParameter specifies on parameter in an RTFxBase.
struct NBFRTFxParameter
{
  str_t   name;                   //!< Specifies the name of the parameter
  PADDING(2);                     //!< Padding bits
  ubyte_t count;                  //!< Specifies the number of elements of the parameter
  ubyte_t type;                   //!< Specifies the type of the parameter
  union
  {
    byteArray_t data;             //!< Specifies the array of data, stored as bytes
    uint_t      buffer;           //!< Specifies the file offset of an RTBuffer
    uint_t      texture;          //!< Specifies the file offset of an RTTextureSampler
  };
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFRTFxParameter,4);   //!< Compile-time assert on size of structure

//! An NBFRTFxBase is a common base for both RTFx and RTFxSceneAttribute
struct NBFRTFxBase
{
  uint_t  numParameters;          //!< Specifies the number of parameters.
  uint_t  parameters;             //!< Specifies the file offset of the parameters.
  //  NOTE: no need to store the textures! They're resolved by the associated RTFxProgram!
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFRTFxBase,4);   //!< Compile-time assert on size of structure

//! The NBFRTFx structure represents an effect attribute.
/** The object code for a NBFRTFx is NBF_RTFX. */
struct NBFRTFx : public NBFStateAttribute, public NBFRTFxBase
{
  uint_t  program;                //!< Specifies the file offset of the RTFxProgram.
  PADDING(4);                     //!< Padding bits to ensure the size of NBFRTFx is a multiple of 8, regardless of packing.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFRTFx,8);   //!< Compile-time assert on size of structure

//! The NBFRTFxSceneAttribute structure represents the effect scene attribute.
/** The object code for a NBFRTFxSceneAttribute is NBF_RTFX_SCENE_ATTRIBUTE. */
typedef struct NBFRTFx NBFRTFxSceneAttribute;

//! The NBFRTFxProgram structure represents an RTFx program
/** The object code for a NBFRTFxProgram is NBF_RTFX_PROGRAM */
struct NBFRTFxProgram : public NBFObject
{
  PADDING(3);                     //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
  ubyte_t fromFile;               //!< Specifies if the effect is from a file or a string
  str_t   effect;                 //!< Specifies the effect file or source, depending on fromFile
  PADDING(4);                     //!< Padding bits to ensure size of NBFRTFxProgram is a multiple of 8
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFRTFxProgram,8);   // Compile-time assert on size of structure

//! The NBFDepthAttribute structure represents depth buffer settings.
/** The object code for a NBFDepthAttribute is NBF_DEPTH_ATTRIBUTE. */
struct NBFDepthAttribute : public NBFStateAttribute
{
  ubyte_t     depthFunction;        //!< Specifies the depth function
  ubyte_t     depthTestingEnabled;  //!< Specifies if depth testing is enabled
  ubyte_t     depthWritingEnabled;  //!< Specifies if depth writing is enabled
  ubyte_t     depthClampingEnabled; //!< Specifies if depth clamping is enabled
  PADDING(4);                       //!< Padding bits to ensure size of NBFBlendAttribute is a multiple of 8
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFDepthAttribute,8);   //!< Compile-time assert on size of structure

//! The NBFFaceAttribute structure represents a render mode.
/** The object code for a NBFFaceAttribute is NBF_FACE_ATTRIBUTE. */
struct NBFFaceAttribute : public NBFStateAttribute
{
  ubyte_t     cullMode;             //!< Specifies the face culling mode.
  ubyte_t     polygonOffsetEnabled; //!< Specifies if polygon offset should be enabled for rendering.
  PADDING(2);                       //!< Padding bits to ensure offset of polygonOffsetFactor is on a 4-byte boundary, regardless of packing
  float       polygonOffsetFactor;  //!< Specifies the scale factor to be used to create a variable depth offset
                                    //!< for a polygon.
  float       polygonOffsetUnits;   //!< Specifies a unit that is multiplied by a render API implementation-specific
                                    //!< value to create a constant depth offset.
  ubyte_t     twoSidedLighting;     //!< Specifies if two-sided lighting should be enabled for rendering.
  ubyte_t     frontFaceMode;        //!< Specifies the face mode (points/lines/faces) for front faces.
  ubyte_t     backFaceMode;         //!< Specifies the face mode (points/lines/faces) for back faces.
  ubyte_t     faceWindingCCW;       //!< Specifies if the face winding is counter clock wise
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFFaceAttribute,8);    //!< Compile-time assert on size of structure

//! The NBFLightingAttribute structure represents a render mode.
/** The object code for a NBFLightingAttribute is NBF_LIGHTING_ATTRIBUTE. */
struct NBFLightingAttribute : public NBFStateAttribute
{
  ubyte_t     enabled;    //!< Specifies if lighting is enabled or disabled.
  PADDING(3);             //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFLightingAttribute,4);    //!< Compile-time assert on size of structure

//! The NBFLineAttribute structure represents a render mode.
/** The object code for a NBFLineAttribute is NBF_LINE_ATTRIBUTE. */
struct NBFLineAttribute : public NBFStateAttribute
{
  ubyte_t     antiAliasing;         //!< Specifies if the lines are anti aliased.
  PADDING(3);                       //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
  uint_t      stippleFactor;        //!< Specifies the stipple factor
  uint_t      stipplePattern;       //!< Specifies the stipple pattern
  float       width;                //!< Specifies the line width
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFLineAttribute,8);    //!< Compile-time assert on size of structure

//! The NBFMaterial structure represents a material.
/** The object code for a NBFMaterial is NBF_MATERIAL. */
struct NBFMaterial : public NBFStateAttribute
{
  float3_t    frontAmbientColor;      //!< Specifies the ambient part of the front material color. 
  float3_t    frontDiffuseColor;      //!< Specifies the diffuse part of the front material color.
  float3_t    frontEmissiveColor;     //!< Specifies the emissive part of the front material color.
  float       frontIndexOfRefraction; //!< Specifies the index of refraction of the front material.
  float3_t    frontOpacityColor;      //!< Specifies the opacity per color channel of the front material.
  float3_t    frontReflectivityColor; //!< Specifies the reflectivity per color channel of the front material.
  float3_t    frontSpecularColor;     //!< Specifies the specular part of the front material color.
  float       frontSpecularExponent;  //!< Specifies the specular exponent of the front material color.
  float3_t    backAmbientColor;       //!< Specifies the ambient part of the back material color. 
  float3_t    backDiffuseColor;       //!< Specifies the diffuse part of the back material color.
  float3_t    backEmissiveColor;      //!< Specifies the emissive part of the back material color.
  float       backIndexOfRefraction;  //!< Specifies the index of refraction of the back material.
  float3_t    backOpacityColor;      //!< Specifies the opacity per color channel of the back material.
  float3_t    backReflectivityColor; //!< Specifies the reflectivity per color channel of the back material.
  float3_t    backSpecularColor;      //!< Specifies the specular part of the back material color.
  float       backSpecularExponent;   //!< Specifies the specular exponent of the back material color.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFMaterial,8);   //!< Compile-time assert on size of structure

//! The NBFPointAttribute structure represents a render mode.
/** The object code for a NBFPointAttribute is NBF_POINT_ATTRIBUTE. */
struct NBFPointAttribute : public NBFStateAttribute
{
  ubyte_t     antiAliasing;         //!< Specifies if the points are anti aliased.
  PADDING(3);                       //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
  float       size;                 //!< Specifies the point size
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFPointAttribute,8);   //!< Compile-time assert on size of structure

//! The NBFRTBufferAttribute structure represents a ray tracing buffer attribute.
/** The object code for a NBFRTBufferAttribute is NBF_RTBUFFER_ATTRIBUTE. */
struct NBFRTBufferAttribute : public NBFStateAttribute
{
  str_t       variableName;       //!< Name of variable to assign in RTFx
  uint_t      format;             //!< Specifies the buffer format
  uint_t      width;              //!< Specifies the width of the buffer
  uint_t      height;             //!< Specifies the height of the buffer
  uint_t      depth;              //!< Specifies the depth of the buffer
  uint_t      elementSize;        //!< Specifies the element size if format is RT_FORMAT_USER
  uint_t      buffer;             //!< Offset to the buffer data
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFRTBufferAttribute,8);   //!< Compile-time assert on size of structure

//! The NBFTextureAttribute structure represents a texture attribute.
/** The object code for a NBFTextureAttribute is NBF_TEXTURE_ATTRIBUTE. */
struct NBFTextureAttribute : public NBFStateAttribute
{
  uint_t      numBindings;        //!< Specifies the number of contained texture bindings.
  uint_t      bindings;           //!< Specifies the file offset to the texture bindings. Texture bindings in
                                  //!< the NBF format are stored as texBinding_t objects.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFTextureAttribute,8);   //!< Compile-time assert on size of structure

//! The NBFTextureAttributeItem structure represents a single texture object.
/** The object code for a NBFTextureAttributeItem is NBF_TEXTURE_ATTRIBUTE_ITEM. 
  * A NBFTextureAttributeItem needs to be consider in conjunction with a NBFTextureAttribute, 
  * which specifies the binding of texture objects. */
struct NBFTextureAttributeItem : public NBFObject
{
  uint_t        texImg;             //!< Specifies the file offset to the texture image object
  float4_t      texEnvColor;        //!< Specifies the texture environment color.
  uint_t        texEnvMode;         //!< Specifies the texture environment mode for the actual texture object. 
                                    //!< Valid modes are TEM_REPLACE, TEM_MODULATE, TEM_DECAL, TEM_BLEND, and TEM_ADD.
  uint_t        texEnvScale;        //!< Specifies the texture environment scale used with rasterization 
  uint_t        texWrapS;           //!< Specifies the wrap parameter for texture coordinate s. 
  uint_t        texWrapT;           //!< Specifies the wrap parameter for texture coordinate t.
  uint_t        texWrapR;           //!< Specifies the wrap parameter for texture coordinate r.
  uint_t        minFilter;          //!< Specifies the filter used with minimizing. 
                                    //!< Valid values are TFM_MIN_NEAREST, TFM_MIN_LINEAR, TFM_MIN_LINEAR_MIPMAP_LINEAR,
                                    //!< TFM_MIN_NEAREST_MIPMAP_NEAREST, TFM_MIN_NEAREST_MIPMAP_LINEAR, TFM_MIN_LINEAR_MIPMAP_NEAREST.
  uint_t        magFilter;          //!< Specifies the filter used with magnifying.
                                    //!< Valid values are TFM_MAG_NEAREST, and TFM_MAG_LINEAR.
  float4_t      texBorderColor;     //!< Specifies the texture border RGBA color.
  trafo_t       trafo;              //!< Specifies the texture transformation
  uint_t        texGenMode[4];      //!< Specifies the texture coordinate generation modes
  float4_t      texGenPlane[2][4];  //!< Specifies the texture coordinate generation planes
  float         maxAnisotropy;      //!< Maximum Anisotropy value.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFTextureAttributeItem,8);   // Compile-time assert on size of structure

/*! \brief The NBFUnlitColorAttribute structure represents the color for unlit objects.
 *  \remarks The object code for a NBFUnlitColorAttribute is NBF_UNLIT_COLOR_ATTRIBUTE. */
struct NBFUnlitColorAttribute : public NBFStateAttribute
{
  float4_t      color;              //!< Specifies the color for unlit objects
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFUnlitColorAttribute,8);    //!< Compile-time assert on size of structure

/*! \brief The NBFStencilAttribute structure represents a stencil attribute.
 *  \remarks The object code for a NBFStencilAttribute is NBF_STENCIL_ATTRIBUTE. */
struct NBFStencilAttribute : public NBFStateAttribute
{
  ubyte_t       testing;            //!< Specifies whether to test or not.
  ubyte_t       separate;           //!< Specifies whether two-sided variant or front values are used.
  PADDING(2);                       //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
  uint_t        writeMaskFront;     //!< Specifies the write mask for the front faces.
  uint_t        failFront;          //!< Specifies the stencil test fail operation for the front faces.
  uint_t        zfailFront;         //!< Specifies the depth test fail operation for the front faces.
  uint_t        zpassFront;         //!< Specifies the depth test pass operation for the front faces.
  uint_t        funcFront;          //!< Specifies the stencil function for the front faces.
  int_t         refValueFront;      //!< Specifies the reference value for the front faces.
  uint_t        valMaskFront;       //!< Specifies the value mask for the front faces.
  uint_t        writeMaskBack;      //!< Specifies the write mask for the back faces.
  uint_t        failBack;           //!< Specifies the stencil test fail operation for the back faces.
  uint_t        zfailBack;          //!< Specifies the depth test fail operation for the back faces.
  uint_t        zpassBack;          //!< Specifies the depth test pass operation for the back faces.
  uint_t        funcBack;           //!< Specifies the stencil function for the back faces.
  int_t         refValueBack;       //!< Specifies the reference value for the back faces.
  uint_t        valMaskBack;        //!< Specifies the value mask for the back faces.
  PADDING(4);                       //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFStencilAttribute,8);    //!< Compile-time assert on size of structure

/*! \brief The NBFLink structure represents a link between two objects using a callback. */
struct NBFLink
{
  uint_t  linkID;                   //!< Specifies the class id of the callback
  uint_t  subject;                  //!< Specifies the offset of the subject
  uint_t  observer;                 //!< Specifies the offset of the observer
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFLink,4);               //!< Compile-time assert on size of structure

/*! \brief The NBFPatchesBase structure is the base of all patches structures. */
struct NBFPatchesBase : public NBFPrimitive
{
  // nothing required here any more
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFPatchesBase,8);        //!< Compile-time assert on size of structure

/*! \brief The NBFQuadPatches structure represents a general quad patches object.
 *  \remarks The object code for a NBFQuadPatches is NBF_QUAD_PATCHES. */ 
struct NBFQuadPatches : public NBFPatchesBase
{
  uint_t  size;                     //!< Specifies the size of the quad patches. Each patch is specified by size^2 vertices
  PADDING(4);                       //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFQuadPatches,8);        //!< Compile-time assert on size of structure

/*! \brief The NBFQuadPatches4x4 structure represents a 4x4 quad patches object.
 *  \remarks The object code for a NBFQuadPatches4x4 is NBF_QUAD_PATCHES_4X4. */
struct NBFQuadPatches4x4 : public NBFPatchesBase
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFQuadPatches4x4,8);     // Compile-time assert on size of structure

/*! \brief The NBFRectPatches structure represents a general rectangular patches object.
 *  \remarks The object code for a NBFRectPatches is NBF_RECT_PATCHES. */
struct NBFRectPatches : public NBFPatchesBase
{
  uint_t  width;                  //!< Specifies the width of the patches. Each patch is specified by with*height vertices.
  uint_t  height;                 //!< Specifies the height of the patches. Each patch is specified by width*height vertices.
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFRectPatches,8);      //!< Compile-time assert on size of structure

/*! \brief The NBFRectPatches structure represents a general triangular patches object.
 *  \remarks The object code for a NBFRectPatches is NBF_TRI_PATCHES. */
struct NBFTriPatches : public NBFPatchesBase
{
  uint_t  size;                     //!< Specifies the size of the patches. Each patch is specified by 1+2+...+size vertices
  PADDING(4);                       //!< Padding bits ensure offset of next elements is on a 4-byte boundary, regardless of packing
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFTriPatches,8);     //!< Compile-time assert on size of structure

/*! \brief The NBFTriPatches4 structure represents a 4-vertices-per-edge triangular patches object.
 *  \remarks The object code for a NBFTriPatches4 is NBF_TRI_PATCHES_4. */
struct NBFTriPatches4 : public NBFPatchesBase
{
};
NVSG_CTASSERT_MODULO_BYTESIZE(NBFTriPatches4,8);      // Compile-time assert on size of structure

#pragma pack(pop)

#undef __NVSG_CTASSERT_PREFIX

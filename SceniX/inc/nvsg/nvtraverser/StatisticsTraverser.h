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
/** \file */

#include <map>
#include <ostream>
#include <set>
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
#include "nvmath/Trafo.h"
#include "nvsg/Camera.h"
#include "nvsg/Drawable.h"
#include "nvsg/MeshedPrimitiveSet.h"
#include "nvsg/Object.h"
#include "nvsg/StateAttribute.h"
#include "nvsg/StrippedPrimitiveSet.h"
#include "nvtraverser/Traverser.h"

namespace nvtraverser
{
  /*! \brief Base class of all statistics classes. */
  class StatisticsBase
  {
    public:
      NVSG_API StatisticsBase()  : m_count(0), m_referenced(0)  {}

    public:
      /*! \brief Checks if an object is encountered for the first time in the current traversal.
       *  \param p The pointer to the object.
       *  \return \c true, if this object has not been encountered before in the current traversal, otherwise \c false. */
      bool  firstEncounter( const void *p );

    public:
      unsigned int            m_count;        //!< Counts the number of occurences of objects of a specific type.
      std::set<const void *>  m_objects;      //!< A set of pointers to hold all objects already encountered.
      unsigned int            m_referenced;   //!< Counts the number of references of objects of a specific type.
  };

  /*! \brief Class to hold statistics information about Object objects. */
  class StatObject : public StatisticsBase
  {
  };

  /*! \brief Class to hold statistics information about Camera objects. */
  class StatCamera : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about MatrixCamera objects. */
  class StatMatrixCamera : public StatCamera
  {
  };

  /*! \brief Class to hold statistics information about FrustumCamera objects. */
  class StatFrustumCamera : public StatCamera
  {
  };

  /*! \brief Class to hold statistics information about ParallelCamera objects. */
  class StatParallelCamera : public StatFrustumCamera
  {
  };

  /*! \brief Class to hold statistics information about PerspectiveCamera objects. */
  class StatPerspectiveCamera : public StatFrustumCamera
  {
  };

  /*! \brief Class to hold statistics information about Drawable objects. */
  class StatDrawable : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about PrimitiveSet objects. */
  class StatPrimitiveSet : public StatDrawable
  {
  };

  /*! \brief Class to hold statistics information about Primitive objects. */
  class StatPrimitive : public StatDrawable
  {
    public:
      StatPrimitive() 
        : m_indexed(0)
        , m_arrays(0)
        , m_patches(0)
        , m_lineStripAdj(0) 
        , m_linesAdj(0) 
        , m_triStripAdj(0) 
        , m_trisAdj(0) 
        , m_polygons(0) 
        , m_lineLoops(0) 
        , m_quads(0) 
        , m_quadStrips(0) 
        , m_tris(0) 
        , m_triStrips(0) 
        , m_triFans(0) 
        , m_lines(0) 
        , m_lineStrips(0) 
        , m_points(0) 
        , m_faces(0) 
      {}

    public:
      unsigned int m_indexed;         //!< Counts the number of Primitive objects with indices.
      unsigned int m_arrays;          //!< Counts the number of Primitive objects without indices.
      unsigned int m_patches;         //!< Counts the number of Primitive objects of type PRIMITIVE_PATCHES
      unsigned int m_lineStripAdj;    //!< Counts the number of Primitive objects of type PRIMITIVE_LINE_STRIP_ADJACENCY
      unsigned int m_linesAdj;        //!< Counts the number of Primitive objects of type PRIMITIVE_LINES_ADJACENCY
      unsigned int m_triStripAdj;     //!< Counts the number of Primitive objects of type PRIMITIVE_TRIANGLE_STRIP_ADJACENCY
      unsigned int m_trisAdj;         //!< Counts the number of Primitive objects of type PRIMITIVE_TRIANGLES_ADJACENCY
      unsigned int m_polygons;        //!< Counts the number of Primitive objects of type PRIMITIVE_POLYGON
      unsigned int m_lineLoops;       //!< Counts the number of Primitive objects of type PRIMITIVE_LINE_LOOP
      unsigned int m_quads;           //!< Counts the number of Primitive objects of type PRIMITIVE_QUADS
      unsigned int m_quadStrips;      //!< Counts the number of Primitive objects of type PRIMITIVE_QUAD_STRIP
      unsigned int m_tris;            //!< Counts the number of Primitive objects of type PRIMITIVE_TRIANGLES
      unsigned int m_triStrips;       //!< Counts the number of Primitive objects of type PRIMITIVE_TRIANGLE_STRIP
      unsigned int m_triFans;         //!< Counts the number of Primitive objects of type PRIMITIVE_TRIANGLE_FAN
      unsigned int m_lines;           //!< Counts the number of Primitive objects of type PRIMITIVE_LINES
      unsigned int m_lineStrips;      //!< Counts the number of Primitive objects of type PRIMITIVE_LINE_STRIP
      unsigned int m_points;          //!< Counts the number of Primitive objects of type PRIMITIVE_POINTS
      unsigned int m_faces;           //!< Counts the nubmer of faces in all encountered Primitive objects.
  };

  /*! \brief Class to hold statistics information about VertexAttributeSet objects. */
  class StatVertexAttributeSet : public StatObject
  {
    public:
      StatVertexAttributeSet()  : m_numberOfVertices(0), m_numberOfNormaled(0), m_numberOfNormals(0), m_numberOfTextured(0)
                                , m_numberOfTextureUnits(0), m_numberOfTextureDimensions(0), m_numberOfColored(0)
                                , m_numberOfColors(0), m_numberOfSecondaryColored(0), m_numberOfSecondaryColors(0)
                                , m_numberOfFogged(0), m_numberOfFogCoords(0)
      {
        for ( int i=0 ; i<8 ; i++ )
        {
          m_numberOfTextures[i] = 0;
        }
      }
  
    public:
      unsigned int  m_numberOfVertices;           //!< Counts the number of vertices.
      unsigned int  m_numberOfNormaled;           //!< Counts the number of VertexAttributeSet objects with normals.
      unsigned int  m_numberOfNormals;            //!< Counts the number of normals.
      unsigned int  m_numberOfTextured;           //!< Counts the number of VertexAttributeSet objects with texture coordinates.
      unsigned int  m_numberOfTextureUnits;       //!< Counts the number of used texture units.
      unsigned int  m_numberOfTextureDimensions;  //!< Counts the number of texture dimensions.
      unsigned int  m_numberOfTextures[8];        //!< Counts the number of texture coordinates per texture unit.
      unsigned int  m_numberOfColored;            //!< Counts the number of VertexAttributeSet objects with colors.
      unsigned int  m_numberOfColors;             //!< Counts the number of colors.
      unsigned int  m_numberOfSecondaryColored;   //!< Counts the number of VertexAttributeSet objects with secondary colors.
      unsigned int  m_numberOfSecondaryColors;    //!< Counts the number of secondary colors.
      unsigned int  m_numberOfFogged;             //!< Counts the number of VertexAttributeSet objects with fog coordinates.
      unsigned int  m_numberOfFogCoords;          //!< Counts the number of fog coordinates.
  };

  /*! \brief Class to hold statistics information about Skin objects. */
  class StatSkin : public StatObject
  {
    public:
      StatSkin() : m_numberOfIndices(0), m_numberOfInfluences(0), m_numberOfJoints(0) {}

    public:
      unsigned int m_numberOfIndices;       //!< Counts the number of indices.
      unsigned int m_numberOfInfluences;    //!< Counts the number of influences.
      unsigned int m_numberOfJoints;        //!< Counts the number of joints.
  };

  /*! \brief Class to hold statistics information about MeshedPrimitiveSet objects. */
  class StatMeshedPrimitiveSet : public StatPrimitiveSet
  {
    public:
      StatMeshedPrimitiveSet()  : m_numberOfMeshes(0) {}
  
    public:
      unsigned int  m_numberOfMeshes;     //!< Counts the number of meshes.
  };

  /*! \brief Class to hold statistics information about StrippedPrimitiveSet objects. */
  class StatStrippedPrimitiveSet : public StatPrimitiveSet
  {
    public:
      StatStrippedPrimitiveSet()  : m_numberOfStrips(0) {}
  
    public:
      unsigned int  m_numberOfStrips;     //!< Counts the number of strips.
  };

  /*! \brief Class to hold statistics information about Lines objects. */
  class StatLines : public StatPrimitiveSet
  {
    public:
      StatLines()  : m_numberOfSegments(0) {}
  
    public:
      unsigned int  m_numberOfSegments;   //!< Counts the number of segments.
  };

  /*! \brief Class to hold statistics information about LineStrips objects. */
  class StatLineStrips : public StatStrippedPrimitiveSet
  {
    public:
      StatLineStrips()  : m_numberOfSegments(0) {}
  
    public:
      unsigned int  m_numberOfSegments;   //!< Counts the number of segments.
  };

  /*! \brief Class to hold statistics information about PatchesBase objects. */
  class StatPatchesBase : public StatPrimitiveSet
  {
    public:
      StatPatchesBase() : m_numberOfPatches(0)  {}

    public:
      unsigned int m_numberOfPatches;     //!< Counts the number of patches.
  };

  /*! \brief Class to hold statistics information about Patches objects. */
  class StatPatches : public StatPatchesBase
  {
  };

  /*! \brief Class to hold statistics information about RectPatches objects. */
  class StatRectPatches : public StatPatchesBase
  {
  };

  /*! \brief Class to hold statistics information about QuadPatches objects. */
  class StatQuadPatches : public StatPatchesBase
  {
  };

  /*! \brief Class to hold statistics information about QuadPatches4x4 objects. */
  class StatQuadPatches4x4 : public StatPatchesBase
  {
  };

  /*! \brief Class to hold statistics information about TriPatches objects. */
  class StatTriPatches : public StatPatchesBase
  {
  };

  /*! \brief Class to hold statistics information about TriPatches4 objects. */
  class StatTriPatches4 : public StatPatchesBase
  {
  };

  /*! \brief Class to hold statistics information about Points objects. */
  class StatPoints : public StatPrimitiveSet
  {
  };

  /*! \brief Class to hold statistics information about QuadMeshes objects. */
  class StatQuadMeshes : public StatMeshedPrimitiveSet
  {
  public:
    StatQuadMeshes()  : m_numberOfFaces(0) {}
  
  public:
    unsigned int  m_numberOfFaces;      //!< Counts the number of faces.
  };

  /*! \brief Class to hold statistics information about Quads objects. */
  class StatQuads : public StatPrimitiveSet
  {
    public:
      StatQuads()  : m_numberOfFaces(0) {}
  
    public:
      unsigned int  m_numberOfFaces;      //!< Counts the number of faces.
  };

  /*! \brief Class to hold statistics information about QuadStrips objects. */
  class StatQuadStrips : public StatStrippedPrimitiveSet
  {
    public:
      StatQuadStrips()  : m_numberOfFaces(0) {}
  
    public:
      unsigned int  m_numberOfFaces;      //!< Counts the number of faces.
  };

  /*! \brief Class to hold statistics information about Triangles objects. */
  class StatTriangles : public StatPrimitiveSet
  {
    public:
      StatTriangles()  : m_numberOfFaces(0) {}
  
    public:
      unsigned int  m_numberOfFaces;      //!< Counts the number of faces.
  };

  /*! \brief Class to hold statistics information about TriFans objects. */
  class StatTriFans : public StatStrippedPrimitiveSet
  {
    public:
      StatTriFans()  : m_numberOfFaces(0) {}
  
    public:
      unsigned int  m_numberOfFaces;      //!< Counts the number of faces.
  };

  /*! \brief Class to hold statistics information about TriStrips objects. */
  class StatTriStrips : public StatStrippedPrimitiveSet
  {
    public:
      StatTriStrips()  : m_numberOfFaces(0) {}
    
    public:
      unsigned int  m_numberOfFaces;      //!< Counts the number of faces.
  };

  /*! \brief Class to hold statistics information about AnimationState objects. */
  class StatAnimationState : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about VertexAttributeAnimation objects. */
  class StatVertexAttributeAnimation : public StatAnimationState
  {
  };

  /*! \brief Class to hold statistics information about StatePass objects. */
  class StatStatePass : public StatObject
  {
    public:
      StatStatePass()
        : m_numberOfAttributes(0), m_numberOfCgFxed(0), m_numberOfMaterialed(0), m_numberOfTextured(0)
      {}

    public:
      unsigned int  m_numberOfAttributes;     //!< Counts the number of attributes.
      unsigned int  m_numberOfCgFxed;         //!< Counts the number of StatePass objects with a CgFx attribute.
      unsigned int  m_numberOfMaterialed;     //!< Counts the number of StatePass objects with a Material attribute.
      unsigned int  m_numberOfTextured;       //!< Counts the number of StatePass objects with a Texture attribute.
  };

  /*! \brief Class to hold statistics information about Node objects. */
  class StatNode : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about Group objects. */
  class StatGroup : public StatNode
  {
    public:
      StatGroup() : m_numberOfChildren(0) {}
  
    public:
      unsigned int                        m_numberOfChildren;   //!< Counts the number of children.
      std::map<unsigned int,unsigned int> m_childrenHistogram;  //!< Counts the number of Group objects per number of children.
  };

  /*! \brief Class to hold statistics information about IndexAnimation objects. */
  class StatIndexAnimation : public StatAnimationState
  {
  };

  /*! \brief Class to hold statistics information about LOD objects. */
  class StatLOD : public StatGroup
  {
  };

  /*! \brief Class to hold statistics information about MapObject objects. */
  class StatMapObject : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about MapArray objects. */
  class StatMapArray : public StatMapObject
  {
  };

  /*! \brief Class to hold statistics information about MapElement objects. */
  class StatMapElement : public StatMapObject
  {
  };

  /*! \brief Class to hold statistics information about MapList objects. */
  class StatMapList : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about MapMatrix objects. */
  class StatMapMatrix : public StatMapObject
  {
  };

  /*! \brief Class to hold statistics information about Billboard objects. */
  class StatBillboard : public StatGroup
  {
  };

  /*! \brief Class to hold statistics information about Transform objects. */
  class StatTransform : public StatGroup
  {
  };

  /*! \brief Class to hold statistics information about FlipbookAnimation objects. */
  class StatFlipbookAnimation : public StatGroup
  {
  };

  /*! \brief Class to hold statistics information about IndexAnimationDescription objects. */
  class StatIndexAnimationDescription : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about FramedIndexAnimationDescription objects. */
  class StatFramedIndexAnimationDescription : public StatIndexAnimationDescription
  {
  };

  /*! \brief Class to hold statistics information about TrafoAnimationDescription objects. */
  class StatTrafoAnimationDescription : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about KeyFramedTrafoAnimationDescription objects. */
  class StatKeyFramedTrafoAnimationDescription : public StatTrafoAnimationDescription
  {
  };

  /*! \brief Class to hold statistics information about LinearInterpolatedTrafoAnimationDescription objects. */
  class StatLinearInterpolatedTrafoAnimationDescription : public StatKeyFramedTrafoAnimationDescription
  {
  };

  /*! \brief Class to hold statistics information about FramedTrafoAnimationDescription objects. */
  class StatFramedTrafoAnimationDescription : public StatTrafoAnimationDescription
  {
  };

  /*! \brief Class to hold statistics information about VertexAttributeAnimationDescription objects. */
  class StatVertexAttributeAnimationDescription : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about KeyFramedVertexAttributeAnimationDescription objects. */
  class StatKeyFramedVertexAttributeAnimationDescription : public StatVertexAttributeAnimationDescription
  {
  };

  /*! \brief Class to hold statistics information about LinearInterpolatedVertexAttributeAnimationDescription objects. */
  class StatLinearInterpolatedVertexAttributeAnimationDescription : public StatKeyFramedVertexAttributeAnimationDescription
  {
  };

  /*! \brief Class to hold statistics information about FramedVertexAttributeAnimationDescription objects. */
  class StatFramedVertexAttributeAnimationDescription : public StatVertexAttributeAnimationDescription
  {
  };

  /*! \brief Class to hold statistics information about AnimatedTransform objects. */
  class StatAnimatedTransform : public StatTransform
  {
  };

  /*! \brief Class to hold statistics information about AnimatedVertexAttributeSet objects. */
  class StatAnimatedVertexAttributeSet : public StatVertexAttributeSet
  {
    public:
      StatAnimatedVertexAttributeSet() : m_numberOfAnimations(0)  {}
  
    public:
      unsigned int                        m_numberOfAnimations;   //!< Counts the number of animations.
      std::map<unsigned int,unsigned int> m_animationsHistogram;  //!< Counts the number of AnimatedVertexAttributeSet objects per number of animations.
  };

  /*! \brief Class to hold statistics information about Switch objects. */
  class StatSwitch : public StatGroup
  {
  };

  /*! \brief Class to hold statistics information about LightSource objects. */
  class StatLightSource : public StatNode
  {
  };

  /*! \brief Class to hold statistics information about DirectedLight objects. */
  class StatDirectedLight : public StatLightSource
  {
  };

  /*! \brief Class to hold statistics information about PointLight objects. */
  class StatPointLight : public StatLightSource
  {
    public:
      StatPointLight() : m_numberOfAttenuated(0)  {}
  
    public:
      unsigned int  m_numberOfAttenuated;     //!< Counts the number of attenuated PointLight objects.
  };

  /*! \brief Class to hold statistics information about SpotLight objects. */
  class StatSpotLight : public StatLightSource
  {
    public:
      StatSpotLight() : m_numberOfAttenuated(0)  {}
  
    public:
      unsigned int  m_numberOfAttenuated;     //!< Counts the number of attenuated SpotLight objects.
  };

  /*! \brief Class to hold statistics information about Shape objects. */
  class StatShape : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about GeoNode objects. */
  class StatGeoNode : public StatNode
  {
    public:
      StatGeoNode() : m_numberOfStateSets(0) {}
  
    public:
      unsigned int  m_numberOfStateSets;      //!< Counts the number of StateSet objects.
  };

  /*! \brief Class to hold statistics information about StateAttribute objects. */
  class StatStateAttribute : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about LightingAttribute objects. */
  class StatLightingAttribute : public StatStateAttribute
  {
  };

  /*! \brief Class to hold statistics information about AlphaTestAttribute objects. */
  class StatAlphaTestAttribute : public StatStateAttribute
  {
  };

  /*! \brief Class to hold statistics information about BlendAttribute objects. */
  class StatBlendAttribute : public StatStateAttribute
  {
  };

  /*! \brief Class to hold statistics information about CgFx objects. */
  class StatCgFx : public StatStateAttribute
  {
    public:
      StatCgFx()  : m_numberOfTechniques(0), m_numberOfPasses(0) {}
  
    public:
      unsigned int  m_numberOfTechniques;     //!< Counts the number of techniques.
      unsigned int  m_numberOfPasses;         //!< Counts the number of passes.
  };

  /*! \brief Class to hold statistics information about DepthAttribute objects. */
  class StatDepthAttribute : public StatStateAttribute
  {
  };

  /*! \brief Class to hold statistics information about Material objects. */
  class StatMaterial : public StatStateAttribute
  {
    public:
      StatMaterial() : m_numberOfTransparent(0) {}
  
    public:
      unsigned int  m_numberOfTransparent;      //!< Counts the number of transparent Material objects.
  };

  /*! \brief Class to hold statistics information about FaceAttribute objects. */
  class StatFaceAttribute : public StatStateAttribute
  {
    public:
      StatFaceAttribute()  : m_numberOfTwoSidedLighting(0) {}
  
    public:
      unsigned int  m_numberOfTwoSidedLighting;   //!< Counts the number of FaceAttribute objects with two-sided lighting.
  };

  /*! \brief Class to hold statistics information about LineAttribute objects. */
  class StatLineAttribute : public StatStateAttribute
  {
  };

  /*! \brief Class to hold statistics information about PointAttribute objects. */
  class StatPointAttribute : public StatStateAttribute
  {
  };

  /*! \brief Class to hold statistics information about RTFx objects. */
  class StatRTFx : public StatStateAttribute
  {
    public:
      StatRTFx() : m_numberOfParameters(0), m_numberOfTextures(0) {}

    public:
      unsigned int m_numberOfParameters;      //!< Counts the number of parameters.
      unsigned int m_numberOfTextures;        //!< Counts the number of textures.
  };

  /*! \brief Class to hold statistics information about RTFxProgram objects. */
  class StatRTFxProgram : public StatObject
  {
    public:
      StatRTFxProgram() : m_numberOfFileDefined(0), m_numberOfLumpDefined(0)
                        , m_numberOfParameters(0), m_numberOfDefinedParameters(0)
                        , m_numberOfRayTypes(0), m_numberOfEntryPoints(0), m_numberOfPTXFormats(0)
                        , m_numberOfCUDAFormats(0), m_numberOfTextures(0) {}

    public:
      unsigned int m_numberOfFileDefined;         //!< Counts the number of RTFxProgram objects defined by file.
      unsigned int m_numberOfLumpDefined;         //!< Counts the number of RTFxProgram objects defined by lump.
      unsigned int m_numberOfParameters;          //!< Counts the number of parameters.
      unsigned int m_numberOfDefinedParameters;   //!< Counts the number of defined parameters.
      unsigned int m_numberOfRayTypes;            //!< Counts the number of ray types.
      unsigned int m_numberOfEntryPoints;         //!< Counts the number of entry points.
      unsigned int m_numberOfPTXFormats;          //!< Counts the number of RTFxProgram objects in ptx format.
      unsigned int m_numberOfCUDAFormats;         //!< Counts the number of RTFxProgram objects in CUDA format.
      unsigned int m_numberOfTextures;            //!< Counts the number of textures.
  };

  /*! \brief Class to hold statistics information about TextureAttribute objects. */
  class StatTextureAttribute : public StatStateAttribute
  {
  };

  /*! \brief Class to hold statistics information about TextureAttributeItem objects. */
  class StatTextureAttributeItem : public StatObject
  {
  };

  /*! \brief Class to hold statistics information about TrafoAnimation objects. */
  class StatTrafoAnimation : public StatAnimationState
  {
  };

  /*! \brief Class to hold statistics information about StateSet objects. */
  class StatStateSet : public StatObject
  {
    public:
      StatStateSet()  : m_numberOfVariants(0) {}
  
    public:
      unsigned int  m_numberOfVariants;       //!< Counts the number of variants.
  };  

  /*! \brief Class to hold statistics information about StateVariant objects. */
  class StatStateVariant : public StatObject
  {
    public:
      StatStateVariant()  : m_numberOfPasses(0) {}

    public:
      unsigned int  m_numberOfPasses;         //!< Counts the number of passes.
  };

  /*! \brief Class to hold statistics information about Texture objects. */
  class StatTexture : public StatisticsBase
  {
    public:
      StatTexture() : m_numImages(0), m_sumOfSizes(0) {}
  
    public:
      unsigned int                m_numImages;    //!< Counts the number of images.
      std::map<int,unsigned int>  m_widths;       //!< Counts the number of Texture objects per width.
      std::map<int,unsigned int>  m_heights;      //!< Counts the number of Texture objects per height.
      std::map<int,unsigned int>  m_depths;       //!< Counts the number of Texture objects per depth.
      std::map<int,unsigned int>  m_sizes;        //!< Counts the number of Texture objects per size.
      unsigned int                m_sumOfSizes;   //!< Sum of all the sizes.
  };

  /*! \brief Class to hold statistics information about all objects. */
  class Statistics
  {
    public:
      StatAlphaTestAttribute                          m_statAlphaTestAttribute;                           //!< Statistics for AlphaTestAttribute objects.
      StatAnimatedTransform                           m_statAnimatedTransform;                            //!< Statistics for AnimatedTransform objects.
      StatAnimatedVertexAttributeSet                  m_statAnimatedVertexAttributeSet;                   //!< Statistics for AnimatedVertexAttributeSet objects.
      StatBillboard                                   m_statBillboard;                                    //!< Statistics for Billboard objects.
      StatBlendAttribute                              m_statBlendAttribute;                               //!< Statistics for BlendAttribute objects.
      StatCgFx                                        m_statCgFx;                                         //!< Statistics for CgFx objects.
      StatDepthAttribute                              m_statDepthAttribute;                               //!< Statistics for DepthAttribute objects.
      StatDirectedLight                               m_statDirectedLight;                                //!< Statistics for DirectedLight objects.
      StatFaceAttribute                               m_statFaceAttribute;                                //!< Statistics for FaceAttribute objects.
      StatFlipbookAnimation                           m_statFlipbookAnimation;                            //!< Statistics for FlipbookAnimation objects.
      StatFramedIndexAnimationDescription             m_statFramedIndexAnimationDescription;              //!< Statistics for FramedIndexAnimationDescription objects.
      StatFramedTrafoAnimationDescription             m_statFramedTrafoAnimationDescription;              //!< Statistics for FramedTrafoAnimationDescription objects.
      StatFramedVertexAttributeAnimationDescription   m_statFramedVertexAttributeAnimationDescription;    //!< Statistics for FramedVertexAttributeAnimationDesription objects.
      StatGeoNode                                     m_statGeoNode;                                      //!< Statistics for GeoNode objects.
      StatGroup                                       m_statGroup;                                        //!< Statistics for Group objects.
      StatIndexAnimation                              m_statIndexAnimation;                               //!< Statistics for IndexAnimation objects.
      StatLightingAttribute                           m_statLightingAttribute;                            //!< Statistics for LightingAttribute objects.
      StatLinearInterpolatedTrafoAnimationDescription m_statLinearInterpolatedTrafoAnimationDescription;  //!< Statistics for LinearInterpolatedTrafoAnimationDescription objects.
      StatLinearInterpolatedVertexAttributeAnimationDescription m_statLinearInterpolatedVertexAttributeAnimationDescription;  //!< Statistics for LinearInterpolatedVertexAttributeAnimationDescription objects.
      StatLineAttribute                               m_statLineAttribute;                                //!< Statistics for LineAttribute objects.
      StatLines                                       m_statLines;                                        //!< Statistics for Lines objects.
      StatLineStrips                                  m_statLineStrips;                                   //!< Statistics for LineStrips objects.
      StatLOD                                         m_statLOD;                                          //!< Statistics for LOD objects.
      StatMapArray                                    m_statMapArray;                                     //!< Statistics for MapArray objects.
      StatMapElement                                  m_statMapElement;                                   //!< Statistics for MapElement objects.
      StatMapList                                     m_statMapList;                                      //!< Statistics for MapList objects.
      StatMapMatrix                                   m_statMapMatrix;                                    //!< Statistics for MapMatrix objects.
      StatMaterial                                    m_statMaterial;                                     //!< Statistics for Material objects.
      StatMatrixCamera                                m_statMatrixCamera;                                 //!< Statistics for MatrixCamera objects.
      StatParallelCamera                              m_statParallelCamera;                               //!< Statistics for ParallelCamera objects.
      StatPatches                                     m_statPatches;                                      //!< Statistics for Patches objects.
      StatPerspectiveCamera                           m_statPerspectiveCamera;                            //!< Statistics for PerspectiveCamera objects.
      StatPointAttribute                              m_statPointAttribute;                               //!< Statistics for PointAttribute objects.
      StatPointLight                                  m_statPointLight;                                   //!< Statistics for PointLight objects.
      StatPoints                                      m_statPoints;                                       //!< Statistics for Points objects.
      StatPrimitive                                   m_statPrimitives;                                   //!< Statistics for Primitives objects.
      StatQuadMeshes                                  m_statQuadMeshes;                                   //!< Statistics for QuadMeshes objects.
      StatQuadPatches                                 m_statQuadPatches;                                  //!< Statistics for QuadPatches objects.
      StatQuadPatches4x4                              m_statQuadPatches4x4;                               //!< Statistics for QuadPatches4x4 objects.
      StatQuads                                       m_statQuads;                                        //!< Statistics for Quads objects.
      StatQuadStrips                                  m_statQuadStrips;                                   //!< Statistics for QuadStrips objects.
      StatRectPatches                                 m_statRectPatches;                                  //!< Statistics for RectPatches objects.
      StatRTFx                                        m_statRTFx;                                         //!< Statistics for RTFx objects.
      StatRTFxProgram                                 m_statRTFxProgram;                                  //!< Statistics for RTFxProgram objects.
      StatSpotLight                                   m_statSpotLight;                                    //!< Statistics for SpotLight objects.
      StatShape                                       m_statShape;                                        //!< Statistics for Shape objects.
      StatSkin                                        m_statSkin;                                         //!< Statistics for Skin objects.
      StatStatePass                                   m_statStatePass;                                    //!< Statistics for StatePass objects.
      StatStateSet                                    m_statStateSet;                                     //!< Statistics for StateSet objects.
      StatStateVariant                                m_statStateVariant;                                 //!< Statistics for StateVariant objects.
      StatSwitch                                      m_statSwitch;                                       //!< Statistics for Switch objects.
      StatTexture                                     m_statTexture;                                      //!< Statistics for Texture objects.
      StatTextureAttribute                            m_statTextureAttribute;                             //!< Statistics for TextureAttribute objects.
      StatTextureAttributeItem                        m_statTextureAttributeItem;                         //!< Statistics for TextureAttributeItem objects.
      StatTrafoAnimation                              m_statTrafoAnimation;                               //!< Statistics for TrafoAnimation objects.
      StatTransform                                   m_statTransform;                                    //!< Statistics for Transform objects.
      StatTriangles                                   m_statTriangles;                                    //!< Statistics for Triangles objects.
      StatTriFans                                     m_statTriFans;                                      //!< Statistics for TriFans objects.
      StatTriPatches                                  m_statTriPatches;                                   //!< Statistics for TriPatches objects.
      StatTriPatches4                                 m_statTriPatches4;                                  //!< Statistics for TriPatches4 objects.
      StatTriStrips                                   m_statTriStrips;                                    //!< Statistics for TriStrips objects.
      StatVertexAttributeAnimation                    m_statVertexAttributeAnimation;                     //!< Statistics for VertexAttributeAnimation objects.
      StatVertexAttributeSet                          m_statVertexAttributeSet;                           //!< Statistics for VertexAttributeSet objects.
  };
  
  //! Traverser to record some statistics of a scene.
  class StatisticsTraverser : public nvtraverser::SharedTraverser
  {
    public:
      //! Constructor
      NVSG_API StatisticsTraverser(void);
  
      //! Get a constant pointer to the statistics results.
      NVSG_API const Statistics  * getStatistics( void ) const;
  
      //! Record statistics of a Camera.
      /** Just records the statistics of an Object object.  */
      NVSG_API void  statCamera( const nvsg::Camera *p, StatCamera &stats );
  
      //! Record statistics of a Drawable.
      /** Just records the statistics of an Object object.  */
      NVSG_API void  statDrawable( const nvsg::Drawable *p, StatDrawable &stats );
  
      //! Record statistics of a FrustumCamera.
      /** Just records the statistics of an Object object.  */
      NVSG_API void  statFrustumCamera( const nvsg::FrustumCamera *p, StatFrustumCamera &stats );
  
      //! Record statistics of a Group.
      /** Records the number of children and traverses them. Then the statistics of a Node are recorded.  */
      NVSG_API void  statGroup( const nvsg::Group *p, StatGroup &stats );
  
      //! Record statistics of a LightSource.
      /** Just records the statistics of a Node.  */
      NVSG_API void  statLightSource( const nvsg::LightSource *p, StatLightSource &stats );
  
      /*! \brief Record statistics of a MeshedPrimitiveSet.
       *  \remarks Records the number of meshes and the statistics of a PrimitiveSet. */
      NVSG_API void  statMeshedPrimitiveSet( const nvsg::MeshedPrimitiveSet *p, StatMeshedPrimitiveSet &stats );
  
      //! Record statistics of a Node.
      /** Just records the statistics of an Object. */
      NVSG_API void  statNode( const nvsg::Node *p, StatNode &stats );
  
      //! Record statistics of an Object.
      /** Does nothing. */
      NVSG_API void  statObject( const nvsg::Object *p, StatObject &stats );
  
      //! Record statistics of a PatchesBase.
      /** Records the total number of patches. Then the statistics of a PrimitiveSet are recorded.  */
      NVSG_API void  statPatchesBase( const nvsg::PatchesBase *p, StatPatchesBase &stats );
  
      //! Record statistics of a PrimitiveSet.
      /** Records the statistics of the VertexAttributeSet. Then the statistics of a Drawable are recorded. */
      NVSG_API void  statPrimitiveSet( const nvsg::PrimitiveSet *p, StatPrimitiveSet &stats );
  
      //! Record statistics of a Primitive.
      /** Records the statistics of the VertexAttributeSet. Then the statistics of a Drawable are recorded. */
      NVSG_API void  statPrimitive( const nvsg::Primitive *p, StatPrimitive &stats );
  
      //! Record statistics of a Quads.
      /** Records the total number of faces. Then the statistics of a PrimitiveSet are recorded.  */
      NVSG_API void  statQuads( const nvsg::Quads *p, StatQuads &stats );

      //! Record statistics of a StateAttribute.
      /** Just records the statistics of an Object. */
      NVSG_API void  statStateAttribute( const nvsg::StateAttribute *p, StatStateAttribute &stats );

      /*! \brief Record statistics of a StrippedPrimitiveSet.
       *  \remarks Records the number of strips and the statistics of a PrimitiveSet. */
      NVSG_API void statStrippedPrimitiveSet( const nvsg::StrippedPrimitiveSet *p, StatStrippedPrimitiveSet &stats );

      //! Record statistics of a Transform.
      /** Just records the statistics of a Group. */
      NVSG_API void statTransform( const nvsg::Transform *p, StatTransform &stats );

      //! Record statistics of a Triangles.
      /** Records the total number of faces. Then the statistics of a PrimitiveSet are recorded.  */
      NVSG_API void statTriangles( const nvsg::Triangles *p, StatTriangles &stats );

      //! Record statistics of a VertexAttributeSet.
      /** Records the number of vertices, the number of PrimitiveSet objects with normals, the number of
        * normals, the number of textured PrimitiveSet objects, the number of used texture units, the
        * total dimension of the textures, the total number of texture coordinates, the number of colored
        * PrimitiveSet object, the total number of colors, the number of secondary colored PrimitiveSet
        * objects, the total number of secondary colors, the number of fogged PrimitiveSet objects, and
        * the total number of fog coordinates. Then the statistics of an Object are recorded. */
      NVSG_API void statVertexAttributeSet( const nvsg::VertexAttributeSet *p, StatVertexAttributeSet &stats );

      /*! \brief Record statistics of a Skin object.
       *  \remarks Records the number of indices, influences, and joints, and the statistics of as an Object. */
      NVSG_API void statSkin( const nvsg::Skin *p, StatSkin &stats );

      /*! \brief Record statistics of a MapObject object.
       *  \remarks Just records the statistics as an Object. */
      NVSG_API void statMapObject( const nvsg::MapObject * p, StatMapObject & stats );

      /*! \brief Record statistics of an AnimationState object. 
       *  \remarks Just records the statistics as an Object. */
      NVSG_API void statAnimationState( const nvsg::AnimationState * p, StatAnimationState & stats );

      /*! \brief Record statistics of an IndexAnimationDescription object. 
       *  \remarks Just records the statistics as an Object. */
      NVSG_API void statIndexAnimationDescription( const nvsg::AnimationDescription<unsigned int> * p, StatIndexAnimationDescription & stats );

      /*! \brief Record statistics of an TrafoAnimationDescription object. 
       *  \remarks Just records the statistics as an Object. */
      NVSG_API void statTrafoAnimationDescription( const nvsg::AnimationDescription<nvmath::Trafo> * p, StatTrafoAnimationDescription & stats );

      /*! \brief Record statistics of an VertexAttributeAnimationDescription object. 
       *  \remarks Just records the statistics as an Object. */
      NVSG_API void statVertexAttributeAnimationDescription( const nvsg::AnimationDescription<nvsg::VertexAttribute> * p, StatVertexAttributeAnimationDescription & stats );

    protected:
      //! Destructor
      NVSG_API virtual ~StatisticsTraverser(void);

      //--  Functions implemented from Traverser --
      //! Handle an AnimatedTransform object.
      /** The number of AnimatedTransform objects and the total number of animation keys are count. Usually the number of
        * animation keys should be equal on each AnimatedTransform. Then the statistics of a Transform are recorded. */
      NVSG_API virtual void  handleAnimatedTransform( const nvsg::AnimatedTransform *p  //!<  AnimatedTransform to handle
                                        );

      NVSG_API virtual void  handleAnimatedVertexAttributeSet( const nvsg::AnimatedVertexAttributeSet *avas );

      //! Handle a Billboard object.
      /** The number of Billboard objects are count. Then the statistics of a Group is recorded.  */
      NVSG_API virtual void  handleBillboard( const nvsg::Billboard *p     //!<  Billboard to handle
                                   );

      //! Handle a BlendAttribute object.
      /** The number of BlendAttribute objects are count. Then the statistics of a StateAttribute are recorded.  */
      NVSG_API virtual void  handleBlendAttribute( const nvsg::BlendAttribute *p         //!<  BlendAttribute to handle
                                        );

      //! Handle a CgFX object.
      /** The number of CgFx objects and the total number of techniques and passes is count. Then the statistics of a
        * StateAttribute is recorded. */
      NVSG_API virtual void  handleCgFx( const nvsg::CgFx *p               //!<  CgFx to handle
                              );

      //! Handle a DepthAttribute object.
      /** The number of DepthAttribute objects are count. Then the statistics of a StateAttribute are recorded.  */
      NVSG_API virtual void  handleDepthAttribute( const nvsg::DepthAttribute *p         //!<  DepthAttribute to handle
                                        );
  
      //! Handle a DírectedLight object.
      NVSG_API virtual void  handleDirectedLight( const nvsg::DirectedLight *p    //!<  DirectedLight to handle
                                       );
  
      //! Handle a FaceAttribute object.
      /** The number of FaceAttribute objects and the number of FaceAttributes with back face culling, lighting, line anti-
        * aliasing, local viewer property, and two sided lighting are count. Then the statistics of a StateAttribute
        * are recorded.  */
      NVSG_API virtual void  handleFaceAttribute( const nvsg::FaceAttribute *p         //!<  FaceAttribute to handle
                                       );
  
      //! Handle a FlipbookAnimation object.
      /** The number of FlipbookAnimation objects and the total number of animation keys are count. Usually the number of
        * animation keys should be equal on each FlipbookAnimation. Then the statistics of a Group are recorded. */
      NVSG_API virtual void  handleFlipbookAnimation( const nvsg::FlipbookAnimation *p  //!<  FlipbookAnimation to handle
        );
  
      //! Handle a GeoNode object.
      /** The number of GeoNode objects and the total number of geometries are count. After having traversed the geometries,
        * the statistics of a Node is recorded. */
      NVSG_API virtual void  handleGeoNode( const nvsg::GeoNode *p            //!<  GeoNode to handle
                                        );
  
      //! Handle a Group object.
      /** The number of Group objects are count. */
      NVSG_API virtual void  handleGroup( const nvsg::Group *p                 //!<  Group to handle
                               );
  
      //! Handle a LineAttribute object.
      /** The number of LineAttribute objects is count. Then the statistics of a StateAttribute are recorded.  */
      NVSG_API virtual void  handleLineAttribute( const nvsg::LineAttribute *p         //!<  LineAttribute to handle
                                       );
  
      //! Handle a Lines object.
      /** The number of Lines objects and the total number of lines are count. Then the statistics of a
        * PrimitiveSet are recorded. */
      NVSG_API virtual void  handleLines( const nvsg::Lines *p             //!<  Lines to handle
                               );
  
      //! Handle a LineStrips object.
      /** The number of LineStrips objects and the total number of segments are count. Then the statistics
        * of a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleLineStrips( const nvsg::LineStrips *p   //!<  LineStrips to handle
                                    );
  
      //! Handle a LOD object.
      /** The number of LOD objects is counted. Then the statistics of a Group is recorded.  */
      NVSG_API virtual void  handleLOD( const nvsg::LOD *p                //!<  LOD to save
                             );
  
      //! Handle a Material object.
      /** The number of Material objects and the number of transparent materials are count. Then the statistics of a
        * StateAttribute are recorded.  */
      NVSG_API virtual void  handleMaterial( const nvsg::Material *p           //!<  Material to handle
                                  );
  
      //! Handle a MatrixCamera object.
      /** The number of MatrixCamera objects is counted. Then the statistics of a Camera is recoreded.  */
      NVSG_API virtual void  handleMatrixCamera( const nvsg::MatrixCamera *p   //!<  MatrixCamera to handle
                                        );
  
      //! Handle a ParallelCamera object.
      /** The number of ParallelCamera objects is counted. Then the statistics of a Camera is recoreded.  */
      NVSG_API virtual void  handleParallelCamera( const nvsg::ParallelCamera *p     //!<  ParallelCamera to handle
                                        );
  
      //! Handle a PerspectiveCamera object.
      /** The number of PerspectiveCamera objects is counted. Then the statistics of a Camera is recoreded.  */
      NVSG_API virtual void  handlePerspectiveCamera( const nvsg::PerspectiveCamera *p   //!<  PerspectiveCamera to handle
                                        );
  
      //! Handle a PointAttribute object.
      /** The number of PointAttribute objects is count. Then the statistics of a StateAttribute are recorded.  */
      NVSG_API virtual void  handlePointAttribute( const nvsg::PointAttribute *p         //!<  PointAttribute to handle
                                        );
  
      //! Handle a PointLight object.
      /** The number of PointLight objects and the number of attenuated PointLight objects are count. The the statistics
        * of a LightSource is recorded. */
      NVSG_API virtual void  handlePointLight( const nvsg::PointLight *p       //!<  PointLight to handle
                                    );
  
      //! Handle a Points object.
      /** The number of Points objects are count. Then the statistics of a PrimitiveSet are recorded. */
      NVSG_API virtual void  handlePoints( const nvsg::Points *p               //!<  Points to handle
                                );
  
      //! Handle a Primitive object.
      /** The number of Primitive objects are counted. */
      NVSG_API virtual void  handlePrimitive( const nvsg::Primitive *p               //!<  Primitive to handle
                                );
  
      //! Handle a QuadMeshes object.
      /** The number of QuadMeshes objects and the total number of faces are count. Then the statistics of
        * a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleQuadMeshes( const nvsg::QuadMeshes *p         //!<  QuadMeshes to handle
        );
  
      //! Handle a QuadPatches object.
      /** The number of QuadPatches objects and the total number of patches are count. Then the statistics
        * of a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleQuadPatches( const nvsg::QuadPatches *p       //!<  QuadPatches to handle
                                        );
  
      //! Handle a QuadPatches4x4 object.
      /** The number of QuadPatches4x4 objects and the total number of patches are count. Then the
        * statistics of a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleQuadPatches4x4( const nvsg::QuadPatches4x4 *p  //!<  QuadPatches4x4 to handle
                                        );
  
      //! Handle a Quads object.
      /** The number of Quads objects and the total number of faces are count. Then the statistics of a
        * PrimitiveSet are recorded. */
      NVSG_API virtual void  handleQuads( const nvsg::Quads *p              //!<  Quads to handle
                                        );

      //! Handle a QuadStrips object.
      /** The number of QuadStrips objects and the total number of faces are count. Then the statistics of
        * a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleQuadStrips( const nvsg::QuadStrips *p         //!<  QuadStrips to handle
                                        );

      //! Handle a RectPatches object.
      /** The number of RectPatches objects and the total number of patches are count. Then the statistics
        * of a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleRectPatches( const nvsg::RectPatches *p       //!<  RectPatches to handle
                                        );

      /*! \brief Handle an RTFx object.
       *  \remarks The number of RTFx objects and the total number of parameters are count. Then the statistics of a
       *  StateAttribute are recorded. */
      NVSG_API virtual void handleRTFx( const nvsg::RTFx * p );

      /*! \brief Handle an RTFxProgram object. */
      NVSG_API virtual void handleRTFxProgram( const nvsg::RTFxProgram * p );

      /*! \brief Handle a Skin object. */
      NVSG_API virtual void handleSkin( const nvsg::Skin * p );
  
      //! Handle a SpotLight object.
      /** The number of SpotLight objects and the number of attenuated SpotLight objects are count. The the statistics of a
        * LightSource is recorded. */
      NVSG_API virtual void  handleSpotLight( const nvsg::SpotLight *p        //!<  SpotLight to handle
                                   );

      //! Handle a StateSet object.
      /** The number of StateSet objects, the total number of attributes, and the number of StateAttributes of type CgFx
        * Material, TextureAttribute are count. After having traversed the attributes, the statistics of an Object are
        * recorded. */
      NVSG_API virtual void  handleStateSet( const nvsg::StateSet *p           //!<  StateSet to handle
                                        );

      //! Handle a Switch object.
      /** The number of Switch objects is count. Then the statistics of a Group is recoreded. */
      NVSG_API virtual void  handleSwitch( const nvsg::Switch *p             //!<  Switch to handle
                                        );
  
      //! Handle a TextureAttribute object.
      /** The number of TextureAttribute objects, the number of Texture objects, the total number of Images in these
        * Textures, a histogram of width, height, depth, and size of these Textures and the total size of these Textures
        * are count. Then the statistics of a StateAttribute is recorded. */
      NVSG_API virtual void  handleTextureAttribute( const nvsg::TextureAttribute *p   //!<  TextureAttribute to handle
                                        );
  
      //! Handle a Transform object.
      /** The number of Transform objects are count. Then the statistics of a Group is recorded.  */
      NVSG_API virtual void  handleTransform( const nvsg::Transform *p          //!<  Transform to handle
                                        );
  
      //! Handle a Triangles object.
      /** The number of Triangles objects and the total number of faces are count. Then the statistics of
        * a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleTriangles( const nvsg::Triangles *p          //!<  Triangles to handle
                                        );
  
      //! Handle a TriFans object.
      /** The number of TriFans objects and the total number of faces are count. Then the statistics of a
        * PrimitiveSet are recorded. */
      NVSG_API virtual void  handleTriFans( const nvsg::TriFans *p               //!<  TriFans to handle
                                 );
  
      //! Handle a TriPatches object.
      /** The number of TriPatches objects and the total number of patches are count. Then the statistics
        * of a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleTriPatches( const nvsg::TriPatches *p       //!<  TriPatches to handle
                                        );

      //! Handle a TriPatches4 object.
      /** The number of TriPatches4 objects and the total number of patches are count. Then the statistics
        * of a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleTriPatches4( const nvsg::TriPatches4 *p       //!<  TriPatches4 to handle
                                        );

      //! Handle a TriStrips object.
      /** The number of TriStrips objects and the total number of faces are count. Then the statistics of
        * a PrimitiveSet are recorded. */
      NVSG_API virtual void  handleTriStrips( const nvsg::TriStrips *p          //!<  TriStrips to handle
                                        );

      NVSG_API virtual void  handleVertexAttributeSet( const nvsg::VertexAttributeSet *vas );

      NVSG_API virtual void handleShape( const nvsg::Shape * shape );

      NVSG_API virtual void handleStateVariant( const nvsg::StateVariant * variant );

      NVSG_API virtual void handleStatePass( const nvsg::StatePass * pass );

      NVSG_API virtual void handleTextureAttributeItem( const nvsg::TextureAttributeItem * texAttribItem );

      NVSG_API virtual void handleAlphaTestAttribute( const nvsg::AlphaTestAttribute * alphaTestAttrib );

      NVSG_API virtual void handleLightingAttribute( const nvsg::LightingAttribute * laAttrib );

      NVSG_API virtual void handleMapArray( const nvsg::MapArray * mapArray );

      NVSG_API virtual void handleMapElement( const nvsg::MapElement * mapElement );

      NVSG_API virtual void handleMapMatrix( const nvsg::MapMatrix * mapMatrix );

      NVSG_API virtual void handleMapList( const nvsg::MapList * mapList );

      NVSG_API virtual void handleIndexAnimation( const nvsg::Animation<unsigned int> * anim );

      NVSG_API virtual void handleTrafoAnimation( const nvsg::Animation<nvmath::Trafo> * anim );

      NVSG_API virtual void handleVertexAttributeAnimation( const nvsg::Animation<nvsg::VertexAttribute> * anim );

      NVSG_API virtual void handleFramedIndexAnimationDescription( const nvsg::FramedAnimationDescription<unsigned int> * anim );

      NVSG_API virtual void handleFramedTrafoAnimationDescription( const nvsg::FramedAnimationDescription<nvmath::Trafo> * trafo );

      NVSG_API virtual void handleFramedVertexAttributeAnimationDescription( const nvsg::FramedAnimationDescription<nvsg::VertexAttribute> *p );

      NVSG_API virtual void handleLinearInterpolatedTrafoAnimationDescription( const nvsg::LinearInterpolatedAnimationDescription<nvmath::Trafo> * trafo );

      NVSG_API virtual void handleLinearInterpolatedVertexAttributeAnimationDescription( const nvsg::LinearInterpolatedAnimationDescription<nvsg::VertexAttribute> *p );

    private:
      Statistics  * m_statistics;
  };
 
  //! Output a statistics summary. 
  NVSG_API std::ostream& operator<<( std::ostream& os, const StatisticsTraverser& obj );
  
  inline const Statistics * StatisticsTraverser::getStatistics( void ) const
  {
    return( m_statistics );
  }
}


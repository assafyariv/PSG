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

#include "nvutil/Handle.h" // Handle<T> definition
#include "nvutil/HandledObject.h"
#include "nvutil/SmartPtr.h"

// required declaration
namespace nvmath
{ 
  class Trafo;

  // Vector
  template<unsigned int n, typename T> class Vecnt;
  typedef Vecnt<2,float>  Vec2f;
  typedef Vecnt<2,double> Vec2d;
  typedef Vecnt<3,float>  Vec3f;
  typedef Vecnt<3,double> Vec3d;
  typedef Vecnt<4,float>  Vec4f;
  typedef Vecnt<4,double> Vec4d;
  typedef Vecnt<2,int> Vec2i;
  typedef Vecnt<3,int> Vec3i;
  typedef Vecnt<4,int> Vec4i;
  typedef Vecnt<2,unsigned int> Vec2ui;
  typedef Vecnt<3,unsigned int> Vec3ui;
  typedef Vecnt<4,unsigned int> Vec4ui;


  // Matrix
  template<unsigned int n, typename T> class Matnnt;
  typedef Matnnt<3,float>   Mat33f;
  typedef Matnnt<3,double>   Mat33d;
  typedef Matnnt<4,float>   Mat44f;
  typedef Matnnt<4,double>   Mat44d;

  // Quaternions
  template<typename T> class Quatt;
  typedef Quatt<float>  Quatf;
  typedef Quatt<double>  Quatd;
}

namespace nvsg
{
// Object types
class Object;
  // cameras ...
  class Camera;
    class FrustumCamera;
      class ParallelCamera;
      class PerspectiveCamera;
    class MatrixCamera;
  // nodes and node components ...
  class Node;
    // ... groups
    class Group;
      class Billboard;
      class FlipbookAnimation;
      class LOD;
      class Transform;
        class AnimatedTransform;
      class Switch;
    // ... lights
    class LightSource;
      class DirectedLight;
      class PointLight;
      class SpotLight;
    // ... clip plane
    class ClipPlane;
    // ... geometry
    class GeoNode;

  // drawables
  class Drawable;
    // ... primitive sets
    class Primitive;
    class PrimitiveSet;
      // ... independent
      class IndependentPrimitiveSet;
        class PatchesBase;
          class QuadPatches;
          class QuadPatches4x4;
          class RectPatches;
          class TriPatches;
          class TriPatches4;
        class Quads;
        class Triangles;
        class Lines;
        class Points;
      // ... meshed
      class MeshedPrimitiveSet;
        class QuadMeshes;
      // ... stripped
      class StrippedPrimitiveSet;
        class QuadStrips;
        class TriFans;
        class TriStrips;
        class LineStrips;
    // ... shapes
    class Shape;
    // ... UserDrawable
    class UserDrawable;

  // vertex attributes
  class Skin;
  class VertexAttributeSet;
    class AnimatedVertexAttributeSet;

  // Indices
  class IndexSet;

  // states
  class StateAttribute;
    class AlphaTestAttribute;
    class BlendAttribute;
    class CgFx;
    class CgFxEffect;
    class DepthAttribute;
    class Material;
    class LightingAttribute;
    class PrimitiveAttribute;
      class FaceAttribute;
      class LineAttribute;
      class PointAttribute;
    class RTBufferAttribute;
    class RTFx;
    class StencilAttribute;
    class TextureAttribute;
    class TextureAttributeItem;
    class UnlitColorAttribute;
  class StateSet;
  class StateVariant;
  class StatePass;

  // scene attributes
  class SceneAttribute;
    class RTFxSceneAttribute;
    
  // animations
  class AnimationState;
    template<typename T> class Animation;

  template<typename T> class AnimationDescription;
    template<typename T> class FramedAnimationDescription;
    template<typename T> class KeyFramedAnimationDescription;
      template<typename T> class LinearInterpolatedAnimationDescription;

  // maps
  class Map;
    class Bitmap;
  class MapObject;
    class MapArray;
    class MapElement;
    class MapMatrix;
  class MapList;

// additionally required declarations
class DALHost;
typedef std::vector<unsigned int> IndexList;
class IndexMesh;
class RTFxProgram;
class Scene;
class ViewState;
class VertexAttribute;

// buffer types
class Buffer;
class BufferHost;

// texture types
class Texture;
class TextureHost;

  typedef Animation<float>            FloatAnimation;
  typedef Animation<unsigned int>     IndexAnimation;
  typedef Animation<nvmath::Mat33f>   Mat33fAnimation;
  typedef Animation<nvmath::Mat44f>   Mat44fAnimation;
  typedef Animation<nvmath::Quatf>    QuatfAnimation;
  typedef Animation<nvmath::Trafo>    TrafoAnimation;
  typedef Animation<nvmath::Vec2f>    Vec2fAnimation;
  typedef Animation<nvmath::Vec3f>    Vec3fAnimation;
  typedef Animation<nvmath::Vec4f>    Vec4fAnimation;
  typedef Animation<VertexAttribute>  VertexAttributeAnimation;

  typedef AnimationDescription<float>           FloatAnimationDescription;
  typedef AnimationDescription<unsigned int>    IndexAnimationDescription;
  typedef AnimationDescription<nvmath::Mat33f>  Mat33fAnimationDescription;
  typedef AnimationDescription<nvmath::Mat44f>  Mat44fAnimationDescription;
  typedef AnimationDescription<nvmath::Quatf>   QuatfAnimationDescription;
  typedef AnimationDescription<nvmath::Trafo>   TrafoAnimationDescription;
  typedef AnimationDescription<nvmath::Vec2f>   Vec2fAnimationDescription;
  typedef AnimationDescription<nvmath::Vec3f>   Vec3fAnimationDescription;
  typedef AnimationDescription<nvmath::Vec4f>   Vec4fAnimationDescription;
  typedef AnimationDescription<VertexAttribute> VertexAttributeAnimationDescription;

  typedef FramedAnimationDescription<float>            FramedFloatAnimationDescription;
  typedef FramedAnimationDescription<unsigned int>     FramedIndexAnimationDescription;
  typedef FramedAnimationDescription<nvmath::Mat33f>   FramedMat33fAnimationDescription;
  typedef FramedAnimationDescription<nvmath::Mat44f>   FramedMat44fAnimationDescription;
  typedef FramedAnimationDescription<nvmath::Quatf>    FramedQuatfAnimationDescription;
  typedef FramedAnimationDescription<nvmath::Trafo>    FramedTrafoAnimationDescription;
  typedef FramedAnimationDescription<nvmath::Vec2f>    FramedVec2fAnimationDescription;
  typedef FramedAnimationDescription<nvmath::Vec3f>    FramedVec3fAnimationDescription;
  typedef FramedAnimationDescription<nvmath::Vec4f>    FramedVec4fAnimationDescription;
  typedef FramedAnimationDescription<VertexAttribute>  FramedVertexAttributeAnimationDescription;

  typedef KeyFramedAnimationDescription<float>            KeyFramedFloatAnimationDescription;
  typedef KeyFramedAnimationDescription<unsigned int>     KeyFramedIndexAnimationDescription;
  typedef KeyFramedAnimationDescription<nvmath::Mat33f>   KeyFramedMat33fAnimationDescription;
  typedef KeyFramedAnimationDescription<nvmath::Mat44f>   KeyFramedMat44fAnimationDescription;
  typedef KeyFramedAnimationDescription<nvmath::Quatf>    KeyFramedQuatfAnimationDescription;
  typedef KeyFramedAnimationDescription<nvmath::Trafo>    KeyFramedTrafoAnimationDescription;
  typedef KeyFramedAnimationDescription<nvmath::Vec2f>    KeyFramedVec2fAnimationDescription;
  typedef KeyFramedAnimationDescription<nvmath::Vec3f>    KeyFramedVec3fAnimationDescription;
  typedef KeyFramedAnimationDescription<nvmath::Vec4f>    KeyFramedVec4fAnimationDescription;
  typedef KeyFramedAnimationDescription<VertexAttribute>  KeyFramedVertexAttributeAnimationDescription;

  typedef LinearInterpolatedAnimationDescription<float>            LinearInterpolatedFloatAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<nvmath::Mat33f>   LinearInterpolatedMat33fAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<nvmath::Mat44f>   LinearInterpolatedMat44fAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<nvmath::Quatf>    LinearInterpolatedQuatfAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<nvmath::Trafo>    LinearInterpolatedTrafoAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<nvmath::Vec2f>    LinearInterpolatedVec2fAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<nvmath::Vec3f>    LinearInterpolatedVec3fAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<nvmath::Vec4f>    LinearInterpolatedVec4fAnimationDescription;
  typedef LinearInterpolatedAnimationDescription<VertexAttribute>  LinearInterpolatedVertexAttributeAnimationDescription;

// Handle types
  CORE_TYPES( Object, nvutil::HandledObject );
  CORE_TYPES( Camera, Object );
  CORE_TYPES( FrustumCamera, Camera );
  CORE_TYPES( ParallelCamera, FrustumCamera );
  CORE_TYPES( PerspectiveCamera, FrustumCamera );
  CORE_TYPES( MatrixCamera, Camera );
  CORE_TYPES( Node, Object );
  CORE_TYPES( Group, Node );
  CORE_TYPES( Billboard, Group );
  CORE_TYPES( FlipbookAnimation, Group );
  CORE_TYPES( LOD, Group );
  CORE_TYPES( Transform, Group );
  CORE_TYPES( AnimatedTransform, Transform );
  CORE_TYPES( Switch, Group );
  CORE_TYPES( LightSource, Object );
  CORE_TYPES( DirectedLight, LightSource );
  CORE_TYPES( PointLight, LightSource );
  CORE_TYPES( SpotLight, LightSource );
  CORE_TYPES( ClipPlane, Object );
  CORE_TYPES( GeoNode, Node );
  CORE_TYPES( Drawable, Object );
  CORE_TYPES( Primitive, Drawable );
  CORE_TYPES( PrimitiveSet, Drawable );
  CORE_TYPES( IndependentPrimitiveSet, PrimitiveSet );
  CORE_TYPES( PatchesBase, Primitive );
  CORE_TYPES( QuadPatches, PatchesBase );
  CORE_TYPES( QuadPatches4x4, PatchesBase );
  CORE_TYPES( RectPatches, PatchesBase );
  CORE_TYPES( TriPatches, PatchesBase );
  CORE_TYPES( TriPatches4, PatchesBase );
  CORE_TYPES( Quads, IndependentPrimitiveSet );
  CORE_TYPES( Triangles, IndependentPrimitiveSet );
  CORE_TYPES( Lines, IndependentPrimitiveSet );
  CORE_TYPES( Points, IndependentPrimitiveSet );
  CORE_TYPES( MeshedPrimitiveSet, PrimitiveSet );
  CORE_TYPES( QuadMeshes, MeshedPrimitiveSet );
  CORE_TYPES( StrippedPrimitiveSet, PrimitiveSet );
  CORE_TYPES( QuadStrips, StrippedPrimitiveSet );
  CORE_TYPES( TriFans, StrippedPrimitiveSet );
  CORE_TYPES( TriStrips, StrippedPrimitiveSet );
  CORE_TYPES( LineStrips, StrippedPrimitiveSet );
  CORE_TYPES( Shape, Drawable );
  CORE_TYPES( VertexAttributeSet, Object );
  CORE_TYPES( IndexSet, Object );
  CORE_TYPES( AnimatedVertexAttributeSet, VertexAttributeSet );
  CORE_TYPES( Skin, Object );
  CORE_TYPES( StateAttribute, Object );
  CORE_TYPES( CgFx, StateAttribute );
  CORE_TYPES( CgFxEffect, Object );
  CORE_TYPES( Material, StateAttribute );
  CORE_TYPES( RTBufferAttribute, StateAttribute );
  CORE_TYPES( RTFx, StateAttribute );
  CORE_TYPES( StencilAttribute, StateAttribute );
  CORE_TYPES( TextureAttribute, StateAttribute );
  CORE_TYPES( TextureAttributeItem, Object );
  CORE_TYPES( BlendAttribute, StateAttribute );
  CORE_TYPES( AlphaTestAttribute, StateAttribute );
  CORE_TYPES( DepthAttribute, StateAttribute );
  CORE_TYPES( UnlitColorAttribute, StateAttribute );
  CORE_TYPES( PrimitiveAttribute, StateAttribute );
  CORE_TYPES( FaceAttribute, PrimitiveAttribute );
  CORE_TYPES( LightingAttribute, StateAttribute );
  CORE_TYPES( LineAttribute, PrimitiveAttribute );
  CORE_TYPES( PointAttribute, PrimitiveAttribute );
  CORE_TYPES( StateSet, Object );
  CORE_TYPES( StateVariant, Object );
  CORE_TYPES( StatePass, Object );
  CORE_TYPES( AnimationState, Object );
  CORE_TYPES( VertexAttributeAnimation, AnimationState );
  CORE_TYPES( TrafoAnimation, AnimationState );
  CORE_TYPES( IndexAnimation, AnimationState );
  CORE_TYPES( FloatAnimation, AnimationState );
  CORE_TYPES( Vec2fAnimation, AnimationState );
  CORE_TYPES( Vec3fAnimation, AnimationState );
  CORE_TYPES( Vec4fAnimation, AnimationState );
  CORE_TYPES( Mat33fAnimation, AnimationState );
  CORE_TYPES( Mat44fAnimation, AnimationState );
  CORE_TYPES( QuatfAnimation, AnimationState );
  CORE_TYPES( VertexAttributeAnimationDescription, Object );
  CORE_TYPES( TrafoAnimationDescription, Object );
  CORE_TYPES( IndexAnimationDescription, Object );
  CORE_TYPES( FloatAnimationDescription, Object );
  CORE_TYPES( Vec2fAnimationDescription, Object );
  CORE_TYPES( Vec3fAnimationDescription, Object );
  CORE_TYPES( Vec4fAnimationDescription, Object );
  CORE_TYPES( Mat33fAnimationDescription, Object );
  CORE_TYPES( Mat44fAnimationDescription, Object );
  CORE_TYPES( QuatfAnimationDescription, Object );
  CORE_TYPES( FramedVertexAttributeAnimationDescription, VertexAttributeAnimationDescription );
  CORE_TYPES( FramedTrafoAnimationDescription, TrafoAnimationDescription );
  CORE_TYPES( FramedIndexAnimationDescription, IndexAnimationDescription );
  CORE_TYPES( FramedFloatAnimationDescription, FloatAnimationDescription );
  CORE_TYPES( FramedVec2fAnimationDescription, Vec2fAnimationDescription );
  CORE_TYPES( FramedVec3fAnimationDescription, Vec3fAnimationDescription );
  CORE_TYPES( FramedVec4fAnimationDescription, Vec4fAnimationDescription );
  CORE_TYPES( FramedMat33fAnimationDescription, Mat33fAnimationDescription );
  CORE_TYPES( FramedMat44fAnimationDescription, Mat44fAnimationDescription );
  CORE_TYPES( FramedQuatfAnimationDescription, QuatfAnimationDescription );
  CORE_TYPES( KeyFramedVertexAttributeAnimationDescription, VertexAttributeAnimationDescription );
  CORE_TYPES( KeyFramedTrafoAnimationDescription, TrafoAnimationDescription );
  CORE_TYPES( KeyFramedIndexAnimationDescription, IndexAnimationDescription );
  CORE_TYPES( KeyFramedFloatAnimationDescription, FloatAnimationDescription );
  CORE_TYPES( KeyFramedVec2fAnimationDescription, Vec2fAnimationDescription );
  CORE_TYPES( KeyFramedVec3fAnimationDescription, Vec3fAnimationDescription );
  CORE_TYPES( KeyFramedVec4fAnimationDescription, Vec4fAnimationDescription );
  CORE_TYPES( KeyFramedMat33fAnimationDescription, Mat33fAnimationDescription );
  CORE_TYPES( KeyFramedMat44fAnimationDescription, Mat44fAnimationDescription );
  CORE_TYPES( KeyFramedQuatfAnimationDescription, QuatfAnimationDescription );
  CORE_TYPES( LinearInterpolatedVertexAttributeAnimationDescription, KeyFramedVertexAttributeAnimationDescription );
  CORE_TYPES( LinearInterpolatedTrafoAnimationDescription, KeyFramedTrafoAnimationDescription );
  CORE_TYPES( LinearInterpolatedFloatAnimationDescription, KeyFramedFloatAnimationDescription );
  CORE_TYPES( LinearInterpolatedVec2fAnimationDescription, KeyFramedVec2fAnimationDescription );
  CORE_TYPES( LinearInterpolatedVec3fAnimationDescription, KeyFramedVec3fAnimationDescription );
  CORE_TYPES( LinearInterpolatedVec4fAnimationDescription, KeyFramedVec4fAnimationDescription );
  CORE_TYPES( LinearInterpolatedMat33fAnimationDescription, KeyFramedMat33fAnimationDescription );
  CORE_TYPES( LinearInterpolatedMat44fAnimationDescription, KeyFramedMat44fAnimationDescription );
  CORE_TYPES( LinearInterpolatedQuatfAnimationDescription, KeyFramedQuatfAnimationDescription );
  CORE_TYPES( MapObject, Object );
  CORE_TYPES( MapArray, MapObject );
  CORE_TYPES( MapElement, MapObject );
  CORE_TYPES( MapMatrix, MapObject );
  CORE_TYPES( MapList, Object );
  CORE_TYPES( Scene, Object );
  CORE_TYPES( SceneAttribute, Object );
  CORE_TYPES( RTFxSceneAttribute, SceneAttribute );
  CORE_TYPES( ViewState, nvutil::HandledObject );
  CORE_TYPES( DALHost, nvutil::HandledObject );
  CORE_TYPES( RTFxProgram, Object );
  CORE_TYPES( Buffer, nvutil::HandledObject );
  CORE_TYPES( BufferHost, Buffer );
  CORE_TYPES( Texture, nvutil::HandledObject );
  CORE_TYPES( TextureHost, Texture );
  CORE_TYPES( UserDrawable, Drawable );

} // namespace nvsg

namespace nvgl 
{
  class BufferGL;
  class TextureGL;
  class DisplayListGL;

  CORE_TYPES( BufferGL, nvsg::Buffer );
  CORE_TYPES( TextureGL, nvsg::Texture );
  CORE_TYPES( DisplayListGL, nvutil::HandledObject );
} // namespace nvgl

namespace nvtraverser
{
  class CameraUpdateSemaphore; 
  class SpotLightUpdateSemaphore;

  CORE_TYPES( CameraUpdateSemaphore, nvsg::Object );
  CORE_TYPES( SpotLightUpdateSemaphore, nvsg::Object );
}

namespace nvui
{
  class RendererOptions;

  CORE_TYPES( RendererOptions, nvutil::HandledObject );
}

namespace nvutil
{
  OBJECT_TRAITS( nvsg::Object, nvutil::HandledObject );
  OBJECT_TRAITS( nvsg::Camera, nvsg::Object );
  OBJECT_TRAITS( nvsg::FrustumCamera, nvsg::Camera );
  OBJECT_TRAITS( nvsg::ParallelCamera, nvsg::FrustumCamera );
  OBJECT_TRAITS( nvsg::PerspectiveCamera, nvsg::FrustumCamera );
  OBJECT_TRAITS( nvsg::MatrixCamera, nvsg::Camera );
  OBJECT_TRAITS( nvsg::Node, nvsg::Object );
  OBJECT_TRAITS( nvsg::Group, nvsg::Node );
  OBJECT_TRAITS( nvsg::Billboard, nvsg::Group );
  OBJECT_TRAITS( nvsg::FlipbookAnimation, nvsg::Group );
  OBJECT_TRAITS( nvsg::LOD, nvsg::Group );
  OBJECT_TRAITS( nvsg::Transform, nvsg::Group );
  OBJECT_TRAITS( nvsg::AnimatedTransform, nvsg::Transform );
  OBJECT_TRAITS( nvsg::Switch, nvsg::Group );
  OBJECT_TRAITS( nvsg::LightSource, nvsg::Object );
  OBJECT_TRAITS( nvsg::DirectedLight, nvsg::LightSource );
  OBJECT_TRAITS( nvsg::PointLight, nvsg::LightSource );
  OBJECT_TRAITS( nvsg::SpotLight, nvsg::LightSource );
  OBJECT_TRAITS( nvsg::ClipPlane, nvsg::Object );
  OBJECT_TRAITS( nvsg::GeoNode, nvsg::Node );
  OBJECT_TRAITS( nvsg::Drawable, nvsg::Object );
  OBJECT_TRAITS( nvsg::Primitive, nvsg::Drawable );
  OBJECT_TRAITS( nvsg::PrimitiveSet, nvsg::Drawable );
  OBJECT_TRAITS( nvsg::IndependentPrimitiveSet, nvsg::PrimitiveSet );
  OBJECT_TRAITS( nvsg::PatchesBase, nvsg::Primitive );
  OBJECT_TRAITS( nvsg::QuadPatches, nvsg::PatchesBase );
  OBJECT_TRAITS( nvsg::QuadPatches4x4, nvsg::PatchesBase );
  OBJECT_TRAITS( nvsg::RectPatches, nvsg::PatchesBase );
  OBJECT_TRAITS( nvsg::TriPatches, nvsg::PatchesBase );
  OBJECT_TRAITS( nvsg::TriPatches4, nvsg::PatchesBase );
  OBJECT_TRAITS( nvsg::Quads, nvsg::IndependentPrimitiveSet );
  OBJECT_TRAITS( nvsg::Triangles, nvsg::IndependentPrimitiveSet );
  OBJECT_TRAITS( nvsg::Lines, nvsg::IndependentPrimitiveSet );
  OBJECT_TRAITS( nvsg::Points, nvsg::IndependentPrimitiveSet );
  OBJECT_TRAITS( nvsg::MeshedPrimitiveSet, nvsg::PrimitiveSet );
  OBJECT_TRAITS( nvsg::QuadMeshes, nvsg::MeshedPrimitiveSet );
  OBJECT_TRAITS( nvsg::StrippedPrimitiveSet, nvsg::PrimitiveSet );
  OBJECT_TRAITS( nvsg::QuadStrips, nvsg::StrippedPrimitiveSet );
  OBJECT_TRAITS( nvsg::TriFans, nvsg::StrippedPrimitiveSet );
  OBJECT_TRAITS( nvsg::TriStrips, nvsg::StrippedPrimitiveSet );
  OBJECT_TRAITS( nvsg::LineStrips, nvsg::StrippedPrimitiveSet );
  OBJECT_TRAITS( nvsg::UserDrawable, nvsg::Drawable );
  OBJECT_TRAITS( nvsg::Shape, nvsg::Drawable );
  OBJECT_TRAITS( nvsg::VertexAttributeSet, nvsg::Object );
  OBJECT_TRAITS( nvsg::IndexSet, nvsg::Object );
  OBJECT_TRAITS( nvsg::AnimatedVertexAttributeSet, nvsg::VertexAttributeSet );
  OBJECT_TRAITS( nvsg::Skin, nvsg::Object );
  OBJECT_TRAITS( nvsg::StateAttribute, nvsg::Object );
  OBJECT_TRAITS( nvsg::CgFx, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::CgFxEffect, nvsg::Object );
  OBJECT_TRAITS( nvsg::Material, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::RTBufferAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::RTFx, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::StencilAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::TextureAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::TextureAttributeItem, nvsg::Object );
  OBJECT_TRAITS( nvsg::BlendAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::AlphaTestAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::DepthAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::UnlitColorAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::PrimitiveAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::FaceAttribute, nvsg::PrimitiveAttribute );
  OBJECT_TRAITS( nvsg::LightingAttribute, nvsg::StateAttribute );
  OBJECT_TRAITS( nvsg::LineAttribute, nvsg::PrimitiveAttribute );
  OBJECT_TRAITS( nvsg::PointAttribute, nvsg::PrimitiveAttribute );
  OBJECT_TRAITS( nvsg::StateSet, nvsg::Object );
  OBJECT_TRAITS( nvsg::StateVariant, nvsg::Object );
  OBJECT_TRAITS( nvsg::StatePass, nvsg::Object );
  OBJECT_TRAITS( nvsg::AnimationState, nvsg::Object );
  OBJECT_TRAITS( nvsg::VertexAttributeAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::TrafoAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::IndexAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::FloatAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::Vec2fAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::Vec3fAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::Vec4fAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::Mat33fAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::Mat44fAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::QuatfAnimation, nvsg::AnimationState );
  OBJECT_TRAITS( nvsg::VertexAttributeAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::TrafoAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::IndexAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::FloatAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::Vec2fAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::Vec3fAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::Vec4fAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::Mat33fAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::Mat44fAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::QuatfAnimationDescription, nvsg::Object );
  OBJECT_TRAITS( nvsg::FramedVertexAttributeAnimationDescription, nvsg::VertexAttributeAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedTrafoAnimationDescription, nvsg::TrafoAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedIndexAnimationDescription, nvsg::IndexAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedFloatAnimationDescription, nvsg::FloatAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedVec2fAnimationDescription, nvsg::Vec2fAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedVec3fAnimationDescription, nvsg::Vec3fAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedVec4fAnimationDescription, nvsg::Vec4fAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedMat33fAnimationDescription, nvsg::Mat33fAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedMat44fAnimationDescription, nvsg::Mat44fAnimationDescription );
  OBJECT_TRAITS( nvsg::FramedQuatfAnimationDescription, nvsg::QuatfAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedVertexAttributeAnimationDescription, nvsg::VertexAttributeAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedTrafoAnimationDescription, nvsg::TrafoAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedIndexAnimationDescription, nvsg::IndexAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedFloatAnimationDescription, nvsg::FloatAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedVec2fAnimationDescription, nvsg::Vec2fAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedVec3fAnimationDescription, nvsg::Vec3fAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedVec4fAnimationDescription, nvsg::Vec4fAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedMat33fAnimationDescription, nvsg::Mat33fAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedMat44fAnimationDescription, nvsg::Mat44fAnimationDescription );
  OBJECT_TRAITS( nvsg::KeyFramedQuatfAnimationDescription, nvsg::QuatfAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedVertexAttributeAnimationDescription, nvsg::KeyFramedVertexAttributeAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedTrafoAnimationDescription, nvsg::KeyFramedTrafoAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedFloatAnimationDescription, nvsg::KeyFramedFloatAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedVec2fAnimationDescription, nvsg::KeyFramedVec2fAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedVec3fAnimationDescription, nvsg::KeyFramedVec3fAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedVec4fAnimationDescription, nvsg::KeyFramedVec4fAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedMat33fAnimationDescription, nvsg::KeyFramedMat33fAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedMat44fAnimationDescription, nvsg::KeyFramedMat44fAnimationDescription );
  OBJECT_TRAITS( nvsg::LinearInterpolatedQuatfAnimationDescription, nvsg::KeyFramedQuatfAnimationDescription );
  OBJECT_TRAITS( nvsg::MapObject, nvsg::Object );
  OBJECT_TRAITS( nvsg::MapArray, nvsg::MapObject );
  OBJECT_TRAITS( nvsg::MapElement, nvsg::MapObject );
  OBJECT_TRAITS( nvsg::MapMatrix, nvsg::MapObject );
  OBJECT_TRAITS( nvsg::MapList, nvsg::Object );
  OBJECT_TRAITS( nvsg::Scene, nvsg::Object );
  OBJECT_TRAITS( nvsg::SceneAttribute, nvsg::Object );
  OBJECT_TRAITS( nvsg::RTFxSceneAttribute, nvsg::SceneAttribute );
  OBJECT_TRAITS( nvsg::ViewState, nvutil::HandledObject );
  OBJECT_TRAITS( nvsg::DALHost, nvutil::HandledObject );
  OBJECT_TRAITS( nvsg::RTFxProgram, nvsg::Object );
  OBJECT_TRAITS( nvsg::Buffer, nvutil::HandledObject );
  OBJECT_TRAITS( nvsg::BufferHost, nvsg::Buffer );
  OBJECT_TRAITS( nvsg::Texture, nvutil::HandledObject );
  OBJECT_TRAITS( nvsg::TextureHost, nvsg::Texture );

  // namespace nvgl
  OBJECT_TRAITS( nvgl::BufferGL, nvsg::Buffer );
  OBJECT_TRAITS( nvgl::TextureGL, nvsg::Texture );
  OBJECT_TRAITS( nvgl::DisplayListGL, nvutil::HandledObject );

  // namespace nvtraverser
  OBJECT_TRAITS( nvtraverser::CameraUpdateSemaphore, nvsg::Object );
  OBJECT_TRAITS( nvtraverser::SpotLightUpdateSemaphore, nvsg::Object );

  // namespace nvui
  OBJECT_TRAITS( nvui::RendererOptions, nvutil::HandledObject );

} // namespace nvutil

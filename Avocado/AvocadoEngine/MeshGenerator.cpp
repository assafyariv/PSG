#include "MeshGenerator.h"

#include <nvmath/nvmath.h>
#include <nvsg/ViewState.h>
#include <nvsg/Face.h>
#include <nvsg/FaceAttribute.h>
#include <nvsg/GeoNode.h>
#include <nvsg/LOD.h>
#include <nvsg/Material.h>
#include <nvsg/Node.h>
#include <nvsg/PointLight.h>
#include <nvsg/SpotLight.h>
#include <nvsg/StateSet.h>
#include <nvsg/Switch.h>
#include <nvsg/TextureAttribute.h>
#include <nvsg/Transform.h>
#include <nvsg/TriPatches4.h>
#include <nvsg/QuadPatches4x4.h>
#include <nvsg/BlendAttribute.h>
#include <nvutil/Tools.h>
#include <nvutil/PlugIn.h>
#include <nvsg/PlugInterface.h>
#include <nvsg/PlugInterfaceID.h>

#include "nvsg/DirectedLight.h"
#include "SceneFunctions.h"
#include <vector>

#include "nvutil/DbgNew.h" // this must be the last include

using namespace nvmath;
using namespace nvsg;
using namespace nvutil;
using namespace std;

namespace nvutil
{
  namespace
  {
    //! Helper function to calculate the face normal of the face a, b, c
    Vec3f calculateFaceNormal( const Vec3f &a, const Vec3f &b, const Vec3f &c )
    {
      Vec3f d1 = b - a;
      Vec3f d2 = c - a;
      Vec3f n  = d1 ^ d2;
      n.normalize();
      return n;
    }

    StateSetSharedPtr createPatchesStateSet( const std::string & tessFile, const std::vector<std::string> & searchPaths )
    {
      // Create the tesselation shader first
      CgFxSharedPtr tessCgFx = CgFx::create();
      {
        CgFxEffectWriteLock effect( CgFxReadLock( tessCgFx )->getEffect() );
        std::string file;
        std::string err;
        if (   !FindFileFirst( tessFile, searchPaths, file )
            || !effect->createFromFile( file, searchPaths, err ) )
        {
          return( StateSetSharedPtr() );
        }
      }

      // Create a Material
      MaterialSharedPtr material = Material::create();
      {
        MaterialWriteLock m(material);
        m->setAmbientColor(  Vec3f(0.0f, 0.0f, 0.0f) );
        m->setDiffuseColor(  Vec3f(0.0f, 0.0f, 1.0f) );
        m->setEmissiveColor( Vec3f(0.0f, 0.0f, 0.0f) );
        m->setSpecularColor( Vec3f(0.3f, 0.3f, 0.3f) );
        m->setSpecularExponent( 10.f );
        m->setOpacity( 1.0f );
      }

      // Create a StateSet and attach material and shader
      StateSetSharedPtr stateSet = StateSet::create();
      {
        StateSetWriteLock ss( stateSet );
        ss->addAttribute( material );
        ss->addAttribute( tessCgFx );
      }

      return( stateSet );
    }

    //! Helper function to setup the faces, vertices, normals and texccords of a tessellated plane
    //with \a subdiv subdivisions and a transformation-matrix transf
    void setupTessellatedPlane( unsigned int subdiv, const Mat44f &transf,
                                vector < Face3 > &faces, vector < Vec3f > &vertices,
                                vector < Vec3f > &normals, vector < Vec2f > &texcoords,
                                vector <unsigned int> &indices )
    {
      float step = 2.0f/(float)(subdiv + 1);
      unsigned int row = subdiv + 2;
      unsigned int offset = checked_cast<unsigned int>(vertices.size());


      // This is expensive do it once outside the loops!
      Mat44f transfIT;
      bool validInverse = invert( transf, transfIT );
      if (validInverse)
      {
        transfIT = ~transfIT;
      }

      float y = -1.0f;
      for ( unsigned int sY = 0; sY < row; sY++ )
      {
        float x = -1.0f;
        for ( unsigned int sX = 0; sX < row; sX++ )
        {
          vertices.push_back( Vec3f(Vec4f( x, y, 0.0f, 1.0f ) * transf ) ); 
          Vec3f normal(0.0f, 0.0f, 1.0f); // Initialize to some valid normal in case the transf matrix cannot be inverted.
          if ( validInverse )
          {
            normal = Vec3f(Vec4f( 0.0f, 0.0f, 1.0f, 0.0f) * transfIT);
            normal.normalize();
          }
          normals.push_back( normal );
          texcoords.push_back( Vec2f( x * 0.5f + 0.5f, y * 0.5f + 0.5f ) );  
          x += step;
        }
        y += step;
      }

      for ( unsigned int sY = 0; sY <= subdiv; sY++ )
      {
        for ( unsigned int sX = 0; sX <= subdiv; sX++ )
        {
          indices.push_back( offset + sX + sY * row );
          indices.push_back( offset + sX + 1 + sY * row );
          indices.push_back( offset + sX + 1 + (sY+1) * row );

          indices.push_back( offset + sX + 1 + (sY+1) * row );
          indices.push_back( offset + sX + (sY+1) * row );
          indices.push_back( offset + sX + sY * row );

          Face3 f;

          f[0] = offset + sX + sY * row;
          f[1] = offset + sX + 1 + sY * row;
          f[2] = offset + sX + 1 + (sY+1) * row;
          faces.push_back( f );

          f[0] = offset + sX + 1 + (sY+1) * row;
          f[1] = offset + sX + (sY+1) * row;
          f[2] = offset + sX + sY * row;
          faces.push_back( f );
        }
      }
    }
  }

  // ===========================================================================
  
  DrawableSharedPtr createQuadSet( unsigned int m, unsigned int n, const float size, const float gap )
  {
    NVSG_ASSERT( m >= 1 && n >= 1 && "createQuadSet(): m and n both have to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals and faces for n X m tiles
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;
    vector<Face4> faces;

    const int size_v = 4 * m * n;
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( m * n );

    // lower-left corner of the current tile
    float dy = 0.0f; 

    // n tiles in x-direction
    for( unsigned int i = 0; i < m; ++i )
    {
      // reset x-offset
      float dx = 0.0f;

      // n tiles in x-direction
      for( unsigned int j = 0; j < n; ++j )
      {
        // add 4 vertices
        Vec3f a = Vec3f( dx       , dy       ,              0.0f );
        Vec3f b = Vec3f( dx + size, dy       , (float)j/(float)n );
        Vec3f c = Vec3f( dx + size, dy + size, (float)j/(float)n );
        Vec3f d = Vec3f( dx       , dy + size,              0.0f );
        vertices.push_back( a );
        vertices.push_back( b );
        vertices.push_back( c );
        vertices.push_back( d );

        unsigned int first_index = ( i * n + j ) * 4;
        Vec3f nv = calculateFaceNormal( a, b, d );
        
        // Setup normals and faces
        for( unsigned int k = 0; k < 4; ++k )
        {
          normals.push_back(nv);
          indices.push_back( first_index + k );
        }

        dx += size + gap;
      }
      dy += size + gap;
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_QUADS );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }    

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createQuadStrip( unsigned int n, float height, float radius )
  {
    NVSG_ASSERT( n >= 1 && "createQuadStrip(): n has to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals and indices
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;

    const int size_v = 2*(n+1);
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );

    // for n quads, we need 2*(n+1) vertices, so run to including n
    for( unsigned int i = 0; i <= n; ++i )
    {
      float phi = PI * ( 1.0f - (float)i/(float)n );
      float x = cos(phi);
      float z = sin(phi);
      Vec3f v( x, 0.0f, z );

      normals.push_back( v );
      normals.push_back( v );
      vertices.push_back( v * radius + Vec3f( 0.0f, height, 0.0f ) );
      vertices.push_back( v * radius );
      
      indices.push_back( i * 2 );
      indices.push_back( i * 2 + 1 );

    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas(vasPtr);
      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_QUAD_STRIP );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createTriSet( unsigned int m, unsigned int n, const float size, const float gap )
  {
    NVSG_ASSERT( m >= 1 && n >= 1 && "createTriSet(): m and n both have to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals and faces for n X m tiles
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;
    vector<Face3> faces;

    const int size_v = 3 * m * n;
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( m * n );

    // lower-left corner of the current tile
    float dy = 0.0f; 

    // m tiles in y-direction
    for( unsigned int i = 0; i < m; ++i )
    {
      // reset x-offset
      float dx = 0.0f;

      // n tiles in x-direction
      for( unsigned int j = 0; j < n; ++j )
      {
        // add 3 vertices
        Vec3f a = Vec3f( dx       , dy       ,              0.0f );
        Vec3f b = Vec3f( dx + size, dy       , (float)j/(float)n );
        Vec3f c = Vec3f( dx       , dy + size,              0.0f );
        vertices.push_back( a );
        vertices.push_back( b );
        vertices.push_back( c );

        // Setup faces and normals
        unsigned int first_index = ( i * n + j ) * 3;
        Vec3f nf = calculateFaceNormal( a, b, c );

        for( unsigned int k = 0; k < 3; ++k )
        {
          normals.push_back(nf);
          indices.push_back( first_index + k );
        }

        dx += size + gap;
      }
      dy += size + gap;
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas(vasPtr);
      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;                  
    }

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createTriFan( unsigned int n, const float radius, const float elevation )
  {
    NVSG_ASSERT( n > 1 && "createTriFan(): n has to be at least 2." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices and faces for n triangles
    vector<Vec3f> vertices;
    vector<unsigned int> indices;
    
    vertices.reserve( n + 2 );
    indices.reserve( n + 2 );

    vertices.push_back( Vec3f( 0.0f, 0.0f, elevation ) );
    vertices.push_back( Vec3f( radius, 0.0f, 0.0f ) );

    indices.push_back(0);
    indices.push_back(1);

    // n triangles ( i from 1 to n ! )
    for( unsigned int i = 1; i <= n; ++i )
    {
      float phi = PI * ((float)i/(float)n);
      float x = radius * cos(phi);
      float y = radius * sin(phi);
      vertices.push_back( Vec3f(x, y, 0.f) );
      indices.push_back( i + 1 );
    }

    // Calculate normals
    vector<Vec3f> normals;
    normals.resize(vertices.size());
    for( size_t i = 0; i < normals.size(); ++i )
    {
      normals[i] = Vec3f( 0.0f, 0.0f, 0.0f );
    }

    for( unsigned int i = 0; i < n; ++i )
    {
      // calculate face normal for face i
      Vec3f fn = calculateFaceNormal( vertices.at(0), vertices.at(i+1), vertices.at(i+2) );
      // accumulate in vertex normals
      normals.at(0)   += fn;
      normals.at(i+1) += fn;
      normals.at(i+2) += fn;
    }

    for( size_t i = 0; i < normals.size(); ++i )
    {
      normals[i].normalize();
    }


    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas(vasPtr);
      vas->setVertices( &vertices[0], n + 2 );
      vas->setNormals( &normals[0], n + 2 );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLE_FAN );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createTriStrip( unsigned int rows, unsigned int columns, float width, float height )
  {
    NVSG_ASSERT( rows >= 1 && columns >= 1 && "createTriStrip(): rows and columns both have to be at least 1." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // Setup vertices, normals and indices
    vector<Vec3f> vertices;
    vector<Vec3f> normals;
    vector<unsigned int> indices;
    std::vector< IndexList > indexlist;

    unsigned int size_v = ( rows + 1 ) * ( columns + 1 );
    vertices.reserve( size_v );
    normals.reserve( size_v );
    indices.reserve( size_v );
    
    for( unsigned int i = 0; i <= rows; ++i )
    {
      for( unsigned int j = 0; j <= columns; ++j )
      {
        vertices.push_back( Vec3f( j * width, i * height, 0.0f ) );
        normals.push_back( Vec3f( 0.0f, 0.0f, 1.0f ) );
      }
    }

    for( unsigned int i = 0; i < rows; ++i )
    {
      for( unsigned int j = 0; j <= columns; ++j )
      {
        indices.push_back( i * ( columns + 1 ) + j + columns + 1 );
        indices.push_back( i * ( columns + 1 ) + j );
      }
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLE_STRIP );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    /*
    FaceAttributeSharedPtr faceHdl = FaceAttribute::create();
    {
      FaceAttributeWriteLock(faceHdl)->setTwoSidedLighting( true );
      FaceAttributeWriteLock(faceHdl)->setCullMode( false, false );
    }
    */

    return drawablePtr;
  }

  // ===========================================================================

  GeoNodeSharedPtr createTriPatches4( const std::vector<std::string> & searchPaths, unsigned int m, unsigned int n, const Vec3f & size, const Vec2f & offset )
  {
    // Create a StateSet with a Material and a CgFx first
    StateSetSharedPtr stateSet = createPatchesStateSet( "phongTessPatch10V.fx", searchPaths );
    if ( ! stateSet )
    {
      return( GeoNodeSharedPtr() );
    }

    // Set up the tri patch:
    //  9
    //  7 8
    //  4 5 6
    //  0 1 2 3
    //
    Vec3f verts[10] =
    { 
      Vec3f(size[0]*0.0f/4.0f, size[1]*0.0f/4.0f, size[2]*0.0f/4.0f), // 0 
      Vec3f(size[0]*1.0f/4.0f, size[1]*0.0f/4.0f, size[2]*1.0f/4.0f), // 1
      Vec3f(size[0]*2.0f/4.0f, size[1]*0.0f/4.0f, size[2]*1.0f/4.0f), // 2
      Vec3f(size[0]*3.0f/4.0f, size[1]*0.0f/4.0f, size[2]*0.0f/4.0f), // 3
      Vec3f(size[0]*0.0f/4.0f, size[1]*1.0f/4.0f, size[2]*1.0f/4.0f), // 4
      Vec3f(size[0]*1.0f/4.0f, size[1]*1.0f/4.0f, size[2]*2.0f/4.0f), // 5
      Vec3f(size[0]*2.0f/4.0f, size[1]*1.0f/4.0f, size[2]*1.0f/4.0f), // 6
      Vec3f(size[0]*0.0f/4.0f, size[1]*2.0f/4.0f, size[2]*1.0f/4.0f), // 7
      Vec3f(size[0]*1.0f/4.0f, size[1]*2.0f/4.0f, size[2]*1.0f/4.0f), // 8
      Vec3f(size[0]*0.0f/4.0f, size[1]*3.0f/4.0f, size[2]*0.0f/4.0f)  // 9
    };

    std::vector<Vec3f> vertices;

    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    for( unsigned int i = 0; i < m; ++i )
    {
      for( unsigned int j = 0; j < n; ++j )
      {
        for( unsigned int k = 0; k < 10; ++k )
        {
          vertices.push_back( verts[k] + Vec3f(x, y, z) );
        }
        x += offset[0];
      }
      y += offset[1];
      x = 0.0f;
    }

    // Create a VertexAttributeSet
    VertexAttributeSetSharedPtr vas = VertexAttributeSet::create();
    VertexAttributeSetWriteLock( vas )->setVertices( &vertices[0], checked_cast<unsigned int>(vertices.size()) );

    // Create a TriPatches4
    TriPatches4SharedPtr triPatches = TriPatches4::create();
    TriPatches4WriteLock( triPatches )->setVertexAttributeSet( vas );

    // Create a GeoNode and add the geometry
    GeoNodeSharedPtr geoNode = GeoNode::create();
    GeoNodeWriteLock( geoNode )->addDrawable( stateSet, triPatches );

    return geoNode;
  }

  // ===========================================================================

  GeoNodeSharedPtr createQuadPatches4x4( const std::vector<std::string> & searchPaths, unsigned int n, unsigned int m, const Vec2f & offset )
  {  
    // Create a StateSet with a Material and a CgFx
    StateSetSharedPtr stateSet = createPatchesStateSet( "phongTessPatch16V.fx", searchPaths );
    if ( ! stateSet )
    {
      return( GeoNodeSharedPtr() );
    }

    const float r = std::min<float>( offset[0], offset[1] )/2.0f * 0.75f;  
    const float dy = r;  // distance between rows of vertices

    // For a cylinder, we need four quad patches, each extruding an approximation
    // of a quarter of a circle

    // For a bezier curve to approximate a quarter of a circle, we need to use a 
    // cubic bezier curve with inner points at a distance x of r*4(sqrt(2)-1)/3 from
    // the end points in the tangent direction at the end points:
    //
    //   0--d--1
    //   |      \
    //   r       2
    //   |       |
    //   *       3
    //
    
    // Distance of the boundary control points to the corner control points
    const float d = r * 4.0f * (sqrt(2.0f)-1.0f)/3.0f;

    // A zero for nice alignment
    const float o = 0.0f;
    
    // First construct the bezier curves to be extruded
    Vec3f bezierCurves[16];
    bezierCurves[ 0] = Vec3f( o, o, r ); //
    bezierCurves[ 1] = Vec3f( d, o, r ); //
    bezierCurves[ 2] = Vec3f( r, o, d ); //
    bezierCurves[ 3] = Vec3f( r, o, o ); //        |
    bezierCurves[ 4] = Vec3f( r, o, o ); //        |
    bezierCurves[ 5] = Vec3f( r, o,-d ); //      9 786
    bezierCurves[ 6] = Vec3f( d, o,-r ); //     A  |  5
    bezierCurves[ 7] = Vec3f( o, o,-r ); //  --BC-----34--x
    bezierCurves[ 8] = Vec3f( o, o,-r ); //     D  |  2
    bezierCurves[ 9] = Vec3f(-d, o,-r ); //      E F0 1 
    bezierCurves[10] = Vec3f(-r, o,-d ); //        | 
    bezierCurves[11] = Vec3f(-r, o, o ); //        z 
    bezierCurves[12] = Vec3f(-r, o, o ); // 
    bezierCurves[13] = Vec3f(-r, o, d ); //  
    bezierCurves[14] = Vec3f(-d, o, r ); //  
    bezierCurves[15] = Vec3f( o, o, r ); // 

    // Extrude the curves to form the patches
    //            y  C D E F  
    // one patch: |  8 9 A B  
    //            |  4 5 6 7  
    //            O  0 1 2 3
    vector< Vec3f > verts;
    for( unsigned int i = 0; i < 4; ++i )
    {
      // patch i
      for( unsigned int j = 0; j < 4; ++j )
      {
        // row j
        for( unsigned int k = 0; k < 4; ++k )
        {
          // column k
          verts.push_back( bezierCurves[i*4+k] + Vec3f( o, dy*j, o ));
        }
      }
    }

    // Generate n x m cylinders
    vector< Vec3f > vertices;
    for( unsigned int i = 0; i < n; ++i )
    {
      for( unsigned int j = 0; j < m; ++j )
      {
        float x = i * offset[0];
        float z = j * offset[1];
        for( unsigned int k = 0; k < verts.size(); ++k )
        {
          vertices.push_back( verts.at( k ) + Vec3f( x, o, z ) );
        }        
      }
    }

    // Create a VertexAttributeSet
    VertexAttributeSetSharedPtr vas = VertexAttributeSet::create();
    VertexAttributeSetWriteLock( vas )->setVertices( &vertices[0], checked_cast<unsigned int>( vertices.size() ) );

    // Create a PrimitiveSet
    QuadPatches4x4SharedPtr patches = QuadPatches4x4::create();
    QuadPatches4x4WriteLock( patches )->setVertexAttributeSet( vas );

    // Create a GeoNode and add the geometry
    GeoNodeSharedPtr geoNode = GeoNode::create();
    GeoNodeWriteLock( geoNode )->addDrawable( stateSet, patches );

    return geoNode;
  }

  // ===========================================================================
#define ONE_THIRD 1.0f/3.0f
#define TWO_THIRD 2.0f/3.0f

  DrawableSharedPtr createCube( bool applyCubeMapTexCoords)
  {
    // Right handed model coordinates.
    // Vertex numbering and coordinate setup match the 3-bit pattern: (z << 2) | (y << 1) | x
    // ASCII art:
    /*
         y

         2--------------3
        /              /|
       / |            / |
      6--------------7  |
      |  |           |  |
      |              |  |
      |  |           |  |
      |  0 -  -  -  -| -1  x
      | /            | /
      |/             |/
      4--------------5
     z
    */
    
    // Setup vertices
    static const Vec3f vertices[8] =
    { 
      Vec3f( -1.0f, -1.0f, -1.0f ), // 0
      Vec3f(  1.0f, -1.0f, -1.0f ), // 1
      Vec3f( -1.0f,  1.0f, -1.0f ), // 2
      Vec3f(  1.0f,  1.0f, -1.0f ), // 3
      Vec3f( -1.0f, -1.0f,  1.0f ), // 4
      Vec3f(  1.0f, -1.0f,  1.0f ), // 5
      Vec3f( -1.0f,  1.0f,  1.0f ), // 6
      Vec3f(  1.0f,  1.0f,  1.0f )  // 7
    };

    // Setup faces
    static const Face3 faces[12] =
    {
      {1, 0, 2}, {2, 3, 1}, // back
      {0, 4, 6}, {6, 2, 0}, // left
      {0, 1, 5}, {5, 4, 0}, // bottom
      {4, 5, 7}, {7, 6, 4}, // front
      {5, 1, 3}, {3, 7, 5}, // right
      {3, 2, 6}, {6, 7, 3}  // top 
    };

    // Setup texture coordinates
    static const Vec2f texcoords[4] =
    {
      Vec2f(0.0f, 0.0f),
      Vec2f(1.0f, 0.0f),
      Vec2f(1.0f, 1.0f),
      Vec2f(0.0f, 1.0f)
    };

	// FACE One texture coordinates
    static const Vec2f texcoordsF1[4] =
    {
		  
      Vec2f(ONE_THIRD, 0.25f),
	  Vec2f(TWO_THIRD, 0.25f),
	  Vec2f(TWO_THIRD, 0.0f),
      Vec2f(ONE_THIRD, 0.0f)
      
    };

	// FACE Two texture coordinates
	static const Vec2f texcoordsF2[4] =
    {
      /*Vec2f(ONE_THIRD, 0.25f),
      Vec2f(TWO_THIRD, 0.25f),
      Vec2f(TWO_THIRD, 0.5f),
      Vec2f(ONE_THIRD, 0.5f)*/
	  Vec2f(TWO_THIRD, 0.25f),
      Vec2f(ONE_THIRD, 0.25f),
      Vec2f(ONE_THIRD, 0.5f),
      Vec2f(TWO_THIRD, 0.5f)

    };

	// FACE Three texture coordinates
	static const Vec2f texcoordsF5[4] =
    {
      /*Vec2f(0.33f, 0.5f),
      Vec2f(0.66f, 0.5f),
      Vec2f(0.66f, 0.75f),
      Vec2f(0.33f, 0.75f)*/
      Vec2f(TWO_THIRD, 0.5f),		
      Vec2f(ONE_THIRD, 0.5f),
	  Vec2f(ONE_THIRD, 0.75f),
      Vec2f(TWO_THIRD, 0.75f)
      

    };
	// FACE Four texture coordinates
	static const Vec2f texcoordsF4[4] =
    {
	/*  Vec2f(0.66f, 1.0f),
      Vec2f(0.33f, 1.0f),
      Vec2f(0.33f, 0.75f),
      Vec2f(0.66f, 0.75f)*/
		 Vec2f(ONE_THIRD, 1.0f),
      Vec2f(TWO_THIRD, 1.0f),
      Vec2f(TWO_THIRD, 0.75f),
      Vec2f(ONE_THIRD, 0.75f)
     
    };
	// FACE Five texture coordinates
	static const Vec2f texcoordsF6[4] =
    {
   /*   Vec2f(0.0f, 0.5f),
      Vec2f(0.33f, 0.5f),
      Vec2f(0.33f, 0.75f),
      Vec2f(0.0f, 0.75f)*/
	  Vec2f(ONE_THIRD, 0.5f),
      Vec2f(0.0f, 0.5f),
      Vec2f(0.0f, 0.75f),      
      Vec2f(ONE_THIRD, 0.75f)
    };
	// FACE Six texture coordinates
	static const Vec2f texcoordsF3[4] =
    {
      /*Vec2f(0.66f, 0.5f),
      Vec2f(1.0f, 0.5f),
      Vec2f(1.0f, 0.75f),
      Vec2f(0.66f, 0.75f)*/

      Vec2f(1.0f, 0.5f),
	  Vec2f(TWO_THIRD, 0.5f),
	  Vec2f(TWO_THIRD, 0.75f),
      Vec2f(1.0f, 0.75f)
      

    };
    Vec3f v[36];
    Vec3f n[36];
    Vec2f tc[36];
    vector<unsigned int> indices;
    
    Vec3f v0, v1, v2, fn;

    for ( int kf = 0, kv = 0; kf < 12; kf++, kv += 3 )
    {
      v0 = vertices[faces[kf][0]];
      v1 = vertices[faces[kf][1]];
      v2 = vertices[faces[kf][2]];
      fn = calculateFaceNormal( v0, v1, v2 );

      indices.push_back(kv);
      indices.push_back(kv+1);
      indices.push_back(kv+2);

      v[kv]    = v0;
      v[kv+1]  = v1;
      v[kv+2]  = v2;

      n[kv]    = fn; 
      n[kv+1]  = fn; 
      n[kv+2]  = fn; 

	  bool texApplied = false;
	  if (applyCubeMapTexCoords)
	  {
			if (kf == 0)
			{
				tc[kv]    = texcoordsF1[0];
				tc[kv+1]  = texcoordsF1[1];
				tc[kv+2]  = texcoordsF1[2];
				texApplied = true;
			}
			if (kf == 1)
			{
				tc[kv]    = texcoordsF1[2];
				tc[kv+1]  = texcoordsF1[3];
				tc[kv+2]  = texcoordsF1[0];
				texApplied = true;
			}
			if (kf == 4)
			{
				tc[kv]    = texcoordsF2[0];
				tc[kv+1]  = texcoordsF2[1];
				tc[kv+2]  = texcoordsF2[2];
				texApplied = true;
			}
			if (kf == 5)
			{
				tc[kv]    = texcoordsF2[2];
				tc[kv+1]  = texcoordsF2[3];
				tc[kv+2]  = texcoordsF2[0];
				texApplied = true;
			}
			if (kf == 2)
			{
				tc[kv]    = texcoordsF3[0];
				tc[kv+1]  = texcoordsF3[1];
				tc[kv+2]  = texcoordsF3[2];
				texApplied = true;
			}
			if (kf == 3)
			{
				tc[kv]    = texcoordsF3[2];
				tc[kv+1]  = texcoordsF3[3];
				tc[kv+2]  = texcoordsF3[0];
				texApplied = true;
			}
			if (kf == 10)
			{
				tc[kv]    = texcoordsF4[0];
				tc[kv+1]  = texcoordsF4[1];
				tc[kv+2]  = texcoordsF4[2];
				texApplied = true;
			}
			if (kf == 11)
			{
				tc[kv]    = texcoordsF4[2];
				tc[kv+1]  = texcoordsF4[3];
				tc[kv+2]  = texcoordsF4[0];
				texApplied = true;
			}
			if (kf == 6)
			{
				tc[kv]    = texcoordsF5[0];
				tc[kv+1]  = texcoordsF5[1];
				tc[kv+2]  = texcoordsF5[2];
				texApplied = true;
			}
			if (kf == 7)
			{
				tc[kv]    = texcoordsF5[2];
				tc[kv+1]  = texcoordsF5[3];
				tc[kv+2]  = texcoordsF5[0];
				texApplied = true;
			}
			if (kf == 8)
			{
				tc[kv]    = texcoordsF6[0];
				tc[kv+1]  = texcoordsF6[1];
				tc[kv+2]  = texcoordsF6[2];
				texApplied = true;
			}
			if (kf == 9)
			{
				tc[kv]    = texcoordsF6[2];
				tc[kv+1]  = texcoordsF6[3];
				tc[kv+2]  = texcoordsF6[0];
				texApplied = true;
			}
	  }
	  if (texApplied == false)
	  {
		  // Assign texture coordinates
		  if (kf & 1)
		  { // odd faces
			tc[kv]    = texcoords[2];
			tc[kv+1]  = texcoords[3];
			tc[kv+2]  = texcoords[0];
		  }
		  else
		  { // Even faces
			tc[kv]    = texcoords[0];
			tc[kv+1]  = texcoords[1];
			tc[kv+2]  = texcoords[2];
		  }
	  }
    }
          
    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( v, 36 );
      vas->setNormals( n, 36 );
      vas->setTexCoords( 0, tc, 36 );
    }
    
    // Create a PrimitiveSet
    IndexSetSharedPtr indexSet( IndexSet::create() );
    IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
      primitive->setVertexAttributeSet( vasPtr );
      primitive->setIndexSet( indexSet );
    }
    
    return primitivePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createTetrahedron()
  {
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices
    static const Vec3f vertices[4] =
    { 
      Vec3f( -1.0f, -1.0f, -1.0f ),
      Vec3f( 1.0f, 1.0f, -1.0f ),
      Vec3f( 1.0f, -1.0f, 1.0f ),
      Vec3f( -1.0f, 1.0f, 1.0f )
    };

    // Setup faces:
    static const Face3 faces[4] =
    {
      {0, 3, 1},
      {0, 1, 2},
      {0, 2, 3},
      {1, 3, 2}
    };

    // Setup texture coordinates
    static const Vec2f texcoords[4] =
    {
      Vec2f(0.0f, 0.0f),
      Vec2f(1.0f, 1.0f),
      Vec2f(1.0f, 0.0f),
      Vec2f(0.0f, 1.0f)
    };

    // Calculate normals
    Vec3f v[12];
    Vec3f n[12];
    Vec2f tc[12];

    Vec3f v0, v1, v2, fn;

    vector<unsigned int> indices;

    for ( int kf = 0, kv = 0; kf < 4; kf++, kv += 3 )
    {
      v0 = vertices[faces[kf][0]];
      v1 = vertices[faces[kf][1]];
      v2 = vertices[faces[kf][2]];

      fn = calculateFaceNormal( v0, v1, v2 );

      indices.push_back(kv);
      indices.push_back(kv+1);
      indices.push_back(kv+2);

      v[kv]    = v0;
      v[kv+1]  = v1;
      v[kv+2]  = v2;

      n[kv]    = fn; 
      n[kv+1]  = fn; 
      n[kv+2]  = fn; 
    }

    // Assign texture coordinates
    tc[0]  = texcoords[0];
    tc[1]  = texcoords[1];
    tc[2]  = texcoords[2];
    tc[3]  = texcoords[0];
    tc[4]  = texcoords[3];
    tc[5]  = texcoords[1];
    tc[6]  = texcoords[0];
    tc[7]  = texcoords[2];
    tc[8]  = texcoords[3];
    tc[9]  = texcoords[1];
    tc[10] = texcoords[3];
    tc[11] = texcoords[2];


    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( v, 12 );
      vas->setNormals( n, 12 );
      vas->setTexCoords( 0, tc, 12 );
    }

    {
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createOctahedron()
  {
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices
    static const Vec3f vertices[6] =
    { 
      Vec3f( 0.0f, 1.0f, 0.0f ),
      Vec3f( 0.0f, -1.0f, 0.0f ),
      Vec3f( 1.0f, 0.0f, 0.0f ),
      Vec3f( -1.0f, 0.0f, 0.0f ),
      Vec3f( 0.0f, 0.0f, 1.0f ),
      Vec3f( 0.0f, 0.0f, -1.0f )
    };

    // Setup faces
    static const Face3 faces[8] =
    {
      {0, 4, 2},
      {0, 2, 5},
      {0, 5, 3},
      {0, 3, 4},
      {1, 2, 4},
      {1, 5, 2},
      {1, 3, 5},
      {1, 4, 3}
    };

    // Setup texture coordinates
    static const Vec2f texcoords[5] =
    {
      Vec2f(0.0f, 0.0f),  //0
      Vec2f(1.0f, 0.0f),  //1
      Vec2f(1.0f, 1.0f),  //2
      Vec2f(0.0f, 1.0f),  //3
      Vec2f(0.5f, 0.5f)   //4
    };

    // Calculate normals
    Vec3f v[24];
    Vec3f n[24];
    Vec2f tc[24];

    Vec3f v0, v1, v2, fn;

    vector<unsigned int> indices;      

    for ( int kf = 0, kv = 0; kf < 8; kf++, kv += 3 )
    {
      v0 = vertices[faces[kf][0]];
      v1 = vertices[faces[kf][1]];
      v2 = vertices[faces[kf][2]];

      fn = calculateFaceNormal( v0, v1, v2 );

      indices.push_back(kv);
      indices.push_back(kv+1);
      indices.push_back(kv+2);

        v[kv]    = v0;
      v[kv+1]  = v1;
      v[kv+2]  = v2;

      n[kv]    = fn; 
      n[kv+1]  = fn; 
      n[kv+2]  = fn; 
    }


    // Assign texture coordinates
    tc[0]  = texcoords[2];
    tc[1]  = texcoords[4];
    tc[2]  = texcoords[1];
    tc[3]  = texcoords[2];
    tc[4]  = texcoords[1];
    tc[5]  = texcoords[4];
    tc[6]  = texcoords[2];
    tc[7]  = texcoords[4];
    tc[8]  = texcoords[3];
    tc[9]  = texcoords[2];
    tc[10] = texcoords[3];
    tc[11] = texcoords[4];
    tc[12] = texcoords[0];
    tc[13] = texcoords[1];
    tc[14] = texcoords[4];
    tc[15] = texcoords[0];
    tc[16] = texcoords[4];
    tc[17] = texcoords[1];
    tc[18] = texcoords[0];
    tc[19] = texcoords[3];
    tc[20] = texcoords[4];
    tc[21] = texcoords[0];
    tc[22] = texcoords[4];
    tc[23] = texcoords[3];


    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( v, 24 );
      vas->setNormals( n, 24 );
      vas->setTexCoords( 0, tc, 24 );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }
    
      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createDodecahedron()
  {
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    //USE ICOSAHEDRON VERTICES AND FACES TO SETUP DODECAHEDRON
    //--------------------------------------------------------
    static const float x = 0.525731112119133606f;
    static const float z = 0.850650808352039932f;

    static const Vec3f icoVertices[12] =
    { 
      Vec3f(   -x , 0.0f ,    z ),
      Vec3f(    x , 0.0f ,    z ),
      Vec3f(   -x , 0.0f ,   -z ),
      Vec3f(    x , 0.0f ,   -z ),
      Vec3f( 0.0f ,    z ,    x ),
      Vec3f( 0.0f ,    z ,   -x ),
      Vec3f( 0.0f ,   -z ,    x ),
      Vec3f( 0.0f ,   -z ,   -x ),
      Vec3f(    z ,    x , 0.0f ),
      Vec3f(   -z ,    x , 0.0f ),
      Vec3f(    z ,   -x , 0.0f ),
      Vec3f(   -z ,   -x , 0.0f )
    };

    static const Face3 icoFaces[20] =
    {
      { 0,  1,  4}, //  0
      { 0,  4,  9}, //  1
      { 0,  9, 11}, //  2
      { 0,  6,  1}, //  3
      { 0, 11,  6}, //  4
      { 1,  6, 10}, //  5
      { 1, 10,  8}, //  6
      { 1,  8,  4}, //  7
      { 2,  3,  7}, //  8
      { 2,  5,  3}, //  9
      { 2,  9,  5}, // 10
      { 2, 11,  9}, // 11
      { 2,  7, 11}, // 12
      { 3,  5,  8}, // 13
      { 3,  8, 10}, // 14
      { 3, 10,  7}, // 15
      { 4,  5,  9}, // 16
      { 4,  8,  5}, // 17
      { 6,  7, 10}, // 18
      { 6, 11,  7}  // 19
    };

    //--------------------------------------------------------

    // Setup indices of dodecahedron
    static const int idxDode[12][5] = 
    {
      { 0,  1,  2,  4,  3}, //  0
      { 0,  3,  5,  6,  7}, //  1
      { 9,  8, 12, 11, 10}, //  2
      { 9, 13, 14, 15,  8}, //  3
      { 0,  7, 17, 16,  1}, //  4
      { 9, 10, 16, 17, 13}, //  5
      {18,  5,  3,  4, 19}, //  6
      {15, 18, 19, 12,  8}, //  7
      { 6, 14, 13, 17,  7}, //  8
      { 1, 16, 10, 11,  2}, //  9
      { 5, 18, 15, 14,  6}, // 10
      { 2, 11, 12, 19,  4}  // 11
    };

    // Calculate vertices
    Vec3f vertices[20];
    Vec3f v;

    // The 20 vertices of the dodecahedron are the centers of the 20 icosahedron triangle faces 
    // pushed out to unit sphere radius by normalization
    for ( int i = 0; i < 20; i++ )
    {
      v = icoVertices[icoFaces[i][0]] +  icoVertices[icoFaces[i][1]] +  icoVertices[icoFaces[i][2]];
      v.normalize();
      vertices[i] = v;
    }

    // Setup vertices, normals and faces
    vector<Vec3f> vv;
    vector<Vec3f> vn;

    vv.reserve( 72 );
    vn.reserve( 72 );

    vector<unsigned int> indices;
    vector<Face3> faces;
    indices.reserve( 180 );
    faces.reserve( 60 );
    
    for( unsigned int i = 0 ; i < 12; ++i )
    {
      Vec3f vf( 0.0f, 0.0f, 0.0f );
      for( unsigned int j = 0 ; j < 5; ++j )
      {
        vf += vertices[idxDode[i][j]];
      }
      vf /= 5.0f;

      // center point of face
      vv.push_back(v);
      vv.push_back(vertices[idxDode[i][0]]);
      vv.push_back(vertices[idxDode[i][1]]);
      vv.push_back(vertices[idxDode[i][2]]);
      vv.push_back(vertices[idxDode[i][3]]);
      vv.push_back(vertices[idxDode[i][4]]);

      vf.normalize();
      for( unsigned int j=0 ; j<6; ++j )
      {
        vn.push_back(v);
      }

      unsigned int k = i * 6;
      
      indices.push_back(k);
      indices.push_back(k+1);
      indices.push_back(k+2);

      indices.push_back(k);
      indices.push_back(k+2);
      indices.push_back(k+3);

      indices.push_back(k);
      indices.push_back(k+3);
      indices.push_back(k+4);

      indices.push_back(k);
      indices.push_back(k+4);
      indices.push_back(k+5);

      indices.push_back(k);
      indices.push_back(k+5);
      indices.push_back(k+1);
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( &vv[0], 72 );
      vas->setNormals( &vn[0], 72 );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createIcosahedron()
  {
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    static const float x = 0.525731112119133606f;
    static const float z = 0.850650808352039932f;

    // Setup vertices
    static const Vec3f vertices[12] =
    { 
      Vec3f(   -x , 0.0f ,    z ),
      Vec3f(    x , 0.0f ,    z ),
      Vec3f(   -x , 0.0f ,   -z ),
      Vec3f(    x , 0.0f ,   -z ),
      Vec3f( 0.0f ,    z ,    x ),
      Vec3f( 0.0f ,    z ,   -x ),
      Vec3f( 0.0f ,   -z ,    x ),
      Vec3f( 0.0f ,   -z ,   -x ),
      Vec3f(    z ,    x , 0.0f ),
      Vec3f(   -z ,    x , 0.0f ),
      Vec3f(    z ,   -x , 0.0f ),
      Vec3f(   -z ,   -x , 0.0f )
    };

    // Setup faces
    static const Face3 faces[20] =
    {
      { 0,  1,  4}, //  0
      { 0,  4,  9}, //  1
      { 0,  9, 11}, //  2
      { 0,  6,  1}, //  3
      { 0, 11,  6}, //  4
      { 1,  6, 10}, //  5
      { 1, 10,  8}, //  6
      { 1,  8,  4}, //  7
      { 2,  3,  7}, //  8
      { 2,  5,  3}, //  9
      { 2,  9,  5}, // 10
      { 2, 11,  9}, // 11
      { 2,  7, 11}, // 12
      { 3,  5,  8}, // 13
      { 3,  8, 10}, // 14
      { 3, 10,  7}, // 15
      { 4,  5,  9}, // 16
      { 4,  8,  5}, // 17
      { 6,  7, 10}, // 18
      { 6, 11,  7}  // 19
    };

    vector<Vec3f> vv;
    vector<Vec3f> vn;

    vv.reserve(60);
    vn.reserve(60);

    unsigned int j=0;

    vector<unsigned int> indices;
    vector<Face3> facesIco;
    indices.reserve( 60 );
    facesIco.reserve( 20 );

    for( unsigned int i = 0 ; i < 20; ++i )
    {
      Vec3f a = vertices[faces[i][0]];
      Vec3f b = vertices[faces[i][1]];
      Vec3f c = vertices[faces[i][2]];
      vv.push_back(a);
      vv.push_back(b);
      vv.push_back(c);

      Vec3f n = calculateFaceNormal( a, b, c );
      vn.push_back(n);
      vn.push_back(n);
      vn.push_back(n);

      indices.push_back(j++);
      indices.push_back(j++);
      indices.push_back(j++);
    }

    // Create a VertexAttributeSet with vertices and normals
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( &vv[0], 60 );
      vas->setNormals( &vn[0], 60 );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }



  // ===========================================================================

  DrawableSharedPtr createSphere( unsigned int m, unsigned int n, float radius ,bool cubemapTexcoords)
  {
    NVSG_ASSERT( m >= 3 && n >= 3 && "createSphere(): m and n both have to be at least 3." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    // setup vertices, normals, indices/faces and texture coordinates
    vector< Vec3f > vertices;
    vector< Vec3f > tangents;  
    vector< Vec3f > binormals;  
    vector< Vec3f > normals;  
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face3 > faces;

    const int size_v = ( m + 1 ) * n;
    vertices.reserve( size_v );
    tangents.reserve( size_v );
    binormals.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( 6 * m * ( n - 1 ) );
    faces.reserve( 2 * m * ( n - 1 ) );

    float phi_step = 2.0f * PI / (float) m;
    float theta_step = PI / (float) (n - 1);

    // Latitudinal rings.
    // Starting at the south pole going upwards.
    for( unsigned int latitude = 0 ; latitude < n ; latitude++ ) // theta angle
    {
      float theta = (float) latitude * theta_step;
      float sinTheta = sinf( theta );
      float cosTheta = cosf( theta );
      float texv = (float) latitude / (float) (n - 1); // Range [0.0f, 1.0f]

      // Generate vertices along the latitudinal rings.
      // On each latitude there are m + 1 vertices, 
      // the last one and the first one are on identical positions but have different texture coordinates.
      for( unsigned int longitude = 0 ; longitude <= m ; longitude++ ) // phi angle
      {
        float phi = (float) longitude * phi_step;
        float sinPhi = sinf( phi );
        float cosPhi = cosf( phi );
        float texu = (float) longitude / (float) m; // Range [0.0f, 1.0f]
        
        // Unit sphere coordinates are the normals.
        Vec3f v = Vec3f( cosPhi * sinTheta, 
                        -cosTheta,                 // -y to start at the south pole.
                        -sinPhi * sinTheta );
		
		float newtexu = texu;
		float newtexv = texv;

		if (cubemapTexcoords)
		{
			//if (phi > -nvmath::PI_QUARTER && phi < nvmath::PI_QUARTER &&
			if (phi >= 0.0f && phi <= 2.0*nvmath::PI && theta >= nvmath::PI_QUARTER && theta < 3*nvmath::PI_QUARTER)
			{
				while ((texu - 0.25f) >= 0.0f)
						texu-= 0.25f;
				float texvn = texv;
				//while ((texvn - 0.5f) >= 0.0f)
					//	texvn-= 0.5f;
				texu *= 4.0f;
				texvn = 1.0f - ((cosTheta + 1.0f)/2.0f);
				
				//texu = acos ((texu)/(nvmath::PI *2.0f));
				//texvn = asin ((texvn)/(nvmath::PI *2.0f));
			//	texu = 1.0f - texu;
				// wall one
				if (phi >= 0.0 && phi < nvmath::PI_HALF)
				{
			//		texu = (sin (phi*4.0f) + 1.0f) / 2.0f;
					newtexu =  ONE_THIRD*texu;
					newtexv = 0.5f + 0.25f * texvn ;
				}
				else
			   if (phi >= nvmath::PI_HALF && phi < nvmath::PI)
				{
				//	texu = (sin ((phi - nvmath::PI_HALF)*4.0f) + 1.0f) / 2.0f;
					newtexu = ONE_THIRD + ONE_THIRD*texu;
					newtexv = 0.50f + 0.25f * texvn ;
				} 
				else if (phi >= nvmath::PI && phi < nvmath::PI * 1.5f)
				{
					//texu = (sin ((phi - nvmath::PI)*4.0f) + 1.0f) / 2.0f;
					newtexu = 2*ONE_THIRD + ONE_THIRD*texu;
					newtexv = (0.50f + 0.25f * texvn);
				}
				else
				{
					//texu = (sin ((phi - nvmath::PI*1.5)*4.0f) + 1.0f) / 2.0f;
					newtexu = ONE_THIRD + ONE_THIRD* (texu);
					newtexv = (0.25f - 0.25f * texvn);
				}
			} else
			if (theta >=3*nvmath::PI_QUARTER && theta < 5*nvmath::PI_QUARTER)
			{
				// ceiling..
				float rem = texu - float(int(texu * 6.0)) * (1.0f/6.0f);
				float texvn = 1.0f - ((cosTheta + 1.0f)/2.0f);
				newtexu = ONE_THIRD + ONE_THIRD*texu;
				newtexv = 0.75f + 0.25f * texvn;
			} else
			{
				// flooar
				while ((texu - 0.25f) >= 0.0f)
						texu-= 0.25f;
			/*	float texvn = texv;
				while ((texvn - 0.25f) >= 0.0f)
						texvn-= 0.25f;
				texu *= 4.0f;
				texvn *= 4.0f;
				*/
				float texvn = 1.0f - ((cosTheta + 1.0f)/2.0f);
				texu *= 4.0f;
				//float rem = texu - float(int(texu * 6.0)) * (1.0f/6.0f);
				//texu = ( sinPhi + 1.0) / 2.0f;
				newtexu = ONE_THIRD + ONE_THIRD*texu;
				newtexv = 0.50f + 0.25f * texvn;
			}

		//	else
			//if (phi >= PI_QUARTER && phi < 2*PI_QUARTER && theta >= nvmath::PI_QUARTER && theta < 3*nvmath::PI_QUARTER)
			//{
				// wall one
				//newtexu = ONE_THIRD +ONE_THIRD*texu;
				//newtexv = 0.50f + 0.25f * texv;
		//	}
			/*if (phi < nvmath::PI && theta > 2*nvmath::PI_QUARTER && theta < 3*nvmath::PI_QUARTER)
			{
				newtexu = 2*ONE_THIRD + 2*ONE_THIRD*texu;
				newtexv = 0.5f + 0.25f * texv;
			}*/
			/*if (phi > nvmath::PI && theta > nvmath::PI_QUARTER && theta < 2*nvmath::PI_QUARTER)
			{
				newtexu = 3*ONE_THIRD + 2.0*ONE_THIRD*texu;
				newtexv = 0.5f + 0.25f * texv;
			}*/
#if 0
			else if (phi > nvmath::PI && theta > 2*nvmath::PI_QUARTER && theta < 3*nvmath::PI_QUARTER)
			{
				newtexu = 2.0*ONE_THIRD*texu;
				newtexv = 0.5f + 0.5*0.25f * texv;
			}
			else if (theta > 3.0f*nvmath::PI_QUARTER && phi > nvmath::PI )
			//if (phi > nvmath::PI_QUARTER && phi < 2*nvmath::PI_QUARTER && theta < nvmath::PI_QUARTER && theta > -nvmath::PI_QUARTER)
			{
				newtexu = 0.0f;//ONE_THIRD + ONE_THIRD*texu;
				newtexv = 0.0f;//0.25f + 0.25f * texv;
			}
			else if (theta > 5*nvmath::PI_QUARTER && phi > nvmath::PI )
			//if (phi > -2*nvmath::PI_QUARTER && phi < -nvmath::PI_QUARTER && theta < nvmath::PI_QUARTER && theta > -nvmath::PI_QUARTER)
			{
				newtexu = 0.0f;//ONE_THIRD + ONE_THIRD*texu;
				newtexv = 0.0f;//0.75f + 0.25f * texv;
			}
#endif
			//else //if (theta < nvmath::PI_QUARTER && theta > -nvmath::PI_QUARTER)
			//if (phi > -2*nvmath::PI_QUARTER && phi < -nvmath::PI_QUARTER && theta < nvmath::PI_QUARTER && theta > -nvmath::PI_QUARTER)
			{
				//newtexu = 0.0f;//ONE_THIRD + ONE_THIRD*texu;
				//newtexv = 0.0f;//0.0f + 0.25f * texv;
			}

		}

        vertices.push_back( v * radius );
        texcoords.push_back( Vec2f( newtexu , newtexv ) );
        normals.push_back( v );
        tangents.push_back( Vec3f( -sinPhi, 0.0f, -cosPhi ) );
        binormals.push_back( Vec3f( cosTheta * cosPhi, sinTheta, cosTheta * -sinPhi ) );
      }
    }
    
    // We have generated m + 1 vertices per latitude.
    const unsigned int columns = m + 1;

    // Calculate indices
    for( unsigned int latitude = 0 ; latitude < n - 1 ; latitude++ )
    {                                           
      for( unsigned int longitude = 0 ; longitude < m ; longitude++ )
      {
        indices.push_back(  latitude      * columns + longitude     );  // lower left
        indices.push_back(  latitude      * columns + longitude + 1 );  // lower right
        indices.push_back( (latitude + 1) * columns + longitude + 1 );  // upper right 

        indices.push_back( (latitude + 1) * columns + longitude + 1 );  // upper right 
        indices.push_back( (latitude + 1) * columns + longitude     );  // upper left
        indices.push_back(  latitude      * columns + longitude     );  // lower left
      }
    }

    // Create a VertexAttributeSet with vertices, normals and texcoords
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
      vas->setTexCoords( 0, &texcoords[0], size_v );
      vas->setTexCoords( VertexAttributeSet::NVSG_TANGENT  - VertexAttributeSet::NVSG_TEXCOORD0, &tangents[0], size_v );
      vas->setTexCoords( VertexAttributeSet::NVSG_BINORMAL - VertexAttributeSet::NVSG_TEXCOORD0, &binormals[0], size_v );
    }

   {
     // Create a PrimitiveSet
     IndexSetSharedPtr indexSet( IndexSet::create() );
     IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

     PrimitiveSharedPtr primitivePtr = Primitive::create();
     {
       PrimitiveWriteLock primitive(primitivePtr);
       primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
       primitive->setVertexAttributeSet( vasPtr );
       primitive->setIndexSet( indexSet );
     }

     drawablePtr = primitivePtr;
   }

   return drawablePtr;
  }
  DrawableSharedPtr createCylinderWithCap( float r, float h, unsigned int hdivs, unsigned int thdivs, bool bOuter, float thstart /*= 0.0f*/, float thend /*= 2.0f*nvmath::PI*/ ,bool cone)
  {
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;

    unsigned int size_v = (hdivs+1)*thdivs + 2*thdivs + 2;

    // ---Vertex order---
    // 1) 1                 bottom center vert
    // 2) thdivs            bottom rim
    // 3) thdivs*(hdivs+1)  all height division circles including bottom and top rim
    // 4) 1                 top center point
    // 5) thdivs            top rim

    float th_step = (thend - thstart) / (float) thdivs;
    float h_step = h / (float) hdivs;

    //-------------------------------
    // Generate the vertices/normals
    //-------------------------------

    vertices.push_back( Vec3f( 0.0f, -h/2.0f, 0.0f ) );
    normals.push_back( Vec3f( 0.0f, bOuter ? -1.0f : 1.0f, 0.0f) );
    //texcoords.push_back( Vec2f(0.5f, 0.5f) );
	texcoords.push_back( Vec2f(ONE_THIRD + ONE_THIRD * 0.5f,0.25f+ 0.25f*0.5f));
    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      float curTh = thstart + ith*th_step;
      vertices.push_back( Vec3f( r*cosf(curTh), -h/2.0f, r*sinf(curTh) ) );
      normals.push_back( Vec3f( 0.0f, bOuter ? -1.0f : 1.0f, 0.0f) );
      texcoords.push_back( Vec2f( (ONE_THIRD + ONE_THIRD*(0.5f*cos(curTh) + 0.5f)), 
									(0.25f + 0.25f *(0.5f*sin(curTh) + 0.5f))  ));
    }

    for(unsigned int ih = 0; ih < hdivs+1; ih++)
    {
      float curH = -h/2.0f + ih*h_step;

      for(unsigned int ith = 0; ith < thdivs; ith++)
      {
        float curTh = thstart + ith*th_step;

        vertices.push_back( Vec3f( r*cosf(curTh), curH, r*sinf(curTh)) );
        normals.push_back( Vec3f( (bOuter ? 1.0f : -1.0f)*cosf(curTh), 0.0f, (bOuter ? 1.0f : -1.0f)*sinf(curTh)) );
		// divide into quarters.
		bool quarterX =  ( cos (curTh) > 0.0f ? true : false);
		bool quarterY = ( sin (curTh) > 0.0f ? true : false);
		if (quarterX && quarterY)
		{
			  float q =  curTh/(thend - thstart);
				if (q > 0.75f)
					q = q - 0.75f;
				else if ( q > 0.5f )
					q = q - 0.5f;
				else if (q > 0.25f)
					q = q - 0.25f;
			texcoords.push_back( Vec2f(   2*ONE_THIRD + 4.0f*ONE_THIRD*q , 0.625f+ (0.25f*curH/h) ) );
		} else if (!quarterX && (quarterY))
		{
			    float q =  curTh/(thend - thstart);
				if (q > 0.75f)
					q = q - 0.75f;
				else if ( q > 0.5f )
					q = q - 0.5f;
				else if (q > 0.25f)
					q = q - 0.25f;

				texcoords.push_back( Vec2f( ONE_THIRD + 4.0f*ONE_THIRD*q  , 0.625f+(0.25f*curH/h) ) );
		}
		else if ((!quarterY) && quarterX)
		{
						    float q =  curTh/(thend - thstart);
				if (q > 0.75f)
					q = q - 0.75f;
				else if ( q > 0.5f )
					q = q - 0.5f;
				else if (q > 0.25f)
					q = q - 0.25f;


//			float tTh = curTh - (thend - thstart) * 0.25f * 2;
				texcoords.push_back( Vec2f(4.0f*ONE_THIRD*q  , 0.625f+ (0.25f*curH/h) ) );
		}
		else if (!quarterY && !quarterX)
		{
						    float q =  curTh/(thend - thstart);
				if (q > 0.75f)
					q = q - 0.75f;
				else if ( q > 0.5f )
					q = q - 0.5f;
				else if (q > 0.25f)
					q = q - 0.25f;


				texcoords.push_back( Vec2f( ONE_THIRD + 4.0f*ONE_THIRD*q  , 0.625f+ (0.25f*curH/h) ) );
		}
        
      }
    }

    vertices.push_back( Vec3f( 0.0f, h/2.0f, 0.0f ) );
    normals.push_back( Vec3f( 0.0f, bOuter ? 1.0f : -1.0f, 0.0f) );
 //   texcoords.push_back( Vec2f(0.5f, 0.5f) );
	   texcoords.push_back( Vec2f(ONE_THIRD + ONE_THIRD*0.5f, 0.75f + 0.25f*0.5f) );
    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      float curTh = thstart + ith*th_step;
      vertices.push_back( Vec3f( r*cosf(curTh), h/2.0f, r*sinf(curTh) ) );
      normals.push_back( Vec3f( 0.0f, bOuter ? 1.0f : -1.0f, 0.0f ) );
      texcoords.push_back( Vec2f((ONE_THIRD + ONE_THIRD*(0.5f*cos(curTh) + 0.5f)), (0.75f +0.25f*(0.5f*sin(curTh) + 0.5f) ) ) );
    }

    //-------------------------------
    // Generate the indices
    //-------------------------------

    int curvcount = 0;


    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      indices.push_back( curvcount );
      indices.push_back( curvcount+1 + ith );
      indices.push_back( curvcount+2 + (ith < thdivs-1 ? ith : -1) );
    }



    curvcount += 1 + thdivs;

    for(unsigned int ih = 0; ih < hdivs; ih++)
    {
      for(unsigned int ith = 0; ith < thdivs; ith++)
      {
        indices.push_back( curvcount+ith );
        indices.push_back( curvcount+ith+thdivs );
        indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) );

        indices.push_back( curvcount+ith+thdivs );
        indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) + thdivs );
        indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) );
      }

      curvcount += thdivs;
    }

    curvcount += thdivs;


    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      indices.push_back( curvcount );
      indices.push_back( curvcount+2 + (ith < thdivs-1 ? ith : -1) );
      indices.push_back( curvcount+1 + ith );
    }



    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
      vas->setTexCoords( 0, &texcoords[0], size_v );
    }

    IndexSetSharedPtr indexSet( IndexSet::create() );
    IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
      primitive->setVertexAttributeSet( vasPtr );
      primitive->setIndexSet( indexSet );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
  }
  DrawableSharedPtr createCylinder( float r, float h, unsigned int hdivs, unsigned int thdivs, bool bOuter, float thstart /*= 0.0f*/, float thend /*= 2.0f*nvmath::PI*/,bool cone )
  {
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
	float tr = r;
    unsigned int size_v = (hdivs+1)*thdivs + 2*thdivs + 2;

    // ---Vertex order---
    // 1) 1                 bottom center vert
    // 2) thdivs            bottom rim
    // 3) thdivs*(hdivs+1)  all height division circles including bottom and top rim
    // 4) 1                 top center point
    // 5) thdivs            top rim

    float th_step = (thend - thstart) / (float) thdivs;
    float h_step = h / (float) hdivs;

    //-------------------------------
    // Generate the vertices/normals
    //-------------------------------

    vertices.push_back( Vec3f( 0.0f, -h/2.0f, 0.0f ) );
    normals.push_back( Vec3f( 0.0f, bOuter ? -1.0f : 1.0f, 0.0f) );
    texcoords.push_back( Vec2f(0.5f, 0.5f) );
	tr = r;
    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      float curTh = thstart + ith*th_step;
      vertices.push_back( Vec3f( tr*cosf(curTh), -h/2.0f, tr*sinf(curTh) ) );
      normals.push_back( Vec3f( 0.0f, bOuter ? -1.0f : 1.0f, 0.0f) );
      texcoords.push_back( Vec2f( 0.5f*cos(curTh) + 0.5f, 0.5f*sin(curTh) + 0.5f) );
    }
	tr = r;
    for(unsigned int ih = 0; ih < hdivs+1; ih++)
    {
      float curH = -h/2.0f + ih*h_step;
	  if (cone)
	  tr = r - (r / (hdivs + 1)) * ih;
      for(unsigned int ith = 0; ith < thdivs; ith++)
      {
        float curTh = thstart + ith*th_step;

        vertices.push_back( Vec3f( tr*cosf(curTh), curH, tr*sinf(curTh)) );
        normals.push_back( Vec3f( (bOuter ? 1.0f : -1.0f)*cosf(curTh), 0.0f, (bOuter ? 1.0f : -1.0f)*sinf(curTh)) );
        texcoords.push_back( Vec2f( curTh/(thend - thstart), curH/h ) );
      }
    }

    vertices.push_back( Vec3f( 0.0f, h/2.0f, 0.0f ) );
    normals.push_back( Vec3f( 0.0f, bOuter ? 1.0f : -1.0f, 0.0f) );
    texcoords.push_back( Vec2f(0.5f, 0.5f) );
	tr = 0;
    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      float curTh = thstart + ith*th_step;
      vertices.push_back( Vec3f( tr*cosf(curTh), h/2.0f, tr*sinf(curTh) ) );
      normals.push_back( Vec3f( 0.0f, bOuter ? 1.0f : -1.0f, 0.0f ) );
      texcoords.push_back( Vec2f( 0.5f*cos(curTh) + 0.5f, 0.5f*sin(curTh) + 0.5f) );
    }

    //-------------------------------
    // Generate the indices
    //-------------------------------

    int curvcount = 0;


    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      indices.push_back( curvcount );
      indices.push_back( curvcount+1 + ith );
      indices.push_back( curvcount+2 + (ith < thdivs-1 ? ith : -1) );
    }



    curvcount += 1 + thdivs;

    for(unsigned int ih = 0; ih < hdivs; ih++)
    {
      for(unsigned int ith = 0; ith < thdivs; ith++)
      {
        indices.push_back( curvcount+ith );
        indices.push_back( curvcount+ith+thdivs );
        indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) );

        indices.push_back( curvcount+ith+thdivs );
        indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) + thdivs );
        indices.push_back( curvcount+(ith < thdivs-1 ? ith+1 : 0) );
      }

      curvcount += thdivs;
    }

    curvcount += thdivs;


    for(unsigned int ith = 0; ith < thdivs; ith++)
    {
      indices.push_back( curvcount );
      indices.push_back( curvcount+2 + (ith < thdivs-1 ? ith : -1) );
      indices.push_back( curvcount+1 + ith );
    }



    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
      vas->setTexCoords( 0, &texcoords[0], size_v );
    }

    IndexSetSharedPtr indexSet( IndexSet::create() );
    IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
      primitive->setVertexAttributeSet( vasPtr );
      primitive->setIndexSet( indexSet );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createTorus( unsigned int m, unsigned int n, float innerRadius , float outerRadius, bool quarter )
  {
    // The torus is a ring with radius outerRadius rotated around the y-axis along the circle with innerRadius.
    
    /*           y
       ___       |       ___
     /     \           /     \
    |       |    |    |       |
    |       |         |       |
     \ ___ /     |     \ ___ /
                          <--->
                          outerRadius
                 <------->
                 innerRadius
    */

    NVSG_ASSERT( m >= 3 && n >= 3 && "createTorus(): m and n both have to be at least 3." );

    // create pointer to return
    DrawableSharedPtr drawablePtr;

    vector< Vec3f > vertices;
    vector< Vec3f > tangents;
    vector< Vec3f > binormals;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face4 > faces;

    unsigned int size_v = ( m + 1 ) * ( n + 1 );

    vertices.reserve( size_v );
    tangents.reserve( size_v );
    binormals.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( 4 * m * n );
    faces.reserve( m * n );

    float mf = (float) m ;
    float nf = (float) n;

    float phi_step   = 2.0f * PI / mf;
    float theta_step = 2.0f * PI / nf;

	unsigned int quarterStart = 0;
	unsigned int quarterEnd = m;
	if (quarter)
	{
		quarterStart = (m/4)*2  ;
		quarterEnd = (m/4)*3  ;
	}

    // Setup vertices and normals
    // Generate the Torus exactly like the sphere with rings around the origin along the latitudes.
    for ( unsigned int latitude = 0; latitude <= n; latitude++ ) // theta angle
    {
      float theta = (float) latitude * theta_step;
      float sinTheta = sinf(theta);
      float cosTheta = cosf(theta);

      float radius = innerRadius + outerRadius * cosTheta;

      for ( unsigned int longitude = quarterStart; longitude <=quarterEnd; longitude++ ) // phi angle
      {
        float phi = (float) longitude * phi_step;
        float sinPhi = sinf(phi);
        float cosPhi = cosf(phi);

        vertices.push_back( Vec3f( radius      *  cosPhi, 
                                   outerRadius *  sinTheta, 
                                   radius      * -sinPhi ) );

        tangents.push_back( Vec3f( -sinPhi, 0.0f, -cosPhi ) );

        binormals.push_back( Vec3f( cosPhi * -sinTheta,
                                    cosTheta, 
                                    sinPhi * sinTheta ) );

        normals.push_back( Vec3f( cosPhi * cosTheta,
                                  sinTheta,  
                                 -sinPhi * cosTheta ) );
        
        texcoords.push_back( Vec2f( (float) longitude / mf , (float) latitude / nf ) );
      }
    }

	const unsigned int columns = (quarter ? m/4 + 1 : m + 1); 

    // Setup indices
    for( unsigned int latitude = 0 ; latitude < n ; latitude++ ) 
    {
	  unsigned int le = (quarter ? m /4 : m );
      for( unsigned int longitude = 0 ; longitude < le ; longitude++ )
      {
        indices.push_back(  latitude      * columns + longitude     );  // lower left
        indices.push_back(  latitude      * columns + longitude + 1 );  // lower right
        indices.push_back( (latitude + 1) * columns + longitude + 1 );  // upper right
        indices.push_back( (latitude + 1) * columns + longitude     );  // upper left
      }
    }
    
    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
      vas->setTexCoords( 0, &texcoords[0], size_v );
      vas->setTexCoords( VertexAttributeSet::NVSG_TANGENT  - VertexAttributeSet::NVSG_TEXCOORD0, &tangents[0], size_v );
      vas->setTexCoords( VertexAttributeSet::NVSG_BINORMAL - VertexAttributeSet::NVSG_TEXCOORD0, &binormals[0], size_v );
    }
    
    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_QUADS );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }

  // ===========================================================================
  
  DrawableSharedPtr createTessellatedPlane( unsigned int subdiv, const Mat44f &transf )
  {
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices, normals, faces and texture coordinates (and indices for primitive creation mode) 
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face3 > faces;

    const int size_v =  ( subdiv + 2 ) * ( subdiv + 2 );
    vertices.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( 2 * ( subdiv + 1 ) * ( subdiv + 1 ) );

    // Setup tessellated plane
    setupTessellatedPlane( subdiv, transf, faces, vertices, normals, texcoords, indices);

    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
      vas->setTexCoords( 0, &texcoords[0], size_v );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }
   // ===========================================================================
  DrawableSharedPtr createCallout( float x0, float y0, float z0,
   float x1, float y1, float z1)
  {
    vector< Vec3f > vertices;

    vertices.push_back( Vec3f(x0, y0, z0) );
    vertices.push_back(  Vec3f(x1, y0, z0) );
	//
	 vertices.push_back(  Vec3f(x1, y0, z0) );
	 vertices.push_back(  Vec3f(x1, y1, z0)  );

	 vertices.push_back(  Vec3f(x1, y1, z0)  );
     vertices.push_back( Vec3f(x1, y1, z1) );

    /////

   // vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );

   // vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );
	//
	//vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );

  //  vertices.push_back( Vec3f(x0, y0, 0.0f) );


    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], 6 );
    }

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_LINES );
      primitive->setVertexAttributeSet( vasPtr );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
  }
    // ===========================================================================
  DrawableSharedPtr createPathLine( float x0, float y0, float z0,
   float x1, float y1, float z1)
  {
    vector< Vec3f > vertices;

    vertices.push_back( Vec3f(x0, y0, z0) );
    vertices.push_back(  Vec3f(x1, y0, z0) );
	//
	 vertices.push_back(  Vec3f(x1, y0, z0) );
	 vertices.push_back(  Vec3f(x1, y1, z0)  );

	 vertices.push_back(  Vec3f(x1, y1, z0)  );
     vertices.push_back( Vec3f(x1, y1, z1) );

    /////

   // vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );

   // vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );
	//
	//vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );

  //  vertices.push_back( Vec3f(x0, y0, 0.0f) );


    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], 6 );
    }

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_LINES );
      primitive->setVertexAttributeSet( vasPtr );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
  }
  // ===========================================================================
  DrawableSharedPtr createLine( float x0, float y0, 
    float width, float height,
    float wext, float hext)
  {
    vector< Vec3f > vertices;

    vertices.push_back( Vec3f(x0, y0, 0.0f) );

   // vertices.push_back( Vec3f(x0 + width, y0, 0.0f) );
	//
	//  vertices.push_back( Vec3f(x0 + width, y0, 0.0f) );

    vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );

    /////

   // vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );

   // vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );
	//
	//vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );

  //  vertices.push_back( Vec3f(x0, y0, 0.0f) );


    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], 2 );
    }

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_LINES );
      primitive->setVertexAttributeSet( vasPtr );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
  }
  DrawableSharedPtr createPlaneBorders( float x0, float y0, 
    float width, float height,
    float wext, float hext)
  {
    vector< Vec3f > vertices;

    vertices.push_back( Vec3f(x0, y0, 0.0f) );

    vertices.push_back( Vec3f(x0 + width, y0, 0.0f) );
	//
	  vertices.push_back( Vec3f(x0 + width, y0, 0.0f) );

    vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );

    /////

    vertices.push_back( Vec3f(x0 + width, y0 + height, 0.0f) );

    vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );
	//
	vertices.push_back( Vec3f(x0, y0 + height, 0.0f) );

    vertices.push_back( Vec3f(x0, y0, 0.0f) );


    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], 8 );
    }

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_LINES );
      primitive->setVertexAttributeSet( vasPtr );
    }


    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
  }
  DrawableSharedPtr createPlane( float x0, float y0, 
    float width, float height,
    float wext, float hext , float depth)
  {
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
	// Added offsets..
	float wh = 0.0f;
	float hh = 0.0f;
    vertices.push_back( Vec3f(x0 -wh, y0 - hh, depth) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(0.0f, 0.0f) );

    vertices.push_back( Vec3f(x0 + width -wh, y0- hh, depth) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(wext, 0.0f) );

    vertices.push_back( Vec3f(x0 + width -wh, y0 + height- hh, depth) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(wext, hext) );

    /////

    vertices.push_back( Vec3f(x0 + width -wh , y0 + height- hh, depth) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(wext, hext) );

    vertices.push_back( Vec3f(x0 -wh , y0 + height- hh, depth) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(0.0f, hext) );

    vertices.push_back( Vec3f(x0 -wh , y0- hh, depth) );
    normals.push_back( Vec3f(0.0f, 0.0f, 1.0f) );
    texcoords.push_back( Vec2f(0.0f, 0.0f) );


    //-------------------------------
    // Register the primitive
    //-------------------------------

    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );

      vas->setVertices( &vertices[0], 6 );
      vas->setNormals( &normals[0], 6 );
      vas->setTexCoords( 0, &texcoords[0], 6 );
      vas->setTexCoords( 1, &texcoords[0], 6 );
    }

    PrimitiveSharedPtr primitivePtr = Primitive::create();
    {
      PrimitiveWriteLock primitive(primitivePtr);
      primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
      primitive->setVertexAttributeSet( vasPtr );
    }

    DrawableSharedPtr drawablePtr = primitivePtr;

    return drawablePtr;
  }

  // ===========================================================================

  DrawableSharedPtr createTessellatedBox( unsigned int subdiv )
  {
    // create pointer to return
    DrawableSharedPtr drawablePtr;
    
    // Setup vertices, normals, faces and texture coordinates
    vector< Vec3f > vertices;
    vector< Vec3f > normals;
    vector< Vec2f > texcoords;
    vector<unsigned int> indices;
    vector< Face3 > faces;

    const int size_v = 6 * ( subdiv + 2 ) * ( subdiv + 2 );
    vertices.reserve( size_v );
    normals.reserve( size_v );
    texcoords.reserve( size_v );
    indices.reserve( size_v );
    faces.reserve( 6 * 2 * ( ( subdiv + 1 ) * ( subdiv + 1 ) ) );

    // Setup transformations for 6 box sides
    Mat44f transf[6];
    transf[0] = Mat44f( 1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  1.0f ); // front

    transf[1] = Mat44f(-1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        0.0f,  0.0f, -1.0f,  0.0f,
                        0.0f,  0.0f, -1.0f,  1.0f ); // back, 180 degrees around y-axis

    transf[2] = Mat44f( 0.0f,  0.0f,  1.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                       -1.0f,  0.0f,  0.0f,  0.0f,
                       -1.0f,  0.0f,  0.0f,  1.0f ); // left, -90 degrees around y-axis

    transf[3] = Mat44f( 0.0f,  0.0f, -1.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        1.0f,  0.0f,  0.0f,  0.0f,
                        1.0f,  0.0f,  0.0f,  1.0f ); // right, 90 degrees around y-axis

    transf[4] = Mat44f( 1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  0.0f,  1.0f,  1.0f,
                        0.0f, -1.0f,  0.0f,  0.0f,
                        0.0f, -1.0f,  0.0f,  1.0f ); // bottom, 90 degrees around x-axis

    transf[5] = Mat44f( 1.0f,  0.0f,  0.0f,  0.0f,
                        0.0f,  0.0f, -1.0f,  1.0f,
                        0.0f,  1.0f,  0.0f,  0.0f,
                        0.0f,  1.0f,  0.0f,  1.0f ); // top, -90 degrees around x-axis
    
    for ( unsigned int i=0; i<6; i++ )
    {
      setupTessellatedPlane( subdiv, transf[i], faces, vertices, normals, texcoords, indices);
    }

    // Create a VertexAttributeSet with vertices, normals and texture coordinates
    VertexAttributeSetSharedPtr vasPtr = VertexAttributeSet::create();
    {
      VertexAttributeSetWriteLock vas( vasPtr );
      vas->setVertices( &vertices[0], size_v );
      vas->setNormals( &normals[0], size_v );
      vas->setTexCoords( 0, &texcoords[0], size_v );
    }

    {
      // Create a PrimitiveSet
      IndexSetSharedPtr indexSet( IndexSet::create() );
      IndexSetWriteLock(indexSet)->setData( &indices[0], checked_cast<unsigned int>(indices.size()) );

      PrimitiveSharedPtr primitivePtr = Primitive::create();
      {
        PrimitiveWriteLock primitive(primitivePtr);
        primitive->setPrimitiveType( PRIMITIVE_TRIANGLES );
        primitive->setVertexAttributeSet( vasPtr );
        primitive->setIndexSet( indexSet );
      }

      drawablePtr = primitivePtr;
    }

    return drawablePtr;
  }

  // ===========================================================================

  StateSetSharedPtr createDefaultMaterial( const Vec3f &diffuseColor )
  {
    // Create a Material
    MaterialSharedPtr materialPtr = Material::create();
    StateSetSharedPtr statePtr = StateSet::create();
    MaterialWriteLock mw( materialPtr );
    mw->setDiffuseColor( diffuseColor );

    // Create a StateSet
    StateSetWriteLock ssw( statePtr );
    ssw->addAttribute( materialPtr );

    return statePtr;
  }

  // ===========================================================================

  StateSetSharedPtr createDefinedMaterial( const Vec3f &ambientColor,
                                           const Vec3f &diffuseColor,
                                           const Vec3f &specularColor,
                                           const float specularExponent,
                                           const Vec3f &emissiveColor,
                                           const float opacity,
                                           const float reflectivity,
                                           const float indexOfRefraction )
  {
    // Create a Material
    MaterialSharedPtr materialPtr = createMaterialAttribute( ambientColor,
                                                             diffuseColor,
                                                             specularColor,
                                                             specularExponent,
                                                             emissiveColor,
                                                             opacity,
                                                             reflectivity,
                                                             indexOfRefraction );

    // Create a StateSet
    StateSetSharedPtr statePtr = StateSet::create();
    StateSetWriteLock( statePtr )->addAttribute( materialPtr );

    return statePtr;
  }

  // ===========================================================================

  MaterialSharedPtr createMaterialAttribute( const Vec3f &ambientColor,
                                             const Vec3f &diffuseColor,
                                             const Vec3f &specularColor,
                                             const float specularExponent,
                                             const Vec3f &emissiveColor,
                                             const float opacity,
                                             const float reflectivity,
                                             const float indexOfRefraction )
  {
    // Create a Material
    MaterialSharedPtr materialPtr = Material::create();
    {
      MaterialWriteLock mw( materialPtr );
      mw->setAmbientColor( ambientColor ,MF_FRONT_AND_BACK);
      mw->setDiffuseColor( diffuseColor,MF_FRONT_AND_BACK );
      mw->setSpecularColor( specularColor,MF_FRONT_AND_BACK );
      mw->setSpecularExponent( specularExponent ,MF_FRONT_AND_BACK);
      mw->setEmissiveColor( emissiveColor,MF_FRONT_AND_BACK );
	  if (opacity < 1.0)
		  mw->setOpacityColor ( Vec3f (diffuseColor[0]*opacity,diffuseColor[1]*opacity,diffuseColor[2]*opacity)/*Vec3f (0.5f,0.5f,0.5f)*/,MF_FRONT_AND_BACK);
	  else
      mw->setOpacityColor ( Vec3f (1.0f,1.0f,1.0f)/*Vec3f (0.5f,0.5f,0.5f)*/,MF_FRONT_AND_BACK);
	  mw->setOpacity( opacity,MF_FRONT_AND_BACK );
	  //mw->setOpacityColor ( Vec3f (1.0f,1.0f,1.0f)/*Vec3f (0.5f,0.5f,0.5f)*/,MF_FRONT_AND_BACK);
      mw->setReflectivity( reflectivity ,MF_FRONT_AND_BACK);
      mw->setIndexOfRefraction( indexOfRefraction ,MF_FRONT_AND_BACK); 
    }
    return materialPtr;
  }

  // ===========================================================================

  StateSetSharedPtr createTexture()
  {
    vector<Vec4f> tex;
    tex.resize(64);

    // Create pattern
    for( unsigned int i = 0; i < 8; ++i )
    {
      for( unsigned int j = 0; j < 8; ++j )
      {
        unsigned int pos = i * 8 + j;
        Vec4f col(float(( i ^ j ) & 1), float((( i ^ j ) & 2) / 2), float(((i  ^ j ) & 4) / 4), 1.0f);
        tex.at(pos) = col;
      }
    }

    TextureHostSharedPtr tisp = TextureHost::create();
    {
      TextureHostWriteLock tiw( tisp );
      tiw->setCreationFlags( TextureHost::F_PRESERVE_IMAGE_DATA_AFTER_UPLOAD );
      unsigned int index = tiw->addImage( 8, 8, 1, Image::IMG_RGBA, Image::IMG_FLOAT32 );
      NVSG_ASSERT( index != -1 );
      tiw->setImageData( index, (const void *) &tex[0] );
      tiw->setTextureTarget( NVSG_TEXTURE_2D );
      tiw->setTextureGPUFormat(TextureHost::TGF_FIXED8);
    }

    TextureAttributeSharedPtr texAttPtr( TextureAttribute::create() );
    {
      TextureAttributeWriteLock texAtt( texAttPtr );
      TextureAttributeItemSharedPtr texAttItemPtr( TextureAttributeItem::create() );
      TextureAttributeItemWriteLock texAttItem( texAttItemPtr );
      texAttItem->setTexture( tisp );
      texAttItem->setMagFilterMode( TFM_MAG_NEAREST );
      texAttItem->setMinFilterMode( TFM_MIN_NEAREST );
      texAtt->bindTextureAttributeItem( texAttItemPtr, 0 );
    }

    // Create a StateSet and attach material and shader
    StateSetSharedPtr stateSetPtr = StateSet::create();
    {
      StateSetWriteLock stateSet( stateSetPtr );
      stateSet->addAttribute( texAttPtr );
    }

    return stateSetPtr;
  }

  // ===========================================================================

  StateSetSharedPtr createAlphaTexture( unsigned int n /* =64 */ )
  {
    vector<Vec4f> tex;
    tex.resize(n*n);
    
    // Create pattern
    for( unsigned int i = 0; i < n; ++i )
    {
      for( unsigned int j = 0; j < n; ++j )
      {
        float dx = ( (float)i - (float)(n-1)/2.0f ) / ( (float)(n-1)/2.0f );
        float dy = ( (float)j - (float)(n-1)/2.0f ) / ( (float)(n-1)/2.0f );

        float val = max(0.0f, 1.0f - (dx*dx+dy*dy));       

        unsigned int pos = i * n + j;
        Vec4f col( val, val, val, val );
        tex.at(pos) = col;
      }
    }

    TextureHostSharedPtr tisp = TextureHost::create();
    {
      TextureHostWriteLock tiw( tisp );
      tiw->setCreationFlags( TextureHost::F_PRESERVE_IMAGE_DATA_AFTER_UPLOAD );
      unsigned int index = tiw->addImage( n, n, 1, Image::IMG_RGBA, Image::IMG_FLOAT32 );
      NVSG_ASSERT( index != -1 );
      tiw->setImageData( index, (const void *) &tex[0] );
      tiw->setTextureTarget( NVSG_TEXTURE_2D );
      tiw->setTextureGPUFormat(TextureHost::TGF_FIXED8);
    }

    TextureAttributeSharedPtr texAttPtr( TextureAttribute::create() );
    {
      TextureAttributeWriteLock texAtt( texAttPtr );
      TextureAttributeItemSharedPtr texAttItemPtr( TextureAttributeItem::create() );
      TextureAttributeItemWriteLock texAttItem( texAttItemPtr );
      texAttItem->setTexture( tisp );
      texAttItem->setMagFilterMode( TFM_MAG_NEAREST );
      texAttItem->setMinFilterMode( TFM_MIN_NEAREST );
      texAtt->bindTextureAttributeItem( texAttItemPtr, 0 );
    }

    // Create a StateSet and attach material and shader
    StateSetSharedPtr stateSetPtr = StateSet::create();
    {
      StateSetWriteLock stateSet( stateSetPtr );
      stateSet->addAttribute( texAttPtr );
    }

    return stateSetPtr;
  }

  // ===========================================================================

  GeoNodeSharedPtr createGeoNode( const DrawableSharedPtr &drawable, const StateSetSharedPtr &stateSet )
  {
    //Create a GeoNode combining StateSet and Shape
    GeoNodeSharedPtr geoPtr;
    geoPtr = GeoNode::create();
    GeoNodeWriteLock gnw( geoPtr );
    gnw->addDrawable( stateSet, drawable );

    return geoPtr;
  }

  // ===========================================================================

  TransformSharedPtr createTransform( const NodeSharedPtr &node , const Vec3f &translation, const Quatf &orientation, const Vec3f &scaling )
  {
    // Make a Transformation
    Trafo trafo;
    trafo.setTranslation( translation );
    trafo.setOrientation( orientation );
    trafo.setScaling( scaling );

    // Create a Transform
    TransformSharedPtr transPtr = Transform::create();
    TransformWriteLock tw( transPtr );
    tw->setTrafo( trafo );
    tw->addChild( node );

    return transPtr;
  }

  // ===========================================================================

  TransformSharedPtr imitateRaster( const NodeSharedPtr &node, unsigned int width, unsigned int height )
  {
    Trafo trafo;

    trafo.setTranslation( Vec3f( -0.5f*width/height,  -0.5f,  0.0f ) );
    trafo.setOrientation( Quatf( Vec3f(0.0, 1.0, 0.0), 0.0) );
    trafo.setScaling( Vec3f( 1.0f/height, 1.0f/height, 1.0f ) );

    TransformSharedPtr transPtr = Transform::create();
    TransformWriteLock tw( transPtr );
    tw->setTrafo( trafo );
    tw->addChild( node );

    return transPtr;
  }

  // ===========================================================================

  TransformSharedPtr imitateRaster( unsigned int width, unsigned int height )
  {
    Trafo trafo;

    trafo.setTranslation( Vec3f( -0.5f*width/height,  -0.5f,  0.0f ) );
    trafo.setOrientation( Quatf( Vec3f(0.0, 1.0, 0.0), 0.0) );
    trafo.setScaling( Vec3f( 1.0f/height, 1.0f/height, 1.0f ) );


    TransformSharedPtr transPtr = Transform::create();
    TransformWriteLock tw( transPtr );
    tw->setTrafo( trafo );

    return transPtr;
  }

  // ===========================================================================

  void setCameraPOV(float x, float y, float z, ViewStateSharedPtr viewState)
  {

    Vec3f pos(x, y, z);

    ViewStateWriteLock theViewState(viewState);
    CameraWriteLock theCamera(theViewState->getCamera());

    Vec3f curpov = theCamera->getPosition();

    theCamera->setPosition( pos );
  }

  // ===========================================================================

  void setCameraDirNoRoll(float x, float y, float z, ViewStateSharedPtr viewState)
  {

    Vec3f dir(x, y, z);

    ViewStateWriteLock theViewState(viewState);
    CameraWriteLock theCamera(theViewState->getCamera());

    dir.normalize();

    theCamera->setDirection( dir );
    theCamera->setUpVector( Vec3f(0.0f, 1.0f, 0.0f) );
  }

  // ===========================================================================

  void setCameraDir(float x, float y, float z, ViewStateSharedPtr viewState)
  {

    Vec3f dir(x, y, z);

    ViewStateWriteLock theViewState(viewState);
    CameraWriteLock theCamera(theViewState->getCamera());

    dir.normalize();

    Vec3f up;

    if( ( dir != Vec3f(0.0f, 1.0f, 0.0f) ) && ( dir != Vec3f(0.0f, -1.0f, 0.0f) ) )
    {
      Vec3f yup(0.0f, 1.0f, 0.0f);

      up = yup - dir*(dir*yup);
      up.normalize();
    }
    else
    {
      up = Vec3f(0.0f, 0.0f, dir[1]);
    }

    theCamera->setUpVector( up );
    theCamera->setDirection( dir );
  }

  // ===========================================================================

  DirectedLightSharedPtr createDirectedLight( const Vec3f &direction , const Vec3f &diffuseColor )
  {
    DirectedLightSharedPtr directedLightPtr = DirectedLight::create();
    DirectedLightWriteLock dlw( directedLightPtr );
    dlw->setAmbientColor(  Vec3f( 1.0f, 1.0f, 1.0f ) );
    dlw->setDiffuseColor( diffuseColor );
    dlw->setSpecularColor( Vec3f( 1.0f, 1.0f, 1.0f ) );
    Vec3f dir = direction;
    dir.normalize();
    dlw->setDirection( dir );

    return directedLightPtr;
  }

  // ===========================================================================

  PointLightSharedPtr createPointLight( const Vec3f &position , const Vec3f &diffuseColor )
  {
    PointLightSharedPtr pointLightPtr = PointLight::create();
    PointLightWriteLock plw( pointLightPtr );
    plw->setAmbientColor(  Vec3f( 1.0f, 1.0f, 1.0f ) );
    plw->setDiffuseColor( diffuseColor );
    plw->setSpecularColor( Vec3f( 1.0f, 1.0f, 1.0f ) );
    plw->setPosition( position );

    return pointLightPtr;
  }

  // ===========================================================================

  SpotLightSharedPtr createSpotLight( const Vec3f &position , const Vec3f &direction , const float angle , const Vec3f &diffuseColor )
  {
    SpotLightSharedPtr spotLightPtr = SpotLight::create();
    SpotLightWriteLock slw( spotLightPtr );
    slw->setPosition( position );
    Vec3f dir = direction;
    dir.normalize();
    slw->setDirection( dir );
    slw->setCutoffAngle( angle );
	slw->setShadowCasting (true);
    slw->setAmbientColor(  Vec3f( 1.0f, 1.0f, 1.0f ) );
    slw->setDiffuseColor( diffuseColor );
    slw->setSpecularColor( Vec3f( 1.0f, 1.0f, 1.0f ) );

    return spotLightPtr;
  }

} //namespace nvutil

#if 0

using namespace nvutil;
namespace avocado {
	static std::vector<TextureHostSharedPtr> GlobalTextureStack;
	using namespace nvutil;
	class MaterialBase {
	public:
		~MaterialBase ()
		{
			m_cachedStateSet = 0;
		}
		
		MaterialBase ()
		{
			name = "AvocadoDefaultMaterial";
			ambient = 0.5f;
			diffuse = 0.0f;
			baseColor[0] = 0.5f;
			baseColor[1] = 0.5f;
			baseColor[2] = 0.5f;
			specular = 0.25f;
			shininess = 12.7f;
			opacity = 1.0f;
			roughness = 0.5f;
			polished = 0.5f;
			frenselExp = 5.0;
			reflectionMin = 0.0;
			reflectionMax = 0.0;
			reflectivity = 0.0;
			exposure = -0.65f;
			ior = 0.0;
			bumpHeight = 0.8f;
			bumpScale = 1.0f;
			bumpIsNormalMap = false;
			bumpTexture = "";
			environmentMap = "";
			m_shaderFileName = "";
			isWood = false;
			woodScale = 0.0f;
			wooble = 0.0f;
			ringScale = 0.0f;
			woodOffset[0] = 0.0f;
			woodOffset[1] = 0.0f;
			woodOffset[2] = 0.0f;
		}

		string name;
		// standard  phong
		float ambient;
		float diffuse;
		float baseColor[3];
		float specular;
		float shininess;
		// transparency control
		float opacity;
		// beckman.
		float roughness;
		// car paint.
		float polished;
		// HDR and frensel reflection.
		string environmentMap;
		float frenselExp;
		float reflectionMin;
		float reflectionMax;
		float exposure;
		// Optix.
		float reflectivity;
		float ior;
		// bump.
		float bumpHeight;
		float bumpScale;
		bool  bumpIsNormalMap;
		string bumpTexture;
	
		// Wood properties , zero scale is no-wood.
		bool isWood;
		float woodScale;
		float wooble;
		float ringScale;
		float woodOffset[3];
		nvsg::StateSetSharedPtr m_cachedStateSet;
		string m_shaderFileName;
		
		nvmath::Vec3f GetSpecularColor ()
		{
				float specR,specG,specB;
				if (specular >=0)
				{
				// Specular is always brigther. Use 1 for white , 0 for base color.
					specR = 1.0f * specular + (1.0f-specular) * baseColor[0];
					specG = 1.0f * specular + (1.0f-specular) * baseColor[1];
					specB = 1.0f * specular + (1.0f-specular) * baseColor[2];
				} else
				{
					float specAbs = abs (specular);
					specR = (specAbs) * baseColor[0];
					specG = (specAbs) * baseColor[1];
					specB = (specAbs) * baseColor[2];
				}
				return nvmath::Vec3f (specR,specG,specB);
		}
		nvmath::Vec3f GetAmbientColor ()
		{
			return (ambient * nvmath::Vec3f (baseColor[0],baseColor[1],baseColor[2]));
		}
		nvmath::Vec3f GetDiffuseColor ()
		{
				float diffR,diffG,diffB;
				// Special logic for darker / brighther diffuse colors.
				// Dark diffuse is good for metal , bright is good for plastic.
				if (diffuse >= 0)
				{
					diffR = 1.0f * diffuse + (1.0f-diffuse) * baseColor[0];
					diffG = 1.0f * diffuse + (1.0f-diffuse) * baseColor[1];
					diffB = 1.0f * diffuse + (1.0f-diffuse) * baseColor[2];
				} else
				{
					float diffuseAbs = abs (diffuse);
					diffR =  (diffuse) * baseColor[0];
					diffG =  (diffuse) * baseColor[1];
					diffB =  (diffuse) * baseColor[2];
				}
				return nvmath::Vec3f (diffR,diffG,diffB);
		}
		bool SetFloatUniform (CgFxSharedPtr tessCgFx , string paramName, float value)
		{
			bool result = true;
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName (paramName);
			if (pone)
			{
				float vf = value;
				CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf);
			}
			return result;
		}
		bool SetFloatVec3Uniform (CgFxSharedPtr tessCgFx , string paramName, float value1,float value2,float value3,float value4)
		{
			bool result = true;
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName (paramName);
			if (pone)
			{
				float vfp[4];
				vfp[0]= value1;
				vfp[1] = value2;
				vfp[2] = value3;
				vfp[3] = value4;
				CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,vfp);
			}
			return result;
		}
		bool SetSamplerUniform (CgFxSharedPtr tessCgFx , string paramName, string filename,unsigned int &width,unsigned int &height)
		{
			bool result = true;
			TextureHostSharedPtr tex = 0;
			std::vector<std::string> searchPaths; 
				 
			searchPaths.push_back (string (GetProccessDirectory())+ string ("textures\\"));
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();

			for (size_t j=0;j<GlobalTextureStack.size();j++)
			{
				if (TextureHostReadLock (GlobalTextureStack[j])->getFileName () == filename )
				{
					tex = GlobalTextureStack[j];
				}
			}
			if (tex == 0)
			{
				// not found in global cache. lets create it and add to cache.
				tex = TextureHost::createFromFile (filename,searchPaths); 
				GlobalTextureStack.push_back (tex);
			}

			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName (paramName);
			
			if (samEnv && tex && ((TextureHostWriteLock( tex ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv) )))
			{
					CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,tex);
					width = TextureHostWriteLock (tex)->getWidth();
					height = TextureHostWriteLock (tex)->getHeight();
			}
			
			return result;
		}
		nvsg::CgFxSharedPtr LoadAvocadoShader (std::string ifilename )
			{
			CgFxSharedPtr tessCgFx = CgFx::create();  
			CgFxEffectWriteLock effect( CgFxReadLock( tessCgFx )->getEffect() );
			std::string file;
			std::string err;
			std::vector<std::string> spats;
			spats.push_back (string (GetProccessDirectory())+ string ("effects\\"));
			spats.push_back (string (GetProccessDirectory())+ string ("scenes\\"));
			spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

			if (   !FindFileFirst( ifilename, spats, file )
				|| !effect->createFromFile( file, spats, err ) )
			{
				return( CgFxSharedPtr() );
			}
			return tessCgFx;
		}
		bool CreateShaderLogic ()
		{
			bool skipFres = false;
			// First lets create the state set with the common params.
			nvsg::StateSetSharedPtr	newStateSet = nvutil::createDefinedMaterial (
			  GetAmbientColor(), 
			  GetDiffuseColor (), 
			  GetSpecularColor (),
			  shininess,	
			  nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor - not just yet.
			  opacity, 
			  reflectivity, 
			  ior 
			  );

			StateSetWriteLock (newStateSet)->setName (name);
			CgFxSharedPtr tessCgFx;
			// Now Pick a shader and set specific params.
			if (isWood)
			{
				m_shaderFileName = "BumpReflectWood.cgfx";
				tessCgFx = LoadAvocadoShader (m_shaderFileName);
				
				SetFloatUniform (tessCgFx, "RingScale",ringScale);
				SetFloatUniform (tessCgFx, "AmpScale",wooble);
				SetFloatUniform (tessCgFx, "WoodScale",woodScale);
				SetFloatUniform (tessCgFx, "WoodOffsetX",woodOffset[0]);
				SetFloatUniform (tessCgFx, "WoodOffsetY",woodOffset[1]);
				SetFloatUniform (tessCgFx, "WoodOffsetZ",woodOffset[2]);
				if (bumpTexture != string (""))
				{
					SetFloatUniform (tessCgFx, "bumpHeight",bumpHeight);
					unsigned int texW = 0;
					unsigned int texH = 0;
					SetSamplerUniform (tessCgFx, "normalMapSampler",bumpTexture,texW,texH);
				}
			}
			else if (bumpTexture != string (""))
			{
				if (environmentMap != string (""))
				{
					//  bump with reflect.
					if (opacity < 1.0)
					{
						m_shaderFileName = "BumpReflectPhongHDRGlass.cgfx";
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
					}
					else
					{
						m_shaderFileName = "BumpReflectPhongHDR.cgfx";
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
					} // end if transparent
					SetFloatUniform (tessCgFx, "bumpHeight",bumpHeight);
					unsigned int texW = 0;
					unsigned int texH = 0;
					SetSamplerUniform (tessCgFx, "normalMapSampler",bumpTexture,texW,texH);
					if (bumpIsNormalMap)
					{
						// This tells the shader that we are using a normal map and not a height map.
						// A normal map is better quality but is also tangent space dependent.
						texW = 0;
						texH = 0;
					}
					SetFloatUniform (tessCgFx, "texDimWidth",float(texW));
					SetFloatUniform (tessCgFx, "texDimHeight",float(texH));

					// specific HDR / PAINT params.
					SetFloatUniform (tessCgFx, "Exposure",exposure);
					SetSamplerUniform (tessCgFx, "textureHDR_RGBA",environmentMap,texW,texH);
					SetFloatUniform (tessCgFx, "GrayTarget",0.33f); // fixed gray target for now.
				} 
				else
				{
					// simple bump , no reflect.
					m_shaderFileName = "BumpReflectPhong.cgfx";
					tessCgFx = LoadAvocadoShader (m_shaderFileName);
					SetFloatUniform (tessCgFx, "bumpHeight",bumpHeight);
					unsigned int texW = 0;
					unsigned int texH = 0;
					SetSamplerUniform (tessCgFx, "normalMapSampler",bumpTexture,texW,texH);
					if (bumpIsNormalMap)
					{
						// This tells the shader that we are using a normal map and not a height map.
						// A normal map is better quality but is also tangent space dependent.
						texW = 0;
						texH = 0;
					}
					SetFloatUniform (tessCgFx, "texDimWidth",float(texW));
					SetFloatUniform (tessCgFx, "texDimHeight",float(texH));
				}  // end if environemtn bump
			}  // end if bump
			else
			{
				if (environmentMap != string (""))
				{
					if (opacity < 1.0f)
					{
						// reflection with transparency , use glss shader..
						m_shaderFileName = "glass_hdr.fx";
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
						unsigned int texW = 0;
						unsigned int texH = 0;
						SetSamplerUniform (tessCgFx, "textureHDR_RGBA",environmentMap,texW,texH);
						// specific HDR / PAINT params.
						SetFloatUniform (tessCgFx, "Exposure",exposure);
						SetFloatUniform (tessCgFx, "GrayTarget",0.33f); // fixed gray target for now.
						//	SetFloatVec3Uniform (tessCgFx, "lacquer",baseColor[0],baseColor[1],baseColor[2],1.0f);
					}
					else
					{
						//reflective material. no bumps.
						m_shaderFileName = "car_paint_hdr.fx";
						tessCgFx = LoadAvocadoShader (m_shaderFileName);
						unsigned int texW = 0;
						unsigned int texH = 0;
						SetSamplerUniform (tessCgFx, "textureHDR_RGBA",environmentMap,texW,texH);
						// specific HDR / PAINT params.
						SetFloatUniform (tessCgFx, "Exposure",exposure);
						SetFloatUniform (tessCgFx, "GrayTarget",polished);
						SetFloatVec3Uniform (tessCgFx, "lacquer",baseColor[0],baseColor[1],baseColor[2],1.0f);
					}  // end if transparent
				} 
				else
				{
					// simple material , no bumps , no reflect.
					m_shaderFileName = "phong_gloss.fx";
					tessCgFx = LoadAvocadoShader (m_shaderFileName);
				}  // end if environment-simple
			}
			
			// Simple common parameters - 
			// Roughness , frenselExp , reflectMin,reflectMax;
			if (!skipFres)
			{
				// for bump mapped shader (and actually in all the beckmann functions ) , we use a fixed frens parameter.. should be tuned up as well..no time..
				SetFloatUniform (tessCgFx, "minRefl",reflectionMin);
				SetFloatUniform (tessCgFx, "maxRefl",reflectionMax);
				SetFloatUniform (tessCgFx, "frenselEXP",frenselExp);
			}
			SetFloatUniform (tessCgFx, "roughness",roughness);

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);

			if (opacity<1.0)
			{
							FaceAttributeSharedPtr fa = FaceAttribute::create ();
							BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
							BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
							FaceAttributeWriteLock (fa)->setCullMode (false,false);
							StateSetWriteLock (newStateSet)->addAttribute (fa);
							StateSetWriteLock (newStateSet)->addAttribute (ba);
			}
			// save the state set for later updates.
			m_cachedStateSet = newStateSet;
			return true;
		}
	};

	std::string GetProccessDirectory ()
	{ 
		// windows specific stuff inside the engine is never a good thing :(
		 char  szPath[MAX_PATH];
		if (GetModuleFileNameA(NULL, szPath, ARRAYSIZE(szPath)))
		{
			std::string mfp (szPath);
			mfp = mfp.substr (0,mfp.rfind ("\\"));
			mfp += string ("\\media\\");
			return mfp; 
		}
		return string(".\\media\\");
	}
	const char avocadoSearchPaths[] = ".\\media\\";
	// TODO: THIS ENTIRE THING BELOW SHOULD HAVE ITS OWN MODULE.
	// MANY MANY CRAP BELOW CAN BE OPTIMIZED AND CLEANER.

	// We currently keep global shared pointers to the env map textures.
	// Ofcouse this is temporary.. i nfuture - material module should
	// handle all textures and shaders.. we also want an expandable material library.
	// its currently very static :)
	static TextureHostSharedPtr staticTexPtr_CarPaintHdr = 0;
	static TextureHostSharedPtr staticTexPtr_CarPaint = 0;
	static TextureHostSharedPtr staticTexEnvPtr_Glass = 0;
	static TextureHostSharedPtr staticTexEnvPtr_GlassHdr = 0;
	static TextureHostSharedPtr staticTexEnvPtr_Bump = 0;
	static TextureHostSharedPtr staticTexEnvPtr_BumpNormal = 0;
	static TextureHostSharedPtr staticTexEnvPtr_BumpNormal2 = 0;
	static TextureHostSharedPtr staticTexEnvPtr_BumpNormal3 = 0;
	static TextureHostSharedPtr staticTexEnvPtr_BumpNormal4 = 0;
	static TextureHostSharedPtr staticTexEnvPtr_BumpNormal5 = 0;
	static TextureHostSharedPtr staticTexEnvPtr_BumpNormal6 = 0;
	static TextureHostSharedPtr staticTexEnvPtr_NoiseMap = 0;
	static int wasInitGlass = 0;
	static int wasInitGlassHdr = 0;
	static int wasInitHdr = 0;
	static int wasInitBump = 0;
	static int wasInitBump2 = 0;
	static int wasInitBump3 = 0;
	static int wasInitBump4 = 0;
	static int wasInitBump5 = 0;
	static int wasInitBump6 = 0;
	static int wasInitNoiseMap = 0;
#define _AVOHDRI 1
	// VERY BAD STUFF  !!! we must clean up the global textures before we try to destroy
	// the document , or else.. scenix will try to dimiss the textures but we still 
	// have a refference to it. of course - a seperate material module will resolve this.
	static nvsg::StateSetSharedPtr createBasicMaterial  ( float r,float g, float b, float amb,float diff, float spec, float sh, float refl, float refr,float specFactor)
	{
		float specR,specG,specB;
		specR = 1.0f * spec + (1.0f-spec) * r;
		specG = 1.0f * spec + (1.0f-spec) * g;
		specB = 1.0f * spec + (1.0f-spec) * b;

		nvsg::StateSetSharedPtr newSS = nvutil::createDefinedMaterial(
			amb * nvmath::Vec3f (r,g,b), //ambient color
			diff * nvmath::Vec3f (r,g,b), // diffuse color
			specFactor*nvmath::Vec3f(specR,specG , specB ),  // specularColor,
			sh,								// specularExponent,
			nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
			1.0f,								// opacity,
			refl,								// reflectivity,
			1.00f);
		return newSS;
	}
	void ClearAvocadoShaders ()
	{
		staticTexEnvPtr_Glass = 0;
		staticTexEnvPtr_GlassHdr = 0;
		staticTexPtr_CarPaint = 0;
		staticTexPtr_CarPaintHdr = 0;
		staticTexEnvPtr_Bump = 0;
		staticTexEnvPtr_BumpNormal = 0;
		staticTexEnvPtr_BumpNormal2 = 0;
			staticTexEnvPtr_BumpNormal3 = 0;
			staticTexEnvPtr_BumpNormal4 = 0;
			staticTexEnvPtr_BumpNormal5 = 0;
			staticTexEnvPtr_BumpNormal6 = 0;
			staticTexEnvPtr_NoiseMap = 0;
			for (size_t k=0;k<GlobalTextureStack.size();k++)
			{
				GlobalTextureStack[k] = 0;
			}
	}
	nvsg::CgFxSharedPtr CreateAvocadoShader (std::string ifilename )
	{
		CgFxSharedPtr tessCgFx = CgFx::create();  
		CgFxEffectWriteLock effect( CgFxReadLock( tessCgFx )->getEffect() );
		std::string file;
		std::string err;
		std::vector<std::string> spats;
		spats.push_back (string (GetProccessDirectory())+ string ("effects\\"));
		spats.push_back (string (GetProccessDirectory())+ string ("scenes\\"));
		spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

		if (   !FindFileFirst( ifilename, spats, file )
			|| !effect->createFromFile( file, spats, err ) )
		{
			return( CgFxSharedPtr() );
		}
		return tessCgFx;
	}
	nvsg::StateSetSharedPtr MaterialLibraryFetch (int matID, int r, int g, int b)
	{
		float rf,gf,bf;

		rf = float (r) / 255.0f;
		gf = float (g) / 255.0f;
		bf = float (b) / 255.0f;
		if (rf < 0)
			rf = 0.5f;
		if (bf < 0)
			bf = 0.5f;
		if (gf < 0)
			gf = 0.5f;
		nvsg::StateSetSharedPtr newStateSet;
		 if (matID == 30)
		{
#if 0
			//Sketch
			// HIGH DIFFUSE , SPECULAR 1.0 , HIGH EXPONENT.
			float ambientFactor = 0.4f;
			float diffuseFactor = 0.15f;
			float diffR,diffG,diffB;

			diffR = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(1.0f ,1.0f , 1.0f ),  // specularColor,
				92.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );

			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("phong.fx");

			tessCgFx = CreateAvocadoShader ("SketchShaderDraw.cgfx");

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
#endif
			MaterialBase mb;
			mb.baseColor[0] = rf; mb.baseColor[1] = gf;mb.baseColor[2] = bf;
			mb.ambient = 0.4f;
			mb.diffuse = 0.1f;
			mb.specular = 0.5f;
			mb.shininess = 13.0f;
			mb.roughness = 0.4f;
			mb.frenselExp = 3.5f;
			mb.opacity = 0.5f;
			mb.reflectionMax = 0.7f;
			mb.reflectionMin = 0.25f;
			mb.polished = 1.0f;
			mb.bumpTexture = "mybump3.dds";
			mb.environmentMap = "crosscube.hdr";
			mb.isWood = true;
			mb.wooble = 0.7f;
			mb.woodScale = 19.0f;
			mb.ringScale = 2.47f;
			mb.woodOffset[0] = -10.0;
			mb.woodOffset[0] =  -11.0;
			mb.woodOffset[0] =  7.0f;

			mb.CreateShaderLogic ();
			
			newStateSet = mb.m_cachedStateSet;
		} else
		if (matID == 0)
		{
			//MATT
			newStateSet = nvutil::createDefinedMaterial
				(nvmath::Vec3f( 0.5f, 0.5f, 0.5f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(0.5f,0.5f,0.5f),      // specularColor,
				2.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.00f,								// reflectivity,
				1.0f );	
		} 
		else if (matID == 1)
		{
			/// STEEL	
#if 0
			newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(0.5f,0.5f,0.5f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				2.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.00f,								// reflectivity,
				1.0f );	

#endif
			newStateSet = createBasicMaterial (rf,gf,bf,0.25f,0.75f,0.2f,15.3f,0.1f,1.0f,1.0f);    
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("phong.fx");

			tessCgFx = CreateAvocadoShader ("phong.fx");

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");

		}
		else if (matID == 2)
		{
			//PLASTIC
			// HIGH DIFFUSE , SPECULAR 1.0 , HIGH EXPONENT.
			float ambientFactor = 0.2f;
			float diffuseFactor = 0.0f;
			float diffR,diffG,diffB;

			diffR = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(1.0f ,1.0f , 1.0f ),  // specularColor,
				92.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );

			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("phong.fx");

			tessCgFx = CreateAvocadoShader ("phong_gloss.fx");

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
		}
		//else if (matID == 10)
		//{
		//	// rubber.. put some bumps on that plastic..
		//}
		else if (matID == 15)
		{
			//TRANSPARENT PLASTIC
			// HIGH DIFFUSE , SPECULAR 1.0 , HIGH EXPONENT.
			float ambientFactor = 0.4f;
			float diffuseFactor = 0.05f;
			float diffR,diffG,diffB;

			diffR = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(0.0f ,0.0f , 0.0f ),  // specularColor,
				0.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				0.35f,//0.75f,								// opacity,
				0.0051f,								// reflectivity,
				1.0f );

			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("phong.fx");

			tessCgFx = CreateAvocadoShader ("phong_gloss.fx");

			FaceAttributeSharedPtr fa = FaceAttribute::create ();
			BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);//SOURCE_ALPHA);
			FaceAttributeWriteLock (fa)->setCullMode (false,false);
			FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

			StateSetWriteLock (newStateSet)->addAttribute (fa);
			StateSetWriteLock (newStateSet)->addAttribute (ba);
		//	StateSetWriteLock (newStateSet)
			//StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
		}
		else if (matID == 3)
		{
			// METAL	
			// Low DIFFUSE. HIGH SPECULAR COLOR, LOW EXPONENT.

			newStateSet = createBasicMaterial (rf,gf,bf,0.4f,0.8f,0.3f,12.3f,0.33f,1.0f,1.0f);

			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("phong.fx");

			tessCgFx = CreateAvocadoShader ("phong.fx");

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
		}
		else if (matID == 4)
		{
			// GLASS
			newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.3f, 0.3f, 0.3f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f,1.0f,1.0f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				17.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				0.02f,								// opacity,
				0.0251f,								// reflectivity,
				1.2f );	

			float vf1 = 0.0f;
			float vf2 = 1.0f;
			float vf3 = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("glass.fx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("Min");
			CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
			CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);


			TextureHostSharedPtr ee ;	
			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("environmentMapSampler");
			if (wasInitGlass == 0)
			{
				wasInitGlass = 1;
				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\");		 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				staticTexEnvPtr_Glass = ee;
			} else
				ee = TextureHostSharedPtr (staticTexEnvPtr_Glass);

			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			FaceAttributeSharedPtr fa = FaceAttribute::create ();
			BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			FaceAttributeWriteLock (fa)->setCullMode (false,false);
			FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

			StateSetWriteLock (newStateSet)->addAttribute (fa);
			StateSetWriteLock (newStateSet)->addAttribute (ba);
StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
		}
		else if (matID == 5)
		{
			newStateSet = createBasicMaterial (rf,gf,bf,0.4f,0.6f,0.83f,30.0f,0.75f,1.0f,1.0f);

			float vf1 = 0.0f;
			float vf2 = 1.0f;
			float vf3 = 1.0f;
			float col[4];
			col[0] = rf;
			col[1] = gf;
			col[2] = bf;
			col[3] = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");
			static int wasInit = 0;

			tessCgFx = CreateAvocadoShader ("car_paint.fx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("minRefl");
			CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("maxRefl");
			CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("fresnelEXP");
			CgFxParameter pcolor = CgFxEffectWriteLock (eff)->getTweakableByName ("lacquer");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			CgFxEffectWriteLock (eff)->setFloatParameterValue (pcolor,col);

			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("TextureENV");
			TextureHostSharedPtr ee ;

			if (wasInit == 0)
			{
				wasInit = 1;

				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\"); 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				ee = TextureHost::createFromFile ("nvlobby_new.dds",spats);
				if (ee)
				{
					staticTexPtr_CarPaint = ee;
				}
			} else
				ee = TextureHostSharedPtr (staticTexPtr_CarPaint);
			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}

StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
			// CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			//  CgFxParameter pcolor = CgFxEffectWriteLock (eff)->getTweakableByName ("lacquer");
			//  CgFxEffectWriteLock (eff)->setFloatParameterValue (pcolor,col);
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
		}
		else if (matID==6)
		{
			// Thin GLASS
			newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( rf,gf,bf),//0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(0.25f,0.25f,0.25f),//rf,rf,rf),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				50.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				0.025f,								// opacity,
				0.22f,								// reflectivity,
				1.09f );	

			float vf1 = 0.0f;
			float vf2 = 0.6f;
			float vf3 = 1.0f;
			float vf5 = -0.65f;
			//float vf4 = 12.0f;
			float vf6 = 0.33f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");
#ifdef _AVOHDRI
			tessCgFx = CreateAvocadoShader ("glass_hdr.fx");
#else
			tessCgFx = CreateAvocadoShader ("glass.fx");
#endif
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("minRefl");
			CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("maxRefl");
			CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("fresnelEXP");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);
#ifdef _AVOHDRI
			CgFxParameter hdri_pfive = CgFxEffectWriteLock (eff)->getTweakableByName ("Exposure");
			CgFxParameter hdri_psix = CgFxEffectWriteLock (eff)->getTweakableByName ("GrayTarget");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_pfive,&vf5);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_psix,&vf6);
#endif

			TextureHostSharedPtr ee ;	
#ifdef _AVOHDRI
			CgFxParameter samEnvhdr = CgFxEffectWriteLock (eff)->getSamplerByName ("textureHDR_RGBA");
			if (wasInitGlassHdr == 0)
			{
				wasInitGlassHdr = 1;
				std::vector<std::string> spats;
			//	spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\");		 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				ee = TextureHost::createFromFile ("crosscube.hdr",spats); 
				staticTexEnvPtr_GlassHdr = ee;
			}
			else
				ee = TextureHostSharedPtr (staticTexEnvPtr_GlassHdr);

			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnvhdr ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnvhdr,ee);
			}
#else
			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("environmentMapSampler");
			if (wasInitGlass == 0)
			{
				wasInitGlass = 1;
				std::vector<std::string> spats;
			//	spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\");		 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				staticTexEnvPtr_Glass = ee;
			} else
				ee = TextureHostSharedPtr (staticTexEnvPtr_Glass);

			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}

#endif
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			FaceAttributeSharedPtr fa = FaceAttribute::create ();
			BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			FaceAttributeWriteLock (fa)->setCullMode (false,false);
			StateSetWriteLock (newStateSet)->addAttribute (fa);
			StateSetWriteLock (newStateSet)->addAttribute (ba);
		}
		else if (matID==7)
		{
			// DIAMOND GLASS
			newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(0.9f,0.9f,0.9f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				6.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				0.3f,								// opacity,
				0.35f,								// reflectivity,
				2.23f );	

			float vf1 = 0.0f;
			float vf2 = 0.7f;
			float vf3 = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("glass.fx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("Min");
			CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
			CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			TextureHostSharedPtr ee ;	
			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("environmentMapSampler");
			if (wasInitGlass == 0)
			{
				wasInitGlass = 1;
				std::vector<std::string> spats;
				 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				staticTexEnvPtr_Glass = ee;
			} else
				ee = TextureHostSharedPtr (staticTexEnvPtr_Glass);

			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}

			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			FaceAttributeSharedPtr fa = FaceAttribute::create ();
			BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			FaceAttributeWriteLock (fa)->setCullMode (false,false);
			StateSetWriteLock (newStateSet)->addAttribute (fa);
			StateSetWriteLock (newStateSet)->addAttribute (ba);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
		}
		else if (matID == 13)
		{
			// just paint , lets go crazy with reflection and specular..
			newStateSet = createBasicMaterial (rf,gf,bf,0.4f,0.50f,0.92f,60.0f,0.2f,1.06f,1.0f);

			float vf1 = 0.12f;
			float vf2 = 0.7f;
			float vf3 = 3.5f;//1.2f;
			float vf5 = -1.15f;//0.65f;
			float vf4 = 12.0f;
			float vf6 = 0.5f;
			float vf7 = 0.12f; // gray target tweaked to control poslihness
			float col[4];
			//scale down diffuse
			col[0] = rf;
			col[1] = gf ;
			col[2] = bf ;
			col[3] = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");
			static int wasInit = 0;

#ifdef _AVOHDRI
			tessCgFx = CreateAvocadoShader ("car_paint_hdr.fx");
#else
			tessCgFx = CreateAvocadoShader ("car_paint.fx");
#endif
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("minRefl");
			CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("maxRefl");
			CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("fresnelEXP");
			CgFxParameter pcolor = CgFxEffectWriteLock (eff)->getTweakableByName ("lacquer");
#ifdef _AVOHDRI
			//CgFxParameter hdri_pfour = CgFxEffectWriteLock (eff)->getTweakableByName ("shininess");
			CgFxParameter hdri_pfive = CgFxEffectWriteLock (eff)->getTweakableByName ("Exposure");
			CgFxParameter hdri_psix = CgFxEffectWriteLock (eff)->getTweakableByName ("GrayTarget");
			//hdri	
			//CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_pfour,&vf4);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_pfive,&vf5);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_psix,&vf6);

			//CgFxParameter hdri_pseven = CgFxEffectWriteLock (eff)->getTweakableByName ("Roughness");
			//CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_pseven,&vf7);
#endif
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			CgFxEffectWriteLock (eff)->setFloatParameterValue (pcolor,col);


			TextureHostSharedPtr ee ;
#ifdef _AVOHDRI
			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("textureHDR_RGBA");
			if (wasInitHdr == 0)
			{
				wasInitHdr = 1;

				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\"); 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats);
				ee = TextureHost::createFromFile ("crosscube.hdr",spats);
				if (ee)
				{
					staticTexPtr_CarPaintHdr = ee;
				}
			} else
				ee = TextureHostSharedPtr (staticTexPtr_CarPaintHdr);
			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}

#else
			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("TextureENV");
			

			if (wasInit == 0)
			{
				wasInit = 1;

				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\"); 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				ee = TextureHost::createFromFile ("nvlobby_new.dds",spats);
				if (ee)
				{
					staticTexPtr_CarPaint = ee;
				}
			} else
				ee = TextureHostSharedPtr (staticTexPtr_CarPaint);

			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}
#endif

			// CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			//  CgFxParameter pcolor = CgFxEffectWriteLock (eff)->getTweakableByName ("lacquer");
			//  CgFxEffectWriteLock (eff)->setFloatParameterValue (pcolor,col);
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
		}
		else if (matID == 14)
		{
			// shiny paint , lets go crazy with reflection and specular..
			newStateSet = createBasicMaterial (rf,gf,bf,0.4f,0.80f,0.92f,60.0f,0.9f,1.15f,1.0f);


			//float vf1 = 0.0f;
			//float vf2 = 1.0f;
			
			float vf1 = 0.2f;
			float vf2 = 0.75f;
			float vf3 = 1.0f;
			float vf5 = -0.65f;
			float vf4 = 12.0f;
			float vf6 = 0.33f;
			float col[4];
			col[0] = rf;
			col[1] = gf;
			col[2] = bf;
			col[3] = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");
			static int wasInit = 0;
#ifdef _AVOHDRI
			tessCgFx = CreateAvocadoShader ("car_paint_hdr.fx");
#else
			tessCgFx = CreateAvocadoShader ("car_paint.fx");
#endif
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("minRefl");
			CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("maxRefl");
			CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("fresnelEXP");
			CgFxParameter pcolor = CgFxEffectWriteLock (eff)->getTweakableByName ("lacquer");
#ifdef _AVOHDRI
			//CgFxParameter hdri_pfour = CgFxEffectWriteLock (eff)->getTweakableByName ("shininess");
			CgFxParameter hdri_pfive = CgFxEffectWriteLock (eff)->getTweakableByName ("Exposure");
			CgFxParameter hdri_psix = CgFxEffectWriteLock (eff)->getTweakableByName ("GrayTarget");
			//hdri	
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_pfour,&vf4);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_pfive,&vf5);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (hdri_psix,&vf6);
#endif

			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			CgFxEffectWriteLock (eff)->setFloatParameterValue (pcolor,col);


			TextureHostSharedPtr ee ;
#ifdef _AVOHDRI
			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("textureHDR_RGBA");
			if (wasInitHdr == 0)
			{
				wasInitHdr = 1;

				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\"); 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats);
				ee = TextureHost::createFromFile ("crosscube.hdr",spats);
				if (ee)
				{
					staticTexPtr_CarPaintHdr = ee;
				}
			} else
				ee = TextureHostSharedPtr (staticTexPtr_CarPaintHdr);
			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}

#else

			CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("TextureENV");
			if (wasInit == 0)
			{
				wasInit = 1;

				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\"); 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));

				ee = TextureHost::createFromFile ("nvlobby_new.dds",spats);

				if (ee)
				{
					staticTexPtr_CarPaint = ee;
				}
			} else
				ee = TextureHostSharedPtr (staticTexPtr_CarPaint);
			if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}

#endif
			

			

			// CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			//  CgFxParameter pcolor = CgFxEffectWriteLock (eff)->getTweakableByName ("lacquer");
			//  CgFxEffectWriteLock (eff)->setFloatParameterValue (pcolor,col);
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_Phong");
		}
		else if (matID == 11)
		{
			float ambientFactor = 0.4f;
			float diffuseFactor = 0.12f;
			float diffR,diffG,diffB;

			diffR = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (diffR,diffG,diffB),//diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(1.0f ,1.0f , 1.0f ),  // specularColor,
				92.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );
			// LEATHER
			/*newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.3f, 0.3f, 0.3f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f,1.0f,1.0f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				17.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.0251f,								// reflectivity,
				1.0f );	*/

			float vf1 = 0.817f;//007f;
			float vf2 = 1.0f;
			float vf3 = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("BumpReflectPhong.cgfx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("bumpHeight");
		//	CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
	//		CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			
			TextureHostSharedPtr bumpee ;	
			CgFxParameter bumpsamEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("normalMapSampler");		

			//TextureHostSharedPtr ee ;	
			//CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("envMapSampler");
			if (wasInitBump == 0)
			{
				wasInitBump = 1;
				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\");		 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				bumpee = TextureHost::createFromFile ("mybump.dds"/*"default_bump_normal.dds"*/,spats); 
				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				//staticTexEnvPtr_Bump = ee;
				staticTexEnvPtr_BumpNormal=bumpee;
			} else
			{
				bumpee = TextureHostSharedPtr (staticTexEnvPtr_BumpNormal);
				//ee = TextureHostSharedPtr (staticTexEnvPtr_Bump);
			}

			//if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
			//	CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}
			if (bumpee && ((TextureHostWriteLock( bumpee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( bumpsamEnv ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (bumpsamEnv,bumpee);
			}
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
		//	FaceAttributeSharedPtr fa = FaceAttribute::create ();
		//	BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			//BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			//FaceAttributeWriteLock (fa)->setCullMode (false,false);
		//	FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

		//	StateSetWriteLock (newStateSet)->addAttribute (fa);
		//	StateSetWriteLock (newStateSet)->addAttribute (ba);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_PhongBump");
		}
		else if (matID == 12)
		{
			float ambientFactor = 0.4f;
			float diffuseFactor = 0.12f;//5f;
			float diffR,diffG,diffB;

			diffR =0.5f*rf;//= 1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG =0.5f*gf;//= 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB =0.5f*bf;//= 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (diffR,diffG,diffB),//(diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(0.95f ,0.95f , 0.95f ),  // specularColor,
				92.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );
			// LEATHER
			/*newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.3f, 0.3f, 0.3f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f,1.0f,1.0f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				17.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.0251f,								// reflectivity,
				1.0f );	*/

			float vf1 = 1.14f;//007f;
			float vf2 = 1.0f;
			float vf3 = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("BumpReflectPhong.cgfx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("bumpHeight");
		//	CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
	//		CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			
			TextureHostSharedPtr bumpee2 ;	
			CgFxParameter bumpsamEnv2 = CgFxEffectWriteLock (eff)->getSamplerByName ("normalMapSampler");		

			//TextureHostSharedPtr ee ;	
			//CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("envMapSampler");
			if (wasInitBump2 == 0)
			{
				wasInitBump2 = 1;
				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\");		 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				bumpee2 = TextureHost::createFromFile ("mybump2.dds"/*"default_bump_normal.dds"*/,spats); 
				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				//staticTexEnvPtr_Bump = ee;
				staticTexEnvPtr_BumpNormal2=bumpee2;
			} else
			{
				bumpee2 = TextureHostSharedPtr (staticTexEnvPtr_BumpNormal2);
				//ee = TextureHostSharedPtr (staticTexEnvPtr_Bump);
			}

			//if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
			//	CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}
			if (bumpee2 && ((TextureHostWriteLock( bumpee2 ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( bumpsamEnv2 ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (bumpsamEnv2,bumpee2);
			}
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
		//	FaceAttributeSharedPtr fa = FaceAttribute::create ();
		//	BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			//BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			//FaceAttributeWriteLock (fa)->setCullMode (false,false);
		//	FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

		//	StateSetWriteLock (newStateSet)->addAttribute (fa);
		//	StateSetWriteLock (newStateSet)->addAttribute (ba);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_PhongBump");
		}
		else if (matID == 16)
		{
			// WOOD SHADER SO COOL !!!
			float ambientFactor = 0.4f;
			float diffuseFactor = 0.08f;
			float diffR,diffG,diffB;

			diffR = 0.5f*rf;//1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 0.5f*gf;//1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 0.5f*bf;//1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (1.0f,1.0f,1.0f),//(diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(0.8f ,0.9f , 0.9f ),  // specularColor,
				23.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );
			// LEATHER
			/*newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.3f, 0.3f, 0.3f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f,1.0f,1.0f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				17.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.0251f,								// reflectivity,
				1.0f );	*/

			float vf1 = 0.8f; // bump height

			float vf2 = 0.7f; // "Wobbliness";
			float vf3 = 19.0; //  WoodScale 
			float vf4[3];     // log center WoodOffset 
			vf4[0] = -10.0f;
			vf4[1] = -11.0f;
			vf4[1] = 7.0f;
			float vf5 = 2.46f;//56f;//0.46f; // RingScale 

			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("BumpReflectWood.cgfx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();

			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("bumpHeight");
			CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("AmpScale");
			CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("WoodScale");
			CgFxParameter pfourX = CgFxEffectWriteLock (eff)->getTweakableByName ("WoodOffsetX");
			CgFxParameter pfourY = CgFxEffectWriteLock (eff)->getTweakableByName ("WoodOffsetY");
			CgFxParameter pfourZ = CgFxEffectWriteLock (eff)->getTweakableByName ("WoodOffsetZ");
			CgFxParameter pfive = CgFxEffectWriteLock (eff)->getTweakableByName ("RingScale");
		//	CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
	//		CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);
			//CgFxEffectWriteLock (eff)->setFloatParameterValue (pfour,&vf4);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pfive,&vf5);
			//CgFxParameter pfour0 = CgFxEffectWriteLock (eff)->getgetArrayParameterElement (pfour,0);
		//	CgFxParameter pfour1 = CgFxEffectWriteLock (eff)->getArrayParameterElement (pfour,1);
		//	CgFxParameter pfour2 = CgFxEffectWriteLock (eff)->getArrayParameterElement (pfour,2);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pfourX,&vf4[0]);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pfourY,&vf4[1]);
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pfourZ,&vf4[2]);

			// bind noise map
			TextureHostSharedPtr noiseTex ;	
			CgFxParameter NoiseSampler = CgFxEffectWriteLock (eff)->getSamplerByName ("NoiseSamp");		
			if (wasInitNoiseMap == 0)

			{
				wasInitNoiseMap =1;
				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\");		 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				noiseTex = TextureHost::createFromFile ("noiseL8_32x32x32.dds"/*"default_bump_normal.dds"*/,spats); 
				staticTexEnvPtr_NoiseMap=noiseTex;
			}
			else
			{
				noiseTex = TextureHostSharedPtr (staticTexEnvPtr_NoiseMap);
			}
			if (noiseTex && ((TextureHostWriteLock( noiseTex ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( NoiseSampler ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (NoiseSampler,noiseTex);
			}
			//end noise map

			TextureHostSharedPtr bumpee3 ;	
			CgFxParameter bumpsamEnv3 = CgFxEffectWriteLock (eff)->getSamplerByName ("normalMapSampler");		

			//TextureHostSharedPtr ee ;	
			//CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("envMapSampler");
			if (wasInitBump3 == 0)
			{
				wasInitBump3 = 1;
				std::vector<std::string> spats;
				//spats.push_back ("C:\\Education\\SceniX72\\media\\textures\\");		 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				bumpee3 = TextureHost::createFromFile ("mybump3.dds"/*"default_bump_normal.dds"*/,spats); 
				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				//staticTexEnvPtr_Bump = ee;
				staticTexEnvPtr_BumpNormal3=bumpee3;
			} else
			{
				bumpee3 = TextureHostSharedPtr (staticTexEnvPtr_BumpNormal3);
				//ee = TextureHostSharedPtr (staticTexEnvPtr_Bump);
			}

			//if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
			//	CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}
			if (bumpee3 && ((TextureHostWriteLock( bumpee3 ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( bumpsamEnv3 ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (bumpsamEnv3,bumpee3);
			}
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
		//	FaceAttributeSharedPtr fa = FaceAttribute::create ();
		//	BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			//BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			//FaceAttributeWriteLock (fa)->setCullMode (false,false);
		//	FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

		//	StateSetWriteLock (newStateSet)->addAttribute (fa);
		//	StateSetWriteLock (newStateSet)->addAttribute (ba);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_PhongBump");
		}
		else if (matID == 17)
		{
			// CRAKCED LEATHER
			float ambientFactor = 0.5f;
			float diffuseFactor = 0.2f;//08f;//5f;
			float diffR,diffG,diffB;

			diffR =1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(0.95f ,0.95f , 0.95f ),  // specularColor,
				25.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );
			// LEATHER
			/*newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.3f, 0.3f, 0.3f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f,1.0f,1.0f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				17.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.0251f,								// reflectivity,
				1.0f );	*/

			float vf1 = 0.8f;//007f;
			float vf2 = 1.0f;
			float vf3 = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("BumpReflectPhong.cgfx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("bumpHeight");
		//	CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
	//		CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			
			TextureHostSharedPtr bumpee4 ;	
			CgFxParameter bumpsamEnv4 = CgFxEffectWriteLock (eff)->getSamplerByName ("normalMapSampler");		

			//TextureHostSharedPtr ee ;	
			//CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("envMapSampler");
			if (wasInitBump4 == 0)
			{
				wasInitBump4 = 1;
				std::vector<std::string> spats;
					 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				bumpee4 = TextureHost::createFromFile ("mybump4.dds"/*"default_bump_normal.dds"*/,spats); 
				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				//staticTexEnvPtr_Bump = ee;
				staticTexEnvPtr_BumpNormal4=bumpee4;
			} else
			{
				bumpee4 = TextureHostSharedPtr (staticTexEnvPtr_BumpNormal4);
				//ee = TextureHostSharedPtr (staticTexEnvPtr_Bump);
			}

			//if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
			//	CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}
			if (bumpee4 && ((TextureHostWriteLock( bumpee4 ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( bumpsamEnv4 ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (bumpsamEnv4,bumpee4);
			}
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
		//	FaceAttributeSharedPtr fa = FaceAttribute::create ();
		//	BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			//BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			//FaceAttributeWriteLock (fa)->setCullMode (false,false);
		//	FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

		//	StateSetWriteLock (newStateSet)->addAttribute (fa);
		//	StateSetWriteLock (newStateSet)->addAttribute (ba);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_PhongBump");
		}		
		else if (matID == 20)
		{
			// ROUGH LEATHER
			float ambientFactor = 0.5f;
			float diffuseFactor = 0.12f;//04f;//5f;
			float diffR,diffG,diffB;

			diffR = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f(diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(0.95f ,0.95f , 0.95f ),  // specularColor,
				34.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );
			// LEATHER
			/*newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.3f, 0.3f, 0.3f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f,1.0f,1.0f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				17.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.0251f,								// reflectivity,
				1.0f );	*/

			float vf1 = 0.5f;//007f;
			float vf2 = 1.0f;
			float vf3 = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("BumpReflectPhong.cgfx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("bumpHeight");
		//	CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
	//		CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			
			TextureHostSharedPtr bumpee6 ;	
			CgFxParameter bumpsamEnv6 = CgFxEffectWriteLock (eff)->getSamplerByName ("normalMapSampler");		

			//TextureHostSharedPtr ee ;	
			//CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("envMapSampler");
			if (wasInitBump6 == 0)
			{
				wasInitBump6 = 1;
				std::vector<std::string> spats;
				 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				bumpee6 = TextureHost::createFromFile ("mybump6.dds"/*"default_bump_normal.dds"*/,spats); 
				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				//staticTexEnvPtr_Bump = ee;
				staticTexEnvPtr_BumpNormal6=bumpee6;
			} else
			{
				bumpee6 = TextureHostSharedPtr (staticTexEnvPtr_BumpNormal6);
				//ee = TextureHostSharedPtr (staticTexEnvPtr_Bump);
			}

			//if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
			//	CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}
			if (bumpee6 && ((TextureHostWriteLock( bumpee6 ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( bumpsamEnv6 ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (bumpsamEnv6,bumpee6);
			}
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
		//	FaceAttributeSharedPtr fa = FaceAttribute::create ();
		//	BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			//BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			//FaceAttributeWriteLock (fa)->setCullMode (false,false);
		//	FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

		//	StateSetWriteLock (newStateSet)->addAttribute (fa);
		//	StateSetWriteLock (newStateSet)->addAttribute (ba);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_PhongBump");
		}		
		else if (matID == 10)
		{
			// Rubber
			float ambientFactor = 0.5f;
			float diffuseFactor = 0.12f;//5f;
			float diffR,diffG,diffB;

			diffR = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * rf;
			diffG = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * gf;
			diffB = 1.0f * diffuseFactor + (1.0f-diffuseFactor) * bf;

			newStateSet = nvutil::createDefinedMaterial
				(ambientFactor*nvmath::Vec3f (diffR,diffG,diffB),//,0.6,0.6),//nvmath::Vec3f( 0.6f, 0.6f, 0.6f ),  // ambientColor,
				nvmath::Vec3f (diffR,diffG,diffB),								 // diffuseColor,
				nvmath::Vec3f(0.95f ,0.95f , 0.95f ),  // specularColor,
				50.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor
				1.0f,								// opacity,
				0.02f,								// reflectivity,
				1.0f );
			
			/*newStateSet = nvutil::createDefinedMaterial(
				nvmath::Vec3f( 0.3f, 0.3f, 0.3f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f,1.0f,1.0f),// rf,gf,bf), //1.0f, 1.0f, 1.0f ),  // specularColor,
				17.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.0251f,								// reflectivity,
				1.0f );	*/

			float vf1 = 0.07f;//007f;
			float vf2 = 1.0f;
			float vf3 = 1.0f;
			CgFxSharedPtr tessCgFx;// = CreateAvocadoShader ("glass.fx");

			tessCgFx = CreateAvocadoShader ("BumpReflectPhong.cgfx");
			CgFxEffectSharedPtr eff = CgFxWriteLock (tessCgFx)->getEffect ();
			CgFxParameter pone = CgFxEffectWriteLock (eff)->getTweakableByName ("bumpHeight");
		//	CgFxParameter ptwo = CgFxEffectWriteLock (eff)->getTweakableByName ("Max");
	//		CgFxParameter pthree = CgFxEffectWriteLock (eff)->getTweakableByName ("FresnelExponent");
			CgFxEffectWriteLock (eff)->setFloatParameterValue (pone,&vf1);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (ptwo,&vf2);
		//	CgFxEffectWriteLock (eff)->setFloatParameterValue (pthree,&vf3);

			
			TextureHostSharedPtr bumpee5 ;	
			CgFxParameter bumpsamEnv5 = CgFxEffectWriteLock (eff)->getSamplerByName ("normalMapSampler");		

			//TextureHostSharedPtr ee ;	
			//CgFxParameter samEnv = CgFxEffectWriteLock (eff)->getSamplerByName ("envMapSampler");
			if (wasInitBump5 == 0)
			{
				wasInitBump5 = 1;
				std::vector<std::string> spats;
						 
				spats.push_back (string (GetProccessDirectory())+ string ("textures\\"));
				bumpee5 = TextureHost::createFromFile ("mybump5.dds"/*"default_bump_normal.dds"*/,spats); 
				//ee = TextureHost::createFromFile ("nvlobby_new.dds",spats); 
				//staticTexEnvPtr_Bump = ee;
				staticTexEnvPtr_BumpNormal5=bumpee5;
			} else
			{
				bumpee5 = TextureHostSharedPtr (staticTexEnvPtr_BumpNormal5);
				//ee = TextureHostSharedPtr (staticTexEnvPtr_Bump);
			}

			//if (ee && ((TextureHostWriteLock( ee ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( samEnv ) )))
			{
			//	CgFxEffectWriteLock (eff)->setSamplerTexture (samEnv,ee);
			}
			if (bumpee5 && ((TextureHostWriteLock( bumpee5 ))->convertToTextureTarget( (CgFxEffectReadLock(eff))->getSamplerTextureTarget( bumpsamEnv5 ) )))
			{
				CgFxEffectWriteLock (eff)->setSamplerTexture (bumpsamEnv5,bumpee5);
			}
			StateSetWriteLock (newStateSet)->addAttribute (tessCgFx);
		//	FaceAttributeSharedPtr fa = FaceAttribute::create ();
		//	BlendAttributeSharedPtr ba = nvsg::BlendAttribute::create();
			//BlendAttributeWriteLock (ba)->setBlendFunctions (BF_SOURCE_ALPHA,BF_ONE_MINUS_SOURCE_ALPHA);
			//FaceAttributeWriteLock (fa)->setCullMode (false,false);
		//	FaceAttributeWriteLock (fa)->setTwoSidedLighting (true);

		//	StateSetWriteLock (newStateSet)->addAttribute (fa);
		//	StateSetWriteLock (newStateSet)->addAttribute (ba);
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_PhongBump");
		}
		else
		{
			newStateSet = nvutil::createDefinedMaterial(nvmath::Vec3f( 0.2f, 0.2f, 0.2f ),  // ambientColor,
				nvmath::Vec3f (rf,gf,bf),								 // diffuseColor,
				nvmath::Vec3f(1.0f, 1.0f, 1.0f ),  // specularColor,
				25.0f,								// specularExponent,
				nvmath::Vec3f( 0.0f, 0.0f, 0.0f ),  // emissiveColor,
				1.0f,								// opacity,
				0.00f,								// reflectivity,
				1.0f );	
			StateSetWriteLock (newStateSet)->setName ("AvocadoMaterial_NoShade");
		}
		return newStateSet;
	}
}
#endif
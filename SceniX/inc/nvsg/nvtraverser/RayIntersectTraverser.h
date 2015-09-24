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

#include "nvsgcommon.h"
#include "nvsg/Drawable.h"
#include "nvsg/Path.h"
#include "nvtraverser/ModelViewTraverser.h"
#include "nvsg/Buffer.h"
#include "nvutil/SmartPtr.h"


namespace nvsg
{
  class Camera;
}

namespace nvtraverser
{
  //! Intersection class.
  /** This class is used to store intersection results generated by the RayIntersectTraverser. 
    * It should only be used with classes derived from RayIntersectTraveser because some 
    * information like the distance makes sense only when knowing the ray data. 
    * \note Needs a valid ViewState, which in turns holds a valid camera. Call setViewState prior to apply().*/
  class Intersection
  {
    public:
      //! Default constructor.
      Intersection(void);

      //! Constructor.
      /** This contructor creates an intersection object from the given data. */
      NVSG_API Intersection( nvsg::Path * pPath                         //!< Path to the intersected drawable.
                           , const nvsg::DrawableSharedPtr & pDrawable  //!< Intersected drawable.
                           , const nvmath::Vec3f & isp                  //!< Intersection point on the drawable.
                           , float dist                                 //!< Distance from the ray origin to the drawable.
                           , unsigned int primitiveIndex                //!< Index of the intersected primitive in the Drawable.
                           , const std::vector<unsigned int> & vertexIndices //!< Indices of the vertices of the intersected primitive.
                           );

      //! Copy constructor.
      /** Creates a new Intersection object from the given one. */
      NVSG_API Intersection( const Intersection & rhs    //!< Intersection object to copy from.
                           );

      //! Default destructor.
      NVSG_API virtual ~Intersection(void);

      //! Assignment operator.
      NVSG_API Intersection & operator = (const Intersection & rhs);

      //! Get the Path to the intersected drawable.
      /** \return Path to the drawable. */
      NVSG_API const nvsg::Path * getPath() const;

      //! Get the intersected drawable.
      /** \return Intersected drawable. */
      NVSG_API const nvsg::DrawableSharedPtr & getDrawable() const;

      //! Get the intersection point on the intersected drawable.
      /** \return Intersection Point. */
      NVSG_API const nvmath::Vec3f & getIsp() const;

      //! Get the distance from the ray origin to the intersection point.
      /** \return Distance from ray origin to intersection point. */
      NVSG_API float getDist() const;

      /*! \brief Get the index of the intersected primitive. 
       *  \return An index to the intersected primitive.
       *  \remarks This function returns the index of the intersected primitive.
       *  e.g. the i-th strip in a TriangleStrip or the i-th triangle in Triangles.
       *  With this index you have the abilitiy to access the primitive and modify it. */
      NVSG_API unsigned int getPrimitiveIndex() const;

      /*! \brief Get the indices if the intersected face, line or point.
       *  \return Indices to the vertex data of the intersected line, face or point.
       *  \remarks The vertices that are returned can be used to access the vertices of the 
       *  intersected object. 
       *  The size of this vector determines the primitive category. E.g. If the size of this vector 
       *  is 3 then the intersected face is a triangle. (4 is a Quad, 2 is a line, 1 a point...)
       *  \code
       *  // How to access the vertices and normals of the intersected triangle in a triangle strip
       *  const Vec3f * vertices = intersection->getDrawable()->getVertexAttributeSet()->getVertices();
       *  const Vec3f * normals = intersection->getDrawable()->getVertexAttributeSet()->getNormals();
       *  const IndexList & strip = intersection->getDrawable()->getStrips()[intersection->getPrimitiveIndex()]; 
       *  const std::vector<unsigned int> & vertIndices = intersection->getVertexIndices(); 
       *
       *  const Vec3f & v0 = vertices[strip[vertIndices[0]]];
       *  const Vec3f & v1 = vertices[strip[vertIndices[1]]];
       *  const Vec3f & v2 = vertices[strip[vertIndices[2]]];
       * 
       *  const Vec3f & n0 = normals[strip[vertIndices[0]]];
       *  const Vec3f & n1 = normals[strip[vertIndices[1]]];
       *  const Vec3f & n2 = normals[strip[vertIndices[2]]];
       *  \endcode
       */
      NVSG_API const std::vector<unsigned int> & getVertexIndices() const;

    protected:
      //! Clone the whole object.
      /** Use this, for example, for the copy constructor. */
      NVSG_API void clone( const Intersection & rhs    //!< Object to clone.
                         ); 
      
      nvsg::DrawableSharedPtr       m_pDrawable;      //!< Intersected drawable.
      nvutil::SmartPtr<nvsg::Path>  m_pPath;          //!< Path to the intersected drawable.
      nvmath::Vec3f                 m_isp;            //!< Intersection point on the intersected drawable.
      float                         m_dist;           //!< Distance from the ray origin to the intersection point.
      unsigned int                  m_primitiveIndex; //!< Index of the intersected primitive in the Drawable.
      std::vector<unsigned int>     m_vertexIndices;  //!< Indices of the vertices of the intersected primitive.

  };

  inline Intersection::~Intersection() 
  {
  }

  inline Intersection::Intersection()
  : m_pDrawable(NULL)
  , m_pPath(NULL)
  , m_isp(nvmath::Vec3f())
  , m_dist(0.0f)
  , m_primitiveIndex (0xffffffff)
  {
  }

   inline Intersection::Intersection ( nvsg::Path * pPath
                                     , const nvsg::DrawableSharedPtr & pDrawable
                                     , const nvmath::Vec3f & isp
                                     , float dist
                                     , unsigned int primitiveIndex
                                     , const std::vector<unsigned int> & vertexIndices)

  {
    NVSG_ASSERT(pDrawable);
    NVSG_ASSERT(pPath);

    m_isp       = isp; 
    m_dist      = dist;
    m_pDrawable = pDrawable;
    m_pPath     = pPath;
    m_primitiveIndex = primitiveIndex;
    m_vertexIndices = vertexIndices;
  }

  inline Intersection::Intersection (const Intersection & rhs)
  {
    clone(rhs);
  }

  inline Intersection & Intersection::operator = (const Intersection & rhs) 
  {
    clone(rhs);
    return (*this);
  }

  inline void Intersection::clone(const Intersection & rhs)
  {
    m_isp       = rhs.m_isp; 
    m_dist      = rhs.m_dist; 
    m_pDrawable = rhs.m_pDrawable; 
    m_pPath     = rhs.m_pPath; 
    m_primitiveIndex = rhs.m_primitiveIndex;
    m_vertexIndices = rhs.m_vertexIndices;
  }

  inline const nvmath::Vec3f & Intersection::getIsp() const
  {
    return m_isp;
  }

  inline float Intersection::getDist() const
  {
    return m_dist;
  }

  inline const nvsg::DrawableSharedPtr & Intersection::getDrawable() const
  {
    return m_pDrawable;
  }

  inline const nvsg::Path * Intersection::getPath() const
  {
    return m_pPath.get();
  }

  inline unsigned int Intersection::getPrimitiveIndex() const
  {
    return m_primitiveIndex;
  }
  
  inline const std::vector<unsigned int> & Intersection::getVertexIndices() const
  {
    return m_vertexIndices;
  }

  //! RayIntersectTraverser
  /** A \a RayIntersectTraverser is a specialized traverser that calculates 
    * the intersections of a given ray that passes through the scene.
    * It does \b NOT consider face culling modes.
    * \note If you add new geometry objects classes to SceniX you must also derive from this class and 
    *  add an operator for this object to handle the intersection calculation.
    * \note If you create a special kind of transformation node you also have to 
    *  take care of the transformation stack in this class. 
    * \note Needs a valid ViewState. Call setViewState prior to apply().*/
  class RayIntersectTraverser : public SharedModelViewTraverser
  {
    public:
      //! Default constructor.
      NVSG_API RayIntersectTraverser(void);

      //! Release all resources.
      /** By releasing all resources we can use this traverser for the 
        * next intersection calculation. */
      NVSG_API void release();

      //! Set the ray for the intersection test.
      /** Sets a world-space ray along which to pick. The ray is defined as a world space 
        * \a origin point and \a direction vector. The direction vector must be normalized. */
      NVSG_API void setRay( const nvmath::Vec3f &origin      //!< World space origin
                          , const nvmath::Vec3f &dir         //!< Direction of the ray - must be normalized!
                          );

      //! Enable/disable camera plane clipping. 
      /** Enabling camera plane clipping forces the traverser to calculate only 
        * intersections that lie in between the camera's clip planes.
          \note
          if the RayIntersectTraverser is applied before the scene is rendered
          and the camera has been changed since the latest rendering,
          then this mode may yield incorrect results because the clip
          planes wouldn't have gotten a chance to re-adapt to the new camera
          settings*/
      NVSG_API void setCamClipping( bool b    //!< true = enable clipping 
                                  );

      //! Get the nearest intersection.
      /** \returns The nearest intersection regarding the rays origin. */
      NVSG_API const Intersection & getNearest( void ) const;
      
      //! Get intersections.
      /** \returns All intersections along the ray. */
      NVSG_API const Intersection * getIntersections( void ) const;
      
      //! Get number of intersections.
      /** \returns The number of intersections along the ray. */
      NVSG_API unsigned int getNumberOfIntersections( void ) const;

      //! Set the viewport size for intersection calculations with lines and points
      /** \note Both width and height have to be positive. */
      NVSG_API void setViewportSize( unsigned int width, unsigned int height );

      
    protected:
      //! Default destructor.
      NVSG_API virtual ~RayIntersectTraverser(void);

      //! Apply the traverser to the scene.
      /** Start the intersection calculation of the given 
        * ray with the scene objects. */
      NVSG_API virtual void  doApply( const nvsg::NodeSharedPtr & root );

      // Nodes in the tree:
      NVSG_API virtual void handleGeoNode( const nvsg::GeoNode *gnode );

      // Groups in the tree:
      NVSG_API virtual void handleAnimatedTransform( const nvsg::AnimatedTransform *p );
      NVSG_API virtual void handleBillboard( const nvsg::Billboard * p);
      NVSG_API virtual void handleGroup( const nvsg::Group * group);
      NVSG_API virtual void handleLOD( const nvsg::LOD * lod);
      NVSG_API virtual void handleSwitch( const nvsg::Switch * swtch);
      NVSG_API virtual void handleTransform( const nvsg::Transform * p);

      // Other operators:
      NVSG_API virtual void handleLines( const nvsg::Lines * p );
      NVSG_API virtual void handleLineStrips( const nvsg::LineStrips * p );
      NVSG_API virtual void handlePoints( const nvsg::Points * p );
      NVSG_API virtual void handleQuadMeshes( const nvsg::QuadMeshes * p );
      NVSG_API virtual void handleQuadPatches( const nvsg::QuadPatches * p );
      NVSG_API virtual void handleQuadPatches4x4( const nvsg::QuadPatches4x4 * p );
      NVSG_API virtual void handleQuads( const nvsg::Quads * p );
      NVSG_API virtual void handleQuadStrips( const nvsg::QuadStrips * p );
      NVSG_API virtual void handleRectPatches( const nvsg::RectPatches * p );
      NVSG_API virtual void handleTriangles( const nvsg::Triangles *p );
      NVSG_API virtual void handleTriFans( const nvsg::TriFans * p );
      NVSG_API virtual void handleTriPatches( const nvsg::TriPatches * p );
      NVSG_API virtual void handleTriPatches4( const nvsg::TriPatches4 * p );
      NVSG_API virtual void handleTriStrips( const nvsg::TriStrips * p );

      NVSG_API virtual void handlePrimitive( const nvsg::Primitive * p );
      NVSG_API virtual void handlePoints( const nvsg::Primitive * p );
      NVSG_API virtual void checkAPoint( const nvsg::Primitive * p, const nvmath::Vec3f & v, unsigned int );
      NVSG_API virtual void handleLines( const nvsg::Primitive * p );
      NVSG_API virtual void handleLineStrip( const nvsg::Primitive * p );
      NVSG_API virtual void handleLineLoop( const nvsg::Primitive * p );
      NVSG_API virtual void handleTriangles( const nvsg::Primitive * p );
      NVSG_API virtual void handleTriangleStrip( const nvsg::Primitive * p );
      NVSG_API virtual void handleTriangleFan( const nvsg::Primitive * p );
      NVSG_API virtual void handleQuads( const nvsg::Primitive * p );
      NVSG_API virtual void handleQuadStrip( const nvsg::Primitive * p );

      NVSG_API virtual void handleLineAttribute( const nvsg::LineAttribute *lineAttrib );
      NVSG_API virtual void handlePointAttribute( const nvsg::PointAttribute *pointAttrib );
      NVSG_API virtual void handleStateSet( const nvsg::StateSet *stateSet );
      NVSG_API virtual void handleStateVariant( const nvsg::StateVariant *stateVariant );

      /*! \brief Store the active clip planes of \a p for later usage.
       *  \param p A pointer to the constant Group to get the active clip planes from.
       *  \return The result of ModelViewTraverser::preTraverseGroup().
       *  \sa postTraverseGroup */
      NVSG_API virtual bool preTraverseGroup( const nvsg::Group *p );

      /*! \brief Restore the set of active clip planes to the set before entering \a p.
       *  \param p A pointer to the constant Group just traversed.
       *  \remarks The clip planes stored in preTraverseGroup are removed.
       *  \sa preTraverseGroup */
      NVSG_API virtual void postTraverseGroup( const nvsg::Group *p );

      //! Handles actions to take between transform stack adjustment and traversal.
      /** Adjusts the ray origin and direction and the camera direction in model space.
        * \return result of the base class (ModelViewTraverser). */
      NVSG_API virtual bool preTraverseTransform( const nvmath::Trafo *p    //!< Trafo of node to traverse next 
                                                );

      //! Handles stuff to do between traversal and transform stack adjustment.
      /** Readjusts the ray origin and direction and the camera direction in model space. */
      NVSG_API virtual void postTraverseTransform( const nvmath::Trafo *p   //!< Trafo of node that was traversed immediately before this call. 
                                                 );

      /*! \brief Test a point against all active clip planes
       *  \param p A reference to a constant point to test against the clip planes.
       *  \return \c true if p is on the visible side of all active clip planes, otherwise \c false. */
      NVSG_API bool checkClipPlanes( const nvmath::Vec3f & p );

      /*! \brief Test a sphere against all active clip planes
       *  \param p A reference to a constant sphere to test agains the clip planes.
       *  \return \c true if p is at least partially on the visible side of all active clip planes,
       *  otherwise \c false. */
      NVSG_API bool checkClipPlanes( const nvmath::Sphere3f & p );

      //! %Intersection type (bounding sphere).
      /** These are the possible types of intersections between the ray 
        * and the boundingsphere of an object in the scene. These types 
        * are used for the preintersection calculation with the objects 
        * bounding sphere. Regarding the intersection type we can skip 
        * objects. */
      enum ISType { _NONE = 0       //!< No intersection
                  , _BSTANGENT      //!< Tangential intersection
                  , _BSNEAR         //!< Intersection with the front part of the bounding sphere
                  , _BSFAR          //!< Intersection with the back part of the bounding sphere
                  , _BSNEARFAR      //!< Intersection with the front and back part of the bounding sphere 
                  };

      //! Check the current ray for intersection with a \c Sphere3f.
      /** \return true, if the ray intersects \a sphere, otherwise false. */
      NVSG_API bool checkIntersection( const nvmath::Sphere3f &sphere );

      //! Get the intersection point in model coordinates.
      /** \returns the point of intersection in model coordinates */
      NVSG_API nvmath::Vec3f getModelIntersection( float dist  //!< Input: modelspace distance between the ray origin and the point of intersection
                                                 );     

      //! Get the intersection point in world coordinates.
      /** \returns the point of intersection in world coordinates */
      NVSG_API nvmath::Vec3f getWorldIntersection( const nvmath::Vec3f& misp  //!< Input: the point of intersection, in model coordinates
                                                 );

      //!Check if the intersection point is within the camera's clips planes.
      /** \return true: intersection is within the camera's clip planes, false: intersection is outside of the camera's clip planes */
      NVSG_API bool isClipped( const nvmath::Vec3f& isp   //!< Input: The point of intersection, in world coordinates
                             , const nvmath::Vec3f& misp  //!< Input: The point of intersection, in model coordinates
                             );

      //! Intersect with a box.
      /** This methods tells if it intersects a box or not.
        * It also calculates the intersection point between a box and a ray and the distance of
        * the intersection from the origin of the ray.
        * \returns true: intersection in front of the ray origin, false: no such intersection */
      NVSG_API bool intersectBox( const nvmath::Vec3f &p0   //!< lower left front point of box
                                , const nvmath::Vec3f &p1   //!< upper right back point of box
                                , nvmath::Vec3f &isp        //!< Result: intesection point (world space)
                                , float &dist               //!< Result: distance form camera to intersection
                                );

      //! Intersect with line.
      /** This method tells if it intersects the line or not. 
        * It also calculates the intersection point between line and ray and the distance 
        * of the intersection from the origin of the ray.
        * \returns true: intersection in front of the ray origin, false: no such intersection */
      NVSG_API bool intersectLine( const nvmath::Vec3f & v0  //!< First vertex of the line
                                 , const nvmath::Vec3f & v1  //!< Second vertex of the line
                                 , nvmath::Vec3f & isp       //!< Result: intersection point (world space)
                                 , float & dist              //!< Result: distance from camera to intersection
                                 );    

      //! Intersect with point.
      /** This method tells if it intersects the point or not. 
        * It also calculates the intersection point between point and ray and the distance 
        * of the intersection from the origin of the ray.
        * \returns true: intersection in front of the ray origin, false: no such intersection */
      NVSG_API bool intersectPoint( const nvmath::Vec3f & v0  //!< vertex of the point
                                  , nvmath::Vec3f & isp       //!< Result: intersection point (world space)
                                  , float & dist              //!< Result: distance from camera to intersection
                                  );    

      //! Intersect with triangle.
      /** This method tells if it intersects the triangle or not. 
        * It also calculates the intersection point between triangle and ray and the distance 
        * of the intersection from the origin of the ray.
        * \returns true: intersection in front of the ray origin, false: no such intersection */
      NVSG_API bool intersectTriangle( const nvmath::Vec3f & v0  //!< First vertex of the triangle
                                     , const nvmath::Vec3f & v1  //!< Second vertex of the triangle
                                     , const nvmath::Vec3f & v2  //!< Third vertex of the triangle
                                     , nvmath::Vec3f & isp       //!< Result: intersection point (world space)
                                     , float & dist              //!< Result: distance from camera to intersection
                                     );    

      //! Store the intersection point \a isp.
      /** If the point isn't clipped, it is stored to the intersection list.  */
      NVSG_API void storeIntersection( const nvsg::Drawable *p       //!< Drawable that holds the intersection
                                     , const nvmath::Vec3f &isp      //!< Intersection point
                                     , float dist                    //!< Distance from camera to intersection
                                     , unsigned int primitiveIndex   //!< Index of the intersected primitive
                                     , const std::vector<unsigned int> & vertexIndices //!< Indices of the vertices of the intersected primitive
                                     );

#if !defined(NDEBUG)
      /** Dump intersection map - for Debug*/
      NVSG_API void dumpIntersectionList();
#endif

    private:
      void checkLine( const nvsg::Drawable * p, const nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type &vertices
                    , unsigned int i0, unsigned int i1, unsigned int pi );
      void checkQuad( const nvsg::Drawable * p, const nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type &vertices
                    , unsigned int i0, unsigned int i1, unsigned int i2, unsigned int i3, unsigned int pi );
      void checkRectPatches( const nvsg::PatchesBase * p, unsigned int width, unsigned int height );
      void checkTriangle( const nvsg::Drawable * p, const nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type &vertices
                        , unsigned int i0, unsigned int i1, unsigned int i2, unsigned int pi );
      void checkTriPatches( const nvsg::PatchesBase* vash, unsigned int size );
      bool equal( const nvmath::Vec3f & v0, const nvmath::Vec3f & v1, float width ) const;

      bool continueTraversal(unsigned int hints, const nvmath::Sphere3f& bs);
       
      static const nvmath::Vec3f _RAY_ORIGIN_DEFAULT;    //!< Default value: (0.f, 0.f,  0.f)
      static const nvmath::Vec3f _RAY_DIRECTION_DEFAULT; //!< Default value: (0.f, 0.f, -1.f)

      bool                          m_camClipping;      //!< true: use camera far/near clipping planes

      std::stack<std::vector<nvsg::ClipPlaneSharedPtr> > m_clipPlanes;   //!< vector of active clip planes
      nvutil::SmartPtr<nvsg::Path>  m_curPath;          //!< Current path.
      std::vector<Intersection>     m_intersectionList; //!< Intersection list containing all intersections.
      float                         m_lineWidth;        //!< current line width
      float                         m_lineWidthDefault; //!< default line width
      std::stack<nvmath::Vec3f>     m_msRayOrigin;      //!< stack of Origin point of the ray in model space
      std::stack<nvmath::Vec3f>     m_msRayDir;         //!< stack of Direction of the ray in model space
      std::stack<nvmath::Vec3f>     m_msCamDir;         //!< stack of camera direction in model space
      unsigned int                  m_nearestIntIdx;    //!< Index of the nearest z intersection.
      float                         m_pointSize;        //!< current point size
      float                         m_pointSizeDefault; //!< default point size
      nvmath::Vec3f                 m_rayOrigin;        //!< Origin point of the ray - default (0,0,0)
      nvmath::Vec3f                 m_rayDir;           //!< Direction of the ray - default (0,0,-1)
      std::stack<float>             m_scaleFactors;
      unsigned int                  m_viewportHeight;
      unsigned int                  m_viewportWidth;

      std::vector<unsigned int>     m_currentHints;
  };

  inline void RayIntersectTraverser::setCamClipping(bool flag)
  {
    m_camClipping = flag;
  }

  inline unsigned int RayIntersectTraverser::getNumberOfIntersections() const
  {
    return( checked_cast<unsigned int>(m_intersectionList.size()) );
  }

  inline const Intersection & RayIntersectTraverser::getNearest() const
  {
    NVSG_ASSERT(!m_intersectionList.empty());
    return m_intersectionList[m_nearestIntIdx];
  }
   
  inline const Intersection * RayIntersectTraverser::getIntersections() const
  {
    NVSG_ASSERT(!m_intersectionList.empty());
    return &m_intersectionList[0];
  }
}

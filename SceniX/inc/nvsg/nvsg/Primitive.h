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
#include "nvsg/Drawable.h" // base class definition
#include "nvsg/IndexSet.h"

namespace nvsg
{
  /*! \brief PrimitiveTypes associated with this Primitive */
  enum PrimitiveType
  {
    // default Primitive types
    PRIMITIVE_POINTS,
    PRIMITIVE_LINE_STRIP,
    PRIMITIVE_LINE_LOOP,
    PRIMITIVE_LINES,
    PRIMITIVE_TRIANGLE_STRIP,
    PRIMITIVE_TRIANGLE_FAN,
    PRIMITIVE_TRIANGLES,
    PRIMITIVE_QUAD_STRIP,
    PRIMITIVE_QUADS,
    PRIMITIVE_POLYGON,

    //  If NV_geometry_program4 is supported, then the following primitive types are also supported:
    PRIMITIVE_TRIANGLES_ADJACENCY,
    PRIMITIVE_TRIANGLE_STRIP_ADJACENCY,
    PRIMITIVE_LINES_ADJACENCY,
    PRIMITIVE_LINE_STRIP_ADJACENCY,

    // if NV_tessellation_program5 / ARB_tessellation_shader is supported
    PRIMITIVE_PATCHES,

    PRIMITIVE_UNINITIALIZED = ~0
  };

  bool needsPrimitiveRestart( PrimitiveType pt );

  /*! \brief Class for Drawable objects holding a VertexAttributeSet.
   *  \sa Drawable, VertexAttributeSet, IndexSet */
  class Primitive : public Drawable
  {
    public:
      NVSG_API static PrimitiveSharedPtr create();

    public:
      // internal render flags used to know how to draw this primitive
      enum
      {
        DRAW_ARRAYS     = 0,      // here to make code more readable
        DRAW_INDEXED    = BIT(0),
        DRAW_INSTANCED  = BIT(1),
      };

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant Primitive to copy from.
       *  \return A reference to the assigned Primitive.
       */
      NVSG_API Primitive & operator=( const Primitive & rhs );

      /*! \brief Set the primitive type
       *  \param pt The primitive type to use to render this primitive.
       *  \remarks If the selected primitive type is not available on the target hardware, nothing will be drawn for this primitive.
       *  \sa PrimitiveType
       */
      NVSG_API void setPrimitiveType( PrimitiveType t );
      NVSG_API PrimitiveType getPrimitiveType() const;

      /*! \brief Get the Skin of this Primitive.
       *  \return The Skin of this Primitive.
       *  \remarks If a Primitive holds a Skin, the associated VertexAttributeSet is skinned
       *  using that Skin object.
       *  \sa setSkin, getVertexAttributeSet */
      NVSG_API const SkinSharedPtr & getSkin() const;

      /*! \brief Set the Skin of this Primitive.
       *  \param skin The Skin to set.
       *  \remarks If a Primitive holds a Skin, the associated VertexAttributeSet is skinned
       *  using that Skin object.
       *  \sa getSkin, getVertexAttributeSet */
      NVSG_API void setSkin( const SkinSharedPtr & skin );

      /*! \brief Set the range of elements to use.
       * If an IndexSet is present the range applies to that, if not, it applies to the VertexAttributeSet directly.
       * \param offset The starting index or vertex to use for this primitive. 
       * The default value is 0, meaning the traverser should begin sourcing indices or vertices from the start 
       * of the IndexSet or VertexAttributeSet.
       * \param count The count of indices or vertices to use for this primitive.
       * The default value of ~0 describes the end of the IndexSet or VertexAttributeSet. 
       * It will cause all elements from offset to the end of the IndexSet or VertexAttributeSet to be used.
       * Results are undefined if offset is greater than the size of the IndexSet or VertexArray.
       * Results are undefined if the count is not equal to ~0 and offset + count is greater than the IndexSet or VertexAttributeSet size.
       * \remarks Sets the number of elements to use for this primitive. This is the number of indices or vertices, NOT the
       * number of primitives. 
       * \note This method must be called \b after setIndexSet() or setVertexAttributeSet() for proper validation.
       * The defaults match the call setElementRange(0, ~0) which will render the whole Primitive.
       * With these defaults it is not required to update the range after the IndexSet or VertexAttributeSet size has been changed 
       * if the whole Primitive should be rendered.
       * \sa getElementRange, getElementOffset, getElementCount, getMaxElementCount
       */
      NVSG_API void setElementRange( unsigned int offset, unsigned int count );

      /*! \brief Get the element range values offset and count exactly as specified by the user via setElementRange.
       * If setElementRange hasn't been used the returned values will be offset = 0 and count = ~0 which describes 
       * the whole IndexSet or VertexAttributeSet.
       * \param offset Returns the user defined offset value for the starting index or vertex.
       * \param count Returns the user defined count value for indices or vertices.
       * This can be ~0 which describes the IndexSet or VertexAttributeSet end.
       * \remarks The range describes the start and count of indices or vertices to use, \b NOT the number of primitives.
       * setElementRange and getElementRange are symmetric to be able to use them in succession while the special count parameter ~0 is used.
       * To get the effective number of elements use getElementCount. It will return the user count if it is not ~0 and
       * the IndexSet or VertexAttributeSet size minus offset when the user count is ~0.
       * \sa setElementRange, getElementOffset, getElementCount, getMaxElementCount
       */
      NVSG_API void getElementRange( unsigned int & offset, unsigned int & count ) const;

      /*! \brief Get the user defined element offset value.
       * Returns the user defined offset value for the starting index or vertex.
       * If setElementRange hasn't been used the returned value will be 0 describing the start of the IndexSet or VertexAttributeSet.
       * \remarks The offset value describes the start of indices or vertices to use, \b NOT the number of primitives.
       * \sa setElementRange, getElementRange, getElementCount, getElementCount
       */
      NVSG_API unsigned int getElementOffset() const;

      /*! \brief Get the \b effective element count.
       * Returns the effective count value for indices or vertices.
       * \remarks The count describes the number of indices or vertices, \b NOT the number of primitives.
       * With the default element count value of ~0 this function returns the IndexSet or VertexAttribute size minus the element offset.
       * This is in contrast to the getElementRange function which returns the user defined range values which can have count at ~0.
       * There is no setElementCount defined on purpose because that wouldn't be symmetric to this getElementCount function.
       * \sa setElementRange, getElementRange, getElementOffset, getMaxElementCount
       */
      NVSG_API unsigned int getElementCount() const;

      /*! \brief Get the maximum allowed element count of the IndexSet or VertexAttributeSet.
       * This is a convenience function which allows to query the IndexSet or VertexAttributeSet size 
       * without considering the currently active user defined element range offset or count values.
       * Only with the default setElementRange(0, ~0) setting the result will match the one from getElementCount.
       * \sa setElementRange, getElementRange, getElementOffset, getElementCount
       */
      NVSG_API unsigned int getMaxElementCount() const;

      /*! \brief Set the instance count for this primitive.
       * \param icount The number of times to instance (render) this primitive.  Typically the shader used to render an instanced primitive 
       * must be written to be instance-aware, and separate data must be packed into vertex attributes in order to appropriately render each 
       * instance.  The value must be greater than or equal to 1.
       * \remarks If icount is greater than 1, and OpenGL 3.0 or ARB_draw_instanced is not available, nothing will be rendered for this 
       * primitive since a compatible software fallback is not possible.
       *  \note The default instance count is 1, rendering exactly 1 primitive.
       */
      NVSG_API void setInstanceCount( unsigned int icount );
      NVSG_API unsigned int getInstanceCount() const;

      /*! \brief Set the number of vertices per patch.
       *  \param vpp The number of vertices per patch.
       *  \remarks If the \c PrimitiveType for this Primitive is \b PRIMITIVE_PATCHES then each consecutive set of \a verticesPerPatch indices 
       *  are treated as one patch.
       *  \note This value MUST be set if rendering a patch primitive.  If not rendering a patch primitive, it is ignored.
       *  \sa getNumberOfVerticesPerPatch */
      NVSG_API void setNumberOfVerticesPerPatch( unsigned int vpp );
      NVSG_API unsigned int getNumberOfVerticesPerPatch() const;

      /*! \brief Set the VertexAttributeSet.
       *  \param vash The VertexAttributeSet to set.
       *  \sa VertexAttributeSet */
      NVSG_API void setVertexAttributeSet( const VertexAttributeSetSharedPtr & vash );

      /*! \brief Get The VertexAttributeSet of this Primitive.
       *  \return The VertexAttributeSet of this Primitive.
       *  \sa VertexAttributeSet */
      NVSG_API const VertexAttributeSetSharedPtr & getVertexAttributeSet() const;

      /*! \brief Get the IndexSet 
       *  \return The IndexSet.
       *  \note If a Primitive does not contain an IndexSet then it is considered to be a non-indexed array of primitives 
       *  that will be drawn in linear order as specified in the VAS.
       *  \sa IndexSet */
      NVSG_API const IndexSetSharedPtr & getIndexSet() const;

      /*! \brief Set the IndexSet.
       *  \param iset A pointer to the IndexSet.
       *  \sa IndexSet, getIndexSet */
      NVSG_API void setIndexSet( const IndexSetSharedPtr & iset );

      /*! \brief Check whether this Primitive is indexed or not.
       *  \return \c true if the Primitive \a p is indexed (contains a IndexSet), otherwise \c false.
       *  \sa IndexSet, setIndexSet */
      NVSG_API bool isIndexed() const;

      /*! \brief Make this Primitive indexed.
       *  \return \c true if the Primitive was not indexed before, otherwise \c false.
       *  \remark If this Primitive is not indexed, an IndexSet is created, indexing the vertices
       *  from offset to offset+count. Otherwise nothing is done.
       *  \sa isIndexed, getIndexSet, setIndexSet, getElementOffset, getElementCount */
      NVSG_API bool makeIndexed();

      /*! \brief Test for equivalence with an other Primitive.
       *  \param p A reference to the constant Primitive to test for equivalence with.
       *  \param ignoreNames Optional parameter to ignore the names of the objects; default is \c
       *  true.
       *  \param deepCompare Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the Primitive \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as Drawable, they are equivalent if the
       *  VertexAttributeSet objects are equivalent. If \a deepCompare is \c true, a full
       *  equivalence test is performed on the VertexAttributeSet objects, otherwise they are
       *  considered to be equivalent if the pointers are equal.
       *  \note The behavior is undefined if \a p is not a Primitive nor derived from one.
       *  \sa Drawable, VertexAttributeSet */
      NVSG_API virtual bool isEquivalent( const Object * p
                                        , bool ignoreNames
                                        , bool deepCompare ) const;

      /*! \brief Get the renderer specific render flags.
       *  \remarks This function is used by the renderer to accelerate rendering of this primitive
       *  \return The render flags
       */
      NVSG_API unsigned int getRenderFlags() const;

      /*! \brief Get the number of geometric primitives in this Primitive.
       *  \remarks This function returns the number of geometric primitives in this primitive.  This count varies
       *  depending on the primitive type, whether the primitive is indexed, is stripped, and whether the IndexSet
       *  contains Primitive Restart indices or not.  If the primitive is "singular" (*_STRIP, TRIANGLE_FAN, etc) the 
       *  count of that type (not the face count) is returned.  If the primitive is "plural" (TRIANGLES, QUADS, etc) the 
       *  face count is returned.
       *  \return The primitive count.
       *  \sa getNumberOfFaces, getNumberOfVerticesPerPrimitive
       */
      NVSG_API unsigned int getNumberOfPrimitives() const;

      /*! \brief Get the number of faces in this primitive.
       *  \remarks This function returns the number of faces in this primitive.  The face count varies
       *  depending on the primitive type, whether the primitive is indexed, is stripped, and whether the IndexSet
       *  contains Primitive Restart indices or not.  If the primitive is "singular" (*_STRIP, TRIANGLE_FAN, etc) the 
       *  primitive will be processed to determine the actual number of faces in the strip or fan, etc.  If the primitive 
       *  is "plural" (TRIANGLES, QUADS, etc) the face count equals the number of primitives.
       *  \return The face count.
       *  \sa getNumberOfVerticesPerPrimitive, getNumberOfPrimitives
       */
      NVSG_API unsigned int getNumberOfFaces() const;

      /*! \brief Get the number of vertices per Primitive
       *  \remarks This function returns the number of vertices required to specify a complete Primitive.  For instance
       *  it would be 3 for PRIMITIVE_TRIANGLES, 4 for PRIMITIVE_QUADS, etc.  For types with a variable number of 
       *  vertices per primitive (PRIMITIVE_POLYGON, PRIMITIVE_TRIANGLE_STRIP, etc) the return value is zero.
       *  \return The vertex count, 0, or ~0 if the primitive is uninitialized.
       *  \sa getNumberOfFaces, getNumberOfPrimitives, getNumberOfPrimitiveRestarts
       */
      NVSG_API unsigned int getNumberOfVerticesPerPrimitive() const;

      NVSG_API virtual void notify( const Subject * originator, unsigned int state ) const;

      REFLECTION_INFO_API( NVSG_API, Primitive );
    protected:
      friend struct nvutil::Holder<Primitive>;

      /*! \brief default constructor 
       */
      NVSG_API Primitive();

      /*! \brief Copy constructor
       *  \param rhs A reference to the constant Primitive to copy from
       *  \remarks The new Primitive holds a copy of the VertexAttributeSet and IndexSet (if any) of \a rhs. */
      NVSG_API Primitive( const Primitive& rhs );

      /*! \brief Destructor
       */
      NVSG_API virtual ~Primitive();

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;
      NVSG_API virtual bool determineAnimationContainment() const;

      NVSG_API void determinePrimitiveAndFaceCount() const;

      /*! \brief Get the number of primitive restarts in the IndexSet.
       *  \remarks This function returns the number of primitive restarts found in the IndexSet.
       *  \return The number of primitive restarts found in the IndexSet, which may be zero if there is no index set or 
       *  no primitive restarts therein.
       *  \sa getNumberofFaces, getNumberOfElements, getNumberOfVerticesPerPrimitive
       */
      NVSG_API unsigned int getNumberOfPrimitiveRestarts() const;

      /*! \brief Override to specialize calculation of texture coords */
      NVSG_API virtual void calculateTexCoords(TextureCoordType type, unsigned int texcoords, bool overwrite);

      /*! \brief Calculates the bounding box of this Primitive.
       *  \return The axis-aligned bounding box of this Primitive.
       *  \remarks This function is called by the framework when re-calculation
       *  of the bounding box is required for this Primitive. */
      NVSG_API virtual nvmath::Box3f calculateBoundingBox() const;

      /*! \brief Calculate the bounding sphere of this Primitive.
       *  \return A nvmath::Sphere3f that contains the complete Primitive.
       *  \remarks This function is called by the framework to determine a sphere that completely
       *  contains the Primitive. */
      NVSG_API virtual nvmath::Sphere3f calculateBoundingSphere() const;

      /*! \brief Override to specialize normals calculation for Primitives
       *  \param overwrite A flag indicating whether to overwrite existing vertex normals.
       *  \return \c true, if normals could be calculated, otherwise \c false.
       *  \remarks This function gets called from the generateNormals API. If \a overwrite is \c true or the
       *  Primitive has no normals, and the Primitive is of type PRIMITIVE_TRIANGLE_STRIP, PRIMITIVE_TRIANGLE_FAN,
       *  PRIMITIVE_TRIANGLES, PRIMITIVE_QUAD_STRIP, or PRIMITIVE_QUADS, normals are calculated and \c true is
       *  returned. Otherwise \c false is returned.
       *  \sa generateNormals */
      NVSG_API virtual bool calculateNormals(bool overwrite);

      /*! \brief Override to specialize tangent space calculation for Primitives
       *  \param texcoords Addresses the vertex attribute to hold the input 2D texture coordinates used to calculate
       *  the tangent space.
       *  \param tangents Addresses the vertex attribute where to output the calculated tangents.
       *  \param binormals Addresses the vertex attribute where to output the calculated binormals.
       *  \param overwrite A flag indicating whether to overwrite existing vertex data.
       *  \remarks This function gets called from the generateTangentSpace API. If \a overwrite is \c true or the
       *  Primitive has no normals, and the Primitive is of type PRIMITIVE_TRIANGLE_STRIP, PRIMITIVE_TRIANGLE_FAN,
       *  PRIMITIVE_TRIANGLES, PRIMITIVE_QUAD_STRIP, or PRIMITIVE_QUADS, tangents and binormals are calculated.
       * \sa generateTangentSpace */
      NVSG_API virtual void calculateTangentSpace(unsigned int texcoords, unsigned int tangents, unsigned int binormals, bool overwrite);

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      bool calculateNormals( const nvsg::VertexAttributeSetSharedPtr& vassp, bool overwrite );
      void calculateNormalsPolygon( nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type & vertices
                                  , std::vector<nvmath::Vec3f> & normals );
      void calculateNormalsQuad( nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type & vertices
                               , std::vector<nvmath::Vec3f> & normals );
      void calculateNormalsQuadStrip( nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type & vertices
                                    , std::vector<nvmath::Vec3f> & normals );
      void calculateNormalsTriangle( nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type & vertices
                                   , std::vector<nvmath::Vec3f> & normals );
      void calculateNormalsTriFan( nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type & vertices
                                 , std::vector<nvmath::Vec3f> & normals );
      void calculateNormalsTriStrip( nvsg::Buffer::ConstIterator<nvmath::Vec3f>::Type & vertices
                                   , std::vector<nvmath::Vec3f> & normals );
      void calculateTangentsQuad( nvsg::VertexAttributeSet * vas, unsigned int tc
                                , std::vector<nvmath::Vec3f> & tangents );
      void calculateTangentsQuadStrip( nvsg::VertexAttributeSet * vas, unsigned int tc
                                     , std::vector<nvmath::Vec3f> & tangents );
      void calculateTangentsTriangle( nvsg::VertexAttributeSet * vas, unsigned int tc
                                    , std::vector<nvmath::Vec3f> & tangents );
      void calculateTangentsTriFan( nvsg::VertexAttributeSet * vas, unsigned int tc
                                  , std::vector<nvmath::Vec3f> & tangents );
      void calculateTangentsTriStrip( nvsg::VertexAttributeSet * vas, unsigned int tc
                                    , std::vector<nvmath::Vec3f> & tangents );

      void clearCachedCounts() const;

    private:
      PrimitiveType               m_primitiveType;
      VertexAttributeSetSharedPtr m_vertexAttributeSet;
      SkinSharedPtr               m_skin;
      IndexSetSharedPtr           m_indexSet;
      unsigned int                m_elementOffset;
      unsigned int                m_elementCount;
      unsigned int                m_instanceCount;
      unsigned int                m_verticesPerPatch;
      unsigned int                m_renderFlags;
      mutable unsigned int        m_cachedNumberOfPrimitives;
      mutable unsigned int        m_cachedNumberOfFaces;
      mutable unsigned int        m_cachedNumberOfPrimitiveRestarts;
  };

  inline bool needsPrimitiveRestart( PrimitiveType pt )
  {
    return(   ( pt == PRIMITIVE_LINE_STRIP )
          ||  ( pt == PRIMITIVE_LINE_LOOP )
          ||  ( pt == PRIMITIVE_TRIANGLE_STRIP )
          ||  ( pt == PRIMITIVE_TRIANGLE_FAN )
          ||  ( pt == PRIMITIVE_QUAD_STRIP )
          ||  ( pt == PRIMITIVE_POLYGON )
          ||  ( pt == PRIMITIVE_TRIANGLE_STRIP_ADJACENCY )
          ||  ( pt == PRIMITIVE_LINE_STRIP_ADJACENCY ) );
  }

  inline void Primitive::clearCachedCounts() const
  {
    m_cachedNumberOfPrimitives = ~0;
    m_cachedNumberOfFaces = ~0;
    m_cachedNumberOfPrimitiveRestarts = ~0;
  }

  inline const VertexAttributeSetSharedPtr & Primitive::getVertexAttributeSet() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_vertexAttributeSet );
  }

  inline bool Primitive::isIndexed() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return !!m_indexSet;
  }

  inline const IndexSetSharedPtr & Primitive::getIndexSet() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_indexSet;
  }

  inline PrimitiveType Primitive::getPrimitiveType() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_primitiveType;
  }

  inline unsigned int Primitive::getInstanceCount() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_instanceCount;
  }

  inline unsigned int Primitive::getNumberOfVerticesPerPatch() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_verticesPerPatch;
  }

  inline unsigned int Primitive::getNumberOfVerticesPerPrimitive() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    switch( getPrimitiveType() )
    {
      case PRIMITIVE_LINE_STRIP:
      case PRIMITIVE_LINE_LOOP:
      case PRIMITIVE_TRIANGLE_STRIP:
      case PRIMITIVE_TRIANGLE_FAN:
      case PRIMITIVE_QUAD_STRIP:
      case PRIMITIVE_POLYGON:
      case PRIMITIVE_TRIANGLE_STRIP_ADJACENCY:
      case PRIMITIVE_LINE_STRIP_ADJACENCY:
        return 0; // we can't tell

      case PRIMITIVE_POINTS:
        return 1;

      case PRIMITIVE_LINES:
        return 2;

      case PRIMITIVE_TRIANGLES:
        return 3;

      case PRIMITIVE_QUADS:
      case PRIMITIVE_LINES_ADJACENCY:
        return 4;

      case PRIMITIVE_TRIANGLES_ADJACENCY:
        return 6;

      case PRIMITIVE_PATCHES:
        return getNumberOfVerticesPerPatch();

      default:
      case PRIMITIVE_UNINITIALIZED:
        return ~0; // we can't tell
    }
  }

  inline unsigned int Primitive::getRenderFlags() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    
    return m_renderFlags;
  }

  inline unsigned int Primitive::getNumberOfPrimitives() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    // MMM: TODO: add support here for observer to understand if VAS or IS changes
    if( m_cachedNumberOfPrimitives == ~0 )
    {
      // determine face and element count
      determinePrimitiveAndFaceCount();
    }

    return m_cachedNumberOfPrimitives;
  }

  inline unsigned int Primitive::getNumberOfFaces() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    // MMM: TODO: add support here for observer to understand if VAS or IS changes
    if( m_cachedNumberOfFaces == ~0 )
    {
      // determine face and element count
      determinePrimitiveAndFaceCount();
    }

    return m_cachedNumberOfFaces;
  }

  inline void Primitive::getElementRange( unsigned int & offset, unsigned int & count ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);

    offset = m_elementOffset;
    count  = m_elementCount;  // Note that this can be ~0. Use getElementCount() to query the effective count.
  }

  inline unsigned int Primitive::getElementOffset() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return m_elementOffset;
  }

  inline const SkinSharedPtr & Primitive::getSkin() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_skin );
  }

  inline void Primitive::notify( const Subject * originator, unsigned int state ) const
  {
    clearCachedCounts();
    Drawable::notify( originator, state );
  }

} //  namespace nvsg


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

#include "nvsg/VertexAttributeSet.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Provides functionality to work with animated vertex attribute data
   *  \remarks An AnimatedVertexAttributeSet holds a set of pointers to
   *  VertexAttributeAnimation, one for each animated vertex attribute. The values for a
   *  specified animation frame are copied into corresponding vertex attribute data of the base
   *  class. Vertex attributes without an animation are not changed there. That way, for example
   *  vertices and normals of a VertexAttributeSet can be animated while any texture coordinates are
   *  kept constant.
   *  \sa VertexAttributeData, VertexAttribute, VertexAttributeAnimation */
  class AnimatedVertexAttributeSet : public VertexAttributeSet
  {
    public:
      NVSG_API static AnimatedVertexAttributeSetSharedPtr create();

    public:
      /*! \brief Assignment operator.
       *  \param rhs AnimatedVertexAttributeSet to assign.
       *  \return A reference to \a this.
       *  \remarks Any animations in \a rhs are referenced once.
       *  \sa Animation */
      NVSG_API AnimatedVertexAttributeSet & operator=( const AnimatedVertexAttributeSet & rhs );

      /*! \brief Test for equivalence with an other AnimatedVertexAttributeSet.
       *  \param p A pointer to an Object to test for equivalence with.
       *  \param ignoreNames  \c true, if names are to be ignored in the equivalence test, otherwise \c false.
       *  \param deepCompare  \c true, if a deep compare should be done, otherwise \c false.
       *  \return \c true, if \a this and \a p are equivalent, otherwise \c false.
       *  \remarks An Object \a p is considered to be equivalent to the AnimatedVertexAttributeSet
       *  \a this, if it also is an AnimatedVertexAttributeSet, is equivalent as a
       *  VertexAttributeSet, have the same normalize flags, and, if \a deepCompare is \c true, have
       *  animations on the same vertex attributes and those are pairwise equivalent, or, if \a
       *  deepCompare is \c false, have the exact same animations on the vertex attributes.
       *  \sa Animation, VertexAttribute */
      NVSG_API virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Overwrite of setVertexData in VertexAttributeSet.
       *  \param attrib Index to identify the vertex attribute. NVSG_POSITION, NVSG_NORMAL,
       *  NVSG_COLOR, NVSG_SECONDARY_COLOR, NVSG_FOG_COORD, NVSG_TEXCOORD0 - NVSG_TEXCOORD7, and
       *  NVSG_ATTR0 - NVSG_ATTR15 are allowed indices.
       *  \param size Specifies the number of coordinates per vertex; must be 1, 2, 3, or 4.
       *  \param type Specifies the data type of each coordinate in the input data array. 
       *  In general, sympbolic constants NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT,
       *  NVSG_UNSIGNED_SHORT, NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted.
       *  \param data Specifies the start address of the input data array.
       *  \param strideInBytes Specifies the stride between two elements in data. A stride of 0 assumes packed data.
       *  \param count Specifies the number of vertex data to copy.
       *  \remarks If there's an animation defined for the specified attribute, the data is also set
       *  into the current step of the animation.
       *  \sa VertexAttributeSet, removeVertexData */
      NVSG_API virtual void setVertexData( unsigned int attrib, unsigned int size, unsigned int type
                                         , const void * data, unsigned int strideInBytes, unsigned int count );

      /*! \brief Overwrite of setVertexData in VertexAttributeSet.
       *  \param attrib Index to identify the vertex attribute. NVSG_POSITION, NVSG_NORMAL,
       *  NVSG_COLOR, NVSG_SECONDARY_COLOR, NVSG_FOG_COORD, NVSG_TEXCOORD0 - NVSG_TEXCOORD7, and
       *  NVSG_ATTR0 - NVSG_ATTR15 are allowed indices.
       *  \param pos Marks the position of the first vertex inside the already specified array of
       *  vertex data, where overwriting should start. If the magic value ~0 is specified, the input
       *  data will be appended to the array of vertex data.
       *  \param size Specifies the number of coordinates per vertex; must be 1, 2, 3, or 4.
       *  \param type Specifies the data type of each coordinate in the input data array. 
       *  In general, sympbolic constants NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT,
       *  NVSG_UNSIGNED_SHORT, NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted.
       *  \param data Specifies the start address of the input data array.
       *  \param strideInBytes Specifies the stride between two elements in data. A stride of 0 assumes packed data.
       *  \param count Specifies the number of vertex data to copy.
       *  \remarks If there's an animation defined for the specified attribute, the data is also set
       *  into the current step of the animation.
       *  \sa VertexAttributeSet, removeVertexData */
      NVSG_API virtual void setVertexData( unsigned int attrib, unsigned int pos, unsigned int size
                                         , unsigned int type, const void * data, unsigned int strideInBytes, unsigned int count );

      /*! \brief Overwrite of setVertexData in VertexAttributeSet.
       *  \param attrib Index to identify the vertex attribute. NVSG_POSITION, NVSG_NORMAL,
       *  NVSG_COLOR, NVSG_SECONDARY_COLOR, NVSG_FOG_COORD, NVSG_TEXCOORD0 - NVSG_TEXCOORD7, and
       *  NVSG_ATTR0 - NVSG_ATTR15 are allowed indices.
       *  \param to Specifies the start address of the index array, that will be taken to reference
       *  the respective vertex data to be re-specified. If NULL is passed here, the input vertex
       *  data will be appended to the array of vertex data that previously was specified.
       *  \param from Specifies the start address of the index array, that will be taken to
       *  reference the input vertex data to overwrite the previously specified vertex data.
       *  \param size Specifies the number of coordinates per vertex; must be 1, 2, 3, or 4.
       *  \param type Specifies the data type of each coordinate in the input data array. 
       *  In general, sympbolic constants NVSG_BYTE, NVSG_UNSIGNED_BYTE, NVSG_SHORT,
       *  NVSG_UNSIGNED_SHORT, NVSG_INT, NVSG_UNSIGNED_INT, NVSG_FLOAT, and NVSG_DOUBLE are accepted.
       *  \param data Specifies the start address of the input data array.
       *  \param strideInBytes Specifies the stride between two elements in data. A stride of 0 assumes packed data.
       *  \param count Specifies the number of vertex data to copy.
       *  \remarks If there's an animation defined for the specified attribute, the data is also set
       *  into the current step of the animation.
       *  \sa VertexAttributeSet, removeVertexData */
      NVSG_API virtual void setVertexData( unsigned int attrib, const unsigned int * to
                                         , const unsigned int * from, unsigned int size
                                         , unsigned int type, const void * data, unsigned int strideInBytes, unsigned int count );

      /*! \brief Overwrite of removeVertexData in VertexAttributeSet.
       *  \param attrib Specifies the vertex attribute for which to remove previously copied vertex
       *  data.
       *  \remarks If there's an animation defined for the specified attribute, the data of the
       *  current step is removed.
       *  \sa VertexAttributeSet, setVertexData */
      NVSG_API virtual void removeVertexData( unsigned int attrib );

      /*! \brief Overwrite of getVertexData in VertexAttributeSet.
       *  \param attrib Indicates the vertex attribute to query.
       *  \return The function returns a type-less pointer to the vertex data that was assigned to 
       *  the vertex attribute indicated by \a attrib prior to this call. If for this vertex
       *  attribute no vertex data was specified before, the function returns NULL. 
       *  \remarks If there's an animation defined for the specified attribute, the VertexAttribute
       *  data from the current animation step is returned.
       *  \sa getSizeOfVertexData, getTypeOfVertexData, getNumberOfVertexData */
      NVSG_API virtual Buffer::DataReadLock getVertexData( unsigned int attrib ) const;

      /*! \brief Retrieve the nvsg::BufferSharedPtr corresponding to the given attribute at the current animation value
       * \param attrib Specifies the vertex attribute from which to get the vertex data.
       * \return The Buffer for the given attribute. It is invalid if no data was specified before.
      **/
      NVSG_API virtual const BufferSharedPtr & getVertexBuffer(unsigned int attrib) const;

      /*! \brief Overwrite of getSizeOfVertexData in VertexAttribute.
       *  \param attrib Indicates the vertex attribute to query. 
       *  \return The function returns the number of coordinates per vertex stored for the indicated
       *  attribute. This number can be 1, 2, 3, or 4. If no vertex data was specified for this
       *  vertex attribute before, the function returns 0.
       *  \remarks If there's an animation defined for the specified attribute, the size of the
       *  VertexAttribute data from the current animation step is returned.
       *  \sa getVertexData, getTypeOfVertexData, getNumberOfVertexData */
      NVSG_API virtual unsigned int getSizeOfVertexData( unsigned int attrib ) const;

      /*! \brief Overwrite of getSizeOfVertexData in VertexAttributeSet.
       *  \param attrib Indicates the vertex attribute to query. 
       *  \return The function returns a symbolic constant indicating the type of each coordinate in
       *  the vertex array stored for the specified vertex attribute. If no vertex data was
       *  specified for this vertex attribute before, the function returns NVSG_UNSUPPORTED_TYPE.
       *  \remarks The function returns the per-coordinate type specifier that was used with a
       *  corresponding call to setVertexData. Use getSizeOfVertexData to query the number of
       *  coordinates for each vertex stored. You can use the nvsg::sizeOfType convenient function
       *  to determine the size in bytes of the type returned by this function.
       *  \remarks If there's an animation defined for the specified attribute, the type of the
       *  VertexAttribute data from the current animation step is returned.
       *  \sa getVertexData, getSizeOfVertexData, getNumberOfVertexData */
      NVSG_API virtual unsigned int getTypeOfVertexData( unsigned int attrib ) const;

      /*! \brief Overwrite to getNumberOfVertexData in VertexAttributeSet.
       *  \param attrib Indicates the vertex attribute to query. 
       *  \return The function returns the number of data elements stored for the indicated vertex
       *  attribute.
       *  \remarks If there's an animation defined for the specified attribute, the number of data
       *  elements of the VertexAttribute data from the current animation step is returned.
       *  \sa getVertexData, getSieOfVertexData, getTypeOfVertexData */
      NVSG_API virtual unsigned int getNumberOfVertexData( unsigned int attrib ) const;

      /*! \brief Get the animation of a specified VertexAttribute.
       *  \param attrib The identifier of the VertexAttribute.
       *  \return The VertexAttributeAnimation attached to the VertexAttribute \a attrib.
       *  \sa Animation, VertexAttribute, setAnimation */
      NVSG_API const VertexAttributeAnimationSharedPtr & getAnimation( unsigned int attrib ) const;

      /*! \brief Set the animation of a specified VertexAttribute.
       *  \param attrib The identifier of the VertexAttribute.
       *  \param animation The VertexAttributeAnimation to attach to the VertexAttribute \a attrib
       *  \remarks If \a animation is not NULL, it's reference count is incremented. If there is an
       *  animation attached to the VertexAttribute specified by \ attrib, it's reference count is
       *  decremented. \a animation is stored as the animation of the VertexAttribute \a attrib, if
       *  the current frame is set, the corresponding values are copied from the new animation into
       *  the VertexAttribute, and the incarnation of this AnimatedVertexAttributeSet is
       *  invalidated.
       *  \sa getAnimation */
      NVSG_API void setAnimation( unsigned int attrib, const VertexAttributeAnimationSharedPtr & animation );

      /*! \brief Set the normalize flag for a specified VertexAttribute.
       *  \param attrib The identifier of the VertexAttribute.
       *  \param normalize \c true, if this VertexAttribute should be normalized, otherwise \c false.
       *  \remarks Some animations perform some kind of interpolation between two or more key frame
       *  values. If the normalize flag is set for such a VertexAttribute, the resulting values are
       *  normalized. This is necessary for normals, for example.
       *  \sa getNormalize */
      NVSG_API void setNormalize( unsigned int attrib, bool normalize );

      /*! \brief Get the normalize flag for a specified VertexAttribute.
       *  \param attrib The identifier of the VertexAttribute.
       *  \return \c true, if the normalize flag for the VertexAttribute specified by \a attrib is
       *  set, otherwise \c false.
       *  \sa setNormalize */
      NVSG_API bool getNormalize( unsigned int attrib ) const;

      /*! \brief Get the normalize flags of all VertexAttributes as a bit mask.
       *  \return The normalize flags of all VertexAttributes as a bit mask.
       *  \sa getNormalize, setNormalize */
      NVSG_API unsigned int getNormalizeFlags() const;

      REFLECTION_INFO_API( NVSG_API, AnimatedVertexAttributeSet );
    protected:
      friend struct nvutil::Holder<AnimatedVertexAttributeSet>;

      /*! \brief Default constructor.
       *  \remarks Creates an empty AnimatedVertexAttributeSet, without any animations. */
      NVSG_API AnimatedVertexAttributeSet();

      /*! \brief Copy constructor.
       *  \param rhs AnimatedVertexAttributeSet to copy from.
       *  \remarks Creates an AnimatedVertexAttributeSet by copying the data from \a rhs. Any
       *  animations in \a rhs are referenced in the resulting AnimatedVertexAttributeSet. */
      NVSG_API AnimatedVertexAttributeSet( const AnimatedVertexAttributeSet & rhs );

      /*! \brief Destructor.
       *  \remarks Any Animation in that AnimatedVertexAttributeSet is dereferenced once.
       *  \sa Animation */
      NVSG_API virtual ~AnimatedVertexAttributeSet();

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;
      NVSG_API virtual bool determineAnimationContainment() const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      VertexAttributeAnimationSharedPtr m_animations[NVSG_VERTEX_ATTRIB_COUNT];
      unsigned int                      m_normalizeFlags;
  };

  inline const VertexAttributeAnimationSharedPtr & AnimatedVertexAttributeSet::getAnimation( unsigned int attrib ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT(attrib>=NVSG_POSITION && attrib<=NVSG_ATTR15); // undefined behavior on invalid attrib

    return( m_animations[attribIndex(attrib)] );
  }

  inline bool AnimatedVertexAttributeSet::getNormalize( unsigned int attrib ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT(attrib>=NVSG_POSITION && attrib<=NVSG_ATTR15); // undefined behavior on invalid attrib

    return( !!(m_normalizeFlags & (1 << attribIndex(attrib))) );
  }

  inline unsigned int AnimatedVertexAttributeSet::getNormalizeFlags() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_normalizeFlags );
  }

} //  namespace nvsg

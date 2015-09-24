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
#include "nvsg/OwnedObject.h"
#include "nvsg/VertexAttributeSet.h"
#include "nvmath/Matnnt.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Provides functionality to work with skinned vertex attribute data.
   *  \remarks A Skin can be used in conjunction with a VertexAttributeSet. In addition to the
   *  reference to a VertexAttributeSet, a PrimitiveSet might hold a reference to a Skin.
   *  That way, each PrimitiveSet object can be skinned.\n\n
   *  The base mesh is defined in the VertexAttributeSet part.\n\n
   *  Skin::m_weights hold a vector of weights per vertex. For each vertex, the number of elements
   *  in m_indices and m_weights have to be equal.
   *  For each VertexAttribute, the SkinningType has to be set. NVSG_SKIN_NONE (the default) means
   *  that the corresponding VertexAttribute is not changed by the skinning process. NVSG_SKIN_POINT
   *  uses the skinning matrices. NVSG_SKIN_VECTOR uses the inverse transpose of the skinning
   *  matrices and normalizes the vector at the end.\n\n
   *  The values in m_indices index into the current set of matrices m_jointMatrices.
   *  \note It might happen, that the skinned vertex data is not created as part of this Skin, but
   *  is created in a way that can be directly accessed by the rendering engine.
   *  \note It might be useful, to set the NVSG_HINT_ALWAYS_VISIBLE hint for a GeoNode holding a Skin, to
   *  prevent culling of that object, because the vertices are moved by the skinning process. */
  class Skin : public OwnedObject<Drawable>
  {
    friend class PrimitiveSet;

    public:
      typedef enum
      {
        NVSG_SKIN_NONE,   // don't skin this VertexAttribute (for texture coordinates, for example)
        NVSG_SKIN_POINT,  // skin with the skinning matrices (like positions)
        NVSG_SKIN_VECTOR  // skin with the inverse transpose matrices (like normals) and normalize the resulting vector
      } SkinningType;

    public:
      NVSG_API static SkinSharedPtr create();

    public:
      /*! \brief Assignment operator.
       *  \param rhs Skin to assign.
       *  \return A reference to \a this. */
      NVSG_API Skin & operator=( const Skin & rhs );

      /*! \brief Test for equivalence with an other Skin.
       *  \param p A pointer to an Object to test for equivalence with.
       *  \param ignoreNames  \c true, if names are to be ignored in the equivalence test, otherwise \c false.
       *  \param deepCompare  \c true, if a deep compare should be done, otherwise \c false.
       *  \return \c true, if \a this and \a p are equivalent, otherwise \c false.
       *  \remarks An Object \a p is considered to be equivalent to the Skin \a this, if it also is
       *  a Skin and has the same bind shape matrix, joints, inverse bind matrices, counts, indices,
       *  and weights. */
      NVSG_API virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Set the initial bind pose of this Skin.
       *  \param bindPose A VertexAttributeSet with the initial bind pose.
       *  \sa getBindPose */
      NVSG_API void setBindPose( const VertexAttributeSetSharedPtr & bindPose );

      /*! \brief Get the initial bind pose of this Skin.
       *  \return The initial bind pose.
       *  \sa setBindPose */
      NVSG_API const VertexAttributeSetSharedPtr & getBindPose() const;

      /*! \brief Get the skinning type of the specified attribute.
       *  \param attrib Specifies the vertex attribute to get the skinning type for.
       *  \return The skinning type for the specified attribute.
       *  \remarks There are three possibilities how a vertex attribute should be handled by the
       *  skinning process. It should not be modified (NVSG_SKIN_NONE, the default, for texture
       *  coordinates for example), it should be handled like position information (NVSG_SKIN_POINT),
       *  or it should be handled like normal information (NVSG_SKIN_VECTOR).
       *  \sa setSkinningType */
      NVSG_API Skin::SkinningType getSkinningType( unsigned int attrib ) const;

      /*! \brief Set the skinning type for the specified attribute.
       *  \param attrib Specifies the vertex attribute to set the skinning type for.
       *  \param type The skinning type to set.
       *  \remarks There are three possibilities how a vertex attribute should be handled by the
       *  skinning process. It should not be modified (NVSG_SKIN_NONE, the default, for texture
       *  coordinates for example), it should be handled like position information (NVSG_SKIN_POINT),
       *  or it should be handled like normal information (NVSG_SKIN_VECTOR).
       *  \sa getSkinningType */
      NVSG_API void setSkinningType( unsigned int attrib, SkinningType type );

      /*! \brief Set the complete set of influence data.
       *  \param weights A pointer to the weights.
       *  \param indices A pointer to the indices.
       *  \param counts A pointer to the counts.
       *  \param numberOfInfluences The number of influences to be set.
       *  \remarks The \a numberOfInfluences has to be equal to the number of vertices in the
       *  associated VertexAttributeSet. It is expected, that \a counts points to \a
       *  numberOfInfluences values. Each one specifies how many indices and weights are used for
       *  the corresponding vertex. Is is expected that weights and indices point to a number of
       *  values, that are equal to the sum of all the values in \a counts. The \a indices index
       *  into the array of joint matrices. The \a weights specify the weight to use with each
       *  of those matrices.
       *  \sa getWeights, getIndices, getCounts, getOffsets, getNumberOfInfluences */
      NVSG_API void setInfluenceData( const float * weights
                                    , const unsigned int * indices
                                    , const unsigned int * counts
                                    , unsigned int numberOfInfluences );

      /*! \brief Set influence data, starting at the specified position.
       *  \param pos Specifies the position of the first vertex to set the influence data for.
       *  \param weights A pointer to the weights.
       *  \param indices A pointer to the indices.
       *  \param counts A pointer to the counts.
       *  \param numberOfInfluences The number of influences to be set.
       *  \remarks This inserts the influence data inside (or at the end of) an already defined set
       *  of influence data.
       *  \sa setInfluenceData, getWeights, getIndices, getCounts, getOffsets, getNumberOfInfluences */
      NVSG_API void setInfluenceData( unsigned int pos
                                    , const float * weights
                                    , const unsigned int * indices
                                    , const unsigned int * counts
                                    , unsigned int numberOfInfluences );

      /*! \brief Get a pointer to the weights.
       *  \return A pointer to the weights.
       *  \remarks The weights for each vertex are concatenated to one large array.
       *  \sa setInfluenceData, getIndices, getCounts, getOffsets, getNumberOfInfluences */
      NVSG_API const float * getWeights() const;

      /*! \brief Get a pointer to the indices.
       *  \return A pointer to the indices.
       *  \remarks The indices for each vertex are concatenated to one large array.
       *  \sa setInfluenceData, getWeights, getCounts, getOffsets, getNumberOfInfluences */
      NVSG_API const unsigned int * getIndices() const;

      /*! \brief Get a pointer to the counts.
       *  \return A pointer to the counts.
       *  \remarks The array of counts specifies for each vertex how many index/weight pairs are used.
       *  \sa setInfluenceData, getWeights, getIndices, getOffsets, getNumberOfInfluences */
      NVSG_API const unsigned int * getCounts() const;

      /*! \brief Get a pointer to the offsets.
       *  \return A pointer to the offsets.
       *  \remarks The array of offsets specifies for each vertex the offset into the weights and
       *  indices arrays, where the index/weight pairs start for that vertex.
       *  \sa setInfluenceData, getWeights, getIndices, getCounts, getNumberOfInfluences */
      NVSG_API const unsigned int * getOffsets() const;

      /*! \brief Get the number of influence data.
       *  \return The number of influence data.
       *  \remarks The number of influence data should be equal to the number of vertices of the
       *  associated VertexAttributeSet.
       *  \sa setInfluenceData, getWeights, getIndices, getCounts, getOffsets */
      NVSG_API unsigned int getNumberOfInfluences() const;

      /*! \brief Get the total number of indices.
       *  \return The total number of indices.
       *  \remarks The total number of weights has to be equal to the total number of indices.
       *  \sa getCounts, getNumberOfInfluences */
      NVSG_API unsigned int getTotalNumberOfIndices() const;

      /*! \brief Get the bind shape matrix of this Skin.
       *  \return The bind shape matrix of this Skin.
       *  \remarks The bind shape matrix is the transformation that moves the skin in the skeleton
       *  coordinate system. The default of this matrix is the identity.
       *  \sa setBindShapeMatrix */
      NVSG_API const nvmath::Mat44f & getBindShapeMatrix() const;

      /*! \brief Set the bind shape matrix of this Skin.
       *  \param matrix The bind shape matrix of this Skin.
       *  \remarks The bind shape matrix is the transformation that moves the skin in the skeleton
       *  coordinate system. The default of this matrix is the identity.
       *  \sa getBindShapeMatrix */
      NVSG_API void setBindShapeMatrix( const nvmath::Mat44f & matrix );

      /*! \brief Get the number of Joints of this Skin.
       *  \return The number of Joints of this Skin.
       *  \sa addJoint, getJoint, getInverseBindMatrix, removeJoint */
      NVSG_API unsigned int getNumberOfJoints() const;

      /*! \brief Add a Joint to this Skin.
       *  \param joint The Transform to use as a Joint.
       *  \param inverseBindMatrix The transformation that is applied to a vertex to express it in the joint coordinate system.
       *  \param position Optional position where to place the new Joint.
       *  \return The function returns the zero-based position where the Joint has been added.
       *  \remarks The reference count and the joint marker of \a joint is increased.\n\n
       *  When omitting the \a position parameter, the \a joint will be added at the end. If \a
       *  position specifies a position greater than or equal to the actual number of joints, the
       *  new \a joint will be added to the end. Otherwise the joint will be inserted at that
       *  position forcing all subsequent joints to move up in the list of joints.
       *  \sa getNumberOfJoints, getJoint, getInverseBindMatrix, removeJoint */
      NVSG_API unsigned int addJoint( const TransformSharedPtr & joint, const nvmath::Mat44f & inverseBindMatrix, unsigned int position = ~0 );

      /*! \brief Get the joint at the specified position.
       *  \param position Zero-based position of the joint to get.
       *  \return The function returns the Transform at the specified position.
       *  \remarks The behavior of this function is undefined, if \a position is greater than or
       *  equal to the number of joints of this Skin.
       *  \sa getNumberOfJoints, addJoint, getInverseBindMatrix, removeJoint */
      NVSG_API const TransformSharedPtr & getJoint( unsigned int position ) const;

      /*! \brief Get the inverse bind matrix of the joint at the specified position.
       *  \param position Zero-based position of the inverse bind matrix to get.
       *  \return The function returns a constant reference to the inverse bind matrix at the specified position.
       *  \remarks The behavior of this function is undefined, if \a position is greater than or
       *  equal to the number of joints of this Skin.
       *  \sa getNumberOfJoints, addJoint, getJoint, removeJoint */
      NVSG_API const nvmath::Mat44f & getInverseBindMatrix( unsigned int position ) const;

      /*! \brief Remove the joint at the specified position.
       *  \param position Zero-based position of the joint to remove.
       *  \remarks When \a position is greater than or equal to the number of joints of this Skin,
       *  the last joint is removed. Otherwise the joint at the specified position is removed,
       *  forcing all subsequent joints to move one down in the list of joints.\n\n
       *  When removing a joint, the joint marker and the reference count of the corresponding
       *  Transform is decreased.
       *  \sa getNumberOfJoints, addJoint, getJoint, getInverseBindMatrix */
      NVSG_API void removeJoint( unsigned int position );

      REFLECTION_INFO_API( NVSG_API, Skin );
    protected:
      friend struct nvutil::Holder<Skin>;

      /*! \brief Default constructor.
       *  \remarks Creates an empty Skin. */
      NVSG_API Skin();

      /*! \brief Copy constructor.
       *  \param rhs Skin to copy from.
       *  \remarks Creates a Skin by copying the data from \a rhs. */
      NVSG_API Skin( const Skin & rhs );

      /*! \brief Destructor. */
      NVSG_API virtual ~Skin();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;
    private:
      nvsg::VertexAttributeSetSharedPtr m_bindPose;
      nvmath::Mat44f                    m_bindShapeMatrix;
      std::vector<unsigned int>         m_counts;
      std::vector<unsigned int>         m_indices;
      std::vector<nvmath::Mat44f>       m_inverseBindMatrices;
      std::vector<TransformSharedPtr>   m_joints;
      SkinningType                      m_skinningType[VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT];
      std::vector<float>                m_weights;
      std::vector<unsigned int>         m_offsets;
  };

  inline const nvsg::VertexAttributeSetSharedPtr & Skin::getBindPose() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_bindPose );
  }

  inline Skin::SkinningType Skin::getSkinningType( unsigned int attrib ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( attrib < VertexAttributeSet::NVSG_VERTEX_ATTRIB_COUNT );
    return( m_skinningType[attrib] );
  }

  inline const float * Skin::getWeights() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( &m_weights[0] );
  }

  inline const unsigned int * Skin::getIndices() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( &m_indices[0] );
  }

  inline const unsigned int * Skin::getCounts() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( &m_counts[0] );
  }

  inline const unsigned int * Skin::getOffsets() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( &m_offsets[0] );
  }

  inline unsigned int Skin::getNumberOfInfluences() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_counts.size()) );
  }

  inline unsigned int Skin::getTotalNumberOfIndices() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_indices.size()) );
  }

  inline const nvmath::Mat44f & Skin::getBindShapeMatrix() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_bindShapeMatrix );
  }

  inline void Skin::setBindShapeMatrix( const nvmath::Mat44f & matrix )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( matrix != m_bindShapeMatrix )
    {
      m_bindShapeMatrix = matrix;
      notifyChange( this );
    }
  }

  inline unsigned int Skin::getNumberOfJoints() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_joints.size()) );
  }

  inline const TransformSharedPtr & Skin::getJoint( unsigned int position ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( position < m_joints.size() );
    return( m_joints[position] );
  }

  inline const nvmath::Mat44f & Skin::getInverseBindMatrix( unsigned int position ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( position < m_inverseBindMatrices.size() );
    return( m_inverseBindMatrices[position] );
  }

} //  namespace nvsg

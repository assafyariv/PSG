// Copyright NVIDIA Corporation 2011
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
#include "nvsg/Camera.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Class describing a Camera by a projection matrix.
   *  \par Namespace: nvsg
   *  \remarks A MatrixCamera overwrites the projection-specific functions of a Camera.
   *  \sa Camera, ParallelCamera, PerspectiveCamera */
  class MatrixCamera : public Camera
  {
    public:
      NVSG_API static MatrixCameraSharedPtr create();

    public:
      NVSG_API void setMatrices( const nvmath::Mat44f & projection, const nvmath::Mat44f & inverse );

      /*! \brief Get the projection matrix of this MatrixCamera.
       *  \return The projection transformation.
       *  \sa Camera, getInverseProjection, getWorldToViewMatrix, getViewToWorldMatrix */
      NVSG_API virtual nvmath::Mat44f getProjection() const;

      /*! \brief Get the inverse projection matrix of this MatrixCamera.
       *  \return The inverse projection transformation.
       *  \sa Camera, getProjection,  getWorldToViewMatrix, getViewToWorldMatrix */
      NVSG_API virtual nvmath::Mat44f getInverseProjection()  const;

      NVSG_API void virtual zoom( float factor );

      /*! \brief Assignment operator
       *  \param rhs A reference to the constant MatrixCamera to copy from.
       *  \return A reference to the assigned MatrixCamera.
       *  \remarks The assignment operator calls the assignment operator of Camera. */
      NVSG_API MatrixCamera & operator=(const MatrixCamera & rhs);

      /*! \brief Determine the CullCode of a Sphere3f relative to the view volume.
       *  \param sphere A reference to the constant Sphere3f to determine the CullCode for.
       *  \return CC_IN, if the Sphere3f \a sphere is completely inside the view volume; CC_OUT
       *  if it is completely out of the view volume; otherwise CC_PART. */
      NVSG_API virtual CullCode determineCullCode( const nvmath::Sphere3f &sphere ) const;

      REFLECTION_INFO_API( NVSG_API, MatrixCamera );
    protected:
      friend struct nvutil::Holder<MatrixCamera>;

      /*! \brief Default-constructs a MatrixCamera. 
       *  \remarks The MatrixCamera initially is positioned at (0.0,0.0,1.0), has the y-axis
       *  as up-vector and looks down the negative z-axis. */
      NVSG_API MatrixCamera();

      /*! \brief Copy-constructs a MatrixCamera from another MatrixCamera. */
      NVSG_API MatrixCamera( const MatrixCamera &rhs );

      /*! Destructs a MatrixCamera. */
      NVSG_API virtual ~MatrixCamera();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;
    private:
      nvmath::Mat44f  m_projection;
      nvmath::Mat44f  m_inverse;
  };

} // namespace

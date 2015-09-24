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

#include "nvsgcommon.h"
#include "nvmath/Matnnt.h"
#include "nvsg/Group.h"
#include "nvutil/HashGenerator.h"
#include "nvutil/SmartPtr.h"

namespace nvsg
{
  /*! \brief Distance-based level of detail class.
   *  \par Namespace: nvsg
   *  \remarks This class is a special kind of a group containing different 
   *  representations of the same object as well as some range information. 
   *  The representations include different levels of detail, meaning 
   *  that the first child is the best and most precise representation 
   *  of the object, the second child is of lower resolution, and so on.
   *  The current implementation determines the distance between the 
   *  camera and the given center point of the object (dist). The distance 
   *  is compared against the ranges and the corresponding range defines 
   *  the LOD representation to use.\n
   *  \n
   *  It works like this:\n
   *  \code
   *  IF (dist < ranges[0])                    THEN activeChild = 0
   *  ELSE IF (ranges[i-1] < dist < ranges[i]) THEN activeChild = i
   *  ELSE IF (dist > ranges[LASTRANGE])       THEN activeChild = LASTRANGE+1
   *  \endcode
   *  \n
   *  You should provide <b>n</b> ranges and <b>n+1</b> children. 
   *  If you specify too few ranges, the extra children will never 
   *  be used. If you instead provide too few children, the last child will be 
   *  used for all the extra ranges.\n
   *  \n
   *  In some cases you do not want to draw something on the last (infinite)
   *  level. In this case, simply add an empty Group node at the lowest resolution level.
   *  The same applies to the highest resolution level if you want geometry that is too  
   *  near to the camera to disappear. */
  class LOD : public Group
  {
    public:
      NVSG_API static LODSharedPtr create();

    public:
      /*! \brief Define the different ranges where the LOD should switch the representation.
       *  \param ranges A vector of range values representing the world-space distances. 
       *  Pass in NULL to clear the range data.
       *  \param count Number of ranges in the given vector.
       *  \remarks This function can be used to set the different ranges for switching 
       *  between the different representations. You need to provide n ranges for n+1 children.
       *  \par Example
       *  \code
       *  float ranges[3];
       *  range[0] = 10;
       *  range[1] = 20;
       *  range[2] = 30;
       *  LODWriteLock(pLOD)->setRanges( &ranges[0], 3 );
       *  \endcode
       *  \sa getRanges*/
      NVSG_API void setRanges(const float * ranges, unsigned int count);

      /*! \brief Get the ranges of the LOD.
       *  \return A pointer to a constant vector of ranges. If there are no ranges defined, the 
       *  return value is NULL.
       *  \remarks You need to call nvsg::getNumberOfRanges to determine the number of ranges.
       *  The ranges represent the world-space distances where the LOD switches to the 
       *  next representation.
       *  \sa getNumberOfRanges*/
      NVSG_API const float * getRanges() const;

      /*! \brief Get the number of ranges.
       *  \return The number of ranges in the LOD. 
       *  \sa setRanges*/
      NVSG_API unsigned int getNumberOfRanges() const;
      
      /*! \brief Set the center of the LOD object.
       *  \param center Center point of the LOD object in world-space coordinates. 
       *  \remarks The center is the point used for calculating the exact level of detail to use.
       *  By calculating the distance between this center point and the camera position in 
       *  the LOD, can decide which representation to choose.\n
       *  A good center point is, for example, the center of the bounding sphere of the object in the 
       *  highest representation.
       *  \sa getCenter */
      NVSG_API void setCenter(const nvmath::Vec3f & center);

      /*! \brief Get the center point of the LOD object.
       *  \return The center point in world-space coordinates. 
       *  \remarks See nvsg::setCenter for a detailed description on the center point.
       *  \sa setCenter*/
      NVSG_API const nvmath::Vec3f & getCenter() const;

      /*! \brief Calculate the LOD level to use.
       *  \param modelViewMatrix currentWorldMatrix * currentViewingMatrix
       *  \param scaleFactor This factor is used to scale the ranges of the LOD node. Default is 1.f.
       *  See ViewState::getLODRangeScale for further details.
       *  \return The index of the LOD level that this node will use. 
       *  0xffffffff will be returned when there is no LOD level available.
       *  \remarks This method is called by the ModelViewTraverser. It calculates
       *  the distance between the center point and the camera.\n
       *  We calculate the distance by transforming the center into 
       *  object space, and because the camera is located at the origin in 
       *  object space, the calculation is trivial.\n
       *  The scale factor can be used to globally scale the ranges without changing the LOD node 
       *  ranges directly. This can be used, for example, for scenes that were initially created for use 
       *  on small monitors. You can use this scaling to fine-tune these scenes for large projection
       *  walls by scaling the LOD levels to switch later to a lower resolution representation. 
       *  \sa ViewState::getLODRangeScale*/
      NVSG_API virtual unsigned int getLODToUse(const nvmath::Mat44f & modelViewMatrix, float scaleFactor) const;

      /*! \brief Lock the range selection to a specific LOD level.
       *  \param on true to turn on the range lock.
       *  \param rangeNumber The range that should be used for all the rendering. 
       *  \remarks This feature prevents the distance calculation and forces the LOD to use 
       *  a specific representation.\n
       *  If the provided range number is larger than the available ranges, the LOD always uses the 
       *  representation of the last range - lowest resolution. 
       *  \par Example
       *  The following sample code demonstrates how to lock all LOD node of a scene to a specified
       *  range.
       *  \code
       *  void LODDlg::lockLODRange(bool on, unsigned int level)
       *  {
       *    SceneSharedPtr pScene( m_pRA->getScene() );
       *    if (pScene)
       *    {
       *      SmartPtr<SearchTraverser> st( new SearchTraverser );
       *      st->setClassName( "class nvsg::LOD" );
       *      st->setBaseClassSearch( true );
       *      st->apply( pScene );  
       *      const vector<const Path *> &vp = st->getPaths();
       *      for ( unsigned int i=0 ; i<vp.size() ; i++ )
       *      {
       *        LODWriteLock( dynamic_cast<LOD*>(vp[i]->getTail()) )->setRangeLock( on, level );
       *      }
       *    }
       *  }
       *  \endcode
       *  \sa getRangeLock */
      NVSG_API void setRangeLock(bool on, unsigned int rangeNumber);

      /*! \brief Get the range lock number.
       *  \return The range number this LOD is forced to use. 
       *  \remarks The returned value is only valid if LOD::isRangeLockedEnabled returns true.
       *  \sa setRangeLock, isRangeLockEnabled*/
      NVSG_API unsigned int getRangeLock() const;

      /*! \brief Check if the LOD is forced to use a specific range. 
       *  \return This function returns true when the range lock is turned on. 
       *  \sa setRangeLock */
      NVSG_API bool isRangeLockEnabled() const;
            
      /*! \brief Copy operator
       *  \param rhs A reference to the constant LOD to copy from
       *  \return A reference to the copied LOD
       *  \remarks The copy operator calls the copy-operator of Group and then copies the
       *  Group-specific data.
       *  \sa Group */      
      NVSG_API LOD & operator=(const LOD & rhs);
      
      /*! \brief Test for equivalence with an other LOD. 
       *  \param p A pointer to the LOD to test for equivalence with.
       *  \param ignoreNames  Optional parameter to ignore the names of the objects; default is \c true.
       *  \param deepCompare  Optional parameter to perform a deep comparsion; default is \c false.
       *  \return \c true if the LOD \a p is equivalent to \c this, otherwise \c false.
       *  \remarks If \a p and \c this are equivalent as LOD, they are equivalent
       *  if they have the same center, ranges, and range lock settings.
       *  \note The behavior is undefined if \a p is not an LOD or derived from one. */
      NVSG_API virtual bool isEquivalent(const Object *p, bool ignoreNames, bool deepCompare) const;

    // reflected properties
    public:

      REFLECTION_INFO_API( NVSG_API, LOD );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( Center );
      END_DECLARE_STATIC_PROPERTIES

    protected:
      friend struct nvutil::Holder<LOD>;

      /*! \brief Default-constructs a LOD. 
       */
      NVSG_API LOD( void );

      /*! \brief Constructs a LOD as a copy of another LOD.
      */
      NVSG_API LOD( const LOD &rhs );
      
      /*! \brief Destructs a LOD.
       */
      NVSG_API virtual ~LOD( void );

      /*! \brief Overrides Node::determineLODContainment.
       * \return The function returns \c true if this LOD contain more than one selectable level.
       * If the LOD object is range-locked, or if it holds only a single range, the function returns \c false. */
      NVSG_API virtual bool determineLODContainment() const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      nvmath::Vec3f        m_center;
      std::vector<float>   m_ranges;
      unsigned int         m_rangeLock;
      bool                 m_isRangeLocked;
  };

  /*! \brief Performs a lexical less than comparison of two LODs 
   * This functor can be used as comparison function to order elements of LODSharedPtr 
   * in an associative STL container like map. */
  struct LODLessThan
  {
    /*! \brief Performs a lexical less than comparison of two LODs
     * \param lhs Specifies the left-hand-sided LOD considered for comparison.
     * \param rhs Specifies the right-hand-sided LOD considered for comparison.
     * \return \c true if the left-hand-sided LOD is lexical less than the right-hand-sided LOD. 
     * This function is used in the nvsg::LODLessThan functor, which can be used as comparison function
     * to order elements of LODSharedPtr in an associative STL container like map. */
    bool operator()( const LODSharedPtr& lhs, const LODSharedPtr& rhs) const;
  };
}

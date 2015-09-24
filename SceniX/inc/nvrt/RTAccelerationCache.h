// Copyright NVIDIA Corporation 2009
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

#include "nvsgrtapi.h"
#include "nvsg/CoreTypes.h"
#include "RTAcceleration.h"

namespace nvrt
{
  /*! \brief Pure virtual base class for caching acceleration structures.
   *  \remarks The acceleration structures for GeoNodes tend to be large, and their creation takes
   *  substantial time. A class derived from this base class can be used to implement a caching
   *  mechanism for that data structure.\n
   *  The file name for the cache can be used to locate some file specific caches.\n
   *  Besides that, concrete derivatives of this class have to resolve the deletion, loading, and
   *  storing of a cache.
   */
  class RTAccelerationCache : public nvutil::HandledObject
  {
    public:
      /*! \brief Default constructor of RTAccelerationCache. */
      NVRT_API RTAccelerationCache();

      /*! \brief Copy constructor of RTAccelerationCache.
       *  \param rhs The constant RTAccelerationCache to copy from. */
      NVRT_API RTAccelerationCache( const RTAccelerationCache & rhs );

      /*! \brief Destructor of RTAccelerationCache. */
      NVRT_API virtual ~RTAccelerationCache();

      /*! \brief Copy operator.
       *  \param rhs The constant RTAccelerationCache to copy from. */
      NVRT_API RTAccelerationCache & operator=( const RTAccelerationCache & rhs );

      /*! \brief Set the file name for the following caching operations.
       *  \param fileName The file name to use for following caching operations.
       *  \remarks The base implementation just stores the file name. Classes overwriting this
       *  function should call the base class implementation from that.
       *  \sa getFileName */
      NVRT_API virtual void setFileName( const std::string & fileName );

      /*! \brief Get the file name associated with the current acceleration cache.
       *  \return The file name currently associated with the acceleration cache.
       *  \sa setFileName */
      NVRT_API const std::string & getFileName() const;

      /*! \brief Interface to delete the cache.
       *  \param gnh The GeoNode to cache.
       *  \return \c true, if the cache for \a gnh could be found and deleted, otherwise \c false.
       *  \remarks All caches previously associated with \a gnh by a call to loadCache or storeCache
       *  with that same pointer should be deleted.
       *  \sa loadCache, storeCache */
      NVRT_API virtual bool deleteCache( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType ) = 0;

      /*! \brief Interface to load a cache.
       *  \param gnh The GeoNode to load the cache for.
       *  \param cache A reference to a vector of byte to fill with the cache data.
       *  \return \c true, if the cache could be loaded, otherwise \c false.
       *  \remarks If a cache for \a gnh is available, it has to be loaded into \a cache. Otherwise
       *  \a cache should be cleared.
       *  \sa deleteCache, storeCache */
      NVRT_API virtual bool loadCache( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType, std::vector<char> & cache ) = 0;

      /*! \brief Interface to store a cache.
       *  \param gnh The GeoNode to store the cache for.
       *  \param cache Constant reference to a vector of bytes that holds the cache data.
       *  \return \c true, if the cache was successfully stored, otherwise \c false.
       *  \note It might be worth to also delete any old caches that were associated with \a gnh but
       *  turn out to be outdated.
       *  \sa deleteCache, loadCache */
      NVRT_API virtual bool storeCache( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType, const std::vector<char> & cache ) = 0;

    private:
      std::string m_fileName;
  };
}

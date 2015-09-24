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

#include "nvrt/nvsgrtapi.h"
#include "nvrt/RTAccelerationCache.h"
#include "nvsg/CoreTypes.h"
#include "nvutil/HashGeneratorMD5.h"


namespace nvrt
{
  /*! \brief Concrete class for caching acceleration structures.
   *  \remarks The acceleration structures for GeoNodes tend to be large, and their creation takes
   *  substantial time. This class implements a caching mechanism that creates one file per GeoNode
   *  to cache.\n
   *  The file name for the cache is used to create a directory to hold the cache files.\n
   *  \sa RTAccelerationCache
   */
  class RTMultiFileCache : public RTAccelerationCache
  {
    public:
      NVRT_API static RTMultiFileCacheSharedPtr create();

    public:
      /*! \brief Default constructor of RTMultiFileCache. */
      NVRT_API RTMultiFileCache();

      /*! \brief Copy constructor of RTMultiFileCache.
       *  \param rhs The constant RTMultiFileCache to copy from. */
      NVRT_API RTMultiFileCache( const RTMultiFileCache & rhs );

      /*! \brief Destructor of RTMultiFileCache. */
      NVRT_API virtual ~RTMultiFileCache();

      /*! \brief Copy operator.
       *  \param rhs The constant RTMultiFileCache to copy from. */
      NVRT_API RTMultiFileCache & operator=( const RTMultiFileCache & rhs );

      /*! \brief Set the file name for the following caching operations.
       *  \param fileName The file name to use for following caching operations.
       *  \remarks The MultiFile cache appends the string "_cache/" to the \a fileName and uses that
       *  name as the directory for the cache files. If that directory does not exist, it is created.
       *  \sa getFileName */
      NVRT_API virtual void setFileName( const std::string & fileName );

      /*! \brief Delete the cache of the specified GeoNode.
       *  \param gnh The GeoNode to delete the cache of.
       *  \return \c true, if the cache for \a gnh could be found and deleted, otherwise \c false.
       *  \remarks All caches previously associated with \a gnh by a call to loadCache or storeCache
       *  with that same pointer should are deleted.
       *  \sa loadCache, storeCache */
      NVRT_API virtual bool deleteCache( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType builderType );

      /*! \brief Load the cache of the specified GeoNode.
       *  \param gnh The GeoNode to load the cache for.
       *  \param cache A reference to a vector of bytes to fill with the cache data.
       *  \return \c true, if the cache could be loaded, otherwise \c false.
       *  \remarks If a cache for \a gnh is available, it is loaded into \a cache. Otherwise
       *  \a cache is cleared.
       *  \sa deleteCache, storeCache */
      NVRT_API virtual bool loadCache( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType builderType, std::vector<char> & cache );

      /*! \brief Store the cache of the specified GeoNode.
       *  \param gnh The GeoNode to store the cache for.
       *  \param cache Constant reference to a vector of bytes that holds the cache data.
       *  \return \c true, if the cache was successfully stored, otherwise \c false.
       *  \note Any old cache file that was associated with \a gnh but turned out to be outdated is
       *  deleted.
       *  \sa deleteCache, loadCache */
      NVRT_API virtual bool storeCache( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType builderType, const std::vector<char> & cache );

    private:
      void        addToHash( const nvsg::DrawableSharedPtr & dh );
      void        addToHash( const nvsg::IndependentPrimitiveSetSharedPtr & ipsh );
      void        addToHash( const nvsg::MeshedPrimitiveSetSharedPtr & mpsh );
      void        addToHash( const nvsg::PrimitiveSetSharedPtr & psh );
      void        addToHash( const nvsg::StrippedPrimitiveSetSharedPtr & spsh );
      void        addToHash( const nvsg::VertexAttributeSetSharedPtr & vash );
      std::string getHash( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType builderType );

    private:
      struct Key {
        Key( const nvsg::GeoNodeWeakPtr & gnh, RTAcceleration::RTBuilderType type )
          : m_gnh(gnh)
          , m_type(type)
        {}

        bool operator<( const Key& rhs ) const
        {
          return m_gnh < rhs.m_gnh || (m_gnh == rhs.m_gnh && m_type < rhs.m_type);
        }

        nvsg::GeoNodeWeakPtr m_gnh;
        RTAcceleration::RTBuilderType  m_type;
      };

      typedef std::map<Key ,std::string> CacheMap;

      std::string               m_cacheDir;
      CacheMap                  m_cacheMap;
      nvutil::HashGeneratorMD5  m_hash;
  };
}


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
#include "nvsg/MapObject.h"
#include "nvsg/Object.h"
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Class to hold a list of two-dimensional maps.
   *  \par Namespace: nvsg
   *  \remarks A MapList is used in a ViewState to hold a list of two-dimensional maps. These maps
   *  are displayed on top of the scene. They can be, for example, some information overlay or type 
   *  of menus.\n
   *  The MapList holds a list of MapObject objects. A MapObject can be a MapElement, holding a
   *  single Map; a MapArray, holding a row or column of Map objects; or a MapMatrix, holding a
   *  matrix of Map objects.\n
   *  The MapObject objects are positioned in the viewport by raster coordinates.
   *  \sa Map, MapArray, MapElement, MapObject, MapMatrix */
  class MapList : public Object
  {
    public:
      NVSG_API static MapListSharedPtr create();

    public:
      /*! \brief Get the number of MapObject objects in this MapList.
       *  \return The number of contained MapObject objects.
       *  \sa addMapObject, getMapObject */
      NVSG_API unsigned int getNumberOfMapObjects( void ) const;

      /*! \brief Get a pointer to the constant MapObject at the specified index.
       *  \param index Index of the MapObject to get.
       *  \return A pointer to the constant MapObject at the specified index in the MapList.
       *  \note The behavior is undefined if \a index is larger than or equal to the number of
       *  MapObject objects in this MapList.
       *  \sa addMapObject, getNumberOfMapObjects, pickMap */
      NVSG_API const MapObjectSharedPtr & getMapObject( unsigned int index ) const;

      /*! \brief Add a MapObject to the end of the MapList.
       *  \param mo A pointer to the constant MapObject to add
       *  \remarks The reference count of \a mo is increased.
       *  \sa getMapObject, getNumberOfMapObjects, insertMapObject */
      NVSG_API void addMapObject( const MapObjectSharedPtr & mo );

      /*! \brief Insert a MapObject in front of the specified index.
       *  \param index Index to insert a MapObject before.
       *  \param mo A pointer to the constant MapObject to insert.
       *  \remarks The reference count of \a mo is incremented.
       *  \note The behavior is undefined if \a index is larger than or equal to the number of
       *  MapObject objects in this MapList.
       *  \sa addMapObject, getMapObject, getNumberOfMapObjects */
      NVSG_API void insertMapObject( unsigned int index, const MapObjectSharedPtr & mo );

      /*! \brief Replace the MapObject at the specified index.
       *  \param index Index to determine where to replace the MapObject.
       *  \param mo A pointer to the constant MapObject to replace with.
       *  \remarks The reference count of \a mo is incremented, and the reference count of the
       *  MapObject previously at index \a index is decremented.
       *  \note The behavior is undefined if \a index is larger than or equal to the number of
       *  MapObject objects in this MapList.
       *  \sa addMapObject, getMapObject, getNumberOfMapObjects, removeMapObject */
      NVSG_API void replaceMapObject( unsigned int index, const MapObjectSharedPtr & mo );

      /*! \brief Remove the MapObject at the specified index.
       *  \param index Index to determine where to remove the MapObjec.
       *  \remarks The reference count of the MapObject at index \a index is decremented.
       *  \note The behavior is undefined if \a index is larger than or equal to the number of
       *  MapObject objects in this MapList.
       *  \sa addMapObject, getMapObject, getNumberOfMapObjects, replaceMapObject */
      NVSG_API void removeMapObject( unsigned int index );

      /*! \brief Clear the MapList
       *  \remarks The reference count of every MapObject in this MapList is decremented, and the
       *  list is cleared.
       *  \sa addMapObject, getNumberOfMapObjects, removeMapObject */
      NVSG_API void clear( void );

      /*! \brief Get the image located at the specified raster position.
       *  \param x Horizontal raster coordinate to use for getting the image
       *  \param y Vertical raster coordinate to use for getting the image.
       *  \param index Optional pointer to get the index of the MapObject that holds the picked image.
       *  \return The TextureHost specifying the image at the specified raster position. If an image
       *  is picked and \a index is not NULL, it returns the index of the MapObject holding the picked image.
       *  \remarks This function determines the image of this MapList at the specified raster
       *  position (\a x, \a y), if any. It asks each of its MapObjects if it has an image at the
       *  specified position, and if so, that image is returned. If \a index is not NULL, the index
       *  of the MapObject holding that image is returned there, too.
       *  \sa getMapObject, getNumberOfMapObjects */
      NVSG_API const TextureHostSharedPtr & pickMap( int x, int y, unsigned int * index = NULL ) const;

      REFLECTION_INFO_API( NVSG_API, MapList );
    protected:
      friend struct nvutil::Holder<MapList>;

      /*! \brief Constructs an empty MapList. */
      NVSG_API MapList();

      /*! \brief Copy-Constructs a MapList from another MapList. */
      NVSG_API MapList( const MapList &rhs );

      /*! \brief Destructs a MapList. */
      NVSG_API virtual ~MapList(void);

      NVSG_API virtual void syncIncarnation(PFNGETINCARNATION) const;

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      typedef std::vector<MapObjectSharedPtr>::const_iterator ConstMapObjectIterator;
      typedef std::vector<MapObjectSharedPtr>::iterator MapObjectIterator;
      std::vector<MapObjectSharedPtr>  m_list;
  };

  inline void MapList::addMapObject( const MapObjectSharedPtr & mo )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( mo );
    m_list.push_back( mo );
    notifyChange( this );
  }

  inline const MapObjectSharedPtr & MapList::getMapObject( unsigned int index ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( index < m_list.size() );
    return( m_list[index] );
  }

  inline unsigned int MapList::getNumberOfMapObjects( void ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_list.size()) );
  }

  inline void MapList::insertMapObject( unsigned int index, const MapObjectSharedPtr & mo )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( mo && ( index < m_list.size() ) );
    m_list.insert( m_list.begin() + index, mo );
    notifyChange( this );
  }

  inline void MapList::removeMapObject( unsigned int index )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( index < m_list.size() );
    m_list.erase( m_list.begin() + index );
    notifyChange( this );
  }

  inline void MapList::replaceMapObject( unsigned int index, const MapObjectSharedPtr & mo )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( mo && ( index < m_list.size() ) );
    if ( m_list[index] != mo )
    {
      m_list[index] = mo;
      notifyChange( this );
    }
  }
}

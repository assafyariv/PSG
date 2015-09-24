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
#include "nvsg/TextureHost.h"
#include "nvutil/HashGenerator.h"
#include <vector>

namespace nvsg
{
  /*! \brief Class to hold an array of Map objects.
   *  \par Namespace: nvsg
   *  \remarks This class is used to add an array of Map objects to a MapList. This array can be
   *  oriented horizontally or vertically. The Map objects can be aligned along a common base,
   *  center, or top line. The traversal direction can be from the lower to the higher index, or
   *  vice versa. A spacing can be determined to be between each of the Map objects.
   *  \sa Map, MapElement, MapList, MapMatrix */
  class MapArray : public MapObject
  {
    public:
      NVSG_API static MapArraySharedPtr create();

    public:
      /*! \brief Get the alignment of the MapArray.
       *  \return The MapAlignment of the MapArray.
       *  \remarks If the Map objects of the MapArray have different sizes, they can be aligned
       *  along their lower (MA_LOWER), center (MA_CENTER), or upper (MA_UPPER) coordinate. The
       *  default is MA_LOWER.
       *  \par Example
       *  If the MapArray is horizontally oriented, and the Map objects have different
       *  heights, they can have a common base line (MA_LOWER), a common center line (MA_CENTER),
       *  or a common top line (MA_UPPER).\n
       *  If the MapArray is vertically oriented, and the Map objects have different widths, they
       *  can have a common left margin (MA_LOWER), a common center line (MA_CENTER), or a common
       *  right margin (MA_UPPER).
       *  \sa getOrientation, setAlignment */
      NVSG_API MapAlignment getAlignment() const;

      /*! \brief Set the alignment of the MapArray.
       *  \param alignment The MapAlignment of the MapArray.
       *  \remarks If the Map objects of the MapArray have different sizes, they can be aligned
       *  along their lower (MA_LOWER), center (MA_CENTER), or upper (MA_UPPER) coordinate. The
       *  default is MA_LOWER.
       *  \par Example
       *  If the MapArray is horizontally oriented, and the Map objects have different
       *  heights, they can have a common base line (MA_LOWER), a common center line (MA_CENTER),
       *  or a common top line (MA_UPPER).\n
       *  If the MapArray is vertically oriented, and the Map objects have different widths, they
       *  can have a common left margin (MA_LOWER), a common center line (MA_CENTER), or a common
       *  right margin (MA_UPPER).
       *  \sa getAlignment, setOrientation */
      NVSG_API void setAlignment( MapAlignment alignment );

      /*! \brief Get the orientation of the MapArray.
       *  \return The MapOrientation of the MapArray.
       *  \remarks The Map objects of the MapArray can be oriented either horizontally
       *  (MO_HORIZONTAL) or vertically (MO_VERTICAL), thus resulting in either a row of Map
       *  objects or a column of Map objects. The default is MO_VERTICAL.
       *  \sa getAlignment, setOrientation */
      NVSG_API MapOrientation getOrientation() const;

      /*! \brief Set the orientation of the MapArray.
       *  \param orientation The MapOrientation of the MapArray.
       *  \remarks The Map objects of the MapArray can be oriented either horizontally
       *  (MO_HORIZONTAL) or vertically (MO_VERTICAL), thus resulting in either a row of Map
       *  objects or a column of Map objects. The default is MO_VERTICAL.
       *  \sa getOrientation, setAlignment */
      NVSG_API void setOrientation( MapOrientation orientation );

      /*! \brief Get the spacing between consecutive Map objects.
       *  \return The spacing between consecutive Map objects.
       *  \remarks Between each Map of the MapArray, there can be some open space remaining. The default
       *  is one.
       *  \sa setSpacing */
      NVSG_API unsigned int getSpacing() const;

      /*! \brief Set the spacing between consecutive Map objects.
       *  \param spacing The spacing between consecutive Map objects.
       *  \remarks Between each Map of the MapArray, there can be some open space remaining. The default
       *  is one.
       *  \sa getSpacing */
      NVSG_API void setSpacing( unsigned int spacing );

      /*! \brief Get the traversal direction of the MapArray
       *  \return The MapTraversalDirection of the MapArray.
       *  \remarks The Map objects of the MapArray can be traversed either from lower to higher
       *  index (MTD_LOW_TO_HIGH) or from higher to lower index (MTD_HIGH_TO_LOW). The default is
       *  MTD_LOW_TO_HIGH.
       *  \par Example
       *  If the MapArray is vertically oriented, aligned to the lower base line, and
       *  the traversal direction is MTD_LOW_TO_HIGH, the Map with index 0 is drawn at the bottom
       *  base line, the Map with index 1 is drawn above, and so on.
       *  \sa getAlignment, getOrientation, setTraversalDirection */
      NVSG_API MapTraversalDirection getTraversalDirection() const;

      /*! \brief Set the traversal direction of the MapArray.
       *  \param direction The MapTraversalDirection of the MapArray. 
       *  \remarks The Map objects of the MapArray can be traversed either from lower to higher
       *  index (MTD_LOW_TO_HIGH) or from higher to lower index (MTD_HIGH_TO_LOW). The default is
       *  MTD_LOW_TO_HIGH.
       *  \par Example
       *  If the MapArray is vertically oriented, aligned to the lower base line, and
       *  the traversal direction is MTD_LOW_TO_HIGH, the Map with index 0 is drawn at the bottom
       *  base line, the Map with index 1 is drawn above, and so on.
       *  \sa getTraversalDirection, setAlignment, setOrientation */
      NVSG_API void setTraversalDirection( MapTraversalDirection direction );

      /*! \brief Get the number of Map objects in this MapArray.
       *  \return The number of Map objects in this MapArray.
       *  \sa addMap, getMap, insertMap, pickMap, removeMap, replaceMap */
      NVSG_API unsigned int getNumberOfMaps() const;

      /*! \brief Get the image at the specified index.
       *  \param index Index of the image to get.
       *  \return The image at the specified index in the MapArray.
       *  \note The behavior is undefined, if \a index is larger than or equal to the number of
       *  images in this MapArray.
       *  \sa addMap, getNumberOfMaps, insertMap, removeMap, replaceMap */
      NVSG_API const TextureHostSharedPtr & getMap( unsigned int index ) const;

      /*! \brief Get the index of a image in this MapArray.
       *  \param img A pointer to the image to get index for.
       *  \return The index of \a img in this MapArray, or (-1) if \a img is not part of this
       *  MapArray.
       *  \sa getMap, pickMap */
      NVSG_API unsigned int getIndexOfMap( const TextureHostSharedPtr & img ) const;

      /*! \brief Add an image to the end of the MapArray.
       *  \param img A pointer to the image to add.
       *  \remarks The reference count of \a img is incremented.
       *  \sa getMap, getNumberOfMaps, insertMap, replaceMap, removeMap */
      NVSG_API void addMap( const TextureHostSharedPtr & img );

      /*! \brief Insert an image in front of the specified index.
       *  \param index Index to insert an image before.
       *  \param img A pointer to the image to insert.
       *  \remarks The reference count of \a img is incremented.
       *  \note The behavior is undefined if \a index is larger than or equal to the number of
       *  images in this MapArray.
       *  \sa addMap, getMap, getNumberOfMaps, replaceMap, removeMap */
      NVSG_API void insertMap( unsigned int index, const TextureHostSharedPtr & img );

      /*! \brief Replace the image at the specified index.
       *  \param index Index to replace the image at.
       *  \param img A pointer to the image to replace with.
       *  \remarks The reference count of \a img is incremented, and the reference count of the
       *  image previously at index \a index is decremented.
       *  \note The behavior is undefined if \a index is larger than or equal to the number of
       *  Map objects in this MapArray.
       *  \sa addMap, getMap, getNumberOfMaps, insertMap, removeMap */
      NVSG_API void replaceMap( unsigned int index, const TextureHostSharedPtr & img );

      /*! \brief Remove the Map at the specified index.
       *  \param index Index to remove the Map at.
       *  \remarks The reference count of the Map at index \a index is decremented.
       *  \note The behavior is undefined if \a index is larger than or equal to the number of
       *  Map objects in this MapArray.
       *  \sa addMap, getMap, getNumberOfMaps, insertMap, replaceMap */
      NVSG_API void removeMap( unsigned int index );

      /*! \brief Clear the MapArray
       *  \remarks The reference count of every Map in this MapArray is decremented, and the
       *  array is cleared.
       *  \sa addMap, getNumberOfMaps, removeMap */
      NVSG_API void clear( void );

      /*! \brief Get the maximal width and height of the Map objects.
       *  \param width Reference to the maximal width, in pixels, to get.
       *  \param height Reference to the maximal height, in pixels, to get.
       *  \remarks For a horizontally oriented MapArray, \a height is used as the overall height;
       *  for a vertically oriented MapArray, \a width is used as the overall width.
       *  \sa getOrientation, getSize*/
      NVSG_API void getEntrySize( unsigned int &width
                                , unsigned int &height ) const;

      /*! \brief Get the overall size of the MapArray.
       *  \param width Reference to the overall width, in pixels, to get.
       *  \param height Reference to the overall height, in pixels, to get.
       *  \remarks The overall size of a MapArray is the size, in pixels, that the complete MapArray
       *  has.
       *  \par Example
       *  For a horizontally oriented MapArray, \a height is the same as what you get with
       *  getEntrySize(), while \a width is the sum of the widths of every Map in the MapArray,
       *  plus the spacing in between them.\n
       *  For a vertically oriented MapArray, \a width is the same as what you get with getEntrySize(),
       *  while \a height is the sum of the heights of every Map in the MapArray, plus the spacing
       *  in between them.
       *  \sa getEntrySize, getOrientation */
      NVSG_API virtual void getSize( unsigned int &width, unsigned int &height ) const;

      /*! \brief Get the image being at the specified raster position.
       *  \param x Horizontal raster coordinate to get Map at.
       *  \param y Vertical raster coordinate to get Map at.
       *  \return The TextureHost specifying the image at the specified raster position.
       *  \remarks This function determines the image of this MapArray at the specified raster
       *  position (\a x, \a y), if any. It takes into account the position, alignment, direction,
       *  orientation, and spacing. For a horizontally oriented MapArray, the images are considered
       *  to be of equal height; for a vertically oriented MapArray, the images are considered to be
       *  of equal width. If the position is in the spacing between two images, or outside of the
       *  overall size of the MapArray, NULL is returned.
       *  \sa getEntrySize, getSize, getMap */
      NVSG_API virtual const TextureHostSharedPtr & pickMap( int x, int y ) const;

      REFLECTION_INFO_API( NVSG_API, MapArray );
    protected:
      friend struct nvutil::Holder<MapArray>;

      /*! \brief Default-constructs a MapArray.
       *  \remarks The MapArray initially is aligned along the lower base line (MA_LOWER),
       *  is vertical oriented (MO_VERTICAL), has a spacing of 1 pixel, and will be traversed 
       *  from low index to high index (MTD_LOW_TO_HIGH).*/
      NVSG_API MapArray();

      /*! \brief Copy-constructs a MapArray from another MapArray. */
      NVSG_API MapArray( const MapArray &rhs );

      /*! \brief Destructs a MapArray. */
      NVSG_API virtual ~MapArray();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      typedef std::vector<TextureHostSharedPtr>  TextureHostContainer;

    private:
      MapAlignment          m_alignment;
      MapTraversalDirection m_direction;
      MapOrientation        m_orientation;
      TextureHostContainer m_maps;
      unsigned int          m_spacing;
  };

  inline MapAlignment MapArray::getAlignment() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_alignment );
  }

  inline void MapArray::setAlignment( MapAlignment alignment )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_alignment != alignment )
    {
      m_alignment = alignment;
      notifyChange( this );
    }
  }

  inline MapTraversalDirection MapArray::getTraversalDirection() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_direction );
  }

  inline void MapArray::setTraversalDirection( MapTraversalDirection direction )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_direction != direction )
    {
      m_direction = direction;
      notifyChange( this );
    }
  }

  inline MapOrientation MapArray::getOrientation() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_orientation );
  }

  inline void MapArray::setOrientation( MapOrientation orientation )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_orientation != orientation )
    {
      m_orientation = orientation;
      notifyChange( this );
    }
  }

  inline unsigned int MapArray::getSpacing() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_spacing );
  }

  inline void MapArray::setSpacing( unsigned int spacing )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    if ( m_spacing != spacing )
    {
      m_spacing = spacing;
      notifyChange( this );
    }
  }

  inline void MapArray::addMap( const TextureHostSharedPtr & img )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    m_maps.push_back( img );
    notifyChange( this );
  }

  inline void MapArray::insertMap( unsigned int index, const TextureHostSharedPtr & img )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( img && ( index < m_maps.size() ) );
    m_maps.insert( m_maps.begin() + index, img );
    notifyChange( this );
  }

  inline void MapArray::replaceMap( unsigned int index, const TextureHostSharedPtr & img )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( img && ( index < m_maps.size() ) );
    if ( m_maps[index] != img )
    {
      m_maps[index] = img;
      notifyChange( this );
    }
  }

  inline void MapArray::removeMap( unsigned int index )
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_WRITE_LOCKED(this);
    NVSG_ASSERT( index < m_maps.size() );
    m_maps.erase( m_maps.begin() + index );
    notifyChange( this );
  }

  inline unsigned int MapArray::getNumberOfMaps() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( checked_cast<unsigned int>(m_maps.size()) );
  }

  inline const TextureHostSharedPtr & MapArray::getMap( unsigned int index ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    NVSG_ASSERT( index < m_maps.size() );
    return( m_maps[index] );
  }

  inline unsigned int MapArray::getIndexOfMap( const TextureHostSharedPtr & img ) const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    TextureHostContainer::const_iterator it = std::find( m_maps.begin(), m_maps.end(), img );
    return( ( it != m_maps.end() ) ? checked_cast<unsigned int>(it - m_maps.begin()) : ~0 );
  }
}

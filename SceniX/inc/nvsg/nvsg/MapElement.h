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
#include "nvutil/HashGenerator.h"

namespace nvsg
{
  /*! \brief Class to hold a single Map.
   *  \par Namespace: nvsg
   *  \remarks This class is used to add a single Map to a MapList.
   *  \sa Map, MapArray, MapList, MapMatrix */
  class MapElement : public MapObject
  {
    public:
      NVSG_API static MapElementSharedPtr create();

    public:
      /*! \brief Get a pointer to the image.
       *  \return A pointer to the image of this MapElement.
       *  \sa pickMap */
      NVSG_API const TextureHostSharedPtr & getMap() const;

      /*! \brief Get the image being at the specified raster position.
       *  \param x Horizontal raster coordinate to get the image at.
       *  \param y Vertical raster coordinate to get the image at.
       *  \return A pointer to the image specified for the specified raster position, or NULL if no
       *  image was found at that position.
       *  \remarks This functions determines if the image of this MapElement is at the specified
       *  raster position (\a x, \a y). If it is, a pointer to the image is returned, otherwise NULL.
       *  \sa getMap */
      NVSG_API virtual const TextureHostSharedPtr & pickMap( int x, int y ) const;

      /*! \brief Set the image of the MapElement.
       *  \param img A pointer to a image to be used by this MapElement. */
      NVSG_API void setMap( const TextureHostSharedPtr & img );

      /*! \brief Get the overall size of the MapElement.
       *  \param width Reference to the overall width, in pixels, to get.
       *  \param height Reference to the overall height, in pixels, to get.
       *  \remarks The overall size of the MapElement is the size, in pixels, of it's Map. */
      NVSG_API virtual void getSize( unsigned int &width, unsigned int &height ) const;

      REFLECTION_INFO_API( NVSG_API, MapElement );
    protected:
      friend struct nvutil::Holder<MapElement>;

      /*! \brief Default constructor of a MapElement.
       *  \remarks The default constructor creates an empty MapElement without associated Map.
       *  To set the Map use setMap(). */
      NVSG_API MapElement();

      /*! \brief Copy-constructs a MapElement.
       *  \param rhs Source MapElement. */
      NVSG_API MapElement( const MapElement &rhs );

      /*! \brief Destructs a MapElement. */
      NVSG_API virtual ~MapElement();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;
    private:
      TextureHostSharedPtr m_map;
  };

  inline const TextureHostSharedPtr & MapElement::getMap() const
  {
    NVSG_TRACE();
    NVSG_ASSERT_OBJECT_READ_LOCKED(this);
    return( m_map );
  }
}

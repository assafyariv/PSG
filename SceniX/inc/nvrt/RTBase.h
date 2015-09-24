// Copyright NVIDIA Corporation 2002-2008
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
#include <nvutil/HandledObject.h>

namespace nvrt
{
  /*! \brief Enumeration to identify the RayTracing object type.
   *  \par Namespace: nvrt
   *  \remarks RTType identifies each type of RayTracing object. That way, the type of an RTBase can
   *  be queried, to determine which class type in the class hierarchy that RTBase really is. */
  typedef enum
  {
    RT_TYPE_ACCELERATION,
    RT_TYPE_BUFFER,
    RT_TYPE_CONTEXT,
    RT_TYPE_GEOMETRY,
    RT_TYPE_GEOMETRY_GROUP,
    RT_TYPE_GEOMETRY_INSTANCE,
    RT_TYPE_GROUP,
    RT_TYPE_MATERIAL,
    RT_TYPE_PROGRAM,
    RT_TYPE_SELECTOR,
    RT_TYPE_TEXTURE_SAMPLER,
    RT_TYPE_TRANSFORM
  } RTType;

  /*! \brief Pure virtual base class of all RayTracing objects
   *  \par Namespace: nvrt
   *  \remarks RTBase is the purve virtual base class of all RayTracing classes. These are wrappers
   *  around the NVIRT ray tracing functions.\n
   *  This class provides the owner handling for its derivatives, and specifies the pure virtual
   *  interface to get the type of the actual object.
   *  \sa nvutil::HandledObject, nvrt::RTContext  */
  class RTBase : public nvutil::HandledObject
  {
    friend class RTContext;

    public :
      /*! \brief Default-constructor of RTBase */
      NVRT_API RTBase();

      /*! \brief Destructor of RTBase */
      NVRT_API ~RTBase();

      /*! \brief Add an Owner to this object.
       *  \param owner A pointer to the handle of the additional owner of this object.
       *  \remarks The owners are used to carry dirty states up the object hierarchy.
       *  \sa getNumberOfOwners, getOwner, removeOwner, markOwnersDirty */
      NVRT_API void addOwner( const RTBaseWeakPtr & owner );

      /*! \brief Add Get the number of owners of this object.
       *  \return The number of owners of this object.
       *  \remarks The owners are used to carry dirty states up the object hierarchy.
       *  \sa addOwner, getOwner, removeOwner, markOwnersDirty */
      NVRT_API unsigned int getNumberOfOwners() const;

      /*! \brief Get the owner of this object, specified by an index.
       *  \return A pointer to the specified owner handle.
       *  \remarks The owners are used to carry dirty states up the object hierarchy.
       *  \note The behavior is undefined if \a index is greater or equal to the number of owners of
       *  this object.
       *  \sa addOwner, getNumberOfOwners, removeOwner, markOwnersDirty */
      NVRT_API RTBaseWeakPtr getOwner( unsigned int index ) const;

      /*! \brief Remove the specified owner from this object.
       *  \param A pointer to the handle of the owner to remove from this object.
       *  \remarks The owners are used to carry dirty states up the object hierarchy.
       *  \note The behavior is undefined if \a owner is not an owner of this object.
       *  \sa addOwner, getNumberOfOwners, getOwner, markOwnersDirty */
      NVRT_API void removeOwner( const RTBaseWeakPtr & owner );

      /*! \brief Recursively mark all owners of this object as dirty.
       *  \remarks If an object in the hierarchy changes, that object, as well as all its owners
       *  recursively have to be marked as dirty, in order to correctly handle the changed object.
       *  \sa addOwner, getNumberOfOwners, getOwner, removeOwner */
      NVRT_API void markOwnersDirty();

      /*! \brief Interface to get the actual type of this object.
       *  \return An enumeration value specifying the actual type of this object.
       *  \remarks All concrete derivatives of RTBase has to overwrite this interface and return
       *  the proper enumeration value. */
      NVRT_API virtual RTType getType() const = 0;

      /*! \brief Interface to check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const = 0;

    private:
      std::vector<RTBaseWeakPtr>  m_owners;    // NOTE: no refCount on owners to prevent circular refs
  };
}


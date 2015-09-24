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

#include "optix.h"
#include "nvsgrtapi.h"
#include "nvrt/RTBase.h"
#include "nvutil/Incarnation.h"

namespace nvrt
{
  /*! \brief Wrapper class for a nvirt RTacceleration object.
   *  \par Namespace: nvrt
   *  \remarks An RTAcceleration represents a ray tracing acceleration structure. It can be created
   *  by using RTContext::newAcceleration(). An RTAcceleration needs to be attached to RTGroups
   *  and RTGeometryGroups.
   *  \sa nvrt::RTBase, nvrt::RTContext, nvrt::RTGroup, nvrt::RTGeometryGroup  */
  class RTAcceleration : public RTBase
  {
    friend class RTContext;         // A RTContext calls init
    friend class RTAccelerationManager;

    public:
      /*! \brief Enumeration to specify the builder of an acceleration structure. */
      typedef enum
      {
        RTBT_UNINITIALIZED, //!< default, uninitialized state
        RTBT_NONE,        //!< No builder: simple list of primitives. Valid traverser types: RTTT_NONE.
        RTBT_BVH_LOW,     //!< Low quality builder: simple bvh, built very quickly on the GPU; useful for dynamic content. Valid trverser types: RTTT_BVH_STANDARD, RTTT_BVH_OPTIMIZED
        RTBT_BVH_MEDIUM,  //!< Medium quality builder: medium quality bvh with quick performance on the CPU. Valid traverser types: RTTT_BVH_STANDARD, RTTT_BVH_OPTIMIZED
        RTBT_BVH_HIGH,    //!< High quality builder: build on the CPU. Valid traverser types: RTTT_BVH_STANDARD, RTTT_BVH_OPTIMIZED
        RTBT_BVH_MAXIMUM, //!< Maximum quality builder: slower build speed and slightly higher memory footpring than RTBT_BVH_HIGHT. Valid traverser types: RTTT_BVH_STANDARD, RTTT_BVH_OPTIMIZED
        RTBT_KDT_HIGH     //!< High quality builder for triangle geometry only: Valid traverser types: RTTT_KDT_STANDARD
      } RTBuilderType;

      /*!  brief Enumeration to specify the traverser of an acceleration structure. */
      typedef enum
      {
        RTTT_UNINITIALIZED, //!< default, uninitialized state
        RTTT_NONE,          //!< No traverser: linearly loops through the list of primitives. Valid builder types: RTBT_NONE.
        RTTT_BVH_STANDARD,  //!< Standard traversal for bounding volume hierarchies. Valid builder types: RTBT_BVH_LOW, RTBT_BVH_MEDIUM, RTBT_BVH_HIGH, RTBT_BVH_MAXIMUM.
        RTTT_BVH_OPTIMIZED, //!< Optimized traversal for bounding volume hierarchies. Valid builder types: RTBT_BVH_LOW, RTBT_BVH_MEDIUM, RTBT_BVH_HIGH, RTBT_BVH_MAXIMUM.
        RTTT_KDT_STANDARD   //!< Standard traversal for kd-trees. Valid builder types: RTBG_KDT_HIGH.
      } RTTraverserType;

    public:
      NVRT_API static RTAccelerationSharedPtr create();

    public:
      /*! \brief Default-constructor of RTAcceleration */
      NVRT_API RTAcceleration();

      /*! \brief Destructor of RTAcceleration */
      NVRT_API virtual ~RTAcceleration();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_ACCELERATION. */
      NVRT_API virtual RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Specify the methods to use to construct and traverse the acceleration structure.
       *  \param builder An enumeration value specifying the builder type.
       *  \param traverser An enumeration value specifying the traverser type.
       *  \return \c true, if \a builder and \a traverser are compatible, otherwise \c false.
       *  \remarks You can not mix each RTBuilderType with each RTTraverserType. The allowed
       *  combinations are:\n
       *    RTBT_NONE: RTTT_NONE\n
       *    RTBT_BVH_LOW; RTBT_BVH_MEDIUM, RTBT_BVH_HIGH, RTBT_BVH_MAXIMUM: RTTT_BVH_STANDARD, RTTT_BVH_OPTIMIZED\n
       *    RTBT_KDT_HIGH : RTTT_KDT_STANDARD
       *  \sa getBuilderType, getTraverserType */
      NVRT_API bool setTypes( RTBuilderType builder, RTTraverserType traverser );

      /*! \brief Get the builder type of this RTAcceleration.
       *  \return The current builder type of this RTAcceleration
       *  \sa setTypes, getTraverserType */
      NVRT_API RTBuilderType getBuilderType() const;

      /*! \brief Get the traverser type of this RTAcceleration
       *  \return The current traverser type of this RTAcceleration
       *  \sa setTypes, getBuilderType */
      NVRT_API RTTraverserType getTraverserType() const;

      /*! \brief Set a named property value.
       *  \param name The name of the property.
       *  \param value The value of the property.
       *  \remarks Properties can be used to fine tune the way an acceleration structure is built,
       *  in order to achieve faster build times or better ray tracing performance. Properties are
       *  evaluated and applied by the acceleration structure only during build time, and different
       *  builders recognize different properties. Setting a property will never fail. Properties
       *  that are not recognized by an acceleration structure will be ignored.\n
       *  Currently, there are just two properties, that are used by the builders RTBT_BVH_LOW,
       *  RTBT_BVH_MEDIUM, RTBT_BVH_HIGH, and RTBT_BVH_MAXIMUM:\n
       *    "leaf_size", which specifies a target value for the preferred number of primitives in a
       *  leaf node and defaults to 1.\n
       *    "max_tree_depth", which specifies the maximum tree depth of the BVH tree and defaults to
       *  64.
       *  \sa getPropertyValue, getNumberOfProperties, getPropertyName, getPropertyValue */
      NVRT_API void setProperty( const std::string & name, const std::string & value );

      /*! \brief Get the value of the specified property
       *  \param name The name of the property to get the value for.
       *  \return The value of the property \a name, if it is set, otherwise an empty string.
       *  \sa setProperty, getNumberOfProperties, getPropertyName, getPropertyValue */
      NVRT_API std::string getPropertyValue( const std::string & name ) const;

#if 0
      // These functions are not really possible, due to the very nature of setData (below)!
      // setData also sets the properties, and we can't get that information back.

      /*! \brief Get the number of properties of this RTAcceleration.
       *  \return The number of properties of this RTAcceleration.
       *  \sa setProperty, getPropertyValue, getPropertyName, getPropertyValue */
      NVRT_API unsigned int getNumberOfProperties() const;

      /*! \brief Get the name of the specified property.
       *  \param index The index of the property to get the name of.
       *  \return The name of the specified property.
       *  \note The behavior is undefined, if index is larger or equal to the number of properties
       *  of this RTAcceleration.
       *  \sa setProperty, getNumberOfProperties, getNumberOfProperties, getPropertyValue */
      NVRT_API const std::string & getPropertyName( unsigned int index ) const;

      /*! \brief Get the value of the specified property.
       *  \param index The index of the property to get the value of.
       *  \return The value of the specified property.
       *  \note The behavior is undefined, if index is larger or equal to the number of properties
       *  of this RTAcceleration.
       *  \sa setProperty, getNumberOfProperties, getNumberOfProperties, getPropertyName */
      NVRT_API const std::string & getPropertyValue( unsigned int index ) const;
#endif

      /*! \brief Set the full state of the acceleration object.
       *  \param data A vector of bytes, that contains the complete state of the acceleration object.
       *  \return \c true, if setting the data was successful, otherwise \c false.
       *  \remark The \a data contains the full state of an acceleration object, including builder,
       *  traverser, and properties. The \a data must be unaltered memory previously retrieved from
       *  a (potentially different) RTAcceleration. This mechanism is useful for implementing
       *  caching mechanisms, especially when using high quality structures which are expensive to
       *  build.
       *  \note No check is performed on whether the contents of \a data match the actual underlying
       *  geometry on which the acceleration structure is used. If the children of associated groups
       *  or geometry groups differ in number of children, layout of bounding boxes, or geometry,
       *  then behavior after this call is undefined.
       *  \sa getDataSize, getData */
      NVRT_API bool setData( const std::vector<char> & data );

      /*! \brief Get the size of the data to be retrieved from this acceleration structure.
       *  \return The size of the data in bytes.
       *  \sa setData, getData */
      NVRT_API size_t getDataSize() const;

      /*! \brief The the acceleration structure data.
       *  \param data Reference to a vector of bytes, to hold the acceleration structure data.
       *  \remarks \a data is resized appropriately.
       *  \sa setData, getDataSize */
      NVRT_API void getData( std::vector<char> & data ) const;

      /*! \brief Mark this acceleration structure as dirty.
       *  \remarks An acceleration structure which is marked dirty will be rebuilt on a call to
       *  RTContext::launch(), and its dirty flag will be reset. The dirty flag will also be reset on
       *  a sucessful call to setData.\n
       *  An acceleration structure which is not marked dirty will never be rebuilt, even if
       *  associated groups, geometry, properties, or any other values have changed.\n
       *  Initially after creation, acceleration structures are marked dirty.
       *  \sa isDirty */
      NVRT_API void markDirty();

      /*! \brief Check if this acceleration structure is marked as dirty
       *  \return \c true, if this acceleration structure is marked as dirty, otherwise \c false.
       *  \remarks An acceleration structure which is marked dirty will be rebuilt on a call to
       *  RTContext::launch(), and its dirty flag will be reset. The dirty flag will also be reset on
       *  a sucessful call to setData.\n
       *  An acceleration structure which is not marked dirty will never be rebuilt, even if
       *  associated groups, geometry, properties, or any other values have changed.\n
       *  Initially after creation, acceleration structures are marked dirty.
       *  \sa markDirty */
      NVRT_API bool isDirty() const;

      /*! \brief Get a human readable string for a builder.
       *  \param builder The RTBuilderType for which the name should be returned.
       *  \return A string with the builder type.
       */
      static NVRT_API const char * getBuilderString( RTBuilderType builder );

      /*! \brief Get a human readable string for a traverser.
       *  \param builder The RTTraverserType for which the name should be returned.
       *  \return A string with the builder type.
       */
      static NVRT_API const char * getTraverserString( RTTraverserType traverser );

      /*! \brief Convert a human readable string into a builder type.
       *  \param builder The builder type which should be converted to a RTBuilderType.
       *  \return The RTBuilderType value for the given string.
       */
      static NVRT_API RTBuilderType stringToBuilderType( const char * builder );

      /*! \brief Convert a human readable string into a traverser type.
       *  \param builder The builder type which should be converted to a RTTraverserType.
       *  \return The RTTraverserType value for the given string.
       */
      static NVRT_API RTTraverserType stringToTraverserType( const char * traverser );
    protected :
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the acceleration structure with.
       *  \remarks This function is called from RTContext::newAcceleration to initialize the new
       *  acceleration structure.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTAcceleration wraps the functionality of an nvirt::RTacceleration. This
       *  function is used internally for accessing that resource. */
      NVRT_API RTacceleration getResource() const;

    private:
      NVRT_API bool checkCompatibility( RTBuilderType builder, RTTraverserType traverser ) const;
      NVRT_API RTBuilderType doGetBuilderType() const;
      NVRT_API RTTraverserType doGetTraverserType() const;
#if 0
      // see comment above !!
      NVRT_API void getPropertyIterators() const;
#endif

    private:
      RTacceleration m_acceleration;
      RTBuilderType m_builderType;
      RTTraverserType m_traverserType;

#if 0
      // see comment above !!
      std::map<std::string,std::string> m_properties;

      mutable std::vector<std::map<std::string,std::string>::const_iterator> m_propertyIterators;
#endif
  };
}


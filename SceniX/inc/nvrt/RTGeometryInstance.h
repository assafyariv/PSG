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
#include "nvrt/RTBase.h"
#include "nvrt/RTChildrenManager.h"
#include "nvrt/RTVariablesManager.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Wrapper class for an nvirt RTgeometryinstance object.
   *  \par Namespace: nvrt
   *  \remarks An RTGeometryInstance represents the combination of an instance of a Geometry and an
   *  associated set of Material descriptions.
   *  \sa RTBase, RTVariablesManager, RTGeometry, RTMaterial */
  class RTGeometryInstance : public RTBase, public RTVariablesManager
  {
    friend class RTContext;
    friend class RTChildrenManager<RTGeometryInstance>;

    public:
      NVRT_API static RTGeometryInstanceSharedPtr create();

    public:
      /*! \brief Default-constructor of RTGeometryInstance */
      NVRT_API RTGeometryInstance();

      /*! \brief Destructor of RTGeometryInstance */
      NVRT_API virtual ~RTGeometryInstance();

      /*! \brief Get the actual type of the object.
       *  \return The enumeration value RT_TYPE_GEOMETRY_INSTANCE. */
      NVRT_API RTType getType() const;

      /*! \brief Check if this object is valid.
       *  \return \c true, if this object is valid, \c false otherwise. */
      NVRT_API virtual bool isValid() const;

      /*! \brief Set geometry for this geometry instance.
       *  \param geometry The geometry to set.
       *  \sa getGeometry */
      NVRT_API void setGeometry( const RTGeometryWeakPtr & geometry );

      /*! \brief Get the geometry of this geometry instance.
       *  \return The geometry of this geometry instance.
       *  \sa setGeometry */
      NVRT_API RTGeometryWeakPtr getGeometry() const;

      /*! \brief Get the number of materials in this geometry instance.
       *  \return The number of materials in this geometry instance.
       *  \sa getMaterial, addMaterial, replaceMaterial, removeMaterial */
      NVRT_API unsigned int getNumberOfMaterials() const;

      /*! \brief Get the specified material from this geometry instance.
       *  \param index The index of the material to get.
       *  \return A pointer to the specified material.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of materials
       *  in this geometry instance.
       *  \sa getNumberOfMaterials, addMaterial, replaceMaterial, removeMaterial */
      NVRT_API RTMaterialWeakPtr getMaterial( unsigned int index ) const;

      /*! \brief Add a material to this geometry instance.
       *  \param material A pointer to the material to add.
       *  \return The index of the newly added material in this geometry instance.
       *  \note The behavior is undefined, if this same material already is part of his geometry
       *  instance.
       *  \sa getNumberOfMaterials, getMaterial, replaceMaterial, removeMaterial */
      NVRT_API unsigned int addMaterial( const RTMaterialWeakPtr & material );

      /*! \brief Replace a specified material in this geometry instance.
       *  \param index The index of the material to replace.
       *  \param material A pointer to the material used to replace.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of materials
       *  in this geometry instance.
       *  \sa getNumberOfMaterials, getMaterial, addMaterial, removeMaterial */
      NVRT_API bool replaceMaterial( unsigned int index, const RTMaterialWeakPtr & material );

      /*! \brief Remove a specified material from this geometry instance.
       *  \param index The index of the material to remove.
       *  \note The behavior is undefined, if \a index is larger or equal to the number of materials
       *  in this geometry instance.
       *  \sa getNumberOfMaterials, getMaterial, addMaterial, replaceMaterial */
      NVRT_API void removeMaterial( unsigned int index );

    protected:
      /*! \brief Initialize this object.
       *  \param context The RTContext to create the geometry instance with.
       *  \remarks This function is called from RTContext::newGeometryInstance to initialize the new
       *  geometry instance.
       *  \sa nvrt::RTContext */
      NVRT_API void init( RTcontext context );

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTGeometryInstance wraps the functionality of an nvirt::RTgeometryinstance.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTgeometryinstance getResource() const;

      /*! \brief Declare a variable.
       *  \param name The name of the variable to declare.
       *  \return The newly declared variable.
       *  \remarks This function is called by the framework to declare a variable in this
       *  RTGeometryInstance.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doDeclareVariable( const std::string & name );

      /*! \brief Remove a variable.
       *  \param v The variable to remove.
       *  \remarks This function is called by the framework to remove a variable in this
       *  RTGeometryInstance.
       *  \sa RTVariablesManager */
      NVRT_API void doRemoveVariable( RTvariable v );

      /*! \brief Query a variable.
       *  \param name The name of the variable to query.
       *  \return The queried variable.
       *  \remarks This function is called by the framework to query in this RTGeometryInstance.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doQueryVariable( const std::string & name ) const;

      /*! \brief Get the number of variables in this RTGeometryInstance.
       *  \return The number of variables in this RTGeometryInstance.
       *  \remarks This function is called by the framework to get the number of variables in this
       *  RTGeometryInstance.
       *  \sa RTVariablesManager */
      NVRT_API unsigned int doGetVariableCount() const;

      /*! \brief Get a specified variable from this RTGeometryInstance.
       *  \param index The index of the variable to get.
       *  \return The specified variable.
       *  \remarks This function is called by the framework to get a specific variable in this
       *  RTGeometryInstance.
       *  \sa RTVariablesManager */
      NVRT_API RTvariable doGetVariable( unsigned int index ) const;

    private:
#if !defined(NDEBUG)
      bool checkGeometry() const;
#endif

    private:
      RTGeometrySharedPtr               m_geometry;
      RTgeometryinstance                  m_geometryInstance;
      std::vector<RTMaterialSharedPtr>  m_materials;
  };
}


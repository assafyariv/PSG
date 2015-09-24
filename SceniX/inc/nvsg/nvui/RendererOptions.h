// Copyright NVIDIA Corporation 2010-2011
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

#include <nvsg/CoreTypes.h>
#include <nvutil/HandledObject.h>
#include <nvutil/Reflection.h>

namespace nvui
{
  /*! \brief   This class is a container for RendererOptions.
      \remarks Use SceneRenderer*::addRendererOptions( const RendererOptionsSharedPtr &options ) to initialize the
               options for a specific SceneRenderer. It is possible to store the options for multiple renderers at
               the same time. All options of a SceneRenderer* should be prefix with the name of the SceneRenderer
               to ensure that no collisions between options of different Renderers occur.
               The properties and values should be stored by savers if possible. It is possible to add custom attributes
               which contain information like last active renderer. Those attributes can be evaluated by the application
               to initialize a Window.
  !*/
  class RendererOptions : public nvutil::HandledObject
  {
  public:
    NVSG_API static nvui::RendererOptionsSharedPtr create();

    /*! \brief Add a new property.
        \param name Name of the property.
        \param value Initial value of the property.
        \remarks Behaviour when adding two properties with the same name is undefined.
    !*/
    template <typename ValueType> void addProperty( const std::string &name, const std::string &annonation, const ValueType &value );

    /* \brief Get the value of a property.
       \param name Name of the property to get the value for.
       \param annotation An arbitrary annotation string.
       \returns Vale of the given property name.
       \remarks This is a shortcut for getValue(getProperty(name));
    !*/
    template <typename ValueType> ValueType getValue( const std::string &name ) const;

    /*! \brief Set the value of a property.
        \param name Name of the property to set the value for.
        \param value Value to set for the given property name.
        \remarks This is a shortcut for setValue(getProperty(name), value);.
    !*/
    template <typename ValueType> void setValue( const std::string &name, const ValueType &value );

    /*! \brief Get the annotation of this property. 
        \param name Name of the property to get the annotation for.
        \return A string with the annotation of this property.
    !*/
    NVSG_API std::string getAnnotation( const std::string &name) const;

    using Reflection::getValue;
    using Reflection::setValue;
  public:
    RendererOptions();
    RendererOptions( const RendererOptions &rhs );
    virtual ~RendererOptions();

    REFLECTION_INFO_API( NVSG_API, RendererOptions );
    BEGIN_DECLARE_STATIC_PROPERTIES
    END_DECLARE_STATIC_PROPERTIES
  protected:

  private:
    nvutil::PropertyListImpl *m_dynamicProperties;
  };

  /*! \brief Base class for classes that need RendererOptions.
   *  \remarks Use this class as base class for every class that needs to keep RendererOptions.
  !*/
  class RendererOptionsProvider
  {
  public:
    RendererOptionsProvider() : m_rendererOptions( RendererOptions::create() )  {}
    virtual ~RendererOptionsProvider()                                          {}

    //! Add all renderer options required to the given rendererOptions object.
    NVSG_API virtual void addRendererOptions( const RendererOptionsSharedPtr& rendererOptions );

    //! Set this renderOptions object as current render options. 
    NVSG_API virtual void setRendererOptions( const RendererOptionsSharedPtr& rendererOptions );

  protected:
    //! Overwrite this function if you need to add options
    NVSG_API virtual void doAddRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions ) = 0;

    //! Overwrite this function if you need to re-validate property ids on rendererOptions change
    NVSG_API virtual void doSetRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions ) = 0;

  protected:
    RendererOptionsSharedPtr m_rendererOptions;
  };

  template <typename ValueType> void RendererOptions::addProperty( const std::string &name, const std::string &annotation, const ValueType &value )
  {
    NVSG_ASSERT(!getProperty(name) && "Property has already been added");
    m_dynamicProperties->addProperty( name, new nvutil::TypedPropertyValue<ValueType>( nvutil::Property::SEMANTIC_VALUE, annotation, true, value ) );
  }

  template <typename ValueType> ValueType RendererOptions::getValue( const std::string &name ) const
  {
    NVSG_ASSERT( getProperty(name) && "Property is not available" );
    return Reflection::getValue<ValueType>( getProperty( name ) );
  }

  template <typename ValueType> void RendererOptions::setValue( const std::string &name, const ValueType &value )
  {
    NVSG_ASSERT( getProperty(name) && "Property is not available" );
    return Reflection::setValue<ValueType>( getProperty( name ), value );
  }

  inline void RendererOptionsProvider::addRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions )
  {
    if( rendererOptions )
    {
      doAddRendererOptions(rendererOptions);
    }
  }

  inline void RendererOptionsProvider::setRendererOptions( const nvui::RendererOptionsSharedPtr& rendererOptions )
  {
    if( rendererOptions && rendererOptions != m_rendererOptions )
    {
      // new renderer options, add needed options
      addRendererOptions( rendererOptions );

      // give derived classes the opportunity to update themselves
      doSetRendererOptions( rendererOptions );

      // update renderer options object
      m_rendererOptions = rendererOptions;
    }
  }

}

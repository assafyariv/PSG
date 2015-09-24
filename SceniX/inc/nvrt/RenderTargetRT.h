// Copyright NVIDIA Corporation 2010
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

#include "nvui/RenderTarget.h"

#include <nvrt/RTContext.h>
#include <nvrt/RTBuffer.h>
#include <nvrt/RTVariable.h>

#include <map>
#include <string>

namespace nvrt
{
  class RenderTargetRT : public nvui::RenderTarget
  {
  protected:
    NVRT_API RenderTargetRT( const nvrt::RTContextSharedPtr & rtContext );

  public:
    NVRT_API static nvutil::SmartPtr<RenderTargetRT> create( const nvrt::RTContextSharedPtr & rtContext );

    NVRT_API virtual ~RenderTargetRT();

    // RenderTarget interface
    NVRT_API virtual void setSize( unsigned int width, unsigned int height );
    NVRT_API virtual void getSize( unsigned int &width, unsigned int &height ) const;

    NVRT_API virtual nvsg::TextureHostSharedPtr getTextureHost( 
      nvsg::Image::PixelFormat pixelFormat = nvsg::Image::IMG_BGR,
      nvsg::Image::PixelDataType = nvsg::Image::IMG_UNSIGNED_BYTE );

    NVRT_API virtual bool isValid();

    // RenderTargetRT interface
    NVRT_API void setVariable( const std::string &name, const nvrt::RTBufferSharedPtr &buffer, bool manageSize = false, StereoTarget stereoTarget = LEFT ); // add an unmanaged buffer

    NVRT_API void setVariable( const std::string &name, int value, StereoTarget stereoTarget = LEFT );
    NVRT_API void setVariable( const std::string &name, unsigned int value, StereoTarget stereoTarget = LEFT );
    NVRT_API void setVariable( const std::string &name, float value, StereoTarget stereoTarget = LEFT );
    NVRT_API void setVariable( const std::string &name, RTTextureSamplerSharedPtr *value, StereoTarget stereoTarget = LEFT );
    
    template <int n, typename T> void setVariable( const std::string &name, const nvmath::Vecnt<n, T> &value, StereoTarget stereoTarget = LEFT );
    template <int n, typename T> void setVariable( const std::string &name, const nvmath::Matnnt<n, T> &value, StereoTarget stereoTarget = LEFT );

    /** Type can be any RT* which can be attached to a variable. Currently the following types support this feature.
      RTBuffer
      RTGeometryGroup
      RTGroup
      RTSelector
      RTTextureSampler
      RTTransform
    **/
    template <typename Type> void setVariable( const std::string &name, const typename nvutil::ObjectTraits<Type>::SharedPtr & value, StereoTarget stereoTarget = LEFT );

    NVRT_API void unsetVariable( const std::string &name, StereoTarget stereoTarget = LEFT ); // unset a variable

    NVRT_API nvrt::RTBufferSharedPtr getBuffer( const std::string &name, StereoTarget stereoTarget = LEFT ) const;

    // update buffer variables
    NVRT_API virtual bool beginRendering( const nvsg::ViewStateSharedPtr & scene );
    // currently nothing to do 
    NVRT_API virtual void endRendering( const nvsg::ViewStateSharedPtr & scene );

    // Stereo API
    NVRT_API virtual void setStereoEnabled(bool stereoEnabled);
    NVRT_API virtual bool isStereoEnabled() const;

    NVRT_API virtual bool setStereoTarget( StereoTarget target );
    NVRT_API virtual nvui::RenderTarget::StereoTarget getStereoTarget() const;


    NVRT_API nvrt::RTContextSharedPtr getRTContext() { return m_rtContext; }

    enum ColorFormat 
    { 
      BGRA_UNSIGNED_BYTE, 
      RGB_FLOAT, 
      RGBA_FLOAT 
    };

    NVRT_API virtual void setColorFormat( ColorFormat colorFormat );
    NVRT_API ColorFormat getColorFormat( ) const;

    /** Updates the given variable with a value **/
    class RTVariableSetter : public RCObject
    {
    public:
      NVRT_API virtual ~RTVariableSetter() {};
      NVRT_API virtual void set(RTVariable *) const {};
    };

    class TextureSamplerSetter : public RTVariableSetter
    {
      public:
        NVRT_API TextureSamplerSetter( const RTTextureSamplerSharedPtr &value );
        NVRT_API virtual ~TextureSamplerSetter() {};

        NVRT_API virtual void set(RTVariable *) const {};
      protected:
        RTTextureSamplerSharedPtr m_textureSampler;
    };

    template <int n, typename ValueType>
    class VectorSetter : public RTVariableSetter
    {
    public:
      VectorSetter( const nvmath::Vecnt<n, ValueType> &value );
      virtual ~VectorSetter() {};

      void set(RTVariable *variable) const;
    protected:
      nvmath::Vecnt<n, ValueType> m_value;
    };

    template <int n, typename ValueType>
    class MatrixSetter : public RTVariableSetter
    {
    public:
      MatrixSetter( const nvmath::Matnnt<n, ValueType> &value );
      virtual ~MatrixSetter() {};

      void set(RTVariable *variable) const;
    protected:
      nvmath::Matnnt<n, ValueType> m_value;
    };

    template <typename Type>
    class SharedPtrSetter : public RTVariableSetter
    {
    public:
      SharedPtrSetter( const typename nvutil::ObjectTraits<Type>::SharedPtr & value );
      virtual ~SharedPtrSetter() {};

      void set(RTVariable *variable) const;
    protected:
      typename nvutil::ObjectTraits<Type>::SharedPtr m_value;
    };

  protected:
    NVRT_API int getStereoTargetId( StereoTarget stereoTarget ) const; // 0 -> left eye/mono, 1 -> right eye, assert on LEFT_AND_RIGHT
    NVRT_API void updateVariables( StereoTarget stereoTarget ); //! Set the variables for the given StereoTarget

    template <typename VariableType> VariableType* getOrCreateVariable( const std::string &name );
    NVRT_API void setVariable( const std::string &name, const nvutil::SmartPtr< RTVariableSetter > &setter, StereoTarget stereoTarget ); // add an unmanaged buffer

  private:
    void decreaseManagedRefCount( const RTBufferSharedPtr &rtBuffer, StereoTarget stereoTarget );

  private:
    unsigned int m_width;
    unsigned int m_height;
    unsigned int m_entryPointIndex;

    nvrt::RTContextSharedPtr m_rtContext;

    typedef std::map< std::string, nvrt::RTBufferSharedPtr > BufferMap;
    typedef std::map< nvrt::RTBufferSharedPtr, int> BufferCount; // it is possible that a single buffer is managed multiple times. for this reason 

    BufferMap m_buffers[2];         //!< name->buffer map
    BufferCount m_managedBuffers[2];  //!< list of managed buffers

    typedef std::map< std::string, nvutil::SmartPtr< RTVariableSetter > > VariableMap;

    VariableMap m_variables[2];  //!< name -> setter for RTVariable

    // RTRenderArea compability interface until full separation of default programs has been done
    ColorFormat  m_colorFormat;

    StereoTarget m_stereoTarget;
    bool         m_stereoEnabled;
    bool         m_isRendering;
  };

  template <int n, typename T>
  RenderTargetRT::VectorSetter<n, T>::VectorSetter( const nvmath::Vecnt<n, T> &value )
    : m_value( value )
  {
  }

  template <int n, typename T>
  void RenderTargetRT::VectorSetter<n, T>::set(RTVariable *variable) const
  {
    NVSG_ASSERT( ( dynamic_cast<nvrt::RTVariableNT<n, T> *>(variable) ) );

    RTVariableNT<n, T> *var = static_cast<RTVariableNT<n, T> *>(variable);
    var->set( m_value.getPtr() );
  }

  template <int n, typename T>
  RenderTargetRT::MatrixSetter<n, T>::MatrixSetter( const nvmath::Matnnt<n, T> &value )
    : m_value( value )
  {
  }

  template <int n, typename ValueType>
  void RenderTargetRT::MatrixSetter<n, ValueType>::set(RTVariable *variable) const
  {
    NVSG_ASSERT( ( dynamic_cast<RTVariableNxMT<n, n, ValueType> *>(variable) ) );

    RTVariableNxMT<n, n, ValueType> *var = static_cast<RTVariableNxMT<n, n, ValueType> *>(variable);
    var->set( m_value.getPtr() );
  }

  template <typename ValueType>
  RenderTargetRT::SharedPtrSetter<ValueType>::SharedPtrSetter( const typename nvutil::ObjectTraits<ValueType>::SharedPtr & value )
    : m_value( value )
  {
  }

  template <typename Type>
  void RenderTargetRT::SharedPtrSetter<Type>::set( RTVariable *variable) const
  {
    NVSG_ASSERT( ( dynamic_cast<RTVariableType<Type> *>(variable) ) );

    RTVariableType<Type> *var = static_cast<RTVariableType<Type> *>(variable);
    var->set( m_value.get() );
  }

  template <typename VariableType>
  VariableType *RenderTargetRT::getOrCreateVariable( const std::string &name )
  {
    RTContextWriteLock context(m_rtContext);
    nvrt::RTVariable *currentVariable = context->getVariable( name );

    return currentVariable ? dynamic_cast<VariableType *>( currentVariable ) : context->newVariable<VariableType>( name );
  }

  template <int n, typename T>
  void RenderTargetRT::setVariable( const std::string &name, const nvmath::Vecnt<n, T> &value, StereoTarget stereoTarget )
  {
    getOrCreateVariable<nvrt::RTVariableNT<n, T> >( name );
    setVariable( name, nvutil::SmartPtr<RTVariableSetter>(new VectorSetter<n, T>(value)), stereoTarget );
  }

  template <int n, typename T>
  void RenderTargetRT::setVariable( const std::string &name, const nvmath::Matnnt<n, T> &value, StereoTarget stereoTarget )
  {
    getOrCreateVariable<nvrt::RTVariableNT<n, T> >( name );
    setVariable( name, nvutil::SmartPtr<RTVariableSetter>(new MatrixSetter<n, T>(value)), stereoTarget );
  }

  template <typename Type>
  void RenderTargetRT::setVariable( const std::string &name, const typename nvutil::ObjectTraits<Type>::SharedPtr & value, StereoTarget stereoTarget )
  {
    getOrCreateVariable<nvrt::RTVariableType<typename nvutil::ObjectTraits<Type>::Handle> >( name );
    setVariable( name, nvutil::SmartPtr<RTVariableSetter>(new SharedPtrSetter<Type>(value)), stereoTarget );
  }

  typedef nvutil::SmartPtr<RenderTargetRT> SmartRenderTargetRT;

} // namespace nvrt

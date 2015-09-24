// Copyright NVIDIA Corporation 2009-2011
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
/* @file */

#include "nvsg/OwnedObject.h"
#include "nvsg/TextureAttribute.h"
#include "nvutil/HashGenerator.h"
#include "nvutil/SmartPtr.h"
#include <map>

class RTFxParser;   // forward declaration of an internally used class.

namespace nvsg
{
  class RTFxBase;

  class RTFxParameter
  {
    friend class RTFxProgram;
    friend class RTFxBase;

    public:
      // enum value for parameter types besides the standard scalar values
      // note - must be named, or can't be used in checked_cast<>
      enum AdditionalTypes
      {
        PT_BUFFER           = 0x80,
        PT_OBJECT,
        PT_TEXTURE_SAMPLER,
        PT_USER_DATA, 
        PT_BOOL
      };

      struct Annotation
      {
        std::string type;
        std::string name;
        std::string value;
        NVSG_API bool operator==( const Annotation & rhs ) const;
      };

    public:
      NVSG_API RTFxParameter();
      /*! \brief Constructs a new parameter.
       *  \param name The parameter's name.
       *  \param type The parameter's type.  For a 'float3' type this would be NVSG_FLOAT.
       *  \param count The number of elements in the parameter or array.  For float3 this would be 3.
       *  \param size The parameter's size in bytes.  For a PT_USER_DATA parameter, this would be 
       *  sizeof( structure ) * count.  For other parameter types, it is ignored since the size can be
       *  determined from the data type and count.
       */
      NVSG_API RTFxParameter( const std::string & name, unsigned char type, unsigned char count, unsigned int size );
      NVSG_API RTFxParameter( const RTFxParameter & rhs );
      NVSG_API ~RTFxParameter();

      NVSG_API RTFxParameter & operator=( const RTFxParameter & rhs );
      NVSG_API bool operator==( const RTFxParameter & rhs ) const;

      NVSG_API const std::string & getName() const;
      /*! \brief Returns the data type */
      NVSG_API unsigned char getType() const;
      /*! \brief Returns the number of elements in the array.
       *  \note Since the RTFx parser only has access to the PTX declaring the parameter's total size in bytes, is not 
       *  possible to independently determine the count and size of the data structure for PT_USER_DATA types. 
       *  Therefore, all PT_USER_DATA typed parameters will have a count of 1. */
      NVSG_API unsigned char  getCount() const;
      /*! \brief Returns the total size of the data structure (sizeof(type)*count).  
       *  \note Since the RTFx parser only has access to the PTX declaring the parameter's total size in bytes, is not 
       *  possible to independently determine the count and size of the data structure for PT_USER_DATA types. 
       *  Therefore, all PT_USER_DATA typed parameters will have a size equal to the entire data structure array. */
      NVSG_API unsigned int  getSize() const;

      /*! \brief Gets a parameter's value using a void *.
       *  \param v A pointer to untyped memory, that is at least the size of the parameter.
       *  \remarks Gets a parameter's value.
       *  \note This method is typically used for parameters of type PT_USER_DATA.
       *  \sa getType, getSize, getName, setValue */
      NVSG_API void getValue( void * v ) const;

      /*! \brief Sets a parameter's value using a void *.
       *  \param v A pointer to untyped memory, that is at least the size of the parameter.
       *  \remarks Sets a parameter's value.
       *  \note This method is typically used for parameters of type PT_USER_DATA.
       *  \sa getType, getSize, getName, getValue */
      NVSG_API void setValue( const void * v );

      /*! \brief Gets a parameter's value using a char *.
       *  \param c A pointer to an array of char data, that is at least the size of the parameter.
       *  \remarks Gets a parameter's value.
       *  \note This method is typically used for parameters of type NVSG_BYTE or PT_BOOL.
       *  \note The behavior is undefined if the specified parameter is not a char or bool parameter.
       *  \sa getType, getSize, getName, setValue */
      NVSG_API void getValue( char * c ) const;

      /*! \brief Sets a parameter's value using a char *.
       *  \param c A pointer to an array of char data, that is at least the size of the parameter.
       *  \remarks Sets a parameter's value.
       *  \note This method is typically used for parameters of type NVSG_BYTE or PT_BOOL.
       *  \note The behavior is undefined if the specified parameter is not a char or bool parameter.
       *  \sa getType, getSize, getName, getValue */
      NVSG_API void setValue( const char * c );

      /*! \brief Gets a parameter's value using a unsigned char *.
       *  \param uc A pointer to an array of unsigned char data, that is at least the size of the parameter.
       *  \remarks Gets a parameter's value.
       *  \note This method is used for parameters of type NVSG_UNSIGNED_BYTE.
       *  \note The behavior is undefined if the specified parameter is not an unsigned char parameter.
       *  \sa getType, getSize, getName, setValue */
      NVSG_API void getValue( unsigned char * uc ) const;

      /*! \brief Sets a parameter's value using an unsigned char *.
       *  \param uc A pointer to an array of unsigned char data, that is at least the size of the parameter.
       *  \remarks Sets a parameter's value.
       *  \note This method is used for parameters of type NVSG_UNSIGNED_BYTE.
       *  \note The behavior is undefined if the specified parameter is not an unsigned char parameter.
       *  \sa getType, getSize, getName, getValue */
      NVSG_API void setValue( const unsigned char * uc );

      /*! \brief Gets a parameter's value using a short *.
       *  \param s A pointer to an array of signed short data, that is at least the size of the parameter.
       *  \remarks Gets a parameter's value.
       *  \note This method is used for parameters of type NVSG_SHORT.
       *  \note The behavior is undefined if the specified parameter is not a signed short parameter.
       *  \sa getType, getSize, getName, setValue */
      NVSG_API void getValue( short * s ) const;

      /*! \brief Sets a parameter's value using an short *.
       *  \param s A pointer to an array of signed short data, that is at least the size of the parameter.
       *  \remarks Sets a parameter's value.
       *  \note This method is used for parameters of type NVSG_SHORT.
       *  \note The behavior is undefined if the specified parameter is not a signed short parameter.
       *  \sa getType, getSize, getName, getValue */
      NVSG_API void setValue( const short * s );

      /*! \brief Gets a parameter's value using a unsigned short *.
       *  \param us A pointer to an array of unsigned short data, that is at least the size of the parameter.
       *  \remarks Gets a parameter's value.
       *  \note This method is used for parameters of type NVSG_UNSIGNED_SHORT.
       *  \note The behavior is undefined if the specified parameter is not an unsigned short parameter.
       *  \sa getType, getSize, getName, setValue */
      NVSG_API void getValue( unsigned short * us ) const;

      /*! \brief Sets a parameter's value using an unsigned short *.
       *  \param us A pointer to an array of unsigned short data, that is at least the size of the parameter.
       *  \remarks Sets a parameter's value.
       *  \note This method is used for parameters of type NVSG_UNSIGNED_SHORT.
       *  \note The behavior is undefined if the specified parameter is not an unsigned short parameter.
       *  \sa getType, getSize, getName, getValue */
      NVSG_API void setValue( const unsigned short * us );

      /*! \brief Gets a parameter's value using signed integers.
       *  \param i A pointer to an array of signed integer data, that is at least the size of the
       *  parameter.
       *  \remarks Gets a signed integer parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not a signed integer parameter.
       *  \sa getType, getName, setValue */
      NVSG_API void getValue( int * i ) const;

      /*! \brief Sets a parameter's value using signed integers.
       *  \param i A pointer to an array of signed integer data, that is at least the size of the
       *  parameter.
       *  \remarks Sets a signed integer parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not a signed integer parameter.
       *  \sa getType, getName, getValue */
      NVSG_API void setValue( const int * i );

      /*! \brief Gets a parameter's value using unsigned integers.
       *  \param ui A pointer to an array of unsigned integer data, that is at least the size of the
       *  parameter specified by \a index.
       *  \remarks Gets an unsigned integer parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not an unsigned integer parameter.
       *  \sa getType, getName, setValue */
      NVSG_API void getValue( unsigned int * ui ) const;

      /*! \brief Sets a parameter's value using unsigned integers.
       *  \param ui A pointer to an array of unsigned integer data, that is at least the size of the
       *  parameter specified by \a index.
       *  \remarks Sets an unsigned integer parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not an unsigned integer parameter.
       *  \sa getType, getName, getValue */
      NVSG_API void setValue( const unsigned int * ui );

      /*! \brief Gets a parameter's value using floats.
       *  \param f A pointer to an array of floating-point data, that is at least the size of the
       *  parameter.
       *  \remarks Gets a floating-point parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined the specified parameter is not a floating-point parameter.
       *  \sa getType, getName, setValue */
      NVSG_API void getValue( float * f ) const;

      /*! \brief Sets a parameter's value using floats.
       *  \param f A pointer to an array of floating-point data, that is at least the size of the
       *  parameter.
       *  \remarks Sets a floating-point parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not a floating-point parameter.
       *  \sa getType, getName, getValue */
      NVSG_API void setValue( const float * f );

      /*! \brief Gets a parameter's value using doubles.
       *  \param d A pointer to an array of double data, that is at least the size of the
       *  parameter.
       *  \remarks Gets a double parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined the specified parameter is not a double parameter.
       *  \sa getType, getName, setValue */
      NVSG_API void getValue( double * d ) const;

      /*! \brief Sets a parameter's value using doubles.
       *  \param d A pointer to an array of double data, that is at least the size of the
       *  parameter.
       *  \remarks Sets a double parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not a double parameter.
       *  \sa getType, getName, getValue */
      NVSG_API void setValue( const double * d );

      /*! \brief Gets an RTBufferAttribute parameter's value
       *  \param buffer A reference to an RTBufferAttribute to receive the value.
       *  \remarks Gets an RTBufferAttribute parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not an RTBufferAttribute.
       *  \sa getType, getName, setValue */
      NVSG_API void getValue( RTBufferAttributeSharedPtr & buffer ) const;

      /*! \brief Sets a parameter's value using an RTBufferAttribute
       *  \param buffer A reference to an RTBufferAttribute.
       *  \remarks Sets an RTBufferAttribute parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not an RTBufferAttribute-
       *  \sa getType, getName, getValue */
      NVSG_API void setValue( const RTBufferAttributeSharedPtr & buffer );

      /*! \brief Gets an TextureAttributeItem parameter's value
       *  \param tex The TextureAttributeItem to receive the value.
       *  \remarks Gets an TextureAttributeItem parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not a TextureAttributeItem.
       *  \sa getType, getName, setValue */
      NVSG_API void getValue( TextureAttributeItemSharedPtr & tex ) const;

      /*! \brief Sets a parameter's value using a TextureAttributeItem.
       *  \param tex The TextureAttributeItem to set.
       *  \remarks Sets an TextureAttributeItem parameter's value. Note that no conversions are performed.
       *  \note The behavior is undefined if the specified parameter is not an TextureAttributeItem
       *  \sa getType, getName, getValue */
      NVSG_API void setValue( const TextureAttributeItemSharedPtr & tex );

      /*! \brief Returns a pointer to the parameter's value
       *  \remarks If the parameter is an RTBufferAttribute or TextureAttributeItem a pointer to the associated handle 
       *  is returned.  Otherwise, a pointer to the data storage backing the data type is returned.
       *  \sa getType, getName, setValue */
      NVSG_API const void * getValue() const;

      /*! \brief Get the number of annotations for this parameter.
       *  \return int Number of annotations for this parameter.
       *  \sa getAnnotation */
      NVSG_API unsigned int getNumberOfAnnotations() const;

      /*! \brief Get an annotation by index.
       *  \param index The annotation index.  Must be < the value returned from getNumberOfAnnotations().
       *  \param result The annotation structure at the given index.
       *  \return bool True if annotation index was valid, false otherwise.
       *  \note annotations are currently immutable
       *  \sa getNumberofAnnotations getSemantic */
      NVSG_API bool getAnnotation( unsigned int index, Annotation & result ) const;

      /*! \brief Get an annotation by name.
       *  \param name The name of the annotation to look up.
       *  \param result The annotation structure with the given name.
       *  \return bool True if annotation name was valid, false otherwise.
       *  \note annotations are currently immutable
       *  \sa getNumberofAnnotations getSemantic */
      NVSG_API bool getAnnotation( const std::string & name, Annotation & result ) const;

      /*! \brief Get all annotations.
       *  \return A constant vector of annotations
       *  \sa getNumberofAnnotations getSemantic */
      NVSG_API const std::vector<Annotation> & getAnnotations() const;

      /*! \brief Get variable's semantic.
       *  \param result The returned semantic.
       *  \return bool True if the variable has a semantic, false otherwise.
       *  \note semantics are currently immutable
       *  \sa getAnnotation */
      NVSG_API bool getSemantic( std::string & result ) const;
      NVSG_API const std::string & getSemantic() const;

      /*! \brief Get variable's type and count from a string description. 
       *  \note Unrecognized type names will return type=PT_USER_DATA assuming the type was a data structure.
       *  In addition, for parameters determined to be of type PT_USER_DATA, count will always be 1.
       * */
      NVSG_API static bool stringToParameterDescription( const std::string & str, unsigned char & type, unsigned char & count );

      /*! \brief Returns the named type of this parameter
       *  \remarks If the parameter is a type PT_USER_DATA, and this parameter was parsed from an RTFx file, 
       *  then this will be the data type parsed from the RTFx (ie: the struct identifier).
       */
      NVSG_API const std::string & getParameterTypeName() const;

      /*! \brief Get the hash key of this RTFxParameter.
       *  \return The hash key of this RTFxParameter.
       *  \remarks If the hash key is not valid, the virtual function feedHashGenerator() is called to
       *  recursively determine a hash string, using a HashGenerator, which then is converted into
       *  a HashKey.
       *  \sa feedHashGenerator */
      NVSG_API HashKey getHashKey() const;

  protected:
      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      NVSG_API void addAnnotation( const Annotation & anno );

      // Note: the type is specified by the variable of the same name in the associated RTFxProgram !!
      std::string                   m_name;        // name of parameter
      unsigned char                 m_type;        // type of the parameter in m_data
      unsigned char                 m_count;       // number of elements in m_data
      std::vector<unsigned char>    m_data;        // the data buffer for a single parameter
      RTBufferAttributeSharedPtr    m_buffer;      // buffer parameter
      TextureAttributeItemSharedPtr m_texture;     // texture parameter
      std::vector<Annotation>       m_annotations; // variable's annotations
      std::string                   m_semantic;    // variable's annotations
      std::string                   m_typeName;    // variable's type string

      nvutil::SWMRSync              m_lock;        // helps managing exclusive access to mutable members while reading the object 
      mutable HashKey               m_hashKey;
      mutable bool                  m_hashKeyValid;
  };

  inline const std::vector< RTFxParameter::Annotation > & RTFxParameter::getAnnotations() const
  {
    return m_annotations;
  }

  inline unsigned int RTFxParameter::getNumberOfAnnotations() const
  {
    return checked_cast< unsigned int >( m_annotations.size() );
  }

  inline void RTFxParameter::addAnnotation( const Annotation & anno )
  {
    m_annotations.push_back( anno );
  }

  inline bool RTFxParameter::getAnnotation( unsigned int index, Annotation & anno ) const
  {
    if( index < m_annotations.size() )
    {
      anno = m_annotations[index];
      return true;
    }

    return false;
  }

  inline bool RTFxParameter::getAnnotation( const std::string & name, Annotation & anno ) const
  {
    for( size_t i = 0; i < m_annotations.size(); i++ )
    {
      if( m_annotations[i].name == name )
      {
        anno = m_annotations[i];
        return true;
      }
    }

    return false;
  }

  inline bool RTFxParameter::getSemantic( std::string & result ) const
  {
    result = m_semantic;
    return !m_semantic.empty();
  }

  inline const std::string & RTFxParameter::getSemantic() const
  {
    return m_semantic;
  }

  /*! \brief Translate a string into an enumeration value type, and a count.
   *  \param str A string like "float" or "uint3", describing a parameter.
    * \param type A reference to the type enum being set by this function.
    * \param count A reference to the count value being set by this function.
   *  \return \c true, if the string correctly describes a parameter type, otherwise \c false.
   *  \sa typeSize, RTFxParameter, RTFxParameterType */
  NVSG_API bool stringToParameterDescription( const std::string & str
                                            , unsigned char & type
                                            , unsigned char & count );

  class RTFxProgram : public OwnedObject<Object>
  {
    friend class Object;

    public:
      NVSG_API static RTFxProgramSharedPtr create();

    public:
      typedef enum
      {
        FT_UNKNOWN,
        FT_RTFX
      } FileType;

      typedef enum
      {
        PF_UNKNOWN,
        PF_CUDA,
        PF_PTX
      } ProgramFormat;

      typedef enum
      {
        PT_ANY_HIT,
        PT_BOUNDING_BOX,
        PT_CLOSEST_HIT,
        PT_EXCEPTION,
        PT_INTERSECTION,
        PT_MISS,
        PT_RAY_GENERATION
      } ProgramType;

      typedef enum
      {
        TF_FLOAT3,
        TF_FLOAT4
      } TextureFormat;

      struct WrapModes
      {
        nvsg::TextureWrapMode wrapS;
        nvsg::TextureWrapMode wrapT;
        nvsg::TextureWrapMode wrapR;
        bool operator==( const WrapModes & rhs ) const;
      };

      typedef enum
      {
        TMF_MIP_NONE,       // no mipmapping
        TMF_MIP_NEAREST,    // nearest
        TMF_MIP_LINEAR      // linear
      } TextureMipFilterMode;

      typedef enum
      {
        TRM_FLOAT,          // read floats or normalized floats
        TRM_INTEGER         // read integers
      } TextureReadMode;

      struct FilterModes
      {
        nvsg::TextureMinFilterMode minFilter;
        nvsg::TextureMagFilterMode magFilter;
        TextureMipFilterMode       mipFilter;   // mip not really supported yet
        bool operator==( const FilterModes & rhs ) const;
      };

      /*! \brief The container type of the RTFxParameters
       *  \remarks If the \c bool in the pair of RTFxParameter and a bool is \c true, this parameter
       *  is defined in this RTFxProgram. Otherwise it is not. */
      typedef std::vector<std::pair<RTFxParameter,bool> >                             ParameterContainer;

      /*! \brief The iterator over the ParameterContainer */
      typedef nvutil::SceniXIterator<RTFxProgram,ParameterContainer::iterator>        ParameterIterator;

      /*! \brief The const iterator over the ParameterContainer */
      typedef nvutil::SceniXIterator<RTFxProgram,ParameterContainer::const_iterator>  ParameterConstIterator;

    public:
      NVSG_API RTFxProgram & operator=( const RTFxProgram & rhs );
      NVSG_API virtual bool isEquivalent( const Object * p, bool ignoreNames, bool deepCompare ) const;

      /*! \brief Get the search paths for this program.
       *  \return A constant reference to the search paths. */
      NVSG_API const std::vector<std::string> & getSearchPaths() const;

      /*! \brief Get the number of unique ray types used in this program.
       *  \note "Ray Types" are only used in ClosestHit, AnyHit and Miss Program domains.
       *  \return The number of ray types used in the program. */
      NVSG_API unsigned int getNumberOfRayTypes() const;

      /*! \brief Get the ray type associated with a program index.
       *  \param index An index in the range 0 to getNumberOfRayTypes()-1.
       *  \note "Ray Types" are only used in ClosestHit, AnyHit and Miss Program domains.  If the index refers to 
       *  a different domain, results are undefined.
       *  \return The ray type associated with that index.  Results are undefined if the index is out of range. */
      NVSG_API unsigned int getRayType( unsigned int index ) const;

      /*! \brief Query if this program uses the given rayType.
       *  \remarks Checks to see if this program uses the given rayType.
       *  \param raytype The raytype to query.
       *  \note "Ray Types" are only used in ClosestHit, AnyHit and Miss Program domains.
       *  \return boolean true if the rayType is used, false if not. */
      NVSG_API bool usesRayType( unsigned int rayType ) const;

      /*! \brief Get the number of unique entry point indices used in this program.
       *  \note "Entry Point" indices are only used in RayGeneration and Exception Program domains.
       *  \return The number of entry point indices used in the program. */
      NVSG_API unsigned int getNumberOfEntryPointIndices() const;

      /*! \brief Get the entry point index associated with a program index.
       *  \param index An index in the range 0 to getNumberOfEntryPointIndices()-1.
       *  \note "Entry Point" indices are only used in RayGeneration and Exception Program domains.  If the index
       *  refers to a different domain, the results are undefined.
       *  \return The entry point associated with that index.  Results are undefined if the index is out of range. */
      NVSG_API unsigned int getEntryPointIndex( unsigned int index ) const;

      /*! \brief Query if this program uses the given entry point index.
       *  \remarks Checks to see if this program uses the given entry point index.
       *  \param entryPoint The entry point index to query.
       *  \note "Entry Point" indices are only used in RayGeneration and Exception Program domains.
       *  \return boolean true if the rayType is used, false if not. */
      NVSG_API bool usesEntryPointIndex( unsigned int entryPoint ) const;

      /*! \brief Get the entry point function name.
       *  \remarks Get the entry point function name associated with a particular program and raytype or entry point.
       *  \param index The raytype or entry point index to use in the query.
       *  \param programType from the ProgramType enumeration.
       *  \return The entry point method name, or an empty string if the program did not contain the programType, 
       *  rayType, or entryPointIndex supplied. */
      NVSG_API const std::string & getProgramEntryPoint( unsigned int index, ProgramType programType ) const;

      /*! \brief Query this program's format.
       *  \remarks Gets the format of the program, currently PF_UNKNOWN, PF_CUDA or PF_PTX.
       *  \return ProgramFormat representing this program's format. */
      NVSG_API ProgramFormat getProgramFormat() const;

      /*! \brief Gets the RTFx source string.
       *  \remarks Gets the source string that created this RTFxProgram.  Does not include technique block.
       *  \return std::string containing the source */
      NVSG_API const std::string & getProgramString() const;

      /*! \brief Gets the RTFx technique string.
       *  \remarks Gets the technique block string that created this RTFxProgram.  Does not include source code.
       *  \return std::string containing the source */
      NVSG_API const std::string & getTechniqueString() const;

      /*! \brief Get the number of textures in this RTFxProgram.
       *  \return The number of textures.
       *  \sa getTextureFileName, getTextureSamplerName */
      NVSG_API unsigned int getNumberOfTextures() const;

      /*! \brief Get the format of the specified texture.
       *  \param index the index of the texture to query, in the range of 0 to getNumberOfTextures()-1.
       *  \return The texture's format, which is currently TF_FLOAT3 or TF_FLOAT4.
       *  \remarks The texture's format is currently ignored in SceniX - all textures are assumed to be float4.
       *  \sa getTextureFileName, getTextureSamplerName */
      NVSG_API TextureFormat getTextureFormat( unsigned int index ) const;

      /*! \brief Get the target of the specified texture.
       *  \param index the index of the texture to query, in the range of 0 to getNumberOfTextures()-1.
       *  \return The texture's target, currently either NVSG_TEXTURE_2D or NVSG_TEXTURE_RECTANGLE
       *  \sa getTextureFilterModes, getTextureWrapModes, getTextureReadMode */
      NVSG_API nvsg::TextureTarget getTextureTarget( unsigned int index ) const;

      /*! \brief Get the readmode of the specified texture.
       *  \param index the index of the texture to query, in the range of 0 to getNumberOfTextures()-1.
       *  \return The texture's read mode, determining how to interpret the texture data.
       *  \sa getTextureFilterModes, getTextureWrapModes */
      NVSG_API RTFxProgram::TextureReadMode getTextureReadMode( unsigned int index ) const;

      /*! \brief Get the filter modes of the specified texture.
       *  \param index the index of the texture to query, in the range of 0 to getNumberOfTextures()-1.
       *  \return The texture's filter modes structure.
       *  \sa getTextureFileName, getTextureSamplerName getTextureWrapModes */
      NVSG_API const RTFxProgram::FilterModes & getTextureFilterModes( unsigned int index ) const;

      /*! \brief Get the wrap modes of the specified texture.
       *  \param index the index of the texture to query, in the range of 0 to getNumberOfTextures()-1.
       *  \return The texture's wrap modes data structure.
       *  \sa getTextureFileName, getTextureSamplerName getTextureFilterModes */
      NVSG_API const RTFxProgram::WrapModes & getTextureWrapModes( unsigned int index ) const; 

      /*! \brief Get the texture's sampler name.
       *  \param index the index of the texture to query, in the range of 0 to getNumberOfTextures()-1.
       *  \return The name of the sampler in the source code that this texture is bound to.
       *  \sa getTextureFileName, getTextureFormat */
      NVSG_API const std::string & getTextureSamplerName( unsigned int index ) const;

      /*! \brief Get the texture's file name.
       *  \param index the index of the texture to query, in the range of 0 to getNumberOfTextures()-1.
       *  \return The texture's file name, if the texture originated from a file.
       *  \sa getTextureFormat, getTextureSamplerName */
      NVSG_API const std::string & getTextureFileName( unsigned int index ) const;

      /*  \brief Get the number of parameters in this RTFxProgram.
       *  \return The number of parameters in this RTFxProgram.
       *  \sa beginParameters, endParameters, findParameter */
      NVSG_API unsigned int getNumberOfParameters() const;

      /*! \brief Get a const iterator to the first parameter in this RTFxProgram.
       *  \return A const iterator to the first parameter in this RTFxProgram.
       *  \sa getNumberOfParameters, endParameters, findParameter */
      NVSG_API ParameterConstIterator beginParameters() const;

      /*! \brief Get an iterator to the first parameter in this RTFxProgram.
       *  \return An iterator to the first parameter in this RTFxProgram
       *  \sa getNumberOfParameters, endParameters, findParameter */
      NVSG_API ParameterIterator beginParameters();

      /*! \brief Get a const iterator that points just beyond the end of the parameters in this RTFxProgram.
       *  \return A const iterator that points just beyond the end of the parameters in this RTFxProgram.
       *  \sa getNumberOfParameters, beginParameters, findParameter */
      NVSG_API ParameterConstIterator endParameters() const;

      /*! \brief Get an iterator that points just beyond the end of the parameters in this RTFxProgram.
       *  \return An iterator that points just beyond the end of the parameters in this RTFxProgram.
       *  \sa getNumberOfParameters, beginParameters, findParameter */
      NVSG_API ParameterIterator endParameters();

      /*  \brief Find a specified parameter in this RTFxProgram.
       *  \param name The name of the parameter to find.
       *  \return A const iteator to the found parameter in this RTFxProgram.
       *  \sa getNumberOfParameters, beginParameters, endParameters */
      NVSG_API ParameterConstIterator findParameter( const std::string & name ) const;

      /*  \brief Find a specified parameter in this RTFxProgram.
       *  \param name The name of the parameter to find.
       *  \return An iterator to the found parameter in this RTFxProgram.
       *  \sa getNumberOfParameters, beginParameters, endParameters */
      NVSG_API ParameterIterator findParameter( const std::string & name );

      /*! \brief Returns whether the effect was created from a file.
       *  \return The function returns \c true if the effect was created from an RTFx source file
       *  Otherwise, the function returns \c false.
       *  \remarks An RTFx can be created either from a source file using RTFx::createFromFile, or
       *  alternatively from a string of RTFx source code using RTFx::createFromLump.\n
       *  Information on whether a certain effect was created from a file or a string could be
       *  useful, for example, for implementing a scene saver which probably needs to track this
       *  information.
       *  \sa RTFx::createFromFile */
      NVSG_API bool isFromFile() const;

      /*! \brief Returns whether the effect was created from a lump of RTFx source code.
       *  \return The function returns \c true if the effect was create from a lump of RTFx source
       *  code. Otherwise the function returns \c false.
       *  \remarks An RTFx can be created either from a source file using RTFx::createFromFile, or
       *  alternatively from a string of RTFx source code using RTFx::createFromLump.\n
       *  Information on whether a certain effect was created from a file or a string could be
       *  useful, for example, for implementing a scene saver which probably needs to track this
       *  information.
       *  \sa RTFx::createFromLump */
      NVSG_API bool isFromLump() const;

      /*! \brief Returns the file name of the RTFx source file from which the RTFxProgram was created.
       *  \return If this RTFxProgram was created from a file, the path and name of that file is
       *  returned. Otherwise an empty string is returned.
       *  \sa RTFx::isFromFile, RTFx::createFromFile */
      NVSG_API const std::string & getFileName() const;

      /*! \brief Returns RTFx source string from which the RTFxProgram was created.
       *  \return If this RTFxProgram was created from a source string, that string is returned.
       *  Otherwise an empty string is returned.
       *  \sa RTFx::isFromLump, RTFx::createFromLump */
      NVSG_API const std::string & getSourceCode() const;

      /*! \brief Create an RTFxProgram from a file, caching the resulting RTFxProgram
       *  \param file The filename containing the RTFx code.
       *  \param searchPaths A vector containing search paths to use for shader and include paths.
       *  \param errorReturn If the method returns NULL, then errorReturn will contain an error
       *  message indicating what the possible errors were.
       *  \param compargs A vector containing arguments to the NVCC compiler used to compile CUDA to PTX.  An example
       *  would be "-Iinclude" or "-DMYAPP", etc.  These arguments are ignored if the source is already in compiled PTX 
       *  format.
       *  \remarks Loads RTFx code from a file, parses it, and prepares it for the RTTraverser which
       *  will compile it at runtime.\n
       *  The search paths will be used for all shader and include paths.\n
       *  If an RTFxProgram of the same file name is found in the cache, a pointer to it will be returned instead.
       *  \returns RTFxProgram possibly cached.
       *  \sa CgFxEffect::createFromFile */
      static NVSG_API RTFxProgramSharedPtr createFromFile( const std::string & file,
                                                           const std::vector< std::string > & searchPaths, 
                                                           const std::vector< std::string > & compargs,
                                                           std::string & errorReturn );

      /*! \brief Create an RTFxProgram from a string, caching the resulting RTFxProgram
       *  \param string A string containing RTFx code.
       *  \param searchPaths A vector containing search paths to use for shader and include paths.
       *  \param errorReturn If the method returns NULL, then errorReturn will contain an error
       *  message indicating what the possible errors were.
       *  \param filetype Defaults to RTFxProgram::FT_RTFX indicating that this is an RTFx formatted
       *  file. In the future, RTFxProgram::FT_METASL support may be added using this same entry
       *  point.
       *  \param compargs A vector containing arguments to the NVCC compiler used to compile CUDA to PTX.  An example
       *  would be "-Iinclude" or "-DMYAPP", etc.  These arguments are ignored if the source is already in compiled PTX 
       *  format.
       *  \remarks Loads RTFx code from the given string, parses it, and prepares it for the
       *  RTTraverser which will compile it at runtime.\n
       *  The search paths will be used for all shader and include paths.
       *  \returns boolean indicating success or failure.
       *  \sa CgFxEffect::createFromLump */
      static NVSG_API RTFxProgramSharedPtr createFromLump( const std::string & lump,
                                                           const std::vector< std::string > & searchPaths, 
                                                           const std::vector< std::string > & compargs,
                                                           std::string & errorReturn,
                                                           FileType filetype = FT_RTFX );

      REFLECTION_INFO_API( NVSG_API, RTFxProgram );
    protected:
      friend struct nvutil::Holder<RTFxProgram>;

      NVSG_API RTFxProgram();
      NVSG_API RTFxProgram( const RTFxProgram & rhs );
      NVSG_API virtual ~RTFxProgram();

      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      NVSG_API void constructProgram( RTFxParser * parser );
      NVSG_API bool createFromFile( const std::string & file, const std::vector<std::string> & args, std::string & errorReturn );
      NVSG_API bool createFromLump( const std::string & lump, const std::vector<std::string> & args, std::string & errorReturn, FileType filetype = FT_RTFX );
      NVSG_API void setSearchPaths( const std::vector<std::string> & paths );
      NVSG_API void decomposeAnnotations( RTFxParameter & param, const std::string & annos );

    private:
      struct TextureEntry
      {
        std::string   file;                   // file containing texture
        TextureFormat format;                 // format of the buffer for texture conversion; currently ignored !
        std::string   samplerName;            // name of sampler in code to assign bits to
        FilterModes   filterModes;
        WrapModes     wrapModes;
        nvsg::TextureTarget target;           // target: 2D, RECT, etc
        TextureReadMode readMode;             // how to interpret the data from the texture map
        bool operator==( const TextureEntry & rhs ) const;
      };

      struct ProgramSet
      {
        std::string names[PT_RAY_GENERATION+1];   // number of program types
        bool operator==( const ProgramSet & rhs ) const;
      };

    private:
      std::string                                 m_effectFile;
      std::string                                 m_effectSource;
      ParameterContainer                          m_parameters;
      std::map<unsigned int, ProgramSet>          m_programEntryPoints;
      ProgramFormat                               m_programFormat;
      std::string                                 m_programString;
      std::string                                 m_techniqueString;
      std::vector<unsigned int>                   m_rayTypes;   // just a helper for book-keeping and iteration
      std::vector<unsigned int>                   m_entryPointIndices; // ditto
      std::vector<std::string>                    m_searchPaths;
      std::vector<TextureEntry>                   m_textures;
      typedef std::map< std::string, nvsg::RTFxProgramSharedPtr > CacheMap;
  };

}

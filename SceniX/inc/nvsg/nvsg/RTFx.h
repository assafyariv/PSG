// Copyright NVIDIA Corporation 2005-2011
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
#include "nvsg/CoreTypes.h"
#include "nvsg/RTFxProgram.h"
#include "nvsg/StateAttribute.h"
#include "nvsg/SceneAttribute.h"
#include "nvsg/TextureAttribute.h"
#include "nvutil/HashGenerator.h"
#include <vector>
#include <set>
#include <map>

class RTFxParser;

namespace nvtraverser
{
  class RTTraverser;
}

namespace nvsg
{
  /*! \brief A StateAttribute to apply render effects to the Ray Tracing System.
   * \par Namespace: nvsg
   */
  class RTFxBase
  {
  public:
    /*! \brief The container type of the RTFxParameters */
    typedef std::vector<RTFxParameter>                                          ParameterContainer;

    /*! \brief The iterator over the ParameterContainer */
    typedef nvutil::SceniXIterator<RTFxBase,ParameterContainer::iterator>       ParameterIterator;

    /*! \brief The const iterator over the ParameterContainer */
    typedef nvutil::SceniXIterator<RTFxBase,ParameterContainer::const_iterator> ParameterConstIterator;

  public:
    /*! \brief Default-constructs an RTFx object.
      */
    NVSG_API RTFxBase();
    NVSG_API RTFxBase(const RTFxBase & rhs);

    /*! \brief Destructs a RTFx object.
     */
    NVSG_API virtual ~RTFxBase(); 

    /*! \brief Assigns contents from a source RTFx object.
      * \param
      * rhs Source RTFx object
      * \return
      * The assignment operator returns a reference to this RTFx object.
      * \remarks
      * The assignment operator releases old contents before re-assigning the new contents.
      */
    NVSG_API RTFxBase& operator=(const RTFxBase& rhs);

    /*! \brief Overrides Object::isEquivalent.  
      * \param
      * object Pointer to the const Object to test for equivalence with this RTFx object.
      * \param
      * ignoreNames Object names will be ignored while testing if this is \c true.
      * \param
      * deepCompare The function performs a deep-compare instead of a shallow compare if this is \c true.
      * \return
      * The function returns \c true if the Object pointed to by \a object is detected to be equivalent
      * to this RTFx object.
      * \remarks
      * The test will consider the optional control parameters ignoreNames and deepCompare. 
      * If you omit these two, the function ignores the object names and performs a shallow
      * compare only.
      * \sa Object::isEquivalent
      */
    NVSG_API virtual bool isSameAs(const RTFxBase *object, bool ignoreNames, bool deepCompare) const;

    /*! \brief Interface to get the RTFxProgram of this object.
     *  \return A pointer to the RTFxProgram of this object.
     *  \remarks All classes deriving from RTFxBase have to provide this function. As ownership handling
     *  of an RTFxProgram needs an Object, the RTFxProgram can't be a part of the RTFxBase. That is, all
     *  concrete classes deriving from RTFxBase should also derive from Object (or a class derived from
     *  Object). */
    NVSG_API virtual const nvsg::RTFxProgramSharedPtr & getProgram() const = 0;

    /*  \brief Get the number of parameters in this object.
     *  \return The number of parameters in this object.
     *  \sa beginParameters, endParameters, addParameter, removeParameter, clearParameters, findParameter */
    NVSG_API unsigned int getNumberOfParameters() const;

    /*! \brief Get a const iterator to the first parameter in this object.
     *  \return A const iterator to the first parameter in this object.
     *  \sa getNumberOfParameters, endParameters, addParameter, removeParameter, clearParameters, findParameter */
    NVSG_API ParameterConstIterator beginParameters() const;

    /*! \brief Get an iterator to the first parameter in this object.
     *  \return An iterator to the first parameter in this object.
     *  \sa getNumberOfParameters, endParameters, addParameter, removeParameter, clearParameters, findParameter */
    NVSG_API ParameterIterator beginParameters();

    /*! \brief Get a const iterator that points just beyond the end of the parameters in this object.
     *  \return A const iterator that points just beyond the end of the parameters in this Object.
     *  \sa getNumberOfParameters, beginParameters, addParameter, removeParameter, clearParameters, findParameter */
    NVSG_API ParameterConstIterator endParameters() const;

    /*! \brief Get an iterator that points just beyond the end of the parameters in this object.
     *  \return An iterator that points just beyond the end of the parameters in this Object.
     *  \sa getNumberOfParameters, beginParameters, addParameter, removeParameter, clearParameters, findParameter */
    NVSG_API ParameterIterator endParameters();

    /*! \brief Add a parameter to this object.
     *  \param name Name of the parameter to add.
     *  \param type Type of the parameter to add.
     *  \param count Number of elements of type \a type in the parameter to add
     *  \param size Memory footprint size in bytes (sizeof(structure)*count) if the type is PT_USER_DATA.  Otherwise, ignored.
     *  \param semantic The semantic to assign to the variable (see note).
     *  \param annotations The annotations to add to this variable (see note).
     *  \return An iterator that points to the added parameter.
     *  \remarks An RTFxBase manages the values of parameters used in an associated RTFxProgram.
     *  \note The behavior is undefined if there is a variable in the associated RTFxProgram with
     *  the same name, but of a different type or count.
     *  \note Semantics and Annotations assigned this way are NOT stored to any NVSG output files.  Only the 
     *  semantics and annotations that appear in the RTFx file itself will be stored to disk.
     *  \sa getNumberOfParameters, beginParameters, endParameters, removeParameter, clearParameters, findParameter */
    NVSG_API ParameterIterator addParameter( const std::string & name, unsigned char type, unsigned int count
                                           , unsigned int size = 0, const std::string & semantic = std::string()
                                           , const std::vector< RTFxParameter::Annotation > & annotations = std::vector< RTFxParameter::Annotation >() );

    /*! \brief Remove a parameter from this object.
     *  \param name The name of the parameter to remove from this object
     *  \return \c true, if the parameter has been removed from this object, otherwise \c false.
     *  \sa getNumberOfParameters, beginParameters, endParameters, addParameter, clearParameters, findParameter */
    NVSG_API bool removeParameter( const std::string & name );

    /*! \brief Remove a parameter from this object.
     *  \param pi An iterator to the parameter to remove from this object.
     *  \return An iterator pointing to the new location of the parameter that followed the one removed by
     *  this function call, which is endParameters() if the operation removed the last parameter.
     *  \sa getNumberOfParameters, beginParameters, endParameters, addParameter, clearParameters, findParameter */
    NVSG_API ParameterIterator removeParameter( const ParameterIterator & pi );

    /*! \brief Remove all parameters from this object.
     *  \sa getNumberOfParameters, beginParameters, endParameters, addParameter, removeParameter, findParameter */
    NVSG_API void clearParameters();

    /*  \brief Find a specified parameter in this object.
     *  \param name The name of the parameter to find.
     *  \return A const iteator to the found parameter in this object.
     *  \sa getNumberOfParameters, beginParameters, endParameters, addParameter, removeParameter, clearParameters */
    NVSG_API ParameterConstIterator findParameter( const std::string & name ) const;

    /*  \brief Find a specified parameter in this object.
     *  \param name The name of the parameter to find.
     *  \return An iterator to the found parameter in this object.
     *  \sa getNumberOfParameters, beginParameters, endParameters, addParameter, removeParameter, clearParameters */
    NVSG_API ParameterIterator findParameter( const std::string & name );

    /*! \brief Add a clone of an RTFxProgram parameter to the RTFxBase.
     *  \param name Name of the parameter to add.
     *  \return The iterator to the cloned and added parameter.
     *  \remarks An RTFxBase manages the values of parameters used in an associated RTFxProgram.  This method will add 
     *           an editable clone of an RTFxProgram parameter to the RTFx.  The cloned copy will contain the original 
     *           parameters value, semantics and annotations.
     *  \note This is a convenience function, and performs actions similar to the following:
     *  <code>
     *    RTFxBase::ParamterIterator clonedParam = rtfx->endParameters();
     *    RTFxProgramReadLock prog( rtfx->getProgram() );
     *    RTFxProgram::ParameterIterator progParam = prog->findParameter( "name" );
     *    if ( progParam != prog->endParameters() )
     *    {
     *      RTFxBase::ParameterIterator rtfxParam = rtfx->findParameter( "name" );
     *      if ( rtfxParam == rtfx->endParameters() )
     *      {
     *        clonedParameter = rtfx->addParameter( progParam->getname(), progParam->getType(), progParam->getSize()
     *                                            , progParam->getSemantic(), progParam->getAnnotations() );
     *        switch( progParam->getType() )
     *        {
     *          case [...]
     *            rtfx->setParameterValue( clonedParameter, (cast *)progParam->getValue() );
     *        }
     *      }
     *    }
     *    return( clonedParam );
     *  </code>
     *  \sa addParameter, removeParameter */
    NVSG_API ParameterIterator cloneProgramParameter( const std::string & name );

    /*! \brief Sets a parameter's value using char (signed 8-bit integers).
     *  \param pi The iterator to the parameter to set.
     *  \param c A pointer to a constant array of char data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using char values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined if the specified parameter is not a char parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const char * c );

    /*! \brief Sets a parameter's value using unsigned chars.
     *  \param pi The iterator to the parameter to set.
     *  \param uc A pointer to a constant array of unsigned char data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using unsigned char values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined if the specified parameter is not an unsigned char parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const unsigned char * uc );

    /*! \brief Sets a parameter's value using signed shorts.
     *  \param pi The iterator to the parameter to set.
     *  \param s A pointer to a constant array of signed short data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using signed short values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined if the specified parameter is not a signed short parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const short * s );

    /*! \brief Sets a parameter's value using unsigned shorts.
     *  \param pi The iterator to the parameter to set.
     *  \param us A pointer to a constant array of unsigned integer data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using unsigned short values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined if the specified parameter is not an unsigned short parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const unsigned short * us );

    /*! \brief Sets a parameter's value using signed integers.
     *  \param pi The iterator to the parameter to set.
     *  \param i A pointer to a constant array of signed integer data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using signed integer values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined if the specified parameter is not a signed integer parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const int * i );

    /*! \brief Sets a parameter's value using unsigned integers.
     *  \param pi The iterator to the parameter to set.
     *  \param ui A pointer to a constant array of unsigned integer data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using unsigned integer values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined if the specified parameter is not an unsigned integer parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const unsigned int * ui );

    /*! \brief Sets a parameter's value using floats.
     *  \param pi The iterator to the parameter to set.
     *  \param f A pointer to a constant array of floating-point data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using floating-point values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined the specified parameter is not a floating-point parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const float * f);

    /*! \brief Sets a parameter's value using doubles.
     *  \param pi The iterator to the parameter to set.
     *  \param d A pointer to a constant array of double data, that is at least the size of
     *  the parameter specified by \a pi.
     *  \remarks Sets a parameter's value using double values. Note that no conversions are
     *  performed.
     *  \note The behavior is undefined the specified parameter is not a double parameter.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const double * d);

    /*! \brief Sets a parameter's value using untyped pointer.
     *  \param pi The iterator to the parameter to set.
     *  \param v A pointer to untyped memory, that is at least the size of the parameter specified by \a pi.
     *  \sa getNumberOfParameters, RTFxProgram::getVariableType */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const void * v );

    /*! \brief set buffer value */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const RTBufferAttributeSharedPtr & buffer );

    /*! \brief set texture value */
    NVSG_API void setParameterValue( const ParameterIterator & pi, const TextureAttributeItemSharedPtr & tex );

  protected:
    NVSG_API virtual void onParameterValueChanged() = 0;
    NVSG_API virtual void resetToDefaults();
    NVSG_API virtual void setProgram( const RTFxProgramSharedPtr & program ) = 0;
    NVSG_API void initFromProgram( const RTFxProgramSharedPtr & prgHdl );

    /*! \brief Feed the data of this object into the provied HashGenerator.
     *  \param hg The HashGenerator to update with the data of this object.
     *  \sa getHashKey */
    NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

  private:
    nvsg::TextureAttributeItemSharedPtr loadTexture( const std::string & fileName, const std::vector<std::string> & searchPaths );

  private:
    ParameterContainer  m_parameters;
  };

  /*! \brief A StateAttribute to represent a per-object ray tracing effect.
   * \par Namespace: nvsg
   */
  class RTFx : public StateAttribute, public RTFxBase 
  {
    public:
      NVSG_API static RTFxSharedPtr create();

    public:
      NVSG_API RTFx& operator=(const RTFx& rhs);
      NVSG_API virtual bool isEquivalent(const Object *object, bool ignoreNames, bool deepCompare) const;
      NVSG_API virtual bool isDataShared() const;
      NVSG_API virtual DataID getDataID() const;

      NVSG_API virtual void resetToDefaults();
      NVSG_API virtual void setProgram( const RTFxProgramSharedPtr & program );
      NVSG_API virtual const RTFxProgramSharedPtr & getProgram() const;
      NVSG_API virtual void onParameterValueChanged();

      REFLECTION_INFO_API( NVSG_API, RTFx );
    protected:
      friend struct nvutil::Holder<RTFx>;

      NVSG_API RTFx();
      NVSG_API RTFx(const RTFx & rhs);
      NVSG_API virtual ~RTFx();
      /*! \brief Feed the data of this object into the provied HashGenerator.
       *  \param hg The HashGenerator to update with the data of this object.
       *  \sa getHashKey */
      NVSG_API virtual void feedHashGenerator( nvutil::HashGenerator & hg ) const;

    private:
      // Can't have m_program in RTFxBase due to ownership handling, that needs to be done in from inside an Object
      RTFxProgramSharedPtr  m_program;
  };

  /*! \brief A SceneAttribute to represent a per-scene ray tracing effect.
   * \par Namespace: nvsg
   */
  class RTFxSceneAttribute : public SceneAttribute, public RTFxBase 
  {
    public:
      NVSG_API static RTFxSceneAttributeSharedPtr create();

    public:
      NVSG_API RTFxSceneAttribute& operator=(const RTFxSceneAttribute& rhs);
      NVSG_API virtual bool isEquivalent(const Object *object, bool ignoreNames, bool deepCompare) const;
      NVSG_API virtual bool isDataShared() const;
      NVSG_API virtual DataID getDataID() const;

      NVSG_API virtual void resetToDefaults();
      NVSG_API virtual void setProgram( const RTFxProgramSharedPtr & program );
      NVSG_API virtual const RTFxProgramSharedPtr & getProgram() const;
      NVSG_API virtual void onParameterValueChanged();

      REFLECTION_INFO_API( NVSG_API, RTFxSceneAttribute );
    protected:
      friend struct nvutil::Holder<RTFxSceneAttribute>;

      NVSG_API RTFxSceneAttribute();
      NVSG_API RTFxSceneAttribute(const RTFxSceneAttribute & rhs);
      NVSG_API virtual ~RTFxSceneAttribute();

    private:
      // Can't have m_program in RTFxBase due to ownership handling, that needs to be done in from inside an Object
      RTFxProgramSharedPtr  m_program;
  };

} // namespace nvsg

namespace nvutil
{
  template<>
  struct ObjectTraits<nvsg::RTFxBase>
  {
    typedef nvsg::RTFxBase  ObjectType;
  };

}


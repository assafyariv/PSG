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
#include "nvrt/RTBuffer.h"
#include "nvrt/RTGeometryGroup.h"
#include "nvrt/RTGroup.h"
#include "nvrt/RTHandleError.h"
#include "nvrt/RTProgram.h"
#include "nvrt/RTSelector.h"
#include "nvrt/RTTextureSampler.h"
#include "nvrt/RTTransform.h"
#include "nvsgrtapi.h"

namespace nvrt
{
  /*! \brief Wrapper class for an optix RTvariable object.
   *  \par Namespace: nvrt
   *  \remarks An RTVariable is the base class for a number of specific variable classes. It manages
   *  the common resource handling of all those variable classes.
   *  \sa RTVariableNT, RTVariableNxMT, RTVariableType, RTVariableUser */
  class RTVariable
  {
    friend class RTVariablesManager;

    public:
      /*! \brief Get the name of the variable.
       *  \return The name of the variable as a std::string.
       *  \note The name of the variable is specified by the RTvariable argument to the constructor. */
      NVRT_API std::string getName() const;

    protected:
      /*! \brief Default-constructor of RTVariable */
      NVRT_API RTVariable( RTvariable var );

      /*! \brief Destructor of RTVariable */
      NVRT_API virtual ~RTVariable();

      /*! \brief Get the underlying resource of this object.
       *  \remarks An RTVariable wraps the functionality of an optix::RTvariable.
       *  This function is used internally for accessing that resource. */
      NVRT_API RTvariable getResource() const;

    private:
      std::string m_name;
      RTvariable  m_variable;
  };

  /*! \brief Templated base class for all variables with arrays of values.
   *  \par Namespace: nvrt
   *  \remarks An RTVariableNT is the base class for specific variable classes with arrays of values.
   *  It manages those values.
   *  \sa RTVariable, RTVariableNxMT, RTVariableType, RTVariableUser */
  template<unsigned char N, typename T>
  class RTVariableNT : public RTVariable
  {
    friend class RTVariablesManager;

    public:
      /*! \brief Set the value of this variable.
       *  \param v A constant pointer to at least \c N values of type \c T.
       *  \sa get */
      void set( const T * v );

      /*! \brief Get the value of this variable.
       *  \param v A pointer to memory that can hold at least \c N values of type \c T.
       *  \sa set */
      void get( T * v ) const;

    protected:
      /*! \brief Default-constructor of RTVariableNT */
      RTVariableNT( RTvariable var );

      /*! \brief Destructor of RTVariableNT */
      virtual ~RTVariableNT();

      /*! \brief Perform the variable setting operation.
       *  \param v A pointer to constant memory holding enough values to set.
       *  \sa doGet */
      void doSet( const T * v );

      /*! \brief Perform the variable getting operation.
       *  \param v A pointer to memory large enough to hold the values of this variable.
       *  \sa doSet */
      void doGet( T * v ) const;

    protected:
      T m_values[N];
  };

  template<unsigned char N, typename T>
  inline RTVariableNT<N,T>::RTVariableNT( RTvariable var )
    : RTVariable(var)
  {
    memset( m_values, 0, N * sizeof(T) );
    doSet( m_values );
  }

  template<unsigned char N, typename T>
  inline RTVariableNT<N,T>::~RTVariableNT()
  {
  }

  template<unsigned char N, typename T>
  inline void RTVariableNT<N,T>::set( const T * v )
  {
#if !defined(NDEBUG)
    T realValues[N];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, N * sizeof(T) ) == 0 );
#endif
    if ( memcmp( v, m_values, N * sizeof(T) ) != 0 )
    {
      memcpy( m_values, v, N * sizeof(T) );
      doSet( v );
    }
  }

  template<unsigned char N, typename T>
  inline void RTVariableNT<N,T>::get( T * v ) const
  {
#if !defined(NDEBUG)
    T realValues[N];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, N * sizeof(T) ) == 0 );
#endif
    memcpy( v, m_values, N * sizeof(T) );
  }

  template<unsigned char N, typename T>
  inline void RTVariableNT<N,T>::doSet( const T * v )
  {
    RT_VALIDATE( RTVariableSetUserData( getResource(), N * sizeof(T), v ) );
  }

  template<unsigned char N, typename T>
  inline void RTVariableNT<N,T>::doGet( T * v ) const
  {
    RT_VALIDATE( RTVariableGetUserData( getResource(), N * sizeof(T), v ) );
  }

  template<>
  inline void RTVariableNT<1,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSet1fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<1,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGet1fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<1,int>::doSet( const int * v )
  {
    RT_VALIDATE( RTVariableSet1iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<1,int>::doGet( int * v ) const
  {
    RT_VALIDATE( RTVariableGet1iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<1,unsigned int>::doSet( const unsigned int * v )
  {
    RT_VALIDATE( RTVariableSet1uiv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<1,unsigned int>::doGet( unsigned int * v ) const
  {
    RT_VALIDATE( RTVariableGet1uiv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<2,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSet2fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<2,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGet2fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<2,int>::doSet( const int * v )
  {
    RT_VALIDATE( RTVariableSet2iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<2,int>::doGet( int * v ) const
  {
    RT_VALIDATE( RTVariableGet2iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<2,unsigned int>::doSet( const unsigned int * v )
  {
    RT_VALIDATE( RTVariableSet2uiv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<2,unsigned int>::doGet( unsigned int * v ) const
  {
    RT_VALIDATE( RTVariableGet2uiv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<3,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSet3fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<3,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGet3fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<3,int>::doSet( const int * v )
  {
    RT_VALIDATE( RTVariableSet3iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<3,int>::doGet( int * v ) const
  {
    RT_VALIDATE( RTVariableGet3iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<3,unsigned int>::doSet( const unsigned int * v )
  {
    RT_VALIDATE( RTVariableSet3uiv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<3,unsigned int>::doGet( unsigned int * v ) const
  {
    RT_VALIDATE( RTVariableGet3uiv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<4,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSet4fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<4,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGet4fv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<4,int>::doSet( const int * v )
  {
    RT_VALIDATE( RTVariableSet4iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<4,int>::doGet( int * v ) const
  {
    RT_VALIDATE( RTVariableGet4iv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<4,unsigned int>::doSet( const unsigned int * v )
  {
    RT_VALIDATE( RTVariableSet4uiv( getResource(), v ) );
  }

  template<>
  inline void RTVariableNT<4,unsigned int>::doGet( unsigned int * v ) const
  {
    RT_VALIDATE( RTVariableGet4uiv( getResource(), v ) );
  }

  /*! \brief Templated base class for all variables with one value.
   *  \par Namespace: nvrt
   *  \remarks An RTVariable1T is the base class for specific variable classes with one value.
   *  \sa RTVariable, RTVariableNT, RTVariableNxMT, RTVariableType, RTVariableUser */
  template<typename T>
  class RTVariable1T : public RTVariableNT<1,T>
  {
    friend class RTVariablesManager;

    public:
      using RTVariableNT<1,T>::set;

    public:
      /*! \brief Set the value of this variable.
       *  \param v0 The value to set.
       *  \sa get */
      void set( T v0 );

      /*! \brief Get the value of this variable.
       *  \param v0 A reference to the memory to write the value of the variable to.
       *  \sa set */
      void get( T & v0 );

    protected:
      /*! \brief Default-constructor of RTVariable1T */
      RTVariable1T( RTvariable var );

      /*! \brief Destructor of RTVariable1T */
      virtual ~RTVariable1T();

      using RTVariableNT<1, T>::m_values;
  };

  template<typename T>
  inline RTVariable1T<T>::RTVariable1T( RTvariable var )
    : RTVariableNT<1,T>(var)
  {
  }

  template<typename T>
  inline RTVariable1T<T>::~RTVariable1T()
  {
  }

  template<typename T>
  inline void RTVariable1T<T>::set( T v0 )
  {
    if ( v0 != m_values[0] )
    {
      m_values[0] = v0;
      doSet( &m_values[0] );
    }
  }

  template<typename T>
  inline void RTVariable1T<T>::get( T & v0 )
  {
#if !defined(NDEBUG)
    T realValues[1];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, 1 * sizeof(T) ) == 0 );
#endif
    v0 = m_values[0];
  }

  /*! \brief Templated base class for all variables with two values.
   *  \par Namespace: nvrt
   *  \remarks An RTVariable2T is the base class for specific variable classes with two values.
   *  \sa RTVariable, RTVariableNT, RTVariableNxMT, RTVariableType, RTVariableUser */
  template<typename T>
  class RTVariable2T : public RTVariableNT<2,T>
  {
    friend class RTVariablesManager;

    public:
      using RTVariableNT<2,T>::set;

    public:
      /*! \brief Set the values of this variable.
       *  \param v0 The first value to set.
       *  \param v1 The second value to set.
       *  \sa get */
      void set( T v0, T v1 );

      /*! \brief Get the values of this variable.
       *  \param v0 A reference to the memory to write the first value of the variable to.
       *  \param v1 A reference to the memory to write the second value of the variable to.
       *  \sa set */
      void get( T & v0, T & v1 );

    protected:
      /*! \brief Default-constructor of RTVariable2T */
      RTVariable2T( RTvariable var );

      /*! \brief Destructor of RTVariable2T */
      virtual ~RTVariable2T();

      using RTVariableNT<2, T>::m_values;
  };

  template<typename T>
  inline RTVariable2T<T>::RTVariable2T( RTvariable var )
    : RTVariableNT<2,T>(var)
  {
  }

  template<typename T>
  inline RTVariable2T<T>::~RTVariable2T()
  {
  }

  template<typename T>
  inline void RTVariable2T<T>::set( T v0, T v1 )
  {
    if ( ( v0 != m_values[0] ) || ( v1 != m_values[1] ) )
    {
      m_values[0] = v0;
      m_values[1] = v1;
      doSet( &m_values[0] );
    }
  }

  template<typename T>
  inline void RTVariable2T<T>::get( T & v0, T & v1 )
  {
#if !defined(NDEBUG)
    T realValues[2];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, 2 * sizeof(T) ) == 0 );
#endif
    v0 = m_values[0];
    v1 = m_values[1];
  }

  /*! \brief Templated base class for all variables with three values.
   *  \par Namespace: nvrt
   *  \remarks An RTVariable3T is the base class for specific variable classes with three values.
   *  \sa RTVariable, RTVariableNT, RTVariableNxMT, RTVariableType, RTVariableUser */
  template<typename T>
  class RTVariable3T : public RTVariableNT<3,T>
  {
    friend class RTVariablesManager;

    public:
      using RTVariableNT<3,T>::set;

    public:
      /*! \brief Set the values of this variable.
       *  \param v0 The first value to set.
       *  \param v1 The second value to set.
       *  \param v2 The third value to set.
       *  \sa get */
      void set( T v0, T v1, T v2 );

      /*! \brief Get the values of this variable.
       *  \param v0 A reference to the memory to write the first value of the variable to.
       *  \param v1 A reference to the memory to write the second value of the variable to.
       *  \param v2 A reference to the memory to write the third value of the variable to.
       *  \sa set */
      void get( T & v0, T & v1, T & v2 );

    protected:
      /*! \brief Default-constructor of RTVariable3T */
      RTVariable3T( RTvariable var );

      /*! \brief Destructor of RTVariable3T */
      virtual ~RTVariable3T();

      using RTVariableNT<3, T>::m_values;
  };

  template<typename T>
  inline RTVariable3T<T>::RTVariable3T( RTvariable var )
    : RTVariableNT<3,T>(var)
  {
  }

  template<typename T>
  inline RTVariable3T<T>::~RTVariable3T()
  {
  }

  template<typename T>
  inline void RTVariable3T<T>::set( T v0, T v1, T v2 )
  {
    if ( ( v0 != m_values[0] ) || ( v1 != m_values[1] ) || ( v2 != m_values[2] ) )
    {
      m_values[0] = v0;
      m_values[1] = v1;
      m_values[2] = v2;
      doSet( &m_values[0] );
    }
  }

  template<typename T>
  inline void RTVariable3T<T>::get( T & v0, T & v1, T & v2 )
  {
#if !defined(NDEBUG)
    T realValues[3];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, 3 * sizeof(T) ) == 0 );
#endif
    v0 = m_values[0];
    v1 = m_values[1];
    v2 = m_values[2];
  }

  /*! \brief Templated base class for all variables with four values.
   *  \par Namespace: nvrt
   *  \remarks An RTVariable4T is the base class for specific variable classes with four values.
   *  \sa RTVariable, RTVariableNT, RTVariableNxMT, RTVariableType, RTVariableUser */
  template<typename T>
  class RTVariable4T : public RTVariableNT<4,T>
  {
    friend class RTVariablesManager;

    public:
      using RTVariableNT<4,T>::set;

    public:
      /*! \brief Set the values of this variable.
       *  \param v0 The first value to set.
       *  \param v1 The second value to set.
       *  \param v2 The third value to set.
       *  \param v3 The fourth value to set.
       *  \sa get */
      void set( T v0, T v1, T v2, T v3 );

      /*! \brief Get the values of this variable.
       *  \param v0 A reference to the memory to write the first value of the variable to.
       *  \param v1 A reference to the memory to write the second value of the variable to.
       *  \param v2 A reference to the memory to write the third value of the variable to.
       *  \param v3 A reference to the memory to write the fourth value of the variable to.
       *  \sa set */
      void get( T & v0, T & v1, T & v2, T & v3 );

    protected:
      /*! \brief Default-constructor of RTVariable4T */
      RTVariable4T( RTvariable var );

      /*! \brief Destructor of RTVariable4T */
      virtual ~RTVariable4T();

      using RTVariableNT<4, T>::m_values;
  };

  template<typename T>
  inline RTVariable4T<T>::RTVariable4T( RTvariable var )
    : RTVariableNT<4,T>(var)
  {
  }

  template<typename T>
  inline RTVariable4T<T>::~RTVariable4T()
  {
  }

  template<typename T>
  inline void RTVariable4T<T>::set( T v0, T v1, T v2, T v3 )
  {
    if ( ( v0 != m_values[0] ) || ( v1 != m_values[1] ) || ( v2 != m_values[2] ) || ( v3 != m_values[3] ) )
    {
      m_values[0] = v0;
      m_values[1] = v1;
      m_values[2] = v2;
      m_values[3] = v3;
      doSet( &m_values[0] );
    }
  }

  template<typename T>
  inline void RTVariable4T<T>::get( T & v0, T & v1, T & v2, T & v3 )
  {
#if !defined(NDEBUG)
    T realValues[4];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, 4 * sizeof(T) ) == 0 );
#endif
    v0 = m_values[0];
    v1 = m_values[1];
    v2 = m_values[2];
    v3 = m_values[3];
  }

  typedef RTVariable1T<float>         RTVariable1f;
  typedef RTVariable1T<int>           RTVariable1i;
  typedef RTVariable1T<unsigned int>  RTVariable1ui;
  typedef RTVariable2T<float>         RTVariable2f;
  typedef RTVariable2T<int>           RTVariable2i;
  typedef RTVariable2T<unsigned int>  RTVariable2ui;
  typedef RTVariable3T<float>         RTVariable3f;
  typedef RTVariable3T<int>           RTVariable3i;
  typedef RTVariable3T<unsigned int>  RTVariable3ui;
  typedef RTVariable4T<float>         RTVariable4f;
  typedef RTVariable4T<int>           RTVariable4i;
  typedef RTVariable4T<unsigned int>  RTVariable4ui;


  /*! \brief Templated base class for all variables with a matrix of values.
   *  \par Namespace: nvrt
   *  \remarks An RTVariableNxMT is the base class for specific variable classes with a matrix of values.
   *  \sa RTVariable, RTVariableNT, RTVariableNxMT, RTVariableType, RTVariableUser */
  template<unsigned char N, unsigned char M, typename T>
  class RTVariableNxMT : public RTVariable
  {
    friend class RTVariablesManager;

    public:
      /*! \brief Set the values of this variable.
       *  \param v A constant pointer to at least \c N x \c M values of type \c T.
       *  \sa get */
      void set( const T * v );

      /*! \brief Get the values of this variable.
       *  \param v A pointer to memory that can hold at least \c N x \c M values of type \c T.
       *  \sa set */
      void get( T * v ) const;

    protected:
      /*! \brief Default-constructor of RTVariableNxMT */
      RTVariableNxMT( RTvariable var );

      /*! \brief Destructor of RTVariableNxMT */
      virtual ~RTVariableNxMT();

      /*! \brief Perform the variable setting operation.
       *  \param v A pointer to constant memory holding enough values to set.
       *  \sa doGet */
      void doSet( const T * v );

      /*! \brief Perform the variable getting operation.
       *  \param v A pointer to memory large enough to hold the values of this variable.
       *  \sa doSet */
      void doGet( T * v ) const;

    private:
      T m_values[N*M];
  };

  template<unsigned char N, unsigned char M, typename T>
  inline RTVariableNxMT<N,M,T>::RTVariableNxMT( RTvariable var )
    : RTVariable(var)
  {
    memset( m_values, 0, N * M * sizeof(T) );
    doSet( m_values );
  }

  template<unsigned char N, unsigned char M, typename T>
  inline RTVariableNxMT<N,M,T>::~RTVariableNxMT()
  {
  }

  template<unsigned char N, unsigned char M, typename T>
  inline void RTVariableNxMT<N,M,T>::set( const T * v )
  {
#if !defined(NDEBUG)
    T realValues[N * M];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, N * M * sizeof(T) ) == 0 );
#endif
    if ( memcmp( v, m_values, N * M * sizeof(T) ) != 0 )
    {
      memcpy( m_values, v, N * M * sizeof(T) );
      doSet( v );
    }
  }

  template<unsigned char N, unsigned char M, typename T>
  inline void RTVariableNxMT<N,M,T>::get( T * v ) const
  {
#if !defined(NDEBUG)
    T realValues[N * M];
    doGet( realValues );
    NVSG_ASSERT( memcmp( m_values, realValues, N * M * sizeof(T) ) == 0 );
#endif
    memcpy( v, m_values, N * M * sizeof(T) );
  }

  template<>
  inline void RTVariableNxMT<2,2,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix2x2fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<2,2,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix2x2fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<2,3,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix2x3fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<2,3,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix2x3fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<2,4,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix2x4fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<2,4,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix2x4fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<3,2,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix3x2fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<3,2,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix3x2fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<3,3,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix3x3fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<3,3,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix3x3fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<3,4,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix3x4fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<3,4,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix3x4fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<4,2,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix4x2fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<4,2,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix4x2fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<4,3,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix4x3fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<4,3,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix4x3fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<4,4,float>::doSet( const float * v )
  {
    RT_VALIDATE( RTVariableSetMatrix4x4fv( getResource(), false, v ) );
  }

  template<>
  inline void RTVariableNxMT<4,4,float>::doGet( float * v ) const
  {
    RT_VALIDATE( RTVariableGetMatrix4x4fv( getResource(), false, v ) );
  }

  typedef RTVariableNxMT<2,2,float> RTVariable2x2f;
  typedef RTVariableNxMT<2,3,float> RTVariable2x3f;
  typedef RTVariableNxMT<2,4,float> RTVariable2x4f;
  typedef RTVariableNxMT<3,2,float> RTVariable3x2f;
  typedef RTVariableNxMT<3,3,float> RTVariable3x3f;
  typedef RTVariableNxMT<3,4,float> RTVariable3x4f;
  typedef RTVariableNxMT<4,2,float> RTVariable4x2f;
  typedef RTVariableNxMT<4,3,float> RTVariable4x3f;
  typedef RTVariableNxMT<4,4,float> RTVariable4x4f;

  /*! \brief Templated base class for all variables with a handled value.
   *  \par Namespace: nvrt
   *  \remarks An RTVariableNxMT is the base class for specific variable classes with a handled value.
   *  \sa RTVariable, RTVariableNT, RTVariableNxMT, RTVariableUser */
  template<typename Type>
  class RTVariableType : public RTVariable
  {
    friend class RTVariablesManager;

    public:
      /*! \brief Set the handled value of this variable.
       *  \param v A constant pointer to the handled value.
       *  \sa get */
      void set( const typename nvutil::ObjectTraits<Type>::WeakPtr & v );

      /*! \brief Set the handled value of this variable.
       *  \param v A smart constant pointer to the handled value.
       *  \sa get */
      void set( const typename nvutil::ObjectTraits<Type>::SharedPtr & v );

      /*! \brief Get the handled value of this variable.
       *  \param v A pointer to handled value of this variable.
       *  \sa set */
      typename nvutil::ObjectTraits<Type>::WeakPtr get() const;

    protected:
      /*! \brief Default-constructor of RTVariableType */
      RTVariableType( RTvariable var );

      /*! \brief Destructor of RTVariableType */
      virtual ~RTVariableType();

    private:
#if !defined(NDEBUG)
      bool checkValue() const;
#endif
      RTobject getValueResource() const;

    private:
      typename nvutil::ObjectTraits<Type>::SharedPtr  m_value;
  };

  template<typename Type>
  RTVariableType<Type>::RTVariableType( RTvariable var )
    : RTVariable(var)
  {
  }

  template<typename Type>
  RTVariableType<Type>::~RTVariableType()
  {
  }

  template<typename Type>
  void RTVariableType<Type>::set( const typename nvutil::ObjectTraits<Type>::WeakPtr & value )
  {
    if ( m_value != value )
    {
      m_value = value;
      RT_VALIDATE( RTVariableSetObject( getResource(), getValueResource() ) );
    }
  }

  template<typename Type>
  void RTVariableType<Type>::set( const typename nvutil::ObjectTraits<Type>::SharedPtr & value )
  {
    set( value.get() );
  }

  template<typename Type>
  typename nvutil::ObjectTraits<Type>::WeakPtr RTVariableType<Type>::get() const
  {
    NVSG_ASSERT( checkValue() );
    return( m_value.get() );
  }

  template<>
  inline RTobject RTVariableType<RTBuffer>::getValueResource() const
  {
    return( m_value ? RTBufferReadLock(m_value)->getResource() : NULL );
  }

  template<>
  inline RTobject RTVariableType<RTGeometryGroup>::getValueResource() const
  {
    return( m_value ? RTGeometryGroupReadLock(m_value)->getResource() : NULL );
  }

  template<>
  inline RTobject RTVariableType<RTGroup>::getValueResource() const
  {
    return( m_value ? RTGroupReadLock(m_value)->getResource() : NULL );
  }

  template<>
  inline RTobject RTVariableType<RTSelector>::getValueResource() const
  {
    return( m_value ? RTSelectorReadLock(m_value)->getResource() : NULL );
  }

  template<>
  inline RTobject RTVariableType<RTTextureSampler>::getValueResource() const
  {
    return( m_value ? RTTextureSamplerReadLock(m_value)->getResource() : NULL );
  }

  template<>
  inline RTobject RTVariableType<RTTransform>::getValueResource() const
  {
    return( m_value ? RTTransformReadLock(m_value)->getResource() : NULL );
  }

#if !defined(NDEBUG)
  template<typename Type>
  inline bool RTVariableType<Type>::checkValue() const
  {
    if ( m_value )
    {
      RTobject obj;
      RT_VALIDATE( RTVariableGetObject( getResource(), &obj ) );
      return( getValueResource() == obj );
    }
    return( true );
  }
#endif

  typedef RTVariableType<RTBuffer>          RTVariableBuffer;
  typedef RTVariableType<RTGeometryGroup>   RTVariableGeometryGroup;
  typedef RTVariableType<RTGroup>           RTVariableGroup;
  typedef RTVariableType<RTSelector>        RTVariableSelector;
  typedef RTVariableType<RTTextureSampler>  RTVariableTextureSampler;
  typedef RTVariableType<RTTransform>       RTVariableTransform;

  /*! \brief Class for all variables with user specified value.
   *  \par Namespace: nvrt
   *  \remarks An RTVariableUser is the class for variables with user specified value.
   *  It manages those values.
   *  \sa RTVariable, RTVariableNT, RTVariableNxMT, RTVariableType */
  class RTVariableUser : public RTVariable
  {
    friend class RTVariablesManager;

    public:
      /*! \brief Get the value of this variable.
       *  \param data A pointer to memory of at least the specified size.
       *  \note The size of this variable has to be specified, using \c setSize(), before getting it.
       *  \sa set, setSize */
      NVRT_API void get( void * data ) const;

      /*! \brief Set the value of this variable.
       *  \param data A constant pointer to memory of at least the specified size.
       *  \note The size of this variable has to be specified, using \c setSize(), before setting it.
       *  \sa get, setSize */
      NVRT_API void set( const void * data );

      /*! \brief Set the size of the value of this variable
       *  \param size The size of the value of this variable in byte.
       *  \note The arguments to the functions \c get() and \c set() must point to memory of at least
       *  \a size bytes.
       *  \sa get, set */
      NVRT_API void setSize( unsigned int size );

    protected:
      /*! \brief Default-constructor of RTVariableUser */
      NVRT_API RTVariableUser( RTvariable var );

      /*! \brief Destructor of RTVariableUser */
      NVRT_API virtual ~RTVariableUser();

    private:
      bool            m_set;
      unsigned int    m_size;
      void          * m_value;
  };

}

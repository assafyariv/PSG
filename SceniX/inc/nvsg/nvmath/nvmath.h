// Copyright NVIDIA Corporation 2002-2006
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
/** @file */

#include "nvsgcommon.h"

#include  <math.h>
#include  <algorithm> // for std::max, std::min (_MAX, _MIN)
#include  <float.h>
#include  <limits>

//! Namespace for mathematical helper functions and classes.
namespace nvmath
{
  // define the stand trig values
#ifdef PI
# undef PI
# undef PI_HALF
# undef PI_QUARTER
#endif
  //! constant PI
  const float PI            = 3.14159265358979323846f;
  //! constant PI half
  const float PI_HALF       = 1.57079632679489661923f;
  //! constant PI quarter
  const float PI_QUARTER    = 0.78539816339744830962f;
  //! constant square root two
  const float SQRT_TWO      = 1.41421356237309504880f;
  //! constant square root two half
  const float SQRT_TWO_HALF = 0.70710678118654752440f;
  //! constant square root three
  const float SQRT_THREE    = 1.73205080756887729353f;
  //! constant square root three
  const float SQRT_THREE_HALF = 0.86602540378443864676f;

  //! Template to clamp an object of type T to a lower and an upper limit.
  /** \returns clamped value of \a v between \a l and \a u */
  template<class T> T     clamp( T v                  //!<  value to clamp
                               , T l                  //!<  lower limit
                               , T u                  //!<  upper limit
                               )
  {
    return std::min(u, std::max(l,v));
  }

  //! Template to cube an object of Type T.
  /** \param t value to cube
    * \returns triple product of \a t with itself */
  template<typename T> 
  inline T cube( const T& t )
  {
    return( t * t * t );
  }

  //! Transform an angle in degrees to radians.
  /** \returns angle in radians */
  template<typename T>
  inline T degToRad( T angle )
  {
    return( angle*(PI/180) );
  }

  /*! \brief Returns the highest bit set for the specified positive input value */
  template<typename T>
  inline int highestBit( T i )
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    NVSG_ASSERT( 0 <= i );
    int hb = -1;
    for ( T h = i ; h ; h>>=1, hb++ )
      ;
    return hb;
  }

  /*! \brief Returns the highest bit value for the specified positive input value */
  template<typename T>
  inline T highestBitValue(T i)
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    NVSG_ASSERT( 0 <= i );
    T h = 0;
    while (i)
    {
      h = (i & (~i+1)); // grab lowest bit
      i &= ~h; // clear lowest bit
    }
    return h;
  }

  /*! \brief Calculate the vertical field of view out of the horizontal field of view */
  inline float horizontalToVerticalFieldOfView( float hFoV, float aspectRatio )
  {
    return( 2.0f * atanf( tanf( 0.5f * hFoV ) / aspectRatio ) );
  }

  //! Determine if an integer is a power of two.
  /** \returns \c true if \a n is a power of two, otherwise \c false  */
  template<typename T>
  inline bool isPowerOfTwo( T n )
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    return (n && !(n & (n-1)));
  }

  //!  Linear interpolation between two values \a v0 and \a v1.
  /**  v = ( 1 - alpha ) * v0 + alpha * v1 */
  template<typename T> 
  inline T lerp( float alpha  //!<  interpolation parameter
               , const T &v0  //!<  starting value
               , const T &v1  //!<  ending value
               )
  {
    return( (T)( ( 1.0f - alpha ) * v0 + alpha * v1 ) );
  }

  template<typename T>
  inline void lerp( float alpha, const T & v0, const T & v1, T & vr )
  {
    vr = (T)( ( 1.0f - alpha ) * v0 + alpha * v1 );
  }

  /*! \brief Determine the maximal value out of three.
   *  \param a A constant reference of the first value to consider.
   *  \param b A constant reference of the second value to consider.
   *  \param c A constant reference of the third value to consider.
   *  \return A constant reference to the maximal value out of \a a, \a b, and \a c.
   *  \sa min */
#ifndef max
  template<typename T>  
  inline const T & max( const T &a, const T &b, const T &c )
  {
    return( std::max( a, std::max( b, c ) ) );
  }
#endif
  /*! \brief Determine the minimal value out of three.
   *  \param a A constant reference of the first value to consider.
   *  \param b A constant reference of the second value to consider.
   *  \param c A constant reference of the third value to consider.
   *  \return A constant reference to the minimal value out of \a a, \a b, and \a c.
   *  \sa max */
#ifndef min
  template<typename T>  
  inline const T & min( const T &a, const T &b, const T &c )
  {
    return( std::min( a, std::min( b, c ) ) );
  }
#endif
  /*! \brief Determine the smallest power of two equal to or larger than \a n.
   *  \param n The value to determine the nearest power of two above.
   *  \return \a n, if \a n is a power of two, otherwise the smallest power of two above \a n.
   *  \sa powerOfTowBelow */
  template<typename T>
  inline T powerOfTwoAbove( T n )
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    if ( isPowerOfTwo( n ) )
    {
      return( n );
    }
    else
    {
      return highestBitValue(n) << 1;
    }
  }

  /*! \brief Determine the largest power of two equal to or smaller than \a n.
   *  \param n The value to determine the nearest power of two below.
   *  \return \a n, if \a n is a power of two, otherwise the largest power of two below \a n.
   *  \sa powerOfTowAbove */
  template<typename T>
  inline T powerOfTwoBelow( T n )
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    if ( isPowerOfTwo( n ) )
    {
      return( n );
    }
    else
    {
      return highestBitValue(n);
    }
  }

  /*! \brief Calculates the nearest power of two for the specified integer. 
   *  \param n Integer for which to calculate the nearest power of two. 
   *  \returns nearest power of two for integer \a n. */ 
  template<typename T>
  inline T powerOfTwoNearest( T n)
  {
    NVSG_CTASSERT( std::numeric_limits<T>::is_integer );
    if ( isPowerOfTwo( n ) )
    {
      return n;
    }
    
    T prev = highestBitValue(n); // POT below
    T next = prev<<1; // POT above
    return (next-n) < (n-prev) ? next : prev; // return nearest
  }

  //! Transform an angle in radian to degree.
  /** \param angle angle in radians
    * \returns angle in degrees */
  inline float radToDeg( float angle )
  {
    return( angle*180.0f/PI );
  }

  //! Determine the sign of a scalar.
  /** \param t scalar value
    * \returns sign of \a t */
  template<typename T> 
  inline int sign( const T &t )
  {
    return( ( t < 0 ) ? -1 : ( ( t > 0 ) ? 1 : 0 ) );
  }

  //! Solve the quadratic equation a*x^2 + b*x + c = 0
  /** \param a Coefficient of the quadratic term.
    * \param b Coefficient of the linear term.
    * \param c Coefficient of the constant term.
    * \param x0 Reference to hold the first of up to two solutions.
    * \param x1 Reference to hold the second of up to two solutions.
    * \returns The number of real solutions (0, 1, or 2)
    * \remarks If 2 is returned, x0 and x1 hold those two solutions. If 1 is returned, it is a double solution
    * (turning point), returned in x0. If 0 is returned, there are only two complex conjugated solution, that
    * are not calculated here. */
  template<typename T>
  inline unsigned int solveQuadraticEquation( T a, T b, T c, T & x0, T & x1 )
  {
    if ( std::numeric_limits<T>::epsilon() < fabs( a ) )
    {
      T D = b * b - 4 * a * c;
      if ( 0 < D )
      {
        D = sqrt( D );
        x0 = 0.5f * ( - b + D ) / a;
        x1 = 0.5f * ( - b - D ) / a;
        return( 2 );
      }
      if ( D < 0 )
      {
        return( 0 );
      }
      x0 = - 0.5f * b / a;
      return( 1 );
    }
    if ( std::numeric_limits<T>::epsilon() < fabs( b ) )
    {
      x0 = - c / b;
      return( 1 );
    }
    return( 0 );
  }

  //! Solve the cubic equation a*x^3 + b*x^2 + c*x + d = 0
  /** \param a Coefficient of the cubic term.
    * \param b Coefficient of the quadratic term.
    * \param c Coefficient of the linear term.
    * \param d Coefficient of the constant term.
    * \param x0 Reference to hold the first of up to three solutions.
    * \param x1 Reference to hold the second of up to three solutions.
    * \param x2 Reference to hold the second of up to three solutions.
    * \returns The number of real solutions (0, 1, 2, or 3)
    * \remarks If the absolute value of \a a is less than the type specific epsilon, the cubic equation is handled
    * as a quadratic only.
    * If 3 is returned, x0, x1, and x2 hold those three solutions. If 2 is returned, the cubic equation
    * in fact was a quadratic one, and x0 and x1 hold those two solutions. If 1 is returned, the cubic equation
    * has two conjugate complex, and one real solution; as complex numbers are not treated by this function, it is
    * just the real solution returned in x0. If 0 is returned, the cubic equationn again war in fact a quadratic one,
    * and there are only two complex conjugated solution, that are not calculated here. */
  template<typename T>
  inline unsigned int solveCubicEquation( T a, T b, T c, T d, T & x0, T & x1, T & x2 )
  {
    if ( std::numeric_limits<T>::epsilon() < abs( a ) )
    {
      T bOverThreeA = b / ( 3 * a );
      T p = - square( bOverThreeA ) + c / ( 3 * a );
      T q = cube( bOverThreeA ) - T(0.5) * bOverThreeA * c / a + T(0.5) * d / a;
      T discriminant = square( q ) + cube( p );
      if ( discriminant < - std::numeric_limits<T>::epsilon() )    // < 0 ?
      {
        NVSG_ASSERT( p <= 0 );
        T f = 2 * sqrt( -p );
        T phi = acos( -q / sqrt( - cube( p ) ) );
        x0 = f * cos( phi / 3 ) - bOverThreeA;
        x1 = - f * cos( ( phi + PI ) / 3 ) - bOverThreeA;
        x2 = - f * cos( ( phi - PI ) / 3 ) - bOverThreeA;
        return( 3 );
      }
      else if ( std::numeric_limits<T>::epsilon() < discriminant )  // > 0 ?
      {
        T t = sqrt( discriminant );
        T u = sign( - q + t ) * pow( abs( - q + t ), 1 / T(3) );
        T v = sign( - q - t ) * pow( abs( - q - t ), 1 / T(3) );
        x0 = u + v - bOverThreeA;
        return( 1 );    // plus two complex solutions
      }
      else
      {
        T t = sign( - q ) * pow( abs( - q ), 1 / T(3) );
        x0 = 2 * t - bOverThreeA;
        x1 = -t - bOverThreeA;
        x2 = -t - bOverThreeA;
        return( 3 );
      }
    }
    else
    {
      return( solveQuadraticEquation( b, c, d, x0, x1 ) );
    }
  }

  //! Template to square an object of Type T.
  /** \param t value to square
    * \returns product of \a t with itself */
  template<typename T> 
  inline T square( const T& t )
  {
    return( t * t );
  }

  /*! \brief Calculate the horizontal field of view out of the vertical field of view */
  inline float verticalToHorizontalFieldOfView( float vFoV, float aspectRatio )
  {
    return( 2.0f * atanf( tanf( 0.5f * vFoV ) * aspectRatio ) );
  }

  //! Compares two numerical values
  /** \returns -1 if \a lhs < \a rhs, 1 if \a lhs > \a rhs, and 0 if \a lhs == \a rhs. */
  template<typename T> 
  inline int compare(T lhs, T rhs)
  {
    return (lhs == rhs) ? 0 : (lhs < rhs) ? -1 : 1;
  }

  // specializations for float and double below

  //! Compares two float values
  /** \returns -1 if \a lhs < \a rhs, 1 if \a lhs > \a rhs, and 0 if \a lhs == \a rhs. */
  template<> 
  inline int compare(float lhs, float rhs)
  {
    return ((fabs(lhs-rhs) < FLT_EPSILON) ? 0 : (lhs < rhs) ? -1 : 1);
  }

  //! Compares two double values
  /** \returns -1 if \a lhs < \a rhs, 1 if \a lhs > \a rhs, and 0 if \a lhs == \a rhs. */
  template<> 
  inline int compare(double lhs, double rhs)
  {
    return ((fabs(lhs-rhs) < DBL_EPSILON) ? 0 : (lhs < rhs) ? -1 : 1);
  }
}

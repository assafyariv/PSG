#pragma once

#include "nvsggnudefs.h" // GNU compiler specific defines
#include "nvsg/nvsgapi.h"  // storage-class defines
#include "nvsg/nvsg.h"
#include <limits>

#if !defined(NDEBUG) && !defined(NVSG_HYBRID)

namespace nvutil
{
  typedef int (*UserAssertCallback)( const char * message );

  //! Set a user-defined assertion-failure callback.
  /** This function registers a user-defined routine to call in the event of an
   * assertion failure in SceniX.
   * \param cb A callback routine to call that is of type UserAssertCallback.
   * \return Returns the previous callback, if one was registered.
   * \note The user-defined callback should have a prototype:
   *   int callback( const char * message );
   * and should return zero to let SceniX simply return from the assertion failure
   * and not process it any further, or return non-zero to have SceniX continue with
   * assertion processing for the host OS.
   * IMPORTANT!
   * The standard behavior of assert() or _wassert() under Windows is to print 
   * the message to the stderr stream for console applications, and to display them
   * inside a MessageBox for Windows-based applications.
   * After the assert message has been presented resp. after the user has clicked OK
   * inside the MessageBox, the application aborts immediately!
   * If that is not desired, use a user-defined assert callback to change this behavior.
   *
   * An example callback function implementation which prints the assertion condition 
   * to the stderr output stream, breaks into the debugger, and returns zero to end the 
   * assertion handling, could look like this:
   *
   * int AssertWithBreak( const char *message )
   * {
   *   std::cerr << message << std::endl;
   * #if defined(_WIN32)
   *   DebugBreak();
   * #elif defined(LINUX)
   *   asm( "int $03;" );
   * #endif
   *   return 0;
   * }
   *
   */
  extern NVSG_API UserAssertCallback SetUserAssertCallback( UserAssertCallback cb );

  inline  NVSG_API void AssertionFailed(const char * expr, const char * file, unsigned int line)
   {
   }
} // namespace nvutil


// assert macro
#define NVSG_ASSERT(exp) \
  do { \
    if (exp){ \
    } else { \
      if ( nvsg::nvsgGetDebugFlags() & nvsg::NVSG_DBG_ASSERT ) { \
        nvutil::AssertionFailed(#exp, __FILE__, __LINE__); \
      } \
    } \
  } while( false )

// verify the expression f succeeds
#define NVSG_VERIFY(f) NVSG_ASSERT((f))

// NOTE: GNU compilers < 3.3.0 have serious problems with meta programing 
// techniques that we use e.g. to implement our compile-time assert 
#if defined(__GNUC__) && GCC_VERSION < 30300

#define NVSG_CTASSERT(exp)
#define NVSG_CTASSERT_BYTESIZE(type,size)
#define NVSG_CTASSERT_BYTESIZEi(type,size,ln)
#define NVSG_CTASSERT_BYTESIZEii(type,size,ln)
#define NVSG_CTASSERT_MODULO_BYTESIZE(type,size)
#define NVSG_CTASSERT_MODULO_BYTESIZEi(type,size,ln)
#define NVSG_CTASSERT_MODULO_BYTESIZEii(type,size,ln)
#define NVSG_CTASSERT_SAME(lhs,rhs)
#define NVSG_CTASSERT_SAMEi(lhs,rhs,ln)
#define NVSG_CTASSERT_SAMEii(lhs,rhs,ln)

#else 

// compile time assert
template <bool> class CompileTimeAssert;
template<> class CompileTimeAssert<true> {};
#define NVSG_CTASSERT(exp) sizeof(CompileTimeAssert<(exp)!=0>)

#define NVSG_CTASSERT_BYTESIZE(type,size) NVSG_CTASSERT_BYTESIZEi(type,size, __LINE__)
#define NVSG_CTASSERT_BYTESIZEi(type,size,ln) NVSG_CTASSERT_BYTESIZEii(type,size,ln)
#define NVSG_CTASSERT_BYTESIZEii(type,size,ln) \
  static CompileTimeAssert<sizeof(type)==size> error_at_line_##ln##__##type##_has_unexpected_size

#define NVSG_CTASSERT_MODULO_BYTESIZE(type,size) NVSG_CTASSERT_MODULO_BYTESIZEi(type,size, __LINE__)
#define NVSG_CTASSERT_MODULO_BYTESIZEi(type,size,ln) NVSG_CTASSERT_MODULO_BYTESIZEii(type,size,ln)
#define NVSG_CTASSERT_MODULO_BYTESIZEii(type,size,ln) \
  static CompileTimeAssert<sizeof(type)%size==0> error_at_line_##ln##__##type##_has_unexpected_size

#define NVSG_CTASSERT_SAME(lhs,rhs) NVSG_CTASSERT_SAMEi(lhs,rhs, __LINE__)
#define NVSG_CTASSERT_SAMEi(lhs,rhs,ln) NVSG_CTASSERT_SAMEii(lhs,rhs,ln)
#define NVSG_CTASSERT_SAMEii(lhs,rhs,ln) \
  static CompileTimeAssert<lhs==rhs> error_at_line_##ln##__##lhs##_and_##rhs##_differ


#endif

#else // non-debug configuration

#define NVSG_ASSERT(exp) static_cast<void>(0)
#define NVSG_VERIFY(f)  (static_cast<void>(f))
#define NVSG_CTASSERT(exp)
#define NVSG_CTASSERT_BYTESIZE(type,size)
#define NVSG_CTASSERT_BYTESIZEi(type,size,ln)
#define NVSG_CTASSERT_BYTESIZEii(type,size,ln)
#define NVSG_CTASSERT_MODULO_BYTESIZE(type,size)
#define NVSG_CTASSERT_MODULO_BYTESIZEi(type,size,ln)
#define NVSG_CTASSERT_MODULO_BYTESIZEii(type,size,ln)
#endif

template<typename TOut, typename TIn>
inline TOut checked_cast_integer( TIn in )
{
  // this is exclusively for TOut and Tin being integer
  NVSG_CTASSERT(std::numeric_limits<TOut>::is_integer && std::numeric_limits<TIn>::is_integer);

  TOut out = static_cast<TOut>(in);

#if !defined(NDEBUG)
  bool test = false;

  if ( std::numeric_limits<TIn>::digits == std::numeric_limits<TOut>::digits )
  {
    test = std::numeric_limits<TIn>::is_signed == std::numeric_limits<TOut>::is_signed && (out == in);
  }
  else if ( std::numeric_limits<TIn>::digits < std::numeric_limits<TOut>::digits )
  {
    TOut m;

    if ( std::numeric_limits<TOut>::is_signed == std::numeric_limits<TIn>::is_signed )
    {
      m = ~0;
    }
    else
    {
      m = (TOut(1) << (std::numeric_limits<TOut>::digits-1));
      if ( std::numeric_limits<TOut>::is_signed ) 
      {
        m <<= 1;
      }
      m = ~m;
    }

    test = ((out & m) == in);
  }
  else 
  {
    TIn m;

    if ( std::numeric_limits<TOut>::is_signed == std::numeric_limits<TIn>::is_signed )
    {
      m = ~0;
    } 
    else {
      m = (TIn(1) << (std::numeric_limits<TIn>::digits-1));
      if ( std::numeric_limits<TIn>::is_signed )
      {
        m <<= 1;
      }
      m = ~m;
    } 

    test = ((in & m) == out);
  }

  NVSG_ASSERT(test && "Invalid conversion detected!");
#endif

  return out;
}

// general implementation handle non-pure integer cases
template <typename TOut, typename TIn, bool integer>
struct Caster
{
  TOut operator()(TIn in) { 
    // more checks on non-pure integer to be done
    return static_cast<TOut>(in); 
  }
};

// specialization for pure-integer conversion
template <typename TOut, typename TIn>
struct Caster<TOut, TIn, true>
{
  TOut operator()(TIn in) { return checked_cast_integer<TOut>(in); }
};

template<typename TOut, typename TIn>
inline TOut checked_cast( TIn in )
{
  Caster<TOut, TIn, std::numeric_limits<TOut>::is_integer && std::numeric_limits<TIn>::is_integer> theCaster;
  return theCaster(in);
}


#if defined(NVSG_PRIVATE)
# define NVSG_PRIVATE_ASSERT(exp)   NVSG_ASSERT(exp)
#else
# define NVSG_PRIVATE_ASSERT(exp)   static_cast<void>(0)
#endif

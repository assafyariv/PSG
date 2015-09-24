// Copyright NVIDIA Corporation 2011
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

#include <nvutil/Allocator.h>
#include <list>
#include <map>
#include <set>
#include <vector>
#include <stddef.h>

namespace nvutil
{
#if defined(WIN32)
  template <typename T> class STLAllocator;

  template <> class STLAllocator<void>
  {
    public:
      typedef void* pointer;
      typedef const void* const_pointer;
      // reference to void members are impossible.
      typedef void value_type;
      template <class U> struct rebind { typedef STLAllocator<U> other; };
  };    

  template <typename T>
  class STLAllocator
  {
    public:
      typedef size_t size_type;
      typedef ptrdiff_t difference_type;
      typedef T* pointer;
      typedef const T* const_pointer;
      typedef T& reference;
      typedef const T& const_reference;
      typedef T value_type;

      template <class U> 
          struct rebind { typedef STLAllocator<U> other; };

      STLAllocator()                                              {}
      template <class U> STLAllocator( const STLAllocator<U> & )  {}
      ~STLAllocator()                                             {}

      pointer       address( reference x ) const        { return( & x ); }
      const_pointer address( const_reference x ) const  { return( & x) ; }
      size_type     max_size() const throw()            { return( size_t(-1) / sizeof(value_type) ); }

      pointer allocate( size_type size, STLAllocator<void>::const_pointer hint = 0 )
      {
        return( static_cast<pointer>(nvutil::Singleton<nvutil::Allocator>::instance()->alloc( size * sizeof(T) )) );
      }

      void deallocate( pointer p, size_type n )
      {
        nvutil::Singleton<nvutil::Allocator>::instance()->dealloc( p, n * sizeof(T) );
      }

      void deallocate(void *p, size_type n)
      {
        nvutil::Singleton<nvutil::Allocator>::instance()->dealloc( p, n * sizeof(T) );
      }

      void construct( pointer p, const T & val )
      {
        new(static_cast<void*>(p)) T(val);
      }

      void construct( pointer p )
      {
        new(static_cast<void*>(p)) T();
      }

      void destroy( pointer p )
      {
        p->~T();
      }
  };

  template <typename T, typename U>
  inline bool operator==( const STLAllocator<T> &, const STLAllocator<U> & )  { return( true ); }

  template <typename T, typename U>
  inline bool operator!=( const STLAllocator<T> &, const STLAllocator<U> & )  { return( false ); }


  template <typename T>
  class STLList : public std::list<T,STLAllocator<T> >
  {
  };

  template <typename S, typename T>
  class STLMap : public std::map<S,T,std::less<S>,STLAllocator<std::pair<S,T> > >
  {
  };

  template <typename S, typename T>
  class STLMultimap : public std::multimap<S,T,std::less<S>,STLAllocator<std::pair<S,T> > >
  {
  };

  template <typename T>
  class STLMultiset : public std::multiset<T,std::less<T>, STLAllocator<T> >
  {
  };

  template <typename T>
  class STLSet : public std::set<T,std::less<T>,STLAllocator<T> >
  {
  };

  template <typename T>
  class STLVector : public std::vector<T,STLAllocator<T> >
  {
  };
#elif defined(LINUX)
  template <typename T>
  class STLList : public std::list<T>
  {
  };

  template <typename S, typename T>
  class STLMap : public std::map<S,T>
  {
  };

  template <typename S, typename T>
  class STLMultimap : public std::multimap<S,T>
  {
  };

  template <typename T>
  class STLMultiset : public std::multiset<T>
  {
  };

  template <typename T>
  class STLSet : public std::set<T>
  {
  };

  template <typename T>
  class STLVector : public std::vector<T>
  {
  };
#endif
} // namespace nvutil

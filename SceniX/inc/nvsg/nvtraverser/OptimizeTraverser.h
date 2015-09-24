// Copyright NVIDIA Corporation 2002-2011
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

#include <list>
#include "nvsgcommon.h"
#include "nvsg/Node.h"
#include "nvtraverser/Traverser.h"

namespace nvtraverser
{

  //! Base for Traversers that perform some optimizations on a scene.
  class OptimizeTraverser : public ExclusiveTraverser
  {
    public:
      //! Constructor
      NVSG_API OptimizeTraverser( void );

    public:
      //! Get the 'ignore names' flag.
      /** If the 'ignore names' flag is set, the names of the GeoNodes are ignored.
        * \return true if the names will be ignored, otherwise false */
      NVSG_API bool getIgnoreNames( void ) const;

      //! Set the 'ignore names' flags.
      /** If the 'ignore names' flag is set, the names of the GeoNodes are ignored. */
      NVSG_API void setIgnoreNames( bool ignore   //!<  set true to ignore names on comparison
                                  );
      REFLECTION_INFO_API( NVSG_API, OptimizeTraverser );
      BEGIN_DECLARE_STATIC_PROPERTIES
          NVSG_API DECLARE_STATIC_PROPERTY( IgnoreNames );
      END_DECLARE_STATIC_PROPERTIES
    protected:
      //! returns whether we can/should optimize a given object.
      //  currently, if the node is not marked "DYNAMIC", and it doesn't have
      //  any callbacks registered, we say it is optimizable
      NVSG_API virtual bool optimizationAllowed( const nvsg::Object * obj );

      //! Protected destructor to prevent instantiation of a TreeOptimizeTraverser on stack.
      NVSG_API virtual ~OptimizeTraverser( void );

      //! doApply override
      NVSG_API virtual void doApply( const nvsg::NodeSharedPtr & root );

    private:
      bool  m_ignoreNames;
  };

  inline bool OptimizeTraverser::getIgnoreNames( void ) const
  {
    return( m_ignoreNames );
  }

  inline void OptimizeTraverser::setIgnoreNames( bool ignore )
  {
    if ( m_ignoreNames != ignore )
    {
      m_ignoreNames = ignore;
      notify( PID_IgnoreNames );
    }
  }

}


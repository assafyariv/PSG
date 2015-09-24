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

#include <nvgl/GLContext.h>
#include "RenderContextGLFormat.h"
#include <stack>

namespace nvgl
{

  class RenderContextGL;
  typedef nvutil::SmartPtr< RenderContextGL > SmartRenderContextGL;

  class ShareGroupGL;
  CORE_TYPES( ShareGroupGL, nvutil::HandledObject );

  class ShareGroupGLImpl;

  /** \brief RenderContextGL wraps an OpenGL context. 
  **/
  class RenderContextGL : public nvutil::RCObject
  {
  protected:
    class NativeContext;
    typedef nvutil::SmartPtr<NativeContext> SmartNativeContext;

  public:
    // Constructor parameters
    struct Attach
    {
      Attach()
      {
      }

      Attach( const SmartRenderContextGL &pSharedContext )
        : m_shared(pSharedContext)
      {
      }

      Attach( const Attach &rhs )
       : m_shared( rhs.m_shared )
      {
      }

      ~Attach()
      {
      }

      SmartRenderContextGL getContext() const
      {
        return( m_shared );
      }

    private:
      // prohibit assignment
      Attach &operator=( const Attach & );

      SmartRenderContextGL  m_shared;
    };

    struct Clone
    {
      Clone( const SmartRenderContextGL &pContext, bool pShare )
      : m_context( pContext )
      , m_share(pShare)
      {
      }

      Clone( bool pShare )
        : m_share(pShare)
      {
      }

      Clone( const Clone &rhs )
       : m_context( rhs.m_context )
       , m_share( rhs.m_share )
      {
      }

      ~Clone()
      {
      }

      SmartRenderContextGL getContext() const
      {
        return( m_context );
      }

      bool isShared() const
      {
        return( m_share );
      }

    private:
      // prohibit assignment
      Clone &operator=( const Clone & );

      SmartRenderContextGL  m_context;
      bool                  m_share;
    };

    struct Headless
    {
      Headless( RenderContextGLFormat *pFormat, const SmartRenderContextGL &pShared )
        : m_format(pFormat)
        , m_shared(pShared)
#if defined(LINUX)
        , m_display(0)
        , m_screen(0)
#endif
      {
      }

#if defined(LINUX)
      Headless( RenderContextGLFormat *pFormat, const char *pDisplay = 0, int pScreen = 0 )
        : m_format(pFormat)
        , m_display(pDisplay)
        , m_screen(pScreen)
      {
      }
#endif

      Headless( const Headless &rhs )
      : m_format( rhs.m_format)
      , m_shared( rhs.m_shared )
#if defined(LINUX)
      , m_display( rhs.m_display )
      , m_screen( rhs.m_screen )
#endif
      {
      }

      ~Headless()
      {
      }

      const RenderContextGLFormat * getFormat() const
      {
        return( m_format );
      }

      SmartRenderContextGL getContext() const
      {
        return( m_shared );
      }

#if defined(LINUX)
      const char * getDisplay() const
      {
        return( m_display );
      }

      int getScreen() const
      {
        return( m_screen );
      }
#endif

    private:
      // prohibit assignment
      Headless &operator=( const Headless & );

      RenderContextGLFormat * m_format;
      SmartRenderContextGL    m_shared;
#if defined(LINUX)
      const char *            m_display;
      int                     m_screen;
#endif
    };

#if defined( WIN32 )
    struct FromHDC
    {
      FromHDC( HDC pHdc, const SmartRenderContextGL &pShared )
        : m_hdc(pHdc)
        , m_shared(pShared)
      {
      }

      FromHDC( const FromHDC &rhs )
       : m_hdc( rhs.m_hdc )
       , m_shared( rhs.m_shared )
      {
      }

      ~FromHDC()
      {
      }

      HDC getHDC() const
      {
        return( m_hdc );
      }

      SmartRenderContextGL getContext() const
      {
        return( m_shared );
      }

    private:
      // prohibit assignment
      FromHDC &operator=( const FromHDC & );

      HDC                   m_hdc;
      SmartRenderContextGL  m_shared;
    };

    struct FromHWND
    {
      FromHWND( HWND pHwnd, RenderContextGLFormat *pFormat, const SmartRenderContextGL &pShared )
        : m_hwnd(pHwnd)
        , m_format(pFormat)
        , m_shared(pShared)
      {
      }

      FromHWND( const FromHWND &rhs )
        : m_hwnd( rhs.m_hwnd )
        , m_format( rhs.m_format )
        , m_shared( rhs.m_shared )
      {
      }

      ~FromHWND()
      {
      }

      HWND getHWND() const
      {
        return( m_hwnd );
      }

      const RenderContextGLFormat * getFormat() const
      {
        return( m_format );
      }

      SmartRenderContextGL getContext() const
      {
        return( m_shared );
      }

    private:
      // prohibit assignment
      FromHWND &operator=( const FromHWND & );

      HWND                    m_hwnd;
      RenderContextGLFormat * m_format;
      SmartRenderContextGL    m_shared;
    };
#endif

#if defined(LINUX)
    struct FromDrawable
    {
      FromDrawable( Display *pDisplay, int pScreen, GLXDrawable pDrawable, const SmartRenderContextGL &pShared )
        : display(pDisplay)
        , screen( pScreen )
        , drawable(pDrawable)
        , shared(pShared)
      {
      }

      FromDrawable( const FromDrawable &rhs)
      : display( rhs.display )
      , screen( rhs.screen )
      , drawable( rhs.drawable )
      , shared( rhs.shared )
      {
      }

      Display *display;
      int screen;
      GLXDrawable drawable;
      SmartRenderContextGL shared;
    private:
      // prohibit assignment
      FromDrawable &operator=( const FromDrawable & );
    };
#endif

  protected:
    // abstract base class
    NVSG_API RenderContextGL();

    NVSG_API RenderContextGL( const SmartNativeContext &nativeContext, const ShareGroupGLSharedPtr &shareGroupGL );
    NVSG_API RenderContextGL( const SmartNativeContext &nativeContext, const ShareGroupGLSharedPtr &shareGroupGL, const RenderContextGLFormat &format );
    NVSG_API RenderContextGL( const Attach &creation );
    NVSG_API RenderContextGL( const Headless &headless);
    NVSG_API RenderContextGL( const Clone &creation );
#if defined(LINUX)
    NVSG_API RenderContextGL( const FromDrawable &creation );
#endif
    // no copy
    NVSG_API RenderContextGL(const RenderContextGL &);
    NVSG_API RenderContextGL &operator=(const RenderContextGL &);

    NVSG_API virtual ~RenderContextGL();

  public:
    NVSG_API static SmartRenderContextGL create( const Attach &creation );
    NVSG_API static SmartRenderContextGL create( const Clone &creation );
    NVSG_API static SmartRenderContextGL create( const Headless &creation );
#if defined(WIN32)
    NVSG_API static SmartRenderContextGL create( const FromHDC &creation );
    NVSG_API static SmartRenderContextGL create( const FromHWND &creation );
#elif defined(LINUX)
    NVSG_API static SmartRenderContextGL create( const FromDrawable &creation );
#endif

    // may introduce locking to ensure that context is been used only by one thread at the same time
    NVSG_API bool makeCurrent();

    /* Not sure if we want to have the uncurrent calls. They could be useful for multithreaded programs.
       Unfortunately it seems they'll introduce a lot of new complexity which cannot be solved easily.
    **/
    // make context uncurrent
    NVSG_API void makeNoncurrent();

    NVSG_API static const SmartRenderContextGL & getCurrentRenderContextGL();
    NVSG_API ShareGroupGLSharedPtr getShareGroupGL() const;

    NVSG_API void swap();

    // call this before context is being destroyed
    NVSG_API virtual void notifyDestroy();

    NVSG_API const RenderContextGLFormat &getFormat() const;

#if defined(_WIN32)
    NVSG_API HWND  getHWND()  const;
    NVSG_API HDC   getHDC()   const;
    NVSG_API HGLRC getHGLRC() const;

#elif defined(LINUX)
    GLXContext  getContext()  const;
    GLXDrawable getDrawable() const;
    Display*    getDisplay()  const;
#endif

  protected:
    // clone a context
#if defined(LINUX)
    NVSG_API void clone( GLXContext context, bool share );
#elif defined(_WIN32)
    NVSG_API void clone( HDC hdc, HGLRC shareContext );
#endif

  protected:
    // platform specific variables and functions
  #if defined (_WIN32)
    NVSG_API static LRESULT CALLBACK RenderContextGLWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    // create a native invisible dummy window
    NVSG_API static HWND createWindow();
    NVSG_API static HGLRC createContext( HDC hdc, HGLRC shareContext );
  #elif defined(LINUX)
    NVSG_API static GLXContext createContext( Display *display, GLXFBConfig config, GLXContext shareContext );
    NVSG_API static GLXPbuffer createPbuffer( Display *display, GLXFBConfig config );
  #endif
    NVSG_API static ShareGroupGLSharedPtr createShareGroupGL( const RenderContextGL::SmartNativeContext &context );

    /** Hold a native OGL context **/
    class NativeContext : public RCObject
    {
    public:
#if defined(WIN32)
      NVSG_API NativeContext( HWND hwnd, bool destroyHWND, HDC hdc, bool destroyHDC, HGLRC hglrc, bool destroyHGLRC );
#elif defined(LINUX)
      NVSG_API NativeContext( GLXContext context, bool destroyContext, GLXDrawable drawable, bool destroyDrawable, GLXPbuffer pbuffer, bool destroyPbuffer, Display *display, bool destroyDisplay );
#endif
      NVSG_API virtual ~NativeContext();

      NVSG_API bool makeCurrent();
      NVSG_API void makeNoncurrent();
      NVSG_API void swap();

      NVSG_API void notifyDestroyed();

#if defined(_WIN32)
      HDC   m_hdc;
      HWND  m_hwnd;
      HGLRC m_hglrc;

      bool m_destroyHDC;
      bool m_destroyHWND;
      bool m_destroyHGLRC;
#elif defined(LINUX)
      GLXContext  m_context;
      GLXDrawable m_drawable;
      GLXPbuffer  m_pbuffer;
      Display*    m_display;

      bool        m_destroyContext;
      bool        m_destroyDrawable;
      bool        m_destroypBuffer;
      bool        m_destroyDisplay;
#endif
    };

    RenderContextGLFormat m_format;
    ShareGroupGLSharedPtr m_shareGroup;
    SmartNativeContext    m_context;

    friend class ShareGroupGL;
    friend class ShareGroupGLImpl;
  };

  class RenderContextGLStack {
  public:
    /* \brief Push the current RenderContextGL on the stack and make a the given RenderContextGL object context active.
     * \param renderContextGL The RenderContextGL which should be active
     */
    NVSG_API void push( const SmartRenderContextGL &renderContextGL );

    /* \brief Make the RenderContextGl object at the top of the stack active and remove it from the stack.
     */
    NVSG_API void pop();

    /* \brief Check if there are elements on the context stack
       \return true if there is an element on the context stack, false otherwise
       **/
    NVSG_API bool empty() const;
  private:
    struct StackEntry {
      NVSG_API StackEntry();
      NVSG_API StackEntry( const StackEntry &rhs );
#if defined(_WIN32)
      HDC   hdc;
      HWND  hwnd;
      HGLRC hglrc;
#elif defined(LINUX)
      GLXContext  context;
      GLXDrawable drawable;
      Display*    display;
#endif
      SmartRenderContextGL renderContextGL;
    };
    std::stack<StackEntry> m_stack;
  };

  /**********************/
  /* ShareGroup classes */
  /**********************/

  class ShareGroupGLResourceHolder;
  typedef nvutil::SmartPtr<ShareGroupGLResourceHolder> ShareGroupGLResource;
  
  /** \brief A ShareGroupTask is a small task which can be executed in the GL thread of a ShareGroup.
  **/
  // FIXME does it make sense to introduce a generic task function?
  class ShareGroupGLTask : public nvutil::RCObject
  {
  public:
    NVSG_API virtual ~ShareGroupGLTask() {};

    /** \brief This function is being called by the ShareGroup.
        \remarks At the time this function is being called a GL context of the ShareGroup is being active.
    **/
    NVSG_API virtual void execute() = 0;
  };

  typedef nvutil::SmartPtr<ShareGroupGLTask> SmartShareGroupGLTask;

  class ShareGroupGLResourceHolder;
  typedef nvutil::SmartPtr<ShareGroupGLResourceHolder> SmartShareGroupGLResource;

  class ShareGroupGL : public nvutil::HandledObject
  {
  public:
    NVSG_API static ShareGroupGLSharedPtr create( const RenderContextGL::SmartNativeContext &nativeContext );

    NVSG_API SmartShareGroupGLResource registerResource( size_t key, unsigned int type, const nvutil::SmartRCObject &resource );
    NVSG_API SmartShareGroupGLResource getResource( size_t key, unsigned int type );

    NVSG_API void executeTask( const SmartShareGroupGLTask &task, bool async = true );

  protected:
    friend class ShareGroupGLResourceHolder;
    
    friend struct nvutil::Holder<ShareGroupGL>;
    NVSG_API ShareGroupGL( const RenderContextGL::SmartNativeContext &nativeContext );
    NVSG_API ShareGroupGL( const ShareGroupGL & );
    NVSG_API virtual ~ShareGroupGL();

    /** \brief Key used for ResourceMap **/
    struct Key
    {
      Key( size_t key, unsigned int type )
        : m_key(key)
        , m_type(type)
      {
      }

      bool operator<(const Key &rhs) const
      {
        return m_key < rhs.m_key || (m_key == rhs.m_key && m_type < rhs.m_type);
      }

      size_t       m_key;
      unsigned int m_type;
    };

    void unregisterResource( const ShareGroupGL::Key &key );

    typedef nvutil::STLMap<Key, std::pair<int, ShareGroupGLResourceHolder*> > ResourceMap;
    ResourceMap m_resources;

    RenderContextGL::SmartNativeContext m_nativeContext;  // headless context for resource cleanup

    ShareGroupGLImpl *m_impl;
  };

  /** \brief A ShareGroupGLResourceHolder holds a reference to a Resource in a ShareGroupGL. It'll keep the
             used count of the resource always up to date.
  **/
  class ShareGroupGLResourceHolder : public nvutil::RCObject
  {
  public:
    NVSG_API ShareGroupGLResourceHolder( const ShareGroupGLSharedPtr &shareGroup, ShareGroupGL::Key key, const nvutil::SmartRCObject &resource );
    NVSG_API ShareGroupGLResourceHolder( const ShareGroupGLResourceHolder &rhs );
    NVSG_API virtual ~ShareGroupGLResourceHolder( );

    NVSG_API nvutil::SmartRCObject getResource();
  protected:
    ShareGroupGLResourceHolder &operator=( const ShareGroupGLResourceHolder &rhs );

    ShareGroupGLSharedPtr m_shareGroup;
    ShareGroupGL::Key     m_key;
    nvutil::SmartRCObject m_resource;
  };
} // namespace nvgl

namespace nvutil
{
  OBJECT_TRAITS( nvgl::ShareGroupGL, nvutil::HandledObject );
}

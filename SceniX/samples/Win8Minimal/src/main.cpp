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


// ##### INCLUDES

#include <windows.h>

#include <nvsg/CoreTypes.h>
#include <nvsg/nvsg.h>

#include <nvsg/ViewState.h>
#include <nvd3d/RenderContextD3D.h>
#include <nvd3d/SceneRendererD3D.h>
#include <nvd3d/RenderTargetD3D.h>

#include "SceneFunctions.h"
#include "SimpleScene.h"

#include "resource.h"

// ##### VARIABLES

char szAppName[]  = "SceniX_WinMinimal";
char szAppTitle[] = "SceniX WinMinimal";

HINSTANCE hInstMain;
HACCEL    hAccel;

nvgl::SmartRenderTargetGL   g_renderTargetGL;    // RenderTarget for SceneRenderer
nvgl::SmartSceneRendererGL2 g_sceneRendererGL2;  // OpenGL based renderer

// Initial client window size.
int nWidth  = 512;
int nHeight = 512;

// ##### PROTOTYPES

int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow);

LRESULT CALLBACK WndProc(HWND   hWnd,
                         UINT   message,
                         WPARAM wParam,
                         LPARAM lParam);

BOOL InitApplication(HINSTANCE hInstance);

BOOL InitInstance(HINSTANCE hInstance,
                  int       nCmdShow);

void ParseCommandLine(LPSTR lpCmdLine);

void OGL_Display(HDC hdc);


// ##### FUNCTIONS

// Small helper function to find the exact size of a window for a given client region.
void ClientSizeToWindowSize(int wClient, int hClient, 
                            int *wWindow, int *hWindow)
{
  *wWindow = wClient + GetSystemMetrics(SM_CXSIZEFRAME) * 2;
  *hWindow = hClient + GetSystemMetrics(SM_CYSIZEFRAME) * 2 + 
                       GetSystemMetrics(SM_CYCAPTION);
}


int CALLBACK WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
  // Initialize SceniX
  nvsg::nvsgInitialize();

  // Fill in window class structure with parameters that describe the main window.
  WNDCLASS  wc;
  wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;  // Class style(s). 
  wc.lpfnWndProc   = (WNDPROC) WndProc;                   // Window Procedure 
  wc.cbClsExtra    = 0;                                   // No per-class extra data. 
  wc.cbWndExtra    = 0;                                   // No per-window extra data. 
  wc.hInstance     = hInstance;                           // Owner of this class 
  wc.hIcon         = NULL;                                // Icon 
  wc.hCursor       = LoadCursor(NULL, IDC_ARROW);         // Cursor 
  wc.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);         // Default color 
  wc.lpszMenuName  = NULL;                                // No Menu 
  wc.lpszClassName = szAppName;                           // Name to register as 

  // Register the window class and return success/failure code. 
  if (!RegisterClass(&wc))
  {
    return (FALSE);  // Exits if unable to initialize
  }

  // Save the instance handle in static variable, which will be used in 
  // many subsequence calls from this application to Windows. 
  hInstMain = hInstance; // Store instance handle in our global variable 

  // Calculate window outer dimension for given client size.
  int width; 
  int height;
  ClientSizeToWindowSize(nWidth, nHeight, &width, &height);

  // Create a main window for this application instance. 
  // The WM_CREATE event needs SceniX to be initialized already.
  HWND hWnd = CreateWindow(szAppName,            // See RegisterClass() call.   
                           szAppTitle,           // Text for window title bar.  
                           WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,  // Window style. (OpenGL requires the two clip ones!)   
                           CW_USEDEFAULT,
                           CW_USEDEFAULT,
                           width,
                           height,
                           NULL,                 // Overlapped windows have no parent.     
                           NULL,                 // Use the window class menu.             
                           hInstance,            // This instance owns this window.        
                           NULL);                // We don't use any data in our WM_CREATE 

  // If window could not be created, return "failure" 
  if (!hWnd)
  {
    return (FALSE);
  }

  // Make the window visible; update its client area.
  ShowWindow(hWnd, nCmdShow);  // Show the window 
  UpdateWindow(hWnd);          // Sends WM_PAINT message 

  // Currently only handling the ESCAPE key for exit.
  hAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDR_ACCELERATOR1));

  MSG msg;
  // Acquire and dispatch messages until a WM_QUIT message is received.
  while (GetMessage(&msg,     // message structure
                    NULL,     // handle of window receiving the message
                    0,        // lowest message to examine
                    0))       // highest message to examine
  {
    if (!TranslateAccelerator(msg.hwnd, hAccel, &msg))
    {
      TranslateMessage(&msg);   // Translates virtual key codes
      DispatchMessage(&msg);    // Dispatches message to window
    }
  }

  // reset the scene renderer and the render target objects
  // (otherwise you get memory leaks reported on nvsgTerminate() for these refcounted objects)
  g_sceneRendererGL2.reset();
  g_renderTargetGL.reset();

  // Terminate SceniX
  nvsg::nvsgTerminate();

  return (int) msg.wParam;    // Returns the value from PostQuitMessage
}


LRESULT CALLBACK WndProc(HWND   hWnd,
                         UINT   uMessage,
                         WPARAM wParam,
                         LPARAM lParam)
{
  switch (uMessage)
  {
  case WM_CREATE:
    {
      // create a SimpleScene object, containing a SceniX scene
      SimpleScene scene;
      // create a ViewState object, containing view specific state like a camera and a scene
      nvsg::ViewStateSharedPtr viewState = nvsg::ViewState::create();
      // pass the scene contained in the SimpleScene object to the ViewState object
      nvsg::ViewStateWriteLock(viewState)->setScene( scene.m_sceneHandle );
      // setup the view state with some default values, e.g. create a camera looking at the scene
      setupDefaultViewState(viewState);

      // create a render context with a standard format (RGBA8, DEPTH24) on our window.
      nvgl::RenderContextGLFormat format;
      nvgl::SmartRenderContextGL renderContextGL = nvgl::RenderContextGL::create( nvgl::RenderContextGL::FromHWND(hWnd, &format, 0) );

      // create a render target in the render context
      g_renderTargetGL = nvgl::RenderTargetGLFB::create( renderContextGL ); 
      // configure the render target to clear the color and depth buffer
      g_renderTargetGL->setClearMask( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
      
      // create a SceneRenderer for OpenGL
      g_sceneRendererGL2 = nvgl::SceneRendererGL2::create();
      // pass the render target and the view state to it
      g_sceneRendererGL2->setRenderTarget( g_renderTargetGL );
      g_sceneRendererGL2->setViewState( viewState );
    }
    return 0;

  case WM_ERASEBKGND: // Keep GDI from clearing the client region.
    return 1;         // Note the non-zero return!

  case WM_SIZE:
    g_renderTargetGL->setSize(LOWORD(lParam), HIWORD(lParam));
    return 0;

  case WM_PAINT:
    {
      PAINTSTRUCT ps;
      HDC hdc = BeginPaint(hWnd, &ps);

      g_sceneRendererGL2->render();

      SwapBuffers(hdc);
      EndPaint(hWnd, &ps);
      // Note: ValidateRect(hWnd, NULL); here would be the faster than 
      // BeginPaint-EndPaint if you know the HDC for the SwapBuffers().
    }
    return 0;

  case WM_COMMAND:
    if (LOWORD(wParam) == IDCANCEL)
    {
      PostMessage(hWnd, WM_DESTROY, 0, 0L);
    }
    return 0;

  case WM_DESTROY:  // message: window being destroyed 
    PostQuitMessage(0);
    break;
  }

  return (DefWindowProc(hWnd, uMessage, wParam, lParam));
}

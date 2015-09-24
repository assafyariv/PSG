#include "AvocadoManagedAPI.h"
#pragma once
 
#undef _AVENG
#pragma managed(push, off) 
#include <windows.h>
#include <string>
#include "../AvocadoEngine/AvocadoAppInterface.h"
#pragma comment(lib, "../Release/AvocadoEngine.lib") 
#pragma managed(pop)

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

namespace avocado {

	static bool needRepaint = false;

	CAvocadoViewInterface::CAvocadoViewInterface () {}
	CAvocadoViewInterface::~CAvocadoViewInterface () {}
	void CAvocadoViewInterface::InvokePaintView()
	{
		needRepaint = true;
	}

	void AvocadoManagedEngine::ConstructAvocado(System::Windows::Forms::CreateParams ^ cp,
											unsigned int iWidth, unsigned int iHeight)
	{
		// Set the position on the form
		cp->X = 0;
		cp->Y = 0;
		cp->Height = iHeight;
		cp->Width = iWidth;

		// Create as a child of the specified parent
		// and make OpenGL compliant (no clipping)
		cp->Style = WS_CHILD | WS_VISIBLE | 
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		// Create the actual window
		this->CreateHandle(cp);

		if (this->Handle.ToPointer())
		{
			SetupAvocado((void *)this->Handle.ToPointer(), iWidth ,iHeight );
		}
	}
			
	AvocadoManagedEngine::AvocadoManagedEngine(System::Windows::Forms::UserControl ^ parentForm, 
												unsigned int iWidth, unsigned int iHeight)
	{
		CreateParams^ cp = gcnew CreateParams;
		// Specify the form as the parent.
		cp->Parent = parentForm->Handle;
		m_width = iWidth;
		m_height = iHeight;
		ConstructAvocado(cp,iWidth,iHeight);
	}

	AvocadoManagedEngine::AvocadoManagedEngine(System::Windows::Forms::Form ^ parentForm, 
					unsigned int iWidth, unsigned int iHeight)
	{
		CreateParams^ cp = gcnew CreateParams;
		// Specify the form as the parent.
		cp->Parent = parentForm->Handle;
		m_width = iWidth;
		m_height = iHeight;
		ConstructAvocado(cp,iWidth,iHeight);
	}

	AvocadoManagedEngine::~AvocadoManagedEngine(System::Void)
	{
		this->DestroyHandle();
		AvocadoTerminate();
	}

	System::Void AvocadoManagedEngine::doRender(System::Void)
	{
		if (needRepaint)
		{
			OnSendAvocadoGeneralStringMessage("OnPaint",m_avoViewId,"");
			SwapOpenGLBuffers();
			needRepaint = false;
		}
	}
	
	System::Void AvocadoManagedEngine::CallPaint(System::Void)
	{
		needRepaint = true;
		doRender();
	
	}		
	System::Void AvocadoManagedEngine::SwapOpenGLBuffers(System::Void)
	{
		m_hDC = GetDC((HWND)this->Handle.ToPointer());
		if (m_hDC)
			SwapBuffers((HDC)m_hDC) ;
	}

	void  AvocadoManagedEngine::SetupAvocado (void * hwnd, unsigned int iWidth, unsigned int iHeight) 
	{
		AvocadoInit(); 
		m_avoDocId = OnCreateDoc();
		SetActiveDoc (m_avoDocId);
		HWND hWndx = (HWND)hwnd;
		m_AvoViewInter = new CAvocadoViewInterface ();
		m_avoViewId = OnCreateView((void*)(&hWndx), m_AvoViewInter);
		OnSerializeDoc(m_avoDocId, std::string ("C:\\Education\\SceniX72\\media\\scenes\\shader\\rasterize.nbf"),true);
		SetFocus(hWndx);
		m_animLoaded = false;
		m_animating = false;
	}

	void  AvocadoManagedEngine::SetAvocadoViewSize(unsigned int iWidth, unsigned int iHeight)
	{
		if (iWidth != m_width || iHeight != m_height && ((iWidth > 0) && (iHeight == 0)))
		{
			m_width = iWidth;
			m_height = iHeight;
		    HWND parent = (HWND)this->Handle.ToPointer(); 
		    SetWindowPos(parent,HWND_BOTTOM/*HWND_TOPMOST*/, 0, 0, iWidth, iHeight, 0/*SWP_DRAWFRAME*/) ;
		}
	}

	static void MarshalString ( String ^ s, std::string& os ) 
	{
	   using namespace Runtime::InteropServices;
	   const char* chars = 
		  (const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
	   os = chars;
	   Marshal::FreeHGlobal(IntPtr((void*)chars));
	}

	void AvocadoManagedEngine::LoadFileDialog(System::String ^fName) 
	{
		//String^ fNameStr = fName;
		//System::String str = fName;
		std::string msg;
		MarshalString(fName,msg);
		OnSerializeDoc(m_avoDocId,msg);
		m_animLoaded = false;
	}

	void AvocadoManagedEngine::PlayAnimation()
	{
		if (!m_animLoaded)
		{
			OnSendAvocadoGeneralStringMessage("OnLoadAnimation",m_avoViewId,"");
			m_animLoaded = true;
		}
		OnSendAvocadoGeneralStringMessage("OnPlayAnimation",m_avoViewId,"");
		m_animating = true;
		CallPaint();
	}
	void AvocadoManagedEngine::PauseAnimation()
	{
		OnSendAvocadoGeneralStringMessage("OnPauseAnimation",m_avoViewId,"");
		m_animating =! m_animating;
		if (m_animating && !m_animLoaded)
		{
			OnSendAvocadoGeneralStringMessage("OnLoadAnimation",m_avoViewId,"");
			m_animLoaded = true;
		}
		CallPaint();
	}
	void AvocadoManagedEngine::StopAnimation()
	{
		OnSendAvocadoGeneralStringMessage("OnStopAnimation",m_avoViewId,"");
		m_animating = false;
		CallPaint();
	}
	void AvocadoManagedEngine::ToggleHighLight()
	{
		OnSendAvocadoGeneralStringMessage("OnTogHighlights",m_avoViewId,"");
		CallPaint();
	}
	void AvocadoManagedEngine::ToggleHLR()
	{
		OnSendAvocadoGeneralStringMessage("OnTogHLR",m_avoViewId,"");
		CallPaint();
	}
	void AvocadoManagedEngine::ToggleCellShading()
	{
		OnSendAvocadoGeneralStringMessage("OnTogCellShading",m_avoViewId,"");
		CallPaint();
	}
	void AvocadoManagedEngine::ToggleOptix()
	{
		OnSendAvocadoGeneralStringMessage("OnTogUseOptix",m_avoViewId,"");
		CallPaint();
	}

	void  AvocadoManagedEngine::MouseLBDown(int x, int y, int flags)
	{
		OnSendAvocadoMouseStringMessage(AVC_MOUSE_LDOWN,m_avoViewId,x,y,flags);
		SetFocus((HWND)this->Handle.ToPointer());
	}

	void  AvocadoManagedEngine::MouseLBUp(int x, int y, int flags)
	{
		OnSendAvocadoMouseStringMessage(AVC_MOUSE_LUP,m_avoViewId,x,y,flags);
	}

	void AvocadoManagedEngine::MouseMBDown(int x, int y, int flags)
	{
		OnSendAvocadoMouseStringMessage(AVC_MOUSE_MDOWN,m_avoViewId,x,y,flags);
	}

	void  AvocadoManagedEngine::MouseMBUp(int x, int y, int flags)
	{
		OnSendAvocadoMouseStringMessage(AVC_MOUSE_MUP,m_avoViewId,x,y,flags);
	}

	void  AvocadoManagedEngine::MouseMove(int x, int y, int flags)
	{
		SetCursor(LoadCursor(NULL,IDC_ARROW)) ;
		OnSendAvocadoMouseStringMessage(AVC_MOUSE_MOVE,m_avoViewId,x,y,flags);
	}

	void  AvocadoManagedEngine::MouseWheel(int x, int y, int zDelta)
	{
		needRepaint = false;
		OnSendAvocadoMouseStringMessage(AVC_MOUSE_WHEEL,m_avoViewId,x,y,zDelta);
		doRender();
	}

	void AvocadoManagedEngine::TimerTick()
	{
		OnSendAvocadoMouseStringMessage(AVC_TIMER_TICK, m_avoViewId, 0, 0, 0);
		m_frameCount++;
		if (m_frameCount == 1000)
			m_frameCount = 0;
		if(m_animLoaded && m_animating)
		{
			CallPaint();
		}
	}

	int AvocadoManagedEngine::MakeLParam(int LoWord, int HiWord)  
	{  
		int i = (HiWord << 16) | (LoWord & 0xffff);  
		return i;  
	}  

	void AvocadoManagedEngine::WndProc( Message %m ) 
	{
		// Listen for operating system messages
		LPARAM lparap;
		WPARAM wparap;
		lparap = ((LPARAM)m.LParam.ToPointer());
		wparap = ((WPARAM)m.WParam.ToPointer());
		int x = LOWORD (lparap);
		int y = HIWORD (lparap);
		int zDelta = HIWORD (wparap);
		int mFlag = LOWORD (wparap);

		switch ( m.Msg )
		{
			case WM_SIZE:
				{
					needRepaint = false;
					OnSizeView(m_avoViewId,x,y);
					doRender ();
				}
				break;
			case WM_MOUSEMOVE :
				{
					needRepaint = false;
					MouseMove (x,y,mFlag);
					doRender ();
				}
				break;

			case WM_LBUTTONUP:
				{
					needRepaint = false;
					MouseLBUp (x,y,mFlag);
					doRender ();
				}
				break;
			case WM_LBUTTONDOWN:
				{
					needRepaint = false;
					MouseLBDown(x,y,mFlag);
					doRender ();
				}
				break;
			case WM_MBUTTONUP :
				{
					needRepaint = false;
					MouseMBUp(x,y,mFlag);
					doRender ();
				}
				break;
			case WM_MBUTTONDOWN :
				{
					needRepaint = false;
					MouseMBDown(x,y,mFlag);
					doRender ();
				}
				break;
	/*	
			case WM_MOUSEWHEEL :
				{
					needRepaint = false;
					MouseWheel(x,y,zDelta);
					doRender ();
				}
				break;
	*/
			default:
				break;
		}
		NativeWindow::WndProc( m );
	}		
}
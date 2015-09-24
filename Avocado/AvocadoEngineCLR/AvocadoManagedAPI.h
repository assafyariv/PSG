#pragma once
#include "../AvocadoEngine/AvocadoAppInterface.h"
namespace avocado {

	class CAvocadoViewInterface: public AvocadoViewInterface
	{
	public:
		CAvocadoViewInterface ();
		~CAvocadoViewInterface ();
		virtual void InvokePaintView();
		virtual void ViewParamChanged (const char *paramName,const char *value){}
		virtual void SelectionChanged ( int selectedElement ){} 
	};

	public ref class  AvocadoManagedEngine: public System::Windows::Forms::NativeWindow
	{
	public:
		AvocadoManagedEngine(System::Windows::Forms::UserControl ^ parentForm, 
								unsigned int iWidth, unsigned int iHeight);
		AvocadoManagedEngine(System::Windows::Forms::Form ^ parentForm, 
								unsigned int iWidth, unsigned int iHeight);
		virtual ~AvocadoManagedEngine(System::Void);
		virtual System::Void CallPaint(System::Void);
		

	private:
		void					*m_hDC;	/*HDC*/
		int						m_avoDocId;
		int						m_avoViewId;
		CAvocadoViewInterface	*m_AvoViewInter;
		int						m_width;
		int						m_height;
		bool					m_animLoaded;
		bool					m_animating;
		int						m_frameCount;
			
	protected:
		virtual void WndProc( System::Windows::Forms::Message %m ) override;
		System::Void SwapOpenGLBuffers(System::Void);
		virtual System::Void doRender(System::Void);
		void	ConstructAvocado(System::Windows::Forms::CreateParams ^ cp,
									unsigned int iWidth, unsigned int iHeight);
		void	SetupAvocado (void * hwnd, unsigned int x, unsigned int y);

		void	MouseLBDown(int x, int y, int flags);
		void	MouseLBUp(int x, int y, int flags);
		void	MouseMBDown(int x, int y, int flags);
		void	MouseMBUp(int x, int y, int flags);
		void	MouseMove(int x, int y, int flags);
		int		MakeLParam(int LoWord, int HiWord);

	public:
		void	MouseWheel(int x, int y, int zDelta);
		void	SetAvocadoViewSize(unsigned int iWidth, unsigned int iHeight);	
		void	LoadFileDialog(System::String ^fName); 
		void	PlayAnimation();
		void	PauseAnimation();
		void	StopAnimation();
		void	ToggleHighLight();
		void	ToggleHLR();
		void	ToggleCellShading();
		void	ToggleOptix();
		void	TimerTick();

	};
}
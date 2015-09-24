// AvocadoWFApp.cpp : main project file.

#include "stdafx.h"
#include "CalloutsAppForm.h"

using namespace AvocadoWFApp;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	// Create the main window and run it
//	Application::Run(gcnew Form1());
	Application::Run(gcnew CalloutsAppForm());
	return 0;
}

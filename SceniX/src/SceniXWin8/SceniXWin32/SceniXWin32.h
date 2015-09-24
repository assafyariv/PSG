// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SCENIXWIN32_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SCENIXWIN32_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef SCENIXWIN32_EXPORTS
#define SCENIXWIN32_API __declspec(dllexport)
#else
#define SCENIXWIN32_API __declspec(dllimport)
#endif

// This class is exported from the SceniXWin32.dll
class SCENIXWIN32_API CSceniXWin32 {
public:
	CSceniXWin32(void);
	// TODO: add your methods here.
};

extern SCENIXWIN32_API int nSceniXWin32;

SCENIXWIN32_API int fnSceniXWin32(void);

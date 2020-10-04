#ifdef _WIN32

#include "_Module/Module.h"

#include <windows.h>

//-------------------------------------------------------------------------

BOOL WINAPI DllMain( _In_  HINSTANCE hinstDLL, _In_  DWORD fdwReason, _In_  LPVOID lpvReserved )
{
    return TRUE;
}

extern "C" __declspec( dllexport ) KRG::ResourceCompilerModule* CreateModule()
{
    return KRG::New<KRG::Render::ResourceCompilerModule>();
}
#endif
#pragma once

#ifndef DLL_EXPORT
#define DLL_EXPORT __declspec(dllexport)
#endif //DLL_EXPORT

extern HMODULE g_LocalDllHandle;

#ifdef __cplusplus
extern "C"
{
#endif

	DLL_EXPORT bool pluginit(PLUG_INITSTRUCT *InitStruct);
	DLL_EXPORT bool plugstop();
	DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT *SetupStruct);

#ifdef __cplusplus
}
#endif
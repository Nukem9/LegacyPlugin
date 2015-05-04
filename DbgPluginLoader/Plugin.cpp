#include "stdafx.h"

int g_MenuHandle;
int g_PluginHandle;

HMODULE g_LocalDllHandle;

bool IDARunDllEvent(duint DllBase, duint DllSize);

void LoadDllCallback(CBTYPE Type, PLUG_CB_LOADDLL *Info)
{
	IDARunDllEvent(Info->modInfo->BaseOfImage, Info->modInfo->ImageSize);
}

void MenuEntryCallback(CBTYPE Type, PLUG_CB_MENUENTRY *Info)
{
	switch (Info->hEntry)
	{
	}
}

DLL_EXPORT bool pluginit(PLUG_INITSTRUCT *InitStruct)
{
	InitStruct->pluginVersion	= PLUGIN_VERSION;
	InitStruct->sdkVersion		= PLUG_SDKVERSION;
	g_PluginHandle				= InitStruct->pluginHandle;
	strcpy_s(InitStruct->pluginName, PLUGIN_NAME);

	// Add any of the callbacks
	_plugin_registercallback(g_PluginHandle, CB_LOADDLL,	(CBPLUGIN)LoadDllCallback);
	_plugin_registercallback(g_PluginHandle, CB_MENUENTRY,	(CBPLUGIN)MenuEntryCallback);
	return true;
}

DLL_EXPORT bool plugstop()
{
	// Clear the menu
	_plugin_menuclear(g_MenuHandle);

	// Remove callbacks
	_plugin_unregistercallback(g_PluginHandle, CB_MENUENTRY);
	return true;
}

HMODULE LoadIDAPlugin(const char *Path);
bool IDASetupPlugin(HMODULE Library);

DLL_EXPORT void plugsetup(PLUG_SETUPSTRUCT *SetupStruct)
{
	g_MenuHandle = SetupStruct->hMenu;

	// Initialize the menu
	_plugin_menuaddentry(g_MenuHandle, 0, "&About");

	_plugin_printf("Loading IDA plugin...\n");
	HMODULE mod = LoadIDAPlugin("C:\\Users\\REDACTED\\Desktop\\findcrypt2\\findcrypt.plw");
	IDASetupPlugin(mod);
}

BOOL APIENTRY DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
		g_LocalDllHandle = hinstDLL;

	return TRUE;
}
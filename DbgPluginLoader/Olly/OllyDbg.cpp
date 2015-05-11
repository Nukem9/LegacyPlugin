#include "../stdafx.h"

ModuleExport OllyDbgExports[] =
{
	END_EXPORTS(),
};

bool LoaderResolveImport(const char *Dll, const char *Name, ULONG_PTR *Import)
{
	HMODULE module = GetModuleHandle(Dll);

	// Was the DLL loaded previously?
	if (!module)
		module = LoadLibrary(Dll);

	// If LoadLibrary/GetProcAddress fails, use the custom IDA import table
	FARPROC proc = nullptr;

	if (!module)
	{
		for (int i = 0; i < ARRAYSIZE(OllyDbgExports); i++)
		{
			// Check the name (Case does matter)
			if (strcmp(Name, OllyDbgExports[i].Name) != 0)
				continue;

			// Set the new pointer
			proc = (FARPROC)OllyDbgExports[i].Address;
			break;
		}
	}
	else
	{
		// Resolve the import with GetProcAddress
		proc = GetProcAddress(module, Name);
	}

	if (Import)
		*Import = (ULONG_PTR)proc;

	return (proc != nullptr);
}

HMODULE LoadOllyPlugin(const char *Path)
{
	// Load the library with imports resolved
	HMODULE library = LoadPluginDll(Path, LoaderResolveImport, true);

	if (!library)
	{
		_plugin_printf("Not loading plugin: Invalid library!\n");
		return nullptr;
	}

	// Find the exported ordinal #1 for IDA
	plugin_t *pluginStruct = (plugin_t *)GetProcAddress(library, MAKEINTRESOURCE(1));

	if (!pluginStruct)
	{
		_plugin_printf("Not loading plugin: Initialization export structure not found.\n");
		return nullptr;
	}

	// Validate function pointers
	if (!pluginStruct->init || IsBadReadPtr(pluginStruct, 1) != 0)
	{
		_plugin_printf("Not loading plugin: Bad initialization function pointer.\n");
		return nullptr;
	}

	// Create the internal data structure and call init()
	IDAPlugin plugin;

	plugin.Library		= library;
	plugin.ExternalData = pluginStruct;
	plugin.LoadFlags	= pluginStruct->init();

	if (plugin.LoadFlags == PLUGIN_SKIP)
	{
		_plugin_printf("Not loading plugin: Requested to be skipped.\n");
		return false;
	}

	// Everything set up correctly, add it to the global list
	return library;
}
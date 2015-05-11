#include "../stdafx.h"

BYTE inf[8192];

idaman void ida_export sistack_t_flush(void *ss)
{
}

ModuleExport IDAExports[] =
{
	{ "_qsnprintf", qsnprintf },

	// Kernel
	WRAP_EXPORT(verror),

	// DB
	{ "_inf", &inf },

	// UI
	{ "_callui", &callui },
	{ "callui", &callui },

	// Bytes
	WRAP_EXPORT(nextaddr),
	WRAP_EXPORT(prevaddr),
	WRAP_EXPORT(get_byte),
	WRAP_EXPORT(get_db_byte),
	WRAP_EXPORT(get_word),
	WRAP_EXPORT(get_3byte),
	WRAP_EXPORT(get_long),
	WRAP_EXPORT(get_qword),
	WRAP_EXPORT(get_full_byte),
	WRAP_EXPORT(get_full_word),
	WRAP_EXPORT(get_full_long),
	WRAP_EXPORT(get_8bit),
	WRAP_EXPORT(get_ascii_char),
	WRAP_EXPORT(get_16bit),
	WRAP_EXPORT(get_32bit),
	WRAP_EXPORT(get_64bit),
	WRAP_EXPORT(get_many_bytes),

	// Name
	WRAP_EXPORT(do_name_anyway),

	// Moves
	WRAP_EXPORT(curloc_mark),
	WRAP_EXPORT(curloc_markedpos),
	WRAP_EXPORT(curloc_markdesc),

	// Loader
	WRAP_EXPORT(hook_to_notification_point),
	WRAP_EXPORT(unhook_from_notification_point),

	// ?
	WRAP_EXPORT(sistack_t_flush),

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
		for (int i = 0; i < ARRAYSIZE(IDAExports); i++)
		{
			// Check the name (Case does matter)
			if (strcmp(Name, IDAExports[i].Name) != 0)
				continue;

			// Set the new pointer
			proc = (FARPROC)IDAExports[i].Address;
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

std::vector<IDAPlugin> Plugins;

HMODULE LoadIDAPlugin(const char *Path)
{
	// temp
	callui = kernel_callui;

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
	Plugins.push_back(plugin);
	return library;
}

idaman THREAD_SAFE int qsnprintf(char *buffer, size_t n, const char *format, ...)
{
	va_list va;
	va_start(va, format);
	return qsnprintf_args(buffer, n, format, va);
}

int qsnprintf_args(char *buffer, size_t n, const char *format, va_list args)
{
	/*
	https://hex-rays.com/products/ida/support/sdkdoc/group__qsnprintf.html

	Our definitions of sprintf-like functions support one additional format specifier
	"%a" which corresponds to ea_t
	*/
	auto strrep = [](const char *Subject, const char *Needle, const char *Replace)
	{
		size_t pos = 0;

		std::string output(Subject);
		std::string needle(Needle);

		while ((pos = output.find(needle, pos)) != std::string::npos)
		{
			output.replace(pos, needle.length(), Replace);
			pos += strlen(Replace);
		}

		return output;
	};

	// '%a' -> hexadecimal output
	char fixedFormat[4096];
	strcpy_s(fixedFormat, strrep(format, "%a", "0x%08X").c_str());

	return _vsnprintf_s(buffer, n, _TRUNCATE, fixedFormat, args);
}
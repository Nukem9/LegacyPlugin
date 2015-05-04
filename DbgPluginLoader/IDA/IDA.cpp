#include "../stdafx.h"

#define WRAP_CALL(Function) { #Function, &Function }

BYTE inf[8192];

idaman void ida_export sistack_t_flush(void *ss)
{

}

IDAFunctionExport IDAExports[] =
{
	{ "_qsnprintf", qsnprintf },

	// Kernel
	WRAP_CALL(verror),

	// DB
	{ "_inf", &inf },

	// UI
	{ "_callui", &callui },
	{ "callui", &callui },

	// Bytes
	WRAP_CALL(nextaddr),
	WRAP_CALL(prevaddr),
	WRAP_CALL(get_byte),
	WRAP_CALL(get_db_byte),
	WRAP_CALL(get_word),
	WRAP_CALL(get_3byte),
	WRAP_CALL(get_long),
	WRAP_CALL(get_qword),
	WRAP_CALL(get_full_byte),
	WRAP_CALL(get_full_word),
	WRAP_CALL(get_full_long),
	WRAP_CALL(get_8bit),
	WRAP_CALL(get_ascii_char),
	WRAP_CALL(get_16bit),
	WRAP_CALL(get_32bit),
	WRAP_CALL(get_64bit),
	WRAP_CALL(get_many_bytes),

	// Name
	WRAP_CALL(do_name_anyway),

	// Moves
	WRAP_CALL(curloc_mark),
	WRAP_CALL(curloc_markedpos),
	WRAP_CALL(curloc_markdesc),

	// Loader
	WRAP_CALL(hook_to_notification_point),
	WRAP_CALL(unhook_from_notification_point),

	{ "sistack_t_flush", sistack_t_flush },
};

#define PtrFromRva(base, rva) (((PBYTE)base) + rva)

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
			proc = (FARPROC)IDAExports[i].Function;
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

HMODULE LoadIDAPlugin(const char *Path)
{
	callui = kernel_callui;
	// Load the library, but without executing any code.
	// The relocation table is still parsed.
	HMODULE library = LoadLibraryEx(Path, nullptr, DONT_RESOLVE_DLL_REFERENCES);

	if (!library)
		return nullptr;

	// Get pointers to the main PE information
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)library;
	PIMAGE_NT_HEADERS ntHeader	= (PIMAGE_NT_HEADERS)PtrFromRva(dosHeader, dosHeader->e_lfanew);

	// All PE fields have been validated, now get the import table
	auto moduleImports = (PIMAGE_IMPORT_DESCRIPTOR)PtrFromRva(dosHeader, ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	// Iterate over import descriptors/DLLs
	for (ULONG index = 0; moduleImports[index].Characteristics != 0; index++)
	{
		auto importDescriptor	= &moduleImports[index];
		PSTR dllName			= (PSTR)PtrFromRva(dosHeader, importDescriptor->Name);

		// Check if the import thunk(s) exist
		if (!importDescriptor->FirstThunk || !importDescriptor->OriginalFirstThunk)
			return nullptr;

		// Iterate over all function addresses stored in the thunk
		PIMAGE_THUNK_DATA thunk		= (PIMAGE_THUNK_DATA)PtrFromRva(dosHeader, importDescriptor->FirstThunk);
		PIMAGE_THUNK_DATA origThunk = (PIMAGE_THUNK_DATA)PtrFromRva(dosHeader, importDescriptor->OriginalFirstThunk);

		for (; origThunk->u1.Function != 0; origThunk++, thunk++)
		{
			if (origThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				//
				// Ordinal import - we can handle named imports
				// ony, so skip it.
				//
				continue;
			}

			PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)PtrFromRva(dosHeader, origThunk->u1.AddressOfData);

			// Patch the function pointer
			DWORD oldProtection;

			if (!VirtualProtect(&thunk->u1.Function, sizeof(ULONG_PTR), PAGE_READWRITE, &oldProtection))
				return nullptr;

			// Manually look up the request DLL address and apply redirection if needed
			if (!LoaderResolveImport(dllName, import->Name, &thunk->u1.Function))
				return nullptr;

			if (!VirtualProtect(&thunk->u1.Function, sizeof(ULONG_PTR), oldProtection, &oldProtection))
				return nullptr;
		}
	}

	// Check if the entry point needs to be called
	if (ntHeader->OptionalHeader.AddressOfEntryPoint != 0)
	{
		PBYTE entryPoint = PtrFromRva(dosHeader, ntHeader->OptionalHeader.AddressOfEntryPoint);
	
		// Execute DllMain
		// Signature: BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
		BOOL ret = ((BOOL (WINAPI *)(HINSTANCE, DWORD, LPVOID))entryPoint)((HINSTANCE)dosHeader, DLL_PROCESS_ATTACH, nullptr);

		if (ret == FALSE)
			return nullptr;
	}

	// All imports patched successfully
	return library;
}

std::vector<IDAPlugin> Plugins;

bool IDASetupPlugin(HMODULE Library)
{
	// Library must be valid
	if (!Library)
	{
		_plugin_printf("Not loading plugin: Invalid library!\n");
		return false;
	}

	// Find the exported ordinal #1 for IDA
	plugin_t *pluginStruct = (plugin_t *)GetProcAddress(Library, MAKEINTRESOURCE(1));

	if (!pluginStruct)
	{
		_plugin_printf("Not loading plugin: Initialization export structure not found.\n");
		return false;
	}

	// Validate function pointers
	if (!pluginStruct->init || IsBadReadPtr(pluginStruct, 1) != 0)
	{
		_plugin_printf("Not loading plugin: Bad initialization function pointer.\n");
		return false;
	}

	// Create the internal data structure and call init()
	IDAPlugin plugin;

	plugin.Library		= Library;
	plugin.ExternalData = pluginStruct;
	plugin.LoadFlags	= pluginStruct->init();

	if (plugin.LoadFlags == PLUGIN_SKIP)
	{
		_plugin_printf("Not loading plugin: Requested to be skipped.\n");
		return false;
	}

	// Everything set up correctly, add it to the global list
	Plugins.push_back(plugin);
	return true;
}

std::string strrep(const char *Subject, const char *Needle, const char *Replace)
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

	// '%a' -> hexadecimal output
	char fixedFormat[4096];
	strcpy_s(fixedFormat, strrep(format, "%a", "0x%08X").c_str());

	return _vsnprintf_s(buffer, n, _TRUNCATE, fixedFormat, args);
}
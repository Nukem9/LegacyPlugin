#include "../stdafx.h"

#define RvaToPtr(base, rva) (((PBYTE)base) + rva)

HMODULE LoadPluginDll(const char *Path, DllResolveImport ImportCallback, bool SupportOrdinal)
{
	// Load the library, but without executing any code.
	// The relocation table is still parsed.
	HMODULE library = LoadLibraryEx(Path, nullptr, DONT_RESOLVE_DLL_REFERENCES);

	if (!library)
		return nullptr;

	// Get pointers to the main PE information
	PIMAGE_DOS_HEADER dosHeader = (PIMAGE_DOS_HEADER)library;
	PIMAGE_NT_HEADERS ntHeader	= (PIMAGE_NT_HEADERS)RvaToPtr(dosHeader, dosHeader->e_lfanew);

	// All PE fields have been validated, now get the import table
	auto moduleImports = (PIMAGE_IMPORT_DESCRIPTOR)RvaToPtr(dosHeader, ntHeader->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT].VirtualAddress);

	// Iterate over import descriptors/DLLs
	for (ULONG index = 0; moduleImports[index].Characteristics != 0; index++)
	{
		auto importDescriptor	= &moduleImports[index];
		PSTR dllName			= (PSTR)RvaToPtr(dosHeader, importDescriptor->Name);

		// Check if the import thunk(s) exist
		if (!importDescriptor->FirstThunk || !importDescriptor->OriginalFirstThunk)
			return nullptr;

		// Iterate over all function addresses stored in the thunk
		PIMAGE_THUNK_DATA thunk		= (PIMAGE_THUNK_DATA)RvaToPtr(dosHeader, importDescriptor->FirstThunk);
		PIMAGE_THUNK_DATA origThunk = (PIMAGE_THUNK_DATA)RvaToPtr(dosHeader, importDescriptor->OriginalFirstThunk);

		for (; origThunk->u1.Function != 0; origThunk++, thunk++)
		{
			if (origThunk->u1.Ordinal & IMAGE_ORDINAL_FLAG)
			{
				// If ordinals are not accepted, skip them
				if (!SupportOrdinal)
					continue;
			}

			PIMAGE_IMPORT_BY_NAME import = (PIMAGE_IMPORT_BY_NAME)RvaToPtr(dosHeader, origThunk->u1.AddressOfData);

			// Patch the function pointer
			DWORD oldProtection;

			if (!VirtualProtect(&thunk->u1.Function, sizeof(ULONG_PTR), PAGE_READWRITE, &oldProtection))
				return nullptr;

			// Manually look up the request DLL address and apply redirection if needed
			if (!ImportCallback(dllName, import->Name, &thunk->u1.Function))
				return nullptr;

			// This call can fail, the data was already written
			VirtualProtect(&thunk->u1.Function, sizeof(ULONG_PTR), oldProtection, &oldProtection);
		}
	}

	// Check if the entry point needs to be called
	if (ntHeader->OptionalHeader.AddressOfEntryPoint != 0)
	{
		PBYTE entryPoint = RvaToPtr(dosHeader, ntHeader->OptionalHeader.AddressOfEntryPoint);

		// Execute DllMain
		// Signature: BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved);
		BOOL ret = ((BOOL(WINAPI *)(HINSTANCE, DWORD, LPVOID))entryPoint)((HINSTANCE)dosHeader, DLL_PROCESS_ATTACH, nullptr);

		if (!ret)
		{
			FreeLibrary(library);
			return nullptr;
		}
	}

	// All imports patched successfully
	return library;
}
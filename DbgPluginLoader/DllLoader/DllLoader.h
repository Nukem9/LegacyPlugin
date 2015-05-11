#pragma once

typedef bool (* DllResolveImport)(const char *Dll, const char *Name, ULONG_PTR *Import);

HMODULE LoadPluginDll(const char *Path, DllResolveImport ImportCallback, bool SupportOrdinal);
#pragma once

#define WRAP_EXPORT(Variable)	{ #Variable, &Variable }
#define END_EXPORTS()			{ nullptr, nullptr }

struct ModuleExport
{
	const char	*Name;
	PVOID		Address;
};
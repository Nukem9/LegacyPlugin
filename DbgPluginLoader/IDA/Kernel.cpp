#include "../stdafx.h"

idaman NORETURN void ida_export verror(const char *format, va_list va)
{
	char buffer[4096];
	qsnprintf_args(buffer, ARRAYSIZE(buffer), format, va);

	MessageBox(GuiGetWindowHandle(), buffer, "FATAL ERROR", MB_ICONERROR);
	ExitProcess(1);
}
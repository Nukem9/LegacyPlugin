#include "../stdafx.h"

#define SET_ARG(Index, Type)				args[Index] = (PBYTE)va_arg(va, Type)

#define SET_ARGS_0()
#define SET_ARGS_1(arg1)					SET_ARG(0, arg1);
#define SET_ARGS_2(arg1, arg2)				SET_ARGS_1(arg1); SET_ARG(1, arg2);
#define SET_ARGS_3(arg1, arg2, arg3)		SET_ARGS_2(arg1, arg2); SET_ARG(2, arg3);
#define SET_ARGS_4(arg1, arg2, arg3, arg4)	SET_ARGS_3(arg1, arg2, arg3); SET_ARG(3, arg4);

#define FUNC_ARGS_0(func)					SET_ARGS_0();					return func();
#define FUNC_ARGS_1(func, arg1)				SET_ARGS_1(arg1);				return func((arg1)args[0]);
#define FUNC_ARGS_2(func, arg1, arg2)		SET_ARGS_2(arg1, arg2);			return func((arg1)args[0], (arg2)args[1]);
#define FUNC_ARGS_3(func, arg1, arg2, arg3) SET_ARGS_3(arg1, arg2, arg3);	return func((arg1)args[0], (arg2)args[1], (arg3)args[2]);

#define CALLUI_NULL {nullptr}

callui_t(idaapi*callui)(ui_notification_t what, ...);

callui_t idaapi kernel_callui(ui_notification_t what, ...)
{
	va_list va;
	va_start(va, what);

	PBYTE args[8];

	switch (what)
	{
//	case ui_mbox:
//		FUNC_ARGS_3(UI_mbox, mbox_kind_t, const char *, va_list);

	case ui_msg:
		FUNC_ARGS_2(UI_msg, const char *, va_list);

	case ui_wasbreak:
		FUNC_ARGS_0(UI_wasbreak);

	case ui_showauto:
		FUNC_ARGS_2(UI_showauto, ea_t, int);
	}

	_plugin_printf("Callui - %d\n", what);
	return CALLUI_NULL;
}

/*callui_t UI_mbox(mbox_kind_t kind, const char *format, va_list va)
{
	return CALLUI_NULL;
}*/

callui_t UI_msg(const char *format, va_list va)
{
	callui_t ret;
	char buffer[4096];

	// Return value is the number of bytes in the formatted string
	ret.i = qsnprintf_args(buffer, 4096, format, va);

	_plugin_logprintf(buffer);
	return ret;
}

callui_t UI_wasbreak()
{
	callui_t ret;

	// Indicate that there was no Ctrl-Break detected
	ret.i = 0;
	return ret;
}

callui_t UI_showauto(ea_t ea, int auto_t)
{
	char buffer[4096];
	sprintf_s(buffer, "Analyzing: 0x%08X\n", ea);

	// Update the auto-analysis state
	GuiAddStatusBarMessage(buffer);
	return CALLUI_NULL;
}
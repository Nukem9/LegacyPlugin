#pragma once

// Display an error message and die. See also: error(), warning(), info(), msg()

idaman NORETURN void ida_export verror(const char *format, va_list va);
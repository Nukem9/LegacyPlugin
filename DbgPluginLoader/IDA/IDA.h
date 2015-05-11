#pragma once

struct IDAPlugin
{
	HMODULE			Library;
	class plugin_t	*ExternalData;
	int				LoadFlags;
};

extern ModuleExport IDAExports[];

//
// Various API defines and values
//
#define EXTERNC			extern "C"
#define THREAD_SAFE
#define NORETURN		__declspec(noreturn)

#define idaman			EXTERNC
#define idaapi			__stdcall
#define ida_export		idaapi
#define ida_export_data

#ifdef NO_OBSOLETE_FUNCS
#define OBSOLETE(event) obsolete_ ## event
#else
#define OBSOLETE(event) event
#endif

//
// Types
//
typedef long long			longlong;
typedef unsigned long long	ulonglong;

typedef unsigned char   uchar;
typedef unsigned short  ushort;
typedef unsigned int    uint;
typedef          char   int8;
typedef signed   char   sint8;
typedef unsigned char   uint8;
typedef          short  int16;
typedef unsigned short  uint16;
typedef          int    int32;
typedef unsigned int    uint32;
typedef longlong        int64;
typedef ulonglong       uint64;
typedef ptrdiff_t       ssize_t;
typedef long			val_t;
typedef unsigned long	uval_t;

#ifdef _WIN64
typedef uint64 ea_t;
#else
typedef uint32 ea_t;
#endif // _WIN64

#define BADADDR ea_t(-1)  // this value is used for 'bad address'

//
// Functions
//
idaman THREAD_SAFE int qsnprintf(char *buffer, size_t n, const char *format, ...);
int qsnprintf_args(char *buffer, size_t n, const char *format, va_list args);

//
// Internal database information
//
#define DB_SET_MIN_EA(ea) (*(ea_t *)&inf[43]) = (ea)
#define DB_SET_MAX_EA(ea) (*(ea_t *)&inf[47]) = (ea)

#define DB_GET_MIN_EA (*(ea_t *)&inf[43])
#define DB_GET_MAX_EA (*(ea_t *)&inf[47])

extern BYTE inf[];
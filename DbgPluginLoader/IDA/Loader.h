#pragma once

//----------------------------------------------------------------------
//              DEFINITION OF PLUGIN MODULES
//----------------------------------------------------------------------
// A plugin is a module in plugins subdirectory which can be called by
// pressing a hotkey. It usually performs an action asked by the user.

#pragma pack(push, 1)

class plugin_t
{
public:
	int version;                  // Should be equal to IDP_INTERFACE_VERSION
	int flags;                    // Features of the plugin:
#define PLUGIN_MOD  0x0001      // Plugin changes the database.
	// IDA won't call the plugin if
	// the processor prohibited any changes
	// by setting PR_NOCHANGES in processor_t.
#define PLUGIN_DRAW 0x0002      // IDA should redraw everything after calling
	// the plugin
#define PLUGIN_SEG  0x0004      // Plugin may be applied only if the
	// current address belongs to a segment
#define PLUGIN_UNL  0x0008      // Unload the plugin immediately after
	// calling 'run'.
	// This flag may be set anytime.
	// The kernel checks it after each call to 'run'
	// The main purpose of this flag is to ease
	// the debugging of new plugins.
#define PLUGIN_HIDE 0x0010      // Plugin should not appear in the Edit, Plugins menus
	// This flag is checked at the start
#define PLUGIN_DBG  0x0020      // A debugger plugin. init() should put
	// the address of debugger_t to dbg
	// See idd.hpp for details
#define PLUGIN_PROC 0x0040      // Load plugin when a processor module is loaded and keep it
	// until the processor module is unloaded
#define PLUGIN_FIX  0x0080      // Load plugin when IDA starts and keep it in the
	// memory until IDA stops
#define PLUGIN_SCRIPTED 0x8000  // Scripted plugin. Should not be used by plugins,
	// the kernel sets it automatically.
	int (idaapi* init)(void);     // Initialize plugin
#define PLUGIN_SKIP  0  // Plugin doesn't want to be loaded
#define PLUGIN_OK    1  // Plugin agrees to work with the current database
	// It will be loaded as soon as the user presses the hotkey
#define PLUGIN_KEEP  2  // Plugin agrees to work with the current database
	// and wants to stay in the memory
	void (idaapi* term)(void);    // Terminate plugin. This function will be called
	// when the plugin is unloaded. May be NULL.
	void (idaapi* run)(int arg);  // Invoke plugin
	const char *comment;          // Long comment about the plugin
	// it could appear in the status line
	// or as a hint
	const char *help;             // Multiline help about the plugin
	const char *wanted_name;      // The preferred short name of the plugin
	const char *wanted_hotkey;    // The preferred hotkey to run the plugin
};

#pragma pack(pop)
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

//--------------------------------------------------------------------------
// A plugin can hook to the notification point and receive notifications
// of all major events in IDA. The callback function will be called
// for each event. The parameters of the callback:
//      user_data         - data supplied in call to hook_to_notification_point()
//      notification_code - idp_notify or ui_notification_t code, depending on
//                          the hoot type
//      va                - additional parameters supplied with the notification
//                          see the event descriptions for information
// The callback should return:
//      0   - ok, the event should be processed further
//      !=0 - the event is blocked and should be discarded
//            in the case of processor modules, the returned value is used
//            as the return value of notify()

typedef int idaapi hook_cb_t(void *user_data, int notification_code, va_list va);

enum hook_type_t
{
	HT_IDP,         // Hook to the processor module.
	// The callback will receive all idp_notify events.
	// See file idp.hpp for the list of events.
	HT_UI,          // Hook to the user interface.
	// The callback will receive all ui_notification_t events.
	// See file kernwin.hpp for the list of events.
	HT_DBG,         // Hook to the debugger.
	// The callback will receive all dbg_notification_t events.
	// See file dbg.hpp for the list of events.
	HT_IDB,         // Hook to the database events.
	// These events are separated from the HT_IDP group
	// to speed up things (there are too many plugins and
	// modules hooking to the HT_IDP). Some essential events
	// are still generated in th HT_IDP group:
	// make_code, make_data, undefine, rename, add_func, del_func.
	// This list is not exhaustive.
	// A common trait of all events in this group: the kernel
	// does not expect any reaction to the event and does not
	// check the return code. For event names, see the idb_event
	// in idp.hpp
	HT_DEV,         // Internal debugger events.
	// Not stable and undocumented for the moment
	HT_VIEW,        // Custom/IDA views notifications.
	// Refer to kernwin.hpp view_notification_t
	// enum for notification codes
	HT_OUTPUT,      // Output window notifications.
	// Refer to kernwin.hpp msg_notification_t
	// (viewer_notification_t)
	HT_LAST
};

idaman bool ida_export hook_to_notification_point(hook_type_t hook_type,
	hook_cb_t *cb,
	void *user_data);


// The plugin should unhook before being unloaded:
// (preferably in its termination function)
// Returns number of unhooked functions.
// If different callbacks have the same callback function pointer
// and user_data is not NULL, only the callback whose associated
// user defined data matchs will be removed.

idaman int ida_export unhook_from_notification_point(hook_type_t hook_type,
	hook_cb_t *cb,
	void *user_data = NULL);
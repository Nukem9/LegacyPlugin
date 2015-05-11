#include "../stdafx.h"
#include <array>

struct HookCbInfo
{
	hook_cb_t *cb;
	void *user_data;
};

std::array<std::vector<HookCbInfo>, HT_LAST> LdrCallbacks;

bool IDARunDllEvent(duint DllBase, duint DllSize)
{
	DB_SET_MIN_EA(DllBase);
	DB_SET_MAX_EA(DllBase + DllSize);

	// Fake a "Database" loaded event to the plugin,
	// which then runs its notification callback.
	for (auto& entry : LdrCallbacks[HT_IDP])
	{
		int ret = entry.cb(entry.user_data, idp_notify::newfile, nullptr);

		// == 0, just continue
		// != 0, kill loop
		if (ret != 0)
			break;
	}

	return true;
}

idaman bool ida_export hook_to_notification_point(hook_type_t hook_type, hook_cb_t *cb, void *user_data)
{
	// Is the hooking type within these bounds?
	if (hook_type < 0 || hook_type >= HT_LAST)
		return false;

	HookCbInfo info;
	info.cb = cb;
	info.user_data = user_data;

	// Add it to the existing array
	LdrCallbacks[hook_type].push_back(info);
	return true;
}

idaman int ida_export unhook_from_notification_point(hook_type_t hook_type, hook_cb_t *cb, void *user_data)
{
	auto& callbackVector = LdrCallbacks[hook_type];

	for (auto itr = callbackVector.begin(); itr != callbackVector.end(); itr++)
	{
		// Is this the correct callback?
		if (itr->cb != cb)
			continue;

		// Now remove it from the list
		itr = callbackVector.erase(itr);
		return true;
	}

	return false;
}
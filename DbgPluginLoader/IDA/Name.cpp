#include "../stdafx.h"

idaman bool ida_export do_name_anyway(ea_t ea, const char *name, size_t maxlen)
{
	// Names must be valid
	if (!name || name[0] == '\0')
		return false;

	// NOTE: maxlen is ignored
	return DbgSetAutoLabelAt((duint)ea, name);
}
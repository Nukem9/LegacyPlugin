#pragma once

// Give a name anyway
// This function tries to give the specified name the specified addresss
// and tries to modify the name with a postfix ("_00") if the name
// already exists.
// It will try 100 different postfixes and fail only if all such names
// are already in use. For example, if the specified name is "xxx",
// the function will try to give names "xxx","xxx_0","xxx_2",... "xxx_99"
//      ea     - linear address
//      name   - new name
//                 NULL: return 0
//                 ""  : return 0
//      maxlen - if specified, the name will be truncated to have the specified
//               maximum length
// returns: 1-ok, 0-failure

idaman bool ida_export do_name_anyway(ea_t ea, const char *name, size_t maxlen = 0);
#include "../stdafx.h"

int ida_export curloc_mark(const curloc &loc, int marker, const char *title, const char *desc)
{
	// Hack-y method to extract the current address from 'curloc'
	DbgSetAutoCommentAt((duint)(*(ea_t *)((PBYTE)&loc + 0x40C)), desc);
	return 0;
}

ea_t ida_export curloc_markedpos(const curloc &, int *marker)
{
	return BADADDR;
}

ssize_t ida_export curloc_markdesc(const curloc &, int marker, char *buf, size_t bufsize)
{
	return 0;
}
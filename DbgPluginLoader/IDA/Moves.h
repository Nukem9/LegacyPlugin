#pragma once

typedef void *curloc;

int ida_export curloc_mark(const curloc &, int marker, const char *title, const char *desc);
ea_t ida_export curloc_markedpos(const curloc &, int *marker);
ssize_t ida_export curloc_markdesc(const curloc &, int marker, char *buf, size_t bufsize);
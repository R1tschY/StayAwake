#ifndef GETTEXT_H
#define GETTEXT_H

#include <string>

void init_gettext();

std::wstring wgettext(const char *msgid);

#define _(...) wgettext(__VA_ARGS__)

#endif // GETTEXT_H

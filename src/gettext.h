#ifndef GETTEXT_H
#define GETTEXT_H

#include <boost/locale.hpp>

#define _(...) ::boost::locale::gettext(__VA_ARGS__)
#define N_(...) ::boost::locale::ngettext(__VA_ARGS__)
#define P_(...) ::boost::locale::pgettext(__VA_ARGS__)
#define NP_(...) ::boost::locale::npgettext(__VA_ARGS__)

#endif // GETTEXT_H

#include "gettext.h"

#include <iostream>
#include <libintl.h>
#include <boost/filesystem.hpp>
#include <lightports/extra/charcodecs.h>
#include <lightports/user/application.h>

#include "config.h"

void init_gettext()
{
  boost::filesystem::path local_path = Windows::Application::getExecutablePath();
  local_path.remove_filename();
  local_path /= L"locale";

  ::setlocale(LC_ALL, "");
  ::bindtextdomain(PROJECT_NAME, local_path.string().c_str());
  ::bind_textdomain_codeset(PROJECT_NAME, "UTF-8");
  ::textdomain(PROJECT_NAME);
}

std::wstring wgettext(const char *msgid)
{
  return Windows::to_wstring(::gettext(msgid));
}

#ifndef SRC_UTILS_I18N_H_
#define SRC_UTILS_I18N_H_

#include <string>
#include <unordered_map>
#include <vector>

#include <cpp-utils/strings/string_view.h>

/// \brief
class I18N
{
public:
  struct Entry {
    const wchar_t* key;
    const wchar_t* translation;
  };

  struct LanguageDescription {
    LanguageDescription(Entry* entries_, const wchar_t* language_):
      entries(entries_), language(language_)
    {}
    Entry* entries;
    const wchar_t* language;
  };

  I18N();

  void addLanguage(LanguageDescription language);
  void setLanguage(cpp::wstring_view name);
  void setNeutralLanguage(cpp::wstring_view name);

  cpp::wstring_view get(cpp::wstring_view key);

private:
  std::wstring neutral_language_ = L"en";
  std::wstring language_ = L"en";

  std::vector<LanguageDescription> languages_;
  std::unordered_map<cpp::wstring_view, cpp::wstring_view> entries_;

  LanguageDescription searchLanguage(cpp::wstring_view name);
  LanguageDescription findLanguage(cpp::wstring_view name);
  void loadLanguage(LanguageDescription description);
};

#endif /* SRC_UTILS_I18N_H_ */

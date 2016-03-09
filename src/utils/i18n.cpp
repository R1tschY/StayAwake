/// \file i18n.cpp

#include "i18n.h"

#include <algorithm>

I18N::I18N()
{
}

void I18N::addLanguage(LanguageDescription language)
{
  languages_.push_back(language);
}

void I18N::setLanguage(cpp::wstring_view name)
{
  loadLanguage(searchLanguage(name));
}

void I18N::setNeutralLanguage(cpp::wstring_view name)
{
  neutral_language_.assign(name.begin(), name.end());
}

I18N::LanguageDescription I18N::searchLanguage(cpp::wstring_view name)
{
  auto exact = findLanguage(name);
  if (exact.language)
    return exact;

  auto neutral = findLanguage(neutral_language_);
  if (neutral.language)
    return neutral;

  return {nullptr, nullptr};
}

I18N::LanguageDescription I18N::findLanguage(cpp::wstring_view name)
{
  // find exact match
  auto exact_match = std::find_if(languages_.begin(), languages_.end(),
    [=](const LanguageDescription& description)
    {
      return name.compare(description.language) == 0;
    }
  );
  if (exact_match != languages_.end())
  {
    return *exact_match;
  }

  // find sub match
  auto sub_match = std::find_if(languages_.begin(), languages_.end(),
    [=](const LanguageDescription& description)
    {
      return name.substr(0,2)  == cpp::wstring_view(description.language).substr(0,2);
    }
  );
  if (sub_match != languages_.end())
  {
    return *sub_match;
  }

  return {nullptr, nullptr};
}

cpp::wstring_view I18N::get(cpp::wstring_view key)
{
  auto iter = entries_.find(key);
  if (iter != entries_.end())
  {
    return iter->second;
  }
  else
  {
    return key;
  }
}

void I18N::loadLanguage(LanguageDescription description)
{
  entries_.clear();

  if (!description.language)
    return;

  for (Entry* e = description.entries; e->key != nullptr; ++e)
  {
    entries_.emplace(
      cpp::wstring_view(e->key), cpp::wstring_view(e->translation)
    );
  }
}

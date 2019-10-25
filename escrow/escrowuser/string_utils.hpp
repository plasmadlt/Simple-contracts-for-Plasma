#pragma once

#include <string>

#define TRIM_CHARS "\t\n\v\f\r "

namespace plasma
{

//----------------------------------------------------------------------------------------------------------------------
std::string& ltrim(std::string& str, const std::string& chars = TRIM_CHARS)
{
  const auto found = str.find_first_not_of(chars);
  if (found != std::string::npos)
    str.erase(0, found);
  else
    str.clear();

  return str;
}

//----------------------------------------------------------------------------------------------------------------------
std::string& rtrim(std::string& str, const std::string& chars = TRIM_CHARS)
{
  const auto found = str.find_last_not_of(chars);
  if (found != std::string::npos)
    str.erase(found+1);
  else
    str.clear();

  return str;
}

//----------------------------------------------------------------------------------------------------------------------
std::string& trim(std::string& str, const std::string& chars = TRIM_CHARS)
{
  return ltrim(rtrim(str, chars), chars);
}

//----------------------------------------------------------------------------------------------------------------------
std::string trim_copy(const std::string& str, const std::string& chars = TRIM_CHARS)
{
  auto copy(str);
  return trim(copy, chars);
}

}

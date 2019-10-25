#pragma once

#include <sstream>
#include <string>
#include <vector>

//======================================================================================================================
namespace plasma
{

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
std::string bigIntToStr(T value)
{
  std::string ret;
  ret.reserve(40);

  for (T tmp = value < 0 ? -value : value; tmp != 0;)
  {
    ret += "0123456789"[tmp % 10];
    tmp /= 10;
  }

  if (value < 0)
    ret += '-';

  return std::string(ret.crbegin(), ret.crend());
}

//----------------------------------------------------------------------------------------------------------------------
std::string to_string(int x) { return std::to_string(x); }
std::string to_string(unsigned int x) { return std::to_string(x); }
std::string to_string(long x) { return std::to_string(x); }
std::string to_string(unsigned long x) { return std::to_string(x); }
std::string to_string(long long x) { return std::to_string(x); }
std::string to_string(unsigned long long x) { return std::to_string(x); }
std::string to_string(float x) { return std::to_string(x); }
std::string to_string(double x) { return std::to_string(x); }
std::string to_string(long double x) { return std::to_string(x); }
std::string to_string(const char *x) { return std::string(x); }
std::string to_string(const std::string &x) { return x; }
std::string to_string(int128_t v) { return bigIntToStr(v); }
std::string to_string(uint128_t v) { return bigIntToStr(v); }

//----------------------------------------------------------------------------------------------------------------------
template<typename T> std::string to_string(const T &t)
{
    std::stringstream ss;
    ss << t;
    return ss.str();
}

//----------------------------------------------------------------------------------------------------------------------
std::string vtformat_impl(const std::string &fmt, const std::vector<std::string> &strs)
{
    static const char FORMAT_SYMBOL = '%';
    std::string res;
    std::string buf;
    bool arg = false;
    for (int i = 0; i <= static_cast<int>(fmt.size()); ++i)
    {
        bool last = i == static_cast<int>(fmt.size());
        char ch = fmt[i];
        if (arg)
        {
            if (ch >= '0' && ch <= '9')
            {
                buf += ch;
            }
            else
            {
                int num = 0;
                if (!buf.empty() && buf.length() < 10)
                    num = atoi(buf.c_str());
                if (num >= 1 && num <= static_cast<int>(strs.size()))
                    res += strs[num - 1];
                else
                    res += FORMAT_SYMBOL + buf;
                buf.clear();
                if (ch != FORMAT_SYMBOL)
                {
                    if (!last)
                        res += ch;
                    arg = false;
                }
            }
        }
        else
        {
            if (ch == FORMAT_SYMBOL)
            {
                arg = true;
            }
            else
            {
                if (!last)
                    res += ch;
            }
        }
    }
    return res;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename Arg, typename ... Args> inline std::string vtformat_impl(const std::string& fmt, std::vector<std::string>& strs, Arg&& arg, Args&& ... args)
{
    strs.push_back(to_string(std::forward<Arg>(arg)));
    return vtformat_impl(fmt, strs, std::forward<Args>(args) ...);
}

//----------------------------------------------------------------------------------------------------------------------
inline std::string vtformat(const std::string& fmt)
{
    return fmt;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename Arg, typename ... Args> inline std::string vtformat(const std::string& fmt, Arg&& arg, Args&& ... args)
{
    std::vector<std::string> strs;
    return vtformat_impl(fmt, strs, std::forward<Arg>(arg), std::forward<Args>(args) ...);
}

//----------------------------------------------------------------------------------------------------------------------
template<typename T>
T to_number(const std::string& str, bool& ok)
{
  ok = false;

  if (!str.size())
    return 0;

  T value = 0;
  T sign = 1;

  if ('-' == str.front())
  {
    sign = -sign;

    if (str.size() < 2)
      return 0;
  }

  for (std::string::const_iterator it = ((sign > 0) ? str.begin() : str.begin() + 1); it != str.end(); ++it)
  {
    if (!std::isdigit(*it))
      return 0;

    const auto digit = std::atoi((char[2]){*it, 0});
    const auto future = (value * 10) + digit;

    // Check overflow
    if (value != ((future - digit)/10))
      return 0;

    value = future;
  }

  // Set sign
  value *= sign;

  ok = true;
  return value;
}

//----------------------------------------------------------------------------------------------------------------------
std::string to_hex(const char* d, uint32_t s) {
    std::string r;
    const char* to_hex = "0123456789abcdef";
    uint8_t* c = (uint8_t*)d;
    for (uint32_t i = 0; i < s; ++i)
        (r += to_hex[(c[i] >> 4)]) += to_hex[(c[i] & 0x0f)];
    return r;
}

//----------------------------------------------------------------------------------------------------------------------
std::string sha256_to_hex(const ion::checksum256& sha256) {
    const auto arr_sha256 = sha256.extract_as_byte_array();
    const std::string str_sha256(arr_sha256.begin() , arr_sha256.end());

    return to_hex(str_sha256.c_str(), str_sha256.length());
}

} // namespace plasma

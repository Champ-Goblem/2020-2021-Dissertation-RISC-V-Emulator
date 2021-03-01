#ifndef __Helpers__
#define __Helpers__

#include <iostream>
#include <codecvt>
#include <locale>
#include <sstream>
#include <iomanip>

#include "../emustd.h"

using namespace std;

inline wstring stringToWString(string input) {
  return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(input);
}

inline wstring ulongToHexWstring(ulong input) {
  ostringstream oss;
  oss << setfill('0') << setw(sizeof(ulong)*2) << hex << input << flush;
  return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(oss.str());
}

inline wstring byteToHexWstring(byte input) {
  ostringstream oss;
  oss << setfill('0') << setw(sizeof(byte)*2) << hex << (0xff & input) << flush;
  return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(oss.str());
}

inline string wstringToString(wstring input) {
  using convert_typeX = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_typeX, wchar_t> converterX;

  return converterX.to_bytes(input);
}

#endif
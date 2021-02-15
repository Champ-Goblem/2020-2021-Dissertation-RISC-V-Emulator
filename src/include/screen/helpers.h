#ifndef __Helpers__
#define __Helpers__

#include <iostream>
#include <codecvt>
#include <locale>
#include <sstream>
#include <iomanip>

using namespace std;

inline wstring stringToWString(string input) {
  return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(input);
}

inline wstring ulongToHexWstring(ulong input) {
  ostringstream oss;
  oss << setfill('0') << setw(sizeof(ulong)*2) << hex << (unsigned long long)input;
  return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(oss.str());
}

#endif
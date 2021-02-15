#ifndef __Helpers__
#define __Helpers__

#include <iostream>
#include <codecvt>
#include <locale>

using namespace std;

inline wstring stringToWString(string input) {
  return wstring_convert<codecvt_utf8<wchar_t>>().from_bytes(input);
}

#endif
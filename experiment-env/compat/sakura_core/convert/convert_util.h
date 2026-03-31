// convert_util.h - compatibility shim for macOS test build
#ifndef SAKURA_CONVERT_UTIL_COMPAT_H_
#define SAKURA_CONVERT_UTIL_COMPAT_H_

#include "sakura_compat.h"

void Convert_ZeneisuToHaneisu(wchar_t* pData, int nLength);
void Convert_HaneisuToZeneisu(wchar_t* pData, int nLength);

#endif

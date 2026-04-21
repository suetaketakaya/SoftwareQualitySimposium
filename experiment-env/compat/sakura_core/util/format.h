// format.h - compatibility shim for macOS test build
#ifndef SAKURA_FORMAT_COMPAT_H_
#define SAKURA_FORMAT_COMPAT_H_

#include "sakura_compat.h"
#include <string>
#include <string_view>

std::wstring GetDateTimeFormat(std::wstring_view format, const SYSTEMTIME& systime);
UINT32 ParseVersion(const WCHAR* ver);
int CompareVersion(const WCHAR* verA, const WCHAR* verB);

#endif

// =============================================================================
// sakura_compat.h
// Sakura-editor specific types and macros for macOS compilation
// Only what is needed by the target pure functions
// =============================================================================
#ifndef SAKURA_COMPAT_H_
#define SAKURA_COMPAT_H_

#include "windows_compat.h"

#include <cstddef>
#include <cwchar>
#include <string>
#include <string_view>
#include <array>
#include <algorithm>

// =====================================================================
// From basis/primitive.h
// =====================================================================
typedef char ACHAR;
typedef wchar_t WChar;
typedef wchar_t EDIT_CHAR;
typedef unsigned char  uchar_t;
typedef unsigned short uchar16_t;
typedef unsigned long  uchar32_t;
typedef long           wchar32_t;
typedef char KEYCODE;
typedef int Int;

// =====================================================================
// From basis/SakuraBasis.h - CLogicInt (non-strict mode)
// =====================================================================
typedef int CLogicInt;
typedef int CLayoutInt;

// =====================================================================
// From util/std_macro.h
// =====================================================================
#define SAFE_DELETE(p) { delete p; p=nullptr; }
#define __LTEXT(A) L##A
#define LTEXT(A) __LTEXT(A)
#define LCHAR(A) __LTEXT(A)
#define ATEXT(A) A

// =====================================================================
// From debug/Debug1.h - DEBUG_TRACE (no-op in release)
// =====================================================================
#ifndef _DEBUG
#define DEBUG_TRACE(...)
#else
#define DEBUG_TRACE(...)
#endif

// =====================================================================
// From util/string_ex.h - wcsncmp_literal, wcsnicmp_literal
// =====================================================================
template <size_t Size>
inline int wcsncmp_literal(const wchar_t* strData1, const wchar_t (&literalData2)[Size]) {
    return ::wcsncmp(strData1, literalData2, Size - 1);
}

template <size_t Size>
inline int wcsnicmp_literal(const wchar_t* strData1, const wchar_t (&literalData2)[Size]) {
    return ::wcsncasecmp(strData1, literalData2, Size - 1);
}

// =====================================================================
// From parse/CWordParse.h - ECharKind enum
// =====================================================================
enum ECharKind{
    CK_NULL,
    CK_TAB,
    CK_CR,
    CK_LF,
    CK_CTRL,
    CK_SPACE,
    CK_CSYM,
    CK_KATA,
    CK_LATIN,
    CK_UDEF,
    CK_ETC,
    CK_ZEN_SPACE,
    CK_ZEN_NOBASU,
    CK_ZEN_DAKU,
    CK_ZEN_CSYM,
    CK_ZEN_KIGO,
    CK_HIRA,
    CK_ZEN_KATA,
    CK_GREEK,
    CK_ZEN_ROS,
    CK_ZEN_SKIGO,
    CK_ZEN_ETC,
};

// =====================================================================
// CWordParse class declaration (minimal, for the target functions)
// =====================================================================
class CNativeW; // forward declaration

class CWordParse {
public:
    static ECharKind WhatKindOfTwoChars(ECharKind kindPre, ECharKind kindCur);
    static ECharKind WhatKindOfTwoChars4KW(ECharKind kindPre, ECharKind kindCur);
};

// =====================================================================
// IsMailAddress declaration
// =====================================================================
BOOL IsMailAddress(const wchar_t* pszBuf, int offset, size_t nBufLen, int* pnAddressLength);

inline BOOL IsMailAddress(const wchar_t* pszBuf, size_t nBufLen, int* pnAddressLength)
{
    return IsMailAddress(pszBuf, 0, nBufLen, pnAddressLength);
}

#endif /* SAKURA_COMPAT_H_ */

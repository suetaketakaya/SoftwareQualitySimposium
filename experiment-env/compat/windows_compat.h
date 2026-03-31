// =============================================================================
// windows_compat.h
// Windows API types and macros compatibility layer for macOS
// Minimal definitions required to compile sakura-editor pure functions
// =============================================================================
#ifndef WINDOWS_COMPAT_H_
#define WINDOWS_COMPAT_H_

#include <cstdint>
#include <cwchar>
#include <cstring>

// -- Prevent inclusion of real <windows.h> / <Windows.h> --
#define _WINDEF_
#define _WINDOWS_
#define _WINBASE_

// -- Basic Windows integer types --
typedef int            BOOL;
typedef unsigned short WORD;
typedef unsigned long  DWORD;
typedef unsigned int   UINT;
typedef uint32_t       UINT32;
typedef long           LONG;
typedef unsigned char  BYTE;
typedef int            INT;

// -- Boolean constants --
#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

// -- Character types --
typedef wchar_t        WCHAR;
typedef char           CHAR;
typedef const wchar_t* LPCWSTR;
typedef wchar_t*       LPWSTR;
typedef const char*    LPCSTR;
typedef char*          LPSTR;
typedef void*          LPVOID;
typedef const void*    LPCVOID;

// -- TCHAR (always wide in sakura-editor UNICODE build) --
#ifndef _T
#define _T(x) L##x
#endif
typedef wchar_t TCHAR;

// -- Handle types --
typedef void*          HANDLE;
typedef void*          HWND;
typedef void*          HDC;
typedef void*          HFONT;
typedef void*          HINSTANCE;

// -- SYSTEMTIME structure --
typedef struct _SYSTEMTIME {
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME;

// -- SIZE structure --
typedef struct tagSIZE {
    LONG cx;
    LONG cy;
} SIZE;

// -- POINT structure --
typedef struct tagPOINT {
    LONG x;
    LONG y;
} POINT;

// -- RECT structure --
typedef struct tagRECT {
    LONG left;
    LONG top;
    LONG right;
    LONG bottom;
} RECT;

// -- LOGFONT --
#ifndef LF_FACESIZE
#define LF_FACESIZE 32
#endif

typedef struct tagLOGFONTW {
    LONG  lfHeight;
    LONG  lfWidth;
    LONG  lfEscapement;
    LONG  lfOrientation;
    LONG  lfWeight;
    BYTE  lfItalic;
    BYTE  lfUnderline;
    BYTE  lfStrikeOut;
    BYTE  lfCharSet;
    BYTE  lfOutPrecision;
    BYTE  lfClipPrecision;
    BYTE  lfQuality;
    BYTE  lfPitchAndFamily;
    WCHAR lfFaceName[LF_FACESIZE];
} LOGFONTW, LOGFONT;

// -- String functions compatibility --
// _wcsnicmp is MSVC-specific; on POSIX use wcsncasecmp
#ifndef _wcsnicmp
#define _wcsnicmp wcsncasecmp
#endif

// _memicmp is MSVC-specific
#ifndef _memicmp
inline int _memicmp(const void* s1, const void* s2, size_t n) {
    const unsigned char* p1 = (const unsigned char*)s1;
    const unsigned char* p2 = (const unsigned char*)s2;
    for (size_t i = 0; i < n; i++) {
        int c1 = tolower(p1[i]);
        int c2 = tolower(p2[i]);
        if (c1 != c2) return c1 - c2;
    }
    return 0;
}
#endif

// -- _MAX_PATH --
#ifndef _MAX_PATH
#define _MAX_PATH 260
#endif

#ifndef MAX_PATH
#define MAX_PATH 260
#endif

// -- wctomb_s --
// MSVC extension; not used in our target code, but just in case
#ifndef wctomb_s
#define wctomb_s(retval, buf, bufsz, wc) (*(retval) = wctomb((buf), (wc)), 0)
#endif

#endif /* WINDOWS_COMPAT_H_ */

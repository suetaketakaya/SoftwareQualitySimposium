// =============================================================================
// convert_wrapper.cpp
// Extracted pure functions from sakura_core/convert/convert_util.cpp
// Only: Convert_ZeneisuToHaneisu, Convert_HaneisuToZeneisu
// =============================================================================

#include "StdAfx.h"
#include <cwchar>

// -- Conversion tables (from convert_util.cpp) --

// Full-width symbol table
static const wchar_t tableZenKigo[] =
    L"\u3000\uFF0C\uFF0E"
    L"\uFF0B\uFF0D\uFF0A\uFF0F\uFF05\uFF1D\uFF5C\uFF06"
    L"\uFF3E\uFFE5\uFF20\uFF1B\uFF1A"
    L"\u201D\u2018\u2019\uFF1C\uFF1E\uFF08\uFF09\uFF5B\uFF5D\uFF3B\uFF3D"
    L"\uFF01\uFF1F\uFF03\uFF04\uFFE3\uFF3F";

// Half-width symbol table
static const wchar_t tableHanKigo[] =
    L" ,."
    L"+-*/%=|&"
    L"^\\@;:"
    L"\"`'<>(){}[]"
    L"!?#$~_";

// -- Helper --
static bool wcschr_idx(const wchar_t* str, wchar_t c, int* idx)
{
    const wchar_t* p = wcschr(str, c);
    if(p) *idx = (int)(p - str);
    return p != nullptr;
}

// -- Full-width alphanumeric to half-width --
static inline wchar_t ZeneisuToHaneisu_(wchar_t c)
{
    int n;

         if(c>=L'\uFF21' && c<=L'\uFF3A'){ c=L'A'+(c-L'\uFF21'); }
    else if(c>=L'\uFF41' && c<=L'\uFF5A'){ c=L'a'+(c-L'\uFF41'); }
    else if(c>=L'\uFF10' && c<=L'\uFF19'){ c=L'0'+(c-L'\uFF10'); }
    else if(wcschr_idx(tableZenKigo,c,&n)){ c=tableHanKigo[n]; }

    return c;
}

void Convert_ZeneisuToHaneisu(wchar_t* pData, int nLength)
{
    wchar_t* p=pData;
    wchar_t* q=p+nLength;
    while(p<q){
        *p = ZeneisuToHaneisu_(*p);
        p++;
    }
}

// -- Half-width alphanumeric to full-width --
static inline wchar_t HaneisuToZeneisu_(wchar_t c)
{
    int n;

         if(c>=L'A' && c<=L'Z'){ c=L'\uFF21'+(c-L'A'); }
    else if(c>=L'a' && c<=L'z'){ c=L'\uFF41'+(c-L'a'); }
    else if(c>=L'0' && c<=L'9'){ c=L'\uFF10'+(c-L'0'); }
    else if(wcschr_idx(tableHanKigo,c,&n)){ c=tableZenKigo[n]; }

    return c;
}

void Convert_HaneisuToZeneisu(wchar_t* pData, int nLength)
{
    wchar_t* p=pData;
    wchar_t* q=p+nLength;
    while(p<q){
        *p = HaneisuToZeneisu_(*p);
        p++;
    }
}

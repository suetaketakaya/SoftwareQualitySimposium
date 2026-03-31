// =============================================================================
// format_wrapper.cpp
// Extracted pure functions from sakura_core/util/format.cpp
// =============================================================================

#include "StdAfx.h"

// --- Forward declarations from format.h ---
#include <string>
#include <string_view>

std::wstring GetDateTimeFormat(std::wstring_view format, const SYSTEMTIME& systime);
UINT32 ParseVersion(const WCHAR* ver);
int CompareVersion(const WCHAR* verA, const WCHAR* verB);

// === Implementation (copied from format.cpp) ===

std::wstring GetDateTimeFormat( std::wstring_view format, const SYSTEMTIME& systime )
{
    std::wstring result;
    wchar_t str[6] = {};
    bool inSpecifier = false;

    result.reserve( format.length() * 2 );

    for( const auto f : format ){
        if( inSpecifier ){
            inSpecifier = false;
            if( f == L'Y' ){
                swprintf( str, int(std::size(str)), L"%d", systime.wYear );
            }else if( f == L'y' ){
                swprintf( str, int(std::size(str)), L"%02d", systime.wYear % 100 );
            }else if( f == L'm' ){
                swprintf( str, int(std::size(str)), L"%02d", systime.wMonth );
            }else if( f == L'd' ){
                swprintf( str, int(std::size(str)), L"%02d", systime.wDay );
            }else if( f == L'H' ){
                swprintf( str, int(std::size(str)), L"%02d", systime.wHour );
            }else if( f == L'M' ){
                swprintf( str, int(std::size(str)), L"%02d", systime.wMinute );
            }else if( f == L'S' ){
                swprintf( str, int(std::size(str)), L"%02d", systime.wSecond );
            }else{
                swprintf( str, int(std::size(str)), L"%lc", (wint_t)f );
            }
            result.append( str );
        }else if( f == L'%' ){
            inSpecifier = true;
        }else if( f == L'\0' ){
            break;
        }else{
            result.push_back( f );
        }
    }

    return result;
}

UINT32 ParseVersion( const WCHAR* sVer )
{
    int nVer;
    int nShift = 0;
    int nDigit = 0;
    UINT32 ret = 0;

    const WCHAR *p = sVer;
    int i;

    for( i=0; *p && i<4; i++){
        if( *p == L'a' ){
            if( wcsncmp_literal( p, L"alpha" ) == 0 )p += 5;
            else p++;
            nShift = -0x60;
        }
        else if( *p == L'b' ){
            if( wcsncmp_literal( p, L"beta" ) == 0 )p += 4;
            else p++;
            nShift = -0x40;
        }
        else if( *p == L'r' || *p == L'R' ){
            if( wcsnicmp_literal( p, L"rc" ) == 0 )p += 2;
            else p++;
            nShift = -0x20;
        }
        else if( *p == L'p' ){
            if( wcsncmp_literal( p, L"pl" ) == 0 )p += 2;
            else p++;
            nShift = 0x20;
        }
        else if( !iswdigit(*p) ){
            nShift = -0x80;
        }
        else{
            nShift = 0;
        }
        while( *p && !iswdigit(*p) ){ p++; }
        for( nVer = 0, nDigit = 0; iswdigit(*p); p++ ){
            if( ++nDigit > 2 )break;
            nVer = nVer * 10 + *p - L'0';
        }
        while( *p && wcschr( L".-_+", *p ) ){ p++; }

        DEBUG_TRACE(L"  VersionPart%d: ver=%d,shift=%d\n", i, nVer, nShift);
        ret |= ( (nShift + nVer + 128) << (24-8*i) );
    }
    for( ; i<4; i++ ){
        ret |= ( 128 << (24-8*i) );
    }

    DEBUG_TRACE(L"ParseVersion %ls -> %08x\n", sVer, ret);
    return ret;
}

int CompareVersion( const WCHAR* verA, const WCHAR* verB )
{
    UINT32 nVerA = ParseVersion(verA);
    UINT32 nVerB = ParseVersion(verB);

    return nVerA - nVerB;
}

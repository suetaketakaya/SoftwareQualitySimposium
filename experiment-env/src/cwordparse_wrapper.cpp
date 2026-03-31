// =============================================================================
// cwordparse_wrapper.cpp
// Extracted pure functions from sakura_core/parse/CWordParse.cpp
// Only: WhatKindOfTwoChars, WhatKindOfTwoChars4KW, IsMailAddress
// =============================================================================

#include "StdAfx.h"

// === WhatKindOfTwoChars ===

ECharKind CWordParse::WhatKindOfTwoChars( ECharKind kindPre, ECharKind kindCur )
{
    if( kindPre == kindCur )return kindCur;

    if( ( kindPre == CK_ZEN_NOBASU || kindPre == CK_ZEN_DAKU ) &&
        ( kindCur == CK_ZEN_KATA   || kindCur == CK_HIRA     ) )return kindCur;
    if( ( kindCur == CK_ZEN_NOBASU || kindCur == CK_ZEN_DAKU ) &&
        ( kindPre == CK_ZEN_KATA   || kindPre == CK_HIRA     ) )return kindPre;
    if( ( kindPre == CK_ZEN_NOBASU || kindPre == CK_ZEN_DAKU ) &&
        ( kindCur == CK_ZEN_NOBASU || kindCur == CK_ZEN_DAKU ) )return kindCur;

    if( kindPre == CK_LATIN )kindPre = CK_CSYM;
    if( kindCur == CK_LATIN )kindCur = CK_CSYM;
    if( kindPre == CK_UDEF )kindPre = CK_ETC;
    if( kindCur == CK_UDEF )kindCur = CK_ETC;
    if( kindPre == CK_CTRL )kindPre = CK_ETC;
    if( kindCur == CK_CTRL )kindCur = CK_ETC;

    if( kindPre == kindCur )return kindCur;

    return CK_NULL;
}

// === WhatKindOfTwoChars4KW ===

ECharKind CWordParse::WhatKindOfTwoChars4KW( ECharKind kindPre, ECharKind kindCur )
{
    if( kindPre == kindCur )return kindCur;

    if( ( kindPre == CK_ZEN_NOBASU || kindPre == CK_ZEN_DAKU ) &&
        ( kindCur == CK_ZEN_KATA   || kindCur == CK_HIRA     ) )return kindCur;
    if( ( kindCur == CK_ZEN_NOBASU || kindCur == CK_ZEN_DAKU ) &&
        ( kindPre == CK_ZEN_KATA   || kindPre == CK_HIRA     ) )return kindPre;
    if( ( kindPre == CK_ZEN_NOBASU || kindPre == CK_ZEN_DAKU ) &&
        ( kindCur == CK_ZEN_NOBASU || kindCur == CK_ZEN_DAKU ) )return kindCur;

    if( kindPre == CK_LATIN )kindPre = CK_CSYM;
    if( kindCur == CK_LATIN )kindCur = CK_CSYM;
    if( kindPre == CK_UDEF )kindPre = CK_CSYM;
    if( kindCur == CK_UDEF )kindCur = CK_CSYM;
    if( kindPre == CK_CTRL )kindPre = CK_CTRL;
    if( kindCur == CK_CTRL )kindCur = CK_CTRL;

    if( kindPre == kindCur )return kindCur;

    return CK_NULL;
}

// === IsMailAddress ===

BOOL IsMailAddress( const wchar_t* pszBuf, int offset, size_t cchBuf, int* pnAddressLength )
{
    auto nBufLen = int(cchBuf);

    struct {
        bool operator()(const wchar_t ch)
        {
            return 0x21 <= ch && ch <= 0x7E && nullptr == wcschr(L"\"(),:;<>@[\\]", ch);
        }
    } IsValidChar;

    if (0 < offset && IsValidChar(pszBuf[offset-1])) {
        return FALSE;
    }

    pszBuf  += offset;
    nBufLen -= offset;
    offset   = 0;

    int     j;
    int     nDotCount;
    int     nBgn;

    j = 0;
    if(pszBuf[j] != L'.' && IsValidChar(pszBuf[j])){
        j++;
    }else{
        return FALSE;
    }
    while( j < nBufLen - 2 && IsValidChar(pszBuf[j]) ){
        j++;
    }
    if( j == 0 || j >= nBufLen - 2  ){
        return FALSE;
    }
    if( L'@' != pszBuf[j] ){
        return FALSE;
    }
    j++;
    nDotCount = 0;

    for (;;) {
        nBgn = j;
        while( j < nBufLen &&
            (
            (pszBuf[j] >= L'a' && pszBuf[j] <= L'z')
         || (pszBuf[j] >= L'A' && pszBuf[j] <= L'Z')
         || (pszBuf[j] >= L'0' && pszBuf[j] <= L'9')
         || (pszBuf[j] == L'-')
         || (pszBuf[j] == L'_')
            )
        ){
            j++;
        }
        if( 0 == j - nBgn ){
            return FALSE;
        }
        if( L'.' != pszBuf[j] ){
            if( 0 == nDotCount ){
                return FALSE;
            }else{
                break;
            }
        }else{
            nDotCount++;
            j++;
        }
    }
    if( nullptr != pnAddressLength){
        *pnAddressLength = j;
    }
    return TRUE;
}

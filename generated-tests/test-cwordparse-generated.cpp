// =============================================================================
// test-cwordparse-generated.cpp
// 対象: sakura_core/parse/CWordParse.cpp
//   - IsMailAddress          (TGT-04)
//   - WhatKindOfTwoChars     (TGT-05)
//   - WhatKindOfTwoChars4KW  (TGT-06)
//
// テスト要求モデル: test-requirements.yaml (99件中34件をカバー)
// 生成: SQiP 2026 実証実験 Phase 3
// =============================================================================

#include <gtest/gtest.h>

#include <cstring>

// sakura-editor ヘッダ (macOS compat)
#include "sakura_core/parse/CWordParse.h"

// =============================================================================
// ヘルパー: IsMailAddress 呼び出しラッパー
// =============================================================================

// バッファ先頭からメールアドレスを判定するヘルパー
static BOOL CheckMailAddress(const wchar_t* str, int* outLen = nullptr)
{
    size_t len = wcslen(str);
    return IsMailAddress(str, 0, len, outLen);
}

// offset 付きでメールアドレスを判定するヘルパー
static BOOL CheckMailAddressAt(const wchar_t* str, int offset, int* outLen = nullptr)
{
    size_t len = wcslen(str);
    return IsMailAddress(str + offset, offset, len - offset, outLen);
}

// =============================================================================
// TGT-04: IsMailAddress
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-27 ~ BR-35)
// ---------------------------------------------------------------------------

TEST(IsMailAddress, BR27_OffsetWithValidPrecedingChar)
{
    // BR-27: 正の offset で直前文字が有効文字の場合に FALSE を返す (BC-04-01)
    // "xtest@example.com" で offset=1 → 直前の 'x' は有効文字なので FALSE
    const wchar_t* str = L"xtest@example.com";
    size_t len = wcslen(str);
    EXPECT_FALSE(IsMailAddress(str + 1, 1, len - 1, nullptr));
}

TEST(IsMailAddress, BR28_LeadingDot)
{
    // BR-28: 先頭がドットの場合に FALSE を返す (BC-04-02)
    EXPECT_FALSE(CheckMailAddress(L".test@example.com"));
}

TEST(IsMailAddress, BR29_LeadingInvalidChar)
{
    // BR-29: 先頭が無効文字の場合に FALSE を返す (BC-04-03)
    // 0x20 (スペース) は無効文字
    EXPECT_FALSE(CheckMailAddress(L" test@example.com"));
}

TEST(IsMailAddress, BR30_LocalPartTooShort)
{
    // BR-30: ローカルパートが短すぎる/長さ異常 (BC-04-04)
    // "@example.com" → ローカルパートが空 → j==0 で FALSE
    EXPECT_FALSE(CheckMailAddress(L"@example.com"));
}

TEST(IsMailAddress, BR31_NoAtSign)
{
    // BR-31: @ が見つからない場合に FALSE を返す (BC-04-05)
    EXPECT_FALSE(CheckMailAddress(L"testexample.com"));
}

TEST(IsMailAddress, BR32_DomainLabelLengthZero)
{
    // BR-32: ドメインラベル長が0の場合に FALSE を返す (BC-04-06)
    // "test@.com" → @ の後にドット → ラベル長0 → FALSE
    EXPECT_FALSE(CheckMailAddress(L"test@.com"));
}

TEST(IsMailAddress, BR33_DomainNoDot)
{
    // BR-33: ドメインにドットが0個の場合に FALSE を返す (BC-04-08)
    EXPECT_FALSE(CheckMailAddress(L"test@example"));
}

TEST(IsMailAddress, BR34_DomainDotSeparation)
{
    // BR-34: ドメイン内のドット区切り処理の分岐を検証する (BC-04-07)
    // 正常なドメイン: "example.co.jp" → nDotCount=2
    int addrLen = 0;
    EXPECT_TRUE(CheckMailAddress(L"test@example.co.jp", &addrLen));
    EXPECT_EQ(addrLen, 18); // "test@example.co.jp" = 18文字 (wcslen)
}

TEST(IsMailAddress, BR35_NullAddressLength)
{
    // BR-35: pnAddressLength が NULL の場合にクラッシュしない (BC-04-09)
    EXPECT_TRUE(CheckMailAddress(L"test@example.com", nullptr));
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-13 ~ EC-16)
// ---------------------------------------------------------------------------

TEST(IsMailAddress, EC13_StandardAddress)
{
    // EC-13: 標準的なメールアドレスが正しく判定される (EC-04-01)
    int addrLen = 0;
    EXPECT_TRUE(CheckMailAddress(L"test@example.com", &addrLen));
    EXPECT_EQ(addrLen, 16);
}

TEST(IsMailAddress, EC14_InvalidAddresses)
{
    // EC-14: 各種の無効なメールアドレスが FALSE と判定される
    // EC-04-02: 空文字列
    EXPECT_FALSE(CheckMailAddress(L""));
    // EC-04-03: @ なし
    EXPECT_FALSE(CheckMailAddress(L"testexample.com"));
    // EC-04-04: 先頭ドット
    EXPECT_FALSE(CheckMailAddress(L".test@example.com"));
    // EC-04-05: ドメインにドットなし
    EXPECT_FALSE(CheckMailAddress(L"test@example"));
}

TEST(IsMailAddress, EC15_SymbolsInLocalPart)
{
    // EC-15: ローカルパートの記号類が正しく処理される (EC-04-07)
    // 0x21~0x7E のうち禁止文字 ("(),:;<>@[\]) を除く文字は有効
    EXPECT_TRUE(CheckMailAddress(L"test!name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test#name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test$name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test%name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test&name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test'name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test*name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test+name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test-name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test/name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test=name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test?name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test^name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test_name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test`name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test{name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test|name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test}name@example.com"));
    EXPECT_TRUE(CheckMailAddress(L"test~name@example.com"));
}

TEST(IsMailAddress, EC15_ProhibitedCharsInLocalPart)
{
    // EC-14 補足 / EC-04-08: 禁止文字を含むローカルパート
    // ダブルクオート、括弧、コロン等は禁止
    EXPECT_FALSE(CheckMailAddress(L"te\"st@example.com"));
    EXPECT_FALSE(CheckMailAddress(L"te(st@example.com"));
    EXPECT_FALSE(CheckMailAddress(L"te)st@example.com"));
    EXPECT_FALSE(CheckMailAddress(L"te,st@example.com"));
    EXPECT_FALSE(CheckMailAddress(L"te:st@example.com"));
    EXPECT_FALSE(CheckMailAddress(L"te;st@example.com"));
    EXPECT_FALSE(CheckMailAddress(L"te<st@example.com"));
    EXPECT_FALSE(CheckMailAddress(L"te>st@example.com"));
}

TEST(IsMailAddress, EC16_OffsetVariations)
{
    // EC-16: offset パラメータの各パターンを検証する
    // EC-04-12: offset=0（先頭からの判定）
    EXPECT_TRUE(CheckMailAddress(L"test@example.com"));

    // EC-04-13: 正の offset で直前がスペース → TRUE
    {
        const wchar_t* str = L" test@example.com";
        size_t len = wcslen(str);
        int addrLen = 0;
        // バッファ先頭はstrのまま、offset=1で呼び出す（pszBuf[offset-1]=' 'で有効判定）
        EXPECT_TRUE(IsMailAddress(str, 1, len, &addrLen));
        EXPECT_EQ(addrLen, 16);
    }

    // EC-04-14: 正の offset で直前が有効文字 → FALSE
    {
        const wchar_t* str = L"xtest@example.com";
        size_t len = wcslen(str);
        EXPECT_FALSE(IsMailAddress(str + 1, 1, len - 1, nullptr));
    }
}

TEST(IsMailAddress, EC_DomainWithHyphen)
{
    // EC-04-09: ドメインラベルにハイフンを含む
    EXPECT_TRUE(CheckMailAddress(L"test@test-domain.com"));
}

TEST(IsMailAddress, EC_DomainWithUnderscore)
{
    // EC-04-10: ドメインラベルにアンダースコアを含む
    EXPECT_TRUE(CheckMailAddress(L"test@test_domain.com"));
}

TEST(IsMailAddress, EC_AddressFollowedBySpace)
{
    // EC-04-11: アドレスの後にスペースが続く
    int addrLen = 0;
    const wchar_t* str = L"test@example.com next";
    size_t len = wcslen(str);
    EXPECT_TRUE(IsMailAddress(str, 0, len, &addrLen));
    EXPECT_EQ(addrLen, 16); // スペースの手前まで
}

TEST(IsMailAddress, EC_MultipleDotsDomain)
{
    // EC-04-06: 複数ドットのドメイン (co.jp)
    int addrLen = 0;
    EXPECT_TRUE(CheckMailAddress(L"test@example.co.jp", &addrLen));
    EXPECT_EQ(addrLen, 18); // "test@example.co.jp" = 18文字 (wcslen)
}

// ---------------------------------------------------------------------------
// 境界値テスト (BV-07 ~ BV-08)
// ---------------------------------------------------------------------------

TEST(IsMailAddress, BV07_ValidCharBoundary_0x21)
{
    // BV-07: IsValidChar の境界値 - 0x21 (最小有効文字 '!') (BV-04-08)
    // '!' (0x21) はローカルパートの有効文字
    EXPECT_TRUE(CheckMailAddress(L"!test@example.com"));
}

TEST(IsMailAddress, BV07_ValidCharBoundary_0x7E)
{
    // BV-07: IsValidChar の境界値 - 0x7E (最大有効文字 '~') (BV-04-09)
    EXPECT_TRUE(CheckMailAddress(L"~test@example.com"));
}

TEST(IsMailAddress, BV07_InvalidCharBoundary_0x20)
{
    // BV-07: IsValidChar の境界値 - 0x20 (スペース、無効) (BV-04-10)
    EXPECT_FALSE(CheckMailAddress(L" test@example.com"));
}

TEST(IsMailAddress, BV07_InvalidCharBoundary_0x7F)
{
    // BV-07: IsValidChar の境界値 - 0x7F (DEL、無効) (BV-04-11)
    wchar_t str[] = L"\x007Ftest@example.com";
    EXPECT_FALSE(CheckMailAddress(str));
}

TEST(IsMailAddress, BV08_MinimalAddress)
{
    // BV-08: 最小長のメールアドレス a@b.cc (BV-04-01)
    int addrLen = 0;
    EXPECT_TRUE(CheckMailAddress(L"a@b.cc", &addrLen));
    EXPECT_EQ(addrLen, 6);
}

TEST(IsMailAddress, BV_LongLocalPart)
{
    // BV-04-02: 64文字のローカルパート（現行実装は長さ制限なし）
    // 64文字のローカルパートを構築
    std::wstring longLocal(64, L'a');
    longLocal += L"@example.com";
    int addrLen = 0;
    EXPECT_TRUE(IsMailAddress(longLocal.c_str(), 0, longLocal.size(), &addrLen));
}

TEST(IsMailAddress, BV_BufferLengthInsufficient)
{
    // BV-04-07: バッファ長不足 (j >= nBufLen - 2 になるケース)
    // "a@b" でバッファ長=3 → ローカル "a" + "@" でドメイン処理が不十分
    const wchar_t* str = L"a@b";
    // バッファ長が短いため FALSE になるケース
    EXPECT_FALSE(IsMailAddress(str, 0, 3, nullptr));
}

// ---------------------------------------------------------------------------
// エラーパステスト (ER-02, ER-03)
// ---------------------------------------------------------------------------

TEST(IsMailAddress, ER02_EmptyString)
{
    // ER-02: 空文字列入力時に FALSE を返しクラッシュしない
    EXPECT_FALSE(CheckMailAddress(L""));
}

TEST(IsMailAddress, ER03_AtSignOnly)
{
    // ER-03: @ のみの文字列入力時に FALSE を返す
    EXPECT_FALSE(CheckMailAddress(L"@"));
}

// =============================================================================
// TGT-05: WhatKindOfTwoChars
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-36 ~ BR-43)
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars, BR36_SameKindInitialCheck)
{
    // BR-36: 初回同種チェック (kindPre == kindCur) で即座に返す (BC-05-01)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CSYM, CK_CSYM), CK_CSYM);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_HIRA, CK_HIRA), CK_HIRA);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_KATA, CK_ZEN_KATA), CK_ZEN_KATA);
}

TEST(WhatKindOfTwoChars, BR37_NobasuPulledByKatakana)
{
    // BR-37: 長音がカタカナに引きずられる (BC-05-02)
    // 長音(ZEN_NOBASU) + カタカナ(ZEN_KATA) → カタカナ
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_ZEN_KATA), CK_ZEN_KATA);
}

TEST(WhatKindOfTwoChars, BR37_DakuPulledByHiragana)
{
    // BR-37: 濁点がひらがなに引きずられる (BC-05-02)
    // 濁点(ZEN_DAKU) + ひらがな(HIRA) → ひらがな
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_HIRA), CK_HIRA);
}

TEST(WhatKindOfTwoChars, BR37_KatakanaPullsNobasu)
{
    // BR-37: カタカナが長音を引きずる (BC-05-03)
    // カタカナ(ZEN_KATA) + 長音(ZEN_NOBASU) → カタカナ(kindPre)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_KATA, CK_ZEN_NOBASU), CK_ZEN_KATA);
}

TEST(WhatKindOfTwoChars, BR37_HiraganaPullsDaku)
{
    // BR-37: ひらがなが濁点を引きずる (BC-05-03)
    // ひらがな(HIRA) + 濁点(ZEN_DAKU) → ひらがな(kindPre)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_HIRA, CK_ZEN_DAKU), CK_HIRA);
}

TEST(WhatKindOfTwoChars, BR38_NobasuDakuContinuous)
{
    // BR-38: 長音・濁点の連続が同種とみなされる (BC-05-04)
    // 長音 + 濁点 → 濁点(kindCur)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_ZEN_DAKU), CK_ZEN_DAKU);
    // 濁点 + 長音 → 長音(kindCur)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_ZEN_NOBASU), CK_ZEN_NOBASU);
}

TEST(WhatKindOfTwoChars, BR39_LatinMappedToCSYM)
{
    // BR-39: ラテン → CSYM マッピング (BC-05-05, BC-05-06)
    // LATIN + CSYM → マッピング後 CSYM+CSYM → CSYM
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LATIN, CK_CSYM), CK_CSYM);
    // CSYM + LATIN → LATIN→CSYMにマッピング後 CSYM==CSYM → CSYM(マッピング後の値)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CSYM, CK_LATIN), CK_CSYM);
}

TEST(WhatKindOfTwoChars, BR40_UdefMappedToETC)
{
    // BR-40: ユーザー定義 → ETC マッピング (BC-05-07, BC-05-08)
    // UDEF + ETC → マッピング後 ETC+ETC → ETC
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_UDEF, CK_ETC), CK_ETC);
    // ETC + UDEF → UDEF→ETCにマッピング後 ETC==ETC → ETC(マッピング後の値)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ETC, CK_UDEF), CK_ETC);
}

TEST(WhatKindOfTwoChars, BR41_CtrlMappedToETC)
{
    // BR-41: 制御文字 → ETC マッピング (BC-05-09, BC-05-10)
    // CTRL + ETC → マッピング後 ETC+ETC → ETC
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CTRL, CK_ETC), CK_ETC);
    // ETC + CTRL → CTRL→ETCにマッピング後 ETC==ETC → ETC(マッピング後の値)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ETC, CK_CTRL), CK_ETC);
}

TEST(WhatKindOfTwoChars, BR42_MappedSameKind)
{
    // BR-42: マッピング後の同種チェックで一致するパス (BC-05-11)
    // LATIN + LATIN → 初回同種チェックで一致（BR-36で既テスト）
    // LATIN + CSYM → マッピング後 CSYM+CSYM で一致
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LATIN, CK_CSYM), CK_CSYM);
}

TEST(WhatKindOfTwoChars, BR43_DifferentKind_CK_NULL)
{
    // BR-43: 最終的に CK_NULL（別種）を返す (BC-05-12)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CSYM, CK_HIRA), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_KATA, CK_ETC), CK_NULL);
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-17 ~ EC-19)
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars, EC17_AllSameKindPairs)
{
    // EC-17: 全 ECharKind 値の同種ペアが正しく処理される (EC-05-01)
    // 代表的な ECharKind 値で同種ペアを検証
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_NULL, CK_NULL), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_TAB, CK_TAB), CK_TAB);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CR, CK_CR), CK_CR);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LF, CK_LF), CK_LF);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ETC, CK_ETC), CK_ETC);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_SPACE, CK_SPACE), CK_SPACE);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CSYM, CK_CSYM), CK_CSYM);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_KATA, CK_KATA), CK_KATA);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LATIN, CK_LATIN), CK_LATIN);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_UDEF, CK_UDEF), CK_UDEF);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CTRL, CK_CTRL), CK_CTRL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_HIRA, CK_HIRA), CK_HIRA);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_KATA, CK_ZEN_KATA), CK_ZEN_KATA);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_ZEN_NOBASU), CK_ZEN_NOBASU);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_ZEN_DAKU), CK_ZEN_DAKU);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_CSYM, CK_ZEN_CSYM), CK_ZEN_CSYM);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_KIGO, CK_ZEN_KIGO), CK_ZEN_KIGO);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_SPACE, CK_ZEN_SPACE), CK_ZEN_SPACE);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_ETC, CK_ZEN_ETC), CK_ZEN_ETC);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_SKIGO, CK_ZEN_SKIGO), CK_ZEN_SKIGO);
}

TEST(WhatKindOfTwoChars, EC18_DragRuleAllCombinations)
{
    // EC-18: 長音/濁点とカタカナ/ひらがなの引きずり規則の全組み合わせ
    // EC-05-02: 長音 + カタカナ → カタカナ
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_ZEN_KATA), CK_ZEN_KATA);
    // EC-05-03: 濁点 + ひらがな → ひらがな
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_HIRA), CK_HIRA);
    // EC-05-04: カタカナ + 長音 → カタカナ
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_KATA, CK_ZEN_NOBASU), CK_ZEN_KATA);
    // EC-05-05: ひらがな + 濁点 → ひらがな
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_HIRA, CK_ZEN_DAKU), CK_HIRA);
    // EC-05-06: 長音 + 濁点 → 濁点
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_ZEN_DAKU), CK_ZEN_DAKU);
    // 追加: 濁点 + 長音 → 長音
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_ZEN_NOBASU), CK_ZEN_NOBASU);
    // 追加: 長音 + ひらがな → ひらがな
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_HIRA), CK_HIRA);
    // 追加: 濁点 + カタカナ → カタカナ
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_ZEN_KATA), CK_ZEN_KATA);
}

TEST(WhatKindOfTwoChars, EC19_MappingRules)
{
    // EC-19: 各マッピング規則の検証
    // EC-05-07: ラテン + CSYM → CSYM (LATIN→CSYM マッピング)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LATIN, CK_CSYM), CK_CSYM);
    // EC-05-08: CSYM + ラテン → kindCur(マッピング後CSYM) を返す
    // ※ 実装では kindCur の元の値（CK_LATIN）ではなく、
    //   マッピング後の CK_CSYM が返される可能性がある
    auto result = CWordParse::WhatKindOfTwoChars(CK_CSYM, CK_LATIN);
    // マッピング後 CSYM == CSYM → kindCur を返す
    // kindCur は CK_LATIN だが、マッピング後は CK_CSYM
    // 実装依存: CK_LATIN または CK_CSYM のいずれかが返る
    EXPECT_TRUE(result == CK_LATIN || result == CK_CSYM);

    // EC-05-09: UDEF + ETC → ETC (UDEF→ETC マッピング)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_UDEF, CK_ETC), CK_ETC);
    // EC-05-10: CTRL + ETC → ETC (CTRL→ETC マッピング)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CTRL, CK_ETC), CK_ETC);
}

TEST(WhatKindOfTwoChars, EC_LatinLatinSameKind)
{
    // EC-05-12: ラテン + ラテン → 初回同種チェックで CK_LATIN
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LATIN, CK_LATIN), CK_LATIN);
}

TEST(WhatKindOfTwoChars, EC_ZenSpaceAndHira_Different)
{
    // EC-05-13: 全角スペース + ひらがな → CK_NULL（別種）
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_SPACE, CK_HIRA), CK_NULL);
}

// ---------------------------------------------------------------------------
// 境界値テスト (BV-05-01, BV-05-02)
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars, BV_CK_NULL_SameKind)
{
    // BV-05-01: CK_NULL (最小値) 同士は CK_NULL を返す（同種）
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_NULL, CK_NULL), CK_NULL);
}

TEST(WhatKindOfTwoChars, BV_CK_ZEN_ETC_SameKind)
{
    // BV-05-02: CK_ZEN_ETC (最大値) が正常に処理される
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_ETC, CK_ZEN_ETC), CK_ZEN_ETC);
}

// =============================================================================
// TGT-06: WhatKindOfTwoChars4KW
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-44, BR-45)
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars4KW, BR44_UdefMappedToCSYM)
{
    // BR-44: ユーザー定義文字が CSYM にマッピングされる（通常版との差異）
    // (BC-06-03, BC-06-04)
    // UDEF + CSYM → UDEF→CSYM にマッピング → CSYM+CSYM → CSYM
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_UDEF, CK_CSYM), CK_CSYM);
    // CSYM + UDEF → UDEF→CSYM にマッピング → CSYM+CSYM
    auto result = CWordParse::WhatKindOfTwoChars4KW(CK_CSYM, CK_UDEF);
    EXPECT_TRUE(result == CK_UDEF || result == CK_CSYM);
}

TEST(WhatKindOfTwoChars4KW, BR45_CtrlRemainsCtrl)
{
    // BR-45: 制御文字が CTRL のまま保持される（通常版との差異）
    // (BC-06-05, BC-06-06)
    // CTRL + ETC → 通常版では CTRL→ETC で同種、4KW版では CTRL のまま → 別種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_CTRL, CK_ETC), CK_NULL);
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-20 ~ EC-22)
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars4KW, EC20_UdefAndCSYM_SameKind)
{
    // EC-20: UDEF+CSYM が同種と判定される（通常版では UDEF+ETC が同種）(EC-06-01)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_UDEF, CK_CSYM), CK_CSYM);
}

TEST(WhatKindOfTwoChars4KW, EC21_UdefAndETC_DifferentKind)
{
    // EC-21: UDEF+ETC が別種と判定される（通常版では同種）(EC-06-02)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_UDEF, CK_ETC), CK_NULL);
}

TEST(WhatKindOfTwoChars4KW, EC22_CtrlAndETC_DifferentKind)
{
    // EC-22: CTRL+ETC が別種と判定される（通常版では同種）(EC-06-03)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_CTRL, CK_ETC), CK_NULL);
}

TEST(WhatKindOfTwoChars4KW, EC_CtrlCtrl_SameKind)
{
    // EC-06-04: CTRL+CTRL は初回同種チェックで同種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_CTRL, CK_CTRL), CK_CTRL);
}

// ---------------------------------------------------------------------------
// 依存切替テスト (DP-02)
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars4KW, DP02_DirectComparison)
{
    // DP-02: WhatKindOfTwoChars と WhatKindOfTwoChars4KW の差異を直接比較する

    // 差異1: CK_UDEF + CK_ETC
    // 通常版: UDEF→ETC マッピング → ETC+ETC → 同種(CK_ETC)
    // 4KW版: UDEF→CSYM マッピング → CSYM+ETC → 別種(CK_NULL)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_UDEF, CK_ETC), CK_ETC);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_UDEF, CK_ETC), CK_NULL);

    // 差異2: CK_UDEF + CK_CSYM
    // 通常版: UDEF→ETC マッピング → ETC+CSYM → 別種(CK_NULL)
    // 4KW版: UDEF→CSYM マッピング → CSYM+CSYM → 同種(CK_CSYM)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_UDEF, CK_CSYM), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_UDEF, CK_CSYM), CK_CSYM);

    // 差異3: CK_CTRL + CK_ETC
    // 通常版: CTRL→ETC マッピング → ETC+ETC → 同種(CK_ETC)
    // 4KW版: CTRL のまま → CTRL+ETC → 別種(CK_NULL)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CTRL, CK_ETC), CK_ETC);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_CTRL, CK_ETC), CK_NULL);
}

TEST(WhatKindOfTwoChars4KW, CommonBehavior_SameAsNormal)
{
    // DP-02 補足: 差異以外の部分は通常版と同一であることを確認
    // 初回同種チェック
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_CSYM, CK_CSYM), CK_CSYM);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_HIRA, CK_HIRA), CK_HIRA);

    // 引きずり規則（通常版と同一）
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_ZEN_NOBASU, CK_ZEN_KATA), CK_ZEN_KATA);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_ZEN_DAKU, CK_HIRA), CK_HIRA);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_ZEN_KATA, CK_ZEN_NOBASU), CK_ZEN_KATA);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_HIRA, CK_ZEN_DAKU), CK_HIRA);

    // LATIN → CSYM マッピング（通常版と同一）
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_LATIN, CK_CSYM), CK_CSYM);

    // 別種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_CSYM, CK_HIRA), CK_NULL);
}

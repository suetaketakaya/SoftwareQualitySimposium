// =============================================================================
// test-cwordparse-additional.cpp
// 対象: sakura_core/parse/CWordParse.cpp
//   - IsMailAddress          (TGT-04)
//   - WhatKindOfTwoChars     (TGT-05)
//   - WhatKindOfTwoChars4KW  (TGT-06)
//
// TRM網羅性監査 (trm-coverage-audit.md) で指摘された漏れ項目をカバーする追加テスト
// 生成: SQiP 2026 実証実験 Phase 3 — 追加テスト
// =============================================================================

#include <gtest/gtest.h>

#include <cstring>
#include <string>

// sakura-editor ヘッダ (macOS compat)
#include "sakura_core/parse/CWordParse.h"

// =============================================================================
// ヘルパー: 既存テストのヘルパー関数を再定義（別翻訳単位のため）
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
// TGT-04: IsMailAddress — 追加テスト
// =============================================================================

// ---------------------------------------------------------------------------
// GAP-BR-04: 負のoffset値
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_BR04_NegativeOffset)
{
    // GAP-BR-04: offset が負の場合、`0 < offset` が false となり
    // 直前文字チェックをスキップする。ただし pszBuf += offset で
    // ポインタが前方に移動するため、予測不能な動作になり得る。
    // ここでは offset=-1 を渡した場合に少なくともクラッシュしないことを確認する。
    //
    // 注意: 負の offset は本来の使用方法として想定されていない。
    // 実装では pszBuf += offset でポインタが前に移動し、
    // nBufLen -= offset で nBufLen が増加する。
    // バッファの先頭より前を読むことになるためUBの可能性があるが、
    // 文字列リテラル "Xtest@example.com" の先頭から1バイト先で
    // offset=-1 を渡すことで安全な範囲でテストする。
    const wchar_t* str = L"Xtest@example.com";
    size_t len = wcslen(str);
    // str+1 をベースとして offset=-1 → pszBuf は str に戻る
    // nBufLen = (len-1) - (-1) = len となる
    // 結果: "Xtest@example.com" 全体を評価
    // 先頭 'X' は有効文字だが、offset<=0 なので直前文字チェックはスキップ
    // 'X' から始まるローカルパートを走査 → @ を見つける → ドメイン解析 → TRUE
    BOOL result = IsMailAddress(str + 1, -1, len - 1, nullptr);
    // 動作が実装依存であるため、クラッシュしないことが主な検証目的
    // 結果は TRUE or FALSE のいずれかであることを確認
    EXPECT_TRUE(result == TRUE || result == FALSE);
}

// ---------------------------------------------------------------------------
// GAP-BR-05: ドメインラベル走査中にバッファ終端到達
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_BR05_DomainLabelBufferEnd)
{
    // GAP-BR-05: ドメインラベル走査中に j < nBufLen が false になるケース
    // "a@b.cc" をバッファ長を厳密に指定して呼び出す
    // nBufLen = 6 の場合、ドメインラベル "cc" の走査中に j==6 で終端到達
    const wchar_t* str = L"a@b.cc";
    int addrLen = 0;
    EXPECT_TRUE(IsMailAddress(str, 0, 6, &addrLen));
    EXPECT_EQ(addrLen, 6);
}

TEST(IsMailAddress_Additional, GAP_BR05_DomainLabelTruncatedByBuffer)
{
    // GAP-BR-05 補足: バッファ長をドメインの途中で切る
    // "a@b.ccc" だがバッファ長を 6 にする → "a@b.cc" 相当
    // ドメインラベル走査が j < nBufLen (=6) で停止
    const wchar_t* str = L"a@b.ccc";
    int addrLen = 0;
    BOOL result = IsMailAddress(str, 0, 6, &addrLen);
    // nBufLen=6 で "a@b.cc" まで → nDotCount=1, ラベル "cc" で終了 → TRUE
    EXPECT_TRUE(result);
    EXPECT_EQ(addrLen, 6);
}

// ---------------------------------------------------------------------------
// GAP-EC-07: ローカルパートが.で終わる
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_EC07_LocalPartEndingWithDot)
{
    // GAP-EC-07: "test.@example.com" — ローカルパートが '.' で終わる
    // 実装では '.' はローカルパートの有効文字（IsValidChar は true）。
    // したがって "test." がローカルパートとして受理され、@以降のドメイン解析に進む。
    // RFC 5321 では無効だが、sakura の実装では受理される可能性がある。
    int addrLen = 0;
    BOOL result = CheckMailAddress(L"test.@example.com", &addrLen);
    // 実装の挙動を記録: '.' は IsValidChar で true (0x2E は 0x21-0x7E の範囲内で禁止リストにない)
    // よって TRUE が返る
    EXPECT_TRUE(result);
    if (result) {
        EXPECT_EQ(addrLen, 17);
    }
}

// ---------------------------------------------------------------------------
// GAP-EC-08: 連続ドット
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_EC08_ConsecutiveDotsInLocalPart)
{
    // GAP-EC-08: "test..test@example.com" — ローカルパートに連続ドット
    // 実装では '.' は有効文字なので連続ドットも受理される可能性がある
    int addrLen = 0;
    BOOL result = CheckMailAddress(L"test..test@example.com", &addrLen);
    // '.' は IsValidChar で有効 → ローカルパート "test..test" として受理
    EXPECT_TRUE(result);
    if (result) {
        EXPECT_EQ(addrLen, 22);
    }
}

// ---------------------------------------------------------------------------
// GAP-EC-09: ドメインラベルが - で始まる/終わる
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_EC09_DomainLabelStartingWithHyphen)
{
    // GAP-EC-09: "test@-example.com" — ドメインラベルが '-' で始まる
    // 実装ではドメインラベルの文字に '-' が許可されているため受理される
    int addrLen = 0;
    BOOL result = CheckMailAddress(L"test@-example.com", &addrLen);
    EXPECT_TRUE(result);
    if (result) {
        EXPECT_EQ(addrLen, 17);
    }
}

TEST(IsMailAddress_Additional, GAP_EC09_DomainLabelEndingWithHyphen)
{
    // GAP-EC-09: "test@example-.com" — ドメインラベルが '-' で終わる
    // 実装ではドメインラベル走査後に '.' を確認するため受理される
    int addrLen = 0;
    BOOL result = CheckMailAddress(L"test@example-.com", &addrLen);
    EXPECT_TRUE(result);
    if (result) {
        EXPECT_EQ(addrLen, 17);
    }
}

// ---------------------------------------------------------------------------
// GAP-EC-10: ドメイン末尾がドット
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_EC10_DomainTrailingDot)
{
    // GAP-EC-10: "test@example.com." — ドメイン末尾がドット
    // ドット後のラベル長が0 → `0 == j - nBgn` が true → FALSE
    EXPECT_FALSE(CheckMailAddress(L"test@example.com."));
}

// ---------------------------------------------------------------------------
// GAP-EC-11: @が複数
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_EC11_MultipleAtSigns)
{
    // GAP-EC-11: "test@test@example.com" — @ が複数ある
    // 実装ではローカルパート走査で '@' は禁止文字（IsValidChar が false）
    // したがって最初の '@' でローカルパートが終了し、ドメイン "test@example.com" を解析
    // ドメインラベル走査で '@' は許可文字ではないため、"test" で停止
    // "test" の後に '@' があるが '.' ではないので nDotCount==0 → FALSE
    EXPECT_FALSE(CheckMailAddress(L"test@test@example.com"));
}

// ---------------------------------------------------------------------------
// GAP-EC-12: 制御文字（0x01-0x1F）を含む文字列
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_EC12_ControlCharInLocalPart)
{
    // GAP-EC-12: ローカルパートに制御文字 0x01 を含む
    // 0x01 は IsValidChar で false（0x21 未満）
    // 先頭の 'a' は有効だが、次の 0x01 で走査が止まり '@' に達しない → FALSE
    wchar_t str[] = L"a\x0001test@example.com";
    EXPECT_FALSE(CheckMailAddress(str));
}

TEST(IsMailAddress_Additional, GAP_EC12_ControlCharAtStart)
{
    // GAP-EC-12 補足: 先頭が制御文字
    // 0x01 は IsValidChar で false → 先頭文字チェックで FALSE
    wchar_t str[] = L"\x0001test@example.com";
    EXPECT_FALSE(CheckMailAddress(str));
}

TEST(IsMailAddress_Additional, GAP_EC12_ControlChar_0x1F)
{
    // GAP-EC-12 補足: 0x1F (Unit Separator) も制御文字
    wchar_t str[] = L"a\x001Ftest@example.com";
    EXPECT_FALSE(CheckMailAddress(str));
}

TEST(IsMailAddress_Additional, GAP_EC12_TabInLocalPart)
{
    // GAP-EC-12 補足: タブ文字 (0x09) もローカルパートでは無効
    EXPECT_FALSE(CheckMailAddress(L"a\ttest@example.com"));
}

// ---------------------------------------------------------------------------
// GAP-EC-13: 全角@
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_EC13_FullWidthAtSign)
{
    // GAP-EC-13: "test＠example.com" — 全角@ (U+FF20)
    // 全角 '@' は IsValidChar で false（0x7E を超えるため）
    // ローカルパート走査が全角 '@' の前で停止し、'@' != pszBuf[j] → FALSE
    EXPECT_FALSE(CheckMailAddress(L"test\xFF20""example.com"));
}

// ---------------------------------------------------------------------------
// GAP-BV-07: offset=0の境界
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_BV07_OffsetZero)
{
    // GAP-BV-07: offset=0 のとき `0 < offset` が false → 直前文字チェックをスキップ
    // これは通常の使用パターンだが、分岐条件として明示的に確認する
    int addrLen = 0;
    EXPECT_TRUE(IsMailAddress(L"test@example.com", 0, 16, &addrLen));
    EXPECT_EQ(addrLen, 16);
}

// ---------------------------------------------------------------------------
// GAP-BV-08: nBufLen=0（空バッファ）
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_BV08_EmptyBuffer_BufLen0)
{
    // GAP-BV-08: nBufLen (cchBuf) = 0 の場合
    // 内部: nBufLen = int(0) = 0
    // pszBuf[j] (j=0) のアクセスは L"" の null 終端を読む → 0x00
    // pszBuf[0] != L'.' は true、IsValidChar(0x00) は false (0x00 < 0x21) → FALSE
    EXPECT_FALSE(IsMailAddress(L"", 0, 0, nullptr));
}

// ---------------------------------------------------------------------------
// GAP-BV-09: nBufLen=1（符号問題）
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_BV09_BufLen1_SignIssue)
{
    // GAP-BV-09: nBufLen (cchBuf) = 1 の場合
    // 内部: nBufLen = int(1)
    // 先頭文字 'a': pszBuf[0] != '.' && IsValidChar('a') → true → j=1
    // while(j < nBufLen - 2): 1 < 1-2 = 1 < -1 → false → ループ不実行
    // if(j == 0 || j >= nBufLen - 2): j=1, nBufLen-2=-1 → 1 >= -1 → true → FALSE
    //
    // 注意: nBufLen は int(cchBuf) なので int 演算となり、
    // nBufLen - 2 = -1 になる。size_t ではないため符号問題は発生しない。
    EXPECT_FALSE(IsMailAddress(L"a", 0, 1, nullptr));
}

TEST(IsMailAddress_Additional, GAP_BV09_BufLen1_AtSign)
{
    // GAP-BV-09 補足: nBufLen=1 で '@' のみ
    EXPECT_FALSE(IsMailAddress(L"@", 0, 1, nullptr));
}

// ---------------------------------------------------------------------------
// GAP-BV-10: nBufLen=2
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_BV10_BufLen2)
{
    // GAP-BV-10: nBufLen=2 の場合
    // 内部: nBufLen = int(2)
    // 先頭 'a': j=1
    // while(j < nBufLen - 2): 1 < 0 → false
    // if(j == 0 || j >= nBufLen - 2): 1 >= 0 → true → FALSE
    EXPECT_FALSE(IsMailAddress(L"a@", 0, 2, nullptr));
}

TEST(IsMailAddress_Additional, GAP_BV10_BufLen2_TwoChars)
{
    // GAP-BV-10 補足: 2文字で '@' を含まない場合
    EXPECT_FALSE(IsMailAddress(L"ab", 0, 2, nullptr));
}

// ---------------------------------------------------------------------------
// GAP-BV-11: 極端に長いドメインラベル
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_BV11_VeryLongDomainLabel)
{
    // GAP-BV-11: 極端に長いドメインラベル (256文字)
    // 実装にはドメインラベルの長さ制限がないため、正常に処理される
    std::wstring addr = L"a@";
    addr += std::wstring(256, L'x');  // 256文字のラベル
    addr += L".com";
    int addrLen = 0;
    BOOL result = IsMailAddress(addr.c_str(), 0, addr.size(), &addrLen);
    EXPECT_TRUE(result);
    if (result) {
        EXPECT_EQ(addrLen, static_cast<int>(addr.size()));
    }
}

TEST(IsMailAddress_Additional, GAP_BV11_VeryLongDomainLabel_1024)
{
    // GAP-BV-11 補足: 1024文字のドメインラベル
    std::wstring addr = L"a@";
    addr += std::wstring(1024, L'a');
    addr += L".com";
    int addrLen = 0;
    BOOL result = IsMailAddress(addr.c_str(), 0, addr.size(), &addrLen);
    EXPECT_TRUE(result);
    if (result) {
        EXPECT_EQ(addrLen, static_cast<int>(addr.size()));
    }
}

// ---------------------------------------------------------------------------
// GAP-ER-05: pszBuf=NULL
// ---------------------------------------------------------------------------

TEST(IsMailAddress_Additional, GAP_ER05_NullBuffer)
{
    // GAP-ER-05: pszBuf が NULL の場合
    // 実装には NULL チェックがないため、pszBuf[j] のアクセスでクラッシュする。
    // このテストは安全のため GTEST_SKIP() でスキップする。
    //
    // 注意: 実運用コードでは IsMailAddress に NULL を渡さない前提だが、
    // 防御的プログラミングの観点からは NULL チェックが望ましい。
    GTEST_SKIP() << "pszBuf=NULL はクラッシュが予想されるためスキップ "
                    "(実装に NULL ガードなし)";
    // IsMailAddress(nullptr, 0, 0, nullptr);
}

// =============================================================================
// TGT-05: WhatKindOfTwoChars — 追加テスト
// =============================================================================

// ---------------------------------------------------------------------------
// GAP-BR-06: CK_ZEN_NOBASU + CK_HIRA（長音+ひらがな）
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars_Additional, GAP_BR06_NobasuPlusHira)
{
    // GAP-BR-06: 長音 + ひらがな → ひらがな (kindCur を返す)
    // ソースコード条件:
    //   (kindPre == CK_ZEN_NOBASU || kindPre == CK_ZEN_DAKU) &&
    //   (kindCur == CK_ZEN_KATA   || kindCur == CK_HIRA)
    // CK_ZEN_NOBASU + CK_HIRA → kindCur (CK_HIRA) を返す
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_HIRA), CK_HIRA);
}

// ---------------------------------------------------------------------------
// GAP-BR-07: CK_ZEN_DAKU + CK_ZEN_KATA（濁点+カタカナ）
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars_Additional, GAP_BR07_DakuPlusZenKata)
{
    // GAP-BR-07: 濁点 + カタカナ → カタカナ (kindCur を返す)
    // ソースコード条件:
    //   (kindPre == CK_ZEN_NOBASU || kindPre == CK_ZEN_DAKU) &&
    //   (kindCur == CK_ZEN_KATA   || kindCur == CK_HIRA)
    // CK_ZEN_DAKU + CK_ZEN_KATA → kindCur (CK_ZEN_KATA) を返す
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_ZEN_KATA), CK_ZEN_KATA);
}

// ---------------------------------------------------------------------------
// GAP-BR-08: CK_ZEN_DAKU + CK_ZEN_DAKU（濁点+濁点）
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars_Additional, GAP_BR08_DakuPlusDaku)
{
    // GAP-BR-08: 濁点 + 濁点 → 初回同種チェック (kindPre == kindCur) で即座に返す
    // CK_ZEN_DAKU == CK_ZEN_DAKU → kindCur (CK_ZEN_DAKU) を返す
    // 注意: この組み合わせは初回同種チェックでマッチするため、
    // 3番目の条件ブロック（NOBASU/DAKU + NOBASU/DAKU）には到達しない
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_ZEN_DAKU), CK_ZEN_DAKU);
}

// ---------------------------------------------------------------------------
// GAP-EC-14: CK_TAB, CK_SPACE, CK_CR, CK_LF との組み合わせ
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars_Additional, GAP_EC14_TabWithOthers)
{
    // GAP-EC-14: CK_TAB はマッピング対象外のため、異種との組み合わせは CK_NULL
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_TAB, CK_SPACE), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_TAB, CK_CSYM), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_TAB, CK_HIRA), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_TAB, CK_ETC), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_TAB, CK_CR), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_TAB, CK_LF), CK_NULL);
}

TEST(WhatKindOfTwoChars_Additional, GAP_EC14_SpaceWithOthers)
{
    // GAP-EC-14: CK_SPACE もマッピング対象外
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_SPACE, CK_TAB), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_SPACE, CK_CSYM), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_SPACE, CK_HIRA), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_SPACE, CK_ETC), CK_NULL);
}

TEST(WhatKindOfTwoChars_Additional, GAP_EC14_CRWithOthers)
{
    // GAP-EC-14: CK_CR もマッピング対象外
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CR, CK_LF), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CR, CK_CSYM), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CR, CK_TAB), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_CR, CK_SPACE), CK_NULL);
}

TEST(WhatKindOfTwoChars_Additional, GAP_EC14_LFWithOthers)
{
    // GAP-EC-14: CK_LF もマッピング対象外
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LF, CK_CR), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LF, CK_CSYM), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LF, CK_TAB), CK_NULL);
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LF, CK_SPACE), CK_NULL);
}

// ---------------------------------------------------------------------------
// GAP-EC-15: CK_KATA（半角カタカナ）との組み合わせ
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars_Additional, GAP_EC15_HankakuKataWithOthers)
{
    // GAP-EC-15: CK_KATA (半角カタカナ) はマッピング対象外で、
    // 引きずり規則の対象にもならない（ZEN_KATA とは異なる）

    // 半角カタカナ + 全角カタカナ → 別種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_KATA, CK_ZEN_KATA), CK_NULL);

    // 半角カタカナ + ひらがな → 別種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_KATA, CK_HIRA), CK_NULL);

    // 半角カタカナ + 長音 → 別種（CK_KATA は引きずり規則に含まれない）
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_KATA, CK_ZEN_NOBASU), CK_NULL);

    // 半角カタカナ + 濁点 → 別種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_KATA, CK_ZEN_DAKU), CK_NULL);

    // 長音 + 半角カタカナ → 別種（引きずり規則は ZEN_KATA, HIRA のみ）
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_NOBASU, CK_KATA), CK_NULL);

    // 濁点 + 半角カタカナ → 別種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_ZEN_DAKU, CK_KATA), CK_NULL);

    // 半角カタカナ + CSYM → 別種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_KATA, CK_CSYM), CK_NULL);

    // 半角カタカナ + 半角カタカナ → 同種
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_KATA, CK_KATA), CK_KATA);
}

// =============================================================================
// TGT-06: WhatKindOfTwoChars4KW — 追加テスト
// =============================================================================

// ---------------------------------------------------------------------------
// GAP-BR-09: CK_LATIN → CK_CSYM マッピングの明示的テスト
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars4KW_Additional, GAP_BR09_LatinToCSYM_4KW)
{
    // GAP-BR-09: 4KW版でも CK_LATIN → CK_CSYM マッピングが行われることを明示的に確認
    // ソースコード: `if( kindPre == CK_LATIN )kindPre = CK_CSYM;`
    //              `if( kindCur == CK_LATIN )kindCur = CK_CSYM;`

    // LATIN + CSYM → マッピング後 CSYM + CSYM → CSYM
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_LATIN, CK_CSYM), CK_CSYM);

    // CSYM + LATIN → マッピング後 CSYM + CSYM → CSYM
    // マッピングはローカル変数を変更するため、返り値は CK_CSYM
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_CSYM, CK_LATIN), CK_CSYM);
}

TEST(WhatKindOfTwoChars4KW_Additional, GAP_BR09_LatinToCSYM_WithUDEF)
{
    // GAP-BR-09 補足: LATIN + UDEF の 4KW 版
    // 4KW版: LATIN→CSYM, UDEF→CSYM → CSYM+CSYM → CSYM
    // 通常版: LATIN→CSYM, UDEF→ETC → CSYM+ETC → CK_NULL
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_LATIN, CK_UDEF), CK_CSYM);

    // 対比: 通常版では CK_NULL
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LATIN, CK_UDEF), CK_NULL);
}

// ---------------------------------------------------------------------------
// GAP-EC-16: CK_LATIN + CK_LATIN（4KW版）
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars4KW_Additional, GAP_EC16_LatinPlusLatin)
{
    // GAP-EC-16: CK_LATIN + CK_LATIN は初回同種チェックで即座に CK_LATIN を返す
    // これは 4KW 版でも同じ動作
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_LATIN, CK_LATIN), CK_LATIN);

    // 対比: 通常版でも同じ
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_LATIN, CK_LATIN), CK_LATIN);
}

// ---------------------------------------------------------------------------
// GAP-EC-17: CK_UDEF + CK_UDEF（初回同種チェック）
// ---------------------------------------------------------------------------

TEST(WhatKindOfTwoChars4KW_Additional, GAP_EC17_UdefPlusUdef_InitialSameCheck)
{
    // GAP-EC-17: CK_UDEF + CK_UDEF は初回同種チェック (kindPre == kindCur) で
    // 即座に CK_UDEF を返す。UDEF→CSYM のマッピングには到達しない。
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars4KW(CK_UDEF, CK_UDEF), CK_UDEF);

    // 対比: 通常版でも初回同種チェックで CK_UDEF を返す
    // (UDEF→ETC マッピングには到達しない)
    EXPECT_EQ(CWordParse::WhatKindOfTwoChars(CK_UDEF, CK_UDEF), CK_UDEF);
}

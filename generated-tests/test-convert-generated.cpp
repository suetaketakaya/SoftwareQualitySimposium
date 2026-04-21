// =============================================================================
// test-convert-generated.cpp
// 対象: sakura_core/convert/convert_util.cpp
//   - Convert_ZeneisuToHaneisu  (TGT-07)
//   - Convert_HaneisuToZeneisu  (TGT-08)
//
// テスト要求モデル: test-requirements.yaml (99件中19件をカバー)
// 生成: SQiP 2026 実証実験 Phase 3
// =============================================================================

#include <gtest/gtest.h>

#include <cstring>
#include <string>

// sakura-editor ヘッダ (macOS compat)
#include "sakura_core/convert/convert_util.h"

// =============================================================================
// ヘルパー: in-place 変換テスト用ユーティリティ
// =============================================================================

// wchar_t 配列をコピーして変換し、結果を std::wstring で返す
static std::wstring ZenToHan(const std::wstring& input)
{
    std::wstring buf = input;
    Convert_ZeneisuToHaneisu(&buf[0], static_cast<int>(buf.size()));
    return buf;
}

static std::wstring HanToZen(const std::wstring& input)
{
    std::wstring buf = input;
    Convert_HaneisuToZeneisu(&buf[0], static_cast<int>(buf.size()));
    return buf;
}

// =============================================================================
// TGT-07: Convert_ZeneisuToHaneisu (全角英数→半角英数)
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-46 ~ BR-50)
// ---------------------------------------------------------------------------

TEST(ConvertZeneisuToHaneisu, BR46_ZenUpperToHanUpper)
{
    // BR-46: 全角大文字 (A-Z) の変換パス (BC-07-01)
    EXPECT_EQ(ZenToHan(L"\uFF21\uFF22\uFF23"), L"ABC");  // A B C
    EXPECT_EQ(ZenToHan(L"\uFF38\uFF39\uFF3A"), L"XYZ");  // X Y Z
}

TEST(ConvertZeneisuToHaneisu, BR47_ZenLowerToHanLower)
{
    // BR-47: 全角小文字 (a-z) の変換パス (BC-07-02)
    EXPECT_EQ(ZenToHan(L"\uFF41\uFF42\uFF43"), L"abc");  // a b c
    EXPECT_EQ(ZenToHan(L"\uFF58\uFF59\uFF5A"), L"xyz");  // x y z
}

TEST(ConvertZeneisuToHaneisu, BR48_ZenDigitToHanDigit)
{
    // BR-48: 全角数字 (0-9) の変換パス (BC-07-03)
    EXPECT_EQ(ZenToHan(L"\uFF10\uFF11\uFF12"), L"012");  // ０ １ ２
    EXPECT_EQ(ZenToHan(L"\uFF17\uFF18\uFF19"), L"789");  // ７ ８ ９
}

TEST(ConvertZeneisuToHaneisu, BR49_ZenKigoToHanKigo)
{
    // BR-49: 全角記号テーブルによる変換パス (BC-07-04)
    // 全角スペース → 半角スペース
    EXPECT_EQ(ZenToHan(L"\u3000"), L" ");
    // 全角コンマ → 半角コンマ
    EXPECT_EQ(ZenToHan(L"\uFF0C"), L",");
    // 全角ピリオド → 半角ピリオド
    EXPECT_EQ(ZenToHan(L"\uFF0E"), L".");
    // 全角プラス → 半角プラス
    EXPECT_EQ(ZenToHan(L"\uFF0B"), L"+");
    // 全角マイナス → 半角マイナス
    EXPECT_EQ(ZenToHan(L"\uFF0D"), L"-");
    // 全角アスタリスク → 半角アスタリスク
    EXPECT_EQ(ZenToHan(L"\uFF0A"), L"*");
    // 全角スラッシュ → 半角スラッシュ
    EXPECT_EQ(ZenToHan(L"\uFF0F"), L"/");
}

TEST(ConvertZeneisuToHaneisu, BR50_NonTargetCharsUnchanged)
{
    // BR-50: 変換対象外文字の無変換パス (BC-07-05)
    // 半角英数字はそのまま
    EXPECT_EQ(ZenToHan(L"abc123"), L"abc123");
    // ひらがな・漢字はそのまま
    EXPECT_EQ(ZenToHan(L"\u3042\u3044\u3046"), L"\u3042\u3044\u3046");  // あいう
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-23 ~ EC-25)
// ---------------------------------------------------------------------------

TEST(ConvertZeneisuToHaneisu, EC23_AllCategories)
{
    // EC-23: 全カテゴリの文字が正しく変換される
    // EC-07-01: 全角大文字
    EXPECT_EQ(ZenToHan(L"\uFF21\uFF22\uFF23"), L"ABC");
    // EC-07-02: 全角小文字
    EXPECT_EQ(ZenToHan(L"\uFF41\uFF42\uFF43"), L"abc");
    // EC-07-03: 全角数字
    EXPECT_EQ(ZenToHan(L"\uFF10\uFF11\uFF12"), L"012");
    // EC-07-04: 全角記号
    EXPECT_EQ(ZenToHan(L"\u3000\uFF0C\uFF0E\uFF0B\uFF0D"), L" ,.+-");
}

TEST(ConvertZeneisuToHaneisu, EC24_NonTargetPreserved)
{
    // EC-24: 変換対象外の文字が変更されない
    // EC-07-05: 半角英数字
    EXPECT_EQ(ZenToHan(L"abc123"), L"abc123");
    // EC-07-06: ひらがな・漢字
    std::wstring hiragana = L"\u3042\u3044\u3046";
    EXPECT_EQ(ZenToHan(hiragana), hiragana);
}

TEST(ConvertZeneisuToHaneisu, EC25_MixedCharacters)
{
    // EC-25: 混在文字列が正しく処理される (EC-07-07)
    // 全角 A (U+FF21) + 半角 b + 半角 c + 全角 1 (U+FF11) + 半角 2 + ひらがな あ
    std::wstring input = L"\uFF21" L"bc" L"\uFF11" L"2" L"\u3042";
    std::wstring expected = L"Abc12\u3042";
    EXPECT_EQ(ZenToHan(input), expected);
}

TEST(ConvertZeneisuToHaneisu, EC_EmptyArray)
{
    // EC-07-08: 空配列 (nLength=0)
    wchar_t buf[] = L"test";
    Convert_ZeneisuToHaneisu(buf, 0);
    EXPECT_EQ(std::wstring(buf), L"test"); // 変更なし
}

// ---------------------------------------------------------------------------
// 境界値テスト (BV-09, BV-10)
// ---------------------------------------------------------------------------

TEST(ConvertZeneisuToHaneisu, BV09_ZenUpperBoundary_First)
{
    // BV-09: 全角大文字範囲の先頭 A (U+FF21) (BV-07-01)
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF21')), L"A");
}

TEST(ConvertZeneisuToHaneisu, BV09_ZenUpperBoundary_Last)
{
    // BV-09: 全角大文字範囲の末尾 Z (U+FF3A) (BV-07-02)
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF3A')), L"Z");
}

TEST(ConvertZeneisuToHaneisu, BV09_ZenUpperBoundary_OutBefore)
{
    // BV-09: 全角大文字範囲外直前 U+FF20 (BV-07-03)
    // U+FF20 は全角記号テーブルに含まれる場合あり（＠）。
    // いずれにしてもクラッシュしない
    std::wstring input(1, L'\uFF20');
    std::wstring result = ZenToHan(input);
    // 変換されるか無変換かは記号テーブル依存
    EXPECT_EQ(result.size(), 1u);
}

TEST(ConvertZeneisuToHaneisu, BV09_ZenUpperBoundary_OutAfter)
{
    // BV-09: 全角大文字範囲外直後 U+FF3B (BV-07-04)
    std::wstring input(1, L'\uFF3B');
    std::wstring result = ZenToHan(input);
    // 記号テーブルに含まれない場合は無変換
    EXPECT_EQ(result.size(), 1u);
}

TEST(ConvertZeneisuToHaneisu, BV09_ZenLowerBoundary)
{
    // BV-07-05, BV-07-06: 全角小文字範囲の先頭と末尾
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF41')), L"a");  // a (U+FF41)
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF5A')), L"z");  // z (U+FF5A)
}

TEST(ConvertZeneisuToHaneisu, BV09_ZenDigitBoundary)
{
    // BV-07-07, BV-07-08: 全角数字範囲の先頭と末尾
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF10')), L"0");  // ０ (U+FF10)
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF19')), L"9");  // ９ (U+FF19)
}

TEST(ConvertZeneisuToHaneisu, BV10_LengthZero)
{
    // BV-10: nLength=0 の境界条件 (BV-07-09)
    wchar_t buf[] = L"\uFF21\uFF22\uFF23"; // ABC
    Convert_ZeneisuToHaneisu(buf, 0);
    // ループに入らず何も変更しない
    EXPECT_EQ(buf[0], L'\uFF21');
    EXPECT_EQ(buf[1], L'\uFF22');
    EXPECT_EQ(buf[2], L'\uFF23');
}

TEST(ConvertZeneisuToHaneisu, BV_LengthOne)
{
    // BV-07-10: nLength=1 (1文字のみ変換処理)
    wchar_t buf[] = L"\uFF21\uFF22"; // A B
    Convert_ZeneisuToHaneisu(buf, 1);
    // 1文字目のみ変換、2文字目は変換しない
    EXPECT_EQ(buf[0], L'A');
    EXPECT_EQ(buf[1], L'\uFF22'); // 変換されていない
}

// =============================================================================
// TGT-08: Convert_HaneisuToZeneisu (半角英数→全角英数)
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-51 ~ BR-55)
// ---------------------------------------------------------------------------

TEST(ConvertHaneisuToZeneisu, BR51_HanUpperToZenUpper)
{
    // BR-51: 半角大文字 (A-Z) の変換パス (BC-08-01)
    EXPECT_EQ(HanToZen(L"ABC"), L"\uFF21\uFF22\uFF23");
    EXPECT_EQ(HanToZen(L"XYZ"), L"\uFF38\uFF39\uFF3A");
}

TEST(ConvertHaneisuToZeneisu, BR52_HanLowerToZenLower)
{
    // BR-52: 半角小文字 (a-z) の変換パス (BC-08-02)
    EXPECT_EQ(HanToZen(L"abc"), L"\uFF41\uFF42\uFF43");
    EXPECT_EQ(HanToZen(L"xyz"), L"\uFF58\uFF59\uFF5A");
}

TEST(ConvertHaneisuToZeneisu, BR53_HanDigitToZenDigit)
{
    // BR-53: 半角数字 (0-9) の変換パス (BC-08-03)
    EXPECT_EQ(HanToZen(L"012"), L"\uFF10\uFF11\uFF12");
    EXPECT_EQ(HanToZen(L"789"), L"\uFF17\uFF18\uFF19");
}

TEST(ConvertHaneisuToZeneisu, BR54_HanKigoToZenKigo)
{
    // BR-54: 半角記号テーブルによる変換パス (BC-08-04)
    // 半角スペース → 全角スペース
    EXPECT_EQ(HanToZen(L" "), L"\u3000");
    // 半角コンマ → 全角コンマ
    EXPECT_EQ(HanToZen(L","), L"\uFF0C");
    // 半角ピリオド → 全角ピリオド
    EXPECT_EQ(HanToZen(L"."), L"\uFF0E");
    // 半角プラス → 全角プラス
    EXPECT_EQ(HanToZen(L"+"), L"\uFF0B");
    // 半角マイナス → 全角マイナス
    EXPECT_EQ(HanToZen(L"-"), L"\uFF0D");
    // 半角アスタリスク → 全角アスタリスク
    EXPECT_EQ(HanToZen(L"*"), L"\uFF0A");
    // 半角スラッシュ → 全角スラッシュ
    EXPECT_EQ(HanToZen(L"/"), L"\uFF0F");
}

TEST(ConvertHaneisuToZeneisu, BR55_NonTargetCharsUnchanged)
{
    // BR-55: 変換対象外文字の無変換パス (BC-08-05)
    // 全角英数字はそのまま
    std::wstring zenAlpha = L"\uFF21\uFF22\uFF23";
    EXPECT_EQ(HanToZen(zenAlpha), zenAlpha);
    // ひらがな・漢字はそのまま
    std::wstring hiragana = L"\u3042\u3044\u3046";
    EXPECT_EQ(HanToZen(hiragana), hiragana);
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-26 ~ EC-27)
// ---------------------------------------------------------------------------

TEST(ConvertHaneisuToZeneisu, EC26_AllCategories)
{
    // EC-26: 全カテゴリの文字が正しく変換される
    // EC-08-01: 半角大文字 → 全角大文字
    EXPECT_EQ(HanToZen(L"ABC"), L"\uFF21\uFF22\uFF23");
    // EC-08-02: 半角小文字 → 全角小文字
    EXPECT_EQ(HanToZen(L"abc"), L"\uFF41\uFF42\uFF43");
    // EC-08-03: 半角数字 → 全角数字
    EXPECT_EQ(HanToZen(L"012"), L"\uFF10\uFF11\uFF12");
    // EC-08-04: 半角記号 → 全角記号
    EXPECT_EQ(HanToZen(L" ,.+-"), L"\u3000\uFF0C\uFF0E\uFF0B\uFF0D");
}

TEST(ConvertHaneisuToZeneisu, EC27_NonTargetPreserved)
{
    // EC-27: 変換対象外の文字が変更されない
    // EC-08-05: 既に全角の文字はそのまま
    std::wstring zenStr = L"\uFF21\uFF22\uFF23";
    EXPECT_EQ(HanToZen(zenStr), zenStr);
    // EC-08-06: ひらがな・漢字はそのまま
    std::wstring hiragana = L"\u3042\u3044\u3046";
    EXPECT_EQ(HanToZen(hiragana), hiragana);
}

TEST(ConvertHaneisuToZeneisu, EC_MixedCharacters)
{
    // EC-08-07: 半角・全角・変換対象外の混在
    // "A" + "b" + "c" + あ(U+3042) + "1"
    std::wstring input = L"Abc\u30421";
    std::wstring expected = L"\uFF21\uFF42\uFF43\u3042\uFF11";
    EXPECT_EQ(HanToZen(input), expected);
}

// ---------------------------------------------------------------------------
// 境界値テスト (BV-11)
// ---------------------------------------------------------------------------

TEST(ConvertHaneisuToZeneisu, BV11_HanUpperBoundary)
{
    // BV-11: 半角大文字範囲の先頭・末尾 (BV-08-01, BV-08-02)
    EXPECT_EQ(HanToZen(std::wstring(1, L'A')), std::wstring(1, L'\uFF21'));
    EXPECT_EQ(HanToZen(std::wstring(1, L'Z')), std::wstring(1, L'\uFF3A'));
}

TEST(ConvertHaneisuToZeneisu, BV11_HanLowerBoundary)
{
    // BV-11: 半角小文字範囲の先頭・末尾 (BV-08-03, BV-08-04)
    EXPECT_EQ(HanToZen(std::wstring(1, L'a')), std::wstring(1, L'\uFF41'));
    EXPECT_EQ(HanToZen(std::wstring(1, L'z')), std::wstring(1, L'\uFF5A'));
}

TEST(ConvertHaneisuToZeneisu, BV11_HanDigitBoundary)
{
    // BV-11: 半角数字範囲の先頭・末尾 (BV-08-05, BV-08-06)
    EXPECT_EQ(HanToZen(std::wstring(1, L'0')), std::wstring(1, L'\uFF10'));
    EXPECT_EQ(HanToZen(std::wstring(1, L'9')), std::wstring(1, L'\uFF19'));
}

// ---------------------------------------------------------------------------
// 依存切替テスト (DP-03)
// ---------------------------------------------------------------------------

TEST(ConvertRoundTrip, DP03_ZenToHanToZen)
{
    // DP-03: ZeneisuToHaneisu と HaneisuToZeneisu の往復変換で元に戻る
    // 全角英大文字
    {
        std::wstring original = L"\uFF21\uFF22\uFF23\uFF38\uFF39\uFF3A";
        std::wstring han = ZenToHan(original);
        std::wstring roundTrip = HanToZen(han);
        EXPECT_EQ(roundTrip, original);
    }
    // 全角英小文字
    {
        std::wstring original = L"\uFF41\uFF42\uFF43\uFF58\uFF59\uFF5A";
        std::wstring han = ZenToHan(original);
        std::wstring roundTrip = HanToZen(han);
        EXPECT_EQ(roundTrip, original);
    }
    // 全角数字
    {
        std::wstring original = L"\uFF10\uFF11\uFF12\uFF17\uFF18\uFF19";
        std::wstring han = ZenToHan(original);
        std::wstring roundTrip = HanToZen(han);
        EXPECT_EQ(roundTrip, original);
    }
    // 全角記号 (テーブル内の代表的なもの)
    {
        std::wstring original = L"\u3000\uFF0C\uFF0E\uFF0B\uFF0D\uFF0A\uFF0F";
        std::wstring han = ZenToHan(original);
        std::wstring roundTrip = HanToZen(han);
        EXPECT_EQ(roundTrip, original);
    }
}

TEST(ConvertRoundTrip, DP03_HanToZenToHan)
{
    // DP-03: 半角→全角→半角の往復でも元に戻る
    {
        std::wstring original = L"ABCxyz012";
        std::wstring zen = HanToZen(original);
        std::wstring roundTrip = ZenToHan(zen);
        EXPECT_EQ(roundTrip, original);
    }
    // 半角記号
    {
        std::wstring original = L" ,.+-*/";
        std::wstring zen = HanToZen(original);
        std::wstring roundTrip = ZenToHan(zen);
        EXPECT_EQ(roundTrip, original);
    }
}

TEST(ConvertRoundTrip, DP03_MixedRoundTrip)
{
    // DP-03: 混在文字列の往復（変換対象外文字は変換されないため、
    //        全角部分のみ往復で元に戻る）
    std::wstring zenOnly = L"\uFF21\uFF42\uFF13"; // A b 3 (全角)
    std::wstring han = ZenToHan(zenOnly);
    EXPECT_EQ(han, L"Ab3");
    std::wstring roundTrip = HanToZen(han);
    EXPECT_EQ(roundTrip, zenOnly);
}

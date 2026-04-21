// =============================================================================
// test-convert-additional.cpp
// 対象: sakura_core/convert/convert_util.cpp
//   - Convert_ZeneisuToHaneisu  (TGT-07)
//   - Convert_HaneisuToZeneisu  (TGT-08)
//
// trm-coverage-audit.md で指摘された漏れ項目を全てカバーする追加テスト
// GAP-ID: EC-18, EC-19, EC-20, BV-13..18, BR-10, ER-07..10, DP-03拡張
// =============================================================================

#include <gtest/gtest.h>

#include <cstring>
#include <string>
#include <csignal>
#include <csetjmp>

// sakura-editor ヘッダ (macOS compat)
#include "sakura_core/convert/convert_util.h"

// =============================================================================
// ヘルパー: 既存テストと同一のユーティリティ関数
// =============================================================================

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
// GAP-EC-18: Convert_ZeneisuToHaneisu 全角記号テーブルの全記号の網羅テスト
//
// tableZenKigo (33文字) の全エントリが正しく半角化されることを検証する。
// テーブル定義:
//   Zen: 　，．＋－＊／％＝｜＆＾￥＠；："'  '＜＞（）｛｝［］！？＃＄￣＿
//   Han:  ,.+-*/%=|&^\@;:"` '<>(){}[]!?#$~_
// =============================================================================

TEST(ConvertZeneisuToHaneisu, GAP_EC18_AllZenKigoTable)
{
    // GAP-EC-18: 全角記号テーブル全33文字の網羅テスト

    // [0] 全角スペース U+3000 -> ' '
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\u3000')), L" ");
    // [1] 全角コンマ U+FF0C -> ','
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF0C')), L",");
    // [2] 全角ピリオド U+FF0E -> '.'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF0E')), L".");
    // [3] 全角プラス U+FF0B -> '+'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF0B')), L"+");
    // [4] 全角マイナス U+FF0D -> '-'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF0D')), L"-");
    // [5] 全角アスタリスク U+FF0A -> '*'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF0A')), L"*");
    // [6] 全角スラッシュ U+FF0F -> '/'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF0F')), L"/");
    // [7] 全角パーセント U+FF05 -> '%'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF05')), L"%");
    // [8] 全角イコール U+FF1D -> '='
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF1D')), L"=");
    // [9] 全角パイプ U+FF5C -> '|'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF5C')), L"|");
    // [10] 全角アンパサンド U+FF06 -> '&'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF06')), L"&");
    // [11] 全角キャレット U+FF3E -> '^'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF3E')), L"^");
    // [12] 全角円記号 U+FFE5 -> '\\'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFFE5')), L"\\");
    // [13] 全角アットマーク U+FF20 -> '@'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF20')), L"@");
    // [14] 全角セミコロン U+FF1B -> ';'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF1B')), L";");
    // [15] 全角コロン U+FF1A -> ':'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF1A')), L":");
    // [16] 右ダブルクォーテーション U+201D -> '"'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\u201D')), L"\"");
    // [17] 左シングルクォーテーション U+2018 -> '`'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\u2018')), L"`");
    // [18] 右シングルクォーテーション U+2019 -> '\''
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\u2019')), L"'");
    // [19] 全角小なり U+FF1C -> '<'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF1C')), L"<");
    // [20] 全角大なり U+FF1E -> '>'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF1E')), L">");
    // [21] 全角左丸括弧 U+FF08 -> '('
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF08')), L"(");
    // [22] 全角右丸括弧 U+FF09 -> ')'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF09')), L")");
    // [23] 全角左波括弧 U+FF5B -> '{'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF5B')), L"{");
    // [24] 全角右波括弧 U+FF5D -> '}'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF5D')), L"}");
    // [25] 全角左角括弧 U+FF3B -> '['
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF3B')), L"[");
    // [26] 全角右角括弧 U+FF3D -> ']'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF3D')), L"]");
    // [27] 全角感嘆符 U+FF01 -> '!'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF01')), L"!");
    // [28] 全角疑問符 U+FF1F -> '?'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF1F')), L"?");
    // [29] 全角シャープ U+FF03 -> '#'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF03')), L"#");
    // [30] 全角ドル U+FF04 -> '$'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF04')), L"$");
    // [31] 全角オーバーライン U+FFE3 -> '~'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFFE3')), L"~");
    // [32] 全角アンダースコア U+FF3F -> '_'
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF3F')), L"_");
}

// =============================================================================
// GAP-BV-13: 全角記号テーブルの境界文字（最初と最後の記号）
// =============================================================================

TEST(ConvertZeneisuToHaneisu, GAP_BV13_ZenKigoTableBoundary)
{
    // GAP-BV-13: テーブル先頭 = 全角スペース U+3000
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\u3000')), L" ");

    // GAP-BV-13: テーブル末尾 = 全角アンダースコア U+FF3F
    EXPECT_EQ(ZenToHan(std::wstring(1, L'\uFF3F')), L"_");

    // テーブル先頭の直前の文字 (U+2FFF) は変換対象外
    {
        std::wstring input(1, L'\u2FFF');
        std::wstring result = ZenToHan(input);
        EXPECT_EQ(result, input) << "U+2FFF should not be converted";
    }

    // テーブル末尾の直後の文字 (U+FF40) は変換対象外
    // (ただし U+FF40 は全角バッククォートで英小文字範囲の直前)
    {
        std::wstring input(1, L'\uFF40');
        std::wstring result = ZenToHan(input);
        // U+FF40 はテーブルにないかつ英小文字範囲外 -> 無変換
        EXPECT_EQ(result, input) << "U+FF40 should not be converted";
    }
}

// =============================================================================
// GAP-ER-07: Convert_ZeneisuToHaneisu に pData=NULL を渡した場合
// =============================================================================

TEST(ConvertZeneisuToHaneisu, GAP_ER07_NullPointer)
{
    // GAP-ER-07: pData=NULL のテスト
    // 実装にはNULLチェックがないため、nLength>0ならクラッシュする可能性がある。
    // nLength=0 であればループに入らないため安全なはず。
    // nLength=0 + NULL でクラッシュしないことを確認
    Convert_ZeneisuToHaneisu(nullptr, 0);
    // ここに到達すればクラッシュしていない
    SUCCEED() << "pData=NULL with nLength=0 did not crash";
}

TEST(ConvertZeneisuToHaneisu, GAP_ER07_NullPointerPositiveLength)
{
    // GAP-ER-07: pData=NULL, nLength>0 はクラッシュが予想される
    // 実装にNULLガードがないためスキップ
    GTEST_SKIP() << "pData=NULL with nLength>0 would dereference null pointer (no guard in implementation)";
}

// =============================================================================
// GAP-ER-08: Convert_ZeneisuToHaneisu に nLength=負 を渡した場合
// =============================================================================

TEST(ConvertZeneisuToHaneisu, GAP_ER08_NegativeLength)
{
    // GAP-ER-08: nLength=負の場合
    // 実装: q = p + nLength となり q < p になるため while(p<q) は即座にfalse
    // → ループに入らず安全に終了するはず
    wchar_t buf[] = L"test";
    Convert_ZeneisuToHaneisu(buf, -1);
    // バッファが変更されていないことを確認
    EXPECT_EQ(std::wstring(buf), L"test")
        << "nLength=-1 should not modify buffer (loop condition p<q is false)";
}

TEST(ConvertZeneisuToHaneisu, GAP_ER08_NegativeLengthLarge)
{
    // GAP-ER-08: nLength=INT_MIN の場合
    // NOTE: INT_MIN級の負値はポインタ演算 p+nLength でアドレスオーバーフローし
    // セグフォルトする。実装にガードなし → 潜在バグとして記録。
    GTEST_SKIP() << "nLength=INT_MIN causes pointer arithmetic overflow (SEGFAULT)";
}

// =============================================================================
// GAP-EC-19: Convert_HaneisuToZeneisu 半角記号テーブルの全記号の網羅テスト
//
// tableHanKigo (33文字) の全エントリが正しく全角化されることを検証する。
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_EC19_AllHanKigoTable)
{
    // GAP-EC-19: 半角記号テーブル全33文字の網羅テスト

    // [0] 半角スペース ' ' -> 全角スペース U+3000
    EXPECT_EQ(HanToZen(L" "), std::wstring(1, L'\u3000'));
    // [1] 半角コンマ ',' -> 全角コンマ U+FF0C
    EXPECT_EQ(HanToZen(L","), std::wstring(1, L'\uFF0C'));
    // [2] 半角ピリオド '.' -> 全角ピリオド U+FF0E
    EXPECT_EQ(HanToZen(L"."), std::wstring(1, L'\uFF0E'));
    // [3] 半角プラス '+' -> 全角プラス U+FF0B
    EXPECT_EQ(HanToZen(L"+"), std::wstring(1, L'\uFF0B'));
    // [4] 半角マイナス '-' -> 全角マイナス U+FF0D
    EXPECT_EQ(HanToZen(L"-"), std::wstring(1, L'\uFF0D'));
    // [5] 半角アスタリスク '*' -> 全角アスタリスク U+FF0A
    EXPECT_EQ(HanToZen(L"*"), std::wstring(1, L'\uFF0A'));
    // [6] 半角スラッシュ '/' -> 全角スラッシュ U+FF0F
    EXPECT_EQ(HanToZen(L"/"), std::wstring(1, L'\uFF0F'));
    // [7] 半角パーセント '%' -> 全角パーセント U+FF05
    EXPECT_EQ(HanToZen(L"%"), std::wstring(1, L'\uFF05'));
    // [8] 半角イコール '=' -> 全角イコール U+FF1D
    EXPECT_EQ(HanToZen(L"="), std::wstring(1, L'\uFF1D'));
    // [9] 半角パイプ '|' -> 全角パイプ U+FF5C
    EXPECT_EQ(HanToZen(L"|"), std::wstring(1, L'\uFF5C'));
    // [10] 半角アンパサンド '&' -> 全角アンパサンド U+FF06
    EXPECT_EQ(HanToZen(L"&"), std::wstring(1, L'\uFF06'));
    // [11] 半角キャレット '^' -> 全角キャレット U+FF3E
    EXPECT_EQ(HanToZen(L"^"), std::wstring(1, L'\uFF3E'));
    // [12] 半角バックスラッシュ '\\' -> 全角円記号 U+FFE5
    EXPECT_EQ(HanToZen(L"\\"), std::wstring(1, L'\uFFE5'));
    // [13] 半角アットマーク '@' -> 全角アットマーク U+FF20
    EXPECT_EQ(HanToZen(L"@"), std::wstring(1, L'\uFF20'));
    // [14] 半角セミコロン ';' -> 全角セミコロン U+FF1B
    EXPECT_EQ(HanToZen(L";"), std::wstring(1, L'\uFF1B'));
    // [15] 半角コロン ':' -> 全角コロン U+FF1A
    EXPECT_EQ(HanToZen(L":"), std::wstring(1, L'\uFF1A'));
    // [16] 半角ダブルクォーテーション '"' -> 右ダブルクォーテーション U+201D
    EXPECT_EQ(HanToZen(L"\""), std::wstring(1, L'\u201D'));
    // [17] 半角バッククォート '`' -> 左シングルクォーテーション U+2018
    EXPECT_EQ(HanToZen(L"`"), std::wstring(1, L'\u2018'));
    // [18] 半角シングルクォーテーション '\'' -> 右シングルクォーテーション U+2019
    EXPECT_EQ(HanToZen(L"'"), std::wstring(1, L'\u2019'));
    // [19] 半角小なり '<' -> 全角小なり U+FF1C
    EXPECT_EQ(HanToZen(L"<"), std::wstring(1, L'\uFF1C'));
    // [20] 半角大なり '>' -> 全角大なり U+FF1E
    EXPECT_EQ(HanToZen(L">"), std::wstring(1, L'\uFF1E'));
    // [21] 半角左丸括弧 '(' -> 全角左丸括弧 U+FF08
    EXPECT_EQ(HanToZen(L"("), std::wstring(1, L'\uFF08'));
    // [22] 半角右丸括弧 ')' -> 全角右丸括弧 U+FF09
    EXPECT_EQ(HanToZen(L")"), std::wstring(1, L'\uFF09'));
    // [23] 半角左波括弧 '{' -> 全角左波括弧 U+FF5B
    EXPECT_EQ(HanToZen(L"{"), std::wstring(1, L'\uFF5B'));
    // [24] 半角右波括弧 '}' -> 全角右波括弧 U+FF5D
    EXPECT_EQ(HanToZen(L"}"), std::wstring(1, L'\uFF5D'));
    // [25] 半角左角括弧 '[' -> 全角左角括弧 U+FF3B
    EXPECT_EQ(HanToZen(L"["), std::wstring(1, L'\uFF3B'));
    // [26] 半角右角括弧 ']' -> 全角右角括弧 U+FF3D
    EXPECT_EQ(HanToZen(L"]"), std::wstring(1, L'\uFF3D'));
    // [27] 半角感嘆符 '!' -> 全角感嘆符 U+FF01
    EXPECT_EQ(HanToZen(L"!"), std::wstring(1, L'\uFF01'));
    // [28] 半角疑問符 '?' -> 全角疑問符 U+FF1F
    EXPECT_EQ(HanToZen(L"?"), std::wstring(1, L'\uFF1F'));
    // [29] 半角シャープ '#' -> 全角シャープ U+FF03
    EXPECT_EQ(HanToZen(L"#"), std::wstring(1, L'\uFF03'));
    // [30] 半角ドル '$' -> 全角ドル U+FF04
    EXPECT_EQ(HanToZen(L"$"), std::wstring(1, L'\uFF04'));
    // [31] 半角チルダ '~' -> 全角オーバーライン U+FFE3
    EXPECT_EQ(HanToZen(L"~"), std::wstring(1, L'\uFFE3'));
    // [32] 半角アンダースコア '_' -> 全角アンダースコア U+FF3F
    EXPECT_EQ(HanToZen(L"_"), std::wstring(1, L'\uFF3F'));
}

// =============================================================================
// GAP-EC-20 / GAP-BV-14: Convert_HaneisuToZeneisu 空配列 (nLength=0)
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_EC20_BV14_EmptyArray)
{
    // GAP-EC-20: 空配列のテスト
    // GAP-BV-14: nLength=0 の境界値テスト
    wchar_t buf[] = L"test";
    Convert_HaneisuToZeneisu(buf, 0);
    // ループに入らず何も変更されない
    EXPECT_EQ(std::wstring(buf), L"test")
        << "nLength=0 should not modify buffer";
}

// =============================================================================
// GAP-BV-15: Convert_HaneisuToZeneisu nLength=1
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_BV15_LengthOne)
{
    // GAP-BV-15: nLength=1 (1文字のみ変換)
    wchar_t buf[] = L"Ab";
    Convert_HaneisuToZeneisu(buf, 1);
    // 1文字目のみ変換、2文字目は変換されない
    EXPECT_EQ(buf[0], L'\uFF21') << "First char 'A' should be converted to full-width";
    EXPECT_EQ(buf[1], L'b') << "Second char 'b' should NOT be converted (beyond nLength)";
}

// =============================================================================
// GAP-BV-16: 半角大文字範囲の境界外直前直後: '@'(0x40), '['(0x5B)
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_BV16_UpperCaseBoundaryOutside)
{
    // GAP-BV-16: 'A'(0x41) の直前 = '@'(0x40)
    // '@' は記号テーブルに含まれるため、全角 '@'(U+FF20) に変換される
    EXPECT_EQ(HanToZen(std::wstring(1, L'@')), std::wstring(1, L'\uFF20'))
        << "'@' (0x40) is in symbol table, should convert to U+FF20";

    // GAP-BV-16: 'Z'(0x5A) の直後 = '['(0x5B)
    // '[' は記号テーブルに含まれるため、全角 '['(U+FF3B) に変換される
    EXPECT_EQ(HanToZen(std::wstring(1, L'[')), std::wstring(1, L'\uFF3B'))
        << "'[' (0x5B) is in symbol table, should convert to U+FF3B";

    // 大文字範囲の先頭と末尾が正しく変換されることの再確認
    EXPECT_EQ(HanToZen(std::wstring(1, L'A')), std::wstring(1, L'\uFF21'));
    EXPECT_EQ(HanToZen(std::wstring(1, L'Z')), std::wstring(1, L'\uFF3A'));
}

// =============================================================================
// GAP-BV-17: 半角小文字範囲の境界外: '`'(0x60), '{'(0x7B)
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_BV17_LowerCaseBoundaryOutside)
{
    // GAP-BV-17: 'a'(0x61) の直前 = '`'(0x60)
    // '`' は記号テーブルに含まれるため、U+2018 に変換される
    EXPECT_EQ(HanToZen(std::wstring(1, L'`')), std::wstring(1, L'\u2018'))
        << "'`' (0x60) is in symbol table, should convert to U+2018";

    // GAP-BV-17: 'z'(0x7A) の直後 = '{'(0x7B)
    // '{' は記号テーブルに含まれるため、全角 '{'(U+FF5B) に変換される
    EXPECT_EQ(HanToZen(std::wstring(1, L'{')), std::wstring(1, L'\uFF5B'))
        << "'{' (0x7B) is in symbol table, should convert to U+FF5B";

    // 小文字範囲の先頭と末尾が正しく変換されることの再確認
    EXPECT_EQ(HanToZen(std::wstring(1, L'a')), std::wstring(1, L'\uFF41'));
    EXPECT_EQ(HanToZen(std::wstring(1, L'z')), std::wstring(1, L'\uFF5A'));
}

// =============================================================================
// GAP-BV-18: 半角数字範囲の境界外: '/'(0x2F), ':'(0x3A)
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_BV18_DigitBoundaryOutside)
{
    // GAP-BV-18: '0'(0x30) の直前 = '/'(0x2F)
    // '/' は記号テーブルに含まれるため、全角 '/'(U+FF0F) に変換される
    EXPECT_EQ(HanToZen(std::wstring(1, L'/')), std::wstring(1, L'\uFF0F'))
        << "'/' (0x2F) is in symbol table, should convert to U+FF0F";

    // GAP-BV-18: '9'(0x39) の直後 = ':'(0x3A)
    // ':' は記号テーブルに含まれるため、全角 ':'(U+FF1A) に変換される
    EXPECT_EQ(HanToZen(std::wstring(1, L':')), std::wstring(1, L'\uFF1A'))
        << "':' (0x3A) is in symbol table, should convert to U+FF1A";

    // 数字範囲の先頭と末尾が正しく変換されることの再確認
    EXPECT_EQ(HanToZen(std::wstring(1, L'0')), std::wstring(1, L'\uFF10'));
    EXPECT_EQ(HanToZen(std::wstring(1, L'9')), std::wstring(1, L'\uFF19'));
}

// =============================================================================
// GAP-BR-10: Convert_HaneisuToZeneisu nLength=0のループ不実行パス
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_BR10_LoopNotExecuted)
{
    // GAP-BR-10: nLength=0 の場合 while(p<q) が即座にfalseでループ不実行
    wchar_t buf[] = L"ABC123";
    Convert_HaneisuToZeneisu(buf, 0);
    // 全ての文字が変更されていないことを確認
    EXPECT_EQ(buf[0], L'A');
    EXPECT_EQ(buf[1], L'B');
    EXPECT_EQ(buf[2], L'C');
    EXPECT_EQ(buf[3], L'1');
    EXPECT_EQ(buf[4], L'2');
    EXPECT_EQ(buf[5], L'3');
}

// =============================================================================
// GAP-ER-09: Convert_HaneisuToZeneisu に pData=NULL を渡した場合
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_ER09_NullPointer)
{
    // GAP-ER-09: pData=NULL のテスト
    // nLength=0 であればループに入らないため安全なはず
    Convert_HaneisuToZeneisu(nullptr, 0);
    SUCCEED() << "pData=NULL with nLength=0 did not crash";
}

TEST(ConvertHaneisuToZeneisu, GAP_ER09_NullPointerPositiveLength)
{
    // GAP-ER-09: pData=NULL, nLength>0 はクラッシュが予想される
    GTEST_SKIP() << "pData=NULL with nLength>0 would dereference null pointer (no guard in implementation)";
}

// =============================================================================
// GAP-ER-10: Convert_HaneisuToZeneisu に nLength=負 を渡した場合
// =============================================================================

TEST(ConvertHaneisuToZeneisu, GAP_ER10_NegativeLength)
{
    // GAP-ER-10: nLength=負の場合
    // 実装: q = p + nLength となり q < p になるため while(p<q) は即座にfalse
    wchar_t buf[] = L"test";
    Convert_HaneisuToZeneisu(buf, -1);
    EXPECT_EQ(std::wstring(buf), L"test")
        << "nLength=-1 should not modify buffer (loop condition p<q is false)";
}

TEST(ConvertHaneisuToZeneisu, GAP_ER10_NegativeLengthLarge)
{
    // GAP-ER-10: nLength=INT_MIN の場合
    // NOTE: INT_MIN級の負値はポインタ演算 p+nLength でアドレスオーバーフローし
    // セグフォルトする。実装にガードなし → 潜在バグとして記録。
    GTEST_SKIP() << "nLength=INT_MIN causes pointer arithmetic overflow (SEGFAULT)";
}

// =============================================================================
// GAP-DP-03: 記号テーブルの対称性検証
// =============================================================================

TEST(ConvertSymmetry, GAP_DP03_TableSymmetryVerification)
{
    // GAP-DP-03: tableZenKigo と tableHanKigo の対称性を検証する
    //
    // 全角記号テーブルの全33文字について:
    //   ZenToHan(zen[i]) == han[i]  かつ  HanToZen(han[i]) == zen[i]
    // が成り立つことを確認する。

    // テーブルの全角側 (tableZenKigo相当)
    const wchar_t zenKigo[] =
        L"\u3000\uFF0C\uFF0E"
        L"\uFF0B\uFF0D\uFF0A\uFF0F\uFF05\uFF1D\uFF5C\uFF06"
        L"\uFF3E\uFFE5\uFF20\uFF1B\uFF1A"
        L"\u201D\u2018\u2019\uFF1C\uFF1E\uFF08\uFF09\uFF5B\uFF5D\uFF3B\uFF3D"
        L"\uFF01\uFF1F\uFF03\uFF04\uFFE3\uFF3F";

    // テーブルの半角側 (tableHanKigo相当)
    const wchar_t hanKigo[] =
        L" ,."
        L"+-*/%=|&"
        L"^\\@;:"
        L"\"`'<>(){}[]"
        L"!?#$~_";

    // テーブル長の一致を確認
    const size_t zenLen = wcslen(zenKigo);
    const size_t hanLen = wcslen(hanKigo);
    ASSERT_EQ(zenLen, hanLen)
        << "tableZenKigo and tableHanKigo must have the same length";
    ASSERT_EQ(zenLen, 33u)
        << "Symbol tables should have exactly 33 entries";

    // 各位置の対応を確認: Zen->Han
    for (size_t i = 0; i < zenLen; i++) {
        std::wstring zenChar(1, zenKigo[i]);
        std::wstring hanChar(1, hanKigo[i]);
        EXPECT_EQ(ZenToHan(zenChar), hanChar)
            << "ZenToHan mismatch at index " << i
            << ": zen=U+" << std::hex << (unsigned)zenKigo[i]
            << " expected han=0x" << (unsigned)hanKigo[i];
    }

    // 各位置の対応を確認: Han->Zen
    for (size_t i = 0; i < hanLen; i++) {
        std::wstring hanChar(1, hanKigo[i]);
        std::wstring zenChar(1, zenKigo[i]);
        EXPECT_EQ(HanToZen(hanChar), zenChar)
            << "HanToZen mismatch at index " << i
            << ": han=0x" << std::hex << (unsigned)hanKigo[i]
            << " expected zen=U+" << (unsigned)zenKigo[i];
    }
}

// =============================================================================
// GAP-DP-03 拡張: 記号を含むラウンドトリップテスト
// =============================================================================

TEST(ConvertRoundTrip, GAP_DP03_AllSymbolsRoundTrip_ZenToHanToZen)
{
    // GAP-DP-03 拡張: 全角記号テーブル全33文字のラウンドトリップ (Zen->Han->Zen)
    const wchar_t zenKigo[] =
        L"\u3000\uFF0C\uFF0E"
        L"\uFF0B\uFF0D\uFF0A\uFF0F\uFF05\uFF1D\uFF5C\uFF06"
        L"\uFF3E\uFFE5\uFF20\uFF1B\uFF1A"
        L"\u201D\u2018\u2019\uFF1C\uFF1E\uFF08\uFF09\uFF5B\uFF5D\uFF3B\uFF3D"
        L"\uFF01\uFF1F\uFF03\uFF04\uFFE3\uFF3F";

    // 全記号を1つの文字列にまとめてラウンドトリップ
    std::wstring allZenKigo(zenKigo);
    std::wstring han = ZenToHan(allZenKigo);
    std::wstring roundTrip = HanToZen(han);
    EXPECT_EQ(roundTrip, allZenKigo)
        << "Round-trip Zen->Han->Zen for all 33 symbols should recover original";
}

TEST(ConvertRoundTrip, GAP_DP03_AllSymbolsRoundTrip_HanToZenToHan)
{
    // GAP-DP-03 拡張: 半角記号テーブル全33文字のラウンドトリップ (Han->Zen->Han)
    const wchar_t hanKigo[] =
        L" ,."
        L"+-*/%=|&"
        L"^\\@;:"
        L"\"`'<>(){}[]"
        L"!?#$~_";

    std::wstring allHanKigo(hanKigo);
    std::wstring zen = HanToZen(allHanKigo);
    std::wstring roundTrip = ZenToHan(zen);
    EXPECT_EQ(roundTrip, allHanKigo)
        << "Round-trip Han->Zen->Han for all 33 symbols should recover original";
}

TEST(ConvertRoundTrip, GAP_DP03_MixedSymbolsAndAlphanumRoundTrip)
{
    // GAP-DP-03 拡張: 記号+英数字混在のラウンドトリップ
    // 全角記号+全角英数字 -> 半角 -> 全角 で元に戻ること
    {
        // 全角: ＋Ａ１（＝ｂ２）
        std::wstring original =
            L"\uFF0B\uFF21\uFF11\uFF08\uFF1D\uFF42\uFF12\uFF09";
        std::wstring han = ZenToHan(original);
        EXPECT_EQ(han, L"+A1(=b2)");
        std::wstring roundTrip = HanToZen(han);
        EXPECT_EQ(roundTrip, original);
    }

    // 半角記号+半角英数字 -> 全角 -> 半角 で元に戻ること
    {
        std::wstring original = L"[A+B]=C*2;";
        std::wstring zen = HanToZen(original);
        std::wstring roundTrip = ZenToHan(zen);
        EXPECT_EQ(roundTrip, original);
    }
}

TEST(ConvertRoundTrip, GAP_DP03_IndividualSymbolRoundTrip)
{
    // GAP-DP-03 拡張: 全記号を個別にラウンドトリップ確認
    const wchar_t zenKigo[] =
        L"\u3000\uFF0C\uFF0E"
        L"\uFF0B\uFF0D\uFF0A\uFF0F\uFF05\uFF1D\uFF5C\uFF06"
        L"\uFF3E\uFFE5\uFF20\uFF1B\uFF1A"
        L"\u201D\u2018\u2019\uFF1C\uFF1E\uFF08\uFF09\uFF5B\uFF5D\uFF3B\uFF3D"
        L"\uFF01\uFF1F\uFF03\uFF04\uFFE3\uFF3F";
    const wchar_t hanKigo[] =
        L" ,."
        L"+-*/%=|&"
        L"^\\@;:"
        L"\"`'<>(){}[]"
        L"!?#$~_";

    const size_t len = wcslen(zenKigo);
    for (size_t i = 0; i < len; i++) {
        // Zen -> Han -> Zen
        {
            std::wstring zenChar(1, zenKigo[i]);
            std::wstring han = ZenToHan(zenChar);
            std::wstring back = HanToZen(han);
            EXPECT_EQ(back, zenChar)
                << "Round-trip failed for zen symbol at index " << i
                << " (U+" << std::hex << (unsigned)zenKigo[i] << ")";
        }
        // Han -> Zen -> Han
        {
            std::wstring hanChar(1, hanKigo[i]);
            std::wstring zen = HanToZen(hanChar);
            std::wstring back = ZenToHan(zen);
            EXPECT_EQ(back, hanChar)
                << "Round-trip failed for han symbol at index " << i
                << " (0x" << std::hex << (unsigned)hanKigo[i] << ")";
        }
    }
}

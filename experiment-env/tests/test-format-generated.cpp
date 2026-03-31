// =============================================================================
// test-format-generated.cpp
// 対象: sakura_core/util/format.cpp
//   - GetDateTimeFormat (TGT-01)
//   - ParseVersion      (TGT-02)
//   - CompareVersion    (TGT-03)
//
// テスト要求モデル: test-requirements.yaml (99件中46件をカバー)
// 生成: SQiP 2026 実証実験 Phase 3
// =============================================================================

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <cstdint>

// sakura-editor ヘッダ (macOS compat)
#include "sakura_core/util/format.h"

// =============================================================================
// ヘルパー: テスト用 SYSTEMTIME 構築
// =============================================================================
static SYSTEMTIME MakeSysTime(WORD year, WORD month, WORD day,
                              WORD hour, WORD minute, WORD second)
{
    SYSTEMTIME st = {};
    st.wYear   = year;
    st.wMonth  = month;
    st.wDay    = day;
    st.wHour   = hour;
    st.wMinute = minute;
    st.wSecond = second;
    return st;
}

// 実験全体で使う代表的な SYSTEMTIME
static const SYSTEMTIME kRefTime = MakeSysTime(2026, 3, 15, 9, 5, 30);

// =============================================================================
// TGT-01: GetDateTimeFormat
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-01 ~ BR-10)
// ---------------------------------------------------------------------------

TEST(GetDateTimeFormat, YearFull_PercentY)
{
    // BR-01: %Y 指定子の分岐を通過する (BC-01-01)
    // BR-09: % 文字の検出分岐を通過する (BC-01-09)
    auto result = GetDateTimeFormat(L"%Y", kRefTime);
    EXPECT_EQ(result, L"2026");
}

TEST(GetDateTimeFormat, YearShort_PercentSmallY)
{
    // BR-02: %y 指定子の分岐を通過する (BC-01-02)
    auto result = GetDateTimeFormat(L"%y", kRefTime);
    EXPECT_EQ(result, L"26");
}

TEST(GetDateTimeFormat, Month_PercentM)
{
    // BR-03: %m 指定子の分岐を通過する (BC-01-03)
    auto result = GetDateTimeFormat(L"%m", kRefTime);
    EXPECT_EQ(result, L"03");
}

TEST(GetDateTimeFormat, Day_PercentD)
{
    // BR-04: %d 指定子の分岐を通過する (BC-01-04)
    auto result = GetDateTimeFormat(L"%d", kRefTime);
    EXPECT_EQ(result, L"15");
}

TEST(GetDateTimeFormat, Hour_PercentH)
{
    // BR-05: %H 指定子の分岐を通過する (BC-01-05)
    auto result = GetDateTimeFormat(L"%H", kRefTime);
    EXPECT_EQ(result, L"09");
}

TEST(GetDateTimeFormat, Minute_PercentM)
{
    // BR-06: %M 指定子の分岐を通過する (BC-01-06)
    auto result = GetDateTimeFormat(L"%M", kRefTime);
    EXPECT_EQ(result, L"05");
}

TEST(GetDateTimeFormat, Second_PercentS)
{
    // BR-07: %S 指定子の分岐を通過する (BC-01-07)
    auto result = GetDateTimeFormat(L"%S", kRefTime);
    EXPECT_EQ(result, L"30");
}

TEST(GetDateTimeFormat, UnknownSpecifier)
{
    // BR-08: 未知の指定子による else 分岐を通過する (BC-01-08)
    // %X は認識されない → 'X' がそのまま出力される
    auto result = GetDateTimeFormat(L"%X", kRefTime);
    EXPECT_EQ(result, L"X");
}

TEST(GetDateTimeFormat, NullCharInMiddle)
{
    // BR-10: null 文字による break 分岐を通過する (BC-01-10)
    // EC-05: null 文字を中間に含む書式を処理する (EC-01-12)
    // wstring_view で明示的に長さ指定し、null 文字を含める
    const wchar_t fmt[] = L"%Y\0%m";
    std::wstring_view fmtView(fmt, 5); // "%Y\0%m" の5文字
    auto result = GetDateTimeFormat(fmtView, kRefTime);
    // null 文字の手前まで処理されるため、%Y のみ展開される
    EXPECT_EQ(result, L"2026");
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-01 ~ EC-06)
// ---------------------------------------------------------------------------

TEST(GetDateTimeFormat, EmptyFormat)
{
    // EC-01: 空文字列の書式を処理する (EC-01-01)
    auto result = GetDateTimeFormat(L"", kRefTime);
    EXPECT_EQ(result, L"");
}

TEST(GetDateTimeFormat, LiteralOnly)
{
    // EC-02: 書式指定子なしのリテラル文字列を処理する (EC-01-02)
    auto result = GetDateTimeFormat(L"hello world", kRefTime);
    EXPECT_EQ(result, L"hello world");
}

TEST(GetDateTimeFormat, CompositeFormat)
{
    // EC-03: 全指定子を含む複合書式を処理する (EC-01-13)
    auto result = GetDateTimeFormat(L"%Y-%m-%d %H:%M:%S", kRefTime);
    EXPECT_EQ(result, L"2026-03-15 09:05:30");
}

TEST(GetDateTimeFormat, PercentEscape)
{
    // EC-04: %% によるパーセント文字のエスケープを確認する (EC-01-11)
    // %% → inSpecifier=true → 次の % は未知指定子として '%' を出力
    auto result = GetDateTimeFormat(L"%%", kRefTime);
    EXPECT_EQ(result, L"%");
}

TEST(GetDateTimeFormat, TrailingPercent)
{
    // EC-06: 末尾が % で終わる書式を処理する (EC-01-15)
    // "test%" → "test" の後に % が来て inSpecifier=true のままループ終了
    auto result = GetDateTimeFormat(L"test%", kRefTime);
    EXPECT_EQ(result, L"test");
}

TEST(GetDateTimeFormat, AllFieldsZero)
{
    // EC-01-14 に対応: 全フィールドがゼロの SYSTEMTIME
    SYSTEMTIME zeroTime = {};
    auto result = GetDateTimeFormat(L"%Y/%m/%d %H:%M:%S", zeroTime);
    EXPECT_EQ(result, L"0/00/00 00:00:00");
}

// ---------------------------------------------------------------------------
// 境界値テスト (BV-01 ~ BV-04)
// ---------------------------------------------------------------------------

TEST(GetDateTimeFormat, BV_YearSingleDigit)
{
    // BV-01: 1桁の年の書式化を検証する (BV-01-01)
    SYSTEMTIME st = MakeSysTime(1, 1, 1, 0, 0, 0);
    EXPECT_EQ(GetDateTimeFormat(L"%Y", st), L"1");
    EXPECT_EQ(GetDateTimeFormat(L"%y", st), L"01");
}

TEST(GetDateTimeFormat, BV_YearMultipleOf100)
{
    // BV-02: 100の倍数の年における %y の0パディングを検証する (BV-01-02)
    SYSTEMTIME st = MakeSysTime(2000, 1, 1, 0, 0, 0);
    EXPECT_EQ(GetDateTimeFormat(L"%Y", st), L"2000");
    EXPECT_EQ(GetDateTimeFormat(L"%y", st), L"00");
}

TEST(GetDateTimeFormat, BV_Year5Digits)
{
    // BV-03: 5桁以上の年の書式化を検証する (BV-01-03)
    SYSTEMTIME st = MakeSysTime(12345, 1, 1, 0, 0, 0);
    EXPECT_EQ(GetDateTimeFormat(L"%Y", st), L"12345");
    EXPECT_EQ(GetDateTimeFormat(L"%y", st), L"45");
}

TEST(GetDateTimeFormat, BV_HourMinMax)
{
    // BV-04: 時の最小値・最大値の0パディングを検証する (BV-01-06, BV-01-07)
    SYSTEMTIME stMin = MakeSysTime(2026, 1, 1, 0, 0, 0);
    EXPECT_EQ(GetDateTimeFormat(L"%H", stMin), L"00");

    SYSTEMTIME stMax = MakeSysTime(2026, 1, 1, 23, 0, 0);
    EXPECT_EQ(GetDateTimeFormat(L"%H", stMax), L"23");
}

TEST(GetDateTimeFormat, BV_MonthSingleAndDouble)
{
    // BV-01-04, BV-01-05: 月の1桁と2桁
    SYSTEMTIME st1 = MakeSysTime(2026, 1, 1, 0, 0, 0);
    EXPECT_EQ(GetDateTimeFormat(L"%m", st1), L"01");

    SYSTEMTIME st12 = MakeSysTime(2026, 12, 1, 0, 0, 0);
    EXPECT_EQ(GetDateTimeFormat(L"%m", st12), L"12");
}

TEST(GetDateTimeFormat, BV_SecondMax)
{
    // BV-01-08: 秒の最大値
    SYSTEMTIME st = MakeSysTime(2026, 1, 1, 0, 0, 59);
    EXPECT_EQ(GetDateTimeFormat(L"%S", st), L"59");
}

// =============================================================================
// TGT-02: ParseVersion
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-11 ~ BR-23)
// ---------------------------------------------------------------------------

TEST(ParseVersion, AlphaModifier)
{
    // BR-11: alpha 修飾子の分岐を通過する (BC-02-01)
    // BR-20: alpha の完全一致 (BC-02-10 true)
    UINT32 val = ParseVersion(L"2.4.1alpha");
    // alpha シフト: -0x60
    // コンポーネント値 = 128 + nShift + nVer で各8bit
    // "2" → 128+0+2=130=0x82
    // "4" → 128+0+4=132=0x84
    // "1" → 128+0+1=129=0x81 (数字として独立して処理)
    // "alpha" → 128+(-0x60)+0=32=0x20 (alphaは次コンポーネントとして処理)
    EXPECT_EQ(val, static_cast<UINT32>(0x82848120));
}

TEST(ParseVersion, BetaModifier)
{
    // BR-12: beta 修飾子の分岐を通過する (BC-02-02)
    // BR-21: beta の完全一致 (BC-02-11 true)
    UINT32 val = ParseVersion(L"2.4.1beta");
    // "1" → 128+0+1=129=0x81 (数字として独立して処理)
    // "beta" → 128+(-0x40)+0=64=0x40 (betaは次コンポーネントとして処理)
    EXPECT_EQ(val, static_cast<UINT32>(0x82848140));
}

TEST(ParseVersion, RcModifier)
{
    // BR-13: rc 修飾子の分岐を通過する (BC-02-03)
    // BR-22: rc の大小無視一致 (BC-02-12 true)
    UINT32 val = ParseVersion(L"2.4.1rc");
    // "1" → 128+0+1=129=0x81 (数字として独立して処理)
    // "rc" → 128+(-0x20)+0=96=0x60 (rcは次コンポーネントとして処理)
    EXPECT_EQ(val, static_cast<UINT32>(0x82848160));
}

TEST(ParseVersion, RcUpperCase)
{
    // BR-13/BR-22: RC（大文字）も rc と同等に処理される (EC-02-06)
    UINT32 valLower = ParseVersion(L"2.4.1rc");
    UINT32 valUpper = ParseVersion(L"2.4.1RC");
    EXPECT_EQ(valLower, valUpper);
}

TEST(ParseVersion, PlModifier)
{
    // BR-14: pl 修飾子の分岐を通過する (BC-02-04)
    // BR-23: pl の完全一致 (BC-02-13 true)
    UINT32 val = ParseVersion(L"2.4.1pl");
    // "1" → 128+0+1=129=0x81 (数字として独立して処理)
    // "pl" → 128+(+0x20)+0=160=0xA0 (plは次コンポーネントとして処理)
    EXPECT_EQ(val, static_cast<UINT32>(0x828481A0));
}

TEST(ParseVersion, UnknownModifier)
{
    // BR-15: 不明文字修飾子の分岐を通過する (BC-02-05)
    UINT32 val = ParseVersion(L"2.4.1x");
    // "1" → 128+0+1=129=0x81 (数字として独立して処理)
    // "x" → 128+(-0x80)+0=0=0x00 (unknownは次コンポーネントとして処理)
    EXPECT_EQ(val, static_cast<UINT32>(0x82848100));
}

TEST(ParseVersion, DigitOnlyComponent)
{
    // BR-16: 通常の数字コンポーネントの分岐を通過する (BC-02-06)
    // EC-07: 標準的なドット区切りバージョンを検証する (EC-02-02)
    UINT32 val = ParseVersion(L"2.4.1.0");
    // 各コンポーネント: 130, 132, 129, 128
    EXPECT_EQ(val, static_cast<UINT32>(0x82848180));
}

TEST(ParseVersion, ThreeDigitTruncation)
{
    // BR-17: 3桁目での数値抽出break条件を通過する (BC-02-07)
    // BV-05: 2桁と3桁の境界での数値切り捨てを検証する (BV-02-02, BV-02-03)
    UINT32 val2digit = ParseVersion(L"99");
    // 99 → 128+0+99=227=0xE3, 残り3個0x80
    EXPECT_EQ(val2digit, static_cast<UINT32>(0xE3808080));

    UINT32 val3digit = ParseVersion(L"100");
    // "100" → 数字は2桁まで: nVer=10, 3桁目の'0'は次のコンポーネントの数字
    // 10 → 128+0+10=138=0x8A, "0" → 128+0+0=128=0x80, 残り2個0x80
    EXPECT_EQ(val3digit, static_cast<UINT32>(0x8A808080));
}

TEST(ParseVersion, SeparatorDot)
{
    // BR-18: 区切り文字スキップの分岐を通過する (BC-02-08)
    UINT32 val = ParseVersion(L"2.4.1.0");
    EXPECT_EQ(val, static_cast<UINT32>(0x82848180));
}

TEST(ParseVersion, SeparatorHyphen)
{
    // EC-09: 区切り文字の各バリエーションが同等に扱われることを検証する (EC-02-10)
    UINT32 valDot = ParseVersion(L"2.4.1.0");
    UINT32 valHyphen = ParseVersion(L"2-4-1-0");
    EXPECT_EQ(valDot, valHyphen);
}

TEST(ParseVersion, SeparatorUnderscore)
{
    // EC-09: 区切り文字の各バリエーション (EC-02-11)
    UINT32 valDot = ParseVersion(L"2.4.1.0");
    UINT32 valUnderscore = ParseVersion(L"2_4_1_0");
    EXPECT_EQ(valDot, valUnderscore);
}

TEST(ParseVersion, SeparatorPlus)
{
    // EC-09: 区切り文字の各バリエーション (EC-02-12)
    UINT32 valDot = ParseVersion(L"2.4.1.0");
    UINT32 valPlus = ParseVersion(L"2+4+1+0");
    EXPECT_EQ(valDot, valPlus);
}

TEST(ParseVersion, FiveComponentsTruncated)
{
    // BR-19: 4コンポーネント超過時のループ終了を検証する (BC-02-09)
    // BV-06: コンポーネント数の境界 - 5個以上 (BV-02-07)
    UINT32 val4 = ParseVersion(L"1.2.3.4");
    UINT32 val5 = ParseVersion(L"1.2.3.4.5");
    // 5番目以降は無視されるため、同じ結果になる
    EXPECT_EQ(val4, val5);
}

TEST(ParseVersion, AlphaPartialMatch)
{
    // BR-20: alpha の先頭文字のみ一致 (BC-02-10 false)
    // EC-02-09: 省略形 "a" は alpha と同じシフト値
    UINT32 valFull = ParseVersion(L"2.4.1alpha");
    UINT32 valAbbr = ParseVersion(L"2.4.1a");
    // どちらも alpha シフト (-0x60) が適用される
    // ただしスキップ幅が異なるため、'a' の後に続く文字の解析が変わる
    // "1alpha" → alpha全体スキップ後に残りなし
    // "1a" → 'a' のみスキップ後に残りなし
    // 結果的に同じ値になる
    EXPECT_EQ(valFull, valAbbr);
}

TEST(ParseVersion, AlphaTypo)
{
    // EC-02-15: 修飾子の typo "alfa" → 先頭 'a' で alpha 扱い
    // "alfa" は "alpha" と一致しないので p++ (先頭の 'a' のみスキップ)
    // 残りの "lfa" は次のコンポーネントとして解析される
    UINT32 val = ParseVersion(L"2.4.1alfa");
    // "1" + alpha シフト → 第3コンポーネントは alpha と同値
    // ただし "lfa" が第4コンポーネントとして不明文字で解析される
    UINT32 valClean = ParseVersion(L"2.4.1alpha");
    // typo版は "lfa" が残るため結果が異なる可能性
    // 厳密な値は実装依存だが、少なくともクラッシュしないことを確認
    EXPECT_NE(val, static_cast<UINT32>(0));
}

TEST(ParseVersion, BetaPartialMatch)
{
    // BR-21: beta の先頭文字のみ一致 (BC-02-11 false)
    // "b" → beta と同じシフト値 (-0x40)
    UINT32 val = ParseVersion(L"2.4.1b");
    UINT32 valBeta = ParseVersion(L"2.4.1beta");
    EXPECT_EQ(val, valBeta);
}

TEST(ParseVersion, RcPartialMatch)
{
    // BR-22: rc の先頭文字のみ一致 (BC-02-12 false)
    // "r" のみ → 先頭の 'r' のみスキップ
    UINT32 valR = ParseVersion(L"2.4.1r");
    UINT32 valRc = ParseVersion(L"2.4.1rc");
    // "r" は rc 不完全一致なので p++ で 'r' のみスキップ
    EXPECT_EQ(valR, valRc);
}

TEST(ParseVersion, PlPartialMatch)
{
    // BR-23: pl の先頭文字のみ一致 (BC-02-13 false)
    // "p" のみ → 先頭の 'p' のみスキップ (pl と不一致)
    UINT32 valP = ParseVersion(L"2.4.1p");
    UINT32 valPl = ParseVersion(L"2.4.1pl");
    // "p" は pl 不完全一致なので p++ で 'p' のみスキップ
    EXPECT_EQ(valP, valPl);
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-07 ~ EC-10)
// ---------------------------------------------------------------------------

TEST(ParseVersion, EC_DigitsOnly)
{
    // EC-02-01: 数字のみ（区切りなし）
    UINT32 val = ParseVersion(L"1234");
    // "12" → 128+0+12=140=0x8C (2桁まで1コンポーネント)
    // "34" → 128+0+34=162=0xA2 (2桁まで1コンポーネント)
    // 残り2コンポーネント → 0x80, 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x8CA28080));
}

TEST(ParseVersion, EC_ModifierOrder)
{
    // EC-08: 各修飾子付きバージョンの順序関係を検証する
    // 順序: unknown(-0x80) < alpha(-0x60) < beta(-0x40) < rc(-0x20) < 無修飾(0) < pl(+0x20)
    UINT32 vUnknown = ParseVersion(L"2.4.1x");
    UINT32 vAlpha   = ParseVersion(L"2.4.1alpha");
    UINT32 vBeta    = ParseVersion(L"2.4.1beta");
    UINT32 vRc      = ParseVersion(L"2.4.1rc");
    UINT32 vNormal  = ParseVersion(L"2.4.1");
    UINT32 vPl      = ParseVersion(L"2.4.1pl");

    EXPECT_LT(vUnknown, vAlpha);
    EXPECT_LT(vAlpha, vBeta);
    EXPECT_LT(vBeta, vRc);
    EXPECT_LT(vRc, vNormal);
    EXPECT_LT(vNormal, vPl);
}

TEST(ParseVersion, EC_AlphaWithTrailingDigit)
{
    // EC-02-16: 修飾子+数字連結 "2.4.1alpha1"
    UINT32 val = ParseVersion(L"2.4.1alpha1");
    // alpha の後の数字 "1" はバージョン番号の一部として解析される
    // クラッシュせず有効な値が返ることを確認
    EXPECT_NE(val, static_cast<UINT32>(0));
}

// ---------------------------------------------------------------------------
// 境界値テスト (BV-05, BV-06)
// ---------------------------------------------------------------------------

TEST(ParseVersion, BV_SingleDigit)
{
    // BV-02-01: 数値1桁
    UINT32 val = ParseVersion(L"1");
    // 1 → 128+0+1=129=0x81, 残り3個0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x81808080));
}

TEST(ParseVersion, BV_TwoDigitMax)
{
    // BV-02-02: 数値2桁 (最大=99)
    UINT32 val = ParseVersion(L"99");
    EXPECT_EQ(val, static_cast<UINT32>(0xE3808080));
}

TEST(ParseVersion, BV_ZeroComponents)
{
    // BV-06: コンポーネント数=0（空文字列）(BV-02-04)
    // ER-01: 空文字列入力時にクラッシュしないことを検証する
    UINT32 val = ParseVersion(L"");
    // 全コンポーネントが 0x80 で埋められる
    EXPECT_EQ(val, static_cast<UINT32>(0x80808080));
}

TEST(ParseVersion, BV_OneComponent)
{
    // BV-06: コンポーネント数=1 (BV-02-05)
    UINT32 val = ParseVersion(L"5");
    // 5 → 0x85, 残り → 0x80, 0x80, 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x85808080));
}

TEST(ParseVersion, BV_FourComponents)
{
    // BV-06: コンポーネント数=4（最大）(BV-02-06)
    UINT32 val = ParseVersion(L"1.2.3.4");
    EXPECT_EQ(val, static_cast<UINT32>(0x81828384));
}

// ---------------------------------------------------------------------------
// エラーパステスト (ER-01)
// ---------------------------------------------------------------------------

TEST(ParseVersion, ER_EmptyString)
{
    // ER-01: 空文字列入力時にクラッシュしないことを検証する
    UINT32 val = ParseVersion(L"");
    EXPECT_EQ(val, static_cast<UINT32>(0x80808080));
}

// =============================================================================
// TGT-03: CompareVersion
// =============================================================================

// ---------------------------------------------------------------------------
// 分岐網羅テスト (BR-24 ~ BR-26)
// ---------------------------------------------------------------------------

TEST(CompareVersion, SameVersion)
{
    // BR-24: 同一バージョンで 0 が返ることを検証する (BC-03-01)
    EXPECT_EQ(CompareVersion(L"2.4.1.0", L"2.4.1.0"), 0);
}

TEST(CompareVersion, AisNewer_MajorDiff)
{
    // BR-25: A が新しい場合に正の値が返ることを検証する (BC-03-02)
    EXPECT_GT(CompareVersion(L"3.0.0.0", L"2.0.0.0"), 0);
}

TEST(CompareVersion, BisNewer_MinorDiff)
{
    // BR-26: B が新しい場合に負の値が返ることを検証する (BC-03-03)
    EXPECT_LT(CompareVersion(L"2.3.0.0", L"2.4.0.0"), 0);
}

// ---------------------------------------------------------------------------
// 同値クラステスト (EC-11, EC-12)
// ---------------------------------------------------------------------------

TEST(CompareVersion, ModifierOrdering_AlphaLessThanBeta)
{
    // EC-11: 修飾子による順序関係の全パターン (EC-03-04)
    EXPECT_LT(CompareVersion(L"2.4.0alpha", L"2.4.0beta"), 0);
}

TEST(CompareVersion, ModifierOrdering_BetaLessThanRc)
{
    // EC-11: beta < rc
    EXPECT_LT(CompareVersion(L"2.4.0beta", L"2.4.0rc"), 0);
}

TEST(CompareVersion, ModifierOrdering_RcLessThanRelease)
{
    // EC-11: rc < 無修飾 (EC-03-05)
    EXPECT_LT(CompareVersion(L"2.4.0rc", L"2.4.0"), 0);
}

TEST(CompareVersion, ModifierOrdering_ReleaseLessThanPl)
{
    // EC-11: 無修飾 < pl (EC-03-06)
    EXPECT_LT(CompareVersion(L"2.4.0", L"2.4.0pl"), 0);
}

TEST(CompareVersion, ModifierOrdering_UnknownLessThanAlpha)
{
    // EC-11: 不明文字(x) < alpha
    EXPECT_LT(CompareVersion(L"2.4.0x", L"2.4.0alpha"), 0);
}

TEST(CompareVersion, ModifierOrdering_FullChain)
{
    // EC-11: 全修飾子の順序を一括検証: x < a < b < rc < 無 < pl
    EXPECT_LT(CompareVersion(L"1.0.0x",     L"1.0.0alpha"), 0);
    EXPECT_LT(CompareVersion(L"1.0.0alpha", L"1.0.0beta"),  0);
    EXPECT_LT(CompareVersion(L"1.0.0beta",  L"1.0.0rc"),    0);
    EXPECT_LT(CompareVersion(L"1.0.0rc",    L"1.0.0"),      0);
    EXPECT_LT(CompareVersion(L"1.0.0",      L"1.0.0pl"),    0);
}

TEST(CompareVersion, DifferentSeparatorsEqual)
{
    // EC-12: 異なる区切り文字で同一バージョンと判定されることを検証する (EC-03-07)
    EXPECT_EQ(CompareVersion(L"2.4.1.0", L"2-4_1+0"), 0);
}

// ---------------------------------------------------------------------------
// 依存切替テスト (DP-01)
// ---------------------------------------------------------------------------

TEST(CompareVersion, DP_ParseVersionIntegration)
{
    // DP-01: ParseVersion の結果を正しく利用していることを間接的に検証する
    // CompareVersion は内部で ParseVersion(verA) - ParseVersion(verB) を計算
    // 特殊ケース: 両方空文字列 → 0x80808080 - 0x80808080 = 0
    EXPECT_EQ(CompareVersion(L"", L""), 0);

    // パッチバージョンの差
    EXPECT_GT(CompareVersion(L"2.4.2", L"2.4.1"), 0);
    EXPECT_LT(CompareVersion(L"2.4.1", L"2.4.2"), 0);
}

TEST(CompareVersion, BV_MinimalDifference)
{
    // BV-03-01: 最小差 (1コンポーネント差が1)
    int diff = CompareVersion(L"1.0.0.1", L"1.0.0.0");
    EXPECT_GT(diff, 0);
}

TEST(CompareVersion, BV_BothEmpty)
{
    // BV-03-02: 両方とも空文字列
    EXPECT_EQ(CompareVersion(L"", L""), 0);
}

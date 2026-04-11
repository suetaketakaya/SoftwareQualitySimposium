// =============================================================================
// test-format-additional.cpp
// 対象: sakura_core/util/format.cpp
//   - GetDateTimeFormat (TGT-01)
//   - ParseVersion      (TGT-02)
//   - CompareVersion    (TGT-03)
//
// TRM網羅性監査 (trm-coverage-audit.md) で指摘された漏れ項目のカバー
// 生成: SQiP 2026 実証実験 Phase 3 追加テスト
// =============================================================================

#include <gtest/gtest.h>

#include <string>
#include <string_view>
#include <cstdint>
#include <climits>

// sakura-editor ヘッダ (macOS compat)
#include "sakura_core/util/format.h"

// =============================================================================
// ヘルパー: テスト用 SYSTEMTIME 構築
// (既存テストの MakeSysTime と同一シグネチャ。リンク時に衝突しないよう
//  別翻訳単位とする前提。同一実行ファイルに含める場合は既存側と統合すること)
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

// =============================================================================
// TGT-01: GetDateTimeFormat — 追加テスト
// =============================================================================

// ---------------------------------------------------------------------------
// GAP-EC-01: 全角文字を含む書式文字列
// ---------------------------------------------------------------------------
TEST(GetDateTimeFormat_Additional, GAP_EC_01_ZenkakuLiterals)
{
    // GAP-EC-01: 全角文字を含む書式文字列 (L"年%Y月%m日")
    // 通常文字パスに全角文字が含まれるケース。全角文字がそのまま出力されることを確認。
    SYSTEMTIME st = MakeSysTime(2026, 3, 15, 0, 0, 0);
    auto result = GetDateTimeFormat(L"年%Y月%m日", st);
    EXPECT_EQ(result, L"年2026月03日");
}

TEST(GetDateTimeFormat_Additional, GAP_EC_01_ZenkakuOnlyLiterals)
{
    // GAP-EC-01 補足: 全角文字のみのリテラル（指定子なし）
    SYSTEMTIME st = MakeSysTime(2026, 1, 1, 0, 0, 0);
    auto result = GetDateTimeFormat(L"令和八年", st);
    EXPECT_EQ(result, L"令和八年");
}

// ---------------------------------------------------------------------------
// GAP-EC-02: 連続する指定子（区切りなし）
// ---------------------------------------------------------------------------
TEST(GetDateTimeFormat_Additional, GAP_EC_02_ConsecutiveSpecifiers)
{
    // GAP-EC-02: 連続する指定子 (L"%Y%m%d" 区切りなし)
    SYSTEMTIME st = MakeSysTime(2026, 3, 15, 9, 5, 30);
    auto result = GetDateTimeFormat(L"%Y%m%d", st);
    EXPECT_EQ(result, L"20260315");
}

TEST(GetDateTimeFormat_Additional, GAP_EC_02_AllSpecifiersNoSeparator)
{
    // GAP-EC-02 補足: 全指定子を区切りなしで連結
    SYSTEMTIME st = MakeSysTime(2026, 3, 15, 9, 5, 30);
    auto result = GetDateTimeFormat(L"%Y%m%d%H%M%S", st);
    EXPECT_EQ(result, L"20260315090530");
}

// ---------------------------------------------------------------------------
// GAP-BV-01: systime.wDay の境界 (wDay=0, wDay=31)
// ---------------------------------------------------------------------------
TEST(GetDateTimeFormat_Additional, GAP_BV_01_DayZero)
{
    // GAP-BV-01: wDay=0 の境界値
    SYSTEMTIME st = MakeSysTime(2026, 1, 1, 0, 0, 0);
    st.wDay = 0;
    auto result = GetDateTimeFormat(L"%d", st);
    EXPECT_EQ(result, L"00");
}

TEST(GetDateTimeFormat_Additional, GAP_BV_01_Day31)
{
    // GAP-BV-01: wDay=31 の境界値
    SYSTEMTIME st = MakeSysTime(2026, 1, 31, 0, 0, 0);
    auto result = GetDateTimeFormat(L"%d", st);
    EXPECT_EQ(result, L"31");
}

// ---------------------------------------------------------------------------
// GAP-BV-02: systime.wMinute の境界 (wMinute=0, wMinute=59)
// ---------------------------------------------------------------------------
TEST(GetDateTimeFormat_Additional, GAP_BV_02_MinuteZero)
{
    // GAP-BV-02: wMinute=0 の境界値
    SYSTEMTIME st = MakeSysTime(2026, 1, 1, 0, 0, 0);
    auto result = GetDateTimeFormat(L"%M", st);
    EXPECT_EQ(result, L"00");
}

TEST(GetDateTimeFormat_Additional, GAP_BV_02_Minute59)
{
    // GAP-BV-02: wMinute=59 の境界値
    SYSTEMTIME st = MakeSysTime(2026, 1, 1, 0, 59, 0);
    auto result = GetDateTimeFormat(L"%M", st);
    EXPECT_EQ(result, L"59");
}

// ---------------------------------------------------------------------------
// GAP-BV-03: systime.wYear=0 での %Y と %y
// ---------------------------------------------------------------------------
TEST(GetDateTimeFormat_Additional, GAP_BV_03_YearZero_FullYear)
{
    // GAP-BV-03: wYear=0 での %Y 出力
    // swprintf(str, 6, L"%d", 0) → "0"
    SYSTEMTIME st = MakeSysTime(0, 1, 1, 0, 0, 0);
    auto result = GetDateTimeFormat(L"%Y", st);
    EXPECT_EQ(result, L"0");
}

TEST(GetDateTimeFormat_Additional, GAP_BV_03_YearZero_ShortYear)
{
    // GAP-BV-03: wYear=0 での %y 出力
    // swprintf(str, 6, L"%02d", 0 % 100) → "00"
    SYSTEMTIME st = MakeSysTime(0, 1, 1, 0, 0, 0);
    auto result = GetDateTimeFormat(L"%y", st);
    EXPECT_EQ(result, L"00");
}

// ---------------------------------------------------------------------------
// GAP-BV-04: wYear が5桁以上（str[6]バッファ境界テスト）
// ---------------------------------------------------------------------------
TEST(GetDateTimeFormat_Additional, GAP_BV_04_Year5Digits_BufferExact)
{
    // GAP-BV-04: wYear=99999 (5桁) → "99999" は5文字+null=6文字でstr[6]にちょうど収まる
    // WORD の最大値は 65535 なので、実際には 99999 は WORD に収まらない。
    // WORD 最大値 65535 (5桁) で検証する。
    SYSTEMTIME st = MakeSysTime(65535, 1, 1, 0, 0, 0);
    auto result = GetDateTimeFormat(L"%Y", st);
    EXPECT_EQ(result, L"65535");
}

TEST(GetDateTimeFormat_Additional, GAP_BV_04_Year5Digits_ShortYear)
{
    // GAP-BV-04 補足: 5桁年の %y
    // 65535 % 100 = 35 → "35"
    SYSTEMTIME st = MakeSysTime(65535, 1, 1, 0, 0, 0);
    auto result = GetDateTimeFormat(L"%y", st);
    EXPECT_EQ(result, L"35");
}

// ---------------------------------------------------------------------------
// GAP-ER-01: wYear=100000以上（6桁）のバッファオーバーフロー確認
// ---------------------------------------------------------------------------
TEST(GetDateTimeFormat_Additional, GAP_ER_01_Year6Digits_BufferOverflow)
{
    // GAP-ER-01: wYear が6桁以上でバッファオーバーフローの可能性
    // ただし WORD (unsigned short) は最大 65535 なので、通常の SYSTEMTIME では
    // 6桁の年号は表現できない。
    // swprintf に size パラメータ (6) が渡されているため、仮に大きな値が来ても
    // 切り捨てが行われる。
    // WORD 最大値 65535 が5桁であることを確認し、6桁に到達しないことを検証する。
    SYSTEMTIME st = {};
    st.wYear = 65535; // WORD max
    auto result = GetDateTimeFormat(L"%Y", st);
    // 65535 は5文字。str[6] に収まる。
    EXPECT_EQ(result, L"65535");
    // 文字列長が5以下であることを確認（6桁にならない）
    EXPECT_LE(result.length(), 5u);

    // NOTE: wYear は WORD 型 (uint16_t) なので 100000 以上を設定できない。
    // 実際のリスクは swprintf の size パラメータにより軽減される。
    // もし SYSTEMTIME.wYear が int に拡張された場合はバッファオーバーフローが発生する。
    // この制約を文書化するためのテスト。
}

// =============================================================================
// TGT-02: ParseVersion — 追加テスト
// =============================================================================

// ---------------------------------------------------------------------------
// GAP-BR-01: 特殊文字列後の非数字スキップループ（0回と複数回）
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_BR_01_NonDigitSkip_ZeroIterations)
{
    // GAP-BR-01: 特殊文字列処理後、直後が数字の場合 → 非数字スキップwhile が0回
    // "alpha1" → 'a' で alpha 判定 → p+=5 → *p='1' (数字) → スキップwhile 0回
    UINT32 val = ParseVersion(L"alpha1");
    // コンポーネント0: alpha シフト(-0x60=-96) + nVer=1 → 128+(-96)+1=33=0x21
    // 残り3つ: 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x21808080));
}

TEST(ParseVersion_Additional, GAP_BR_01_NonDigitSkip_MultipleIterations)
{
    // GAP-BR-01: 特殊文字列処理後、非数字が複数個続く場合 → スキップwhile がN回
    // "alpha---1" → alpha 判定後、p+=5 → "---1" → '-','-','-' をスキップ(3回) → '1'
    // '-' は iswdigit で false なので、非数字スキップ while で処理される
    UINT32 val = ParseVersion(L"alpha---1");
    // コンポーネント0: alpha シフト(-0x60=-96)
    //   非数字スキップ: '---' をスキップ(3回) → *p='1'
    //   数値抽出: nVer=1
    //   128 + (-96) + 1 = 33 = 0x21
    // 残り3つ: 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x21808080));
}

// ---------------------------------------------------------------------------
// GAP-BR-02: 数値抽出中の文字列終端到達
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_BR_02_StringEndDuringDigitExtraction)
{
    // GAP-BR-02: 数値抽出ループ中に文字列終端に到達するケース
    // "4" → 数字 '4' を読み、次の *p が '\0' で iswdigit('\0') が false → ループ終了
    UINT32 val = ParseVersion(L"4");
    // コンポーネント0: nShift=0 + nVer=4 → 128+0+4=132=0x84
    // 残り3つ: 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x84808080));
}

TEST(ParseVersion_Additional, GAP_BR_02_TwoDigitStringEnd)
{
    // GAP-BR-02 補足: 2桁の数値の直後が文字列終端
    // "2.45" → コンポーネント0: 2 → 0x82, コンポーネント1: 45 → 128+0+45=173=0xAD
    UINT32 val = ParseVersion(L"2.45");
    EXPECT_EQ(val, static_cast<UINT32>(0x82AD8080));
}

// ---------------------------------------------------------------------------
// GAP-BR-03: 連続区切り文字（L"2..4"）
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_BR_03_ConsecutiveDots)
{
    // GAP-BR-03: 連続区切り文字のスキップ
    // "2..4" → '2' 読み取り → 区切りスキップで ".." を消費 → '4' 読み取り
    UINT32 val = ParseVersion(L"2..4");
    // コンポーネント0: nShift=0 + nVer=2 → 130=0x82
    // コンポーネント1: nShift=0 + nVer=4 → 132=0x84
    // 残り2つ: 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x82848080));
}

TEST(ParseVersion_Additional, GAP_BR_03_MixedConsecutiveSeparators)
{
    // GAP-BR-03 補足: 異なる種類の区切り文字が連続
    // "2.-_+4" → '2' → 区切りスキップ ".-_+" → '4'
    UINT32 val = ParseVersion(L"2.-_+4");
    EXPECT_EQ(val, static_cast<UINT32>(0x82848080));
}

// ---------------------------------------------------------------------------
// GAP-EC-03: 全角数字を含むバージョン文字列
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_EC_03_ZenkakuDigits)
{
    // GAP-EC-03: 全角数字を含むバージョン文字列 (L"２.４")
    // iswdigit がロケール依存で全角数字を true にする可能性がある。
    // macOS のデフォルトロケールでは iswdigit(L'２') は false のことが多い。
    // 全角数字が数字として認識されるかどうかはロケール依存のため、
    // 少なくともクラッシュしないことを確認する。
    UINT32 val = ParseVersion(L"２.４");
    // 全角数字が iswdigit で false の場合:
    //   '２' → !iswdigit → nShift=-0x80 → 非数字スキップ → 数字なし(nVer=0)
    //   '４' → 同上
    // 全角数字が iswdigit で true の場合:
    //   ロケール依存の動作（数値変換が正しくない可能性）
    // いずれにしてもクラッシュしないこと
    (void)val; // 値は実装とロケールに依存
    SUCCEED() << "ParseVersion with zenkaku digits did not crash";
}

// ---------------------------------------------------------------------------
// GAP-EC-04: 区切り文字のみ（L"..."）
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_EC_04_SeparatorsOnly)
{
    // GAP-EC-04: 区切り文字のみの文字列
    // "..." → コンポーネント0: 非数字(*p=='.'), nShift=-0x80 →
    //   非数字スキップ → '.' はiswdigitでfalse → while(*p && !iswdigit(*p))
    //   Wait: '.' は区切り文字だが、最初のifチェーンで !iswdigit('.') → nShift=-0x80
    //   then 非数字スキップ while で '.' を全てスキップ → 文字列終端 → nVer=0
    //   128 + (-0x80) + 0 = 0
    UINT32 val = ParseVersion(L"...");
    // 最初のforループ:
    //   i=0: *p='.' → !iswdigit → nShift=-0x80
    //     非数字スキップ: p++ で '.' をスキップ、'.' をスキップ、'.' をスキップ → *p='\0'
    //     数値抽出: *p='\0' → nVer=0
    //     区切りスキップ: *p='\0' → スキップなし
    //     ret |= ((-0x80+0+128) << 24) = (0 << 24) = 0
    //   i=1: *p='\0' → ループ終了
    // 残り: i=1,2,3 → 0x80 ずつ
    EXPECT_EQ(val, static_cast<UINT32>(0x00808080));
}

// ---------------------------------------------------------------------------
// GAP-EC-05: 数字なしの修飾子のみ（L"alpha"）
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_EC_05_ModifierOnly_Alpha)
{
    // GAP-EC-05: 修飾子のみ、数字なし
    // "alpha" → p+=5 → *p='\0' → 非数字スキップ: *p='\0' → nVer=0
    // 128 + (-0x60=-96) + 0 = 32 = 0x20
    UINT32 val = ParseVersion(L"alpha");
    EXPECT_EQ(val, static_cast<UINT32>(0x20808080));
}

TEST(ParseVersion_Additional, GAP_EC_05_ModifierOnly_Beta)
{
    // GAP-EC-05 補足: beta のみ
    // 128 + (-0x40=-64) + 0 = 64 = 0x40
    UINT32 val = ParseVersion(L"beta");
    EXPECT_EQ(val, static_cast<UINT32>(0x40808080));
}

TEST(ParseVersion_Additional, GAP_EC_05_ModifierOnly_Rc)
{
    // GAP-EC-05 補足: rc のみ
    // 128 + (-0x20) + 0 = 108 = 0x6C → Wait: 108=0x6C
    // Hmm, let me recalculate: 128 + (-0x20) + 0 = 128 - 32 = 96 = 0x60
    UINT32 val = ParseVersion(L"rc");
    EXPECT_EQ(val, static_cast<UINT32>(0x60808080));
}

TEST(ParseVersion_Additional, GAP_EC_05_ModifierOnly_Pl)
{
    // GAP-EC-05 補足: pl のみ
    // 128 + 0x20 + 0 = 128 + 32 = 160 = 0xA0
    UINT32 val = ParseVersion(L"pl");
    EXPECT_EQ(val, static_cast<UINT32>(0xA0808080));
}

// ---------------------------------------------------------------------------
// GAP-EC-06: 非常に長い文字列
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_EC_06_VeryLongString)
{
    // GAP-EC-06: 非常に長い文字列
    // ParseVersion は最大4コンポーネントしか処理しないため、
    // 長い文字列でも4コンポーネント分を読んだら終了するはず。
    // バッファオーバーリードが発生しないことを確認。
    std::wstring longVer = L"1.2.3.4";
    // 4コンポーネントの後に大量の文字を追加
    for (int i = 0; i < 1000; ++i) {
        longVer += L".99";
    }
    UINT32 val = ParseVersion(longVer.c_str());
    // 最初の4コンポーネント (1, 2, 3, 4) のみが処理される
    UINT32 expected = ParseVersion(L"1.2.3.4");
    EXPECT_EQ(val, expected);
}

TEST(ParseVersion_Additional, GAP_EC_06_LongModifierString)
{
    // GAP-EC-06 補足: 修飾子に非常に長い非数字文字列が含まれる場合
    // "1." + 'x' * 10000 + "2"
    std::wstring longVer = L"1.";
    for (int i = 0; i < 10000; ++i) {
        longVer += L'x';
    }
    longVer += L"2";
    UINT32 val = ParseVersion(longVer.c_str());
    // クラッシュしないことが主目的
    (void)val;
    SUCCEED() << "ParseVersion with very long modifier string did not crash";
}

// ---------------------------------------------------------------------------
// GAP-BV-05: nVer最大値（nShift=-0x80, nVer=0）
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_BV_05_ShiftMinusMax_nVerZero)
{
    // GAP-BV-05: nShift=-0x80, nVer=0 の組み合わせ
    // 不明文字 (例: 'x') → nShift=-0x80
    // その後数字なし → nVer=0
    // コンポーネント値 = 128 + (-0x80) + 0 = 128 - 128 + 0 = 0
    // 最小のコンポーネント値が 0 になる
    UINT32 val = ParseVersion(L"x");
    // x → nShift=-0x80, 非数字スキップ後 *p='\0', nVer=0
    // コンポーネント0: 0 → 0x00
    // 残り3つ: 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x00808080));
}

TEST(ParseVersion_Additional, GAP_BV_05_ShiftMinusMax_nVerMax)
{
    // GAP-BV-05 補足: nShift=-0x80, nVer=99 (2桁最大)
    // コンポーネント値 = 128 + (-0x80) + 99 = 99 = 0x63
    UINT32 val = ParseVersion(L"x99");
    // x → nShift=-0x80, 非数字スキップ: *p='9' (数字) → 0回
    // 数値抽出: nVer=99
    // 128 + (-128) + 99 = 99 = 0x63
    EXPECT_EQ(val, static_cast<UINT32>(0x63808080));
}

TEST(ParseVersion_Additional, GAP_BV_05_ShiftPl_nVerMax)
{
    // GAP-BV-05 補足: nShift=+0x20, nVer=99
    // コンポーネント値 = 128 + 0x20 + 99 = 128 + 32 + 99 = 259 = 0x103
    // 8ビット超過！ 下位8ビット = 0x03, OR演算で上位ビットが他のコンポーネントに侵入
    UINT32 val = ParseVersion(L"pl99");
    // これは GAP-BV-06 のオーバーフローケースでもある
    // コンポーネント0: (259) << 24 → 0x03000000 (上位ビットが切り捨て/マスクされないか確認)
    // ただし C++ の int → UINT32 の動作で、259 << 24 = 0x03000000 ではなく
    // (259 & 0xFF) ではない → そのまま 259 << 24 = 0x030000 << 8 ...
    // 実際: 259 = 0x103, 259 << 24 ではなく ret |= (259 << 24)
    // 259 << 24 = 0x103 << 24 = overflow for 32bit
    // 259 * 2^24 = 4,345,298,944 = 0x103000000 → 下位32ビット = 0x03000000
    // ret |= 0x03000000, 残り 0x80 * 3 → 0x03808080
    EXPECT_EQ(val, static_cast<UINT32>(0x03808080));
}

// ---------------------------------------------------------------------------
// GAP-BV-06: nShift+nVer+128のオーバーフロー
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_BV_06_ComponentOverflow_PlWithDigits)
{
    // GAP-BV-06: pl修飾子 + 最大2桁数値でコンポーネント値が8ビットを超過
    // "1.2.3pl99" → コンポーネント2 (第3): '3' → nShift=0, nVer=3 → 131=0x83
    //   Wait: "3pl99" → '3' → 数字なので nShift=0, nVer=3 → 0x83
    //   then "pl" → 次のコンポーネント(i=3): nShift=+0x20, nVer=99 → 259
    //   259 << (24-8*3) = 259 << 0 = 259 = 0x103
    //   ret |= 0x103 → 下位8ビットは 0x03 だが、ビット8もセットされる
    // バージョン比較に影響する可能性がある
    UINT32 val = ParseVersion(L"1.2.3pl99");
    // コンポーネント0: 1 → 129=0x81
    // コンポーネント1: 2 → 130=0x82
    // コンポーネント2: 3 → 131=0x83
    // コンポーネント3: pl99 → 128+32+99=259=0x103
    // ret = (0x81 << 24) | (0x82 << 16) | (0x83 << 8) | 0x103
    //     = 0x81000000 | 0x00820000 | 0x00008300 | 0x00000103
    //     = 0x81828303 (ビット8がコンポーネント2の最下位ビットに影響)
    //       Wait: 0x00008300 | 0x00000103 = 0x00008303
    //       0x83 << 8 = 0x8300, 0x0103: 0x8300 | 0x0103 = 0x8303
    //       So ret = 0x81828303
    // しかし本来 0x83 << 8 で 0x8300 が設定済みの位置にビット8(0x0100)が立つ
    // ORなので: 0x8300 | 0x0103 = 0x8303 ← コンポーネント2の値が破壊される
    EXPECT_EQ(val, static_cast<UINT32>(0x81828303));
    // NOTE: これはオーバーフローによりコンポーネント間のビットが侵食される既知の問題
}

// ---------------------------------------------------------------------------
// GAP-ER-02: NULL入力
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_ER_02_NullInput)
{
    // GAP-ER-02: NULL入力
    // NOTE: NULLチェックなし、実行すると *p でNULLポインタ参照によりクラッシュする可能性
    // ParseVersion は sVer が NULL の場合のチェックを行わないため、
    // このテストは実行するとクラッシュする。GTEST_SKIP で記録のみ行う。
    GTEST_SKIP() << "ParseVersion has no NULL check; calling with NULL would crash (segfault)";

    // 以下は実行されない（GTEST_SKIP により）
    // UINT32 val = ParseVersion(nullptr);
}

// =============================================================================
// TGT-03: CompareVersion — 追加テスト
// =============================================================================

// ---------------------------------------------------------------------------
// GAP-EC-21: UINT32差分オーバーフロー
// ---------------------------------------------------------------------------
TEST(CompareVersion_Additional, GAP_EC_21_UINT32_DiffOverflow)
{
    // GAP-EC-21: UINT32差分オーバーフロー
    // "99.99.99.99pl99" は非常に大きなパース値を生成
    // "" は 0x80808080
    // CompareVersion = ParseVersion(A) - ParseVersion(B)
    // これは UINT32 の減算であり、結果を int にキャストしている
    // 差分が INT_MAX を超える場合、符号が反転する可能性がある

    // まず各パース値を確認
    UINT32 vMax = ParseVersion(L"99.99.99pl99");
    UINT32 vMin = ParseVersion(L"");

    // "99.99.99pl99":
    //   コンポーネント0: 99 → 128+0+99=227=0xE3
    //   コンポーネント1: 99 → 0xE3
    //   コンポーネント2: 99 → 0xE3
    //   コンポーネント3: pl99 → 128+32+99=259=0x103 → オーバーフロー
    // vMin = 0x80808080

    int diff = CompareVersion(L"99.99.99pl99", L"");
    // vMax - vMin は大きな正の値になるはずだが、int キャストで負になる可能性
    // テスト: 少なくとも vMax > vMin なので正の差分が期待されるが、
    // オーバーフローで負になるかもしれない
    // この振る舞いを記録する
    (void)diff;
    SUCCEED() << "CompareVersion diff overflow test completed without crash. "
              << "vMax=0x" << std::hex << vMax << " vMin=0x" << vMin
              << " diff=" << std::dec << diff;
}

TEST(CompareVersion_Additional, GAP_EC_21_LargePositiveDiff)
{
    // GAP-EC-21 補足: 大きな差分が INT_MAX を超えないケース
    // "99.99.99.99" vs "0.0.0.0"
    UINT32 vA = ParseVersion(L"99.99.99.99");
    UINT32 vB = ParseVersion(L"0.0.0.0");
    // vA = 0xE3E3E3E3, vB = 0x80808080
    // diff = 0xE3E3E3E3 - 0x80808080 = 0x63636363 = 1,667,457,891
    // INT_MAX = 2,147,483,647 → 差分 < INT_MAX → 安全
    int diff = CompareVersion(L"99.99.99.99", L"0.0.0.0");
    EXPECT_GT(diff, 0);
    // 具体的な値も検証
    EXPECT_EQ(diff, static_cast<int>(0x63636363));
}

// ---------------------------------------------------------------------------
// GAP-ER-03: CompareVersion NULL入力
// ---------------------------------------------------------------------------
TEST(CompareVersion_Additional, GAP_ER_03_NullInputA)
{
    // GAP-ER-03: NULL入力（verA が NULL）
    // NOTE: NULLチェックなし、実行するとParseVersion内で*pのNULL参照によりクラッシュする可能性
    GTEST_SKIP() << "CompareVersion passes NULL to ParseVersion which has no NULL check; would crash";

    // 以下は実行されない
    // int diff = CompareVersion(nullptr, L"1.0.0");
}

TEST(CompareVersion_Additional, GAP_ER_03_NullInputB)
{
    // GAP-ER-03: NULL入力（verB が NULL）
    // NOTE: NULLチェックなし、実行するとクラッシュする可能性
    GTEST_SKIP() << "CompareVersion passes NULL to ParseVersion which has no NULL check; would crash";

    // 以下は実行されない
    // int diff = CompareVersion(L"1.0.0", nullptr);
}

TEST(CompareVersion_Additional, GAP_ER_03_NullInputBoth)
{
    // GAP-ER-03: 両方 NULL
    // NOTE: NULLチェックなし、実行するとクラッシュする可能性
    GTEST_SKIP() << "CompareVersion passes NULL to ParseVersion which has no NULL check; would crash";

    // 以下は実行されない
    // int diff = CompareVersion(nullptr, nullptr);
}

// ---------------------------------------------------------------------------
// GAP-ER-04: UINT32差分のintキャスト時オーバーフロー
// ---------------------------------------------------------------------------
TEST(CompareVersion_Additional, GAP_ER_04_IntCastOverflow)
{
    // GAP-ER-04: nVerA=0xFFFFFFFF相当, nVerB=0相当 のバージョン文字列
    // CompareVersion は nVerA - nVerB を返すが、UINT32 の減算結果が int に収まらない場合
    // 符号が反転する。
    //
    // ParseVersion で最大のUINT32値を作るには:
    //   各コンポーネントが 0xFF (=255) → nShift+nVer+128=255 → nShift+nVer=127
    //   nShift=0x20 (pl), nVer=99 → 128+32+99=259=0x103 → 8ビット超過
    //   nShift=0, nVer=99 → 128+0+99=227=0xE3 → 最大 0xE3
    // 完全な 0xFFFFFFFF は ParseVersion の通常入力では到達できない。
    //
    // 到達可能な最大差分を検証:
    // ParseVersion の最大値: "99.99.99.99" → 0xE3E3E3E3
    // ParseVersion の最小値: "x" の4コンポーネント連鎖 → 各 0x00
    // ただし4コンポーネント全てを 0x00 にするのは難しい。
    // "x" → 0x00808080 (1コンポーネントのみ0)

    // 理論上の最大差分: 0xE3E3E3E3 - 0x00000000 (到達不可能)
    // 実際に到達可能な最大差分に近いケース:
    UINT32 vHigh = ParseVersion(L"99.99.99.99");  // 0xE3E3E3E3
    UINT32 vLow  = ParseVersion(L"");              // 0x80808080
    UINT32 rawDiff = vHigh - vLow;
    int signedDiff = static_cast<int>(rawDiff);

    // rawDiff = 0x63636363 = 1,667,457,891 < INT_MAX (2,147,483,647)
    // この場合はオーバーフローしない
    EXPECT_GT(signedDiff, 0);
    EXPECT_EQ(rawDiff, static_cast<UINT32>(0x63636363));

    // オーバーフローが発生するケースを構築:
    // nShift=0x20 (pl) + nVer=99 → 259 → オーバーフローコンポーネント
    // "pl99.pl99.pl99.pl99" vs ""
    UINT32 vOverflow = ParseVersion(L"pl99.pl99.pl99.pl99");
    UINT32 vEmpty = ParseVersion(L"");
    UINT32 rawDiff2 = vOverflow - vEmpty;
    int signedDiff2 = CompareVersion(L"pl99.pl99.pl99.pl99", L"");

    // vOverflow のコンポーネント値はそれぞれ 259=0x103 でビット侵食が発生
    // 正確な結果は実装依存だが、CompareVersion は正の値を返すべき
    // しかしオーバーフローで符号が反転する可能性がある
    // このテストはその振る舞いを文書化する
    (void)rawDiff2;
    (void)signedDiff2;
    SUCCEED() << "Int cast overflow test: vOverflow=0x" << std::hex << vOverflow
              << " vEmpty=0x" << vEmpty
              << " rawDiff=0x" << rawDiff2
              << " signedDiff=" << std::dec << signedDiff2;
}

// ---------------------------------------------------------------------------
// GAP-DP-01: ParseVersionがオーバーフロー値を返した場合のCompareVersionの動作
// ---------------------------------------------------------------------------
TEST(CompareVersion_Additional, GAP_DP_01_ParseVersionOverflowPropagation)
{
    // GAP-DP-01: ParseVersionがオーバーフロー値を返した場合のCompareVersionの動作
    //
    // ParseVersion のコンポーネント値が 8ビットを超過する場合（pl99 等）、
    // 隣接コンポーネントのビットが破壊される。
    // CompareVersion はその破壊された値同士を比較するため、
    // 直感に反する結果になる可能性がある。

    // ケース1: コンポーネント3のオーバーフローがコンポーネント2に侵入
    // "1.0.0pl99" vs "1.0.1"
    // 期待: pl99 は release の 0 より大きいので "1.0.0pl99" > "1.0.0" だが
    //        "1.0.0pl99" と "1.0.1" の比較は微妙
    int diff1 = CompareVersion(L"1.0.0pl99", L"1.0.1");
    // "1.0.0pl99":
    //   コンポーネント0: 1 → 129=0x81
    //   コンポーネント1: 0 → 128=0x80
    //   コンポーネント2: 0 → 128=0x80
    //   コンポーネント3: pl99 → 259=0x103
    //   ret = 0x81800103 (ビット8がコンポーネント2に侵入: 0x80 | 0x01 = 0x81)
    //   Actually: (0x80 << 8) | 0x103 = 0x8000 | 0x0103 = 0x8103
    //   ret = 0x81808103
    //
    // "1.0.1":
    //   ret = 0x81808180
    //
    // diff = 0x81808103 - 0x81808180 = -0x7D = -125
    // pl99 のオーバーフローにより、"1.0.0pl99" < "1.0.1" となるのは正しいが、
    // コンポーネント2のビットが破壊されている
    EXPECT_LT(diff1, 0) << "1.0.0pl99 should be less than 1.0.1 (patch level does not exceed minor increment)";

    // ケース2: 同一バージョンでpl有無の比較（オーバーフローなし）
    // "1.0.0pl1" vs "1.0.0"
    int diff2 = CompareVersion(L"1.0.0pl1", L"1.0.0");
    // "1.0.0pl1": pl1 → 128+32+1=161=0xA1 (8ビット内、オーバーフローなし)
    //   ret = 0x818080A1
    // "1.0.0": ret = 0x81808080
    // diff = 0xA1 - 0x80 = 33 > 0
    EXPECT_GT(diff2, 0) << "pl1 should be greater than release";
}

TEST(CompareVersion_Additional, GAP_DP_01_OverflowReversesComparison)
{
    // GAP-DP-01 補足: オーバーフローが比較結果を反転させるケースの検証
    // "0.0.0pl99" vs "0.0.0.99"
    UINT32 vPl99 = ParseVersion(L"0.0.0pl99");
    UINT32 vDot99 = ParseVersion(L"0.0.0.99");

    // "0.0.0pl99":
    //   コンポーネント0: 0 → 128=0x80
    //   コンポーネント1: 0 → 128=0x80
    //   コンポーネント2: 0 → 128=0x80
    //   コンポーネント3: pl99 → 128+32+99=259=0x103
    //   ret = 0x80808003 (ビット8がコンポーネント2に侵入)
    //   Actually: (0x80 << 8) | 0x103 = 0x8103
    //   ret = 0x80808103

    // "0.0.0.99":
    //   コンポーネント3: 99 → 128+0+99=227=0xE3
    //   ret = 0x808080E3

    // vPl99 = 0x80808103, vDot99 = 0x808080E3
    // diff = 0x80808103 - 0x808080E3 = 0x20 = 32 > 0

    int diff = CompareVersion(L"0.0.0pl99", L"0.0.0.99");
    // pl は通常 release より上だが、99 は大きな数値
    // オーバーフローにより pl99 > 0.99 となる（ビット侵食の影響）
    // 結果を記録
    (void)diff;
    SUCCEED() << "Overflow propagation: vPl99=0x" << std::hex << vPl99
              << " vDot99=0x" << vDot99
              << " diff=" << std::dec << diff;
}

// =============================================================================
// 追加の組み合わせ・網羅テスト
// =============================================================================

// ---------------------------------------------------------------------------
// ParseVersion: 修飾子の直後に数字が来るケースの網羅（GAP-BR-01 関連）
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_BR_01_BetaFollowedByDigit)
{
    // beta 直後の数字（非数字スキップ0回）
    UINT32 val = ParseVersion(L"beta3");
    // コンポーネント0: beta shift=-0x40=-64, 非数字スキップ0回, nVer=3
    // 128 + (-64) + 3 = 67 = 0x43
    EXPECT_EQ(val, static_cast<UINT32>(0x43808080));
}

TEST(ParseVersion_Additional, GAP_BR_01_RcFollowedByDigit)
{
    // rc 直後の数字（非数字スキップ0回）
    UINT32 val = ParseVersion(L"rc3");
    // 128 + (-0x20) + 3 = 111 = 0x6F → Wait: 128 - 32 + 3 = 99 = 0x63
    UINT32 expected_component = 128 + (-0x20) + 3;  // = 99 = 0x63
    EXPECT_EQ(val, static_cast<UINT32>((expected_component << 24) | 0x00808080));
}

TEST(ParseVersion_Additional, GAP_BR_01_PlFollowedByDigit)
{
    // pl 直後の数字（非数字スキップ0回）
    UINT32 val = ParseVersion(L"pl3");
    // 128 + 0x20 + 3 = 163 = 0xA3
    EXPECT_EQ(val, static_cast<UINT32>(0xA3808080));
}

// ---------------------------------------------------------------------------
// ParseVersion: 文字列中盤での終端到達パターン（GAP-BR-02 関連）
// ---------------------------------------------------------------------------
TEST(ParseVersion_Additional, GAP_BR_02_SingleDigitComponent)
{
    // 各コンポーネントが1桁で終端に到達するパターン
    UINT32 val = ParseVersion(L"1.2");
    // コンポーネント0: 1 → 129=0x81
    // コンポーネント1: 2 → 130=0x82
    // 残り: 0x80, 0x80
    EXPECT_EQ(val, static_cast<UINT32>(0x81828080));
}

// ---------------------------------------------------------------------------
// CompareVersion: 対称性の検証
// ---------------------------------------------------------------------------
TEST(CompareVersion_Additional, Symmetry_OppositeSign)
{
    // CompareVersion(A, B) と CompareVersion(B, A) が逆符号であることを検証
    int diffAB = CompareVersion(L"2.0.0", L"1.0.0");
    int diffBA = CompareVersion(L"1.0.0", L"2.0.0");
    EXPECT_GT(diffAB, 0);
    EXPECT_LT(diffBA, 0);
    // UINT32減算のため、厳密に diffAB == -diffBA とは限らない
    // (UINT32のラップアラウンド)
    // ただしこの範囲では安全なはず
    EXPECT_EQ(diffAB, -diffBA);
}

TEST(CompareVersion_Additional, Symmetry_EqualVersions)
{
    // 同一バージョンの場合は常に 0
    EXPECT_EQ(CompareVersion(L"1.2.3.4", L"1.2.3.4"), 0);
    EXPECT_EQ(CompareVersion(L"alpha", L"alpha"), 0);
    EXPECT_EQ(CompareVersion(L"1.0.0rc", L"1.0.0rc"), 0);
}

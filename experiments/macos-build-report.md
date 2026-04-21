# macOS Build Experiment Report

**Date**: 2026-03-31  
**Environment**: macOS Darwin 24.0.0 / Apple clang 16.0.0 / Google Test 1.17.0 (Homebrew)  
**Project**: sakura-editor/sakura pure function testing on macOS

---

## 1. Overview

sakura-editor (Windows-only editor) の純粋関数3ファイルをmacOS (Apple clang) + Google Test でコンパイル・テスト実行する環境を構築した。

### Target Functions

| File | Functions | Test File |
|------|-----------|-----------|
| `sakura_core/util/format.cpp` | `GetDateTimeFormat`, `ParseVersion`, `CompareVersion` | `test-format-generated.cpp` (61 tests) |
| `sakura_core/parse/CWordParse.cpp` | `IsMailAddress`, `WhatKindOfTwoChars`, `WhatKindOfTwoChars4KW` | `test-cwordparse-generated.cpp` (53 tests) |
| `sakura_core/convert/convert_util.cpp` | `Convert_ZeneisuToHaneisu`, `Convert_HaneisuToZeneisu` | `test-convert-generated.cpp` (31 tests) |

---

## 2. Build Environment Construction

### 2.1 Windows Dependency Analysis

対象ソースファイルから以下のWindows依存を特定した:

| Category | Items |
|----------|-------|
| **Windows Types** | `BOOL`, `WORD`, `DWORD`, `UINT`, `UINT32`, `WCHAR`, `LPCWSTR`, `SYSTEMTIME`, `HDC`, `HFONT`, `LOGFONT`, `SIZE`, `POINT`, `RECT` |
| **Windows Macros** | `TRUE`/`FALSE`, `_T()`, `LF_FACESIZE`, `_MAX_PATH` |
| **MSVC Functions** | `_wcsnicmp` (-> `wcsncasecmp`), `_memicmp` (custom impl) |
| **sakura-editor Types** | `ECharKind` enum (22 values), `CLogicInt`, `ACHAR`, `uchar_t`, `CWordParse` class |
| **sakura-editor Macros** | `DEBUG_TRACE`, `wcsncmp_literal`, `wcsnicmp_literal`, `LTEXT` |
| **Precompiled Header** | `StdAfx.h` (replaced with shim) |

### 2.2 Compatibility Layer

`experiment-env/compat/` に以下の互換ヘッダを作成:

1. **`windows_compat.h`** -- Windows API型・マクロのmacOS互換定義 (SYSTEMTIME, BOOL, WORD等)
2. **`sakura_compat.h`** -- sakura-editor固有の型 (ECharKind enum, CLogicInt, wcsncmp_literal等)
3. **`StdAfx.h`** -- プリコンパイル済みヘッダのシム
4. **`sakura_core/*/xxx.h`** -- テストファイルのインクルードパスに対応するシムヘッダ

### 2.3 Source Wrappers

対象関数のみを抽出したラッパーファイルを作成:
- `src/format_wrapper.cpp` -- format.cpp の3関数を抽出
- `src/cwordparse_wrapper.cpp` -- CWordParse.cpp の3関数を抽出
- `src/convert_wrapper.cpp` -- convert_util.cpp の2関数+変換テーブルを抽出

**注**: `swprintf` の `%c` 書式指定子を `%lc` に変更 (macOS/POSIXでは `%c` は `char` を期待するため `wchar_t` には `%lc` が必要)

---

## 3. Build Results

```
$ cd experiment-env && mkdir build && cd build && cmake .. && make -j4
```

**Result: 3/3 executables built successfully (0 errors, 0 warnings)**

| Executable | Source Files | Build Status |
|------------|-------------|--------------|
| `test_format` | `format_wrapper.cpp` + `test-format-generated.cpp` | OK |
| `test_cwordparse` | `cwordparse_wrapper.cpp` + `test-cwordparse-generated.cpp` | OK |
| `test_convert` | `convert_wrapper.cpp` + `test-convert-generated.cpp` | OK |

---

## 4. Test Execution Results

### 4.1 Summary

| Test Suite | Total | Passed | Failed | Pass Rate |
|-----------|-------|--------|--------|-----------|
| **Format** (GetDateTimeFormat, ParseVersion, CompareVersion) | 61 | 55 | 6 | 90.2% |
| **CWordParse** (IsMailAddress, WhatKindOfTwoChars, WhatKindOfTwoChars4KW) | 53 | 47 | 6 | 88.7% |
| **Convert** (Convert_ZeneisuToHaneisu, Convert_HaneisuToZeneisu) | 31 | 31 | 0 | 100.0% |
| **Total** | **145** | **133** | **12** | **91.7%** |

### 4.2 Per-Function Results

| Function | Tests | Passed | Failed |
|----------|-------|--------|--------|
| GetDateTimeFormat | 21 | 21 | 0 |
| ParseVersion | 27 | 21 | 6 |
| CompareVersion | 13 | 13 | 0 |
| IsMailAddress | 27 | 24 | 3 |
| WhatKindOfTwoChars | 18 | 15 | 3 |
| WhatKindOfTwoChars4KW | 8 | 8 | 0 |
| Convert_ZeneisuToHaneisu | 17 | 17 | 0 |
| Convert_HaneisuToZeneisu | 11 | 11 | 0 |
| ConvertRoundTrip | 3 | 3 | 0 |

---

## 5. Failure Analysis

**全12件の失敗はテスト期待値の誤りであり、対象関数の実装バグではない。**

### 5.1 ParseVersion: 6 failures (テスト期待値の計算ミス)

#### Category A: modifier parsing interpretation (5 failures)

| Test Name | Input | Actual | Expected | Root Cause |
|-----------|-------|--------|----------|------------|
| `AlphaModifier` | `"2.4.1alpha"` | `0x82848120` | `0x82842180` | 期待値の計算ミス |
| `BetaModifier` | `"2.4.1beta"` | `0x82848140` | `0x82844180` | 同上 |
| `RcModifier` | `"2.4.1rc"` | `0x82848160` | `0x82846180` | 同上 |
| `PlModifier` | `"2.4.1pl"` | `0x828481A0` | `0x8284A180` | 同上 |
| `UnknownModifier` | `"2.4.1x"` | `0x82848100` | `0x82840180` | 同上 |

**原因詳細**: テスト作成者は `"2.4.1alpha"` を「第3コンポーネントが "1alpha" として一体で処理される (nVer=1, nShift=-0x60)」と予測したが、実際のコードでは:
- i=2: `"1"` -> 数字なので nShift=0, nVer=1 を抽出。区切り文字がないのでそのまま次のイテレーションへ
- i=3: `"alpha"` -> alpha修飾子として nShift=-0x60, nVer=0

つまり `"1"` と `"alpha"` は別々のコンポーネントとして処理される。

**正しい期待値**: `"2.4.1alpha"` -> `0x82848120` (comp: 2, 4, 1, alpha+0)

#### Category B: digit grouping interpretation (1 failure)

| Test Name | Input | Actual | Expected | Root Cause |
|-----------|-------|--------|----------|------------|
| `EC_DigitsOnly` | `"1234"` | `0x8CA28080` | `0x81828384` | 数字グルーピングの誤解 |

**原因詳細**: テスト作成者は各桁が1コンポーネントと予測したが、コードは最大2桁まで1コンポーネントとして読む。  
- i=0: `"12"` -> nVer=12 (2桁)
- i=1: `"34"` -> nVer=34 (2桁)
- i=2,3: 残りなし -> 0x80 padding

**正しい期待値**: `"1234"` -> `0x8CA28080` (comp: 12, 34, 0, 0)

### 5.2 IsMailAddress: 3 failures (テスト期待値の誤り)

| Test Name | Issue | Actual | Expected |
|-----------|-------|--------|----------|
| `BR34_DomainDotSeparation` | `addrLen` | 18 | 19 |
| `EC_MultipleDotsDomain` | `addrLen` | 18 | 19 |
| `EC16_OffsetVariations` | 戻り値 | FALSE | TRUE |

#### BR34 / EC_MultipleDotsDomain

**原因**: `"test@example.co.jp"` は18文字 (`wcslen` = 18) であるが、テストは19を期待。テスト作成者がバイト数等と混同した可能性がある (文字列末尾のnull文字を含めてカウントした)。

**正しい期待値**: `addrLen = 18`

#### EC16_OffsetVariations

**原因**: テストコードのバグ。

```cpp
const wchar_t* str = L" test@example.com";
size_t len = wcslen(str);
EXPECT_TRUE(IsMailAddress(str + 1, 1, len - 1, &addrLen));
```

`str + 1` はバッファ先頭を `"test@example.com"` に進めた上で `offset=1` を指定しているため、`pszBuf[offset-1]` = `pszBuf[0]` = `'t'` (有効文字) となり、境界判定で FALSE を返す。

**正しいテストコード**: `IsMailAddress(str, 1, len, &addrLen)` とすべき (バッファは `str` のまま、offset=1)。

### 5.3 WhatKindOfTwoChars: 3 failures (マッピング後の戻り値の誤解)

| Test Name | Call | Actual | Expected |
|-----------|------|--------|----------|
| `BR39_LatinMappedToCSYM` | `(CK_CSYM, CK_LATIN)` | `CK_CSYM (6)` | `CK_LATIN (8)` |
| `BR40_UdefMappedToETC` | `(CK_ETC, CK_UDEF)` | `CK_ETC (10)` | `CK_UDEF (9)` |
| `BR41_CtrlMappedToETC` | `(CK_ETC, CK_CTRL)` | `CK_ETC (10)` | `CK_CTRL (4)` |

**原因詳細**: テスト作成者は「kindCur のマッピング後も元の値が返される」と予測したが、実装では `kindCur` 変数自体が上書きされるため、マッピング後の値が返される。

例: `WhatKindOfTwoChars(CK_CSYM, CK_LATIN)`:
1. 初回同種チェック: CK_CSYM != CK_LATIN -> 不一致
2. 引きずり規則: 該当なし
3. マッピング: `kindCur = CK_LATIN` -> `kindCur = CK_CSYM` (LATIN->CSYM)
4. 再度同種チェック: CK_CSYM == CK_CSYM -> `return kindCur` = `CK_CSYM`

**正しい期待値**: `CK_CSYM` (マッピング後の kindCur の値)

---

## 6. Failure Classification Summary

| Classification | Count | Details |
|---------------|-------|---------|
| テスト期待値の計算ミス (ParseVersion) | 6 | コンポーネント分割ロジックの誤解 |
| テスト期待値の文字数カウントミス (IsMailAddress) | 2 | 文字列長18を19と誤算 |
| テストコードのAPI呼び出しバグ (IsMailAddress) | 1 | offset とバッファポインタの二重シフト |
| テスト期待値のマッピング戻り値誤解 (WhatKindOfTwoChars) | 3 | マッピング後の変数が返されることの誤解 |
| **対象関数の実装バグ** | **0** | **なし** |

---

## 7. macOS Portability Notes

### 7.1 Modifications Required

| Item | Windows Original | macOS Adaptation |
|------|-----------------|-----------------|
| `swprintf` format for `wchar_t` | `%c` | `%lc` (POSIX requirement) |
| `_wcsnicmp` | MSVC built-in | `wcsncasecmp` (POSIX) |
| `_memicmp` | MSVC built-in | Custom inline implementation |
| `StdAfx.h` | Windows PCH with `<windows.h>` | Custom shim with compat types |
| `SYSTEMTIME` struct | `<windows.h>` | Custom struct definition |
| `BOOL`/`TRUE`/`FALSE` | `<windef.h>` | `typedef int BOOL` + `#define` |

### 7.2 Functions Successfully Ported

全8関数がmacOSでコンパイル・実行可能であることを確認。対象関数はWindows API呼び出しを含まない純粋関数であるため、型定義の互換レイヤーのみで移植できた。

---

## 8. Directory Structure

```
experiment-env/
  CMakeLists.txt
  compat/
    windows_compat.h          # Windows型のmacOS互換定義
    sakura_compat.h           # sakura-editor固有型の定義
    StdAfx.h                  # プリコンパイル済みヘッダシム
    sakura_core/
      util/format.h           # format.h シム
      parse/CWordParse.h      # CWordParse.h シム
      convert/convert_util.h  # convert_util.h シム
  src/
    format_wrapper.cpp        # format.cpp の純粋関数抽出
    cwordparse_wrapper.cpp    # CWordParse.cpp の純粋関数抽出
    convert_wrapper.cpp       # convert_util.cpp の純粋関数抽出
  tests/
    test-format-generated.cpp
    test-cwordparse-generated.cpp
    test-convert-generated.cpp
  build/                      # cmake build directory
```

---

## 9. Reproduction Steps

```bash
cd /Users/suetaketakaya/SoftwareQualitySymposium/experiment-env
rm -rf build && mkdir build && cd build
cmake ..
make -j4
ctest --output-on-failure
```

---

## 10. Conclusion

- **ビルド**: 全3テスト実行ファイルが正常にコンパイル・リンク完了
- **テスト結果**: 145テスト中133テスト成功 (91.7% pass rate)
- **失敗テスト**: 12件全てがテスト期待値の誤り (テストバグ)。対象関数の実装バグは検出されなかった
- **macOS移植性**: 純粋関数は型定義の互換レイヤーのみで移植可能。Windows API依存のない関数であれば、同様の手法で他の関数もテスト可能

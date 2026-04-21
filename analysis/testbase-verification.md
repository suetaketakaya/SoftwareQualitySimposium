# テストベース検証レポート

検証日: 2026-03-31
対象: sakura-editor/sakura (master) vs experiment-env ラッパーファイル

---

## 1. 関数ごとの差分表

### 1.1 GetDateTimeFormat (format_wrapper.cpp)

| 箇所 | オリジナル (format.cpp L55) | ラッパー (format_wrapper.cpp L44) | 種別 |
|---|---|---|---|
| swprintf書式 | `L"%c", f` | `L"%lc", (wint_t)f` | 意図的変更 |
| インデント | タブ文字 | スペース4つ | 書式変更 |
| コメント | 日本語Doxygenコメントあり | 省略 | 書式変更 |

関数本体のロジック: **完全一致**（上記1行の意図的変更を除く）

### 1.2 ParseVersion (format_wrapper.cpp)

| 箇所 | オリジナル (format.cpp L80-81) | ラッパー (format_wrapper.cpp L61-62) | 種別 |
|---|---|---|---|
| 変数宣言コメント | `int nShift = 0; //特別な文字列による下駄` | `int nShift = 0;` | コメント省略 |
| 変数宣言コメント | `int nDigit = 0; //連続する数字の数` | `int nDigit = 0;` | コメント省略 |
| ループ内コメント | `//特別な文字列の処理` 等 | 省略 | コメント省略 |
| インデント | タブ文字 | スペース4つ | 書式変更 |

関数本体のロジック: **完全一致**

### 1.3 CompareVersion (format_wrapper.cpp)

差分なし（インデントの違いのみ）。

関数本体のロジック: **完全一致**

### 1.4 WhatKindOfTwoChars (cwordparse_wrapper.cpp)

| 箇所 | オリジナル (CWordParse.cpp) | ラッパー (cwordparse_wrapper.cpp) | 種別 |
|---|---|---|---|
| インデント | タブ文字 | スペース4つ | 書式変更 |
| コメント | `// 同種ならその種別を返す` 等 | 省略 | コメント省略 |

関数本体のロジック: **完全一致**

### 1.5 WhatKindOfTwoChars4KW (cwordparse_wrapper.cpp)

| 箇所 | オリジナル (CWordParse.cpp) | ラッパー (cwordparse_wrapper.cpp) | 種別 |
|---|---|---|---|
| インデント | タブ文字 | スペース4つ | 書式変更 |
| コメント | `// ラテン系文字はアルファベットとみなす` 等 | 省略 | コメント省略 |

関数本体のロジック: **完全一致**

### 1.6 IsMailAddress (cwordparse_wrapper.cpp)

| 箇所 | オリジナル (CWordParse.cpp) | ラッパー (cwordparse_wrapper.cpp) | 種別 |
|---|---|---|---|
| インデント | タブ文字 | スペース4つ | 書式変更 |
| コメント | `/* 直前の文字を利用した境界判定 */` 等 | 省略 | コメント省略 |
| コメントアウト行 | `// nAtPos = j;` `// nAlphaCount = 0;` | 省略 | コメント省略 |

関数本体のロジック: **完全一致**

### 1.7 Convert_ZeneisuToHaneisu (convert_wrapper.cpp)

| 箇所 | オリジナル (convert_util.cpp) | ラッパー (convert_wrapper.cpp) | 種別 |
|---|---|---|---|
| 全角文字リテラル | `L'Ａ'` 等のリテラル表記 | `L'\uFF21'` 等のUnicodeエスケープ | 表記変更（等価） |
| tableZenKigo[16] | U+201D `"` (RIGHT DOUBLE QUOTATION MARK) | U+201C `"` (LEFT DOUBLE QUOTATION MARK) | **意図しない変更** |
| wcschr_idx修飾子 | `bool wcschr_idx(...)` | `static bool wcschr_idx(...)` | リンケージ変更 |
| ZeneisuToHaneisu_修飾子 | `inline wchar_t` | `static inline wchar_t` | リンケージ変更 |
| HaneisuToZeneisu_修飾子 | `inline wchar_t` | `static inline wchar_t` | リンケージ変更 |
| コメント | `//一部の記号も変換する` 等 | 省略 | コメント省略 |
| インデント | タブ文字 | スペース4つ | 書式変更 |

関数本体のロジック: **1箇所の不一致あり**（tableZenKigo[16]の文字）

### 1.8 Convert_HaneisuToZeneisu (convert_wrapper.cpp)

全角文字のUnicodeエスケープ表記（等価）と `static inline` 修飾子の付加を除き、ロジックは同一。ただし **tableZenKigoの不一致が間接的に影響**する。

関数本体のロジック: tableZenKigoの不一致を除けば**完全一致**

---

## 2. 意図的変更の一覧

### 2.1 `%c` -> `%lc` + `(wint_t)` キャスト

- **ファイル**: format_wrapper.cpp L44
- **オリジナル**: `swprintf( str, int(std::size(str)), L"%c", f );`
- **ラッパー**: `swprintf( str, int(std::size(str)), L"%lc", (wint_t)f );`
- **変更理由**: macOS (POSIX) の `swprintf` では、ワイド文字列（`L"..."` フォーマット）中の `%c` は `char` 型引数を期待する。`wchar_t` を正しく出力するには `%lc` を使い、引数を `wint_t` にキャストする必要がある。Windows MSVC の `swprintf` では `%c` でも `wchar_t` を処理するが、POSIX では挙動が異なる。
- **テスト結果への影響**: なし。POSIX環境で正しく動作させるための必要な修正であり、機能的に同等の出力を生成する。

### 2.2 `static` 修飾子の追加

- **ファイル**: convert_wrapper.cpp
- **対象**: `wcschr_idx`, `ZeneisuToHaneisu_`, `HaneisuToZeneisu_`
- **変更理由**: オリジナルではファイルスコープ内で暗黙的に内部リンケージだが、ラッパーでは明示的に `static` を付加。単一翻訳単位でのコンパイルを前提とした安全策。
- **テスト結果への影響**: なし。リンケージの違いは外部からのアクセスに影響するが、テスト対象の関数動作には影響しない。

### 2.3 Unicode エスケープ表記

- **ファイル**: convert_wrapper.cpp
- **対象**: 全角文字リテラル（`L'Ａ'` -> `L'\uFF21'` 等）
- **変更理由**: ソースファイルのエンコーディングに依存しない表記への変換。
- **テスト結果への影響**: なし。コンパイル後のバイナリは同一。

### 2.4 `_wcsnicmp` -> `wcsncasecmp`

- **ファイル**: sakura_compat.h (wcsnicmp_literal テンプレート)
- **オリジナル**: `::_wcsnicmp(strData1, literalData2, Size - 1)`
- **ラッパー**: `::wcsncasecmp(strData1, literalData2, Size - 1)`
- **変更理由**: `_wcsnicmp` はMSVC固有。POSIX相当の `wcsncasecmp` を使用。
- **テスト結果への影響**: なし。同一の機能を提供する。

### 2.5 コメントの省略

- **ファイル**: 全ラッパーファイル
- **変更内容**: 日本語コメント、Doxygenコメント、コメントアウトされたコード行の省略
- **テスト結果への影響**: なし。

### 2.6 インデントの変更

- **ファイル**: 全ラッパーファイル
- **変更内容**: タブ -> スペース4つ
- **テスト結果への影響**: なし。

---

## 3. 意図しない変更・省略の一覧

### 3.1 [重要] tableZenKigo[16]: U+201D -> U+201C

- **ファイル**: convert_wrapper.cpp L17
- **オリジナル**: `"` (U+201D, RIGHT DOUBLE QUOTATION MARK)
- **ラッパー**: `\u201C` (U+201C, LEFT DOUBLE QUOTATION MARK)
- **原因推定**: 抽出時に全角引用符のリテラルをUnicodeエスケープに変換する際、`"` (U+201D) を `"` (U+201C) と取り違えた可能性が高い。見た目が非常に似ているため。
- **影響範囲**:
  - `Convert_ZeneisuToHaneisu`: 全角 -> 半角変換で、オリジナルは `"` (U+201D) を `"` (U+0022) に変換するが、ラッパーは `"` (U+201C) を `"` (U+0022) に変換する。つまり変換対象の文字が異なる。
  - `Convert_HaneisuToZeneisu`: 半角 -> 全角変換で、オリジナルは `"` (U+0022) を `"` (U+201D) に変換するが、ラッパーは `"` (U+0022) を `"` (U+201C) に変換する。つまり変換結果の文字が異なる。
- **テスト結果への影響**: `"` (U+201C/U+201D) を含む入力データに対するテストケースで、ラッパーはオリジナルと異なる結果を返す。それ以外の文字に対しては影響なし。
- **修正推奨**: `\u201C` を `\u201D` に修正すべき。

---

## 4. 型互換性の検証結果

| 型名 | Windows定義 | ラッパー定義 (windows_compat.h) | 判定 |
|---|---|---|---|
| BOOL | `typedef int BOOL;` | `typedef int BOOL;` | 一致 |
| WORD | `typedef unsigned short WORD;` | `typedef unsigned short WORD;` | 一致 |
| DWORD | `typedef unsigned long DWORD;` | `typedef unsigned long DWORD;` | 一致 |
| UINT | `typedef unsigned int UINT;` | `typedef unsigned int UINT;` | 一致 |
| UINT32 | `typedef unsigned __int32 UINT32;` | `typedef uint32_t UINT32;` | 等価 |
| WCHAR | `typedef wchar_t WCHAR;` | `typedef wchar_t WCHAR;` | 一致 |
| TRUE/FALSE | `#define TRUE 1` / `#define FALSE 0` | `#define TRUE 1` / `#define FALSE 0` | 一致 |
| SYSTEMTIME | 8つのWORDフィールド（wYear, wMonth, wDayOfWeek, wDay, wHour, wMinute, wSecond, wMilliseconds） | 同一構造 | 一致 |

| 型名 | sakura-editor定義 | ラッパー定義 (sakura_compat.h) | 判定 |
|---|---|---|---|
| ECharKind | 22値のenum（CK_NULL〜CK_ZEN_ETC） | 同一の22値、同一順序 | 一致 |
| CLogicInt | `typedef int CLogicInt;` (非strictモード) | `typedef int CLogicInt;` | 一致 |
| wcsncmp_literal | テンプレート、`::wcsncmp` | テンプレート、`::wcsncmp` | 一致 |
| wcsnicmp_literal | テンプレート、`::_wcsnicmp` | テンプレート、`::wcsncasecmp` | 等価（POSIX互換） |
| DEBUG_TRACE | デバッグ時はOutputDebugString等 | 常にno-op (`#define DEBUG_TRACE(...)`) | 等価（テスト影響なし） |

**注意事項**: macOSでは `wchar_t` が4バイト (UTF-32)、Windowsでは2バイト (UTF-16) であるが、テスト対象の8関数はBMP範囲内の文字のみを扱うため、この差異はテスト結果に影響しない。ただし、サロゲートペア処理を含む関数（WhatKindOfChar等）をテストする場合は考慮が必要になる。

---

## 5. カバー範囲の確認

### 5.1 format.cpp の全関数

| 関数名 | テスト対象 | ラッパー収録 |
|---|---|---|
| `GetDateTimeFormat` | 対象 | 収録済 |
| `ParseVersion` | 対象 | 収録済 |
| `CompareVersion` | 対象 | 収録済 |

**ファイル内の全3関数が収録済み。**

### 5.2 CWordParse.cpp の全関数

| 関数名 | テスト対象 | ラッパー収録 |
|---|---|---|
| `CWordParse::WhereCurrentWord_2` | 対象外 | 未収録 |
| `isCSymbol` (inline helper) | 対象外 | 未収録 |
| `isCSymbolZen` (inline helper) | 対象外 | 未収録 |
| `CWordParse::WhatKindOfChar` | 対象外 | 未収録 |
| `CWordParse::WhatKindOfTwoChars` | **対象** | **収録済** |
| `CWordParse::WhatKindOfTwoChars4KW` | **対象** | **収録済** |
| `CWordParse::SearchNextWordPosition` | 対象外 | 未収録 |
| `CWordParse::SearchNextWordPosition4KW` | 対象外 | 未収録 |
| `CWordParse::SearchPrevWordPosition` | 対象外 | 未収録 |
| `wc_to_c` | 対象外 | 未収録 |
| `IsURL` | 対象外 | 未収録 |
| `IsMailAddress` | **対象** | **収録済** |

**12関数中3関数が収録済み。**

未収録の関数のうち、`WhereCurrentWord_2`, `WhatKindOfChar`, `SearchNextWordPosition`, `SearchPrevWordPosition`, `IsURL` はクラスメンバ関数やCNativeW等の外部依存が大きく、純粋関数としての抽出が困難なため、対象外とするのは妥当。

### 5.3 convert_util.cpp の全関数

| 関数名 | テスト対象 | ラッパー収録 |
|---|---|---|
| `wcschr_idx` (helper) | 補助関数 | 収録済（static） |
| `ZenhiraToZenkata_` (inline helper) | 対象外 | 未収録 |
| `Convert_ZenhiraToZenkata` | 対象外 | 未収録 |
| `ZenkataToZenhira_` (inline helper) | 対象外 | 未収録 |
| `Convert_ZenkataToZenhira` | 対象外 | 未収録 |
| `ZeneisuToHaneisu_` (inline helper) | 補助関数 | 収録済（static inline） |
| `Convert_ZeneisuToHaneisu` | **対象** | **収録済** |
| `HaneisuToZeneisu_` (inline helper) | 補助関数 | 収録済（static inline） |
| `Convert_HaneisuToZeneisu` | **対象** | **収録済** |
| `Convert_ZenkataToHankata` | 対象外 | 未収録 |
| `Convert_ToHankaku` | 対象外 | 未収録 |
| `Convert_HankataToZenkata` | 対象外 | 未収録 |
| `Convert_HankataToZenhira` | 対象外 | 未収録 |

**13関数中2関数（＋補助関数3つ）が収録済み。**

未収録の関数群（カタカナ変換系）は多数の変換テーブル（tableZenkata_Normal, tableZenkata_Dakuten等）に依存しており、必要に応じて追加可能。

---

## 6. プリプロセッサの影響

### 6.1 `#ifdef _DEBUG` / DEBUG_TRACE

- **オリジナル**: `DEBUG_TRACE` はデバッグビルドでログ出力、リリースビルドでno-op。
- **ラッパー**: 常にno-op（`#define DEBUG_TRACE(...)`）。
- **影響**: なし。テスト対象関数の動作に影響しない。

### 6.2 UNICODE / _UNICODE

- **オリジナル**: sakura-editorは常にUNICODEビルド（`TCHAR = wchar_t`）。
- **ラッパー**: 明示的に `typedef wchar_t TCHAR` とし、`_T(x)` を `L##x` と定義。
- **影響**: なし。同等の動作。

### 6.3 条件コンパイル

テスト対象の8関数には `#ifdef` / `#ifndef` による条件コンパイルブロックは含まれていない。オリジナルのファイル全体に含まれるプリプロセッサディレクティブは、ヘッダーインクルードガードのみであり、テスト対象関数の動作には影響しない。

---

## 7. 総合判定

### 判定: ほぼ適切。ただし1箇所の修正を推奨。

#### 合格項目

1. **関数シグネチャ**: 全8関数で引数の型、戻り値の型、const修飾子がオリジナルと完全一致。
2. **関数ロジック**: 7関数（GetDateTimeFormat, ParseVersion, CompareVersion, WhatKindOfTwoChars, WhatKindOfTwoChars4KW, IsMailAddress, Convert_HaneisuToZeneisu）でロジックがオリジナルと完全一致。Convert_ZeneisuToHaneisuも関数本体は一致（テーブルデータの問題のみ）。
3. **型互換性**: SYSTEMTIME, BOOL, ECharKind, UINT32等の互換定義がオリジナルと同等に動作。
4. **意図的変更**: macOS移植のための `%c` -> `%lc` 変更、`_wcsnicmp` -> `wcsncasecmp` 変更はいずれも正当かつ必要。
5. **プリプロセッサ**: 条件コンパイルの影響なし。

#### 要修正項目

1. **[重要度: 中] convert_wrapper.cpp L17 の tableZenKigo**
   - `\u201C` (LEFT DOUBLE QUOTATION MARK) を `\u201D` (RIGHT DOUBLE QUOTATION MARK) に修正すべき。
   - この不一致により、`"` / `"` を含む文字列の全角⇔半角変換テストで、オリジナルと異なる結果が生じる。
   - ただし、一般的なテストケースでこの文字が含まれない場合は実質的な影響は限定的。

#### 修正コマンド案

```
convert_wrapper.cpp L17:
変更前: L"\u201C\u2018\u2019\uFF1C..."
変更後: L"\u201D\u2018\u2019\uFF1C..."
```

### リスク評価まとめ

| リスク項目 | 評価 |
|---|---|
| tableZenKigo U+201C/U+201D 不一致 | 中（特定文字を含むテストケースで偽陽性/偽陰性の可能性） |
| wchar_t サイズ差 (4byte vs 2byte) | 低（BMP範囲のみ使用のため影響なし） |
| %c -> %lc 変更 | なし（正当な移植変更） |
| static修飾子の追加 | なし（テスト動作に影響なし） |
| コメント省略 | なし |
| インデント変更 | なし |

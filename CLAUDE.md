# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Project Overview

SQiP 2026（ソフトウェア品質シンポジウム2026）への経験発表投稿プロジェクト。静的解析に基づくテスト要求モデル（TRM）の自動生成と、それを用いたLLM単体テスト生成の実証評価を行う。

**実証対象:** sakura-editor/sakura (C++, OSS テキストエディタ)
**対象関数:** 8関数、約385行（format.cpp / CWordParse.cpp / convert_util.cpp）
**提出締切:** 2026年4月14日（EasyChair または メール添付）

## Build & Test Commands

```bash
# テスト環境のビルド（experiment-env/内で実行）
cd experiment-env && rm -rf build && mkdir build && cd build
cmake ..
make -j4

# 全テスト実行
ctest --output-on-failure

# 個別テスト実行
./test_format           # Format系: 107テスト (103 PASS + 4 SKIP)
./test_cwordparse       # CWordParse系: 88テスト (87 PASS + 1 SKIP)
./test_convert          # Convert系: 53テスト (49 PASS + 4 SKIP)

# フィルタ付き実行
./test_format --gtest_filter='ParseVersion*'
./test_format --gtest_filter='*_Additional*'

# 簡易出力
./test_format --gtest_brief=1
```

**前提:** macOS + Apple clang + Google Test (Homebrew: `brew install googletest`)

## Document Generation

```bash
# 図表生成（matplotlib, 日本語フォント: Hiragino Sans）
python3 scripts/generate_figures.py       # fig1-5のPNG生成
python3 scripts/generate_figures_v2.py    # 改善版fig1 + fig5

# 提出用docx生成（python-docx, cairosvg不使用）
python3 scripts/generate_submission_final.py   # → report/submission_final_2026.docx

# プレゼン生成（python-pptx）
python3 scripts/generate_presentation.py       # → report/presentation_2026.pptx
```

**重要:** docx生成スクリプトでは cairosvg を使わないこと（日本語が文字化けする）。matplotlib で直接PNGを生成し、そのパスを参照する。

## Architecture

### 3段階パイプライン

```
Step 1: リポジトリ解析     → analysis/repo-analysis.md
Step 2: TRM生成           → test-requirements/test-requirements.yaml (99件)
Step 3: テストコード生成   → experiment-env/tests/ (248件)
                            ↓
        網羅性監査         → analysis/trm-coverage-audit.md (追加64件特定)
```

### TRM（テスト要求モデル）スキーマ

YAML形式。5種別のテスト要求をID付きで構造化:
- **BR** (Branch Coverage): if/else/switchの各パス → 55件
- **EC** (Equivalence Class): 入力の同値分割 → 27件
- **BV** (Boundary Value): 同値クラス境界 → 11件
- **ER** (Error Path): 異常系 → 3件
- **DP** (Dependency Path): 関数間依存 → 3件

### マルチエージェント（.claude/commands/）

| コマンド | 役割 |
|---|---|
| `/project:repo-analysis` | 対象リポジトリ解析・関数選定 |
| `/project:test-requirement` | TRM YAML生成 |
| `/project:test-generation` | TRMからテストコード生成 |
| `/project:experiment-eval` | 実験結果評価 |
| `/project:paper-writing` | 論文執筆 |
| `/project:peer-review` | 査読レビュー |
| `/project:coordinator` | 進捗管理 |
| `/project:run-all` | 全フェーズ一括実行 |

### experiment-env/ の構成

```
experiment-env/
├── compat/                    # macOS用互換レイヤー
│   ├── windows_compat.h       # SYSTEMTIME, BOOL等のWindows型定義
│   ├── sakura_compat.h        # ECharKind enum, CWordParse等のsakura固有型
│   └── StdAfx.h               # プリコンパイル済みヘッダのシム
├── src/                       # sakura-editorから抽出した対象関数
│   ├── format_wrapper.cpp     # GetDateTimeFormat, ParseVersion, CompareVersion
│   ├── cwordparse_wrapper.cpp # IsMailAddress, WhatKindOfTwoChars, WhatKindOfTwoChars4KW
│   └── convert_wrapper.cpp    # Convert_ZeneisuToHaneisu, Convert_HaneisuToZeneisu
├── tests/
│   ├── test-format-generated.cpp       # TRMベース生成テスト (61件)
│   ├── test-format-additional.cpp      # 網羅性監査後の追加テスト (46件)
│   ├── test-cwordparse-generated.cpp   # TRMベース生成テスト (53件)
│   ├── test-cwordparse-additional.cpp  # 追加テスト (35件)
│   ├── test-convert-generated.cpp      # TRMベース生成テスト (31件)
│   └── test-convert-additional.cpp     # 追加テスト (22件)
└── CMakeLists.txt
```

### テスト実行結果（248件）

| スイート | PASS | SKIP | 合計 |
|---|---|---|---|
| Format | 103 | 4 | 107 |
| CWordParse | 87 | 1 | 88 |
| Convert | 49 | 4 | 53 |
| **合計** | **239** | **9** | **248** |

SKIP 9件はNULLポインタ入力・INT_MINポインタ演算等で、実装にガードがない潜在バグとして記録。

## Key Constraints

- **匿名査読:** 著者名・所属・個人特定情報を本文・ファイル名に含めない
- **日本語:** 論文・発表資料は日本語で記述
- **参考文献:** 17件（URL付き）。literature-review.mdに39件の調査済み文献
- **テスト期待値:** 実際のsakura-editor実装から導出すること。LLMの推論だけで期待値を決めない（初回12件のFAILはLLMの期待値推論誤りだった）

# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

GitHubリポジトリのURLを入力として、ホワイトボックステスト要求モデル（TRM）の自動生成とテストコード生成を行うパイプラインツール。

**v3.1**: OOP拡張 + EN種別追加 — クラス継承(CI)・状態変数(SV)・コードパターン(CP)・**カプセル化(EN)** の解析・テスト要求生成に対応。

## Quick Start

```bash
# 1. 対象リポジトリのセットアップ
/project:setup https://github.com/owner/repo

# 2. パイプライン一括実行
/project:run-pipeline
```

## Pipeline Commands

| コマンド | 役割 |
|---|---|
| `/project:setup` | GitHubリンクから初期設定を生成 |
| `/project:analyze` | リポジトリ解析・テスト対象関数の選定・OOP構造解析 |
| `/project:generate-trm` | TRM（テスト要求モデル）をYAML形式で生成 |
| `/project:audit-trm` | TRMの網羅性監査（漏れの特定・追加要求の導出） |
| `/project:generate-tests` | TRMからテストコードを生成 |
| `/project:run-pipeline` | 上記を順序立てて一括実行 |

### 補助コマンド

| コマンド | 役割 |
|---|---|
| `/project:repo-analysis` | リポジトリ解析（analyzeの別名） |
| `/project:test-requirement` | TRM設計（generate-trmの設計寄り版） |
| `/project:test-generation` | テスト生成（generate-testsの別名） |
| `/project:experiment-eval` | 実験結果評価 |
| `/project:coordinator` | 進捗管理・整合性確認 |
| `/project:run-all` | 全フェーズ一括実行（評価含む） |

## TRM (Test Requirement Model)

テスト要求を構造化したYAML形式の中間成果物。

### 従来型（関数レベル）— 5種別

| 種別 | 記号 | 定義 |
|------|------|------|
| 分岐網羅 | BR | if/else/switchの各パスを検証 |
| 同値クラス | EC | 入力パラメータの同値分割を検証 |
| 境界値 | BV | 同値クラス境界の上下限を検証 |
| エラーパス | ER | 異常系・エラー処理を検証 |
| 依存切替 | DP | 関数間の依存関係・差異を検証 |

### OOP拡張 — 4種別（v3.0: CI/SV/CP、v3.1: EN追加）

| 種別 | 記号 | 定義 | サブタイプ |
|------|------|------|-----------|
| クラス継承 | CI | 継承階層・多態性・オーバーライドを検証 | polymorphic_dispatch / override_correctness / liskov_substitution / abstract_coverage / super_delegation / interface_contract |
| 状態変数 | SV | インスタンス変数の状態遷移・不変条件・**メンバ宣言妥当性** を検証 | initialization / mutation_sequence / invariant_maintenance / state_dependent_behavior / lifecycle / cross_method_state / **member_declaration_validity** (v3.1) / **member_initialization_requirement** (v3.1) |
| コードパターン | CP | デザインパターン・言語イディオム・FW規約を検証 | design_pattern_conformance / idiom_correctness / resource_lifecycle / concurrency_safety / framework_contract / macro_expansion |
| **カプセル化** | **EN** | **メンバの可視性・漏洩アクセサ・不変性契約・構築契約を検証（v3.1追加）** | access_control_correctness / leaky_accessor / mutability_contract / construction_contract / invariant_surface |

スキーマ定義: `templates/trm-schema.yaml` (v3.1)
OOP解析スキーマ: `templates/oop-analysis-schema.yaml` (v1.1)

## Directory Structure

```
.
├── CLAUDE.md
├── .claude/commands/           # パイプラインエージェント（実行用）
│   ├── setup.md               # 初期設定
│   ├── analyze.md             # リポジトリ解析 + OOP解析
│   ├── generate-trm.md        # TRM生成（8種別対応）
│   ├── audit-trm.md           # TRM網羅性監査（OOP含む）
│   ├── generate-tests.md      # テストコード生成（OOP対応）
│   ├── run-pipeline.md        # 一括実行
│   ├── run-all.md             # 全フェーズ実行
│   ├── coordinator.md         # 進捗管理
│   ├── repo-analysis.md       # リポジトリ解析（汎用）
│   ├── test-requirement.md    # TRM設計
│   ├── test-generation.md     # テスト生成
│   ├── experiment-eval.md     # 実験評価
│   ├── paper-writing.md       # 論文執筆
│   └── peer-review.md         # 査読レビュー
├── templates/
│   ├── project-config.yaml    # プロジェクト設定テンプレート（v3.1: OOP + EN設定含む）
│   ├── trm-schema.yaml        # TRM YAMLスキーマ（v3.1: CI/SV/CP/EN）
│   ├── oop-analysis-schema.yaml # OOP解析結果スキーマ（v1.1: encapsulation_analysis含む）
│   └── commands/              # エージェント定義のマスター
├── project-config.yaml        # (setup後に生成) 対象PJの設定
├── analysis/                  # (実行後に生成) 解析結果
│   ├── repo-analysis.md       #   関数選定結果
│   └── oop-analysis.md        #   OOP構造解析結果
├── test-requirements/         # (実行後に生成) TRM YAML
├── generated-tests/           # (実行後に生成) テストコード
│   ├── test-*-generated.*     #   従来型テスト
│   ├── test-*-oop.*           #   OOP拡張テスト (CI/SV/CP)
│   └── test-*-additional.*    #   監査後追加テスト
└── reports/                   # (実行後に生成) レポート
```

## Supported Languages

| 言語 | テストFW | OOP対応 |
|------|---------|---------|
| C++ | Google Test / Catch2 | virtual/override, 多重継承, RAII, CRTP |
| Python | pytest | ABC, MRO, Mixin, デコレータ, コンテキストマネージャ |
| Java | JUnit | インターフェース, abstract, try-with-resources, DI |
| TypeScript | Jest / Vitest | interface, abstract, 判別共用体, 型ガード |
| Go | testing | インターフェース, 埋め込み構造体 |
| Rust | cargo test | トレイト, ジェネリクス, ライフタイム |

## Configuration

`project-config.yaml` で以下を制御:
- `selection_criteria`: テスト対象の選定基準（純粋関数優先、最大行数、除外パターン等）
- `trm.types`: 生成するテスト要求の種別（BR/EC/BV/ER/DP/CI/SV/CP/EN から選択）
- `trm.include_audit`: TRM網羅性監査の実施有無
- `oop_analysis.enabled`: OOP解析の有効/無効
- `oop_analysis.class_inheritance`: クラス継承解析の詳細設定
- `oop_analysis.state_variables`: 状態変数解析の詳細設定
- `oop_analysis.code_patterns`: コードパターン解析の詳細設定
- `oop_analysis.encapsulation`: カプセル化解析の詳細設定（v3.1追加）

## sakura-editor実証実験の成果物

`experiment-env/` にsakura-editor/sakuraを対象とした実証実験の成果物が残っています。
詳細は `experiments/` および `drafts/` を参照してください。

### テスト実行結果（248件 — 従来型TRMのみ）

| スイート | PASS | SKIP | 合計 |
|---|---|---|---|
| Format | 103 | 4 | 107 |
| CWordParse | 87 | 1 | 88 |
| Convert | 49 | 4 | 53 |
| **合計** | **239** | **9** | **248** |

## Key Constraints

- **テスト期待値:** 実際の実装から導出すること。LLMの推論だけで期待値を決めない
- **OOP解析:** `oop_analysis.enabled: false` に設定すれば従来型（BR/EC/BV/ER/DP）のみで動作する

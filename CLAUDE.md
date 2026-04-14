# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## Overview

GitHubリポジトリのURLを入力として、ホワイトボックステスト要求モデル（TRM）の自動生成とテストコード生成を行うパイプラインツール。

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
| `/project:analyze` | リポジトリ解析・テスト対象関数の選定 |
| `/project:generate-trm` | TRM（テスト要求モデル）をYAML形式で生成 |
| `/project:audit-trm` | TRMの網羅性監査（漏れの特定・追加要求の導出） |
| `/project:generate-tests` | TRMからテストコードを生成 |
| `/project:run-pipeline` | 上記を順序立てて一括実行 |

## TRM (Test Requirement Model)

テスト要求を5種別で構造化したYAML形式の中間成果物:

| 種別 | 記号 | 定義 |
|------|------|------|
| 分岐網羅 | BR | if/else/switchの各パスを検証 |
| 同値クラス | EC | 入力パラメータの同値分割を検証 |
| 境界値 | BV | 同値クラス境界の上下限を検証 |
| エラーパス | ER | 異常系・エラー処理を検証 |
| 依存切替 | DP | 関数間の依存関係・差異を検証 |

スキーマ定義: `templates/trm-schema.yaml`

## Directory Structure

```
.
├── CLAUDE.md
├── .claude/commands/           # パイプラインエージェント
│   ├── setup.md               # 初期設定
│   ├── analyze.md             # リポジトリ解析
│   ├── generate-trm.md        # TRM生成
│   ├── audit-trm.md           # TRM網羅性監査
│   ├── generate-tests.md      # テストコード生成
│   └── run-pipeline.md        # 一括実行
├── templates/
│   ├── project-config.yaml    # プロジェクト設定テンプレート
│   ├── trm-schema.yaml        # TRM YAMLスキーマ
│   └── commands/              # エージェント定義のマスター
├── project-config.yaml        # (setup後に生成) 対象PJの設定
├── analysis/                  # (実行後に生成) 解析結果
├── test-requirements/         # (実行後に生成) TRM YAML
├── generated-tests/           # (実行後に生成) テストコード
└── reports/                   # (実行後に生成) レポート
```

## Supported Languages

| 言語 | テストFW | 対応状況 |
|------|---------|---------|
| C++ | Google Test / Catch2 | 実証済み (sakura-editor) |
| Python | pytest | 対応 |
| Java | JUnit | 対応 |
| TypeScript | Jest / Vitest | 対応 |
| Go | testing | 対応 |
| Rust | cargo test | 対応 |

## Configuration

`project-config.yaml` で以下を制御:
- `selection_criteria`: テスト対象の選定基準（純粋関数優先、最大行数、除外パターン等）
- `trm.types`: 生成するテスト要求の種別（BR/EC/BV/ER/DPから選択）
- `trm.include_audit`: TRM網羅性監査の実施有無

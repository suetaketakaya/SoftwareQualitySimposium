# TRM Pipeline - ホワイトボックステスト要求モデル自動生成ツール

GitHubリポジトリのURLを入力として、**テスト要求モデル（TRM）の自動生成**と**ホワイトボックステストコードの自動生成**を行うClaude Code マルチエージェントパイプラインです。

## 特徴

- **GitHubリンクを入れるだけ**: リポジトリURLからテスト対象の選定、TRM生成、テストコード生成までを自動実行
- **5種別のテスト要求**: 分岐網羅(BR)・同値クラス(EC)・境界値(BV)・エラーパス(ER)・依存切替(DP)で体系的に分類
- **網羅性監査付き**: LLMが生成したTRMの漏れを独立監査で検出・補完
- **トレーサビリティ**: テスト要求ID ↔ テストケースの双方向追跡
- **多言語対応**: C++, Python, Java, TypeScript, Go, Rust

## 使い方

### 前提

- [Claude Code](https://claude.ai/code) がインストール済みであること

### 手順

```bash
# 1. セットアップ（GitHubリンクを渡す）
/project:setup https://github.com/owner/repo

# 2. パイプライン実行
/project:run-pipeline
```

### 個別ステップの実行

```bash
/project:analyze          # Step 1: リポジトリ解析
/project:generate-trm     # Step 2: TRM生成
/project:audit-trm        # Step 3: 網羅性監査
/project:generate-tests   # Step 4: テストコード生成
```

## 出力物

| ディレクトリ | 内容 |
|---|---|
| `analysis/` | リポジトリ解析結果、TRM網羅性監査レポート |
| `test-requirements/` | TRM (YAML形式のテスト要求モデル) |
| `generated-tests/` | 生成されたテストコード + トレーサビリティマトリクス |
| `reports/` | パイプライン実行サマリ |

## 実証実績

sakura-editor/sakura (C++, 8関数, 約385行) での実証:
- TRM: 99件 → 網羅性監査後163件
- テスト: 248件 (C0/C1: 100%, MC/DC: 99%)
- 詳細は `sakura-editor-evaluation` ブランチを参照

## ライセンス

MIT

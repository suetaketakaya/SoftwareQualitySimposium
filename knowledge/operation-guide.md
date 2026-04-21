# TRMパイプライン運用ガイド

> **対象読者**: TRMパイプラインを別のリポジトリに流用したい / チームに導入したい人
> **前提**: Claude Code（claude.ai/code）が利用可能な環境
> **関連**: `./architecture-diagram.md`（構造図）、`/SoftwareQualitySymposium/CLAUDE.md`（コマンド一覧）

---

## 1. 何をするツールか

GitHubリポジトリのURLを入力として、以下を自動生成する:

1. **TRM (Test Requirement Model)** — YAML形式のテスト要求仕様
2. **TRM網羅性監査レポート** — 漏れている観点を追加要求として導出
3. **テストコード** — TRMに対応する単体テスト（6言語対応）
4. **トレーサビリティマトリクス** — TRM ID ↔ テストケースの対応表

v3.0でOOP拡張（クラス継承/状態変数/コードパターン）に対応済み。

---

## 2. セットアップに必要な情報

### 2.1 必須入力

| 項目 | 例 | 備考 |
|---|---|---|
| GitHubリポジトリURL | `https://github.com/owner/repo` | パブリックリポジトリ推奨。プライベートの場合は `gh auth login` 済みであること |

### 2.2 自動推定される情報

`/project:setup` 実行時、エージェントが以下を推定して `project-config.yaml` に書き込む:

| フィールド | 推定ロジック |
|---|---|
| `project.name` | URLから `owner/repo` を抽出 |
| `project.branch` | GitHubのデフォルトブランチ |
| `project.language` | 使用量が最大の言語 |
| `project.test_framework` | ビルドファイル・依存関係から推定（下表） |

| 言語 | 推定テストFW | 検出元 |
|---|---|---|
| C++ | Google Test / Catch2 | `CMakeLists.txt`, `*.vcxproj` |
| Python | pytest / unittest | `pyproject.toml`, `setup.cfg`, `conftest.py` |
| Java | JUnit / TestNG | `pom.xml`, `build.gradle` |
| TypeScript | Jest / Vitest | `package.json` の `devDependencies` |
| Go | `testing` (stdlib) | `*_test.go` の存在 |
| Rust | `cargo test` | `#[cfg(test)]` の存在 |

### 2.3 手動で確認・調整すべき項目

自動推定後、`project-config.yaml` で以下を確認する:

- `selection_criteria.max_functions` — 解析対象の関数数上限（デフォルト10）
- `selection_criteria.exclude_patterns` — ベンダー・サードパーティ・生成コードの除外
- `selection_criteria.prefer_pure_functions` — 純粋関数優先（状態持ちクラス中心なら `false`）
- `trm.types` — 生成する要求種別（BR/EC/BV/ER/DP/CI/SV/CP のサブセット可）
- `oop_analysis.enabled` — 手続き型・関数型コードが中心なら `false`
- `trm.include_audit` — 監査フェーズ省略なら `false`

---

## 3. クイックスタート

```bash
# 1. リポジトリのセットアップ（対話的）
/project:setup https://github.com/owner/repo

# 2. project-config.yaml を確認（必要に応じて編集）
#    ─ 特に selection_criteria.exclude_patterns と max_functions

# 3. パイプライン一括実行
/project:run-pipeline

# 4. 結果確認
#    ─ analysis/                 解析結果
#    ─ test-requirements/        TRM YAML
#    ─ generated-tests/          生成テスト
#    ─ reports/pipeline-summary.md  サマリ
```

評価フェーズ（実テスト実行結果の取り込み）まで進めたい場合は `/project:run-all`。

---

## 4. 成果物の構造

実行後、以下のディレクトリが生成される:

```
<repo-root>/
├── project-config.yaml           # 設定（setup で生成）
├── analysis/
│   ├── repo-analysis.md          # 関数選定結果・依存関係
│   ├── oop-analysis.md           # クラス階層・状態・パターン
│   └── trm-coverage-audit.md     # 監査結果（audit-trm で生成）
├── test-requirements/
│   ├── test-requirements.yaml    # TRM本体
│   └── test-requirement-model.md # 設計根拠・スキーマ説明
├── generated-tests/
│   ├── test-<module>-generated.<ext>   # 従来型 (BR/EC/BV/ER/DP)
│   ├── test-<module>-oop.<ext>         # OOP拡張 (CI/SV/CP)
│   ├── test-<module>-additional.<ext>  # 監査で追加された要求
│   └── traceability-matrix.md          # TRM-テスト対応表
└── reports/
    └── pipeline-summary.md       # プロジェクト情報・件数・網羅率
```

---

## 5. コマンド選択ガイド

| 状況 | 使うコマンド |
|---|---|
| 新規リポジトリの初期化 | `/project:setup <url>` |
| 全工程を通して実行したい | `/project:run-pipeline` |
| 評価レポートまで含めたい | `/project:run-all` |
| 関数選定だけやり直したい | `/project:analyze` |
| TRMだけ再生成したい | `/project:generate-trm` |
| 網羅性だけ再監査したい | `/project:audit-trm` |
| テストコードだけ再生成 | `/project:generate-tests` |
| 現在の状況を俯瞰したい | `/project:coordinator` |
| 進捗整合性を確認したい | `/project:coordinator` |

---

## 6. 他リポジトリへ流用する際のチェックリスト

### 6.1 最小限の変更箇所

- [ ] `/project:setup <新しいURL>` でベース設定を再生成
- [ ] `selection_criteria.exclude_patterns` — 以下のような sakura-editor 固有パターンが残っていないか確認
  - `tests/unit/**` 等の既存テストディレクトリを除外対象に追加（解析対象から外す）
  - ベンダー配下 (`third_party/`, `vendor/`, `node_modules/`) が含まれているか
- [ ] `oop_analysis.enabled` — 対象が手続き型ならオフにする（解析コスト削減）
- [ ] `trm.types` — 不要な種別があれば削る（例: シングルトン設計がなければ CP は縮小）

### 6.2 言語別の注意点

| 言語 | 確認項目 |
|---|---|
| C++ | マクロ・プリプロセッサの条件分岐 (`#ifdef`) は `oop_analysis.code_patterns.language_specific.cpp.macros.track_conditional_compilation` で制御 |
| Python | ABC/Mixin 多用なら `oop_analysis.class_inheritance.language_specific.python.track_mro: true` を維持。純粋関数中心なら OOP オフ |
| Java | Spring等のDIコンテナは `code_patterns.frameworks` に追加 |
| TypeScript | Reactコンポーネントを対象にする場合は状態管理（Hooks）の追跡が未対応 — カスタムロジック中心を選定 |
| Go | インターフェース実装の暗黙性のため、`class_hierarchy` の結果を手動確認 |
| Rust | trait境界・ライフタイムは現状ヒューリスティック — 解析結果を要レビュー |

### 6.3 大規模リポジトリでの工夫

- `max_functions` を段階的に増やす（10 → 30 → 50）
- サブディレクトリ単位で `project-config.yaml` を分けて実行
- `selection_criteria.max_lines_per_function` で超長関数を除外（リファクタ対象を炙り出す用途には活用可）

---

## 7. 既知の制約

| 制約 | 回避策 |
|---|---|
| 期待値はLLM推論に委ねず実装から導出する | `generate-tests` 後に `traceability-matrix.md` で期待値根拠を必ずレビュー |
| プライベートリポジトリは `gh` 認証必須 | `gh auth login` 後に実行 |
| 巨大リポジトリは解析が長時間化 | `exclude_patterns` で範囲を絞る / `max_functions` を下げる |
| 生成テストが初回100%通るわけではない | 初回PASS率は実証で91.7%（sakura-editor）。残る失敗は実装差異として個別調整 |
| マクロ展開依存のC++コードは分岐カウントにブレ | `oop_analysis.code_patterns.language_specific.cpp.macros.track_macro_expansion: true` で展開を追跡 |

---

## 8. 運用上のベストプラクティス

1. **設定はバージョン管理する** — `project-config.yaml` はコミット対象。チーム内で設定を共有
2. **監査は必ず通す** — `trm.include_audit: true` を維持。追加要求の質がTRM品質を左右する
3. **生成テストは必ずレビュー** — 期待値はLLM推論に依存し得るため、最初の1周はケースを1件ずつ確認
4. **トレーサビリティマトリクスを成果物の一級市民に** — テスト変更時はTRM IDと突き合わせて整合を保つ
5. **OOP解析の粒度を段階投入** — 初回は `enabled: false` で動かし、基本フロー確認後に CI/SV/CP を順次オン

---

## 9. トラブルシュート

| 症状 | 原因の可能性 | 対処 |
|---|---|---|
| 関数が1件も選定されない | `exclude_patterns` が広すぎ / `max_lines_per_function` が小さすぎ | パターン縮小・上限緩和 |
| TRMが極端に少ない | `trm.types` のサブセット指定を失念 | 8種別全部を含めて再生成 |
| 生成テストがコンパイルエラー | `test_framework` 推定が誤り | `project-config.yaml` を手動修正して `generate-tests` 再実行 |
| OOP解析結果が空 | `oop_analysis.enabled: false` になっている | `true` にして `analyze` 再実行 |
| 監査でGAPが大量発生 | 初回の `generate-trm` がシンプルすぎる | `max_functions` を縮小し、1関数ずつ丁寧に生成 |

---

## 10. 関連ドキュメント

- `knowledge/architecture-diagram.md` — システム構造のクラス図
- `knowledge/direction.md` — 研究方向性・投稿戦略
- `knowledge/revised-framing.md` — 問題設定の再定義
- `CLAUDE.md`（プロジェクトルート） — コマンドの完全なリファレンス
- `templates/trm-schema.yaml` — TRMデータ形式の正式仕様
- `templates/oop-analysis-schema.yaml` — OOP解析結果の正式仕様
- `templates/project-config.yaml` — 設定ファイルのテンプレート

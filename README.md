# TRM Pipeline — テスト要求モデルの自動生成と可視化

ホワイトボックス層の品質情報を「人間がレビュー可能な中間成果物（TRM）」として固定し、LLM 駆動でテストコードと非エンジニア向け可視化まで一貫して生成するパイプライン、およびその実証研究リポジトリです。

## 研究の問い

LLM を先行させた AI 駆動開発が広がる中、実装領域のホワイトボックス層の保守が弱まり、品質情報がコード内部に閉じ込められる「可読性ギャップ」が生じている。本研究はコードのみを入力として次の2点の妥当性を問う:

1. **十分性**: ホワイトボックス評価で **確認対象領域の抜け漏れ** を減らせるか
2. **妥当性**: **非エンジニアにも伝わる形** で品質情報を共有できるか

## パイプラインの概要

```
GitHub URL
    ↓ setup
project-config.yaml
    ↓ analyze
repo-analysis.md + oop-analysis.md (継承・状態・パターン・カプセル化)
    ↓ generate-trm
test-requirements.yaml (8種別)
    ↓ audit-trm
trm-coverage-audit.md
    ↓ generate-tests
test-*.ext (言語別)
    ↓ generate_visualizations.py
Sunburst / Sankey / Heatmap / Chord (非エンジニア向け)
    ↓ classify_readability.py
可読性レベル (L1/L2/L3) ごとの集計
```

## TRM スキーマ v3.1 — 9種別

**従来5種別（関数レベル）:**

| 記号 | 名称 | 内容 |
|---|---|---|
| BR | 分岐網羅 | if/else/switch の各パスを検証 |
| EC | 同値クラス | 入力の値域を同値分割して検証 |
| BV | 境界値 | 同値クラス境界の上下限を検証 |
| ER | エラーパス | 異常系・エラー処理を検証 |
| DP | 依存切替 | 関数間の依存・差異を検証 |

**OOP 拡張 4種別（v3.0〜v3.1）:**

| 記号 | 名称 | 内容 |
|---|---|---|
| CI | クラス継承 | 多態性・オーバーライド・LSP・super委譲 |
| SV | 状態変数 | 初期化・変異・不変条件・ライフサイクル + メンバ宣言妥当性 (v3.1) |
| CP | コードパターン | デザインパターン・イディオム・リソース管理 |
| **EN** | **カプセル化** (v3.1 新設) | アクセス制御・漏洩アクセサ・不変性契約・構築契約・不変条件サーフェス |

詳細: `templates/trm-schema.yaml`, `knowledge/architecture-diagram.md`

## 実証結果（2026-04-21 時点）

| 対象 | 言語 | 設計 | 要求数 | 可読率 | 特記事項 |
|---|---|---|---|---|---|
| sakura-editor/sakura (8関数) | C++ | 手続き型 | 99 | 65.7% (手動) | 初回PASS率 91.7% → 修正後 100% |
| sakura CMemoryIterator | C++ | OOP | 48 | — | v3.1 遡及、EN 8件検出 |
| pallets/click (8 target) | Python | OOP | 173 | 24.3% (自動v2) | EN 21件、既存テストGAP 86.7% |
| 自作リバーシ | Python | 関数 | 46 | 97.8% (自動v2) | survey題材 |

## ディレクトリ構成

```
├── CLAUDE.md                # Claude Code 向けプロジェクト指示
├── README.md                # 本書
├── .claude/commands/        # パイプラインエージェント
├── templates/               # スキーマ・設定テンプレート
│   ├── trm-schema.yaml      #   TRM v3.1 (9種別)
│   ├── project-config.yaml  #   プロジェクト設定
│   └── oop-analysis-schema.yaml
├── scripts/                 # 自動化スクリプト
│   ├── generate_visualizations.py  # 4種図式の自動生成
│   ├── classify_readability.py     # L1/L2/L3 自動分類 (v2辞書)
│   ├── generate_submission_v6.py   # Markdown→docx 変換
│   └── create_survey_form.gs       # Google Forms 自動生成
├── knowledge/               # 研究方針・設計ノート
│   ├── paper-draft-full.md         # 論文完全ドラフト (17,000字)
│   ├── paper-draft-outline.md      # 論文章立て
│   ├── paper-experiment-section.md # §4 実験設計
│   ├── paper-figures-plan.md       # 図版プラン (9図)
│   ├── trm-evaluation-criteria.md  # 評価軸 (A/B/C/D)
│   ├── complex-scope-approaches.md # 複雑範囲の5パターン
│   ├── candidate-repositories.md   # 検証対象候補
│   ├── survey-design.md            # アンケート設計
│   ├── survey-deployment-plan.md   # アンケート運用プラン
│   ├── operation-guide.md          # 流用手順
│   └── architecture-diagram.md     # Mermaidクラス図・シーケンス図
├── experiments/             # 実証実験
│   ├── click/               #   pallets/click 実証 (TRM, 可視化, 比較)
│   ├── reversi/             #   リバーシ合成題材
│   ├── sakura-oop-extension/ #  CMemoryIterator 追加実証
│   ├── visualizations/      #   PoC トリガー可視化 (3対象)
│   ├── evaluation-report.md #   sakura 実証の実験評価
│   └── additional-analysis.md #  sakura 可読性 3レベル分類 (手動)
├── analysis/                # sakura 既存実証の解析結果
├── test-requirements/       # sakura 既存 TRM
├── test-design/             # sakura テスト設計書
├── generated-tests/         # sakura 生成テスト
├── drafts/                  # 論文草稿 (v3 関連、既提出分)
└── report/                  # 投稿用 docx
    ├── submission_final_2026.docx  # SQiP 2026 提出版 (v3.0時点)
    └── submission_v6_2026.docx     # 次稿ドラフト (v3.1時点)
```

## クイックスタート

### 新規対象への適用

```bash
# 1. 対象リポジトリのセットアップ
/project:setup https://github.com/owner/repo

# 2. パイプライン一括実行
/project:run-pipeline

# 3. 可視化レイヤ生成
python scripts/generate_visualizations.py \
    --trm test-requirements/test-requirements.yaml \
    --output visualizations-auto/

# 4. 可読性分類
python scripts/classify_readability.py \
    --trm test-requirements/test-requirements.yaml \
    --output reports/readability-classification.md
```

### 論文 docx 生成

```bash
python scripts/generate_submission_v6.py \
    --input knowledge/paper-draft-full.md \
    --output report/submission_v6_2026.docx
```

## 対応言語

| 言語 | テストFW | OOP対応 |
|---|---|---|
| C++ | Google Test / Catch2 | virtual, 多重継承, RAII, CRTP, const |
| Python | pytest | ABC, MRO, Mixin, デコレータ, context manager |
| Java | JUnit | インターフェース, abstract, DI |
| TypeScript | Jest / Vitest | interface, 判別共用体, 型ガード |
| Go | testing | インターフェース, 埋め込み構造体 |
| Rust | cargo test | トレイト, ジェネリクス, ライフタイム |

## 研究進捗

| 項目 | 状態 |
|---|---|
| SQiP 2026 提出 | 完了 (2026-04-14) |
| 次稿論文ドラフト (§1〜§8) | 初稿完成 (`paper-draft-full.md`) |
| click 実証 (173要求・EN 21件) | 完了 |
| sakura CMemoryIterator 追実証 | 完了 |
| 可視化レイヤ実装 | 完了 |
| 可読性自動分類器 v2 | 完了 (手動一致率 98%) |
| 論文 docx 生成 (v6) | 完了 |
| アンケート実施 | 準備完了、展開は2週間以内に予定 |

## 匿名査読に関する注意

`drafts/`, `report/`, `knowledge/paper-draft-full.md` 等の投稿関連ドキュメントでは、著者名・所属・個人特定情報を本文やファイル名に含めません（SQiP 匿名査読要件に準拠）。

## ライセンス・取り扱い

- 本リポジトリは研究用メタリポジトリ
- 実証対象 `sakura-editor/sakura` は公式リポジトリから取得し各自のライセンスに従う
- 実証対象 `pallets/click` は BSD-3-Clause。`experiments/click/src/` の参照用スナップショットも同ライセンス

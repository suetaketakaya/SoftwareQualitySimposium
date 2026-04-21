# knowledge/ — 研究・運用ナレッジ

次稿論文（TRM v3.1 + EN種別 + 可視化レイヤ）および関連研究プロジェクトの調査・設計・運用ドキュメント集約先。

## 論文執筆（最新・優先参照）

| ファイル | 用途 |
|---|---|
| [paper-draft-full.md](./paper-draft-full.md) | **論文完全ドラフト** §1〜§8 + 参考文献30件 + 付録。約17,000字 |
| [paper-draft-outline.md](./paper-draft-outline.md) | 論文章立て・Abstract 草案・タイトル案・投稿チェックリスト |
| [paper-experiment-section.md](./paper-experiment-section.md) | 実験設計 §4 の詳細（対象選定方針・click実証・sakura遡及） |
| [paper-figures-plan.md](./paper-figures-plan.md) | 必須9図の配置・ソース・生成手順（Mermaid/matplotlib） |
| [survey-design.md](./survey-design.md) | 非エンジニア向けTRM可読性アンケート完成形（25問・リバーシ題材） |
| [survey-deployment-plan.md](./survey-deployment-plan.md) | アンケート展開プラン（パイロット・本展開・分析・統計検定） |

## 運用ドキュメント

| ファイル | 用途 | 対象読者 |
|---|---|---|
| [operation-guide.md](./operation-guide.md) | パイプラインを他リポジトリへ流用する手順・設定項目・チェックリスト | 導入担当・新規ユーザー |
| [architecture-diagram.md](./architecture-diagram.md) | エージェント・スキーマ・成果物の構造をMermaid クラス図・シーケンス図で可視化（v3.1対応） | 内部構造を把握したい人・拡張担当 |

## 汎用化・拡張検討

| ファイル | 用途 |
|---|---|
| [trm-evaluation-criteria.md](./trm-evaluation-criteria.md) | TRM評価軸（A.網羅性 / B.可読性 / C.検証性 / D.保守性）の汎用定義。v3.1 OOP/EN指標を含む |
| [complex-scope-approaches.md](./complex-scope-approaches.md) | 複雑化範囲へのTRMアプローチ5パターン（階層化・スライス分割・シナリオ駆動・視覚化・段階監査） |
| [candidate-repositories.md](./candidate-repositories.md) | sakura-editor以外の検証対象候補。確定ショートリスト: sakura + click + zod(任意) + リバーシ |

## 研究・投稿（旧版・背景資料）

| ファイル | 用途 |
|---|---|
| [direction.md](./direction.md) | 投稿戦略・エージェント命令の初期設計（SQiP 2026版） |
| [revised-framing.md](./revised-framing.md) | 「品質情報の可読性ギャップ」への問題設定刷新 |
| [literature-review.md](./literature-review.md) | 関連研究サーベイ（36件） |
| [additional-references.md](./additional-references.md) | 追加参考文献 |

## 参照関係（v3.1時点）

```
[論文執筆レイヤ]
paper-draft-full.md ◀──── paper-draft-outline.md （骨組み）
    │
    ├── §4 実験設計 ◀─── paper-experiment-section.md
    ├── §4.3 アンケート ◀─── survey-design.md ◀── survey-deployment-plan.md (運用)
    ├── §5 評価指標 ◀─── trm-evaluation-criteria.md
    ├── §6 結果 ◀──── experiments/ (click, reversi, sakura-oop-extension)
    └── §3 図版 ◀──── paper-figures-plan.md → architecture-diagram.md

[運用レイヤ]
operation-guide.md ───▶ templates/ (trm-schema v3.1, project-config, oop-analysis-schema)
         │
         └─▶ architecture-diagram.md (構造)

[設計拡張レイヤ]
trm-evaluation-criteria.md ──┬──▶ complex-scope-approaches.md (可視化パターン)
                              │
                              └──▶ candidate-repositories.md (適用対象)
```

## ドキュメント更新ガイド

| 変更内容 | 更新先 |
|---|---|
| 論文本文の内容変更 | `paper-draft-full.md` → 必要なら `generate_submission_v6.py` で docx 再生成 |
| 実験設計の詳細変更 | `paper-experiment-section.md` |
| 運用手順（設定項目・CLI） | `operation-guide.md` |
| エージェント責務・スキーマ | `architecture-diagram.md` + `templates/*.yaml` |
| 評価軸・指標定義 | `trm-evaluation-criteria.md` |
| 複雑範囲パターン追加 | `complex-scope-approaches.md` |
| 候補リポジトリ増減 | `candidate-repositories.md` |
| アンケート質問改訂 | `survey-design.md` + `survey-deployment-plan.md` + `scripts/create_survey_form.gs` |

**スキーマバージョン更新時の波及**:
- `templates/trm-schema.yaml` → `CLAUDE.md`, `README.md`, `architecture-diagram.md`, `operation-guide.md`, `paper-draft-full.md`
- `templates/project-config.yaml` → `operation-guide.md`, `paper-experiment-section.md`

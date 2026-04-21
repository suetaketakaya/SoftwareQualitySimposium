# knowledge/ — 研究・運用ナレッジ

SQiP 2026投稿プロジェクトに関する調査・設計・運用ドキュメントの集約先。

## 運用ドキュメント (input.md ④⑤)

| ファイル | 用途 | 対象読者 |
|---|---|---|
| [operation-guide.md](./operation-guide.md) | パイプラインを他リポジトリへ流用する手順・設定項目・チェックリスト | 導入担当・新規ユーザー |
| [architecture-diagram.md](./architecture-diagram.md) | エージェント・スキーマ・成果物の構造をMermaidクラス図・シーケンス図で可視化 | 内部構造を把握したい人・拡張担当 |

## 汎用化・拡張検討 (input.md ①②③)

| ファイル | 対応タスク | 用途 |
|---|---|---|
| [trm-evaluation-criteria.md](./trm-evaluation-criteria.md) | ① | TRM評価軸（網羅性・可読性・検証性・保守性）の汎用定義。sakura-editorの実証値を参照値として記録 |
| [complex-scope-approaches.md](./complex-scope-approaches.md) | ② | 複雑化範囲へのTRMアプローチ5パターン（階層化・スライス分割・シナリオ駆動・視覚化・段階監査） |
| [candidate-repositories.md](./candidate-repositories.md) | ③ | sakura-editor以外の検証対象候補リポジトリ。§7に確定ショートリスト（sakura + click + zod + リバーシ） |

## 次稿論文 執筆支援

| ファイル | 用途 |
|---|---|
| [paper-experiment-section.md](./paper-experiment-section.md) | 論文「4. 実験設計」章の段落ドラフト（対象選定方針・選定結果・非エンジニア検証方針） |
| [survey-design.md](./survey-design.md) | 非エンジニア向けTRM可読性アンケート完成形（リバーシ80行サンプル + 3パターン提示 + 分析計画） |

## 研究・投稿

| ファイル | 用途 |
|---|---|
| [direction.md](./direction.md) | 投稿戦略・エージェント命令の初期設計 |
| [revised-framing.md](./revised-framing.md) | 「品質情報の可読性ギャップ」への問題設定刷新 |
| [literature-review.md](./literature-review.md) | 関連研究サーベイ |
| [additional-references.md](./additional-references.md) | 追加参考文献 |

## 参照関係

```
[運用]                                    [拡張検討]
operation-guide.md ────┐              trm-evaluation-criteria.md ◀── experiments/
         │             │                        │
         └─参照─▶ architecture-diagram.md       │
                       │                        ├─▶ complex-scope-approaches.md
                       │                        │      (ここで評価軸のカテゴリBを深掘り)
                       │                        │
                       └──▶ templates/          └─▶ candidate-repositories.md
                            trm-schema.yaml            (評価軸を適用する対象)
                            oop-analysis-schema.yaml
                            project-config.yaml
```

## ドキュメント更新ガイド

- **運用上の変更（手順・設定項目）** → `operation-guide.md` を更新
- **構造の変更（エージェント責務・スキーマ）** → `architecture-diagram.md` を更新
- **評価軸の追加・修正** → `trm-evaluation-criteria.md` を更新
- **複雑範囲対応のパターン追加** → `complex-scope-approaches.md` を更新
- **候補リポジトリの追加・除外** → `candidate-repositories.md` を更新
- スキーマバージョン更新時は、operation-guide.md と architecture-diagram.md を同時メンテナンス

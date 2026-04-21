# pallets/click 実験（第2実証）

> **位置づけ**: sakura-editor (C++) に続く第2実証対象。動的型Pythonでの手法汎用性検証
> **開始日**: 2026-04-21
> **関連**: `../../knowledge/candidate-repositories.md` §7、`../../knowledge/paper-experiment-section.md` §4.2

## 対象の基本情報

| 項目 | 値 |
|---|---|
| リポジトリ | https://github.com/pallets/click |
| 言語 | Python |
| テストFW | pytest |
| ライセンス | BSD-3-Clause |
| デフォルトブランチ | main |
| 規模 | 約4.7MB / 約20ファイル (src/click配下) |
| 最終更新 | 2026-04-17 時点で活発 |
| スター数 | 17,000 以上 |

## 重点モジュール

| モジュール | 行数目安 | 選定理由 |
|---|---|---|
| `src/click/types.py` | 約1300行 | `ParamType` 階層（`StringParamType`, `IntParamType`, `Choice`, `DateTime`, `Path`, ...）でCI/SVを検証 |
| `src/click/decorators.py` | 約600行 | `@command`, `@option`, `@argument` のデコレータパターンでCPを検証 |
| `src/click/exceptions.py` | 約300行 | `ClickException` → `UsageError` → `BadParameter` の例外階層でCIの補足検証 |

## 実施手順

```bash
# 1. クローン（必要に応じて）
git clone https://github.com/pallets/click /tmp/click

# 2. 設定確認
cat experiments/click/project-config.yaml

# 3. 解析（project-config.yaml を参照）
/project:analyze  # experiments/click/analysis/ に出力

# 4. TRM生成
/project:generate-trm  # experiments/click/test-requirements/ に出力

# 5. 監査
/project:audit-trm

# 6. テスト生成
/project:generate-tests  # experiments/click/generated-tests/ に出力

# 7. 評価
/project:experiment-eval  # experiments/click/reports/ に出力
```

**注**: この実験では `project-config.yaml` の `output.*` が `experiments/click/` 配下を指しているため、リポジトリルートの sakura-editor の成果物と分離される。

## 期待される要求数（事前見積）

| 対象 | BR | EC | BV | ER | DP | CI | SV | CP |
|---|---|---|---|---|---|---|---|---|
| `types.py ParamType.convert` 階層 | 40 | 25 | 15 | 10 | 5 | 25 | 10 | 5 |
| `decorators.py` | 15 | 10 | 5 | 5 | 10 | 5 | 0 | 15 |
| `exceptions.py` | 10 | 8 | 0 | 5 | 3 | 12 | 0 | 0 |
| **合計見積** | **65** | **43** | **20** | **20** | **18** | **42** | **10** | **20** |

**sakura-editor比較**: sakura-editor の総要求数99に対し、click見積 **238** （約2.4倍）。OOP要素のCI/SV/CPが加算されるためである。

## 評価計画

`../../knowledge/trm-evaluation-criteria.md` のカテゴリ A + B + C を適用:
- A: 網羅性（要求数、カバー率、種別内訳、OOP指標）
- B: 可読性（3レベル分類、可読率）
- C: 検証性（初回PASS率、期待値根拠トレース）

評価結果は `experiments/click/reports/evaluation-report.md` として保存する。

## 進捗

| フェーズ | 状態 | 担当エージェント | 成果物 |
|---|---|---|---|
| セットアップ | **完了** (2026-04-21) | `setup`（手動） | `project-config.yaml`, `README.md` |
| リポジトリ解析（types.py スコープ） | **完了** (2026-04-21) | `analyze` 相当 | `analysis/repo-analysis.md` |
| TRM生成（types.py スコープ） | **完了** (2026-04-21) | `generate-trm` 相当 | `test-requirements/test-requirements.yaml` (131件) |
| サマリ作成 | **完了** (2026-04-21) | 手動 | `reports/pipeline-summary.md` |
| 網羅性監査 | 未実施 | `/project:audit-trm` | |
| テスト生成 | 未実施 | `/project:generate-tests` | |
| 実験評価 | 未実施 | `/project:experiment-eval` | |
| types.py以外への拡張 | 未実施 | 同上 | decorators.py / exceptions.py |

## 初回実施結果（2026-04-21）

- **スコープ**: types.py の ParamType 階層 5クラス（全19クラス中）
- **要求数**: 131件（BR37/EC18/BV11/ER9/DP5/CI18/SV9/CP9/**EN15**）
- **v3.1 初実戦**: EN 15件を導出 — Choice の public mutable state、IntRange のダイヤモンド継承を含む
- **sakura-editor 比較**: 同規模コード（~400行）で要求数 +32%。動的型Python + OOP 設計の意義を定量化
- 詳細は `reports/pipeline-summary.md` 参照

## 留意点

- `src/click/core.py` (約130KB) は巨大すぎるため初回解析対象から除外した。後続で部分的に採り入れるか判断する。
- click は `Context` オブジェクトによる状態管理が強い。SV（状態変数）観点の検証で v3.0 OOP拡張の有効性が顕在化する見込み。
- click 自体がフレームワーク（デコレータベースのCLI構築）であるため、利用者視点のシナリオ駆動型TRMの副次検討材料にもなり得る。

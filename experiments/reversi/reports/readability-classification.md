# 可読性自動分類レポート — reversi-sample

> **対象**: 46件の要求
> **判定**: ルールベース分類（`scripts/classify_readability.py`）
> **準拠**: `knowledge/trm-evaluation-criteria.md` §3.1

---

## 1. 全体サマリ

| Level | 件数 | 割合 |
|---|---|---|
| L1 (コード知識不要) | 27 | 58.7% |
| L2 (ドメイン知識のみ) | 18 | 39.1% |
| L3 (コード知識必要) | 1 | 2.2% |
| **合計** | **46** | 100% |

**可読率 (L1+L2): 97.8%**

## 2. 種別別可読性

| 種別 | 件数 | L1 | L2 | L3 | 可読率 (L1+L2) |
|---|---|---|---|---|---|
| boundary_value | 9 | 2 | 7 | 0 | 100.0% |
| branch_coverage | 20 | 15 | 5 | 0 | 100.0% |
| dependency_path | 4 | 3 | 1 | 0 | 100.0% |
| equivalence_class | 8 | 5 | 3 | 0 | 100.0% |
| error_path | 5 | 2 | 2 | 1 | 80.0% |

## 3. ターゲット別可読性

| TGT | 対象 | 件数 | L1 | L2 | L3 | 可読率 |
|---|---|---|---|---|---|---|
| TGT-01 | is_on_board(x: int, y: int) -> | 10 | 4 | 6 | 0 | 100.0% |
| TGT-02 | is_empty(board, x: int, y: int | 5 | 4 | 0 | 1 | 80.0% |
| TGT-03 | count_flippable(board, x, y, d | 15 | 12 | 3 | 0 | 100.0% |
| TGT-04 | is_valid_move(board, x, y, me) | 16 | 7 | 9 | 0 | 100.0% |

## 4. 分類結果サンプル

### L1 の例

- `BR-01-01` (branch_coverage): 全原子条件が真のとき True を返すことを検証する 
- `BR-01-04` (branch_coverage): y が負のとき False を返すこと 
- `BR-01-05` (branch_coverage): y が BOARD_SIZE 以上のとき False を返すこと 

### L2 の例

- `BR-01-02` (branch_coverage): x が負のとき False を返すこと（下限違反） （L2キーワード: 下限）
- `BR-01-03` (branch_coverage): x が BOARD_SIZE 以上のとき False を返すこと（上限違反） （L2キーワード: 上限）
- `BV-01-01` (boundary_value): x の下限境界 0 / -1 で判定が正しく切り替わること （L2キーワード: 下限）

### L3 の例

- `ER-02-01` (error_path): 前提違反（x, y が範囲外）で IndexError を発生させることを観察する （L3キーワード: IndexError）

## 5. 参照値との比較

| 対象 | 可読率 | 出典 |
|---|---|---|
| reversi-sample (本分析) | 97.8% | 自動分類 |
| sakura-editor (手動分類) | 65.7% | experiments/additional-analysis.md |
| 目標: 成熟設計・ドメイン主導 | ≥ 75% | trm-evaluation-criteria.md §3.4 |

## 6. 留意点

- 本分類は **ルールベースのヒューリスティック**。LLM分類や手動分類との整合性検証は別途必要。
- キーワード辞書は `scripts/classify_readability.py` の `L2_KEYWORDS` / `L3_KEYWORDS` で拡張可能。
- 「誤分類」が疑われるサンプルは手動確認のうえ辞書を更新する運用を推奨。

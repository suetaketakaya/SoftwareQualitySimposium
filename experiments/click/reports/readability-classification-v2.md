# 可読性自動分類レポート — pallets/click

> **対象**: 173件の要求
> **判定**: ルールベース分類（`scripts/classify_readability.py`）
> **準拠**: `knowledge/trm-evaluation-criteria.md` §3.1

---

## 1. 全体サマリ

| Level | 件数 | 割合 |
|---|---|---|
| L1 (コード知識不要) | 13 | 7.5% |
| L2 (ドメイン知識のみ) | 29 | 16.8% |
| L3 (コード知識必要) | 131 | 75.7% |
| **合計** | **173** | 100% |

**可読率 (L1+L2): 24.3%**

## 2. 種別別可読性

| 種別 | 件数 | L1 | L2 | L3 | 可読率 (L1+L2) |
|---|---|---|---|---|---|
| boundary_value | 11 | 1 | 8 | 2 | 81.8% |
| branch_coverage | 47 | 3 | 11 | 33 | 29.8% |
| class_inheritance | 24 | 0 | 2 | 22 | 8.3% |
| code_pattern | 15 | 2 | 2 | 11 | 26.7% |
| dependency_path | 7 | 0 | 0 | 7 | 0.0% |
| encapsulation | 21 | 0 | 2 | 19 | 9.5% |
| equivalence_class | 22 | 4 | 3 | 15 | 31.8% |
| error_path | 13 | 0 | 0 | 13 | 0.0% |
| state_variable | 13 | 3 | 1 | 9 | 30.8% |

## 3. ターゲット別可読性

| TGT | 対象 | 件数 | L1 | L2 | L3 | 可読率 |
|---|---|---|---|---|---|---|
| TGT-01 | ParamType | 19 | 0 | 2 | 17 | 10.5% |
| TGT-02 | StringParamType | 17 | 1 | 3 | 13 | 23.5% |
| TGT-03 | Choice | 36 | 5 | 5 | 26 | 27.8% |
| TGT-04 | _NumberParamTypeBase | 11 | 0 | 0 | 11 | 0.0% |
| TGT-05 | IntRange | 48 | 4 | 18 | 26 | 45.8% |
| TGT-06 | ClickException | 12 | 2 | 0 | 10 | 16.7% |
| TGT-07 | BadParameter, MissingParameter | 17 | 0 | 1 | 16 | 5.9% |
| TGT-08 | make_pass_decorator(object_typ | 13 | 1 | 0 | 12 | 7.7% |

## 4. 分類結果サンプル

### L1 の例

- `EC-02-03` (equivalence_class): argv encoding と fs encoding が同一の場合と異なる場合 
- `BR-03-09` (branch_coverage): required argument のとき {} 括弧 
- `BR-03-10` (branch_coverage): 非必須または option のとき [] 括弧 

### L2 の例

- `CI-01-03` (class_inheritance): 派生クラスの name 属性がクラス特有の値に設定されていること （L2キーワード: 派生）
- `CP-01-01` (code_pattern): Template Method パターン: 基底の既定動作と派生の上書きが共存できること （L2キーワード: 派生, 上書き, Template Method）
- `BR-02-03` (branch_coverage): argv encoding 失敗時に fs encoding にフォールバックすること （L2キーワード: フォールバック）

### L3 の例

- `BR-01-01` (branch_coverage): __call__(None) が convert を呼ばずに None を返すこと （L3キーワード: None, __call__）
- `BR-01-02` (branch_coverage): __call__(非None値) が convert を呼び出すこと （L3キーワード: None, __call__）
- `BR-01-03` (branch_coverage): to_info_dict で name 属性がある場合に self.name が使われること （L3キーワード: to_info_dict）

## 5. 参照値との比較

| 対象 | 可読率 | 出典 |
|---|---|---|
| pallets/click (本分析) | 24.3% | 自動分類 |
| sakura-editor (手動分類) | 65.7% | experiments/additional-analysis.md |
| 目標: 成熟設計・ドメイン主導 | ≥ 75% | trm-evaluation-criteria.md §3.4 |

## 6. 留意点

- 本分類は **ルールベースのヒューリスティック**。LLM分類や手動分類との整合性検証は別途必要。
- キーワード辞書は `scripts/classify_readability.py` の `L2_KEYWORDS` / `L3_KEYWORDS` で拡張可能。
- 「誤分類」が疑われるサンプルは手動確認のうえ辞書を更新する運用を推奨。

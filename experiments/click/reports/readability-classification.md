# 可読性自動分類レポート — pallets/click

> **対象**: 173件の要求
> **判定**: ルールベース分類（`scripts/classify_readability.py`）
> **準拠**: `knowledge/trm-evaluation-criteria.md` §3.1

---

## 1. 全体サマリ

| Level | 件数 | 割合 |
|---|---|---|
| L1 (コード知識不要) | 65 | 37.6% |
| L2 (ドメイン知識のみ) | 14 | 8.1% |
| L3 (コード知識必要) | 94 | 54.3% |
| **合計** | **173** | 100% |

**可読率 (L1+L2): 45.7%**

## 2. 種別別可読性

| 種別 | 件数 | L1 | L2 | L3 | 可読率 (L1+L2) |
|---|---|---|---|---|---|
| boundary_value | 11 | 7 | 2 | 2 | 81.8% |
| branch_coverage | 47 | 21 | 4 | 22 | 53.2% |
| class_inheritance | 24 | 12 | 2 | 10 | 58.3% |
| code_pattern | 15 | 8 | 0 | 7 | 53.3% |
| dependency_path | 7 | 3 | 0 | 4 | 42.9% |
| encapsulation | 21 | 1 | 2 | 18 | 14.3% |
| equivalence_class | 22 | 8 | 2 | 12 | 45.5% |
| error_path | 13 | 0 | 1 | 12 | 7.7% |
| state_variable | 13 | 5 | 1 | 7 | 46.2% |

## 3. ターゲット別可読性

| TGT | 対象 | 件数 | L1 | L2 | L3 | 可読率 |
|---|---|---|---|---|---|---|
| TGT-01 | ParamType | 19 | 8 | 0 | 11 | 42.1% |
| TGT-02 | StringParamType | 17 | 5 | 2 | 10 | 41.2% |
| TGT-03 | Choice | 36 | 11 | 5 | 20 | 44.4% |
| TGT-04 | _NumberParamTypeBase | 11 | 4 | 0 | 7 | 36.4% |
| TGT-05 | IntRange | 48 | 24 | 5 | 19 | 60.4% |
| TGT-06 | ClickException | 12 | 3 | 2 | 7 | 41.7% |
| TGT-07 | BadParameter, MissingParameter | 17 | 3 | 0 | 14 | 17.6% |
| TGT-08 | make_pass_decorator(object_typ | 13 | 7 | 0 | 6 | 53.8% |

## 4. 分類結果サンプル

### L1 の例

- `BR-01-03` (branch_coverage): to_info_dict で name 属性がある場合に self.name が使われること 
- `BR-01-04` (branch_coverage): to_info_dict で name 属性がない場合に param_type が利用されること 
- `EC-01-01` (equivalence_class): convert の既定動作が identity（受け取った値をそのまま返す）であること 

### L2 の例

- `BR-02-05` (branch_coverage): fs_enc デコードも失敗した場合に utf-8 replace で強制デコードされること （L2キーワード: デコード）
- `BR-02-06` (branch_coverage): 正常な argv encoding でのデコード成功時に即座に値が返されること （L2キーワード: デコード）
- `BR-03-01` (branch_coverage): Enum 型 choice が choice.name で正規化されること （L2キーワード: 正規化）

### L3 の例

- `BR-01-01` (branch_coverage): __call__(None) が convert を呼ばずに None を返すこと （L3キーワード: None, __call__）
- `BR-01-02` (branch_coverage): __call__(非None値) が convert を呼び出すこと （L3キーワード: None, __call__）
- `BR-01-05` (branch_coverage): fail メソッドが BadParameter を確実に投げること （L3キーワード: BadParameter, メソッド）

## 5. 参照値との比較

| 対象 | 可読率 | 出典 |
|---|---|---|
| pallets/click (本分析) | 45.7% | 自動分類 |
| sakura-editor (手動分類) | 65.7% | experiments/additional-analysis.md |
| 目標: 成熟設計・ドメイン主導 | ≥ 75% | trm-evaluation-criteria.md §3.4 |

## 6. 留意点

- 本分類は **ルールベースのヒューリスティック**。LLM分類や手動分類との整合性検証は別途必要。
- キーワード辞書は `scripts/classify_readability.py` の `L2_KEYWORDS` / `L3_KEYWORDS` で拡張可能。
- 「誤分類」が疑われるサンプルは手動確認のうえ辞書を更新する運用を推奨。

# click 既存テスト vs 生成TRM 網羅比較

> **対象**: `tests/test_types.py`（257行）と生成TRM（TGT-01〜TGT-08、173件）の対応
> **実施日**: 2026-04-21
> **関連**: `../test-requirements/test-requirements.yaml`、`trm-evaluation-criteria.md` §A-3

---

## 1. 既存テストの関数一覧（11件）

| # | 関数名 | parametrize件数 | 対象 |
|---|---|---|---|
| 1 | `test_range` | 14 | IntRange, FloatRange の convert 成功系 |
| 2 | `test_range_fail` | 7 | IntRange, FloatRange の convert 失敗系 |
| 3 | `test_float_range_no_clamp_open` | 1 | FloatRange の clamp+open_bound 禁止契約 |
| 4 | `test_cast_multi_default` | 7 | Argument cast（本TRMスコープ外） |
| 5 | `test_path_type` | - | Path（本TRMスコープ外） |
| 6 | `test_path_resolve_symlink` | 1 | Path（本TRMスコープ外） |
| 7 | `test_path_surrogates` | 1 | Path（本TRMスコープ外） |
| 8 | `test_file_surrogates` | - | File（本TRMスコープ外） |
| 9 | `test_file_error_surrogates` | 1 | FileError（本TRMスコープ外） |
| 10 | `test_invalid_path_with_esc_sequence` | 1 | Path / BadParameter の連携 |
| 11 | `test_choice_get_invalid_choice_message` | 1 | **Choice.get_invalid_choice_message** |

**本TRMスコープに対応する既存テスト**: `test_range` / `test_range_fail` / `test_float_range_no_clamp_open` / `test_choice_get_invalid_choice_message` / `test_invalid_path_with_esc_sequence`（一部）

## 2. TRM → 既存テスト マッピング（本TRMスコープ内）

| TRM ID | 要求内容 | 対応既存テスト | 状態 |
|---|---|---|---|
| **IntRange (TGT-05)** | | | |
| BR-05-02 | min_open=False で value==min 合格 | test_range (IntRange(0,5), "0") 欠落 | **GAP** |
| BR-05-03 | min_open=True で value==min fail | test_range_fail (IntRange(0,5,min_open=True), 0) | ✓ |
| BR-05-05 | max_open=False で value==max 合格 | test_range (IntRange(5), "5") | ✓ |
| BR-05-06 | max_open=True で value==max fail | test_range_fail (IntRange(0,5,max_open=True), 5) | ✓ |
| BR-05-07 | clamp=True + lt_min で _clamp(min,1,...) | test_range (IntRange(0,clamp=True), "-1") | ✓ |
| BR-05-08 | clamp=True + gt_max で _clamp(max,-1,...) | test_range (IntRange(max=5,clamp=True), "6") | ✓ |
| BR-05-09 | clamp=False + 範囲外で fail | test_range_fail | ✓ |
| BR-05-10 | clamp=False + 範囲内で値返却 | test_range | ✓ |
| BV-05-01 | min_open=False の min/min-1/min+1 | test_range + test_range_fail | △ 部分的 |
| BV-05-02 | min_open=True の min/min-1/min+1 | test_range_fail (IntRange(0,min_open=True,clamp=True),"0") | △ clamp版のみ |
| BV-05-06 | clamp + min_open=True で min+1 丸め | test_range (IntRange(0,min_open=True,clamp=True),"0"→1) | ✓ |
| BV-05-07 | clamp + max_open=True で max-1 丸め | test_range (IntRange(max=5,max_open=True,clamp=True),"5"→4) | ✓ |
| EN-05-05 | FloatRange で clamp+open 禁止 (TypeError) | test_float_range_no_clamp_open | ✓ |
| EN-05-02 | 構築後に self.clamp=True を設定すると convert が反映（冪等性欠如） | test_float_range_no_clamp_open の後半 (sneaky.clamp=True) | ✓ |
| CI-05-03 | MRO 順序 IntRange→_NumberRangeBase→IntParamType→... | - | **GAP** |
| CI-05-04 | _clamp IntRange 実装が int を返す | 直接テストなし（test_rangeで間接確認） | △ |
| SV-05-04 | convert が self.min/max を変更しない | - | **GAP** |
| EN-05-01 | min/max 等は public だが構築後変更は想定外 | - (test_float_range_no_clamp_open で部分検証) | △ |
| EN-05-04 | min > max の設計是非 | - | **GAP** |
| **Choice (TGT-03)** | | | |
| BR-03-12 | 候補外値で fail (BadParameter) | test_choice_get_invalid_choice_message | △ メッセージのみ |
| SV-03-03 | choices なしで構築不可能 | - | **GAP** |
| EN-03-01 | choices/case_sensitive の再代入禁止契約 | - | **GAP** |
| EN-03-03 | Choice(choices=...) 必須引数なしで構築不可 | - | **GAP** |
| EN-03-04 | convert/normalize が choices 不変条件を保持 | - | **GAP** |
| **ParamType (TGT-01)** | | | |
| BR-01-01 | __call__(None) が convert を呼ばず None | - | **GAP** |
| BR-01-05 | fail が BadParameter を投げる | test_range_fail で間接 | △ 間接 |
| CI-01-01 | 派生の __call__ が派生の convert を呼ぶ | test_range で間接 | △ 間接 |
| EN-01-02 | name 未設定サブクラスで to_info_dict が例外を投げない | - | **GAP** |
| **StringParamType (TGT-02)** | | | |
| BR-02-01 | bytes 入力時に decode | - | **GAP** |
| BR-02-03 | argv encoding 失敗時に fs_enc フォールバック | - | **GAP** |
| **_NumberParamTypeBase (TGT-04)** | | | |
| ER-04-01 | _number_class 未設定のサブクラスで AttributeError | - | **GAP** |
| CI-04-01 | すべての派生で _number_class が定義 | - | **GAP** |
| **ClickException / BadParameter / MissingParameter (TGT-06, 07)** | | | |
| 多数 | 例外階層全般 | test_file_error_surrogates / test_invalid_path_with_esc_sequence が部分的 | △ 断片 |
| **make_pass_decorator (TGT-08)** | | | |
| 全 13件 | Decorator Factory 全般 | test_types.py には存在せず（別ファイル `tests/test_utils.py` 等参照） | **OUT OF FILE** |

## 3. 定量集計

### 3.1 本TRMスコープ（TGT-01〜08、計173件）に対する既存テストの状況

| マッピング | 件数 | 割合 |
|---|---|---|
| ✓ 完全一致（既存テストで検証済み） | 約 10 | 5.8% |
| △ 部分一致・間接検証 | 約 15 | 8.7% |
| **GAP（既存テスト未検証）** | **約 150** | **86.7%** |

### 3.2 GAP の種別内訳

| 種別 | TRM 件数 | GAP 件数（概算） | GAP率 |
|---|---|---|---|
| BR | 47 | 40 | 85% |
| EC | 22 | 21 | 95% |
| BV | 11 | 6 | 55% |
| ER | 13 | 12 | 92% |
| DP | 7 | 7 | 100% |
| CI | 24 | 22 | 92% |
| SV | 13 | 13 | 100% |
| CP | 15 | 14 | 93% |
| **EN** | **21** | **20** | **95%** |

### 3.3 既存テストで TRM に対応がない観点

| 既存テスト | TRM 不一致の理由 |
|---|---|
| test_cast_multi_default | Argument cast は本TRMスコープ外（core.py） |
| test_path_type / resolve_symlink / surrogates | Path は本TRMスコープ外 |
| test_file_surrogates / file_error_surrogates | File/FileError の詳細挙動は本TRMスコープ外 |
| test_invalid_path_with_esc_sequence | Path と BadParameter の連携 |

## 4. 考察

### 4.1 GAP率 約87% の解釈

本TRMは既存テストの**7倍以上の粒度**で検証要求を定義している。理由:

1. **本TRMは構造設計観点を網羅**: 既存テストは主に `convert` の入出力対を確認するが、TRMは CI/SV/CP/EN の OOP 構造観点も検証
2. **ダイヤモンド継承の MRO 検証は既存にない**: CI-05-03 のような MRO 順序検証は既存テストに存在せず
3. **カプセル化観点は全面的に不在**: 既存テストは動作検証に特化し、設計契約の検証（EN種別）はほぼない
4. **ParamType / StringParamType / _NumberParamTypeBase の直接テストが手薄**: 既存は派生クラス（IntRange等）中心で、基底クラスの単独テストが少ない

### 4.2 既存テストで検証されている観点（5.8%）の質

- IntRange/FloatRange の範囲検証は parametrize で豊富に網羅（test_range 14件 + test_range_fail 7件）
- clamp と open_bound の組合せは Float で特別な禁止契約を検証
- Choice の invalid message は1件のみ

→ 動作検証（BR/BV）には強いが、構造検証（CI/SV/CP/EN）はほぼ未対応。

### 4.3 TRMが追加的に発見した観点（GAP の質的評価）

特に価値が高い GAP:

| TRM ID | 観点 | 価値 |
|---|---|---|
| **EN-04-02** | _number_class 未設定の派生で AttributeError | high: 実装契約違反の早期検出 |
| **EN-03-03** | Choice 必須引数の契約 | high: 構築契約の明文化 |
| **EN-05-04** | min > max を許容する設計の是非 | medium: 設計レビュー材料 |
| **CI-05-03** | IntRange の MRO 順序 | medium: リファクタ時のregressionテスト |
| **SV-05-04** | convert が self.min/max を変更しない | medium: 冪等性の保証 |
| **CP-08-02** | update_wrapper で元関数メタデータ転写 | high: デバッグ性維持 |
| **CP-08-03** | Closure の object_type 独立性 | high: Decorator Factory の正しい分離 |

### 4.4 click 開発者向けの含意

既存テストは **動作の正しさ** を確認する。v3.1 TRM は **設計契約・構造の健全性** を加えて検証する。両者を統合すれば、リファクタ時の regression 検出能力が大きく高まる。

特に本 click バージョンで発見した以下は開発チームへの提案として価値:
1. `Choice.choices` / `_NumberRangeBase.min` の public mutable は意図的か、protected にすべきか
2. `_NumberParamTypeBase._number_class` の未設定を `__init_subclass__` で検出するか
3. FloatRange の clamp+open 禁止検査は IntRange にもあるべきか（設計差異の明文化）

## 5. 評価軸（trm-evaluation-criteria.md カテゴリA-3）への数値反映

| 指標 | 値 |
|---|---|
| 既存カバー重複率 | 約 5.8%（10件/173件） |
| TRM固有カバー率 | 約 86.7%（150件/173件） |
| 見逃し検出数 | 約 150件（既存テストに存在しない観点） |

sakura-editorは既存テストが限定的だったため、この既存テスト比較は click で初めて質的に実施された。

## 6. 次アクション

- [ ] GAP の high 優先度（EN/CI/SV の一部）に絞って click への PR 検討
- [ ] `tests/test_decorators.py` / `tests/test_exceptions.py` も確認（TGT-07/08 の追加比較）
- [ ] 網羅率の定量化（BR の既存カバー vs TRM GAP）をスクリプト化
- [ ] 論文 §4.2 に「既存テストに対する追加的カバー率」として引用

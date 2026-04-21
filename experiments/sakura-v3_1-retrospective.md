# sakura-editor TRM への v3.1 遡及適用評価

> **対象**: 既存 sakura-editor TRM (`test-requirements/test-requirements.yaml`, schema v1.0, 295要求)
> **評価日**: 2026-04-21
> **目的**: スキーマ v3.1 追加種別 (CI/SV/CP/EN) が遡及適用できるか、追加要求がどれだけ発生するかを実測
> **関連**: `experiments/click/reports/pipeline-summary.md`（第2実証との比較）

---

## 1. 結論（要約）

sakura-editor は **手続き型中心（free functionのみを対象とした）** の実証であったため、v3.1 の遡及適用による追加要求数は **極めて限定的**。具体的には:

- **CI (クラス継承)**: 0件 — 対象にクラスが存在しない
- **SV (状態変数)**: 0件 — インスタンス変数なし
- **CP (コードパターン)**: **+7〜10件** — C++ RAII / const correctness / マクロ展開等
- **EN (カプセル化)**: 0件 — 被カプセル対象（メンバ）なし
- **SV新サブタイプ** (member_declaration_validity / member_initialization_requirement): 0件

**推定追加**: **約 +7〜10件（+3%程度）**。現行295件からv3.1対応版では約 **300件** に微増。

## 2. 対象関数の再確認

既存TRMの対象関数（全8件、すべて free function）:

| TGT | 関数 | クラス所属 | v3.1新種別の適用可能性 |
|---|---|---|---|
| TGT-01 | GetDateTimeFormat | なし | CP: マクロ `_T()` / バッファ管理 |
| TGT-02 | ParseVersion | なし | CP: const correctness |
| TGT-03 | CompareVersion | なし | なし（3行の単純比較） |
| TGT-04 | IsMailAddress | なし | CP: ポインタ引数のnullチェック |
| TGT-05 | WhatKindOfTwoChars | なし | CP: ECharKind enum 依存 |
| TGT-06 | WhatKindOfTwoChars4KW | なし | CP: WhatKindOfTwoCharsとのDP差異（既存DP-02で部分カバー） |
| TGT-07 | Convert_ZeneisuToHaneisu | なし | CP: 文字テーブル参照、range-based for |
| TGT-08 | Convert_HaneisuToZeneisu | なし | CP: 同上 |

**CI / SV / EN は 0件**: 対象にクラスがないため、v3.1 の価値は実現しない。

## 3. CP（コードパターン）遡及追加候補（7件）

現行TRMは CP種別が0件。v3.1適用で以下が追加候補:

| 追加 CP ID | サブタイプ | 対象 | 内容 |
|---|---|---|---|
| CP-01-01 | macro_expansion | GetDateTimeFormat | `_T()` マクロが `L""` (wchar) に展開されるパスを検証 |
| CP-02-01 | idiom_correctness | ParseVersion | `const WCHAR*` 引数の const 伝搬（C++ const correctness） |
| CP-04-01 | idiom_correctness | IsMailAddress | `const ACHAR*` + `int*` 出力パラメータの C イディオム |
| CP-04-02 | resource_lifecycle | IsMailAddress | `pnAddressLength` が NULL 可能でも正しく扱う（C++ RAII未適用の手動API） |
| CP-05-01 | macro_expansion | WhatKindOfTwoChars | `ECharKind` enum + マクロ `CK_CSYM` 等のテーブル参照 |
| CP-07-01 | idiom_correctness | Convert_ZeneisuToHaneisu | 文字テーブル（array）へのインデックスアクセスの安全性 |
| CP-08-01 | resource_lifecycle | Convert_HaneisuToZeneisu | 出力バッファの書き換え（in-place vs copy の設計契約） |

**推定追加件数**: +7件（種別内訳は全て CP）

## 4. SV/EN が適用可能な潜在対象（参考）

現行TRMは対象外だが、sakura-editor の中核クラス群には v3.1 遡及適用で得られる観点が豊富にある:

| クラス | ファイル | 推定適用観点 |
|---|---|---|
| CEditView | CEditView.cpp | CI/SV/EN すべて |
| CEditDoc | CEditDoc.cpp | SV/EN 多数（文書状態） |
| CMemoryIterator | CMemoryIterator.cpp | SV/CP (iterator パターン) |
| CFilePath | CFilePath.cpp | SV/EN (パス検証) |
| CShareData | CShareData.cpp | SV (共有メモリ状態) |

これらは今回のTRM対象から意図的に外されている（外部依存・UI層のため）。今後クラスを対象に追加する場合、**v3.1 の価値は大きく実現する**。

## 5. sakura vs click の v3.1 適用効果の比較

| 指標 | sakura-editor | click (types.py 5クラス) |
|---|---|---|
| 対象選定方針 | 純粋関数中心（OOP回避） | OOP中心（ParamType階層） |
| v3.0 5種別 | 99 (+ 追加カバレッジで295) | 80 |
| **v3.1 遡及追加** | **+7件（+2.4%）** | **+51件（+63.8%）** |
| OOP 相対強度 | 非該当 | 強い |

**示唆**: v3.1 追加種別の価値は対象コードの **OOP 度合い** に比例する。これは本研究の主張と整合的で、「OOP特性を活かす場面で手法の差別化効果が大きい」という含意を持つ。

## 6. sakura で v3.1 を本格適用する場合のロードマップ（参考）

もし sakura の選定関数を拡張して OOP を含める場合の見通し:

### 6.1 対象候補の拡張

- CEditDoc の主要メソッド（5個程度）を追加
- CMemoryIterator のイテレーションロジック
- CFilePath のパス検証・構築メソッド

### 6.2 期待される v3.1 追加要求数

| 種別 | 期待件数（推定） | 根拠 |
|---|---|---|
| CI | 10-15 | CEditDoc派生、Iterator interface |
| SV | 20-30 | 文書状態・Iterator状態の多数field |
| CP | 15-20 | C++ RAII / CRTP / rule of 5 多数 |
| EN | 10-15 | public/private の境界、mutable member多数 |
| **合計 +55-80件** | | sakura全体で約 +18〜27% |

### 6.3 実施コスト

- 対象関数選定: 0.5日
- analyze 再実行: 1日
- generate-trm 再実行: 1〜2日
- 既存テストとの比較: 1日
- 合計: **3.5〜4.5日**

## 7. 本retrospectiveの論文への利用

次稿論文で以下の論点として利用可能:

1. **v3.1 の差別化効果は対象の OOP 度合いに依存する**（sakura：+2.4% vs click：+64%）
2. **v3.0 と v3.1 の線引きが明確になる**（手続き型コードでは v3.0 で十分、OOPなら v3.1 の価値が大きい）
3. **「どの対象にどの拡張を適用すべきか」の判断フレームワーク**に発展可能

## 8. 次アクション

- [ ] 時間があれば sakura の CEditDoc 1クラスだけ v3.1 適用して実測
- [ ] 本retrospectiveを `knowledge/paper-experiment-section.md` §4.4 の補足として引用
- [ ] v3.1 適用の判断フレームワーク（OOP度で分岐）を knowledge/ にドキュメント化

---

## 付録: 定量サマリ

| 指標 | 現行 (v1.0) | v3.1 遡及後（推定） |
|---|---|---|
| 総要求数 | 295 | ~302 |
| BR | — | 同数 |
| EC | — | 同数 |
| BV | — | 同数 |
| ER | — | 同数 |
| DP | — | 同数 |
| CI | 0 | 0 |
| SV | 0 | 0 |
| CP | 0 | +7 |
| EN | 0 | 0 |
| SV新サブタイプ | 0 | 0 |

sakura の現行実証は手続き型に特化しており、v3.1 遡及による増加は CP の +7件にとどまる。

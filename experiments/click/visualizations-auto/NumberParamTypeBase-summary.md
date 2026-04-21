# _NumberParamTypeBase (TGT-04) — 可視化レイヤ（自動生成）

> **対象**: `class _NumberParamTypeBase(ParamType)`
> **責務**: 数値型 (int/float) 変換の共通基盤。_number_class で戦略を切替
> **総要求数**: 11
> **種別内訳**: 🟦 分岐網羅 (BR) 2, 🟩 同値クラス (EC) 2, 🟥 エラーパス (ER) 1, 🔷 クラス継承 (CI) 2, ⬛ コードパターン (CP) 2, 🟧 カプセル化 (EN) 2

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((_NumberParamTypeBase))
    分岐網羅 (BR)
      BR-04-01: _number_class(value) が成功する値で数値が返ること
      BR-04-02: _number_class(value) が ValueError を投げたとき
    同値クラス (EC)
      EC-04-01: 派生 IntParamType で int 変換、FloatParamType 
      EC-04-02: convert 可能な str/int/float/bool と変換不可能な s
    エラーパス (ER)
      ER-04-01: _number_class 未設定のサブクラスで AttributeError 
    クラス継承 (CI)
      CI-04-01: すべての派生クラス (IntParamType, FloatParamType)
      CI-04-02: IntParamType と FloatParamType で convert 
    コードパターン (CP)
      CP-04-01: Strategy パターン: _number_class の差し替えで振る舞いが
      CP-04-02: ClassVar 宣言された _number_class が派生で実体を持つこと
    カプセル化 (EN)
      EN-04-01: _number_class は _ prefix による慣例的 protecte
      EN-04-02: 派生クラスが _number_class を必ず設定する契約。テストでは全派生ク
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

_NumberParamTypeBase,分岐網羅 (BR),2
_NumberParamTypeBase,同値クラス (EC),2
_NumberParamTypeBase,エラーパス (ER),1
_NumberParamTypeBase,クラス継承 (CI),2
_NumberParamTypeBase,コードパターン (CP),2
_NumberParamTypeBase,カプセル化 (EN),2
分岐網羅 (BR),優先度:high,1
分岐網羅 (BR),優先度:medium,1
同値クラス (EC),優先度:high,1
同値クラス (EC),優先度:medium,1
エラーパス (ER),優先度:high,1
クラス継承 (CI),優先度:high,1
クラス継承 (CI),優先度:medium,1
コードパターン (CP),優先度:high,1
コードパターン (CP),優先度:medium,1
カプセル化 (EN),優先度:high,1
カプセル化 (EN),優先度:medium,1
```

## 3. 複合影響のヒートマップ（field × risk）

| field | missing_validation | leaky_getter | leaky_setter | unintended_mutability | external_mutation | invariant_breach | public_mutable_field |
|---|---|---|---|---|---|---|---|
| _number_class | 🔴 | — | — | — | — | — | — |

**凡例**: 🔴 high / 🟡 medium / 🟢 low / — 検出なし

## 4. トリガー相互関係（Chord 風 Flowchart）

```mermaid
flowchart LR
    SVAR_04_01["🔵 _number_class"]
    RISK_0["🔴 missing_validation"]
    SVAR_04_01 ==> RISK_0
```

---

## 自動生成のメタ情報

- ツール: `scripts/generate_visualizations.py`
- 入力スキーマ: TRM v3.1 (`templates/trm-schema.yaml`)
- 図解形式: Mermaid + Markdown
- 対象読者: 非エンジニア + 技術系PM + レビュアー
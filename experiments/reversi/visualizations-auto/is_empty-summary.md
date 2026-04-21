# is_empty (TGT-02) — 可視化レイヤ（自動生成）

> **対象**: `is_empty(board, x: int, y: int) -> bool`
> **責務**: 指定マスが空（値=0）かを判定する
> **総要求数**: 5
> **種別内訳**: 🟦 分岐網羅 (BR) 2, 🟩 同値クラス (EC) 2, 🟥 エラーパス (ER) 1

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((is_empty))
    分岐網羅 (BR)
      BR-02-01: 空マス（値=0）で True を返すこと
      BR-02-02: 非空マス（値≠0）で False を返すこと
    同値クラス (EC)
      EC-02-01: 自分の石が置かれたマスは非空
      EC-02-02: 相手の石が置かれたマスは非空
    エラーパス (ER)
      ER-02-01: 前提違反（x, y が範囲外）で IndexError を発生させることを観察す
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

is_empty,分岐網羅 (BR),2
is_empty,同値クラス (EC),2
is_empty,エラーパス (ER),1
分岐網羅 (BR),優先度:high,1
分岐網羅 (BR),優先度:medium,1
同値クラス (EC),優先度:high,1
同値クラス (EC),優先度:medium,1
```

## 3. 複合影響のヒートマップ（field × risk）

> (state_variables または encapsulation_risks が空のためヒートマップ対象外)

## 4. トリガー相互関係（Chord 風 Flowchart）

> (state_variables が空のため Chord 生成不可)

---

## 自動生成のメタ情報

- ツール: `scripts/generate_visualizations.py`
- 入力スキーマ: TRM v3.1 (`templates/trm-schema.yaml`)
- 図解形式: Mermaid + Markdown
- 対象読者: 非エンジニア + 技術系PM + レビュアー
# is_on_board (TGT-01) — 可視化レイヤ（自動生成）

> **対象**: `is_on_board(x: int, y: int) -> bool`
> **責務**: 座標 (x, y) が 8×8 盤面の範囲内かを判定する
> **総要求数**: 10
> **種別内訳**: 🟦 分岐網羅 (BR) 5, 🟩 同値クラス (EC) 1, 🟨 境界値 (BV) 4

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((is_on_board))
    分岐網羅 (BR)
      BR-01-01: 全原子条件が真のとき True を返すことを検証する
      BR-01-02: x が負のとき False を返すこと（下限違反）
      BR-01-03: x が BOARD_SIZE 以上のとき False を返すこと（上限違反）
      BR-01-04: y が負のとき False を返すこと
      ...他1件
    同値クラス (EC)
      EC-01-01: x が範囲内の代表値（3）で True
    境界値 (BV)
      BV-01-01: x の下限境界 0 / -1 で判定が正しく切り替わること
      BV-01-02: x の上限境界 7 / 8 で判定が正しく切り替わること
      BV-01-03: y の下限境界 0 / -1 で判定が正しく切り替わること
      BV-01-04: y の上限境界 7 / 8 で判定が正しく切り替わること
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

is_on_board,分岐網羅 (BR),5
is_on_board,同値クラス (EC),1
is_on_board,境界値 (BV),4
分岐網羅 (BR),優先度:high,4
同値クラス (EC),優先度:high,1
境界値 (BV),優先度:high,4
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
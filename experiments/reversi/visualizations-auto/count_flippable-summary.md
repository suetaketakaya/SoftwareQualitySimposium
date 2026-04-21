# count_flippable (TGT-03) — 可視化レイヤ（自動生成）

> **対象**: `count_flippable(board, x, y, dx, dy, me) -> int`
> **責務**: (x, y) から (dx, dy) 方向に、自分の石で挟める相手の石の数を返す
> **総要求数**: 15
> **種別内訳**: 🟦 分岐網羅 (BR) 9, 🟩 同値クラス (EC) 2, 🟨 境界値 (BV) 2, 🟥 エラーパス (ER) 1, 🟪 依存切替 (DP) 1

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((count_flippable))
    分岐網羅 (BR)
      BR-03-01: me == 1 のとき opp = 2 になり、値 2 の石を数えること
      BR-03-02: me != 1 のとき opp = 1 になり、値 1 の石を数えること
      BR-03-03: 盤面内かつ相手の石が続く間、ループが継続すること
      BR-03-04: 盤端でループが終了すること
      ...他5件
    同値クラス (EC)
      EC-03-01: me=1 と me=2 の両方で対称に動作すること
      EC-03-02: 8方向すべてで挟み判定が機能すること（軸4 + 対角4）
    境界値 (BV)
      BV-03-01: 連続相手石数 0/1 の境界で返り値が切り替わること
      BV-03-02: 盤端で連続が途切れる境界（連続6と7）で正しく 0 を返すこと
    エラーパス (ER)
      ER-03-01: 起点 (x, y) から方向先がすぐ盤外のとき 0 を返すこと
    依存切替 (DP)
      DP-03-01: is_on_board が依存関数として正しく利用されること（スタブ化は不要）
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

count_flippable,分岐網羅 (BR),9
count_flippable,同値クラス (EC),2
count_flippable,境界値 (BV),2
count_flippable,エラーパス (ER),1
count_flippable,依存切替 (DP),1
分岐網羅 (BR),優先度:high,6
分岐網羅 (BR),優先度:medium,2
分岐網羅 (BR),優先度:low,1
同値クラス (EC),優先度:high,1
同値クラス (EC),優先度:medium,1
境界値 (BV),優先度:high,1
境界値 (BV),優先度:medium,1
エラーパス (ER),優先度:high,1
依存切替 (DP),優先度:high,1
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
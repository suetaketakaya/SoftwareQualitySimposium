# is_valid_move (TGT-04) — 可視化レイヤ（自動生成）

> **対象**: `is_valid_move(board, x, y, me) -> bool`
> **責務**: (x, y) に自分(me) の石を置けるかを判定する
> **総要求数**: 16
> **種別内訳**: 🟦 分岐網羅 (BR) 4, 🟩 同値クラス (EC) 3, 🟨 境界値 (BV) 3, 🟥 エラーパス (ER) 3, 🟪 依存切替 (DP) 3

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((is_valid_move))
    分岐網羅 (BR)
      BR-04-01: 範囲外の (x, y) で即座に False を返すこと
      BR-04-02: 非空マスに対して False を返すこと
      BR-04-03: 空マスかつ1方向以上で挟める場合に True を返すこと
      BR-04-04: 空マスだが8方向すべて挟めない場合に False を返すこと
    同値クラス (EC)
      EC-04-01: 対象マスの3状態（空/自分/相手）を網羅して判定が正しく切り替わること
      EC-04-02: 自分/相手（me=1, me=2）の両視点で対称に動作すること
      EC-04-03: 挟める方向が 0 個と 1 個以上 の両条件を網羅すること
    境界値 (BV)
      BV-04-01: 盤面の四隅（角）で正しく合法/非合法が判定されること
      BV-04-02: 盤面の辺で正しく合法/非合法が判定されること
      BV-04-03: x の範囲外直前直後（-1, 0, 7, 8）で判定が切り替わること
    エラーパス (ER)
      ER-04-01: 範囲外入力でクラッシュせず False を返すこと
      ER-04-02: 空盤面で全マスが False を返すこと（置ける場所がない）
      ER-04-03: 満盤で全マスが False を返すこと（空マスがない）
    依存切替 (DP)
      DP-04-01: is_on_board が False を返した時点で以降の呼び出しをスキップす
      DP-04-02: is_empty が False を返した時点で以降の呼び出しをスキップすること
      DP-04-03: count_flippable が 0 を返した方向は is_valid_mov
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

is_valid_move,分岐網羅 (BR),4
is_valid_move,同値クラス (EC),3
is_valid_move,境界値 (BV),3
is_valid_move,エラーパス (ER),3
is_valid_move,依存切替 (DP),3
分岐網羅 (BR),優先度:high,2
分岐網羅 (BR),優先度:medium,2
同値クラス (EC),優先度:high,2
同値クラス (EC),優先度:medium,1
境界値 (BV),優先度:high,2
境界値 (BV),優先度:medium,1
エラーパス (ER),優先度:high,2
エラーパス (ER),優先度:medium,1
依存切替 (DP),優先度:high,2
依存切替 (DP),優先度:medium,1
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
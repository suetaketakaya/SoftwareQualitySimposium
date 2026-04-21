# void Convert_ZeneisuToHaneisu (TGT-07) — 可視化レイヤ（自動生成）

> **対象**: `void Convert_ZeneisuToHaneisu(wchar_t* pData, int nLength)`
> **責務**: 全角英数字・記号を半角英数字・記号に in-place 変換する。文字数は変化しない。 内部ヘルパー ZeneisuToHaneisu_ で1文字ずつ変換を行う。

> **総要求数**: 10
> **種別内訳**: 🟦 分岐網羅 (BR) 5, 🟩 同値クラス (EC) 3, 🟨 境界値 (BV) 2

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((void Convert_ZeneisuToHaneisu))
    分岐網羅 (BR)
      BR-46: 全角大文字 (A-Z) の変換パスを検証する
      BR-47: 全角小文字 (a-z) の変換パスを検証する
      BR-48: 全角数字 (0-9) の変換パスを検証する
      BR-49: 全角記号テーブルによる変換パスを検証する
      ...他1件
    同値クラス (EC)
      EC-23: 全カテゴリの文字が正しく変換されることを検証する
      EC-24: 変換対象外の文字が変更されないことを検証する
      EC-25: 混在文字列が正しく処理されることを検証する
    境界値 (BV)
      BV-09: 全角文字範囲の境界値（先頭・末尾・範囲外直前直後）を検証する
      BV-10: nLength=0 の境界条件を検証する
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

void Convert_ZeneisuToHaneisu,分岐網羅 (BR),5
void Convert_ZeneisuToHaneisu,同値クラス (EC),3
void Convert_ZeneisuToHaneisu,境界値 (BV),2
分岐網羅 (BR),優先度:high,4
分岐網羅 (BR),優先度:medium,2
同値クラス (EC),優先度:high,2
同値クラス (EC),優先度:medium,1
境界値 (BV),優先度:high,1
境界値 (BV),優先度:medium,1
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
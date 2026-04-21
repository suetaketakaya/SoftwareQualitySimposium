# void Convert_HaneisuToZeneisu (TGT-08) — 可視化レイヤ（自動生成）

> **対象**: `void Convert_HaneisuToZeneisu(wchar_t* pData, int nLength)`
> **責務**: 半角英数字・記号を全角英数字・記号に in-place 変換する。文字数は変化しない。 ZeneisuToHaneisu の逆変換。内部ヘルパー HaneisuToZeneisu_ で1文字ずつ変換を行う。

> **総要求数**: 9
> **種別内訳**: 🟦 分岐網羅 (BR) 5, 🟩 同値クラス (EC) 2, 🟨 境界値 (BV) 1, 🟪 依存切替 (DP) 1

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((void Convert_HaneisuToZeneisu))
    分岐網羅 (BR)
      BR-51: 半角大文字 (A-Z) の変換パスを検証する
      BR-52: 半角小文字 (a-z) の変換パスを検証する
      BR-53: 半角数字 (0-9) の変換パスを検証する
      BR-54: 半角記号テーブルによる変換パスを検証する
      ...他1件
    同値クラス (EC)
      EC-26: 全カテゴリの文字が正しく変換されることを検証する
      EC-27: 変換対象外の文字が変更されないことを検証する
    境界値 (BV)
      BV-11: 半角文字範囲の境界値を検証する
    依存切替 (DP)
      DP-03: ZeneisuToHaneisu と HaneisuToZeneisu の往復変
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

void Convert_HaneisuToZeneisu,分岐網羅 (BR),5
void Convert_HaneisuToZeneisu,同値クラス (EC),2
void Convert_HaneisuToZeneisu,境界値 (BV),1
void Convert_HaneisuToZeneisu,依存切替 (DP),1
分岐網羅 (BR),優先度:high,4
分岐網羅 (BR),優先度:medium,1
同値クラス (EC),優先度:high,2
境界値 (BV),優先度:high,1
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
# UINT32 ParseVersion (TGT-02) — 可視化レイヤ（自動生成）

> **対象**: `UINT32 ParseVersion(const WCHAR* sVer)`
> **責務**: バージョン文字列を解析し、4つのバージョンコンポーネント（各8ビット）にパックした UINT32 値を返す。各コンポーネントは符号1bit+数値7bitの構成で、特殊文字列 (alpha/beta/rc/pl) によるシフト値を加算して大小比較可能にする。

> **総要求数**: 20
> **種別内訳**: 🟦 分岐網羅 (BR) 13, 🟩 同値クラス (EC) 4, 🟨 境界値 (BV) 2, 🟥 エラーパス (ER) 1

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((UINT32 ParseVersion))
    分岐網羅 (BR)
      BR-11: alpha 修飾子の分岐を通過する
      BR-12: beta 修飾子の分岐を通過する
      BR-13: rc/RC 修飾子の分岐を通過する
      BR-14: pl 修飾子の分岐を通過する
      ...他9件
    同値クラス (EC)
      EC-07: 標準的なドット区切りバージョンを検証する
      EC-08: 各修飾子付きバージョンの順序関係を検証する (x < alpha < beta 
      EC-09: 区切り文字の各バリエーションが同等に扱われることを検証する
      EC-10: 空文字列の入力を検証する
    境界値 (BV)
      BV-05: 2桁と3桁の境界での数値切り捨てを検証する
      BV-06: コンポーネント数の境界（0, 1, 4, 5）を検証する
    エラーパス (ER)
      ER-01: 空文字列入力時にクラッシュしないことを検証する
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

UINT32 ParseVersion,分岐網羅 (BR),13
UINT32 ParseVersion,同値クラス (EC),4
UINT32 ParseVersion,境界値 (BV),2
UINT32 ParseVersion,エラーパス (ER),1
分岐網羅 (BR),優先度:high,6
分岐網羅 (BR),優先度:medium,6
同値クラス (EC),優先度:high,2
同値クラス (EC),優先度:medium,2
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
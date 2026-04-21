# int CompareVersion (TGT-03) — 可視化レイヤ（自動生成）

> **対象**: `int CompareVersion(const WCHAR* verA, const WCHAR* verB)`
> **責務**: 2つのバージョン文字列を比較し、その大小関係を返す。 内部で ParseVersion を呼び出し、結果の UINT32 値の差分を返す。

> **総要求数**: 6
> **種別内訳**: 🟦 分岐網羅 (BR) 3, 🟩 同値クラス (EC) 2, 🟪 依存切替 (DP) 1

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((int CompareVersion))
    分岐網羅 (BR)
      BR-24: 同一バージョンで 0 が返ることを検証する
      BR-25: A が新しい場合に正の値が返ることを検証する
      BR-26: B が新しい場合に負の値が返ることを検証する
    同値クラス (EC)
      EC-11: 修飾子による順序関係の全パターンを検証する (x < a < b < rc < 
      EC-12: 異なる区切り文字で同一バージョンと判定されることを検証する
    依存切替 (DP)
      DP-01: ParseVersion の結果を正しく利用していることを間接的に検証する
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

int CompareVersion,分岐網羅 (BR),3
int CompareVersion,同値クラス (EC),2
int CompareVersion,依存切替 (DP),1
分岐網羅 (BR),優先度:high,2
分岐網羅 (BR),優先度:medium,1
同値クラス (EC),優先度:high,1
同値クラス (EC),優先度:medium,1
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
# static ECharKind CWordParse::WhatKindOfTwoChars4KW (TGT-06) — 可視化レイヤ（自動生成）

> **対象**: `static ECharKind CWordParse::WhatKindOfTwoChars4KW(ECharKind kindPre, ECharKind kindCur)`
> **責務**: WhatKindOfTwoChars のキーワード検索用バリアント。 基本ロジックは同一だが、ユーザー定義文字と制御文字のマッピング規則が異なる。

> **総要求数**: 6
> **種別内訳**: 🟦 分岐網羅 (BR) 2, 🟩 同値クラス (EC) 3, 🟪 依存切替 (DP) 1

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((static ECharKind CWordParse::WhatKindOfTwoChars4KW))
    分岐網羅 (BR)
      BR-44: ユーザー定義文字が CSYM にマッピングされるパスを検証する（通常版との差異）
      BR-45: 制御文字が CTRL のまま保持されるパスを検証する（通常版との差異）
    同値クラス (EC)
      EC-20: UDEF+CSYM が同種と判定されることを検証する（通常版では UDEF+ET
      EC-21: UDEF+ETC が別種と判定されることを検証する（通常版では同種）
      EC-22: CTRL+ETC が別種と判定されることを検証する（通常版では同種）
    依存切替 (DP)
      DP-02: WhatKindOfTwoChars と WhatKindOfTwoChars4
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

static ECharKind CWordParse::WhatKindOfTwoChars4KW,分岐網羅 (BR),2
static ECharKind CWordParse::WhatKindOfTwoChars4KW,同値クラス (EC),3
static ECharKind CWordParse::WhatKindOfTwoChars4KW,依存切替 (DP),1
分岐網羅 (BR),優先度:high,2
同値クラス (EC),優先度:high,3
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
# static ECharKind CWordParse::WhatKindOfTwoChars (TGT-05) — 可視化レイヤ（自動生成）

> **対象**: `static ECharKind CWordParse::WhatKindOfTwoChars(ECharKind kindPre, ECharKind kindCur)`
> **責務**: 2つの文字種別を受け取り、それらが同一の単語として結合可能かを判定する。 結合可能な場合は結合後の文字種別を返し、不可能な場合は CK_NULL を返す。 全角長音・濁点のひらがな/カタカナへの引きずり規則を実装する。

> **総要求数**: 11
> **種別内訳**: 🟦 分岐網羅 (BR) 8, 🟩 同値クラス (EC) 3

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((static ECharKind CWordParse::WhatKindOfTwoChars))
    分岐網羅 (BR)
      BR-36: 初回同種チェック (kindPre == kindCur) で即座に返すパスを検
      BR-37: 長音/濁点がカタカナ/ひらがなに引きずられるパスを検証する
      BR-38: 長音・濁点の連続が同種とみなされるパスを検証する
      BR-39: ラテン → CSYM マッピングパスを検証する
      ...他4件
    同値クラス (EC)
      EC-17: 全ECharKind 値の同種ペアが正しく処理されることを検証する
      EC-18: 長音/濁点とカタカナ/ひらがなの引きずり規則の全組み合わせを検証する
      EC-19: 各マッピング規則（LATIN→CSYM, UDEF→ETC, CTRL→ETC）
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

static ECharKind CWordParse::WhatKindOfTwoChars,分岐網羅 (BR),8
static ECharKind CWordParse::WhatKindOfTwoChars,同値クラス (EC),3
分岐網羅 (BR),優先度:high,6
分岐網羅 (BR),優先度:medium,2
同値クラス (EC),優先度:high,2
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
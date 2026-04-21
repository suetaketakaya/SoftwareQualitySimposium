# BOOL IsMailAddress (TGT-04) — 可視化レイヤ（自動生成）

> **対象**: `BOOL IsMailAddress(const wchar_t* pszBuf, int offset, size_t cchBuf, int* pnAddressLength)`
> **責務**: 文字列バッファ中の指定位置がメールアドレスの先頭であるかを判定する。 ローカルパート@ドメインパートの形式を検証し、TRUEの場合はアドレス長を出力する。

> **総要求数**: 17
> **種別内訳**: 🟦 分岐網羅 (BR) 9, 🟩 同値クラス (EC) 4, 🟨 境界値 (BV) 2, 🟥 エラーパス (ER) 2

---

## 1. トリガー階層（Sunburst / Mindmap）

```mermaid
mindmap
  root((BOOL IsMailAddress))
    分岐網羅 (BR)
      BR-27: 正の offset で直前文字が有効文字の場合に FALSE を返す境界判定を検
      BR-28: 先頭がドットの場合に FALSE を返すことを検証する
      BR-29: 先頭が無効文字の場合に FALSE を返すことを検証する
      BR-30: ローカルパートが短すぎる場合に FALSE を返すことを検証する
      ...他5件
    同値クラス (EC)
      EC-13: 標準的なメールアドレスが正しく判定されることを検証する
      EC-14: 各種の無効なメールアドレスが FALSE と判定されることを検証する
      EC-15: ローカルパートの記号類が正しく処理されることを検証する
      EC-16: offset パラメータの各パターンを検証する
    境界値 (BV)
      BV-07: IsValidChar の境界値（0x20, 0x21, 0x7E, 0x7F）
      BV-08: 最小長のメールアドレス（a@b.cc）を検証する
    エラーパス (ER)
      ER-02: 空文字列入力時に FALSE を返しクラッシュしないことを検証する
      ER-03: @ のみの文字列入力時に FALSE を返すことを検証する
```

## 2. 種別分布の流量（Sankey）

```mermaid
sankey-beta

BOOL IsMailAddress,分岐網羅 (BR),9
BOOL IsMailAddress,同値クラス (EC),4
BOOL IsMailAddress,境界値 (BV),2
BOOL IsMailAddress,エラーパス (ER),2
分岐網羅 (BR),優先度:high,5
分岐網羅 (BR),優先度:medium,4
同値クラス (EC),優先度:high,2
同値クラス (EC),優先度:medium,2
境界値 (BV),優先度:high,1
境界値 (BV),優先度:medium,1
エラーパス (ER),優先度:high,1
エラーパス (ER),優先度:medium,1
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
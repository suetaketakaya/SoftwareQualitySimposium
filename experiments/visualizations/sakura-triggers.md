# sakura-editor: トリガー×影響 可視化

> **対象**: `sakura_core/parse/CWordParse.cpp` の `IsMailAddress(const ACHAR*, int offset, int* pnAddressLength)`
> **質問の意図**: 「この文字列はメールアドレスか？」の判定がどう動くかを、コードを読まずに理解する

---

## 1. 全体俯瞰（非エンジニア向け導入）

```mermaid
flowchart LR
    A[🟢 ユーザが<br/>テキストを入力] --> B[📄 テキストバッファ]
    B --> C[🔵 検索位置<br/>offset を指定]
    C --> D{メール判定処理}
    D -->|✅ メール| E[長さを返す]
    D -->|❌ 非メール| F[FALSEを返す]

    style A fill:#e8f5e9
    style B fill:#f3e5f5
    style C fill:#e3f2fd
    style E fill:#c8e6c9
    style F fill:#ffcdd2
```

ユーザが入力したテキストの中から **メールアドレスらしき文字列** があるかを、指定位置 (offset) から判定します。

---

## 2. トリガー階層（Sunburst風 / Mindmap）

```mermaid
mindmap
  root((メール判定))
    🟢 ユーザ入力由来
      入力文字列 pString
      開始位置 offset
    🔵 構造ルール
      先頭文字の妥当性
      @ の存在
      ローカル部の長さ
      ドメイン部のドット数
      ドメインラベルの長さ
    🟣 外部出力
      pnAddressLength ポインタ
      NULL可能性
    🔴 境界条件
      文字コード境界
      0x20 0x7E の範囲
      空文字列
```

**読み方**: メール判定には「ユーザ入力」「形式ルール」「出力先」「境界」の4系統のトリガーが絡みます。リバーシ（1系統）より複雑。

---

## 3. トリガーと結果の流れ（Sankey）

```mermaid
sankey-beta

ユーザ操作,テキスト入力,100
テキスト入力,offset指定,100
offset指定,先頭妥当性チェック,100
先頭妥当性チェック,合格,60
先頭妥当性チェック,失敗,40
合格,@検索,60
@検索,見つかる,50
@検索,見つからない,10
見つかる,ドメイン解析,50
見つからない,❌ 非メール,10
ドメイン解析,ラベル検証OK,35
ドメイン解析,ドット不足,15
ラベル検証OK,✅ メール判定,35
ドット不足,❌ 非メール,15
失敗,❌ 非メール,40
```

**読み方**: 入力テキストが左、最終判定が右。途中の各チェックで「何件がどちらへ流れたか」が帯の太さでわかります。**"ドット不足" と "先頭不正" が主な落選理由** だと一目瞭然。

---

## 4. トリガー同士の関係（Chord風）

```mermaid
flowchart TB
    subgraph 関係図
        TEXT[🟢 入力文字列]
        OFFSET[🔵 offset]
        AT[🔵 @ 位置]
        DOT[🔵 ドット位置]
        LEN[🟣 pnAddressLength]
        RESULT[判定結果]

        TEXT ---|"要素文字の集合"| OFFSET
        OFFSET ---|"先頭判定の起点"| AT
        AT ---|"左右を分ける"| DOT
        DOT ---|"ドメイン解析"| RESULT
        LEN -.->|"NULL許容"| RESULT
        OFFSET -.->|"直前文字が有効ならFALSE"| RESULT
    end

    style TEXT fill:#e8f5e9
    style OFFSET fill:#e3f2fd
    style AT fill:#e3f2fd
    style DOT fill:#e3f2fd
    style LEN fill:#f3e5f5
    style RESULT fill:#fff9c4
```

**読み方**: 実線が必須依存、点線が任意依存。OFFSET → AT → DOT の順にチェックが進みます。

---

## 5. 複合影響のヒートマップ

ユーザ入力の代表的パターンと offset 位置の組合せで、判定結果がどうなるか:

| 入力 \ offset | offset=0（先頭） | offset=途中<br/>直前が有効文字 | offset=途中<br/>直前がスペース等 |
|---|---|---|---|
| `"a@b.cc"` | ✅ メール | ⚠️ FALSE<br/>（"メール継続中"判定） | ✅ メール |
| `".abc@d.co"` | ❌ 先頭がドット | ❌ 同上 | ❌ 同上 |
| `"a@bcom"` | ❌ ドット不足 | — | ❌ ドット不足 |
| `"abc"` | ❌ @ 不足 | — | ❌ @ 不足 |
| `""` 空文字列 | ❌ FALSE | — | — |
| `"user+tag@x.co"` | ✅ メール | ⚠️ 直前文字次第 | ✅ メール |

**読み方**: 横軸が offset（検索位置）の種類、縦軸が文字列の形。2つを組合せて結果を読み取ります。**同じメール形式でも offset の前文字次第で判定が変わる** 点が非自明で、このヒートマップで視覚化されます。

---

## 6. 他関数との相互作用（関連機能の全体像）

CWordParse.cpp 内の関連判定との関係:

```mermaid
flowchart LR
    INPUT[🟢 テキスト入力] --> MAIL[IsMailAddress]
    INPUT --> URL[IsURL]
    INPUT --> PATH[IsFilePath]

    MAIL ---|"排他的"| URL
    URL ---|"排他的"| PATH

    MAIL --> RESULT1[メール長]
    URL --> RESULT2[URL長]
    PATH --> RESULT3[パス長]

    style INPUT fill:#e8f5e9
    style MAIL fill:#fff9c4
    style URL fill:#fff9c4
    style PATH fill:#fff9c4
```

**注**: IsMailAddress は URL 判定・ファイルパス判定と **排他的に試される**（サクラエディタの仕組み）。ユーザ操作はテキスト入力1つだが、裏では複数の判定関数が "誰が最初にマッチするか" 競う。

---

## 7. まとめ（非エンジニア向け）

### このコードの本質

入力テキストの特定位置から見て、以下の全条件を満たすとメールと判定されます:

1. 先頭が有効な文字（英数字等）で始まる
2. `@` が含まれている
3. ドメイン部にドット `.` が1つ以上ある
4. ドメインラベル（ドット区切りの各部分）が空でない
5. offset が 0 より大きい場合、直前の文字が区切り（空白等）である

→ **メールらしい見た目** と **区切りの位置関係** の両方が必要。

### トリガー数と結果

- ユーザ操作: 1系統（テキスト入力）
- 内部トリガー: 5種類のルール + offset 依存
- 結果: TRUE/FALSE の2値 + 長さ出力

リバーシ（1系統・3判定）に比べ、**内部トリガーが倍以上**。非エンジニアに伝える場合は **ヒートマップが最も効果的** と推察します。

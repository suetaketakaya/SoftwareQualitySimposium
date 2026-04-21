# ホワイトボックステスト要求モデル設計書

## 1. モデル設計方針

### 1.1 目的

本ドキュメントは、SQiP 2026 実証実験「静的解析とLLMを組み合わせたホワイトボックステスト要求モデル生成による単体テスト半自動化」における、テスト要求モデルの設計方針と具体的なテスト要求を定義する。

テスト要求モデルは以下の役割を果たす:

1. **人間レビュー可能性**: テスト設計の妥当性を人間が確認できる粒度で記述する
2. **LLM入力可能性**: YAMLファイルとしてLLMに渡し、テストコード生成の入力として利用する
3. **網羅性の測定基盤**: テスト要求の充足率をもってカバレッジを定量評価する

### 1.2 設計原則

| 原則 | 説明 |
|------|------|
| ソースコード忠実性 | 実際のソースコードの分岐構造を解析し、その構造に忠実にテスト要求を導出する |
| テスト要求とテストケースの分離 | テスト要求は「何を検証すべきか」を定義し、「どのように検証するか」（テストケース）は定義しない |
| ID体系による追跡可能性 | 全てのテスト要求にIDを付与し、ソースコードの分岐条件・同値クラス・境界値との対応を明示する |
| 優先度の明示 | 各テスト要求に high/medium/low の優先度を付与し、テスト実装の順序を支援する |

### 1.3 テスト要求ID体系

| 接頭辞 | 意味 | 導出元 |
|--------|------|--------|
| BR-xx | 分岐網羅 (Branch Coverage) | ソースコードの if/else/switch 分岐 |
| EC-xx | 同値クラス (Equivalence Class) | 入力パラメータの同値分割 |
| BV-xx | 境界値 (Boundary Value) | 同値クラスの境界における値 |
| ER-xx | エラーパス (Error Path) | 異常系・エラー処理パス |
| DP-xx | 依存切替 (Dependency Path) | 依存先の振る舞いに基づくパス |

### 1.4 対象関数の選定基準

Phase 1 のリポジトリ解析結果に基づき、以下の基準で8関数を選定した:

- 純粋関数またはそれに近い構造（外部依存が最小）
- 明確な分岐構造を持つ
- 既存テストが存在し、比較評価が可能
- 論文で説明可能な適度なコード量

---

## 2. スキーマ定義

### 2.1 テスト対象 (Target) のスキーマ

```yaml
target:
  id: string            # 対象ID (TGT-XX)
  file: string           # ソースファイルパス
  header: string         # ヘッダファイルパス
  function_signature: string  # 関数シグネチャ
  responsibility: string      # 責務の要約
  preconditions: list[string] # 前提条件
  constraints: list[string]   # 制約・禁止条件

  branch_conditions:     # 分岐条件一覧
    - id: string         # BC-XX-YY
      condition: string  # 分岐条件の記述
      true_action: string
      false_action: string

  equivalence_classes:   # 入力同値クラス
    - id: string         # EC-XX-YY
      parameter: string  # 対象パラメータ
      class_name: string # クラス名
      representative: string  # 代表値
      expected: string   # 期待される振る舞い

  boundary_values:       # 境界値
    - id: string         # BV-XX-YY
      parameter: string
      boundary: string   # 境界の説明
      expected: string

  dependencies: list     # 依存先一覧
  stub_strategy: string  # スタブ化方針

  observable_results: list[string]  # 期待される観測可能結果

  test_requirements:     # テスト要求ID一覧
    - id: string         # BR-xx / EC-xx / BV-xx / ER-xx / DP-xx
      description: string
      branch_ref: string | list  # 対応する分岐条件ID
      class_ref: string | list   # 対応する同値クラスID
      boundary_ref: string | list # 対応する境界値ID
      priority: enum[high, medium, low]
```

---

## 3. 対象関数一覧

| 対象ID | 関数名 | ファイル | 領域 |
|--------|--------|----------|------|
| TGT-01 | GetDateTimeFormat | sakura_core/util/format.cpp | 主実験 |
| TGT-02 | ParseVersion | sakura_core/util/format.cpp | 主実験 |
| TGT-03 | CompareVersion | sakura_core/util/format.cpp | 主実験 |
| TGT-04 | IsMailAddress | sakura_core/parse/CWordParse.cpp | 補助実験 |
| TGT-05 | WhatKindOfTwoChars | sakura_core/parse/CWordParse.cpp | 補助実験 |
| TGT-06 | WhatKindOfTwoChars4KW | sakura_core/parse/CWordParse.cpp | 補助実験 |
| TGT-07 | Convert_ZeneisuToHaneisu | sakura_core/convert/convert_util.cpp | オプション |
| TGT-08 | Convert_HaneisuToZeneisu | sakura_core/convert/convert_util.cpp | オプション |

---

## 4. 各対象のテスト要求詳細

### 4.1 TGT-01: GetDateTimeFormat

**関数シグネチャ**: `std::wstring GetDateTimeFormat(std::wstring_view format, const SYSTEMTIME& systime)`

**責務**: 書式文字列に従い SYSTEMTIME 構造体の日時情報を文字列に変換する。`%Y`, `%y`, `%m`, `%d`, `%H`, `%M`, `%S` の7種の書式指定子を認識する。

**前提条件**:
- format は有効な std::wstring_view
- systime は有効な SYSTEMTIME 構造体（値域チェックは関数側で行わない）

#### 分岐条件一覧

| ID | 条件 | True側動作 | False側動作 |
|----|------|-----------|-------------|
| BC-01-01 | f == L'Y' | wYear を %d で書式化 | 次の条件へ |
| BC-01-02 | f == L'y' | wYear%100 を %02d で書式化 | 次の条件へ |
| BC-01-03 | f == L'm' | wMonth を %02d で書式化 | 次の条件へ |
| BC-01-04 | f == L'd' | wDay を %02d で書式化 | 次の条件へ |
| BC-01-05 | f == L'H' | wHour を %02d で書式化 | 次の条件へ |
| BC-01-06 | f == L'M' | wMinute を %02d で書式化 | 次の条件へ |
| BC-01-07 | f == L'S' | wSecond を %02d で書式化 | 次の条件へ |
| BC-01-08 | % の後の未知文字 | その文字をそのまま出力 | - |
| BC-01-09 | f == L'%' | inSpecifier = true | 次の条件へ |
| BC-01-10 | f == L'\0' | break (ループ脱出) | 通常文字として出力 |

#### 入力同値クラス

| ID | パラメータ | クラス名 | 代表値 | 期待結果 |
|----|-----------|---------|--------|---------|
| EC-01-01 | format | 空文字列 | `L""` | 空文字列 |
| EC-01-02 | format | リテラル文字列のみ | `L"hello"` | そのまま |
| EC-01-03〜09 | format | 各指定子 (%Y,%y,%m,%d,%H,%M,%S) | 各指定子単体 | 対応値で置換 |
| EC-01-10 | format | 未知の指定子 %X | `L"%X"` | "X" がそのまま |
| EC-01-11 | format | %% (エスケープ) | `L"%%"` | "%" が出力 |
| EC-01-12 | format | null文字を中間に含む | `L"%Y\0%m"` | null手前まで |
| EC-01-13 | format | 複合書式 | `L"%Y-%m-%d %H:%M:%S"` | 全置換 |
| EC-01-14 | systime | 全フィールドゼロ | `SYSTEMTIME{0}` | 年=0,月=00... |
| EC-01-15 | format | 末尾が % | `L"test%"` | %が展開されずループ終了 |

#### 境界値

| ID | パラメータ | 境界 | 期待結果 |
|----|-----------|------|---------|
| BV-01-01 | wYear | 1桁 (wYear=1) | %Y→"1", %y→"01" |
| BV-01-02 | wYear | 100の倍数 (wYear=2000) | %y→"00" |
| BV-01-03 | wYear | 5桁 (wYear=12345) | %Y→"12345", %y→"45" |
| BV-01-04〜05 | wMonth | 1桁/2桁 | 0パディング確認 |
| BV-01-06〜07 | wHour | 0 / 23 | "00" / "23" |
| BV-01-08 | wSecond | 59 | "59" |

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-01〜07 | 分岐 | 各書式指定子の分岐通過 | high | 7つの if-else 分岐各々を通過する |
| BR-08 | 分岐 | 未知指定子の else 分岐 | high | default パスの検証 |
| BR-09 | 分岐 | % 文字の検出 | medium | inSpecifier フラグ遷移 |
| BR-10 | 分岐 | null 文字による break | medium | 文字列途中終了のパス |
| EC-01〜06 | 同値 | 各入力カテゴリの検証 | medium〜high | 同値分割の代表値テスト |
| BV-01〜04 | 境界 | 桁数や0パディングの境界 | medium | 0パディングが正しく動作する境界 |

---

### 4.2 TGT-02: ParseVersion

**関数シグネチャ**: `UINT32 ParseVersion(const WCHAR* sVer)`

**責務**: バージョン文字列を解析し、4コンポーネント x 8ビットの UINT32 値にパックする。alpha/beta/rc/pl の修飾子を認識し、比較可能な数値表現を生成する。

**前提条件**:
- sVer は null 終端された有効な WCHAR 文字列

#### 分岐条件一覧

| ID | 条件 | 動作 |
|----|------|------|
| BC-02-01 | *p == L'a' | alpha 判定 → nShift = -0x60 |
| BC-02-02 | *p == L'b' | beta 判定 → nShift = -0x40 |
| BC-02-03 | *p == L'r'/L'R' | rc 判定 → nShift = -0x20 |
| BC-02-04 | *p == L'p' | pl 判定 → nShift = +0x20 |
| BC-02-05 | !iswdigit(*p) (上記以外) | nShift = -0x80 (不明文字) |
| BC-02-06 | 数字で始まる | nShift = 0 |
| BC-02-07 | nDigit > 2 | 3桁目で数値抽出を打ち切る |
| BC-02-08 | *p が .-_+ | 区切り文字をスキップ |
| BC-02-09 | *p==0 \|\| i>=4 | ループ終了 → 残りを 0x80 で埋める |
| BC-02-10〜13 | 各修飾子の完全一致 vs 先頭文字のみ | alpha/beta/rc/pl のスキップ幅が変わる |

#### 入力同値クラス

| ID | クラス名 | 代表値 | 期待結果 |
|----|---------|--------|---------|
| EC-02-01 | 数字のみ（区切りなし） | `L"1234"` | 4コンポーネント解析 |
| EC-02-02 | ドット区切り標準 | `L"2.4.1.0"` | 正常解析 |
| EC-02-03〜07 | 各修飾子付き | alpha/beta/rc/RC/pl | 対応シフト値適用 |
| EC-02-08 | 不明文字修飾子 | `L"2.4.1x"` | nShift=-0x80 |
| EC-02-09 | 省略形修飾子 | `L"2.4.1a"` | alpha扱い |
| EC-02-10〜12 | 区切り文字バリエーション | `-` / `_` / `+` | ドットと同等 |
| EC-02-13 | 空文字列 | `L""` | 全コンポーネント 0x80 |
| EC-02-14 | 5コンポーネント以上 | `L"1.2.3.4.5"` | 5番目以降無視 |
| EC-02-15 | 修飾子のtypo | `L"2.4.1alfa"` | 先頭'a'で alpha 扱い |
| EC-02-16 | 修飾子+数字連結 | `L"2.4.1alpha1"` | alpha の後の数字も解析 |

#### 境界値

| ID | 境界 | 期待結果 |
|----|------|---------|
| BV-02-01 | 数値1桁 | 正常解析 |
| BV-02-02 | 数値2桁 (最大) | 99が最大 |
| BV-02-03 | 数値3桁 | 3桁目切り捨て |
| BV-02-04〜07 | コンポーネント数 0/1/4/5 | 未使用分は 0x80 |
| BV-02-08 | 最小コンポーネント値 (nShift=-0x80, nVer=0) | 0x00 |
| BV-02-09 | 最大コンポーネント値 (nShift=+0x20, nVer=99) | オーバーフロー可能性 |

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-11〜16 | 分岐 | 各修飾子・数字の分岐通過 | high | 6つの主要分岐パス |
| BR-17 | 分岐 | 3桁目break | medium | 数値長制限の検証 |
| BR-18 | 分岐 | 区切り文字スキップ | medium | 区切り文字の網羅 |
| BR-19 | 分岐 | ループ終了条件 | medium | 4コンポーネント超過時 |
| BR-20〜23 | 分岐 | 完全一致 vs 先頭文字一致 | medium | 修飾子の部分一致パス |
| EC-07〜10 | 同値 | 主要入力パターン | high〜medium | 同値分割の網羅 |
| BV-05〜06 | 境界 | 桁数・コンポーネント数の境界 | high〜medium | 切り捨てや0x80埋めの正確性 |
| ER-01 | エラー | 空文字列入力 | high | 堅牢性 |

---

### 4.3 TGT-03: CompareVersion

**関数シグネチャ**: `int CompareVersion(const WCHAR* verA, const WCHAR* verB)`

**責務**: 2つのバージョン文字列を比較し、大小関係を返す薄いラッパー関数。

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-24 | 分岐 | 同一バージョンで 0 が返る | high | 等価パス |
| BR-25 | 分岐 | A が新しい場合に正の値 | high | 大小パス |
| BR-26 | 分岐 | B が新しい場合に負の値 | high | 大小パス |
| EC-11 | 同値 | 修飾子の順序関係全パターン | high | x < a < b < rc < 無 < pl の検証 |
| EC-12 | 同値 | 区切り文字違いの等価性 | medium | 区切り文字の互換性 |
| DP-01 | 依存 | ParseVersion の結果を正しく利用 | medium | 統合テストとしての検証 |

**特記事項**: CompareVersion は `nVerA - nVerB` で計算するが、UINT32 同士の減算を int で返すため、極端に大きな差の場合にオーバーフローの可能性がある。ただし通常のバージョン番号の範囲では問題にならない。

---

### 4.4 TGT-04: IsMailAddress

**関数シグネチャ**: `BOOL IsMailAddress(const wchar_t* pszBuf, int offset, size_t cchBuf, int* pnAddressLength)`

**責務**: 文字列バッファ中の指定位置がメールアドレスの先頭であるかを判定する。

#### 分岐条件一覧

| ID | 条件 | 動作 |
|----|------|------|
| BC-04-01 | 0 < offset && 直前文字が有効 | FALSE（メールアドレスの途中） |
| BC-04-02 | 先頭がドット | FALSE |
| BC-04-03 | 先頭が無効文字 | FALSE |
| BC-04-04 | ローカルパートの長さ異常 | FALSE |
| BC-04-05 | @ が見つからない | FALSE |
| BC-04-06 | ドメインラベル長が0 | FALSE |
| BC-04-07 | ドメイン内のドット処理 | nDotCount++ or 終了判定 |
| BC-04-08 | nDotCount == 0 | FALSE |
| BC-04-09 | pnAddressLength != NULL | 長さを出力パラメータに設定 |

#### 入力同値クラス

| ID | クラス名 | 代表値 | 期待結果 |
|----|---------|--------|---------|
| EC-04-01 | 標準メールアドレス | `test@example.com` | TRUE |
| EC-04-02 | 空文字列 | `""` | FALSE |
| EC-04-03 | @ なし | `testexample.com` | FALSE |
| EC-04-04 | 先頭ドット | `.test@example.com` | FALSE |
| EC-04-05 | ドメインにドットなし | `test@example` | FALSE |
| EC-04-06 | 複数ドット (co.jp) | `test@example.co.jp` | TRUE |
| EC-04-07 | 記号類を含むローカルパート | `test!#$%&'*+-/=?^_`{|}~@example.com` | TRUE |
| EC-04-08 | 禁止文字を含む | ダブルクオート等 | FALSE |
| EC-04-09〜10 | ドメインにハイフン/アンダースコア | `-` / `_` を含む | TRUE |
| EC-04-11 | アドレス後にスペース | `test@example.com ` | TRUE (長さ指定あり) |
| EC-04-12〜14 | offset バリエーション | 0 / 正(有効) / 正(境界拒否) | 各パターン |
| EC-04-15 | pnAddressLength = NULL | NULL | クラッシュしない |

#### 境界値

| ID | 境界 | 期待結果 |
|----|------|---------|
| BV-04-01 | 最小ローカルパート (1文字) | TRUE |
| BV-04-02 | 64文字ローカルパート | TRUE（現行実装は制限なし） |
| BV-04-04 | ドメインドット数=0 | FALSE |
| BV-04-05 | ドメインドット数=1（最小有効） | TRUE |
| BV-04-07 | nBufLen 不足 | FALSE |
| BV-04-08〜11 | IsValidChar 境界 (0x20/0x21/0x7E/0x7F) | 有効/無効の切り替わり |

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-27〜35 | 分岐 | 各FALSEパスとTRUEパス | high〜medium | 9つの分岐条件の網羅 |
| EC-13〜16 | 同値 | メールアドレスの各形式 | high | RFC準拠の形式検証 |
| BV-07〜08 | 境界 | IsValidChar 境界と最小長 | medium | 文字コード境界の正確性 |
| ER-02 | エラー | 空文字列 | high | 堅牢性 |
| ER-03 | エラー | @ のみ | medium | 最小構造の不正入力 |

---

### 4.5 TGT-05: WhatKindOfTwoChars

**関数シグネチャ**: `static ECharKind CWordParse::WhatKindOfTwoChars(ECharKind kindPre, ECharKind kindCur)`

**責務**: 2つの文字種別が同一単語として結合可能かを判定する。日本語テキストエディタの単語選択基盤ロジック。

#### 分岐条件一覧

| ID | 条件 | 動作 |
|----|------|------|
| BC-05-01 | kindPre == kindCur (初回) | kindCur を返す |
| BC-05-02 | 長音/濁点 + カタカナ/ひらがな | kindCur を返す（引きずり） |
| BC-05-03 | カタカナ/ひらがな + 長音/濁点 | kindPre を返す（引きずり） |
| BC-05-04 | 長音/濁点 + 長音/濁点 | kindCur を返す（同種） |
| BC-05-05〜06 | LATIN → CSYM マッピング | マッピング適用 |
| BC-05-07〜08 | UDEF → ETC マッピング | マッピング適用 |
| BC-05-09〜10 | CTRL → ETC マッピング | マッピング適用 |
| BC-05-11 | kindPre == kindCur (マッピング後) | kindCur を返す |
| BC-05-12 | 上記全て不一致 | CK_NULL（別種） |

#### 入力同値クラスと引きずり規則

引きずり規則は日本語テキスト処理に固有の重要なロジックである:

| パターン | 例 | 結合結果 | 説明 |
|---------|----|---------|----- |
| 長音(ー) + カタカナ | ー + カ | CK_ZEN_KATA | 長音がカタカナに引きずられる |
| 濁点(゛) + ひらがな | ゛ + か | CK_HIRA | 濁点がひらがなに引きずられる |
| カタカナ + 長音(ー) | カ + ー | CK_ZEN_KATA | カタカナが長音を引きずる |
| ひらがな + 濁点(゛) | か + ゛ | CK_HIRA | ひらがなが濁点を引きずる |
| 長音 + 濁点 | ー + ゛ | CK_ZEN_DAKU | 長音・濁点の連続は同種 |

マッピング規則（通常版）:

| 変換元 | 変換先 | 説明 |
|--------|--------|------|
| CK_LATIN | CK_CSYM | ラテン系文字はアルファベット扱い |
| CK_UDEF | CK_ETC | ユーザー定義文字はその他の半角扱い |
| CK_CTRL | CK_ETC | 制御文字はその他の半角扱い |

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-36 | 分岐 | 初回同種チェック | high | 最も頻出するパス |
| BR-37 | 分岐 | 引きずり規則 | high | 日本語処理の核心ロジック |
| BR-38 | 分岐 | 長音・濁点連続 | high | 特殊パス |
| BR-39〜41 | 分岐 | 各マッピング規則 | high〜medium | マッピング後の同種判定 |
| BR-42 | 分岐 | マッピング後同種チェック | high | マッピング機能の検証 |
| BR-43 | 分岐 | 最終的な CK_NULL | high | 別種判定パス |
| EC-17 | 同値 | 全ECharKind値の同種ペア | high | 22種の enum 値各々 |
| EC-18 | 同値 | 引きずり規則の全組み合わせ | high | 8パターンの引きずり |
| EC-19 | 同値 | マッピング規則の各パターン | medium | LATIN/UDEF/CTRL の変換 |

---

### 4.6 TGT-06: WhatKindOfTwoChars4KW

**関数シグネチャ**: `static ECharKind CWordParse::WhatKindOfTwoChars4KW(ECharKind kindPre, ECharKind kindCur)`

**責務**: WhatKindOfTwoChars のキーワード検索用バリアント。

#### 通常版との差異点

| 文字種 | 通常版 (WhatKindOfTwoChars) | 4KW版 (WhatKindOfTwoChars4KW) |
|--------|---------------------------|------------------------------|
| CK_UDEF | → CK_ETC（その他の半角） | → CK_CSYM（アルファベット扱い） |
| CK_CTRL | → CK_ETC（その他の半角） | → CK_CTRL（そのまま） |

この差異により以下の結合判定結果が変わる:

| kindPre | kindCur | 通常版の結果 | 4KW版の結果 | 理由 |
|---------|---------|-------------|-------------|------|
| CK_UDEF | CK_ETC | CK_ETC (同種) | CK_NULL (別種) | UDEF→CSYM vs UDEF→ETC |
| CK_UDEF | CK_CSYM | CK_NULL (別種) | CK_CSYM (同種) | UDEF→CSYM vs UDEF→ETC |
| CK_CTRL | CK_ETC | CK_ETC (同種) | CK_NULL (別種) | CTRL→ETC vs CTRL→CTRL |

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-44 | 分岐 | UDEF → CSYM マッピング | high | 通常版との差異の検証 |
| BR-45 | 分岐 | CTRL → CTRL 保持 | high | 通常版との差異の検証 |
| EC-20 | 同値 | UDEF+CSYM が同種 | high | 差異パターンの具体的検証 |
| EC-21 | 同値 | UDEF+ETC が別種 | high | 差異パターンの具体的検証 |
| EC-22 | 同値 | CTRL+ETC が別種 | high | 差異パターンの具体的検証 |
| DP-02 | 依存 | 通常版と4KW版の直接比較 | high | 差異の網羅的な検証 |

---

### 4.7 TGT-07: Convert_ZeneisuToHaneisu

**関数シグネチャ**: `void Convert_ZeneisuToHaneisu(wchar_t* pData, int nLength)`

**責務**: 全角英数字・記号を半角英数字・記号に in-place 変換する。

#### 分岐条件一覧

| ID | 条件 | 変換規則 |
|----|------|---------|
| BC-07-01 | c >= L'A' && c <= L'Z' | → 'A' + (c - L'A') |
| BC-07-02 | c >= L'a' && c <= L'z' | → 'a' + (c - L'a') |
| BC-07-03 | c >= L'0' && c <= L'9' | → '0' + (c - L'0') |
| BC-07-04 | tableZenKigo にヒット | → tableHanKigo[n] |
| BC-07-05 | いずれも該当しない | 無変換 |

#### 記号変換テーブル

ソースコードの tableZenKigo / tableHanKigo に基づく対応表:

| 全角記号 | 半角記号 | Unicode |
|---------|---------|---------|
| `　` (全角スペース) | ` ` (半角スペース) | U+3000 → U+0020 |
| `，` | `,` | U+FF0C → U+002C |
| `．` | `.` | U+FF0E → U+002E |
| `＋` | `+` | U+FF0B → U+002B |
| `－` | `-` | U+FF0D → U+002D |
| `＊` | `*` | U+FF0A → U+002A |
| `／` | `/` | U+FF0F → U+002F |
| (以下略 -- 全27組) | | |

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-46〜50 | 分岐 | 各変換分岐パス (大文字/小文字/数字/記号/無変換) | high〜medium | 5つの分岐各々 |
| EC-23 | 同値 | 全カテゴリの変換正確性 | high | 各同値クラスの代表値 |
| EC-24 | 同値 | 変換対象外文字の保持 | medium | 非対象文字の安全性 |
| EC-25 | 同値 | 混在文字列の処理 | high | 実用的なシナリオ |
| BV-09 | 境界 | 全角文字範囲の境界 (先頭/末尾/範囲外) | high | off-by-one エラー防止 |
| BV-10 | 境界 | nLength=0 | medium | 空入力の安全性 |

---

### 4.8 TGT-08: Convert_HaneisuToZeneisu

**関数シグネチャ**: `void Convert_HaneisuToZeneisu(wchar_t* pData, int nLength)`

**責務**: 半角英数字・記号を全角英数字・記号に in-place 変換する（TGT-07 の逆変換）。

#### テスト要求一覧

| 要求ID | 種別 | 説明 | 優先度 | 根拠 |
|--------|------|------|--------|------|
| BR-51〜55 | 分岐 | 各変換分岐パス | high〜medium | TGT-07 の逆方向 |
| EC-26 | 同値 | 全カテゴリの変換正確性 | high | 半角→全角の各クラス |
| EC-27 | 同値 | 変換対象外文字の保持 | medium | 全角文字等の安全性 |
| BV-11 | 境界 | 半角文字範囲の境界 | high | 'A','Z','a','z','0','9' |
| DP-03 | 依存 | ラウンドトリップ検証 | high | 全角→半角→全角で元に戻ること |

---

## 5. テスト要求サマリ

### 5.1 全体統計

| 指標 | 値 |
|------|-----|
| 対象関数数 | 8 |
| 分岐条件定義数 | 63 |
| 同値クラス定義数 | 85 |
| 境界値定義数 | 48 |
| **テスト要求総数** | **99** |

### 5.2 種別内訳

| 種別 | 件数 | 割合 |
|------|------|------|
| BR (分岐網羅) | 55 | 55.6% |
| EC (同値クラス) | 27 | 27.3% |
| BV (境界値) | 11 | 11.1% |
| ER (エラーパス) | 3 | 3.0% |
| DP (依存切替) | 3 | 3.0% |

### 5.3 優先度内訳

| 優先度 | 件数 | 割合 |
|--------|------|------|
| high | 61 | 61.6% |
| medium | 36 | 36.4% |
| low | 2 | 2.0% |

### 5.4 領域別内訳

| 領域 | 対象数 | テスト要求数 |
|------|--------|-------------|
| 主実験 (format.cpp) | 3関数 (TGT-01〜03) | 46 |
| 補助実験 (CWordParse.cpp) | 3関数 (TGT-04〜06) | 34 |
| オプション (convert_util.cpp) | 2関数 (TGT-07〜08) | 19 |

---

## 6. 既存テストとの対応関係

### 6.1 test-format.cpp の既存テストカバー範囲

既存テストファイル `src/test/cpp/tests1/test-format.cpp` は以下のテスト要求をカバーしている:

| テスト名 | カバーするテスト要求 |
|---------|-------------------|
| format::GetDateTimeFormat | BR-01〜08, EC-03〜05, BV-01〜03 |
| format::CompareVersion_SameVersion | BR-24, EC-11, EC-12 |
| format::CompareVersion_NewerIsA | BR-25 |
| format::CompareVersion_NewerIsB | BR-26, EC-11 |

**既存テストで未カバーの要求**: EC-01 (空文字列), EC-06 (末尾%), BV-04 (時の境界値), ER-01 (空文字列入力)

### 6.2 test-is_mailaddress.cpp の既存テストカバー範囲

| テスト名 | カバーするテスト要求 |
|---------|-------------------|
| testIsMailAddress::CheckBlank | ER-02 |
| testIsMailAddress::CheckExample | EC-13 |
| testIsMailAddress::CheckPunctuation | EC-15 |
| testIsMailAddress::OffsetParameter | EC-16 |
| testIsMailAddress::OffsetParameter2 | BR-27, EC-16 (網羅的) |

**既存テストで未カバーの要求**: BV-07 (IsValidChar 境界の明示的テスト), BR-32 (ドメインラベル長0), ER-03 (@ のみ)

---

## 7. 次フェーズへの入力仕様

本テスト要求モデルは `test-requirements.yaml` としてYAML形式で構造化されており、Phase 3（テストコード生成）でLLMへの入力として使用される。LLMに渡す際の推奨プロンプト構造:

```
以下のテスト要求モデル(YAML)に基づき、Google Test フレームワークを用いた
C++ 単体テストコードを生成してください。

対象: {target.id} - {target.function_signature}
責務: {target.responsibility}

テスト要求:
{test_requirements のリスト}

分岐条件:
{branch_conditions のリスト}

同値クラス:
{equivalence_classes のリスト}

境界値:
{boundary_values のリスト}
```

各テスト要求に対して最低1つのテストケースが生成されることを期待する。

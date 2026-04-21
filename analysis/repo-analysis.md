# sakura-editor/sakura リポジトリ解析レポート

## 1. リポジトリ概要

| 項目 | 値 |
|------|-----|
| リポジトリ | https://github.com/sakura-editor/sakura |
| 概要 | SAKURA Editor (Japanese text editor for MS Windows) |
| 言語 | C++ |
| デフォルトブランチ | master |
| Star数 | 1,425 |
| Fork数 | 186 |
| ライセンス | Zlib |

sakura-editor は Windows 向けの日本語テキストエディタであり、C++ で記述された歴史の長いオープンソースプロジェクトである。GUI コンポーネント、文字コード変換、テキスト解析、ファイル I/O など多岐にわたる機能を持つ。

---

## 2. ディレクトリ構成

```
sakura/
├── sakura_core/           # メインソースコード
│   ├── _main/             # エントリポイント、グローバル定義
│   ├── _os/               # OS依存処理
│   ├── basis/             # 基本型定義（CEol, CMyString, CLaxInteger, CStrictInteger等）
│   ├── charset/           # 文字コード変換（CCodeBase, CESI, CJis, CUtf8等）
│   ├── cmd/               # コマンド処理
│   ├── config/            # 設定管理
│   ├── convert/           # 文字変換（全角半角、タブ空白、Base64、トリム等）
│   ├── debug/             # デバッグ支援
│   ├── dlg/               # ダイアログ関連
│   ├── doc/               # ドキュメントモデル
│   ├── env/               # 環境情報
│   ├── func/              # 機能管理
│   ├── grep/              # Grep機能
│   ├── io/                # ファイル入出力
│   ├── macro/             # マクロ機能
│   ├── mem/               # メモリ管理（CMemory, CNative等）
│   ├── outline/           # アウトライン解析
│   ├── parse/             # テキスト解析（CWordParse, URL/メール判定等）
│   ├── plugin/            # プラグイン機構
│   ├── print/             # 印刷関連
│   ├── prop/              # プロパティ画面
│   ├── types/             # タイプ別設定
│   ├── uiparts/           # UI部品
│   ├── util/              # ユーティリティ（format, string_ex, zoom等）
│   ├── view/              # エディタビュー
│   └── window/            # ウィンドウ管理
├── src/
│   └── test/              # テストコード
│       ├── cpp/tests1/    # Google Test ベースの単体テスト群
│       ├── cmake/         # CMakeベースのテスト設定
│       ├── msbuild/       # MSBuildベースのテスト設定
│       └── resources/     # テストリソース
├── help/                  # ヘルプファイル
├── installer/             # インストーラー
├── tools/                 # ビルド補助ツール
├── externals/             # 外部ライブラリ
├── CMakeLists.txt
└── sakura.sln
```

---

## 3. 既存テストの状況

### テストフレームワーク
- **Google Test** を使用（src/test/msbuild/googletest.targets, googletest.props）
- **テストプロジェクト**: `sakura_core/tests1.vcxproj` および CMake 対応

### 既存テストファイル一覧（src/test/cpp/tests1/）

| テストファイル | テスト対象 | テスト充実度 |
|---|---|---|
| test-format.cpp | GetDateTimeFormat, CompareVersion, ParseVersion | 充実 |
| test-is_mailaddress.cpp | IsMailAddress | 充実（境界値テスト含む） |
| test-cconvert.cpp | 全角半角変換、タブ空白変換、トリム等 | 充実 |
| test-cdecode.cpp | Base64Decode, UuDecode | パラメタライズドテスト |
| test-zoom.cpp | GetZoomedValue | 充実（境界条件含む） |
| test-ceol.cpp | CEol クラス | あり |
| test-cnative.cpp | CNativeW/CNativeA | あり |
| test-cmemory.cpp | CMemory クラス | あり |
| test-ccodebase.cpp | 文字コード変換 | あり |
| test-cwordparse.cpp | CWordParse | あり |
| test-charcode.cpp | charcode ユーティリティ | あり |
| test-string_ex.cpp | string_ex ユーティリティ | あり |
| test-cfileext.cpp | CFileExt | あり |
| test-clayoutint.cpp | CLayoutInt | あり |
| test-cerrorinfo.cpp | CErrorInfo | あり |
| test-editinfo.cpp | EditInfo | あり |
| test-grepinfo.cpp | GrepInfo | あり |

### 所見
- 既存テストは Google Test で体系的に整備されている
- テストの多くは `sakura_core/util/`, `sakura_core/convert/`, `sakura_core/basis/`, `sakura_core/parse/` の関数を対象
- パラメタライズドテスト（`testing::TestWithParam`）も活用されている
- **既にテストが存在する関数群も実験対象として有効**（LLM 生成テストと人手テストの比較が可能）

---

## 4. 実験候補トップ5（詳細な表形式）

### 候補1: `GetDateTimeFormat` / `ParseVersion` / `CompareVersion`

| 項目 | 内容 |
|------|------|
| **ファイル** | `sakura_core/util/format.cpp` (約150行) |
| **関数名** | `GetDateTimeFormat`, `ParseVersion`, `CompareVersion` |
| **主な責務** | 日時書式変換、バージョン番号の解析・比較 |
| **想定入力** | `GetDateTimeFormat`: 書式文字列（`%Y-%m-%d`等）+ SYSTEMTIME構造体 |
|  | `ParseVersion`: バージョン文字列（`"2.4.1alpha"`等） |
|  | `CompareVersion`: 2つのバージョン文字列 |
| **想定出力** | `GetDateTimeFormat`: 変換済み文字列 |
|  | `ParseVersion`: UINT32（4バイトにパックされたバージョン） |
|  | `CompareVersion`: int（正/負/0） |
| **主な分岐** | `GetDateTimeFormat`: 書式指定子（`%Y`,`%y`,`%m`,`%d`,`%H`,`%M`,`%S`）+ `%`後の不明文字 + null終端 |
|  | `ParseVersion`: alpha/beta/rc/pl/不明文字の識別、数字2桁制限、区切り文字(`.`,`-`,`_`,`+`) |
| **テストしやすさ** | 極めて高い -- 純粋関数、状態なし |
| **スタブ必要性** | なし（`GetDateTimeFormat`はSYSTEMTIMEを直接渡すため外部依存なし） |
| **既存テスト** | あり（test-format.cpp: 3テストケース） |
| **実験対象優先度** | **最高 (S)** |

### 候補2: `IsMailAddress`

| 項目 | 内容 |
|------|------|
| **ファイル** | `sakura_core/parse/CWordParse.cpp` (約80行、522-601行目) |
| **関数名** | `IsMailAddress` |
| **主な責務** | 文字列中のメールアドレス判定 |
| **想定入力** | 文字列バッファ、オフセット、バッファ長 |
| **想定出力** | BOOL（TRUE/FALSE）、アドレス長（出力パラメータ） |
| **主な分岐** | (1) offset境界判定（直前文字が有効文字か） (2) ローカルパート先頭文字チェック（`.`不可） (3) `@`の検出 (4) ドメインパートのループ（英数字/ハイフン/アンダースコア） (5) ドット区切りの検証 (6) ドット数0の検出 |
| **テストしやすさ** | 高い -- 入力は文字列のみ、出力はBOOL+長さ |
| **スタブ必要性** | なし |
| **既存テスト** | あり（test-is_mailaddress.cpp: 約15テストケース、境界値網羅的） |
| **実験対象優先度** | **最高 (S)** |

### 候補3: `WhatKindOfTwoChars` / `WhatKindOfTwoChars4KW`

| 項目 | 内容 |
|------|------|
| **ファイル** | `sakura_core/parse/CWordParse.cpp` (187-236行目、各約25行) |
| **関数名** | `CWordParse::WhatKindOfTwoChars`, `CWordParse::WhatKindOfTwoChars4KW` |
| **主な責務** | 2つの文字種類の結合判定（単語選択の基盤ロジック） |
| **想定入力** | `ECharKind kindPre`（前方文字種）, `ECharKind kindCur`（後方文字種） -- enum値の組み合わせ |
| **想定出力** | `ECharKind` -- 結合後の文字種別、または `CK_NULL`（別種判定） |
| **主な分岐** | (1) 同種チェック (2) 全角長音/濁点 + ひらがな/カタカナの引きずり (3) 濁点+長音の連続 (4) ラテン文字→CSYMマッピング (5) ユーザー定義文字→ETC/CSYMマッピング (6) 制御文字マッピング (7) 最終同種チェック |
| **テストしやすさ** | 極めて高い -- 純粋関数、enum入力のみ、組み合わせテスト向き |
| **スタブ必要性** | なし |
| **既存テスト** | test-cwordparse.cpp にあるが、この関数の網羅的テストは未確認 |
| **実験対象優先度** | **最高 (S)** |

### 候補4: `Convert_ZeneisuToHaneisu` / `Convert_HaneisuToZeneisu` （全角英数 <-> 半角英数変換）

| 項目 | 内容 |
|------|------|
| **ファイル** | `sakura_core/convert/convert_util.cpp` (123-167行目) |
| **関数名** | `Convert_ZeneisuToHaneisu`, `Convert_HaneisuToZeneisu` + 内部ヘルパー `ZeneisuToHaneisu_`, `HaneisuToZeneisu_` |
| **主な責務** | 全角英数字・記号と半角英数字・記号の相互変換 |
| **想定入力** | wchar_t配列（変換対象文字列）+ 文字列長 |
| **想定出力** | 配列のin-place書き換え（文字数不変） |
| **主な分岐** | `ZeneisuToHaneisu_`: (1) 全角大文字A-Z (2) 全角小文字a-z (3) 全角数字0-9 (4) 全角記号テーブル照合 (5) 無変換 |
|  | `HaneisuToZeneisu_`: 同様の逆方向5分岐 |
| **テストしやすさ** | 高い -- in-place変換だが文字数不変、入出力が明確 |
| **スタブ必要性** | なし（`wcschr_idx`はファイル内ローカル関数） |
| **既存テスト** | test-cconvert.cpp に `CConvert_ZeneisuToHaneisu` のテストが存在する可能性あり |
| **実験対象優先度** | **高 (A)** |

### 候補5: `GetZoomedValue` / `GetPositionInTable` / `GetQuantizedValue`

| 項目 | 内容 |
|------|------|
| **ファイル** | `sakura_core/util/zoom.cpp` (約177行) |
| **関数名** | `GetZoomedValue`（公開API）, `GetPositionInTable`, `GetQuantizedValue`（内部関数） |
| **主な責務** | ズーム倍率テーブルに基づく段階的ズーム値の算出 |
| **想定入力** | ZoomSetting（倍率テーブル、上下限、最小単位）、基準値、現在ズーム、ステップ数 |
| **想定出力** | bool（ズーム可否）、ズーム後の値、ズーム後の倍率（出力パラメータ） |
| **主な分岐** | (1) nSteps==0 (2) 拡大/縮小方向判定 (3) テーブル範囲外チェック (4) 値の上下限クランプ (5) 値変化なし判定 (6) nullptr出力チェック |
|  | `GetPositionInTable`: テーブル内の位置検索（一致/非一致/範囲外） |
|  | `GetQuantizedValue`: 丸め処理（最小単位が0の場合の分岐） |
| **テストしやすさ** | 高い -- 数値入出力のみ、ZoomSettingは構造体として構築可能 |
| **スタブ必要性** | なし |
| **既存テスト** | あり（test-zoom.cpp: 5テストケース、充実） |
| **実験対象優先度** | **高 (A)** |

---

## 5. 各候補の採用理由

### 候補1: format.cpp (`GetDateTimeFormat`, `ParseVersion`, `CompareVersion`)
- **静的解析との相性**: 書式指定子ごとの if-else チェイン（7分岐 + default）、ParseVersion の特殊文字列処理（alpha/beta/rc/pl/不明の5分岐）は、静的解析で分岐条件を自動抽出する題材として理想的。
- **入力同値クラス定義**: 書式指定子の種類、バージョン文字列のパターン（数字のみ、alpha付き、区切り文字種別等）により同値クラスを明確に定義できる。
- **論文説明性**: コード量が適度（各関数20-60行）で、責務が明確。日時フォーマットとバージョン比較という直感的に理解しやすいドメイン。
- **既存テストとの比較**: 既存テスト（test-format.cpp）が存在するため、LLM 生成テストの品質を人手テストと定量比較できる。

### 候補2: CWordParse.cpp (`IsMailAddress`)
- **静的解析との相性**: メールアドレスの RFC に基づく複雑な条件分岐（ローカルパート/ドメインパート/境界判定）を持つ。分岐パスが複雑だが構造化されている。
- **入力同値クラス定義**: 正常なメールアドレス、`@`なし、ドメイン不正、記号含有、オフセット境界等の同値クラスが自然に定義できる。
- **論文説明性**: メールアドレス検証は一般に理解されやすいドメインであり、テスト生成の妥当性を読者が直感的に判断できる。
- **既存テストとの比較**: test-is_mailaddress.cpp に充実したテストケースがあり、LLM が同等以上のケースを生成できるかの検証に最適。

### 候補3: CWordParse.cpp (`WhatKindOfTwoChars`)
- **静的解析との相性**: enum 値同士の組み合わせによる分岐であり、静的解析で全分岐パスを列挙しやすい。日本語文字種（ひらがな、カタカナ、漢字等）の分類ルールが明示的。
- **入力同値クラス定義**: `ECharKind` は20種程度の enum であり、ペアの組み合わせ（20x20=400通り）から意味のある同値クラスを体系的に抽出できる。
- **論文説明性**: 日本語テキストエディタ固有の「単語区切り判定」という特徴的な機能であり、論文の独自性を示せる。2つの関数（通常版/強調キーワード版）の差異比較も題材になる。

### 候補4: convert_util.cpp（全角半角英数変換）
- **静的解析との相性**: 文字範囲による明確な条件分岐（`>='A' && <='Z'`等）とテーブル参照の組み合わせ。分岐条件が数値範囲で表現されるため、同値分割が容易。
- **入力同値クラス定義**: 全角大文字/全角小文字/全角数字/全角記号/変換対象外、の5クラスが明確。
- **論文説明性**: 文字変換は入力と出力の対応が1対1であり、テスト結果の正否判定が簡単。

### 候補5: zoom.cpp (`GetZoomedValue`)
- **静的解析との相性**: 数値条件による複合的な分岐（ステップ0、拡大/縮小方向、テーブル範囲外、値上下限超過等）。浮動小数点演算を含む条件分岐。
- **入力同値クラス定義**: ステップ値（正/負/0）、基準値（範囲内/外）、現在ズーム（テーブル内/外）の組み合わせ。
- **論文説明性**: ズーム機能は直感的に理解でき、数値入出力でテスト結果の妥当性検証が容易。純粋関数であり副作用がない。

---

## 6. 除外候補と除外理由

| 除外候補 | ファイル | 除外理由 |
|----------|----------|----------|
| `DetectIndentationStyle` | parse/DetectIndentationStyle.cpp | `CEditDoc*` への依存が強く、スタブ化が困難。ファイルシステムアクセス（.editorconfigの探索）を含む |
| `CConvert_TabToSpace::DoConvert` | convert/CConvert_TabToSpace.cpp | `CNativeW`, `CEol`, `GetNextLineW` への依存が多い。クラスメンバ変数（`m_nTabWidth`, `m_nStartColumn`等）への依存あり |
| `CConvert_SpaceToTab::DoConvert` | convert/CConvert_SpaceToTab.cpp | 同上。加えて `new/delete` によるメモリ管理を含み、バッファサイズ計算のための二重ループ構造が複雑 |
| `CConvert_Trim::DoConvert` | convert/CConvert_Trim.cpp | `GetNextLineW`, `CNativeW::GetSizeOfChar` 等への依存。動的メモリ確保あり |
| `WhereCurrentWord_2` | parse/CWordParse.cpp | `CNativeW::GetCharPrev`, `CNativeW::GetSizeOfChar` への依存。ポインタ演算が多く、テスト設定が複雑 |
| `SearchNextWordPosition` | parse/CWordParse.cpp | `CNativeW::GetSizeOfChar` への依存、`WhatKindOfChar` のチェイン呼び出しにより、単独テストには不向き |
| `WhatKindOfChar` | parse/CWordParse.cpp | `CNativeW::GetSizeOfChar`, `WCODE::Is*` 系関数群、`gm_keyword_char` グローバルテーブルへの依存が多い |
| `CDecode_Base64Decode::DoDecode` | convert/CDecode_Base64Decode.cpp | `CNativeW`, `CMemory` クラスへの依存。既にパラメタライズドテストが充実しており追加実験の価値が相対的に低い |
| `IsURL` | parse/CWordParse.cpp | `IsMailAddress` を内部呼び出し。`url_table` のconstexprテーブル依存。分岐構造は興味深いが、`IsMailAddress` との結合が強い |
| `CStrictInteger` (全体) | basis/CStrictInteger.h | テンプレートクラスであり、コンパイル時の型チェックが主目的。ランタイム分岐が少なく、テスト要求モデル生成の題材として不適 |
| `CLaxInteger` | basis/CLaxInteger.h | ほぼ `int` のラッパーで分岐条件が皆無。テストとしての価値が低い |

---

## 7. 最終推奨する1～3領域

### 推奨領域1（最優先）: `sakura_core/util/format.cpp` -- バージョン解析・比較

**対象関数**: `ParseVersion`, `CompareVersion`, `GetDateTimeFormat`

**推奨理由**:
- **3関数で1ファイル完結**（約150行）であり、論文の実証実験として扱いやすいサイズ
- `ParseVersion` は複合的な分岐構造（特殊文字列alpha/beta/rc/pl + 数値抽出 + 区切り文字処理）を持ち、静的解析で分岐条件を抽出するデモとして最適
- `CompareVersion` は `ParseVersion` を内部呼び出しする薄いラッパーであり、統合テストの階層を示せる
- `GetDateTimeFormat` は書式指定子による明確な7+1分岐を持ち、同値分割の教科書的な適用が可能
- **外部依存ゼロ**（SYSTEMTIME は構造体として渡すだけ、Windows API 呼び出しなし）
- 既存テスト（test-format.cpp）があるため、**LLM 生成テストと人手テストの定量比較**が可能

### 推奨領域2: `sakura_core/parse/CWordParse.cpp` -- メールアドレス判定 + 文字種結合判定

**対象関数**: `IsMailAddress`, `WhatKindOfTwoChars`, `WhatKindOfTwoChars4KW`

**推奨理由**:
- `IsMailAddress` は実用的なメールアドレス検証ロジック（約80行）で、RFC に基づく複雑な条件分岐を持つ
- `WhatKindOfTwoChars` / `WhatKindOfTwoChars4KW` は enum 値のペア入力に対する純粋関数であり、静的解析で分岐網羅テーブルを自動生成する実験に最適
- 2つの `WhatKindOfTwoChars` 関数は**ほぼ同じ構造だが一部の分岐が異なる**ため、差分に着目したテスト生成の題材として価値がある
- `IsMailAddress` には充実した既存テスト（test-is_mailaddress.cpp）があり、LLM の境界値分析能力を評価できる

### 推奨領域3: `sakura_core/convert/convert_util.cpp` -- 全角半角文字変換

**対象関数**: `Convert_ZeneisuToHaneisu`, `Convert_HaneisuToZeneisu`, `Convert_ZenhiraToZenkata`, `Convert_ZenkataToZenhira`

**推奨理由**:
- 文字範囲チェックによる明確な条件分岐（数値範囲で表現される同値クラス）
- 入出力が1対1対応であるため、テスト期待値の設定が容易
- in-place変換のうち文字数不変のもの（全角英数<->半角英数、ひらがな<->カタカナ）を選定することで、テスト設計を簡潔にできる
- 日本語テキスト処理という**ドメイン固有の課題**であり、LLM がどこまで日本語文字の同値クラスを理解できるかの評価に有用

---

## 付録: 各候補関数の分岐条件サマリ

### ParseVersion の分岐ツリー

```
ループ (i=0; i<4; 文字列終端まで):
  ├── *p == 'a' → alpha判定 → nShift = -0x60
  ├── *p == 'b' → beta判定  → nShift = -0x40
  ├── *p == 'r'/'R' → rc判定 → nShift = -0x20
  ├── *p == 'p' → pl判定    → nShift = +0x20
  ├── !iswdigit(*p)         → nShift = -0x80 (不明文字)
  └── (数字)                → nShift = 0
  数値抽出: 2桁制限ループ
  区切り文字スキップ: '.' '-' '_' '+'
残り部分: 0x80 (signed 0) で埋める
```

### IsMailAddress の分岐ツリー

```
offset境界判定:
  └── 0 < offset && 直前文字が有効 → FALSE

ローカルパート:
  ├── 先頭文字 == '.' → FALSE
  ├── 先頭文字が無効文字 → FALSE
  └── 有効文字の連続をスキップ

@マーク検出:
  └── pszBuf[j] != '@' → FALSE

ドメインパート (無限ループ):
  ├── ラベル内文字: [a-zA-Z0-9] / '-' / '_' をスキップ
  ├── ラベル長 == 0 → FALSE
  ├── '.' あり → nDotCount++, 次のラベルへ
  └── '.' なし:
      ├── nDotCount == 0 → FALSE
      └── nDotCount >= 1 → 終了（アドレス長を設定）
```

### WhatKindOfTwoChars の分岐マトリクス（概要）

```
kindPre == kindCur → kindCur を返す (同種)

全角長音/濁点 + カタカナ/ひらがな → 引きずられる側を返す
カタカナ/ひらがな + 全角長音/濁点 → 引きずる側を返す
全角長音/濁点 + 全角長音/濁点 → kindCur を返す

ラテン → CSYM にマッピング
ユーザー定義 → ETC にマッピング (通常版) / CSYM にマッピング (4KW版) ← 差異点
制御文字 → ETC にマッピング (通常版) / CTRL のまま (4KW版) ← 差異点

マッピング後 kindPre == kindCur → kindCur を返す (同種)
それ以外 → CK_NULL (別種)
```

---

## 実験設計への示唆

1. **推奨領域1（format.cpp）を主実験**とし、静的解析→テスト要求モデル→テストコード生成の全パイプラインを実証する
2. **推奨領域2（WhatKindOfTwoChars）を補助実験**とし、enum値の組み合わせに対するテスト網羅性をLLM vs 人手で比較する
3. **推奨領域3（convert_util.cpp）をオプション実験**とし、日本語文字ドメインに対するLLMの同値クラス理解度を評価する
4. 全候補に既存テストが存在するため、**既存テストのカバレッジ**をベースラインとして、LLM 生成テストが追加的にカバーできる分岐を測定する実験設計が可能

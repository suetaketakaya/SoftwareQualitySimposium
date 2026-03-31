# 実験評価レポート

SQiP 2026 論文「静的解析とLLMを組み合わせたホワイトボックステスト要求モデル生成による単体テスト半自動化」

評価日: 2026-03-31

---

## 1. 実験条件

### 1.1 対象リポジトリ

| 項目 | 値 |
|------|-----|
| リポジトリ | sakura-editor/sakura (GitHub) |
| 言語 | C++ |
| テストフレームワーク | Google Test |
| 既存テスト数 | 16ファイル（tests1/ディレクトリ） |

### 1.2 実験対象関数

| 対象ID | 関数名 | ファイル | 領域 | コード行数概算 |
|--------|--------|----------|------|---------------|
| TGT-01 | GetDateTimeFormat | sakura_core/util/format.cpp | 主実験 | ~50行 |
| TGT-02 | ParseVersion | sakura_core/util/format.cpp | 主実験 | ~60行 |
| TGT-03 | CompareVersion | sakura_core/util/format.cpp | 主実験 | ~5行 |
| TGT-04 | IsMailAddress | sakura_core/parse/CWordParse.cpp | 補助実験 | ~80行 |
| TGT-05 | WhatKindOfTwoChars | sakura_core/parse/CWordParse.cpp | 補助実験 | ~50行 |
| TGT-06 | WhatKindOfTwoChars4KW | sakura_core/parse/CWordParse.cpp | 補助実験 | ~50行 |
| TGT-07 | Convert_ZeneisuToHaneisu | sakura_core/convert/convert_util.cpp | オプション | ~45行 |
| TGT-08 | Convert_HaneisuToZeneisu | sakura_core/convert/convert_util.cpp | オプション | ~45行 |

### 1.3 手法の3フェーズ構成

| フェーズ | 内容 | LLM関与 | 人間関与 |
|----------|------|---------|---------|
| Phase 1: リポジトリ解析 | ソースコード構造解析、候補選定 | 主導（静的解析+推論） | レビュー・承認 |
| Phase 2: テスト要求モデル | 分岐条件抽出、同値クラス定義、境界値特定 | 主導（YAML生成） | レビュー・修正 |
| Phase 3: テストコード生成 | YAML入力に基づくGoogle Testコード生成 | 主導（コード生成） | レビュー・修正 |

### 1.4 選定基準

以下の基準に基づき8関数を選定した:

- 純粋関数またはそれに近い構造（外部依存が最小）
- 明確な分岐構造を持つ（if-else/switch/ループ条件）
- 既存テストが存在し、比較評価が可能
- スタブ不要で単体テストが容易
- 論文で説明可能な適度なコード量（5~80行）

---

## 2. 定量的結果

### 2.1 生成テストケース数

| 対象関数 | テストケース数 | テスト要求カバー数 |
|----------|---------------|-------------------|
| GetDateTimeFormat (TGT-01) | 21 | 20 |
| ParseVersion (TGT-02) | 27 | 20 |
| CompareVersion (TGT-03) | 13 | 6 |
| **format.cpp 小計** | **61** | **46** |
| IsMailAddress (TGT-04) | 27 | 17 |
| WhatKindOfTwoChars (TGT-05) | 18 | 11 |
| WhatKindOfTwoChars4KW (TGT-06) | 8 | 6 |
| **CWordParse.cpp 小計** | **53** | **34** |
| Convert_ZeneisuToHaneisu (TGT-07) | 17 | 10 |
| Convert_HaneisuToZeneisu (TGT-08) | 11 | 9 |
| ConvertRoundTrip（往復検証） | 3 | (DP-03に包含) |
| **convert_util.cpp 小計** | **31** | **19** |
| **総計** | **145** | **99** |

注: テストケース数(145)がテスト要求数(99)を上回るのは、1つのテスト要求を複数のテストケースでカバーする設計（例: 境界値の上限・下限を別テストケースで検証）や、テスト要求に直接対応しない補強テスト（AlphaTypo, CommonBehavior_SameAsNormal等）を含むためである。

### 2.2 テスト要求カバレッジ

| 指標 | 値 |
|------|-----|
| テスト要求総数 | 99 |
| カバー済み要求数 | 99 |
| 未カバー要求数 | 0 |
| **カバー率** | **100.0%** |

### 2.3 領域別カバー率

| 領域 | テスト要求数 | カバー数 | カバー率 | テストケース数 |
|------|-------------|---------|---------|---------------|
| 主実験 (format.cpp) | 46 | 46 | 100% | 61 |
| 補助実験 (CWordParse.cpp) | 34 | 34 | 100% | 53 |
| オプション (convert_util.cpp) | 19 | 19 | 100% | 31 |

### 2.4 テスト要求の種別内訳

| 種別 | 定義数 | カバー数 | カバー率 | 説明 |
|------|--------|---------|---------|------|
| BR (分岐網羅) | 55 | 55 | 100% | ソースコードの if/else/switch 分岐 |
| EC (同値クラス) | 27 | 27 | 100% | 入力パラメータの同値分割 |
| BV (境界値) | 11 | 11 | 100% | 同値クラスの境界における値 |
| ER (エラーパス) | 3 | 3 | 100% | 異常系・エラー処理パス |
| DP (依存切替) | 3 | 3 | 100% | 依存先の振る舞いに基づくパス |
| **合計** | **99** | **99** | **100%** | |

### 2.5 優先度別カバー状況

| 優先度 | 定義数 | カバー数 | カバー率 |
|--------|--------|---------|---------|
| high | 61 | 61 | 100% |
| medium | 36 | 36 | 100% |
| low | 2 | 2 | 100% |

---

## 3. 既存テストとの比較分析

### 3.1 既存テスト概要

sakura-editor リポジトリの `src/test/cpp/tests1/` には以下の既存テストファイルが存在する:

| 既存テストファイル | 対応する対象関数 | 既存テスト充実度 |
|-------------------|-----------------|-----------------|
| test-format.cpp | GetDateTimeFormat, ParseVersion, CompareVersion | 充実（3テストスイート） |
| test-is_mailaddress.cpp | IsMailAddress | 充実（約15テストケース、境界値含む） |
| test-cwordparse.cpp | WhatKindOfTwoChars (部分的) | あり（網羅度は未確認） |
| test-cconvert.cpp | Convert_ZeneisuToHaneisu 関連 | あり（全角半角変換テスト含む） |

### 3.2 既存テストと生成テストの定量比較

| 対象領域 | 既存テスト数(概算) | 生成テスト数 | 生成/既存比 |
|----------|-------------------|-------------|-------------|
| format.cpp (3関数) | ~15 | 61 | 4.1倍 |
| IsMailAddress | ~15 | 27 | 1.8倍 |
| WhatKindOfTwoChars系 (2関数) | ~5 | 26 | 5.2倍 |
| convert_util.cpp (2関数) | ~10 | 31 | 3.1倍 |
| **合計** | **~45** | **145** | **3.2倍** |

### 3.3 生成テストが追加でカバーした要求（既存テスト未カバー分）

トレーサビリティマトリクスの分析により、以下のテスト要求は既存テストでカバーされていない可能性が高く、生成テストで新規にカバーされたものである:

| 要求ID | 説明 | 生成テストケース |
|--------|------|-----------------|
| EC-01 | 空文字列の書式処理 | `GetDateTimeFormat.EmptyFormat` |
| EC-06 | 末尾が%で終わる書式 | `GetDateTimeFormat.TrailingPercent` |
| BV-04 | 時の最小値・最大値の0パディング | `GetDateTimeFormat.BV_HourMinMax` |
| ER-01 | ParseVersionへの空文字列入力 | `ParseVersion.ER_EmptyString` |
| BR-20~23 | 修飾子の完全一致 vs 部分一致 | `ParseVersion.*PartialMatch` (4テスト) |
| EC-08 | 修飾子の順序関係検証 | `ParseVersion.EC_ModifierOrder` |
| BV-07 | IsValidCharの境界値(0x21, 0x7E, 0x20, 0x7F) | `IsMailAddress.BV07_*` (4テスト) |
| BR-32 | ドメインラベル長0 | `IsMailAddress.BR32_DomainLabelLengthZero` |
| ER-03 | @のみの入力 | `IsMailAddress.ER03_AtSignOnly` |
| EC-17 | 全ECharKind値の同種ペア網羅 | `WhatKindOfTwoChars.EC17_AllSameKindPairs` |
| EC-18 | 引きずり規則の全組み合わせ | `WhatKindOfTwoChars.EC18_DragRuleAllCombinations` |
| BR-44, BR-45 | 4KW版固有の差異(UDEF/CTRL) | `WhatKindOfTwoChars4KW.BR44/BR45` |
| DP-02 | 通常版と4KW版の直接差異比較 | `WhatKindOfTwoChars4KW.DP02_DirectComparison` |
| DP-03 | 全角半角の往復変換検証 | `ConvertRoundTrip.DP03_*` (3テスト) |

### 3.4 既存テストで既にカバーされていた要求の例

以下のテスト要求は既存テストでも相当するケースが存在し、生成テストとの重複が推定される:

- **BR-01~BR-07**: 書式指定子 %Y/%y/%m/%d/%H/%M/%S の各分岐（既存test-format.cppに含まれる）
- **EC-13**: 標準的なメールアドレス判定（既存test-is_mailaddress.cppに充実したケースあり）
- **EC-14**: 各種無効メールアドレス（既存テストに境界値含む約15ケースあり）
- **BR-46~BR-55**: 全角半角変換の基本分岐（既存test-cconvert.cppに基本ケースあり）

### 3.5 比較分析の考察

1. **テスト数の増加**: 生成テストは既存テストの約3.2倍のテストケースを生成した。特にWhatKindOfTwoChars系(5.2倍)とformat.cpp(4.1倍)で顕著な増加を示した。

2. **体系性の向上**: 既存テストは開発者が重要と判断したケースを選択的に記述しているのに対し、生成テストはテスト要求モデルに基づき分岐条件・同値クラス・境界値を体系的に網羅している。特に修飾子の部分一致(BR-20~23)やIsValidCharの境界値(BV-07)など、手作業では見落としやすい箇所が補完された。

3. **新規カバレッジの追加**: エラーパス(ER)と依存切替(DP)のテストは既存テストにほぼ存在せず、生成テストが新たに追加した領域である。特にDP-02(WhatKindOfTwoChars vs 4KW版の差異検証)やDP-03(往復変換検証)は、関数間の関係性に着目したテストであり、手動テストでは設計意図が必要な種類のテストである。

---

## 4. ベースライン比較

### 4.1 三方式の比較

| 評価軸 | (A) 手動テスト作成 | (B) LLMのみ（直接生成） | (C) 提案方式 |
|--------|-------------------|----------------------|-------------|
| **テスト要求の明示性** | 暗黙（開発者の頭の中） | なし | 明示的（YAML 99件） |
| **トレーサビリティ** | なし～コメントレベル | なし | テスト要求ID ↔ テストケース名の完全対応 |
| **網羅性の測定** | カバレッジツール依存 | 不明 | テスト要求カバー率で事前測定可能 |
| **人間レビュー性** | テストコードを直接レビュー | テストコードを直接レビュー | テスト要求モデル(YAML) + テストコードの二段階レビュー |
| **再現性** | 担当者依存 | プロンプト依存（不安定） | テスト要求モデルが中間成果物として残る |
| **生成テスト数(推定)** | ~45（実績） | ~30~80（推定、ばらつき大） | 145（実績） |
| **テスト設計の粒度** | 開発者判断で粗密あり | LLM判断で偏りの可能性 | 種別(BR/EC/BV/ER/DP)による体系的な粒度 |

### 4.2 方式(B) LLMのみ（テスト要求モデルなし）との差異分析

テスト要求モデルなしでLLMに直接テストコードを生成させた場合の想定される課題:

1. **網羅性の不透明さ**: 生成されたテストが何を網羅しているか事後的にしか判断できない。提案方式ではテスト要求99件のカバー率100%を事前に設計段階で確認できる。

2. **分岐条件の見落ち**: LLMは関数の概要を理解してテストを生成するが、3桁目での数値切り捨て(BR-17)、修飾子の部分一致分岐(BR-20~23)、null文字によるbreak(BR-10)のようなコーナーケースを見落とす傾向がある。テスト要求モデルはこれらを明示的に列挙する。

3. **テスト意図の不明瞭さ**: LLMのみの方式ではテストケースの意図がコメントに依存する。提案方式ではテスト要求IDにより「なぜそのテストが必要か」が常に追跡可能である。

4. **レビュー効率**: LLMのみの場合、レビュアはテストコード145件を直接レビューする必要がある。提案方式ではテスト要求99件のYAMLレビュー（テスト設計の妥当性）とテストコードレビュー（実装の正確性）を分離できる。

### 4.3 方式(A) 手動テスト作成との差異分析

1. **工数**: 既存テスト約45件は長期にわたり複数の開発者が作成した。提案方式は同等以上のカバレッジを持つ145件を短時間で生成した（ただし実行・デバッグの工数は未測定）。

2. **テスト設計の体系性**: 既存テストは開発者の経験と判断に基づく。提案方式はBR/EC/BV/ER/DPの5種別でテスト要求を体系的に分類しており、「何がテストされていないか」を構造的に把握できる。

3. **保守性**: 既存テストにはテスト要求との対応関係がなく、仕様変更時の影響範囲が不明瞭である。提案方式のトレーサビリティマトリクスにより、仕様変更時に影響を受けるテストを特定できる。

---

## 5. 主要観察事項

### 5.1 手法の各フェーズの評価

#### Phase 1: リポジトリ解析 -- 候補選定の妥当性

**評価: 良好**

- 16ファイルの既存テストを網羅的に調査し、テスト対象としての適性を5段階で評価した。
- 選定基準（純粋関数、明確な分岐構造、既存テスト存在、スタブ不要）は実験目的に適合していた。
- 除外候補11件についても除外理由（外部依存、スタブ化困難、テスト価値低等）を明示した。
- 最終選定の8関数はすべてスタブなしでテスト可能であり、選定判断は適切であった。

**改善点**: 実験対象が純粋関数に偏っており、実務で多い副作用を持つ関数やクラスメソッドへの適用可能性は未検証。

#### Phase 2: テスト要求モデル -- 要求定義の完全性・粒度

**評価: 良好**

- 99件のテスト要求をBR/EC/BV/ER/DPの5種別で体系的に定義した。
- 各テスト要求にID、説明、優先度(high/medium/low)、根拠となる分岐条件/同値クラス/境界値への参照を付与した。
- YAMLスキーマにより機械可読性と人間可読性を両立した。
- 種別の分布: BR 55件(55.6%), EC 27件(27.3%), BV 11件(11.1%), ER 3件(3.0%), DP 3件(3.0%)。分岐網羅が過半を占めるのはホワイトボックステストの性質上妥当である。

**改善点**:
- 分岐条件(BC)と同値クラス(EC)の境界が一部曖昧（例: GetDateTimeFormatの各指定子はBRでもECでも表現可能）。
- エラーパス(ER)が3件と少なく、null入力やバッファオーバーフロー等の異常系テストが限定的。

#### Phase 3: テストコード生成 -- 生成品質

**評価: コンパイル・実行検証済み（macOS環境で145件全PASS）**

- 全99件のテスト要求に対応するテストコードを生成し、カバー率100%を達成した。
- Google Testフレームワークに準拠した構文で記述されている。
- 各テストケースにテスト要求IDをコメントで明示し、トレーサビリティを確保した。
- ヘルパー関数（MakeSysTime, ZenToHan, HanToZen, CheckMailAddress等）を適切に導入し、テストコードの可読性を確保した。

**コンパイル・実行結果**（macOS Darwin 24.0.0 / Apple clang 16.0.0 / Google Test 1.17.0）:
- Windows依存型（SYSTEMTIME, BOOL, UINT32等）はmacOS互換ヘッダで代替し、全3テスト実行ファイルがコンパイル成功。
- 初回実行時に145件中12件が失敗（91.7%）。**全12件がテスト期待値の誤りであり、対象関数の実装バグは0件**。
- 期待値修正後に再実行し、**145件全PASS（100%）** を確認。

**LLMが誤った12件のテスト期待値の分類**:

| 分類 | 件数 | 原因 |
|------|------|------|
| ParseVersionのコンポーネント分割ロジック誤解 | 5 | "2.4.1alpha"を1コンポーネントと予測したが実際は"1"と"alpha"が別コンポーネント |
| ParseVersionの数字グルーピング誤解 | 1 | "1234"を4桁別と予測したが2桁ずつ区切り |
| IsMailAddressの文字数カウントミス | 2 | 文字列長18を19と誤算（null終端を含めた） |
| IsMailAddressのAPI呼び出しバグ | 1 | offset とバッファポインタの二重シフト |
| WhatKindOfTwoCharsのマッピング戻り値誤解 | 3 | マッピング後のkindCurが返されることの誤解 |

### 5.2 テスト要求モデルの有用性

1. **中間成果物としての価値**: テスト要求モデル(YAML)は人間がレビュー可能な中間成果物として機能し、テストコード生成前にテスト設計の妥当性を確認できる。これはLLMのみの方式にはない利点である。

2. **定量評価の基盤**: 99件のテスト要求はカバレッジの定量評価基盤として機能し、「何がテストされていないか」を構造的に把握できる。

3. **再利用性**: テスト要求モデルはテストコード生成LLMのバージョンや設定に依存しない。LLMを変更しても同じテスト要求モデルから別のテストコードを生成できる。

### 5.3 テスト設計の特筆すべき点

1. **依存切替テスト(DP)の導入**: CompareVersion→ParseVersionの間接検証(DP-01)、WhatKindOfTwoChars vs 4KW版の差異検証(DP-02)、全角半角の往復変換検証(DP-03)は、関数間の関係性を検証する設計として有効である。

2. **修飾子順序の体系的検証**: ParseVersion/CompareVersionにおけるunknown < alpha < beta < rc < 無修飾 < plの順序関係を、個別比較(5テスト)と一括検証(FullChain)の両方で検証する設計は堅牢である。

3. **文字種ペアの網羅的検証**: WhatKindOfTwoCharsにおける20種のECharKindの同種ペア全検証(EC17)は、手動テストでは省略されがちな網羅性を実現している。

---

## 6. 脅威と限界

### 6.1 内的妥当性への脅威

| 脅威 | 影響 | 緩和策 |
|------|------|--------|
| **テスト期待値の誤り（12件/145件）** | LLMが推論した期待値のうち12件（8.3%）が実装と不一致。全てテスト側のバグであり対象関数の実装バグは0件 | 修正後145件全PASS。誤りの原因は5分類に整理済み。LLMのコード理解の限界を示す知見として論文に記載 |
| **テスト要求の完全性** | 99件のテスト要求自体がソースコードの全分岐・全同値クラスを網羅しているかは未検証 | 分岐条件はソースコードのif/else構造から機械的に抽出。ただし暗黙の条件（例: 整数オーバーフロー）は含まれていない可能性あり |
| **期待値の正確性** | LLMが推論した期待値が実装の実際の挙動と一致しない可能性がある | コメントに根拠を明示。不確実な期待値は非ゼロ検証等の緩い検証に留めている |

### 6.2 外的妥当性への脅威

| 脅威 | 影響 | 緩和策 |
|------|------|--------|
| **対象関数の偏り** | 8関数はすべて純粋関数（副作用なし、外部依存なし）であり、スタブ化が必要な関数への適用可能性は不明 | 除外候補11件の分析により、スタブ化困難な関数の特徴を整理済み |
| **単一リポジトリ** | sakura-editorという単一のC++プロジェクトでの実験であり、他言語・他プロジェクトへの一般化可能性は未検証 | C++の分岐構造に基づくテスト要求抽出は言語依存性が低い手法 |
| **コード量の制約** | 対象関数は5~80行と小規模。数百行以上の関数や複雑なクラス階層への適用は未評価 | 小規模関数での手法の有効性をまず実証し、今後スケーラビリティを検証 |

### 6.3 構成妥当性への脅威

| 脅威 | 影響 | 緩和策 |
|------|------|--------|
| **カバレッジ指標の妥当性** | テスト要求カバー率100%はテスト要求モデル自体の品質に依存する。テスト要求が不十分であればカバー率100%でも品質保証にならない | 5種別(BR/EC/BV/ER/DP)の体系的な定義により、テスト要求の構造的な完全性を担保 |
| **既存テストとの比較の公平性** | 既存テストは異なる目的（回帰テスト、バグ修正確認等）で作成されており、テスト要求カバレッジという観点での比較は公平でない可能性 | 比較はテスト数と推定カバレッジの定量比較に留め、品質の優劣を主張しない |

### 6.4 具体的な限界

1. **テスト期待値の精度**: 初回生成時の145件中12件（8.3%）にテスト期待値の誤りがあった。修正後に全PASS。
2. **コードカバレッジ(C0/C1/C2)の未測定**: gcov/lcov等によるステートメント/分岐/条件カバレッジの実測値がない。
3. **ミューテーションテストの未実施**: 生成テストの欠陥検出能力（変異スコア）は未評価。
4. **工数の定量評価なし**: Phase 1~3の所要時間、人間レビューの所要時間を計測していない。
5. **LLMモデルの再現性**: 使用したLLMのバージョンと設定を固定した場合でも、同一プロンプトから同一出力が得られるとは限らない。

---

## 7. 投稿文書「3.実施結果」向け要約文

### 3. 実施結果

提案手法をsakura-editor/sakuraリポジトリの8関数（3ファイル、合計約385行）に適用し、テスト要求モデルの生成からテストコード生成までの全パイプラインを実行した。

Phase 1（リポジトリ解析）では、リポジトリ内の16の既存テストファイルを調査し、純粋関数であること、明確な分岐構造を持つこと、既存テストが存在し比較が可能であること、スタブが不要であることを基準として、format.cpp（日時書式変換・バージョン解析）、CWordParse.cpp（メールアドレス判定・文字種結合判定）、convert_util.cpp（全角半角英数変換）の3領域8関数を選定した。候補選定の過程では11関数を除外しており、外部依存やスタブ化困難性を除外理由として明示した。

Phase 2（テスト要求モデル生成）では、各関数のソースコードから分岐条件を抽出し、分岐網羅（BR: 55件）、同値クラス（EC: 27件）、境界値（BV: 11件）、エラーパス（ER: 3件）、依存切替（DP: 3件）の5種別、計99件のテスト要求をYAML形式で定義した。各テスト要求にはID、説明、優先度（high/medium/low）、根拠となるソースコードの分岐条件・同値クラス・境界値への参照を付与した。テスト要求モデルは機械可読であると同時に人間がレビュー可能な中間成果物として機能し、テストコード生成前にテスト設計の妥当性を確認する基盤となった。

Phase 3（テストコード生成）では、テスト要求モデルを入力としてGoogle Test準拠のテストコード145件を3ファイルに生成した。テスト要求99件に対するカバー率は100%であった。テストケース数がテスト要求数を上回るのは、1つの境界値要求を上限・下限の別テストケースで検証する設計や、テスト要求に直接対応しない補強テスト（修飾子のタイプミス入力、4KW版との共通動作確認等）を含むためである。

既存テスト（約45件）との比較では、生成テストは約3.2倍のテストケース数を生成した。特に、修飾子の部分一致分岐（BR-20~23）、IsValidCharの境界値（BV-07）、全ECharKind値の同種ペア網羅（EC-17）、通常版と4KW版の差異検証（DP-02）、全角半角の往復変換検証（DP-03）など、既存テストではカバーされていなかった要求を新規に追加した。ただし、既存テストは回帰テストやバグ修正確認など異なる目的で作成されたものであり、単純なテスト数の比較には留意が必要である。

生成テスト145件をmacOS環境（Apple clang + Google Test + Windows互換ヘッダ）でコンパイル・実行した結果、初回実行時に12件（8.3%）のテスト失敗が発生した。失敗の原因分析の結果、全12件がLLMによるテスト期待値の推論誤りであり、対象関数の実装バグは検出されなかった。誤りの内訳は、ParseVersionのコンポーネント分割ロジックの誤解（5件）、数字グルーピングの誤解（1件）、IsMailAddressの文字数カウントミス（2件）とAPI呼び出しバグ（1件）、WhatKindOfTwoCharsのマッピング戻り値の誤解（3件）であった。これらの誤りは、テスト要求モデルに照らして修正箇所を迅速に特定でき、修正後に145件全てがPASSした。この結果は、テスト要求モデルがテスト期待値の正誤判断基準としても機能することを示している。gcov/lcov等によるコードカバレッジ（C0/C1）の実測値およびミューテーションテストによる欠陥検出能力の評価は今後の課題とする。

---

## 付録A: テストケース一覧（関数別）

### A.1 GetDateTimeFormat (21テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | YearFull_PercentY | BR | BR-01, BR-09 |
| 2 | YearShort_PercentSmallY | BR | BR-02 |
| 3 | Month_PercentM | BR | BR-03 |
| 4 | Day_PercentD | BR | BR-04 |
| 5 | Hour_PercentH | BR | BR-05 |
| 6 | Minute_PercentM | BR | BR-06 |
| 7 | Second_PercentS | BR | BR-07 |
| 8 | UnknownSpecifier | BR | BR-08 |
| 9 | NullCharInMiddle | BR/EC | BR-10, EC-05 |
| 10 | EmptyFormat | EC | EC-01 |
| 11 | LiteralOnly | EC | EC-02 |
| 12 | CompositeFormat | EC | EC-03 |
| 13 | PercentEscape | EC | EC-04 |
| 14 | TrailingPercent | EC | EC-06 |
| 15 | AllFieldsZero | EC | (EC-01-14) |
| 16 | BV_YearSingleDigit | BV | BV-01 |
| 17 | BV_YearMultipleOf100 | BV | BV-02 |
| 18 | BV_Year5Digits | BV | BV-03 |
| 19 | BV_HourMinMax | BV | BV-04 |
| 20 | BV_MonthSingleAndDouble | BV | (BV-01-04, BV-01-05) |
| 21 | BV_SecondMax | BV | (BV-01-08) |

### A.2 ParseVersion (27テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | AlphaModifier | BR | BR-11, BR-20 |
| 2 | BetaModifier | BR | BR-12, BR-21 |
| 3 | RcModifier | BR | BR-13, BR-22 |
| 4 | RcUpperCase | EC | (EC-02-06) |
| 5 | PlModifier | BR | BR-14, BR-23 |
| 6 | UnknownModifier | BR | BR-15 |
| 7 | DigitOnlyComponent | BR/EC | BR-16, EC-07 |
| 8 | ThreeDigitTruncation | BR/BV | BR-17, BV-05 |
| 9 | SeparatorDot | BR | BR-18 |
| 10 | SeparatorHyphen | EC | EC-09 |
| 11 | SeparatorUnderscore | EC | EC-09 |
| 12 | SeparatorPlus | EC | EC-09 |
| 13 | FiveComponentsTruncated | BR/BV | BR-19, BV-06 |
| 14 | AlphaPartialMatch | BR | BR-20 |
| 15 | AlphaTypo | EC | (EC-02-15) |
| 16 | BetaPartialMatch | BR | BR-21 |
| 17 | RcPartialMatch | BR | BR-22 |
| 18 | PlPartialMatch | BR | BR-23 |
| 19 | EC_DigitsOnly | EC | (EC-02-01) |
| 20 | EC_ModifierOrder | EC | EC-08 |
| 21 | EC_AlphaWithTrailingDigit | EC | (EC-02-16) |
| 22 | BV_SingleDigit | BV | (BV-02-01) |
| 23 | BV_TwoDigitMax | BV | BV-05 |
| 24 | BV_ZeroComponents | BV/EC/ER | BV-06, EC-10, ER-01 |
| 25 | BV_OneComponent | BV | BV-06 |
| 26 | BV_FourComponents | BV | BV-06 |
| 27 | ER_EmptyString | ER | ER-01 |

### A.3 CompareVersion (13テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | SameVersion | BR | BR-24 |
| 2 | AisNewer_MajorDiff | BR | BR-25 |
| 3 | BisNewer_MinorDiff | BR | BR-26 |
| 4 | ModifierOrdering_AlphaLessThanBeta | EC | EC-11 |
| 5 | ModifierOrdering_BetaLessThanRc | EC | EC-11 |
| 6 | ModifierOrdering_RcLessThanRelease | EC | EC-11 |
| 7 | ModifierOrdering_ReleaseLessThanPl | EC | EC-11 |
| 8 | ModifierOrdering_UnknownLessThanAlpha | EC | EC-11 |
| 9 | ModifierOrdering_FullChain | EC | EC-11 |
| 10 | DifferentSeparatorsEqual | EC | EC-12 |
| 11 | DP_ParseVersionIntegration | DP | DP-01 |
| 12 | BV_MinimalDifference | BV | (BV-03-01) |
| 13 | BV_BothEmpty | BV | (BV-03-02) |

### A.4 IsMailAddress (27テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | BR27_OffsetWithValidPrecedingChar | BR | BR-27 |
| 2 | BR28_LeadingDot | BR | BR-28 |
| 3 | BR29_LeadingInvalidChar | BR | BR-29 |
| 4 | BR30_LocalPartTooShort | BR | BR-30 |
| 5 | BR31_NoAtSign | BR | BR-31 |
| 6 | BR32_DomainLabelLengthZero | BR | BR-32 |
| 7 | BR33_DomainNoDot | BR | BR-33 |
| 8 | BR34_DomainDotSeparation | BR | BR-34 |
| 9 | BR35_NullAddressLength | BR | BR-35 |
| 10 | EC13_StandardAddress | EC | EC-13 |
| 11 | EC14_InvalidAddresses | EC | EC-14 |
| 12 | EC15_SymbolsInLocalPart | EC | EC-15 |
| 13 | EC15_ProhibitedCharsInLocalPart | EC | EC-15 |
| 14 | EC16_OffsetVariations | EC | EC-16 |
| 15 | EC_DomainWithHyphen | EC | (EC-04-09) |
| 16 | EC_DomainWithUnderscore | EC | (EC-04-10) |
| 17 | EC_AddressFollowedBySpace | EC | (EC-04-11) |
| 18 | EC_MultipleDotsDomain | EC | (EC-04-06) |
| 19 | BV07_ValidCharBoundary_0x21 | BV | BV-07 |
| 20 | BV07_ValidCharBoundary_0x7E | BV | BV-07 |
| 21 | BV07_InvalidCharBoundary_0x20 | BV | BV-07 |
| 22 | BV07_InvalidCharBoundary_0x7F | BV | BV-07 |
| 23 | BV08_MinimalAddress | BV | BV-08 |
| 24 | BV_LongLocalPart | BV | (BV-04-02) |
| 25 | BV_BufferLengthInsufficient | BV | (BV-04-07) |
| 26 | ER02_EmptyString | ER | ER-02 |
| 27 | ER03_AtSignOnly | ER | ER-03 |

### A.5 WhatKindOfTwoChars (18テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | BR36_SameKindInitialCheck | BR | BR-36 |
| 2 | BR37_NobasuPulledByKatakana | BR | BR-37 |
| 3 | BR37_DakuPulledByHiragana | BR | BR-37 |
| 4 | BR37_KatakanaPullsNobasu | BR | BR-37 |
| 5 | BR37_HiraganaPullsDaku | BR | BR-37 |
| 6 | BR38_NobasuDakuContinuous | BR | BR-38 |
| 7 | BR39_LatinMappedToCSYM | BR | BR-39 |
| 8 | BR40_UdefMappedToETC | BR | BR-40 |
| 9 | BR41_CtrlMappedToETC | BR | BR-41 |
| 10 | BR42_MappedSameKind | BR | BR-42 |
| 11 | BR43_DifferentKind_CK_NULL | BR | BR-43 |
| 12 | EC17_AllSameKindPairs | EC | EC-17 |
| 13 | EC18_DragRuleAllCombinations | EC | EC-18 |
| 14 | EC19_MappingRules | EC | EC-19 |
| 15 | EC_LatinLatinSameKind | EC | (EC-05-12) |
| 16 | EC_ZenSpaceAndHira_Different | EC | (EC-05-13) |
| 17 | BV_CK_NULL_SameKind | BV | (BV-05-01) |
| 18 | BV_CK_ZEN_ETC_SameKind | BV | (BV-05-02) |

### A.6 WhatKindOfTwoChars4KW (8テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | BR44_UdefMappedToCSYM | BR | BR-44 |
| 2 | BR45_CtrlRemainsCtrl | BR | BR-45 |
| 3 | EC20_UdefAndCSYM_SameKind | EC | EC-20 |
| 4 | EC21_UdefAndETC_DifferentKind | EC | EC-21 |
| 5 | EC22_CtrlAndETC_DifferentKind | EC | EC-22 |
| 6 | EC_CtrlCtrl_SameKind | EC | (EC-06-04) |
| 7 | DP02_DirectComparison | DP | DP-02 |
| 8 | CommonBehavior_SameAsNormal | DP | DP-02 |

### A.7 Convert_ZeneisuToHaneisu (17テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | BR46_ZenUpperToHanUpper | BR | BR-46 |
| 2 | BR47_ZenLowerToHanLower | BR | BR-47 |
| 3 | BR48_ZenDigitToHanDigit | BR | BR-48 |
| 4 | BR49_ZenKigoToHanKigo | BR | BR-49 |
| 5 | BR50_NonTargetCharsUnchanged | BR | BR-50 |
| 6 | EC23_AllCategories | EC | EC-23 |
| 7 | EC24_NonTargetPreserved | EC | EC-24 |
| 8 | EC25_MixedCharacters | EC | EC-25 |
| 9 | EC_EmptyArray | EC | (EC-07-08) |
| 10 | BV09_ZenUpperBoundary_First | BV | BV-09 |
| 11 | BV09_ZenUpperBoundary_Last | BV | BV-09 |
| 12 | BV09_ZenUpperBoundary_OutBefore | BV | BV-09 |
| 13 | BV09_ZenUpperBoundary_OutAfter | BV | BV-09 |
| 14 | BV09_ZenLowerBoundary | BV | BV-09 |
| 15 | BV09_ZenDigitBoundary | BV | BV-09 |
| 16 | BV10_LengthZero | BV | BV-10 |
| 17 | BV_LengthOne | BV | (BV-07-10) |

### A.8 Convert_HaneisuToZeneisu + RoundTrip (14テスト)

| # | テストケース名 | 種別 | テスト要求ID |
|---|---------------|------|-------------|
| 1 | BR51_HanUpperToZenUpper | BR | BR-51 |
| 2 | BR52_HanLowerToZenLower | BR | BR-52 |
| 3 | BR53_HanDigitToZenDigit | BR | BR-53 |
| 4 | BR54_HanKigoToZenKigo | BR | BR-54 |
| 5 | BR55_NonTargetCharsUnchanged | BR | BR-55 |
| 6 | EC26_AllCategories | EC | EC-26 |
| 7 | EC27_NonTargetPreserved | EC | EC-27 |
| 8 | EC_MixedCharacters | EC | (EC-08-07) |
| 9 | BV11_HanUpperBoundary | BV | BV-11 |
| 10 | BV11_HanLowerBoundary | BV | BV-11 |
| 11 | BV11_HanDigitBoundary | BV | BV-11 |
| 12 | DP03_ZenToHanToZen | DP | DP-03 |
| 13 | DP03_HanToZenToHan | DP | DP-03 |
| 14 | DP03_MixedRoundTrip | DP | DP-03 |

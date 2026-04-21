# 検証対象候補リポジトリ

> **目的**: sakura-editor以外でTRMパイプラインを検証する候補を選定する
> **起点**: input.md ③「サクラエディタ以外のソースを考える」
> **選定観点**: ライセンス / 言語多様性 / OOP複雑度 / 規模 / ドメイン
> **関連**: `./trm-evaluation-criteria.md`（評価軸）、`./operation-guide.md`（流用手順）

---

## 1. 選定基準

候補リポジトリは以下の全項目を満たすこと:

| 基準 | 詳細 | 重要度 |
|---|---|---|
| 1. OSSライセンス | MIT / Apache 2.0 / BSD 等、実験利用に制約のないもの | 必須 |
| 2. 規模 | 総行数 5万行以下、対象モジュール 1万行以下が望ましい | 高 |
| 3. ロジック中心 | UI・グルーコード中心ではなく、純粋計算・変換・解析を含む | 高 |
| 4. テスタブル | 外部依存が限定的、または明確にスタブ可能 | 高 |
| 5. 既存テスト | 比較評価のため既存ユニットテストが存在する | 中 |
| 6. OOP構造 | クラス継承・状態変数・パターン・カプセル化を含む（v3.1検証のため） | 中 |
| 7. 活発な保守 | 直近1年以内にコミットあり | 低 |
| 8. 日本語話者の可読性 | ドメインが難解すぎない（暗号・カーネル等は回避） | 低 |

---

## 2. 候補リポジトリ一覧

sakura-editorとの差別化（言語・ドメイン・規模）を意識して選出。

### 2.1 C++ 系候補（sakura-editorと同言語、異ドメイン）

| # | リポジトリ | 概要 | ライセンス | 規模目安 | OOP要素 | 備考 |
|---|---|---|---|---|---|---|
| C1 | nlohmann/json | JSONパーサ・シリアライザ | MIT | 単一ヘッダ | テンプレート、CRTP | C++テンプレート検証、CP観点に有効 |
| C2 | leethomason/tinyxml2 | XMLパーサ | zlib | 小〜中 | クラス継承、状態持ち | 状態遷移（パーサ状態）を検証、SV観点に有効 |
| C3 | catchorg/Catch2 | テストフレームワーク | BSL-1.0 | 中 | マクロ、テンプレート | メタプログラミング、CP観点 |

### 2.2 Python 系候補（動的型・OOPの別軸検証）

| # | リポジトリ | 概要 | ライセンス | 規模目安 | OOP要素 | 備考 |
|---|---|---|---|---|---|---|
| P1 | pallets/click | CLIフレームワーク | BSD-3 | 中 | デコレータ、ABC、context manager | CP観点の Python イディオム検証に最適 |
| P2 | psf/requests | HTTPクライアント | Apache 2.0 | 中 | セッション状態、Mixin | SV観点（セッション状態遷移）に有効 |
| P3 | python-jsonschema/jsonschema | JSONスキーマ検証 | MIT | 中 | 継承、Validator階層 | CI観点（継承階層の多態性）検証に有効 |

### 2.3 Java 系候補（静的型・OOP王道）

| # | リポジトリ | 概要 | ライセンス | 規模目安 | OOP要素 | 備考 |
|---|---|---|---|---|---|---|
| J1 | google/gson | JSONライブラリ | Apache 2.0 | 中 | TypeAdapter階層、Builder | CI/SV/CP全観点をカバー可能 |
| J2 | apache/commons-lang | 汎用ユーティリティ | Apache 2.0 | 中 | static中心だがクラス多 | 純粋関数多く、BR/EC/BV検証に適する |

### 2.4 TypeScript 系候補（型システム + OOP）

| # | リポジトリ | 概要 | ライセンス | 規模目安 | OOP要素 | 備考 |
|---|---|---|---|---|---|---|
| T1 | colinhacks/zod | スキーマ検証ライブラリ | MIT | 中 | 判別共用体、型ガード、継承 | CP観点（discriminated_union, type_guard）の検証に理想的 |
| T2 | date-fns/date-fns | 日時操作ライブラリ | MIT | 中 | 関数型中心 | OOP少なく、従来5種別（BR/EC/BV）の検証用 |

### 2.5 Go 系候補（インターフェース暗黙性の検証）

| # | リポジトリ | 概要 | ライセンス | 規模目安 | OOP要素 | 備考 |
|---|---|---|---|---|---|---|
| G1 | spf13/cobra | CLIフレームワーク | Apache 2.0 | 中 | 構造体埋め込み、インターフェース | Goの"継承風"パターン検証 |
| G2 | gorilla/mux | HTTPルーター | BSD-3 | 小〜中 | Matcher interface階層 | CI観点（interface実装）検証 |

### 2.6 Rust 系候補（トレイト・所有権）

| # | リポジトリ | 概要 | ライセンス | 規模目安 | OOP要素 | 備考 |
|---|---|---|---|---|---|---|
| R1 | serde-rs/serde | シリアライズフレームワーク | MIT/Apache 2.0 | 中 | トレイト多用、マクロ | CP観点（trait、derive macro） |
| R2 | rust-lang/regex | 正規表現エンジン | MIT/Apache 2.0 | 中 | 内部状態機械 | SV観点（状態マシン）検証 |

---

## 3. 推奨プロセス

### 3.1 優先順位（最初の1-2リポジトリ）

**第1候補: P1 pallets/click (Python)**
- 理由: 言語の違いによる汎用性検証、デコレータ・ABCのCP検証、既存テストも充実、ドキュメントが整備
- 予想される学び: Python固有のCP（デコレータ、@contextmanager）の抽出精度

**第2候補: T1 colinhacks/zod (TypeScript)**
- 理由: v3.1で謳った TypeScript サポート（判別共用体・型ガード・EN種別）の実地検証、非C++で異言語対応の厚みを見せられる
- 予想される学び: TypeScript型システムの表現力とTRMの相性

### 3.2 段階的実験計画

```
フェーズ1（単一言語、1リポジトリ）
  ↓
  P1 click で全パイプラインを通す
  ↓
  評価: trm-evaluation-criteria.md のA+B+C
  ↓
  既知の弱点洗い出し、パイプライン修正
  ↓
フェーズ2（異言語、2リポジトリ）
  ↓
  T1 zod を追加、言語差異による問題を収集
  ↓
フェーズ3（広域評価、3-5リポジトリ）
  ↓
  C1, J1, G1, R1 のうち2-3個で評価軸のブレを確認
  ↓
  論文/発表用の比較マトリクス作成
```

---

## 4. 候補ごとの適用戦略メモ

### P1 pallets/click
- 対象モジュール候補: `click/types.py`（型変換）、`click/parser.py`（引数パーサ）
- 選定関数例: `ParamType.convert`, `Argument.process_value`, `Option.process_value`
- OOP観点: `ParamType` 継承階層（`StringParamType`, `IntParamType` 等）
- 期待: CI 要求 20件以上、SV 要求 10件程度

### T1 colinhacks/zod
- 対象モジュール候補: `src/types.ts`、`src/helpers/parseUtil.ts`
- 選定関数例: `ZodString.safeParse`, `ZodObject.shape`, `ZodUnion.parse`
- OOP観点: `ZodType` 基底と派生の多態性、判別共用体
- 期待: CI 要求 30件以上、CP（discriminated_union）要求 10件程度

### C2 leethomason/tinyxml2
- 対象モジュール候補: `tinyxml2.cpp` の `XMLDocument::Parse`, `XMLElement::Accept`
- OOP観点: `XMLNode` 階層、Visitor パターン
- 期待: CI 要求 20件、SV 要求（パーサ状態）15件

---

## 5. 避けるべきリポジトリの特徴

以下は候補から外す:

- UIフレームワーク中心（React, Vue, Flutter等）— コア機能がビュー層に分散
- OS・カーネル・ドライバ系 — テスト環境構築コストが過大
- 機械学習・数値計算ライブラリ — 期待値が確率的でTRMと整合しにくい
- 超大規模（10万行超）— 評価期間に収まらない
- プロプライエタリ・ソースが非公開 — 研究発表に利用不可

---

## 6. チェックリスト（候補1件を選定する際）

- [ ] GitHub上でリポジトリのライセンスを確認（MIT/Apache/BSD系か）
- [ ] 総行数とテスト対象モジュールの規模を `cloc` または `tokei` で測定
- [ ] `CONTRIBUTING.md` / テストセクションで既存テストの規模を把握
- [ ] `/project:setup <url>` を実行してみて自動推定が機能するか確認
- [ ] `project-config.yaml` の `selection_criteria.exclude_patterns` を調整
- [ ] 小規模な1モジュールだけでまず `/project:analyze` を通す
- [ ] 結果が想定通りなら `/project:run-pipeline` にスコープ拡大

---

## 7. 確定ショートリスト（2026-04-21時点）

論文構成・対象ドメインの多様性・実施リソースを踏まえ、以下を本命とする:

| 位置づけ | リポジトリ | 言語 | 選定理由 |
|---|---|---|---|
| **A. 既存実証** | sakura-editor/sakura | C++ | SQiP 2026 実証対象。比較基準点として保持 |
| **B. 第2実証（本命）** | pallets/click | Python | 言語ジャンプ（動的型）の検証、OOP（ABC・デコレータ・context manager）でCP検証、ドキュメントが整備され非エンジニアにも題材として触れやすい |
| **C. 第3実証（任意）** | colinhacks/zod | TypeScript | 型システムを活かしたCPパターン（discriminated_union・type_guard）の検証。論文で"v3.1のOOP/EN拡張が言語横断で機能する"証拠にする場合に実施 |
| D. 読みやすさアンケート用 | 自作 reversi (約80行) | 複数言語可 | 非エンジニア向けアンケート（`./survey-design.md` 参照）の前提サンプル。実在OSSではなく、読みやすさ評価のために最小化した合成例 |

### 論文上の構図

- **主張の骨格**: Aで手法を提案・実証 → B/Cで汎用性を確認 → Dで非エンジニアへの情報提供価値を検証
- **採用範囲の推奨**: 最小構成は **A + B**（Python汎用性の実証）。C（TypeScript）は時間があれば追加実施。Dはアンケートのみで実験対象としては扱わない

### この決定の根拠

- 実在OSSを本命とする方針（ユーザー明示）により、Dは**比較対象**・**アンケート素材**に限定
- B (Python/click) は sakura-editor と **言語パラダイム・対象ドメイン・設計スタイル** すべてが異なり、汎用性検証として情報量が大きい
- C (TypeScript/zod) は独自の貢献（型ガード・判別共用体）を引き出せるが、リソース次第で省略可

---

## 8. 次アクション

- [ ] pallets/click で `/project:setup` → `/project:run-pipeline` を試行
- [ ] `trm-evaluation-criteria.md` のA+B+Cカテゴリで評価
- [ ] 得られた結果を `experiments/click-evaluation-report.md` として格納
- [ ] `./survey-design.md` のリバーシ素材を確定し、非エンジニア向けアンケートを実施
- [ ] 論文の実験章に `./paper-experiment-section.md` の段落を反映

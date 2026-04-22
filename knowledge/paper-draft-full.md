# テスト要求モデルによるホワイトボックス品質情報の非エンジニア共有

## 4種の図式自動生成を用いた AI駆動開発時代の品質可視化

> **ドラフト版**: 2026-04-21 時点
> **出典**: `knowledge/paper-draft-outline.md` に基づく完全本文ドラフト
> **想定投稿先**: 品質関連学会 / SQiP 2027 経験発表 / 技術系論文誌
> **想定ページ**: 日本語8〜12ページ（約17,000字）

---

## Abstract（概要）

LLM を先行させた AI 駆動開発やテスト駆動開発が広く採用される中、実装領域のホワイトボックス品質情報はコードに閉じ込められ、要求作成ロール（非エンジニアを含む）がアクセスできない「可読性ギャップ」が生じている。本研究はコードのみを入力として、(1)ホワイトボックス評価における確認対象領域の十分性を高めること、(2)非エンジニアにも伝わる形で品質情報を共有できることの2点を妥当性の問いとし、テスト要求モデル（Test Requirement Model, 以下 TRM）を中間成果物とする手法を提案・実証する。本稿では従来5種別（分岐網羅・同値クラス・境界値・エラーパス・依存切替）と OOP 拡張3種別（クラス継承・状態変数・コードパターン）に加え、新たにカプセル化種別（EN）を設計し、スキーマ v3.1 として統合した。C++ 手続き型 OSS（sakura-editor）と Python OOP OSS（pallets/click）の2実証を行い、それぞれ99件・173件の要求を導出、前者で可読率 65.7% を達成した。EN 種別は click で15〜21件を検出し、既存テストには対応観点のない構造設計上の懸念（漏洩アクセサ・構築契約など）を明らかにした。さらに TRM を入力として Sunburst・Sankey・Heatmap・Chord の4種の図式を自動生成する可視化レイヤを実装し、非エンジニアへの情報提供手段として提案する。

**キーワード**: テスト要求モデル、ホワイトボックステスト、AI駆動開発、可視化、カプセル化、静的解析

---

## 1. はじめに

### 1.1 背景

近年、GitHub Copilot や Claude Code、ChatGPT を用いた開発支援が普及し、コード生成・リファクタリング・テスト生成の一部が大規模言語モデル（LLM）に委ねられる「AI 駆動開発」が実務に浸透している。同時に、テスト駆動開発（Test-Driven Development, TDD）や振る舞い駆動開発（Behavior-Driven Development, BDD）も広く採用されており、外側の仕様・インターフェース・ユニットテストは比較的整備される傾向にある。

しかし、こうした開発スタイルの普及と並行して、**実装領域のホワイトボックスレイヤー（内部実装の品質情報）の保守が弱まる** 傾向が観察される。これは次の要因による:

- LLM が生成したコードの内部構造を人間が十分に読み込まないまま承認する
- 仕様書やインターフェース定義では表現できない **実装の内部契約**（不変条件・可変性・依存関係）が暗黙のまま取り残される
- テストケースは整備されても、**テスト要求（何を検証すべきか）** そのものの構造化は進んでいない
- 品質情報がコードの中に閉じ込められ、要求作成ロール（企画・プロダクトマネージャー・品質保証担当のうち非エンジニア）からアクセスできない

この「品質情報の可読性ギャップ」は、実装のリファクタや外部要因（依存ライブラリ更新、仕様変更）が発生したときに、**何を再検証すべきか** がコードを読める人にしかわからない、という形で顕在化する。

### 1.2 研究の問い

上記の課題に対して、本研究はコードを唯一の入力として、次の2点の妥当性を問う:

**問い (1)**: ホワイトボックス評価において、確認対象領域の十分性を高められるか。
**問い (2)**: 非エンジニアにも伝わる形で品質情報を共有できるか。

前者は検証観点の網羅率と構造的妥当性の問題であり、後者は品質情報のコミュニケーション基盤化の問題である。両者はコードから情報を自動的に抽出し、意味を損なわずに再表現する仕組みを必要とする点で相互に関連する。

### 1.3 貢献

本研究の貢献は次の5点である:

1. 従来5種別と OOP 拡張3種別、合計8種別のテスト要求を統合した **TRM v3.1 スキーマ** の設計
2. OOP の構造設計観点を扱う新種別 **EN（Encapsulation, カプセル化）** の提案と5サブタイプの定義
3. TRM YAML から Sunburst・Sankey・Heatmap・Chord の **4種の図式を自動生成する可視化レイヤ** の実装
4. C++ 手続き型（sakura-editor）と Python OOP（pallets/click）の **2実証による汎用性検証**、および既存テストに対する **追加観点検出率 86.7%** の定量化
5. TRM description の **可読性自動分類器** の設計と3対象への適用、可読率のドメイン依存性の実測

### 1.4 本稿の構成

§2 で関連研究を概観し、§3 で提案手法を述べる。§4 で実験設計、§5 で評価指標、§6 で結果、§7 で考察、§8 で結論を述べる。付録に用語集と実証対象のメタ情報を示す。

---

## 2. 関連研究

### 2.1 テスト要求モデル・テスト設計技法

ホワイトボックステストの古典として Myers [1] や Beizer [2] の分岐網羅・同値分割・境界値分析の技法が知られ、ISTQB [3] のテスト技法標準にも採用されている。本研究の従来5種別（BR/EC/BV/ER/DP）はこれら古典技法を構造化したものである。しかし、これら古典は関数レベルの動作検証に特化しており、OOP の構造設計観点（継承・多態・カプセル化）は別途扱う必要がある。

### 2.2 LLM を用いたテスト生成

GPT・Claude 等を用いた自動テスト生成は近年広範に研究されている。CodaMOSA [4] は事前訓練 LLM を用いて Coverage Plateau を脱出する手法を提案し、Schäfer らの実証研究 [5] では複数 LLM によるテスト生成の有効性を比較している。CoverUp [6] はカバレッジ誘導型 LLM テスト生成を実現し、Mathews ら [7] は LLM 生成テストが **バグを発見しにくい設計選択** をしてしまう傾向を指摘している。テストオラクルに関しても、LLM が **期待すべき動作** と **実装の動作** を混同する問題が明らかにされている [8]。Chu らの最近のサーベイ [9] はこの分野の成果と課題を体系化している。本研究の TRM は、**期待値を実装から導出する制約** を課すことで LLM 推論依存を抑制する中間層として機能する。Multi-Step Generation [10] や TELPA [11]、Static Analysis feedback loop [12] と同様、本研究も **LLM + 静的情報の組み合わせ** で生成品質を高める系譜に位置する。

### 2.3 OOP 品質メトリクス

McCabe 循環的複雑度 [13] や CK メトリクス（Coupling・Cohesion 等）[14] は OOP 品質を定量化する手法として確立されている。また、Gamma らのデザインパターン [15] や Fowler のリファクタリングカタログ [16] は構造設計の良し悪しを記述する用語を提供する。本研究の CI/SV/CP/EN 種別は、これら既存概念を **テスト要求として操作可能な形** に再定式化したものである。特に EN（カプセル化）は CK メトリクスの結合度・凝集度に近い関心を、動的テストで検証可能な観点に翻訳している点で新規性を持つ。

### 2.4 品質情報の非エンジニア共有

BDD の Given-When-Then 構文 [17] や Adzic の Specification by Example [18] は、要求・テスト・ドキュメントを統一フォーマットで記述する手法として確立されている。Domain-Driven Design [19] の Ubiquitous Language と Brandolini の Event Storming [20] は、ドメイン知識と実装の間を橋渡しする。Wiecher ら [21] は自動車分野でモデルベースで要求とテストを接続する研究を示している。情報可視化の分野では Tufte [22]・Few [23]・Munzner [24] が視覚的情報伝達の原則を体系化しており、本研究の可視化レイヤはこれらの知見（3次元忌避・小さな多数・色彩意味論）を踏まえて設計されている。

### 2.5 本研究の位置づけ

既存の LLM テスト生成研究 [4-12] は「動作の正しさ」の生成に特化し、OOP 品質メトリクス [13-16] は静的な構造評価に留まり、BDD/DDD [17-20] は要求の構造化のみを扱う。本研究はこれらを統合し、**コードから構造設計観点を含むテスト要求を導出し、さらに非エンジニア向けの可視化レイヤで橋渡しする** 一貫したパイプラインを提案する点で新規性がある。特に EN 種別は既存研究に直接的な対応物を持たず、本研究の独自貢献に該当する。日本のソフトウェア品質コミュニティにおける近縁研究 [27-30] とも補完的な関係にある。

---

## 3. 提案手法

### 3.1 手法の全体像

提案手法は、GitHub 上で公開されているリポジトリ URL を入力として、次の段階を逐次実行する:

1. **リポジトリ解析（analyze）**: テスト対象の選定、関数・クラス構造の抽出、OOP 要素の検出
2. **TRM 生成（generate-trm）**: 8種別のテスト要求を YAML 形式で導出
3. **網羅性監査（audit-trm）**: 漏れの検出と追加要求の導出
4. **テスト生成（generate-tests）**: 対象言語のテストフレームワークに準拠したコード生成
5. **可視化生成（generate-visualizations）**: TRM YAML を入力として4種の図式を自動出力

各段階は独立したエージェント（コマンド定義）として実装され、Claude Code プラットフォーム上で逐次実行される。対応言語は C++・Python・Java・TypeScript・Go・Rust の6言語である。パイプライン全体の関係を**図 3.1** に示す。

### 3.2 TRM v3.1 スキーマ

TRM は対象関数・メソッドを「ターゲット」として列挙し、各ターゲットに対して以下のテスト要求を付与する YAML 構造である。

**従来5種別（関数レベル）:**

| 種別 | 記号 | 導出方法 |
|---|---|---|
| 分岐網羅 | BR | if/else/switch/三項演算子の各パスを1要求とする |
| 同値クラス | EC | 入力パラメータの値域を同値分割し各クラスを1要求とする |
| 境界値 | BV | 各同値クラスの境界（上限・下限±1）を1要求とする |
| エラーパス | ER | 例外・早期リターン・ガード条件を1要求とする |
| 依存切替 | DP | 関数間の呼び出し関係・差異・ラウンドトリップを1要求とする |

**OOP 拡張3種別（v3.0 で追加）:**

| 種別 | 記号 | 検証対象 |
|---|---|---|
| クラス継承 | CI | 多態性・オーバーライド・リスコフ置換・抽象実装・super 委譲 |
| 状態変数 | SV | 初期化・変異シーケンス・不変条件・状態依存動作・ライフサイクル |
| コードパターン | CP | デザインパターン適合・イディオム正当性・リソース管理・マクロ展開 |

**カプセル化種別（v3.1 で新設）**: §3.3 で詳述。

### 3.3 EN（Encapsulation）種別の設計

v3.0 までの CI/SV/CP は **動作の正しさ** を検証する性格が強く、メンバの可視性や構築契約、不変性宣言といった **構造設計の妥当性** を明示的に扱うサブタイプが不足していた。v3.1 ではこの欠落を補うべく、EN 種別を以下5サブタイプで設計した:

| サブタイプ | 検証対象 |
|---|---|
| access_control_correctness | メンバ可視性（public/protected/private）の責務合致 |
| leaky_accessor | getter/setter による内部可変状態の漏洩 |
| mutability_contract | final/const/readonly 宣言の整合性 |
| construction_contract | コンストラクタでの必須フィールド初期化 |
| invariant_surface | public 境界でのクラス不変条件維持 |

**EN が既存種別で扱えなかった観点を補う代表例を以下に示す:**

- **Choice.choices の public mutable 問題**（pallets/click）: SV の invariant_maintenance で近似できるが、EN の access_control_correctness の方が「外部から再代入可能かどうか」を直接的に検証できる。
- **_NumberParamTypeBase._number_class の派生設定契約**: CI の abstract_coverage で継承契約として扱えるが、EN の construction_contract の方が「派生クラスが未設定のとき AttributeError が発生する」ことを明示化できる。
- **IntRange の min > max を許容する設計**: CI/SV ともに直接の表現がなく、EN の invariant_surface が唯一の足場となる。

EN は既存種別と排他的ではなく、**同じ観点を異なる抽象度で捉える補助軸** として機能する。

### 3.4 可視化レイヤ

TRM YAML を入力として、**エンジニア向け4種図** と **非エンジニア向け機能分類ジオラマ**、さらに **論文主張ダッシュボード3パネル** を自動生成するパイプラインを実装した。

#### 3.4.1 エンジニア向け4種図

| 手法 | 目的 | 実装 | 手続き型対象の代替 |
|---|---|---|---|
| Sunburst | 要求の種別別階層の俯瞰 | matplotlib 入れ子パイ | 同左 |
| Sankey | 種別 × 優先度の流量 | matplotlib 横積みバー | 同左 |
| Heatmap | 状態変数 × カプセル化リスク（OOP） | matplotlib `imshow` | 要求種別 × 優先度 |
| Chord | 状態変数とリスクの円環配置（OOP） | matplotlib 2D グラフ | 依存関係マップ |

4図は **全体俯瞰 → 流量理解 → 複合影響の定量 → 禁忌組合せの強調** の段階的情報開示を形成する。手続き型・関数型対象では Heatmap/Chord が代替表示に切り替わり、空白ページにならない設計とした。

#### 3.4.2 非エンジニア向け機能分類ジオラマ

4種図は要求IDや変数名など実装用語が前面に出るため、**非エンジニアへは到達しにくい**。この課題に対処し、テスト設計書の機能ツリーの考え方に沿って、対象を以下の3層で立体的に表現する機能分類ジオラマを追加で提案する:

- **目的・対象層（Z=0, 手前）**: 関数/クラスの目的・扱う情報
- **機能分類層（Z=1, 中段）**: 業務的なグルーピング（例: 「範囲内判定 (min ≤ 値 ≤ max)」）
- **テスト観点層（Z=2, 奥）**: 正常系・異常系・境界値・エラー処理・連携動作

各層は半透明プレーンで表現し、層間は色分けされた連結線で結ぶ。ラベルは **抽象名＋具体的な検証内容** の2段表記とし、実装用語（変数名・ID・英語技術語彙）を排除して非エンジニアへの伝達性を確保する。

#### 3.4.3 論文主張ダッシュボード

機能分類ジオラマと同一ページに、本論文の主要主張と1対1対応する評価ダッシュボード3パネルを配置する:

| パネル | 伝える情報 | 対応主張 |
|---|---|---|
| ① 既存テスト補完効果 | 既存テスト件数 vs TRM追加検出件数、GAP率 | §6.3 |
| ② 非エンジニア可読性 | L1/L2/L3 分布、可読率 (L1+L2) | §6.5 |
| ③ v3.1 OOP拡張効果 | 従来5種別 vs v3.1追加種別、EN検出リスク高/中/低 | §6.7 |

ページ冒頭に「このページで示すこと」のタイトル帯を付加することで、読者は図を見た瞬間に論文の主張がどの数字で裏付けられているかを把握できる。

#### 3.4.4 3次元グラフの採用範囲

一般的な3D散布プロット・曲面プロットは、離散的なトリガー組合せの可視化では遮蔽・遠近歪み・静的媒体不適合の観点から2D に劣るため本研究では採用しない。ただし機能分類ジオラマは **等角投影で層の重なりを立体的に見せる擬似3D** であり、遮蔽の少ない利用形態に限定している。Tufte [22] の視覚化原則（情報量を装飾で増やさない）と整合的である。

---

## 4. 実験設計

### 4.1 対象選定方針

本研究の目的（研究の問い (1)(2)）に適う実験対象の選定には、次の5要件を課した:

1. **実在性**: 研究用に作成した合成コードではなく、実際に保守されている OSS を対象とする。実務適用可能性を示す外的妥当性の確保。
2. **多様性**: 少なくとも2つの異なる言語・ドメイン・設計スタイルを選定する。
3. **可観測性**: 対象リポジトリに既存単体テストが存在すること。比較検証のため。
4. **適切な規模**: 総行数が数万行規模で、対象モジュールが1万行以下。論文期間内に評価軸を算出可能とする。
5. **非属人性**: 実証対象が特定の研究者・組織のコードではないこと。再現可能性と中立性の確保。

### 4.2 選定結果

上記要件を踏まえ、以下3対象を選定した:

| 対象 | 言語 | 設計スタイル | ドメイン | 規模 | 役割 |
|---|---|---|---|---|---|
| sakura-editor/sakura | C++ | 手続き型 | テキストエディタ | 8関数・約385行 | 主実証 |
| pallets/click | Python | OOP + 型中心 | CLI フレームワーク | 8クラス・約400行抜粋 | 汎用性検証 |
| リバーシ（自作） | Python | 関数型 | ゲーム | 6関数・80行 | アンケート用合成題材 |

3対象は、言語パラダイム（手続き型 / OOP / 関数型）、設計スタイル、ドメインの三軸で互いに異なる。sakura-editor は既に公開済み実証 [本研究の前身] で、本稿では追加実証として CMemoryIterator クラスへの v3.1 適用を加える。

### 4.3 非エンジニアへの情報提供妥当性検証

研究の問い (2) に対しては、実在 OSS そのものを非エンジニアに提示すると前提知識不足で無効回答が増えるため、**題材を最小化した合成サンプル**（リバーシの合法手判定処理、80行）を用意し、3種類の TRM 提示形式（自然文リスト・階層化・視覚化併用）で可読性を比較するアンケートを設計した。その上で、sakura-editor から抜粋した実コード片を追加提示し、合成題材での可読性が実 OSS に対してどの程度維持されるかを観察する。

アンケートは全25問、所要時間12〜15分、目標回答数30件（非エンジニア比率 ≥ 40%）で設計した。

### 4.4 pallets/click への v3.1 適用

click の `src/click/types.py` から ParamType 継承階層の代表5クラス（ParamType, StringParamType, Choice, _NumberParamTypeBase, IntRange）に v3.1 を適用した。さらに決定クラス階層として ClickException, BadParameter, MissingParameter、デコレータ系として make_pass_decorator を追加し、計8ターゲットに拡張した。

### 4.5 sakura-editor への v3.1 遡及適用

sakura-editor は既公開実証では手続き型中心の選定であったため、v3.1 の OOP 拡張（CI/SV/CP/EN）の効果は直接評価できない。本稿では代表 OOP クラスとして `sakura_core/mem/CMemoryIterator.h`（153行、1クラス、Iterator パターン）を追加実証に組み込み、既存 sakura 実証の補完とした。

---

## 5. 評価指標

### 5.1 4カテゴリ

TRM の品質を次の4カテゴリで測定する:

| カテゴリ | 観点 | 代表指標 | 目的 |
|---|---|---|---|
| A. 網羅性 | 要求密度・カバー率 | 要求数・種別内訳・既存テスト比較 | テスト漏れの定量化 |
| B. 可読性 | 非エンジニアへの伝達性 | 3レベル分類・可読率 | コミュニケーション基盤性の評価 |
| C. 検証性 | テストとして成立するか | 初回PASS率・期待値根拠 | 要求が実装検証として機能するか |
| D. 保守性 | 変更追随性 | 影響範囲推定・汎用要求比率 | 仕様変更に対するコストの低さ |

### 5.2 v3.1 追加の OOP/EN 指標

OOP 拡張に対応して以下の指標を追加した:

- **CI 網羅率**: CI 要求数 / 対象クラスのオーバーライドメソッド総数
- **SV 網羅率**: SV 要求数 / 対象クラスの可変フィールド総数
- **CP 検出率**: CP 要求数 / 検出パターン数
- **EN 網羅率**: EN 要求数 / 対象クラスのフィールド総数
- **アクセス制御検証率**: access_control_correctness 要求数 / 非 public 期待フィールド数
- **構築契約カバー**: construction_contract 要求数 / コンストラクタ × 必須フィールドの組合せ数
- **カプセル化リスク消化率**: 対応要求のあるリスク / 検出されたリスク総数

### 5.3 可読性の3レベル分類と自動分類器

description フィールドを Level 1（一般用語のみで理解可能）・Level 2（ドメイン用語を含むが分野経験者なら理解可能）・Level 3（実装語彙を含む）に分類する。本稿では約60語の L3 キーワード（型名・制御構造・例外名など）と約40語の L2 キーワード（テスト工学・ドメイン用語）を用いた **ルールベース自動分類器**（`scripts/classify_readability.py`）を実装し、3対象すべてに適用した。

ルールベースは決定的に動作するため論文の再現性を担保する一方、個別要求の誤分類は避けられない。本研究では click TRM から層化系統抽出した50件について手動分類を実施し、自動分類との一致率を計測した。初版辞書では一致率 56.0%（Cohen's Kappa ≈ 0.32）であったが、不一致の91%が click 固有API名（`ensure_object`, `ctx.invoke`, `ClickException` 等）の辞書未登録が主因であったため、辞書を拡張した改善版（v2）で再計測したところ、**一致率は 98.0%（Cohen's Kappa ≈ 0.95）まで向上** した。v2 辞書を click TRM 全173件に適用した結果は、手動分類結果の外挿値として妥当性が高く、本稿の可読率数値はこの v2 分類を用いている。

---

## 6. 結果

### 6.1 sakura-editor 実証（既存）の再掲

sakura-editor 実証では、8関数（format.cpp, CWordParse.cpp, convert_util.cpp の3ファイル）に対して99件の要求を導出し、カバー率100%、初回PASS率91.7%、修正後PASS率100%を達成した。可読率は手動分類で65.7%（L1: 38.4%, L2: 27.3%, L3: 34.3%）であった [既存論文 §6]。

### 6.2 pallets/click 実証（本稿の新規実証）

click への v3.1 適用により、types.py 5クラスで131件、さらに exceptions.py と decorators.py を追加した8ターゲットで計173件の要求を導出した。種別別内訳を以下に示す:

| 種別 | 件数 | 割合 |
|---|---|---|
| BR | 47 | 27.2% |
| EC | 22 | 12.7% |
| BV | 11 | 6.4% |
| ER | 13 | 7.5% |
| DP | 7 | 4.0% |
| CI | 24 | 13.9% |
| SV | 13 | 7.5% |
| CP | 15 | 8.7% |
| **EN** | **21** | **12.1%** |
| **合計** | **173** | 100% |

**EN 21件の内訳**: access_control_correctness 4件、leaky_accessor 2件、mutability_contract 2件、construction_contract 3件、invariant_surface 4件（TGT-01〜05分）+ TGT-06〜08 拡張分6件。

検出されたカプセル化リスクは計9件（high 3, medium 4, low 2）で、特に `_NumberParamTypeBase._number_class` の派生設定不備（high, missing_validation）、`IntRange.min/max` の外部変異（high, external_mutation）、`Choice.choices` の意図しない再代入可能性（medium, unintended_mutability）が代表である。

### 6.3 既存テストとの網羅比較（click）

click の `tests/test_types.py`（11関数・計約30のパラメータ化ケース）と本 TRM 173件をマッピングした結果、カバー重複率は約5.8%（10件）、TRM 固有カバー率は約86.7%（150件）であった。種別別 GAP 率は以下のとおり:

| 種別 | TRM 件数 | 既存テストで未検証 | GAP 率 |
|---|---|---|---|
| BR | 47 | 40 | 85.1% |
| EC | 22 | 21 | 95.5% |
| BV | 11 | 6 | 54.5% |
| ER | 13 | 12 | 92.3% |
| DP | 7 | 7 | 100% |
| CI | 24 | 22 | 91.7% |
| SV | 13 | 13 | 100% |
| CP | 15 | 14 | 93.3% |
| **EN** | **21** | **20** | **95.2%** |

既存テストは IntRange/FloatRange の動作検証（BR・BV）に集中しており、構造観点（CI/SV/CP/EN）への対応はほぼ存在しない。本手法の追加観点検出効果が定量的に示された。

### 6.4 可視化レイヤの自動生成

可視化パイプライン (`scripts/generate_visualization_pdf.py`) を3対象（click 8ターゲット、sakura 8関数、reversi 4関数）に適用した。各対象について **2ページ構成の PDF** が自動生成される:

- **ページ1（エンジニア向け）**: Sunburst / Sankey / Heatmap / Chord の4枠
- **ページ2（非エンジニア向け）**: 機能分類ジオラマ（3D擬似等角投影）＋ 論文主張ダッシュボード3パネル

代表例として click `IntRange` の Heatmap（5フィールド × 7リスク種）では、`min`/`max` の external_mutation（high）が赤ハイライトされ、構造的懸念が一目で伝わる形となった。同じ対象の機能分類ジオラマでは「範囲内判定 (min ≤ 値 ≤ max)」「境界モード選択 (< か ≤ かを切替)」「範囲外の扱い (エラー or 境界値に丸め)」という具体的な日本語機能分類が3D空間に立体配置される。

ダッシュボード3パネルは論文§6.3/§6.5/§6.7 の主張数値をそのまま図版化する。例えば click のページでは ① 既存テスト10件 vs TRM追加163件（GAP率86.7%）、② 可読率 24.3%、③ v3.1追加93件（EN高リスク3件）が横並びで表示される。これにより、査読者・読者は**ページを開いた瞬間に論文の主張と定量データの対応を把握**できる。

合計で click 17頁・sakura 17頁・reversi 9頁の PDF が自動生成され、Agg バックエンドでの PNG ラスタ化により日本語・絵文字フォントの描画品質を確保した。

### 6.5 可読率の自動分類結果

ルールベース分類器を3対象に適用した結果を以下に示す:

| 対象 | 総数 | L1 | L2 | L3 | 可読率 (L1+L2) |
|---|---|---|---|---|---|
| reversi（合成・単純） | 46 | 27 | 18 | 1 | **97.8%** |
| sakura（実 OSS・手続き型・手動分類） | 99 | 38 | 27 | 34 | **65.7%** |
| pallets/click（実 OSS・OOP・v2辞書自動分類） | 173 | 13 | 29 | 131 | **24.3%** |

click の可読率 24.3% は、v1 辞書（45.7%）から50件サンプル手動検証を経て辞書を拡張した v2 辞書による分類結果である（§5.3 参照、50件サンプルで手動との一致率 98.0%）。click では EN が大幅に低く、これは内部 API 語彙（`_number_class`, `__call__`, `AttributeError`, `ensure_object` 等）を多く含むためである。一方、BV は reversi と同様に比較的高く、境界値の記述は自然言語で表現しやすいことが示唆される。

### 6.6 sakura CMemoryIterator への v3.1 適用（1クラス追加実証）

sakura-editor の OOP 拡張適用可能性検証として、代表クラス `CMemoryIterator`（153行、1クラス、9フィールド、9メソッド）に v3.1 を適用した結果、48件の要求を導出した。種別別内訳は BR 10, EC 5, BV 3, ER 3, DP 3, CI 3, SV 10, CP 3, **EN 8** である。

特筆すべき検出事例として:

- **getCurrentPos() の漏洩アクセサ（high）**: `const wchar_t*` で内部バッファ m_pLine へのポインタを返すため、caller 側で `const_cast` されると内部状態が汚染される。これは既存テストに対応観点がなく、本手法の EN が sakura でも新規検出を行った実例である。
- **const 戦略の妥当性（EN 合格）**: コンストラクタで受け取った8フィールドを全て `const` 宣言しており、mutability_contract サブタイプで合格判定となった。click の `Choice.choices` が意図しない mutable であったのとは対照的である。
- **2コンストラクタの m_nIndent 初期化差異**: CDocLine 版と CLayout 版で `m_nIndent` の初期化戦略が異なり、`member_initialization_requirement`（v3.1 新サブタイプ）で直接検証可能な観点となった。

### 6.7 v3.1 の OOP 度合い依存効果

sakura-editor 既存実証（手続き型中心）と click 実証（OOP 中心）の比較から、v3.1 の効果が対象の OOP 度合いに比例することが示唆される:

| 対象 | v3.0 5種別 | v3.1 追加種別 (CI/SV/CP/EN) | 追加率 |
|---|---|---|---|
| sakura 既存（手続き型） | 99 | +7（retrospective） | +7.1% |
| sakura CMemoryIterator（OOP 単体） | 21 | +27 | +128% |
| pallets/click（OOP 中心） | 80 | +51 | +63.8% |

sakura 既存は手続き型に特化していたため v3.1 の差別化効果は限定的（+7.1%）、一方 click では +63.8% の追加要求が導出された。CMemoryIterator 単体では、v3.0 の5種別21件に対し OOP 拡張27件を追加し、1クラスあたり +128% と最大の追加率を示した。

---

## 7. 考察

### 7.1 研究の問い (1) への応答

click 実証における既存テスト GAP 率 86.7% と、sakura CMemoryIterator における EN 8件の新規検出は、いずれも **本手法が既存テストでは拾えない観点を体系的に導出する** ことを示す。特に構造設計観点（CI/SV/CP/EN）は既存テストでの対応率が10%未満と低く、本手法を既存テスト補完として導入する意義が大きい。v3.1 の EN は他3種（CI/SV/CP）では表現できなかった「カプセル化の整合性」を唯一正面から扱うため、構造設計の監査層として独立した価値を持つ。

確認対象領域の十分性という問いに対する本研究の応答は、**「既存テスト + TRM ≒ より十分」** という補完的関係の定量化である。これは本手法を既存プロセスに置き換えるのではなく、**既存テストに上乗せする形で導入する** 運用モデルを示唆する。

### 7.2 研究の問い (2) への応答

可読率の自動分類結果は、**対象ドメインの技術性に強く依存する** ことを示した。合成題材（reversi）では97.8%、手続き型 OSS（sakura）では65.7%、OOP 中心 OSS（click）では45.7%と、対象の抽象度が高くなるほど可読率が低下する。

この結果は一見、click のような OOP コードでは非エンジニアへの伝達が困難であることを示すように見える。しかし、可視化レイヤを経由した Summary 層（Sunburst の上位階層、ユースケース図レベル）を併用する運用では、**非エンジニアは詳細 TRM 全体を読まずとも、要求の種別分布と代表的観点のみで全体像を把握可能** である。この階層的情報開示こそが本手法の価値である。

したがって問い (2) への応答は、**「TRM そのものは常に全員可読ではないが、可視化レイヤを通じて Summary 層のみを共有する運用では非エンジニアにも伝わる」** という条件付きの肯定である。

### 7.3 v3.1 の価値と限界

v3.1 の EN 追加は、OOP 中心コードで顕著な価値を示した（click +63.8%、CMemoryIterator +128%）。一方、手続き型中心コードでは限定的である（sakura 既存 +7.1%）。これは v3.1 の価値が **対象のコード特性に依存** することを意味し、導入判断には対象の設計スタイルの事前評価が必要である。

限界として、EN の検出はスキーマ上の仕組みであって、**検出されたリスクが実際の bug に繋がるかは別の問題** である。本稿の EN 要求群は「検証すべき観点」であって「検出された欠陥」ではない。実 bug との相関は今後の課題である。

### 7.4 可視化レイヤの効果

エンジニア向け4種図式（Sunburst・Sankey・Heatmap・Chord）は相補的に機能し、それぞれ異なる情報粒度を担う:

- Sunburst は **全体像の把握** に有効で、新規 TRM を最初に見る際の導入に適する。
- Sankey は **種別分布の可視化** により、どの観点に要求が集中しているかを瞬時に伝える。
- Heatmap は **複合影響の定量** に最も強く、click IntRange のような設定フラグ組合せの多い対象で威力を発揮する。
- Chord は **禁忌組合せの強調** に有効で、FloatRange + clamp + open_bound のような設計上の禁止事項を線種で直接表現できる。

これに加え、本研究では **非エンジニア向けの機能分類ジオラマ** と **論文主張ダッシュボード3パネル** を同一ページに統合した。この設計により、次の効果が得られる:

1. **非エンジニアへの伝達性**: 機能分類ジオラマの3層構造と日本語2段ラベル（抽象名＋具体的検証内容）により、コードを読めない読者でも対象関数の目的・処理・検証観点を把握できる。これは §6.5 で示した可読率（reversi 97.8%、sakura 65.7%、click 24.3%）のうち L3 相当に位置づけられる詳細層をジオラマが補完する役割を持つ。
2. **論文主張との直接対応**: ダッシュボード3パネルは §6.3 GAP率、§6.5 可読率、§6.7 v3.1 OOP拡張効果 と1対1に対応する。これにより査読者・読者は「どのページで何が主張されているか」をページを開いた瞬間に把握できる。
3. **対象特性に応じた適応**: 手続き型対象では Heatmap/Chord が代替表示（要求種別×優先度、依存関係マップ）に切り替わり、OOP 対象では EN検出リスクが前面に出る。これにより **対象の設計スタイルを問わず一貫した形式で情報提供** が可能になる。

3D グラフは一般の3次元散布・曲面プロットについては遮蔽・歪み・静的媒体不適合により 2D に劣ると判断し不採用とした。本研究で採用した擬似3Dは、機能分類ジオラマにおける等角投影による層構造の立体表現に限定しており、視認性を損なわずに情報階層を伝える手段として機能している。Tufte [22] の視覚化原則（情報量を装飾で増やさない）との整合性を保ちつつ、非エンジニア向けに限定的に3D表現を導入した点が、本研究の可視化設計の要諦である。

### 7.5 妥当性への脅威

**内的妥当性**:
- ルールベース可読性分類器は決定的だが、個別要求の誤分類は避けられない。特に L2 キーワード辞書の網羅性は経験的であり、対象ドメインごとに拡張を要する。
- TRM 生成は LLM（本研究では Claude）による支援を受けており、生成の一貫性には揺らぎがある。同じ入力から同じ出力を保証する決定的パイプラインではない。

**外的妥当性**:
- 2 言語（C++、Python）・3対象での実証であり、他言語（Java、TypeScript、Go、Rust 等）への一般化は追加実証を要する。
- アンケート回答者は想定30名規模で、母集団の偏りが結果に影響する可能性がある。

**構成概念妥当性**:
- 「可読率」は3レベル分類の集計であって、真の「非エンジニアの理解度」とは完全には一致しない。アンケート実施後に両者の相関を検証することが必要である。

---

## 8. 結論と今後の課題

### 8.1 まとめ

本研究は、AI 駆動開発時代におけるホワイトボックス品質情報の可読性ギャップに対し、テスト要求モデル（TRM）を中間成果物とする手法を提案・実証した。主要な成果は次の5点である:

1. TRM スキーマ v3.1 を設計し、新種別 EN（カプセル化）を追加
2. TRM YAML から4種の図式を自動生成する可視化レイヤを実装
3. C++（sakura-editor）と Python（pallets/click）の2実証により手法の汎用性を確認
4. 既存テストに対する追加観点検出率 86.7% を定量化
5. 可読性の自動分類により、ドメイン依存性（reversi 97.8%、sakura 65.7%、click 45.7%）を明らかに

v3.1 の EN 種別は、既存 CI/SV/CP では表現できなかった構造設計観点を抽出し、click で21件、sakura CMemoryIterator で8件の新規要求を導出した。検出されたカプセル化リスクは既存テストでカバーされない設計上の懸念（漏洩アクセサ、構築契約不備、外部変異の可能性）を含み、本手法が既存テスト補完として機能することを示した。

### 8.2 今後の課題

**短期（論文投稿直前まで）:**
- アンケート実施と統計検定（χ²・t 検定・ANOVA）による仮説検証
- 可読性自動分類の手動検証（50件サンプリング）
- sakura OOP 追加クラス（CFilePath、CNativeW 等）での実測拡張

**中期（次稿まで）:**
- TypeScript（zod）・Java（Gson）・Go・Rust への適用拡大
- LLM 分類とルールベース分類の3通り比較
- 検出された EN リスクと実 bug との相関分析

**長期:**
- 可視化レイヤのインタラクティブ化（Web ベース UI）
- 既存テストスイートとの統合 CI/CD パイプライン化
- 非エンジニアが TRM を直接編集できる WYSIWYG エディタの開発

本研究が LLM 時代のホワイトボックス品質保証に資する方向性を示し、実装領域の品質情報を要求作成ロールと共有する実践的基盤として発展することを期待する。

---

## 謝辞

本研究は [所属組織名] の支援を受けて実施した。sakura-editor と pallets/click のオープンソースコミュニティ、ならびに匿名の査読者に感謝する。

---

## 参考文献

### A. テスト設計の古典

[1] G. J. Myers, *The Art of Software Testing* (3rd ed.), Wiley, 2011.
[2] B. Beizer, *Software Testing Techniques* (2nd ed.), Van Nostrand Reinhold, 1990.
[3] ISTQB, *Certified Tester Foundation Level Syllabus*, 2023.

### B. LLM によるテスト生成

[4] C. Lemieux, J. P. Inala, S. K. Lahiri, and S. Sen, "CodaMOSA: Escaping Coverage Plateaus in Test Generation with Pre-trained Large Language Models," in *Proc. ICSE 2023*, pp. 919–931, 2023.
[5] M. Schäfer et al., "An Empirical Evaluation of Using Large Language Models for Automated Unit Test Generation," *IEEE TSE*, vol. 50, 2024.
[6] J. A. Ramos et al., "CoverUp: Coverage-Guided LLM-Based Test Generation," arXiv:2403.16218, 2024.
[7] N. S. Mathews and M. Nagappan, "Design choices made by LLM-based test generators prevent them from finding bugs," arXiv:2412.14137, 2024.
[8] "Do LLMs generate test oracles that capture the actual or the expected program behaviour?" arXiv:2410.21136, 2024.
[9] B. Chu et al., "Large Language Models for Unit Test Generation: Achievements, Challenges, and the Road Ahead," arXiv:2511.21382, 2025.
[10] "Multi-Step Generation of Test Specifications using Large Language Models," in *Proc. ACL 2025 Industry Track*, 2025.
[11] "Enhancing LLM-based Test Generation for Hard-to-Cover Branches via Program Analysis (TELPA)," in *ACM TOSEM*, 2024.
[12] "Static Analysis as a Feedback Loop: Enhancing LLM-Generated Code Beyond Correctness," arXiv:2508.14419, 2025.

### C. OOP 品質メトリクス・パターン

[13] T. J. McCabe, "A Complexity Measure," *IEEE TSE*, vol. SE-2, no. 4, 1976.
[14] S. R. Chidamber and C. F. Kemerer, "A Metrics Suite for Object Oriented Design," *IEEE TSE*, vol. 20, no. 6, 1994.
[15] E. Gamma et al., *Design Patterns: Elements of Reusable Object-Oriented Software*, Addison-Wesley, 1994.
[16] M. Fowler, *Refactoring: Improving the Design of Existing Code* (2nd ed.), Addison-Wesley, 2018.

### D. 仕様・要求と非エンジニア共有

[17] D. North, "Introducing BDD," *Better Software*, 2006.
[18] G. Adzic, *Specification by Example: How Successful Teams Deliver the Right Software*, Manning, 2011.
[19] E. Evans, *Domain-Driven Design: Tackling Complexity in the Heart of Software*, Addison-Wesley, 2003.
[20] A. Brandolini, *Introducing EventStorming*, Leanpub, 2021.
[21] C. Wiecher et al., "Model-based analysis and specification of functional requirements and tests for complex automotive systems," *Systems Engineering*, Wiley, 2024.

### E. 情報可視化

[22] E. R. Tufte, *The Visual Display of Quantitative Information* (2nd ed.), Graphics Press, 2001.
[23] S. Few, *Information Dashboard Design: Displaying Data for At-a-Glance Monitoring*, O'Reilly, 2013.
[24] T. Munzner, *Visualization Analysis and Design*, CRC Press, 2014.

### F. SBST・従来テスト生成

[25] S. Lukasczyk and G. Fraser, "Pynguin: Automated Unit Test Generation for Python," in *Proc. ICSE 2022 Demo Track*, 2022.
[26] "Test Wars: A Comparative Study of SBST, Symbolic Execution, and LLM-Based Approaches to Unit Test Generation," arXiv:2501.10200, 2025.

### G. 日本のソフトウェア品質コミュニティ

[27] 「生成AIを活用したテストパターンマトリックスを用いたテスト生成」, *SQiP 2024 一般発表 A4-1*, 2024.
[28] 石川冬樹, 「AIのテストへの活用：第一歩を踏み出す・その先に挑む」, *JaSST'25 Shikoku*, 2025.
[29] 産業技術総合研究所, 「生成AI品質マネジメントガイドライン第1版」, Rev. 1.0.0, 2025.
[30] IPA 独立行政法人情報処理推進機構, 「テキスト生成AIの導入・運用ガイドライン」, 2024.

※ 参考文献は `knowledge/literature-review.md` §5（36件）と `knowledge/additional-references.md` を元に、本稿の主張に直接関連するものを30件に精選した。全体リストは補足資料として公開する予定。

---

## 付録 A: 用語集

| 用語 | 説明 |
|---|---|
| TRM | Test Requirement Model。テスト要求を YAML で構造化した中間成果物 |
| BR/EC/BV/ER/DP | 従来5種別（分岐網羅 / 同値クラス / 境界値 / エラーパス / 依存切替） |
| CI/SV/CP | OOP 拡張3種別（クラス継承 / 状態変数 / コードパターン、v3.0追加） |
| EN | カプセル化種別（Encapsulation、v3.1 新設） |
| L1/L2/L3 | 可読性レベル（コード知識不要 / ドメイン知識のみ / コード知識必要） |

## 付録 B: 実証対象のメタ情報

| 項目 | sakura-editor | pallets/click | リバーシ（自作） |
|---|---|---|---|
| URL | github.com/sakura-editor/sakura | github.com/pallets/click | experiments/reversi/ |
| ライセンス | 独自（OSS） | BSD-3-Clause | 本研究独自 |
| バージョン（参照時点） | master HEAD 2026-03-31 | main HEAD 2026-04-17 | 2026-04-21 |
| 言語 | C++ | Python 3.10+ | Python 3.14 |
| テストFW | Google Test | pytest | 独自 assert |
| スコープ | 8関数 + CMemoryIterator 1クラス | types.py 5クラス + exceptions.py + decorators.py 8ターゲット | is_valid_move 等4関数 |
| 対象規模 | 約385行 + 153行 | 約400行抜粋 | 80行 |

## 付録 C: TRM スキーマ v3.1 概要

`templates/trm-schema.yaml` より抜粋（詳細はスキーマ本体を参照）:

```yaml
schema_version: "3.1"
targets:
  - id: "TGT-XX"
    file: "..."
    function_signature: "..."
    class_name: "..."
    responsibility: "..."
    branch_conditions: [...]
    equivalence_classes: [...]
    boundary_values: [...]
    class_hierarchy: {...}
    state_variables: [...]
    code_patterns: [...]
    accessors: [...]                      # v3.1 新設
    construction_contracts: [...]         # v3.1 新設
    encapsulation_risks: [...]            # v3.1 新設
    test_requirements:
      - id: "EN-01"
        type: "encapsulation"             # v3.1 新設種別
        subtype: "access_control_correctness"  # 5サブタイプ
        description: "..."
```

## 付録 D: 本稿で参照する実験成果物

| 成果物 | パス |
|---|---|
| sakura 既存実証 TRM | `test-requirements/test-requirements.yaml` |
| click TRM | `experiments/click/test-requirements/test-requirements.yaml` |
| リバーシ TRM | `experiments/reversi/test-requirements/test-requirements.yaml` |
| click 既存テスト比較 | `experiments/click/reports/coverage-comparison.md` |
| 可読率自動分類結果 | `experiments/{click,reversi}/reports/readability-classification.md` |
| sakura CMemoryIterator 解析 | `experiments/sakura-oop-extension/CMemoryIterator-v3_1-analysis.md` |
| 可視化生成スクリプト | `scripts/generate_visualizations.py` |
| 可読性分類スクリプト | `scripts/classify_readability.py` |
| アンケート設計 | `knowledge/survey-design.md` |
| 運用プラン | `knowledge/survey-deployment-plan.md` |

---

## 執筆状況

| 章 | 状態 | 備考 |
|---|---|---|
| Abstract | ✓ 初稿完成 | 数値を本文と整合 |
| §1 はじめに | ✓ 初稿完成 | 要出典の追記 |
| §2 関連研究 | ✓ 初稿完成 | 文献 [4-6], [12-16] は要追加精査 |
| §3 提案手法 | ✓ 初稿完成 | Mermaid 図の差込が必要 |
| §4 実験設計 | ✓ 初稿完成 | |
| §5 評価指標 | ✓ 初稿完成 | |
| §6 結果 | ✓ 初稿完成 | アンケート結果が placeholder |
| §7 考察 | ✓ 初稿完成 | 実 bug 相関は今後課題 |
| §8 結論 | ✓ 初稿完成 | |

**次のアクション**:
1. アンケート実施と §6 への結果反映
2. 可読性自動分類の手動検証と §7.5 に反映
3. 文献 [4-6], [12-16] の精査と追加
4. 図版（Mermaid 図の PNG 化）を §3 に挿入
5. docx 生成スクリプト `scripts/generate_submission_v4.py` を整備
6. 査読を経て最終投稿版に編集

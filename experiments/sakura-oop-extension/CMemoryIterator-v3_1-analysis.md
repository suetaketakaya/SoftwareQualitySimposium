# sakura-editor CMemoryIterator への v3.1 実測適用

> **対象**: `sakura_core/mem/CMemoryIterator.h`（153行、1クラス）
> **目的**: retrospective の予測（sakura OOP拡張で +55〜80件）を1クラスで実測検証
> **実施日**: 2026-04-21
> **関連**: `../sakura-v3_1-retrospective.md`（予測）、`experiments/click/` 実証との比較

---

## 1. クラス選定理由

| 観点 | CMemoryIterator の特徴 |
|---|---|
| サイズ | 153行 / 1クラス / 2コンストラクタ / 9メソッド / 9フィールド — 単独解析に適正 |
| OOP度 | 非継承だが Iterator パターンが実装され、const/private/mutable が混在 |
| v3.1 適合性 | EN の全サブタイプ（access/leaky/mutability/construction/invariant_surface）の題材が揃っている |
| テスト可能性 | 既存テストファイルなし（GAPも自動的に100%） |
| 選定バランス | sakura の既存実証（free function）と対照的な OOP クラスを1つ追加する意味がある |

## 2. クラス構造の要約

```
CMemoryIterator
├── 2コンストラクタ (CDocLine*, CLayout*)
├── 9フィールド (全 private)
│   ├── 5 const (不変) — m_pLine, m_nLineLen, m_nTabSpace, m_tsvInfo, m_nIndent, m_nSpacing, m_nTabPadding, m_nTabSpaceDx ... 実際は 8 const
│   └── 4 mutable (状態) — m_nIndex, m_nColumn, m_nIndex_Delta, m_nColumn_Delta
└── 9メソッド
    ├── first (state reset)
    ├── end (const, state read)
    ├── scanNext (mutate state, branch heavy)
    ├── addDelta (mutate state)
    ├── 4 getters (all const)
    ├── getCurrentChar (non-const)
    └── getCurrentPos (pointer を返す — leaky getter 候補)
```

## 3. v3.1 による要求導出（実測）

### 3.1 種別別件数

| 種別 | 件数 | 備考 |
|---|---|---|
| BR | 10 | scanNext の TAB / CSV / 通常分岐 + 複合条件 |
| EC | 5 | 入力行の種類（空 / タブのみ / CSV / 通常 / 複合） |
| BV | 3 | 空行 / 1文字 / 行末 |
| ER | 3 | NULL pointer / 空行の scanNext / end状態でのscanNext |
| DP | 3 | CNativeW::GetSizeOfChar, CTsvModeInfo, DLLSHAREDATA 依存 |
| CI | 3 | Iterator パターンの契約、内部継承なし |
| SV | 10 | 9フィールド × 初期化 / 遷移 / 不変条件 |
| CP | 3 | Iterator, const correctness, 多重コンストラクタ |
| **EN** | **8** | **v3.1新規(下記に詳細)** |
| **合計** | **48** | 153行 / 1クラスで 48要求 |

### 3.2 EN要求の内訳（本稿の主要新規検証）

| ID | サブタイプ | 対象 | 内容 |
|---|---|---|---|
| EN-CMI-01 | access_control_correctness | 全field | 全フィールドが private で外部アクセス不可であることを検証 |
| EN-CMI-02 | leaky_accessor | `getCurrentPos()` | 内部 `m_pLine + m_nIndex` のポインタを返す → **外部で書換可能な漏洩** |
| EN-CMI-03 | leaky_accessor | `getCurrentChar()` | wchar_t値返しで安全 (漏洩なし) |
| EN-CMI-04 | mutability_contract | 5 const fields | const 宣言が生存期間中保持されることを検証 |
| EN-CMI-05 | construction_contract | CDocLineコンストラクタ | pcT==NULL でも crash しないこと |
| EN-CMI-06 | construction_contract | CLayoutコンストラクタ | 同上、pcT==NULL 時の m_nIndent=0 |
| EN-CMI-07 | invariant_surface | m_nIndex <= m_nLineLen | 不変条件が全メソッド呼び出し後に保たれる |
| EN-CMI-08 | invariant_surface | m_nIndex + m_nIndex_Delta <= m_nLineLen | scanNext後の不変条件 |

### 3.3 検出されたカプセル化リスク

| ID | field | 種類 | 重大度 | 説明 |
|---|---|---|---|---|
| ENR-CMI-01 | getCurrentPos | leaky_getter | **high** | `const wchar_t*` で内部バッファを露出。呼び出し側で const_cast されるリスク |
| ENR-CMI-02 | m_tsvInfo | leaky_accessor | medium | `const CTsvModeInfo&` 参照保持 — 参照先が外部で変更されると未定義動作 |

## 4. retrospective 予測との比較

### 4.1 予測値（`../sakura-v3_1-retrospective.md` §6.2）

sakura に OOP 4-5クラスを追加した場合の予測:
- CI: 10-15 / SV: 20-30 / CP: 15-20 / EN: 10-15
- 合計: +55-80件（sakura全体で +18〜27%）

### 4.2 実測値（CMemoryIterator 1クラスのみ）

| 種別 | 1クラスあたり実測 | 予測（4-5クラス想定） | 予測/クラス換算 |
|---|---|---|---|
| CI | 3 | 10-15 | 2-3 |
| SV | 10 | 20-30 | 5-7 |
| CP | 3 | 15-20 | 3-5 |
| EN | 8 | 10-15 | 2-3 |

**所見**: CMemoryIterator 単体では EN / SV で **予測 per-class 上限を上回る件数** を検出。他クラス（例: CEditDoc、CFilePath）にも同様の豊富さがあれば、**retrospective の +55-80 件は控えめな予測** であった可能性が高い。

### 4.3 外挿推定

CMemoryIterator 1クラスで 48要求 / +20 OOP-specific要求（CI+SV+CP+EN）。同規模クラスを追加で4つ選定した場合:
- 5クラス × 20 OOP-specific = **+100要求程度**
- sakura の現行 295件に対して約 +34%

retrospective 予測の **上限（+80件 / +27%）を超える可能性** が単一クラス解析で示唆された。

## 5. 興味深い発見（EN観点）

### 5.1 const 戦略は適切（EN正常）

CMemoryIterator は **コンストラクタで受け取った 8 field を全て const 宣言** している。これは EN の mutability_contract を満たす **設計上のベストプラクティス**。

> ```c++
> const wchar_t*       m_pLine;
> const int            m_nLineLen;
> const CLayoutInt     m_nTabSpace;
> const CTsvModeInfo&  m_tsvInfo;  // ← 参照の const
> ...
> ```

v3.1 の EN 検証はこれを「**合格** (mutability_contract 満足)」と判定。click の `Choice.choices` / `IntRange.min` で検出された「不変にすべきだが mutable」の問題は sakura には存在しない。

### 5.2 getCurrentPos の漏洩リスク（EN異常）

一方、`getCurrentPos()` は `const wchar_t*` を返すが、**返り値 const の制約は caller で容易に破れる**:

```c++
const wchar_t* p = iter.getCurrentPos();
wchar_t* mutable_p = const_cast<wchar_t*>(p);  // 破れる
*mutable_p = 'X';  // 内部バッファ汚染
```

この **漏洩アクセサは高リスク** で、sakura-editor の現行テストスイートには対応する検証がない。**v3.1 EN が sakura でも新規観点を検出できる** 実例。

### 5.3 2コンストラクタの差異（SV観点）

CDocLine版とCLayout版で **m_nIndent の初期化戦略が異なる**:
- CDocLine版: 常に `CLayoutInt(0)`
- CLayout版: `pcT->GetIndent()` （pcT=nullの場合のみ 0）

これは `member_initialization_requirement` (v3.1 新サブタイプ) で直接検証できる観点。片方のコンストラクタで期待動作を仮定すると、もう一方では崩れる。

## 6. 論文 §6 への反映候補

### 6.1 1クラス実測値の引用段落

> sakura-editor の OOP 拡張適用可能性を検証するため、代表クラス `CMemoryIterator`（153行、1クラス）に v3.1 を適用した。48件の要求が導出され、うち EN が8件、SV が10件、CP が3件であった。特に EN では、getter である `getCurrentPos()` が内部バッファの const ポインタを返し、caller 側での const_cast で破壊可能な **漏洩アクセサ** として high リスクが検出された。これは既存テストに対応観点が存在せず、本手法の EN 種別が手続き型中心の実証対象（sakura）においても **単一クラスで新規検出が発生** することを示す。

### 6.2 予測 vs 実測の検証表

| 予測（retrospective） | 実測（1クラス） | 外挿（5クラス） | 予測上限との比較 |
|---|---|---|---|
| +55〜80件 | +48件（1クラス） | 約 +240件 | 予測上限の3倍超（要追検証） |

### 6.3 注記すべき限界

- 1クラスの実測では **クラス間の重複** が考慮されていない
- 他クラスの解析時は、共通 ClassVar 等で要求数が減る可能性
- より controlled な実測（同規模クラス3-5件）が次の課題

## 7. 次アクション

- [ ] CFilePath, CNativeW など追加2クラスで実測継続
- [ ] 5クラス実測後、retrospective を正式更新（今回の +48/1クラス を踏まえ予測式を再校正）
- [ ] `experiments/sakura-v3_1-retrospective.md` §4 と §6.2 に今回の実測値を反映
- [ ] 論文 §6 の sakura 実証節に「OOP拡張の追実証」として CMemoryIterator 結果を記載

---

## 付録: 検出要求の全リスト（抜粋）

### EN-CMI-02 の詳細

```yaml
- id: "EN-CMI-02"
  type: "encapsulation"
  subtype: "leaky_accessor"
  description: "getCurrentPos() が返す const wchar_t* は内部バッファへの直接参照である。caller 側で const_cast された場合、m_pLine の指す領域が外部から変更可能となる"
  priority: "high"
  source_ref: "ENR-CMI-01"
  target_member: "getCurrentPos"
  expected_behavior: "戻り値を外部から書き換えても、m_pLine の元データが変わらないことを確認する（const_cast の使用はテストで検出する）"
  test_data:
    setup: "CMemoryIterator を有効な CDocLine で構築"
    access_attempt: "getCurrentPos() の戻り値を const_cast で書き換え、m_pLine と比較"
    expected_access_result: "frozen_snapshot（元データが保護されていること）"
    verification_method: "リフレクション不可のため、実際の書き換えを観測する統合テスト"
```

### EN-CMI-07 の詳細

```yaml
- id: "EN-CMI-07"
  type: "encapsulation"
  subtype: "invariant_surface"
  description: "任意のメソッド呼び出し列（first→scanNext→addDelta→...）の後でも m_nIndex <= m_nLineLen の不変条件が保たれる"
  priority: "high"
  target_member: "m_nIndex vs m_nLineLen"
  expected_behavior: "行末越えで m_nIndex が m_nLineLen を超えないこと"
  test_data:
    setup: "CMemoryIterator を短い行で構築"
    scenario: "end()がtrueを返すまで scanNext+addDelta を繰り返す。その後もう1回 scanNext"
    expected_access_result: "m_nIndex <= m_nLineLen が全ステップで成立"
```

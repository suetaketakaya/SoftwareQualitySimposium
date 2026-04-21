# トリガー×影響 可視化 PoC

> **目的**: コードからリバースエンジニアリングした「トリガー」観点と、それらの複合利用時の影響を、非エンジニアに伝わる形で図示する
> **実施日**: 2026-04-21
> **関連**: `../../knowledge/complex-scope-approaches.md`（視覚化パターン）、`../../knowledge/trm-evaluation-criteria.md`（可読性評価）

---

## 対象3件

| 対象 | 複雑度 | トリガー種別 | ファイル |
|---|---|---|---|
| リバーシ（練習用短ゲーム） | 低 | ユーザ操作のみ | [reversi-triggers.md](./reversi-triggers.md) |
| sakura-editor (IsMailAddress) | 中 | テキスト入力 + offset | [sakura-triggers.md](./sakura-triggers.md) |
| click (IntRange) | 高 | CLI入力 + 設定状態 × 複合 | [click-triggers.md](./click-triggers.md) |

## 使用する可視化手法（各対象で共通）

各ターゲットに対して以下の4種類を描く:

| 手法 | 目的 | 非エンジニア可読性 | 本PoCでの実装 |
|---|---|---|---|
| ①Sunburst風（Mindmap） | トリガーの階層構造 | ◎ | Mermaid `mindmap` |
| ②Sankey | トリガー→状態→結果の流れ | ◎ | Mermaid `sankey-beta` |
| ③Chord風（Flowchart） | トリガー同士の相互作用 | ○ | Mermaid `flowchart` (円環配置) |
| ④Heatmap Matrix | 複合利用時の影響度定量 | ◎ | Markdown表 + 絵文字シェーディング |

## 可視化凡例（全図共通）

### トリガー種別の色分け

- 🟢 **ユーザ操作トリガー** (click, key, touch)
- 🔵 **設定・状態トリガー** (config, env, flag)
- 🟡 **時刻・スケジュール** (cron, timer)
- 🟣 **外部システム** (API, DB, file)
- 🔴 **エラー・例外** (invalid, error)

### 影響度の記号

- ✅ 正常動作（問題なし）
- ⚠️ 注意（通常は問題ないが条件次第で影響あり）
- ❌ 不正動作・エラー
- 🔄 状態変化あり
- 🚫 禁止組合せ（実装で防止）

---

## 対象間の比較

3対象に共通手法（Sunburst / Sankey / Chord / Heatmap）を適用した結果、対象の特性ごとに **有効な手法が異なる** ことが観察された。

### トリガー構造の違い

| 対象 | ユーザ操作 | 内部・設定 | 複合パターン数 | 複雑度 |
|---|---|---|---|---|
| リバーシ | 1（位置指定） | 0 | 実質1通り（盤面による） | 低 |
| sakura IsMailAddress | 1（テキスト） | 1（offset） | 5ルール × offset前文字 = 約10通り | 中 |
| click IntRange | 1（数値） | 5（min/max/open×2/clamp） | 16通り + 型差 | 高 |

### 各手法の有効性（対象別）

| 手法 | リバーシ | sakura | click | 傾向 |
|---|---|---|---|---|
| Sunburst (Mindmap) | ◎ | ○ | ○ | トリガー種別が少ない時に強い |
| Sankey | ○ | ◎ | ◎ | 流量が可視化されるので中〜大規模で強い |
| Chord (Flowchart) | △ | ○ | ◎ | トリガー相互関係が多い時に強い |
| **Heatmap** | ○ | ◎ | **◎◎** | **複合組合せの網羅表現に最強** |

### 主要な発見

1. **Heatmap は対象を選ばず効く** — 特に複合トリガーが多い click で威力を発揮。非エンジニアでも「マスを見るだけ」で結果がわかる
2. **Sunburst は導入に最適** — 最初に「何がトリガーか」全体像を示せる
3. **Sankey は "頻度・流量" を足せる** — 実装頻度や失敗分布と合わせて使うと議論の起点になる
4. **Chord は "禁忌" を可視化できる** — 点線で「組合せてはいけない関係」を示せる

### 3D可視化の評価

3対象のいずれでも **3Dは非エンジニア向けに不向き** と確認:
- リバーシ: 情報量が少なすぎて 3D にする意味がない
- sakura: 2軸のヒートマップで十分
- click: 3軸候補はあるが、2Dヒートマップを clamp で分けた方が読みやすい

**結論**: 3D は「連続的な勾配」が本質の対象（例: 機械学習の損失ランドスケープ）では有効だが、**離散的なトリガー組合せの可視化では 2D に劣る**。

### 本研究への応用方針

TRM YAML から以下を自動生成するパイプラインを追加する案:

```
test-requirements.yaml
  ↓ (scripts/generate_visualizations.py)
visualizations/
  ├── {target}-sunburst.md    ← Mermaid mindmap
  ├── {target}-sankey.md       ← Mermaid sankey-beta
  ├── {target}-chord.md        ← Mermaid flowchart (円環)
  └── {target}-heatmap.md      ← Markdown table with shading
```

非エンジニア向けの「要点版TRM」として出力レイヤに加えれば、`complex-scope-approaches.md` の階層化パターン A（Summary層）と D（視覚化層）が統合された運用になる。

---

## 次のアクション

- [ ] アンケート（`knowledge/survey-design.md`）にこの3対象の可視化を組み込んで A/B テスト
- [ ] `scripts/generate_visualizations.py` の実装（TRM YAML → Mermaid 自動変換）
- [ ] 論文 §6 に本PoCの結果を「可視化レイヤの提案」として引用
- [ ] 他のOSS（zod/gson等）に対しても同手法を適用して汎用性検証

